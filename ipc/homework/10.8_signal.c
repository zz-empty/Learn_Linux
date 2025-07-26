#include "head.h"

void sigfunc(int signum) {
    printf("the %d is comming, ignore it!\n", signum);
    signal(SIGINT, SIG_DFL);
}

int main()
{
    signal(SIGINT, sigfunc);

    for (int i = 0; i < 10; ++i) {
        sleep(1);
        printf("i = %d\n", i);
    }
    return 0;
}

