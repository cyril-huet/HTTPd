#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stddef.h>

int read_request(int client_fd, char *buffer, size_t size);

#endif /* ! SERVER_UTILS_H */
