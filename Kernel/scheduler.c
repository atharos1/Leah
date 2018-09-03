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

    if( READY_LIST->count == 0 || READY_LIST->first->process->stack.current == oldRSP)
        return oldRSP;

    plist_node * oldFirst = READY_LIST->first;
    if(runningTasks == 1)
        oldFirst->process->stack.current = oldRSP;

    runningTasks = 1;

    READY_LIST->first = oldFirst->next;
    READY_LIST->last = oldFirst;

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







/*

pcb * scheduler_getCurrentProcess() {
    if(READY_LIST->current == NULL)
        return NULL;

    return READY_LIST->current->process;
}

pcb * scheduler_getNextProcess() {
    if(READY_LIST->current == NULL) {
        if( READY_LIST->first == NULL )
            return NULL;
        else
            return READY_LIST->first->process;
    }

    return READY_LIST->current->next->process;
}

void * scheduler_nextProcess(void * oldRSP) {
    READY_LIST->current = READY_LIST->current->next;
    return READY_LIST->current->process->stack.current;
}

void * schedule(void * oldRSP) {

    //printf("%d", READY_LIST->count);

    if(currQuantum % QUANTUM != 0) {
        currQuantum++;
        return oldRSP;
    }
    currQuantum = 0;

    //return sp;

    pcb * currentProcess = scheduler_getCurrentProcess();
    pcb * nextProcess = scheduler_getNextProcess();

    if(nextProcess == NULL) {
        return oldRSP;
    }

    if( currentProcess != NULL ) {
        currentProcess->stack.current = oldRSP;
    } else {
        READY_LIST->current = READY_LIST->first;
    }

    return nextProcess->stack.current;

    //currentProcess->stack.current = oldRSP;
    //return scheduler_nextProcess(oldRSP);

}

void scheduler_set_process_ready(plist_node * pnode) {
    READY_LIST->last = pnode;

    if(READY_LIST->first == NULL) {
        READY_LIST->first = READY_LIST->last;
    }
        
    pnode->next = READY_LIST->first;
}

int scheduler_add_process(char * name, void * code, int stack_size, int heap_size) {
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

    //printf("Stack: %X, Código: %X\n", process->stack.base, code);

    sp = process->stack.current = _initialize_stack_frame(process->stack.base, code);
    process->pid = ++last_pdi;
    /*READY_LIST->count++;
    scheduler_set_process_ready(pnode);


    return TRUE;
}*/