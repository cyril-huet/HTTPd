#ifndef STRING_H
#define STRING_H

#include <ctype.h>
#include <stddef.h>

struct string
{
    size_t size;
    char *data;
};

/* Create a new string and copy size characters from data. */
struct string *string_create(const char *str, size_t size);

/* Compare the string with the first size characters of data. */
int string_compare_n_str(const struct string *str1, const char *str2, size_t n);

/* Append size characters to the string. */
void string_concat_str(struct string *str, const char *to_concat, size_t size);

void string_destroy(struct string *str);

#endif /* ! STRING_H */
