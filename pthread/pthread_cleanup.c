#include "head.h"

void cleanfunc(void *p) {
    printf("cleanfunc\n");
    if (p) {
        free(p);
        p = NULL;
    }
}

void *threadfunc(void *val) {
  printf("I am child_thread func! \n");
  char *p = (char *)malloc(128);
  pthread_cleanup_push(cleanfunc, p);
  strcpy(p, "this is child!");
  printf("p = %s\n", p);
  free(p);
  printf("success free!\n");
  pthread_cleanup_pop(1);
  pthread_exit(NULL);
}

int main() {
  pthread_t thid;
  int ret;

  ret = pthread_create(&thid, NULL, threadfunc, NULL);
  PTHREAD_RET_CHECK(ret, "pthread_create");
  printf("I am main_thread func!\n");

  pthread_cancel(thid);
  pthread_join(thid, NULL);
  return 0;
}
