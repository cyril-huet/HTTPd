#define _POSIX_C_SOURCE 200809L
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

#include "logger_utils.h"

static int open_logger_stream(struct logger *logger, struct config *config)
{
    logger->stream = NULL;
    logger->flag = 0;

    if (config->log == false)
    {
        return 0;
    }

    logger->flag = 1;
    if (config->log_file == NULL)
    {
        logger->stream = stdout;
        return 0;
    }

    logger->stream = fopen(config->log_file, "a");
    if (logger->stream == NULL)
    {
        return -1;
    }

    return 0;
}

struct logger *init_logger(struct config *config)
{
    struct logger *logger = malloc(sizeof(struct logger));
    if (logger == NULL)
    {
        return NULL;
    }

    logger->server_name = to_string(config->servers->server_name);
    if (logger->server_name == NULL || open_logger_stream(logger, config) < 0)
    {
        free(logger->server_name);
        free(logger);
        return NULL;
    }

    return logger;
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

static int logger_is_enabled(struct logger *logger)
{
    if (logger == NULL || logger->flag == 0 || logger->stream == NULL)
    {
        return 0;
    }

    return 1;
}

void logger_request(struct logger *logger, struct request_http *request,
                    char *client_ip)
{
    if (logger_is_enabled(logger) == 0)
    {
        return;
    }

    char *log_date = date_log();
    if (log_date == NULL)
    {
        return;
    }

    if (request == NULL || request->method == NULL || request->path == NULL)
    {
        fprintf(logger->stream, "%s [%s] received Bad Request from %s\n",
                log_date, logger->server_name, client_ip);
    }
    else
    {
        fprintf(logger->stream, "%s [%s] received %s on '%s' from %s\n",
                log_date, logger->server_name, request->method, request->path,
                client_ip);
    }

    fflush(logger->stream);
    free(log_date);
}

static const char *method_for_log(struct answer_http *answer,
                                  struct request_http *request)
{
    if (answer->status_code == 405 || request == NULL
        || request->method == NULL)
    {
        return "UNKNOWN";
    }

    return request->method;
}

static const char *path_for_log(struct request_http *request)
{
    if (request == NULL || request->path == NULL)
    {
        return "";
    }

    return request->path;
}

void logger_response(struct logger *logger, struct answer_http *answer,
                     struct request_http *request, char *client_ip)
{
    if (logger_is_enabled(logger) == 0 || answer == NULL)
    {
        return;
    }

    char *log_date = date_log();
    if (log_date == NULL)
    {
        return;
    }

    fprintf(logger->stream, "%s [%s] responding with %d to %s", log_date,
            logger->server_name, answer->status_code, client_ip);

    if (answer->status_code != 400)
    {
        fprintf(logger->stream, " for %s on '%s'",
                method_for_log(answer, request), path_for_log(request));
    }

    fprintf(logger->stream, "\n");
    fflush(logger->stream);
    free(log_date);
}
