#define _POSIX_C_SOURCE 200809L
#include "daemon.h"

#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "daemon_utils.h"

int daemon_start(struct config *config)
{
    int pid = fork();
    if (pid < 0)
    {
        return -1;
    }
    if (pid > 0)
    {
        exit(0);
    }
    if (write_pid(config->pid_file) < 0)
    {
        return -1;
    }
    return 0;
}

int daemon_stop(struct config *config)
{
    int pid = read_pid(config->pid_file);
    if (pid <= 0)
    {
        return 0;
    }
    kill(pid, SIGINT);
    int fd = open(config->pid_file, O_WRONLY | O_TRUNC);
    if (fd >= 0)
    {
        close(fd);
    }
    return 0;
}

int daemon_restart(struct config *config)
{
    daemon_stop(config);
    int res = daemon_start(config);
    return res;
}
