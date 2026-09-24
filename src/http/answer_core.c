#include "answer_core.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "answer_format.h"
#include "answer_path.h"
#include "http_structs.h"
#include "request.h"
#include "request_utils.h"

struct answer_http *init_answer(void)
{
    struct answer_http *res = malloc(sizeof(struct answer_http));
    if (res == NULL)
    {
        return NULL;
    }
    res->version = NULL;
    res->status_code = 0;
    res->reason_phrase = NULL;
    res->date = NULL;
    res->content_length = 0;
    res->flag_content = 0;
    res->body = NULL;
    res->connection = 0;
    res->file_fd = -1;
    return res;
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
        break;
    case 400:
        return "Bad Request";
        break;
    case 403:
        return "Forbidden";
        break;
    case 404:
        return "Not Found";
        break;
    case 405:
        return "Method Not Allowed";
        break;
    case 505:
        return "HTTP Version Not Supported";
        break;
    default:
        return "Error";
    }
}
int build_answer(struct answer_http *answer, struct config *config,
                 struct request_http *request)
{
    answer->status_code = -1;
    answer->reason_phrase = NULL;
    answer->date = date();
    answer->content_length = 0;
    answer->flag_content = 0;
    answer->body = NULL;
    answer->connection = 0;
    answer->file_fd = -1;
    if (strcmp(request->method, "GET") != 0
        && strcmp(request->method, "HEAD") != 0)
    {
        init_status_code(answer, 405);
        return 405;
    }
    if (strcmp(request->version, "HTTP/1.1") != 0)
    {
        init_status_code(answer, 505);
        return 505;
    }
    if (request->host == NULL)
    {
        init_status_code(answer, 400);
        return 400;
    }
    if (valide_request(request, config) == 0)
    {
        init_status_code(answer, 400);
        return 400;
    }

    int status = find_path(request, answer, config);
    if (status != 200)
    {
        init_status_code(answer, status);
        if (answer->file_fd != -1)
        {
            close(answer->file_fd);
            answer->file_fd = -1;
        }
        return status;
    }
    else
    {
        init_status_code(answer, 200);
    }
    if (strcmp(request->method, "HEAD") == 0)
    {
        answer->flag_content = 0;
        if (answer->file_fd != -1)
        {
            close(answer->file_fd);
            answer->file_fd = -1;
        }
    }
    return 200;
}
void free_answer(struct answer_http *answer)
{
    if (answer == NULL)
    {
        return;
    }
    if (answer->version != NULL)
    {
        free(answer->version);
    }
    if (answer->date != NULL)
    {
        free(answer->date);
    }
    if (answer->body != NULL)
    {
        free(answer->body);
    }
    if (answer->file_fd != -1)
    {
        close(answer->file_fd);
    }
    free(answer);
}
