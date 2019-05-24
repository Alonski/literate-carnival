/*
 * Created by Avihai Adler and Alon Bukai
 * */

void *validate_section(void *arg);

void free_arg_array(char **arg);

int calculate_result();

void init_arg_array(char **arg);

void init_threads(pthread_t *threads, char **arg);

void check_matrix_threads();
