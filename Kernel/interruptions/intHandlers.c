#include <stdint.h>
#include "../asm/libasm.h"
#include "../drivers/include/console.h"
#include "../drivers/include/kb_driver.h"
#include "../drivers/include/speaker.h"
#include "../drivers/include/timer.h"
#include "../drivers/include/video_vm.h"
#include "../include/fileSystem.h"
#include "../include/memoryManagerTest.h"
#include "../include/roundRobinWithPriority.h"
#include "../include/scheduler.h"
#include "../include/sleep.h"

#include "../drivers/include/kb_driver.h"

struct bgnode {
    thread_t* thread;
    struct bgnode* next;
};

typedef struct bgnode* bgNode;
bgNode bgQueue = NULL;
int foregroundPID = 0;

int getForegroundPID() { return foregroundPID; }

void giveForeground(int pid) {
    // if(!isValidProcess(pid) || pid == foregroundPID)
    if (pid == foregroundPID) return;

    foregroundPID = pid;
    bgNode curr = bgQueue;
    bgNode prev = NULL, aux;
    while (curr != NULL) {
        if (curr->thread->process == pid) {
            scheduler_enqueue(curr->thread, 0);
            if (prev == NULL) {  // primero
                bgQueue = curr->next;
            } else {
                prev->next = curr->next;
            }
            aux = curr;
            curr = curr->next;
            freeMemory(aux);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

int int80Handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8) {
    switch (rdi) {
        case 1:  // Exit
            while (1) {
                printf("Codigo de salida: %d\n", rsi);
                _halt();
            }
            break;
        case 3:    //Read
            if (rsi == 0 && getProcessByPID(getCurrentPID())->fd_table[rsi] == NULL && getCurrentPID() != foregroundPID) {  // No es el current
                thread_t* current = scheduler_dequeue_current();

                bgNode n = getMemory(sizeof(struct bgnode));
                n->thread = current;
                if (bgQueue == NULL)
                    bgQueue = n;
                else
                    bgQueue->next = n;

                _force_scheduler();
            }
            return readFromFD(rsi, (char*)rdx, rcx);

            break;
        case 4:  // Write
            if ((rsi == 1 || rsi == 2) && getProcessByPID(getCurrentPID())->fd_table[rsi] == NULL) {
                if (rsi == 2) {  // STD_ERR
                    setFontColor(0x000000);
                    setBackgroundColor(0xDC143C);
                }
                char* str = (char*)rdx;
                int i;
                for (i = 0; i < rcx; i++) printChar(str[i]);
                return i;
            } else {
                return writeToFD(rsi, (char*)rdx, rcx);
            }

            break;
        case 5:  // ClearScreen
            clearScreen();
            return 0;
            break;
        case 6:  // setFontColor
            setFontColor(rsi);
            // printf("color: %d", rsi);
            return 0;
            break;
        case 7:  // setBackgroundColor
            setBackgroundColor(rsi);
            return 0;
            break;
        case 8:  // setFontSize
            setFontSize(rsi);
            return 0;
            break;
        case 9:
            return getFontColor();
            break;
        case 10:
            return getBackgroundColor();
            break;
        case 11:
            return getFontSize();
            break;
        case 12:  // drawPixel
            drawPixel(rsi, rdx, rcx);
            return 0;
            break;
        case 13:  // setCursor
            setCursor(rsi, rdx);
            return 0;
            break;
        case 14:  // setGraphicCursorStatus
            setGraphicCursorStatus(rsi);
            return 0;
            break;
        case 15:
            return SCREEN_HEIGHT;
            break;
        case 16:
            return SCREEN_WIDTH;
            break;
        case 17:  // sys_memoryManagerTest
            memoryManagerTest(rsi);
            break;
        case 18:  // sys_listDir
            listDir((char*)rsi);
            break;
        case 19:  // sys_makeFile
            makeFile((char*)rsi, rdx);
            break;
        case 20:  // sys_removeFile
            removeFileFromPath((char*)rsi);
            break;
        case 21:  // sys_open
            return openFileFromPathToFD((char*)rsi, rdx);
            break;
        case 22:  // sys_close
            closeFileFromFD(rsi);
            break;
        case 23:  // sys_semCreate
            semCreate((char*)rsi, rdx);
            break;
        case 24:  // sys_semDelete
            semDelete((char*)rsi);
            break;
        case 25:  // sys_semOpen
            return semOpen((char*)rsi);
            break;
        case 26:  // sys_semClose
            semClose(rsi);
            break;
        case 27:  // sys_semSet
            semSet(rsi, rdx);
            break;
        case 28:  // sys_semWait
            semWait(rsi);
            break;
        case 29:  // sys_semSignal
            semSignal(rsi);
            break;
        case 30:  // sys_mutexCreate
            mutexCreate((char*)rsi);
            break;
        case 31:  // sys_mutexDelete
            mutexDelete((char*)rsi);
            break;
        case 32:  // sys_mutexOpen
            return mutexOpen((char*)rsi);
            break;
        case 33:  // sys_mutexClose
            mutexClose(rsi);
            break;
        case 34:  // sys_mutexLock
            mutexLock(rsi);
            break;
        case 35:  // sys_mutexUnlock
            mutexUnlock(rsi);
            break;
        case 36:  // sys_chdir
            changeCWD((char*)rsi);
            break;
        case 37:  // sys_getcwd
            getCWDPath((char*)rsi);
            break;
        case 38:  // sys_pipe
            openUnnamedPipe((int*)rsi);
            break;
        case 39:  // sys_execCreate
            execCreate((char*)rsi, (void*)rdx);
            break;
        case 40:  // sleep
            sleepCurrentThread(rsi);
            break;
        case 41:  // ps
            listProcess((ps_struct*)rsi, (int*)rdx);
            break;
        case 42:  // new process
            return createProcess((char*)rsi, (void*)rdx, (char**)rcx, 4, 4, (int**)r8);
            break;
        case 43:  // waitpid
            return waitpid(rsi);
            break;
        case 44:  // getHeapSize
            return getHeapSize(getCurrentPID());
            break;
        case 45:  // getHeapBase
            return (uint64_t)getHeapBase(getCurrentPID());
            break;
        case 46:  // killProcess
            killProcess((int)rsi, -1);
            break;
        case 47:  // exit
            killProcess(getCurrentPID(), (int)rsi);
            break;
        case 48:  // giveForeground
            giveForeground((int)rsi);
            break;
        case 49:  // dup2
            dup2(rsi, rdx);
            break;
        case 50:  // new thread
            return createThread(getProcessByPID(getCurrentPID()), (void*)rsi,
                                (void*)rdx, 4, FALSE)
                ->tid;
            break;
        case 51:  // thread join
            threadJoin(rsi, (void**)rdx);
            break;
        case 52:  // thread cancel
            killThread(getCurrentPID(), rsi, FALSE);
            break;
        case 100:  // timerAppend, return 0 if successful, -1 if error
            // printf("\nParametros: RAX %d rsi %d rdx %d RDX %d\n", rdi, rsi,
            // rdx, rcx);
            return timer_appendFunction((function)rsi, rdx);
            break;
        case 101:  // timerRemove, return 0 if successful, -1 if error
            return timer_removeFunction((function)rsi);
            break;
        case 102:  // beep
            beep(rsi, rdx);
            return 0;
            break;
        case 103:  // nosound
            nosound();
            return 0;
            break;
        case 200:  // RTC
            //_outportb(0x70, rsi);
            // return _inportb(0x71);
            return _RTC(rsi);
            break;
    }
    return 0;
}
