#include "head.h"

int main()
{
    int shmid = shmget(1000, 4096, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");

    char *p = (char*)shmat(shmid, NULL, 0);
    RET_CHECK(p, (char*)-1, "shmat");

    struct shmid_ds stat;
    shmctl(shmid, IPC_STAT, &stat);

    printf("cuid = %d\n", stat.shm_perm.cuid);
    printf("perm = %o\n", stat.shm_perm.mode);
    printf("size = %ld\n", stat.shm_segsz);
    printf("nattach = %ld\n", stat.shm_nattch);

    shmdt(p);
    return 0;
}

