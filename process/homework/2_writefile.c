#include "head.h"

int main(int argc, char **argv)
{
    // A用户打开文件写入hello
    // B用户再打开文件写入world
    ARGS_CHECK(argc, 3);
    char *filename = argv[1];
    char *data = argv[2];

    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644);
    write(fd, data, strlen(data));
    close(fd);
    return 0;
}

