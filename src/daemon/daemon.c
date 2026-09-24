#define _POSIX_C_SOURCE 200809L
#include "daemon.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "daemon_utils.h"

static int process_is_running(int pid)
{
    if (kill(pid, 0) == 0)
    {
        return 1;
    }

    if (errno == EPERM)
    {
        return 1;
    }

    return 0;
}

static int clear_pid_file(char *file)
{
    int descriptor = open(file, O_WRONLY | O_TRUNC);
    if (descriptor < 0)
    {
        return -1;
    }

    close(descriptor);
    return 0;
}

int daemon_start(struct config *config)
{
    if (config == NULL || config->pid_file == NULL)
    {
        return -1;
    }

    int old_pid = read_pid(config->pid_file);
    if (old_pid > 0 && process_is_running(old_pid) == 1)
    {
        return -1;
    }

    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }

    if (pid > 0)
    {
        _exit(0);
    }

    return write_pid(config->pid_file);
}

int daemon_stop(struct config *config)
{
    if (config == NULL || config->pid_file == NULL)
    {
        return -1;
    }

    int pid = read_pid(config->pid_file);
    if (pid <= 0)
    {
        return 0;
    }

    if (kill(pid, SIGINT) < 0 && errno != ESRCH)
    {
        return -1;
    }

    return clear_pid_file(config->pid_file);
}

int daemon_restart(struct config *config)
{
    if (daemon_stop(config) < 0)
    {
        return -1;
    }

    return daemon_start(config);
}
