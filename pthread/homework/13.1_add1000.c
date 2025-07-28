#include "head.h"

#define MAX 10000000

typedef struct {
    int num;
    pthread_mutex_t mutex;
} Data_t;

void cleanupfunc(void *val) {
    printf("cleanup!\n");
    pthread_mutex_unlock((pthread_mutex_t*)val);
}

void *sigfunc(void *val) {
    Data_t *data = (Data_t*)val;
    for (int i = 0; i < MAX; ++i) {
        pthread_mutex_lock(&data->mutex);
        data->num += 1;
        pthread_cleanup_push(cleanupfunc, &data->mutex);
        pthread_mutex_unlock(&data->mutex);
        pthread_cleanup_pop(0);
    }
    printf("%ld, finish work!\n", pthread_self());

    pthread_exit(NULL);
}

int main()
{
    // 初始化锁和初值
    Data_t data;
    data.num = 0;
    pthread_mutex_init(&data.mutex, NULL);

    // 统计计算时间
    struct timeval beg, end;
    gettimeofday(&beg, NULL);

    // 创建两个线程，各加1000万
    pthread_t t1, t2;
    pthread_create(&t1, NULL, sigfunc, &data);
    pthread_create(&t2, NULL, sigfunc, &data);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    gettimeofday(&end, NULL);
    long totaltime = (end.tv_sec - beg.tv_sec) * 1000000 + end.tv_usec - beg.tv_usec;
    long avgtime = totaltime / (2 * MAX);

    printf("total num = %d, totaltime = %ld, avgtime = %ld\n", 
           data.num, totaltime, avgtime);
    return 0;
}
