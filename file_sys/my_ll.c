#include "head.h"

int main(int argc, char **argv)
{
    // ./my_ll [filename]
    ARGS_CHECK(argc, 2);

    // 打开目录流
    DIR *dirp = opendir(argv[1]);

    // 用来获取目录项的详细信息
    struct dirent *pd;
    struct stat statbuf;

    // 遍历目录项
    while ((pd = readdir(dirp))) {
        stat(pd->d_name, &statbuf);
        printf("%d %d %-s %-s %6lu %-15s %s", 
               statbuf.st_mode,
               statbuf.st_nlink,
               getpwuid(statbuf.st_uid)->pw_name,
               getgrgid(statbuf.st_gid)->gr_name,
               statbuf.st_size,
               pd->d_name,
               ctime(&statbuf.st_mtime));
    }

    // 关闭目录流
    closedir(dirp);
    return 0;
}

