#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include "random.h"
#include "neural_network.h"
#include "common.h"

/**********************************************
 Global Variables
**********************************************/



/**********************************************
  Structs
**********************************************/



/**********************************************
 Helper functions
**********************************************/
float _nn_sigmoid(float x) {
    return 1/(1+exp(-x));
}


/**********************************************
   Exported functions
***********************************************/

layer* nn_create_layer() {
    layer *l = malloc(sizeof(struct layer));

    l-> n_cells = 0;

    return l;
}

neural_network* nn_create_neural_network() {
    neural_network *nn = malloc(sizeof(struct neural_network));

    nn->n_layers = 0;

    return nn;
}

int nn_add_layer_to_neural_network(neural_network *nn, layer *l) {
    if (nn->n_layers >= MAX_N_LAYERS) {
        return FALSE;
    }
    nn->layers[nn->n_layers++] = l;

    return TRUE;
}

int nn_train_neural_network(neural_network *nn, matrix *train_data, matrix *train_labels) {
    (void)(nn);
    (void)(train_data);
    (void)(train_labels);
    return TRUE;
}

matrix *nn_train_predict(neural_network *nn, matrix *test_data) {
    (void)(nn);
    if (test_data == NULL) return NULL;

    matrix *res = mat_create(test_data->rows, 10);
    for (int i = 0; i < res->rows; i++) {
        for (int j = 0; j < res->cols; j++) {
            res->data[i*res->cols+j] = get_random(0,1);
        }
    }

    return res;
}

void nn_free_layer(layer *l) {
    if(l != NULL) free(l);
}
void nn_free_neural_network(neural_network *nn) {
    if (nn != NULL) free(nn);
}