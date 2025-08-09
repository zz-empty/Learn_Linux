#include "head.h"

int main(int argc, char **argv)
{
    // ./app conf
    ARGS_CHECK(argc, 2);

    // 加载配置信息
    Config_t cfg = {};
    loadConfig(argv[1], &cfg);
    printConfig(&cfg);
    printf("\n");

    // 进入家目录
    chdir(cfg.home);
    printf("[info] Entry home dir\n");

    // 建立tcp连接
    int serverFd = tcpConnect(cfg);
    printf("[info] already connect server!\n");

    // 接收文件
    receiveFile(serverFd);

    close(serverFd);
    return 0;
}

