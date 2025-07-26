#include "head.h"
#include "chat.h"


int main()
{
    int fd = open("test", O_RDONLY);
    int shmid = shmget(1000, 4096, IPC_CREAT|0600);
    RET_CHECK(shmid, -1, "shmget");
    Msg_t *p = (Msg_t*)shmat(shmid, NULL, 0);

    int semid = semget(1000, 1, IPC_CREAT|0600);
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

    Msg_t msg;
    char c;

    while (1) {
        read(fd, &c, 1);
        memset(&msg, 0, sizeof(msg));
        semop(semid, &P, 1);
        msg = p[0];
        semop(semid, &V, 1);

        if (1 == msg.mtype) {
            printf("stdin info: %s\n", msg.mtext);
            p[0].mtype = 0;
        } else if (2 == msg.mtype) {
            printf("pipe info: %s\n", msg.mtext);
            p[0].mtype = 0;
        }
    }

    return 0;
}

