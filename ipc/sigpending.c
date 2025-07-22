#include "head.h"

void checkSignal(sigset_t *pstatus) {
    sigpending(pstatus);
    if (sigismember(pstatus, SIGINT)) {
        printf("SIGINT is blocked\n");
    } else {
        printf("SIGINT is not blocked\n");
    }
    if (sigismember(pstatus, SIGQUIT)) {
        printf("SIGQUIT is blocked\n");
    } else {
        printf("SIGQUIT is not blocked\n");
    }
}

void sigFunc(int signum) {
    printf("%d is comming!\n", signum);
    printf("beg...\n");
    sleep(5);
    printf("end...\n");

    // 查看当前阻塞状态
    sigset_t status;
    checkSignal(&status);
    // 解除所有阻塞
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    checkSignal(&status);
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
    sigset_t status;
    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    checkSignal(&status);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    checkSignal(&status);


    char buf[128];
    read(STDIN_FILENO, buf, sizeof(buf));
    printf("buf = %s\n", buf);
    return 0;
}

