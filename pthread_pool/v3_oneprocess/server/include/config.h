#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_IP_LEN 64
#define MAX_LINE_LEN 256

// 配置信息结构
typedef struct {
    char ip[MAX_IP_LEN];
    int port;
    int workers;
    char dir[MAX_LINE_LEN];
} Config_t;

// 加载配置文件
int load_config(const char *filename, Config_t *cfg);
// 建立tcp监听
int tcp_listen(Config_t cfg);

#endif
