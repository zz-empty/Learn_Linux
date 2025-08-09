#include "head.h"
#include "pool.h"

int main(int argc, char **argv)
{
    // ./app conf
    ARGS_CHECK(argc, 2);

    // 加载配置信息
    Config_t cfg;
    loadConfig(argv[1], &cfg);
    printConfig(&cfg);

    // 创建进程池
    Pool_t pool;
    initPool(&pool, cfg);
    // 启动
    pool.start = 1;
    printf("[info] pool init succeed!\n");

    // 建立tcp连接
    // 建立epoll监听
    // 退出逻辑

    destroyPool(&pool);
    freeConfig(&cfg);
    return 0;
}
