#include "pthread.h"

pthread_t pthread_create(void *(*start_routine) (void *), void *arg) {
    return sys_newThread(start_routine, arg);
}

void pthread_join(pthread_t thread, void **retval) {
    sys_joinThread(thread, retval);
}

void pthread_cancel(pthread_t thread) {
    sys_cancelThread(thread);
}