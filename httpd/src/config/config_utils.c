#include "config_utils.h"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/string/string.h"

char *my_strdup(char *str)
{
    size_t len = strlen(str);
    char *res = malloc(sizeof(char) * (len + 1));
    if (res == NULL)
        return NULL;
    memcpy(res, str, len);
    res[len] = '\0';
    return res;
}

struct config *init_struct_config(void)
{
    struct config *res = malloc(sizeof(struct config));
    if (res == NULL)
    {
        return NULL;
    }

    res->pid_file = NULL;
    res->log_file = NULL;
    res->log = true;
    res->daemon = NO_OPTION;

    res->servers = malloc(sizeof(struct server_config));
    if (res->servers == NULL)
    {
        free(res);
        return NULL;
    }

    res->servers->server_name = NULL;
    res->servers->port = NULL;
    res->servers->ip = NULL;
    res->servers->root_dir = NULL;
    res->servers->default_file = my_strdup("index.html");
    if (res->servers->default_file == NULL)
    {
        free(res->servers);
        free(res);
        return NULL;
    }
    return res;
}

static enum daemon enum_daemon(char *str)
{
    if (strcmp(str, "start") == 0)
    {
        return START;
    }
    if (strcmp(str, "stop") == 0)
    {
        return STOP;
    }
    if (strcmp(str, "restart") == 0)
    {
        return RESTART;
    }
    return NO_OPTION;
}
int parse_option(struct config *res, struct option *longopts, int argc,
                 char *argv[])
{
    int opt;
    while ((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1)
    {
        switch (opt)
        {
        case 'p':
            res->pid_file = my_strdup(optarg);
            break;
        case 'f':
            res->log_file = my_strdup(optarg);
            break;
        case 'l':
            if (strcmp(optarg, "true") == 0)
            {
                res->log = true;
            }
            else
            {
                res->log = false;
            }
            break;
        case 'm':
            res->daemon = enum_daemon(optarg);
            if (res->daemon == NO_OPTION)
            {
                return 1;
            }
            break;
        case 's':
            res->servers->server_name = string_create(optarg, strlen(optarg));
            break;
        case 't':
            res->servers->port = my_strdup(optarg);
            break;
        case 'i':
            res->servers->ip = my_strdup(optarg);
            break;
        case 'r':
            res->servers->root_dir = my_strdup(optarg);
            break;
        case 'd':
            free(res->servers->default_file);
            res->servers->default_file = my_strdup(optarg);
            break;
        default:
            return 1;
        }
    }
    return 0;
}
