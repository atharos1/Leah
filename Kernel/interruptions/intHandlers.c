#include <stdint.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>
#include <drivers/speaker.h>
#include <drivers/video_vm.h>
#include <interruptions/intHandlers.h>

typedef struct {
	function f;
	unsigned long int ticks;
	unsigned long int remainingTicks;
} timerFunction;

#define MAX_FUNCTIONS 255

timerFunction timerFunctions[MAX_FUNCTIONS] = {0}; //CONIRMAR SI GARANTIZA QUE TODOS LOS VALORES DE F VAN A EMPEZAR EN 0

void timerRestart() {
	for(int i = 0; i < MAX_FUNCTIONS; i++)
		timerFunctions[i].f = 0;
}

void timerTick() {

	for(int i = 0; i < MAX_FUNCTIONS && timerFunctions[i].f != 0; i++) {
		timerFunctions[i].remainingTicks--;
		if( timerFunctions[i].remainingTicks == 0 ) {
			timerFunctions[i].remainingTicks = timerFunctions[i].ticks;
			timerFunctions[i].f();
		}
	}			
}

int appendFunctionToTimer(function f, unsigned long int ticks) {
	for(int i = 0; i < MAX_FUNCTIONS; i++) {
		if( timerFunctions[i].f == 0 ) {
			timerFunctions[i].f = f;
			timerFunctions[i].ticks = timerFunctions[i].remainingTicks = ticks;
			return 0;
		}

	}
	return -1;
}

int removeFunctionFromTimer(function f) {
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

int int80Handler(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx) {
	switch(rax) {
		case 3: //Read
			if( rbx == 1 ) { //stdin
				char * buff = (char*)rcx;
				char r;
				int i;
				for(i = 0; i < rdx && (r = getChar()) != -1; i++ )
					buff[i] = r;

				return i;
			}
			break;
		case 4: //Write
			if( rbx == 2) { //STD_ERR
				setFontColor(0x000000);
				setBackgroundColor(0xDC143C);
			}

			char * str = (char*)rcx;
			int i;
			for(i = 0; i < rdx; i++)
				printChar(str[i]);

			return i;
			break;

		case 5: //ClearScreen
			clearScreen();
			return 0;
			break;
		case 6: //setFontColor
			setFontColor(rbx);
			//printf("color: %d", rbx);
			return 0;
			break;
		case 7: //setBackgroundColor
			setBackgroundColor(rbx);
			return 0;
			break;
		case 8: //setFontSize
			setFontSize(rbx);
			return 0;
			break;
		case 9: //drawPixel
			drawPixel(rbx, rcx, rdx);
			return 0;
			break;
		case 10: //setCursor
			setCursor(rbx, rcx);
			return 0;
			break;
		case 11: //setGraphicCursorStatus
			setGraphicCursorStatus(rbx);
			return 0;
			break;
		case 100: //timerAppend, return 0 if successful, -1 if error
			//printf("\nParametros: RAX %d RBX %d RCX %d RDX %d\n", rax, rbx, rcx, rdx);
			return appendFunctionToTimer( (function)rbx, rcx );
			break;
		case 101: //timerRemove, return 0 if successful, -1 if error
			return removeFunctionFromTimer( (function)rbx );
			break;
		case 102: //beep
			beep(rbx, rcx);
			return 0;
			break;
		case 103: //nosound
			nosound();
			return 0;
			break;
		case 200: //RTC
			return RTC(rbx);
	}
	return 0;
}
