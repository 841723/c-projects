#include <stddef.h>
#include <stdlib.h>
#include <string.h>
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
    int rows, int cols, component* a, component* b, char *name
) {
    if (rows < 0 || cols < 0) return NULL;
    // TODO esto creo que no funciona bien
    component *c = malloc(sizeof(component));
    if (c == NULL) return NULL;

    strcpy(c->name, name);
    // c->name = name;
    c->ct = ct;
    c->cf = cf;
    c->val = mat_create(rows, cols);
    c->grad = mat_create(rows, cols);
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
    nn->components[nn->n_components++] = c;

    return c;
}

component *_nn_component_var(neural_network *nn, int rows, int cols, char *name) {

    return _nn_add_component_to_nn(nn, CT_VAR, 0, rows, cols, NULL, NULL, name);
}

component *_nn_component_input(neural_network *nn, int rows, int cols, char *name) {

    component *res = _nn_add_component_to_nn(nn, CT_INPUT, 0, rows, cols, NULL, NULL, name);
    nn->input_idx = nn->n_components-1;
    return res;
}

component *_nn_component_cost(neural_network *nn, component *a, component *b, char *name) {

    component *res = _nn_add_component_to_nn(nn, CT_COST, 0, 1, nn->output_size, a, b, name);
    nn->cost_idx = nn->n_components-1;
    return res;
}

component *_nn_component_output(neural_network *nn, component *a, char *name) {

    component *res = _nn_add_component_to_nn(nn, CT_OUTPUT, 0, a->val->rows, a->val->cols, a, NULL, name);
    nn->output_idx = nn->n_components-1;
    return res;
}

component *_nn_component_expected_output(neural_network *nn, char *name) {

    component *res = _nn_add_component_to_nn(nn, CT_EXPECTED_OUTPUT, 0, 1, nn->output_size, NULL, NULL, name);
    nn->expected_output_idx = nn->n_components-1;
    return res;
}

component *_nn_component_add(neural_network *nn, component *a, component *b, char *name) {
    if (a->val->rows != b->val->rows || a->val->cols != b->val->cols) return NULL;

    return _nn_add_component_to_nn(nn, CT_ADD, 0, a->val->rows, a->val->cols, a, b, name);
}

component *_nn_component_sub(neural_network *nn, component *a, component *b, char *name) {
    if (a->val->rows != b->val->rows || a->val->cols != b->val->cols) return NULL;

    return _nn_add_component_to_nn(nn, CT_SUB, 0, a->val->rows, a->val->cols, a, b, name);
}

component *_nn_component_mul(neural_network *nn, component *a, component *b, char *name) {
    if (a->val->cols != b->val->rows) return NULL;

    return _nn_add_component_to_nn(nn, CT_MUL, 0, a->val->rows, b->val->cols, a, b, name);
}

component *_nn_component_relu(neural_network *nn, component *a, char *name) {

    return _nn_add_component_to_nn(nn, CT_RELU, 0, 1, a->val->cols, a, NULL, name);
}

component *_nn_component_softmax(neural_network *nn, component *a, char *name) {

    return _nn_add_component_to_nn(nn, CT_SOFTMAX, 0, 1, a->val->cols, a, NULL, name);
}


