#ifndef REQUEST_HEADERS_H
#define REQUEST_HEADERS_H

#include "http_structs.h"

int headers_request_parse(struct request_http *request, char *buffer,
                          size_t size);

#endif /* ! REQUEST_HEADERS_H */
