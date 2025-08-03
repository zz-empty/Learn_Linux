#include "head.h"

// 向管道里传入打开的文件描述符
int send_fd(int pipefd, int fd, char exitflag) {
    struct msghdr msg = {};

    // 设置msg的3、4参数
    struct iovec iov = {.iov_base = &exitflag, .iov_len = sizeof(char)};
    // 挂载到msg
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // 5、6参数放fd
    int len = CMSG_LEN(sizeof(int));
    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1, len);
    cmsg->cmsg_len = len;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *(int*)CMSG_DATA(cmsg) = fd;
    // 挂载到msg
    msg.msg_control = cmsg;
    msg.msg_controllen = len;

    // 发送
    int ret = sendmsg(pipefd, &msg, 0);
    RET_CHECK(ret, -1, "sendmsg");

    free(cmsg);
    return 1;
}

// 从管道中读取打开的文件描述符
int recv_fd(int pipefd, int *fd, char *exitflag) {
    struct msghdr msg = {};

    // 设置msg的3、4参数
    struct iovec iov = {.iov_base = exitflag, .iov_len = sizeof(char)};
    // 挂载到msg
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // 5、6参数放fd
    int len = CMSG_LEN(sizeof(int));
    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1, len);
    cmsg->cmsg_len = len;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    // 挂载到msg
    msg.msg_control = cmsg;
    msg.msg_controllen = len;

    // 接收
    int ret = recvmsg(pipefd, &msg, 0);
    RET_CHECK(ret, -1, "recvmsg");

    *fd = *(int*)CMSG_DATA(cmsg);

    free(cmsg);
    return 1;
}
