#include "head.h"

int main()
{
    if (fork()) {
        printf("parent, pid=%d, pgid=%d, sid=%d\n", getpid(), getpgid(0), getsid(0));
        wait(NULL);
    } else {
        printf("before child, pid=%d, pgid=%d, sid=%d\n", getpid(), getpgid(0), getsid(0));
        setsid();
        printf("after child, pid=%d, pgid=%d, sid=%d\n", getpid(), getpgid(0), getsid(0));
    }
    return 0;
}

