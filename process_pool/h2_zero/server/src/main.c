#include "head.h"
#include "pool.h"

#define WAIT_ARRAY_SIZE 10

// 信号通知的管道
int exitPipe[2];

// 信号处理函数
void sigFunc(int signum) {
    printf("[signal] %d is comming...\n", signum);
    write(exitPipe[1], "1", 1);
}

// 添加描述符到epoll中
static int epollAdd(int epfd, int newfd) {
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = newfd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 0;
}

int main(int argc, char **argv)
{
    // ./app conf
    ARGS_CHECK(argc, 2);

    // 加载配置信息
    Config_t cfg;
    loadConfig(argv[1], &cfg);
    printConfig(&cfg);

    // 切换到家目录
    chdir(cfg.home);
    printf("\n[Master] Entry home dir\n");

    // 创建进程池并启动
    Pool_t pool;
    initPool(&pool, cfg);
    pool.start = 1;

    // 注册退出信号
    pipe(exitPipe);
    struct sigaction sa = {};
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigFunc;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    // 建立tcp连接
    int serverFd = tcpListen(cfg);
    if (-1 == serverFd) {
        fprintf(stderr, "[error] tcp_socket init failed!\n");
        exit(EXIT_FAILURE);
    }

    // 建立epoll监听
    int epfd = epoll_create(1);
    epollAdd(epfd, serverFd);

    epollAdd(epfd, exitPipe[0]);

    for (int i = 0; i < pool.size; ++i) {
        epollAdd(epfd, pool.workers[i].pipeFd);
    }

    // 开始处理各种描述符
    int readyNum = 0;
    struct epoll_event evs[WAIT_ARRAY_SIZE] = {};
    Task_t task = {};
    char done;
    while (1) {
        readyNum = epoll_wait(epfd, evs, WAIT_ARRAY_SIZE, -1);
        if (-1 == readyNum) {
            if (errno == EINTR) {
                // 被信号打断，继续等待（下一轮会发现管道可读）
                continue;
            }
            RET_CHECK(readyNum, -1, "epoll_wait");
        }

        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == exitPipe[0]) {
                // 退出信号到来, 通知所有子进程退出
                printf("[warning] Master recv SIGUSR1!!!\n");
                task.exitFlag = 1;
                for (int j = 0; j < pool.size; ++j) {
                    sendFd(pool.workers[j].pipeFd, &task);
                }

                // 回收所有子进程
                for (int j = 0; j < pool.size; ++j) {
                    waitpid(pool.workers[j].pid, NULL, 0);
                }
                
                // 退出
                printf("[Master] all worker exited, Master exit!\n");
                exit(EXIT_SUCCESS);
            }
            else if (evs[i].data.fd == serverFd) {
                // 新的连接
                memset(&task, 0, sizeof(Task_t));
                task.clientFd = accept(serverFd, NULL, 0);
                RET_CHECK(task.clientFd, -1, "accept");
                printf("[Master] new client fd : %d\n", task.clientFd);

                // 找到空闲的子进程，把任务发给它，再将其设为忙碌
                for (int j = 0; j < pool.size; ++j) {
                    if (pool.workers[j].busy == 0) {
                        printf("[Master] worker [%d] start task\n", pool.workers[j].pid);
                        sendFd(pool.workers[j].pipeFd, &task);
                        // Master不再需要这个副本，关掉它
                        close(task.clientFd);
                        pool.workers[j].busy = 1;
                        break;
                    }
                }
            }
            else {
                // 某个Worker完成任务
                for (int j = 0; j < pool.size; ++j) {
                    // 只回收忙碌的Worker
                    if (pool.workers[j].busy && evs[i].data.fd == pool.workers[j].pipeFd) {
                        // 将子进程设为非忙碌
                        printf("[Master] worker [%d] finish task!\n", pool.workers[j].pid);
                        read(pool.workers[j].pipeFd, &done, 1);
                        pool.workers[j].busy = 0;
                        break;
                    }
                }
            }
        }
    }

    destroyPool(&pool);
    freeConfig(&cfg);
    return 0;
}
