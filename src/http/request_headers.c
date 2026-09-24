#include "request_headers.h"

#include <stdlib.h>
#include <string.h>

#include "http_structs.h"

static size_t find_line_end(char *buffer, size_t size, size_t pos)
{
    while (pos + 1 < size && !(buffer[pos] == '\r' && buffer[pos + 1] == '\n'))
    {
        pos++;
    }
    return pos;
}

static char *extract_name(char *buffer, size_t start, size_t before_colon)
{
    char *name = malloc(sizeof(char) * (before_colon + 1));
    if (name == NULL)
    {
        return NULL;
    }

    size_t index = 0;
    while (index < before_colon)
    {
        name[index] = buffer[start + index];
        index++;
    }
    name[index] = '\0';
    return name;
}

static char *extract_value(char *buffer, size_t size, size_t start_value,
                           size_t *size_res)
{
    if (buffer[start_value] == ' ')
    {
        start_value++;
    }

    *size_res = 0;
    while (start_value + *size_res < size
           && buffer[start_value + *size_res] != '\r')
    {
        (*size_res)++;
    }

    char *res = malloc(sizeof(char) * (*size_res + 1));
    if (res == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < *size_res; i++)
    {
        res[i] = buffer[start_value + i];
    }

    res[*size_res] = '\0';
    return res;
}

static int parse_host(struct request_http *request, char *buffer, size_t size,
                      size_t start_value)
{
    size_t size_res = 0;

    char *res_host = extract_value(buffer, size, start_value, &size_res);
    if (res_host == NULL)
    {
        return -1;
    }

    request->host = res_host;

    return 0;
}

static int parse_content_length(struct request_http *request, char *buffer,
                                size_t size, size_t start_value)
{
    size_t size_res = 0;

    char *res_content = extract_value(buffer, size, start_value, &size_res);
    if (res_content == NULL)
    {
        return -1;
    }

    size_t size_content_length = 0;

    for (size_t i = 0; i < size_res; i++)
    {
        if (res_content[i] < '0' || res_content[i] > '9')
        {
            continue;
        }

        size_content_length = size_content_length * 10 + (res_content[i] - '0');
    }

    request->content_length = size_content_length;
    request->flag_content = 1;

    free(res_content);
    return 0;
}

int headers_request_parse(struct request_http *request, char *buffer,
                          size_t size)
{
    size_t start = 0;

    while (start + 1 < size
           && !(buffer[start] == '\r' && buffer[start + 1] == '\n'))
    {
        start++;
    }
    start += 2;

    int found = 0;

    while (start < size && found != 2)
    {
        size_t before_colon = 0;

        while (start + before_colon < size
               && buffer[start + before_colon] != ':'
               && buffer[start + before_colon] != '\r')
        {
            before_colon++;
        }

        if (buffer[start] == '\r' && buffer[start + 1] == '\n')
        {
            break;
        }

        char *name = extract_name(buffer, start, before_colon);
        if (name == NULL)
        {
            return -1;
        }

        if (strcmp(name, "Host") == 0)
        {
            size_t start_value = start + before_colon + 1;
            if (buffer[start_value] == ' ')
            {
                start_value++;
            }

            if (parse_host(request, buffer, size, start_value) == -1)
            {
                free(name);
                return -1;
            }

            found++;
        }

        if (strcmp(name, "Content-Length") == 0)
        {
            size_t start_value = start + before_colon + 1;
            if (buffer[start_value] == ' ')
            {
                start_value++;
            }

            if (parse_content_length(request, buffer, size, start_value) == -1)
            {
                free(name);
                return -1;
            }

            found++;
        }

        free(name);

        size_t temps = find_line_end(buffer, size, start);
        start = temps + 2;
    }

    return 0;
}
