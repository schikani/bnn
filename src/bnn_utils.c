//
// Created by Shivang Chikani on 29/11/22.
//
#include "bnn_utils.h"

//----------------------------------------------------------------------------------
//DICTIONARY
dict_t *bnn_utils_dict_get(const char *dict_path)
{
    // Try to open if exist or create new
    FILE *dict_file = fopen(dict_path, "r");
    if (dict_file == NULL)
    {
        dict_file = fopen(dict_path, "w");
        if (dict_file == NULL)
        {
            perror("");
            exit(1);
        }
        fclose(dict_file);
        bnn_utils_dict_get(dict_path);
    }

    // Allocate necessary memories
    size_t line_count = bnn_utils_get_line_count(dict_file);
    dict_t *dict = (dict_t *)calloc(1, sizeof(dict_t));
    dict->keys = (char **)calloc(line_count, sizeof(char *));
    dict->values = (char **)calloc(line_count, sizeof(char *));
    dict->size = line_count;
    dict->path = dict_path;

    const int max_line_length = 512;
    char line[max_line_length];
    const char *sep = ":";
    size_t current_line_idx = 0;

    while((fgets(line, max_line_length, dict_file)))
    {
        char *token = strtok(line, sep);
        char *key = token;
        char *value = strtok(NULL, sep);


        dict->keys[current_line_idx] = (char *)calloc(strlen(key), sizeof(char));
        dict->values[current_line_idx] = (char *)calloc(strlen(value), sizeof(char));

        strcpy(dict->keys[current_line_idx], key);
        strcpy(dict->values[current_line_idx], value);

        if (dict->values[current_line_idx][strlen(value)-1] == '\n')
        {
            dict->values[current_line_idx][strlen(value)-1] = '\0';
        }

        current_line_idx++;

    }
    fclose(dict_file);

    return dict;
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

char *bnn_utils_dict_get_value(dict_t *dict, const char *key)
{

    char *value = NULL;

    for (size_t i = 0; i < dict->size; i++)
    {
        // Got a matching key
        if (strcmp(dict->keys[i], key) == 0)
        {
//            value = (char *)calloc(strlen(dict->values[i]), sizeof(char));
//            strcpy(value, dict->values[i]);
            value = dict->values[i];
            break;
        }
    }
    return value;
}

bool bnn_utils_dict_key_exists(dict_t *dict, const char *key)
{
//    dict_t *dict = bnn_utils_dict_get(dict_path);

    for (size_t i = 0; i < dict->size; i++)
    {
        // Got a matching key
        if (strcmp(dict->keys[i], key) == 0)
        {
            return true;
        }
    }
    bnn_utils_dict_free(dict);
    return false;
}


void bnn_utils_dict_add_pair(dict_t *dict, const char *key, const char *value)
{
    bool key_found = false;
//    dict_t *dict = bnn_utils_dict_get(dict_path);

    for (size_t i = 0; i < dict->size; i++)
    {
        // Got a matching key
        if (strcmp(dict->keys[i], key) == 0)
        {
            dict->values[i] = realloc(dict->values[i], strlen(value));
            strcpy(dict->values[i], value);
            key_found = true;
            break;
        }
    }

    // If new key value found!
    if (!key_found)
    {
        dict->size++;
        dict->keys = realloc(dict->keys, sizeof(char *) * dict->size);
        dict->values = realloc(dict->values, sizeof(char *) * dict->size);
        dict->keys[dict->size-1] = (char *)calloc(strlen(key), sizeof(char));
        dict->values[dict->size-1] = (char *)calloc(strlen(value), sizeof(char));

        strcpy(dict->keys[dict->size-1], key);
        strcpy(dict->values[dict->size-1], value);
    }

//    bnn_utils_dict_write(dict);
//    bnn_utils_dict_free(dict);
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

//----------------------------------------------------------------------------------

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

double *bnn_utils_pre_parse(bnn_data_t *bnn_data, char *line, const char *sep)
{
    size_t n_cols = bnn_data->n_inp + bnn_data->n_out;
    double *col_max_count = (double *)calloc(n_cols, sizeof(double));
    memset(col_max_count, 1, sizeof(double) * n_cols);
    double val = 0.0f;
    char *t;
    for (size_t col = 0; col < n_cols; col++)
    {
        char *token = strtok(col == 0 ? line: NULL, sep);

        val = strtod(token, &t);

//        printf("%s\n", token);

        // Category found
        if (token == t)
        {
            // path + "_col_" + col_n
            char col_dict_path[strlen(bnn_data->path) + strlen("_col_") + 10];
            sprintf(col_dict_path, "%s_col_%ld", bnn_data->path, col);
            dict_t *dict = bnn_utils_dict_get(col_dict_path);
            char cat_count[10];
            sprintf(cat_count, "%.lf", col_max_count[col]);
            bnn_utils_dict_add_pair(dict, token, cat_count);
            bnn_utils_dict_write(dict);
            bnn_utils_dict_free(dict);
            col_max_count[col]++;

//            printf("%s\n", token);

        }
        else
        {
            if (val > col_max_count[col])
            {
                col_max_count[col] = val;
            }
        }

    }
    return col_max_count;
}


void bnn_utils_parse(bnn_data_t *bnn_data, char *line, size_t row, const char *sep)
{
    size_t n_cols = bnn_data->n_inp + bnn_data->n_out;
    double *col_max_count = bnn_utils_pre_parse(bnn_data, line, sep);
    double val = 0.0f;
    char *t;
    for (size_t col = 0; col < n_cols; col++)
    {
        char *token = strtok(col == 0 ? line: NULL, sep);
        val = strtod(token, &t);
//        const double val = (double)(atof(token));

        // Category found
        if (token == t)
        {

            // path + "_col_" + col_no
            char col_dict_path[strlen(bnn_data->path) + strlen("_col_") + 10];
            sprintf(col_dict_path, "%s_col_%ld", bnn_data->path, col);
            dict_t *dict = bnn_utils_dict_get(col_dict_path);
//            val = (double)(atof(bnn_utils_dict_get_value(dict, token)) / col_max_count[col] ? col_max_count[col] > 0 : 1);
            val = (double)(atof(bnn_utils_dict_get_value(dict, token)) / col_max_count[col]);

            bnn_utils_dict_free(dict);

//        printf("%s | %.lf\n", token, val);
        }
        // Number found
        else
        {
            val /= col_max_count[col];
        }

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