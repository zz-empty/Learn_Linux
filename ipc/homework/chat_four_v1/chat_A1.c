#include "head.h"
#include "chat.h"

#define PID_FILE "pid_file"

int fd;
int shmid;
int semid;
Msg_t *p;

void write_pid() {
    FILE *f = (FILE*)fopen(PID_FILE, "a+");
    pid_t pid = getpid();
    fprintf(f, "%s %d\n", "A1", pid);
    fclose(f);
}

void cleanup_ipc() {
    if (fd > 0) close(fd);
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

int main()
{
    write_pid();
    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);

    fd = open("test", O_RDONLY);
    shmid = shmget(1000, 4096, IPC_CREAT|0600);
    RET_CHECK(shmid, -1, "shmget");
    p = (Msg_t*)shmat(shmid, NULL, 0);

    semid = semget(1000, 1, IPC_CREAT|0600);
    RET_CHECK(semid, -1, "semget");
    semctl(semid, 0, SETVAL, 1);
    struct sembuf P = {0, -1, SEM_UNDO};
    struct sembuf V = {0, +1, SEM_UNDO};

    Msg_t msg;
    char c;

    while (1) {
        read(fd, &c, 1);
        memset(&msg, 0, sizeof(msg));
        semop(semid, &P, 1);
        msg = p[0];
        semop(semid, &V, 1);

        if (1 == msg.mtype) {
            printf("[local]: %s\n", msg.mtext);
            p[0].mtype = 0;
        } else if (2 == msg.mtype) {
            printf("[remote]: %s\n", msg.mtext);
            p[0].mtype = 0;
        }
    }

    return 0;
}

