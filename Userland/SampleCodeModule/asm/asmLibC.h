#ifndef ASM_LIB_C
#define ASM_LIB_C

typedef void (*function)();

typedef int pthread_t;

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
void sys_memoryManagerTest(int bytes);
void sys_listDir(char * path);
void sys_makeFile(char * path, int type);
void sys_removeFile(char * path);
int sys_open(char * path, int mode);
void sys_close(int fd);
void sys_semCreate(char * name, int value);
void sys_semDelete(char * name);
int sys_semOpen(char * name);
void sys_semClose(int sem);
void sys_semSet(int sem, int value);
void sys_semWait(int sem);
void sys_semSignal(int sem);
void sys_mutexCreate(char * name);
void sys_mutexDelete(char * name);
int sys_mutexOpen(char * name);
void sys_mutexClose(int mutex);
void sys_mutexLock(int mutex);
void sys_mutexUnlock(int mutex);
void sys_timerAppend(function f, unsigned long int ticks);
void sys_timerRemove(function f);
void sys_beep(int nFrequence, unsigned char duration);
int sys_rtc(int fetch);
void _throwInvalidOpCode();
void sys_sleep();
void sys_listProcess();

pthread_t sys_newThread(void *(*start_routine) (void *), void *arg);
void sys_joinThread(pthread_t thread, void ** retVal);
void sys_cancelThread(pthread_t thread);

#endif
