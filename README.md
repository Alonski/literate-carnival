# Sudoku Threads Checker

Created by Avihai Adler and Alon Bukai

## Two Sudoku Checker Implementations

### Threads

### Threads Mutex

## Instructions to Build

To build this project a *Makefile* is included.

Just run `make` in your terminal.

## Instructions to Run

This project can be supplied a 9x9 matrix separated by spaces and line breaks as a file passed as an argument.
If a file is not passed as an argument then the matrix can be inputted from the terminal.

An example to run the *Threads* program:
```
./v1 tests/demo.txt
```
The above should print: `solution is legal`

An example to run the *Threads* program:
```
./v2 tests/wrong1
```
The above should print: `solution is not legal`