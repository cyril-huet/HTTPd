#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#include "../config/config.h"
#include "../http/http_structs.h"

struct logger
{
    FILE *stream;
    int flag;
    char *server_name;
};

struct logger *init_logger(struct config *config);
void log_close(struct logger *logger);
void logger_request(struct logger *logger, struct request_http *request,
                    char *client_ip);
void logger_response(struct logger *logger, struct answer_http *answer,
                     struct request_http *request, char *client_ip);
#endif /* ! LOGGER_H*/
