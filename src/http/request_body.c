#include "request_body.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "http_structs.h"
#include "request_headers.h"
#include "request_line.h"

#define MAX_BODY_SIZE 1048576

static size_t copy_received_body(struct request_http *request,
                                 const char *buffer, size_t size,
                                 size_t body_start)
{
    size_t available = size - body_start;
    if (available > request->content_length)
    {
        available = request->content_length;
    }

    for (size_t index = 0; index < available; index++)
    {
        request->body[index] = buffer[body_start + index];
    }

    return available;
}

static int read_missing_body(int client_fd, struct request_http *request,
                             size_t received)
{
    while (received < request->content_length)
    {
        ssize_t bytes_read = read(client_fd, request->body + received,
                                  request->content_length - received);

        if (bytes_read < 0 && errno == EINTR)
        {
            continue;
        }

        if (bytes_read <= 0)
        {
            return -1;
        }

        received += bytes_read;
    }

    return 0;
}

static int receive_body(int client_fd, struct request_http *request,
                        char *buffer, size_t size)
{
    int body_start = find_end_header(buffer, size);
    if (body_start < 0)
    {
        return -1;
    }

    size_t received = copy_received_body(request, buffer, size, body_start);
    if (read_missing_body(client_fd, request, received) < 0)
    {
        free(request->body);
        request->body = NULL;
        return -1;
    }

    request->body[request->content_length] = '\0';
    return 0;
}

int complete_body(int client_fd, struct request_http *request, char *buffer,
                  size_t size)
{
    if (request->flag_content == 0 || request->content_length == 0)
    {
        return 0;
    }

    if (request->content_length > MAX_BODY_SIZE)
    {
        return -1;
    }

    request->body = malloc(request->content_length + 1);
    if (request->body == NULL)
    {
        return -1;
    }

    return receive_body(client_fd, request, buffer, size);
}

static int validate_request_line(struct request_http *request)
{
    if (strcmp(request->method, "GET") != 0
        && strcmp(request->method, "HEAD") != 0)
    {
        return 405;
    }

    if (strcmp(request->version, "HTTP/1.1") != 0)
    {
        return 505;
    }

    return 0;
}

int parse_httpd(struct request_http *request, char *buffer, size_t size,
                int client_fd)
{
    if (line_request_parse(request, buffer, size) < 0)
    {
        return 400;
    }

    int status = validate_request_line(request);
    if (status != 0)
    {
        return status;
    }

    if (headers_request_parse(request, buffer, size) < 0
        || request->host == NULL)
    {
        return 400;
    }

    if (complete_body(client_fd, request, buffer, size) < 0)
    {
        return 400;
    }

    return 0;
}
