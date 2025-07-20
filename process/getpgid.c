#include "head.h"

int main()
{
    pid_t child = fork();
    if (0 == child) {
        /* setpgid(0, 0); */
        setpgrp();
        printf("pid = %d, pgid = %d, pgid = %d\n", getpid(), getpgid(0), getpgrp());
    } else {
        printf("pid = %d, pgid = %d, pgid = %d\n", getpid(), getpgid(0), getpgrp());
        wait(NULL);
    }
    return 0;
}

