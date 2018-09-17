#include <stdint.h>
#include <drivers/kb_driver.h>
#include <drivers/console.h>
#include <drivers/speaker.h>
#include <drivers/video_vm.h>
#include <drivers/timer.h>
#include <memoryManagerTest.h>
#include <asm/libasm.h>
#include <fileSystem.h>
#include <sleep.h>
#include <scheduler.h>

int int80Handler(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx) {
	switch(rax) {
		case 1: //Exit
			while(1) {
				printf("Codigo de salida: %d\n", rdi);
				_halt();
			}
			break;
		case 3: //Read
			if( rdi == 1 ) { //stdin
				char * buff = (char*)rsi;
				char r;
				int i;
				for(i = 0; i < rdx && (r = getChar()) != -1; i++ )
					buff[i] = r;

				return i;
			}
			break;
		case 4: //Write
			if( rdi == 2) { //STD_ERR
				setFontColor(0x000000);
				setBackgroundColor(0xDC143C);
			}

			char * str = (char*)rsi;
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
			setFontColor(rdi);
			//printf("color: %d", rdi);
			return 0;
			break;
		case 7: //setBackgroundColor
			setBackgroundColor(rdi);
			return 0;
			break;
		case 8: //setFontSize
			setFontSize(rdi);
			return 0;
			break;
		case 9:
			return getFontColor();
			break;
		case 10:
			return getBackgroundColor();
			break;
		case 11:
			return getFontSize();
			break;
		case 12: //drawPixel
			drawPixel(rdi, rsi, rdx);
			return 0;
			break;
		case 13: //setCursor
			setCursor(rdi, rsi);
			return 0;
			break;
		case 14: //setGraphicCursorStatus
			setGraphicCursorStatus(rdi);
			return 0;
			break;
		case 15:
			return SCREEN_HEIGHT;
			break;
		case 16:
			return SCREEN_WIDTH;
			break;
		case 17: //sys_memoryManagerTest
			memoryManagerTest(rdi);
			break;
		case 18: //sys_listDir
			listDir((char*)rdi);
			break;
		case 19: //sys_makeFile
			makeFile((char*)rdi, rsi);
			break;
		case 20: //sys_removeFile
			removeFileFromPath((char*)rdi);
			break;
		case 21: //sys_cat
		case 100: //timerAppend, return 0 if successful, -1 if error
			//printf("\nParametros: RAX %d rdi %d rsi %d RDX %d\n", rax, rdi, rsi, rdx);
			return timer_appendFunction( (function)rdi, rsi );
			break;
		case 101: //timerRemove, return 0 if successful, -1 if error
			return timer_removeFunction( (function)rdi );
			break;
		case 102: //beep
			beep(rdi, rsi);
			return 0;
			break;
		case 103: //nosound
			nosound();
			return 0;
			break;
		case 200: //RTC
			return _RTC(rdi);
	}
	return 0;
}
