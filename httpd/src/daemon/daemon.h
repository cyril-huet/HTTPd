#ifndef DAEMON_H
#define DAEMON_H

#include "../config/config.h"

int daemon_start(struct config *config);
int daemon_stop(struct config *config);
int daemon_restart(struct config *config);

#endif /* DAEMON_H */
