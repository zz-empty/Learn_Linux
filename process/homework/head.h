#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <syslog.h>

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
