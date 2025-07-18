#include "head.h"

int main(int argc, char **argv)
{
    printf("pid=%d, ppid=%d\n", getpid(), getppid());
    printf("uid=%d, euid=%d\n", getuid(), geteuid());
    int fd = open(argv[1], O_RDONLY);
    close(fd);
    return 0;
}

