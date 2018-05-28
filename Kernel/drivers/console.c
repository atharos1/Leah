#include <stdarg.h> //Parámetros ilimitados
#include <stdint.h>
#include <drivers/console.h>
#include <drivers/kb_layout.h>
#include <drivers/video_vm.h>


#define NUMCOLORS 16
#define NUM_COLS 128
#define NUM_ROWS 96


//Librería screen
static char * firstScreenPos = (char*)0xB8000;
unsigned short int curScreenRow = 0;
unsigned short int curScreenCol = 0;
//enum COLOR backgroundColor = BLACK;
int backgroundColor = 0x0;
//enum COLOR fontColor = WHITE;
int fontColor = 0xFFFFFF;
short int cursorStatus = 0;

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

char getNextChar() {
	return 0;
}

void resetCursor() {
	if(cursorStatus == 0)
		cursorTick();
}

char * getCursorPos() {
	return firstScreenPos + (curScreenRow*160) + (curScreenCol*2);
}

void cursorTick() {
	invertChar(curScreenCol*CHAR_WIDTH, curScreenRow*CHAR_HEIGHT*NUM_COLS);
	// char * cursorPosColor = getCursorPos() + 1;
	// if( cursorStatus == 0 )
	// 	*cursorPosColor = WHITE + (16 * BLACK);
	// else
	// 	*cursorPosColor = BLACK + (16 * WHITE);
	//
	// cursorStatus = !cursorStatus;
}

void setCursor(unsigned short int x, unsigned short int y) {
	if( x >= NUM_COLS || y >= NUM_ROWS )
		return;

	curScreenCol = x;
	curScreenRow = y;

}

void shiftCursor(int cant) {
	int newPos = curScreenCol + curScreenRow*NUM_COLS + cant;
	if (newPos >= 0)
		setCursor(newPos%NUM_COLS, newPos/NUM_COLS);
}

void incLine(int cant) {
	setCursor(0, curScreenRow + cant);
}

//Librería screen
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

// void setFontColor(enum COLOR c) {
// 	if(c < NUMCOLORS) fontColor = c;
// 	char * pos = getCursorPos() + 1;
// 	*pos = fontColor + (16 * backgroundColor);
// }
// void setBackgroundColor(enum COLOR c) {
// 	if(c < NUMCOLORS) backgroundColor = c;
// 	char * pos = getCursorPos() + 1;
// 	*pos = fontColor + (16 * backgroundColor);
// }

void setFontColor(int color) {
	fontColor = color;
}
void setBackgroundColor(int color) {
	backgroundColor = color;
}

void printChar(char c) {
		switch(c) {
			case ENTER:
				resetCursor();
				incLine(1);
				break;
			case BACKSPACE:
				resetCursor();
				shiftCursor(-1);
				drawChar(curScreenCol*CHAR_WIDTH, curScreenRow*CHAR_HEIGHT, ' ', fontColor, backgroundColor);
				break;
			case KLEFT:
				shiftCursor(-1);
				break;
			case KRIGHT:
				if(getNextChar != 0)
					shiftCursor(1);
				break;
			default:
				drawChar(curScreenCol*CHAR_WIDTH, curScreenRow*CHAR_HEIGHT, c, fontColor, backgroundColor);
				shiftCursor(1);
				break;
		}
}

// void printChar(char c) {
//
// 	char * pos = getCursorPos();
//
// 	switch(c) {
// 		case ENTER:
// 			resetCursor();
// 			incLine(1);
// 			break;
// 		case BACKSPACE:
// 			resetCursor();
// 			shiftCursor(-1);
// 			pos = getCursorPos();
// 			*pos = 0;
// 			break;
// 		case KLEFT:
// 			shiftCursor(-1);
// 			break;
// 		case KRIGHT:
// 			if(getNextChar != 0)
// 				shiftCursor(1);
// 			break;
// 		default:
// 			*pos = c;
// 			pos++;
// 			*pos = fontColor + (16 * backgroundColor);
// 			shiftCursor(1);
// 			break;
// 	}
//
// }

void clearScreen() {
	clearDisplay();
	setCursor(0, 0);
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



static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
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

//Librería screen



//Librería math
int abs(int n) {
	return n * sign(n);
}

int sign(int n) {
	return (n < 0 ? -1 : 1);
}
//Librería math
