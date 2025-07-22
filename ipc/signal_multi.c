#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming\n", signum);
    printf("beg...\n");
    sleep(5);
    printf("end...\n");
}

int main()
{
    signal(SIGINT, sigFunc);
    signal(SIGQUIT, sigFunc);
    while(1);
    return 0;
}

