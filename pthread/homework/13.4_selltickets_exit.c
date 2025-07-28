#include "head.h"

typedef struct {
    int num;    // 当前票数
    int window;   // 当前窗口
    int totaltickets;   // 总票数
    int exitflag; // 是否退出线程
    pthread_mutex_t mutex;  // 互斥锁
    pthread_cond_t cond;    // 通知放票, 等待放票
    pthread_cond_t hasticket;   // 通知取票, 等待取票
} Data_t;

void *sellTickets(void *val) {
    Data_t *data = (Data_t*)val;
    int window = data->window;

    while (1) {
        pthread_mutex_lock(&data->mutex);
        if (0 == data->num) {
            // 通知放票
            pthread_cond_signal(&data->cond);
            pthread_cond_wait(&data->hasticket, &data->mutex);
        } else if (0 < data->num) {
            // 卖一张
            data->num -= 1;
            printf("[sell window %d:%ld] 卖出1张票\n", window, pthread_self());
        }
        pthread_mutex_unlock(&data->mutex);
        usleep(100);

        if (0 == data->num && data->exitflag) {
            break;
        }
    }

    pthread_exit(NULL);
}

void *releaseTicket(void *val) {
    Data_t *data = (Data_t*)val;

    while (data->totaltickets > 0) {
        pthread_mutex_lock(&data->mutex);
        if (0 < data->num) {
            // 还有票，等待
            pthread_cond_wait(&data->cond, &data->mutex);
        }
        // 放10张
        data->num += 10;
        data->totaltickets -= 10;
        printf("--------[release 10 tickets]----------\n");
        pthread_cond_broadcast(&data->hasticket);
        pthread_mutex_unlock(&data->mutex);
    }

    // 退出所有线程
    pthread_mutex_lock(&data->mutex);
    data->exitflag = 1;
    pthread_mutex_unlock(&data->mutex);

    pthread_exit(NULL);
}


void init_data(Data_t *data, int num) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->cond, NULL);
    pthread_cond_init(&data->hasticket, NULL);
    data->num = num;
    data->exitflag = 0;
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
    data.window = 1;
    ret = pthread_create(&sell_window1, NULL, sellTickets, &data);
    PTHREAD_RET_CHECK(ret, "create pthread");

    usleep(1);
    data.window = 2;
    ret = pthread_create(&sell_window2, NULL, sellTickets, &data);
    PTHREAD_RET_CHECK(ret, "create pthread2");

    // 创建放票线程
    data.totaltickets = 50; // 总票数
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
