#include <stdarg.h> //Parámetros ilimitados

#include "StandardLibrary/stdio.h"
#include "StandardLibrary/string.h"
#include "programs/snake.h"
#include "programs/digitalClock.h"

#define MAX_COMMANDS 255


//VER SI SE USA O NO
#define HISTORY_LENGHT 20
#define COMMAND_LENGHT 255

unsigned int currFontColor = 0xFFFFFF;
unsigned int currBackColor = 0x000000;
unsigned int currFontSize = 1;


int lastHistoryPos = 0;

void invalidArgument(char * args) {
	printf("Argumento '%s' invalido", args);
}

void echo(char * args) {
	printf(args);
}

char comandHistory[HISTORY_LENGHT][COMMAND_LENGHT];

typedef void (*function)();
int _timerAppend(function f, unsigned long int ticks);
int _timerRemove(function f);
int _setFontSize(unsigned int size);
void _setGraphicCursorStatus(unsigned int status);
void _throwInvalidOpCode();
void _setBackgroundColor(unsigned int color);

int _rtc(int fetch);

int _write(int fileDescriptor, char * buffer, int count);

unsigned long commandsNum = 0;

typedef struct command {
	char name[30];
	char desc[300];
	function f;
} command;

struct command commandList[MAX_COMMANDS];

int cursorStatus = 0;

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

int command_register(char * name, function f, char * desc) {
	if(commandsNum >= MAX_COMMANDS - 1 || getCommandFunction(name) )
		return -1;

	strcpy(commandList[commandsNum].name, name);
	strcpy(commandList[commandsNum].desc, desc);
	commandList[commandsNum].f = f;

	commandsNum++;

	return 0;
}

// void cursorTick() {
// 	if(!cursorStatus)
// 		setBackgroundColor(WHITE);
// 	else
// 		setBackgroundColor(BLACK);
//
// 	cursorStatus = !cursorStatus;
// }

int command_unregister(char * name) {

	int id = getCommandID(name);

	if(commandsNum == 0 || id == -1 )
		return -1;

	for(int i = id; i < commandsNum - 1; i++) {
		strcpy(commandList[i].name, commandList[i + 1].name);
		commandList[i].f = commandList[i].f;
	}

	commandsNum--;

	return 0;
}

void prueba() {
	printf("HOLA %5d \7", 1234);
}

int parseCommand(char * cmd, int l) {

	if( *cmd == '\0' )
		return 1;

	char * args;
	int i;
	for(i = 0; i < l; i++) {
		if( cmd[i] == ' ' ) {
			args = &cmd[i] + sizeof(char);
			cmd[i] = 0;
			break;
		}
	}

	function f = getCommandFunction(cmd);
	if( f == 0 ) {
		printf("Comando '%s' desconocido.\n\n", cmd);
		return -1;
	}

	if(i < l)
		f(args);
	else
		f("");

	puts("\n\n");

	return 0;
}

int commandListener() {

	char c;
	char cmd[100];
	int cursor = 0;
	int lastChar = 0;

	setFontColor(0xFFA500);
	puts("Terminalator> ");
	setBackgroundColor(currBackColor);
	setFontColor(currFontColor);

	_setGraphicCursorStatus(1);

	while(c = getchar(), c != '\n') {

		if( c != EOF ) {

			switch(c) {
				case 8: //backspace
					if(cursor > 0) {

						for(int i = cursor; i < lastChar; i++)
							cmd[i] = cmd[i + 1];

						cmd[lastChar] = '\0';
						lastChar--;
						cursor--;
						putchar(c);
					}
					break;
				default:
					if (c >= ' ' && c < 0x80) {
						cmd[cursor] = c;
						cursor++;
						lastChar++;
					}
					putchar(c);
					break;
			}
		}
	}

	_setGraphicCursorStatus(0);

	setBackgroundColor(currBackColor);
	//_timerRemove(cursorTick);

	cmd[lastChar] = '\0';

	putchar('\n');

	if(strcmp(cmd, "exit") == 0)
		return 1;

	parseCommand(cmd, lastChar);
	return 0;

}

