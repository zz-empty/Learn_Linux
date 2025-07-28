#include "head.h"

typedef struct {
    int num;
    pthread_mutex_t mutex;
} Data_t;

void *threadfunc(void *val) {
    int cnt = 0;
    Data_t *data = (Data_t*)val;
    while (data->num > 0) {
        pthread_mutex_lock(&data->mutex);
        if (data->num > 0) {
            data->num--;
            cnt++;
        }
        pthread_mutex_unlock(&data->mutex);
    }

    printf("thread %ld, cnt = %d\n", pthread_self(), cnt);
    pthread_exit(NULL);
}

int main()
{
    pthread_t t1, t2, t3;
    Data_t data;
    data.num = 1000000;
    pthread_mutex_init(&data.mutex, NULL);

    pthread_create(&t1, NULL, threadfunc, &data);
    pthread_create(&t2, NULL, threadfunc, &data);
    pthread_create(&t3, NULL, threadfunc, &data);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    return 0;
}

