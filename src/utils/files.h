#ifndef _MATRIX_FILES
#define _MATRIX_FILES

#include <stdlib.h>
#include <stdio.h>

#include "../config.h"

typedef struct {
	FILE *Xfile;
	FILE *Yfile;
} CompressedMatrix;

int initialse_files(CompressedMatrix *matrix_A, CompressedMatrix *matrix_B, CompressedMatrix *matrix_C);

int open_matrix_file(CompressedMatrix *matrix, const char *Xfile, const char *Yfile);

int open_matrix_temp_file(CompressedMatrix *matrix);

int close_matrix_file(CompressedMatrix *matrix);

int close_files(CompressedMatrix *matrix_A, CompressedMatrix *matrix_B, CompressedMatrix *matrix_C);

#endif