#include <fileSystem.h>
#include <stdint.h>
#include <memoryManager.h>
#include <drivers/console.h>

#define BUFFER_SIZE 4092

static void * createBuffer();
static void * createDirectory();
static void * createLinearFile();
//static void * (*createFileArr[3])();

static void removeDirectory(file_t * file);
static void removeBuffer(file_t * file);
static void removeLinearFile(file_t * file);
//static void (*removeFileArr[3])(file_t*);

static file_t * newFile(char name[], int type);

typedef struct directory {
  file_t * first;
} directory_t;

typedef struct buffer {
  char content[BUFFER_SIZE];
  uint32_t writeCursor;
  uint32_t readCursor;
} buffer_t;

typedef struct linear_file {
  char * content;
  uint32_t size;
  uint32_t totalSize;
} linear_file_t;

typedef struct openedFile {
  file_t * file;
  struct openedFile * next;
  int timesOpened;
} opened_file_t;


static int strcmp(char string1[], char string2[] )
{
    for (int i = 0; ; i++)
    {
        if (string1[i] != string2[i])
        {
            return string1[i] < string2[i] ? -1 : 1;
        }

        if (string1[i] == '\0')
        {
            return 0;
        }
    }
}

static void strcpy(char * dest, char * origin) {
    int i;
    for(i = 0; origin[i] != 0; i++)
        dest[i] = origin[i];

    dest[i+1] = 0;
}


file_t * root;

void init_fileSystem() {
  root = newFile("root", DIRECTORY);
  root->directory = NULL;

  makeFile("Test dir", DIRECTORY);
  makeFile("Test file", LINEAR_FILE);;

  /*createFileArr[DIRECTORY] = createDirectory;
  createFileArr[LINEAR_FILE] = createLinearFile;
  createFileArr[BUFFER] = createBuffer;
  removeFileArr[DIRECTORY] = removeDirectory;
  removeFileArr[LINEAR_FILE] = removeLinearFile;
  removeFileArr[BUFFER] = removeBuffer;*/
}


static file_t * makeFileR(char path[], file_t * dir, int type, char name[MAX_NAME_LENGTH], int getFile) {
  int i;

  for (i = 0; path[i] != '/' && path[i] != 0 && i < MAX_NAME_LENGTH + 1; i++)
    name[i] = path[i];

  name[i] = 0;

  if (i == 0)
    return dir;

  file_t * prevFile = NULL;
  file_t * currFile = ((directory_t*)dir->implementation)->first;

  int cmp;
  while (currFile != NULL && (cmp = strcmp(currFile->name, name)) < 0) {
    if (cmp == 0)
      return currFile;

    prevFile = currFile;
    currFile = currFile->next;
  }

  if (path[i] == 0) {
    if (currFile != NULL && cmp == 0)
      return currFile;
    else {
      if (getFile)
        return NULL;
      file_t * file = newFile(name, type);
      file->next = currFile;
      file->directory = dir;
      if (prevFile != NULL)
        prevFile->next = file;
      else
        ((directory_t*)dir->implementation)->first = file;

      return file;
    }
  }

  if (path[i] == '/' && cmp == 0) {
    if (currFile == NULL || currFile->type != DIRECTORY)
      return NULL;

    return makeFileR(path + i + 1, currFile, type, name, getFile);
  }

  return NULL;
}

file_t * makeFile(char * path, int type) {
  char name[MAX_NAME_LENGTH];
  return makeFileR(path, root, type, name, 0);
  //return makeFileR(path, (directory_t*)root->implementation, type, name);
}


file_t * getFile(char * path) {
  char name[MAX_NAME_LENGTH];
  return makeFileR(path, root, 0, name, 1);
}


static file_t * newFile(char name[], int type) {
  file_t * newFile = getMemory(sizeof(file_t));
  strcpy(newFile->name, name);
  newFile->next = NULL;
  newFile->type = type;

  if (type == DIRECTORY)
    newFile->implementation = createDirectory();
  else if (type == LINEAR_FILE)
    newFile->implementation = createLinearFile();
  else if (type == BUFFER)
    newFile->implementation = createBuffer();

  return newFile;
}

static void * createBuffer() {
  buffer_t * buff = getMemory(sizeof(buffer_t));
  buff->writeCursor = 0;
  buff->readCursor = 0;
  return (void*)buff;
}

static void * createDirectory() {
  directory_t * dir = getMemory(sizeof(directory_t));
  dir->first = NULL;
  return (void*)dir;
}

static void * createLinearFile() {
  linear_file_t * file = getMemory(sizeof(linear_file_t));
  file->content = getMemory(PAGE_SIZE);
  file->size = 0;
  file->totalSize = PAGE_SIZE;
  return (void*)file;
}


static file_t * removeFileR(file_t * currFile, file_t * targetFile) {
  if (currFile == NULL)
    return currFile;

  if (currFile == targetFile) {
    if (currFile->type == DIRECTORY)
      removeDirectory(currFile);
    else if (currFile->type == LINEAR_FILE)
      removeLinearFile(currFile);
    else if (currFile->type == BUFFER)
      removeBuffer(currFile);

    freeMemory(currFile);
    return currFile->next;
  }
  currFile->next = removeFileR(currFile->next, targetFile);
  return currFile;
}

//HAY UN PROBLEMA AL REMOVER CARPETAS CON ARCHIVOS!
void removeFile(file_t * file) {
  if (file == NULL)
    return;

  directory_t * dir = (directory_t*)(file->directory->implementation);
  if (file->type == DIRECTORY) {
    file_t * currFile = ((directory_t*)(file->implementation))->first;
    while(currFile != NULL) {
      removeFile(currFile);
      currFile = currFile->next;
    }

  }
  dir->first = removeFileR(dir->first, file);
}


void removeFileFromPath(char * path) {
  removeFile(getFile(path));
}

static void removeDirectory(file_t * file) {
  freeMemory(file->implementation);
}

static void removeBuffer(file_t * file) {
  freeMemory(file->implementation);
}

static void removeLinearFile(file_t * file) {
  linear_file_t * linearFile = (linear_file_t*)(file->implementation);
  freeMemory(linearFile->content);
  freeMemory(linearFile);
}

void listDir(char * path) {
  file_t * file = getFile(path);

  if (file == NULL)
    return;

  if (file->type != DIRECTORY)
    return;

  file_t * current = ((directory_t*)file->implementation)->first;
  while(current != NULL) {
    printf("\n%s", current->name);
    if (current->type == DIRECTORY)
      printf(" (dir)");
    else if (current->type == LINEAR_FILE)
      printf(" (reg file)");
    else if (current->type == BUFFER)
      printf(" (buffer)");
    current = current->next;
  }
}

void openFile(char * path) {
  //verifica si esta abierto
  //devuelve una instancia de archivo abierto
}

void closeFile(char * path) {
  //busca al archivo en los archivos abiertos
  //decrementa la cantidad de veces abierto
  //si es un buffer y llega a 0, se borra
}

/*int readFile(file_t * file, char * buff, int bytes) {
  char * content;
  uint32_t position;

  memcpy(buff, content + position, bytes);
}

int readBuffer() {
  char * content = ((buffer_t*)file->implementation)->content;
}*/
