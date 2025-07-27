#include "head.h"

int main()
{
    int ret;
    pthread_cond_t cond;
    ret = pthread_cond_init(&cond, NULL);
    PTHREAD_RET_CHECK(ret, "init cond");

    ret = pthread_cond_destroy(&cond);
    PTHREAD_RET_CHECK(ret, "destory cond");
    return 0;
}

