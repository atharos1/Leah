#include "upDown.h"
#include <../StandardLibrary/mutex.h>
#include <../StandardLibrary/pthread.h>
#include <../StandardLibrary/stdio.h>

int global = 0;
int m;

void upDown() {
  mutex_create("prodConsMutex");
  m = mutex_open("prodConsMutex");
  pthread_t ups[5], downs[5];

  for (int i = 0; i < 5; i ++) {
    ups[i] = pthread_create(&up, (void*) 0);
    downs[i] = pthread_create(&down, (void*) 0);
  }
  printf("LISTOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
  sys_sleep(10000);
  printf("global = %d", global);

}

void * up(void * args) {
	for (int i = 0; i < 2; i ++) {
		mutex_lock(m);
    printf("soy up\n");
    global ++ ;

		mutex_unlock(m);
	}
	return 0;
}

void * down(void * args) {
	for (int i = 0; i < 2; i ++) {
		mutex_lock(m);
    printf("soy down\n");

    global -- ;

		mutex_unlock(m);
	}
	return 0;
}
