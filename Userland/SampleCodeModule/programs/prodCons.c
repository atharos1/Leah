#include "include/prodCons.h"
#include "../StandardLibrary/include/mutex.h"
#include "../StandardLibrary/include/sem.h"
#include "../StandardLibrary/include/pthread.h"
#include "../StandardLibrary/include/stdio.h"

int full;
int empty;
int mutex;
int plates = 0;
pthread_t waiters[MAX_PRODCONS] = {0};
pthread_t chefs[MAX_PRODCONS] = {0};
int waitersQty = 0, chefsQty = 0;
int waitersToDie = 0, chefsToDie = 0;

int prodcons() {
  printMenu();
  mutex_create("prodConsMutex");
  mutex = mutex_open("prodConsMutex");
	sem_create("fullSem", TRAYS_QTY);
  full = sem_open("fullSem");
  sem_create("emptySem", 0);
  empty = sem_open("emptySem");
  plates = 0;
  waitersQty = 0;
  chefsQty = 0;
  waitersToDie = 0;
  chefsToDie = 0;

	printTrays();

	int c;
	while((c = getchar()) != QUIT) {
		switch(c) {
			case INC_CHEF:
				callChef();
				break;
			case DEC_CHEF:
				restChef();
				break;
			case INC_WAITER:
				callWaiter();
				break;
			case DEC_WAITER:
				restWaiter();
				break;
			default:
				break;
		}
	}

	terminateAll();
  mutex_delete("prodConsMutex");
  sem_delete("fullSem");
  sem_delete("emptySem");
	printf("La simulacion termino\n");
	return 0;
}

void printMenu() {
  printf("\nEl restaurante Leah tiene un maximo de 20 platos\n");
	printf("Hay 10 chefs y 10 camareros que inicialmente estan descansando\n\n", MAX_PRODCONS, MAX_PRODCONS);
	printf("Inserte:\n");
  printf("    %c para llamar a un chef\n", INC_CHEF);
	printf("    %c para poner a descansar a un chef\n", DEC_CHEF);
	printf("    %c para llamar a un camarero\n", INC_WAITER);
	printf("    %c para poner a descansar a un camarero\n", DEC_WAITER);
	printf("    %c para terminar la simulacion\n\n\n", QUIT);
}

void printTrays() {
	printf("Platos = ");
	for (int i = 0; i < plates; i ++) {
    printf("0");
  }
  for (int i = plates; i < TRAYS_QTY; i ++) {
    printf("_");
  }
	printf("\n\n\n");
}

void callChef() {

  mutex_lock(mutex);
	if(chefsQty >= MAX_PRODCONS) {
		printf("No hay mas chefs descansando\n\n\n");
		mutex_unlock(mutex);
		return;
	}
	chefs[chefsQty++] = pthread_create(&chef, (void*) 0);
	mutex_unlock(mutex);
}

void * chef(void * args) {
	printf("Soy un chef que se pone a trabajar!\n\n\n");
	while(1) {
		sem_wait(full);
		mutex_lock(mutex);
    if (chefsToDie > 0) {
      sem_signal(full);
      chefSuicide();
    }
    plates ++;
    sem_signal(empty);

		printTrays();

		mutex_unlock(mutex);
	}
	return 0;
}

void restChef() {
	mutex_lock(mutex);
  if(chefsQty == 0) {
		printf("No hay chefs trabajando\n\n\n");
		mutex_unlock(mutex);
		return;
	}
  chefsToDie ++;
  mutex_unlock(mutex);
}

void chefSuicide() {
  chefsQty--;
  chefsToDie--;
  pthread_t chefToDie = chefs[chefsQty];
  mutex_unlock(mutex);
  pthread_cancel(chefToDie);
	printf("Un chef se fue a descansar\n\n\n");
}

void callWaiter() {
	mutex_lock(mutex);
	if(waitersQty >= MAX_PRODCONS) {
		printf("No hay mas camareros descansando\n\n\n");
		mutex_unlock(mutex);
		return;
	}
	waiters[waitersQty++] = pthread_create(&waiter, (void*) 0);
	mutex_unlock(mutex);
}

void * waiter(void * args) {
	printf("Soy un camarero que se pone a trabajar!\n\n\n");
	while(1) {
		sem_wait(empty);
		mutex_lock(mutex);
    if (waitersToDie > 0) {
      sem_signal(empty);
      waiterSuicide();
    }
		plates --;
    sem_signal(full);

		printTrays();

		mutex_unlock(mutex);
	}
}

void restWaiter() {
	mutex_lock(mutex);
	if(waitersQty == 0) {
		printf("No hay camareros trabajando\n\n\n");
		mutex_unlock(mutex);
		return;
	}
  waitersToDie ++;
  mutex_unlock(mutex);
}

void waiterSuicide() {
  waitersQty--;
  waitersToDie--;
  pthread_t waiterToDie = waiters[waitersQty];
  mutex_unlock(mutex);
  pthread_cancel(waiterToDie);
	printf("Un camarero se fue a descansar\n\n\n");
}

void terminateAll() {
  mutex_lock(mutex);
	for (int i = 0; i < chefsQty; i++) {
			pthread_cancel(chefs[i]);
	}
  for (int i = 0; i < waitersQty; i++) {
      pthread_cancel(waiters[i]);
  }
  mutex_unlock(mutex);
}
