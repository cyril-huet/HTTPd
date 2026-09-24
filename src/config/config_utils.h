#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include <getopt.h>

#include "config.h"

char *my_strdup(char *str);
struct config *init_struct_config(void);
int parse_option(struct config *res, struct option *longopts, int argc,
                 char *argv[]);

#endif /* ! CONFIG_UTILS_H */
