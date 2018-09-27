#ifndef STD_PTHREAD
#define STD_PTHREAD

#include "../../asm/asmLibC.h"

typedef int (*function_t) ();

pthread_t pthread_create(void *(*start_routine) (void *), void *arg);
void pthread_join(pthread_t thread, void **retval);
void pthread_cancel(pthread_t thread);
//int execv(char * name, int (*start_routine) (char ** args), int foreground, char * fdReplace);
int execv(char * name, int (*start_routine) (), char ** args, int foreground, char * fdReplace);

#endif
