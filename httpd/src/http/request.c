#include "request.h"

#include <stdlib.h>

#include "http_structs.h"
struct request_http *init_request(void)
{
    struct request_http *res = malloc(sizeof(struct request_http));
    if (res == NULL)
    {
        return NULL;
    }
    res->method = NULL;
    res->path = NULL;
    res->version = NULL;
    res->host = NULL;
    res->content_length = 0;
    res->flag_content = 0;
    res->body = NULL;
    return res;
}
void free_request(struct request_http *request)
{
    if (request == NULL)
    {
        return;
    }
    free(request->method);
    free(request->path);
    free(request->version);
    free(request->host);
    free(request->body);
    free(request);
}
