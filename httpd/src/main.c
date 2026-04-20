#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config/config.h"
#include "daemon/daemon.h"
#include "logger/logger.h"
#include "server/server.h"

static void auxi(int a)
{
    if (a)
    {
    }
}

int main(int argc, char *argv[])
{
    struct config *config = parse_configuration(argc, argv);
    if (config == NULL)
    {
        return 2;
    }
    if (config->daemon == START)
    {
        int temps = daemon_start(config);
        if (temps < 0)
        {
            return 1;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    else if (config->daemon == STOP)
    {
        int temps = daemon_stop(config);
        config_destroy(config);
        if (temps < 0)
        {
            return 1;
        }
        return 0;
    }
    else if (config->daemon == RESTART)
    {
        int temps = daemon_restart(config);
        if (temps < 0)
        {
            return 1;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    struct sigaction sigactio;
    sigactio.sa_handler = auxi;
    sigemptyset(&sigactio.sa_mask);
    sigactio.sa_flags = 0;
    sigaction(SIGINT, &sigactio, NULL);
    struct logger *logger = init_logger(config);
    if (logger == NULL && config->log == true)
    {
        config_destroy(config);
        return 1;
    }
    int sockfd = init_server(config->servers->ip, config->servers->port);
    if (sockfd < 0)
    {
        log_close(logger);
        config_destroy(config);
        return 1;
    }
    printf("Serveur lance sur http://%s:%s\n", config->servers->ip,
           config->servers->port);
    run_server(sockfd, config);
    close(sockfd);
    log_close(logger);
    config_destroy(config);
    return 0;
}
