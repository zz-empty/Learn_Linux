#include "head.h"

typedef struct Student {
    int id;
    char name[20];
    float score;
} Stu;

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);

    Stu stu[3] = {
        {101, "tom", 98.2},
        {102, "jul", 78.2},
        {103, "jak", 88.7}
    };

    int ret;
    char buf[1024];
    printf("-------------write file-----------\n");
    for (int i = 0; i < 3; ++i) {
        snprintf(buf, sizeof(buf), "%5d\t%-10s%5.2f\n", stu[i].id, stu[i].name, stu[i].score);
        ret = write(fd, buf, strlen(buf));
        RET_CHECK(ret, -1, "write");
    }

    lseek(fd, 0, SEEK_SET);
    memset(buf, 0, sizeof(buf));
    read(fd, buf, sizeof(buf));
    printf("-------------read file--------------\n");
    puts(buf);

    close(fd);
    return 0;
}

