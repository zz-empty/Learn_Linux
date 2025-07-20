#include "head.h"

void re() {
    printf("I am quit...\n");
}

void func() {
    printf("func\n");
    exit(3);
}

int main()
{
    atexit(re);
    func();
    return 0;
}

