#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>

#define DIRECTORY 0
#define REGULAR_FILE 1
#define BUFFER 2

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

void init_fileSystem();

file_t * makeFile(char * path, int type);
file_t * getFile(char * path);

void removeFile(file_t * file);
void removeFileFromPath(char * path);

opened_file_t * openFile(char * path, int mode);
void closeFile(opened_file_t * openedFile, int mode);

uint32_t writeFile(opened_file_t * openedFile, char * buff, uint32_t bytes, int mode);
uint32_t readFile(opened_file_t * openedFile, char * buff, uint32_t bytes, uint32_t position, int mode);

void listDir(char * path);
void cat(char * path);
void writeTo(char * path, char * str);

#endif
