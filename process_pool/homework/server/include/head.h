#ifndef __HEAD_H__
#define __HEAD_H__

#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>

// 检查命令行参数个数是否正确
#define ARGS_CHECK(argc, num)                                                  \
  {                                                                            \
    if (argc != num) {                                                         \
      fprintf(stderr, "args error!\n");                                        \
      return -1;                                                               \
    }                                                                          \
  }

// 检查系统调用返回值
#define RET_CHECK(ret, num, msg)                                               \
  {                                                                            \
    if (ret == num) {                                                          \
      perror(msg);                                                             \
      return -1;                                                               \
    }                                                                          \
  }

// 检查线程接口的返回值
#define PTHREAD_RET_CHECK(ret, msg)                                            \
  do {                                                                         \
    if (ret != 0) {                                                            \
      printf("%s:%s\n", msg, strerror(ret));                                   \
    }                                                                          \
  } while (0)


#define IP_SIZE 34
#define LINE_SIZE 1024

// 配置信息结构
typedef struct {
    char *ip;
    int port;
    int workers;
    char *home;
} Config_t;

// 配置文件
int loadConfig(const char *filename, Config_t *cfg);
void printConfig(Config_t *cfg);
void freeConfig(Config_t *cfg);


int transfer_file(int client_fd, const char *filename);

#endif
