/*
 * Created by Avihai Adler and Alon Bukai
 * */

typedef struct Tasks {
    char *tasks[MATRIX_SIZE * 3];
    int taskPointer;
    int complete;
} Tasks;

void check_matrix_mutex();

void init_tasks(Tasks *tasks);

void free_tasks(Tasks *tasks);

void *check_tasks(void *arg);

int do_task(char *str);

void wait_for_done();

void init_threads_mutex(pthread_t *threads, Tasks *tasks);
