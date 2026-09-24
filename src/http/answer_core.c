#include "answer_core.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "answer_format.h"
#include "answer_path.h"
#include "http_structs.h"
#include "request_utils.h"

struct answer_http *init_answer(void)
{
    struct answer_http *answer = malloc(sizeof(struct answer_http));
    if (answer == NULL)
    {
        return NULL;
    }

    answer->version = NULL;
    answer->status_code = 0;
    answer->reason_phrase = NULL;
    answer->date = NULL;
    answer->content_length = 0;
    answer->flag_content = 0;
    answer->body = NULL;
    answer->connection = 0;
    answer->file_fd = -1;
    return answer;
}

void init_status_code(struct answer_http *answer, int code)
{
    answer->version = my_strdup2("HTTP/1.1");
    answer->status_code = code;
    answer->reason_phrase = wich_reason_phrase(code);
}

char *wich_reason_phrase(int code)
{
    switch (code)
    {
    case 200:
        return "OK";
    case 400:
        return "Bad Request";
    case 403:
        return "Forbidden";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    case 505:
        return "HTTP Version Not Supported";
    default:
        return "Error";
    }
}

static void close_answer_file(struct answer_http *answer)
{
    if (answer->file_fd != -1)
    {
        close(answer->file_fd);
        answer->file_fd = -1;
    }
}

static int request_status(struct request_http *request, struct config *config)
{
    if (strcmp(request->method, "GET") != 0
        && strcmp(request->method, "HEAD") != 0)
    {
        return 405;
    }

    if (strcmp(request->version, "HTTP/1.1") != 0)
    {
        return 505;
    }

    if (request->host == NULL || valide_request(request, config) == 0)
    {
        return 400;
    }

    return 200;
}

static int save_status(struct answer_http *answer, int status)
{
    init_status_code(answer, status);
    if (answer->version == NULL)
    {
        return -1;
    }

    return status;
}

static int finish_answer(struct answer_http *answer,
                         struct request_http *request, int status)
{
    if (status != 200)
    {
        close_answer_file(answer);
    }

    if (save_status(answer, status) < 0)
    {
        close_answer_file(answer);
        return -1;
    }

    if (status == 200 && strcmp(request->method, "HEAD") == 0)
    {
        answer->flag_content = 0;
        close_answer_file(answer);
    }

    return status;
}

int build_answer(struct answer_http *answer, struct config *config,
                 struct request_http *request)
{
    answer->date = date();
    if (answer->date == NULL)
    {
        return -1;
    }

    int status = request_status(request, config);
    if (status == 200)
    {
        status = find_path(request, answer, config);
    }

    return finish_answer(answer, request, status);
}

void free_answer(struct answer_http *answer)
{
    if (answer == NULL)
    {
        return;
    }

    free(answer->version);
    free(answer->date);
    free(answer->body);
    close_answer_file(answer);
    free(answer);
}
