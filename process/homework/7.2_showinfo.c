#include "head.h"

int main()
{
    printf("pid=%d\tppid=%d\nuid=%d\tgid=%d\neuid=%d\tegid=%d\n",
           getpid(),
           getppid(),
           getuid(),
           getgid(),
           geteuid(),
           getegid());
    return 0;
}

