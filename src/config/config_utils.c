#include "config_utils.h"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/string/string.h"

char *my_strdup(char *string)
{
    if (string == NULL)
    {
        return NULL;
    }

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

static struct server_config *init_server_config(void)
{
    struct server_config *server = malloc(sizeof(struct server_config));
    if (server == NULL)
    {
        return NULL;
    }

    server->server_name = NULL;
    server->port = NULL;
    server->ip = NULL;
    server->root_dir = NULL;
    server->default_file = my_strdup("index.html");
    if (server->default_file == NULL)
    {
        free(server);
        return NULL;
    }

    return server;
}

struct config *init_struct_config(void)
{
    struct config *config = malloc(sizeof(struct config));
    if (config == NULL)
    {
        return NULL;
    }

    config->pid_file = NULL;
    config->log_file = NULL;
    config->log = true;
    config->daemon = NO_OPTION;
    config->servers = init_server_config();
    if (config->servers == NULL)
    {
        free(config);
        return NULL;
    }

    return config;
}

static enum daemon parse_daemon_value(char *value)
{
    if (strcmp(value, "start") == 0)
    {
        return START;
    }

    if (strcmp(value, "stop") == 0)
    {
        return STOP;
    }

    if (strcmp(value, "restart") == 0)
    {
        return RESTART;
    }

    return NO_OPTION;
}

static char **find_string_target(struct config *config, int option)
{
    switch (option)
    {
    case 'p':
        return &config->pid_file;
    case 'f':
        return &config->log_file;
    case 't':
        return &config->servers->port;
    case 'i':
        return &config->servers->ip;
    case 'r':
        return &config->servers->root_dir;
    case 'd':
        return &config->servers->default_file;
    default:
        return NULL;
    }
}

static int replace_string(char **target, char *value)
{
    if (value == NULL || value[0] == '\0')
    {
        return -1;
    }

    char *copy = my_strdup(value);
    if (copy == NULL)
    {
        return -1;
    }

    free(*target);
    *target = copy;
    return 0;
}

static int valid_port(char *value)
{
    if (value == NULL || value[0] == '\0')
    {
        return 0;
    }

    int port = 0;
    for (size_t index = 0; value[index] != '\0'; index++)
    {
        if (value[index] < '0' || value[index] > '9')
        {
            return 0;
        }

        port = port * 10 + value[index] - '0';
        if (port > 65535)
        {
            return 0;
        }
    }

    return port > 0;
}

static int replace_server_name(struct config *config, char *value)
{
    if (value == NULL || value[0] == '\0')
    {
        return -1;
    }

    struct string *name = string_create(value, strlen(value));
    if (name == NULL)
    {
        return -1;
    }

    string_destroy(config->servers->server_name);
    config->servers->server_name = name;
    return 0;
}

static int set_log_value(struct config *config, char *value)
{
    if (strcmp(value, "true") == 0)
    {
        config->log = true;
        return 0;
    }

    if (strcmp(value, "false") == 0)
    {
        config->log = false;
        return 0;
    }

    return -1;
}

static int set_daemon_value(struct config *config, char *value)
{
    enum daemon daemon_value = parse_daemon_value(value);
    if (daemon_value == NO_OPTION)
    {
        return -1;
    }

    config->daemon = daemon_value;
    return 0;
}

static int set_option(struct config *config, int option, char *value)
{
    char **target = find_string_target(config, option);
    if (target != NULL)
    {
        if (option == 't' && valid_port(value) == 0)
        {
            return -1;
        }
        return replace_string(target, value);
    }

    if (option == 'l')
    {
        return set_log_value(config, value);
    }

    if (option == 'm')
    {
        return set_daemon_value(config, value);
    }

    if (option == 's')
    {
        return replace_server_name(config, value);
    }

    return -1;
}

int parse_option(struct config *config, struct option *longopts, int argc,
                 char *argv[])
{
    int option = 0;
    while ((option = getopt_long(argc, argv, "", longopts, NULL)) != -1)
    {
        if (set_option(config, option, optarg) < 0)
        {
            return 1;
        }
    }

    if (optind < argc)
    {
        return 1;
    }

    return 0;
}
