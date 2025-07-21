#include "head.h"
#include "msg.h"

int main(int argc, char **argv)
{
    ARGS_CHECK(argc, 2);
    int msgid = msgget(1000, IPC_CREAT | 0600);
    RET_CHECK(msgid, -1, "msgget");


    Msg_t msg;
    msg.mtype = 1;
    strcpy(msg.mtext, argv[1]);
    msgsnd(msgid, &msg, strlen(msg.mtext), 0);

    return 0;
}

