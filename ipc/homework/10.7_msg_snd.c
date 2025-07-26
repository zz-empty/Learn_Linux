#include "head.h"
#include "msg.h"

int main(int argc, char **argv)
{
    // ./send type msg
    ARGS_CHECK(argc, 3);
    int msgid = msgget(1000, IPC_CREAT|0600); 
    RET_CHECK(msgid, -1, "msgget");

    Msg_t msg;
    msg.mtype = atoi(argv[1]);
    strncpy(msg.mtext, argv[2], sizeof(msg.mtext));
    msgsnd(msgid, &msg, strlen(msg.mtext), 0);
    return 0;
}

