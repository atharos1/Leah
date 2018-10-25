#include <stdarg.h> //Parámetros ilimitados

#include "StandardLibrary/include/linkedList.h"
#include "StandardLibrary/include/mutex.h"
#include "StandardLibrary/include/pthread.h"
#include "StandardLibrary/include/sem.h"
#include "StandardLibrary/include/stdio.h"
#include "StandardLibrary/include/stdlib.h"
#include "StandardLibrary/include/string.h"
#include "StandardLibrary/include/timer.h"
#include "asm/asmLibC.h"
#include "programs/include/digitalClock.h"
#include "programs/include/philosophers.h"
#include "programs/include/prodCons.h"
#include "programs/include/snake.h"
#include "programs/include/toUppercase.h"
#include "programs/include/upDown.h"

#define MAX_COMMANDS 255
#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 50

#define MAX_FUNCTION_IN_COMMAND 10

#define MAX_COMMAND_NAME_LENGTH 30

#define FALSE 0
#define TRUE 1

unsigned int currFontColor = 0xFFFFFF;
unsigned int currBackColor = 0x000000;
unsigned int currFontSize = 1;

typedef void (*function)();
// typedef int (*programCode)(char**args);

void cmd_resetScreen();
void cmd_printWelcome();

unsigned int programStatus = 0;
unsigned long commandsNum = 0;

typedef struct command
{
    char name[MAX_COMMAND_NAME_LENGTH];
    char desc[300];
    function f;
    int isProgram;
    int isFullscreen;
} command;

struct command commandList[MAX_COMMANDS];

function getCommandFunction(char *commandName)
{
    for (int i = 0; i < commandsNum; i++)
        if (strcmp(commandList[i].name, commandName) == 0)
            return commandList[i].f;

    return NULL;
}

int getIsFullscreen(char *commandName)
{
    for (int i = 0; i < commandsNum; i++)
        if (strcmp(commandList[i].name, commandName) == 0)
            return commandList[i].isFullscreen;

    return -1;
}

int getIsProgram(char *commandName)
{
    for (int i = 0; i < commandsNum; i++)
    {
        if (strcmp(commandList[i].name, commandName) == 0)
            return commandList[i].isProgram;
    }
    return -1;
}

long getCommandID(char *commandName)
{
    for (int i = 0; i < commandsNum; i++)
        if (strcmp(commandList[i].name, commandName) == 0)
            return i;

    return -1;
}

int command_register(char *name, function f, char *desc, int isProgram,
                     int isFullscreen)
{
    if (commandsNum >= MAX_COMMANDS - 1 || getCommandFunction(name))
        return -1;

    strcpy(commandList[commandsNum].name, name);
    strcpy(commandList[commandsNum].desc, desc);
    commandList[commandsNum].f = f;
    commandList[commandsNum].isProgram = isProgram;
    commandList[commandsNum].isFullscreen = isFullscreen;

    commandsNum++;

    return 0;
}

void execProgram(char *cmd, char **args, int argn)
{
    int giveAwayForeground = 1;

    int isFullscreen = getIsFullscreen(cmd);

    if (argn > 0 && strcmp(args[argn - 1], "&") == 0)
    {
        giveAwayForeground = 0;
    }

    if (!giveAwayForeground && isFullscreen)
    {
        printf(
            "No puede ejecutar en segundo plano un programa de pantalla "
            "completa.");

        return;
    }

    function f = getCommandFunction(cmd);

    int pid = execv(cmd, (process_t)f, args, TRUE, NULL);

    if (giveAwayForeground)
    {
        sys_setForeground(pid);

        sys_waitPID(pid);

        if (isFullscreen)
            cmd_resetScreen();
    }
}

int command_unregister(char *name)
{
    int id = getCommandID(name);

    if (commandsNum == 0 || id == -1)
        return -1;

    for (int i = id; i < commandsNum - 1; i++)
    {
        strcpy(commandList[i].name, commandList[i + 1].name);
        commandList[i].f = commandList[i + 1].f;
    }

    commandsNum--;

    return 0;
}

int str_is_whitespace_only(char *str)
{
    for (int i = 0; str[i] != 0; i++)
        if (str[i] != ' ' && str[i] != '\t')
            return FALSE;

    return TRUE;
}