int _nn_forward(neural_network *nn) {

    if (nn == NULL) return FALSE;
    
    component *cur;
    int res =  TRUE;
    for (int i = 0; i < nn->n_components; i++) {
        cur = nn->components[i];
        
        switch (cur->ct) {
            case CT_NULL: continue;

            case CT_INPUT: continue; 
            case CT_OUTPUT: { 
                cur->val->rows = cur->inputs[0]->val->rows;
                cur->val->cols = cur->inputs[0]->val->cols;
                memcpy(cur->val->data, cur->inputs[0]->val->data, cur->val->rows * cur->val->cols * sizeof(float)); 
            } break;
            case CT_EXPECTED_OUTPUT: continue; 
            case CT_COST: { res = mat_cost(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;

            case CT_ADD: { res = mat_add(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;
            case CT_SUB: { res = mat_sub(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;
            case CT_MUL: { res = mat_mul(cur->val, cur->inputs[0]->val, cur->inputs[1]->val); } break;
            
            case CT_RELU: { res = mat_relu(cur->val, cur->inputs[0]->val); } break;
            case CT_SOFTMAX: { res = mat_softmax(cur->val, cur->inputs[0]->val); } break;

            case CT_VAR: continue; 
        }

        if (!res) {
            printf("error forward\n");
            return FALSE;
        }
    }
    return TRUE;
}

int _nn_backward(neural_network *nn) {
    if (nn == NULL) return FALSE;
    component *cur;
    int res = TRUE, res2 = TRUE;

    for (int i = 0; i < nn->n_components; i++) {
        cur = nn->components[i];
        if (cur->ct == CT_COST) {
            mat_fill(cur->grad, 1.0f);
        }
        else {
            mat_clear(cur->grad);
        }
    }

    for (int i = nn->n_components - 1; i >= 0; i--) {
        cur = nn->components[i];
        
        switch (cur->ct) {
            case CT_NULL: continue;

            case CT_INPUT: continue;
            case CT_OUTPUT: continue;
            case CT_EXPECTED_OUTPUT: continue;
            case CT_COST: { 
                res = mat_cost_add_grad(cur->inputs[0]->grad, cur->inputs[0]->val, cur->inputs[1]->val, cur->grad);
                // mat_print(cur->inputs[0]->grad);
            } break; 

            case CT_ADD: { 
                res  = mat_add(cur->inputs[0]->grad, cur->inputs[0]->grad, cur->grad);
                res2 = mat_add(cur->inputs[1]->grad, cur->inputs[1]->grad, cur->grad);
            } break; 
            case CT_SUB: { 
                res  = mat_add(cur->inputs[0]->grad, cur->inputs[0]->grad, cur->grad);
                res2 = mat_sub(cur->inputs[1]->grad, cur->inputs[1]->grad, cur->grad);
            } break; 
            case CT_MUL: { 
                // res  = mat_mul(cur->inputs[0]->grad, cur->inputs[0]->val, cur->grad);
                // res2 = mat_sub_add_grad(cur->inputs[1]->grad, cur->inputs[1]->val, cur->grad);
            } break; 
            
            case CT_RELU: { 
                res = mat_relu_add_grad(cur->inputs[0]->grad, cur->inputs[0]->val, cur->grad);
            } break; 
            case CT_SOFTMAX: { continue; } break; 

            case CT_VAR: { 
                mat_scale(cur->grad, nn->lr); 
                mat_sub(cur->val, cur->val, cur->grad);
            } break; 
        }
        if (!res || !res2) {
            printf("error backward\n");
            return  FALSE;   
    
        }
    }


    return TRUE;
}

/**********************************************
   Exported functions
***********************************************/

neural_network* nn_create_neural_network(int epochs, float lr, int batch_size, int input_size, int output_size) {
    neural_network *nn = malloc(sizeof(neural_network));

    nn->epochs = epochs;
    nn->lr = lr;
    nn->batch_size = batch_size;
    nn->input_size = input_size;
    nn->output_size = output_size;

    nn->n_vars = 0;
    nn->n_components = 0;
    nn->n_layers = 0;

    component *input = _nn_component_input(nn, 1, IMAGE_SIZE*IMAGE_SIZE, "x");
    if (input == NULL) {
        // TODO: free nn
        return NULL;
    }

    component *expected_output = _nn_component_expected_output(nn, "y");
    if (expected_output == NULL) {
        // TODO: free nn
        return NULL;
    }

    component *cost = _nn_component_cost(nn, input, expected_output, "c");
    if (cost == NULL) {
        // TODO: free nn
        return NULL;
    }

    component *output = _nn_component_output(nn, input, "^y");
    if (output == NULL) {
        // TODO: free nn
        return NULL;
    }

    return nn;
}

int nn_add_layer_to_neural_network(neural_network *nn, int n_cells, OUTPUT_FUNCTION of) {

    if (nn->n_components < 2) return FALSE;
    printf("nn_add_layer_to_neural_network\n");
    
    component *an_1 = nn->components[nn->n_components - 1]->inputs[0];
    
    // TODO: hacer free de las 3 componentes que borramos
    nn->n_components -= 3;


    component *wn = _nn_component_var(nn, an_1->val->cols, n_cells, "wn");
    if (wn == NULL) return FALSE;
    mat_random(wn->val, -5, 5);

    component *bn = _nn_component_var(nn, 1, n_cells, "bn");
    if (wn == NULL) return FALSE;
    mat_random(bn->val, -5, 5);


    component *azn = _nn_component_mul(nn, an_1, wn, "azn");
    if (azn == NULL) return FALSE;
    component *bzn = _nn_component_add(nn, azn, bn, "bzn");
    if (bzn == NULL) return FALSE;

    component *an = NULL;    
    switch (of) {
        case OF_RELU:
            an = _nn_component_relu(nn, bzn, "r");
            if (an == NULL) return FALSE;
            break;
        case OF_SOFTMAX:
            an = _nn_component_softmax(nn, bzn, "s");
            if (an == NULL) return FALSE;
            break;

        default:
            return FALSE;
    }

    component *expected_output = _nn_component_expected_output(nn, "y");
    if (expected_output == NULL) return FALSE;

    component *cost = _nn_component_cost(nn, an, expected_output, "c");
    if (cost == NULL) return FALSE;

    component *output = _nn_component_output(nn, an, "^y");
    if (output == NULL) return FALSE;

    
    // nn->layers[nn->n_layers++] = _nn_create_layer(n_cells, of);
    // nn->n_layers++

    return TRUE;
}

int nn_train_neural_network(neural_network *nn, matrix *train_data, matrix *train_labels) {

    if (train_data->rows != train_labels->rows) return FALSE;
    int status, max_idx, correct, total;
    float total_cost;
    float *input;
    float *expected_output;
    matrix *output;

    int num_batches = train_data->rows / nn->batch_size;

    for (int epoch = 0; epoch < nn->epochs; epoch++) {
        correct = 0; 
        total = 0;
        for (int batch = 0; batch < num_batches; batch++) {
            total_cost = 0.0f;
            for (int j = batch*nn->batch_size; j < (batch+1)*nn->batch_size-1; j++) {    

                input = train_data->data + j*train_data->cols;
                expected_output = train_labels->data + j*train_labels->cols;

                memcpy(
                    nn->components[nn->input_idx]->val->data, 
                    input, 
                    nn->input_size * sizeof(float)
                );

                memcpy(
                    nn->components[nn->expected_output_idx]->val->data, 
                    expected_output, 
                    nn->output_size * sizeof(float)
                );

                _nn_forward(nn);
                
                _nn_backward(nn);

                output = nn->components[nn->output_idx]->val;
                for (int i = 0; i < output->cols; i++) {
                    printf("%.2f  ", output->data[i]);
                }
                printf("\n");
                correct += mnist_is_correct_pred(output, expected_output);
                total++;
            }
            printf(
                "EPOCH %d/%d\tBATCH %d/%d\t-\tprecision: %.2f%%\n", 
                epoch+1, nn->epochs, batch+1, num_batches, (float)correct/total*100
            );
        }
    }

    // matrix *mat_z = mat_create(1,nn->layers[0]->n_cells);
    // matrix *mat_a = mat_create(1,nn->layers[0]->n_cells);
    // matrix *mat_loss = mat_create(1,nn->layers[0]->n_cells);
    // matrix *mat_data_i;
    // float total_loss = 0.0f;
    
    // for (int epoch = 0; epoch < nn->epochs; epoch++) {
    //     total_loss = 0.0f;
    //     for (int i = 0; i < train_data->rows; i++) {
    //         mat_data_i = mat_copy_soft(train_data, i, 1);
    //         status = mat_mul(mat_z, mat_data_i, nn->layers[0]->w);

    //         if (!status) {
    //             printf("error\n");
    //         }
    //         for (int j = 0; j < mat_z->cols; j++) {
    //             mat_a->data[j] = _nn_sigmoid(mat_z->data[j] + nn->layers[0]->b->data[j]);
    //             mat_loss->data[j] = _nn_loss(mat_a->data[j], train_labels->data[i*train_labels->cols+j]);
    //             for (int k = 0; k < nn->layers[0]->w->rows; k++) {
    //                 nn->layers[0]->w->data[k*nn->layers[0]->w->cols+j] -= (
    //                         nn->lr * 
    //                         mat_data_i->data[k] *
    //                         2 * (mat_a->data[j] - train_labels->data[i*train_labels->cols+j]) *
    //                         mat_a->data[j] * (1 - mat_a->data[j])
    //                     );
    //             }
    //         }
    //         total_loss += mat_sum_values(mat_loss);
            
    //     }
    //     printf("EPOCH %2d avg_loss : %3.2f\n", epoch, total_loss / train_data->rows);
    // }
    // mat_free(mat_z);
    // mat_free(mat_a);
    // mat_free(mat_loss);

    return TRUE;
}

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

int nn_predict(neural_network *nn, matrix *data, matrix *res) {
    if (nn == NULL) return FALSE;
    if (data == NULL || data->data == NULL) return FALSE;
    if (data->rows != 1 || data->cols != nn->input_size) return FALSE;
    if (res->rows != 1 || res->cols != nn->output_size) return FALSE;

    memcpy(
        nn->components[nn->input_idx]->val->data, 
        data->data, 
        nn->input_size * sizeof(float)
    );
    _nn_forward(nn);

    memcpy(
        res->data,
        nn->components[nn->output_idx]->val->data,
        nn->output_size * sizeof(float)
    );

    return TRUE;
}

void nn_free_neural_network(neural_network *nn) {
    if(nn == NULL) return;

    // for (int i = 0; i < nn->n_layers; i++) {
    //     _nn_free_layer(nn->layers[i]);
    // }
}