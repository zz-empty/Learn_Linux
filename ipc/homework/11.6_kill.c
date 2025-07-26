#include "head.h"

int main(int argc, char **argv)
{
    // kill -9 pid
    ARGS_CHECK(argc, 3);
    kill(atoi(argv[2]), atoi(argv[1] + 1));
    return 0;
}

