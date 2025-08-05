#ifndef __PTHREAD_POOL_H__
#define __PTHREAD_POOL_H__

#include "task_queue.h"

// 线程池
typedef struct {
    short startFlag;    // 启动标志
    int threadNum;      // 线程数量
    pthread_t *pthids;  // 线程组
    TaskQueue_t que;    // 任务队列
} ThreadPool_t;

int initPool(ThreadPool_t *pool, int num);


#endif
