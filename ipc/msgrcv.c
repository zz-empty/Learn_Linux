#include "head.h"
#include "msg.h"

int main(int argc, char **argv)
{
    int msgid = msgget(1000, IPC_CREAT | 0600);
    RET_CHECK(msgid, -1, "msgget");


    Msg_t msg;
    msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0);
    printf("type=%ld, text=%s\n", msg.mtype, msg.mtext);

    return 0;
}

