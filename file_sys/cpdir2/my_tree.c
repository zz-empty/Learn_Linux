#include "head.h"

void my_tree(const char *path);
int DFSprint(const char *path, int depth);

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    my_tree(argv[1]);
    return 0;
}

void my_tree(const char *path) {
    puts(path);
    DFSprint(path, 0); 
}

int DFSprint(const char *path, int depth) {
    // 打开目录流
    DIR *dirp = opendir(path);
    if (!dirp) {
        perror("opendir");
        return -1;
    }

    // 得到目录的文件个数
    struct dirent *pd;
    int cnts = 0;
    while ((pd = readdir(dirp))) {
        if (!strcmp(pd->d_name, ".") || !strcmp(pd->d_name, "..") || pd->d_name[0] == '.') continue;
        cnts++;
    }
    rewinddir(dirp);

    // 打印目录中的每个文件信息
    char newpath[1024] = "";
    int cur = 0;
    while ((pd = readdir(dirp))) {
        if (!strcmp(pd->d_name, ".") || !strcmp(pd->d_name, "..") || pd->d_name[0] == '.') continue;
        cur++;
        // 打印tree符号
        for (int i = 0; i < depth; i += 4) {
            printf("|   ");
        }
        cur == cnts ? printf("`--") : printf("|--");
        // 打印文件名
        printf("%s\n", pd->d_name);
        // 递归目录
        if (pd->d_type == DT_DIR) {
            snprintf(newpath, sizeof(newpath), "%s%s%s", path, "/", pd->d_name);
            DFSprint(newpath, depth + 4);
        }
    }

    closedir(dirp);
    return 0;
}
