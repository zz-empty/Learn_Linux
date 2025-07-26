#include "head.h"


int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int MAXNUM = atoi(argv[1]);
    int shmid = shmget(1000, 4096, IPC_CREAT|0600);
    RET_CHECK(shmid, -1, "shmid");

    char *p = (char*)shmat(shmid, NULL, 0);
    p[0] = 0;

    pid_t pid = fork();
    if (pid == 0) {
        for (int i = 1; i <= MAXNUM; ++i) {
            ++p[0];
        }
        _exit(0);
    } else {
        for (int i = 1; i <= MAXNUM; ++i) {
            ++p[0];
        }
        wait(NULL);

        printf("add %d , result = %d\n", MAXNUM, p[0]);
        shmdt(p);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}

