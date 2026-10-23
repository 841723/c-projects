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

    #define TEST_2

    #ifdef TEST_1
    for (int i = 0; i < 1; i++) {
        mnist_display_image(train_images->data + i * IMAGE_SIZE * IMAGE_SIZE);

        for (int j = 0; j < 10; j++) {
            printf("  %d : %.2f\n", j, train_labels->data[(i * 10 + j)]);
        }
        printf("\n");
    }
    #endif

    #ifdef TEST_2
    neural_network *nn = nn_create_neural_network(1, 0.01f, 2000, IMAGE_SIZE*IMAGE_SIZE, 10);

    printf("nn created \n");

    // if (nn_add_layer_to_neural_network(nn, 16, OF_RELU) == FALSE) {
    //     perror("Error adding layer to neural network\n");
    //     return 1;
    // }

    if (nn_add_layer_to_neural_network(nn, 10, OF_RELU) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    for (int i = 0; i < nn->n_components; i++) {
        component *cur = nn->components[i];
        printf("name: %s\n", cur->name);
    }
    
    // gettimeofday(&inicio, NULL);
    nn_train_neural_network(nn, train_images, train_labels);
    // gettimeofday(&fin, NULL);

    int num_test_image = 53;
    matrix *test_image = mat_create(1, IMAGE_SIZE * IMAGE_SIZE);
    memcpy(
        test_image->data,
        test_images->data + (num_test_image * IMAGE_SIZE * IMAGE_SIZE),
        IMAGE_SIZE * IMAGE_SIZE * sizeof(float)
    );
    matrix *res = mat_create(1, nn->output_size);

    if (!nn_predict(nn, test_image, res)) {
        printf("error in pred\n");
        return 1;
    }

    mnist_display_image(test_image->data);
    for (int i = 0; i < res->cols; i++) {
        printf("\t%d : %.2f\n", i, res->data[i]);
    }
    printf("\n");

    #endif


    #ifdef PROD
    neural_network *nn = nn_create_neural_network(10, 0.01f);
    

    if (nn_add_layer_to_neural_network(nn, 10, OF_SOFTMAX) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    gettimeofday(&inicio, NULL);
    if (nn_train_neural_network(nn, train_images, train_labels) == FALSE) {
        perror("Error training \n");
        return 1;
    }
    gettimeofday(&fin, NULL);


    matrix *res = nn_train_predict(nn, test_images);
    if (res == NULL) {
        perror("Error training \n");
        return 1;
    }

    res_file *rf = rf_create();

    rf_save_model_res(rf, res, test_labels);
    rf_save_nn(rf, nn);

    rf_log_res(rf);


    nn_free_neural_network(nn);

    mat_free(res);
    mat_free(train_images);
    mat_free(test_images);
    mat_free(train_labels);
    mat_free(test_labels);
    #endif

    return 0;
}
