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
        // 退出检查，放在循环顶部确保及时响应
        if (que->exitFlag) {
            pthread_mutex_unlock(&que->mutex);
            printf("[work thread] exitflag setted!, thread exitted!\n");
            break;
        }
        // 等待任务，防止虚假唤醒
        while (0 == que->size && !que->exitFlag) {
            pthread_cond_wait(&que->cond, &que->mutex); // 等待任务
        }
        // 退出检查，放在循环顶部确保及时响应
        if (que->exitFlag) {
            pthread_mutex_unlock(&que->mutex);
            printf("[work thread] exitflag setted!, thread exitted!\n");
            break;
        }
        popQueue(que, &clientFd);   // 取一个任务
        pthread_mutex_unlock(&que->mutex);

        // 处理任务，发送文件
        printf("[work thread] start work...\n");
        const char *filename = "file1";
        transfer_file(clientFd, filename); 
        printf("[work thread] work over...\n");
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
