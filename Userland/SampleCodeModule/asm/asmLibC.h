#ifndef ASM_LIB_C
#define ASM_LIB_C

#define O_WRONLY 0
#define O_RDONLY 1
#define O_RDWR 2

#define DIRECTORY 0
#define REGULAR_FILE 1
#define BUFFER 2
#define SEMAPHORE 3
#define MUTEX 4

#define MAX_PROCESS_COUNT 100

typedef void (*function)();
typedef int pthread_t;

typedef struct {
    int pid;
    char *name;
    char *parentName;
    int threadCount;
    int heapSize;
    int status;
    int foreground;
    int niceness;
} ps_struct;
typedef ps_struct *ps_info;

int sys_read(int fileDescriptor, void *buff, int length);
int sys_write(int fileDescriptor, void *buff, int length);
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
void sys_listDir(char *path);
void sys_makeFile(char *path, int type);
void sys_makeDir(char *path);
void sys_makeRegFile(char *path);
void sys_mkFifo(char *path);
void sys_removeFile(char *path);
int sys_open(char *path, int mode);
void sys_close(int fd);
void sys_semCreate(char *name, int value);
void sys_semDelete(char *name);
int sys_semOpen(char *name);
void sys_semClose(int sem);
void sys_semSet(int sem, int value);
void sys_semWait(int sem);
void sys_semSignal(int sem);
void sys_mutexCreate(char *name);
void sys_mutexDelete(char *name);
int sys_mutexOpen(char *name);
void sys_mutexClose(int mutex);
void sys_mutexLock(int mutex);
void sys_mutexUnlock(int mutex);
void sys_chdir(char *path);
void sys_beep(int nFrequence, unsigned char duration);
int sys_rtc(int fetch);
void _throwInvalidOpCode();
void sys_sleep();
void sys_listProcess(ps_struct buffer[], int *bufferCount);

int sys_newProcess(char *name, int (*start_routine)(char **args), char **args);
int sys_waitPID(int pid);
int sys_getHeapSize();
void *sys_getHeapBase();
void sys_killProcess(int pid);
void sys_exit(int retVal);
void sys_setForeground(int pid);

pthread_t sys_newThread(void *(*start_routine)(void *), void *arg);
void sys_joinThread(pthread_t thread, void **retVal);
void sys_cancelThread(pthread_t thread);

#endif
