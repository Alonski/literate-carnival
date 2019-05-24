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

void get_sub_matrix(int *arr, int sub);

int check_correct_input();

void *check_arr(void *arg);

void getMatrixFromTerminal();

int getMatrixFromFile(char *file_name);

void init_threads(pthread_t *threads, char **arg);

void join_threads(pthread_t *threads);

void free_arg_array(char **arg);

int calculate_result();

void init_arg_array(char **arg);

void check_matrix(pthread_t *threads);

int matrix[MATRIX_SIZE][MATRIX_SIZE];
int result[MATRIX_SIZE * 3];

int main(int argc, char *argv[]) {
    pthread_t threads[MATRIX_SIZE * 3];
    if (argc < 2) {
        getMatrixFromTerminal();
    } else {
        if (!getMatrixFromFile(argv[1])) {
            return -1;
        }
    }
    check_matrix(threads);

    return 0;
}

void getMatrixFromTerminal() {
    int i, j;
    do {
        // Get matrix from terminal
        printf("Insert solution:\n");
        for (i = 0; i < MATRIX_SIZE; i++) {
            for (j = 0; j < MATRIX_SIZE; j++) {
                scanf(" %d", &matrix[i][j]);
                if (matrix[i][j] > 9 || matrix[i][j] < 1) {
                    perror("Wrong input\n");
                }
            }
        }
    } while (check_correct_input() < 0);
}

int getMatrixFromFile(char *file_name) {
    // Get matrix from file
    int tmp = 0, i, j;
    FILE *pf = fopen(file_name, "r");
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            fscanf(pf, "%d", &tmp);
            if (tmp > 9 || tmp < 1) {
                perror("Wrong input: check file\n");
                return 0;
            }
            matrix[i][j] = tmp;
        }
    }
    return 1;
}

void check_matrix(pthread_t *threads) {
    char *arg[MATRIX_SIZE * 3];
    init_arg_array(arg);
    init_threads(threads, arg);
    join_threads(threads);
    free_arg_array(arg);
    int result = calculate_result();
    if (result == MATRIX_SIZE * 3)
        printf("solution is legal\n");
    else
        printf("solution is not legal\n");
}

void init_arg_array(char **arg) {
    int i;
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        arg[i] = NULL;
    }
}

void init_threads(pthread_t *threads, char **arg) {
    // Initialize threads
    int i;
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        char tmp[2];
        arg[i] = realloc(arg[i], 6 * sizeof(char));            // initialize the "command" arguments

        if (i < MATRIX_SIZE) {                                    // i between 0 and 9 --> check rows
            arg[i] = strcpy(arg[i], ROW);
        } else if (i >= MATRIX_SIZE && i < MATRIX_SIZE * 2) {    // i between 9 and 18 --> check column
            arg[i] = strcpy(arg[i], COLUMN);
        } else {                                                // i between 18 and 27 --> check sub matrix
            arg[i] = strcpy(arg[i], SUB_MATRIX);
        }

        arg[i] = strcat(arg[i], " ");
        sprintf(tmp, "%i", i % 9);
        arg[i] = strcat(arg[i], tmp);
        pthread_create(&threads[i], NULL, check_arr, arg[i]);
    }
}

void join_threads(pthread_t *threads) {
    int i;
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        pthread_join(threads[i], NULL);
    }
}

void free_arg_array(char **arg) {
    int i;
    for (i = 0; i < MATRIX_SIZE * 3; i++)
        free(arg[i]);
}

int calculate_result() {
    int i, res = 0;
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        if (result[i] > 0)
            res++;
    }
    return res;
}

void *check_arr(void *arg) {
    /* Thread: expect an argument of type string, which contain a command
     * for example: "row 1" or "sub 3"
    */
    int i;
    int arr[MATRIX_SIZE];
    int check[MATRIX_SIZE] = {0};
    char *str = (char *) arg;
    char *type = strtok(str, " ");
    char *s_num = strtok(NULL, " ");
    int num = atoi(s_num);
    int res = 1;

    if (strcmp(type, ROW) == 0) {
        get_row(arr, num);
    } else if (strcmp(type, COLUMN) == 0) {
        get_col(arr, num);
        num += MATRIX_SIZE;
    } else if (strcmp(type, SUB_MATRIX) == 0) {
        get_sub_matrix(arr, num);
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

void get_sub_matrix(int *arr, int sub) {
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
        default:
            return;
    }
    int k, l, m = 0;
    for (k = i; k < i + 3; k++) {
        for (l = j; l < j + 3; l++) {
            arr[m++] = matrix[k][l];
        }
    }
}

int check_correct_input() {
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



