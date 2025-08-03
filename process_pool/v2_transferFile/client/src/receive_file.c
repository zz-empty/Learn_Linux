#include "head.h"

#define MAX_DATA_LEN 1024
typedef struct {
    int datasize;
    char data[MAX_DATA_LEN];
} Data_t;

int recvTotal(int socket_fd, void *buf, int len) {
    int ret = 0;
    int cur = 0;

    while (cur < len) {
        ret = recv(socket_fd, (char*)buf + cur, len - cur, 0);
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
    off_t cursize = 0;
    bzero(&data, sizeof(Data_t));
    ret = recv(server_fd, &data.datasize, sizeof(4), 0);
    ret = recv(server_fd, &filesize, data.datasize, 0);

    printf("[info] ready receive %s, size = %ld\n", filename, filesize);

    // 再接收文件内容
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    RET_CHECK(fd, -1, "open");
    int cnt = 0;
    float rate;     // 下载的百分比
    int width = 50; // 进度条宽度
    int filled = 0;
    while (1) {
        // 先接收本段数据字节数
        ret = recv(server_fd, &data.datasize, sizeof(int), 0);
        RET_CHECK(ret, -1, "recv");

        if (0 == ret) {
            printf("[error] server exit!\n");
            break;
        }

        // 打印进度条
        cursize += data.datasize;
        cnt++;
        if (cnt % 1 == 0) {
            rate = (float)cursize / filesize;
            filled = rate * width;

            printf("\r[%.*s%*s] %6.2f%%",
                   filled,
                   "##################################################",
                   width - filled,
                   "",
                   rate * 100);
            fflush(stdout);
        }

        /* printf("datasize = %d\n", data.datasize); */
        if (0 == data.datasize) {
            // 内容全部接收完毕
            printf("\n[info] %s download over!\n", filename);
            break;
        }

        // 按字节数接收内容，不接收完不停止
        /* ret = recv(server_fd, data.data, data.datasize, MSG_WAITALL); */
        ret = recvTotal(server_fd, data.data, data.datasize);
        RET_CHECK(ret, -1, "recv");

#if 0
        // 通知服务器本次接收成功
        ret = send(server_fd, "a", sizeof(char), 0);
        RET_CHECK(ret, -1, "send");
#endif

        // 写入文件
        ret = write(fd, data.data, data.datasize);
        RET_CHECK(ret, -1, "write");
    }
    close(fd);

    return 1;
}
