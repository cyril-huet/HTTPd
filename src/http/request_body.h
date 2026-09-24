#ifndef REQUEST_BODY_H
#define REQUEST_BODY_H

#include "http_structs.h"

int complete_body(int client_fd, struct request_http *request, char *buffer,
                  size_t size);

int parse_httpd(struct request_http *request, char *buffer, size_t size,
                int client_fd);

#endif /* ! REQUEST_BODY_H */
