#include <stdint.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>
#include <interruptions/intHandlers.h>

unsigned long int ticks = 0;

typedef struct timerFunction{
	function f;
	unsigned long int ticks;
};

#define MAX_FUNCTIONS 255

struct timerFunction timerFunctions[MAX_FUNCTIONS] = {0, 0}; //CONIRMAR SI GARANTIZA QUE TODOS LOS VALORES DE F VAN A EMPEZAR EN 0

void timerRestart() {
	for(int i = 0; i < MAX_FUNCTIONS; i++)
		timerFunctions[i].f = 0;
}

void timerTick() {

	for(int i = 0; i < MAX_FUNCTIONS && timerFunctions[i].f != 0; i++)
		if( ticks % timerFunctions[i].ticks == 0 )
			timerFunctions[i].f();
	
	ticks++;

}

void appendFunctionToTimer(function f, unsigned long int ticks) {
	for(int i = 0; i < MAX_FUNCTIONS; i++) {
		if( timerFunctions[i].f == 0 ) {
			timerFunctions[i].f = f;
			timerFunctions[i].ticks = ticks;
			return;
		}
			
	}	
}

void removeFunctionFromTimer(function f) {
	int i, j;
	for(i = 0; i < MAX_FUNCTIONS; i++) {
		if( timerFunctions[i].f == f ) {
			timerFunctions[i].f = 0;
			timerFunctions[i].ticks = 0;
			for(j = i + 1; j < MAX_FUNCTIONS; j++) {
				timerFunctions[j - 1].f = timerFunctions[j].f;
				timerFunctions[j - 1].ticks = timerFunctions[j].ticks;
			}			
			break;
		}
	}
}

int int80Handler(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx) {
	switch(rax) {
		case 3: //Read
			if( rbx == 0 ) { //stdin
				char * buff = (char*)rcx;
				char r;
				int i;
				for(i = 0; i < rdx && (r = getChar()) != -1; i++ )
					buff[i] = r;

				return i;
			}
			break;
		case 4: //Write
			if( rbx == 1 )
				setFontColor(WHITE);
			else if( rbx == 2)
				setFontColor(RED);

			char * str = (char*)rcx;
			for(int i = 0; i < rdx; i++) {
				printf("%c", str[i]);
			}
			break;
	}
	return 0;
}