int parseCommands(char *cmd, int cmdLength, char *cmdList[], int commandLimit)
{
    int i = 0;
    int lastCommand = 0;
    int foundPipe = TRUE;
    int quoteEnabled = FALSE;
    int isEscaped = FALSE;

    while (i < cmdLength && lastCommand < commandLimit)
    {
        while (cmd[i] == ' ' && !quoteEnabled)
            i++;

        if (foundPipe)
        {
            if (cmd[i] == '|')
            {
                printf("Error de sintaxis.\n");
                return -1;
            }
            cmdList[lastCommand] = cmd + i;

            // TODO: ARREGLAR!!
            /*isProgram = getIsProgram(cmdList[lastCommand]); //TODO: ANDA BIEN?
            if(isProgram == -1) {
                printf("Error: Comando %s desconocido.\n",
            cmdList[lastCommand]); return -1; } else if(isProgram == FALSE) {
                internalCmdCount++;
            }

            if(lastCommand > 0 && internalCmdCount > 0) {
                printf("Error: No pueden usarse tuberias en combinacion con comandos internos de la shell.\n");
                return -1;
            }*/

            foundPipe = FALSE;
            lastCommand++;
        }

        if ((cmd[i] == '\"' || cmd[i] == '\'') && !isEscaped)
            quoteEnabled = !quoteEnabled;
        else if (cmd[i] == '\\' && quoteEnabled)
            isEscaped = TRUE;
        else if (cmd[i] == '|' && !quoteEnabled)
        {
            cmd[i] = 0;
            foundPipe = TRUE;
        }
        else
        {
            isEscaped = FALSE;
        }

        i++;
    }

    if (i == 0)
        return 0;

    if (lastCommand > commandLimit)
    {
        printf("Error: se admite concatenar como mucho %d comandos.\n",
               commandLimit);
        return -1;
    }

    if (foundPipe == TRUE)
    {
        printf("Error de sintaxis.\n");
        return -1;
    }

    /*isProgram = getIsProgram(cmdList[lastCommand - 1]); //TODO: ANDA BIEN?
        if(isProgram == -1) {
            printf("Error: Comando desconocidoAAA.\n");
            return -1;
        } else if(isProgram == FALSE) {
            internalCmdCount++;
        }*/
    return lastCommand;
}

int parseArgs(char *cmd, int cmdLength, char **argv, int maxArgs, int *runInBackground)
{
    int currArg = 0;
    int quoteEnabled = FALSE;
    int foundArg = FALSE;
    *runInBackground = 0;

    // TODO: \0 DENTRO DEL PARAMETRO ENTRE COMILLAS

    for (int i = 0; cmd[i] != 0 && currArg < maxArgs; i++)
    {
        if (cmd[i] == '&' && (cmd[i + 1] == 0 || cmd[i + 1] == ' ' || cmd[i] == '\t'))
        {
            *runInBackground = 1;
            cmd[i] = 0;
        }
        else
        {
            if (foundArg && cmd[i] != ' ' && cmd[i] != '\t')
            {
                argv[currArg] = cmd + i;
                currArg++;
                foundArg = FALSE;
            }

            if (cmd[i] == '\"' || cmd[i] == '\'')
                quoteEnabled = !quoteEnabled;

            if ((cmd[i] == ' ' || cmd[i] == '\t') && !quoteEnabled)
            {
                cmd[i] = 0;
                foundArg = TRUE;
            }
        }
    }

    if (currArg > maxArgs)
    {
        printf("Error: un comando puede contener un maximo de %d parametros.\n",
               maxArgs);
        return -1;
    }

    argv[currArg] = NULL;

    return currArg;
}

/*int commandExec(char * cmd, char ** argv, int argCount, int background) {

        function f = getCommandFunction(cmd);
        if (f == 0) {
            printf("Comando '%s' desconocido.\n\n", cmd);
            return -1;
        }

        if (getIsProgram(cmd) == FALSE) {
            f(argv);
        } else {
            execProgram(cmd, argv, argCount, background);
        }
}*/

typedef char * argv[MAX_ARGS];

