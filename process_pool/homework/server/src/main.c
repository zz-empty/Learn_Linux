#include "head.h"

int main(int argc, char **argv)
{
    // ./app conf
    ARGS_CHECK(argc, 2);
    // 加载配置信息
    Config_t cfg;
    loadConfig(argv[1], &cfg);
    printConfig(&cfg);

    // 创建线程池
    // 建立tcp连接
    // 建立epoll监听
    // 退出逻辑

    freeConfig(&cfg);
    return 0;
}
