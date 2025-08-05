#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__

#include "head.h"

#define MAX_FILE_LEN 256

// 任务队列
typedef struct node {
    int clientFd;      // 已连接的客户端通信管道
    struct node *next;  
} TaskNode_t;

typedef struct {
    short exitFlag;         // 退出标志
    TaskNode_t *head, *tail;// 队头、队尾
    int size;               // 队列大小
    pthread_mutex_t mutex;  // 互斥锁
    pthread_cond_t cond;    // 条件变量
} TaskQueue_t;

int initQueue(TaskQueue_t *que);
int pushQueue(TaskQueue_t *que, int clientFd);
int popQueue(TaskQueue_t *que, int *clientFd);

// 任务
int transfer_file(int client_fd, const char *filename);


#endif
