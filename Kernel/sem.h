#ifndef IPC_SEM_H
#define IPC_SEM_H

#include "circularList.h"
#include "mutex.h"

typedef struct {
	int value;
	NODE * lockedQueue;
	mutex_t mutex;
} sem_struct;

typedef sem_struct * sem_t;

sem_t sem_create(char * name, int startValue);
void sem_wait(sem_t sem);
void sem_signal(sem_t sem);

#endif