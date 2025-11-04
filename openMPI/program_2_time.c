#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int number;
    /*
     * Use a simple ping-pong to measure communication time:
     * - rank 0 sends a message to rank 1, then waits for the reply
     * - rank 0 measures the round-trip time using MPI_Wtime()
     * Tags: 0 for request, 1 for reply
     */
    if (rank == 0) {
        number = 42;
        double t0 = MPI_Wtime();
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&number, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        double t1 = MPI_Wtime();
        printf("Process 0 received number %d back\n", number);
        printf("Round-trip time: %f seconds\n", t1 - t0);
        printf("Estimated one-way time: %f seconds\n", (t1 - t0) / 2.0);
    } else if (rank == 1) {
        MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process 1 received number %d\n", number);
        MPI_Send(&number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
