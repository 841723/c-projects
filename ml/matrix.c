#include <stdlib.h>
#include <stdio.h>


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

matrix *mat_create(int rows, int cols) {
    matrix *mat = malloc(sizeof(matrix));

    if (mat == NULL) {
        return NULL;
    }

    mat->cols = cols;
    mat->rows = rows;
    mat->data = malloc(rows * cols * sizeof(float));

    if (mat->data == NULL) {
        free(mat);
        return NULL;
    }

    return mat;
}

matrix *mat_load(const char *pathname, int rows, int cols) {
    FILE *f = fopen(pathname, "rb");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    long to_read = size > rows * cols * sizeof(float) ? rows * cols * sizeof(float) : size;

    matrix *mat = mat_create(rows, cols);
    fread(mat->data, 1, to_read, f);

    fclose(f);
    return mat;
}

