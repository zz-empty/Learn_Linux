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

    while (1) {
        memset(&data, 0, sizeof(Data_t));
        data.len = read(fd, data.data, DATA_SIZE);
        RET_CHECK(data.len, -1, "read");
        if (0 == data.len) {
            // 已读完文件, 通知客户端
            printf("--[Worker] file read over!\n");
            send(clientFd, &data, sizeof(data.len), 0);
            break;
        }

        // 发送
        ret = send(clientFd, &data, sizeof(int) + data.len, 0);
        RET_CHECK(ret, -1, "send_filecontent");
    }

    close(fd);
    return 0;
}
