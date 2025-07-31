#include <stdio.h>

int main()
{
    int a = 1;
    char *p = (char*)&a;
    printf("%s\n", p);
    return 0;
}

