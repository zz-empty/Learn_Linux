#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming!\n", signum);
    printf("beg...\n");
    sleep(5);
    printf("end...\n");
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

int main()
{
    // 注册2号信号
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigFunc;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    // 全程屏蔽3号信号
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    char buf[128];
    read(STDIN_FILENO, buf, sizeof(buf));
    printf("buf = %s\n", buf);
    return 0;
}

