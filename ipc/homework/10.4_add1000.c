#include "head.h"


int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int MAXNUM = atoi(argv[1]);

    // 创建信号量用于同步
    int semid = semget(2000, 1, IPC_CREAT|0600);
    RET_CHECK(semid, -1, "semid");
    int ret = semctl(semid, 0, SETVAL, 1);
    RET_CHECK(ret, -1, "semctl_setval");
    struct sembuf P;    // 测试并加锁
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;
    struct sembuf V;    // 解锁
    V.sem_num = 0;
    V.sem_op = 1;
    V.sem_flg = SEM_UNDO;

    // 创建共享内存, 并初始化p[0]
    int shmid = shmget(1000, 4096, IPC_CREAT|0600);
    RET_CHECK(shmid, -1, "shmid");
    int *p = (int*)shmat(shmid, NULL, 0);
    p[0] = 0;

    struct timeval beg, end;
    gettimeofday(&beg, NULL);

    pid_t pid = fork();
    if (pid == 0) {
        for (int i = 0; i < MAXNUM; ++i) {
            semop(semid, &P, 1);
            ++p[0];
            semop(semid, &V, 1);
        }
        _exit(0);
    } else {
        for (int i = 0; i < MAXNUM; ++i) {
            semop(semid, &P, 1);
            ++p[0];
            semop(semid, &V, 1);
        }
        wait(NULL);
        gettimeofday(&end, NULL);
        long timeTotal = (end.tv_sec - beg.tv_sec) * 1000000 + end.tv_usec - beg.tv_usec;
        long timeAvg = timeTotal / (2 * MAXNUM);

        printf("add %d , result = %d, timeTotal = %ld, timeAvg = %ld\n", MAXNUM, p[0], timeTotal, timeAvg);
        shmdt(p);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
    }
    return 0;
}

