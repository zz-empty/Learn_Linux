#include "head.h"

int epoll_add(int epfd, int fd) {
    struct epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = fd;

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    RET_CHECK(ret, -1, "EPOLL_CTL_ADD");
    return 1;
}
