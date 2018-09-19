#include "mutex.h"
#include "scheduler.h"
#include "fileSystem.h"

void strcat(char *original, char *add)
{
   while(*original)
      original++;
 
   while(*add)
   {
      *original = *add;
      add++;
      original++;
   }
   *original = '\0';
}

mutex_t mutex_create() {
    /*char path[strlen(name) + strlen("dev/shm/mutex-") + 1];
    strcpy(path, "dev/shm/");
    strcat(path, name);

    makeFile(path, SYMBOLIC_LINK);*/

	mutex_t m = getMemory(sizeof(mutex_struct));
	*(m->value) = 0;
	m->lockedQueue = NULL;
	return m;
}

/*mutex_t mutex_open(char * name) {
    char path[strlen(name) + strlen("dev/shm/mutex-") + 1];
    strcpy(path, "dev/shm/");
    strcat(path, name);

    opened_file_t * fd = openFile(path, O_RDONLY);
    registerFD(getCurrentPID(), fd);

	mutex_t m = getMemory(sizeof(mutex_struct));
	*(m->value) = 0;
	m->lockedQueue = NULL;
	return m;
}*/

void mutex_lock(mutex_t mutex) {
    if(_mutex_acquire(mutex->value)) {
        mutex->owner = getCurrentThread();
    } else {
        mutex->lockedQueue = insertAtEnd(mutex->lockedQueue, scheduler_dequeue_current());
		_force_scheduler();
    }
}

void mutex_unlock(mutex_t mutex) {
    if( mutex->owner == getCurrentThread() ) {
        *(mutex->value) = 0;
        thread_t * t = getFirst(mutex->lockedQueue);
		mutex->lockedQueue = deleteHead(mutex->lockedQueue);
		scheduler_enqueue(t);
    }
}