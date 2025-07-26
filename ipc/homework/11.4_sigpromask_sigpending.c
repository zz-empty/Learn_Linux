#include "head.h"

void checkset() {
    sigset_t status;
    sigpending(&status);
    if (sigismember(&status, SIGINT)) {
        printf("SIGINT is blocked!\n");
    } else {
        printf("SIGINT is not blocked!\n");
    }
    if (sigismember(&status, SIGQUIT)) {
        printf("SIGQUIT is blocked!\n");
    } else {
        printf("SIGQUIT is not blocked!\n");
    }
}

int main()
{
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    sleep(5);
    checkset();
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    char c;
    read(STDIN_FILENO, &c, 1);
    return 0;
}

