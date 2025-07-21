#include "head.h"

int main()
{
    int shmid = shmget(1000, 128, IPC_CREAT | 0600);

    char *p = (char*)shmat(shmid, NULL, 0);

    puts(p);

    shmdt(p);
}

