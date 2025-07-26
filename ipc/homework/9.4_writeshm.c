#include "head.h"

int main()
{
    int shmid = shmget(1000, 4096, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");

    char *p = (char*)shmat(shmid, NULL, 0);

    strcpy(p, "How are you");

    shmdt(p);
    return 0;
}

