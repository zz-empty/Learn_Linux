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
        ret = write(fd, &stu[i], sizeof(Stu));
        RET_CHECK(ret, -1, "write");
    }

    printf("-------------read file--------------\n");
    lseek(fd, 0, SEEK_SET);
    for (int i = 0; i < 3; ++i) {
        memset(buf, 0, sizeof(buf));
        ret = read(fd, buf, sizeof(Stu));
        RET_CHECK(ret, -1, "read");

        Stu tmp = *(Stu*)buf;
        printf("%d\t%s\t%f\n", tmp.id, tmp.name, tmp.score);
    }

    close(fd);
    return 0;
}

