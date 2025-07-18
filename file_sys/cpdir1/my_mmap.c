#include "head.h"

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);

    int fd = open(argv[1], O_RDWR);
    RET_CHECK(fd, -1, "open");

    int ret = ftruncate(fd, 10);
    RET_CHECK(ret, -1, "ftruncate");

    char *p = (char*)mmap(NULL, 10, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    strcpy(p, "hello mmap");
    puts(p);
    p[0] = 'H';
    puts(p);
    munmap(p, 10);

    close(fd);
    return 0;
}

