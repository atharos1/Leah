#ifndef PROCESS_MOD_H
#define PROCESS_MOD_H

#define MAX_THREAD_COUNT 25
#define MAX_PROCESS_COUNT 100
#define MAX_FD_COUNT 25

#include "memoryManager.h"
#include "stdlib.h"
#include "fileSystem.h"

//Estados de threads
#define READY 0
#define SLEEPING 1

typedef struct memblock_t {
    void * base;
    int size;
    void * current;
} memblock_t;

typedef struct thread_t {
    int process;
    int status;
    memblock_t stack;
} thread_t;

typedef struct process_t {
    int pid;
    char * name;
    memblock_t heap;
    int threadCount;
    thread_t * threadList[MAX_THREAD_COUNT];
    opened_file_t * fd_table[MAX_FD_COUNT];
    file_t * cwd;
} process_t;

int createProcess(char * name, void * code, int stack_size, int heap_size);
thread_t * createThread(process_t * process, void * code, int stack_size);
process_t * getProcessByPID(int pid);
void purgeProcessList();
void listProcess();
int registerFD(int pid, opened_file_t * file);

#endif