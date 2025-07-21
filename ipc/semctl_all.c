#include "head.h"

int main()
{
    int semid = semget(3000, 2, IPC_CREAT | 0600);

    unsigned short arr[] = {3, 5};
    semctl(semid, 2, SETALL, arr);

    unsigned short ret[2];
    semctl(semid, 0, GETALL, ret);
    for (int i = 0; i < 2; ++i) {
        printf("ret[%d] = %d\n", i, ret[i]);
    }
    semctl(semid, 2, IPC_RMID);
    return 0;
}

