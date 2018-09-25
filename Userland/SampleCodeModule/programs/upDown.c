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

  //sys_sleep(2000);
  void * retvalue;

  for (int i = 0; i < 5; i ++) {
    pthread_join(ups[i], &retvalue);
    pthread_join(downs[i], &retvalue);
  }

  printf("\nLISTOOO\n");

  printf("global = %d", global);

}

void * up(void * args) {
	for (int i = 0; i < 1000; i ++) {
		mutex_lock(m);

    global ++ ;

		mutex_unlock(m);
	}
	return 0;
}

void * down(void * args) {
	for (int i = 0; i < 1000; i ++) {
		mutex_lock(m);

    global -- ;

		mutex_unlock(m);
	}
	return 0;
}
