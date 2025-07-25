#include "head.h"

int mysystem(const char *cmd) {
    if (!cmd) {
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (0 == pid) {
        // 执行命令
        execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);
        
        // 如果execl失败
        perror("execl failed");
        _exit(127);
    }

    // 父进程回收子进程资源
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    return status;
}

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int status = mysystem(argv[1]);
    if (WIFEXITED(status)) {
        printf("Exit code: %d\n", WEXITSTATUS(status));
    } else {
        printf("Command failed\n");
    }
    return 0;
}

