#include "head.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    // ./client ip port
    ARGS_CHECK(argc, 3);
    int ret = 0;

    // 创建socket, 用于连接服务器
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    // 设置服务器的ip和port
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serAddr.sin_addr.s_addr);

    // 连接服务器
    ret = connect(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "connect");

    // 设置select多路复用的参数
    fd_set fdset;       // select的临时位图
    fd_set initset;     // select轮询的原始位图
    FD_ZERO(&initset);
    FD_SET(STDIN_FILENO, &initset);
    FD_SET(server_fd, &initset);
    // select轮询的范围
    int maxfd = server_fd;

    int readyNum = 0;
    char buf[BUFSIZE] = "";
    while (1) {
        FD_ZERO(&fdset);
        memcpy(&fdset, &initset, sizeof(fd_set));

        // 轮询检查就绪的fd
        readyNum = select(maxfd + 1, &fdset, NULL, NULL, NULL);
        RET_CHECK(readyNum, -1, "select");

        // 处理所有就绪的fd
        for (int i = 0; i < readyNum; ++i) {
            if (FD_ISSET(STDIN_FILENO, &fdset)) {
                // 读取标准输入
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                RET_CHECK(ret, -1, "read");

                // 发送信息时，去掉换行符
                ret = send(server_fd, buf, ret - 1, 0);
                RET_CHECK(ret, -1, "send");
            }

            if (FD_ISSET(server_fd, &fdset)) {
                // 接收服务器的数据
                memset(buf, 0, sizeof(buf));
                ret = recv(server_fd, buf, sizeof(buf) - 1, 0);
                RET_CHECK(ret, -1, "recv");
                // 如果服务器断开连接，退出程序
                if (0 == ret) {
                    printf("---- [server exit!]-----\n");
                    close(server_fd);
                    return 0;
                }

                // 展示服务器的数据 
                printf("[server %d] %s\n", server_fd, buf);
            }
        }
    }

    close(server_fd);
    return 0;
}
