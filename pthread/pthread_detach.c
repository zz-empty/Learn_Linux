#include "head.h"

void *threadfunc(void *val) {
    pthread_detach(pthread_self());
    printf("I am child thread\n");
    pthread_exit(NULL);
}

int main()
{
    int ret;
    pthread_t thid;
    ret = pthread_create(&thid, NULL, threadfunc, NULL);
    PTHREAD_RET_CHECK(ret, "create thid");

    printf("I am main thread!\n");
    sleep(1);

    ret = pthread_join(thid, NULL);
    PTHREAD_RET_CHECK(ret, "join thid");
    return 0;
}

