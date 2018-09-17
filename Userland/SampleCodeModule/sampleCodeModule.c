#include <stdarg.h> //Parámetros ilimitados

#include "StandardLibrary/stdio.h"
#include "StandardLibrary/string.h"
#include "programs/snake.h"
#include "programs/digitalClock.h"
#include "asm/asmLibC.h"

#define MAX_COMMANDS 255

unsigned int currFontColor = 0xFFFFFF;
unsigned int currBackColor = 0x000000;
unsigned int currFontSize = 1;

typedef void (*function)();

unsigned int programStatus = 0;

unsigned long commandsNum = 0;

typedef struct command {
	char name[30];
	char desc[300];
	function f;
} command;

struct command commandList[MAX_COMMANDS];

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

char hist[100][100];
unsigned int histCurrentIndex = 0;
unsigned int histAccessIndex = 0;
unsigned int histSize = 0;

void clearCmd(char cmd[]) {
	for(int i = 0; i < 100; i++) {
		cmd[i] = 0;
	}
}

void clearLine(unsigned int lineLong) {
	for (int i = 0; i < lineLong; i++) {
		printf("\b");
	}
}

void commandListener() {

	char c;
	char cmd[100];
	int cursor = 0;
	int lastChar = 0;

	setFontColor(0xFFA500);
	puts("Terminalator> ");
	setBackgroundColor(currBackColor);
	setFontColor(currFontColor);

	setGraphicCursorStatus(1);

	while(c = getchar(), c != '\n') {

		if( c != EOF ) {

			if (c == 8) { //backspace
						if(cursor > 0) {

							for(int i = cursor; i < lastChar; i++)
								cmd[i] = cmd[i + 1];

							cmd[lastChar] = '\0';
							lastChar--;
							cursor--;
							putchar(c);
						}
			} else if ( c == 1 || c == 2) { //up or down arrow
						if (c == 1) {
								if (histAccessIndex > 0) {
									histAccessIndex--;
								} else if (histSize > 0) {
									histAccessIndex = histSize - 1;
								}
						} else {
								if (histAccessIndex < (histSize - 1)) {
									histAccessIndex ++;
								} else {
									histAccessIndex = 0;
								}
						}
						clearLine(cursor);
						clearCmd(cmd);
						cursor = 0;
						while (hist[histAccessIndex][cursor] != 0) {
							cursor++;
							lastChar++;
						}
						strcpy(cmd,hist[histAccessIndex]);
						printf("%s",cmd);
			} else {
						if (c >= ' ' && c < 0x80) {
							cmd[cursor] = c;
							cursor++;
							lastChar++;
						}
						putchar(c);
			}
		}
	}

	setGraphicCursorStatus(0);

	setBackgroundColor(currBackColor);

	cmd[lastChar] = '\0';

	putchar('\n');

	if (histCurrentIndex >= 100) {
		histCurrentIndex = 0;
	}

	strcpy(hist[histCurrentIndex++], cmd);

	if (histSize != 100) {
		histSize ++;
	}

	histAccessIndex = histCurrentIndex;

	parseCommand(cmd, lastChar);
}

void invalidArgument(char * args) {
	printf("Argumento '%s' invalido", args);
}

void cmd_echo(char * args) {
	puts(args);
}

void cmd_time(char * args) {
	printf("Fecha y hora del sistema: %X/%X/%X %X:%X:%X", sys_rtc(7), sys_rtc(8), sys_rtc(9), sys_rtc(4), sys_rtc(2), sys_rtc(0));
}

void cmd_prueba() {
	printf("HOLA %5d \7", 1234);
}

