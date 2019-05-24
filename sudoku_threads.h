#include <pthread.h>

#define MATRIX_SIZE 9
#define ROW "row"
#define COLUMN "col"
#define SUB_MATRIX "sub"

void get_row(int *arr, int row);

void get_col(int *arr, int col);

void get_sub_matrix(int *arr, int sub);

int check_correct_input();

void *check_arr(void *arg);

void get_matrix_from_terminal();

int get_matrix_from_file(char *file_name);

void free_arg_array(char **arg);

int calculate_result();

void init_arg_array(char **arg);