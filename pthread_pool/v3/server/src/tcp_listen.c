#include "head.h"
#include "config.h"

int tcp_listen(Config_t cfg) {
    int server_fd = socket(AF_INET, SOL_SOCKET, 0);
    RET_CHECK(server_fd, -1, "socket");

    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(cfg.ip);
    serAddr.sin_port = htons(cfg.port);

    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int ret;
    ret = bind(server_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "bind");

    ret = listen(server_fd, 10);
    RET_CHECK(ret, -1, "listen");

    return server_fd;
}
