#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    double start, end;
    int i;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0)
            printf("Run with at least 2 processes.\n");
        MPI_Finalize();
        return 0;
    }

    // Test for different sizes (in bytes)
    int sizes[] = {1 * 1024 * 1024, 10 * 1024 * 1024, 50 * 1024 * 1024, 200 * 1024 * 1024}; // 1MB, 10MB, 50MB
    int num_tests = 4;

    for (i = 0; i < num_tests; i++) {
        int num_ints = sizes[i] / sizeof(int);
        int *data = (int *)malloc(num_ints * sizeof(int));

        if (rank == 0) {
            for (int j = 0; j < num_ints; j++)
                data[j] = j;

            start = MPI_Wtime();
            MPI_Send(data, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD);
            end = MPI_Wtime();
            printf("Process 0: Sent %d MB in %f seconds\n", sizes[i] / (1024 * 1024), end - start);
        } 
        else if (rank == 1) {
            start = MPI_Wtime();
            MPI_Recv(data, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            end = MPI_Wtime();
            printf("Process 1: Received %d MB in %f seconds | First: %d, Last: %d\n",
                   sizes[i] / (1024 * 1024), end - start, data[0], data[num_ints - 1]);
        }

        free(data);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
