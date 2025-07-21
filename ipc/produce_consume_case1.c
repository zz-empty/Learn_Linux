#include "head.h"

int main()
{
    int semid = semget(1000, 1, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");

    // 初始化信号量
    semctl(semid, 0, SETVAL, 1);
    struct sembuf P;
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;
    struct sembuf V;
    V.sem_num = 0;
    V.sem_op = 1;
    V.sem_flg = SEM_UNDO;

    int shmid = shmget(1000, 4096, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");
    char *p = (char*)shmat(shmid, NULL, 0);

    p[0] = 5; // 剩余商品个数
    p[1] = 5; // 剩余货架空位

    if (!fork()) {
        // 消费者
        while (1) {
            sleep(1);
            printf("before consume!\n");
            semop(semid, &P, 1);
            if (p[0] > 0) {
                --p[0];
                ++p[1];
                printf("consume, p[0]=%d, p[1]=%d\n", p[0], p[1]);
            }
            semop(semid, &V, 1);
            printf("after consume!\n");
        }
    } else {
        // 生产者
        while (1) {
            sleep(2);
            printf("before produce!\n");
            semop(semid, &P, 1);
            if (p[1] > 0) {
                ++p[0];
                --p[1];
                printf("produce, p[0]=%d, p[1]=%d\n", p[0], p[1]);
            }
            semop(semid, &V, 1);
            printf("after produce!\n");
        }
    }

    shmdt(p);
    return 0;
}

