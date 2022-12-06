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

#include "bnn.h"

// STATIC FUNCTION DEFINITIONS
// ---------------------------------------------------------------------------------------------------------------------
static val_t bnn_random_double(void);
static void bnn_shuffle(bnn_t *bnn);
static void bnn_update_weights(bnn_t *bnn, val_t l_rate, bool log);
static void bnn_init_random_weights_and_biases(bnn_t *bnn, bool log);
static void bnn_compute_cost(bnn_t *bnn, size_t idx_sample, bool log);
// ---------------------------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------------------------
// STATIC FUNCTION IMPLEMENTATION

static val_t bnn_random_double(void)
{
    return ((val_t) rand()) / ((val_t) RAND_MAX);
}

static void bnn_shuffle(bnn_t *bnn)
{
    for (size_t a = 0; a < bnn->n_samples; a++)
    {
        size_t b = rand() % bnn->n_samples;
        double *ot = bnn->outputs[a];
        double *it = bnn->inputs[a];

        bnn->outputs[a] = bnn->outputs[b];
        bnn->outputs[b] = ot;

        bnn->inputs[a] = bnn->inputs[b];
        bnn->inputs[b] = it;
    }
}

static void bnn_update_weights(bnn_t *bnn, val_t l_rate, bool log)
{
    if (log)
        printf("Updating weights!\n");

    for (size_t l = 0; l < bnn->n_layers-1; l++)
    {
        for (size_t n = 0; n < bnn->n_neu_per_layer[l]; n++)
        {
            for (size_t i = 0; i < bnn->n_neu_per_layer[l+1]; i++)
            {
                // Update weights
                bnn->layers[l].neurons[n].out_w[i] = (bnn->layers[l].neurons[n].out_w[i] - (l_rate * bnn->layers[l].neurons[n].d_w[i]));
                if (log)
                    printf("Layer: %zu\nNeuron: %zu\nWeight: %lf\n", l, n, bnn->layers[l].neurons[n].out_w[i]);
            }

            // Update Bias
            bnn->layers[l].neurons[n].bias = bnn->layers[l].neurons[n].bias - (l_rate * bnn->layers[l].neurons[n].d_bias);
            if (log)
                printf("Layer: %zu | Bias: %lf\n", l, bnn->layers[l].neurons[n].bias);
        }
    }
}

static void bnn_init_random_weights_and_biases(bnn_t *bnn, bool log)
{

//    srand(time(NULL));

    if (log)
        printf("\n");

    for(size_t l = 0; l < bnn->n_layers - 1; l++)
    {
        if (log)
            printf("LAYER: %zu\n", l+1);
        for (size_t n = 0; n < bnn->n_neu_per_layer[l]; n++)
        {
            for (size_t i = 0; i < bnn->n_neu_per_layer[l+1]; i++)
            {
                // Init output weights for each neuron
                bnn->layers[l].neurons[n].out_w[i] = bnn_random_double();
                if (log)
                    printf("%zu:w[%zu][%zu]: %f\n", i, l, n, bnn->layers[l].neurons[n].out_w[i]);
                bnn->layers[l].neurons[n].d_w[i] = 0.0f;
            }

            // If the layer is not input, init biases
            if (l > 0)
            {
                bnn->layers[l].neurons[n].bias = bnn_random_double();
            }
        }
        if (log)
            printf("\n");
    }

    // Init output layer biases
    for (size_t n = 0; n < bnn->n_neu_per_layer[bnn->n_layers - 1]; n++)
    {
        bnn->layers[bnn->n_layers - 1].neurons[n].bias = bnn_random_double();
    }
}

static void bnn_compute_cost(bnn_t *bnn, size_t idx_sample, bool log)
{
    val_t tmp_cost;
    val_t total_cost = 0.0f;

    val_t total_error = 0.0f;


    // Output Array
    for (size_t n = 0; n < bnn->n_out; n++)
    {
        tmp_cost = bnn->outputs[idx_sample][n] - bnn->layers[bnn->n_layers - 1].neurons[n].val;
        bnn->cost[n] = (tmp_cost * tmp_cost) / 2;
        total_cost += bnn->cost[n];
        total_error += fabs(tmp_cost);
    }

//    bnn->full_cost = (bnn->full_cost + total_cost) / compute_cost_n;
    bnn->full_cost = (bnn->full_cost + total_cost) / 2;


    if (log)
    {
        printf("Error: %.1lf %%\n", total_error * 100.0f / (val_t)bnn->n_out);
        printf("Cost: %lf\n", bnn->full_cost);
    }
}
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
// ACTIVATION FUNCTIONS

