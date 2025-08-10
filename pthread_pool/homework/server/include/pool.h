#ifndef __POOL_H__
#define __POOL_H__

#include "head.h"

// 任务节点
typedef struct node {
    int clientFd;
    struct node *next;
} task_node_t;

// 任务队列
typedef struct {
    task_node_t *head;
    task_node_t *tail;
    int size;
} task_queue_t;

// 共享结构
typedef struct {
    short exitFlag;         // 退出标志
    pthread_mutex_t mutex;  // 互斥锁
    pthread_cond_t cond;    // 条件变量
    task_queue_t queue;     // 任务队列
} pool_shared_t;

// 线程池
typedef struct {
    short startFlag;    // 启动标志
    int threadNum;      // 线程数量
    pthread_t *pthids;  // 线程组
    pool_shared_t shares;   // 所有线程共享数据
} thread_pool_t;

// 初始化线程池
int initPool(thread_pool_t *pool, int num);
// 初始化共享数据
int initShares(pool_shared_t *shares);
// 入队
int pushQueue(task_queue_t *que, int clientFd);
// 出队
int popQueue(task_queue_t *que, int *clientFd);

// 工作线程的功能
int transferFile(int clientFd);

#endif
