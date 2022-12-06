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

#include "full_adder.h"

int full_adder_train()
{
//    const size_t iterations = 99000;
    const size_t iterations = 50000;

    const val_t l_rate = 0.1f;

    const size_t n_samples = 8 + 4;

    const size_t n_layers = 5;
    const size_t n_inp = 3;
    const size_t n_out = 2;

    size_t *n_neu_per_layer = (size_t *)calloc(n_layers, sizeof(size_t));

    n_neu_per_layer[0] = n_inp;
    n_neu_per_layer[1] = 4;
    n_neu_per_layer[2] = 4;
    n_neu_per_layer[3] = 4;
    n_neu_per_layer[4] = n_out;

    bnn_t bnn = bnn_init(n_layers, n_neu_per_layer, n_samples, false);

    val_t inputs[n_samples][n_inp] = {
            {0.0f, 0.0f, 0.0f}, // 0 0 0 = 0 0
            {0.0f, 0.0f, 0.0f}, // 0 0 0 = 0 0
            {0.0f, 0.0f, 0.0f}, // 0 0 0 = 0 0
            {0.0f, 0.0f, 1.0f}, // 0 0 1 = 1 0
            {0.0f, 1.0f, 0.0f}, // 0 1 0 = 1 0
            {0.0f, 1.0f, 1.0f}, // 0 1 1 = 0 1
            {1.0f, 0.0f, 0.0f}, // 1 0 0 = 1 0
            {1.0f, 0.0f, 1.0f}, // 1 0 1 = 0 1
            {1.0f, 1.0f, 0.0f}, // 1 1 0 = 0 1
            {1.0f, 1.0f, 1.0f}, // 1 1 1 = 1 1
            {1.0f, 1.0f, 1.0f}, // 1 1 1 = 1 1
            {1.0f, 1.0f, 1.0f}  // 1 1 1 = 1 1
    };

    val_t outputs[n_samples][n_out] = {
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f},
            {1.0f, 0.0f},
            {0.0f, 1.0f},
            {0.0f, 1.0f},
            {1.0f, 1.0f},
            {1.0f, 1.0f},
            {1.0f, 1.0f}
    };


    for (size_t s = 0; s < n_samples; s++)
    {
        bnn_add_example(&bnn, inputs[s], outputs[s], true);
    }


    bnn_shuffle_train(&bnn, iterations, l_rate, true);
    bnn_save(&bnn, "../model.bnn", true);

    bnn_free(&bnn);

    return 0;
}

int full_adder_predict()
{
    bnn_t bnn = bnn_load("../model.bnn", false);
    val_t input[] = {1, 1, 1};

    bnn_predict(&bnn, input, true);

    bnn_free(&bnn);

    return 0;
}



//int main()
//{
//
////    full_adder_train();
////    full_adder_predict();
//
//    return 0;
//}


