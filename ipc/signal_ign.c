#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming\n", signum);
    signal(signum, SIG_DFL);    // 只生效一次
}

int main()
{
    signal(SIGINT, sigFunc);
    while(1);
    return 0;
}

