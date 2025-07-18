#include "head.h"

void my_stat(const char *filename);
char *format_time(time_t tim);
char* format_mode(mode_t mode);

int main(int argc, char **argv)
{
    // ./my_stat filename
    ARGS_CHECK(argc, 2);
    my_stat(argv[1]);
    return 0;
}

void my_stat(const char *filename) {
    struct stat statbuf;
    stat(filename, &statbuf);

    printf("%8s%s\n", "File: ", filename);
    printf("%8s%-10ld\t%8s%-11ld%10s%-7d\n", "Size: ", statbuf.st_size, "Blocks: ", statbuf.st_blocks, "IO Block: ", statbuf.st_blksize);
    printf("%8s%ld\t%s%-12ld%s%d\n", "Device: ", statbuf.st_dev, "Inode: ", statbuf.st_ino, "Link: ", statbuf.st_nlink);
    printf("%8s%-20s%s%-20d%s%-20d\n", "Access: ", format_mode(statbuf.st_mode), "Uid: ", statbuf.st_uid, "Gid: ", statbuf.st_gid);
    printf("%8s%s", "Access: ", format_time(statbuf.st_atime));
    printf("%8s%s", "Modify: ", format_time(statbuf.st_mtime));
    printf("%8s%s", "Change: ", format_time(statbuf.st_ctime));
}

char *format_time(time_t tim) {
    static char buf[128];

    struct tm *gm = localtime(&tim);
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d\n", gm->tm_year + 1900, gm->tm_mon, gm->tm_mday, gm->tm_hour, gm->tm_min, gm->tm_sec);

    return buf;
}

// 将模式转成字符串形式
char* format_mode(mode_t mode) {
    static char str[11];

    // 文件类型
    str[0] = 
        S_ISREG(mode) ? '-' :
        S_ISDIR(mode) ? 'd' :
        S_ISLNK(mode) ? 'l' :
        S_ISCHR(mode) ? 'c' :
        S_ISBLK(mode) ? 'b' :
        S_ISFIFO(mode) ? 'p' :
        S_ISSOCK(mode) ? 's' : '?';

    // 用户权限
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 
        ((mode & S_ISUID) ? 's' : 'x') :
        ((mode & S_ISUID) ? 'S' : '-');

    // 组权限
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 
        ((mode & S_ISGID) ? 's' : 'x') :
        ((mode & S_ISGID) ? 'S' : '-');

    // 其他权限
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 
        ((mode & S_ISVTX) ? 't' : 'x') :
        ((mode & S_ISVTX) ? 'T' : '-');

    str[10] = '\0';
    return str;
}
