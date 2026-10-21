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
float _nn_loss(float a, float y) {
    float base = a - y;
    return base * base;
}

// void _nn_free_layer(layer *l) {
//     if(l == NULL) return;
//     mat_free(l->w);
//     mat_free(l->b);
//     free(l);
// }

layer *_nn_create_layer(int n_cells, OUTPUT_FUNCTION of) {
    // TODO esto creo que no funciona bien
    layer *l = malloc(sizeof(layer));

    l->n_cells = n_cells;
    l->of = of;

    return l;
}

component *_nn_add_component_to_nn(
    neural_network *nn, COMPONENT_TYPE ct, COMPONENT_FLAG cf, 
    int rows, int cols, component* a, component* b
) {
    if (rows < 0 || cols < 0) return NULL;
    // TODO esto creo que no funciona bien
    component *c = malloc(sizeof(component));
    if (c == NULL) return NULL;

    c->ct = ct;
    c->cf = cf;
    c->val = mat_create(rows, cols);
    c->inputs[0] = a;
    c->inputs[1] = b;

    if (c->val == NULL) {
        free(c);
        return NULL;
    }

    if (ct == CT_VAR) {
        nn->n_vars += rows * cols;
    }
    
    // TODO: añadir el componente recien creado al nn->components
    component **tmp_components = realloc(
        nn->components,
        (nn->n_components + 1) * sizeof(nn->components)
    );
    
    if (tmp_components == NULL) {
        return NULL;
    }
    
    nn->components = tmp_components;

    nn->n_components++;
    if (nn->n_components > 2) {
        // keep output as last component
        nn->components[nn->n_components - 1] = nn->components[nn->n_components - 2];
        nn->components[nn->n_components - 2] = c;
    }
    else {
        // add input/output components
        nn->components[nn->n_components - 1] = c;
    }

    return c;
}

component *_nn_component_var(neural_network *nn, int rows, int cols) {

    return _nn_add_component_to_nn(nn, CT_VAR, 0, rows, cols, NULL, NULL);
}

component *_nn_component_input(neural_network *nn, int rows, int cols) {

    return _nn_add_component_to_nn(nn, CT_INPUT, 0, rows, cols, NULL, NULL);
}

component *_nn_component_output(neural_network *nn, int rows, int cols) {

    return _nn_add_component_to_nn(nn, CT_OUTPUT, 0, rows, cols, NULL, NULL);
}

component *_nn_component_add(neural_network *nn, component *a, component *b) {
    if (a->val->rows != b->val->rows || a->val->cols != b->val->cols) return NULL;

    return _nn_add_component_to_nn(nn, CT_ADD, 0, a->val->rows, a->val->cols, a, b);
}

component *_nn_component_sub(neural_network *nn, component *a, component *b) {
    if (a->val->rows != b->val->rows || a->val->cols != b->val->cols) return NULL;

    return _nn_add_component_to_nn(nn, CT_SUB, 0, a->val->rows, a->val->cols, a, b);
}

component *_nn_component_mul(neural_network *nn, component *a, component *b) {
    if (a->val->cols != b->val->rows) return NULL;

    return _nn_add_component_to_nn(nn, CT_MUL, 0, a->val->rows, b->val->cols, a, b);
}

component *_nn_component_relu(neural_network *nn, component *a) {

    return _nn_add_component_to_nn(nn, CT_RELU, 0, 1, a->val->cols, a, NULL);
}

component *_nn_component_softmax(neural_network *nn, component *a) {

    return _nn_add_component_to_nn(nn, CT_SOFTMAX, 0, 1, a->val->cols, a, NULL);
}


