#include "head.h"

int main()
{
    if (fork()) {
        puts("I am parent!\n");
        while(1);
    }
    else {
        puts("I am child\n");
        return 0;
    }
    return 0;
}

