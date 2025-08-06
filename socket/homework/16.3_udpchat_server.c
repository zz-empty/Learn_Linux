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

    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");
    printf("[info] server start...\n");

    // 监控server_fd和STDIN_FILENO
    fd_set monitor;
    FD_ZERO(&monitor);
    FD_SET(server_fd, &monitor);
    FD_SET(STDIN_FILENO, &monitor);

    fd_set rdset;
    int readyNum;
    struct sockaddr_in cliAddr = {};
    socklen_t clilen;
    char buf[1024] = "";
    char ip[INET_ADDRSTRLEN] = "";
    while (1) {
        // 拷贝monitor
        memcpy(&rdset, &monitor, sizeof(fd_set));
        readyNum = select(server_fd + 1, &rdset, NULL, NULL, NULL);
        RET_CHECK(readyNum, -1, "select");

        for (int i = 0; i < readyNum; ++i) {
            if (FD_ISSET(server_fd, &rdset)) {
                // 对端来消息
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(server_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&cliAddr, &clilen);
                RET_CHECK(ret, -1, "recvfrom");
                inet_ntop(AF_INET, &cliAddr.sin_addr, ip, INET_ADDRSTRLEN);

                // 显示消息
                printf("[client ip-%s] %s\n", ip, buf);
            }
            if (FD_ISSET(STDIN_FILENO, &rdset)) {
                // 发送信息给客户端, 必须先接收到一个客户端信息，才知道客户单的socket
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);

                ret = sendto(server_fd, buf, ret - 1, 0, (struct sockaddr*)&cliAddr, clilen);
                RET_CHECK(ret, -1, "sendto");
            }
        }
    }


    close(server_fd);
    return 0;
}

