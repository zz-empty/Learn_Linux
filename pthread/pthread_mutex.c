#include "head.h"

int main()
{
    pthread_mutex_t mutex;
    int ret;
    ret = pthread_mutex_init(&mutex, NULL);
    PTHREAD_RET_CHECK(ret, "init mutex");
    printf("初始化锁！\n");

    ret = pthread_mutex_lock(&mutex);
    PTHREAD_RET_CHECK(ret, "1 lock mutex");

    ret = pthread_mutex_trylock(&mutex);
    PTHREAD_RET_CHECK(ret, "trylock mutex");

    ret = pthread_mutex_unlock(&mutex);
    PTHREAD_RET_CHECK(ret, "unlock mutex");


    ret = pthread_mutex_destroy(&mutex);
    PTHREAD_RET_CHECK(ret, "destory mutex");
    printf("销毁锁!\n");
    return 0;
}
