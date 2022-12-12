//
// Created by Shivang Chikani on 29/11/22.
//
#include "bnn_utils.h"

size_t bnn_utils_get_line_count(FILE *file)
{
    size_t count = 0;  // Line counter (result)
    char c;
    // Extract characters from file and store in character c
    for (c = getc(file); c != EOF; c = getc(file))
        if (c == '\n') // Increment count if this character is newline
            count = count + 1;

//     Close the file
//    fclose(file);
    rewind(file);
    return count;
}

char *bnn_utils_read_line(FILE *file)
{
    size_t count = 0;
    size_t size = 256;
    char *line = (char *)malloc(size * sizeof(char));
    char a = '\n';

    if (file == NULL)
    {
        printf("Can't open filestream for readline\n");
        exit(EXIT_FAILURE);
    }

    while ((a = fgetc(file)) != '\n' && a != EOF)
    {
        line[count++] = a;
    }

    return line;
}

double **bnn_utils_new2D(size_t n_rows, size_t n_cols)
{
    double **_rows = (double **)malloc(n_rows * sizeof(double *));
    for (size_t i = 0; i < n_rows; i++)
    {
        _rows[i] = (double *)malloc(n_cols * sizeof(double));
    }

    return _rows;
}

bnn_data_t *bnn_utils_new_data(size_t n_inp, size_t n_out, size_t n_rows)
{
    bnn_data_t *bnn_data = (bnn_data_t *)malloc(sizeof(bnn_data_t));
    bnn_data->inp = bnn_utils_new2D(n_rows, n_inp);
    bnn_data->out = bnn_utils_new2D(n_rows, n_out);
    bnn_data->n_inp = n_inp;
    bnn_data->n_out = n_out;
    bnn_data->n_rows = n_rows;

    return bnn_data;
}

void bnn_utils_parse(bnn_data_t *bnn_data, char *line, size_t row, const char *sep)
{
    size_t n_cols = bnn_data->n_inp + bnn_data->n_out;
    char *t;
    for (size_t col = 0; col < n_cols; col++)
    {
        char *token = strtok(col == 0 ? line: NULL, sep);
//        const double val = (double)(atof(token));
        const double val = strtod(token, &t);
        if (token == t)
        {
            printf("%s\n", t);
        }
        else
            printf("%lf\n", val);

        if (col < bnn_data->n_inp)
            bnn_data->inp[row][col] = val;
        else
            bnn_data->out[row][col - bnn_data->n_inp] = val;
    }
}

void bnn_utils_free_data(bnn_data_t *bnn_data)
{
    for (size_t row = 0; row < bnn_data->n_rows; row++)
    {
        free(bnn_data->inp[row]);
        free(bnn_data->out[row]);
    }
    free(bnn_data->inp);
    free(bnn_data->out);

    free(bnn_data);
}

void bnn_utils_shuffle(bnn_data_t *bnn_data)
{
    for (size_t a = 0; a < bnn_data->n_rows; a++)
    {
        size_t b = rand() % bnn_data->n_rows;
        double *ot = bnn_data->out[a];
        double *it = bnn_data->inp[a];

        bnn_data->out[a] = bnn_data->out[b];
        bnn_data->out[b] = ot;

        bnn_data->inp[a] = bnn_data->inp[b];
        bnn_data->inp[b] = it;
    }
}

bnn_data_t *bnn_utils_build(const char *path, size_t n_inp, size_t n_out, const char *sep)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Error opening dataset file: %s\n", path);
        exit(1);
    }
    size_t n_rows = bnn_utils_get_line_count(file);
    bnn_data_t *bnn_data = bnn_utils_new_data(n_inp, n_out, n_rows);
    for (size_t row = 0; row < n_rows; row++)
    {
        char *line = bnn_utils_read_line(file);
        bnn_utils_parse(bnn_data, line, row, sep);
        free(line);
    }
    fclose(file);
    return bnn_data;
}