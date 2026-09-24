#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "config/config.h"
#include "daemon/daemon.h"
#include "server/server.h"

/* The signal interrupts accept(), which lets the server stop cleanly. */
static void handle_interrupt(int signal_number)
{
    if (signal_number != SIGINT)
    {
        return;
    }
}

static int install_signal_handler(void)
{
    struct sigaction action;

    action.sa_handler = handle_interrupt;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    return sigaction(SIGINT, &action, NULL);
}

static void close_standard_streams(void)
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

static int start_daemon(struct config *config)
{
    int status = 0;

    if (config->daemon == START)
    {
        status = daemon_start(config);
    }
    else if (config->daemon == RESTART)
    {
        status = daemon_restart(config);
    }

    if (status < 0)
    {
        return -1;
    }

    if (config->daemon == START || config->daemon == RESTART)
    {
        close_standard_streams();
    }

    return 0;
}

static int stop_daemon(struct config *config)
{
    int status = daemon_stop(config);
    config_destroy(config);

    if (status < 0)
    {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct config *config = parse_configuration(argc, argv);
    if (config == NULL)
    {
        return 2;
    }

    if (config->daemon == STOP)
    {
        return stop_daemon(config);
    }

    if (start_daemon(config) < 0 || install_signal_handler() < 0)
    {
        config_destroy(config);
        return 1;
    }

    int server_socket = init_server(config->servers->ip, config->servers->port);
    if (server_socket < 0)
    {
        config_destroy(config);
        return 1;
    }

    printf("Server started on http://%s:%s\n", config->servers->ip,
           config->servers->port);
    run_server(server_socket, config);

    close(server_socket);
    config_destroy(config);
    return 0;
}
