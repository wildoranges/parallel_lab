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

    printf("proc id %d, init data = %d\n", id, sum);

    for (int i = 1; i < num_procs; i = i << 1) {
        if (id & i) {
            MPI_Send(&sum, 1, MPI_INT, id - i, i, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&recv_data, 1, MPI_INT, id + i, i, MPI_COMM_WORLD, &status);
            sum += recv_data;
        }
    }

    for (int i = num_procs; i >= 2; i >>= 1) {
        if (id % i == 0) {
            MPI_Send(&sum, 1, MPI_INT, id + (i >> 1), i, MPI_COMM_WORLD);
        } else if(id % (i >> 1) == 0) {
            MPI_Recv(&sum, 1, MPI_INT, id - (i >> 1), i, MPI_COMM_WORLD, &status);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    printf("proc id %d, sum = %d\n", id, sum);

    MPI_Finalize();
    return 0;
}