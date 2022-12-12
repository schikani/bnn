/*
MIT License

bnn - Copyright (c) 2022 Shivang Chikani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef BNN_BNN_H
#define BNN_BNN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

//#define ERROR_THRESHOLD 0.0001

typedef double val_t;

//-----------------------------------------------------------------
// NEURON TYPE

typedef struct bnn_neuron_t
{
    double val;
    double *out_w;
    double bias;
    double z;

    double d_val;
    double *d_w;
    double d_bias;
    double d_z;

} bnn_neuron_t;

bnn_neuron_t bnn_create_neuron(size_t n_weights);
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// LAYER TYPE

typedef struct bnn_layer_t
{
    size_t n_neurons;
    bnn_neuron_t *neurons;
} bnn_layer_t;

bnn_layer_t bnn_create_layer(size_t n_neurons);
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// BNN TYPE

typedef struct bnn_t
{
    size_t n_layers;
    size_t *n_neu_per_layer;
    bnn_layer_t *layers;
    val_t *cost;
    val_t full_cost;
    size_t n_inp;
    size_t n_out;
    size_t n_samples;
    val_t **inputs;
    val_t **outputs;
    val_t *inp_current;
    val_t *pred_val;
    val_t (*activation_hidden_func)(val_t a);
    val_t (*activation_output_func)(val_t a);

} bnn_t;
//-----------------------------------------------------------------


//-----------------------------------------------------------------
#ifndef BNN_PREDICT_ONLY // To exclude headers if only prediction is the case
void bnn_shuffle(bnn_t *bnn);
void bnn_update_weights(bnn_t *bnn, val_t l_rate, bool log);
void bnn_compute_cost(bnn_t *bnn, size_t idx_sample, bool log);
void bnn_init_random_weights_and_biases(bnn_t *bnn, bool log);
void bnn_train_once(bnn_t *bnn, val_t l_rate, bool log);
void bnn_train(bnn_t *bnn, size_t iterations, val_t l_rate, bool log);
void bnn_shuffle_train(bnn_t *bnn, size_t iterations, val_t l_rate, bool log);
void bnn_back_propogation(bnn_t *bnn, size_t idx_sample, bool log);
int bnn_add_example(bnn_t *bnn, val_t *input, val_t *output, size_t idx_sample, bool log);
void bnn_save(bnn_t *bnn, const char *path, bool log);
#endif
//-----------------------------------------------------------------


//-----------------------------------------------------------------
val_t bnn_random_double(void);
bnn_t bnn_init(size_t n_layers, size_t *n_neu_per_layer, size_t n_samples, bool log);
val_t *bnn_feed_input(bnn_t *bnn, size_t idx_sample, bool log);
int bnn_forward_propogation(bnn_t *bnn, bool log);
val_t *bnn_predict(bnn_t *bnn, const val_t *input, bool log);
bnn_t bnn_load(const char *path, bool log);
int bnn_free(bnn_t *bnn);
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// ACTIVATION FUNCTIONS
val_t bnn_activation_relu(val_t a);
val_t bnn_activation_sigmoid(val_t a);
val_t bnn_activation_tanh(val_t a);
val_t bnn_activation_linear(val_t a);
//-----------------------------------------------------------------



#endif //BNN_BNN_H
