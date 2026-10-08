#include <stddef.h>
#include <stdlib.h>

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
    nn->layers = NULL;

    return nn;
}
int nn_add_layer_to_neural_network(neural_network *nn, layer *l) {
    if (nn->n_layers >= MAX_N_LAYERS) {
        return FALSE;
    }
  
    nn->layers = malloc(sizeof(nn->layers)+sizeof(struct layer));
    nn->layers[nn->n_layers++] = l;

    return TRUE;
}

int nn_train_neural_network(neural_network *nn) {
    return TRUE;
}

int nn_train_predict(neural_network *nn) {
    return TRUE;
}