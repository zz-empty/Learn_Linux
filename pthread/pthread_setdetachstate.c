#include "head.h"

void *threadfunc(void *val) {
    printf("I am child thread\n");
    pthread_exit(NULL);
}

int main()
{
    int ret;
    pthread_t thid;
    pthread_attr_t attr;
    ret = pthread_attr_init(&attr);
    PTHREAD_RET_CHECK(ret, "init attr");
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    PTHREAD_RET_CHECK(ret, "set detach attr");
    ret = pthread_create(&thid, &attr, threadfunc, NULL);
    PTHREAD_RET_CHECK(ret, "create thid");

    printf("I am main thread!\n");

    ret = pthread_join(thid, NULL);
    PTHREAD_RET_CHECK(ret, "join thid");
    return 0;
}

