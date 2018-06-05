#include <stdint.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>
#include <drivers/speaker.h>
#include <drivers/video_vm.h>
#include <drivers/timer.h>

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
			return timer_appendFunction( (function)rbx, rcx );
			break;
		case 101: //timerRemove, return 0 if successful, -1 if error
			return timer_removeFunction( (function)rbx );
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
