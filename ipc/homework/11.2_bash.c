#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 128

void sigint_handler(int signum) {
    // 仅打印换行符和提示符，避免退出程序
    printf("\nmybash> ");
    fflush(stdout);
}

// 将一个字符串拆分
void parse_input(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " \t\n");
    while (args[i] && i < MAX_ARGS - 1) {
        args[++i] = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
}

int main()
{
    char input[MAX_LINE];
    char *args[MAX_ARGS];

    // 设置SIGINT处理（Ctrl + c）
    struct sigaction act;
    act.sa_handler = sigint_handler;
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    while (1) {
        // 打印标识符
        printf("\nmybash> ");
        fflush(stdout);

        if (fgets(input, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;
        }

        parse_input(input, args);
        if (args[0] == NULL) continue;

        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: missing argument\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("chdir");
                }
            }
            continue;
        }

        if (strcmp(args[0], "pwd") == 0) {
            printf("%s", getcwd(NULL, 0));
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            continue;
        } else if (0 == pid) {
            // 恢复SIGINT
            signal(SIGINT, SIG_DFL);
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}

