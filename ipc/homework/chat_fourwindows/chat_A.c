#include "head.h"
#include "chat.h"


void sigint_handler(int signum) {
    printf("ready quit!\n");
    exit(0);
}

int chat(const char *r_pipe, const char *w_pipe, const char *chat_name) {
    signal(SIGINT, sigint_handler);

    int fd = open("test", O_RDWR);

    // 建立共享内存和信号量
    int shmid = shmget(1000, 128, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");
    Msg_t *p = (Msg_t*)shmat(shmid, NULL, 0);

    int semid = semget(1000, 1, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");
    semctl(semid, 0, SETVAL, 1);
    struct sembuf P;
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;
    struct sembuf V;
    V.sem_num = 0;
    V.sem_op = 1;
    V.sem_flg = SEM_UNDO;


    // 接收对端进程消息和stdin
    Msg_t msg;

    int fdw = open(w_pipe, O_RDWR);
    RET_CHECK(fdw, -1, "open_w");
    int fdr = open(r_pipe, O_RDWR);
    RET_CHECK(fdr, -1, "open_r");
    printf("%s pipe ready...\n", chat_name);

    fd_set rdset;
    while (1) {
        // 监听stdin和pipe
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(fdr, &rdset);

        int ret = select(fdr + 2, &rdset, NULL, NULL, NULL);
        RET_CHECK(ret, -1, "select");

        
        if (FD_ISSET(fdr, &rdset)) {
            // 读取管道
            memset(&msg, 0, sizeof(msg));
            ret = read(fdr, msg.mtext, sizeof(msg.mtext));
            RET_CHECK(ret, -1, "read");
            if (0 == ret) {
                // 对端退出
                printf("Chat passively closed!\n");
                break;
            }
            
            // 传给A1显示器
            write(fd, "a", 1);
            msg.mtype = 2;
            semop(semid, &P, 1);
            p[0] = msg;
            semop(semid, &V, 1);
            
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
            if ('\n' == msg.mtext[ret - 1]) {
                msg.mtext[ret - 1] = '\0';
            }
            ret = write(fdw, msg.mtext, ret - 1);

            // 传给A1显示器
            write(fd, "a", 1);
            msg.mtype = 1;
            semop(semid, &P, 1);
            p[0] = msg;
            semop(semid, &V, 1);
        }
    }

    close(fdr);
    close(fdw);
    return 0;
}

int main()
{
    const char *r_pipe = "1.pipe";
    const char *w_pipe = "2.pipe";
    const char *chat_name = "A";

    chat(r_pipe, w_pipe, chat_name);
    return 0;
}

