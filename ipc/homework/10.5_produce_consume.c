#include "head.h"

int main()
{
    // get sem
    int semid = semget(1000, 2, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");
    // init sem
    // 0号商品 1号空位
    unsigned short val[] = {6, 4};
    int ret = semctl(semid, 2, SETALL, val);
    RET_CHECK(ret, -1, "semctl_setall");

    // 消费
    struct sembuf consume[2] = {
        {.sem_num = 0, .sem_op = -1, .sem_flg = SEM_UNDO},
        {.sem_num = 1, .sem_op = 1, .sem_flg = SEM_UNDO},
    };
    // 生产
    struct sembuf produce[2] = {
        {.sem_num = 1, .sem_op = -1, .sem_flg = SEM_UNDO},
        {.sem_num = 0, .sem_op = 1, .sem_flg = SEM_UNDO},
    };

    pid_t pid = fork();
    if (0 == pid) {
        // 子进程消费
        while (1) {
            sleep(1);
            printf("-----B consume, p[0] = %d, p[1] = %d", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
            semop(semid, consume, 2);
            printf("-----A consume, p[0] = %d, p[1] = %d\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
        }
        _exit(0);
    } else {
        // 父进程生产
        while (1) {
            sleep(2);
            printf("B produce, p[0] = %d, p[1] = %d", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
            semop(semid, produce, 2);
            printf("=====A produce, p[0] = %d, p[1] = %d\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
        }
        wait(NULL);

        // 删除sem
        semctl(semid, 2, IPC_RMID);
    }

    return 0;
}

