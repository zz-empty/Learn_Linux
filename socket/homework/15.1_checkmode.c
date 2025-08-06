#include "head.h"

int main()
{
    unsigned int a = 0x12345678;
    // 78是低位的
    // 小端存储：78 56 34 12
    // 大端存储：12 34 56 78
    char *c = (char*)&a;

    if (*c == 0x78) {
        printf("小端\n");
    } else if (*c == 0x12) {
        printf("大端\n");
    }
    return 0;
}

