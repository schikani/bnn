//
// Created by Shivang Chikani on 29/11/22.
//
#include "bnn_utils.h"

size_t bnn_utils_get_line_count(FILE *file)
{
    size_t count = 0;  // Line counter (result)
//    char c;
//    // Extract characters from file and store in character c
//    for (c = getc(file); c != EOF; c = getc(file))
//        if (c == '\n') // Increment count if this character is newline
//            count = count + 1;
        char line[512];
        while (fgets(line, 512, file))
        {
            count += 1;
        }

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

int bnn_utils_dict_free(dict_t *dict)
{
    for (size_t i = 0; i < dict->size; i++)
    {
        free(dict->keys[i]);
        free(dict->values[i]);
    }
    free(dict->keys);
    free(dict->values);
    free(dict);

    return 0;
}

dict_t *bnn_utils_dict_get(const char *dict_path)
{
    // Try to open if exist or create new
    FILE *dict = fopen(dict_path, "r");
    if (dict == NULL)
    {
        dict = fopen(dict_path, "w");
        if (dict == NULL)
        {
            perror("");
            exit(1);
        }
        fclose(dict);
        bnn_utils_dict_get(dict_path);
    }

    size_t line_count = bnn_utils_get_line_count(dict);
    dict_t *d = (dict_t *)calloc(1, sizeof(dict_t));
    d->keys = (char **)calloc(line_count, sizeof(char *));
    d->values = (char **)calloc(line_count, sizeof(char *));
    d->size = line_count;
    d->path = dict_path;

    const int max_line_length = 512;
    char line[max_line_length];
    const char *sep = ":";
    size_t current_line_idx = 0;

    while((fgets(line, max_line_length, dict)))
    {
        char *token = strtok(line, sep);
        char *key = token;
        char *value = strtok(NULL, sep);


        d->keys[current_line_idx] = (char *)calloc(strlen(key), sizeof(char));
        d->values[current_line_idx] = (char *)calloc(strlen(value), sizeof(char));

        strcpy(d->keys[current_line_idx], key);
        strcpy(d->values[current_line_idx], value);

        if (d->values[current_line_idx][strlen(value)-1] == '\n')
        {
            d->values[current_line_idx][strlen(value)-1] = '\0';
        }

        current_line_idx++;

    }
    fclose(dict);

    return d;
}

int bnn_utils_dict_write(dict_t *dict)
{
    FILE *file = fopen(dict->path, "w");
    for (size_t i = 0; i < dict->size; i++)
    {
        // k + v + ':' + '\n' + '\0'
        char to_write[strlen(dict->keys[i]) + strlen(dict->values[i]) + 3];
        sprintf(to_write, "%s:%s\n", dict->keys[i], dict->values[i]);
        fwrite(to_write, sizeof(char), strlen(to_write), file);
    }
    fclose(file);
    return 0;
}

char *bnn_utils_dict_get_value(const char *dict_path, const char *key)
{
    dict_t *d = bnn_utils_dict_get(dict_path);

    char *value = NULL;

    for (size_t i = 0; i < d->size; i++)
    {
        // Got a matching key
        if (strcmp(d->keys[i], key) == 0)
        {
            value = (char *)calloc(strlen(d->values[i]), sizeof(char));
            strcpy(value, d->values[i]);
            break;
        }
    }
    bnn_utils_dict_free(d);
    return value;
}

bool bnn_utils_dict_key_exists(const char *dict_path, const char *key)
{
    dict_t *d = bnn_utils_dict_get(dict_path);

    for (size_t i = 0; i < d->size; i++)
    {
        // Got a matching key
        if (strcmp(d->keys[i], key) == 0)
        {
            return true;
        }
    }
    bnn_utils_dict_free(d);
    return false;
}



void bnn_utils_dict_add_pair(const char *dict_path, const char *key, const char *value)
{
    bool key_found = false;
    dict_t *d = bnn_utils_dict_get(dict_path);

    for (size_t i = 0; i < d->size; i++)
    {
        // Got a matching key
        if (strcmp(d->keys[i], key) == 0)
        {
            d->values[i] = realloc(d->values[i], strlen(value));
            strcpy(d->values[i], value);
            key_found = true;
            break;
        }
    }

    // If new key value found!
    if (!key_found)
    {
        d->size++;
        d->keys = realloc(d->keys, sizeof(char *) * d->size);
        d->values = realloc(d->values, sizeof(char *) * d->size);
        d->keys[d->size-1] = (char *)calloc(strlen(key), sizeof(char));
        d->values[d->size-1] = (char *)calloc(strlen(value), sizeof(char));

        strcpy(d->keys[d->size-1], key);
        strcpy(d->values[d->size-1], value);
    }

    bnn_utils_dict_write(d);
    bnn_utils_dict_free(d);
}

void bnn_utils_parse(bnn_data_t *bnn_data, char *line, size_t row, const char *sep)
{
    size_t n_cols = bnn_data->n_inp + bnn_data->n_out;
    char *t;
    for (size_t col = 0; col < n_cols; col++)
    {
        // path + "_col_" + col_no
        char col_dict_path[strlen(bnn_data->path) + strlen("_col_") + 10];
        sprintf(col_dict_path, "%s_col_%ld", bnn_data->path, col);

        char *token = strtok(col == 0 ? line: NULL, sep);
        double val = strtod(token, &t);
//        const double val = (double)(atof(token));

        // Category found
        if (token == t)
        {
            val = (double)(atof(bnn_utils_dict_get_value(col_dict_path, t)));
//            printf("%s\n", t);
        }
        // Number found
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
    bnn_data->path = path;
    for (size_t row = 0; row < n_rows; row++)
    {
        char *line = bnn_utils_read_line(file);
        bnn_utils_parse(bnn_data, line, row, sep);
        free(line);
    }
    fclose(file);
    return bnn_data;
}