#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>


#include "results.h"
#include "matrix.h"
#include "mnist.h"
#include "neural_network.h"
#include "random.h"
#include "common.h"

int main() {
    matrix *train_images, *test_images, *train_labels, *test_labels;
    mnist_get_data(
        &train_images,
        &test_images,
        &train_labels,
        &test_labels
    );
    printf("files loaded successfully\n");

    struct timeval inicio, fin;

    neural_network *nn = nn_create_neural_network(10, 0.001f, 2000, IMAGE_SIZE*IMAGE_SIZE, 10);

    printf("nn created \n");

    if (nn_add_layer_to_neural_network(nn, 128, OF_RELU) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    if (nn_add_layer_to_neural_network(nn, 128, OF_RELU) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    if (nn_add_layer_to_neural_network(nn, 10, OF_SOFTMAX) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    for (int i = 0; i < nn->n_components; i++) {
        component *cur = nn->components[i];
        printf("name: %s\n", cur->name);
    }

    gettimeofday(&inicio, NULL);
    nn_train_neural_network(nn, train_images, train_labels);
    gettimeofday(&fin, NULL);

    matrix *res = mat_create(test_images->rows, nn->output_size);
    if (res == NULL) {
        fprintf(stderr, "Error allocating prediction results\n");
        return 1;
    }
    if (!nn_predict(nn, test_images, res)) {
        perror("Error predicting\n");
        return 1;
    }

    res_file *rf = rf_create();

    if (rf == NULL ||
        !rf_save_model_res(rf, res, test_labels) ||
        !rf_save_nn(rf, nn) ||
        !rf_save_time(rf, inicio, fin) ||
        !rf_log_res(rf)
    ) {
        fprintf(stderr, "Error saving model statistics\n");
        rf_free(rf);
        return 1;
    }


    nn_free_neural_network(nn);

    rf_free(rf);
    mat_free(res);
    mat_free(train_images);
    mat_free(test_images);
    mat_free(train_labels);
    mat_free(test_labels);

    return 0;
}
