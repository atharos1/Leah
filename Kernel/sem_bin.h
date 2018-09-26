#ifndef IPC_SEM_BIN_H
#define IPC_SEM_BIN_H

#include "circularList.h"
#include "mutex.h"

typedef struct {
	int value;
	NODE * lockedQueue;
	mutex_t mutex;
} sem_bin_struct;

typedef sem_bin_struct * sem_bin_t;

sem_bin_t sem_bin_create(char * name, int startValue);
void sem_bin_wait(sem_bin_t sem);
void sem_bin_signal(sem_bin_t sem);

#endif
