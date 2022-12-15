#include "lab2.h"

int main(int argc, char *argv[]) {

    int id, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int p_cnt = num_procs / 4;
    int q_cnt = num_procs - p_cnt;
    assert(p_cnt > 0 && "num of P is not positive");

    if (id < p_cnt) {
        // server
        MPI_Status status;
        MPI_Request req;
        int sum;
        int average;
        int recv_number;

        while(1) {
            sum = 0;

            for (int i = 1; (i * p_cnt) + id < num_procs; i++) {
                MPI_Recv(&recv_number, 1, MPI_INT, (i * p_cnt) + id, 0, MPI_COMM_WORLD, &status);
                sum += recv_number;
            } 

            for (int i = 0; i < p_cnt; i++) {
                if (id != i) {
                    MPI_Isend(&sum, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &req);
                }
            }

            for (int i = 0; i < p_cnt; i++) {
                if (id != i) {
                    MPI_Recv(&recv_number, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
                    sum += recv_number;
                }
            }
            
            average = sum / q_cnt;

            for (int i = 1; (i * p_cnt) + id < num_procs; i++) {
                MPI_Isend(&average, 1, MPI_INT, (i * p_cnt) + id, 1, MPI_COMM_WORLD, &req);
            }
        }
    } else {
        srand(clock());
        MPI_Status status;
        MPI_Request req;
        int number;
        int recv_number;
        // worker
        while (1) {
            number = rand() % 10;
            MPI_Isend(&number, 1, MPI_INT, id % p_cnt, 0, MPI_COMM_WORLD, &req);
            MPI_Recv(&recv_number, 1, MPI_INT, id % p_cnt, 1, MPI_COMM_WORLD, &status);
            printf("worker proc %d recv average number %d from server proc %d\n", id, recv_number, id % p_cnt);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}