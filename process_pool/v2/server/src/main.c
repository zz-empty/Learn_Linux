#include "head.h"
#include "process_pool.h"

int init_socket(Config_t cfg) {
    int server_fd = socket(AF_INET, SOL_SOCKET, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(cfg.ip);
    serAddr.sin_port = htons(cfg.port);

    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int ret;
    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");

    ret = listen(server_fd, 5);
    RET_CHECK(ret, -1, "listen");

    return server_fd;
}

int epoll_add(int epfd, int fd) {
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = fd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 1;
}

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int ret = 0;

    // 从配置文件中获取ip、port、workersnum
    Config_t cfg = {};
    load_config(argv[1], &cfg);

    printf("[info] server: ip = %s, port = %d, workers = %d, dir = %s\n", cfg.ip, cfg.port, cfg.workers, cfg.dir);

    // 切换工作目录
    ret = chdir(cfg.dir);
    RET_CHECK(ret, -1, "chdir");

    // 创建进程池
    Pool_t pool = {};
    pool.workers = (Worker_t*)calloc(cfg.workers, sizeof(Worker_t));
    pool.size = cfg.workers;
    create_pool(&pool);
    
    // 建立TCP监听套接字
    int server_fd = init_socket(cfg);
    RET_CHECK(server_fd, -1, "init_socket");

    
    // 用epoll管理所有fd
    int epfd = epoll_create(1);
    ret = epoll_add(epfd, server_fd);
    RET_CHECK(ret, -1, "epoll_add");

    for (int i = 0; i < pool.size; ++i) {
        ret = epoll_add(epfd, pool.workers[i].pipefd);
        RET_CHECK(ret, -1, "epoll_add");
    }

    printf("[info] Pool inited, start listen...\n");

    // 开始启动
    // 分配新的客户端任务，给空闲的子进程
    // 回收空闲的子进程
    int readyNum = 0;
    struct epoll_event evs[2];
    int client_fd = 0;
    char buf[10];
    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);

        // 有readyNum个fd响应
        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == server_fd) {
                // 新客户端
                client_fd = accept(server_fd, NULL, 0);
                RET_CHECK(client_fd, -1, "accept");

                // 交给空闲的子进程，修改子进程状态为忙碌
                for (int j = 0; j < pool.size; ++j) {
                    if (pool.workers[j].busy == 0) {
                        send_fd(pool.workers[j].pipefd, client_fd);
                        pool.workers[j].busy = 1;
                        break;  // 退出循环
                    }
                }
                close(client_fd);
            } else {
                // 子进程响应，将其设为空闲
                for (int j = 0; j < pool.size; ++j) {
                    if (evs[i].data.fd == pool.workers[j].pipefd) {
                        // 读取管道
                        read(pool.workers[j].pipefd, buf, sizeof(buf));
                        pool.workers[j].busy = 0;
                    }
                }
            }
        }
    }

    return 0;
}

