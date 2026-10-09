#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int rows, cols;
    float *data;
} matrix;


void mat_free(matrix *mat);
matrix *mat_create(int rows, int cols);
matrix *mat_load(const char *pathname, int rows, int cols);
void mat_fill(matrix *mat, float value);

int mat_add(matrix *res, matrix *a, matrix *b);
int mat_sub(matrix *res, matrix *a, matrix *b);
int mat_mul(matrix *res, matrix *a, matrix *b);

void mat_print(matrix *mat);

#endif