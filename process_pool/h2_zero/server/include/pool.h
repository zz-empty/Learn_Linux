#ifndef __POOL_H__
#define __POOL_H__

#include "head.h"

// 工作进程
typedef struct {
    short busy;
    pid_t pid;
    int pipeFd;
} Worker_t;

// 进程池
typedef struct {
    short start;        // 启动标志
    int size;           // 进程池大小
    Worker_t *workers;   // 工作进程组
} Pool_t;

// 任务结构
typedef struct {
    short exitFlag;
    int clientFd;
} Task_t;

// 进程池
int initPool(Pool_t *pool, Config_t cfg);
void destroyPool(Pool_t *pool);

// Master分配任务
int sendFd(int pipeFd, Task_t *task);
// Worker接收任务
int recvFd(int pipeFd, Task_t *task);

// Worker的功能
int transferFile(int clientFd);

#endif
