#include "head.h"
#define MAX_DATA_LEN 1024

int epoll_add(int epfd, int newfd) {
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = newfd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 1;
}

typedef struct {
    int len;
    char data[MAX_DATA_LEN];
} Buffer_t;

int main()
{
    const char *ip = "127.0.0.1";
    const int port = 2000;
    int ret = 0;

    int server_fd = socket(AF_INET, SOCK_DGRAM, 0); 
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(port);
    socklen_t serlen = sizeof(serAddr);

    // 用epoll监听
    int epfd = epoll_create(1);
    RET_CHECK(epfd, -1, "epoll_create");
    epoll_add(epfd, STDIN_FILENO);
    epoll_add(epfd, server_fd);

    int readyNum = 0;
    struct epoll_event evs[2];
    Buffer_t buffer = {};

    printf("[info] udp_client init succeed! need send something for connect server...\n");
    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);
        RET_CHECK(readyNum, -1, "epoll_wait");

        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == server_fd) {
                // 接收服务器数据
                memset(&buffer, 0, sizeof(buffer));
                buffer.len = recvfrom(server_fd, buffer.data, MAX_DATA_LEN, 0, (struct sockaddr*)&serAddr, &serlen);
                RET_CHECK(buffer.len, -1, "recvfrom");

                // 显示数据
                printf("[server %s] %s\n", ip, buffer.data);
            }

            if (evs[i].data.fd == STDIN_FILENO) {
                // 发送数据给服务器
                memset(&buffer, 0, sizeof(buffer));
                buffer.len = read(STDIN_FILENO, buffer.data, MAX_DATA_LEN);
                RET_CHECK(ret, -1, "read");


                // 发送，需要先收到客户端数据，确认客户端ip和port
                ret = sendto(server_fd, buffer.data, buffer.len - 1, 0, (struct sockaddr*)&serAddr, serlen);
                RET_CHECK(ret, -1, "sendto");
            }
        }
    }

    close(server_fd);
    return 0;
}
