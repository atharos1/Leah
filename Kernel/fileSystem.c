#include <fileSystem.h>
#include <memoryManager.h>
#include <drivers/console.h>
#include <malloc.h>
#include <scheduler.h>
#include "stdlib.h"

#define BUFFER_SIZE PAGE_SIZE

#define WRITE 0
#define READ 1

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
  init_malloc(5*1024*1024);

  root = newFile("root", DIRECTORY);
  root->directory = NULL;

  firstOpenedFile = NULL;

  makeFile("dev", DIRECTORY);
  makeFile("dev/shm", DIRECTORY);
  makeFile("dev/stdin", BUFFER);
  makeFile("TestFile", REGULAR_FILE);
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
  file_t * newFile = malloc(sizeof(file_t));

  if (newFile == NULL)
    return NULL;

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
  buffer_t * buff = malloc(sizeof(buffer_t));
  return (void*)buff;
}

static void * createDirectory() {
  directory_t * dir = malloc(sizeof(directory_t));

  if (dir == NULL)
    return NULL;

  dir->first = NULL;
  return (void*)dir;
}

static void * createRegularFile() {
  regular_file_t * file = malloc(sizeof(regular_file_t));

  if (file == NULL)
    return NULL;

  file->content = getMemory(PAGE_SIZE);

  if (file->content == NULL) {
    free(file);
    return NULL;
  }

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

    free(currFile);
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
  free(file->implementation);
}

static void removeBuffer(file_t * file) {
  free(file->implementation);
}

static void removeRegularFile(file_t * file) {
  regular_file_t * regularFile = (regular_file_t*)(file->implementation);
  freeMemory(regularFile->content);
  free(regularFile);
}


//////////// I/O OPERATIONS ////////////

typedef struct pending_operator {
  thread_t * thread;
  int operation;
  uint32_t bytes;
  char * buff;
  int * ret;
} pending_operator_t;

typedef struct opened_buffer {
  uint32_t writeCursor;
  uint32_t readCursor;
  int hasEOF;
  pending_operator_t * first;
  pending_operator_t * last;
} opened_buffer_t;

static opened_buffer_t * openBuffer();
static uint32_t writeBuffer(opened_file_t * openedFile, char * buff, uint32_t bytes);
static uint32_t writeRegularFile(opened_file_t * openedFile, char * buff, uint32_t bytes);

static uint32_t readRegularFile(opened_file_t * openedFile, char * buff, uint32_t bytes, uint32_t position);
static uint32_t readBuffer(opened_file_t * openedFile, char * buff, uint32_t bytes);
static void updateOperators(opened_file_t * openedFile);


static fd_t * openFile(file_t * file, int mode) {

  if (file == NULL || file->type == DIRECTORY)
    return NULL;

  fd_t * fd = malloc(sizeof(fd_t));
  if (fd == NULL)
    return NULL;

  opened_file_t * openedFile = firstOpenedFile;
  while(openedFile != NULL && openedFile->file != file)
    openedFile = openedFile->next;

  if (openedFile == NULL) {
    openedFile = malloc(sizeof(opened_file_t));
    if (openedFile == NULL)
      return NULL;
    openedFile->next = firstOpenedFile;
    firstOpenedFile = openedFile;
    openedFile->file = file;
    openedFile->readers = 0;
    openedFile->writers = 0;

    if (file->type == BUFFER) {
      openedFile->implementation = (void*)openBuffer();
      if (openedFile->implementation == NULL) {
        free(openedFile);
        return NULL;
      }
    }
  }

  if (mode == O_RDONLY || mode == O_RDWR)
    openedFile->readers++;
  if (mode == O_WRONLY || mode == O_RDWR) {
    openedFile->writers++;
    if (file->type == BUFFER)
      ((opened_buffer_t*)(openedFile->implementation))->hasEOF = 0;
  }

  fd->openedFile = openedFile;
  fd->mode = mode;
  fd->cursor = 0;

  return fd;
}

fd_t * openFileFromPath(char * path, int mode) {
  return openFile(getFile(path), mode);
}

