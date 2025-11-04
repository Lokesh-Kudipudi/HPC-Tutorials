// Hybrid_prime_counter.c
// A hybrid MPI + OpenMP program to count prime numbers up to a given limit.
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define LIMIT 1000000  // Define the upper limit for prime counting

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num == 2) return 1; // 2 is prime
    if (num % 2 == 0) return 0; // Exclude even numbers
    for (int i = 3; i <= sqrt(num); i += 2) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// Hybrid MPI + OpenMP main: each MPI rank takes a strided subset of numbers
// and uses OpenMP threads to check primality in parallel. The total is
// reduced to rank 0 which prints the result and timing.
int main(int argc, char *argv[]) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // default limit, can be overridden by command line
    int limit = LIMIT;
    if (argc > 1) {
        long tmp = atol(argv[1]);
        if (tmp > 1) limit = (int)tmp;
    }

    // compute the number of iterations for this rank using a strided distribution
    int start = 2 + rank; // first number this rank checks
    long iterations = 0;
    if (start <= limit) {
        iterations = (limit - start) / size + 1;
    }

    // warm-up: query number of threads
    int max_threads = omp_get_max_threads();
    if (rank == 0) {
        printf("Hybrid MPI+OpenMP prime counter: limit=%d, ranks=%d, threads_per_rank=%d\n",
               limit, size, max_threads);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    long local_count = 0;

    // Parallelize over the iteration index j; compute the actual number n for each j
#pragma omp parallel for reduction(+:local_count) schedule(dynamic, 64)
    for (long j = 0; j < iterations; ++j) {
        int n = start + (int)(j * (long)size);
        if (n <= limit) {
            if (is_prime(n)) local_count++;
        }
    }

    double local_time = MPI_Wtime() - t0;

    long global_count = 0;
    double max_time = 0.0;
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total primes up to %d: %ld\n", limit, global_count);
        printf("Elapsed time (max over ranks): %.6f s\n", max_time);
    }

    MPI_Finalize();
    return 0;
}