int commandParser(char *cmd, int length)
{
    if (*cmd == '\0')
        return -1;

    char *cmdList[MAX_FUNCTION_IN_COMMAND];
    int commandCount =
        parseCommands(cmd, length, cmdList, MAX_FUNCTION_IN_COMMAND);

    argv argvList[MAX_FUNCTION_IN_COMMAND];
    int paramCount = 0;
    int runInBackground[MAX_COMMAND_NAME_LENGTH];
    int fullScreen = FALSE;

    int fdList[MAX_FUNCTION_IN_COMMAND][2];
    int pidList[MAX_FUNCTION_IN_COMMAND];

    for (int i = 0; i < commandCount; i++)
    {
        paramCount = parseArgs(cmdList[i], length, argvList[i], MAX_ARGS, runInBackground + i);
        if (paramCount == -1)
            return -1;

        if (getCommandFunction(cmdList[i]) == NULL)
        {
            printf("Comando '%s' desconocido\n", cmdList[i]);
            return -1;
        }

        if (!getIsProgram(cmdList[i]))
        {
            if (commandCount > 1)
            {
                printf("No se pueden utilizar pipes con comandos internos de la shell\n");
                return -1;
            }
            else
            {
                getCommandFunction(cmdList[i])(argvList[i]);
                printf("\n");
                return 0;
            }
        }

        if (getIsFullscreen(cmdList[i])) {
          if (commandCount > 1) {
              printf("No se pueden utilizar pipes con programas en pantalla completa\n");
              return -1;
          }
          if (runInBackground[i]) {
              printf("No se pueden correr en background programas en pantalla completa\n");
              return -1;
          }
          fullScreen = TRUE;
        }

        if (commandCount - i > 1) // Hay pipes
            sys_pipe(fdList[i]);
    }

    for (int i = 0; i < commandCount; i++)
    {
        if (i == 0)
        {
            if (commandCount > 1)
            {
                int fdReplace[2][2] = {{fdList[i][1], 1}, {-1, -1}};

                pidList[i] =
                    execv(cmdList[i],
                          (process_t)getCommandFunction(cmdList[i]),
                          argvList[i], FALSE, fdReplace);

                sys_close(fdList[i][1]);
            }

            else
                pidList[i] =
                    execv(cmdList[i],
                          (process_t)getCommandFunction(cmdList[i]),
                          argvList[i], FALSE, NULL);

            if (!runInBackground[i])
            {
                sys_setForeground(pidList[i]);
            }
        }
        else
        {
            if (commandCount - 1 != i)
            {
                int fdReplace[3][2] = {
                    {fdList[i - 1][0], 0}, {fdList[i][1], 1}, {-1, -1}};

                pidList[i] = execv(
                    cmdList[i], (process_t)getCommandFunction(cmdList[i]),
                    argvList[i], FALSE, fdReplace);

                sys_close(fdList[i][1]);
            }
            else
            {
                int fdReplace[2][2] = {{fdList[i - 1][0], 0}, {-1, -1}};

                pidList[i] = execv(
                    cmdList[i], (process_t)getCommandFunction(cmdList[i]),
                    argvList[i], FALSE, fdReplace);
            }
            sys_close(fdList[i - 1][0]);
        }
    }

    for (int i = 0; i < commandCount; i++) {
        if (!runInBackground[i])
            sys_waitPID(pidList[i]);
    }

    if (fullScreen)
      cmd_resetScreen();

    printf("\n");

    return commandCount;
}

char hist[100][MAX_COMMAND_LENGTH];
unsigned int histCurrentIndex = 0;
unsigned int histAccessIndex = 0;
unsigned int histSize = 0;

void clearCmd(char cmd[])
{
    for (int i = 0; i < 100; i++)
    {
        cmd[i] = 0;
    }
}

void clearLine(unsigned int lineLong)
{
    for (int i = 0; i < lineLong; i++)
    {
        printf("\b");
    }
}

int testForeground(char **args)
{
    char c;
    char buff[100];
    int cursor = 0;

    printf("Introduzca el texto que desea imprimir:\n");

    while (c = getchar(), c != '\n')
    {
        if (c != EOF)
        {
            if (c == 8)
            { // backspace
                if (cursor > 0)
                {
                    buff[cursor] = '\0';
                    cursor--;
                    putchar(c);
                }
            }
            else
            {
                if (cursor < 100)
                {
                    if (c >= ' ' && c < 0x80)
                    {
                        buff[cursor] = c;
                        cursor++;
                    }
                    putchar(c);
                }
            }
        }
    }

    buff[cursor] = 0;

    printf("\nEl texto es: %s", buff);

    return 0;
}

