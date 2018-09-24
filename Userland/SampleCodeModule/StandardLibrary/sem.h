#ifndef STD_SEM
#define STD_SEM

//Si el semaforo ya existe no hace nada
//Sino lo crea y setea su valor
void sem_create(char * path, int value);

void sem_delete(char * name);

//Devuelve -1 si no se pudo abrir
int sem_open(char * path);

void sem_close(int sem);

void sem_wait(int sem);

void sem_signal(int sem);

#endif
