#include <stdarg.h> //Parámetros ilimitados

static const enum COLOR {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE} COLOR;
#define NUMCOLORS 16
#define NUM_COLS 80
#define NUM_ROWS 25


//Librería screen
static char * firstScreenPos = (char*)0xB8000;
short int curScreenRow = 0;
short int curScreenCol = 0;
enum COLOR backgroundColor = BLACK;
enum COLOR fontColor = WHITE;


//Librería screen
void printf(char * format, ...) {

	va_list pa; //Lista de parámetros
    va_start(pa, format);

	while( *format != '\0' ) {

		if( *format != '%' ) {

			if(*format == '\n')
				incLine(1);
			else
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
		}

		format++;

	}

	va_end(pa);
}

void setFontColor(enum COLOR c) {
	if(c < NUMCOLORS) fontColor = c;
}
void setBackgroundColor(enum COLOR c) {
	if(c < NUMCOLORS) backgroundColor = c;
}

void incLine(int cant) {
	if(curScreenRow + cant > NUM_ROWS || curScreenRow + cant < 0) return;
	curScreenRow += cant;
	curScreenCol = 0;
}

void printChar(char c) {
	char * pos = firstScreenPos + (curScreenRow*160) + (curScreenCol*2);
	*pos = c;
	pos++;
	*pos = fontColor + (16 * backgroundColor);
	curScreenCol += 1;

	if (curScreenCol >= NUM_COLS)
		incLine(1);
}

void clearScreen() {
	char * pos = firstScreenPos;
	for(int i = 0; i < 25 * 80; i++) {
		*pos = 0;
		pos++;
		*pos = 0;
		pos++;
	}
}

void printString(char * str) {

	for(int i = 0; str[i] != '\0'; i++) {
		if(str[i] == '\n')
			incLine(1);
		else
			printChar(str[i]);
	}

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