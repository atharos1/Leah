#include "scheduler.h"
#include "drivers/console.h"
#include "asm/libasm.h"
#include "lib.h"

plist * READY_LIST;
static const int QUANTUM = 5;
int currQuantum = 0;
int runningTasks;

void scheduler_init() {

    READY_LIST = getMemory( sizeof(READY_LIST) );
    if(READY_LIST == NULL) {
        //Se pudre todo? Hace falta verificar?
    }

    READY_LIST->first = READY_LIST->last = NULL;
    READY_LIST->count = 0;
    currQuantum = 0;
    //lastPID = 0;
    runningTasks = 0;
}


/* plist_node * scheduler_dequeue() {
    plist_node * ret = READY_LIST->first;
    READY_LIST->count--;

    if(READY_LIST->count == 0) {
        READY_LIST->first = READY_LIST->last = NULL;
        runningTasks = 0;
    } else {
        READY_LIST->first = READY_LIST->first->next;
    }

}*/

void scheduler_enqueue(thread_t * thread) {
    plist_node * n = getMemory( sizeof(plist_node) );
    if(n == NULL) {
        //return NULL;
    }
        
    n->thread = thread;
    
    if(READY_LIST->count == 0) {
        READY_LIST->first = READY_LIST->last = n;
    }

    READY_LIST->last->next = n;
    READY_LIST->last = n;
    n->next = READY_LIST->first;

    READY_LIST->count++;
}

void * scheduler_nextTask(void * oldRSP) {
    if( READY_LIST->count == 0 || (READY_LIST->count == 1 && runningTasks == 1 ) )
        return oldRSP;

    plist_node * oldFirst = READY_LIST->first;
    if(runningTasks == 1)
        oldFirst->thread->stack.current = oldRSP;

    runningTasks = 1;

    READY_LIST->first = oldFirst->next;
    READY_LIST->last = oldFirst;

    //printf("Sale: %X | Entra: %X\n", oldRSP, READY_LIST->first->thread->stack.current);

    //printf("\nSale: %s | Entra: %s\n", oldFirst->process->name, READY_LIST->first->process->name);

    return READY_LIST->first->thread->stack.current;
}


void * schedule(void * oldRSP) {

    if(currQuantum % QUANTUM != 0) {
        currQuantum++;
        return oldRSP;
    }
    currQuantum = 0;

    return scheduler_nextTask(oldRSP);

}