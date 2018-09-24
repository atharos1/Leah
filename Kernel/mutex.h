#ifndef IPC_MUTEX_H
#define IPC_MUTEX_H

#include "circularList.h"
#include "process.h"

typedef struct {
	int value;
  	thread_t * owner;
	NODE * lockedQueue;
} mutex_struct;

typedef mutex_struct * mutex_t;

mutex_t mutex_create();
void mutex_delete(mutex_t mutex);
void mutex_lock(mutex_t mutex);
void mutex_unlock(mutex_t mutex);

#endif
