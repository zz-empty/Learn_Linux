#define _GNU_SOURCE
#include "head.h"

#define DATA_SIZE 1024

typedef struct {
    int len;
    char data[DATA_SIZE];
} Data_t;

int receiveFile(int serverFd) {
    char filename[DATA_SIZE] = "";
    int filesize = 0;
    int ret = 0;
    Data_t data = {};

    // 先接收文件名
    ret = recv(serverFd, &data.len, sizeof(int), 0);
    ret = recv(serverFd, data.data, data.len, 0);
    memcpy(filename, data.data, DATA_SIZE);
    printf("[info] recv filename [%s]\n", filename);


    // 再接收文件大小
    ret = recv(serverFd, &filesize, sizeof(int), 0);
    printf("[info] recv filesize [%d]\n", filesize);
    

    // 再接收文件内容
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    RET_CHECK(fd, -1, "open");

    printf("[info] start receive...\n");
    int curLen = 0;
    int fds[2];
    pipe(fds);
    while (curLen < filesize) {
        ret = splice(serverFd, 0, fds[1], 0, filesize, 0);
        ret = splice(fds[0], 0, fd, 0, ret, 0);

        // 打印进度条
        curLen += ret;
        show_progress(curLen, filesize, 80);
    }
    printf("\n[info] download over!\n");

    close(fd);
    return 0;
}
