#include "head.h"

#if 0
void show_progress(off_t downloaded, off_t total, int width) {
    int filled = (double)downloaded / total * width;
    
    printf("\r[");
    for (int i = 0; i < filled; ++i) printf("#");
    for (int i = filled; i < width; ++i) printf(" ");
    printf("] %6.2f%%", (double)downloaded / total * 100.0);
    fflush(stdout);
}
#else
#define MAX_BAR_WIDTH  100

static const char bar[MAX_BAR_WIDTH+1] =
    "####################################################################################################";

void show_progress(off_t downloaded, off_t total, int width) {
    int filled = (double)downloaded / total * width;
    
    printf("\r[%.*s%*s] %6.2f%%", 
           filled, bar,
           width - filled, " ",
           (double)downloaded / total * 100.0);
    fflush(stdout);
}
#endif
