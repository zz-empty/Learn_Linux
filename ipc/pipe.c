#include "head.h"

int main()
{
    int fds1[2];
    int fds2[2];
    pipe(fds1);
    pipe(fds2);

    pid_t child = fork();
    if (child) {
        // 父进程
        close(fds1[0]); // fds1写端
        close(fds2[1]); // fds2读端

        char buf[1024] = "";
        printf("parent, from pipe---------\n");
        read(fds2[0], buf, sizeof(buf));
        puts(buf);

        strcpy(buf, "OK, you are the one");
        write(fds1[1], buf, strlen(buf));

        wait(NULL);
    } else {
        // 子进程
        close(fds1[1]); // fds1读端
        close(fds2[0]); // fds2写端
    
        char buf[1024] = "";
        strcpy(buf, "I am the king");
        write(fds2[1], buf, strlen(buf));

        printf("child, from pipe----------\n");
        read(fds1[0], buf, sizeof(buf));
        puts(buf);
        _exit(0);
    }
    return 0;
}

