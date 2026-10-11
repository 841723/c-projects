#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "results.h"
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
char *_res_get_filename(res_file *rf) {
    int str_size = 1024;
    char *pathname = malloc(str_size +1);

    strftime(pathname, str_size, "%Y_%m_%d_%H_%M_%S", rf->current_time);

    return pathname;
}


/**********************************************
   Exported functions
***********************************************/
res_file *rf_create() {
    res_file *rf = malloc(sizeof(struct res_file));

    time_t t = time(NULL);
    rf->current_time = localtime(&t);

    
    rf->path = "results";
    rf->filename = _res_get_filename(rf);

    rf->mat_precision = mat_create(1, 10);
    // rf->mat_epochs = mat_create(1, 10);

    return rf;
}

int rf_save_model_res(res_file *rf, matrix *results, matrix *labels) {
    // mat_print(labels);
    // mat_print(results);

    if (rf == NULL || results == NULL || labels == NULL) return FALSE;
    if (results->rows != labels->rows || results->cols != labels->cols) return FALSE;

    float max_in_row_val;
    int idx, label, max_in_row_idx;
    int correct = 0, errors = 0;
    matrix *mat_error = mat_create(1, 10);
    mat_fill(mat_error, 0);
    matrix *mat_nums = mat_create(1, 10);
    mat_fill(mat_nums, 0);

    for (int i = 0; i < results->rows; i++) {
        max_in_row_idx = -1;
        max_in_row_val = -1e30f;
        label = -1;

        for (int j = 0; j < results->cols; j++) {
            idx = i*results->cols+j;
        
            if (labels->data[idx] == 1) {
                label = j;
            }
            if (max_in_row_val < results->data[idx]) {
                max_in_row_idx = j;
                max_in_row_val = results->data[idx];
            }
        }
        if (label == max_in_row_idx) {
            correct++;
            mat_nums->data[label]++;
        } 
        else {
            errors++;
            mat_nums->data[label]++;
            mat_error->data[label]++;
        }


    }

    float precision = ((float)(correct)/results->rows)*100;
    rf->correct = correct;
    rf->error = errors;
    rf->total = correct + errors;

    for (int i = 0; i < mat_error->cols; i++) {
        rf->mat_precision->data[i] = ((mat_nums->data[i]-mat_error->data[i])/mat_nums->data[i])*100;
    }
    rf->total_precision = precision;
    
    mat_free(mat_error);
    mat_free(mat_nums);
    return TRUE;
}

int rf_save_nn(res_file *rf, neural_network *nn) {
    if (rf == NULL || nn == NULL) return FALSE;

    rf->nn = nn;
    return TRUE;
}

int rf_log_res(res_file *rf) {
    char content[1024];
    snprintf(content, sizeof(content), "./%s/%s.txt", rf->path, rf->filename);

    FILE *f = fopen(content, "wb");

    strftime(content, 120, "%Y/%m/%d %H:%M:%S", rf->current_time);
    fprintf(f, "%s\n\n", content);

    fprintf(f, "epochs=%d\n", rf->nn->epochs);
    fprintf(f, "lr=%.2f\n", rf->nn->lr);
    fprintf(f, "n_layers=%d\n", rf->nn->n_layers);
    for (int i = 0; i < rf->nn->n_layers; i++) {
        fprintf(f, "  %d: n_cells=%d\n", i, rf->nn->layers[i]->n_cells);
    }
    fprintf(f, "\n");

    fprintf(f, "correct_predictions=%d\n", rf->correct);
    fprintf(f, "wrong_predictions=%d\n", rf->error);
    fprintf(f, "total_predictions=%d\n", rf->total);
    fprintf(f, "\n");
    fprintf(f, "Precision by image matrix:\n");
    for (int i = 0; i < rf->mat_precision->cols; i++) {
        fprintf(f, "  %d: precision=%.2f%%\n", i, rf->mat_precision->data[i]);
    }

    fprintf(f, "\n");
    fprintf(f, "total_precision=%.2f%%\n", rf->total_precision);

    fclose(f);
    
    return TRUE;
}

void rf_free(res_file *rf) {
    if (rf == NULL) return;

    mat_free(rf->mat_precision);

    free(rf);
}