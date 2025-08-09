#include "head.h"

int tcpConnect(Config_t cfg) {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    RET_CHECK(serverFd, -1, "socket");
    int ret = 0;

    /* // 设置地址可重用 */
    /* int reuse = 1; */
    /* setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); */

    // 绑定IP和PORT
    struct sockaddr_in serAddr = {};
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(cfg.ip);
    serAddr.sin_port = htons(cfg.port);

    // 连接服务器
    ret = connect(serverFd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    RET_CHECK(ret, -1, "connect");

    return serverFd;
}
