#include "head.h"

int main()
{
    execl("./bin/add", "./bin/add", "4", "7", NULL);
    printf("you can't see me\n");
    return 0;
}

