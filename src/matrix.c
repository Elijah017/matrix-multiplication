/**
 * Author: Elijah Mullens
 *
 * generate two sparse square matrix of size <matrix-size>, benchmark the
 * multiplication.
 *
 * Compile: a CMakeLists has been provided
 * Execute: mpirun <mpi-options> matrix -s <matrix-size> -d <matrix-density>
 *
 * Arguments:
 *   - matrix-size: integer 2-30,000 inclusive (default 100)
 *   - matrix-density: float 0.001-1 inclusive (default 0.01)
 */

#include <assert.h>
#include <mpi.h>
#include <omp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "config.h"
#include "utils/files.h"
#include "utils/list.h"
#include "utils/strings.h"

typedef struct {
  uint64_t start;
  uint64_t end;
  uint64_t size;
} ProcessRegion;

int initialise_work(ProcessRegion *procs, Args *args) {
  int per_proc  = args->msize / args->wsize;
  int remainder = args->msize % args->wsize;

  if (args->rank < remainder) {
    procs->start = args->rank * (per_proc + 1);
    procs->end   = procs->start + per_proc + 1;
  } else {
    procs->start = (args->rank * per_proc) + remainder;
    procs->end   = procs->start + per_proc;
  }
  procs->size = procs->end - procs->start;

  return args->msize;
}

int parse_args(int argc, char **argv, Args *args) {
  int  opt;
  bool size_set = false, density_set = false;
  bool exit_success = true;

  MPI_Comm_rank(MPI_COMM_WORLD, &args->rank);
  MPI_Comm_size(MPI_COMM_WORLD, &args->wsize);

  while ((opt = getopt(argc, argv, "s:d:")) != -1) {
    switch (opt) {
      case 's': {
        char *endptr;
        long  temp = strtol(optarg, &endptr, 10);

        if ((*endptr != '\0') || (temp < MATRIX_MIN_SIZE) ||
            (temp > MATRIX_MAX_SIZE)) {
          fprintf(
            stderr,
            "Error (-s): incorrect value; expects integer between %d and %d\n",
            (uint32_t)MATRIX_MIN_SIZE, (uint32_t)MATRIX_MAX_SIZE);
          exit_success = false;
          break;
        }

        args->msize = (uint32_t)temp;
        size_set    = true;
        break;
      }
      case 'd': {
        char  *endptr;
        double temp = strtof(optarg, &endptr);

        if ((*endptr != '\0') || (temp < MATRIX_MIN_DENSITY) ||
            (temp > MATRIX_MAX_DENSITY)) {
          fprintf(stderr,
                  "Error (-d): incorrect value; expects float between %.4f and "
                  "%.4f\n",
                  (float)MATRIX_MIN_DENSITY, (float)MATRIX_MAX_DENSITY);
          exit_success = false;
          break;
        }

        args->density = temp;
        density_set   = true;
        break;
      }
      default:
        break;
    }
  }

  if (!exit_success) {
    exit(EXIT_FAILURE);
  }

  if (!size_set) {
    args->msize = (uint32_t)DEFAULT_MATRIX_SIZE;
  }

  if (!density_set) {
    args->density = (float)DEFAULT_MATRIX_DENSITY;
  }

  return 0;
}

int matrix_gather_rows(List *matrix, FILE *cache, ProcessRegion *work,
                       Args *args) {
  List  *first_val = list_get_index(matrix, 0);
  size_t data_size = first_val->_size;

  char *(*to_string)(void *) = first_val->_to_string;
  void  (*free_val)(void *) = first_val->_free_val;

  switch (args->rank) {
    case 0: {
      for (size_t i = 0; i < work->size; i++) {
        List *row = list_get_index(matrix, i);
        list_write_cache(row, cache);
        delete_list(row);
      }
      delete_list(matrix);

      for (int i = 1; i < args->wsize; i++) {
        Args rank_arg;
        memcpy(&rank_arg, args, sizeof(Args));
        rank_arg.rank = i;
        ProcessRegion rank_work;
        initialise_work(&rank_work, &rank_arg);

        for (size_t r = 0; r < rank_work.size; r++) {
          size_t length;
          MPI_Status status;
          MPI_Recv(&length, sizeof(size_t), MPI_BYTE, i, 's', MPI_COMM_WORLD,
                   &status);
          assert(status.MPI_ERROR == MPI_SUCCESS);

          init_list(matrix, data_size, length, to_string, free_val);
          MPI_Recv(matrix->list, matrix->_size * length, MPI_BYTE, i, 'v',
                   MPI_COMM_WORLD, &status);
          assert(status.MPI_ERROR == MPI_SUCCESS);

          list_write_cache(matrix, cache);
          delete_list(matrix);
        }
      }
      break;
    }
    default: {
      for (size_t i = 0; i < work->size; i++) {
        List *row = list_get_index(matrix, i);

        MPI_Send(&row->length,
                 sizeof(size_t),
                 MPI_BYTE,
                 0,
                 's',
                 MPI_COMM_WORLD);
        MPI_Send(row->list, row->length * row->_size, MPI_BYTE, 0, 'v',
                 MPI_COMM_WORLD);
      }

      delete_list(matrix);
      break;
    }
  }

  return 0;
}

int gen_matrix(FILE *cache, ProcessRegion *work, Args *args) {
  List matrix;

  init_list(&matrix, sizeof(List), work->size, NULL, &delete_list);
  matrix.length = work->size;
  size_t row_capacity = args->msize * args->density;

  #pragma omp parallel for num_threads(omp_get_max_threads() / 2)

  for (size_t row_i = 0; row_i < work->size; row_i++) {
    List *row = list_get_index(&matrix, row_i);
    init_list(row, sizeof(u8Compressed), row_capacity, &u8_to_string, NULL);

    for (size_t col = 0; col < args->msize; col++) {
      float validate = rand() / (float)RAND_MAX;

      if (validate > args->density) {
        continue;
      }

      u8Compressed value = (u8Compressed) {
        .val = (rand() % 10) + 1, .col = col
      };
      list_add_element(row, &value);
      printf("%d, %d\n", value.val, value.col);
    }
  }
  printf("\n");

  matrix_gather_rows(&matrix, cache, work, args);
  rewind(cache);

  return 0;
}

void catch_gdb(Args *args) {
  volatile int i = 0;

  while (i == 0) {
    fprintf(stdout, "rank: %d, pid: %d\n", args->rank, getpid());
    fflush(stdout);
    sleep(5);
  }
}

int main(int argc, char **argv) {
  int level;

  srand(time(NULL));

  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &level);
  omp_set_nested(1);
  omp_set_dynamic(1);

  Args args;
  parse_args(argc, argv, &args);

  if (args.rank == 0) { catch_gdb(&args); }

  ProcessRegion work;
  initialise_work(&work, &args);

  switch (args.rank) {
    case 0: {
      FILE *cacheA = tmpfile(), *cacheB = tmpfile();

      #pragma omp parallel num_threads(2)
      {
        #pragma omp single
        {
          #pragma omp task
          { gen_matrix(cacheA, &work, &args); }
          #pragma omp task
          { gen_matrix(cacheB, &work, &args); }
        }
      }

      dump_matrices(cacheA, cacheB, NULL, &args);

      fclose(cacheA);
      fclose(cacheB);
      break;
    }
    default: {
      #pragma omp parallel num_threads(2)
      {
        #pragma omp single
        {
          #pragma omp task
          { gen_matrix(NULL, &work, &args); }
          #pragma omp task
          { gen_matrix(NULL, &work, &args); }
        }
      }
    }
  }

  MPI_Finalize();
  return 0;
}
