#include "head.h"

void Deamon() {
    if (fork()) {
        exit(0);
    }
    setsid();
    chdir("/");
    umask(0);
}

int main()
{
    Deamon();
    while (1) {
        sleep(3);
    }
    return 0;
}

