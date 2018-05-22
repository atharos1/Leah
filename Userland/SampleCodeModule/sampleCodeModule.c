#include <stdarg.h> //Parámetros ilimitados

#include "StandardLibrary/my_stdio.h"

static const enum COLOR {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE} COLOR;
#define NUMCOLORS 16
#define NUM_COLS 80
#define NUM_ROWS 25

//Librería math
int sign(int n) {
	return (n < 0 ? -1 : 1);
}
int abs(int n) {
	return n * sign(n);
}
//Librería math

/*int getchar(char* c) {
	_read(0, c, 1);
}*/

int _write(int fileDescriptor, char * buffer, int count);

int main() {

	/*	clearScreen();
	int a = 1;

	while(a == 1) {
		printf("hola");
	}

	char * c;

	
	while(a == 1) {
		setFontColor(MAGENTA);
	printf("%s\n%s. Numero: %d", "HOLA", "MUNDO!", -357);
	incLine(5);
	}*/

	//_halt();

	int n = 0xFD;

	char a[5] = "CHAU";

	printf("HOLA MUNDO! %X %s", n, a);

	//printf("Hola\nmundo!\nEl numero es %d", n);

	return 200;

}
