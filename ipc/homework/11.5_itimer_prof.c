#include "head.h"

void sigalrm_handler(int signum) {
    // 打印当前时间
    time_t now;
    time(&now);
    printf("%s\n", ctime(&now));
}

int main()
{
    // 修改实用信号
    signal(SIGPROF, sigalrm_handler);

    struct itimerval timer;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 4;
    timer.it_value.tv_usec = 0;

    // 设置实用计时器
    setitimer(ITIMER_PROF, &timer, NULL);
    sigalrm_handler(0);

    while(1);
    return 0;
}

