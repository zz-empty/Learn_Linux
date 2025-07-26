#include "head.h"

int main()
{
    // 利用read确保父子进程执行的顺序
    int fds[2];
    pipe(fds);

    if (fork() == 0) {
        close(fds[0]);
        printf("hello");
        write(fds[1], "1", 1);
        exit(0);
    } else {
        close(fds[1]);
        char c;
        read(fds[0], &c, 1);
        printf("world\n");
        wait(NULL);
    }
    return 0;
}

