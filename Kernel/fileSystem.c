#include "include/fileSystem.h"
#include "asm/libasm.h"
#include "drivers/include/console.h"
#include "drivers/include/kb_driver.h"
#include "include/malloc.h"
#include "include/memoryManager.h"
#include "include/mutex.h"
#include "include/process.h"
#include "include/scheduler.h"
#include "drivers/include/kb_driver.h"
#include "include/stdlib.h"

#define BUFFER_SIZE PAGE_SIZE

static file_t *getCWD();

static void *createBuffer();
static void *createDirectory();
static void *createRegularFile();
static void *createSemaphore();
static void *createMutex();
static void *createExecutable();

static void removeDirectory(file_t *file);
static void removeBuffer(file_t *file);
static void removeRegularFile(file_t *file);
static void removeSemaphore(file_t *file);
static void removeMutex(file_t *file);
static void removeExecutable(file_t *file);

static file_t *newFile(char name[], int type);

typedef struct directory {
    file_t *first;
} directory_t;

typedef struct buffer {
    char content[BUFFER_SIZE];
} buffer_t;

typedef struct regular_file {
    char *content;
    uint32_t size;
    uint32_t totalSize;
} regular_file_t;

typedef struct semaphore {
    sem_t semaphore;
} sem_file_t;

typedef struct mutex {
    mutex_t mutex;
} mutex_file_t;

typedef struct executable {
    void * pointer;
} executable_t;

file_t *root;
opened_file_t *firstOpenedFile;

void init_fileSystem() {
    root = newFile("root", DIRECTORY);
    root->directory = NULL;

    firstOpenedFile = NULL;

    makeFile("/dev", DIRECTORY);
    makeFile("/dev/shm", DIRECTORY);
    makeFile("/dev/stdin", BUFFER);
    makeFile("/TestFile", REGULAR_FILE);
}

static file_t *makeFileR(char path[], file_t *dir, int type,
                         char name[MAX_NAME_LENGTH], int getFile,
                         int pathLength) {
    int i;

    for (i = 0; path[i] != '/' && path[i] != 0 && i <= MAX_NAME_LENGTH; i++) {
        name[i] = path[i];
    }

    if (i > MAX_NAME_LENGTH || pathLength + i > MAX_PATH_LENGTH) return NULL;

    name[i] = 0;

    if (i == 0) {
        if (path[i] == 0)
            return dir;
        else
            return NULL;
    }

    file_t *prevFile = NULL;
    file_t *currFile = ((directory_t *)(dir->implementation))->first;
    int cmp;

    if (name[0] == '.' && name[1] == 0) {
        cmp = 0;
        currFile = dir;
    } else if (name[0] == '.' && name[1] == '.' && name[2] == 0) {
        cmp = 0;
        if (dir == root)
            currFile = root;
        else
            currFile = dir->directory;
    } else {
        while (currFile != NULL && (cmp = strcmp(currFile->name, name)) < 0) {
            prevFile = currFile;
            currFile = currFile->next;
        }
    }

    if (path[i] == 0) {
        if (currFile != NULL && cmp == 0)
            return currFile;
        else {
            if (getFile) return NULL;

            file_t *file = newFile(name, type);

            if (file == NULL) return NULL;

            file->next = currFile;
            file->directory = dir;
            if (prevFile != NULL)
                prevFile->next = file;
            else
                ((directory_t *)(dir->implementation))->first = file;

            return file;
        }
    }

    if (path[i] == '/' && cmp == 0) {
        if (currFile == NULL || currFile->type != DIRECTORY) return NULL;

        return makeFileR(path + i + 1, currFile, type, name, getFile,
                         pathLength + i + 1);
    }

    return NULL;
}

file_t *makeFile(char *path, int type) {
    char name[MAX_NAME_LENGTH];
    if (*path == '/')
        return makeFileR(path + 1, root, type, name, 0, 0);
    else
        return makeFileR(path, getCWD(), type, name, 0, 0);

    return NULL;
}

file_t *getFile(char *path) {
    char name[MAX_NAME_LENGTH];
    if (*path == '/')
        return makeFileR(path + 1, root, 0, name, 1, 0);
    else
        return makeFileR(path, getCWD(), 0, name, 1, 0);

    return NULL;
}

