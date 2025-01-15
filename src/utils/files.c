#include "files.h"

void write_matrix(const char *Xname, const char *Yname, FILE *cache, Args *args, bool u8) {
	FILE *Xfile = fopen(Xname, "w");
	FILE *Yfile = fopen(Yname, "w");

	for (size_t i = 0; i < args->msize; i++) {
		List row;
		if (u8) { init_list(&row, sizeof(uint8_t), 0, NULL, NULL); }
		else { init_list(&row, sizeof(uint64_t), 0, NULL, NULL); }

		list_read_cache(&row, cache);

		fwrite("[ ", sizeof(char), 2, Xfile);
		fwrite("[ ", sizeof(char), 2, Yfile);
		for (size_t v = 0; v < row.length; v++) {
			char *val, *col;
			if (u8) {
				u8Compressed *item = list_get_index(&row, v);
				printf("%d, %d\n", item->val, item->col);
				val = u8_to_string(&item->val);
				col = u32_to_string(&item->col);
			}
			else {
				u64Compressed *item = list_get_index(&row, v);
				val = u64_to_string(&item->val);
				col = u32_to_string(&item->col);
			}

			size_t vallen = strlen(val);
			size_t collen = strlen(col);
			if (v < row.length - 1) {
				val = realloc(val, vallen + 3);
				col = realloc(col, collen + 3);

				strcpy(&(val[vallen]), ", ");
				strcpy(&(col[collen]), ", ");
			} else {
				val = realloc(val, vallen + 2);
				col = realloc(col, collen + 2);

				strcpy(&(val[vallen]), " ");
				strcpy(&(col[collen]), " ");
			}

			fwrite(val, sizeof(char), strlen(val), Xfile);
			fwrite(col, sizeof(char), strlen(col), Yfile);
			free(val); val = NULL;
			free(col); col = NULL;
		}
		if (i < args->msize - 1) {
			fwrite("]\n", sizeof(char), 2, Xfile);
			fwrite("]\n", sizeof(char), 2, Yfile);
		} else {
			fwrite("]", sizeof(char), 1, Xfile);
			fwrite("]", sizeof(char), 1, Yfile);
		}
	}

	fclose(Xfile);
	fclose(Yfile);
}

void dump_matrices(FILE *A, FILE *B, FILE *C, Args *args) {
	#pragma omp parallel num_threads(3)
	{
		#pragma omp single
		{
			if (DUMP_A) {
				#pragma omp task
				{ write_matrix("MatrixAX.txt", "MatrixAY.txt", A, args, true); }
			}
			if (DUMP_B) {
				#pragma omp task
				{ write_matrix("MatrixBX.txt", "MatrixBY.txt", B, args, true); }
			}
			if (DUMP_C) {
				#pragma omp task
				{ write_matrix("MatrixCX.txt", "MatrixCY.txt", C, args, false); }
			}
		}
	}
}