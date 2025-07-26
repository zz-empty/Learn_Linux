#ifndef __CHAT_H__
#define __CHAT_H__

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

// Constants
#define MSG_SIZE 1024
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define MSG_KEY 0x9ABC

// Named FIFOs
#define FIFO_A2B "/tmp/fifo_A2B"
#define FIFO_B2A "/tmp/fifo_B2A"
// PID file
#define PID_FILE "/tmp/chat_pids"

// 返回值检查
#define RET_CHECK(ret, num, msg) { if (ret == num) { perror(msg); exit(EXIT_FAILURE); } }

// IPC identifiers
extern int shmid;
extern int semid;
extern int msgid;
extern int fd_A2B;
extern int fd_B2A;

// Shared memory message
typedef struct {
    long mtype;
    char mtext[MSG_SIZE];
} shm_msg_t;

// Message Queue buffer
typedef struct {
    long mtype;
    char mtext[MSG_SIZE];
} msgbuf_t;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// PV操作
#define P(semid) do { struct sembuf op = {0, -1, 0}; semop(semid, &op, 1); } while(0)
#define V(semid) do { struct sembuf op = {0, +1, 0}; semop(semid, &op, 1); } while(0)


// Process PIDs
extern pid_t pid_A;
extern pid_t pid_A1;
extern pid_t pid_B;
extern pid_t pid_B1;

// Functions
void init_ipc();
void cleanup_ipc();
void sig_handler(int signo);

// PID Management
typedef enum {
    ROLE_A = 'A',
    ROLE_A1 = 'B',
    ROLE_B = 'C',
    ROLE_B1 = 'D'
} Role;

void write_pid_file(Role role);
void read_pid_file();

#endif
