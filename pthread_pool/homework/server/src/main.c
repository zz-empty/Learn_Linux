#include "head.h"
#include "pool.h"

int epollAdd(int epfd, int newfd) {
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = newfd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 0;
}

int exitPipes[2];

void sigFunc(int signum) {
    const char *buf = "[warning] SIGUSER1 is comming...\n";
    write(STDOUT_FILENO, buf, strlen(buf));

    write(exitPipes[1], "1", 1);
}

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    
    pipe(exitPipes);

    if (fork()) {
        // 主进程接收退出信号，通知子进程退出
        close(exitPipes[0]);
        signal(SIGUSR1, sigFunc);

        wait(NULL);
        close(exitPipes[1]);
        exit(EXIT_SUCCESS);
    }

    close(exitPipes[1]);

    // 加载配置文件
    Config_t cfg = {};
    loadConfig(argv[1], &cfg);
    printConfig(cfg);
    printf("\n");
    // 切换到家目录
    chdir(cfg.home);

    // 建立线程池
    thread_pool_t pool;
    initPool(&pool, cfg.workers);

    // 建立tcp套接字
    int serverFd = tcpListen(cfg);

    // 建立epoll监听
    int epfd = epoll_create(1);
    epollAdd(epfd, serverFd);
    epollAdd(epfd, exitPipes[0]);   // 监听退出管道

    int readyNum = 0;
    struct epoll_event evs[2] = {};
    int clientFd = 0;
    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);
        if (-1 == readyNum) {
            if (errno == EINTR) {
                printf("[warning] epoll_wait is interrupted!\n");
                continue;
            }
            perror("epoll_wait");
            return -1;
        }

        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == serverFd) {
                // 新任务，入队, 通知
                clientFd = accept(serverFd, NULL, 0);
                RET_CHECK(clientFd, -1, "accept");
                printf("[Master] new client [%d] comming...\n", clientFd);

                pthread_mutex_lock(&pool.shares.mutex);
                pushQueue(&pool.shares.queue, clientFd);
                pthread_cond_signal(&pool.shares.cond);
                pthread_mutex_unlock(&pool.shares.mutex);
            }

            if (evs[i].data.fd == exitPipes[0]) {
                // 退出信号到来，通知线程池退出
                printf("[Master] ready exit all thread\n");
                char c;
                read(exitPipes[0], &c, 1);

                pthread_mutex_lock(&pool.shares.mutex);
                pool.shares.exitFlag = 1;
                pthread_cond_broadcast(&pool.shares.cond);
                pthread_mutex_unlock(&pool.shares.mutex);

                for (int j = 0; j < pool.threadNum; ++j) {
                    pthread_join(pool.pthids[j], NULL);
                }

                printf("[Master] thread pool already exited!\n");
                close(exitPipes[0]);
                exit(EXIT_SUCCESS);
            }
        }
    }

    return 0;
}
