#include "config.h"

#include <getopt.h>
#include <stdlib.h>

#include "../utils/string/string.h"
#include "config_utils.h"

static struct option long_options[] = {
    { "pid_file", required_argument, NULL, 'p' },
    { "log_file", required_argument, NULL, 'f' },
    { "log", required_argument, NULL, 'l' },
    { "daemon", required_argument, NULL, 'm' },
    { "server_name", required_argument, NULL, 's' },
    { "port", required_argument, NULL, 't' },
    { "ip", required_argument, NULL, 'i' },
    { "root_dir", required_argument, NULL, 'r' },
    { "default_file", required_argument, NULL, 'd' },
    { NULL, 0, NULL, 0 },
};

static int configuration_is_complete(struct config *config)
{
    if (config->pid_file == NULL || config->servers->server_name == NULL)
    {
        return 0;
    }

    if (config->servers->port == NULL || config->servers->ip == NULL
        || config->servers->root_dir == NULL)
    {
        return 0;
    }

    return 1;
}

static int set_default_log_file(struct config *config)
{
    if (config->daemon == NO_OPTION || config->log == false
        || config->log_file != NULL)
    {
        return 0;
    }

    config->log_file = my_strdup("HTTPd.log");
    if (config->log_file == NULL)
    {
        return -1;
    }

    return 0;
}

static struct config *configuration_error(struct config *config)
{
    config_destroy(config);
    return NULL;
}

static int parse_arguments(struct config *config, int argc, char *argv[])
{
    opterr = 0;
    optind = 1;
    return parse_option(config, long_options, argc, argv);
}

struct config *parse_configuration(int argc, char *argv[])
{
    struct config *config = init_struct_config();
    if (config == NULL)
    {
        return NULL;
    }

    if (parse_arguments(config, argc, argv) != 0)
    {
        return configuration_error(config);
    }

    if (config->daemon == STOP)
    {
        if (config->pid_file == NULL)
        {
            return configuration_error(config);
        }
        return config;
    }

    if (configuration_is_complete(config) == 0
        || set_default_log_file(config) < 0)
    {
        return configuration_error(config);
    }

    return config;
}

static void server_config_destroy(struct server_config *server)
{
    if (server == NULL)
    {
        return;
    }

    string_destroy(server->server_name);
    free(server->port);
    free(server->ip);
    free(server->root_dir);
    free(server->default_file);
    free(server);
}

void config_destroy(struct config *config)
{
    if (config == NULL)
    {
        return;
    }

    free(config->pid_file);
    free(config->log_file);
    server_config_destroy(config->servers);
    free(config);
}
