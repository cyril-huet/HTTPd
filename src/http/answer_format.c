#include "answer_format.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "http_structs.h"

int len_int(int value)
{
    if (value == 0)
    {
        return 1;
    }

    int length = 0;
    if (value < 0)
    {
        length++;
    }

    while (value != 0)
    {
        length++;
        value = value / 10;
    }

    return length;
}

static void number_to_string(size_t value, char *result)
{
    size_t length = 0;

    if (value == 0)
    {
        result[0] = '0';
        result[1] = '\0';
        return;
    }

    while (value > 0)
    {
        result[length] = '0' + (value % 10);
        value = value / 10;
        length++;
    }

    result[length] = '\0';
    for (size_t index = 0; index < length / 2; index++)
    {
        char temporary = result[index];
        result[index] = result[length - index - 1];
        result[length - index - 1] = temporary;
    }
}

static void append_text(char *result, size_t *index, const char *text)
{
    size_t text_index = 0;
    while (text[text_index] != '\0')
    {
        result[*index] = text[text_index];
        (*index)++;
        text_index++;
    }
}

static void append_header(char *result, size_t *index, const char *name,
                          const char *value)
{
    append_text(result, index, name);
    append_text(result, index, ": ");
    append_text(result, index, value);
    append_text(result, index, "\r\n");
}

static size_t response_length(struct answer_http *answer, const char *status,
                              const char *content_length)
{
    size_t length = strlen(answer->version) + strlen(status);
    length += strlen(answer->reason_phrase) + strlen(answer->date);
    length += strlen(content_length);
    length += strlen("  \r\nDate: \r\nContent-Length: \r\n");
    length += strlen("Connection: close\r\n\r\n");
    return length;
}

static void write_response(char *result, struct answer_http *answer,
                           const char *status, const char *content_length)
{
    size_t index = 0;
    append_text(result, &index, answer->version);
    append_text(result, &index, " ");
    append_text(result, &index, status);
    append_text(result, &index, " ");
    append_text(result, &index, answer->reason_phrase);
    append_text(result, &index, "\r\n");
    append_header(result, &index, "Date", answer->date);
    append_header(result, &index, "Content-Length", content_length);
    append_header(result, &index, "Connection", "close");
    append_text(result, &index, "\r\n");
    result[index] = '\0';
}

char *answer_http_to_string(struct answer_http *answer)
{
    if (answer == NULL || answer->version == NULL
        || answer->reason_phrase == NULL || answer->date == NULL)
    {
        return NULL;
    }

    char status[32];
    char content_length[32];
    number_to_string(answer->status_code, status);
    number_to_string(answer->content_length, content_length);

    size_t length = response_length(answer, status, content_length);
    char *result = malloc(length + 1);
    if (result == NULL)
    {
        return NULL;
    }

    write_response(result, answer, status, content_length);
    return result;
}

char *date(void)
{
    char buffer[64];
    time_t now = time(NULL);
    struct tm *utc_time = gmtime(&now);
    if (utc_time == NULL)
    {
        return NULL;
    }

    size_t length =
        strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", utc_time);
    if (length == 0)
    {
        return NULL;
    }

    char *result = malloc(length + 1);
    if (result == NULL)
    {
        return NULL;
    }

    strcpy(result, buffer);
    return result;
}
