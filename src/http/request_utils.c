#include "request_utils.h"

#include <stdlib.h>
#include <string.h>

#include "../config/config.h"
#include "../utils/string/string.h"
#include "http_structs.h"

static char lowercase(char character)
{
    if (character >= 'A' && character <= 'Z')
    {
        return character + ('a' - 'A');
    }

    return character;
}

char *my_strdup2(const char *string)
{
    size_t length = strlen(string);
    char *copy = malloc(length + 1);
    if (copy == NULL)
    {
        return NULL;
    }

    for (size_t index = 0; index < length; index++)
    {
        copy[index] = string[index];
    }

    copy[length] = '\0';
    return copy;
}

static int server_name_matches(const char *host, struct string *server_name)
{
    for (size_t index = 0; index < server_name->size; index++)
    {
        if (lowercase(host[index]) != lowercase(server_name->data[index]))
        {
            return 0;
        }
    }

    return 1;
}

int compare_servername(char *host, struct string *server_name)
{
    if (strlen(host) != server_name->size)
    {
        return 0;
    }

    return server_name_matches(host, server_name);
}

static int text_matches(const char *host, size_t start, const char *expected)
{
    size_t index = 0;
    while (expected[index] != '\0')
    {
        if (host[start + index] != expected[index])
        {
            return 0;
        }

        index++;
    }

    return 1;
}

int compare_ip(char *host, char *ip, char *port)
{
    size_t ip_length = strlen(ip);
    size_t port_length = strlen(port);
    size_t expected_length = ip_length + port_length + 1;

    if (strlen(host) != expected_length)
    {
        return -1;
    }

    if (text_matches(host, 0, ip) == 0 || host[ip_length] != ':')
    {
        return -1;
    }

    if (text_matches(host, ip_length + 1, port) == 0)
    {
        return -1;
    }

    return 0;
}

static int compare_servername_with_port(const char *host,
                                        struct string *server_name,
                                        const char *port)
{
    size_t port_length = strlen(port);
    size_t expected_length = server_name->size + port_length + 1;

    if (strlen(host) != expected_length)
    {
        return 0;
    }

    if (server_name_matches(host, server_name) == 0
        || host[server_name->size] != ':')
    {
        return 0;
    }

    return text_matches(host, server_name->size + 1, port);
}

int valide_request(struct request_http *request, struct config *config)
{
    if (request == NULL || request->host == NULL || config == NULL
        || config->servers == NULL)
    {
        return 0;
    }

    char *host = request->host;
    struct server_config *server = config->servers;

    if (compare_servername(host, server->server_name) == 1
        || compare_servername_with_port(host, server->server_name, server->port)
            == 1)
    {
        return 1;
    }

    if (strcmp(host, server->ip) == 0
        || compare_ip(host, server->ip, server->port) == 0)
    {
        return 1;
    }

    return 0;
}
