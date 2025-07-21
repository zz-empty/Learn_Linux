#include "head.h"

int main() {
    int shmid = shmget(1000, 128, IPC_CREAT | 0600);
    RET_CHECK(shmid, -1, "shmget");

    char *p = (char*)shmat(shmid, NULL, 0);
    RET_CHECK(p, (void*)-1, "shmat");

    /* while(1); */
    strcpy(p, "hello shm, ready communication!");
    while(1);

    /* shmdt(p); */

    return 0;
}
