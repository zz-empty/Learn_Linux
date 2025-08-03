#include "head.h"

#define MAX_DATA_LEN 1024
typedef struct {
    int datasize;
    char data[MAX_DATA_LEN];
} Data_t;


int epoll_add(int epfd, int fd) {
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = fd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 1;
}

int receive_file(int server_fd) {
    // 根据协议：先接收数据长度，再根据数据长度接收数据内容
    // 先接收文件名
    Data_t data = {};
    int ret;

    ret = recv(server_fd, &data.datasize, sizeof(int), 0);
    ret = recv(server_fd, data.data, data.datasize, 0);
    RET_CHECK(ret, -1, "recv");

    printf("ready receive [%s]\n", data.data);

    // 再接收文件内容
    char filename[MAX_DATA_LEN];
    strcpy(filename, data.data);
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    RET_CHECK(fd, -1, "open");
    printf("download...\n");
    while (1) {
        // 接收的字节数
        ret = recv(server_fd, &data.datasize, sizeof(int), 0);
        RET_CHECK(ret, -1, "recv");

        if (0 == ret) {
            printf("server exit!\n");
            break;
        }

        /* printf("datasize = %d\n", data.datasize); */
        if (0 == data.datasize) {
            // 接收完毕
            break;
        }

        // 按字节数接收内容
        ret = recv(server_fd, data.data, data.datasize, 0);
        RET_CHECK(ret, -1, "recv");

        // 写入文件
        ret = write(fd, data.data, data.datasize);
        RET_CHECK(ret, -1, "write");
    }
    close(fd);
    printf("[%s] download over!\n", filename);

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

    printf("success connect server!------\n");

    // 接收文件
    receive_file(server_fd);

#if 0
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
#endif
    close(server_fd);

    return 0;
}

