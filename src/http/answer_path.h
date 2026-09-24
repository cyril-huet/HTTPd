#ifndef ANSWER_PATH_H
#define ANSWER_PATH_H

#include "../config/config.h"
#include "http_structs.h"

int find_path(struct request_http *request, struct answer_http *answer,
              struct config *config);

#endif /* ! ANSWER_PATH_H */
