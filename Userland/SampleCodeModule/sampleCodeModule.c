#include <stdarg.h> //Parámetros ilimitados

#include "StandardLibrary/my_stdio.h"

static const enum COLOR {BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GRAY, DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE} COLOR;
#define NUMCOLORS 16


#define MAX_COMMANDS 255

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

void echo(char * args) {
	printf(args);
}

typedef (*function)();
int _timerAppend(function f, unsigned long int ticks);
int _timerRemove(function f);

int _write(int fileDescriptor, char * buffer, int count);

unsigned long commandsNum = 0;

typedef struct command {
	char name[30];
	function f;
};

struct command commandList[MAX_COMMANDS];

char * strcpy(char * destination, char * origin) {
	int i = 0;
    while ((destination[i] = origin[i]) != '\0')
    {
        i++;
    } 
	return destination;
}

function getCommandFunction(char * commandName) {
	for(int i = 0; i < commandsNum; i++)
		if( strcmp(commandList[i].name, commandName) == 0 )
			return commandList[i].f;

	return 0;
}

long getCommandID(char * commandName) {
	for(int i = 0; i < commandsNum; i++)
		if( strcmp(commandList[i].name, commandName) == 0 )
			return i;

	return -1;
}

int command_register(char * name, function f) {
	if(commandsNum >= MAX_COMMANDS - 1 || getCommandFunction(name) )
		return -1;
	
	strcpy(commandList[commandsNum].name, name);
	commandList[commandsNum].f = f;

	commandsNum++;

	return 0;
}

/*int command_unregister(char * name) {

	int id = getCommandID(name);

	if(commandsNum == 0 || id == -1 )
		return -1;

	for(int i = id; i < commandsNum - 1; i++) {
		strcpy(commandList[i].name, commandList[i + 1].name);
		commandList[i].f = commandList[i].f;
	}

	commandsNum--;

	return 0;
}*/

void prueba() {
	printf("HOLA");
}

int strcmp(char * str1, char * str2) {
	int s1;
    int s2;
    do {
        s1 = *str1++;
        s2 = *str2++;
        if (s1 == 0)
            break;
    } while (s1 == s2);
    return (s1 < s2) ? -1 : (s1 > s2);	
}

int parseCommand(char * cmd, int l) {
	char * args;
	for(int i = 0; i < l; i++) {
		if( cmd[i] == ' ' ) {
			args = &cmd[i] + sizeof(char);
			cmd[i] = 0;
			break;
		}
	}

	function f = getCommandFunction(cmd);
	if( f == 0 ) {
		printf("Comando '%s' desconocido.\n", cmd);
		return -1;
	}
	
	f(args);

	printf("\n");
			
	return 0;
}

int commandListener() {
	char c;
	char cmd[255];
	int cursor = 0;
	int lastChar = 0;

	while(c = getchar(), c != '\n') {
		if( c != EOF ) {

			switch(c) {
				case 8: //backspace
					if(cursor != 0) {

						for(int i = cursor; i < lastChar; i++)
							cmd[i] = cmd[i + 1];

						cmd[lastChar] = '\0';
						lastChar--;
						cursor--;
					}
					break;
				default:
					cmd[cursor] = c;
					cursor++;
					lastChar++;
					break;
			}

			putchar(c);
		}
	}

	cmd[++lastChar] = '\0';

	putchar('\n');

	if(strcmp(cmd, "exit") == 0)
		return 1;

	parseCommand(cmd, lastChar);
	return 0;

}

int main() {

	clearScreen();
	//_timerAppend(prueba, 20);

	command_register("prueba", prueba);
	command_register("echo", echo);

	int status = 0;
	while(status != 1) {
		status = commandListener();
	}

	


	return 300;

}
