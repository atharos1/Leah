#include "scheduler.h"
#include "drivers/console.h"
#include "asm/libasm.h"
#include "lib.h"

#include "roundRobin.h"


NODE * CURRENT_QUEUE = NULL;

//SCHEDULER_QUEUE Queues[MAX_QUEUE_COUNT];
NODE * mainQueue;

static const int QUANTUM = 3;
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

/*void scheduler_dequeue_current() {
    return;
}

void scheduler_dequeue_process(int pid) {
    //printf("hola");
    return;
}*/

void scheduler_enqueue(thread_t * thread) {

    SCHEDULER_QUEUE * q = getFirst(mainQueue);

    q->queue = insertAtEnd(q->queue, thread);
    q->threadCount++;
    threadCount++;

}

void * scheduler_nextTask(void * oldRSP) {

    if( threadCount == 0 || (threadCount == 1 && runningTasks == 1 ) )
        return oldRSP;

    NODE * auxQueue = mainQueue;

    if(runningTasks == 1) {
        getCurrentThread()->stack.current = oldRSP;
    }
    runningTasks = 1;

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

    if(currQuantum % QUANTUM != 0) {
        currQuantum++;
        return oldRSP;
    }
    currQuantum = 0;

    return scheduler_nextTask(oldRSP);

}

thread_t * getCurrentThread() {
    return currentThread.thread;
    printf("hola");
    printf("hola");
    printf("hola");
    printf("hola");
    printf("hola");
    printf("hola");
    printf("hola");
    printf("hola");
    printf("hola");
}

int getCurrentPID() {
    return currentThread.thread->process;
}

/*int scheduler_dequeue(thread_t * thread) {
    NODE * auxQueue = mainQueue;
    SCHEDULER_QUEUE * q, *aux;
    int deletionStatus;
    do {
        q = aux = getFirst(auxQueue);
        
        //auxQueue->data

        auxQueue = next(auxQueue);
    } while( auxQueue != mainQueue );

    return 0;
}*/