#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "memoryManager.h"
#include "stdlib.h"
#include "process.h"

typedef struct plist_node {
    thread_t * thread;
    struct plist_node * next;
} plist_node;

/*typedef struct plist {
    plist_node * first;
    plist_node * last;
    plist_node * current;
    int count;
} plist;*/

typedef struct plist {
    plist_node * first;
    plist_node * last;
    int count;
} plist;

void scheduler_init();
void * schedule(void * oldRSP);
//int scheduler_newProcess(char * name, void * code, int stack_size, int heap_size);
void scheduler_enqueue(thread_t * thread);


#endif