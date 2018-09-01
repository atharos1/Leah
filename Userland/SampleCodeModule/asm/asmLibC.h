#ifndef ASM_LIB_C
#define ASM_LIB_C

typedef void (*function)();

int sys_read(int fileDescriptor, void * buff, int length);
int sys_write(int fileDescriptor, void * buff, int length);
void sys_clearScreen();
void sys_setFontColor(unsigned int color);
void sys_setBackgroundColor(unsigned int color);
int sys_setFontSize(unsigned int size);
unsigned int sys_getFontColor();
unsigned int sys_getBackgroundColor();
unsigned int sys_getFontSize();
void sys_drawPixel(int x, int y, int color);
int sys_setCursor(unsigned int x, unsigned int y);
void sys_setGraphicCursorStatus(unsigned int status);
int sys_getScreenHeight();
int sys_getScreenWidth();
void sys_memoryManagerTest(int pages);
void sys_timerAppend(function f, unsigned long int ticks);
void sys_timerRemove(function f);
void sys_beep(int nFrequence, unsigned char duration);
int sys_rtc(int fetch);
void _throwInvalidOpCode();
#endif
