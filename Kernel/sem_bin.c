#include "sem_bin.h"
#include "asm/libasm.h"
#include "memoryManager.h"
#include "scheduler.h"
#include "malloc.h"

sem_bin_t sem_bin_create(char * name, int startValue) {
	sem_bin_t sem = getMemory(sizeof(sem_bin_struct));
  if (sem == NULL)
    return NULL;
	sem->value = (startValue == 0) ? 0 : 1;
	sem->lockedQueue = NULL;
	sem->mutex = mutex_create();
	return sem;
}

void sem_bin_wait(sem_bin_t sem) {
  if (sem == NULL)
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

void sem_bin_signal(sem_bin_t sem) {
  if (sem == NULL)
    return;
	mutex_lock(sem->mutex);
  if (sem->value == 0) {
    if(sem->lockedQueue != NULL) {
  		thread_t * t = getFirst(sem->lockedQueue);
  		sem->lockedQueue = deleteHead(sem->lockedQueue);
  		scheduler_enqueue(t);
  	} else {
      sem->value = 1;
    }
  }
	mutex_unlock(sem->mutex);
}
