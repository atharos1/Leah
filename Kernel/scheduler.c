#include "scheduler.h"
#include "drivers/console.h"
#include "asm/libasm.h"
#include "lib.h"

#include "roundRobin.h"


NODE * CURRENT_QUEUE = NULL;

//SCHEDULER_QUEUE Queues[MAX_QUEUE_COUNT];
NODE * mainQueue;

static const int QUANTUM = 50;
int currQuantum = 0;
int runningTasks;

struct currentThread {
    thread_t * thread;
    SCHEDULER_QUEUE * queue;
} currentThread;

int threadCount = 0;

void scheduler_init() {

    currentThread.thread = NULL;
    currentThread.queue = NULL;

    SCHEDULER_QUEUE * q1 = roundRobin_newQueue(3);
    mainQueue = insertAtHead(mainQueue, q1);

    currQuantum = 0;
    runningTasks = 0;
    threadCount = 0;

    purgeProcessList(); //Esto va acá?
    
}

int pointer_cmp(void * p1, void * p2) {
    return p1 == p2;
}

thread_t * scheduler_dequeue_current() {
    int status;
    (currentThread.queue)->queue = deleteByValue((currentThread.queue)->queue, currentThread.thread, pointer_cmp, &status);
    return currentThread.thread;
}

/*void scheduler_dequeue_process(int pid) {
    //printf("hola");
    return;
}*/

void scheduler_enqueue(thread_t * thread) {

    SCHEDULER_QUEUE * q = getFirst(mainQueue);

    q->queue = insertAtEnd(q->queue, thread);
    q->threadCount++;
    threadCount++;

}

int boludeo = 0;

void * scheduler_nextTask(void * oldRSP) {

    if( threadCount == 0 || (threadCount == 1 && runningTasks == 1 ) )
        return oldRSP;

    NODE * auxQueue = mainQueue;

    if(runningTasks == 1) {
        getCurrentThread()->stack.current = oldRSP;
    }
    runningTasks = 1;

    boludeo++;

    /*if(boludeo > 3)
        scheduler_dequeue_current();*/

    thread_t * nextThread = NULL;    
    SCHEDULER_QUEUE * currQueue;
    

    while(1) {
        currQueue = getFirst(auxQueue);
        nextThread = currQueue->nextThread(auxQueue->data);

        if(nextThread != NULL) {
            currentThread.thread = nextThread;
            currentThread.queue = currQueue;
            return nextThread->stack.current;
        }
        auxQueue = next(auxQueue);
    }

    return NULL;
}


void * schedule(void * oldRSP) {
    currQuantum++;
    
    if(currQuantum % (QUANTUM+1) != 0) {  
        return oldRSP;
    }

    return scheduler_nextTask(oldRSP);

}

thread_t * getCurrentThread() {
    return currentThread.thread;
}

int getCurrentPID() {
    return currentThread.thread->process;
}

void scheduler_dequeue(thread_t * thread) {
    NODE * auxQueue = mainQueue;
    SCHEDULER_QUEUE * q;
    int deletionStatus = 2;
    do {
        q = getFirst(auxQueue);
        q->queue = deleteByValue(q->queue, thread, pointer_cmp, &deletionStatus);
        if(deletionStatus == 1) {
            return;
        }

        auxQueue = next(auxQueue);
    } while( auxQueue != mainQueue );

    return;
}