val_t bnn_activation_relu(val_t a)
{
    if (a < 0)
        return 0;
    else
        return a;

}

val_t bnn_activation_sigmoid(val_t a)
{
    return 1.0 / (1.0 + exp(-a));
}

val_t bnn_activation_tanh(val_t a)
{
//    return (exp(a) - exp(-a)) / (exp(a) + exp(-a));
    return tanh(a);
}

val_t bnn_activation_linear(val_t a)
{
    return a;
}
// ---------------------------------------------------------------------------------------------------------------------



bnn_t bnn_init(size_t n_layers, size_t *n_neu_per_layer, size_t n_samples, bool log)
{
    bnn_t bnn;
    bnn.n_layers = n_layers;
    bnn.n_neu_per_layer = n_neu_per_layer;
    bnn.n_inp = n_neu_per_layer[0];
    bnn.n_out = n_neu_per_layer[n_layers-1];
    bnn.n_samples = n_samples;
    bnn.full_cost = 0.0f;
    bnn.activation_hidden_func = bnn_activation_relu;
    bnn.activation_output_func = bnn_activation_sigmoid;

    bnn.pred_val = (val_t *)calloc(bnn.n_out, sizeof(val_t));

    // Allocate memory for layers
    bnn.layers = (bnn_layer_t *)calloc(n_layers, sizeof(bnn_layer_t));
    for (size_t l = 0; l < n_layers; l++) {
        bnn.layers[l] = bnn_create_layer(n_neu_per_layer[l]);
        bnn.layers[l].n_neurons = n_neu_per_layer[l];
        if (log)
        {
            printf("Created Layer: %zu\n", l+1);
            printf("Number of Neurons in Layer %zu: %zu\n", l+1, bnn.layers[l].n_neurons);
        }

        // Create neurons in each layer
        for (size_t n = 0; n < n_neu_per_layer[l]; n++)
        {
            if (l < (n_layers - 1))
            {
                // l + 1 is the count of neurons for next layer to count the n_weights required
                size_t next_layer_n_neurons = n_neu_per_layer[l + 1];
                bnn.layers[l].neurons[n] = bnn_create_neuron(next_layer_n_neurons);
            }

            if (log)
            {
                printf("Neuron: %zu in Layer: %zu created!\n", n+1, l+1);
            }
        }
        if (log)
            printf("\n");
    }

    // Allocate memory for layer_cost array (output array)
    bnn.cost = (val_t *)calloc(n_neu_per_layer[n_layers - 1], sizeof(val_t));

    // Allocate memory for inputs and outputs
    bnn.inputs = (val_t **)calloc(n_samples, sizeof(val_t *));
    bnn.outputs = (val_t **)calloc(n_samples, sizeof(val_t *));

    for (size_t i = 0; i < n_samples; i++)
    {
        bnn.inputs[i] = (val_t *)calloc(bnn.n_inp, sizeof(val_t));
        bnn.outputs[i] = (val_t *)calloc(bnn.n_out, sizeof(val_t));
    }

    return bnn;
}

val_t *bnn_feed_input(bnn_t *bnn, size_t idx_sample, bool log)
{
    if (log)
        printf("Input: ");

    // To use later in the project (currently unused)
    bnn->inp_current = bnn->inputs[idx_sample];

    for (size_t i = 0; i < bnn->n_inp; i++)
    {
        bnn->layers[0].neurons[i].val = bnn->inp_current[i];
        if (log)
        {
            printf("%lf ", bnn->layers[0].neurons[i].val);
        }
    }
    if (log)
        printf("\n");


    return bnn->inp_current;
}


