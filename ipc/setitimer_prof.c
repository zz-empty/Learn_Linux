#include "head.h"

void sigFunc(int signum) {
    time_t now;
    time(&now);
    printf("%s\n", ctime(&now));
}

int main()
{
    signal(SIGPROF, sigFunc);

    struct itimerval timer;
    timer.it_interval.tv_sec = 1;   // 每次发送信号的间隔
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 3;  // 第一次发送的信号的时间
    timer.it_value.tv_usec = 0;

    setitimer(ITIMER_PROF, &timer, NULL);
    sigFunc(0); // 打印当前时间
    while(1);
    return 0;
}

