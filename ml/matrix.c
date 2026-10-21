#include <stdlib.h>
#include <stdio.h>


#include "common.h"
#include "random.h"
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

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i*mat->cols+j] = 0.0f;
        }
    }


    return mat;
}

matrix *mat_create_not_allocate(int rows, int cols) {
    matrix *mat = malloc(sizeof(matrix));

    if (mat == NULL) {
        return NULL;
    }

    mat->cols = cols;
    mat->rows = rows;
    mat->data = NULL;

    return mat;
}

matrix *mat_copy_rows(matrix *mat, int from_row, int end_row) {
    if (mat == NULL) return NULL;
    if (end_row < from_row || from_row < 0 || end_row > mat->rows) return NULL;

    matrix *res = mat_create_not_allocate(end_row - from_row + 1, mat->cols);
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            res->data[i*res->cols+j] = mat->data[(i+from_row)*mat->cols+j];
        }
    }

    return res;
}

matrix *mat_copy_soft(matrix *mat, int first_row_to_copy, int num_rows_to_copy) {
    if (mat == NULL) return NULL;
    if (first_row_to_copy + num_rows_to_copy > mat->rows) return NULL;

    matrix *res = mat_create(num_rows_to_copy, mat->cols);
    res->data = mat->data + first_row_to_copy*mat->cols;
    return res;
}


matrix *mat_load(const char *pathname, int rows, int cols) {
    FILE *f = fopen(pathname, "rb");
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    unsigned long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    long to_read = MIN(filesize, rows * cols * sizeof(float));

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
void mat_random(matrix *mat, float min, float max) {
    if (mat == NULL) return;

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i*mat->cols+j] = get_random(min, max);
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

int mat_add_constant(matrix *res, matrix *a, float b) {
    if (res == NULL || a == NULL) return FALSE;

    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            res->data[i*res->cols+j] = a->data[i*res->cols+j] + b;
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

int mat_relu(matrix *res, matrix *a) {
    // TODO: implementar
    if (res == NULL || res->data == NULL) return FALSE;
    if (a == NULL || a->data == NULL) return FALSE;
    if (a->rows != res->rows || a->cols != res->cols) return FALSE;

    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            res->data[i*a->cols+j] = MAX(a->data[i*a->cols+j], 0.0f);
        } 
    }
    return TRUE;
}

int mat_softmax(matrix *res, matrix *a) {
    // TODO: implementar
    return TRUE;
}


float mat_sum_values(matrix *mat) {
    if (mat == NULL) return -1.0f;
    float total_sum = 0;

    for (int i = 0; i < mat->rows*mat->cols; i++) {
        total_sum += mat->data[i];
    }

    return total_sum;
}

int mat_is_all_zero(matrix *mat) {
    if (mat == NULL) return FALSE;

    int res = TRUE;

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            res &= mat->data[i*mat->cols+j] == 0;
        }
    }
    return res;
}


void mat_print(matrix *mat) {
    if (mat == NULL) return;
    
    int max_rows = 4, max_cols = 15;

    int rows = MIN(max_rows, mat->rows);
    int cols = MIN(max_cols, mat->cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f ", mat->data[i*mat->cols+j]);
        }
        printf("\n");
    }
}

void mat_print_shape(matrix *mat) {
    if (mat == NULL) return;

    printf("rows %d\tcols %d\n", mat->rows, mat->cols);
}
