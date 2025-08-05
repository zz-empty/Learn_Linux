#define _GNU_SOURCE
#include "pthread_pool.h"

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

    // 再传文件大小
    struct stat st;
    if (stat(filename, &st) < 0) {
        perror("stat");
        return -1;
    }
    printf("[info] filesize = %ld\n", st.st_size);
    data.datasize = sizeof(st.st_size);
    memcpy(data.data, &st.st_size, data.datasize);
    ret = send(client_fd, &data, sizeof(data.datasize) + data.datasize, 0);
    RET_CHECK(ret, -1, "send");

    // 再传输文件内容
    int fd = open(filename, O_RDWR);
    RET_CHECK(fd, -1, "open");

#if 0
    // 使用零拷贝接口 mmap
    char *pMap = (char*)mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    RET_CHECK(pMap, (char*)-1, "mmap");

    // 一次性发送整个文件
    int total = 0;
    while (total < st.st_size) {
        ret = send(client_fd, pMap + total, st.st_size - total, 0);
        RET_CHECK(ret, -1, "send");
        total += ret;
    }

    // 解除映射
    munmap(pMap, st.st_size);

    // 使用零拷贝接口 sendfile
    ret = sendfile(client_fd, fd, 0, st.st_size);
    RET_CHECK(ret, -1, "sendfile");
#endif
    // 使用零拷贝接口 splice
    // 利用每次管道读取的字节数，控制发送速度
    int curlen = 0;
    int sfd[2];
    pipe(sfd);
    while (curlen < st.st_size) {
        /* ret = splice(fd, 0, sfd[1], 0, 65535, 0); */
        ret = splice(fd, 0, sfd[1], 0, 12, 0);
        /* ret = splice(fd, 0, sfd[1], 0, st.st_size, 0); */
        ret = splice(sfd[0], 0, client_fd, 0, ret, 0);
        curlen += ret;
    }

    close(fd);
    return 1;
}
