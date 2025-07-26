#include "head.h"
#include "msg.h"

int main()
{
    int msgid = msgget(1000, IPC_CREAT|0600); 
    RET_CHECK(msgid, -1, "msgget");

    Msg_t msg;
    int ret = msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0);
    RET_CHECK(ret, -1, "msgrcv");
    printf("type:%ld, text:%s", msg.mtype, msg.mtext);
    return 0;
}

