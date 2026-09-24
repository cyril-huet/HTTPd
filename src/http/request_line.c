#include "request_line.h"

#include <stdlib.h>

#include "http_structs.h"

struct line_parts
{
    size_t line_end;
    size_t first_space;
    size_t second_space;
};

static int find_first_line_end(const char *buffer, size_t size,
                               size_t *line_end)
{
    size_t index = 0;

    while (index + 1 < size)
    {
        if (buffer[index] == '\r' && buffer[index + 1] == '\n')
        {
            *line_end = index;
            return 0;
        }

        index++;
    }

    return -1;
}

static int find_spaces(const char *buffer, struct line_parts *parts)
{
    parts->first_space = parts->line_end;
    parts->second_space = parts->line_end;

    for (size_t index = 0; index < parts->line_end; index++)
    {
        if (buffer[index] != ' ')
        {
            continue;
        }

        if (parts->first_space == parts->line_end)
        {
            parts->first_space = index;
        }
        else if (parts->second_space == parts->line_end)
        {
            parts->second_space = index;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}

static int valid_parts(struct line_parts *parts)
{
    if (parts->first_space == 0 || parts->first_space == parts->line_end)
    {
        return 0;
    }

    if (parts->second_space == parts->line_end
        || parts->second_space == parts->first_space + 1)
    {
        return 0;
    }

    if (parts->second_space + 1 >= parts->line_end)
    {
        return 0;
    }

    return 1;
}

static char *copy_part(const char *buffer, size_t start, size_t length)
{
    char *result = malloc(length + 1);

    if (result == NULL)
    {
        return NULL;
    }

    for (size_t index = 0; index < length; index++)
    {
        result[index] = buffer[start + index];
    }

    result[length] = '\0';

    return result;
}

static int save_request_parts(struct request_http *request, const char *buffer,
                              struct line_parts *parts)
{
    char *method = copy_part(buffer, 0, parts->first_space);

    char *path = copy_part(buffer, parts->first_space + 1,
                           parts->second_space - parts->first_space - 1);

    char *version = copy_part(buffer, parts->second_space + 1,
                              parts->line_end - parts->second_space - 1);

    if (method == NULL || path == NULL || version == NULL)
    {
        free(method);
        free(path);
        free(version);

        return -1;
    }

    request->method = method;
    request->path = path;
    request->version = version;

    return 0;
}

int line_request_parse(struct request_http *request, char *buffer, size_t size)
{
    struct line_parts parts;

    if (find_first_line_end(buffer, size, &parts.line_end) < 0)
    {
        return -1;
    }

    if (find_spaces(buffer, &parts) < 0)
    {
        return -1;
    }

    if (valid_parts(&parts) == 0)
    {
        return -1;
    }

    return save_request_parts(request, buffer, &parts);
}

int find_end_header(char *buffer, size_t size)
{
    for (size_t index = 0; index + 3 < size; index++)
    {
        if (buffer[index] == '\r' && buffer[index + 1] == '\n'
            && buffer[index + 2] == '\r' && buffer[index + 3] == '\n')
        {
            return index + 4;
        }
    }

    return -1;
}
