#ifndef __MSG_H__
#define __MSG_H__

// 消息队列-消息结构体
typedef struct {
    long mtype;
    char mtext[256];
} Msg_t;

#endif