void time(char * args) {
	printf("Fecha y hora del sistema: %X/%X/%X %X:%X:%X", _rtc(7), _rtc(8), _rtc(9), _rtc(4), _rtc(2), _rtc(0));
}

void help() {
	for(int i = 0; i < commandsNum; i++) {
		setFontColor(0xFF6347);
		printf("%s\n", commandList[i].name);
		setFontColor(currFontColor);
			if(commandList[i].desc != '\0') {
				printf("  %s", commandList[i].desc);
				if( i < commandsNum - 1)
					putchar('\n');
			}
	}
}

void exit() {
	return;
}

void printWelcome() {
	printf("Leah v0.1\nInterprete de comandos Terminalator. Digite 'help' para mas informacion.");
}

void clear() {
	setFontColor(currFontColor);
	setBackgroundColor(currBackColor);
	_setFontSize(currFontSize);
	clearScreen();
	printWelcome();
}

void setFontSize(char * args) {
	int num;

	sscanf(args, "%d", &num);

	if( num <= 0 ) {
		invalidArgument(args);
		return;
	}

	currFontSize = num;
	clear();

}

void setBackColor(char * args) {
	int r, g, b;

	sscanf(args, "%d %d %d", &r, &g, &b);

	int color = r * 256 * 256 + g * 256 + b;
	int cComplement = 0xFFFFFF - color;

	currFontColor = cComplement;
	currBackColor = color;
	clear();

}

void digitalClock_exec() {
	digitalClock();
	_setFontSize(1);
	clear();
}

// void beep() {
// 	digitalClock();
// 	_setFontSize(1);
// 	clear();
// }

int _checkStack();
void _pushA();

void div100(char * args) {

	int num;
	int leidos = sscanf(args, "%d", &num);

	if( leidos <= 0 ) {
		invalidArgument(args);
		return;
	}

	int r = 100 / num;

	printf("100 / %d = %d", num, r);
}

void play_snake(char * args) {

	int num, grow_rate;

	int leidos = sscanf(args, "%d %d", &num, &grow_rate);

	if(leidos == 0) {
		num = 2;
		grow_rate = 3;
	}

	if(leidos == 1) {
		grow_rate = 3;
	}

	int puntos = game_start(num, grow_rate);

	clear();

	printf("\n\n");

	if(puntos == -1) {
		printf("Has salido del juego");
	} else {
		printf("¡Has perdido! Tu puntuacion: %d", puntos);
	}

}

void throwInvalidOpCode() {
	_throwInvalidOpCode();
}

int main() {

	printWelcome();

	//div100("0\0");

	puts("\n\n");

	//_enableCursor();

	command_register("echo", echo, "Imprime una cadena de caracteres en pantalla");
	command_register("time", time, "Muentra la fecha y hora del reloj del sistema");
	command_register("help", help, "Despliega informacion sobre los comandos disponibles");
	command_register("prueba", prueba, "Comando de prueba");
	command_register("clear", clear, "Limpia la pantalla");
	command_register("font-size", setFontSize, "Establece el tamano de la fuente y limpia la consola");
	command_register("digital-clock", digitalClock_exec, "Muestra un reloj digital en pantalla");
	command_register("div100", div100, "Divide 100 por el valor especificado (Prueba ex0). Parametros: [divisor]");
	command_register("invopcode", throwInvalidOpCode, "Salta a la posicion de memoria 27h, provocando una excepcion InvalidOpCode");
	command_register("exit", exit, "Cierra la Shell");
	command_register("snake", play_snake, "Juego Snake. Se juega con WASD. Parametros: [*ticks por movimiento, *ratio de crecimiento]");
	command_register("back-color", setBackColor, "Cambia el color de fondo e invierte el color de fuente adecuadamente. Parametros: *[R G B]");

	int status = 0;
	while(status != 1) {
		status = commandListener();
	}

	return 0;

}
