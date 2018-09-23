#include "sem.h"
#include "asm/libasm.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "malloc.h"

sem_t sem_create(int startValue) {
	sem_t sem = getMemory(sizeof(sem_struct));
	if (sem == NULL)
		return NULL;
	sem->value = startValue;
	sem->lockedQueue = NULL;
	sem->mutex = mutex_create();
	return sem;
}

void sem_delete(sem_t sem) {
	mutex_delete(sem->mutex);
	freeMemory(sem);
}

void sem_set_value(sem_t sem, int value) {
	for (int i = 0; i < value; i++) {
		sem_signal(sem);
	}
}

void sem_wait(sem_t sem) {
	if (sem == NULL)
		return;
	if (getCurrentThread() == NULL)
		return;
	mutex_lock(sem->mutex);
	if(sem->value == 0) {
		thread_t * thread = getCurrentThread();
		sem->lockedQueue = insertAtEnd(sem->lockedQueue, thread);
		mutex_unlock(sem->mutex);
    scheduler_dequeue_current();
		_force_scheduler();
	} else {
		sem->value--;
		mutex_unlock(sem->mutex);
	}
}

void sem_signal(sem_t sem) {
	if (sem == NULL)
		return;
	mutex_lock(sem->mutex);
	if(sem->lockedQueue != NULL) {
		thread_t * t = getFirst(sem->lockedQueue);
		sem->lockedQueue = deleteHead(sem->lockedQueue);
		scheduler_enqueue(t);
	} else {
		sem->value++;
	}
	mutex_unlock(sem->mutex);
}
