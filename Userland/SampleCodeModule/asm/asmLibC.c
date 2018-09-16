typedef void (*function)();

int _systemCall();

int sys_read(int fileDescriptor, void * buff, int length) {
    _systemCall(3, fileDescriptor, buff, length, 0);
}

int sys_write(int fileDescriptor, void * buff, int length) {
    return _systemCall(4, fileDescriptor, buff, length, 0);
}

void sys_clearScreen() {
    _systemCall(5, 0, 0, 0, 0);
}

void sys_setFontColor(unsigned int color) {
    _systemCall(6, color, 0, 0, 0);
}

void sys_setBackgroundColor(unsigned int color) {
    _systemCall(7, color, 0, 0, 0);
}

int sys_setFontSize(unsigned int size) {
    return _systemCall(8, size, 0, 0, 0);
}

unsigned int sys_getFontColor() {
    return _systemCall(9, 0, 0, 0, 0);
}

unsigned int sys_getBackgroundColor() {
    return _systemCall(10, 0, 0, 0, 0);
}

unsigned int sys_getFontSize() {
    return _systemCall(11, 0, 0, 0, 0);
}

void sys_drawPixel(int x, int y, int color) {
    _systemCall(12, x, y, color, 0);
}

int sys_setCursor(unsigned int x, unsigned int y) {
    return _systemCall(13, x, y, 0, 0);
}

void sys_setGraphicCursorStatus(unsigned int status) {
    _systemCall(14, status, 0, 0, 0);
}

int sys_getScreenHeight() {
    _systemCall(15, 0, 0, 0, 0);
}

int sys_getScreenWidth() {
    _systemCall(16, 0, 0, 0, 0);
}

void sys_memoryManagerTest(int bytes) {
    _systemCall(17, bytes, 0, 0, 0);
}

void sys_listDir(char * path) {
    _systemCall(18, path, 0, 0, 0);
}

void sys_makeFile(char * path, int type) {
    _systemCall(19, path, type, 0, 0);
}

void sys_removeFile(char * path) {
    _systemCall(20, path, 0, 0, 0);
}

void sys_cat(char * path) {
    _systemCall(21, path, 0, 0, 0);
}

void sys_writeTo(char * path, char * str) {
    _systemCall(22, path, str, 0, 0);
}

void sys_timerAppend(function f, unsigned long int ticks) {
    _systemCall(100, f, ticks, 0, 0);
}

void sys_timerRemove(function f) {
    _systemCall(101, f, 0, 0, 0);
}

void sys_beep(int nFrequence, unsigned char duration) {
    _systemCall(102, nFrequence, duration, 0, 0);
}

int sys_rtc(int fetch) {
    return _systemCall(200, fetch, 0, 0, 0);
}