int bnn_forward_propogation(bnn_t *bnn, bool log)
{

    if (log)
        printf("Output: ");

    for (size_t l = 1; l < bnn->n_layers; l++)
    {

        for (size_t n = 0; n < bnn->n_neu_per_layer[l]; n++)
        {
//        printf("qwerty\n");
            bnn->layers[l].neurons[n].z = bnn->layers[l].neurons[n].bias;

            // Exclude output
            for (size_t i = 0; i < bnn->n_neu_per_layer[l-1]; i++)
            {
                bnn->layers[l].neurons[n].z += (bnn->layers[l-1].neurons[i].out_w[n]) * (bnn->layers[l-1].neurons[i].val);
            }

            // RELU Activation function for Hidden layers
            if (l < bnn->n_layers - 1)
            {
                bnn->layers[l].neurons[n].val = bnn->activation_hidden_func(bnn->layers[l].neurons[n].z);
            }

            // SIGMOID Activation function for Output layer
            else
            {
                bnn->layers[l].neurons[n].val = bnn->activation_output_func(bnn->layers[l].neurons[n].z);

                // Fill predicted val array
                bnn->pred_val[n] = bnn->layers[l].neurons[n].val;

                if (log)
                {
                    printf("%.2lf ", bnn->pred_val[n]);
//                    printf("%d ", (int)round(bnn->layers[l].neurons[n].val));

                }
            }
        }
    }
    if (log)
        printf("\n");

    return 0;
}



void bnn_back_propogation(bnn_t *bnn, size_t idx_sample, bool log)
{
    // Output Layer
   //  bnn->n_neu_per_layer[bnn->n_layers - 1]
    for (size_t n = 0; n < bnn->n_out; n++)
    {
        bnn->layers[bnn->n_layers-1].neurons[n].d_z = (bnn->layers[bnn->n_layers-1].neurons[n].val - bnn->outputs[idx_sample][n])
                * (bnn->layers[bnn->n_layers-1].neurons[n].val)
                * (1 - bnn->layers[bnn->n_layers-1].neurons[n].val);

        for (size_t i = 0; i < bnn->n_neu_per_layer[bnn->n_layers - 2]; i++)
        {
            bnn->layers[bnn->n_layers-2].neurons[i].d_w[n] = (bnn->layers[bnn->n_layers-1].neurons[n].d_z * bnn->layers[bnn->n_layers-2].neurons[i].val);
            bnn->layers[bnn->n_layers-2].neurons[i].d_val = (bnn->layers[bnn->n_layers-2].neurons[i].out_w[n] * bnn->layers[bnn->n_layers-1].neurons[n].d_z);
        }
        bnn->layers[bnn->n_layers-1].neurons[n].d_bias = bnn->layers[bnn->n_layers-1].neurons[n].d_z;
    }

    // Hidden Layers
    for (size_t l = bnn->n_layers-2; l > 0; l--)
    {
        for (size_t n = 0; n < bnn->n_neu_per_layer[l]; n++)
        {
            if (bnn->layers[l].neurons[n].z >= 0)
                bnn->layers[l].neurons[n].d_z = bnn->layers[l].neurons[n].d_val;
            else
                bnn->layers[l].neurons[n].d_z = 0;

            for (size_t p = 0; p < bnn->n_neu_per_layer[l-1]; p++)
            {
                bnn->layers[l-1].neurons[p].d_w[n] = bnn->layers[l].neurons[n].d_z * bnn->layers[l-1].neurons[p].val;

                if (l > 1)
                    bnn->layers[l-1].neurons[p].d_val = (bnn->layers[l-1].neurons[p].out_w[n] * bnn->layers[l].neurons[n].d_z);
            }

            bnn->layers[l].neurons[n].d_bias = bnn->layers[l].neurons[n].d_z;
        }
    }
}

void bnn_train_once(bnn_t *bnn, val_t l_rate, bool log)
{
    for (size_t i = 0; i < bnn->n_samples; i++)
    {
        bnn_feed_input(bnn, i, log);
        bnn_forward_propogation(bnn, log);
        bnn_compute_cost(bnn, i, log);
        bnn_back_propogation(bnn, i, false);
        bnn_update_weights(bnn, l_rate, false);
        if (log)
            printf("\n");
    }
}



// Train the Neural Network
void bnn_train(bnn_t *bnn, size_t iterations, val_t l_rate, bool log)
{
    // Initialize weights and biases
    bnn_init_random_weights_and_biases(bnn, log);

    if (log)
        printf("TRAINING STARTED!\n");

    for (size_t it = 0; it < iterations; it++)
    {
        if (log)
            printf("ITERATION: %zu / %zu\r", it + 1, iterations);

        bnn_train_once(bnn, l_rate, log);
    }
}




