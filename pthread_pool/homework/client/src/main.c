#include "head.h"

int tcp_connect(Config_t cfg) {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(cfg.ip);
    serAddr.sin_port = htons(cfg.port);

    int ret = connect(serverFd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "connect");

    return serverFd;
}

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);

    // 读取配置文件信息
    Config_t cfg;
    loadConfig(argv[1], &cfg);
    printConfig(cfg);
    printf("\n");

    // 进入家目录
    chdir(cfg.home);

    // 创建TCP套接字连接服务器
    int serverFd = tcp_connect(cfg);

    // 接收文件
    receiveFile(serverFd);

    close(serverFd);
    return 0;
}
