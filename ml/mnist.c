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

void mnist_get_data(
    matrix **train_images,
    matrix **test_images,
    matrix **train_labels,
    matrix **test_labels
) {
    matrix *train_labels_read = mat_load("./dfs/train_labels.mat", 60000, 1);
    matrix *test_labels_read = mat_load("./dfs/test_labels.mat", 10000, 1);
    
    *train_images = mat_load("./dfs/train_images.mat", 60000, IMAGE_SIZE * IMAGE_SIZE);
    *test_images = mat_load("./dfs/test_images.mat", 10000, IMAGE_SIZE * IMAGE_SIZE);

    if (train_labels_read == NULL || test_labels_read == NULL) return;
    
    *train_labels = mat_create(train_labels_read->rows, 10);
    *test_labels = mat_create(test_labels_read->rows, 10);
    for (int i = 0; i < (*train_images)->rows; i++) {
        for (int j = 0; j < 10; j++) {
            (*train_labels)->data[i * 10 + j] = 0.0f;
        }
        int num = train_labels_read->data[i];
        if (0 <= num && num <= 9) (*train_labels)->data[i * 10 + num] = 1.0f;
    }

    for (int i = 0; i < (*test_images)->rows; i++) {
        for (int j = 0; j < 10; j++) {
            (*test_labels)->data[i * 10 + j] = 0.0f;
        }
        int num = test_labels_read->data[i];
        if (0 <= num && num <= 9) (*test_labels)->data[i * 10 + num] = 1.0f;
    }

    mat_free(train_labels_read);
    mat_free(test_labels_read);
}

void mnist_display_image(float *image) {
    for (int i = 0; i < IMAGE_SIZE; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            int val = image[i * IMAGE_SIZE + j] * 23 + 232;
            printf("\033[48;5;%dm  ", val);
        }
        printf("\033[0m\n");
    }
}
