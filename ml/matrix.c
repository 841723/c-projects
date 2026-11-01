#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
void _mat_mul_nn(matrix* res, matrix* a, matrix* b) {
    for (int i = 0; i < res->rows; i++) {
        for (int k = 0; k < a->cols; k++) {
            for (int j = 0; j < res->cols; j++) {
                res->data[j + i * res->cols] += 
                    a->data[k + i * a->cols] * 
                    b->data[j + k * b->cols];
            }
        }
    }
}

void _mat_mul_nt(matrix* res, matrix* a, matrix* b) {
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            for (int k = 0; k < a->cols; k++) {
                res->data[j + i * res->cols] += 
                    a->data[k + i * a->cols] * 
                    b->data[k + j * b->cols];
            }
        }
    }
}

void _mat_mul_tn(matrix* res, matrix* a, matrix* b) {
    for (int k = 0; k < a->rows; k++) {
        for (int i = 0; i < res->rows; i++) {
            for (int j = 0; j < res->cols; j++) {
                res->data[j + i * res->cols] += 
                    a->data[i + k * a->cols] * 
                    b->data[j + k * b->cols];
            }
        }
    }
}

void _mat_mul_tt(matrix* res, matrix* a, matrix* b) {
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            for (int k = 0; k < a->rows; k++) {
                res->data[j + i * res->cols] += 
                    a->data[i + k * a->cols] * 
                    b->data[k + j * b->cols];
            }
        }
    }
}


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

void mat_clear(matrix *mat) {
    mat_fill(mat, 0.0f);
}

