#include "my_stdio.h"
#include <stdarg.h> //Parámetros ilimitados
#include <stdint.h>
#include <limits.h>

#define STD_OUT 1
#define STD_IN 0 
#define SYSCALL_WRITE 4


int _read(int fileDescriptor, char * buffer, int count);
int _write(int fileDescriptor, char * buffer, int count);
void _halt();
void _clearScreen();

void clearScreen() {
	_clearScreen();
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

void printInt(int i) {
	if(i < 0)
		putchar('-');
	
	printIntR(abs(i));
}

void printIntR(int i) {

	if( !( i/10 ) ) {
		putchar(i + '0');
		return;
	}
	
	printIntR(i/10);

	putchar( (i % 10) + '0');

}

int isDigit(char c) {
    return (c >= '0' && c <= '9');
}

int isAlpha(char c) {
    return ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') );
}

int isAlphanumeric(char c) {
	return ( isAlpha(c) || isDigit(c) );
}

long long int pow(int base, int exp) {
    if( exp == 0 )
        return 1;

    int num = base;

    for(int i = 2; i <= exp; i++)
        num *= base;

    return num;

}

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
				getString( *va_arg(pa, char*), *(format + 1) );
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

	while( *format != '\0' ) {

		if( *format != '%' ) {

			putchar(*format);

			format++;
			continue;
		}

		format++;

		switch(*format) {
			case 'd':
				printInt(va_arg(pa, int));
				break;
			case 'c':
				putchar( va_arg(pa, int) ); //NOTA: en parámatros ilimitados, char promociona a int SIEMPRE.
				break;
			case 's':
				puts( va_arg(pa, char*) );
				break;
			case 'X':
				puts(  itoa(va_arg(pa, int), buffer, 16)  );
		}

		format++;

	}

	va_end(pa);
    return 0;
}