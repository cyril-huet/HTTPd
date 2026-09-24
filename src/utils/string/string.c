#include "string.h"

#include <stddef.h>
#include <stdlib.h>

struct string *string_create(const char *str, size_t size)
{
    struct string *res = malloc(sizeof(struct string));
    if (res == NULL)
    {
        return NULL;
    }
    res->size = size;
    res->data = malloc(size * sizeof(char));
    if (res->data == NULL)
    {
        free(res);
        return NULL;
    }
    for (size_t i = 0; i < size; i++)
    {
        res->data[i] = str[i];
    }
    return res;
}

int string_compare_n_str(const struct string *str1, const char *str2, size_t n)
{
    size_t size_str = 0;

    if (str1->size > n)
    {
        size_str = n;
    }
    else
    {
        size_str = str1->size;
    }
    size_t index = 0;
    while (index != size_str)
    {
        if (str1->data[index] != str2[index])
        {
            return str1->data[index] - str2[index];
        }
        index++;
    }
    if (str1->size < n)
    {
        return -1;
    }
    return 0;
}

void string_concat_str(struct string *str, const char *to_concat, size_t size)
{
    if (size == 0)
    {
        return;
    }
    str->data = realloc(str->data, str->size + size);
    if (str->data == NULL)
    {
        return;
    }
    size_t index = 0;
    for (size_t i = str->size; i < size + str->size; i++)
    {
        str->data[i] = to_concat[index];
        index++;
    }
    str->size += size;
}

void string_destroy(struct string *str)
{
    if (str == NULL)
    {
        return;
    }
    free(str->data);
    free(str);
}
