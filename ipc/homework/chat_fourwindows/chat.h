#ifndef __CHAT_H__
#define __CHAT_H__

#include <sys/types.h>

typedef struct Msg {
    long mtype;
    char mtext[1024];
} Msg_t;

typedef struct {
    pid_t A;
    pid_t B;
    pid_t A1;
    pid_t B1;
} Chatpid_t;

#endif
