#ifndef LOGGER_UTILS_H
#define LOGGER_UTILS_H

#include "../utils/string/string.h"
char *to_string(struct string *str);
size_t append(char *dest, char *src);
char *date_log(void);
char *my_itoa_log(int value, char *buffer);
#endif /* ! LOGGER_UTILS_H */