void cmd_help() {
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

void cmd_exit() {
	printf("Finalizando shell...");
	programStatus = 1;
}

void cmd_printWelcome() {
	printf("Leah v0.1\nInterprete de comandos Terminalator. Digite 'help' para mas informacion.");
}

void cmd_resetScreen() {
	setFontColor(currFontColor);
	setBackgroundColor(currBackColor);
	setFontSize(currFontSize);
	clearScreen();
	cmd_printWelcome();
}

void cmd_setFontSize(char * args) {
	int num;

	sscanf(args, "%d", &num);

	if( num <= 0 ) {
		invalidArgument(args);
		return;
	}

	currFontSize = num;
	cmd_resetScreen();

}

void cmd_setBackColor(char * args) {
	int r, g, b;

	int leidos = sscanf(args, "%d %d %d", &r, &g, &b);

	if(leidos < 3) {
		invalidArgument(args);
		return;
	}

	int color = r * 256 * 256 + g * 256 + b;
	int cComplement = 0xFFFFFF - color;

	currFontColor = cComplement;
	currBackColor = color;
	cmd_resetScreen();

}

void cmd_Div100(char * args) {

	int num;
	int leidos = sscanf(args, "%d", &num);

	if( leidos <= 0 ) {
		invalidArgument(args);
		return;
	}

	int r = 100 / num;

	printf("100 / %d = %d", num, r);
}

void cmd_throwInvalidOpCode() {
	_throwInvalidOpCode();
}

void cmd_memoryManagerTest() {

	char c = 8;
	int bytes = 0;
	int cursor = 0;
	char notNum = 0;
	int blocksAllocated = 0;

	printf("\nPresione ESC para salir (todos los bloques seran liberados)\n\n");
	printf("Inserte numero de bytes para reservar: ");
	while(c = getchar(), c != 27 && blocksAllocated < 16) { //Esc

		if( c != '\n' ) {

				switch(c) {
						case 8: //backspace

								if(cursor > 0) {
									cursor--;
									putchar(c);
									if (cursor == notNum)
										notNum = 0;
								}
								break;

						default:

								if (c != -1) {
									if (isNumeric(c)) {
										bytes = c - '0' + bytes * 10;

									} else {
										if (notNum == 0)
											notNum = cursor;
									}
									cursor ++;
									putchar(c);
								}
								break;
				}

		} else {
				if (bytes != 0) {
					if (notNum == 0) {
						sys_memoryManagerTest(bytes);
						blocksAllocated ++;
						bytes = 0;
						if (blocksAllocated < 16)
								printf("Inserte numero de bytes para reservar: ");
					} else {
						printf("\nSolo se aceptan numeros!\n\n");
						printf("Inserte numero de bytes para reservar: ");
					}
					notNum = 0;
					cursor = 0;
				}
		}
	}
	if (blocksAllocated < 16)
		putchar('-');
	sys_memoryManagerTest(-1);
	printf("\n\n\n      Todos los bloques alocados fueron liberados\n");
}

void program_Snake(char * args) {

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

	cmd_resetScreen();

	printf("\n\n");

	if(puntos == -1) {
		printf("MY LITTLE BOA CONSTRICTOR: Has salido del juego.");
	} else {
		printf("MY LITTLE BOA CONSTRICTOR: ¡Has perdido! Tu puntuacion fue de %d puntos.", puntos);
	}

}

void cmd_listDir(char * args) {
	sys_listDir(args);
}

void cmd_makeDirectory(char * args) {
	sys_makeFile(args, 0);
}

void cmd_touch(char * args) {
	sys_makeFile(args, 1);
}

void cmd_removeFile(char * args) {
	sys_removeFile(args);
}

void program_digitalClock() {
	digitalClock();
	cmd_resetScreen();
}

int main() {

	while(1) {
		printf("hola\n");
		sys_sleep(500);
	}

	return 0;

	cmd_printWelcome();
	currBackColor = getBackgroundColor();
	currFontColor = getFontColor();
	currFontSize = getFontSize();

	puts("\n\n");

	command_register("echo", cmd_echo, "Imprime una cadena de caracteres en pantalla. Argumentos: [cadena]");
	command_register("time", cmd_time, "Muentra la fecha y hora del reloj del sistema");
	command_register("help", cmd_help, "Despliega informacion sobre los comandos disponibles");
	command_register("prueba", cmd_prueba, "Comando de prueba");
	command_register("clear", cmd_resetScreen, "Limpia la pantalla");
	command_register("font-size", cmd_setFontSize, "Establece el tamano de la fuente y limpia la consola");
	command_register("digital-clock", program_digitalClock, "Muestra un reloj digital en pantalla");
	command_register("div100", cmd_Div100, "Divide 100 por el valor especificado (Prueba ex0). Argumentos: [*divisor]");
	command_register("invopcode", cmd_throwInvalidOpCode, "Salta a la posicion de memoria 27h, provocando una excepcion InvalidOpCode");
	command_register("exit", cmd_exit, "Cierra la Shell");
	command_register("snake", program_Snake, "Juego Snake. Se juega con WASD. Argumentos: [*ticks por movimiento, *ratio de crecimiento]");
	command_register("back-color", cmd_setBackColor, "Cambia el color de fondo e invierte el color de fuente adecuadamente. Argumentos: *[R G B]");
	command_register("test-memory-manager", cmd_memoryManagerTest, "Realiza alocaciones de memoria y muestra el mapa en pantalla");
	command_register("ls", cmd_listDir, "Lista los archivos en el directorio especificado");
	command_register("mkdir", cmd_makeDirectory, "Crea un directorio en la ruta especificada");
	command_register("touch", cmd_touch, "Crea un archivo regular en la ruta especificada");
	command_register("rm", cmd_removeFile, "Elimina el archivo especificado");

	while(programStatus != 1) {
		commandListener();
	}

	return 0;

}
