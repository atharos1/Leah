#include <stdint.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>

int cont = 0;

void timerTick() {
	/*cont++;
	if(cont == 18.2 * 5) { //1 segundo
		cont = 0;
		ncPrint("Mxldxtxs pxtxs hxtxrx pxtrxxrcxlxs");
		ncNewline();
	}*/
	
}

void int80Handler(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx) {
	switch(rax) {
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
}