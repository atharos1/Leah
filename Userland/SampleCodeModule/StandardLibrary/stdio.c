#include "include/stdio.h"
#include <stdarg.h> //Parámetros ilimitados
#include <stdint.h>
#include <limits.h>
#include "include/string.h"
#include "include/math.h"
#include "../asm/asmLibC.h"

#define STD_OUT 1
#define STD_IN 0
#define SYSCALL_WRITE 4

int charToDigit(char a);

extern uint64_t stackPointerBackup;

void printIntR(int i);

void clearScreen()
{
	sys_clearScreen();
}

void setBackgroundColor(unsigned int color)
{
	sys_setBackgroundColor(color);
}

unsigned int getBackgroundColor()
{
	return sys_getBackgroundColor();
}

void setFontColor(unsigned int color)
{
	sys_setFontColor(color);
}

unsigned int getFontColor()
{
	return sys_getFontColor();
}

int setFontSize(unsigned int size)
{
	return sys_setFontSize(size);
}

int getFontSize()
{
	return sys_getFontSize();
}

void setGraphicCursorStatus(unsigned int status)
{
	if (status != 0 && status != 1)
		return;

	sys_setGraphicCursorStatus(status);
}

int setCursor(unsigned int x, unsigned int y)
{
	return sys_setCursor(x, y);
}

char *itoa(uint64_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

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

int isDigit(char c)
{
	return (c >= '0' && c <= '9');
}

int charToDigit(char c)
{
	return c - '0';
}

int isAlpha(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isAlphanumeric(char c)
{
	return (isAlpha(c) || isDigit(c));
}

int getString(char *buff, char limit)
{
	char c;
	int i;
	for (i = 0; c = getchar(), c != limit; i++)
	{

		if (c == EOF)
			return EOF; //ERROR

		buff[i] = c;
	}

	return i;
}

unsigned int getInt(char limit)
{

	char c;
	char buff[INT_MAX];
	int i, num = 0;

	int chars_int = 11;

	for (i = 0; i < chars_int; i++)
	{
		c = getchar();
		if (c == EOF)
			return EOF; //ERROR

		if (c == limit)
			break;

		buff[chars_int - i] = c;
	}

	if (c != limit) //OVERFLOW
		return EOF;

	for (int j = 0; j < i; j++)
		num += (buff[j + chars_int - (i + 1)] - '0') * pow(10, j);

	if (buff[chars_int] == '-')
		num *= -1;
	else
		num += buff[chars_int] * pow(10, i);

	return num;
}

unsigned int getchar()
{
	char c;
	if (sys_read(STD_OUT, &c, 1) == 0)
		return EOF;

	return (unsigned int)c;
}

static int scanNumber(char *source, int *dest, int *cantArgs)
{
	int aux = 0;
	int counter1 = 0;
	int counter2 = 0;

	if (!isDigit(*source))
	{
		while (!isDigit(*(source + counter1)))
		{
			counter1++;
		}
	}
	if (isDigit(*(source + counter1)))
	{
		(*cantArgs)++;
		while (isDigit(*(source + counter1)))
		{
			counter2++;
			counter1++;
		}
		for (int i = 0; i < counter2; i++)
		{
			aux = aux + (pow(10, i)) * charToDigit(*(source + counter1 - i - 1));
		}
		*dest = aux;
	}
	return counter1;
}

static int scanString(char *source, char *dest, int *cantArgs)
{
	int counter = 0;
	if ((*source) == ' ' || (*source) == '\n')
	{
		while ((*source++) == ' ' || (*source) == '\n')
		{
			counter++;
		}
	}
	if ((*source) != '\0')
	{
		(*cantArgs)++;
	}
	while ((*source) != ' ' && (*source) != '\0' && (*source) != '\n')
	{
		*dest = *source;
		dest++;
		source++;
		counter++;
	}
	(*dest) = '\0';
	return counter;
}

static int scanChar(char *source, char *dest, int *cantArgs)
{
	if ((*source) == ' ' || (*source) == '\n')
	{
		while ((*source++) == ' ' || (*source) == '\n')
			;
	}
	if ((*source) == '\0' || (*source) == '\n')
	{
		return 0;
	}
	*dest = *source;
	(*cantArgs)++;
	return 1;
}

int vscanf(char *source, char *format, va_list pa)
{
	int cantArgs = 0;
	while ((*source) != '\0' && (*format) != '\0')
	{
		switch (*format)
		{
		case ' ':
			format++;
			break;
		case '%':
			format++;
			break;
		case 'd':
			source += scanNumber(source, va_arg(pa, int *), &cantArgs);
			format++;
			break;
		case 'c':
			source += scanChar(source, va_arg(pa, char *), &cantArgs);
			format++;
			break;
		case 's':
			source += scanString(source, va_arg(pa, char *), &cantArgs);
			format++;
			break;
		}
	}

	return cantArgs;
}

int sscanf(char *source, char *format, ...)
{
	va_list pa;
	va_start(pa, format);
	int aux = vscanf(source, format, pa);
	va_end(pa);
	return aux;
}

unsigned int putchar(char c)
{
	if (sys_write(STD_OUT, &c, 1) == 1)
		return 1;

	return EOF;
}

int puts(char *str)
{

	if (sys_write(STD_OUT, str, strlen(str) + 1) == 1)
		return 1;

	return EOF;
}

int printf(char *fmt, ...)
{

	va_list pa; //Lista de parámetros
	va_start(pa, fmt);
	char *format = fmt;

	char buffer[255];

	char string[255]; //Verificamos si se nos fue la mano?
	int curChar = 0;

	char *tmp;
	int zeroes = 0;

	while (*format != '\0')
	{

		if (*format != '%')
		{

			//putchar(*format);
			string[curChar] = *format;
			curChar++;

			format++;
			continue;
		}

		format++;

		zeroes = 0;
		while ('0' <= *format && *format <= '9')
		{
			zeroes = zeroes * 10 + (*format - '0');
			format++;
		}

		switch (*format)
		{
		case 'd':
		case 'X':
			tmp = itoa(va_arg(pa, int), buffer, (*format == 'd') ? 10 : 16);
			zeroes -= strlen(tmp);
			while (zeroes > 0)
			{
				string[curChar] = '0';
				curChar++;
				//putchar('0');
				zeroes--;
			}
			//puts(tmp);

			strcpy(&string[curChar], tmp);
			curChar += strlen(tmp);
			break;
		case 'c':
			//putchar( va_arg(pa, int) ); //NOTA: en parámatros ilimitados, char promociona a int SIEMPRE.
			string[curChar] = va_arg(pa, int);
			curChar++;
			break;
		case 's':
			curChar += 0;
			tmp = va_arg(pa, char *);

			alignString(tmp, buffer, zeroes);

			strcpy(&string[curChar], buffer);
			curChar += strlen(buffer);

			break;
		}

		format++;
	}

	string[curChar] = 0;
	puts(string);

	va_end(pa);
	return 0;
}
