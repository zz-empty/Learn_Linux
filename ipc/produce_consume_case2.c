#include "head.h"

int main()
{
    int semid = semget(1000, 2, IPC_CREAT | 0600);
    RET_CHECK(semid, -1, "semget");

    // 初始化信号量
    unsigned short val[] = {5, 5};
    semctl(semid, 0, SETALL, val);

    // 先消费后释放空位
    struct sembuf consume[2] = {
        {.sem_num = 0, .sem_op = -1, .sem_flg = SEM_UNDO},
        {.sem_num = 1, .sem_op = 1, .sem_flg = SEM_UNDO}
    };

    // 先申请空位后生产
    struct sembuf produce[2] = {
        {.sem_num = 1, .sem_op = -1, .sem_flg = SEM_UNDO},
        {.sem_num = 0, .sem_op = 1, .sem_flg = SEM_UNDO}
    };

    if (!fork()) {
        // 消费者
        while (1) {
            sleep(1);
            printf("before consume! goods=%d, empty=%d\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
            semop(semid, consume, 2);
            printf("after consume! goods=%d, empty=%d\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
        }
    } else {
        // 生产者
        while (1) {
            sleep(3);
            printf("-----------before produce! goods=%d, empty=%d\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
            semop(semid, produce, 2);
            printf("-----------after produce! goods=%d, empty=%d\n", semctl(semid, 0, GETVAL), semctl(semid, 1, GETVAL));
        }
    }

    return 0;
}

