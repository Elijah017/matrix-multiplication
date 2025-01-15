#ifndef _MATRIX_CONFIG
#define _MATRIX_CONFIG

#include <stdbool.h>

typedef struct {
	int rank;
	int wsize;
	uint32_t msize;
	float density;
} Args;

typedef struct {
	uint8_t val;
	uint32_t col;
} u8Compressed;

typedef struct {
	uint64_t val;
	uint32_t col;
} u64Compressed;

#define DUMP_A true
#define DUMP_B true
#define DUMP_C false

#define DEFAULT_MATRIX_SIZE    100
#define MATRIX_MIN_SIZE 	   2
#define MATRIX_MAX_SIZE 	   30000

#define DEFAULT_MATRIX_DENSITY 0.01
#define MATRIX_MIN_DENSITY 	   0.001
#define MATRIX_MAX_DENSITY 	   1.0

#define U64_MAX_CHARS 21
#define U32_MAX_CHARS 11
#define U8_MAX_CHARS  4

#endif