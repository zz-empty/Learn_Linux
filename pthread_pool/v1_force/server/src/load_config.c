#include "config.h"
#include "head.h"

// 取出字符串首位空白
static char *trim(char *s) {
    char *end;
    // 左侧
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    // 右侧
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return s;
}

int load_config(const char *filename, Config_t *cfg) {
    FILE *fp = fopen(filename, "r+");
    RET_CHECK(fp, NULL, "fopen");

    // 给默认值，防止默认字段缺失
    strncpy(cfg->ip, "127.0.0.1", MAX_IP_LEN);
    cfg->port = 0;
    cfg->workers = 0;

    char line[MAX_LINE_LEN] = "";
    while (fgets(line, MAX_LINE_LEN, fp)) {
        char *p = trim(line);
        // 跳过空行和注释
        if (p[0] == '\0' || p[0] == '#' || p[0] == ';') {
            continue;
        }
        // 分割key=value
        char *eq = strchr(p, '=');
        if (!eq) {
            fprintf(stderr, "invaild config line: %s\n", p);
            continue;
        }
        *eq = '\0';
        char *key = trim(p);
        char *value = trim(eq + 1);

        // 根据key赋值
        if (strcmp(key, "ip") == 0) {
            strncpy(cfg->ip, value, MAX_IP_LEN - 1);
            cfg->ip[MAX_IP_LEN - 1] = '\0';
        }
        else if (strcmp(key, "port") == 0) {
            cfg->port = atoi(value);
        }
        else if (strcmp(key, "workers") == 0) {
            cfg->workers = atoi(value);
        } 
        else if (strcmp(key, "dir") == 0) {
            strncpy(cfg->dir, value, MAX_LINE_LEN - 1);
            cfg->dir[MAX_LINE_LEN - 1] = '\0';
        }
        else {
            fprintf(stderr, "unknown key: %s\n", key);
        }
    }

    fclose(fp);
    return 1;
}
