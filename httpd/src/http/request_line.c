#include "request_line.h"

#include <stdlib.h>
#include <unistd.h>

#include "http_structs.h"
#include "request_utils.h"

int line_request_parse(struct request_http *request, char *buffer, size_t size)
{
    size_t index = 0;

    size_t size_first_line = 0;
    while (size_first_line < size
           && !(buffer[size_first_line] == '\r'
                && buffer[size_first_line + 1] == '\n'))
    {
        size_first_line++;
    }
    char *line = malloc(sizeof(char) * (size_first_line + 1));
    if (line == NULL)
    {
        return -1;
    }
    for (size_t i = 0; i < size_first_line; i++)
    {
        line[i] = buffer[i];
    }
    line[size_first_line] = '\0';
    ssize_t first_space = -1;
    ssize_t second_space = -1;
    while (line[index] != '\0')
    {
        if (line[index] == ' ')
        {
            if (first_space == -1)
            {
                first_space = index;
            }
            else
            {
                second_space = index;
                break;
            }
        }
        index++;
    }
    if (first_space == -1 || second_space == -1)
    {
        free(line);
        return -1;
    }
    line[first_space] = '\0';
    line[second_space] = '\0';
    char *method = line;
    char *path = line + first_space + 1;
    char *version = line + second_space + 1;

    request->method = my_strdup2(method);
    if (request->method == NULL)
    {
        free(line);
        return -1;
    }

    request->path = my_strdup2(path);
    if (request->path == NULL)
    {
        free(request->method);
        free(line);
        return -1;
    }

    request->version = my_strdup2(version);
    if (request->version == NULL)
    {
        free(request->method);
        free(request->path);
        free(line);
        return -1;
    }

    free(line);
    return 0;
}

int find_end_header(char *buffer, size_t size)
{
    size_t index = 0;
    while (index < size)
    {
        if (index < 4)
        {
            index++;
            continue;
        }
        if (buffer[index - 1] == '\n' && buffer[index - 2] == '\r'
            && buffer[index - 3] == '\n' && buffer[index - 4] == '\r')
        {
            return index + 1;
        }
        index++;
    }
    return -1;
}
