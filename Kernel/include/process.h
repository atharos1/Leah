#ifndef PROCESS_MOD_H
#define PROCESS_MOD_H

#define MAX_THREAD_COUNT 25
#define MAX_PROCESS_COUNT 100
#define MAX_FD_COUNT 25

#include "fileSystem.h"
#include "memoryManager.h"
#include "sem.h"
#include "stdlib.h"

// Estados de THREADS
#define READY 0
#define SLEEPING 1
#define DEAD 20

// Estados de PROCESOS
#define ALIVE 0
#define ZOMBIE 1

typedef struct memblock_t {
    void *base;
    int size;
    void *current;
} memblock_t;

typedef struct thread_t {
    int tid;
    int process;
    int status;
    void *retValue;
    void *finishedSem;
    int is_someone_joining;
    memblock_t stack;
    int queueID;
    void *queue;
    int usedProcessor;
    int currTimeSlice;
} thread_t;

typedef struct process_t {
    int pid;
    char *name;
    int parent;
    memblock_t heap;
    // int threadCount;
    thread_t *threadList[MAX_THREAD_COUNT];
    fd_t *fd_table[MAX_FD_COUNT];
    file_t *cwd;
    int retValue;
    sem_t finishedSem;
    int status;
    int nice;
} process_t;

typedef struct {
    int pid;
    char *name;
    char *parentName;
    int threadCount;
    int heapSize;
    int status;
    int foreground;
    int niceness;
} ps_struct;
typedef ps_struct *ps_info;

int createProcess(char *name, void *code, char **args, int stack_size,
                  int heap_size, int fdReplace[][2]);
thread_t *createThread(process_t *process, void *code, void *args,
                       int stack_size, int isMain);
process_t *getProcessByPID(int pid);
void purgeProcessList(int close);
void listProcess(ps_struct buffer[], int *bufferCount);
int getFreeFD(int pid);
int registerFD(int pid, fd_t *file);
fd_t *unregisterFD(int pid, int fdIndex);
fd_t *getFD(int pid, int fd);
int processCount();
int aliveProcessCount();
int waitpid(int pid);
void threadJoin(int tid, void **retVal);
void killProcess(int pid, int retValue);
void killThread(int pid, int tid, int called_from_kill_process);
void eraseTCB(thread_t *thread);
void *getHeapBase(int pid);
int getHeapSize(int pid);
int isValidProcess(int pid);
int setNiceness(int pid, int nice);
void dup2(int fdFrom, int fdTo);

#endif
