#ifndef REQUEST_H
#define REQUEST_H

#include "http_structs.h"

struct request_http *init_request(void);

void free_request(struct request_http *req);

#endif /* ! REQUEST_H */
