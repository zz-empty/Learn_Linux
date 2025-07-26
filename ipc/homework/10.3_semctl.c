#include "head.h"

int main()
{
    int semid = semget(1000, 2, IPC_CREAT | 0600);

    // SETVAL GETVAL
    semctl(semid, 0, SETVAL, 9);
    printf("sem[%d] = %d\n", 0, semctl(semid, 0, GETVAL));
    semctl(semid, 1, SETVAL, 4);
    printf("sem[%d] = %d\n", 1, semctl(semid, 1, GETVAL));

    // SETALL, GETALL
    unsigned short arr[] = {8, 3};
    semctl(semid, 0, SETALL, arr);
    unsigned short ret[2] = {};
    semctl(semid, 0, GETALL, ret);
    for (int i = 0; i < 2; ++i) {
        printf("ret[%d] = %d\n", i, ret[i]);
    }

    semctl(semid, 2, IPC_RMID);
    return 0;
}

