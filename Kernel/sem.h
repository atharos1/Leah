#ifndef IPC_SEM_H
#define IPC_SEM_H

#include "mutex.h"

typedef struct {
	int value;
	NODE * lockedQueue;
	mutex_t mutex;
} sem_struct;

typedef sem_struct * sem_t;

sem_t sem_create(int startValue);
void sem_delete(sem_t sem);
void sem_set_value(sem_t sem, int value);
void sem_wait(sem_t sem);
void sem_signal(sem_t sem);

#endif
