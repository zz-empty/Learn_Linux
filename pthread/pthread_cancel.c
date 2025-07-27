#include "head.h"

void *threadfunc(void *val) {
  printf("I am child_thread func! \n");
  printf("come main!\n");
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
