#include <fileSystem.h>
#include <memoryManager.h>
#include <drivers/console.h>
#include <malloc.h>
#include "stdlib.h"

#define BUFFER_SIZE PAGE_SIZE

static void * createBuffer();
static void * createDirectory();
static void * createRegularFile();

static void removeDirectory(file_t * file);
static void removeBuffer(file_t * file);
static void removeRegularFile(file_t * file);

static file_t * newFile(char name[], int type);

typedef struct directory {
  file_t * first;
} directory_t;

typedef struct buffer {
  char content[BUFFER_SIZE];
} buffer_t;

typedef struct regular_file {
  char * content;
  uint32_t size;
  uint32_t totalSize;
} regular_file_t;


file_t * root;
opened_file_t * firstOpenedFile;

void init_fileSystem() {
  root = newFile("root", DIRECTORY);
  root->directory = NULL;

  firstOpenedFile = NULL;

  makeFile("Test dir", DIRECTORY);
  makeFile("Test file", REGULAR_FILE);
}


static file_t * makeFileR(char path[], file_t * dir, int type, char name[MAX_NAME_LENGTH], int getFile) {
  int i;

  for (i = 0; path[i] != '/' && path[i] != 0 && i < MAX_NAME_LENGTH + 2; i++) {
    name[i] = path[i];
  }

  if (i == MAX_NAME_LENGTH + 2)
    return NULL;

  name[i] = 0;

  if (i == 0) {
    if (path[i] == 0)
      return dir;
    else
      return NULL;
  }

  file_t * prevFile = NULL;
  file_t * currFile = ((directory_t*)(dir->implementation))->first;

  int cmp;
  while (currFile != NULL && (cmp = strcmp(currFile->name, name)) < 0) {
    //if (cmp == 0)
    //  return currFile;
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

      if (file == NULL)
        return NULL;

      file->next = currFile;
      file->directory = dir;
      if (prevFile != NULL)
        prevFile->next = file;
      else
        ((directory_t*)(dir->implementation))->first = file;

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
  else if (type == REGULAR_FILE)
    newFile->implementation = createRegularFile();
  else if (type == BUFFER)
    newFile->implementation = createBuffer();

  return newFile;
}

static void * createBuffer() {
  buffer_t * buff = getMemory(sizeof(buffer_t));
  return (void*)buff;
}

static void * createDirectory() {
  directory_t * dir = getMemory(sizeof(directory_t));
  dir->first = NULL;
  return (void*)dir;
}

static void * createRegularFile() {
  regular_file_t * file = getMemory(sizeof(regular_file_t));
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
    else if (currFile->type == REGULAR_FILE)
      removeRegularFile(currFile);
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

static void removeRegularFile(file_t * file) {
  regular_file_t * regularFile = (regular_file_t*)(file->implementation);
  freeMemory(regularFile->content);
  freeMemory(regularFile);
}


//////////// I/O OPERATIONS ////////////

typedef struct pending_reader {
  int PID;
  uint32_t bytes;
  char * dest;
  int * ret;
} pending_reader_t;

typedef struct opened_buffer {
  uint32_t writeCursor;
  uint32_t readCursor;
  int hasEOF;
  pending_reader_t * first;
  pending_reader_t * last;
} opened_buffer_t;

static opened_buffer_t * openBuffer();
static uint32_t writeBuffer(opened_file_t * openedFile, char * buff, uint32_t bytes);
static uint32_t writeRegularFile(opened_file_t * openedFile, char * buff, uint32_t bytes);

static uint32_t readRegularFile(opened_file_t * openedFile, char * buff, uint32_t bytes, uint32_t position);
static uint32_t readBuffer(opened_file_t * openedFile, char * buff, uint32_t bytes);
static void updateReaders(opened_buffer_t * openedBuffer);

opened_file_t * openFile(char * path, int mode) {
  file_t * file = getFile(path);

  if (file == NULL || file->type == DIRECTORY)
    return NULL;

  opened_file_t * openedFile = firstOpenedFile;
  while(openedFile != NULL && openedFile->file != file)
    openedFile = openedFile->next;

  if (openedFile == NULL) {
    openedFile = getMemory(sizeof(opened_file_t));
    openedFile->next = firstOpenedFile;
    firstOpenedFile = openedFile;
    openedFile->file = file;
    openedFile->readers = 0;
    openedFile->writers = 0;

    if (file->type == BUFFER)
      openedFile->implementation = (void*)openBuffer();
  }

  if (mode == O_RDONLY || mode == O_RDWR)
    openedFile->readers++;
  if (mode == O_WRONLY || mode == O_RDWR) {
    openedFile->writers++;
    if (file->type == BUFFER)
      ((opened_buffer_t*)(openedFile->implementation))->hasEOF = 0;
  }

  return openedFile;
}

void closeFile(opened_file_t * openedFile, int mode) {
  if (openedFile == NULL)
    return;

  if (mode == O_RDONLY || mode == O_RDWR)
    openedFile->readers--;
  if (mode == O_WRONLY || mode == O_RDWR) {
    openedFile->writers--;
    if (openedFile->writers == 0 && openedFile->file->type == BUFFER) {
      ((opened_buffer_t*)openedFile->implementation)->hasEOF = 1;
      updateReaders((opened_buffer_t*)(openedFile->implementation));
    }
  }

  if (openedFile->readers == 0 && openedFile->writers == 0) {
    opened_file_t * previous = NULL;
    opened_file_t * current = firstOpenedFile;

    while (current != openedFile) {
      if (current == NULL)
        return;
      previous = current;
      current = current->next;
    }

    if (previous == NULL)
      firstOpenedFile = current->next;
    else
      previous->next = current->next;

    if (openedFile->file->type == BUFFER)
      freeMemory(openedFile->implementation);

    freeMemory(openedFile);
  }
}

static opened_buffer_t * openBuffer() {
  opened_buffer_t * openedBuffer = getMemory(sizeof(opened_buffer_t));
  openedBuffer->writeCursor = 0;
  openedBuffer->readCursor = 0;
  openedBuffer->hasEOF = 0;
  openedBuffer->first = NULL;
  openedBuffer->last = NULL;
  return openedBuffer;
}


uint32_t writeFile(opened_file_t * openedFile, char * buff, uint32_t bytes, int mode) {
  if (mode != O_WRONLY && mode != O_RDWR)
    return 0;

  if (bytes == 0 || openedFile == NULL)
    return 0;

  if (openedFile->file->type == REGULAR_FILE)
    return writeRegularFile(openedFile, buff, bytes);

  if (openedFile->file->type == BUFFER)
    return writeBuffer(openedFile, buff, bytes);

  return 0;
}

static uint32_t writeRegularFile(opened_file_t * openedFile, char * buff, uint32_t bytes) {
  regular_file_t * regularFile = (regular_file_t*)(openedFile->file->implementation);
  int availableBytes = regularFile->totalSize - regularFile->size;
  availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

  if (availableBytes <= 0)
    return 0;

  for (int i = 0; i < availableBytes; i++) {
    regularFile->content[regularFile->size] = buff[i];
    regularFile->size++;
  }

  return availableBytes;
}

static uint32_t writeBuffer(opened_file_t * openedFile, char * buff, uint32_t bytes) {
  opened_buffer_t * openedBuffer = (opened_buffer_t*)(openedFile->implementation);
  buffer_t * buffer = (buffer_t*)(openedFile->file->implementation);
  int availableBytes = openedBuffer->readCursor - openedBuffer->writeCursor;
  availableBytes = (availableBytes >= 0) ? availableBytes : BUFFER_SIZE + availableBytes;
  availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

  for (int i = 0; i < availableBytes; i++) {
    buffer->content[openedBuffer->writeCursor] = buff[i];
    openedBuffer->writeCursor++;
    if (openedBuffer->writeCursor == BUFFER_SIZE)
      openedBuffer->writeCursor = 0;
  }

  return availableBytes;
}


uint32_t readFile(opened_file_t * openedFile, char * buff, uint32_t bytes, uint32_t position, int mode) {
  if (mode != O_RDONLY && mode != O_RDWR)
    return 0;

  if (bytes == 0 || openedFile == NULL)
    return 0;

  if (openedFile->file->type == REGULAR_FILE)
    return readRegularFile(openedFile, buff, bytes, position);

  if (openedFile->file->type == BUFFER)
    return readBuffer(openedFile, buff, bytes);

  return 0;
}

static uint32_t readRegularFile(opened_file_t * openedFile, char * buff, uint32_t bytes, uint32_t position) {
  regular_file_t * regularFile = (regular_file_t*)(openedFile->file->implementation);
  int availableBytes = regularFile->size - position;
  availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

  if (availableBytes <= 0)
    return 0;

  for (int i = 0; i < availableBytes; i++) {
    buff[i] = regularFile->content[position];
    position++;
  }

  return availableBytes;
}

static uint32_t readBuffer(opened_file_t * openedFile, char * buff, uint32_t bytes) {
  opened_buffer_t * openedBuffer = (opened_buffer_t*)(openedFile->implementation);
  buffer_t * buffer = (buffer_t*)(openedFile->file->implementation);
  int availableBytes = openedBuffer->writeCursor - openedBuffer->readCursor;
  availableBytes = (availableBytes >= 0) ? availableBytes : BUFFER_SIZE + availableBytes;
  availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

  for (int i = 0; i < availableBytes; i++) {
    buff[i] = buffer->content[openedBuffer->readCursor];
    openedBuffer->readCursor++;
    if (openedBuffer->readCursor == BUFFER_SIZE)
      openedBuffer->readCursor = 0;
  }

  return availableBytes;
}

static void updateReaders(opened_buffer_t * openedBuffer) {
  //ToDo
}


///// TESTING FUNCTIONS /////

void listDir(char * path) {
  file_t * file = getFile(path);

  if (file == NULL)
    return;

  if (file->type != DIRECTORY)
    return;

  file_t * current = ((directory_t*)(file->implementation))->first;
  while(current != NULL) {
    printf("\n%s", current->name);
    if (current->type == DIRECTORY)
      printf(" (dir)");
    else if (current->type == REGULAR_FILE)
      printf(" (regular file)");
    else if (current->type == BUFFER)
      printf(" (buffer)");
    current = current->next;
  }
}

void cat(char * path) {
  char str[16];
  opened_file_t * openedFile = openFile(path, O_RDONLY);
  int cursor = 0;
  int aux;
  while ((aux = readFile(openedFile, str, 1, cursor, O_RDONLY)) > 0) {
    cursor += aux;
    str[aux + 1] = 0;
    printf("%s", str);
  }
  closeFile(openedFile, O_RDONLY);
}

void writeTo(char * path, char * str) {
  opened_file_t * openedFile = openFile(path, O_WRONLY);
  writeFile(openedFile, str, strlen(str), O_WRONLY);
  closeFile(openedFile, O_WRONLY);
}
