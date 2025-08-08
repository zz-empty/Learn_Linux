#include "head.h"

#define MAX_NAME_LEN 64
#define MAX_IP_LEN 32
#define MAX_USERS 10
#define MAX_BUF_SIZE 256
#define MAX_DATA_SIZE 512

typedef struct {
    int datalen;
    char data[MAX_DATA_SIZE];
} Data_t;

// 用户信息
typedef struct User {
    int flag;
    int fd;
    char username[MAX_NAME_LEN];
} User_t;

// 聊天室信息
typedef struct {
    User_t *users;
    int size;
    int capacity;
} ChatRoom_t;

// 初始化聊天室
int initRoom(ChatRoom_t *room, int capacity) {
    memset(room, 0, sizeof(ChatRoom_t));
    room->users = (User_t*)calloc(capacity, sizeof(User_t));
    room->capacity = capacity;
    return 1;
}

// 销毁一个聊天室
int destoryRoom(ChatRoom_t *room) {
    free(room->users);
    room->size = 0;
    return 1;
}

// 加入聊天室
int joinRoom(ChatRoom_t *room, User_t *user) {
    if (room->size < room->capacity) {
        // 寻找空闲位置
        for (int i = 0; i < room->capacity; ++i) {
            if (0 == room->users[i].flag) {
                // 找到空闲位置
                memcpy(room->users + i, user, sizeof(User_t));
                room->users[i].flag = 1;
                // 增加size
                ++room->size;
                return 1;
            }
        }
    } 
    // 聊天室已满
    return 0;
}

