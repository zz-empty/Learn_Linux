#ifndef __PROCESS_POOL_H__
#define __PROCESS_POOL_H__

#include "head.h"

#define MAX_IP_LEN 64
#define MAX_LINE_LEN 256

typedef struct {
    short busy;
    int pipefd;
    pid_t pid;
} Worker_t;


typedef struct {
    char ip[MAX_IP_LEN];
    int port;
    int workers;
} Config_t;

int load_config(const char *filename, Config_t *cfg);


#endif
