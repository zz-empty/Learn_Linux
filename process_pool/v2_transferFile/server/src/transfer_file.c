#include "process_pool.h"

#define MAX_DATA_LEN 1024
typedef struct {
    int datasize;
    char data[MAX_DATA_LEN];
} Data_t;

int transfer_file(int client_fd, const char *filename) {
    // 根据协议传输, 先传输数据长度，再传输数据内容
    Data_t data = {};
    int ret = 0;
    // 先传输文件名
    data.datasize = strlen(filename);
    strncpy(data.data, filename, MAX_DATA_LEN - 1);
    ret = send(client_fd, &data, sizeof(data.datasize) + data.datasize, 0);
    RET_CHECK(ret, -1, "send");

    // 传文件大小
    struct stat st;
    if (stat(filename, &st) < 0) {
        perror("stat");
        return -1;
    }
    data.datasize = sizeof(st.st_size);
    memcpy(data.data, &st.st_size, data.datasize);
    ret = send(client_fd, &data, sizeof(data.datasize) + data.datasize, 0);
    RET_CHECK(ret, -1, "send");

    // 再传输文件内容
    /* printf("path: %s\n", getcwd(NULL, 0)); */

    int fd = open(filename, O_RDWR);
    RET_CHECK(fd, -1, "open");

    while (1) {
        memset(&data, 0, sizeof(Data_t));
        data.datasize = read(fd, data.data, MAX_DATA_LEN - 1);
        if (0 == data.datasize) {
            // 传输完毕
            printf("[info] file transfer success!\n");
            send(client_fd, &data, sizeof(data.datasize), 0);
            break;
        }

        /* printf("datasize = %d\n", data.datasize); */

        // 发送给客户端
        ret = send(client_fd, &data, sizeof(data.datasize) + data.datasize, 0);
        RET_CHECK(ret, -1, "send");


#if 0
        // 确定客户端正确接收
        ret = recv(client_fd, &data.datasize, 1, 0);
        if (0 == ret) {
            printf("[warning] client exit!\n");
            break;
        }
#endif
    }

    close(fd);
    return 1;
}
