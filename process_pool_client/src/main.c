#include "head.h"

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

    // 从配置文件中获取ip、port
    Config_t cfg = {};
    load_config(argv[1], &cfg);

    printf("[config] ip = %s, port = %d\n", cfg.ip, cfg.port);

    // 连接服务器
    int server_fd = socket(AF_INET, SOL_SOCKET, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(cfg.ip);
    serAddr.sin_port = htons(cfg.port);
    socklen_t serlen = sizeof(serAddr);

    ret = connect(server_fd, (struct sockaddr *)&serAddr, serlen);
    RET_CHECK(ret, -1, "connect");

    
    // 用epoll管理所有server_fd STDIN_FILENO
    int epfd = epoll_create(1);
    ret = epoll_add(epfd, server_fd);
    RET_CHECK(ret, -1, "epoll_add");
    ret = epoll_add(epfd, STDIN_FILENO);
    RET_CHECK(ret, -1, "epoll_add");

    printf("----already connect server----\n");

    // 开始启动
    int readyNum = 0;
    struct epoll_event evs[2];
    char buf[MAX_LINE_LEN];
    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);

        // 有readyNum个fd响应
        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == server_fd) {
                // 服务器响应的数据
                memset(buf, 0, sizeof(buf));
                ret = recv(server_fd, buf, sizeof(buf) - 1, 0);
                RET_CHECK(ret, -1, "recv");
                if (0 == ret) {
                    // 服务器断开
                    printf("---server exit----\n");
                    close(server_fd);
                    exit(0);
                }
                // 显示
                printf("[server %d] %s\n", server_fd, buf);
            } 
            if (evs[i].data.fd == STDIN_FILENO) {
                // 给服务器发送请求
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                RET_CHECK(ret, -1, "read");
                if (0 == ret) {
                    printf("I quit\n");
                    close(server_fd);
                    exit(0);
                }

                ret = send(server_fd, buf, ret - 1, 0);
                RET_CHECK(ret, -1, "send");
            }
        }
    }

    return 0;
}

