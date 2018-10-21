#include "include/comparators.h"
#include "include/process.h"

int is_thread_from_process_cmp(void * t, void * pid) {
    return ( ((thread_t*)t)->process == *((int*)pid) );
}

int pointer_cmp(void * p1, void * p2) {
    return !(p1 == p2);
}