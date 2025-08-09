#include "head.h"
#include "pool.h"

// Master分配任务
int sendFd(int pipeFd, Task_t *task) {
    struct msghdr msg = {};    

    // msg的3、4参数
    struct iovec iov = {
        .iov_base = task,
        .iov_len = sizeof(Task_t)
    };

    // 挂载到msg
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // msg的5、6参数
    int len = CMSG_LEN(sizeof(Task_t));
    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1, len);
    cmsg->cmsg_len = len;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *(Task_t*)CMSG_DATA(cmsg) = *task;

    // 挂载到msg
    msg.msg_control = cmsg;
    msg.msg_controllen = len;
    
    // 传输给pipeFd
    int ret = sendmsg(pipeFd, &msg, 0);
    RET_CHECK(ret, -1, "sendmsg");

    free(cmsg);
    return 0;
}

// Worker接收任务
int recvFd(int pipeFd, Task_t *task) {
    struct msghdr msg = {};
    
    // msg的3、4参数
    struct iovec iov = {};
    // 挂载到msg
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // msg的5、6参数
    int len = CMSG_LEN(sizeof(Task_t));
    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1, len);
    // 挂载到msg
    msg.msg_control = cmsg;
    msg.msg_controllen = len;

    // 从pipeFd中接收
    int ret = recvmsg(pipeFd, &msg, 0);
    RET_CHECK(ret, -1, "recvmsg");

    // 卸货
    *task = *(Task_t*)CMSG_DATA(cmsg);
    free(cmsg);
    return 0;
}

#if 0
// 测试上面两个接口
int main() {
    int sfd[2];
    socketpair(AF_LOCAL, SOL_SOCKET, 0, sfd);

    if (fork() == 0) {
        close(sfd[1]);
        Task_t task = {};
        recvFd(sfd[0], &task);

        // 写入文件数据
        const char *buf = "I am child process, second write!";
        write(task.client_fd, buf, strlen(buf));

        exit(0);
    }

    close(sfd[0]);

    int fd = open("testfile", O_RDWR | O_CREAT | O_TRUNC, 0600);

    const char *buf = "hello file, I am main process, first write!";
    write(fd, buf, strlen(buf));

    Task_t task = {};
    task.client_fd = fd;
    task.exitFlag = 0;
    sendFd(sfd[1], &task);

    wait(NULL);
    close(fd);
    return 0;
}
#endif
