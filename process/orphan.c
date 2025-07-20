#include "head.h"

int main()
{
    if (fork()) {
        return 0;
    } else {
        while(1);
    }
    return 0;
}
