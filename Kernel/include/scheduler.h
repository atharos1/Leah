#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "circularList.h"
#include "memoryManager.h"
#include "process.h"
#include "stdlib.h"

#define MAX_QUEUE_COUNT 3

// typedef thread_t * (*nextThreadFunction)(SCHEDULER_QUEUE *, void *);

extern int FORCE;

typedef struct SCHEDULER_QUEUE {
    int queueQuantum;

    thread_t *(*queueSchedule)(struct SCHEDULER_QUEUE *, int force);
    int (*removeThread)(struct SCHEDULER_QUEUE *, thread_t *);
    void (*enqueueThread)(struct SCHEDULER_QUEUE *, thread_t *);

    void *queueData;
    int nextQueue;
    int threadCount;
} SCHEDULER_QUEUE;

void scheduler_init();
void *scheduler_shortTerm(void *oldRSP);
void scheduler_midTerm();
void scheduler_enqueue(thread_t *thread, int queue);
thread_t *getCurrentThread();
int getCurrentPID();
thread_t *scheduler_dequeue_current();
int scheduler_dequeue_thread(thread_t *t);
void scheduler_dequeue_process(int pid);
thread_t *getCurrentThread();
void resetCurrentThread();

#endif
