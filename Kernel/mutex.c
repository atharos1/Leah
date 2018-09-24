#include "mutex.h"
#include "scheduler.h"
#include "fileSystem.h"
#include "asm/libasm.h"
#include "malloc.h"

mutex_t mutex_create() {

	mutex_t m = malloc(sizeof(mutex_struct));
	m->value = 0;
  m->owner = NULL;
	m->lockedQueue = NULL;
	return m;
}

void mutex_delete(mutex_t mutex) {
  free(mutex);
}

void mutex_lock(mutex_t mutex) {
  if (getCurrentThread() == NULL)
    return;
  if(!_mutex_acquire(&(mutex->value))) {
    mutex->owner = getCurrentThread();
  } else {
    thread_t * thread = getCurrentThread();
    mutex->lockedQueue = insertAtEnd(mutex->lockedQueue, thread);
    scheduler_dequeue_current();
		_force_scheduler();
  }
}

void mutex_unlock(mutex_t mutex) {
  if (getCurrentThread() == NULL || mutex->owner != getCurrentThread() )
    return;
    while( mutex->lockedQueue != NULL ) {
      thread_t * t = getFirst(mutex->lockedQueue);
      mutex->lockedQueue = deleteHead(mutex->lockedQueue);

      if( t->status == DEAD )
        eraseTCB(t);
      else {
        mutex->owner = t;
        scheduler_enqueue(t);
        return;
      }
    }
    mutex->value = 0;

  //printf("Libero mutex");
  
}

// void mutex_unlock(mutex_t mutex) {
//   if (getCurrentThread() == NULL)
//     return;
//   if( mutex->owner == getCurrentThread() ) {
//     thread_t * t = getFirst(mutex->lockedQueue);
//     mutex->owner = t;
//     if (t == NULL) {
//       mutex->value = 0;
//     } else {
//       mutex->lockedQueue = deleteHead(mutex->lockedQueue);
//       scheduler_enqueue(t);
//     }
//   }
// }
