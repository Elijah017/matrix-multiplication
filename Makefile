CC = mpicc
FLAGS = 

all: matrix

bin/list.o: utils/list.c
	$(CC) $(FLAGS) -c $< -o $@

matrix: matrix.c bin/list.o
	$(CC) $(FLAGS) -fopenmp $^ -o $@

clean:
	rm -f bin/* matrix