void commandListener()
{
    char c;
    char cmd[MAX_COMMAND_LENGTH];
    int cursor = 0;
    int lastChar = 0;

    setFontColor(0xFFA500);
    puts("Terminalator> ");
    setBackgroundColor(currBackColor);
    setFontColor(currFontColor);

    setGraphicCursorStatus(1);

    while (c = getchar(), c != '\n')
    {
        if (c != EOF)
        {
            if (c == 8)
            { // backspace
                if (cursor > 0)
                {
                    for (int i = cursor; i < lastChar; i++)
                        cmd[i] = cmd[i + 1];

                    cmd[lastChar] = '\0';
                    lastChar--;
                    cursor--;
                    putchar(c);
                }
            }
            else if (c == 1 || c == 2)
            { // up or down arrow
                if (c == 1)
                {
                    if (histAccessIndex > 0)
                    {
                        histAccessIndex--;
                    }
                    else if (histSize > 0)
                    {
                        histAccessIndex = histSize - 1;
                    }
                }
                else
                {
                    if (histAccessIndex < (histSize - 1))
                    {
                        histAccessIndex++;
                    }
                    else
                    {
                        histAccessIndex = 0;
                    }
                }
                clearLine(cursor);
                clearCmd(cmd);
                cursor = 0;
                lastChar = 0;
                while (hist[histAccessIndex][cursor] != 0)
                {
                    cursor++;
                    lastChar++;
                }
                strcpy(cmd, hist[histAccessIndex]);
                printf("%s", cmd);
            }
            else
            {
                if (cursor < MAX_COMMAND_LENGTH)
                {
                    if (c >= ' ' && c < 0x80)
                    {
                        cmd[cursor] = c;
                        cursor++;
                        lastChar++;
                    }
                    putchar(c);
                }
            }
        }
    }

    setGraphicCursorStatus(0);

    setBackgroundColor(currBackColor);

    cmd[lastChar] = '\0';

    putchar('\n');

    if (histCurrentIndex >= 100)
    {
        histCurrentIndex = 0;
    }

    strcpy(hist[histCurrentIndex++], cmd);

    if (histSize != 100)
    {
        histSize++;
    }

    histAccessIndex = histCurrentIndex;

    commandParser(cmd, lastChar);
}

void invalidArgument(char *args) { printf("Argumento '%s' invalido", args); }

void cmd_time(char *args)
{
    printf("Fecha y hora del sistema: %X/%X/%X %X:%X:%X", sys_rtc(7),
           sys_rtc(8), sys_rtc(9), sys_rtc(4), sys_rtc(2), sys_rtc(0));
}

void cmd_help()
{
    for (int i = 0; i < commandsNum; i++)
    {
        setFontColor(0xFF6347);
        printf("%20s", commandList[i].name);
        setFontColor(currFontColor);
        printf("%s", commandList[i].desc);
        if (i < commandsNum - 1)
            putchar('\n');
    }
}

void cmd_exit()
{
    printf("Finalizando shell...");
    programStatus = 1;
    sys_exit(0);
}

void cmd_resetScreen()
{
    setFontColor(currFontColor);
    setBackgroundColor(currBackColor);
    setFontSize(currFontSize);
    clearScreen();
    cmd_printWelcome();
    // puts("\n");
}

void cmd_printWelcome()
{
    printf(
        "Leah v0.1\nInterprete de comandos Terminalator. Digite 'help' "
        "para "
        "mas informacion.");
    puts("\n");
}

void cmd_setFontSize(char **args)
{
    int num = atoi(args[0]);

    if (num <= 0)
    {
        invalidArgument(args[num]);
        return;
    }

    currFontSize = num;
    cmd_resetScreen();
}

void cmd_setBackColor(char **args)
{
    // int r, g, b;

    /*int leidos = sscanf(args, "%d %d %d", &r, &g, &b);

    if(leidos < 3) {
            invalidArgument(args);
            return;
    }*/

    int color = atoi(args[0]) * 256 * 256 + atoi(args[1]) * 256 + atoi(args[2]);
    int cComplement = 0xFFFFFF - color;

    currFontColor = cComplement;
    currBackColor = color;
    cmd_resetScreen();
}

