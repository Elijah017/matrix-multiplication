/**
 * Author: Elijah Mullens
 * 
 * generate two sparse square matrix of size <matrix-size>, benchmark the multiplication.
 * 
 * Compile: a CMakeLists has been provided
 * Execute: mpirun <mpi-options> matrix -s <matrix-size> -d <matrix-density>
 * 
 * Arguments:
 *   - matrix-size: integer 2-30,000 inclusive (default 100)
 *   - matrix-density: float 0.001-1 inclusive (default 0.01) 
 */

#include <omp.h>
#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "utils/list.h"
#include "utils/files.h"
#include "config.h"

#define U64_MAX_CHARS 20
#define U8_MAX_CHARS  3

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

char *u8_to_string(void *value) {
	char *string = malloc(U8_MAX_CHARS);
	snprintf(string, U8_MAX_CHARS, "%d", *(uint8_t*)value);
	return string;
}

int initialise_work(ProcessRegion *procs, Args *args) {
    int per_proc = args->msize / args->wsize;
    int remainder = args->msize % args->wsize;

    if (args->rank < remainder) {
        procs->start = args->rank * (per_proc + 1);
        procs->end = procs->start + per_proc + 1;
    } else {
        procs->start = (args->rank * per_proc) + remainder;
        procs->end = procs->start + per_proc;
    }
    procs->size = procs->end - procs->start;

	return args->msize;
}

int rank0_gen_matrix(CompressedMatrix *matrix_files, ProcessRegion *work, int msize, float density) {
	List matrix;
	init_list(&matrix, sizeof(List), msize, NULL);
	int row_capacity = msize * density;

	#pragma omp parallel for schedule(static)
	for (size_t i = 0; i < work->size; i++) {
		List *row = list_get_index(&matrix, i);
		init_list(row, sizeof(uint8_t), row_capacity, &u8_to_string);

		for (size_t j = 0; j < msize; j++) {
			float validate = rand()/RAND_MAX;

			if (validate > density) { continue; }

			int element = (rand() % 10) + 1;
			list_add_element(row, &element);
		}
	}
}

int parse_args(int argc, char **argv, Args *args) {
	int opt;
	bool size_set = false, density_set = false;
	bool exit_success = true;

	MPI_Comm_rank(MPI_COMM_WORLD, &args->rank);
	MPI_Comm_size(MPI_COMM_WORLD, &args->wsize);

	while ((opt = getopt(argc, argv, "s:d:")) != -1) {
		switch (opt) {
			case 's': {
				char *endptr;
				long temp = strtol(optarg, &endptr, 10);

				if (*endptr != '\0' || temp < MATRIX_MIN_SIZE || temp > MATRIX_MAX_SIZE) {
					fprintf(stderr, "Error (-s): incorrect value; expects integer between %d and %d\n",
						(uint32_t)MATRIX_MIN_SIZE,
						(uint32_t)MATRIX_MAX_SIZE);
					exit_success = false;
					break;
				}

				args->msize = (uint32_t)temp;
				size_set = true;
				break;
			}
			case 'd': {
				char *endptr;
				double temp = strtof(optarg, &endptr);

				if (*endptr != '\0' || temp < MATRIX_MIN_DENSITY || temp > MATRIX_MAX_DENSITY) {
					fprintf(stderr, "Error (-d): incorrect value; expects float between %.4f and %.4f\n",
						(float)MATRIX_MIN_DENSITY,
						(float)MATRIX_MAX_DENSITY);
					exit_success = false;
					break;
				}

				args->density = temp;
				density_set = true;
				break;
			}
			default: break;
		}
	}

	if (!exit_success) { exit(EXIT_FAILURE); }

	if (!size_set) {
		args->msize = (uint32_t)DEFAULT_MATRIX_SIZE;
	}
	if (!density_set) {
		args->density = (float)DEFAULT_MATRIX_DENSITY;
	}

	return 0;
}

int main(int argc, char **argv) {
    int size, rank, level = 0;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &level);
	
	Args args;
	parse_args(argc, argv, &args);

	ProcessRegion procs;
	int matrix_size = initialise_work(&procs, &args);

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
