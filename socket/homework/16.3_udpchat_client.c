#include "head.h"

int main(int argc, char **argv)
{
    // ./app ip port
    ARGS_CHECK(argc, 3);
    int ret = 0;

    // 建立一个udp聊天
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serAddr.sin_port = htons(atoi(argv[2]));
    socklen_t serlen = sizeof(serAddr);

    // 监控server_fd和STDIN_FILENO
    fd_set monitor;
    FD_ZERO(&monitor);
    FD_SET(server_fd, &monitor);
    FD_SET(STDIN_FILENO, &monitor);

    fd_set rdset;
    int readyNum;
    char buf[1024] = "";
    printf("[info] client start...\n");
    while (1) {
        // 拷贝monitor
        memcpy(&rdset, &monitor, sizeof(fd_set));
        readyNum = select(server_fd + 1, &rdset, NULL, NULL, NULL);
        RET_CHECK(readyNum, -1, "select");

        for (int i = 0; i < readyNum; ++i) {
            if (FD_ISSET(server_fd, &rdset)) {
                // 服务器的回复
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(server_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&serAddr, &serlen);
                RET_CHECK(ret, -1, "recvfrom");

                // 显示消息
                printf("[server ip-%s] %s\n", argv[1], buf);
            }
            if (FD_ISSET(STDIN_FILENO, &rdset)) {
                // 发送信息给服务器, 需要先发送，才能建立通信
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);

                ret = sendto(server_fd, buf, ret - 1, 0, (struct sockaddr*)&serAddr, serlen);
                RET_CHECK(ret, -1, "sendto");
            }
        }
    }


    close(server_fd);
    return 0;
}

