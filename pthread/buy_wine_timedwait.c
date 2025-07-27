#include "head.h"

typedef struct {
    int num;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Data_t;

void *threadfunc(void *val) {
    // 子线程买酒
    Data_t *data = (Data_t*)val;

    // 设置timedwait等待的时间, 指定时间戳, 需要先清空
    struct timespec spec;
    memset(&spec, 0, sizeof(spec));
    spec.tv_sec = time(NULL) + 6;

    // 准备买酒
    pthread_mutex_lock(&data->mutex);
    if (0 == data->num) {
        // 没酒了，登记
        printf("before wait!\n");
        /* pthread_cond_wait(&data->cond, &data->mutex); */
        pthread_cond_timedwait(&data->cond, &data->mutex, &spec);
        printf("after wait!\n");
    }
    if (0 < data->num) {
        data->num--;
        printf("Child, buy a wine, very nice!\n");
    }
    pthread_mutex_unlock(&data->mutex);

    pthread_exit(NULL);
}

int main()
{
    int ret;
    pthread_t thid;
    Data_t data;

    // 初始化锁。条件变量
    ret = pthread_mutex_init(&data.mutex, NULL);
    PTHREAD_RET_CHECK(ret, "init mutex");
    ret = pthread_cond_init(&data.cond, NULL);
    PTHREAD_RET_CHECK(ret, "init cond");
    // 初始化酒的数量
    data.num = 0;

    // 创建一个线程
    ret = pthread_create(&thid, NULL, threadfunc, &data);
    PTHREAD_RET_CHECK(ret, "create pthread");

    // 等待子线程买酒, 这里后面应该能优化，这么写太low了
    sleep(1);

    // 主线程卖酒
    ret = pthread_mutex_lock(&data.mutex);
    // 生产一瓶酒
    data.num++;
    printf("---main---生产了一瓶酒-----\n");
    // 通知买家来买
    ret = pthread_cond_signal(&data.cond);
    ret = pthread_mutex_unlock(&data.mutex);

    // 回收线程
    ret = pthread_join(thid, NULL);
    PTHREAD_RET_CHECK(ret, "join pthread");

    return 0;
}

