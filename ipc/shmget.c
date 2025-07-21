#include "head.h"

int main() {
  int shmid = shmget(1000, 128, IPC_CREAT | 0600);

  printf("shmid = %d\n", shmid);

  return 0;
}
