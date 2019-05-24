
FLAGS = -Wall -pthread
INCLUDES = ../headers

all: sudoku_threads sudoku_threads_mutex

clean:
	rm -f v1 v2

sudoku_threads: sudoku_threads.c
	gcc -I $(INCLUDES) -o v1 sudoku_threads.c $(FLAGS)

sudoku_threads_mutex: sudoku_threads_mutex.c
	gcc -I $(INCLUDES) -o v2 sudoku_threads_mutex.c $(FLAGS)
