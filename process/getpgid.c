#include "head.h"

int main()
{
    pid_t child = fork();
    if (0 == child) {
        /* setpgid(0, 0); */
        setpgrp();
        printf("pid = %d, pgid = %d\n", getpid(), getpgid(0));
        while(1);
    } else {
        printf("pid = %d, pgid = %d\n", getpid(), getpgid(0));
        wait(NULL);
    }
    return 0;
}

