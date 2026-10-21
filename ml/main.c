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
    neural_network *nn = nn_create_neural_network(10, 0.01f);

    printf("nn created \n");

    if (nn_add_layer_to_neural_network(nn, 10, OF_RELU) == FALSE) {
        perror("Error adding layer to neural network\n");
        return 1;
    }

    for (int i = 0; i < nn->n_components; i++) {
        component *cur = nn->components[i];
        printf("type: %d\n", cur->ct);
    }
    memcpy(
        nn->components[0]->val->data, 
        train_images->data, 
        nn->components[0]->val->cols*sizeof(float)
    );

    _nn_forward(nn);
    
    mnist_display_image(nn->components[0]->val->data);
    mnist_get_pred(nn->components[nn->n_components-1]->val);
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
