#include "include/upDown.h"
#include "../StandardLibrary/include/mutex.h"
#include "../StandardLibrary/include/pthread.h"
#include "../StandardLibrary/include/stdio.h"

int global = 0;
int m;

void upDown() {
  mutex_create("prodConsMutex");
  m = mutex_open("prodConsMutex");
  pthread_t ups[5], downs[5];

  printf("\nVariable global inicializada en %d\n", global);

  for (int i = 0; i < 5; i ++) {
    ups[i] = pthread_create(&up, (void*) 0);
    downs[i] = pthread_create(&down, (void*) 0);
  }

  printf("\nSe crearon 5 threads que aumentan la variable 1000 veces y 5 que disminuyen 1000 veces\n");

  void * retvalue;

  for (int i = 0; i < 5; i ++) {
    pthread_join(ups[i], &retvalue);
    pthread_join(downs[i], &retvalue);
  }

  printf("\nLos threads terminaron de ejecutarse\n");

  printf("\nLa variable global vale %d\n", global);

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
