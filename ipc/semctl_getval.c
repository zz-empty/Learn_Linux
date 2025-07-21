#include "head.h"

int main()
{
    int semid = semget(2000, 1, IPC_CREAT|0600);
    RET_CHECK(semid, -1, "semget");

    int ret = semctl(semid, 0, SETVAL, 1);
    RET_CHECK(ret, -1, "semctl_setval");

    int val = semctl(semid, 0, GETVAL);
    printf("before P, val = %d\n", val);

    struct sembuf P;
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;

    semop(semid, &P, 1);
    val = semctl(semid, 0, GETVAL);
    printf("After P, val = %d\n", val);
    
    // delete
    semctl(semid, 0, IPC_RMID);
    return 0;
}

