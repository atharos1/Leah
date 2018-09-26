#ifndef PROCESS_MOD_H
#define PROCESS_MOD_H

#define MAX_THREAD_COUNT 25
#define MAX_PROCESS_COUNT 100
#define MAX_FD_COUNT 25

#include "memoryManager.h"
#include "stdlib.h"
#include "fileSystem.h"
#include "sem.h"

//Estados de THREADS
#define READY 0
#define SLEEPING 1
#define DEAD 20

//Estados de PROCESOS
/*#define READY 0
#define SLEEPING 1
#define ZOMBIE 1*/

typedef struct memblock_t {
    void * base;
    int size;
    void * current;
} memblock_t;

typedef struct thread_t {
    int tid;
    int process;
    int status;
    void * retValue;
    void * finishedSem;
    int is_someone_joining;
    memblock_t stack;
} thread_t;

typedef struct process_t {
    int pid;
    char * name;
    memblock_t heap;
    //int threadCount;
    thread_t * threadList[MAX_THREAD_COUNT];
    fd_t * fd_table[MAX_FD_COUNT];
    file_t * cwd;
    int retValue;
    sem_t finishedSem;
} process_t;

int createProcess(char * name, void * code, int stack_size, int heap_size);
thread_t * createThread(process_t * process, void * code, void * args, int stack_size, int isMain);
process_t * getProcessByPID(int pid);
void purgeProcessList(int close);
void listProcess();
int getFreeFD(int pid);
int registerFD(int pid, fd_t * file);
fd_t * unregisterFD(int pid, int fdIndex);
fd_t * getFD(int pid, int fd);
int processCount();
int aliveProcessCount();
int waitpid(int pid);
void threadJoin(int tid, void **retVal);
void killProcess(int pid, int retValue);
void killThread(int pid, int tid, int called_from_kill_process);
void eraseTCB(thread_t * thread);
void * getHeapBase(int pid);
int getHeapSize(int pid);

typedef struct
{
    int pid;
    char * name;
    int threadCount;
    void * heapBase;
    int heapSize;
} ps_struct;
typedef ps_struct * ps_info;

#endif
