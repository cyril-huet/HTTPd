#define _POSIX_C_SOURCE 200809L
#include "logger_utils.h"

#include <stdlib.h>
#include <time.h>

char *to_string(const struct string *string)
{
    if (string == NULL)
    {
        return NULL;
    }

    char *result = malloc(string->size + 1);
    if (result == NULL)
    {
        return NULL;
    }

    for (size_t index = 0; index < string->size; index++)
    {
        result[index] = string->data[index];
    }

    result[string->size] = '\0';
    return result;
}

char *date_log(void)
{
    char buffer[64];
    time_t now = time(NULL);
    struct tm *utc_time = gmtime(&now);
    if (utc_time == NULL)
    {
        return NULL;
    }

    size_t length =
        strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", utc_time);
    if (length == 0)
    {
        return NULL;
    }

    char *result = malloc(length + 1);
    if (result == NULL)
    {
        return NULL;
    }

    for (size_t index = 0; index <= length; index++)
    {
        result[index] = buffer[index];
    }

    return result;
}
