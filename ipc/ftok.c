#include "head.h"

int main(int argc, char *argv[]) {
  ARGS_CHECK(argc, 2);
  int key = ftok(argv[1], 0);
  RET_CHECK(key, -1, "ftok");

  printf("key = %d\n", key);

  return 0;
}
