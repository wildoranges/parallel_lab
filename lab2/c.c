#include "lab2.h"

int *mat_mul_add(int *A, int *B, int *dest, int n) {
    if (dest == NULL) {
        dest = (int *)malloc(sizeof(int) * (n * n));
        memset(dest, 0, sizeof(int) * (n * n));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                dest[ID(i, j, n)] += A[ID(i, k, n)] * B[ID(k, j, n)];
            }
        }
    }
    return dest;
}

int main(int argc, char *argv[]) {
    assert(argc == 2 && "not enough args, please specify the mat size");
    int mat_size = atoi(argv[1]);
    assert(mat_size > 0 && "invalid mat size");
    
    MPI_Init(&argc, &argv);
    int id, num_procs;
    MPI_Status status;
    MPI_Request req;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int p = sqrt(num_procs);
    assert(p * p == num_procs && "num procs is not perfect square");

    int submat_size = mat_size / p;
    assert(submat_size * p == mat_size && "mat size is not divisible by p");

    int *MATA, *MATB, *MATC;

    if (id == 0) {
        MATA = (int *)malloc(sizeof(int) * (mat_size * mat_size));
        rand_mat(MATA, mat_size);
        MATB = (int *)malloc(sizeof(int) * (mat_size * mat_size));
        rand_mat(MATB, mat_size);
        MATC = (int *)malloc(sizeof(int) * (mat_size * mat_size));

        printf("mat A: \n");
        print_mat(MATA, mat_size);
        printf("mat B: \n");
        print_mat(MATB, mat_size);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int *submatA = (int *)malloc(sizeof(int) * (submat_size * submat_size));
    int *submatB = (int *)malloc(sizeof(int) * (submat_size * submat_size));
    int *submatC = (int *)malloc(sizeof(int) * (submat_size * submat_size));
    memset(submatC, 0, sizeof(int) * (submat_size * submat_size));
    int *buf = (int *)malloc(sizeof(int) * submat_size);

    MPI_Datatype submat;
    MPI_Type_vector(submat_size, submat_size, mat_size, MPI_INT, &submat);
    MPI_Type_commit(&submat);

    MPI_Datatype mat;
    MPI_Type_vector(submat_size, submat_size, submat_size, MPI_INT, &mat);
    MPI_Type_commit(&mat);

    if (id == 0) {
        for (int i = 0; i < p; i++) {
            for (int j = 0; j < p; j++) {
                int offset = i * mat_size * submat_size + j * submat_size;
                MPI_Isend(MATA+offset, 1, submat, i * p + j, 0, MPI_COMM_WORLD, &req);
                MPI_Isend(MATB+offset, 1, submat, i * p + j, 1, MPI_COMM_WORLD, &req);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Recv(submatA, 1, mat, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(submatB, 1, mat, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Comm row_comm, col_comm;
    int rank_i, rank_j;
    int size_i, size_j;

    MPI_Comm_split(MPI_COMM_WORLD, id / p, id % p, &row_comm);
    MPI_Comm_rank(row_comm, &rank_i);
    MPI_Comm_size(row_comm, &size_i);
    assert(size_i == p && "size_i is not equal to p");
    
    MPI_Comm_split(MPI_COMM_WORLD, id % p, id / p, &col_comm);
    MPI_Comm_rank(col_comm, &rank_j);
    MPI_Comm_size(col_comm, &size_j);
    assert(size_j == p && "size_j is not equal to p");

    for (int iter = 0; iter < p; iter++) {
        if (rank_i == (rank_j + iter) % size_i) {
            memcpy(buf, submatA, sizeof(int) * (submat_size * submat_size));
        }

        MPI_Bcast(buf, 1, mat, (rank_j + iter) % size_i, row_comm);
        mat_mul_add(buf, submatB, submatC, submat_size);

        MPI_Send(submatB, 1, mat, (rank_j - 1 + size_j) % size_j, 0, col_comm);
        MPI_Recv(buf, 1, mat, (rank_j + 1) % size_j, 0, col_comm, &status);
        memcpy(submatB, buf, sizeof(int) * (submat_size * submat_size));
    }
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < p; i++) {
        for (int j = 0; j < p; j++) {
            if (rank_j == i && rank_i == j) {
                MPI_Isend(submatC, 1, mat, 0, 2, MPI_COMM_WORLD, &req);
            }
            if (id == 0) {
                int offset = i * mat_size * submat_size + j * submat_size;
                MPI_Recv(MATC+offset, 1, submat, i * p + j, 2, MPI_COMM_WORLD, &status);
            }
        }
    }

    if (id == 0) {
        printf("calculated result:\n");
        print_mat(MATC, mat_size);
        int *sol = mat_mul_add(MATA, MATB, NULL, mat_size);
        printf("correct result:\n");
        print_mat(sol, mat_size);
        if (mat_eq(MATC, sol, mat_size)) {
            printf("calculation is correct\n");
        } else {
            printf("calculation is wrong\n");
        }
        free(sol);
        free(MATA);
        free(MATB);
        free(MATC);
    }

    free(submatA);
    free(submatB);
    free(submatC);
    free(buf);

    MPI_Barrier(MPI_COMM_WORLD);    
    MPI_Finalize();
    return 0;
}