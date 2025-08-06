#include "head.h"

#define BUFSIZE 6

int main(int argc, char **argv)
{
    // ./client ip port
    ARGS_CHECK(argc, 3);
    int ret = 0;

    // 1. 创建套接字, 连接服务器
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    // 2. 设置服务器的ip和port
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(serAddr.sin_family, argv[1], &serAddr.sin_addr.s_addr);
    socklen_t serlen = sizeof(serAddr);


    // 3. 建立select监听
    fd_set initset;
    fd_set fdset;
    FD_ZERO(&initset);
    FD_SET(STDIN_FILENO, &initset);
    FD_SET(server_fd, &initset);
    int maxfd = server_fd;

    char buf[BUFSIZE] = "";
    int readyNum = 0;
    while (1) {
        FD_ZERO(&fdset);
        memcpy(&fdset, &initset, sizeof(fd_set));

        readyNum = select(maxfd + 1, &fdset, NULL, NULL, NULL);
        RET_CHECK(ret, -1, "select");

        // 处理所有就绪的fd
        for (int i = 0; i < readyNum; ++i) {
            if (FD_ISSET(server_fd, &fdset)) {
                // 接收服务器消息
                ret = recvfrom(server_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&serAddr, &serlen);
                RET_CHECK(ret, -1, "recvfrom");

                // 显示
                printf("[server] %s\n", buf);
            }
            if (FD_ISSET(STDIN_FILENO, &fdset)) {
                // 读取stdin
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                RET_CHECK(ret, -1, "read");

                // 给服务器发请求, 去掉换行符
                ret = sendto(server_fd, buf, ret - 1, 0, (struct sockaddr*)&serAddr, serlen);
                RET_CHECK(ret, -1, "sendto");
            }
        }
    }

    close(server_fd);
    return 0;
}