void bnn_shuffle_train(bnn_t *bnn, size_t iterations, val_t l_rate, bool log)
{

    // Initialize weights and biases
    bnn_init_random_weights_and_biases(bnn, log);

    if (log)
        printf("\nTRAINING STARTED!\n\n");

    for (size_t it = 0; it < iterations; it++)
    {
        if (log)
            printf("ITERATION: %zu / %zu\n", it + 1, iterations);

        bnn_shuffle(bnn);

        bnn_train_once(bnn, l_rate, log);
    }

    if (log)
        printf("TRAINING ENDED!\n\n");
}

int bnn_add_example(bnn_t *bnn, val_t *input, val_t *output, bool log)
{
    static int idx_current_sample = -1;
    idx_current_sample++;

    memcpy(bnn->inputs[idx_current_sample], input, sizeof(val_t) * bnn->n_inp);
    memcpy(bnn->outputs[idx_current_sample], output, sizeof(val_t) * bnn->n_out);

    if (log)
    {
        printf("Sample added at index: %d\n", idx_current_sample);
    }


    return 0;
}

val_t *bnn_predict(bnn_t *bnn, const val_t *input, bool log)
{

    // Fill input at zero index
    memcpy(bnn->inputs[0], input, sizeof(val_t) * bnn->n_inp);

    bnn_feed_input(bnn, 0, log);
    if (log)
        printf("Predicted ");
    bnn_forward_propogation(bnn, log);

    return bnn->pred_val;
}

void bnn_save(bnn_t *bnn, const char *path, bool log)
{

    FILE *file = fopen(path, "w");
    // Save the header
    fprintf(file, "%zu\n", bnn->n_layers);

    for (size_t l = 0; l < bnn->n_layers; l++)
    {
        fprintf(file, "%zu ", bnn->n_neu_per_layer[l]);
    }
    fprintf(file, "\n");

    for(size_t l = 0; l < bnn->n_layers - 1; l++)
    {
        for (size_t n = 0; n < bnn->n_neu_per_layer[l]; n++)
        {
            for (size_t i = 0; i < bnn->n_neu_per_layer[l+1]; i++)
            {
                // Init output weights for each neuron
                fprintf(file, "%lf ", bnn->layers[l].neurons[n].out_w[i]);

            }

            // If the layer is not input, init biases
            if (l > 0)
            {
                fprintf(file, "%lf ", bnn->layers[l].neurons[n].bias);
            }
        }

    }
    fclose(file);

    if (log)
        printf("Model saved to: %s\n\n", path);
}

bnn_t bnn_load(const char *path, bool log)
{

    size_t n_layers = 0;
    size_t *n_neu_per_layer;


    FILE *file = fopen(path, "r");
    // Save the header
    fscanf(file, "%zu\n", &n_layers);

    n_neu_per_layer = (size_t *)calloc(n_layers, sizeof(size_t));

    for (size_t l = 0; l < n_layers; l++)
    {
        fscanf(file, "%zu ", &n_neu_per_layer[l]);
    }
    fscanf(file, "\n");


    bnn_t bnn = bnn_init(n_layers, n_neu_per_layer, 1, log);


    for(size_t l = 0; l < bnn.n_layers - 1; l++)
    {
        for (size_t n = 0; n < bnn.n_neu_per_layer[l]; n++)
        {
            for (size_t i = 0; i < bnn.n_neu_per_layer[l+1]; i++)
            {
                // Init output weights for each neuron
                fscanf(file, "%lf ", &bnn.layers[l].neurons[n].out_w[i]);

            }

            // If the layer is not input, init biases
            if (l > 0)
            {
                fscanf(file, "%lf ", &bnn.layers[l].neurons[n].bias);
            }
        }
    }

    fclose(file);

    if (log)
        printf("Model loaded from: %s\n\n", path);

    return bnn;
}

int bnn_free(bnn_t *bnn)
{
    free(bnn->n_neu_per_layer);
    free(bnn->layers);
    free(bnn->cost);
    free(bnn->pred_val);

    for (size_t s = 0; s < bnn->n_samples; s++)
    {
        free(bnn->inputs[s]);
        free(bnn->outputs[s]);
    }

    return 0;
}