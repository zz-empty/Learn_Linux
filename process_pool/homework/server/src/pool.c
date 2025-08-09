#include "head.h"
#include "pool.h"

int workerFunc(int pipeFd) {
    int ret = 0;
    Task_t task = {};
    printf("--[Worker] worker inited!\n");
    while (1) {
        // 等待任务并处理
        ret = recvFd(pipeFd, &task);
        if (-1 == ret) {
            // recvmsg出错
            printf("--[Worker] recv task error!\n");
            continue;
        }

        if (1 == task.exitFlag) {
            // 结束Worker进程
            printf("--[Worker] SIGUSR1 comming, worker exit!\n");
            return 33;
        }

        // 执行任务
        transferFile(task.clientFd);        

        // 任务结束
        close(task.clientFd);
        // 通知Master，将自己设为非忙碌，等待下个任务
        ret = write(pipeFd, "1", 1);      // pipeFd是全双工的
        RET_CHECK(ret, -1, "write");
    }
}

int initPool(Pool_t *pool, Config_t cfg) {
    memset(pool, 0, sizeof(Pool_t));
    int ret = 0;

    // 初始化pool的参数
    pool->size = cfg.workers;
    pool->workers = (Worker_t*)calloc(pool->size, sizeof(Worker_t));
    if (pool->workers == NULL) {
        return -1;
    }

    // 创建size个进程
    int fds[2];
    pid_t pid;
    for (int i = 0; i < pool->size; ++i) {
        // 创建一对本地的套接口
        ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
        RET_CHECK(ret, -1, "socketpair");

        pid = fork();
        if (0 == pid) {
            // 开启工作逻辑，准备从Master中接收任务并处理
            close(fds[1]);  // Worker只负责接收任务
            ret = workerFunc(fds[0]);
            exit(ret);
        }

        // Master将新创建的子进程的pid和pipefd加入管理组
        close(fds[0]);  // 关闭读端，Master只负责分配任务
        pool->workers[i].busy = 0;
        pool->workers[i].pid = pid;
        pool->workers[i].pipeFd = fds[1];
    }
    return 0;
}

void destroyPool(Pool_t *pool) {
    pool->start = 0;
    pool->size = 0;
    free(pool->workers);
}
