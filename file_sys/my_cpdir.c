#include "head.h"

int copy_dir(const char *dir1, const char *dir2);
int copy_file(const char *f1, const char *f2);

int main(int argc, char **argv)
{
    // ./cp dir1 dir2
    ARGS_CHECK(argc, 3);
    copy_dir(argv[1], argv[2]);
    return 0;
}

int copy_file(const char *f1, const char *f2) {
    int fdr = open(f1, O_RDONLY);
    int fdw = open(f2, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    char buf[1024] = "";
    while (1) {
        int ret = read(fdr, buf, sizeof(buf));
        RET_CHECK(ret, -1, "read");
        if (0 == ret) {
            /* printf("复制完毕!\n"); */
            break;
        }
        ret = write(fdw, buf, ret);
        RET_CHECK(ret, -1, "write");
    }

    close(fdr);
    close(fdw);
    return 0;
}

int copy_dir(const char *dir1, const char *dir2) {
    // 如果dir2不存在则创建
    struct stat st;
    stat(dir1, &st);
    if (access(dir2, F_OK)) {
        mkdir(dir2, st.st_mode & 07777);
    }
    // 处理dir1中的每个文件
    DIR *dirp = opendir(dir1);
    
    struct dirent *pd;
    char newd1[1024];
    char newd2[1024];

    while ((pd = readdir(dirp))) {
        // 屏蔽. ..
        if (!strcmp(pd->d_name, ".") || !strcmp(pd->d_name, "..")) continue;

        snprintf(newd1, sizeof(newd1), "%s%s%s", dir1, "/", pd->d_name);
        snprintf(newd2, sizeof(newd2), "%s%s%s", dir2, "/", pd->d_name);
        // 递归目录
        if (pd->d_type == DT_DIR) {
            copy_dir(newd1, newd2);
        }

        if (pd->d_type == DT_REG) {
            copy_file(newd1, newd2);    
        }
    }

    closedir(dirp);
    return 0;
}
