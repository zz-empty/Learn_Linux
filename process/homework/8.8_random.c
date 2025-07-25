#include "head.h"

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 3);
    printf("pid = %d, ppid = %d, pgid = %d, sid = %d\n", getpid(), getppid(), getpgid(getpid()), getsid(getpid()));
    char * const arr[] = {"./bin/return_random", argv[1], argv[2], NULL};

    execv("./bin/return_random", arr);
    printf("you can not see me!\n");
    return 0;
}

