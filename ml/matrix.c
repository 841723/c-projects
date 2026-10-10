#include <stdlib.h>
#include <stdio.h>


#include "common.h"
#include "matrix.h"
#include "mnist.h"

/**********************************************
 Global Variables
**********************************************/



/**********************************************
  Structs
**********************************************/



/**********************************************
 Helper functions
**********************************************/



/**********************************************
   Exported functions
***********************************************/

void mat_free(matrix *mat) {
    if (mat != NULL) free(mat->data);
    free(mat);
}

matrix *mat_create(int rows, int cols) {
    matrix *mat = malloc(sizeof(matrix));

    if (mat == NULL) {
        return NULL;
    }

    mat->cols = cols;
    mat->rows = rows;
    mat->data = malloc(rows * cols * sizeof(float));

    if (mat->data == NULL) {
        mat_free(mat);
        return NULL;
    }

    return mat;
}

matrix *mat_load(const char *pathname, int rows, int cols) {
    FILE *f = fopen(pathname, "rb");
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    unsigned long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    long to_read = min(filesize, rows * cols * sizeof(float));

    matrix *mat = mat_create(rows, cols);
    if (mat == NULL) {
        fclose(f);
        return NULL;
    }
    long read = fread(mat->data, 1, to_read, f);
    if (to_read != read) {
        printf("Error reading \"%s\"\n", pathname);
        mat_free(mat);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return mat;
}

void mat_fill(matrix *mat, float value) {
    if (mat == NULL || mat->data== NULL) return;

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i*mat->cols+j] = value;
        }
    }
}

int mat_add(matrix *res, matrix *a, matrix *b) {
    if (res == NULL || a == NULL || b == NULL) return FALSE;
    if (a->cols != b->cols || a->cols != res->cols || a->rows != b->rows || a->rows != res->rows) {
        return FALSE;
    }

    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            res->data[i*res->cols+j] = a->data[i*res->cols+j] + b->data[i*res->cols+j];
        }
    }
    return TRUE;
}


int mat_sub(matrix *res, matrix *a, matrix *b) {
    if (res == NULL || a == NULL || b == NULL) return FALSE;
    if (a->cols != b->cols || a->cols != res->cols || a->rows != b->rows || a->rows != res->rows) {
        return FALSE;
    }

    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            res->data[i*res->cols+j] = a->data[i*res->cols+j] - b->data[i*res->cols+j];
        }
    }
    return TRUE;
}

int mat_mul(matrix *res, matrix *a, matrix *b) {
    if (res == NULL || a == NULL || b == NULL) return FALSE;
    if (a->cols != b->rows || a->rows != res->rows || b->cols != res->cols) {
        return FALSE;
    }

    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            res->data[i*res->cols+j] = 0.0f;
            for (int k = 0; k < a->cols; k++) {
                res->data[i*res->cols+j] += 
                    a->data[i*a->cols+k] * b->data[k*b->cols+j];
            }
        }
    }
    return TRUE;
}


void mat_print(matrix *mat) {
    if (mat == NULL) return;
    
    int max_rows = 4, max_cols = 15;

    int rows = min(max_rows, mat->rows);
    int cols = min(max_cols, mat->cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f ", mat->data[i*mat->cols+j]);
        }
        printf("\n");
    }
}
