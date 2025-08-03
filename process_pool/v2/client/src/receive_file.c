#include "head.h"

#define MAX_DATA_LEN 1024
typedef struct {
    int datasize;
    char data[MAX_DATA_LEN];
} Data_t;

int receive_file(int server_fd) {
    // 根据协议：先接收数据长度，再根据数据长度接收数据内容
    // 先接收文件名
    Data_t data = {};
    int ret;

    ret = recv(server_fd, &data.datasize, sizeof(int), 0);
    ret = recv(server_fd, data.data, data.datasize, 0);
    RET_CHECK(ret, -1, "recv");

    printf("[info] ready receive [%s]\n", data.data);

    // 再接收文件内容
    char filename[MAX_DATA_LEN];
    strcpy(filename, data.data);
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    RET_CHECK(fd, -1, "open");
    while (1) {
        // 先接收本段数据字节数
        ret = recv(server_fd, &data.datasize, sizeof(int), 0);
        RET_CHECK(ret, -1, "recv");

        if (0 == ret) {
            printf("[error] server exit!\n");
            break;
        }

        /* printf("datasize = %d\n", data.datasize); */
        if (0 == data.datasize) {
            // 内容全部接收完毕
            printf("[info] %s download over!\n", filename);
            break;
        }

        // 按字节数接收内容
        ret = recv(server_fd, data.data, data.datasize, 0);
        RET_CHECK(ret, -1, "recv");

        // 通知服务器本次接收成功
        ret = send(server_fd, "a", sizeof(char), 0);
        RET_CHECK(ret, -1, "send");

        // 写入文件
        ret = write(fd, data.data, data.datasize);
        RET_CHECK(ret, -1, "write");
    }
    close(fd);

    return 1;
}
