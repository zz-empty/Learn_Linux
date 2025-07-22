#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming!\n", signum);
}

void mysleep(int i) {

    // 注册alarm信号
    signal(SIGALRM, sigFunc);

    // 先阻塞alarm
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    alarm(i);
    
    sigsuspend(&oldmask);
}

int main(int argc, char **argv) {
    ARGS_CHECK(argc, 2);

    mysleep(atoi(argv[1]));
    return 0;
}
