#include "process.h"
#include "scheduler.h"
#include "asm/libasm.h"
#include "drivers/console.h"

int getFreePID();
int getFreeTID(process_t * process);
void purgeThreadList(process_t * process, int close, int called_from_kill_process);
void purgeFdList(process_t * process, int close);
void erasePCB(process_t * process);
int getFreeFD(int pid);

process_t * processList[MAX_PROCESS_COUNT];
int process_count = 0;
int alive_process_count = 0;

// ** WRAPPERS DE CÓDIGO ** // 
void processWrapper(int code()) {
    int (*main_thread_code)() = code;
    int retValue = main_thread_code();

    killProcess(getCurrentPID(), retValue);
}

void threadWrapper(void * code(), void * args) {
    void * (*thread_code)() = code;
    void * retValue = thread_code();

    killThread(getCurrentPID(), getCurrentThread()->tid, FALSE);
}
// ** WRAPPERS DE CÓDIGO ** // 



// ** MANEJO DE PROCESOS ** // 
void purgeProcessList(int close) {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(processList[i] != NULL) {
            if(close == TRUE)
                killProcess(i, -1);
            processList[i] = NULL;
        }
    }
}

int processCount() {
    return process_count;
}

int aliveProcessCount() {
    return alive_process_count;
}

void erasePCB(process_t * process) {
    freeMemory(process->name);
    freeMemory(process->heap.base);
    freeMemory(process);
}

int createProcess(char * name, void * code, int stack_size, int heap_size) {

    int pid = getFreePID();
    if(pid == -1) //No entran mas
        return -1;

    process_t * process = getMemory( sizeof(process_t) );
    if(process == NULL)
        return -1;

    process->name = getMemory( strlen(name) );
    if(process->name == NULL) {
        erasePCB( process );
        return -1;
    }
    strcpy(process->name, name);

    if(heap_size != 0) {
        process->heap.base = getMemory( heap_size * PAGE_SIZE );
        if(process->heap.base == NULL) {
            erasePCB( process );
            return -1;
        }
    } else {
        process->heap.base = NULL;
    }
    process->heap.size = heap_size * PAGE_SIZE;
    uint32_t * heapInitData = process->heap.base;
    *heapInitData = 0;

    process->pid = pid;
    processList[process->pid] = process;

    purgeFdList(process, FALSE);
    purgeThreadList(process, FALSE, FALSE);

    process->cwd = getRoot();

    process->finishedSem = sem_create(0);

    process_count++;
    alive_process_count++;
    
    createThread(process, code, NULL, stack_size, TRUE);    

    //printf("Creando %s\n", process->name);

    return process->pid;
}

int getHeapSize(int pid) {
    return processList[pid]->heap.size;
}

void * getHeapBase(int pid) {
    return processList[pid]->heap.base;
}

int getFreePID() {
    for(int i = 0; i < MAX_PROCESS_COUNT; i++)
        if(processList[i] == NULL)
            return i;

    return -1;
}

void killProcess(int pid, int retValue) {

    int isCurrProcess = (getCurrentPID() == pid);

    purgeThreadList(processList[pid], TRUE, TRUE);
    purgeFdList(processList[pid], TRUE);

    processList[pid]->retValue = retValue;

    alive_process_count--;

    sem_signal( processList[pid]->finishedSem ); //Despertamos a potencial waitpid

    if(isCurrProcess)
        _force_scheduler();
}

int waitpid(int pid) {
    //TODO: VERIFICAR QUE SEA PADRE
    sem_wait( processList[pid]->finishedSem );

    int retValue = processList[pid]->retValue;

    process_count--;
    erasePCB(processList[pid]); //Borramos zombie
    processList[pid] = NULL;

    return retValue;
}
// ** MANEJO DE PROCESOS ** // 


// ** MANEJO DE THREADS ** // 
int getFreeTID(process_t * process) {
    for(int i = 0; i < MAX_THREAD_COUNT; i++)
        if(process->threadList[i] == NULL)
            return i;

    return -1;
}

