#include "pthread_pool.h"
#include "task_queue.h"

// 线程函数
void cleanFunc(void *arg) {
    printf("[info] thread cleanfunc --- unlock\n");
    pthread_mutex_t *mutex = (pthread_mutex_t*)arg;
    pthread_mutex_unlock(mutex);
}
void *threadFunc(void *arg) {
    TaskQueue_t *que = (TaskQueue_t*)arg;
    int clientFd = 0;
    while (1) {
        // 取任务
        pthread_mutex_lock(&que->mutex);
        pthread_cleanup_push(cleanFunc, &que->mutex);
        if (0 == que->size) {
            pthread_cond_wait(&que->cond, &que->mutex); // 等待任务
        }
        popQueue(que, &clientFd);   // 取一个任务
        pthread_mutex_unlock(&que->mutex);

        // 处理任务，发送文件
        const char *filename = "file1";
        transfer_file(clientFd, filename); 
        pthread_cleanup_pop(1);
    }
    pthread_exit(NULL);
}

int initPool(ThreadPool_t *pool, int num) {
    memset(pool, 0, sizeof(ThreadPool_t));
    pool->startFlag = 1;    
    pool->threadNum = num;
    initQueue(&pool->que);

    // 创建空间存放线程组
    pool->pthids = (pthread_t*)calloc(pool->threadNum, sizeof(pthread_t));
    if (!pool->pthids) {
        return -1;
    }
    // 创建num个线程
    for (int i = 0; i < pool->threadNum; ++i) {
        pthread_create(pool->pthids + i, NULL, threadFunc, &pool->que);
    }
    printf("[info] pool init and fire succeed!\n");
    return 1;
}
