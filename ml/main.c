#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"
#include "mnist.h"
#include "neural_network.h"
#include "common.h"

int main() {
    matrix *train_images, *test_images, *train_labels, *test_labels;
    mnist_get_data(
        &train_images,
        &test_images,
        &train_labels,
        &test_labels
    );

    for (int i = 0; i < 0; i++) {
        mnist_display_image(train_images->data + i * IMAGE_SIZE * IMAGE_SIZE);

        for (int j = 0; j < 10; j++) {
            printf("  %d : %.2f\n", j, train_labels->data[(i * 10 + j)]);
        }
        printf("\n");
    }

    layer *l = nn_create_layer();
    neural_network *nn = nn_create_neural_network();

    if (nn_add_layer_to_neural_network(nn, l) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    if (nn_train_neural_network(nn) == FALSE) {
        perror("Error training \n");
        return 1;
    }

    if (nn_train_predict(nn) == FALSE) {
        perror("Error training \n");
        return 1;
    }

    return 0;
}
