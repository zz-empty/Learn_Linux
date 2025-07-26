#include "head.h"
#include "chat.h"
#define PID_FILE "pid_file"

int msgid;

void write_pid() {
    FILE *f = (FILE*)fopen(PID_FILE, "a+");
    pid_t pid = getpid();
    fprintf(f, "%s %d\n", "B1", pid);
    fclose(f);
}

void cleanup_ipc() {
    msgctl(msgid, IPC_RMID, NULL);
}

void sig_handler(int signum) {
    pid_t pids[4] = {};
    FILE *f = (FILE*)fopen("pid_file", "r");
    char buf[10];
    for (int i = 0; i < 4; ++i) {
        fscanf(f, "%s%d", buf, &pids[i]);
        kill(pids[i], SIGUSR1);
    }
    fclose(f);

    cleanup_ipc();
    exit(EXIT_SUCCESS);
}

int main()
{
    write_pid();
    signal(SIGINT, sig_handler);
    signal(SIGUSR1, sig_handler);

    msgid = msgget(1000, IPC_CREAT|0600);
    Msg_t msg;
    memset(&msg, 0, sizeof(msg));

    while (1) {
        memset(&msg, 0, sizeof(msg));
        int ret = msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0);
        RET_CHECK(ret, -1, "msgrcv");
        if (1 == msg.mtype) {
            printf("[local]: %s\n", msg.mtext);
        } else if (2 == msg.mtype) {
            printf("[remote]: %s\n", msg.mtext);
        }
    }
    return 0;
}

