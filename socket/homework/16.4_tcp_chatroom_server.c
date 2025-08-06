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


int main()
{
    const char *ip = "127.0.0.1";
    const int port = 2000;
    int ret = 0;

    // 创建一个聊天室
    ChatRoom_t room;
    initRoom(&room, MAX_USERS);
    printf("[init] ---room init succeed! room.capacity:%d, room.size:%d\n", room.capacity, room.size);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(port);

    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");

    // 开始监听客户端连接
    ret = listen(server_fd, 5);
    RET_CHECK(ret, -1, "listen");
    printf("[init] ---tcp listen start...\n");

    // 建立select监听
    fd_set rdset;
    fd_set monitor;
    FD_ZERO(&monitor);
    FD_SET(server_fd, &monitor);
    int maxfd = server_fd;

    int readyNum = 0;
    User_t user;
    char buf[MAX_BUF_SIZE] = "";
    Data_t data;
    while (1) {
        memcpy(&rdset, &monitor, sizeof(fd_set));
        readyNum = select(maxfd + 1, &rdset, NULL, NULL, NULL);
        RET_CHECK(readyNum, -1, "select");

        for (int k = 0; k < readyNum; ++k) {
            if (FD_ISSET(server_fd, &rdset)) {
                printf("[info] server_fd ready!\n");
                // 有新的客户端到来
                memset(&user, 0, sizeof(User_t));
                user.fd = accept(server_fd, NULL, 0);
                RET_CHECK(user.fd, -1, "accept");

                // 加入监听，修改最大描述符
                FD_SET(user.fd, &monitor);
                maxfd = maxfd > user.fd ? maxfd : user.fd;

                // 先接头长, 再接用户名
                memset(&data, 0, sizeof(data));
                ret = recv(user.fd, &data.datalen, sizeof(int), 0);
                ret = recv(user.fd, user.username, data.datalen, 0);
                RET_CHECK(ret, -1, "recv");
                printf("[info] username recv succeed! username=%s\n", user.username);

                // 加入聊天室
                ret = joinRoom(&room, &user);
                if (0 == ret) {
                    // 加入失败，聊天室已满
                    const char *msg = "room is fulled, cannot join!";
                    printf("[warning] %s\n", msg);

                    // 通知客户端加入失败
                    send(user.fd, "1", 1, 0);
                } else {
                    // 加入成功，
                    printf("[info] %s join room succeed! room.size:%d, room.capacity:%d\n", user.username, room.size, room.capacity);
                    // 拼接欢迎信息
                    memset(&data, 0, sizeof(data));
                    sprintf(data.data, "[chat room inform] new user come : %s!", user.username);
                    data.datalen = strlen(data.data);
                    // 通知所有在线的用户，新用户的到来
                    for (int i = 0; i < room.capacity; ++i) {
                        if (room.users[i].fd != user.fd && room.users[i].flag) {
                            ret = send(room.users[i].fd, &data, sizeof(int) + data.datalen, 0);
                            RET_CHECK(ret, -1, "send");
                        }
                    }
                }
            }
            // 某个客户端发来信息, 进行转发
            else {
                for (int i = 0; i < room.capacity; ++i) {
                    if (room.users[i].flag && FD_ISSET(room.users[i].fd, &rdset)) {
                        // 接收客户端的信息
                        printf("[msg] ---%s msg, fd:%d\n", room.users[i].username, room.users[i].fd);
                        // 先接信息头长
                        memset(&data, 0, sizeof(data));
                        ret = recv(room.users[i].fd, &data.datalen, sizeof(int), 0);

                        // 如果是客户端意外退出，修改room配置，并通知其他所有在线的客户端
                        if (0 == ret) {
                            room.users[i].flag = 0;     // 用户下线
                            room.size--;                // 减少用户数量
                            printf("[info] user-%s exit! room.size:%d, room.capacity:%d\n", room.users[i].username, room.size, room.capacity);

                            memset(&data, 0, sizeof(data));
                            sprintf(data.data, "[chat room inform] %s exit!\n", room.users[i].username);
                            // 通知其他在线的用户，退出信息
                            for (int j = 0; j < room.capacity && room.users[j].flag; ++j) {
                                ret = send(room.users[j].fd, &data, sizeof(int) + data.datalen, 0);
                                RET_CHECK(ret, -1, "send_inform_userexit");
                            }
                            break;
                        }

                        // 正常接收信息，转发给其他在线的客户端
                        memset(buf, 0, sizeof(buf));
                        ret = recv(room.users[i].fd, buf, data.datalen, 0);
                        RET_CHECK(ret, -1, "recv");

                        sprintf(data.data, "[%s] %s", room.users[i].username, buf);
                        data.datalen = strlen(data.data);
                        for (int j = 0; j < room.capacity && room.users[j].flag; ++j) {
                            if (room.users[j].fd != room.users[i].fd) {
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
    return 0;
}
