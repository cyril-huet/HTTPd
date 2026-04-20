#ifndef ANSWER_FORMAT_H
#define ANSWER_FORMAT_H

#include "http_structs.h"

int len_int(int a);

char *answer_http_to_string(struct answer_http *answer);

char *date(void);

#endif /* ! ANSWER_FORMAT_H */
