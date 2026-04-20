#define _POSIX_C_SOURCE 200809L
#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
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

int init_server(char *ip, char *port)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int sockfd;
    int flag = 1;
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    status = getaddrinfo(ip, port, &hints, &res);
    if (status != 0)
    {
        return -1;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        freeaddrinfo(res);
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }
    if (listen(sockfd, SOMAXCONN) == -1)
    {
        close(sockfd);
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);

    return sockfd;
}

void run_server(int sockfd, struct config *config)
{
    int status;
    struct logger *logger = init_logger(config);
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd =
            accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            if (errno == EINTR)
            {
                break;
            }
            continue;
        }
        char client_ip[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip,
                      sizeof(client_ip))
            == NULL)
        {
            strcpy(client_ip, "unknown");
        }
        char buffer[4096];
        status = read_request(client_fd, buffer, sizeof(buffer));
        if (status < 0)
        {
            close(client_fd);
            continue;
        }
        struct request_http *request = init_request();
        parse_httpd(request, buffer, status, client_fd);

        logger_request(logger, request, client_ip);

        struct answer_http *answer = init_answer();
        build_answer(answer, config, request);

        logger_response(logger, answer, request, client_ip);
        char *reponse = answer_http_to_string(answer);
        if (write(client_fd, reponse, strlen(reponse)) < 0)
        {
        }

        if (answer->flag_content == 1 && answer->file_fd != -1)
        {
            sendfile(client_fd, answer->file_fd, NULL, answer->content_length);
        }

        free(reponse);
        free_answer(answer);
        close(client_fd);
        free_request(request);
    }
    log_close(logger);
}
