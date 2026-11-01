#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

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
    const int str_size = 1024;
    char *pathname = malloc(str_size + 1);
    if (pathname == NULL || rf == NULL || rf->current_time == NULL) {
        free(pathname);
        return NULL;
    }

    if (strftime(pathname, str_size, "%Y_%m_%d_%H_%M_%S", rf->current_time) == 0) {
        free(pathname);
        return NULL;
    }

    return pathname;
}


/**********************************************
   Exported functions
***********************************************/
res_file *rf_create() {
    res_file *rf = calloc(1, sizeof(struct res_file));
    if (rf == NULL) return NULL;

    time_t t = time(NULL);
    struct tm *time_info = localtime(&t);
    if (time_info == NULL) {
        free(rf);
        return NULL;
    }
    rf->current_time = malloc(sizeof(struct tm));
    if (rf->current_time == NULL) {
        free(rf);
        return NULL;
    }
    *rf->current_time = *time_info;

    rf->path = "results";
    rf->filename = _res_get_filename(rf);
    if (rf->filename == NULL) {
        rf_free(rf);
        return NULL;
    }

    return rf;
}

int rf_save_model_res(res_file *rf, matrix *results, matrix *labels) {
    if (rf == NULL || results == NULL || labels == NULL) return FALSE;
    if (results->data == NULL || labels->data == NULL) return FALSE;
    if (results->rows <= 0 || results->cols <= 0) return FALSE;
    if (results->rows != labels->rows || results->cols != labels->cols) return FALSE;

    int classes = results->cols;
    matrix *precision_by_class = mat_create(1, classes);
    matrix *recall_by_class = mat_create(1, classes);
    matrix *f1_by_class = mat_create(1, classes);
    int *true_positives = calloc(classes, sizeof(int));
    int *predicted_count = calloc(classes, sizeof(int));
    int *actual_count = calloc(classes, sizeof(int));
    if (precision_by_class == NULL || recall_by_class == NULL || f1_by_class == NULL ||
        true_positives == NULL || predicted_count == NULL || actual_count == NULL) {
        mat_free(precision_by_class);
        mat_free(recall_by_class);
        mat_free(f1_by_class);
        free(true_positives);
        free(predicted_count);
        free(actual_count);
        return FALSE;
    }

    int correct = 0;
    for (int i = 0; i < results->rows; i++) {
        int predicted = -1;
        int actual = -1;
        float max_prediction = -INFINITY;
        float max_label = -INFINITY;

        for (int j = 0; j < classes; j++) {
            int idx = i * classes + j;
            float prediction_value = results->data[idx];
            float label_value = labels->data[idx];
            if (!isfinite(prediction_value) || !isfinite(label_value)) {
                mat_free(precision_by_class);
                mat_free(recall_by_class);
                mat_free(f1_by_class);
                free(true_positives);
                free(predicted_count);
                free(actual_count);
                return FALSE;
            }
            if (prediction_value > max_prediction) {
                max_prediction = prediction_value;
                predicted = j;
            }
            if (label_value > max_label) {
                max_label = label_value;
                actual = j;
            }
        }

        if (predicted < 0 || actual < 0 || max_label <= 0.0f) {
            mat_free(precision_by_class);
            mat_free(recall_by_class);
            mat_free(f1_by_class);
            free(true_positives);
            free(predicted_count);
            free(actual_count);
            return FALSE;
        }

        predicted_count[predicted]++;
        actual_count[actual]++;
        if (predicted == actual) {
            true_positives[actual]++;
            correct++;
        }
    }

    for (int i = 0; i < classes; i++) {
        float precision = predicted_count[i] > 0
            ? 100.0f * true_positives[i] / predicted_count[i]
            : 0.0f;
        float recall = actual_count[i] > 0
            ? 100.0f * true_positives[i] / actual_count[i]
            : 0.0f;
        float f1 = (precision + recall) > 0.0f
            ? 2.0f * precision * recall / (precision + recall)
            : 0.0f;

        precision_by_class->data[i] = precision;
        recall_by_class->data[i] = recall;
        f1_by_class->data[i] = f1;
    }

    mat_free(rf->mat_precision);
    mat_free(rf->mat_recall);
    mat_free(rf->mat_f1);
    rf->mat_precision = precision_by_class;
    rf->mat_recall = recall_by_class;
    rf->mat_f1 = f1_by_class;
    rf->correct = correct;
    rf->error = results->rows - correct;
    rf->total = results->rows;
    rf->total_precision = 100.0f * correct / results->rows;

    free(true_positives);
    free(predicted_count);
    free(actual_count);
    return TRUE;
}

int rf_save_nn(res_file *rf, neural_network *nn) {
    if (rf == NULL || nn == NULL) return FALSE;

    rf->nn = nn;
    return TRUE;
}

int rf_save_time(res_file *rf, struct timeval start, struct timeval end) {
    if (rf == NULL) return FALSE;

    double elapsed = ((double)end.tv_sec - (double)start.tv_sec) +
        ((double)end.tv_usec - (double)start.tv_usec) / 1000000.0;
    if (elapsed < 0.0) return FALSE;

    rf->training_time_seconds = elapsed;
    return TRUE;
}

int rf_log_res(res_file *rf) {
    if (rf == NULL || rf->path == NULL || rf->filename == NULL || rf->current_time == NULL ||
        rf->nn == NULL || rf->mat_precision == NULL || rf->mat_recall == NULL || rf->mat_f1 == NULL) {
        return FALSE;
    }

    char filepath[1024];
    int path_length = snprintf(filepath, sizeof(filepath), "./%s/%s.txt", rf->path, rf->filename);
    if (path_length < 0 || (size_t)path_length >= sizeof(filepath)) return FALSE;

    FILE *f = fopen(filepath, "wb");
    if (f == NULL) return FALSE;

    char timestamp[64];
    if (strftime(timestamp, sizeof(timestamp), "%Y/%m/%d %H:%M:%S", rf->current_time) == 0) {
        fclose(f);
        return FALSE;
    }
    fprintf(f, "%s\n\n", timestamp);

    fprintf(f, "epochs=%d\n", rf->nn->epochs);
    fprintf(f, "lr=%f\n", rf->nn->lr);
    fprintf(f, "training_time_seconds=%.6f\n", rf->training_time_seconds);
    fprintf(f, "n_layers=%d\n", rf->nn->n_layers);
    for (int i = 0; i < rf->nn->n_layers; i++) {
        fprintf(f, "  %d: n_cells=%d\n", i, rf->nn->layers[i]->n_cells);
    }
    fprintf(f, "\n");

    fprintf(f, "correct_predictions=%d\n", rf->correct);
    fprintf(f, "wrong_predictions=%d\n", rf->error);
    fprintf(f, "total_predictions=%d\n", rf->total);
    fprintf(f, "\n");
    fprintf(f, "Metrics by class:\n");
    for (int i = 0; i < rf->mat_precision->cols; i++) {
        fprintf(f, "  %d: precision=%.2f%% recall=%.2f%% f1=%.2f%%\n",
            i,
            rf->mat_precision->data[i],
            rf->mat_recall->data[i],
            rf->mat_f1->data[i]
        );
    }

    fprintf(f, "\n");
    fprintf(f, "overall_accuracy=%.2f%%\n", rf->total_precision);

    return fclose(f) == 0 ? TRUE : FALSE;
}

void rf_free(res_file *rf) {
    if (rf == NULL) return;

    mat_free(rf->mat_precision);
    mat_free(rf->mat_recall);
    mat_free(rf->mat_f1);
    free(rf->current_time);
    free(rf->filename);
    free(rf);
}
