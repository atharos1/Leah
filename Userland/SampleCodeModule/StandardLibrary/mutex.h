#ifndef STD_MUTEX
#define STD_MUTEX

//Crea un mutex. Si ya existe, no hace nada
void mutex_create(char * name);

void mutex_delete(char * name);

int mutex_open(char * name);

void mutex_close(int mutex);

void mutex_lock(int mutex);

void mutex_unlock(int mutex);

#endif
