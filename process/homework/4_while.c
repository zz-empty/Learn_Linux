#include "head.h"

int main()
{
    printf("pid = %d\n", getpid());
    printf("pgid = %d\n", getpgid(getpid()));
    printf("sid = %d\n", getsid(getpid()));
    while(1);
    return 0;
}

