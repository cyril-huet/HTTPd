#define _POSIX_C_SOURCE 200809L
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "logger_utils.h"

struct logger *init_logger(struct config *config)
{
    struct logger *res = malloc(sizeof(struct logger));
    if (res == NULL)
    {
        return NULL;
    }

    res->server_name = to_string(config->servers->server_name);
    if (res->server_name == NULL)
    {
        free(res);
        return NULL;
    }
    if (config->log == false)
    {
        res->flag = 0;
        res->stream = NULL;
        return res;
    }

    if (config->log_file != NULL)
    {
        res->stream = fopen(config->log_file, "a");
        if (res->stream == NULL)
        {
            free(res->server_name);
            free(res);
            return NULL;
        }
        res->flag = 1;
        return res;
    }
    res->flag = 1;
    res->stream = stdout;
    return res;
}

void log_close(struct logger *logger)
{
    if (logger == NULL)
    {
        return;
    }

    if (logger->stream != NULL && logger->stream != stdout)
    {
        fclose(logger->stream);
    }
    free(logger->server_name);

    free(logger);
}

void logger_request(struct logger *logger, struct request_http *request,
                    char *client_ip)
{
    if (logger == NULL || logger->flag == 0 || logger->stream == NULL)
    {
        return;
    }

    char buffer[512];
    size_t len = 0;

    char *dates = date_log();
    if (dates == NULL)
    {
        return;
    }

    len += append(buffer + len, dates);
    len += append(buffer + len, " [");
    len += append(buffer + len, logger->server_name);
    len += append(buffer + len, "] received ");

    if (request == NULL || request->method == NULL || request->path == NULL)
    {
        len += append(buffer + len, "Bad Request from ");
        len += append(buffer + len, client_ip);
    }
    else
    {
        len += append(buffer + len, request->method);
        len += append(buffer + len, " on '");
        len += append(buffer + len, request->path);
        len += append(buffer + len, "' from ");
        len += append(buffer + len, client_ip);
    }

    buffer[len] = '\n';
    len++;

    int fd = fileno(logger->stream);
    if (write(fd, buffer, len) < 0)
    {
    }
    free(dates);
}

void logger_response(struct logger *logger, struct answer_http *answer,
                     struct request_http *request, char *client_ip)
{
    if (logger == NULL || logger->flag == 0 || logger->stream == NULL)
    {
        return;
    }
    char buffer[512];
    size_t len = 0;
    char *dates = date_log();
    if (dates == NULL)
    {
        return;
    }
    len += append(buffer + len, dates);
    len += append(buffer + len, " [");
    len += append(buffer + len, logger->server_name);
    len += append(buffer + len, "] responding with ");
    int status = answer->status_code;
    char code_str[12];
    my_itoa_log(answer->status_code, code_str);
    len += append(buffer + len, code_str);
    len += append(buffer + len, " to ");
    len += append(buffer + len, client_ip);
    if (status == 400)
    {
        buffer[len] = '\n';
        len++;
        if (write(fileno(logger->stream), buffer, len) < 0)
        {
        }
        free(dates);
        return;
    }
    len += append(buffer + len, " for ");
    if (status == 405 || request == NULL || request->method == NULL)
    {
        len += append(buffer + len, "UNKNOWN");
    }
    else
    {
        len += append(buffer + len, request->method);
    }
    len += append(buffer + len, " on '");

    if (request != NULL && request->path != NULL)
    {
        len += append(buffer + len, request->path);
    }

    len += append(buffer + len, "'");
    buffer[len] = '\n';
    len++;
    if (write(fileno(logger->stream), buffer, len) < 0)
    {
    }
    free(dates);
}
