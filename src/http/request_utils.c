#include "request_utils.h"

#include <stdlib.h>
#include <string.h>

#include "../config/config.h"
#include "../utils/string/string.h"
#include "http_structs.h"

char *my_strdup2(const char *str)
{
    size_t len = strlen(str);
    char *res = malloc(sizeof(char) * (len + 1));
    if (res == NULL)
        return NULL;
    memcpy(res, str, len);
    res[len] = '\0';
    return res;
}

int compare_servername(char *parcours, struct string *name_server)
{
    size_t index = 0;
    while (parcours[index] != '\0')
    {
        index++;
    }
    if (index != name_server->size)
    {
        return 0;
    }
    for (size_t i = 0; i < index; i++)
    {
        if (parcours[i] != name_server->data[i])
        {
            return 0;
        }
    }
    return 1;
}

int compare_ip(char *host, char *ip, char *port)
{
    size_t len = 0;
    len += strlen(ip);
    len += 1;
    len += strlen(port);
    char *compare = malloc(sizeof(char) * (len + 1));
    if (compare == NULL)
    {
        return -1;
    }
    size_t index = 0;
    while (index < strlen(ip))
    {
        compare[index] = ip[index];
        index++;
    }
    compare[index] = ':';
    index++;
    while (index < strlen(ip) + 1 + strlen(port))
    {
        compare[index] = port[index - strlen(ip) - 1];
        index++;
    }
    compare[index] = '\0';

    if (index != strlen(host))
    {
        free(compare);
        return -1;
    }
    int res = strcmp(compare, host);
    free(compare);
    return res;
}

int valide_request(struct request_http *request, struct config *config)
{
    if (request->host == NULL)
    {
        return 0;
    }

    char *parcours = request->host;
    struct string *name_server = config->servers->server_name;
    if (compare_servername(parcours, name_server))
    {
        return 1;
    }

    if (strcmp(parcours, config->servers->ip) == 0)
        return 1;

    if (compare_ip(parcours, config->servers->ip, config->servers->port) == 0)
        return 1;

    return 0;
}
