#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"
#include "mnist.h"
#include "neural_network.h"
#include "random.h"
#include "common.h"

void compare_results(matrix *results, matrix *labels) {
    // mat_print(labels);
    // mat_print(results);

    if (results == NULL || labels == NULL) return;
    if (results->rows != labels->rows || results->cols != labels->cols) return;

    float max_in_row_val;
    int idx, label, max_in_row_idx;
    int correct = 0, errors = 0;
    matrix *mat_error = mat_create(1, 10);
    mat_fill(mat_error, 0);
    matrix *mat_nums = mat_create(1, 10);
    mat_fill(mat_nums, 0);

    for (int i = 0; i < results->rows; i++) {
        max_in_row_idx = -1;
        max_in_row_val = 0.0f;
        label = -1;

        for (int j = 0; j < results->cols; j++) {
            idx = i*results->cols+j;
        
            if (labels->data[idx] == 1) {
                label = j;
            }
            if (max_in_row_val < results->data[idx]) {
                max_in_row_idx = j;
                max_in_row_val = results->data[idx];
            }
        }

        if (max_in_row_idx == -1 || label == -1) {
            printf("an error has occurred\n");
        }
        if (label == max_in_row_idx) {
            correct++;
            mat_nums->data[label]++;
        } 
        else {
            errors++;
            mat_nums->data[label]++;
            mat_error->data[label]++;
        }
    }

    float precision = ((float)(correct)/results->rows)*100;
    printf("\n");
    printf("Correct images %d\n", correct);
    printf("Error images %d\n", errors);
    printf("Precision by image matrix:\n");
    for (int i = 0; i < mat_error->cols; i++) {
        printf("%d: %.2f%%\n", i, ((mat_nums->data[i]-mat_error->data[i])/mat_nums->data[i])*100);
    }
    printf("\n");
    printf("Precision has been %3.2f%%\n", precision);
    
}

int main() {
    matrix *train_images, *test_images, *train_labels, *test_labels;
    mnist_get_data(
        &train_images,
        &test_images,
        &train_labels,
        &test_labels
    );

    // for (int i = 0; i < 1; i++) {
    //     mnist_display_image(train_images->data + i * IMAGE_SIZE * IMAGE_SIZE);

    //     for (int j = 0; j < 10; j++) {
    //         printf("  %d : %.2f\n", j, train_labels->data[(i * 10 + j)]);
    //     }
    //     printf("\n");
    // }

    layer *l = nn_create_layer();
    neural_network *nn = nn_create_neural_network();

    if (nn_add_layer_to_neural_network(nn, l) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    if (nn_train_neural_network(nn, train_images, train_labels) == FALSE) {
        perror("Error training \n");
        return 1;
    }

    matrix *res = nn_train_predict(nn, test_images);
    if (res == NULL) {
        perror("Error training \n");
        return 1;
    }

    compare_results(res, test_labels);


    nn_free_layer(l);
    nn_free_neural_network(nn);

    mat_free(res);
    mat_free(train_images);
    mat_free(test_images);
    mat_free(train_labels);
    mat_free(test_labels);

    return 0;
}
