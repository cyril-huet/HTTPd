#include "string.h"

#include <stdint.h>
#include <stdlib.h>

struct string *string_create(const char *data, size_t size)
{
    if (data == NULL && size > 0)
    {
        return NULL;
    }

    struct string *string = malloc(sizeof(struct string));
    if (string == NULL)
    {
        return NULL;
    }

    string->data = malloc(size + 1);
    if (string->data == NULL)
    {
        free(string);
        return NULL;
    }

    for (size_t index = 0; index < size; index++)
    {
        string->data[index] = data[index];
    }

    string->data[size] = '\0';
    string->size = size;
    return string;
}

int string_compare_n_str(const struct string *string, const char *data,
                         size_t size)
{
    if (string == NULL || data == NULL)
    {
        return -1;
    }

    size_t compared_size = string->size;
    if (compared_size > size)
    {
        compared_size = size;
    }

    for (size_t index = 0; index < compared_size; index++)
    {
        if (string->data[index] != data[index])
        {
            return string->data[index] - data[index];
        }
    }

    if (string->size < size)
    {
        return -1;
    }

    return 0;
}

void string_concat_str(struct string *string, const char *data, size_t size)
{
    if (string == NULL || data == NULL || size == 0)
    {
        return;
    }

    if (size > SIZE_MAX - string->size)
    {
        return;
    }

    size_t new_size = string->size + size;
    char *new_data = realloc(string->data, new_size + 1);
    if (new_data == NULL)
    {
        return;
    }

    for (size_t index = 0; index < size; index++)
    {
        new_data[string->size + index] = data[index];
    }

    new_data[new_size] = '\0';
    string->data = new_data;
    string->size = new_size;
}

void string_destroy(struct string *string)
{
    if (string == NULL)
    {
        return;
    }

    free(string->data);
    free(string);
}
