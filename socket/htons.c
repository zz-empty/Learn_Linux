#include "head.h"

int main()
{
    short port =0X1234;
    short nport = htons(port);
    printf("nport = %x\n", nport);
    port = ntohs(nport);
    printf("port = %x\n", port);
    return 0;
}

