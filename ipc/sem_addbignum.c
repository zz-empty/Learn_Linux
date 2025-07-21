#include "head.h"

int main(int argc, char **argv)
{
    // 获取一个信号量
    int semid = semget(1000, 1, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");
    int ret = semctl(semid, 0, SETVAL, 1);
    RET_CHECK(ret, -1, "semctl_setval");
    // 设置P操作，测试并加锁
    struct sembuf P;
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;
    // 设置V操作，解锁
    struct sembuf V;
    V.sem_num = 0;
    V.sem_op = 1;
    V.sem_flg = SEM_UNDO;


    // 从命令行获取目标值
    ARGS_CHECK(argc, 2);
    int num = atoi(argv[1]);

    // 获取一个共享内存
    int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");
    // 连接共享内存
    int *p = (int*)shmat(shmid, NULL, 0);
    RET_CHECK(p, (int*)-1, "shmat");

    // 初始化空间
    p[0] = 0;

    // 计算时间
    struct timeval beg, end;
    gettimeofday(&beg, NULL);
    
    if (!fork()) {
        // child
        for (int i = 0; i < num; ++i) {
            semop(semid, &P, 1);
            p[0]++;
            semop(semid, &V, 1);
        }
        _exit(0);
    } else {
        // parent
        for (int i = 0; i < num; ++i) {
            semop(semid, &P, 1);
            p[0]++;
            semop(semid, &V, 1);
        }
        wait(NULL);

        // 得出计算时间
        gettimeofday(&end, NULL);
        long total_time = (end.tv_sec - beg.tv_sec) * 1000000 + end.tv_usec - beg.tv_usec;
        printf("TotalNum = %d, timeTotal = %ld, timeAvg = %ld\n", p[0], total_time, total_time / (num * 2 ));

        shmdt(p);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}

