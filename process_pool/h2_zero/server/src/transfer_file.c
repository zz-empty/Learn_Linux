#define _GNU_SOURCE
#include "head.h"
#include "pool.h"

#define DATA_SIZE 1024

typedef struct {
    int len;
    char data[DATA_SIZE];
} Data_t;

int transferFile(int clientFd) {
    const char *filename = "file1";
    int ret = 0;
    Data_t data = {};

    // 先传输文件名
    data.len = strlen(filename);
    strncpy(data.data, filename, DATA_SIZE);
    ret = send(clientFd, &data, sizeof(int) + data.len, 0);
    RET_CHECK(ret, -1, "send_filename");
    printf("--[Worker] send filename [%s]\n", filename);


    // 再传输文件大小
    struct stat st = {};
    stat(filename, &st);
    data.len = st.st_size;
    ret = send(clientFd, &data.len, sizeof(int), 0);
    RET_CHECK(ret, -1, "send_filesize");
    printf("--[Worker] send filesize [%ld]\n", st.st_size);
    
    printf("--[Worker] ready send, filename=%s, filesize=%ld\n", filename, st.st_size);


    // 再传输文件内容
    int fd = open(filename, O_RDONLY);
    int curLen = 0;
    int fds[2];
    pipe(fds);
    while (curLen < st.st_size) {
        ret = splice(fd, 0, fds[1], 0, st.st_size, 0);
        ret = splice(fds[0], 0, clientFd, 0, ret, 0);
        curLen += ret;
    }

    close(fd);
    return 0;
}
