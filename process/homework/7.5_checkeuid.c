#include "head.h"

int main()
{
    printf("pid=%d, uid=%d, euid=%d\n", getpid(), getuid(), geteuid());
    while(1);
    return 0;
}