int cmd_memoryManagerTest(void **args)
{
    char c = 8;
    int bytes = 0;
    int cursor = 0;
    char notNum = 0;
    int blocksAllocated = 0;

    printf(
        "\nPresione ESC para salir (todos los bloques seran "
        "liberados)\n\n");
    printf("Inserte numero de bytes para reservar: ");
    while (c = getchar(), c != 27 && blocksAllocated < 16)
    { // Esc

        if (c != '\n')
        {
            switch (c)
            {
            case 8: // backspace

                if (cursor > 0)
                {
                    cursor--;
                    putchar(c);
                    if (cursor == notNum)
                        notNum = 0;
                }
                break;

            default:

                if (c != -1)
                {
                    if (isNumeric(c))
                    {
                        bytes = c - '0' + bytes * 10;
                    }
                    else
                    {
                        if (notNum == 0)
                            notNum = cursor;
                    }
                    cursor++;
                    putchar(c);
                }
                break;
            }
        }
        else
        {
            if (bytes != 0)
            {
                if (notNum == 0)
                {
                    sys_memoryManagerTest(bytes);
                    blocksAllocated++;
                    bytes = 0;
                    if (blocksAllocated < 16)
                        printf("Inserte numero de bytes para reservar: ");
                }
                else
                {
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

    return 0;
}

void cmd_listDir(char **args) { sys_listDir(args[0]); }

void cmd_makeDirectory(char **args) { sys_makeFile(args[0], DIRECTORY); }

void cmd_touch(char **args) { sys_makeFile(args[0], REGULAR_FILE); }

void cmd_killProcess(char **args)
{
    sys_killProcess(atoi(args[0]));
    sys_waitPID(atoi(args[0]));
}

void cmd_removeFile(char **args) {
  //if (sys_removeFile(args[0]) == -1)
  //  printf("No se pudo eliminar el archivo.");
}

void cmd_writeTo(char **args)
{
    char c;
    char buff[100];
    int cursor = 0;

    int fd = sys_open(args[0], O_WRONLY);
    if (fd == -1)
    {
        printf("Error al abrir el archivo\n");
        return;
    }

    printf("Introduzca el texto que desea guardar:\n");

    while (c = getchar(), c != '\n')
    {
        if (c != EOF)
        {
            if (c == 8)
            { // backspace
                if (cursor > 0)
                {
                    buff[cursor] = '\0';
                    cursor--;
                    putchar(c);
                }
            }
            else
            {
                if (cursor < 100)
                {
                    if (c >= ' ' && c < 0x80)
                    {
                        buff[cursor] = c;
                        cursor++;
                    }
                    putchar(c);
                }
            }
        }
    }

    buff[cursor] = 0;

    sys_write(fd, buff, cursor);
    sys_close(fd);
}

void program_digitalClock()
{
    int pid =
        execv("Digital Clock", (process_t)&digitalClock, NULL, TRUE, NULL);

    sys_waitPID(pid);

    cmd_resetScreen();
}

int prog_prueba(char **args)
{
    char c;
    while (c = getchar(), c != -1)
    {
        printf("1%c", c);
    }
    return 0;
}

int prog_echo(char **args) {
  printf("%s\n", args[0]);
  return 0;
}

int prog_cat(char **args)
{
    int fd;
    char str[16];

    if (args[0] == NULL)
    {
        fd = 0;
    }
    else
    {
        fd = sys_open(args[0], O_RDONLY);
        if (fd == -1)
        {
            printf("Error al abrir el archivo\n");
            return -1;
        }
    }

    int aux;
    while ((aux = sys_read(fd, str, 15)) > 0)
    {
        str[aux] = 0;
        printf("%s", str);
    }

    if (args[0] != NULL)
        sys_close(fd);

    return 0;
}

void cmd_cd(char **args) { sys_chdir(args[0]); }

void cmd_ps(char **args)
{
    ps_struct buffer[MAX_PROCESS_COUNT];
    int bufferCount;
    sys_listProcess(buffer, &bufferCount);

    char tmp[20][4];

    printf("%6s%16s%16s%10s%13s%15s%13s%5s", "PID", "NAME", "PARENT", "STATUS",
           "FOREGROUND", "THREAD COUNT", "HEAP SIZE", "NICE");

    for (int i = 0; i < bufferCount; i++)
    {
        printf("\n%6s%16s%16s%10s%13s%15s%13s%d",
               itoa(buffer[i].pid, tmp[0], 10), buffer[i].name,
               buffer[i].parentName,
               (buffer[i].status == 0 ? "Alive" : "Zombie"),
               (buffer[i].foreground == 0 ? "False" : "True"),
               itoa(buffer[i].threadCount, tmp[1], 10),
               itoa(buffer[i].heapSize, tmp[2], 10), buffer[i].niceness);
        // itoa(buffer[i].niceness, tmp[3], 10));
    }
}

void cmd_prodcons(char **args) { prodcons(); }

void cmd_upDown(char *args) { upDown(); }

void cmd_giveForeground(char **args)
{
    int pid = atoi(args[0]);
    sys_setForeground(pid);
}

int arcoiris_main()
{
    int j = 0;
    int colors[7] = {0x4444DD, 0x11aabb, 0xaacc22, 0xd0c310,
                     0xff9933, 0xff4422, 0x72a4c9};
    while (1)
    {
        j++;
        setFontColor(colors[j % 7]);
        sys_sleep(1000);
    }
    return 0;
}

void program_arcoiris()
{
    execv("Arcoiris", (process_t)&arcoiris_main, NULL, TRUE, NULL);
}

int main()
{
    cmd_printWelcome();
    currBackColor = getBackgroundColor();
    currFontColor = getFontColor();
    currFontSize = getFontSize();
    puts("\n");

    command_register("time", cmd_time,
                     "Muestra la fecha y hora del reloj del sistema", TRUE,
                     FALSE);
    command_register("help", cmd_help,
                     "Despliega informacion sobre los comandos disponibles",
                     FALSE, FALSE);
    command_register("clear", cmd_resetScreen, "Limpia la pantalla", FALSE,
                     FALSE);
    command_register("font-size", cmd_setFontSize,
                     "Establece el tamano de la fuente y limpia la consola",
                     TRUE, FALSE);
    command_register("digital-clock", digitalClock,
                     "Muestra un reloj digital en pantalla", TRUE, TRUE);
    command_register("snake", snake_main,
                     "Juego Snake. Se juega con WASD. Argumentos: "
                     "[*movimientos * ms, *ratio de crecimiento]",
                     TRUE, TRUE);
    command_register("back-color", cmd_setBackColor,
                     "Cambia el color de fondo e invierte el color de fuente "
                     "adecuadamente. Argumentos: *[R G B]",
                     TRUE, FALSE);
    command_register(
        "test-memory-manager", (function)cmd_memoryManagerTest,
        "Realiza alocaciones de memoria y muestra el mapa en pantalla", TRUE,
        FALSE);
    command_register("toUppercase", (function)toUppercase, "Test para pipes",
                     TRUE, FALSE);
    command_register("ls", cmd_listDir,
                     "Lista los archivos en el directorio especificado", FALSE,
                     FALSE);
    command_register("cd", cmd_cd, "Cambia el directorio actual", FALSE, FALSE);
    command_register("mkdir", cmd_makeDirectory,
                     "Crea un directorio en la ruta especificada", FALSE,
                     FALSE);
    command_register("touch", cmd_touch,
                     "Crea un archivo regular en la ruta especificada", FALSE,
                     FALSE);
    command_register("rm", cmd_removeFile, "Elimina el archivo especificado",
                     FALSE, FALSE);
    command_register("writeTo", cmd_writeTo,
                     "Escribe en el archivo especificado", FALSE, FALSE);
    command_register("cat", (function)prog_cat, "Imprime el archivo especificado", TRUE,
                     FALSE);
    command_register("ps", cmd_ps,
                     "Lista los procesos con su informacion asociada", TRUE,
                     FALSE);
    command_register("prodcons", (function)prodcons,
                     "Simula el problema de productor consumidor", TRUE, FALSE);
    command_register("philosophers", (function)philosophers,
                     "Simula el problema de los filosofos cenando con numero "
                     "variable de filosofos",
                     TRUE, FALSE);
    command_register(
        "updown", cmd_upDown,
        "Testea si una variable queda en 0 despues de 5000 ups y downs", TRUE,
        FALSE);
    command_register("arcoiris", (function)arcoiris_main,
                     "Cambia cada un segundo el color de fuente", TRUE, FALSE);
    command_register("kill", cmd_killProcess,
                     "Mata al proceso de PID especificado", FALSE, FALSE);
    command_register("foreground", cmd_giveForeground,
                     "Cede el primer plano al procedo de PID especificado",
                     FALSE, FALSE);
    command_register("testforeground", (function)testForeground,
                     "Prueba de foreground. Pide un texto y lo imprime", TRUE,
                     FALSE);
    command_register("echo", (function)prog_echo, "Echo como programa", TRUE, FALSE);
    command_register("prueba", (function)prog_prueba, "Para correr con echo", TRUE, FALSE);
    command_register("exit", cmd_exit, "Cierra la Shell", FALSE, FALSE);

    while (programStatus != 1)
    {
        commandListener();
    }

    return 0;
}
