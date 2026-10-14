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
    COMPONENT_TYPE ct;
    COMPONENT_FLAG cf;

    matrix *val;
    
    component inputs[COMPONENT_MAX_INPUTS];

} component;

typedef struct {
    int n_cells;
    OUTPUT_FUNCTION of;

} layer;


typedef struct {
    int n_components, n_vars, epochs, n_layers;
    float lr;
    
    layer **layers;
    component **components;

} neural_network;


layer *_nn_create_layer(int n_cells, OUTPUT_FUNCTION of);

int _nn_add_component_to_nn(neural_network *nn, component *c);

int _nn_component_var(neural_network *nn, int rows, int cols);
int _nn_component_input(neural_network *nn, int rows, int cols);
int _nn_component_output(neural_network *nn, int rows, int cols);

int _nn_component_add(neural_network *nn, component *a, component *b);
int _nn_component_sub(neural_network *nn, component *a, component *b);
int _nn_component_mul(neural_network *nn, component *a, component *b);

int _nn_component_relu(neural_network *nn, component *a);
int _nn_component_softmax(neural_network *nn, component *a);

layer *_nn_create_layer(int n_cells, OUTPUT_FUNCTION of);

neural_network *nn_create_neural_network(int epochs, float lr);
int nn_add_layer_to_neural_network(neural_network *nn, int n_cells, OUTPUT_FUNCTION of);

int nn_train_neural_network(neural_network *nn, matrix *train_data, matrix *train_labels);
matrix *nn_train_predict(neural_network *nn, matrix *test_data);

void nn_free_neural_network(neural_network *nn);

#endif