void purgeThreadList(process_t * process, int close, int called_from_kill_process) {
    for(int i = 0; i < MAX_THREAD_COUNT; i++) {
        if(process->threadList[i] != NULL) {
            if(close == TRUE)
                killThread(process->pid, i, called_from_kill_process);
            process->threadList[i] = NULL;
        }
    }
}

void killThread(int pid, int tid, int called_from_kill_process) {

    thread_t * t = getProcessByPID(pid)->threadList[tid];
    int isCurrThread = (t == getCurrentThread());
    int thread_in_scheduler = scheduler_dequeue_thread(t);

    if(!thread_in_scheduler) {
        t->status = DEAD;
    }

    if(t->is_someone_joining == FALSE || called_from_kill_process) { //Solo borro si nadie me espera
        processList[pid]->threadList[tid] = NULL;

        if(thread_in_scheduler) //Ya voló
            eraseTCB( processList[pid]->threadList[tid] );

    } else
        sem_signal( t->finishedSem ); //Despertamos al thread que hizo join


    if(tid == 0 && !called_from_kill_process)
        killProcess(pid, -1);

    if (isCurrThread && !called_from_kill_process)
        _force_scheduler();
}

thread_t * createThread(process_t * process, void * code, void * args, int stack_size, int isMain) {

    int tid = getFreeTID(process);
    if(tid == -1) //No entran mas
        return NULL;

    thread_t * thread = getMemory( sizeof(process_t) );
    if(thread == NULL)
        return NULL;

    thread->stack.base = getMemory( stack_size * PAGE_SIZE );
    if(thread->stack.base == NULL) {
        eraseTCB(thread);
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
    thread->finishedSem = sem_create(0);
    thread->is_someone_joining = FALSE;

    thread->status = READY;

    thread->tid = tid;
    process->threadList[thread->tid] = thread;
    //process->threadCount++;

    scheduler_enqueue(thread);

    return thread;
}

void eraseTCB(thread_t * thread) {
    freeMemory(thread->stack.base);
    freeMemory(thread);
}

void threadJoin(int tid, void **retVal) {
    if(tid < 0 || tid >= MAX_THREAD_COUNT)
        return;

    thread_t * awaitThread = processList[getCurrentPID()]->threadList[tid];
    if(awaitThread == NULL)
        return;

    awaitThread->is_someone_joining = TRUE;

    sem_wait( awaitThread->finishedSem );

    if(retVal != NULL)
        *retVal = awaitThread->retValue;

    processList[getCurrentPID()]->threadList[tid] = NULL;
    if(awaitThread->status != DEAD)
        eraseTCB( awaitThread ); //Borramos zombie
}
// ** MANEJO DE THREADS ** // 



// ** MANEJO DE FD ** // 
void purgeFdList(process_t * process, int close) {
    for(int i = 0; i < MAX_FD_COUNT; i++) {
        if(process->fd_table[i] != NULL) {
            if(close == TRUE)
                closeFile(process->fd_table[i]);
            process->fd_table[i] = NULL;
        }
    }
}

void listProcess(ps_struct buffer[], int * bufferCount) {
    int n = 0;
    for(int i = 0; i < MAX_PROCESS_COUNT; i++) {
        if(processList[i] != NULL) {
            buffer[i].pid = processList[i]->pid;
            buffer[i].name = processList[i]->name;
            buffer[i].threadCount = 0;
            buffer[i].heapBase = (int *) processList[i]->heap.base; /* Bien? */
            buffer[i].heapSize = processList[i]->heap.size;
            n++;
        }
    }
    *bufferCount = n;
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
  if (fdIndex < 3 || fdIndex >= MAX_FD_COUNT)
    return NULL;
  fd_t * ret = processList[pid]->fd_table[fdIndex];
  processList[pid]->fd_table[fdIndex] = NULL;
  return ret;
}

fd_t * getFD(int pid, int fdIndex) {
  if (fdIndex < 3 || fdIndex >= MAX_FD_COUNT)
    return NULL;
  return processList[pid]->fd_table[fdIndex];
}

int getFreeFD(int pid) {
    for(int i = 3; i < MAX_FD_COUNT; i++)
        if(processList[pid]->fd_table[i] == NULL)
            return i;

    return -1;
}
// ** MANEJO DE FD ** // 