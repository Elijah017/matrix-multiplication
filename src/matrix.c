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
#include <stdlib.h>
#include <stdbool.h>

#include "utils/list.h"
#include "utils/files.h"

#define U64_MAX_CHARS 20

typedef struct {
    uint64_t start;
    uint64_t end;
    uint64_t size;
} ProcessRegion;


char *u64_to_string(void *value) {
	char* string = malloc(U64_MAX_CHARS);
	snprintf(string, U64_MAX_CHARS, "%ld", *(uint64_t*)value);
	return string;
}

int initialise_work(ProcessRegion *procs, char* msize, int rank, int size) {
    int matrix_size = atoi(msize);

    int per_proc = matrix_size / size;
    int remainder = matrix_size % size;

    if (rank < remainder) {
        procs->start = rank * (per_proc + 1);
        procs->end = procs->start + per_proc + 1;
    } else {
        procs->start = (rank * per_proc) + remainder;
        procs->end = procs->start + per_proc;
    }
    procs->size = procs->end - procs->start;

	return matrix_size;
}


int main(int argc, char **argv) {
    int size, rank, level = 0;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &level);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("threads: %d\n", omp_get_max_threads());

	ProcessRegion procs;
	int matrix_size = initialise_work(&procs, argv[1], rank, size);

	switch (rank) {
		case 0: {
			CompressedMatrix matrix_A, matrix_B, matrix_C;
			initialse_files(&matrix_A, &matrix_B, &matrix_C);

			close_files(&matrix_A, &matrix_B, &matrix_C);
			break;
		}
		default: {}
	}

    MPI_Finalize();
    return 0;
}
