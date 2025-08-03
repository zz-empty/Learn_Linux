#ifndef __PROCESS_POOL_H__
#define __PROCESS_POOL_H__

#include "head.h"

#define MAX_IP_LEN 64
#define MAX_LINE_LEN 256

// 工作线程的状态、通道和数据
typedef struct {
    short busy;
    int pipefd;
    pid_t pid;
} Worker_t;

// 进程池
typedef struct {
    Worker_t *workers;
    int size;
} Pool_t;

// 配置信息结构
typedef struct {
    char ip[MAX_IP_LEN];
    int port;
    int workers;
    char dir[MAX_LINE_LEN];
} Config_t;

// 加载配置文件
int load_config(const char *filename, Config_t *cfg);
// 创建进程池
int create_pool(Pool_t *pool);
// 向管道里传入打开的文件描述符
int send_fd(int pipefd, int fd, char exitflag);
// 从管道中读取打开的文件描述符
int recv_fd(int pipefd, int *fd, char *exitflag);

#endif
