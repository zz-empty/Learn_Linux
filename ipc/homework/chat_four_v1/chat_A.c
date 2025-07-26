#include "head.h"
#include "chat.h"

#define PID_FILE "pid_file"

int fd;
int shmid, semid;
Msg_t *p;
int fdw, fdr;

void write_pid() {
    FILE *f = (FILE*)fopen(PID_FILE, "a+");
    pid_t pid = getpid();
    fprintf(f, "%s %d\n", "A", pid);
    fclose(f);
}

void cleanup_ipc() {
    if (fd > 0) close(fd);
    close(fdr);
    close(fdw);
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
}

void sig_handler(int signum) {
    pid_t pids[4] = {};
    FILE *f = (FILE*)fopen("pid_file", "r");
    char buf[10];
    for (int i = 0; i < 4; ++i) {
        fscanf(f, "%s%d", buf, &pids[i]);
        /* printf("%s--->%d\n", buf, pids[i]); */
        kill(pids[i], SIGUSR1);
    }
    fclose(f);

    cleanup_ipc();
    exit(EXIT_SUCCESS);
}

int chat(const char *r_pipe, const char *w_pipe, const char *chat_name) {
    write_pid();

    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);

    fd = open("test", O_RDWR);
    // 建立共享内存和信号量
    shmid = shmget(1000, 4096, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");
    p = (Msg_t*)shmat(shmid, NULL, 0);

    semid = semget(1000, 1, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");
    semctl(semid, 0, SETVAL, 1);
    struct sembuf P = {0, -1, SEM_UNDO};
    struct sembuf V = {0, +1, SEM_UNDO};

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

