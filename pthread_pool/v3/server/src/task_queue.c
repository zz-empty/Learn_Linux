#include "head.h"
#include "task_queue.h"

int initQueue(TaskQueue_t *que) {
    memset(que, 0, sizeof(TaskNode_t));
    pthread_mutex_init(&que->mutex, NULL);
    pthread_cond_init(&que->cond, NULL);
    return 1;
}

int pushQueue(TaskQueue_t *que, int clientFd) {
    // 创建任务节点
    TaskNode_t *node = (TaskNode_t*)calloc(1, sizeof(TaskNode_t));
    node->clientFd = clientFd;
    // 任务入队
    if (0 == que->size) {
        que->head = que->tail = node;
    } else {
        que->tail->next = node;
        que->tail = node;
    }
    que->size++;
    return 1;
}

int popQueue(TaskQueue_t *que, int *clientFd) {
    if (que->size) {
        // 任务出队
        TaskNode_t *node = que->head;
        *clientFd = que->head->clientFd;
        que->head = que->head->next;
        que->size--;
        if (0 == que->size) {
            que->tail = que->head;
        }
        // 销毁任务节点
        free(node);
        node = NULL;
        return 1;
    }
    return -1;
}
