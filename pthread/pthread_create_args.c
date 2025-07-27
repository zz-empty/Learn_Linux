#include "head.h"

void *threadfunc(void *val) {
  char *buf = (char *)val;
  printf("I am child_thread func! buf = %s \n", buf);
  strcpy(buf, "you are big one!");
  pthread_exit(NULL);
}

int main() {
  pthread_t thid;
  int ret;
  long num = 2;
  char buf[128] = "hello thread!";

  ret = pthread_create(&thid, NULL, threadfunc, buf);
  PTHREAD_RET_CHECK(ret, "pthread_create");

  printf("I am main_thread func!\n");

  pthread_join(thid, NULL);
  printf("buf = %s\n", buf);
  return 0;
}
