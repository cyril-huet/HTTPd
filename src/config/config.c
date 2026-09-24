#include "config.h"

#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/string/string.h"
#include "config_utils.h"

struct config *parse_configuration(int argc, char *argv[])
{
    struct config *res = init_struct_config();
    if (res == NULL)
    {
        return NULL;
    }
    struct option longopts[] = {
        { "pid_file", required_argument, NULL, 'p' },
        { "log_file", required_argument, NULL, 'f' },
        { "log", required_argument, NULL, 'l' },
        { "daemon", required_argument, NULL, 'm' },
        { "server_name", required_argument, NULL, 's' },
        { "port", required_argument, NULL, 't' },
        { "ip", required_argument, NULL, 'i' },
        { "root_dir", required_argument, NULL, 'r' },
        { "default_file", required_argument, NULL, 'd' },
        { NULL, 0, NULL, 0 }

    };
    if (parse_option(res, longopts, argc, argv) == 1)
    {
        config_destroy(res);
        return NULL;
    }
    if (res->daemon != NO_OPTION && res->log_file == NULL)
    {
        res->log_file = my_strdup("HTTPd.log");
        if (res->log_file == NULL)
        {
            config_destroy(res);
            return NULL;
        }
    }
    if (res->daemon == STOP)
    {
        if (res->pid_file == NULL)
        {
            config_destroy(res);

            return NULL;
        }
        return res;
    }

    if (res->pid_file == NULL || res->servers->server_name == NULL
        || res->servers->port == NULL || res->servers->ip == NULL
        || res->servers->root_dir == NULL)
    {
        config_destroy(res);
        return NULL;
    }
    return res;
}
void config_destroy(struct config *config)
{
    if (config == NULL)
    {
        return;
    }
    free(config->pid_file);
    free(config->log_file);
    if (config->servers != NULL)
    {
        string_destroy(config->servers->server_name);
        free(config->servers->port);
        free(config->servers->ip);
        free(config->servers->root_dir);
        free(config->servers->default_file);
        free(config->servers);
    }
    free(config);
}
