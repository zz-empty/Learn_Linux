#include "head.h"

int main()
{
    int semid = semget(2000, 1, IPC_CREAT|0600);
    RET_CHECK(semid, -1, "semget");

    int ret = semctl(semid, 0, SETVAL, 1);
    RET_CHECK(ret, -1, "semctl_setval");
    return 0;
}

