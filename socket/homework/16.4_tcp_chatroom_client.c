#include "head.h"

#define MAX_BUF_SIZE 256
#define MAX_DATA_SIZE 512
#define MAX_NAME_LEN 64

typedef struct {
    int datalen;
    char data[MAX_DATA_SIZE];
} Data_t;

int main(int argc, char **argv)
{
    // ./app username
    ARGS_CHECK(argc, 2);
    const char *ip = "127.0.0.1";
    const int port = 2000;
    char username[MAX_NAME_LEN] = "";
    strcpy(username, argv[1]);
    int ret = 0;
    Data_t data = {};

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(port);

    // 连接服务器
    ret = connect(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "connect");
    printf("[init] ---server connect succeed!\n");

    // 发送用户名
    data.datalen = strlen(username);
    memcpy(data.data, username, data.datalen);
    ret = send(server_fd, &data, sizeof(int) + data.datalen, 0);
    RET_CHECK(ret, -1, "send_username");
    printf("[init] ---username send succedd!\n\n");

    // 建立select监听
    fd_set rdset;
    fd_set monitor;
    FD_ZERO(&monitor);
    FD_SET(server_fd, &monitor);
    FD_SET(STDIN_FILENO, &monitor);
    int maxfd = server_fd;

    int readyNum = 0;
    while (1) {
        memcpy(&rdset, &monitor, sizeof(fd_set));
        readyNum = select(maxfd + 1, &rdset, NULL, NULL, NULL);
        RET_CHECK(readyNum, -1, "select");

        for (int k = 0; k < readyNum; ++k) {
            if (FD_ISSET(server_fd, &rdset)) {
                // 聊天室的其他用户信息
                memset(&data, 0, sizeof(data));
                // 先接收信息的长度
                ret = recv(server_fd, &data.datalen, sizeof(int), 0);
                RET_CHECK(ret, -1, "recv");
                if (-2 == data.datalen) {
                    // 聊天室已满
                    printf("[error] room full, cannot join\n");
                    return -1;
                }
                if (0 == ret) {
                    // 服务器退出
                    printf("[error] server exit!\n");
                    return -1;
                }
                // 接收指定长度的信息
                ret = recv(server_fd, data.data, data.datalen, 0);
                RET_CHECK(ret, -1, "recv");


                // 显示信息
                printf("%s\n", data.data);
            }
            if (FD_ISSET(STDIN_FILENO, &rdset)) {
                // 发送信息给服务器
                memset(&data, 0, sizeof(data));
                ret = read(STDIN_FILENO, data.data, sizeof(data.data) - 1);

                // 发送
                data.datalen = strlen(data.data) - 1;
                ret = send(server_fd, &data, sizeof(int) + data.datalen, 0);
                RET_CHECK(ret, -1, "send");
            }
        }
    }

    close(server_fd);
    return 0;
}
