#include "lab2.h"

int main(int argc, char *argv[]) {
    assert(argc == 2 && "not enough args, please specify the mat size");
    int mat_size = atoi(argv[1]);
    assert(mat_size >= 3 && "invalid mat size");
    
    MPI_Init(&argc, &argv);
    int id, num_procs;
    MPI_Status status;
    MPI_Request req;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    assert(num_procs >= 2 && "at least 2 procs");

    double *MATA, *MATB;
    double *submatA = (double *)malloc(sizeof(double) * (mat_size * 3));
    double *submatB = (double *)malloc(sizeof(double) * (mat_size * 1));
    memset(submatB, 0, sizeof(double) * (mat_size * 1));

    if (id == 0) {
        MATA = (double *)malloc(sizeof(double) * (mat_size * mat_size));
        rand_mat_double(MATA, mat_size);
        MATB = (double *)malloc(sizeof(double) * (mat_size * mat_size));
        memset(MATB, 0, sizeof(double) * (mat_size * mat_size));

        printf("mat A: \n");
        print_mat_double(MATA, mat_size);
    }

    int i = 0;
    while (i < mat_size - 2) {
        for (int j = 1; (j < num_procs) && (i < mat_size - 2); j++) {
            if (id == 0) {
                int offset = i * mat_size;
                MPI_Send(MATA + offset, mat_size * 3, MPI_DOUBLE, j, 0, MPI_COMM_WORLD);
                MPI_Irecv(MATB + offset + mat_size, mat_size * 1, MPI_DOUBLE, j, 1, MPI_COMM_WORLD, &req);
            } else if (id == j) {
                MPI_Recv(submatA, mat_size * 3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
                for (int k = 1; k < mat_size - 1; k++) {
                    submatB[k] = (submatA[ID(0, k, mat_size)] + submatA[ID(1, k+1, mat_size)] + submatA[ID(2, k, mat_size)] + submatA[ID(1, k-1, mat_size)]) / 4.0;
                }
                MPI_Send(submatB, mat_size * 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
            }
            i++;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (id == 0) {
        double *sol = (double *)malloc(sizeof(double) * (mat_size * mat_size));
        memset(sol, 0, sizeof(double) * (mat_size * mat_size));
        for (int i = 1; i < mat_size - 1; i++) {
            for (int j = 1; j < mat_size - 1; j++) {
                sol[ID(i, j, mat_size)] = (MATA[ID(i-1, j, mat_size)] + MATA[ID(i, j+1, mat_size)] + MATA[ID(i+1, j, mat_size)] + MATA[ID(i, j-1, mat_size)]) / 4.0;
            }
        }
        printf("calculated result:\n");
        print_mat_double(MATB, mat_size);
        printf("correct result:\n");
        print_mat_double(sol, mat_size);
        if (memcmp(MATB, sol, sizeof(double) * (mat_size * mat_size)) == 0) {
            printf("calculation is correct\n");
        } else {
            printf("calculation is wrong\n");
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}