#include "process.h"
#include "scheduler.h"
#include "asm/libasm.h"
#include "drivers/console.h"

#define MAX_PROCESS_COUNT 50

int last_pid = 0;

void threadWrapper(int code()) {

    int (*thread_code)() = code;
    thread_code();

    int ret = code();

    printf("\nEl programa finalizó con código de respuesta: %d\n", ret);
    while(1)
        _halt();
}

process_t * createProcess(char * name, void * code, int stack_size, int heap_size) {
    process_t * process = getMemory( sizeof(process_t) );
    if(process == NULL)
        return NULL;

    process->name = getMemory( strlen(name) );
    if(process->name == NULL) {
        freeMemory( process );
        return NULL;
    }
    strcpy(process->name, name);

    process->heap.base = getMemory( heap_size * PAGE_SIZE );
    if(process->heap.base == NULL) {
        freeMemory( process->name );
        freeMemory( process );
        return NULL;
    }
    process->heap.size = heap_size * PAGE_SIZE;

    process->threadNum = 0;

    process->pid = ++last_pid;
    //processList[process->pid] = process;

    thread_t * mainThread = createThread(process, code, stack_size);

    scheduler_enqueue(mainThread);

    return process;
}

thread_t * createThread(process_t * process, void * code, int stack_size) {

    if(process->threadNum  == MAX_THREAD_COUNT)
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

    thread->process = process;

    return thread;
}