void mat_scale(matrix *mat, float value) {
    if (mat == NULL) return;

    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            mat->data[i*mat->cols+j] *= value;
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

int mat_add_add_grad(matrix *a_grad, matrix *a_val, matrix *cur_grad) {
    if (a_grad == NULL || a_grad->data == NULL) return FALSE;
    if (a_val == NULL || a_val->data == NULL) return FALSE;
    if (cur_grad == NULL || cur_grad->data == NULL) return FALSE;
    if (a_val->rows != a_grad->rows || a_val->cols != a_grad->cols) return FALSE;
    if (a_val->rows != cur_grad->rows || a_val->cols != cur_grad->cols) return FALSE;

    int idx;

    for (int i = 0; i < a_val->rows; i++) {
        for (int j = 0; j < a_val->cols; j++) {
            idx = i*a_val->cols+j;
            
            a_grad->data[idx] += cur_grad->data[idx];
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

int mat_sub_add_grad(matrix *a_grad, matrix *a_val, matrix *cur_grad) {
    if (a_grad == NULL || a_grad->data == NULL) return FALSE;
    if (a_val == NULL || a_val->data == NULL) return FALSE;
    if (cur_grad == NULL || cur_grad->data == NULL) return FALSE;
    if (a_val->rows != a_grad->rows || a_val->cols != a_grad->cols) return FALSE;
    if (a_val->rows != cur_grad->rows || a_val->cols != cur_grad->cols) return FALSE;

    int idx;

    for (int i = 0; i < a_val->rows; i++) {
        for (int j = 0; j < a_val->cols; j++) {
            idx = i*a_val->cols+j;
            
            a_grad->data[idx] -= cur_grad->data[idx];
        }
    }

    return TRUE;
}

int mat_mul(matrix *res, matrix *a, matrix *b, int transpose_a, int transpose_b, int zero_out) {
    if (res == NULL || a == NULL || b == NULL) return FALSE;

    int a_rows = transpose_a ? a->cols : a->rows;
    int a_cols = transpose_a ? a->rows : a->cols;
    int b_rows = transpose_b ? b->cols : b->rows;
    int b_cols = transpose_b ? b->rows : b->cols;

    if (a_cols != b_rows || a_rows != res->rows || b_cols != res->cols) {
        return FALSE;
    }

    if (zero_out) mat_clear(res);

    if (!transpose_a && !transpose_b) _mat_mul_nn(res, a, b);
    if (!transpose_a &&  transpose_b) _mat_mul_nt(res, a, b);
    if ( transpose_a && !transpose_b) _mat_mul_tn(res, a, b);
    if ( transpose_a &&  transpose_b) _mat_mul_tt(res, a, b);

    return TRUE;
}

int mat_relu(matrix *res, matrix *a) {
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

int mat_relu_add_grad(matrix *a_grad, matrix *a_val, matrix *cur_grad) {
    if (a_grad == NULL || a_grad->data == NULL) return FALSE;
    if (a_val == NULL || a_val->data == NULL) return FALSE;
    if (cur_grad == NULL || cur_grad->data == NULL) return FALSE;
    if (a_val->rows != a_grad->rows || a_val->cols != a_grad->cols) return FALSE;
    if (a_val->rows != cur_grad->rows || a_val->cols != cur_grad->cols) return FALSE;

    int idx;

    for (int i = 0; i < a_val->rows; i++) {
        for (int j = 0; j < a_val->cols; j++) {
            idx = i*a_val->cols+j;
            
            a_grad->data[idx] += a_val->data[idx] > 0 
                ? (cur_grad->data[idx])
                : (0);
        } 
    }

    return TRUE;
}


int mat_softmax(matrix *res, matrix *a) {
    if (res == NULL || res->data == NULL || a == NULL || a->data == NULL) {
        return FALSE;
    }
    if (res->rows != a->rows || res->cols != a->cols || a->cols <= 0) {
        return FALSE;
    }

    for (int i = 0; i < a->rows; i++) {
        int row_start = i * a->cols;
        float max_value = a->data[row_start];

        for (int j = 0; j < a->cols; j++) {
            float value = a->data[row_start + j];
            if (value > max_value) {
                max_value = value;
            }
        }

        float sum = 0.0f;
        for (int j = 0; j < a->cols; j++) {
            int idx = row_start + j;
            res->data[idx] = expf(a->data[idx] - max_value);
            sum += res->data[idx];
        }

        for (int j = 0; j < a->cols; j++) {
            res->data[row_start + j] /= sum;
        }
    }

    return TRUE;
}

int mat_softmax_add_grad(matrix *a_grad, matrix *a_val, matrix *cur_grad) {
    if (a_grad == NULL || a_grad->data == NULL) return FALSE;
    if (a_val == NULL || a_val->data == NULL) return FALSE;
    if (cur_grad == NULL || cur_grad->data == NULL) return FALSE;
    if (a_grad->rows != a_val->rows || a_grad->cols != a_val->cols) return FALSE;
    if (cur_grad->rows != a_val->rows || cur_grad->cols != a_val->cols) return FALSE;
    if (a_val->cols <= 0) return FALSE;

    for (int i = 0; i < a_val->rows; i++) {
        int row_start = i * a_val->cols;
        float dot = 0.0f;

        for (int j = 0; j < a_val->cols; j++) {
            int idx = row_start + j;
            dot += cur_grad->data[idx] * a_val->data[idx];
        }

        for (int j = 0; j < a_val->cols; j++) {
            int idx = row_start + j;
            a_grad->data[idx] += a_val->data[idx] * (cur_grad->data[idx] - dot);
        }
    }

    return TRUE;
}

int mat_cross_entropy(matrix *res, matrix *p, matrix *q) {
    if (res == NULL || res->data == NULL) return FALSE;
    if (p == NULL || p->data == NULL) return FALSE;
    if (q == NULL || q->data == NULL) return FALSE;
    if (res->rows != p->rows || res->cols != p->cols) return FALSE;
    if (q->rows != p->rows || q->cols != p->cols) return FALSE;

    const float epsilon = 1e-7f;
    int size = p->rows * p->cols;
    for (int idx = 0; idx < size; idx++) {
        float probability = MAX(p->data[idx], epsilon);
        res->data[idx] = -q->data[idx] * logf(probability);
    }

    return TRUE;
}

int mat_cross_entropy_add_grad(
    matrix *p_grad,
    matrix *p,
    matrix *q,
    matrix *cur_grad
) {
    if (p_grad == NULL || p_grad->data == NULL) return FALSE;
    if (p == NULL || p->data == NULL) return FALSE;
    if (q == NULL || q->data == NULL) return FALSE;
    if (cur_grad == NULL || cur_grad->data == NULL) return FALSE;
    if (p_grad->rows != p->rows || p_grad->cols != p->cols) return FALSE;
    if (q->rows != p->rows || q->cols != p->cols) return FALSE;
    if (cur_grad->rows != p->rows || cur_grad->cols != p->cols) return FALSE;

    const float epsilon = 1e-7f;
    int size = p->rows * p->cols;
    for (int idx = 0; idx < size; idx++) {
        float probability = MAX(p->data[idx], epsilon);
        p_grad->data[idx] +=
            -cur_grad->data[idx] * q->data[idx] / probability;
    }

    return TRUE;
}

int mat_cost(matrix *res, matrix *a, matrix *b) {
    if (res == NULL || res->data == NULL) return FALSE;
    if (a == NULL || a->data == NULL) return FALSE;
    if (b == NULL || b->data == NULL) return FALSE;
    if (a->rows != res->rows || a->cols != res->cols) return FALSE;
    if (a->rows != b->rows || a->cols != b->cols) return FALSE;

    float base;
    int idx;

    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < a->cols; j++) {
            idx = i*a->cols+j;
            base = a->data[idx] - b->data[idx];
            res->data[idx] = (base * base) / 2;
        } 
    }
    return TRUE;
}

int mat_cost_add_grad(matrix *a_grad, matrix *a_val, matrix *b_val, matrix *cur_grad) {
    if (a_grad == NULL || a_grad->data == NULL) return FALSE;
    if (a_val == NULL || a_val->data == NULL) return FALSE;
    if (b_val == NULL || b_val->data == NULL) return FALSE;
    if (cur_grad == NULL || cur_grad->data == NULL) return FALSE;
    if (a_val->rows != a_grad->rows || a_val->cols != a_grad->cols) return FALSE;
    if (a_val->rows != b_val->rows || a_val->cols != b_val->cols) return FALSE;
    if (a_val->rows != cur_grad->rows || a_val->cols != cur_grad->cols) return FALSE;
    int idx = 0;
    
    for (int i = 0; i < a_val->rows; i++) {
        for (int j = 0; j < a_val->cols; j++) {
            idx = i*a_val->cols+j;

            a_grad->data[idx] += cur_grad->data[idx] * (a_val->data[idx] - b_val->data[idx]);
        } 
    }
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
