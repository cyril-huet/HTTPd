#ifndef HTTP_STRUCTS_H
#define HTTP_STRUCTS_H

#include <stddef.h>

struct request_http
{
    char *method;
    char *path;
    char *version;
    char *host;
    size_t content_length;
    int flag_content;
    char *body;
};

struct answer_http
{
    char *version;
    int status_code;
    char *reason_phrase;
    char *date;
    size_t content_length;
    int flag_content;
    char *body;
    int connection;
    int file_fd;
};

#endif /* ! HTTP_STRUCTS_H */
