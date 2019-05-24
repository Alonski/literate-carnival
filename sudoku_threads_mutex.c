/*
 * Created by Avihai Adler and Alon Bukai
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "sudoku_threads_shared.h"
#include "sudoku_mutex.h"

#define THREADS_AMOUNT 3

static int done = 0;
int matrix[MATRIX_SIZE][MATRIX_SIZE];
int result = 0;

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t done_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t done_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        get_matrix_from_terminal();
    } else {
        if (!get_matrix_from_file(argv[1])) {
            return -1;
        }
    }
    check_matrix_mutex();

    return 0;
}

void get_matrix_from_terminal() {
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

int get_matrix_from_file(char *file_name) {
    // Get matrix from file
    int tmp = 0, i, j;
    FILE *pf = fopen(file_name, "r");
    if (pf != NULL) {
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
    } else {
        perror("Error: Something wrong with your file\n");
        return 0;
    }
    return 1;
}

void check_matrix_mutex() {
    pthread_t threads[THREADS_AMOUNT];
    Tasks tasks;

    init_tasks(&tasks);
    init_threads_mutex(threads, &tasks);
    join_threads(threads, THREADS_AMOUNT);
    wait_for_done();
    free_tasks(&tasks);

    if (result == MATRIX_SIZE * 3)
        printf("solution is legal\n");
    else
        printf("solution is not legal\n");
}

void init_tasks(Tasks *tasks) {
    /* Initialize an array of tasks e.g: row 0 or sub 3 */
    int i;
    char numAsAscii[MATRIX_SIZE];

    // Initialize strings
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        tasks->tasks[i] = malloc(6 * sizeof(char));
    }

    // Write tasks
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        if (i < MATRIX_SIZE) {
            strcpy(tasks->tasks[i], ROW);
        } else if (i >= MATRIX_SIZE && i < MATRIX_SIZE * 2) {
            strcpy(tasks->tasks[i], COLUMN);
        } else {
            strcpy(tasks->tasks[i], SUB_MATRIX);
        }
        sprintf(numAsAscii, "%i", i % 9);
        strcat(tasks->tasks[i], " ");
        strcat(tasks->tasks[i], numAsAscii);
    }
    // Initialize task pointer
    tasks->taskPointer = 0;

    // Initialize amount of completed threads
    tasks->complete = 0;
}

void init_threads_mutex(pthread_t *threads, Tasks *tasks) {
    // Initialize threads
    int i;
    for (i = 0; i < THREADS_AMOUNT; i++) {
        pthread_create(&threads[i], NULL, check_tasks, tasks);
    }
}

void join_threads(pthread_t *threads, int size) {
    int i;
    for (i = 0; i < size; i++) {
        pthread_join(threads[i], NULL);
    }
}

void wait_for_done() {
    // After all threads are done
    pthread_mutex_lock(&done_mutex);
    if (!done)                                            // Last thread hasn't finish
        pthread_cond_wait(&done_cond, &done_mutex);        // Wait, release lock
    pthread_mutex_unlock(&done_mutex);
}

void free_tasks(Tasks *tasks) {
    // Free allocated tasks
    int i;
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        free(tasks->tasks[i]);
    }
}

void *check_tasks(void *arg) {
    /* Thread function */
    Tasks *tasks = (Tasks *) arg;
    char *task;
    int res, taskPointer;

    while (!done) {
        pthread_mutex_lock(&read_mutex);                    // Acquire lock
        taskPointer = tasks->taskPointer;
        tasks->taskPointer++;                                // Move pointer to next task
        pthread_mutex_unlock(&read_mutex);                    // Release lock

        if (taskPointer < MATRIX_SIZE * 3) {
            task = tasks->tasks[taskPointer];                // Get task
            res = do_task(task);                                // Get result

            if (res > 0) {                                    // Write result in case of correct row/column/sub matrix
                pthread_mutex_lock(&result_mutex);
                result++;
                pthread_mutex_unlock(&result_mutex);
            }
            pthread_mutex_lock(&result_mutex);
            tasks->complete++;
            pthread_mutex_unlock(&result_mutex);
        }
            // Last thread (the one who perform the last task) wake the main thread
        else {
            while (tasks->complete != MATRIX_SIZE * 3);        // Wait on last thread to complete
            pthread_mutex_lock(&done_mutex);
            done = 1;
            pthread_cond_signal(&done_cond);                // Send 'wake' signal
            pthread_mutex_unlock(&done_mutex);
        }

    }

    pthread_exit(NULL);
}

int do_task(char *str) {
    /* Do ONE task: check one row/one column/one sub matrix */
    int i;
    int arr[MATRIX_SIZE];
    int check_arr[MATRIX_SIZE] = {0};
    char *type = strtok(str, " ");    // Get the 'name' of the task: row/col/sub
    int num = atoi(&str[4]); // Get the num of row/column/sub matrix
    int res = 1;

    if (strcmp(type, ROW) == 0) {
        get_row(arr, num);
    } else if (strcmp(type, COLUMN) == 0) {
        get_col(arr, num);
    } else if (strcmp(type, SUB_MATRIX) == 0) {
        get_sub_matrix(arr, num);
    }

    // Iterate over the array: every number (value) of arr have a representation at the check_arr array in [number - 1] position
    for (i = 0; i < MATRIX_SIZE; i++) {
        check_arr[arr[i] - 1]++;
    }

    // Check the number of occurrences of each number
    for (i = 0; i < MATRIX_SIZE; i++) {
        if (check_arr[i] > 1 || check_arr[i] < 1) {
            res = -1;
            break;
        }
    }
    return res;
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

