#include "head.h"

#define BUFSIZE 1024

int main(int argc, char **argv)
{
    // ./server ip port
    ARGS_CHECK(argc, 3);
    int ret = 0;

    // 创建一个服务器监听socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    // 设置socket可重用
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定服务器的ip和port
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serAddr.sin_port = htons(atoi(argv[2]));

    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");
    printf("---[server] init server_fd---\n");

    // 监听客户端的连接
    ret = listen(server_fd, 10);
    printf("---[server] listening...\n");

    // 设置select多路复用的参数
    fd_set fdset;       // select的临时位图
    fd_set initset;     // select轮询的原始位图
    FD_ZERO(&initset);
    FD_SET(STDIN_FILENO, &initset);
    FD_SET(server_fd, &initset);
    // select轮询的范围
    int maxfd = server_fd;

    int readyNum = 0;
    int cnt = 0;
    int newfd = 0;
    char buf[BUFSIZE] = "";
    while (1) {
        FD_ZERO(&fdset);
        memcpy(&fdset, &initset, sizeof(fd_set));

        // 轮询检查就绪的fd
        readyNum = select(maxfd + 1, &fdset, NULL, NULL, NULL);
        RET_CHECK(readyNum, -1, "select");

        // 处理所有就绪的fd
        cnt = 0;
        for (int i = 0; i <= maxfd && cnt != readyNum; ++i) {
            if (FD_ISSET(i, &fdset)) {
                if (i == server_fd) {
                    // 有新的fd到来，加入轮询位图
                    newfd = accept(server_fd, NULL, 0); 
                    RET_CHECK(newfd, -1, "accept");
                    FD_SET(newfd, &initset);
                    // 修改maxfd
                    maxfd = maxfd > newfd ? maxfd : newfd;
                    // 显示信息
                    printf("---[server] new client_fd %d connact...\n", newfd);
                    cnt++;
                } else if (i == STDIN_FILENO) {
                    // 读取标准输入
                    memset(buf, 0, sizeof(buf));
                    ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                    RET_CHECK(ret, -1, "read");

                    // 给最后一个连接的客户端发信息
                    // 发送信息时，去掉换行符
                    ret = send(newfd, buf, ret - 1, 0);
                    RET_CHECK(ret, -1, "send");
                    cnt++;
                } else if (i > 3) {
                    // 跳过0/1/2/3，接收客户端的请求
                    memset(buf, 0, sizeof(buf));
                    ret = recv(i, buf, sizeof(buf) - 1, 0);
                    RET_CHECK(ret, -1, "recv");
                    // 如果客户端断开连接，关闭client_fd，并在轮询位图中删除
                    if (0 == ret) {
                        printf("---[server] client_fd %d already over!!!...\n", i);
                        close(i);
                        FD_CLR(i, &initset);
                    }

                    // 展示客户端的数据
                    printf("[client %d] %s\n", i, buf);
                    cnt++;
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
