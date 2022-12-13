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

//#include "bnn_utils.h"
#include "full_adder.h"
//#include "cars.h"

int main(int argc, char *argv[])
{
    full_adder_train();
    full_adder_predict();
//    get_cars_data();

//    char *value = bnn_utils_dict_get_value("../dict", "Apple");
//
//    if (value)
//        printf("value: %s", value);
//    else
//        printf("key not found!");
//
//    bnn_utils_dict_add_pair("../dict", "Thanos", "500");
//    bnn_utils_dict_add_pair("../dict", "Shivang", "1996");
//    bnn_utils_dict_add_pair("../dict", "Bhagya", "1998");
//    bnn_utils_dict_add_pair("../dict", "Apple", "2000");



    return 0;
}
