#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_SIZE 28

typedef struct {
    int rows, cols;
    float *data;
} matrix;

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

void print_number(float *image) {
    for (int i = 0; i < IMAGE_SIZE; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            int val = image[i * IMAGE_SIZE + j] * 23 + 232;
            printf("\033[48;5;%dm  ", val);
        }
        printf("\033[0m\n");
    }
}

int main() {
    matrix *train_images = mat_load("./dfs/train_images.mat", 60000, IMAGE_SIZE * IMAGE_SIZE);
    matrix *test_images = mat_load("./dfs/test_images.mat", 10000, IMAGE_SIZE * IMAGE_SIZE);
    matrix *train_labels_read = mat_load("./dfs/train_labels.mat", 60000, 1);
    matrix *test_labels_read = mat_load("./dfs/test_labels.mat", 10000, 1);
    matrix *train_labels = mat_create(train_labels_read->rows, 10);
    matrix *test_labels = mat_create(test_labels_read->rows, 10);
    for (int i = 0; i < train_images->rows; i++) {
        for (int j = 0; j < 10; j++) {
            train_labels->data[i * 10 + j] = 0.0f;
        }
        int num = train_labels_read->data[i];
        train_labels->data[i * 10 + num] = 1.0f;
    }
    free(train_labels_read);
    free(test_labels_read);

    for (int i = 0; i < 10; i++) {
        print_number(train_images->data + i * IMAGE_SIZE * IMAGE_SIZE);

        for (int j = 0; j < 10; j++) {
            printf("  %d : %.2f\n", j, train_labels->data[(i * 10 + j)]);
        }
        printf("\n");
    }

    return 0;
}
