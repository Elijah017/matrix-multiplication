CC = mpicc
FLAGS = 

all: matrix

bin/list.o: utils/list.c
	$(CC) $(FLAGS) -c $^ -o $@

bin/files.o: utils/files.c
	$(CC) $(FLAGS) -c $^ -o $@

matrix: matrix.c bin/list.o bin/files.o config.h
	$(CC) $(FLAGS) -fopenmp $^ -o $@

clean:
	rm -f bin/* matrix
