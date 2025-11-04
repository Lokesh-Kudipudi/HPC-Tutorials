#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000  // 1 million integers

int main(int argc, char *argv[]) {
    int rank, size;
    double start, end;
    int *arr = NULL;
    MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0)
            printf("Run with at least 2 processes.\n");
        MPI_Finalize();
        return 0;
    }

    arr = (int *)malloc(N * sizeof(int));

    if (rank == 0) {
        for (int i = 0; i < N; i++)
            arr[i] = i;

        start = MPI_Wtime();
        MPI_Isend(arr, N, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        end = MPI_Wtime();

        printf("Process 0: Non-blocking send of %d integers in %f seconds\n", N, end - start);
    } 
    else if (rank == 1) {
        start = MPI_Wtime();
        MPI_Irecv(arr, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        end = MPI_Wtime();

        printf("Process 1: Non-blocking receive of %d integers in %f seconds | First: %d, Last: %d\n",
               N, end - start, arr[0], arr[N - 1]);
    }

    free(arr);
    MPI_Finalize();
    return 0;
}
