#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

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
