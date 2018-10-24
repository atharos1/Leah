#include "include/pthread.h"

pthread_t pthread_create(void *(*start_routine) (void *), void *arg) {
    return sys_newThread(start_routine, arg);
}

void pthread_join(pthread_t thread, void **retval) {
    sys_joinThread(thread, retval);
}

void pthread_cancel(pthread_t thread) {
    sys_cancelThread(thread);
}

int execv(char * name, int (*start_routine) (), char ** args, int foreground, int fdReplace[][2]) {
    return sys_newProcess(name, start_routine, args, fdReplace);
}

int dup2(int oldFd, int newFd) {
    return sys_dup2(oldFd, newFd);
}