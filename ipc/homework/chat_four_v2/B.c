#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/types.h>
#define RET_CHECK(ret, num, msg) { if (ret == num) { perror(msg); exit(EXIT_FAILURE); } }

// Constants
#define MSG_SIZE 1024
#define MSG_KEY 0x9ABC
#define FIFO_A2B "fifo_A2B"
#define FIFO_B2A "fifo_B2A"
#define PID_FILE "chat_pids"


// IPC identifiers
int msgid;
int fd_A2B;
int fd_B2A;

// Message Queue buffer
typedef struct {
    long mtype;
    char mtext[MSG_SIZE];
} msgbuf_t;


// Process PIDs
pid_t pid_A, pid_A1, pid_B, pid_B1;

// Functions
void init_ipc();
void cleanup_ipc();
void sig_handler(int signo);

// PID Management
typedef enum { ROLE_A = 'A', ROLE_A1 = 'B', ROLE_B = 'C', ROLE_B1 = 'D' } Role;

void write_pid_file(Role role);
void read_pid_file();

int main()
{
    write_pid_file(ROLE_B);
    while (1) {
        struct stat st;
        if (stat(PID_FILE, &st) == 0 && st.st_size > 0) {
            FILE *f = fopen(PID_FILE, "r");
            RET_CHECK(f, NULL, "fopen_pid");
            char c = 0;
            char l[32];
            while (fgets(l, sizeof(l), f)) {
                c++;
            }
            if (c >= 4) break;
            fclose(f);
        }
        usleep(100000);
    }
    read_pid_file();

    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    init_ipc();

    fd_B2A = open(FIFO_B2A, O_WRONLY | O_NONBLOCK);
    fd_A2B = open(FIFO_A2B, O_RDONLY | O_NONBLOCK);
    msgbuf_t msg; 

    while (1) {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);
        FD_SET(fd_A2B, &fdset);
        int maxfd = fd_A2B > STDIN_FILENO ? fd_A2B : STDIN_FILENO;
        if (select(maxfd + 1, &fdset, NULL, NULL, NULL) < 0) break;
        char buf[MSG_SIZE];

        if (FD_ISSET(STDIN_FILENO, &fdset)) {
            if (!fgets(buf, sizeof(buf), stdin)) break;
            write(fd_B2A, buf, strlen(buf) + 1);
            // 发送给显示器
            msg.mtype = 1; 
            strncpy(msg.mtext, buf, sizeof(msg.mtext));
            msgsnd(MSG_KEY, &msg, strlen(msg.mtext), 0);
        } 
        if (FD_ISSET(fd_A2B, &fdset)) {
            if (read(fd_A2B, buf, sizeof(buf)) > 0) {
                msg.mtype = 2;
                strncpy(msg.mtext, buf, sizeof(msg.mtext));
                msgsnd(MSG_KEY, &msg, sizeof(msg.mtext), 0);
            }
        }
    }
    sig_handler(0);
    return 0;
}

void init_ipc() {
    // 创建命名管道
    mkfifo(FIFO_A2B, 0666); 
    mkfifo(FIFO_B2A, 0666);

   // 初始化消息队列
    msgid = msgget(MSG_KEY, IPC_CREAT | 0600);
    RET_CHECK(msgid, -1, "msgget");
}

void cleanup_ipc() {
    // close fifo file descriptors
    if (fd_A2B >= 0) close(fd_A2B);
    if (fd_B2A >= 0) close(fd_B2A);
    unlink(FIFO_A2B);
    unlink(FIFO_B2A);
    msgctl(msgid, IPC_RMID, NULL);
    unlink(PID_FILE);
}

void sig_handler(int signo) {
    // 如果收到退出信号，广播给所有进程
    if (signo == SIGINT || signo == SIGUSR1) {
        read_pid_file();
        if (pid_A > 0) kill(pid_A, SIGUSR1);
        if (pid_A1 > 0) kill(pid_A1, SIGUSR1);
        if (pid_B > 0) kill(pid_B, SIGUSR1);
        if (pid_B1 > 0) kill(pid_B1, SIGUSR1);
    }
    // 清理资源并退出
    cleanup_ipc();
    exit(EXIT_SUCCESS);
}

void write_pid_file(Role role) {
    // 打开pid文件
    int fd = open(PID_FILE, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    // 将pid写入文件末尾
    lseek(fd, 0, SEEK_END);
    char buf[32];
    snprintf(buf, sizeof(buf), "%c:%d", role, getpid());
    write(fd, buf, strlen(buf));
    close(fd);
}

void read_pid_file() {
    FILE *f = fopen(PID_FILE, "r");
    RET_CHECK(f, NULL, "fopen_pid");

    char line[32];
    while (fgets(line, sizeof(line), f)) {
        char r;
        int id;
        if (sscanf(line, "%c:%d", &r, &id) == 2) {
            switch(r){
            case ROLE_A: pid_A = id; break;
            case ROLE_B: pid_B = id; break;
            case ROLE_A1: pid_A1 = id; break;
            case ROLE_B1: pid_B1 = id; break;
            }
        }
    }
    printf("pid_A: %d", pid_A);
    printf("pid_A1: %d", pid_A1);
    printf("pid_B: %d", pid_B);
    printf("pid_B1: %d", pid_B1);
    fclose(f);
}
