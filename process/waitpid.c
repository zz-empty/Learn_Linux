#include "head.h"

int main()
{
    pid_t pid = fork();

    if (!pid) {
        printf("child, pid = %d, ppid = %d\n", getpid(), getppid());
        while(1);
    } else {
        printf("I am parent!\n");
        int status;
        
        while (1) {
            int ret = waitpid(pid, &status, WNOHANG);
            if (ret) {
                if (WIFEXITED(status)) {
                    printf("exit status: %d\n", WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    printf("crash, signum is %d\n", WTERMSIG(status));
                }
                break;
            } else {
                sleep(1);
                printf("No child status was changes!\n");
            }
        }
    }
    return 0;
}

