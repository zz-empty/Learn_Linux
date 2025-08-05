#include "task_queue.h"
#include "pthread_pool.h"
#include "config.h"

int exitfd[2];
void sigfunc(int signum) {
    printf("[parent] sig %d comming...\n", signum);
    write(exitfd[1], "a", 1);
}

int main(int argc, char **argv) {
    // 配置文件
    ARGS_CHECK(argc, 2);
    int ret = 0;

    pipe(exitfd);
    if (fork()) {
        // 父进程负责接收信号, 通知子进程
        // 注册退出信号
        close(exitfd[0]);
        signal(SIGUSR1, sigfunc);

        wait(NULL);
        printf("[parent] child exited, parent exit!\n");
        close(exitfd[1]);   // 清理资源
        exit(0);
    }

    // 子进程接收退出信号
    close(exitfd[1]);
    
    // 加载配置信息
    Config_t cfg;
    load_config(argv[1], &cfg);

    printf("[info] Config info: ip-%s, port-%d, num-%d, dir-%s\n", cfg.ip, cfg.port, cfg.workers, cfg.dir);

    // 切换工作目录
    ret = chdir(cfg.dir);
    RET_CHECK(ret, -1, "chdir");

    // 初始化线程池
    ThreadPool_t pool;
    initPool(&pool, cfg.workers);
    printf("[info] thread_pool init success\n");

    // 建立tcp监听
    int server_fd = tcp_listen(cfg);
    RET_CHECK(server_fd, -1, "tcp_listen");
    printf("[info] start listen...\n");

    // 用epoll监听 
    int epfd = epoll_create(1);
    ret = epoll_add(epfd, server_fd);   // 监听客户端连接
    RET_CHECK(ret, -1, "epoll_add");
    ret = epoll_add(epfd, exitfd[0]);
    RET_CHECK(ret, -1, "epoll_add");    // 监听退出信号

    // epoll 循环
    struct epoll_event evs[2];
    int readyNum = 0;
    int clientFd = 0;
    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);
        RET_CHECK(readyNum, -1, "epoll_wait");

        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == server_fd) {
                // 新的客户端到来
                clientFd = accept(server_fd, NULL, 0);
                RET_CHECK(clientFd, -1, "accept");
                printf("[info] new clientfd: %d\n", clientFd);

                // 加入任务队列
                pthread_mutex_lock(&pool.que.mutex);
                pushQueue(&pool.que, clientFd);
                pthread_cond_signal(&pool.que.cond);    // 通知等待的线程
                pthread_mutex_unlock(&pool.que.mutex);
            }
            if (evs[i].data.fd == exitfd[0]) {
                // 收到退出信号，温和退出
                char c;
                read(exitfd[0], &c, 1);
                printf("[info] ready exit, inform all thread!\n");

                // 加锁设置标志并广播
                pthread_mutex_lock(&pool.que.mutex);
                pool.que.exitFlag = 1;
                pthread_cond_broadcast(&pool.que.cond);
                pthread_mutex_unlock(&pool.que.mutex);

                // 等待所有线程退出
                for (int i = 0; i < pool.threadNum; ++i) {
                    pthread_join(pool.pthids[i], NULL);
                }
                // 清理资源
                close(exitfd[0]);
                close(server_fd);
                free(pool.pthids);
                printf("[info] cancel success, main exit\n");
                // 退出进程
                exit(0);
            }
        }
    }
}
