#include "process_pool.h"

int worker_func(int pipefd) {
    int client_fd = 0;
    while (1) {
        // 等待任务
        int ret = recv_fd(pipefd, &client_fd);
        RET_CHECK(ret, -1, "recv_fd");

        printf("------worker start, client_fd = %d-------\n", client_fd);
        // 执行任务
        transfer_file(client_fd, "file1");
        printf("------worker over, client_fd = %d-------\n", client_fd);


        // 关掉
        close(client_fd);
        // 通知父进程，任务执行完毕
        write(pipefd, "a", 1);
    }
}

int create_pool(Pool_t *pool) {
    // 创建num个worker进程
    pid_t pid;
    int sfd[2];
    int ret;

    for (int i = 0; i < pool->size; ++i) {
        ret = socketpair(AF_LOCAL, SOL_SOCKET, 0, sfd);
        RET_CHECK(ret, -1, "socketpair");
        pid = fork();

        // 工作进程关闭写端，管道只用来读取
        if (0 == pid) {
            close(sfd[1]);
            worker_func(sfd[0]);
            exit(0);
        }

        // Master进程关闭读端
        close(sfd[0]);
        // 将工作线程加入管理
        pool->workers[i].busy = 0;
        pool->workers[i].pid = pid;
        pool->workers[i].pipefd = sfd[1];
    }

    return 1;
}