int _nn_forward(neural_network *nn) {
    component *cur;
    for (int i = 0; i < nn->n_components; i++) {
        cur = nn->components[i];
        
        switch (cur->ct) {
            case CT_ADD: { mat_add(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;
            case CT_SUB: { mat_sub(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;
            case CT_MUL: { mat_mul(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;
    
            case CT_RELU: { mat_relu(cur->val, cur->inputs[0]->val); } break;
            case CT_SOFTMAX: { mat_softmax(cur->val, cur->inputs[0]->val); } break;

            default:
                continue;
        }
    }
}


/**********************************************
   Exported functions
***********************************************/

neural_network* nn_create_neural_network(int epochs, float lr) {
    neural_network *nn = malloc(sizeof(neural_network));

    nn->epochs = epochs;
    nn->lr = lr;
    nn->n_vars = 0;
    nn->n_components = 0;
    nn->n_layers = 0;

    if (_nn_component_input(nn, 1, IMAGE_SIZE*IMAGE_SIZE) == NULL) {
        // TODO: free nn
        return NULL;
    }

    if (_nn_component_output(nn, 1, 10) == NULL) {
        // TODO: free nn
        return NULL;
    }

    return nn;
}

int nn_add_layer_to_neural_network(neural_network *nn, int n_cells, OUTPUT_FUNCTION of) {

    if (nn->n_components < 2) return FALSE;

    component *an_1 = nn->components[nn->n_components - 2];
    printf("nn_add_layer_to_neural_network\n");

    component *wn = _nn_component_var(nn, n_cells, an_1->val->rows);
    if (wn == NULL) return FALSE;
    mat_random(wn->val, -5, 5);

    component *bn = _nn_component_var(nn, n_cells, an_1->val->cols);
    if (wn == NULL) return FALSE;
    mat_random(bn->val, -5, 5);


    component *azn = _nn_component_mul(nn, wn, an_1);
    if (azn == NULL) return FALSE;
    component *bzn = _nn_component_add(nn, azn, bn);
    if (bzn == NULL) return FALSE;

    component *an = NULL;    
    switch (of) {
        case OF_RELU:
            an = _nn_component_relu(nn, bzn);
            if (an == NULL) return FALSE;
            break;
        case OF_SOFTMAX:
            an = _nn_component_softmax(nn, bzn);
            if (an == NULL) return FALSE;
            break;

        default:
            return FALSE;
    }

    // nn->layers[nn->n_layers++] = _nn_create_layer(n_cells, of);
    // nn->n_layers++

    return TRUE;
}

// int nn_train_neural_network(neural_network *nn, matrix *train_data, matrix *train_labels) {

//     if (train_data->rows != train_labels->rows) return FALSE;
//     int status;
//     matrix *mat_z = mat_create(1,nn->layers[0]->n_cells);
//     matrix *mat_a = mat_create(1,nn->layers[0]->n_cells);
//     matrix *mat_loss = mat_create(1,nn->layers[0]->n_cells);
//     matrix *mat_data_i;
//     float total_loss = 0.0f;

//     for (int epoch = 0; epoch < nn->epochs; epoch++) {
//         total_loss = 0.0f;
//         for (int i = 0; i < train_data->rows; i++) {
//             mat_data_i = mat_copy_soft(train_data, i, 1);
//             status = mat_mul(mat_z, mat_data_i, nn->layers[0]->w);

//             if (!status) {
//                 printf("error\n");
//             }
//             for (int j = 0; j < mat_z->cols; j++) {
//                 mat_a->data[j] = _nn_sigmoid(mat_z->data[j] + nn->layers[0]->b->data[j]);
//                 mat_loss->data[j] = _nn_loss(mat_a->data[j], train_labels->data[i*train_labels->cols+j]);
//                 for (int k = 0; k < nn->layers[0]->w->rows; k++) {
//                     nn->layers[0]->w->data[k*nn->layers[0]->w->cols+j] -= (
//                             nn->lr * 
//                             mat_data_i->data[k] *
//                             2 * (mat_a->data[j] - train_labels->data[i*train_labels->cols+j]) *
//                             mat_a->data[j] * (1 - mat_a->data[j])
//                         );
//                 }
//             }
//             total_loss += mat_sum_values(mat_loss);
            
//         }
//         printf("EPOCH %2d avg_loss : %3.2f\n", epoch, total_loss / train_data->rows);
//     }
//     mat_free(mat_z);
//     mat_free(mat_a);
//     mat_free(mat_loss);

//     return TRUE;
// }

// matrix *nn_train_predict(neural_network *nn, matrix *test_data) {
//     (void)(nn);
//     if (test_data == NULL) return NULL;

//     matrix *res = mat_create(test_data->rows, 10);
//     // mat_random(res, 0 ,1);
//     if (!mat_mul(res, test_data, nn->layers[0]->w)) {
//         perror("error in nn_train_predict mat_mul\n");
//         return NULL;
//     }
    
//     // if (!mat_add(res, res, nn->layers[0]->b)) {
//     //     perror("error in nn_train_predict mat_add\n");
//     //     return NULL;
//     // }

//     return res;
// }

void nn_free_neural_network(neural_network *nn) {
    if(nn == NULL) return;

    // for (int i = 0; i < nn->n_layers; i++) {
    //     _nn_free_layer(nn->layers[i]);
    // }
}