/*
 * Created by Avihai Adler and Alon Bukai
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define THREADS_AMOUNT 3
#define MATRIX_SIZE 9
#define ROW "row"
#define COLUMN "col"
#define SUB_MATRIX "sub"

typedef struct Tasks {
    char *tasks[MATRIX_SIZE * 3];
    int taskPointer;
    int complete;
} Tasks;

void initTasks(Tasks *tasks);

void freeTasks(Tasks *tasks);

void *checkMatrix(void *arg);

int doTask(char *str);

void get_row(int *arr, int row);

void get_col(int *arr, int col);

void get_subMatrix(int *arr, int sub);

int check_correctInput();

static int done = 0;
int result = 0;
int matrix[MATRIX_SIZE][MATRIX_SIZE];

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t done_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t done_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[]) {
    int i, j;
    FILE *pf;
    Tasks tasks;
    pthread_t threads[THREADS_AMOUNT];

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
                printf("Wrong input try again\n");
            }
        } while (check_correctInput() < 0);
    } else {
        //get matrix from file
        char c;
        pf = fopen(argv[1], "r");
        if (pf != NULL) {
            for (i = 0; i < MATRIX_SIZE; i++) {
                for (j = 0; j < MATRIX_SIZE; j++) {
                    fscanf(pf, "%d%c", &matrix[i][j], &c);
                }
            }
        } else {
            perror("Error: Something wrong with your file\n");
            return -1;
        }
        if (check_correctInput() < 0) {
            printf("Wrong input: check file\n");
            return -1;
        }
    }

    //Initialize an array of tasks
    initTasks(&tasks);

    //Initialize threads
    for (i = 0; i < THREADS_AMOUNT; i++) {
        pthread_create(&threads[i], NULL, checkMatrix, &tasks);
    }

    //after all threads are done
    pthread_mutex_lock(&done_mutex);
    if (!done)                                            //last thread hasn't finish
        pthread_cond_wait(&done_cond, &done_mutex);        //wait, release lock
    pthread_mutex_unlock(&done_mutex);

    if (result == MATRIX_SIZE * 3) {
        printf("Solution is legal\n");
    } else {
        printf("Solution isn't legal\n");
    }

    //Free allocated tasks
    freeTasks(&tasks);

    for (i = 0; i < THREADS_AMOUNT; i++)
        pthread_join(threads[i], NULL);

    return 0;
}

void *checkMatrix(void *arg) {
    /* Thread function */
    Tasks *tasks = (Tasks *) arg;
    char *task;
    int res, taskPointer;

    while (!done) {
        pthread_mutex_lock(&read_mutex);                    //acquire lock
        taskPointer = tasks->taskPointer;
        tasks->taskPointer++;                                //move pointer to next task
        pthread_mutex_unlock(&read_mutex);                    //release lock

        if (taskPointer < MATRIX_SIZE * 3) {
            task = tasks->tasks[taskPointer];                //get task
            res = doTask(task);                                //get result

            if (res > 0) {                                    //write result in case of correct row/column/sub matrix
                pthread_mutex_lock(&result_mutex);
                result++;
                pthread_mutex_unlock(&result_mutex);
            }
            pthread_mutex_lock(&result_mutex);
            tasks->complete++;
            pthread_mutex_unlock(&result_mutex);
        }
            //Last thread (the one who perform the last task) wake the main thread
        else {
            while (tasks->complete != MATRIX_SIZE * 3);        //wait on last thread to complete
            pthread_mutex_lock(&done_mutex);
            done = 1;
            pthread_cond_signal(&done_cond);                //send 'wake' signal
            pthread_mutex_unlock(&done_mutex);
        }

    }

    pthread_exit(NULL);
}

int doTask(char *str) {
    /* Do ONE task: check one row/one column/one sub matrix */
    int i;
    int num;
    int res = 1;
    char *token = strtok(str, " ");    //get the 'name' of the task: row/col/sub
    int arr[MATRIX_SIZE];
    int check_arr[MATRIX_SIZE] = {0};

    if (strcmp(token, ROW) == 0) {
        num = atoi(&str[4]);            //get the number of row
        get_row(arr, num);                //get the row itself
    } else if (strcmp(token, COLUMN) == 0) {
        num = atoi(&str[4]);            //get the number of column
        get_col(arr, num);                //get the column itself
    } else if (strcmp(token, SUB_MATRIX) == 0) {
        num = atoi(&str[4]);            //get the number of sub matrix
        get_subMatrix(arr, num);        //get the sub matrix itself
    }

    //Make a representation of the occurrences of the number
    for (i = 0; i < MATRIX_SIZE; i++) {
        check_arr[arr[i] - 1]++;
    }

    //Check the number of occurrences of each number
    for (i = 0; i < MATRIX_SIZE; i++) {
        if (check_arr[i] > 1 || check_arr[i] < 1) {
            res = -1;
            break;
        }
    }
    return res;
}

void initTasks(Tasks *tasks) {
    /* Initialize an array of tasks e.g: row 0 or sub 3 */
    int i;
    char numAsAscii[MATRIX_SIZE];

    //Initialize strings
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        tasks->tasks[i] = malloc(6 * sizeof(char));
    }

    //Write tasks
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
    //Initialize task pointer
    tasks->taskPointer = 0;

    //Initialize amount of completed threads
    tasks->complete = 0;
}

void freeTasks(Tasks *tasks) {
    int i;
    for (i = 0; i < MATRIX_SIZE * 3; i++) {
        free(tasks->tasks[i]);
    }
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

