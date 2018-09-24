#include "sem_bin.h"
#include "asm/libasm.h"
#include "scheduler.h"

sem_bin_t sem_bin_create(int startValue) {

	sem_bin_t sem = getMemory(sizeof(sem_bin_struct));
	if (sem == NULL)
		return NULL;

	sem->sem = sem_create(startValue > 0);
	sem->mutex = mutex_create();

	return sem;
}

void sem_bin_wait(sem_bin_t sem) {
	if (sem == NULL)
    return;

  sem_wait(sem->sem);
}

void sem_bin_signal(sem_bin_t sem) {
  if (sem == NULL)
    return;

	mutex_lock(sem->mutex);
	if (sem->sem->value == 0)
		sem_signal(sem->sem);

	mutex_unlock(sem->mutex);
}
