#include "process.h"
#include "scheduler.h"
#include "asm/libasm.h"
#include "drivers/console.h"

int getFreePID();
int getFreeTID(process_t * process);
void purgeThreadList(process_t * process);

process_t * processList[MAX_PROCESS_COUNT];
int process_count = 0;

int last_pid = 0;

void threadWrapper(int code()) {

    int (*thread_code)() = code;
    thread_code();

    thread_t * t = scheduler_dequeue_current();
    //REMOVE FROM PCB THREAD TABLE
    eraseTCB(t);
    _force_scheduler();

    //int ret = code();



    /*printf("\nEl programa finalizó con código de respuesta: %d\n", ret);
    while(1)
        _halt();*/
}

void erasePCB(process_t * process) {
    freeMemory(process->name);
    freeMemory(process->heap.base);
    freeMemory(process);
}

void eraseTCB(thread_t * thread) {
    freeMemory(thread->stack.base);
    freeMemory(thread);
}

void killThread(int pid, int tid) {
    //REMOVE FROM SCHEDULER
    //scheduler_dequeue_current();
    //REMOVE FROM IPC

    eraseTCB(processList[pid]->threadList[tid]);
    _force_scheduler();
}

void killProcess(int pid) {

    //REMOVE FROM PROCESS SLEEP

    for(int i = 0; i < MAX_THREAD_COUNT; i++)
        if(processList[pid]->threadList[i] == NULL)
            killThread(pid, i);

    erasePCB(processList[pid]);
}

int createProcess(char * name, void * code, int stack_size, int heap_size) {

    if( process_count >= MAX_PROCESS_COUNT ) //No entran más
        return 0;

    process_t * process = getMemory( sizeof(process_t) );
    if(process == NULL)
        return 0;

    process->name = getMemory( strlen(name) );
    if(process->name == NULL) {
        erasePCB( process );
        return 0;
    }
    strcpy(process->name, name);

    if(heap_size != 0) {
        process->heap.base = getMemory( heap_size * PAGE_SIZE );
        if(process->heap.base == NULL) {
            erasePCB( process );
            return 0;
        }
    } else {
        process->heap.base == NULL;
    }
    process->heap.size = heap_size * PAGE_SIZE;


    process->pid = getFreePID();
    processList[process->pid] = process;

    purgeFdList(process);

    purgeThreadList(process);
    process->threadCount = 0;

    createThread(process, code, stack_size);

    return process->pid;
}

thread_t * createThread(process_t * process, void * code, int stack_size) {

    if(process->threadCount == MAX_THREAD_COUNT)
        return NULL;

    thread_t * thread = getMemory( sizeof(process_t) );
    if(thread == NULL)
        return NULL;

    thread->stack.base = getMemory( stack_size * PAGE_SIZE );
    if(thread->stack.base == NULL) {
        freeMemory( thread );
        return NULL;
    }
    thread->stack.size = stack_size * PAGE_SIZE;
    thread->stack.base += stack_size * PAGE_SIZE;
    thread->stack.current = _initialize_stack_frame(&threadWrapper, code, thread->stack.base);

    thread->process = process->pid;

    thread->status = READY;

    process->threadList[getFreeTID(process)] = thread;
    process->threadCount++;

    scheduler_enqueue(thread);

    return thread;
}

int getFreePID() {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++)
        if(processList[i] == NULL)
            return i;
}

void purgeProcessList() {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(processList[i] != NULL) {
            //killProcess(processList[i]);
            processList[i] = NULL;
        }
    }
}

int getFreeTID(process_t * process) {
    for(int i = 0; i < MAX_THREAD_COUNT; i++)
        if(process->threadList[i] == NULL)
            return i;
}

int getFreeFD(process_t * process) {
    for(int i = 0; i < MAX_FD_COUNT; i++)
        if(process->fd_table[i] == NULL)
            return i;
}

void purgeThreadList(process_t * process) {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(process->threadList[i] != NULL) {
            //killThread(process->pid, i);
            process->threadList[i] = NULL;
        }
    }
}

void purgeFdList(process_t * process) {
    for(int i = 0; i < MAX_FD_COUNT; i++) {
        if(process->fd_table[i] != NULL) {
            //cerrar archivo?
            process->fd_table[i] = NULL;
        }
    }
}

/*char processData[200][30];

struct processData {
    int pid;
    char name[25];
    int threadCount;
};
*/
void listProcess() {
    printf("PID    |    NAME\n");
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(processList[i] != NULL) {
            printf("%d        %s        %X        %X\n", processList[i]->pid, processList[i]->name, processList[i]->threadList[0]->stack.current, processList[i]->threadList[0]->stack.base);
        }
    }
}

process_t * getProcessByPID(int pid) {
    return processList[pid];
}

int registerFD(int pid, opened_file_t * file) {
    processList[pid]->fd_table[getFreeFD(processList[pid])] = file;
}
