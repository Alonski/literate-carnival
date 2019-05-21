/*
 * Created by Avihai Adler and Alon Bukai
 * */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MATRIX_SIZE 9
#define ROW "row"
#define COLUMN "col"
#define SUB_MATRIX "sub"

void get_row(int *arr, int row);

void get_col(int *arr, int col);

void get_subMatrix(int *arr, int sub);

int check_correctInput();

void *check_arr(void *arg);

void print_arr(int *arr);

int matrix[MATRIX_SIZE][MATRIX_SIZE];
int result[MATRIX_SIZE * 3];

int main(int argc, char *argv[]) {
    pthread_t threads[MATRIX_SIZE * 3];
    FILE *pf;
    int i, j;
    char *arg[MATRIX_SIZE * 3];
    int res = 0;

    if (argc < 2) {
        do {
            //get matrix from terminal
            printf("Insert solution:\n");
            for (i = 0; i < MATRIX_SIZE; i++) {
                for (j = 0; j < MATRIX_SIZE; j++) {
                    scanf(" %d", &matrix[i][j]);
                }
            }
            if (check_correctInput() < 0) {
                perror("Wrong input\n");
            }
        } while (check_correctInput() < 0);
    } else {
        //get matrix from file
        char c;
        pf = fopen(argv[1], "r");
        for (i = 0; i < MATRIX_SIZE; i++) {
            for (j = 0; j < MATRIX_SIZE; j++) {
                fscanf(pf, "%d%c", &matrix[i][j], &c);
            }
        }
        if (check_correctInput() < 0) {
            perror("Wrong input: check file\n");
            return -1;
        }
    }

    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        arg[i] = NULL;
    }
    //Initialize threads
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        char tmp[MATRIX_SIZE];
        arg[i] = realloc(arg[i], 6 * sizeof(char));            // initialize the "command" arguments

        if (i < MATRIX_SIZE) {                                    //i between 0 and 9 --> check rows
            arg[i] = strcpy(arg[i], ROW);
        } else if (i >= MATRIX_SIZE && i < MATRIX_SIZE * 2) {    //i between 9 and 18 --> check column
            arg[i] = strcpy(arg[i], COLUMN);
        } else {                                                //i between 18 and 27 --> check sub matrix
            arg[i] = strcpy(arg[i], SUB_MATRIX);
        }

        arg[i] = strcat(arg[i], " ");
        sprintf(tmp, "%i", i % 9);
        arg[i] = strcat(arg[i], tmp);
        pthread_create(&threads[i], NULL, check_arr, arg[i]);
    }

    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        pthread_join(threads[i], NULL);
    }

    for (i = 0; i < MATRIX_SIZE * 3; i++)
        free(arg[i]);

    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        if (result[i] > 0)
            res++;
    }

    if (res == MATRIX_SIZE * 3)
        printf("Solution is legal\n");
    else
        printf("Solution isn't legal\n");

    return 0;
}

void *check_arr(void *arg) {
    /*Thread: expect an argument of type string, which contain a command
     * for example: "row 1" or "sub 3"*/
    int i;
    int arr[MATRIX_SIZE];
    int check[MATRIX_SIZE] = {0};
    char *str = (char *) arg;
    char *p = strtok(str, " ");
    int num, res = 1;

    if (strcmp(p, ROW) == 0) {
        p = strtok(NULL, " ");
        num = atoi(p);
        get_row(arr, num);
    } else if (strcmp(p, COLUMN) == 0) {
        p = strtok(NULL, " ");
        num = atoi(p);
        get_col(arr, num);
        num += MATRIX_SIZE;
    } else if (strcmp(p, SUB_MATRIX) == 0) {
        p = strtok(NULL, " ");
        num = atoi(p);
        get_subMatrix(arr, num);
        num += MATRIX_SIZE * 2;
    }

    //iterate over the array: every number (value) of arr have a representation at the check array in [number - 1] position
    for (i = 0; i < MATRIX_SIZE; i++) {
        check[arr[i] - 1]++;
    }

    for (i = 0; i < MATRIX_SIZE; i++) {
        if (check[i] == 0 || check[i] > 1) {
            res = 0;
            break;
        }
    }
    result[num] = res;

    pthread_exit(NULL);
}

void print_arr(int *arr) {
    int i;
    for (i = 0; i < MATRIX_SIZE; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void get_row(int *arr, int row) {
    int i;
    for (i = 0; i < MATRIX_SIZE; i++) {
        arr[i] = matrix[row][i];
    }
}

void get_col(int *arr, int col) {
    int i;
    for (i = 0; i < MATRIX_SIZE; i++) {
        arr[i] = matrix[i][col];
    }
}

void get_subMatrix(int *arr, int sub) {
    int i, j;
    switch (sub) {
        case 0:
            i = 0;
            j = 0;
            break;
        case 1:
            i = 0;
            j = 3;
            break;
        case 2:
            i = 0;
            j = 6;
            break;
        case 3:
            i = 3;
            j = 0;
            break;
        case 4:
            i = 3;
            j = 6;
            break;
        case 5:
            i = 3;
            j = 6;
            break;
        case 6:
            i = 6;
            j = 0;
            break;
        case 7:
            i = 6;
            j = 3;
            break;
        case 8:
            i = 6;
            j = 6;
            break;
    }
    int k, l, m = 0;
    for (k = i; k < i + 3; k++) {
        for (l = j; l < j + 3; l++) {
            arr[m++] = matrix[k][l];
        }
    }
}

int check_correctInput() {
    int i, j;
    int res = 1;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            if (matrix[i][j] > 9 || matrix[i][j] < 1)
                res = -1;
        }
    }
    return res;
}

