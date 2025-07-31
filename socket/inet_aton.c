#include "head.h"

int main(int argc, char **argv)
{
    struct in_addr addr;
    memset(&addr, 0, sizeof(addr));
    inet_aton(argv[1], &addr);

    printf("n:%x\n", addr.s_addr);
    printf("a:%s\n", inet_ntoa(addr));
    return 0;
}

