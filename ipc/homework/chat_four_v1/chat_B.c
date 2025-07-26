#include "head.h"
#include "chat.h"
#define BUFSIZE 1024

void sigint_handler(int signum) {
    printf("ready quit!\n");
    exit(0);
}

int chat(const char *r_pipe, const char *w_pipe, const char *chat_name) {
    // 通过消息队列与显示器交互
    int msgid = msgget(1000, IPC_CREAT|0600);
    RET_CHECK(msgid, -1, "msgget");

    // 接收对端进程消息和stdin
    Msg_t msg;

    // 管道与A通信
    int fdw = open(w_pipe, O_RDWR);
    RET_CHECK(fdw, -1, "open_w");
    int fdr = open(r_pipe, O_RDWR);
    RET_CHECK(fdr, -1, "open_r");
    printf("%s pipe ready...\n", chat_name);

    while (1) {
        fd_set rdset;
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(fdr, &rdset);

        int ret = select(fdr + 1, &rdset, NULL, NULL, NULL);
        RET_CHECK(ret, -1, "select");

        if (FD_ISSET(fdr, &rdset)) {
            // 读取管道，打印内容
            memset(&msg, 0, sizeof(msg));
            ret = read(fdr, msg.mtext, sizeof(msg.mtext));
            RET_CHECK(ret, -1, "read");
            if (0 == ret) {
                // 对端退出
                printf("Chat passively closed!\n");
                break;
            }

            // 传给显示器
            msg.mtype = 2;
            msgsnd(msgid, &msg, strlen(msg.mtext), 0);
        }
        if (FD_ISSET(STDIN_FILENO, &rdset)) {
            // 读取stdin，向管道输入
            memset(&msg, 0, sizeof(msg));
            ret = read(STDIN_FILENO, msg.mtext, sizeof(msg.mtext));
            RET_CHECK(ret, -1, "read");
            if (0 == ret) {
                printf("I quit!\n");
                write(fdw, "bye!", 4);
                break;
            }
            ret = write(fdw, msg.mtext, ret - 1);
            RET_CHECK(ret, -1, "write");

            // 传给显示器
            msg.mtype = 1;
            msgsnd(msgid, &msg, ret, 0);

        }
    }

    close(fdr);
    close(fdw);
    return 0;
}

int main()
{
    const char *r_pipe = "2.pipe";
    const char *w_pipe = "1.pipe";
    const char *chat_name = "B";

    chat(r_pipe, w_pipe, chat_name);
    return 0;
}
