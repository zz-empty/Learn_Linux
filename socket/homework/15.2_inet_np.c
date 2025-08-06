#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    const char *ip = "127.0.0.1";
    struct in_addr serAddr;

    // 使用inet_addr inet_ntoa inet_aton
    inet_aton(ip, &serAddr);
    printf("net:%d\n", serAddr.s_addr);
    printf("net:%d\n", inet_addr(ip));
    printf("host:%s\n", inet_ntoa(serAddr));

    // 使用inet_pton inet_ntop
    printf("-----------------\n");
    inet_pton(AF_INET, ip, &serAddr);
    printf("net:%d\n", serAddr.s_addr);
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &serAddr, ip_str, INET_ADDRSTRLEN);
    printf("ip_str:%s\n", ip_str);
    return 0;
}

