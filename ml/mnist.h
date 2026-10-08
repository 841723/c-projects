#ifndef MNIST_H
#define MNIST_H

#include "matrix.h"

#define IMAGE_SIZE 28


void mnist_get_data(
    matrix **train_images,
    matrix **test_images,
    matrix **train_labels,
    matrix **test_labels
);

void mnist_display_image(float *image);
#endif