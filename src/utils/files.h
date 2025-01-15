#ifndef _MATRIX_FILES
#define _MATRIX_FILES

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../config.h"
#include "list.h"
#include "strings.h"

void write_matrix(const char *Xname, const char *Yname, FILE *cache, Args *args, bool u8);

void dump_matrices(FILE *A, FILE *B, FILE *C, Args *args);

#endif