#include "request_headers.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "http_structs.h"

static int find_line_end(const char *buffer, size_t size, size_t start,
                         size_t *line_end)
{
    for (size_t index = start; index + 1 < size; index++)
    {
        if (buffer[index] == '\r' && buffer[index + 1] == '\n')
        {
            *line_end = index;
            return 0;
        }
    }

    return -1;
}

static char lowercase(char character)
{
    if (character >= 'A' && character <= 'Z')
    {
        return character + ('a' - 'A');
    }

    return character;
}

static int name_equals(const char *buffer, size_t start, size_t end,
                       const char *expected)
{
    size_t expected_length = strlen(expected);
    if (end - start != expected_length)
    {
        return 0;
    }

    for (size_t index = 0; index < expected_length; index++)
    {
        if (lowercase(buffer[start + index]) != expected[index])
        {
            return 0;
        }
    }

    return 1;
}

static char *copy_value(const char *buffer, size_t start, size_t end)
{
    size_t length = end - start;
    char *value = malloc(length + 1);
    if (value == NULL)
    {
        return NULL;
    }

    for (size_t index = 0; index < length; index++)
    {
        value[index] = buffer[start + index];
    }

    value[length] = '\0';
    return value;
}

static int parse_host(struct request_http *request, const char *buffer,
                      size_t start, size_t end)
{
    if (start == end || request->host != NULL)
    {
        return -1;
    }

    request->host = copy_value(buffer, start, end);
    if (request->host == NULL)
    {
        return -1;
    }

    return 0;
}

static int parse_length_value(const char *buffer, size_t start, size_t end,
                              size_t *result)
{
    if (start == end)
    {
        return -1;
    }

    *result = 0;
    for (size_t index = start; index < end; index++)
    {
        if (buffer[index] < '0' || buffer[index] > '9')
        {
            return -1;
        }

        size_t digit = buffer[index] - '0';
        if (*result > (SIZE_MAX - digit) / 10)
        {
            return -1;
        }

        *result = *result * 10 + digit;
    }

    return 0;
}

static int parse_content_length(struct request_http *request,
                                const char *buffer, size_t start, size_t end)
{
    if (request->flag_content == 1)
    {
        return -1;
    }

    if (parse_length_value(buffer, start, end, &request->content_length) < 0)
    {
        return -1;
    }

    request->flag_content = 1;
    return 0;
}

static void trim_value(const char *buffer, size_t *start, size_t *end)
{
    while (*start < *end && (buffer[*start] == ' ' || buffer[*start] == '\t'))
    {
        (*start)++;
    }

    while (*end > *start
           && (buffer[*end - 1] == ' ' || buffer[*end - 1] == '\t'))
    {
        (*end)--;
    }
}

static int parse_header(struct request_http *request, const char *buffer,
                        size_t start, size_t end)
{
    size_t colon = start;
    while (colon < end && buffer[colon] != ':')
    {
        colon++;
    }

    if (colon == start || colon == end)
    {
        return -1;
    }

    size_t value_start = colon + 1;
    size_t value_end = end;
    trim_value(buffer, &value_start, &value_end);

    if (name_equals(buffer, start, colon, "host") == 1)
    {
        return parse_host(request, buffer, value_start, value_end);
    }

    if (name_equals(buffer, start, colon, "content-length") == 1)
    {
        return parse_content_length(request, buffer, value_start, value_end);
    }

    return 0;
}

static int find_headers_start(const char *buffer, size_t size, size_t *start)
{
    size_t line_end = 0;
    if (find_line_end(buffer, size, 0, &line_end) < 0)
    {
        return -1;
    }

    *start = line_end + 2;
    return 0;
}

int headers_request_parse(struct request_http *request, char *buffer,
                          size_t size)
{
    size_t start = 0;
    if (find_headers_start(buffer, size, &start) < 0)
    {
        return -1;
    }

    while (start + 1 < size)
    {
        if (buffer[start] == '\r' && buffer[start + 1] == '\n')
        {
            return 0;
        }

        size_t line_end = 0;
        if (find_line_end(buffer, size, start, &line_end) < 0)
        {
            return -1;
        }

        if (parse_header(request, buffer, start, line_end) < 0)
        {
            return -1;
        }

        start = line_end + 2;
    }

    return -1;
}
