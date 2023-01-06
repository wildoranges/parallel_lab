#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define PI 3.14159265358979323846

typedef struct {  
    double r;        
    double i;        
} complex;

complex complex_add(complex a, complex b) {
    complex c;
    c.r = a.r + b.r;
    c.i = a.i + b.i;
    return c;
}

complex complex_sub(complex a, complex b) {
    complex c;
    c.r = a.r - b.r;
    c.i = a.i - b.i;
    return c;
}

complex complex_mul(complex a, complex b) {
    complex c;
    c.r = a.r * b.r - a.i * b.i;  
    c.i = a.r * b.i + a.i * b.r;
    return c;
}

// complex complex_div(complex a, complex b) {
//     complex c;
//     c.r = (a.r * b.r + a.i * b.i) / (b.r * b.r +b.i * b.i);
//     c.i = (a.i * b.r - a.r * b.i) / (b.r * b.r +b.i * b.i);
//     return c;
// }

// double complex_abs(complex a) {
//     return sqrt(a.r * a.r + a.i * a.i);
// }

complex Wn_i(int n, int i) {
    complex c;
    c.r = cos(2 * PI * (double)(i) / (double)(n));
    c.i = - sin(2 * PI * (double)(i) / (double)(n));
    return c;
}

__uint32_t reverse_bits(__uint32_t n, __uint8_t aligned) {
    __uint32_t result = n >> 16 | n << 16;
    result = ((result & 0x00ff00ff) << 8) | ((result & 0xff00ff00) >> 8);
    result = ((result & 0x0f0f0f0f) << 4) | ((result & 0xf0f0f0f0) >> 4);
    result = ((result & 0x33333333) << 2) | ((result & 0xcccccccc) >> 2);
    result = ((result & 0x55555555) << 1) | ((result & 0xaaaaaaaa) >> 1);
    return result >> (32 - aligned);
}

int complex_eq(complex *a, complex *b, int elems, double eps) {
    for (int i = 0; i < elems; i++) {
        if (fabs(a[i].r - b[i].r) > eps) {
            return 0;
        }
        if (fabs(a[i].i - b[i].i) > eps) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int id, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    assert(((num_procs & (num_procs - 1)) == 0) && "must be 2^n");
    assert((num_procs >= 2) && "at least 2");

    int N = num_procs;
    int M = log2(N);

    // if (id == 0) {
    //     printf("n = %d, log2 n = %d, reverse 6 = %u\n", N, M, reverse_bits(6, M));
    // }

    MPI_Request req;
    MPI_Status status;
    complex *f = NULL;
    complex *FFT = NULL;
    complex value;
    complex in;
    complex Wn;
    clock_t start1, end1;
    if (id == 0) {
        f = (complex *)malloc(sizeof(complex) * N);
        FFT = (complex *)malloc(sizeof(complex) * N);
        // init input f
        srand(clock());
        printf("init f val: ");
        for (int i = 0; i < N; i++) {
            f[i].r = rand();
            f[i].i = 0;
            printf("f[%d] = %f ", i, f[i].r);
        }
        printf("\n");
    }

    for (int i = 0; i < N; i++) {
        int dst = reverse_bits(i, M);
        if (id == 0) {
            MPI_Isend(f + i, 2, MPI_DOUBLE, dst, dst, MPI_COMM_WORLD, &req);
        }
        if (id == dst) {
            MPI_Recv(&value, 2, MPI_DOUBLE, 0, id, MPI_COMM_WORLD, &status);
            printf("proc id %d recv %f + %fi\n", id, value.r, value.i);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (id == 0) {
        start1 = clock();
    }
    for (int i = 2; i <= N; i = i << 1) {
        int half_group = i >> 1;
        int dst = id ^ half_group;
        // printf("proc id %d, i %d, dest id %d\n", id, i, dst);
        MPI_Isend(&value, 2, MPI_DOUBLE, dst, i, MPI_COMM_WORLD, &req);
        MPI_Recv(&in, 2, MPI_DOUBLE, dst, i, MPI_COMM_WORLD, &status);
        int tag = id & (half_group - 1);
        Wn = Wn_i(N, tag * (N / i));
        int tag2 = id & (i - 1);
        if (tag2 < half_group) {
            value = complex_add(value, complex_mul(in, Wn));
        } else {
            value = complex_sub(in, complex_mul(value, Wn));
        }
    }
    if (id == 0) {
        end1 = clock();
    }

    MPI_Isend(&value, 2, MPI_DOUBLE, 0, id, MPI_COMM_WORLD, &req);
    if (id == 0) {
        // printf("FFT val: ");
        for (int i = 0; i < N; i++) {
            MPI_Recv(FFT + i, 2, MPI_DOUBLE, i, i, MPI_COMM_WORLD, &status);
            // printf("FFT[%d] = %f + %fi\n", i, FFT[i].r, FFT[i].i);
        }
        // printf("\n");

        complex *f2 = (complex *)malloc(sizeof(complex) * N);

        clock_t start2, end2;
        for (int i = 0; i < N; i++) {
            f2[reverse_bits(i, M)] = f[i];
        }

        start2 = clock();
        for (int i = 2; i <= N; i = i << 1) {
            int half_group = i >> 1;
            for (int j = 0; j < half_group; j++) {
                int r = j * (N / i);
                for (int n = j; n < N; n += i) {
                    int k = n + half_group;
                    Wn = Wn_i(N, r);
                    complex tmp = complex_mul(f2[k], Wn);
                    f2[k] = complex_sub(f2[n], tmp);
                    f2[n] = complex_add(f2[n], tmp);
                }
            }
        }
        end2 = clock();
        clock_t duration1 = end1 - start1;
        clock_t duration2 = end2 - start2;

        if (complex_eq(FFT, f2, N, 1e-3)) {
            printf("calculation is correct!\n");
            for (int i = 0; i < N; i++) {
                printf("FFT[%d] = %f + %fi\n", i, f2[i].r, f2[i].i);
            }
            printf("MPI clocks %ld, Serial clocks %ld, acceleration rate: %f\n", duration1, duration2, (double)duration2 / (double)duration1);
        } else {
            printf("calculation is wrong!\n");
            for (int i = 0; i < N; i++) {
                printf("FFT1[%d] = %f + %fi\n", i, FFT[i].r, FFT[i].i);
                printf("FFT2[%d] = %f + %fi\n", i, f2[i].r, f2[i].i);
            }
        }

        free(f);
        free(f2);
        free(FFT);
    }
    
    MPI_Finalize();
    return 0;
}