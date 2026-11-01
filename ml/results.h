#ifndef RESULTS_H
#define RESULTS_H

#include <time.h>
#include <sys/time.h>

#include "matrix.h"
#include "neural_network.h"


typedef struct res_file {
    char *filename, *path;
    struct tm *current_time;

    matrix *mat_precision;
    matrix *mat_recall;
    matrix *mat_f1;
    float total_precision;
    double training_time_seconds;
    
    int correct, error, total;

    neural_network *nn;    
    // matrix *mat_epochs;
} res_file;

res_file *rf_create();

int rf_save_model_res(res_file *rf, matrix *results, matrix *labels);
int rf_save_nn(res_file *rf, neural_network *nn);
int rf_save_time(res_file *rf, struct timeval start, struct timeval end);

int rf_log_res(res_file *rf);
void rf_free(res_file *rf);

#endif
