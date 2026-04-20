#include "request_body.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "http_structs.h"
#include "request_headers.h"
#include "request_line.h"

int complete_body(int client_fd, struct request_http *request, char *buffer,
                  size_t size)
{
    int end_header = find_end_header(buffer, size);
    if (end_header == -1)
    {
        return -1;
    }
    if (request->flag_content == 0 || request->content_length == 0)
    {
        return 0;
    }
    char *res = malloc(sizeof(char) * request->content_length);
    if (res == NULL)
    {
        return -1;
    }
    size_t index = 0;
    while (index < request->content_length)
    {
        int temps =
            read(client_fd, res + index, request->content_length - index);
        if (temps <= 0)
        {
            free(res);
            return -1;
        }
        index += temps;
    }

    request->body = res;
    return 0;
}
int parse_httpd(struct request_http *request, char *buffer, size_t size,
                int client_fd)
{
    int a = line_request_parse(request, buffer, size);
    if (a < 0)
    {
        return 400;
    }
    if (strcmp(request->method, "GET") != 0
        && strcmp(request->method, "HEAD") != 0)
    {
        return 405;
    }
    if (strcmp(request->version, "HTTP/1.1") != 0)
    {
        return 505;
    }
    int b = headers_request_parse(request, buffer, size);
    if (b < 0)
    {
        return 400;
    }
    if (request->host == NULL)
    {
        return 400;
    }

    if (complete_body(client_fd, request, buffer, size) < 0)
    {
        return 400;
    }
    return 0;
}
