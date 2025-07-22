#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming\n", signum);
}

int main()
{
    signal(SIGINT, sigFunc);
    while(1);
    return 0;
}

