#include "head.h"

void sigfunc(int signum) {
    printf("----beg %d\n", signum);
    sleep(3);
    printf("----end %d\n", signum);
}

int main()
{
    struct sigaction act;
    act.sa_handler = sigfunc;
    act.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGINT, &act, NULL);

#if 1
    char buf[256];
    int ret = read(STDIN_FILENO, buf, sizeof(buf));
    printf("ret = %d, buf = %s\n", ret, buf);
#else
    for (int i = 0; i < 10; ++i) {
        // 这里的sleep会与信号中的冲突
        sleep(1);
        printf("i = %d\n", i);
    }
#endif


    return 0;
}

