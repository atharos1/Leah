#include <stdarg.h> //Parámetros ilimitados
#include <stdint.h>
#include <drivers/console.h>
#include <drivers/kb_layout.h>
#include <drivers/video_vm.h>


#define NUMCOLORS 16
int fontSize = 1;
/*int num_Cols = SCREEN_WIDTH / CHAR_WIDTH;
int num_Rows = (SCREEN_HEIGHT / CHAR_HEIGHT);
int char_Height = CHAR_HEIGHT;
int char_Width = CHAR_WIDTH;*/

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
//enum COLOR backgroundColor = BLACK;
int backgroundColor = 0x0;
//enum COLOR fontColor = WHITE;
int fontColor = 0xFFFFFF;
short int cursorStatus = 0;

void _beep_start(uint16_t freq);
void _beep_stop();

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

void setGraphicCursorStatus(unsigned int status) {
	if( status == 1 )
		appendFunctionToTimer(cursorTick, 18);

	if( status == 0 )
		removeFunctionFromTimer(cursorTick);
}

void setFontSize(unsigned int size) {
	fontSize = size;
	char_Height = CHAR_HEIGHT * fontSize;
	char_Width = CHAR_WIDTH * fontSize;
	num_Cols = SCREEN_WIDTH / (CHAR_WIDTH * fontSize);
	num_Rows = SCREEN_HEIGHT / (CHAR_HEIGHT * fontSize);
}


uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

char getNextChar() {
	return 0;
}

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
	//printf("HOLA\n");


}

void shiftCursor(int cant) {
	int newPos = curScreenCol + curScreenRow*num_Cols + cant;
	if (newPos >= 0)
		setCursor(newPos%num_Cols, newPos/num_Cols);
}

void incLine(int cant) {

	if( curScreenRow + cant >= num_Rows ) {
		scrollUp(char_Height);
		cant--;
	}

	setCursor(0, curScreenRow + cant);

}

//Librería screen
static int scanNumber(char* source, int* dest, int* cantArgs) {
	int aux = 0;
	int counter = 0;

	if((*source) == ' ' && !isDigit(*source)) {
		while((*source++) == ' ' && !isDigit(*source));
	}
	if(isDigit(*source)) {
		*cantArgs++;
		while((*source) != '\0' && isDigit(*source)) {
			aux = (10^counter)*charToDigit(*source);
			counter++;
		}
		*dest = aux;
	}
	return counter;
}

static int scanString(char* source, char*dest, int* cantArgs) {
	int counter = 0;
	if((*source) == ' ' || (*source) == '\n') {
		while((*source++) == ' ' || (*source) == '\n');
	}
	if((*source) != '\0') {
		*cantArgs++;
	}
	while((*source) != ' ' && (*source) != '\0' && (*source) != '\n') {
		*dest = *source;
		dest++;
		source++;
		counter++;
	}
	(*dest) = '\0';
	return counter;
}

static int scanChar(char* source, char* dest, int* cantArgs) {
	if((*source) == ' ' || (*source) == '\n') {
		while((*source++) == ' ' || (*source) == '\n');
	}
	if((*source) == '\0' || (*source) == '\n') {
		return 0;
	}
	*dest = *source;
	*cantArgs++;
	return 1;
}

int vscanf(char* source, char* format, va_list pa) {
	int cantArgs = 0;
	while((*source) != '\0' && (*format) != '\0') {
		switch(*format) {
			case ' ':
				format++;
				break;
			case '%':
				format++;
				break;
			case 'd':
				source += scanNumber(source, va_arg(pa, int*), &cantArgs);
				format++;
				break;
			case 'c':
				source += scanChar(source, va_arg(pa, char*), &cantArgs);
				format++;
				break;
			case 's':
				source += scanString(source, va_arg(pa, char*), &cantArgs);
				format++;
				break;
		}
	}

	return cantArgs;

}

int sscanf(char* source, char* format, ...) {
	va_list pa;
	va_start(pa, format);
	int aux = vscanf(source, format, pa);
	va_end(pa);
	return aux;
}

int scanf(char* fmt, ...) {
	char c;
	char source[255];
	int i = 0;
	while((c = getChar()) != '\0') {
		source[i] = c;
		i++;
	}
	source[i] = '\0';

	va_list pa;
	va_start(pa, fmt);
	int ret = vscanf(source, fmt, pa);
	va_end(pa);
	return ret;
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



void printChar(char c) {
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
		/*case KLEFT:
			shiftCursor(-1);
			break;
		case KRIGHT:
			if(getNextChar != 0)
				shiftCursor(1);
			break;*/
		case BELL:
			_beep_start(1193);
 			break;
		default:
			drawChar(curScreenCol*char_Width, curScreenRow*char_Height, c, fontSize, fontColor, backgroundColor);
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
	clearDisplay(backgroundColor);
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

//Librería screen



//Librería math
int abs(int n) {
	return n * sign(n);
}

int sign(int n) {
	return (n < 0 ? -1 : 1);
}
