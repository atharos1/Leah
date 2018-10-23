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
    NODE *queue;
    int queueQuantum;
    int currQueueQuantum;
    thread_t *(*nextThreadFunction)(struct SCHEDULER_QUEUE *);
    int (*checkEvictFunction)(struct SCHEDULER_QUEUE *);
    void (*restartEvictFunction)(struct SCHEDULER_QUEUE *);
    void (*addToQueue)(struct SCHEDULER_QUEUE *, thread_t *);
    int (*removeThread)(struct SCHEDULER_QUEUE *, thread_t *);
    void (*ageThreads)(struct SCHEDULER_QUEUE *);
    void *queueData;
    int nextQueue;
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

#endif
