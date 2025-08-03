#include "head.h"

// 向管道里传入打开的文件描述符
int send_fd(int pipefd, int fd) {
    struct msghdr msg = {};

    // 设置msg的3、4参数
    char buf[] = "a";
    struct iovec iov = {.iov_base = buf, .iov_len = strlen(buf)};
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
int recv_fd(int pipefd, int *fd) {
    struct msghdr msg = {};

    // 设置msg的3、4参数
    char buf[] = "a";
    struct iovec iov = {.iov_base = buf, .iov_len = strlen(buf)};
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

    // 发送
    int ret = recvmsg(pipefd, &msg, 0);
    RET_CHECK(ret, -1, "recvmsg");

    *fd = *(int*)CMSG_DATA(cmsg);

    free(cmsg);
    return 1;
}


// 测试上面两个接口
int test_sendfd()
{

    // 创建一对用于本地通信的套接字
    int sfd[2];
    int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, sfd);
    RET_CHECK(ret, -1, "socketpair");

    if (fork()) {
        // 子进程发送打开的文件描述符
        close(sfd[0]);
        int fd = open("file", O_RDWR | O_CREAT, 0600);
        RET_CHECK(fd, -1, "open");

        // 向fd发送信息
        write(fd, "dddd", 4);

        send_fd(sfd[1], fd);
        printf("child fd = %d\n", fd);
        exit(0);
    } 

    // 接收子进程发来的fd
    close(sfd[1]);
    int fd;
    recv_fd(sfd[0], &fd);
    printf("parent fd = %d\n", fd);

    // 对fd操作
    write(fd, "dad", 3);

    wait(NULL);

    return 0;
}

