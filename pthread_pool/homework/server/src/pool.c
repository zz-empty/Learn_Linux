#include "pool.h"

// 工作线程
void *threadFunc(void *arg) {
    pool_shared_t *shares = (pool_shared_t*)arg;
    int clientFd = 0;
    
    while (1) {
        pthread_mutex_lock(&shares->mutex);
        // 如果没有任务，就等待
        while (shares->queue.size == 0 && !shares->exitFlag) {
            printf("--[Worker] start wait\n");
            pthread_cond_wait(&shares->cond, &shares->mutex);
        }
        printf("--[Worker] wait over\n");

        // 判断是否退出
        if (shares->exitFlag && shares->queue.size == 0) {
            pthread_mutex_unlock(&shares->mutex);
            printf("--[Worker] ready exit\n");
            pthread_exit(NULL);
        }
        // 取任务
        popQueue(&shares->queue, &clientFd);
        pthread_mutex_unlock(&shares->mutex);

        printf("--[Worker] start task...\n");
        // 执行任务
        transferFile(clientFd);
        // 执行完关闭
        close(clientFd);
        printf("--[Worker] task over\n");
    }
}

int initShares(pool_shared_t *shares) {
    pthread_mutex_init(&shares->mutex, NULL);
    pthread_cond_init(&shares->cond, NULL);
    shares->queue.head = NULL;
    shares->queue.tail = NULL;
    shares->queue.size = 0;
    shares->exitFlag = 0;
    return 0;
}

// 初始化线程池
int initPool(thread_pool_t *pool, int num) {
    memset(pool, 0, sizeof(thread_pool_t));
    pool->threadNum = num;
    pool->startFlag = 1;

    // 初始化共享数据
    initShares(&pool->shares);
    
    // 创建线程池
    pool->pthids = (pthread_t*)calloc(pool->threadNum, sizeof(pthread_t));

    for (int i = 0; i < pool->threadNum; ++i) {
        pthread_create(pool->pthids + i, NULL, threadFunc, &pool->shares);
    }
    return 0;
}
// 入队
int pushQueue(task_queue_t *que, int clientFd) {
    task_node_t *node = (task_node_t*)calloc(1, sizeof(task_node_t));
    if (node == NULL) {
        return -1;
    }
    node->clientFd = clientFd;

    if (0 == que->size) {
        // 空队列
        que->head = que->tail = node;
    }
    else {
        que->tail->next = node;
        que->tail = node;
    }

    que->size++;
    return 0;
}

// 出队
int popQueue(task_queue_t *que, int *clientFd) {
    if (que->size && que->head) {
        // 取任务
        *clientFd = que->head->clientFd;
        // 改队头
        task_node_t *cur = que->head;
        que->head = cur->next;
        // 缩小队列
        que->size--;
        if (0 == que->size) {
            que->tail = NULL;
        }
        // 释放节点
        free(cur);
        cur = NULL;
        return 0;
    } else {
        // 队空
        return -1;
    }
}
