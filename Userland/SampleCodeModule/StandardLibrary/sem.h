#ifndef STD_SEM
#define STD_SEM

#define O_WRONLY 0
#define O_RDONLY 1
#define O_RDWR 2

//Si el semaforo ya existe no hace nada
//Sino lo crea y setea su valor
void sem_create(char * path, int value);

//Devuelve -1 si no se pudo abrir
int sem_open(char * path);

void sem_close(int sem);

void sem_wait(int sem);

void sem_signal(int sem);

#endif
