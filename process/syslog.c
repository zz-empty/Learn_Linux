#include "head.h"

void Deamon() {
    if (fork()) {
        exit(0);
    }
    setsid();
    chdir("/");
    umask(0);
}

int main()
{
    Deamon();

    for (int i = 0; i < 10; ++i) {
        time_t now;
        time(&now);
        struct tm *pT = localtime(&now);
        syslog(LOG_INFO, "%d %04d:%02d:%02d", i, pT->tm_year + 1900, pT->tm_mon + 1, pT->tm_mday);
        sleep(2);
    }
    return 0;
}

