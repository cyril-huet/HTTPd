#ifndef REQUEST_UTILS_H
#define REQUEST_UTILS_H

#include "../config/config.h"
#include "../utils/string/string.h"
#include "http_structs.h"

char *my_strdup2(const char *str);

int compare_servername(char *parcours, struct string *name_server);

int compare_ip(char *host, char *ip, char *port);

int valide_request(struct request_http *request, struct config *config);

#endif /* ! REQUEST_UTILS_H */
