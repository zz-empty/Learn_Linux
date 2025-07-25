#include "head.h"

int main(int argc, char **argv)
{
    // 返回两个数之间的随机数
    ARGS_CHECK(argc, 3);
    int lhs = atoi(argv[1]);
    int rhs = atoi(argv[2]);

    srand(time(NULL));
    int range = rhs - lhs + 1;
    printf("the random num: %d\n", rand() % range + lhs);
    return 0;
}

