#ifndef ANSWER_CORE_H
#define ANSWER_CORE_H

#include "../config/config.h"
#include "http_structs.h"

struct answer_http *init_answer(void);

void init_status_code(struct answer_http *answer, int code);

char *wich_reason_phrase(int code);

int build_answer(struct answer_http *answer, struct config *config,
                 struct request_http *request);

void free_answer(struct answer_http *answer);

#endif /* ! ANSWER_CORE_H */
