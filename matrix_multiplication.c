#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#define MAX_NAME_SIZE 256

typedef struct {
    int **matrix;
    int rows;
    int cols;
} matrix_t;

typedef struct {
    matrix_t *A;
    matrix_t *B;
    matrix_t *C;
    int row_offset;
    int col_offset;
    int rows;
    int cols;
} threaded_matrix_mult_args_t;

typedef struct {
    char *path_a;
    char *path_b;
    char *path_c_per_matrix;
    char *path_c_per_row;
    char *path_c_per_element;
} paths_t;


char *str_append(const char *src, const char *appended) {
    char *str = malloc(strlen(src) + strlen(appended) + 1);
    if (str == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    strcpy(str, src);
    strcat(str, appended);;
    return str;
}

FILE *open_file(const char *path, const char *mode) {
    FILE *file = fopen(path, mode);
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    return file;
}

matrix_t *allocate_matrix(const int rows,const int cols) {
    matrix_t *m = malloc(sizeof(matrix_t));
    if (m == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    m->rows = rows;
    m->cols = cols;
    m->matrix = malloc(sizeof(int*) * rows);
    if (m->matrix == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        m->matrix[i] = malloc(sizeof(int) * cols);
        if (m->matrix[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
    }
    return m;
}

void free_matrix(matrix_t *m) {
    if (m != NULL) {
        if (m->matrix != NULL) {
            for (int i = 0; i < m->rows; i++) {
                free(m->matrix[i]);
            }
            free(m->matrix);
        }
        free(m);
    }
}

matrix_t *read_matrix(FILE *file) {
    int rows, cols;
    fscanf(file, "row=%d col=%d", &rows, &cols);
    matrix_t *m = allocate_matrix(rows, cols);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            fscanf(file, "%d", &m->matrix[i][j]);
        }
    }
    return m;
}

void write_matrix(FILE *file, const matrix_t *m) {
    fprintf(file, "row=%d col=%d\n", m->rows, m->cols);
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols - 1; ++j) {
            fprintf(file, "%d ", m->matrix[i][j]);
        }
        fprintf(file, "%d", m->matrix[i][m->cols - 1]);
        if (i != m->rows - 1) {
            fprintf(file, "\n");
        }
    }
}

void clear_matrix(const matrix_t *m) {
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            m->matrix[i][j] = 0;
        }
    }
}

void *threaded_matrx_mult(void *args) {
    threaded_matrix_mult_args_t *mult_args = args;
    const matrix_t *A = mult_args->A;
    const matrix_t *B = mult_args->B;
    const matrix_t *C = mult_args->C;

    for (int i = 0; i < mult_args->rows; i++) {
        for (int j = 0; j < mult_args->cols; j++) {
            int sum = 0;
            for (int k = 0; k < A->cols; k++) {
                sum += A->matrix[i + mult_args->row_offset][k] * B->matrix[k][j + mult_args->col_offset];
            }
            C->matrix[i + mult_args->row_offset][j + mult_args->col_offset] = sum;
        }
    }
    pthread_exit(NULL);
}

