#include "head.h"

void sigint_handler(int signum) {
    printf("beg %d\n", signum);
    sleep(3);
    sigset_t status;
    sigpending(&status);
    if (sigismember(&status, SIGQUIT)) {
        printf("SIGQUIT is block!\n");
    } else {
        printf("SIGQUIT is not block!\n");
    }
    printf("end %d\n", signum);
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sigint_handler;
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    sigaction(SIGINT, &act, NULL);

    char buf[128] = "";
    read(STDIN_FILENO, buf, sizeof(buf));

    return 0;
}

