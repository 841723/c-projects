#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    int rows, cols;
    float *data;
} matrix;

matrix *mat_create(int rows, int cols);

matrix *mat_load(const char *pathname, int rows, int cols);
#endif