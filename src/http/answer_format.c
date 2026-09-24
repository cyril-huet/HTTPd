#include "answer_format.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "http_structs.h"

static char *my_itoa(int value, char *s)
{
    int index = 0;
    int flag = 0;
    if (value < 0)
    {
        flag = 1;
        value *= -1;
    }

    if (value == 0)
    {
        s[index] = '0';
        index++;
        s[index] = '\0';
        return s;
    }

    while (value != 0)
    {
        s[index] = '0' + (value % 10);
        value = value / 10;
        index++;
    }
    if (flag == 1)
    {
        s[index] = '-';
        index++;
    }
    size_t index2 = 0;
    s[index] = '\0';
    while (s[index2] != '\0')
    {
        index2++;
    }
    for (size_t i = 0; i < index2 / 2; i++)
    {
        char temps = s[i];
        s[i] = s[index2 - i - 1];
        s[index2 - i - 1] = temps;
    }

    return s;
}

int len_int(int a)
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

static char *str_status_codes(struct answer_http *answer,
                              size_t len_status_code)
{
    char *str_status_code = malloc(sizeof(char) * (len_status_code + 1));
    if (str_status_code == NULL)
    {
        return NULL;
    }
    str_status_code = my_itoa(answer->status_code, str_status_code);
    return str_status_code;
}

static char *str_content_lengths(struct answer_http *answer,
                                 size_t len_content_length)
{
    char *str_content_length = malloc(sizeof(char) * (len_content_length + 1));
    if (str_content_length == NULL)
    {
        return NULL;
    }
    str_content_length = my_itoa(answer->content_length, str_content_length);
    return str_content_length;
}

static size_t len_basic(void)
{
    size_t basic_len = strlen("HTTP/1.1 000 OK\r\nDate: \r\nContent-Length: "
                              "\r\nConnection: close\r\n\r\n");
    return basic_len;
}

static void append_version_and_status(char *res, size_t *index,
                                      char *str_status_code,
                                      struct answer_http *answer)
{
    char *version = "HTTP/1.1 ";
    for (size_t i = 0; i < strlen(version); i++)
    {
        res[*index] = version[i];
        (*index)++;
    }

    for (size_t i = 0; i < strlen(str_status_code); i++)
    {
        res[*index] = str_status_code[i];
        (*index)++;
    }

    res[*index] = ' ';
    (*index)++;

    for (size_t i = 0; i < strlen(answer->reason_phrase); i++)
    {
        res[*index] = answer->reason_phrase[i];
        (*index)++;
    }

    res[*index] = '\r';
    (*index)++;
    res[*index] = '\n';
    (*index)++;
}

static void append_date(char *res, size_t *index, struct answer_http *answer)
{
    char *date = "Date: ";
    for (size_t i = 0; i < strlen(date); i++)
    {
        res[*index] = date[i];
        (*index)++;
    }

    for (size_t i = 0; i < strlen(answer->date); i++)
    {
        res[*index] = answer->date[i];
        (*index)++;
    }

    res[*index] = '\r';
    (*index)++;
    res[*index] = '\n';
    (*index)++;
}

static void append_content_length(char *res, size_t *index,
                                  char *str_content_length)
{
    char *content_length = "Content-Length: ";
    for (size_t i = 0; i < strlen(content_length); i++)
    {
        res[*index] = content_length[i];
        (*index)++;
    }

    for (size_t i = 0; i < strlen(str_content_length); i++)
    {
        res[*index] = str_content_length[i];
        (*index)++;
    }

    res[*index] = '\r';
    (*index)++;
    res[*index] = '\n';
    (*index)++;
}

static void append_connection(char *res, size_t *index)
{
    char *connection = "Connection: close";
    for (size_t i = 0; i < strlen(connection); i++)
    {
        res[*index] = connection[i];
        (*index)++;
    }

    res[*index] = '\r';
    (*index)++;
    res[*index] = '\n';
    (*index)++;

    res[*index] = '\r';
    (*index)++;
    res[*index] = '\n';
    (*index)++;
}
char *answer_http_to_string(struct answer_http *answer)
{
    size_t len_status_code = len_int(answer->status_code);
    size_t len_content_length = len_int(answer->content_length);

    char *str_status_code = str_status_codes(answer, len_status_code);
    if (str_status_code == NULL)
    {
        return NULL;
    }

    char *str_content_length = str_content_lengths(answer, len_content_length);
    if (str_content_length == NULL)
    {
        return NULL;
    }

    size_t basic_len = len_basic();

    size_t extra_len = strlen(answer->date) + strlen("close")
        + strlen(answer->reason_phrase) + strlen(str_status_code)
        + strlen(str_content_length) + len_status_code + len_content_length;

    char *res = malloc(sizeof(char) * (basic_len + extra_len + 1));
    if (res == NULL)
    {
        free(str_status_code);
        free(str_content_length);
        return NULL;
    }

    size_t index = 0;

    append_version_and_status(res, &index, str_status_code, answer);
    append_date(res, &index, answer);
    append_content_length(res, &index, str_content_length);
    append_connection(res, &index);

    res[index] = '\0';

    free(str_status_code);
    free(str_content_length);

    return res;
}

char *date(void)
{
    char buf[64];
    time_t now = time(NULL);
    struct tm date_time = *gmtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &date_time);

    char *res = malloc(sizeof(char) * (strlen(buf) + 1));
    if (res == NULL)
    {
        return NULL;
    }

    strcpy(res, buf);
    return res;
}
