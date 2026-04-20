#define _POSIX_C_SOURCE 200809L
#include "logger_utils.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

char *to_string(struct string *str)
{
    char *res = malloc(sizeof(char) * (str->size + 1));
    if (res == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < str->size; i++)
    {
        res[i] = str->data[i];
    }

    res[str->size] = '\0';
    return res;
}

size_t append(char *dest, char *copie)
{
    size_t index = 0;
    while (copie[index] != '\0')
    {
        dest[index] = copie[index];
        index++;
    }
    return index;
}

char *date_log(void)
{
    char buf[64];
    time_t time_null = time(NULL);
    struct tm date_time = *gmtime(&time_null);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &date_time);
    char *res = malloc(sizeof(char) * (strlen(buf) + 1));
    if (res == NULL)
    {
        return NULL;
    }
    strcpy(res, buf);
    return res;
}

char *my_itoa_log(int value, char *s)
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
