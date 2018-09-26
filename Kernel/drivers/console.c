#include <stdarg.h> //Parámetros ilimitados
#include <stdint.h>
#include <drivers/console.h>
#include <drivers/video_vm.h>
#include <drivers/speaker.h>
#include <drivers/kb_driver.h>
#include <drivers/kb_layout.h>
#include <drivers/timer.h>
#include <asm/libasm.h>

void cursorTick();
int abs(int n);
int sign(int n);
void printInt(int i);

int fontSize = 1;
int num_Cols = 128;
int num_Rows = 48;
int char_Height = CHAR_HEIGHT;
int char_Width = CHAR_WIDTH;

#define isDigit(a) ('0'<=a && a<='9')
#define charToDigit(a) (a - '0')

//Librería screen
static char * firstScreenPos = (char*)0xB8000;
unsigned short int curScreenRow = 0;
unsigned short int curScreenCol = 0;

int backgroundColor = 0x000001;
int fontColor = 0xFFFFFF;
short int cursorStatus = 0;

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void setGraphicCursorStatus(unsigned int status) {
	if( status == 1 )
		timer_appendFunction(cursorTick, 18);

	if( status == 0 )
		timer_removeFunction(cursorTick);
}

void setFontSize(unsigned int size) {
	fontSize = size;
	char_Height = CHAR_HEIGHT * fontSize;
	char_Width = CHAR_WIDTH * fontSize;
	num_Cols = SCREEN_WIDTH / (CHAR_WIDTH * fontSize);
	num_Rows = SCREEN_HEIGHT / (CHAR_HEIGHT * fontSize);
}

void setFontColor(int color) {
	fontColor = color;
}
void setBackgroundColor(int color) {
	backgroundColor = color;
}

int getFontColor() {
	return fontColor;
}

int getBackgroundColor() {
	return backgroundColor;
}

int getFontSize() {
	return fontSize;
}


uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void resetCursor() {
	if(cursorStatus == 1)
		cursorTick();
}

char * getCursorPos() {
	return firstScreenPos + (curScreenRow*160) + (curScreenCol*2);
}

void cursorTick() {

	if(cursorStatus == 0)
		drawRectangle(curScreenCol*char_Width, curScreenRow*char_Height, char_Width, char_Height, fontColor);
	else
		drawRectangle(curScreenCol*char_Width, curScreenRow*char_Height, char_Width, char_Height, backgroundColor);

	cursorStatus = !cursorStatus;
}


void setCursor(unsigned short int x, unsigned short int y) {
	if( x >= num_Cols || y >= num_Rows )
		return;

	curScreenCol = x;
	curScreenRow = y;
}

void shiftCursor(int cant) {
	int newPos = curScreenCol + curScreenRow*num_Cols + cant;
	if (newPos >= 0)
		setCursor(newPos%num_Cols, newPos/num_Cols);
}

void incLine(int cant) {

	if( curScreenRow + cant >= num_Rows ) {
		scrollUp(char_Height, backgroundColor);
		cant--;
	}

	setCursor(0, curScreenRow + cant);

}

void printChar(char c) {
	int cantSpaces = 0;
	switch(c) {
		case ENTER:
			resetCursor();
			incLine(1);
			break;
		case BACKSPACE:
			resetCursor();
			shiftCursor(-1);
			drawChar(curScreenCol*char_Width, curScreenRow*char_Height, ' ', fontSize, fontColor, backgroundColor);
			break;
		case BELL:
			beep(1000, 3);
 			break;
		case TAB:
			cantSpaces = curScreenCol % TAB_WIDTH;
			if(cantSpaces == 0) cantSpaces = TAB_WIDTH;
			for(int i = 0; i < cantSpaces; i++)
				printChar(' ');
			break;
		default:
			if (c < ' ' || c >= 0x80)
				break;
			drawChar(curScreenCol*char_Width, curScreenRow*char_Height, c, fontSize, fontColor, backgroundColor);
			shiftCursor(1);
			break;
	}
}

void clearScreen() {
	clearDisplay(backgroundColor);
	setCursor(0, 0);
}

void printf(char * format, ...) {
	va_list pa; //Lista de parámetros
    va_start(pa, format);

	while( *format != '\0' ) {

		if( *format != '%' ) {

			printChar(*format);

			format++;
			continue;
		}

		format++;

		switch(*format) {
			case 'd':
				printInt( va_arg(pa, int) );
				break;
			case 'c':
				printChar( va_arg(pa, int) ); //NOTA: en parámatros ilimitados, char promociona a int SIEMPRE.
				break;
			case 's':
				printString( va_arg(pa, char*) );
				break;
			case 'X':
				printBase(va_arg(pa, int), 16);
		}

		format++;

	}

	va_end(pa);
}

void printString(char * str) {
	for(int i = 0; str[i] != '\0'; i++)
		printChar(str[i]);
}

void printIntR(int i) {

	if( !( i/10 ) ) {
		printChar(i + '0');
		return;
	}

	printIntR(i/10);
	printChar( (i % 10) + '0');
}

void printInt(int i) {
	if(i < 0)
		printChar('-');

	printIntR(abs(i));
}

void printBase(int i, int base) {
    char buffer[64] = { '0' };
    uintToBase(i, buffer, base);
    printString(buffer);
}


uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}


//Librería math
int abs(int n) {
	return n * sign(n);
}

int sign(int n) {
	return (n < 0 ? -1 : 1);
}
