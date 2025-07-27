#include "head.h"

int main()
{
    int semid = semget(1000, 2, IPC_CREAT | 0600);
    unsigned short arr[] = {4, 5};
    semctl(semid, 2, SETALL, arr);

    // 获取信号量的状态
    struct semid_ds statbuf;
    semctl(semid, 0, IPC_STAT, &statbuf);

    printf("perm = %o\n", statbuf.sem_perm.mode);
    printf("cuid = %d\n", statbuf.sem_perm.cuid);
    printf("nsems = %hu\n", statbuf.sem_nsems);

    // 修改信号量的状态
    statbuf.sem_perm.mode = 0666;
    semctl(semid, 0, IPC_SET, &statbuf);

    /* semctl(semid, 2, IPC_RMID); */
    return 0;
}

