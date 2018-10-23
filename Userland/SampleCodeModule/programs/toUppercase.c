#include "../StandardLibrary/include/stdio.h"
#include "../asm/asmLibC.h"
#include "../StandardLibrary/include/pthread.h"

#define MAX_LENGTH 100

int slave();

int toUppercase() {
  char c;
	char buff[MAX_LENGTH];
	int cursor = 0;

  sys_mkFifo("pipe1");
  sys_mkFifo("pipe2");

  execv("Slave", slave, 0, 1, 0);

	int fd1 = sys_open("pipe1", O_WRONLY);
  int fd2 = sys_open("pipe2", O_RDONLY);
	if (fd1 == -1 || fd2 == -1) {
		printf("Error al abrir el archivo\n");
		return 0;
	}

  printf("Presione ESC para salir \n\n");

	printf("Introduzca un texto: ");

	while(c = getchar(), c != 27) {  //Escape

		if( c != EOF ) {

			if (c == 8) { //backspace
				if(cursor > 0) {

					buff[cursor] = '\0';
					cursor--;
					putchar(c);
			  }
			} else if (c == '\n') {
        if (cursor > 0) {
          sys_write(fd1, buff, cursor);
          cursor = 0;
          putchar(c);
          int aux = sys_read(fd2, buff, MAX_LENGTH);
          buff[aux] = 0;
          printf("En mayusculas: %s\n", buff);
          printf("Introduzca un texto: ");
        }
      } else {
				if (cursor < 100) {
					if (c >= ' ' && c < 0x80) {
						buff[cursor] = c;
						cursor++;
					}
					putchar(c);
				}
			}
		}
	}

	sys_close(fd1);
  sys_close(fd2);

  return 0;
}

int slave() {
  char buff[MAX_LENGTH];

  int fd1 = sys_open("pipe1", O_RDONLY);
  int fd2 = sys_open("pipe2", O_WRONLY);
  if (fd1 == -1 || fd2 == -1) {
    printf("Error al abrir el archivo\n");
    return 0;
  }

  int aux;

  while((aux = sys_read(fd1, buff, MAX_LENGTH)) > 0) {
    buff[aux] = 0;
    for (int i = 0; i < aux; i++) {
      if (buff[i] >= 'a' && buff[i] <= 'z')
        buff[i] -= ('a' - 'A');
    }
    sys_write(fd2, buff, MAX_LENGTH);;
  }

  sys_close(fd1);
  sys_close(fd2);

  return 0;
}
