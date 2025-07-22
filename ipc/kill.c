#include "head.h"

int main(int argc, char **argv)
{
    // kill -9 pid
    ARGS_CHECK(argc, 3);
    int signum = atoi(argv[1] + 1);
    int pid = atoi(argv[2]);
    kill(pid, signum);
    return 0;
}

