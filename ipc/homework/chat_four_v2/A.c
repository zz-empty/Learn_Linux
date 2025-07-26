#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#define MSG_SIZE 1024
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

#define FIFO_A2B "fifo_A2B"
#define FIFO_B2A "fifo_B2A"
#define PID_FILE "chat_pids"

// 返回值检查
#define RET_CHECK(ret, num, msg) { if (ret == num) { perror(msg); exit(EXIT_FAILURE); } }

// 全局变量用来清理
int shmid;
int semid;
int fd_A2B;
int fd_B2A;

// 共享内存的消息类型
typedef struct {
    long mtype;
    char mtext[MSG_SIZE];
} shm_msg_t;

// PV操作
#define P(semid) do { struct sembuf op = {0, -1, 0}; semop(semid, &op, 1); } while(0)
#define V(semid) do { struct sembuf op = {0, +1, 0}; semop(semid, &op, 1); } while(0)


// 四个进程pid
pid_t pid_A, pid_A1, pid_B, pid_B1;
// pid宏
typedef enum { ROLE_A = 'A',ROLE_A1 = 'B',ROLE_B = 'C',ROLE_B1 = 'D'} Role;


// Functions
int init_ipc();
void cleanup_ipc();
void sig_handler(int signo);
void write_pid_file(Role role);
void read_pid_file();


int main()
{
    // 向pidfile中写入本机pid
    write_pid_file(ROLE_A);

    // 等待其他进程写入pid
    while (1) {
        struct stat st;
        if (stat(PID_FILE, &st) == 0 && st.st_size > 0) {
            FILE *f = fopen(PID_FILE, "r");
            int c = 0;
            char l[32];
            while (fgets(l, sizeof(l), f)) {
                c++;
            }
            fclose(f);
            if (c >= 4) break;
        }
        usleep(100000);
    }

    // 读取所有的pid
    read_pid_file();
    // 注册退出信号
    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);
    // 初始化ipc变量
    init_ipc();
    // 打开管道
    fd_A2B = open(FIFO_A2B, O_WRONLY);
    fd_B2A = open(FIFO_B2A, O_RDONLY);
    // 连接共享内存
    shm_msg_t *shm = (shm_msg_t*)shmat(shmid, NULL, 0);

    while (1) {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(STDIN_FILENO, &fdset);
        FD_SET(fd_B2A, &fdset);
        int maxfd = fd_B2A > STDIN_FILENO ? fd_B2A : STDIN_FILENO;
        if (select(maxfd + 1, &fdset, NULL, NULL, NULL) < 0) break; 

        char buf[MSG_SIZE];
        if (FD_ISSET(fd_B2A, &fdset)) {
            if (read(fd_B2A, buf, sizeof(buf)) > 0) {
                P(semid);
                shm->mtype = 2;
                strncpy(shm->mtext, buf, sizeof(shm->mtext));
                V(semid);
            }
        }
        if (FD_ISSET(STDIN_FILENO, &fdset)) {
            if (!fgets(buf, sizeof(buf), stdin)) break;
            write(fd_A2B, buf, strlen(buf) + 1);    // 把结尾的'\0'加上
            P(semid);
            shm->mtype = 1;
            strncpy(shm->mtext, buf, sizeof(shm->mtext));
            V(semid);
        }
    }
    sig_handler(0);
    return 0;
}

int init_ipc() {
    // 创建命名管道
    mkfifo(FIFO_A2B, 0666); 
    mkfifo(FIFO_B2A, 0666);
    int ret;
    // 初始化共享内存+信号量
    shmid = shmget(SHM_KEY, sizeof(shm_msg_t), IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");

    semid = semget(SEM_KEY, 1, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");
    ret = semctl(semid, 0, SETVAL, 1);
    RET_CHECK(ret, -1, "semctl_setval");
    return 0;
}

void cleanup_ipc() {
    if (fd_A2B >= 0) close(fd_A2B);
    if (fd_B2A >= 0) close(fd_B2A);
    unlink(FIFO_A2B);
    unlink(FIFO_B2A);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
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
