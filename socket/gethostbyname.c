#include "head.h"

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    struct hostent *pHost = gethostbyname(argv[1]);
    if (!pHost) {
        return -1;
    }

    // 主机名
    printf("[hostname] %s\n", pHost->h_name);

    // 别名
    for (int i = 0; pHost->h_aliases[i]; ++i) {
        printf("[alias] %s\n", pHost->h_aliases[i]);
    }

    // 地址类型
    printf("[addrtype] %d\n", pHost->h_addrtype);
    // 地址长度
    printf("[addlen] %d\n", pHost->h_length);

    // IP地址
    char buf[64] = "";
    for (int i = 0; pHost->h_addr_list[i]; ++i) {
        inet_ntop(pHost->h_addrtype, pHost->h_addr_list[i], buf, sizeof(buf));
        printf("[ip] %s\n", buf);
    }
    
    return 0;
}

