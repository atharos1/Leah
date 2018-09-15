#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "memoryManager.h"
#include "stdlib.h"
#include "process.h"
#include "circularList.h"

#define MAX_QUEUE_COUNT 1

typedef thread_t * (*nextThreadFunction)();

typedef struct {
    NODE * queue;
    nextThreadFunction nextThread;
    int threadCount;
    void * queueData;
} SCHEDULER_QUEUE;

void scheduler_init();
void * schedule(void * oldRSP);
void scheduler_enqueue(thread_t * thread);
thread_t * getCurrentThread();
int getCurrentPID();
thread_t * scheduler_dequeue_current();
//void scheduler_dequeue(thread_t * thread);
void scheduler_dequeue_process(int pid);

#endif