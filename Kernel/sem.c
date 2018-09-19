#include "sem.h"

#include "asm/libasm.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "malloc.h"

sem_t sem_create(char * name, int startValue) {
	sem_t m = getMemory(sizeof(sem_struct));
	m->value = startValue;
	m->lockedQueue = NULL;
	return m;
}

void sem_wait(sem_t sem) {
	if(sem->value == 0) {
		sem->lockedQueue = insertAtEnd(sem->lockedQueue, scheduler_dequeue_current());
		_force_scheduler();
	}

	_sem_decrement();
}

void sem_signal(sem_t sem) {
	if(sem->lockedQueue != NULL) {
		thread_t * t = getFirst(sem->lockedQueue);
		//sem->lockedQueue = deleteHead(sem->lockedQueue);
		scheduler_enqueue(t);
	}
		
	_sem_increment();
}