static file_t *newFile(char name[], int type) {
    file_t *newFile = (file_t *)malloc(sizeof(file_t));

    if (newFile == NULL) return NULL;

    strcpy(newFile->name, name);
    newFile->next = NULL;
    newFile->directory = NULL;
    newFile->type = type;

    if (type == DIRECTORY)
        newFile->implementation = createDirectory();
    else if (type == REGULAR_FILE)
        newFile->implementation = createRegularFile();
    else if (type == BUFFER)
        newFile->implementation = createBuffer();
    else if (type == SEMAPHORE)
        newFile->implementation = createSemaphore();
    else if (type == MUTEX)
        newFile->implementation = createMutex();
    else if (type == EXECUTABLE)
        newFile->implementation = createExecutable();

    return newFile;
}

static void *createBuffer() {
    buffer_t *buff = malloc(sizeof(buffer_t));

    if (buff == NULL) return NULL;

    return (void *)buff;
}

static void *createDirectory() {
    directory_t *dir = malloc(sizeof(directory_t));

    if (dir == NULL) return NULL;

    dir->first = NULL;
    return (void *)dir;
}

static void *createRegularFile() {
    regular_file_t *file = malloc(sizeof(regular_file_t));

    if (file == NULL) return NULL;

    file->content = getMemory(PAGE_SIZE);

    if (file->content == NULL) {
        free(file);
        return NULL;
    }

    file->size = 0;
    file->totalSize = PAGE_SIZE;
    return (void *)file;
}

static void *createSemaphore() {
    sem_file_t *file = malloc(sizeof(sem_file_t));

    if (file == NULL) return NULL;

    file->semaphore = sem_create(0);

    return (void *)file;
}

static void *createMutex() {
    mutex_file_t *file = malloc(sizeof(mutex_file_t));

    if (file == NULL) return NULL;

    file->mutex = mutex_create();

    return (void *)file;
}

static void *createExecutable() {
    executable_t *file = malloc(sizeof(executable_t));

    if (file == NULL) return NULL;

    file->pointer = NULL;

    return (void *)file;
}

static file_t *removeFileR(file_t *currFile, file_t *targetFile) {
    if (currFile == NULL) return currFile;

    if (currFile == targetFile) {
        if (currFile->type == DIRECTORY)
            removeDirectory(currFile);
        else if (currFile->type == REGULAR_FILE)
            removeRegularFile(currFile);
        else if (currFile->type == BUFFER)
            removeBuffer(currFile);
        else if (currFile->type == SEMAPHORE)
            removeSemaphore(currFile);
        else if (currFile->type == MUTEX)
            removeMutex(currFile);
        else if (currFile->type == EXECUTABLE)
            removeExecutable(currFile);

        free(currFile);
        return currFile->next;
    }
    currFile->next = removeFileR(currFile->next, targetFile);
    return currFile;
}

void removeFile(file_t *file) {
    if (file == NULL) return;

    directory_t *dir = (directory_t *)(file->directory->implementation);
    if (file->type == DIRECTORY) {
        file_t *currFile = ((directory_t *)(file->implementation))->first;
        while (currFile != NULL) {
            removeFile(currFile);
            currFile = currFile->next;
        }
    }
    dir->first = removeFileR(dir->first, file);
}

void removeFileFromPath(char *path) { removeFile(getFile(path)); }

static void removeDirectory(file_t *file) { free(file->implementation); }

static void removeBuffer(file_t *file) { free(file->implementation); }

static void removeRegularFile(file_t *file) {
    regular_file_t *regularFile = (regular_file_t *)(file->implementation);
    freeMemory(regularFile->content);
    free(regularFile);
}

static void removeSemaphore(file_t *file) {
    sem_file_t *semaphore = (sem_file_t *)(file->implementation);
    sem_delete(semaphore->semaphore);
    free(semaphore);
}

static void removeMutex(file_t *file) {
    mutex_file_t *mutex = (mutex_file_t *)(file->implementation);
    mutex_delete(mutex->mutex);
    free(mutex);
}

static void removeExecutable(file_t *file) { free(file->implementation); }

file_t *getRoot() { return root; }

void getCWDPath(char *pathBuff) {
    file_t *current = getCWD();
    file_t *dirs[MAX_NAME_LENGTH / 2];
    pathBuff[0] = 0;
    int dirCount = 0;

    while (current != root) {
        dirs[dirCount] = current;
        dirCount++;
        current = current->directory;
    }

    strcat(pathBuff, "/");
    while (dirCount > 0) {
        dirCount--;
        strcat(pathBuff, dirs[dirCount]->name);
        if (dirCount > 0) strcat(pathBuff, "/");
    }
}

