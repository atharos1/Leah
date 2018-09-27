#define MAX_FUNCTIONS 255

#include "include/timer.h"
#include "../include/sleep.h"
#include "../include/scheduler.h"

int timerSkip = FALSE;

typedef struct {
	function f;
	unsigned long int ticks;
	unsigned long int remainingTicks;
} timerFunction;

static timerFunction timerFunctions[MAX_FUNCTIONS] = {{0}}; //CONIRMAR SI GARANTIZA QUE TODOS LOS VALORES DE F VAN A EMPEZAR EN 0

void timer_Restart() {
	for(int i = 0; i < MAX_FUNCTIONS; i++)
		timerFunctions[i].f = 0;
}

void noTimer() {
	timerSkip = TRUE;
	FORCE = TRUE;
}

void timer_Tick() {

	if (timerSkip) {
	 	timerSkip = FALSE;
	 	return;
	}

	sleep_update();

	for(int i = 0; i < MAX_FUNCTIONS && timerFunctions[i].f != 0; i++) {
		timerFunctions[i].remainingTicks--;
		if( timerFunctions[i].remainingTicks == 0 ) {
			timerFunctions[i].remainingTicks = timerFunctions[i].ticks;
			timerFunctions[i].f();
		}
	}
}

int timer_appendFunction(function f, unsigned long int ticks) {
	for(int i = 0; i < MAX_FUNCTIONS; i++) {
		if( timerFunctions[i].f == 0 ) {
			timerFunctions[i].f = f;
			timerFunctions[i].ticks = timerFunctions[i].remainingTicks = ticks;
			return 0;
		}

	}
	return -1;
}

int timer_removeFunction(function f) {
	int i, j;
	for(i = 0; i < MAX_FUNCTIONS; i++) {
		if( timerFunctions[i].f == f ) {
			timerFunctions[i].f = 0;
			timerFunctions[i].ticks = 0;
			for(j = i + 1; j < MAX_FUNCTIONS; j++) {
				timerFunctions[j - 1].f = timerFunctions[j].f;
				timerFunctions[j - 1].remainingTicks = timerFunctions[j].remainingTicks;
				timerFunctions[j - 1].ticks = timerFunctions[j].ticks;
			}
			return 0;
		}
	}
	return -1;
}
