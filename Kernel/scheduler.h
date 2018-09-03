#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "memoryManager.h"

typedef struct process_memory_block {
    void * base;
    int size;
    void * current;
} process_memory_block;

typedef struct pcb {
    int pid;
    char * name;
    process_memory_block stack;
} pcb;

typedef struct plist_node {
    pcb * process;
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
int scheduler_newProcess(char * name, void * code, int stack_size, int heap_size);

#endif