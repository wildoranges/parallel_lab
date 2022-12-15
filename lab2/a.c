#include "lab2.h"

#define BUF_SIZE 256

int main(int argc, char *argv[]) {
    int id, num_procs;
    int tag = 5;
    char msg[] = "MPI LAB";
    char recv_buf[BUF_SIZE];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    MPI_Comm split_world;
    MPI_Status status;
    int rank;
    int size;

    // split
    MPI_Comm_split(MPI_COMM_WORLD, id % 3, id / 3, &split_world);
    MPI_Comm_rank(split_world, &rank);
    MPI_Comm_size(split_world, &size);

    if (id == 0) {
        strcpy(recv_buf, msg);
        for (int i = 1; i < 3; i++) {
            MPI_Send(&msg, sizeof(msg), MPI_CHAR, i, tag, MPI_COMM_WORLD);
        }   
    } else if (id > 0 && id < 3) {
        MPI_Recv(&recv_buf, BUF_SIZE, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    }

    MPI_Bcast(&recv_buf, BUF_SIZE, MPI_CHAR, 0, split_world);

    printf("proc rank %d, old rank %d, group size %d. received msg: %s\n", rank, id, size, recv_buf);
    MPI_Finalize();
    return 0;
}