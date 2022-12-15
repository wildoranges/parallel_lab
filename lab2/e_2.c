#include "lab2.h"

int main(int argc, char *argv[]) {
    assert(argc == 3 && "not enough args, please specify the mat size and block size");
    int mat_size = atoi(argv[1]);
    assert(mat_size >= 3 && "invalid mat size");
    int block_size = atoi(argv[2]);
    assert(block_size <= (mat_size - 2) && "block size can not be geater than mat size");
    int p = (mat_size - 2) / block_size;
    assert(block_size * p == (mat_size - 2) && "mat size - 2(size of MAT B) is not divisible by block size");
    int A_size = block_size + 2;
    
    MPI_Init(&argc, &argv);
    int id, num_procs;
    MPI_Status status;
    MPI_Request req;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    assert(num_procs >= 2 && "at least 2 procs");

    MPI_Datatype submat;
    MPI_Type_vector(block_size + 2, block_size + 2, mat_size, MPI_DOUBLE, &submat);
    MPI_Type_commit(&submat);

    MPI_Datatype mat;
    MPI_Type_vector(block_size + 2, block_size + 2, block_size + 2, MPI_DOUBLE, &mat);
    MPI_Type_commit(&mat);

    MPI_Datatype mat2;
    MPI_Type_vector(block_size, block_size, mat_size, MPI_DOUBLE, &mat2);
    MPI_Type_commit(&mat2);

    double *MATA, *MATB;
    double *submatA = (double *)malloc(sizeof(double) * (block_size + 2) * (block_size + 2));
    double *submatB = (double *)malloc(sizeof(double) * (block_size) * (block_size));
    if (id == 0) {
        MATA = (double *)malloc(sizeof(double) * (mat_size * mat_size));
        rand_mat_double(MATA, mat_size);
        MATB = (double *)malloc(sizeof(double) * (mat_size * mat_size));
        memset(MATB, 0, sizeof(double) * (mat_size * mat_size));

        printf("mat A: \n");
        print_mat_double(MATA, mat_size);
    }

    int i = 0, j = 0;
    while (i < p && j < p) {
        for (int k = 1; (k < num_procs) && (i < p) && (j < p); k++) {
            if (id == 0) {
                int offset = (i * block_size) * mat_size + (j * block_size);
                int offset2 = (i * block_size + 1) * mat_size + (j * block_size + 1);
                MPI_Send(MATA + offset, 1, submat, k, 0, MPI_COMM_WORLD);
                MPI_Irecv(MATB + offset2, 1, mat2, k, 1, MPI_COMM_WORLD, &req);
            } else if (id == k) {
                MPI_Recv(submatA, 1, mat, 0, 0, MPI_COMM_WORLD, &status);
                for (int m = 0; m < block_size; m++) {
                    for (int n = 0; n < block_size; n++) {
                        submatB[ID(m, n, block_size)] = (submatA[ID(m, n+1, A_size)] + submatA[ID(m+1, n+2, A_size)] + submatA[ID(m+2, n+1, A_size)] + submatA[ID(m+1, n, A_size)]) / 4.0;
                    }
                }
                MPI_Send(submatB, block_size * block_size, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
            }
            j ++;
            if (j >= p) {
                i ++;
                j = 0;
            }
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