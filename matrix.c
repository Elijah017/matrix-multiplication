/**
 * Author: Elijah Mullens
 * 
 * generate two sparse square matrix of size <matrix-size>, benchmark the multiplication.
 * 
 * Compile: mpicc -fopenmp <file> -o <out-file>
 * Execute: mpirun <mpi-options> <out-file> <matrix-size>
 */

#include <omp.h>
#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t size;
} ProcessWork;

int main(int argc, char **argv) {
    int size, rank, level = 0;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &level);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("threads: %d\n", omp_get_max_threads());

    int matrix_size = atoi(argv[1]);

    ProcessWork workAlloc;
    int per_proc = matrix_size / size;
    int remainder = matrix_size % size;

    if (rank < remainder) {
        workAlloc.start = rank * (per_proc + 1);
        workAlloc.end = workAlloc.start + per_proc + 1;
    } else {
        workAlloc.start = (rank * per_proc) + remainder;
        workAlloc.end = workAlloc.start + per_proc;
    }
    workAlloc.size = workAlloc.end - workAlloc.start;

    MPI_Finalize();
    return 0;
}
