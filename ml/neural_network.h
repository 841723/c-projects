#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include "matrix.h"

#define COMPONENT_MAX_INPUTS 2

typedef enum {
    CF_NULL

} COMPONENT_FLAG;

typedef enum {
    CT_NULL,

    CT_INPUT,
    CT_OUTPUT,
    CT_EXPECTED_OUTPUT,
    CT_COST,

    CT_ADD,
    CT_SUB,
    CT_MUL,
    
    CT_RELU,
    CT_SOFTMAX,

    CT_VAR

} COMPONENT_TYPE;


typedef enum {
    OF_RELU,
    OF_SOFTMAX

} OUTPUT_FUNCTION;


typedef struct component {
    char name[128];
    COMPONENT_TYPE ct;
    COMPONENT_FLAG cf;

    matrix *val;
    matrix *grad;
    
    struct component *inputs[COMPONENT_MAX_INPUTS];

} component;

typedef struct {
    int n_cells;
    OUTPUT_FUNCTION of;

} layer;


typedef struct {
    int n_components, n_vars, epochs, n_layers, batch_size, input_size, output_size;
    float lr;

    int input_idx, output_idx, expected_output_idx, cost_idx;
    
    layer **layers;
    component **components;

} neural_network;

typedef struct model {
    component *input;
    component *output;
    // exp *output;


} model;




// component *_nn_component_var(neural_network *nn, int rows, int cols);
// component *_nn_component_input(neural_network *nn, int rows, int cols);
// component *_nn_component_output(neural_network *nn, int rows, int cols);

// component *_nn_component_add(neural_network *nn, component *a, component *b);
// component *_nn_component_sub(neural_network *nn, component *a, component *b);
// component *_nn_component_mul(neural_network *nn, component *a, component *b);

// component *_nn_component_relu(neural_network *nn, component *a);
// component *_nn_component_softmax(neural_network *nn, component *a);

layer *_nn_create_layer(int n_cells, OUTPUT_FUNCTION of);

neural_network *nn_create_neural_network(int epochs, float lr, int batch_size, int input_size, int output_size);
int nn_add_layer_to_neural_network(neural_network *nn, int n_cells, OUTPUT_FUNCTION of);

int nn_train_neural_network(neural_network *nn, matrix *train_data, matrix *train_labels);
matrix *nn_train_predict(neural_network *nn, matrix *test_data);

int nn_predict(neural_network *nn, matrix *data, matrix *res);

void nn_free_neural_network(neural_network *nn);

#endif