#include "head.h"

#define BUFSIZE 1024
#define BUFMIN 5

int main(int argc, char **argv)
{
    // ./server ip port
    ARGS_CHECK(argc, 3);
    int ret = 0;

    // 1. 创建一个服务器监听socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    // 2. 设置socket可重用
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 3. 绑定服务器的ip和port
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serAddr.sin_port = htons(atoi(argv[2]));

    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");
    printf("---[server] init server_fd---\n");

    // 4. 监听客户端的连接
    ret = listen(server_fd, 10);
    printf("---[server] listening...\n");

    // 5. 使用epoll多路复用
    int epfd = epoll_create(1);
    RET_CHECK(epfd, -1, "epoll_event");

    // 将server_fd 和 STDIN_FILENO 加入epfd RB树中
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, event.data.fd, &event);
    RET_CHECK(ret, -1, "epoll_add");
    event.data.fd = STDIN_FILENO;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, event.data.fd, &event);
    RET_CHECK(ret, -1, "epoll_add");


    // 6. while处理所有响应的fd
    int readyNum = 0;
    int newfd = 0;
    char buf[BUFSIZE] = "";
    /* char buf[BUFMIN] = "";  // 测试ET模式的接收数据量 */

    struct epoll_event evs[2] = {};
    while (1) {
        // 返回就绪的fd
        readyNum = epoll_wait(epfd, evs, 2, -1);
        RET_CHECK(readyNum, -1, "epoll_wait");

        for (int i = 0; i < readyNum; ++i) {
            if (evs[i].data.fd == server_fd) {
                // 有新的客户端到来
                newfd = accept(server_fd, NULL, 0); 
                RET_CHECK(newfd, -1, "accept");

                // 加入epfd红黑树
                // 设置ET模式
                event.events = EPOLLET | EPOLLIN;
                event.data.fd = newfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, event.data.fd, &event);
                RET_CHECK(ret, -1, "epoll_add");
                printf("---[server] new client_fd %d connact...\n", newfd);
            }
            else if (evs[i].data.fd == STDIN_FILENO) {
                // stdin响应，给最后一个客户端发信息
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                RET_CHECK(ret, -1, "read");

                // 发送信息给客户端
                ret = send(newfd, buf, ret - 1, 0);
                RET_CHECK(ret, -1, "send");
            }
            else {
                // 某个客户端请求，读取
                int client_fd = evs[i].data.fd;

                // 由于是ET模式，需要一次性读取全部数据
                while (1) {
                    memset(buf, 0, sizeof(buf));
                    // recv 设置非阻塞
                    ret = recv(client_fd, buf, sizeof(buf) - 1, MSG_DONTWAIT);

                    if (0 == ret) {
                        // 如果客户端断开,关闭这次连接, 从epfd中删除
                        printf("---[server] client_fd %d already over!!!...\n", evs[i].data.fd);
                        event.data.fd = client_fd;
                        ret = epoll_ctl(epfd, EPOLL_CTL_DEL, event.data.fd, &event);
                        RET_CHECK(ret, -1, "epoll_del");
                        // 断开连接
                        close(client_fd);
                        // 继续下一次wait
                        break;
                    }
                    else if (-1 == ret) {
                        // 本次数据已读完，没有其他数据了，退出recv循环
                        break;
                    }
                    else if (0 < ret) {
                        // 打印客户端请求
                        printf("[client %d] %s\n", client_fd, buf);
                    }
                    else {
                        // 其他错误, 关闭fd
                        perror("recv");
                        close(client_fd);
                        break;
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
