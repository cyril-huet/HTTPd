#define _POSIX_C_SOURCE 200809L
#include "daemon_utils.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int int_len(int a)
{
    if (a == 0)
    {
        return 1;
    }
    int res = 0;
    while (a > 0)
    {
        res++;
        a /= 10;
    }
    return res;
}

int write_pid(char *file)
{
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        return -1;
    }
    int pid = getpid();
    int taille = int_len(pid);
    char *str_pid = malloc(sizeof(char) * (taille + 2));
    if (str_pid == NULL)
    {
        close(fd);
        return -1;
    }
    for (int i = 0; i < taille; i++)
    {
        char temps = pid % 10 + '0';
        str_pid[taille - i - 1] = temps;
        pid = pid / 10;
    }
    str_pid[taille] = '\n';
    str_pid[taille + 1] = '\0';

    if (write(fd, str_pid, taille + 1) < 0)
    {
    }
    close(fd);
    free(str_pid);
    return 0;
}
int file_size(char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }

    int res = 0;
    char c;
    while (read(fd, &c, 1) == 1)
    {
        res++;
    }

    close(fd);
    return res;
}
int read_pid(char *file)
{
    int size = file_size(file);
    if (size <= 0)
    {
        return -1;
    }
    int fd = open(file, O_RDONLY);
    if (fd < 0)
    {
        return -1;
    }

    char *res = malloc(sizeof(char) * (size + 1));
    if (res == NULL)
    {
        close(fd);
        return -1;
    }
    int r = read(fd, res, size);
    if (r <= 0)
    {
        free(res);
        close(fd);
        return -1;
    }
    close(fd);
    int res_pid = 0;
    for (int i = 0; i < size; i++)
    {
        if (res[i] >= '0' && res[i] <= '9')
        {
            res_pid = res_pid * 10 + (res[i] - '0');
        }
    }
    free(res);
    return res_pid;
}
