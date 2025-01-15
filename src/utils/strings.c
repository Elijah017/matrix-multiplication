#include "strings.h"

char *u64_to_string(void *value) {
	char* string = malloc(U64_MAX_CHARS );
	snprintf(string, U64_MAX_CHARS, "%ld", *(uint64_t*)value);
	return string;
}

char *u32_to_string(void *value) {
	char* string = malloc(U32_MAX_CHARS );
	snprintf(string, U32_MAX_CHARS, "%ld", *(uint64_t*)value);
	return string;
}

char *u8_to_string(void *value) {
	char *string = malloc(U8_MAX_CHARS);
	snprintf(string, U8_MAX_CHARS, "%d", *(uint8_t*)value);
	return string;
}