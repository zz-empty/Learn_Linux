#include "head.h"

int main()
{
    FILE *fp = popen("./bin/print", "r");
    char buf[128];
    fread(buf, 1, sizeof(buf), fp);
    printf("%s", buf);
    pclose(fp);
    return 0;
}

