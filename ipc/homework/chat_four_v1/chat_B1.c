#include "head.h"
#include "chat.h"

int main()
{
    int msgid = msgget(1000, IPC_CREAT|0600);
    Msg_t msg;
    memset(&msg, 0, sizeof(msg));
    

    while (1) {
        memset(&msg, 0, sizeof(msg));
        int ret = msgrcv(msgid, &msg, sizeof(msg.mtext), 0, 0);
        RET_CHECK(ret, -1, "msgrcv");
        if (1 == msg.mtype) {
            printf("stdin info: %s\n", msg.mtext);
        } else if (2 == msg.mtype) {
            printf("pipe info: %s\n", msg.mtext);
        }
    }
    return 0;
}

