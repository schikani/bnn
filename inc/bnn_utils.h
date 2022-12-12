//
// Created by Shivang Chikani on 29/11/22.
//

#ifndef BNN_BNN_UTILS_H
#define BNN_BNN_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bnn_data_t
{
    double **inp; // 2D array for inputs
    double **out; // 2D array for outputs
    size_t n_inp; // No of inputs
    size_t n_out; // No of outputs
    size_t n_rows; // No of rows
} bnn_data_t;

char *bnn_utils_read_line(FILE *file);
double **bnn_utils_new2D(size_t n_rows, size_t n_cols);
bnn_data_t *bnn_utils_new_data(size_t n_inp, size_t n_out, size_t n_rows);
void bnn_utils_parse(bnn_data_t *bnn_data, char *line, size_t row, const char *sep);
void bnn_utils_free_data(bnn_data_t *bnn_data);
void bnn_utils_shuffle(bnn_data_t *bnn_data);
bnn_data_t *bnn_utils_build(const char *path, size_t n_inp, size_t n_out, const char *sep);
size_t bnn_utils_get_line_count(FILE *file);
#endif //BNN_BNN_UTILS_H