static file_t *getCWD() {
    process_t *pcb = getProcessByPID(getCurrentPID());
    return pcb->cwd;
}

void changeCWD(char *path) {
    file_t *file = getFile(path);
    if (file != NULL && file->type == DIRECTORY) {
        process_t *pcb = getProcessByPID(getCurrentPID());
        pcb->cwd = file;
    }
}

//////////// I/O OPERATIONS ////////////

typedef struct opened_buffer {
    uint32_t writeCursor;
    uint32_t readCursor;
    int hasEOF;
    mutex_t mutex;
    sem_t writerSem;
    sem_t readerSem;
    int allowWriters;
    int allowReaders;
} opened_buffer_t;

static opened_buffer_t *openBuffer();
static void closeBuffer(opened_buffer_t *openedBuffer);
static uint32_t writeBuffer(opened_file_t *openedFile, char *buff,
                            uint32_t bytes);
static uint32_t writeRegularFile(opened_file_t *openedFile, char *buff,
                                 uint32_t bytes);

static uint32_t readRegularFile(opened_file_t *openedFile, char *buff,
                                uint32_t bytes, uint32_t position);
static uint32_t readBuffer(opened_file_t *openedFile, char *buff,
                           uint32_t bytes);
static uint32_t readExecutable(opened_file_t *openedFile, char *buff);

static fd_t *openFile(file_t *file, int mode) {
    if (file == NULL || file->type == DIRECTORY) return NULL;

    fd_t *fd = malloc(sizeof(fd_t));
    if (fd == NULL) return NULL;

    opened_file_t *openedFile = firstOpenedFile;
    while (openedFile != NULL && openedFile->file != file)
        openedFile = openedFile->next;

    if (openedFile == NULL) {
        openedFile = malloc(sizeof(opened_file_t));
        if (openedFile == NULL) {
            free(fd);
            return NULL;
        }

        openedFile->next = firstOpenedFile;
        firstOpenedFile = openedFile;
        openedFile->file = file;
        openedFile->readers = 0;
        openedFile->writers = 0;

        if (file->type == BUFFER) {
            openedFile->implementation = (void *)openBuffer();
            if (openedFile->implementation == NULL) {
                free(fd);
                free(openedFile);
                return NULL;
            }
        }
    }

    if (mode == O_RDONLY || mode == O_RDWR) openedFile->readers++;
    if (mode == O_WRONLY || mode == O_RDWR) {
        openedFile->writers++;
        if (file->type == BUFFER)
            ((opened_buffer_t *)(openedFile->implementation))->hasEOF = 0;
    }

    fd->openedFile = openedFile;
    fd->mode = mode;
    fd->cursor = 0;

    return fd;
}

fd_t *openFileFromPath(char *path, int mode) {
    return openFile(getFile(path), mode);
}

int openFileToFD(file_t *file, int mode) {
    if (getFreeFD(getCurrentPID()) == -1) return -1;

    fd_t *fd = openFile(file, mode);
    if (fd == NULL) return -1;

    return registerFD(getCurrentPID(), fd);
}

int openFileFromPathToFD(char *path, int mode) {
    return openFileToFD(getFile(path), mode);
}

void closeFile(fd_t *fd) {
    if (fd == NULL) return;

    opened_file_t *openedFile = fd->openedFile;
    int mode = fd->mode;

    if (mode == O_RDONLY || mode == O_RDWR) openedFile->readers--;
    if (mode == O_WRONLY || mode == O_RDWR) {
        openedFile->writers--;
        if (openedFile->writers == 0 && openedFile->file->type == BUFFER) {
            ((opened_buffer_t *)(openedFile->implementation))->hasEOF = 1;
        }
    }

    if (openedFile->readers == 0 && openedFile->writers == 0) {
        opened_file_t *previous = NULL;
        opened_file_t *current = firstOpenedFile;

        while (current != openedFile) {
            if (current == NULL) return;
            previous = current;
            current = current->next;
        }

        if (previous == NULL)
            firstOpenedFile = current->next;
        else
            previous->next = current->next;

        if (openedFile->file->type == BUFFER) {
            closeBuffer(openedFile->implementation);

            if (openedFile->file->directory == NULL) {  // Es un unnamed pipe
                removeBuffer(openedFile->file);
                free(openedFile->file);
            }
        }

        free(openedFile);
    }

    free(fd);
}

