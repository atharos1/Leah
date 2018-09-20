#ifndef STD_PTHREAD
#define STD_PTHREAD

int pthread_create(int *thread, void *(*start_routine) (void *), void *arg);

#endif