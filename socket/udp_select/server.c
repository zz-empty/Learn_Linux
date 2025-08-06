#include "head.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    // ./server ip port
    ARGS_CHECK(argc, 3);
    int ret = 0;

    // 1. 创建服务器监听套接字
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    // 2. 绑定服务器的ip和port
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(serAddr.sin_family, argv[1], &serAddr.sin_addr.s_addr);

    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");

    // 3. 建立select监听
    fd_set initset;
    fd_set fdset;
    FD_ZERO(&initset);
    FD_SET(STDIN_FILENO, &initset);
    FD_SET(server_fd, &initset);
    int maxfd = server_fd;

    // 用于接收客户端的ip和port
    struct sockaddr_in cliAddr;
    socklen_t clilen = 0;

    char buf[10] = "";
    int readyNum = 0;
    char ip[64] = "";
    int port = 0;
    while (1) {
        FD_ZERO(&fdset);
        memcpy(&fdset, &initset, sizeof(fd_set));

        readyNum = select(maxfd + 1, &fdset, NULL, NULL, NULL);
        RET_CHECK(ret, -1, "select");

        // 处理所有就绪的fd
        for (int i = 0; i < readyNum; ++i) {
            if (FD_ISSET(server_fd, &fdset)) {
                // 接收客户端信息，并重置客户端socket
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(server_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&cliAddr, &clilen);
                RET_CHECK(ret, -1, "recvfrom");

                // 显示
                if (ret) {
                    inet_ntop(AF_INET, &cliAddr.sin_addr.s_addr, ip, sizeof(ip)); 
                    port = ntohs(cliAddr.sin_port);
                    printf("[client ip:%s-port:%d] %s\n", ip, port, buf);
                }
            }
            if (FD_ISSET(STDIN_FILENO, &fdset)) {
                // 读取stdin
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                RET_CHECK(ret, -1, "read");

                // 发送给客户端, 去掉换行符
                ret = sendto(server_fd, buf, ret - 1, 0, (struct sockaddr*)&cliAddr, clilen);
                RET_CHECK(ret, -1, "sendto");
            }
        }
    }

    close(server_fd);
    return 0;
}

