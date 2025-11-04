#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000  // 1 million elements

int main(int argc, char *argv[]) {
    int rank, size;
    double start, end;
    int *arr = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0)
            printf("Run with at least 2 processes.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        arr = (int *)malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            arr[i] = i;

        start = MPI_Wtime();
        MPI_Send(arr, N, MPI_INT, 1, 0, MPI_COMM_WORLD);
        end = MPI_Wtime();

        printf("Process 0: Sent %d integers in %f seconds\n", N, end - start);
        free(arr);
    } 
    else if (rank == 1) {
        arr = (int *)malloc(N * sizeof(int));

        start = MPI_Wtime();
        MPI_Recv(arr, N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        end = MPI_Wtime();

        printf("Process 1: Received %d integers in %f seconds | First: %d, Last: %d\n",
               N, end - start, arr[0], arr[N - 1]);
        free(arr);
    }

    MPI_Finalize();
    return 0;
}
