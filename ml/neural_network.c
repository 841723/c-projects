#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "random.h"
#include "mnist.h"
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

float _nn_loss(float a, float y) {
    float base = a - y;
    return base * base;
}

void _nn_free_layer(layer *l) {
    if(l == NULL) return;
    mat_free(l->w);
    mat_free(l->b);
    free(l);
}


/**********************************************
   Exported functions
***********************************************/

layer* nn_create_layer(int n_cells) {
    layer *l = malloc(sizeof(struct layer));

    l->n_cells = n_cells;
    matrix *mat_b = mat_create(1, l->n_cells);
    mat_random(mat_b, -1 ,1);
    l->b = mat_b;
    
    matrix *mat_w = mat_create(IMAGE_SIZE*IMAGE_SIZE, l->n_cells);
    mat_random(mat_w, -0.5 ,0.5);
    l->w = mat_w; 

    return l;
}

neural_network* nn_create_neural_network() {
    neural_network *nn = malloc(sizeof(struct neural_network));

    nn->n_layers = 0;
    nn->epochs = 30;
    nn->lr = 0.1f;

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

    if (train_data->rows != train_labels->rows) return FALSE;
    int status;
    matrix *mat_z = mat_create(1,nn->layers[0]->n_cells);
    matrix *mat_a = mat_create(1,nn->layers[0]->n_cells);
    matrix *mat_loss = mat_create(1,nn->layers[0]->n_cells);
    matrix *mat_data_i;
    float total_loss = 0.0f;

    for (int epoch = 0; epoch < nn->epochs; epoch++) {
        total_loss = 0.0f;
        for (int i = 0; i < train_data->rows; i++) {
            mat_data_i = mat_copy_soft(train_data, i, 1);
            status = mat_mul(mat_z, mat_data_i, nn->layers[0]->w);

            if (!status) {
                printf("error\n");
            }
            for (int j = 0; j < mat_z->cols; j++) {
                mat_a->data[j] = _nn_sigmoid(mat_z->data[j] + nn->layers[0]->b->data[j]);
                mat_loss->data[j] = _nn_loss(mat_a->data[j], train_labels->data[i*train_labels->cols+j]);
                for (int k = 0; k < nn->layers[0]->w->rows; k++) {
                    nn->layers[0]->w->data[k*nn->layers[0]->w->cols+j] -= (
                            nn->lr * 
                            mat_data_i->data[k] *
                            2 * (mat_a->data[j] - train_labels->data[i*train_labels->cols+j]) *
                            mat_a->data[j] * (1 - mat_a->data[j])
                        );
                }
            }
            total_loss += mat_sum_values(mat_loss);
            
        }
        printf("EPOCH %2d avg_loss : %3.2f\n", epoch, total_loss / train_data->rows);
    }
    mat_free(mat_z);
    mat_free(mat_a);
    mat_free(mat_loss);

    return TRUE;
}

matrix *nn_train_predict(neural_network *nn, matrix *test_data) {
    (void)(nn);
    if (test_data == NULL) return NULL;

    matrix *res = mat_create(test_data->rows, 10);
    // mat_random(res, 0 ,1);
    if (!mat_mul(res, test_data, nn->layers[0]->w)) {
        perror("error in nn_train_predict mat_mul\n");
        return NULL;
    }
    
    // if (!mat_add(res, res, nn->layers[0]->b)) {
    //     perror("error in nn_train_predict mat_add\n");
    //     return NULL;
    // }

    return res;
}

void nn_free_neural_network(neural_network *nn) {
    if(nn == NULL) return;

    for (int i = 0; i < nn->n_layers; i++) {
        _nn_free_layer(nn->layers[i]);
    }
}