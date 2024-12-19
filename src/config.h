#ifndef _MATRIX_CONFIG
#define _MATRIX_CONFIG

#include <stdbool.h>

typedef struct {
	int rank;
	int wsize;
	uint32_t msize;
	float density;
} Args;

#define DUMP_A false
#define DUMP_B false
#define DUMP_C false

#define DEFAULT_MATRIX_SIZE    100
#define MATRIX_MIN_SIZE 	   2
#define MATRIX_MAX_SIZE 	   30000

#define DEFAULT_MATRIX_DENSITY 0.01
#define MATRIX_MIN_DENSITY 	   0.001
#define MATRIX_MAX_DENSITY 	   1.0

#endif