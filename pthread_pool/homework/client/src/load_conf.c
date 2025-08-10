#include "head.h"

// 去除前后空格
static char *trim(char *str) {
    char *end;
    // 去除前导空格
    while (isspace((unsigned char)*str)) str++;  
    // 如果空直接返回
    if (*str == '\0') {
        return str;
    }
    
    // 去除后导空格
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

int loadConfig(const char *filename, Config_t *cfg) {
    // 设置默认值
    memset(cfg, 0, sizeof(Config_t));
    // 读取配置信息
    FILE *fp = (FILE*)fopen(filename, "r+");
   
    char line[LINE_SIZE] = "";
    while (fgets(line, LINE_SIZE, fp)) {
        char *p = trim(line);
        // 如果是空行跳过
        if (p[0] == '\0' || p[0] == '\n' || p[0] == '#') {
            continue;
        }
        
        // 分隔值和value
        char *eq = strchr(line, '=');
        if (!eq) {
            fprintf(stderr, "Invaild config, format error!\n");
            return -1;
        }

        *eq = '\0';
        char *key = trim(p);
        char *value = trim(eq + 1);

        // 根据key值来分配value
        if (strcmp(key, "ip") == 0) {
            /* printf("[info] allocate ip address!\n"); */
            cfg->ip = strdup(value);
            /* cfg->ip = (char*)malloc(strlen(value)); */
            /* memcpy(cfg->ip, value, strlen(value)); */
        }
        else if (strcmp(key, "port") == 0) {
            cfg->port = atoi(value);
        }
        else if (strcmp(key, "workers") == 0) {
            cfg->workers = atoi(value);
        }
        else if (strcmp(key, "home") == 0) {
            /* printf("[info] allocate home address!\n"); */
            cfg->home = strdup(value);
            /* cfg->home = (char*)malloc(strlen(value)); */
            /* memcpy(cfg->home, value, strlen(value)); */
        }
    }

    fclose(fp);
    return 0;
}

void freeConfig(Config_t *cfg) {
    free(cfg->ip);
    free(cfg->home);
}

void printConfig(Config_t cfg) {
    printf("[ip] %s\n", cfg.ip);
    printf("[port] %d\n", cfg.port);
    printf("[workers] %d\n", cfg.workers);
    printf("[home] %s\n", cfg.home);
}
