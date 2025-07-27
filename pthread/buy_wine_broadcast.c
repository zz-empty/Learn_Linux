#include "head.h"

typedef struct {
    int num;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Data_t;

void *threadfunc(void *val) {
    // 子线程买酒
    Data_t *data = (Data_t*)val;

    // 准备买酒
    pthread_mutex_lock(&data->mutex);
    if (0 == data->num) {
        // 没酒了，登记
        printf("before wait!\n");
        pthread_cond_wait(&data->cond, &data->mutex);
        printf("after wait!\n");
    }
    if (0 < data->num) {
        data->num--;
        printf("Child, buy a wine, very nice!\n");
    }
    pthread_mutex_unlock(&data->mutex);

    pthread_exit(NULL);
}

void init_data(Data_t *data, int num) {
    // 初始化锁。条件变量
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->cond, NULL);
    // 初始化酒的数量
    data->num = num;
}

void destroy_data(Data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->cond);
}

int main()
{
    int ret;
    pthread_t thid;
    pthread_t thid2;
    Data_t data;

    // 初始化data
    init_data(&data, 0);

    // 创建一个线程
    ret = pthread_create(&thid, NULL, threadfunc, &data);
    PTHREAD_RET_CHECK(ret, "create pthread");

    // 再创建一个线程
    ret = pthread_create(&thid2, NULL, threadfunc, &data);
    PTHREAD_RET_CHECK(ret, "create pthread2");

    // 等待子线程买酒, 这里后面应该能优化，这么写太low了
    sleep(1);

    // 主线程卖酒
    ret = pthread_mutex_lock(&data.mutex);
    PTHREAD_RET_CHECK(ret, "lock");

    // 生产10瓶酒
    data.num += 10;
    printf("---main---生产了10瓶酒-----\n");
    // 通知买家来买
    ret = pthread_cond_broadcast(&data.cond);
    PTHREAD_RET_CHECK(ret, "cond signal");

    ret = pthread_mutex_unlock(&data.mutex);
    PTHREAD_RET_CHECK(ret, "unlock");

    // 回收线程
    ret = pthread_join(thid, NULL);
    PTHREAD_RET_CHECK(ret, "join pthread");
    ret = pthread_join(thid2, NULL);
    PTHREAD_RET_CHECK(ret, "join pthread2");

    // 销毁data
    destroy_data(&data);

    return 0;
}

