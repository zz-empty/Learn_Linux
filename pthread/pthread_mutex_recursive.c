#include "head.h"

int main()
{
    int ret = 0;
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr;
    ret = pthread_mutexattr_init(&mutexattr);
    PTHREAD_RET_CHECK(ret, "init mutexattr");
    ret = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE); // 嵌套锁，可以多次加锁
    PTHREAD_RET_CHECK(ret, "set mutexattr");
    ret = pthread_mutex_init(&mutex, &mutexattr);
    PTHREAD_RET_CHECK(ret, "init mutex");

    // 测试多次加锁
    ret = pthread_mutex_lock(&mutex);
    PTHREAD_RET_CHECK(ret, "lock mutex");
    printf("1 lock!\n");
    ret = pthread_mutex_lock(&mutex);
    PTHREAD_RET_CHECK(ret, "lock mutex");
    printf("2 lock!\n");

    ret = pthread_mutex_unlock(&mutex);
    PTHREAD_RET_CHECK(ret, "unlock mutex");
    ret = pthread_mutex_unlock(&mutex);
    PTHREAD_RET_CHECK(ret, "unlock mutex");

    ret = pthread_mutex_destroy(&mutex);
    PTHREAD_RET_CHECK(ret, "destory mutex");

    return 0;
}

