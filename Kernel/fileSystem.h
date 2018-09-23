#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>

#define DIRECTORY 0
#define REGULAR_FILE 1
#define BUFFER 2
#define SEMAPHORE 3
#define MUTEX 4

#define MAX_NAME_LENGTH 64

#define O_WRONLY 0
#define O_RDONLY 1
#define O_RDWR 2

typedef struct file {
  struct file * next;
  struct file * directory;
  char name[MAX_NAME_LENGTH + 1];
  int type;
  void * implementation;
} file_t;

typedef struct opened_file {
  struct opened_file * next;
  file_t * file;
  int writers;
  int readers;
  void * implementation;
} opened_file_t;

typedef struct file_descriptor {
  opened_file_t * openedFile;
  int mode;
  int cursor;
} fd_t;

void init_fileSystem();

file_t * makeFile(char * path, int type);
file_t * getFile(char * path);

void removeFile(file_t * file);
void removeFileFromPath(char * path);

fd_t * openFileFromPath(char * path, int mode);
int openFileToFD(file_t * file, int mode);
int openFileFromPathToFD(char * path, int mode);
void closeFile(fd_t * fd);
void closeFileFromFD(int fdIndex);
//fd_t * cloneOpenedFile(fd_t * fd);

uint32_t writeFile(fd_t * fd, char * buff, uint32_t bytes);
uint32_t readFile(fd_t * fd, char * buff, uint32_t bytes);

void semCreate(char * path, int value);
void semDelete(char * name);
int semOpen(char * name);
void semClose(int fdIndex);
void semSet(int fdIndex, int value);
void semWait(int fdIndex);
void semSignal(int fd);

void mutexCreate(char * name);
void mutexDelete(char * name);
int mutexOpen(char * name);
void mutexClose(int fdIndex);
void mutexLock(int fdIndex);
void mutexUnlock(int fdIndex);

void listDir(char * path);
void cat(char * path);
void writeTo(char * path, char * str);

#endif
