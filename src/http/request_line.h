#ifndef REQUEST_LINE_H
#define REQUEST_LINE_H

#include "http_structs.h"

int line_request_parse(struct request_http *request, char *buffer, size_t size);

int find_end_header(char *buffer, size_t size);

#endif /* ! REQUEST_LINE_H */