void closeFileFromFD(int fdIndex) {
    closeFile(unregisterFD(getCurrentPID(), fdIndex));
}

static opened_buffer_t *openBuffer() {
    opened_buffer_t *openedBuffer = malloc(sizeof(opened_buffer_t));
    if (openedBuffer == NULL) return NULL;
    openedBuffer->writeCursor = 0;
    openedBuffer->readCursor = 0;
    openedBuffer->hasEOF = 0;
    openedBuffer->mutex = mutex_create();
    openedBuffer->readerSem = sem_create(0);
    openedBuffer->writerSem = sem_create(1);
    openedBuffer->allowReaders = 0;
    openedBuffer->allowWriters = 1;
    return openedBuffer;
}

static void closeBuffer(opened_buffer_t *openedBuffer) {
    if (openedBuffer == NULL) return;

    mutex_delete(openedBuffer->mutex);
    sem_delete(openedBuffer->readerSem);
    sem_delete(openedBuffer->writerSem);

    free(openedBuffer);
}

void openUnnamedPipe(int fd[2]) {
    file_t *pipe = newFile("pipe", BUFFER);
    if (pipe == NULL) {
        fd[0] = -1;
        fd[1] = -1;
        return;
    }

    fd[0] = openFileToFD(pipe, O_RDONLY);
    fd[1] = openFileToFD(pipe, O_WRONLY);

    if (fd[0] == -1 && fd[1] == -1) {
        removeBuffer(pipe);
        free(pipe);
    } else if (fd[0] == -1 || fd[1] == -1) {
        closeFileFromFD(fd[0]);
        closeFileFromFD(fd[1]);
        fd[0] = -1;
        fd[1] = -1;
    }
}

void cloneFD(int fdFrom, int fdTo, void *processNoCast) {
    if (fdFrom < 0 || fdFrom >= MAX_FD_COUNT || fdTo < 0 || fdTo >= MAX_FD_COUNT)
        return;

    process_t *destiny = (process_t *)processNoCast;
    if (destiny->fd_table[fdTo] != NULL) return;

    process_t *origin = getProcessByPID(getCurrentPID());
    fd_t *originFD = origin->fd_table[fdFrom];
    if (originFD == NULL) return;

    fd_t *destinyFD = openFile(originFD->openedFile->file, originFD->mode);
    if (destinyFD == NULL) return;

    destiny->fd_table[fdTo] = destinyFD;
}

uint32_t writeFile(fd_t *fd, char *buff, uint32_t bytes) {
    if (bytes == 0 || fd == NULL) return 0;

    if (fd->mode != O_WRONLY && fd->mode != O_RDWR) return 0;

    if (fd->openedFile->file->type == REGULAR_FILE)
        return writeRegularFile(fd->openedFile, buff, bytes);

    if (fd->openedFile->file->type == BUFFER)
        return writeBuffer(fd->openedFile, buff, bytes);

    return 0;
}

static uint32_t writeRegularFile(opened_file_t *openedFile, char *buff,
                                 uint32_t bytes) {
    regular_file_t *regularFile =
        (regular_file_t *)(openedFile->file->implementation);
    int availableBytes = regularFile->totalSize - regularFile->size;
    availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

    if (availableBytes <= 0) return 0;

    for (int i = 0; i < availableBytes; i++) {
        regularFile->content[regularFile->size] = buff[i];
        regularFile->size++;
    }

    return availableBytes;
}

static uint32_t writeBuffer(opened_file_t *openedFile, char *buff,
                            uint32_t bytes) {
    opened_buffer_t *openedBuffer =
        (opened_buffer_t *)(openedFile->implementation);
    sem_wait(openedBuffer->writerSem);
    mutex_lock(openedBuffer->mutex);

    buffer_t *buffer = (buffer_t *)(openedFile->file->implementation);
    int availableBytes = openedBuffer->readCursor - openedBuffer->writeCursor;
    availableBytes =
        (availableBytes > 0) ? availableBytes : BUFFER_SIZE + availableBytes;
    int bytesToWrite = (availableBytes > bytes) ? bytes : availableBytes;

    if (bytesToWrite < availableBytes)
        sem_signal(openedBuffer->writerSem);
    else
        openedBuffer->allowWriters = 0;

    for (int i = 0; i < bytesToWrite; i++) {
        buffer->content[openedBuffer->writeCursor] = buff[i];
        openedBuffer->writeCursor++;
        if (openedBuffer->writeCursor == BUFFER_SIZE)
            openedBuffer->writeCursor = 0;
    }

    if (openedBuffer->allowReaders == 0) {
        openedBuffer->allowReaders = 1;
        sem_signal(openedBuffer->readerSem);
    }

    mutex_unlock(openedBuffer->mutex);
    return bytesToWrite;
}

