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
#define PID_FILE "chat_pids"

// IPC identifiers
int msgid;

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

    msgbuf_t msg; 

    while (msgrcv(MSG_KEY, &msg, sizeof(msg.mtext), 0, 0) > 0) {
        if (msg.mtype == 1) {
            printf("[local] %s", msg.mtext);
        } else if (msg.mtype == 2) {
            printf("[remote] %s", msg.mtext);
        }
    }
    sig_handler(0);
    return 0;
}

void init_ipc() {
   // 初始化消息队列
    msgid = msgget(MSG_KEY, IPC_CREAT | 0600);
    RET_CHECK(msgid, -1, "msgget");
}

void cleanup_ipc() {
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
