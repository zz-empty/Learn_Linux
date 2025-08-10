#define _GNU_SOURCE
#include "head.h"

#define DATA_SIZE 1024

typedef struct {
    int len;
    char data[DATA_SIZE];
} data_t;

int transferFile(int clientFd) {
    // 先发送文件名，再发送文件大小，再发文件内容
    const char *filename = "file1";
    data_t data = {};
    int ret = 0;

    data.len = strlen(filename);
    strncpy(data.data, filename, DATA_SIZE);
    ret = send(clientFd, &data, sizeof(int) + data.len, 0);
    RET_CHECK(ret, -1, "send_filename");

    struct stat st = {};
    stat(filename, &st);
    data.len = st.st_size;
    ret = send(clientFd, &data, sizeof(int), 0);
    RET_CHECK(ret, -1, "send_filename");

    // 发送文件内容
    int fd = open(filename, O_RDONLY);
    RET_CHECK(fd, -1, "open");

    int curLen = 0;
    int filesize = st.st_size;
    int fds[2];
    pipe(fds);
    /* int rate = 10; */
    int rate = filesize;
    while (curLen < filesize) {
        ret = splice(fd, 0, fds[1], 0, rate, 0);
        ret = splice(fds[0], 0, clientFd, 0, ret, 0);
        curLen += ret;
    }

    close(fd);
    return 0;
}
