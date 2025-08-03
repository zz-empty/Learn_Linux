#include "head.h"

#define MAX_DATA_LEN 1024
typedef struct {
    int datasize;
    char data[MAX_DATA_LEN];
} Data_t;

int recvTotal(int socket_fd, char *buf, int len) {
    int ret = 0;
    int cur = 0;

    while (cur < len) {
        ret = recv(socket_fd, buf + cur, len - cur, 0);
        RET_CHECK(ret, -1, "recv");
        cur += ret;
        if (0 == ret) return -1;

    }
    return cur;
}

int receive_file(int server_fd) {
    // 根据协议：先接收数据长度，再根据数据长度接收数据内容
    // 先接收文件名
    Data_t data = {};
    char filename[MAX_DATA_LEN];
    int ret;

    ret = recv(server_fd, &data.datasize, sizeof(int), 0);
    ret = recv(server_fd, data.data, data.datasize, 0);
    RET_CHECK(ret, -1, "recv");
    strcpy(filename, data.data);

    // 接收文件大小
    off_t filesize = 0;
    bzero(&data, sizeof(Data_t));
    ret = recv(server_fd, &data.datasize, sizeof(4), 0);
    ret = recv(server_fd, &filesize, data.datasize, 0);

    printf("[info] ready receive %s, size = %ld\n", filename, filesize);

    // 再接收文件内容
    // 干干干，我勒个豆，让我找了半天的bug，我说怎么打不开mmap，不应该，结果是open的权限给错了，给成了WRONLY，干
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
    RET_CHECK(fd, -1, "open");


    // 使用mmap接收
    char *pMap = (char*)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    RET_CHECK(pMap, (char*)-1, "mmap");

    // 固定文件大小
    ftruncate(fd, filesize);

    // 一次性全部接收
    ret = recvTotal(server_fd, pMap, filesize);
    RET_CHECK(ret, -1, "recvTotal");

    // 内容全部接收完毕
    printf("[info] %s download over!\n", filename);

    munmap(pMap, filesize);
    close(fd);
    return 1;
}
