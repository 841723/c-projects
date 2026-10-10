#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "matrix.h"

#define MAX_N_LAYERS 16

typedef struct layer {
    int n_cells;
} layer ;

typedef struct neural_network {
    int n_layers;
    layer* layers[MAX_N_LAYERS];
} neural_network;

layer* nn_create_layer();
neural_network* nn_create_neural_network();
int nn_add_layer_to_neural_network(neural_network *nn, layer *l);

int nn_train_neural_network(neural_network *nn, matrix *train_data, matrix *train_labels);
matrix *nn_train_predict(neural_network *nn, matrix *test_data);

// float nn_sigmoid(float x);

void nn_free_layer(layer *l);
void nn_free_neural_network(neural_network *nn);

#endif