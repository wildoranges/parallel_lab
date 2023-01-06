#include "lab2.h"

int main(int argc, char *argv[]) {
    int id, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    srand(clock());
    int sum = rand() % 100;

    int recv_data;
    MPI_Status status;
    int layer = 1;

    printf("proc id %d, init data = %d\n", id, sum);

    for(int i = 1; i < num_procs; i = i << 1) {
        int dest = id ^ i;
        MPI_Send(&sum, 1, MPI_INT, dest, i, MPI_COMM_WORLD);
        MPI_Recv(&recv_data, 1, MPI_INT, dest, i, MPI_COMM_WORLD, &status);
        sum += recv_data;
        printf("layer %d, proc id %d, recv %d from proc %d, sum = %d\n", layer++, id, recv_data, dest, sum);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    printf("proc id %d, sum = %d\n", id, sum);

    MPI_Finalize();
    return 0;
}