uint32_t readFile(fd_t *fd, char *buff, uint32_t bytes) {
    if (bytes == 0 || fd == NULL) return 0;

    if (fd->mode != O_RDONLY && fd->mode != O_RDWR) return 0;

    if (fd->openedFile->file->type == REGULAR_FILE) {
        int ret = readRegularFile(fd->openedFile, buff, bytes, fd->cursor);
        fd->cursor += ret;
        return ret;
    }

    if (fd->openedFile->file->type == BUFFER)
        return readBuffer(fd->openedFile, buff, bytes);

    if (fd->openedFile->file->type == EXECUTABLE)
        return readExecutable(fd->openedFile, buff);

    return 0;
}

static uint32_t readRegularFile(opened_file_t *openedFile, char *buff,
                                uint32_t bytes, uint32_t position) {
    regular_file_t *regularFile =
        (regular_file_t *)(openedFile->file->implementation);
    int availableBytes = regularFile->size - position;
    availableBytes = (availableBytes > bytes) ? bytes : availableBytes;

    if (availableBytes <= 0) return 0;

    for (int i = 0; i < availableBytes; i++) {
        buff[i] = regularFile->content[position];
        position++;
    }

    return availableBytes;
}

static uint32_t readBuffer(opened_file_t *openedFile, char *buff,
                           uint32_t bytes) {
    opened_buffer_t *openedBuffer =
        (opened_buffer_t *)(openedFile->implementation);
    sem_wait(openedBuffer->readerSem);
    mutex_lock(openedBuffer->mutex);

    buffer_t *buffer = (buffer_t *)(openedFile->file->implementation);
    int availableBytes = openedBuffer->writeCursor - openedBuffer->readCursor;
    availableBytes =
        (availableBytes >= 0) ? availableBytes : BUFFER_SIZE + availableBytes;
    int bytesToRead = (availableBytes > bytes) ? bytes : availableBytes;

    if (bytesToRead < availableBytes || openedBuffer->hasEOF)
        sem_signal(openedBuffer->readerSem);
    else
        openedBuffer->allowReaders = 0;

    for (int i = 0; i < bytesToRead; i++) {
        buff[i] = buffer->content[openedBuffer->readCursor];
        openedBuffer->readCursor++;
        if (openedBuffer->readCursor == BUFFER_SIZE)
            openedBuffer->readCursor = 0;
    }

    if (openedBuffer->allowWriters == 0) {
        openedBuffer->allowWriters = 1;
        sem_signal(openedBuffer->writerSem);
    }

    mutex_unlock(openedBuffer->mutex);
    return bytesToRead;
}

static uint32_t readExecutable(opened_file_t *openedFile, char *buff) {
    executable_t *executable = (executable_t*)(openedFile->file->implementation);
    void **pointer = (void**)buff;
    *pointer = executable->pointer;
    return sizeof(void*);
}

uint32_t writeToFD(int fdIndex, char *buff, uint32_t bytes) {
    return writeFile(getFD(getCurrentPID(), fdIndex), buff, bytes);
}

uint32_t readFromFD(int fdIndex, char *buff, uint32_t bytes) {
    return readFile(getFD(getCurrentPID(), fdIndex), buff, bytes);
}

void semCreate(char *name, int value) {
    char tmp[MAX_NAME_LENGTH];
    strcpy(tmp, "/dev/shm/");
    strcat(tmp, name);
    if (getFile(tmp) == NULL) {
        file_t *file = makeFile(tmp, SEMAPHORE);
        if (file != NULL)
            sem_set_value(((sem_file_t *)(file->implementation))->semaphore,
                          value);
    }
}

