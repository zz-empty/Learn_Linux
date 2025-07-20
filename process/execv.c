#include "head.h"

int main()
{
    char * const str[] = {"./bin/add", "8", "9", NULL};
    if (0 == fork()) {
        execv("./bin/add", str);
        printf("you can't see me\n");
    }
    else {
        puts("you can see me");
    }
    return 0;
}

