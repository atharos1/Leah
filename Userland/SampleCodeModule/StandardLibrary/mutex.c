#include "../asm/asmLibC.h"

void mutex_create(char * name) {
  sys_mutexCreate(name);
}

void mutex_delete(char * name) {
  sys_mutexDelete(name);
}

int mutex_open(char * name) {
  return sys_mutexOpen(name);
}

void mutex_close(int mutex) {
  return sys_mutexClose(mutex);
}

void mutex_lock(int mutex) {
  sys_mutexLock(mutex);
}

void mutex_unlock(int mutex) {
  sys_mutexUnlock(mutex);
}
