#include "pthread.h"
#include "../asm/asmLibC.h"

int pthread_create(int * thread, void *(*start_routine) (void *), void *arg) {
    sys_newThread(thread, start_routine, arg);
}