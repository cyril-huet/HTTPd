
#define _POSIX_C_SOURCE 200809L
#include "server_utils.h"

#include <stddef.h>
#include <unistd.h>

int read_request(int client_fd, char *buffer, size_t size)
{
    size_t index = 0;
    while (index < size)
    {
        int temps = read(client_fd, buffer + index, size - index);
        if (temps <= 0)
        {
            return -1;
        }
        for (int i = 0; i < temps; i++)
        {
            index++;
        }
        if (index < 4)
        {
            continue;
        }
        if (buffer[index - 1] == '\n' && buffer[index - 2] == '\r'
            && buffer[index - 3] == '\n' && buffer[index - 4] == '\r')
        {
            return index;
        }
    }
    return index;
}
