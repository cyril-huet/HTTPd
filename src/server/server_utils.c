#define _POSIX_C_SOURCE 200809L
#include "server_utils.h"

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

static int headers_are_complete(const char *buffer, size_t size)
{
    if (size < 4)
    {
        return 0;
    }

    for (size_t index = 3; index < size; index++)
    {
        if (buffer[index - 3] == '\r' && buffer[index - 2] == '\n'
            && buffer[index - 1] == '\r' && buffer[index] == '\n')
        {
            return 1;
        }
    }

    return 0;
}

int read_request(int client_fd, char *buffer, size_t capacity)
{
    size_t total = 0;

    while (total < capacity)
    {
        ssize_t bytes_read = read(client_fd, buffer + total, capacity - total);

        if (bytes_read < 0 && errno == EINTR)
        {
            continue;
        }

        if (bytes_read <= 0)
        {
            return -1;
        }

        total += bytes_read;

        if (headers_are_complete(buffer, total) == 1)
        {
            return total;
        }
    }

    return -1;
}
