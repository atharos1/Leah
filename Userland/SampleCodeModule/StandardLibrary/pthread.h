#ifndef STD_PTHREAD
#define STD_PTHREAD

#include "../asm/asmLibC.h"

pthread_t pthread_create(void *(*start_routine) (void *), void *arg);
void pthread_join(pthread_t thread, void **retval);
void pthread_cancel(pthread_t thread);

#endif