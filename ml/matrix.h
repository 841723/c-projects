#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int rows, cols;
    float *data;
} matrix;


void mat_free(matrix *mat);
matrix *mat_create(int rows, int cols);
matrix *mat_create_not_allocate(int rows, int cols);
matrix *mat_copy_rows(matrix *mat, int from_row, int end_row);
matrix *mat_copy_soft(matrix *mat, int first_row_to_copy, int num_rows_to_copy);
matrix *mat_load(const char *pathname, int rows, int cols);
void mat_fill(matrix *mat, float value);
void mat_random(matrix *mat, float min, float max);

int mat_add(matrix *res, matrix *a, matrix *b);
int mat_add_constant(matrix *res, matrix *a, float b);
int mat_sub(matrix *res, matrix *a, matrix *b);
int mat_mul(matrix *res, matrix *a, matrix *b);
float mat_sum_values(matrix *mat);

void mat_print(matrix *mat);
void mat_print_shape(matrix *mat);

#endif