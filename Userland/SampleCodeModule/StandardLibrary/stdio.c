#include "stdio.h"
#include <stdarg.h> //Parámetros ilimitados
#include <stdint.h>
#include <limits.h>
#include "string.h"
#include "math.h"

#define STD_OUT 1
#define STD_IN 0
#define SYSCALL_WRITE 4

int charToDigit(char a);
int _read(int fileDescriptor, char * buffer, int count);
int _write(int fileDescriptor, char * buffer, int count);
void _halt();
void _clearScreen();
void _setBackgroundColor(enum COLOR backgroundColor, char * buffer, int count);
void _setFontColor(enum COLOR fontColor, char * buffer, int count);

extern uint64_t stackPointerBackup;

void printIntR(int i);

void clearScreen() {
	_clearScreen();
}

void setBackgroundColor(enum COLOR backgroundColor) {
	char c;
	_setBackgroundColor(backgroundColor, &c, 1);
}

void setFontColor(enum COLOR fontColor) {
	char c;
	_setFontColor(fontColor, &c, 1);
}

static char * itoa(uint64_t value, char * buffer, uint32_t base) {
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

	return buffer;
}

// void printInt(int i) {
// 	if(i < 0)
// 		putchar('-');
//
// 	printIntR(abs(i));
// }
//
// void printIntR(int i) {
//
// 	if( !( i/10 ) ) {
// 		putchar(i + '0');
// 		return;
// 	}
//
// 	printIntR(i/10);
//
// 	putchar( (i % 10) + '0');
//
// }

int isDigit(char c) {
    return (c >= '0' && c <= '9');
}

int charToDigit(char c) {
	return c - '0';
}

int isAlpha(char c) {
    return ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') );
}

int isAlphanumeric(char c) {
	return ( isAlpha(c) || isDigit(c) );
}
/*
int getString(char * str, char * buff, char limit)  {
    char c;
    int i;
    for( i = 0; c = str[i], c != limit; i++ ) {

        if( c == EOF  )
            return EOF; //ERROR

        buff[i] = c;

    }

    return i;

}

unsigned int getInt(char * str, char limit) {

    char c;
    char buff[INT_MAX];
    int i, num;

    int chars_int = 11;

    for(i = 0; i < chars_int ; i++) {
        c = str[i];
        if( c == EOF  )
            return EOF; //ERROR

        if( c == limit )
            break;

        buff[chars_int - i] = c;

    }

    if(c != limit) //OVERFLOW
        return EOF;

    for(int j = 0; j < i; j++)
        num += (buff[j + chars_int - (i + 1)] - '0') * pow(10, j);

    if( buff[chars_int] == '-' )
        num *= -1;
    else
        num += buff[chars_int] * pow(10, i);

    return num;
}*/


int getString(char * buff, char limit)  {
    char c;
    int i;
    for( i = 0; c = getchar(), c != limit; i++ ) {

        if( c == EOF  )
            return EOF; //ERROR

        buff[i] = c;

    }

    return i;

}

unsigned int getInt(char limit) {

    char c;
    char buff[INT_MAX];
    int i, num;

    int chars_int = 11;

    for(i = 0; i < chars_int ; i++) {
        c = getchar();
        if( c == EOF  )
            return EOF; //ERROR

        if( c == limit )
            break;

        buff[chars_int - i] = c;

    }

    if(c != limit) //OVERFLOW
        return EOF;

    for(int j = 0; j < i; j++)
        num += (buff[j + chars_int - (i + 1)] - '0') * pow(10, j);

    if( buff[chars_int] == '-' )
        num *= -1;
    else
        num += buff[chars_int] * pow(10, i);

    return num;
}

unsigned int getchar() {
    char c;
    if( _read(STD_OUT, &c, 1) == 0 )
        return EOF;

    return (unsigned int)c;
}

static int scanNumber(char* source, int* dest, int* cantArgs) {
	int aux = 0;
	int counter = 0;

	if((*source) == ' ' && !isDigit(*source)) {
		while((*source++) == ' ' && !isDigit(*source));
	}
	if(isDigit(*source)) {
		(*cantArgs)++;
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
		(*cantArgs)++;
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
	(*cantArgs)++;
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
	while((c = getchar()) != '\0') {
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

/*
int scanf(char * fmt, ...) {

    int rc = 0;

    va_list pa; //Lista de parámetros
    va_start(pa, fmt);
    char * format = fmt;

    while( *format != '\0' ) {

		if( *format != '%' ) {

			if( getchar() != *format )
                return ( rc > 0 ? rc : EOF );

			format++;
			continue;
		}

		format++;

		switch(*format) {
			case 'd':
				*va_arg(pa, int*) = getInt( *(format + 1) );
				break;
			case 'c':
				*va_arg(pa, char*) = getchar();
				break;
			case 's':
				getString( va_arg(pa, char*), *(format + 1) );
				break;
			case 'X':
				//read hexa
                break;
		}
        rc++;

		format++;

	}

	va_end(pa);

    return rc;

}

int sscanf(char * str, char * fmt, ...) {

    int rc = 0;

    va_list pa; //Lista de parámetros
    va_start(pa, fmt);
    char * format = fmt;

    while( *format != '\0' ) {

		if( *format != '%' ) {

			if( str != *format )
                return ( rc > 0 ? rc : EOF );

			str++;
			format++;
			continue;
		}

		format++;

		switch(*format) {
			case 'd':
				*va_arg(pa, int*) = getInt( *(format + 1) );
				break;
			case 'c':
				*va_arg(pa, char*) = getchar();
				break;
			case 's':
				getString( va_arg(pa, char*), *(format + 1) );
				break;
			case 'X':
				//read hexa
                break;
		}
        rc++;

		format++;

	}

	va_end(pa);

    return rc;

}
*/
unsigned int putchar(char c) {
    if( _write(STD_OUT, &c, 1) == 1 )
        return 1;

    return EOF;
}

int puts(char * str) {

	for(int i = 0; str[i] != '\0'; i++) {
		if( putchar( str[i] ) != 1 )
            return EOF;
	}


    return 1;

}

int printf(char * fmt, ...) {

	va_list pa; //Lista de parámetros
  va_start(pa, fmt);
  char * format = fmt;

  char buffer[255];
	char* tmp;
	int zeroes = 0;

	while( *format != '\0' ) {

		if( *format != '%' ) {

			putchar(*format);

			format++;
			continue;
		}

		format++;

		zeroes = 0;
		while ('0' <= *format && *format <= '9') {
			zeroes = zeroes * 10 + (*format - '0');
			format++;
		}

		switch(*format) {
			case 'd':
			case 'X':
				tmp = itoa(va_arg(pa, int), buffer, (*format=='d')?10:16);
				zeroes -= strlen(tmp);
				while (zeroes > 0) {
					putchar('0');
					zeroes--;
				}
				puts(tmp);
				break;
			case 'c':
				putchar( va_arg(pa, int) ); //NOTA: en parámatros ilimitados, char promociona a int SIEMPRE.
				break;
			case 's':
				puts( va_arg(pa, char*) );
				break;
		}

		format++;

	}

	va_end(pa);
    return 0;
}
