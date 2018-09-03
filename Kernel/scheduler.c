#include "scheduler.h"
#include "drivers/console.h"
#include "asm/libasm.h"

plist * READY_LIST;
static const int QUANTUM = 5;
int currQuantum;
int last_pdi;
int runningTasks;

void scheduler_init() {

    READY_LIST = getMemory( sizeof(READY_LIST) );
    if(READY_LIST == NULL) {
        //Se pudre todo? Hace falta verificar?
    }

    READY_LIST->first = READY_LIST->last = NULL;
    READY_LIST->count = 0;
    currQuantum = 0;
    last_pdi = 0;
    runningTasks = 0;
}




int strlen(char * str) {
    int i = 0;
    while(str[i] != 0)
        i++;

    return i;
}

void strcpy(char * dest, char * origin) {
    int i;
    for(i = 0; origin[i] != 0; i++)
        dest[i] = origin[i];

    dest[i+1] = 0;
}


/*plist_node * scheduler_dequeue() {
    plist_node * ret = READY_LIST->first;
    READY_LIST->count--;

    if(READY_LIST->count == 0) {
        READY_LIST->first = READY_LIST->last = NULL;
        runningTasks = 0;
    } else {
        READY_LIST->first


    }

}*/

void scheduler_enqueue(plist_node * n) {
    if(READY_LIST->count == 0) {
        READY_LIST->first = READY_LIST->last = n;
    }

    READY_LIST->last->next = n;
    READY_LIST->last = n;
    n->next = READY_LIST->first;

    READY_LIST->count++;
}

void * scheduler_nextProcess(void * oldRSP) {
    if( READY_LIST->count == 0 || (READY_LIST->count == 1 && runningTasks == 1 ) )
        return oldRSP;

    plist_node * oldFirst = READY_LIST->first;
    if(runningTasks == 1)
        oldFirst->process->stack.current = oldRSP;

    runningTasks = 1;

    READY_LIST->first = oldFirst->next;
    READY_LIST->last = oldFirst;

    //printf("\nSale: %s | Entra: %s\n", oldFirst->process->name, READY_LIST->first->process->name);

    return READY_LIST->first->process->stack.current;
}


void * schedule(void * oldRSP) {

    if(currQuantum % QUANTUM != 0) {
        currQuantum++;
        return oldRSP;
    }
    currQuantum = 0;

    return scheduler_nextProcess(oldRSP);

}

int scheduler_newProcess(char * name, void * code, int stack_size, int heap_size) {
    pcb * process = getMemory( sizeof(pcb) );
    if(process == NULL)
        return NULL;

    process->name = getMemory( strlen(name) );
    if(process->name == NULL) {
        freeMemory( process );
        return NULL;
    }
    strcpy(process->name, name);

    process->stack.base = getMemory( stack_size * PAGE_SIZE );
    if(process->stack.base == NULL) {
        freeMemory( process->name );
        freeMemory( process );
        return NULL;
    }
    process->stack.size = stack_size * PAGE_SIZE;

    plist_node * pnode = getMemory( sizeof(plist_node) );
    if(pnode == NULL) {
        freeMemory( process->stack.base );
        freeMemory( process->name );
        freeMemory( process );
        return NULL;
    }
    pnode->process = process;

    process->stack.current = _initialize_stack_frame(code, process->stack.base);
    process->pid = ++last_pdi;

    scheduler_enqueue(pnode);

    return TRUE;
}