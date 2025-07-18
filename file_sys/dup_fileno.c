#include "head.h"

int main(int argc, char **argv)
{
    // 标准输出重定向到log1文件，标准错误重定向到log2文件
    ARGS_CHECK(argc, 3);

    int f1 = open(argv[1], O_RDWR);
    int f2 = open(argv[2], O_RDWR);

    dup2(f1, STDOUT_FILENO);
    dup2(f2, STDERR_FILENO);

    printf("output to log1\n");
    fprintf(stderr, "error to log2\n");
    
    close(f1);
    close(f2);
    return 0;
}

