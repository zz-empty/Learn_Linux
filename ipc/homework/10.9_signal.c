#include "head.h"

void sigfunc(int signum) {
    printf("beg %d\n", signum);
    sleep(3);
    printf("end %d\n", signum);
}

int main()
{
    signal(SIGINT, sigfunc);
    signal(SIGQUIT, sigfunc);

    for (int i = 0; i < 10; ++i) {
        sleep(1);
        printf("i = %d\n", i);
    }
    return 0;
}

