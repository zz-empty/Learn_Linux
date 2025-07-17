#include "head.h"

void my_ll(const char *filename);
void print_ll(DIR *dirp);
char* format_mode(mode_t mode);
char* format_mtime(time_t mtime);

int main(int argc, char **argv)
{
    // ./my_ll [filename]
    ARGS_CHECK(argc, 2);

    my_ll(argv[1]);

    return 0;
}

void my_ll(const char *filename) {
    // 打开目录流
    DIR *dirp = opendir(filename);
    print_ll(dirp);
    // 关闭目录流
    closedir(dirp);
}

void print_ll(DIR *dirp) {
    // 用来获取目录项的详细信息
    struct dirent *pd;
    struct stat statbuf;

    // 遍历目录项
    while ((pd = readdir(dirp))) {
        stat(pd->d_name, &statbuf);
        printf("%s %d %-s %-s %6lu %13s %s\n", 
               format_mode(statbuf.st_mode),
               statbuf.st_nlink,
               getpwuid(statbuf.st_uid)->pw_name,
               getgrgid(statbuf.st_gid)->gr_name,
               statbuf.st_size,
               format_mtime(statbuf.st_mtime),
               pd->d_name);
    }
}

// 格式化时间
char* format_mtime(time_t mtime) {
    // 将最后修改时间变成字符串形式
    static char buf[13];

    struct tm *timeinfo = localtime(&mtime);
    if (!timeinfo) {
        strcpy(buf, "Unknown");
        return buf;
    }

    // 月份缩写映射表
    static const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    // 格式化字符串: Mon day H:m
    snprintf(buf, sizeof(buf), "%-3s %2d %02d:%02d", 
             months[timeinfo->tm_mon],
             timeinfo->tm_mday,
             timeinfo->tm_hour,
             timeinfo->tm_min);

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