paths_t *create_paths(const int argc, char *argv[]) {
    char *path_a = strdup("a.txt");
    char *path_b = strdup("b.txt");
    char *name_c = strdup("c");

    if (argc == 4) {
        free(path_a);
        free(path_b);
        free(name_c);
        path_a = str_append(argv[1], ".txt");
        path_b = str_append(argv[2], ".txt");
        name_c = strdup(argv[3]);
    }

    char *path_c_per_matrix = str_append(name_c, "_per_matrix.txt");
    char *path_c_per_row = str_append(name_c, "_per_row.txt");
    char *path_c_per_element = str_append(name_c, "_per_element.txt");
    free(name_c);

    paths_t *paths = malloc(sizeof(paths_t));
    if (paths == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    paths->path_a = path_a;
    paths->path_b = path_b;
    paths->path_c_per_matrix = path_c_per_matrix;
    paths->path_c_per_row = path_c_per_row;
    paths->path_c_per_element = path_c_per_element;

    return paths;
}

void free_paths(paths_t *paths) {
    if (paths) {
        free(paths->path_a);
        free(paths->path_b);
        free(paths->path_c_per_matrix);
        free(paths->path_c_per_row);
        free(paths->path_c_per_element);
        free(paths);
    }
}

int main(const int argc, char *argv[]) {
    paths_t *paths = create_paths(argc, argv);

    FILE *file_a = open_file(paths->path_a, "r");
    FILE *file_b = open_file(paths->path_b, "r");
    FILE *file_c_per_matrix = open_file(paths->path_c_per_matrix, "w");
    FILE *file_c_per_row = open_file(paths->path_c_per_row, "w");
    FILE *file_c_per_element = open_file(paths->path_c_per_element, "w");

    matrix_t *A = read_matrix(file_a);
    matrix_t *B = read_matrix(file_b);
    fclose(file_a);
    fclose(file_b);

    if (A->cols != B->rows) {
        fprintf(stderr, "matrix_multiplication: A.col and B.row are not equal\n");
        exit(EXIT_FAILURE);
    }
    matrix_t *C = allocate_matrix(A->rows, B->cols);

    // Per matrix
    struct timeval per_matrix_start, per_matrix_end;
    gettimeofday(&per_matrix_start, NULL);
    pthread_t per_matrix;
    threaded_matrix_mult_args_t args = {A, B, C, 0, 0, A->rows, B->cols};
    pthread_create(&per_matrix, NULL, threaded_matrx_mult, &args);
    pthread_join(per_matrix, NULL);
    gettimeofday(&per_matrix_end, NULL);
    write_matrix(file_c_per_matrix, C);
    fclose(file_c_per_matrix);

    // Per row
    clear_matrix(C);
    struct timeval per_row_start, per_row_end;
    gettimeofday(&per_row_start, NULL);
    pthread_t per_row[A->rows];
    threaded_matrix_mult_args_t per_row_args[A->rows];
    for (int i = 0; i < A->rows; ++i) {
        per_row_args[i] = (threaded_matrix_mult_args_t){A, B, C, i, 0, 1, B->cols};
        pthread_create(per_row + i, NULL, threaded_matrx_mult, &per_row_args[i]);
    }
    for (int i = 0; i < A->rows; ++i) {
        pthread_join(per_row[i], NULL);
    }
    gettimeofday(&per_row_end, NULL);
    write_matrix(file_c_per_row, C);
    fclose(file_c_per_row);

    // Per element
    clear_matrix(C);
    struct timeval per_element_start, per_element_end;
    gettimeofday(&per_element_start, NULL);
    pthread_t per_element[A->rows][B->cols];
    threaded_matrix_mult_args_t per_element_args[A->rows][B->cols];
    for (int i = 0; i < A->rows; ++i) {
        for (int j = 0; j < B->cols; ++j) {
            per_element_args[i][j] = (threaded_matrix_mult_args_t){A, B, C, i, j, 1, 1};
            pthread_create(per_element[i] + j, NULL, threaded_matrx_mult, &per_element_args[i][j]);
        }
    }
    for (int i = 0; i < A->rows; ++i) {
        for (int j = 0; j < B->cols; ++j) {
            pthread_join(per_element[i][j], NULL);
        }
    }
    gettimeofday(&per_element_end, NULL);
    write_matrix(file_c_per_element, C);
    fclose(file_c_per_element);

    printf("Per matrix:\n");
    printf("Time taken (us): %li\n", per_matrix_end.tv_usec - per_matrix_start.tv_usec);
    printf("Threads created: 1\n\n");

    printf("Per row:\n");
    printf("Time taken (us): %li\n", per_row_end.tv_usec - per_row_start.tv_usec);
    printf("Threads created: %d\n\n", A->rows);

    printf("Per element:\n");
    printf("Time taken (us): %li\n", per_element_end.tv_usec - per_element_start.tv_usec);
    printf("Threads created: %d\n\n", A->rows * B->cols);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);

    free_paths(paths);

    return 0;
}