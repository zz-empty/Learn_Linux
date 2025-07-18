#include "head.h"

int process_chat(const char *pipe1, const char *pipe2);

int main(int argc, char **argv)
{
    // ./chat_a 1.pipe 2.pipe
    ARGS_CHECK(argc, 3);

    // 进程即时聊天
    process_chat(argv[1], argv[2]);
    
    return 0;
}

int process_chat(const char *pipe1, const char *pipe2) {
    int fdr = open(pipe1, O_RDWR);
    int fdw = open(pipe2, O_RDWR);
    
    printf("---pipe set success---\n");
    printf("I am %s\n\n", __FILE__);

    // 用select管理fd
    fd_set rdset;
    char buf[1024];
    while (1) {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(fdr, &rdset);
        int ret = select(fdr + 1, &rdset, NULL, NULL, NULL);
        RET_CHECK(ret, -1, "select");

        if (FD_ISSET(fdr, &rdset)) {
            // 读端就绪，读取数据
            memset(buf, 0, sizeof(buf));
            ret = read(fdr, buf, sizeof(buf));
            RET_CHECK(ret, -1, "read_pipe");

            // 对端退出
            if (0 == ret) {
                printf("---pipe close---\n");
                break;
            }

            time_t cur_time = time(NULL);
            struct tm *timeinfo = localtime(&cur_time);

            printf("%02d:%02d:%02d from pipe: %s\n", 
                   timeinfo->tm_hour, 
                   timeinfo->tm_min,
                   timeinfo->tm_sec,
                   buf);
        }

        if (FD_ISSET(STDIN_FILENO, &rdset)) {
            // stdin就绪
            memset(buf, 0, sizeof(buf));
            ret = read(STDIN_FILENO, buf, sizeof(buf));
            RET_CHECK(ret, -1, "read_stdin");

            // 本端退出
            if (0 == ret) {
                printf("I quit\n");
                write(fdw, "byebye!", 7);
                break;
            }

            ret = write(fdw, buf, ret);
            RET_CHECK(ret, -1, "write_pipe");
        }

    }

    close(fdr);
    close(fdw);
    return 0;
}
