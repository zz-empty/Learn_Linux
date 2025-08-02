#include "head.h"
#include "process_pool.h"

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);

    // 获取ip、port、workersnum
    Config_t cfg;

    load_config(argv[1], &cfg);

    printf("ip = %s\n", cfg.ip);
    printf("port = %d\n", cfg.port);
    printf("workersnum = %d\n", cfg.workers);


    return 0;
}

