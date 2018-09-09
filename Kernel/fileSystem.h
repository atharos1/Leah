#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#define DIRECTORY 0
#define LINEAR_FILE 1
#define BUFFER 2
#define MAX_NAME_LENGTH 64

typedef struct file {
  struct file * next;
  struct file * directory;
  char name[MAX_NAME_LENGTH + 1];
  int type;
  void * implementation;
} file_t;

void init_fileSystem();
file_t * makeFile(char * path, int type);
file_t * getFile(char * path);
void removeFile(file_t * file);
void removeFileFromPath(char * path);
void listDir(char * path);

#endif