// 设置fd为非阻塞
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    RET_CHECK(flags, -1, "fcntl_get");

    if (flags & O_NONBLOCK) {
        return 0;   // 已经是非阻塞
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

// 设置fd为非阻塞
int set_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    RET_CHECK(flags, -1, "fcntl_get");

    if (!(flags & O_NONBLOCK)) {
        return 0;   // 已经是非阻塞
    }

    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

// 初始化一个tcp的套接字，并开始监听
int tcp_listen(const char *ip, int port) {
    int ret = 0;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(port);

    // 设置套接字非阻塞
    set_nonblocking(server_fd);

    // 设置套接字可重用
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");

    // 开始监听客户端连接
    ret = listen(server_fd, 5);
    RET_CHECK(ret, -1, "listen");

    return server_fd;
}

int recv_full(int fd, Data_t *data) {
    int curlen = 0;
    int ret = 0;
    while (curlen < data->datalen) {
        ret = recv(fd, data->data + curlen, data->datalen - curlen, 0);
        curlen += ret;
        if (-1 == ret) {
            continue;
        } else if (0 == ret) {
            // 对端关闭
            return -1;
        }
    }
    return 1;
}

// 向epoll中注册描述符
int epoll_add(int epfd, int newfd) {
    struct epoll_event event = {};
    event.events = EPOLLIN | EPOLLET;   // 读事件，边沿触发
    event.data.fd = newfd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 1;
}

// 从epoll中删除描述符
int epoll_del(int epfd, int delfd) {
    struct epoll_event event = {};
    event.events = EPOLLIN | EPOLLET;   // 读事件，边沿触发
    event.data.fd = delfd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, delfd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 1;
}

int main()
{
    const char *ip = "127.0.0.1";
    const int port = 2000;
    int ret = 0;

    // 创建一个聊天室
    ChatRoom_t room;
    initRoom(&room, MAX_USERS);
    printf("[init] ---room init succeed! room.capacity:%d, room.size:%d\n", room.capacity, room.size);

    // 创建一个tcp套接字，并开始监听
    int server_fd = tcp_listen(ip, port);
    printf("[init] ---tcp listen start...\n");

    // 使用epoll监听
    int epfd = epoll_create(1);
    epoll_add(epfd, server_fd);

    int readyNum = 0;
    User_t user;
    char buf[MAX_BUF_SIZE] = "";
    Data_t data;
    struct epoll_event evs[2]; // 返回的就绪事件数组
    while (1) {
        readyNum = epoll_wait(epfd, evs, 2, -1);

        for (int k = 0; k < readyNum; ++k) {
            if (evs[k].data.fd == server_fd) {
                printf("[info] server_fd ready!\n");
                // 有新的客户端到来
                memset(&user, 0, sizeof(User_t));
                user.fd = accept(server_fd, NULL, 0);
                RET_CHECK(user.fd, -1, "accept");

                // 判断聊天室是否已满
                if (room.size == room.capacity) {
                    // 无法加入，聊天室已满
                    const char *msg = "room is fulled, cannot join!";
                    printf("[warning] %s\n", msg);

                    // 通知客户端加入失败
                    memset(&data, 0, sizeof(data));
                    data.datalen = -2;  // 已满标志
                    send(user.fd, &data.datalen, sizeof(int), 0);
                    close(user.fd);
                    continue;
                }

                // 加入监听
                epoll_add(epfd, user.fd);

                // 先接头长, 再接用户名
                memset(&data, 0, sizeof(data));
                ret = recv(user.fd, &data.datalen, sizeof(int), 0);
                ret = recv(user.fd, user.username, data.datalen, MSG_WAITALL);  // 等待全部数据
                RET_CHECK(ret, -1, "recv");
                printf("[info] username recv succeed! username=%s\n", user.username);

                // 加入聊天室
                joinRoom(&room, &user);
                printf("[info] %s join room succeed! room.size:%d, room.capacity:%d\n", user.username, room.size, room.capacity);
                // 给新用户发送欢迎信息
                memset(&data, 0, sizeof(data));
                sprintf(data.data, "Welcome %s join happy chatroom!", user.username);
                data.datalen = strlen(data.data);
                ret = send(user.fd, &data, sizeof(int) + data.datalen, 0);
                RET_CHECK(ret, -1, "send");
                // 通知所有在线的用户，新用户的到来
                for (int i = 0; i < room.capacity; ++i) {
                    if (room.users[i].flag && room.users[i].fd != user.fd) {
                        ret = send(room.users[i].fd, &data, sizeof(int) + data.datalen, 0);
                        RET_CHECK(ret, -1, "send");
                    }
                }
            }
            // 某个客户端发来信息, 进行转发
            else {
                for (int i = 0; i < room.capacity; ++i) {
                    if (room.users[i].flag && evs[k].data.fd == room.users[i].fd) {
                        // 接收客户端的信息
                        printf("[msg] ---%s msg, fd:%d\n", room.users[i].username, room.users[i].fd);
                        // 先接信息头长
                        memset(&data, 0, sizeof(data));
                        ret = recv(room.users[i].fd, &data.datalen, sizeof(int), 0);

                        // 如果是客户端意外退出，修改room配置，并通知其他所有在线的客户端
                        if (0 == ret) {
                            // 从监控的位图中删除
                            epoll_del(epfd, room.users[i].fd);
                            room.users[i].flag = 0;     // 用户下线
                            room.size--;                // 减少用户数量
                            printf("[info] user-%s exit! room.size:%d, room.capacity:%d\n", room.users[i].username, room.size, room.capacity);

                            // 拼接退出信息, 然后修改头长
                            memset(&data, 0, sizeof(data));
                            sprintf(data.data, "[chat room inform] %s exit!\n", room.users[i].username);
                            data.datalen = strlen(data.data);
                            // 通知其他在线的用户，退出信息
                            for (int j = 0; j < room.capacity; ++j) {
                                if (room.users[j].flag) {
                                    ret = send(room.users[j].fd, &data, sizeof(int) + data.datalen, 0);
                                    RET_CHECK(ret, -1, "send_inform_userexit");
                                }
                            }

                            break;
                        }

                        // 正常接收信息，转发给其他在线的客户端
                        memset(buf, 0, sizeof(buf));
                        ret = recv_full(room.users[i].fd, &data);
                        if (-1 == ret) {
                            printf("[error] server exit!\n");
                            return -1;
                        }
                        memcpy(buf, data.data, data.datalen);
                        /* ret = recv(room.users[i].fd, buf, data.datalen, MSG_WAITALL);   // 等待全部数据 */
                        /* RET_CHECK(ret, -1, "recv"); */
                        

                        // 拼接正常信息, 然后修改头长
                        sprintf(data.data, "[%s] %s", room.users[i].username, buf);
                        data.datalen = strlen(data.data);
                        for (int j = 0; j < room.capacity; ++j) {
                            if (room.users[j].flag && room.users[j].fd != room.users[i].fd) {
                                ret = send(room.users[j].fd, &data, sizeof(int) + data.datalen, 0);
                                RET_CHECK(ret, -1, "send");
                            }
                        }
                        break;
                    }
                } 
            } 
        }
    }

    close(server_fd);
    destoryRoom(&room);
    return 0;
}