void semDelete(char *name) {
    char tmp[MAX_NAME_LENGTH];
    strcpy(tmp, "/dev/shm/");
    strcat(tmp, name);
    file_t *file = getFile(tmp);
    if (file != NULL && file->type == SEMAPHORE) removeFile(file);
}

int semOpen(char *name) {
    char tmp[MAX_NAME_LENGTH];
    strcpy(tmp, "/dev/shm/");
    strcat(tmp, name);
    file_t *file = getFile(tmp);
    if (file != NULL && file->type == SEMAPHORE)
        return openFileToFD(file, O_RDWR);
    return -1;
}

void semClose(int fdIndex) {
    fd_t *sem = getFD(getCurrentPID(), fdIndex);
    if (sem != NULL && sem->openedFile->file->type == SEMAPHORE) closeFile(sem);
}

void semSet(int fdIndex, int value) {
    fd_t *fd = getFD(getCurrentPID(), fdIndex);

    if (fd != NULL && fd->openedFile->file->type == SEMAPHORE)
        sem_set_value(
            ((sem_file_t *)(fd->openedFile->file->implementation))->semaphore,
            value);
}

void semWait(int fdIndex) {
    fd_t *fd = getFD(getCurrentPID(), fdIndex);

    if (fd != NULL && fd->openedFile->file->type == SEMAPHORE)
        sem_wait(
            ((sem_file_t *)(fd->openedFile->file->implementation))->semaphore);
}

void semSignal(int fdIndex) {
    fd_t *fd = getFD(getCurrentPID(), fdIndex);

    if (fd != NULL && fd->openedFile->file->type == SEMAPHORE)
        sem_signal(
            ((sem_file_t *)(fd->openedFile->file->implementation))->semaphore);
}

void mutexCreate(char *name) {
    char tmp[MAX_NAME_LENGTH];
    strcpy(tmp, "/dev/shm/");
    strcat(tmp, name);
    makeFile(tmp, MUTEX);
}

void mutexDelete(char *name) {
    char tmp[MAX_NAME_LENGTH];
    strcpy(tmp, "/dev/shm/");
    strcat(tmp, name);
    file_t *file = getFile(tmp);
    if (file != NULL && file->type == MUTEX) removeFile(file);
}

int mutexOpen(char *name) {
    char tmp[MAX_NAME_LENGTH];
    strcpy(tmp, "/dev/shm/");
    strcat(tmp, name);
    file_t *file = getFile(tmp);
    if (file != NULL && file->type == MUTEX) return openFileToFD(file, O_RDWR);
    return -1;
}

void mutexClose(int fdIndex) {
    fd_t *mutex = getFD(getCurrentPID(), fdIndex);
    if (mutex != NULL && mutex->openedFile->file->type == MUTEX)
        closeFile(mutex);
}

void mutexLock(int fdIndex) {
    fd_t *fd = getFD(getCurrentPID(), fdIndex);

    if (fd != NULL && fd->openedFile->file->type == MUTEX)
        mutex_lock(
            ((mutex_file_t *)(fd->openedFile->file->implementation))->mutex);
}

void mutexUnlock(int fdIndex) {
    fd_t *fd = getFD(getCurrentPID(), fdIndex);

    if (fd != NULL && fd->openedFile->file->type == MUTEX)
        mutex_unlock(
            ((mutex_file_t *)(fd->openedFile->file->implementation))->mutex);
}

void execCreate(char * name, void * pointer) {
    file_t * file = makeFile(name, EXECUTABLE);
    if (file != NULL && file->type == EXECUTABLE) {
      executable_t * executable = (executable_t*)(file->implementation);
      executable->pointer = pointer;
    }
}

///// TESTING FUNCTIONS /////

void listDir(char *path) {
    file_t *file = getFile(path);

    if (file == NULL) return;

    if (file->type != DIRECTORY) return;

    char pathBuff[MAX_PATH_LENGTH];
    getCWDPath(pathBuff);
    printf("Directorio actual: %s\n", pathBuff);

    file_t *current = ((directory_t *)(file->implementation))->first;
    while (current != NULL) {
        printf("\n%s", current->name);
        if (current->type == DIRECTORY)
            printf(" (directory)");
        else if (current->type == REGULAR_FILE)
            printf(" (regular file)");
        else if (current->type == BUFFER)
            printf(" (buffer)");
        else if (current->type == SEMAPHORE)
            printf(" (sempahore)");
        current = current->next;
    }
}
