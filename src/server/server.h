#ifndef SERVER_H
#define SERVER_H

#include "../config/config.h"

int init_server(char *ip, char *port);
int run_server(int sockfd, struct config *config);

#endif /* ! SERVER_H */
