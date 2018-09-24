#include "process.h"
#include "scheduler.h"
#include "asm/libasm.h"
#include "drivers/console.h"
#include "sem_bin.h"

int getFreePID();
int getFreeTID(process_t * process);
void purgeThreadList(process_t * process);
void purgeFdList(process_t * process);

process_t * processList[MAX_PROCESS_COUNT];
int process_count = 0;

int last_pid = 0;

int processCount() {
    return process_count;
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

void threadWrapper(void * code(), void * args) {

    void * (*thread_code)() = code;
    void * retValue = thread_code();

    killThread(getCurrentPID(), getCurrentThread()->tid);

    _force_scheduler();

}

void processWrapper(int code()) {

    int (*main_thread_code)() = code;
    int retValue = main_thread_code();

    killProcess(getCurrentPID(), retValue);

}

int killThread(int pid, int tid) {

    thread_t * t = getProcessByPID(pid)->threadList[tid];
    //printf("\npid: %d, tid: %d", pid, tid);
    int isCurrThread = (t == getCurrentThread());
    int thread_in_scheduler = scheduler_dequeue_thread(t);

    if(!thread_in_scheduler) {
        t->status = DEAD;
    }
        
    //TODO: REMOVE FROM IPC

    if(t->is_someone_joining == FALSE) { //Solo borro si nadie me espera
        if(thread_in_scheduler) //Ya voló
            eraseTCB( processList[pid]->threadList[tid] );
        processList[pid]->threadList[tid] = NULL;
    } else {
        sem_bin_signal( t->finishedSem ); //Despertamos al thread que hizo join

    }

    return isCurrThread;
}

void killProcess(int pid, int retValue) {

    int deletedCurrentThread = (getCurrentPID() == pid);

    for(int i = 0; i < MAX_THREAD_COUNT; i++)
        if(processList[pid]->threadList[i] != NULL)
            killThread(pid, i);

    processList[pid]->retValue = retValue;

    sem_bin_signal( processList[pid]->awaitSem ); //Despertamos a potencial waitpid

    if(deletedCurrentThread)
        _force_scheduler();

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
        process->heap.base = NULL;
    }
    process->heap.size = heap_size * PAGE_SIZE;

    //process->finishedSem = sem_bin_create(0);

    process->awaitSem = sem_bin_create(0);

    process->pid = getFreePID();
    processList[process->pid] = process;

    purgeFdList(process);

    purgeThreadList(process);
    process->threadCount = 0;

    process_count++;

    createThread(process, code, NULL, stack_size, TRUE);

    return process->pid;
}

thread_t * createThread(process_t * process, void * code, void * args, int stack_size, int isMain) {

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

    if(isMain == TRUE)
        thread->stack.current = _initialize_stack_frame(&processWrapper, code, thread->stack.base);
    else
        thread->stack.current = _initialize_stack_frame(&threadWrapper, code, thread->stack.base);

    thread->process = process->pid;
    thread->retValue = NULL;
    thread->finishedSem = sem_bin_create(0);
    thread->is_someone_joining = FALSE;

    thread->status = READY;

    thread->tid = getFreeTID(process);
    process->threadList[thread->tid] = thread;
    process->threadCount++;

    scheduler_enqueue(thread);

    return thread;
}

int getFreePID() {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++)
        if(processList[i] == NULL)
            return i;

    return -1;
}

void purgeProcessList() {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(processList[i] != NULL) {
            killProcess(i, -1);
            processList[i] = NULL;
        }
    }
}

int getFreeTID(process_t * process) {
    for(int i = 0; i < MAX_THREAD_COUNT; i++)
        if(process->threadList[i] == NULL)
            return i;

    return -1;
}

int getFreeFD(int pid) {
    for(int i = 0; i < MAX_FD_COUNT; i++)
        if(processList[pid]->fd_table[i] == NULL)
            return i;
    return -1;
}

void purgeThreadList(process_t * process) {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(process->threadList[i] != NULL) {
            killThread(process->pid, i);
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

int registerFD(int pid, fd_t * file) {
    int fdIndex = getFreeFD(pid);
    if (fdIndex != -1) {
      processList[pid]->fd_table[fdIndex] = file;
    }
    return fdIndex;
}

fd_t * unregisterFD(int pid, int fdIndex) {
  if (fdIndex < 0 || fdIndex >= MAX_FD_COUNT)
    return NULL;
  fd_t * ret = processList[pid]->fd_table[fdIndex];
  processList[pid]->fd_table[fdIndex] = NULL;
  return ret;
}

fd_t * getFD(int pid, int fdIndex) {
  if (fdIndex < 0 || fdIndex >= MAX_FD_COUNT)
    return NULL;
  return processList[pid]->fd_table[fdIndex];
}

void threadJoin(int tid, void **retVal) {

    thread_t * awaitThread = processList[getCurrentPID()]->threadList[tid];

    awaitThread->is_someone_joining = TRUE;

    sem_bin_wait( awaitThread->finishedSem );

    *retVal = awaitThread->retValue;

    processList[getCurrentPID()]->threadList[tid] = NULL;

    if(awaitThread->status != DEAD)
        eraseTCB( awaitThread ); //Borramos zombie
}

int waitpid(int pid) {
    //TODO: VERIFICAR QUE SEA PADRE
    sem_bin_wait( processList[pid]->awaitSem );

    int retValue = processList[pid]->retValue;

    process_count--;
    erasePCB(processList[pid]); //Borramos zombie
    processList[pid] = NULL;

    return retValue;
}
