#include "asmLibC.h"

typedef void (*function)();

int _systemCall();

int sys_read(int fileDescriptor, void *buff, int length) {
    _systemCall(3, fileDescriptor, buff, length, 0);
}

int sys_write(int fileDescriptor, void *buff, int length) {
    return _systemCall(4, fileDescriptor, buff, length, 0);
}

void sys_clearScreen() { _systemCall(5, 0, 0, 0, 0); }

void sys_setFontColor(unsigned int color) { _systemCall(6, color, 0, 0, 0); }

void sys_setBackgroundColor(unsigned int color) {
    _systemCall(7, color, 0, 0, 0);
}

int sys_setFontSize(unsigned int size) { return _systemCall(8, size, 0, 0, 0); }

unsigned int sys_getFontColor() { return _systemCall(9, 0, 0, 0, 0); }

unsigned int sys_getBackgroundColor() { return _systemCall(10, 0, 0, 0, 0); }

unsigned int sys_getFontSize() { return _systemCall(11, 0, 0, 0, 0); }

void sys_drawPixel(int x, int y, int color) { _systemCall(12, x, y, color, 0); }

int sys_setCursor(unsigned int x, unsigned int y) {
    return _systemCall(13, x, y, 0, 0);
}

void sys_setGraphicCursorStatus(unsigned int status) {
    _systemCall(14, status, 0, 0, 0);
}

int sys_getScreenHeight() { return _systemCall(15, 0, 0, 0, 0); }

int sys_getScreenWidth() { return _systemCall(16, 0, 0, 0, 0); }

void sys_memoryManagerTest(int bytes) { _systemCall(17, bytes, 0, 0, 0); }

void sys_listDir(char *path) { _systemCall(18, path, 0, 0, 0); }

void sys_makeFile(char *path, int type) { _systemCall(19, path, type, 0, 0); }

void sys_makeDir(char *path) { _systemCall(19, path, DIRECTORY, 0, 0); }

void sys_makeRegFile(char *path) { _systemCall(19, path, REGULAR_FILE, 0, 0); }

void sys_mkFifo(char *path) { _systemCall(19, path, BUFFER, 0, 0); }

void sys_removeFile(char *path) { _systemCall(20, path, 0, 0, 0); }

int sys_open(char *path, int mode) { return _systemCall(21, path, mode, 0, 0); }

void sys_close(int fd) { _systemCall(22, fd, 0, 0, 0); }

void sys_semCreate(char *name, int value) {
    _systemCall(23, name, value, 0, 0);
}

void sys_semDelete(char *name) { _systemCall(24, name, 0, 0, 0); }

int sys_semOpen(char *name) { return _systemCall(25, name, 0, 0, 0); }

void sys_semClose(int sem) { _systemCall(26, sem, 0, 0, 0); }

void sys_semSet(int sem, int value) { _systemCall(27, sem, value, 0, 0); }

void sys_semWait(int sem) { _systemCall(28, sem, 0, 0, 0); }

void sys_semSignal(int sem) { _systemCall(29, sem, 0, 0, 0); }

void sys_mutexCreate(char *name) { _systemCall(30, name, 0, 0, 0); }

void sys_mutexDelete(char *name) { _systemCall(31, name, 0, 0, 0); }

int sys_mutexOpen(char *name) { return _systemCall(32, name, 0, 0, 0); }

void sys_mutexClose(int mutex) { _systemCall(33, mutex, 0, 0, 0); }

void sys_mutexLock(int mutex) { _systemCall(34, mutex, 0, 0, 0); }

void sys_mutexUnlock(int mutex) { _systemCall(35, mutex, 0, 0, 0); }

void sys_chdir(char *path) { _systemCall(36, path, 0, 0, 0); }

void sys_getcwd(char *pathBuff) { _systemCall(37, pathBuff, 0, 0, 0); }

void sys_pipe(int fd[2]) {
    _systemCall(38, fd, 0, 0, 0);
}

void sys_sleep(int millis) {
    _systemCall(40, millis, 0, 0, 0);
}

void sys_listProcess(ps_struct buffer[], int *bufferCount) {
    _systemCall(41, buffer, bufferCount, 0, 0);
}

int sys_newProcess(char *name, int (*start_routine)(char **args), char **args, int ** fdReplace) {
    return _systemCall(42, name, start_routine, args, fdReplace);
}

int sys_waitPID(int pid) { return _systemCall(43, pid, 0, 0); }

int sys_getHeapSize() { return _systemCall(44, 0, 0, 0); }

void *sys_getHeapBase() { return _systemCall(45, 0, 0, 0); }

void sys_killProcess(int pid) { _systemCall(46, pid, 0, 0); }

void sys_exit(int retVal) { _systemCall(47, retVal, 0, 0); }

void sys_setForeground(int pid) { _systemCall(48, pid, 0, 0); }

void sys_dup2(int oldFd, int newFd) { _systemCall(49, oldFd, newFd, 0); }

pthread_t sys_newThread(void *(*start_routine)(void *), void *arg) {
    return _systemCall(50, start_routine, arg, 0);
}

void sys_joinThread(pthread_t thread, void **retVal) {
    _systemCall(51, thread, retVal, 0);
}

void sys_cancelThread(pthread_t thread) { _systemCall(52, thread, 0, 0); }

void sys_beep(int nFrequence, unsigned char duration) {
    _systemCall(102, nFrequence, duration, 0, 0);
}

int sys_rtc(int fetch) { return _systemCall(200, fetch, 0, 0, 0); }
