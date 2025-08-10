#define _GNU_SOURCE
#include "head.h"

#define DATA_SIZE 1024

typedef struct {
    int len;
    char data[DATA_SIZE];
} data_t;

int receiveFile(int serverFd) {
    // 先接文件名，再接文件大小，再接文件内容
    char filename[DATA_SIZE] = "";
    int filesize = 0;
    data_t data = {};
    int ret = 0;

    ret = recv(serverFd, &data.len, sizeof(int), 0);
    ret = recv(serverFd, filename, data.len, 0);
    RET_CHECK(ret, -1, "recv_filename");
    printf("[info] ready receive :[%s]\n", filename);

    ret = recv(serverFd, &filesize, sizeof(int), 0);
    RET_CHECK(ret, -1, "recv_filesize");
    printf("[info] filesize :%d", filesize);

    // 接收文件内容写入文件
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    RET_CHECK(fd, -1, "open");

    int curLen = 0;
    int fds[2];
    pipe(fds);
    while (curLen < filesize) {
        ret = splice(serverFd, 0, fds[1], 0, filesize, 0);
        ret = splice(fds[0], 0, fd, 0, ret, 0);
        curLen += ret;

        // 打印进度条
        show_progress(curLen, filesize, 80);
    }

    close(fd);
    return 0;
}
