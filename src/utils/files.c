#include "files.h"

int initialse_files(CompressedMatrix *matrix_A, CompressedMatrix *matrix_B, CompressedMatrix *matrix_C) {
	if (DUMP_A) {
		open_matrix_file(matrix_A, "MatrixAX.txt", "MatrixAY.txt");
	} else {
		open_matrix_temp_file(matrix_A);
	}

	if (DUMP_B) {
		open_matrix_file(matrix_B, "MatrixBX.txt", "MatrixBY.txt");
	} else {
		open_matrix_temp_file(matrix_B);
	}

	if (DUMP_C) {
		open_matrix_file(matrix_C, "MatrixCX.txt", "MatrixCY.txt");
	} else {
		open_matrix_temp_file(matrix_C);
	}

	return 0;
}

int open_matrix_file(CompressedMatrix *matrix, const char *Xfile, const char *Yfile) {
	matrix->Xfile = fopen(Xfile, "w+");
	assert(matrix->Xfile != NULL);
	matrix->Yfile = fopen(Yfile, "w+");
	assert(matrix->Yfile != NULL);
	return 0;
}

int open_matrix_temp_file(CompressedMatrix *matrix) {
	matrix->Xfile = tmpfile();
	matrix->Yfile = tmpfile();
	return 0;
}

int close_matrix_file(CompressedMatrix *matrix) {
	fclose(matrix->Xfile);
	fclose(matrix->Yfile);
	return 0;
}

int close_files(CompressedMatrix *matrix_A, CompressedMatrix *matrix_B, CompressedMatrix *matrix_C) {
	close_matrix_file(matrix_A);
	close_matrix_file(matrix_B);
	close_matrix_file(matrix_C);
	return 0;
}