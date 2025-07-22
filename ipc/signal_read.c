#include "head.h"

void sigFunc(int signum) {
    printf("%d is comming\n", signum);
    printf("beg...\n");
    sleep(5);
    printf("end...\n");
}

int main()
{
    signal(SIGINT, sigFunc);

    char buf[128] = "";
    int ret = read(STDIN_FILENO, buf, sizeof(buf));
    printf("ret=%d, buf=%s\n", ret, buf);
    return 0;
}