void closeFile(fd_t * fd) {
  if (fd == NULL)
    return;

  opened_file_t * openedFile = fd->openedFile;
  int mode = fd->mode;

  if (mode == O_RDONLY || mode == O_RDWR)
    openedFile->readers--;
  if (mode == O_WRONLY || mode == O_RDWR) {
    openedFile->writers--;
    if (openedFile->writers == 0 && openedFile->file->type == BUFFER) {
      ((opened_buffer_t*)(openedFile->implementation))->hasEOF = 1;
      updateOperators(openedFile);
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
      free(openedFile->implementation);

    free(openedFile);
  }

  free(fd);
}

static opened_buffer_t * openBuffer() {
  opened_buffer_t * openedBuffer = malloc(sizeof(opened_buffer_t));
  if (openedBuffer == NULL)
    return NULL;
  openedBuffer->writeCursor = 0;
  openedBuffer->readCursor = 0;
  openedBuffer->hasEOF = 0;
  openedBuffer->first = NULL;
  openedBuffer->last = NULL;
  return openedBuffer;
}


uint32_t writeFile(fd_t * fd, char * buff, uint32_t bytes) {
  if (fd->mode != O_WRONLY && fd->mode != O_RDWR)
    return 0;

  if (bytes == 0 || fd == NULL)
    return 0;

  if (fd->openedFile->file->type == REGULAR_FILE)
    return writeRegularFile(fd->openedFile, buff, bytes);

  if (fd->openedFile->file->type == BUFFER)
    return writeBuffer(fd->openedFile, buff, bytes);

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
  availableBytes = (availableBytes > 0) ? availableBytes : BUFFER_SIZE + availableBytes;
  availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

  for (int i = 0; i < availableBytes; i++) {
    buffer->content[openedBuffer->writeCursor] = buff[i];
    openedBuffer->writeCursor++;
    if (openedBuffer->writeCursor == BUFFER_SIZE)
      openedBuffer->writeCursor = 0;
  }

  /*if (availableBytes == 0) {
    int ret;
    addOperator(openedBuffer, openedscheduler_dequeue_current(), READ, bytes, buff, &ret);
    return *ret;
  }*/

  return availableBytes;
}


uint32_t readFile(fd_t * fd, char * buff, uint32_t bytes) {
  if (fd->mode != O_RDONLY && fd->mode != O_RDWR)
    return 0;

  if (bytes == 0 || fd == NULL)
    return 0;

  if (fd->openedFile->file->type == REGULAR_FILE) {
    int ret = readRegularFile(fd->openedFile, buff, bytes, fd->cursor);
    fd->cursor += ret;
    return ret;
  }

  if (fd->openedFile->file->type == BUFFER)
    return readBuffer(fd->openedFile, buff, bytes);

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

  /*if (availableBytes == 0 && !(openedBuffer->hasEOF)) {
    int ret;
    addOperator(openedBuffer, openedscheduler_dequeue_current(), READ, bytes, buff, &ret);
    return *ret;
  }*/

  return availableBytes;
}

static void updateOperators(opened_file_t * openedFile) {
  /*if (openedFile->file->type != BUFFER)
    return;

  opened_buffer_t * openedBuffer = (openedBuffer*)(openedFile->implementation);
  pending_operator_t * current = openedBuffer->first;
  pending_operator_t * previous = NULL;

  int ret;

  while(current != NULL) {
    if (current->operation == READ)
      ret = readBuffer(openedFile, current->buff, current->bytes);
    if (current->operation == WRITE)
      ret = writeBuffer(openedFile, current->buff, current->bytes);

    if (ret == 0) {
      if (openedBuffer->hasEOF)
        *(current->ret) = -1;
      else
        break;
    }

    if (current == openedBuffer->first)
      openedBuffer->first = openedBuffer->first->next;


    scheduler_enqueue(current->thread);

    current = current->next;
  }*/
}

static void addOperator(opened_buffer_t * openedBuffer, thread_t * thread, int operation, uint32_t bytes, char * buff, int * ret) {

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
      printf(" (directory)");
    else if (current->type == REGULAR_FILE)
      printf(" (regular file)");
    else if (current->type == BUFFER)
      printf(" (buffer)");
    current = current->next;
  }
}

void cat(char * path) {
  char str[16];
  fd_t * fd = openFileFromPath(path, O_RDONLY);
  int aux;


  while ((aux = readFile(fd, str, 15)) > 0) {
    str[aux] = 0;
    printf("%s", str);
  }

  closeFile(fd);
}

void writeTo(char * path, char * str) {
  fd_t * fd = openFileFromPath(path, O_WRONLY);
  writeFile(fd, str, strlen(str));
  closeFile(fd);
}
