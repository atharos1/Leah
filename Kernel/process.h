#ifndef PROCESS_MOD_H
#define PROCESS_MOD_H

#define MAX_THREAD_COUNT 25

#include "memoryManager.h"
#include "stdlib.h"

//Estados de threads
#define READY 0
#define SLEEPING 1

typedef struct memblock_t {
    void * base;
    int size;
    void * current;
} memblock_t;

typedef struct process_t {
    int pid;
    char * name;
    memblock_t heap;
    int threadNum;
    //thread_t threadList[MAX_THREAD_COUNT];
} process_t;

typedef struct thread_t {
    process_t * process;
    int status;
    memblock_t stack;
} thread_t;

//process_t * processList[250];

process_t * createProcess(char * name, void * code, int stack_size, int heap_size);
thread_t * createThread(process_t * process, void * code, int stack_size);
#endif