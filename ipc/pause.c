#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming!\n", signum);
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigFunc;
    sigaction(SIGINT, &act, NULL);

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    for (int i = 0; i < 5; ++i) {
        sleep(1);
        printf("i = %d\n", i);
    }
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    printf("before pause...\n");
    pause();
    printf("after pause...\n");
    return 0;
}

