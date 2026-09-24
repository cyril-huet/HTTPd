#define _POSIX_C_SOURCE 200809L
#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../config/config.h"
#include "../http/answer_core.h"
#include "../http/answer_format.h"
#include "../http/http_structs.h"
#include "../http/request.h"
#include "../http/request_body.h"
#include "../logger/logger.h"
#include "server_utils.h"

struct client_context
{
    int socket;
    char *ip;
    struct config *config;
    struct logger *logger;
};

static int create_server_socket(struct addrinfo *address)
{
    int server_socket =
        socket(address->ai_family, address->ai_socktype, address->ai_protocol);
    if (server_socket < 0)
    {
        return -1;
    }

    int enabled = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enabled,
                   sizeof(enabled))
            < 0
        || bind(server_socket, address->ai_addr, address->ai_addrlen) < 0
        || listen(server_socket, SOMAXCONN) < 0)
    {
        close(server_socket);
        return -1;
    }

    return server_socket;
}

int init_server(char *ip, char *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *address;
    if (getaddrinfo(ip, port, &hints, &address) != 0)
    {
        return -1;
    }

    int server_socket = create_server_socket(address);
    freeaddrinfo(address);
    return server_socket;
}

static int write_all(int file, const char *buffer, size_t size)
{
    size_t total = 0;

    while (total < size)
    {
        ssize_t written = write(file, buffer + total, size - total);

        if (written < 0 && errno == EINTR)
        {
            continue;
        }

        if (written <= 0)
        {
            return -1;
        }

        total += written;
    }

    return 0;
}

static int send_file_content(int client_socket, int file)
{
    char buffer[4096];

    while (1)
    {
        ssize_t size = read(file, buffer, sizeof(buffer));

        if (size == 0)
        {
            return 0;
        }

        if (size < 0 && errno == EINTR)
        {
            continue;
        }

        if (size < 0 || write_all(client_socket, buffer, size) < 0)
        {
            return -1;
        }
    }
}

static int send_answer(int client_socket, struct answer_http *answer)
{
    char *response = answer_http_to_string(answer);
    if (response == NULL)
    {
        return -1;
    }

    int status = write_all(client_socket, response, strlen(response));
    free(response);

    if (status < 0)
    {
        return -1;
    }

    if (answer->flag_content == 1 && answer->file_fd != -1)
    {
        return send_file_content(client_socket, answer->file_fd);
    }

    return 0;
}

static int build_error_answer(struct answer_http *answer, int status)
{
    answer->date = date();
    if (answer->date == NULL)
    {
        return -1;
    }

    init_status_code(answer, status);
    if (answer->version == NULL)
    {
        return -1;
    }

    return status;
}

static void log_received_request(struct client_context *client,
                                 struct request_http *request, int status)
{
    if (status == 400)
    {
        logger_request(client->logger, NULL, client->ip);
    }
    else
    {
        logger_request(client->logger, request, client->ip);
    }
}

static void answer_request(struct client_context *client,
                           struct request_http *request, int parse_status)
{
    struct answer_http *answer = init_answer();
    if (answer == NULL)
    {
        return;
    }

    int status;
    if (parse_status == 0)
    {
        status = build_answer(answer, client->config, request);
    }
    else
    {
        status = build_error_answer(answer, parse_status);
    }

    if (status >= 0)
    {
        logger_response(client->logger, answer, request, client->ip);
        send_answer(client->socket, answer);
    }

    free_answer(answer);
}

static void handle_client(struct client_context *client)
{
    char buffer[4096];
    int size = read_request(client->socket, buffer, sizeof(buffer));
    if (size < 0)
    {
        return;
    }

    struct request_http *request = init_request();
    if (request == NULL)
    {
        return;
    }

    int parse_status = parse_httpd(request, buffer, size, client->socket);
    log_received_request(client, request, parse_status);
    answer_request(client, request, parse_status);
    free_request(request);
}

static void save_client_ip(struct sockaddr_in *address, char *client_ip)
{
    if (inet_ntop(AF_INET, &address->sin_addr, client_ip, INET_ADDRSTRLEN)
        == NULL)
    {
        strcpy(client_ip, "unknown");
    }
}

static int accept_client(int server_socket, struct config *config,
                         struct logger *logger)
{
    struct sockaddr_in address;
    socklen_t address_size = sizeof(address);
    int client_socket =
        accept(server_socket, (struct sockaddr *)&address, &address_size);

    if (client_socket < 0)
    {
        if (errno == EINTR)
        {
            return -1;
        }
        return 0;
    }

    char client_ip[INET_ADDRSTRLEN];
    save_client_ip(&address, client_ip);

    struct client_context client = {
        client_socket,
        client_ip,
        config,
        logger,
    };

    handle_client(&client);
    close(client_socket);
    return 0;
}

int run_server(int server_socket, struct config *config)
{
    if (config == NULL)
    {
        return -1;
    }

    struct logger *logger = init_logger(config);
    if (logger == NULL && config->log == true)
    {
        return -1;
    }

    while (accept_client(server_socket, config, logger) == 0)
    {
    }

    log_close(logger);
    return 0;
}
