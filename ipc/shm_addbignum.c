#include "head.h"

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int NUM = atoi(argv[1]);

    int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0600);
    int*p = (int*)shmat(shmid, NULL, 0);

    p[0] = 0;

    if (!fork()) {
        // 子进程
        for (int i = 0; i < NUM; ++i) {
            ++p[0];
        }
        _exit(0);
    } else {
        // 父进程
        for (int i = 0; i < NUM; ++i) {
            ++p[0];
        }
        wait(NULL);
        printf("Total = %d\n", p[0]);
        shmdt(p);
        shmctl(shmid, IPC_RMID, NULL);
        exit(0);
    }

    return 0;
}

