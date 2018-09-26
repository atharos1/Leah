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

int int80Handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx) {
	switch(rdi) {
		case 1: //Exit
			while(1) {
				printf("Codigo de salida: %d\n", rsi);
				_halt();
			}
			break;
		case 3: //Read
			if( rsi == 1 ) { //stdin
				char * buff = (char*)rdx;
				char r;
				int i;
				for(i = 0; i < rcx && (r = getChar()) != -1; i++ )
					buff[i] = r;

				return i;
			} else {
				return readFromFD(rsi, (char*)rdx, rcx);
			}

			break;
		case 4: //Write
			if (rsi == 1 || rsi == 2) {
				if( rsi == 2) { //STD_ERR
					setFontColor(0x000000);
					setBackgroundColor(0xDC143C);
				}
				char * str = (char*)rdx;
				int i;
				for(i = 0; i < rcx; i++)
					printChar(str[i]);
				return i;
			} else {
				return writeToFD(rsi, (char*)rdx, rcx);
			}

			break;
		case 5: //ClearScreen
			clearScreen();
			return 0;
			break;
		case 6: //setFontColor
			setFontColor(rsi);
			//printf("color: %d", rsi);
			return 0;
			break;
		case 7: //setBackgroundColor
			setBackgroundColor(rsi);
			return 0;
			break;
		case 8: //setFontSize
			setFontSize(rsi);
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
			drawPixel(rsi, rdx, rcx);
			return 0;
			break;
		case 13: //setCursor
			setCursor(rsi, rdx);
			return 0;
			break;
		case 14: //setGraphicCursorStatus
			setGraphicCursorStatus(rsi);
			return 0;
			break;
		case 15:
			return SCREEN_HEIGHT;
			break;
		case 16:
			return SCREEN_WIDTH;
			break;
		case 17: //sys_memoryManagerTest
			memoryManagerTest(rsi);
			break;
		case 18: //sys_listDir
			listDir((char*)rsi);
			break;
		case 19: //sys_makeFile
			makeFile((char*)rsi, rdx);
			break;
		case 20: //sys_removeFile
			removeFileFromPath((char*)rsi);
			break;
		case 21: //sys_open
			return openFileFromPathToFD((char*)rsi, rdx);
			break;
		case 22: //sys_close
			closeFileFromFD(rsi);
			break;
		case 23:
			semCreate((char*)rsi, rdx);
			break;
		case 24:
			semDelete((char*)rsi);
			break;
		case 25:
			return semOpen((char*)rsi);
			break;
		case 26:
			semClose(rsi);
			break;
		case 27:
			semSet(rsi, rdx);
			break;
		case 28:
			semWait(rsi);
			break;
		case 29:
			semSignal(rsi);
			break;
		case 30:
			mutexCreate((char*)rsi);
			break;
		case 31:
			mutexDelete((char*)rsi);
			break;
		case 32:
			return mutexOpen((char*)rsi);
			break;
		case 33:
			mutexClose(rsi);
			break;
		case 34:
			mutexLock(rsi);
			break;
		case 35:
			mutexUnlock(rsi);
			break;
		case 36:
			changeCWD((char*)rsi);
			break;
		case 37:
			getCWDPath((char*)rsi);
			break;
		case 40: //sleep
			sleepCurrentThread(rsi);
			break;
		case 41: //ps
			listProcess();
			break;
		case 50: //new thread
			return createThread(getProcessByPID(getCurrentPID()), (void*)rsi, (void*)rdx, 4, FALSE)->tid;
			break;
		case 51: //thread join
			threadJoin(rsi, rdx);
			break;
		case 52: //thread cancel
			killThread(getCurrentPID(), rsi, FALSE);
			break;
		case 100: //timerAppend, return 0 if successful, -1 if error
			//printf("\nParametros: RAX %d rsi %d rdx %d RDX %d\n", rdi, rsi, rdx, rcx);
			return timer_appendFunction( (function)rsi, rdx );
			break;
		case 101: //timerRemove, return 0 if successful, -1 if error
			return timer_removeFunction( (function)rsi );
			break;
		case 102: //beep
			beep(rsi, rdx);
			return 0;
			break;
		case 103: //nosound
			nosound();
			return 0;
			break;
		case 200: //RTC
			return _RTC(rsi);
			break;
	}
	return 0;
}
