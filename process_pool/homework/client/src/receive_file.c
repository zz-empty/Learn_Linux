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
    while (1) {
        memset(&data, 0, sizeof(Data_t));
        ret = recv(serverFd, &data.len, sizeof(int), 0);
        RET_CHECK(ret, -1, "recv");
        if (0 == data.len) {
            // 文件已全部接收完毕
            printf("[info] 收到全部数据！\n");
            break;
        }
        ret = recv(serverFd, &data.data, data.len, 0);
        RET_CHECK(ret, -1, "recv");

        // 写入文件
        ret = write(fd, data.data, data.len);
        RET_CHECK(ret, -1, "write");
    }
    

    close(fd);
    return 0;
}
