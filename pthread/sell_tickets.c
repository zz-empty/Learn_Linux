#include "head.h"

typedef struct {
    int num;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t hasticket;
} Data_t;

void *sellTickets1(void *val) {
    Data_t *data = (Data_t*)val;

    while (1) {
        pthread_mutex_lock(&data->mutex);
        if (0 == data->num) {
            // 通知放票
            pthread_cond_signal(&data->cond);
            pthread_cond_wait(&data->hasticket, &data->mutex);
        } else if (0 < data->num) {
            // 卖一张
            data->num -= 1;
            printf("[sell window1] 卖出1张票\n");
        }
        pthread_mutex_unlock(&data->mutex);
        usleep(100000);
    }

    pthread_exit(NULL);
}

void *sellTickets2(void *val) {
    Data_t *data = (Data_t*)val;

    while (1) {
        pthread_mutex_lock(&data->mutex);
        if (0 == data->num) {
            // 通知放票
            pthread_cond_signal(&data->cond);
            pthread_cond_wait(&data->hasticket, &data->mutex);
        } else if (0 < data->num) {
            // 卖一张
            data->num -= 1;
            printf("[sell window2] 卖出1张票\n");
        }
        pthread_mutex_unlock(&data->mutex);
        usleep(100000);
    }

    pthread_exit(NULL);
}

void *releaseTicket(void *val) {
    Data_t *data = (Data_t*)val;
    int n = 3;

    while (n--) {
        pthread_mutex_lock(&data->mutex);
        if (0 < data->num) {
            // 还有票，等待
            pthread_cond_wait(&data->cond, &data->mutex);
        }
        // 放10张
        data->num += 10;
        printf("--------[release 10 tickets]----------\n");
        pthread_cond_broadcast(&data->hasticket);
        pthread_mutex_unlock(&data->mutex);
    }

    pthread_exit(NULL);
}


void init_data(Data_t *data, int num) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->cond, NULL);
    pthread_cond_init(&data->hasticket, NULL);
    data->num = num;
}

void destroy_data(Data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->cond);
    pthread_cond_destroy(&data->hasticket);
}

int main()
{
    int ret;
    pthread_t sell_window1;
    pthread_t sell_window2;
    pthread_t release_window;
    Data_t data;

    // 初始化data
    init_data(&data, 10);

    // 创建卖票线程
    ret = pthread_create(&sell_window1, NULL, sellTickets1, &data);
    PTHREAD_RET_CHECK(ret, "create pthread");

    ret = pthread_create(&sell_window2, NULL, sellTickets2, &data);
    PTHREAD_RET_CHECK(ret, "create pthread2");

    ret = pthread_create(&release_window, NULL, releaseTicket, &data);
    PTHREAD_RET_CHECK(ret, "create pthread2");

    // 回收线程
    ret = pthread_join(sell_window1, NULL);
    PTHREAD_RET_CHECK(ret, "join pthread");
    ret = pthread_join(sell_window2, NULL);
    PTHREAD_RET_CHECK(ret, "join pthread2");
    ret = pthread_join(release_window, NULL);
    PTHREAD_RET_CHECK(ret, "join pthread2");

    // 销毁data
    destroy_data(&data);

    return 0;
}
