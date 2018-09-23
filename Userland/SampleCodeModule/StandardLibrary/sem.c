#include "../asm/asmLibC.h"

void sem_create(char * name, int value) {
  sys_semCreate(name, value);
}

void sem_delete(char * name) {
  sys_semDelete(name);
}

int sem_open(char * name) {
  return sys_semOpen(name);
}

void sem_close(int sem) {
  return sys_semClose(sem);
}

void sem_wait(int sem) {
  sys_semWait(sem);
}

void sem_signal(int sem) {
  sys_semSignal(sem);
}
