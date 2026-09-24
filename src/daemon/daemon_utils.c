#define _POSIX_C_SOURCE 200809L
#include "daemon_utils.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

static int write_all(int file, const char *buffer, size_t size)
{
    size_t written_total = 0;

    while (written_total < size)
    {
        ssize_t written =
            write(file, buffer + written_total, size - written_total);
        if (written < 0 && errno == EINTR)
        {
            continue;
        }

        if (written <= 0)
        {
            return -1;
        }

        written_total += written;
    }

    return 0;
}

int write_pid(char *file)
{
    if (file == NULL)
    {
        return -1;
    }

    int descriptor = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (descriptor < 0)
    {
        return -1;
    }

    char content[32];
    int length = snprintf(content, sizeof(content), "%d\n", getpid());
    if (length < 0 || length >= 32
        || write_all(descriptor, content, length) < 0)
    {
        close(descriptor);
        return -1;
    }

    close(descriptor);
    return 0;
}

static int read_pid_content(int descriptor, char *content, size_t capacity)
{
    size_t length = 0;
    while (length + 1 < capacity)
    {
        ssize_t bytes_read = read(descriptor, content + length, 1);
        if (bytes_read < 0 && errno == EINTR)
        {
            continue;
        }

        if (bytes_read < 0)
        {
            return -1;
        }

        if (bytes_read == 0 || content[length] == '\n')
        {
            break;
        }

        length++;
    }

    content[length] = '\0';
    if (length == 0)
    {
        return -1;
    }

    return 0;
}

static int parse_pid_content(const char *content)
{
    int pid = 0;
    for (size_t index = 0; content[index] != '\0'; index++)
    {
        if (content[index] < '0' || content[index] > '9')
        {
            return -1;
        }

        int digit = content[index] - '0';
        if (pid > (INT_MAX - digit) / 10)
        {
            return -1;
        }

        pid = pid * 10 + digit;
    }

    if (pid == 0)
    {
        return -1;
    }

    return pid;
}

int read_pid(char *file)
{
    if (file == NULL)
    {
        return -1;
    }

    int descriptor = open(file, O_RDONLY);
    if (descriptor < 0)
    {
        return -1;
    }

    char content[32];
    int status = read_pid_content(descriptor, content, sizeof(content));
    close(descriptor);
    if (status < 0)
    {
        return -1;
    }

    return parse_pid_content(content);
}
