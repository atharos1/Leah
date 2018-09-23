#include "sem.h"
#include "../asm/asmLibC.h"

void sem_create(char * path, int value) {
  sys_semCreate(path, value);
}

int sem_open(char * path) {
  return sys_openFile(path, O_RDWR);
}

void sem_close(int sem) {
  return sys_closeFile(sem);
}

void sem_wait(int sem) {
  sys_semWait(sem);
}

void sem_signal(int sem) {
  sys_semSignal(sem);
}
