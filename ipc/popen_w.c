#include "head.h"

int main()
{
    FILE *fp = popen("./bin/sub", "w");
    fwrite("30000 88888", 1, 11, fp);
    pclose(fp);
    return 0;
}

