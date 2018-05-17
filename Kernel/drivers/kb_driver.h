#ifndef KB_DRIVER_H
#define KB_DRIVER_H

#define EOF -1
char kb_fetch();
int8_t getChar();
unsigned char peekChar();
void ungetc(unsigned char c);

#endif
