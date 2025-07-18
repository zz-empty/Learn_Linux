#include "head.h"

int copy_file(const char *src, const char *dest);
int main(int argc, char **argv)
{
    // ./cp file1 file2
    ARGS_CHECK(argc, 3);
    copy_file(argv[1], argv[2]);
    return 0;
}

int copy_file(const char *src, const char *dest) {
    int fdr = open(src, O_RDONLY);
    int fdw = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    char buf[1024] = "";
    while (1) {
        int ret = read(fdr, buf, sizeof(buf));
        RET_CHECK(ret, -1, "read");
        if (0 == ret) {
            printf("复制完毕!\n");
            break;
        }
        ret = write(fdw, buf, ret);
        RET_CHECK(ret, -1, "write");
    }

    close(fdr);
    close(fdw);
    return 0;
}
