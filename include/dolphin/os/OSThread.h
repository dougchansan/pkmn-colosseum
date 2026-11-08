#ifndef DOLPHIN_OS_OSTHREAD_H
#define DOLPHIN_OS_OSTHREAD_H

#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"

typedef struct OSThread OSThread;
typedef struct OSThreadQueue OSThreadQueue;
typedef struct OSThreadLink OSThreadLink;
typedef struct OSMutex OSMutex;
typedef struct OSMutexQueue OSMutexQueue;
typedef struct OSMutexLink OSMutexLink;

typedef s32 (*OSSwitchThreadCallback)(OSThread* from, OSThread* to);

struct OSThreadQueue {
    OSThread* head;
    OSThread* tail;
};

struct OSThreadLink {
    OSThread* next;
    OSThread* prev;
};

struct OSMutexQueue {
    OSMutex* head;
    OSMutex* tail;
};

struct OSMutexLink {
    OSMutex* next;
    OSMutex* prev;
};

struct OSThread {
    /* 0x000 */ OSContext context;
    /* 0x2C8 */ u16 state;
    /* 0x2CA */ u16 attr;
    /* 0x2CC */ s32 suspend;
    /* 0x2D0 */ s32 priority;
    /* 0x2D4 */ s32 base;
    /* 0x2D8 */ u32 val;
    /* 0x2DC */ OSThreadQueue* queue;
    /* 0x2E0 */ OSThreadLink link;
    /* 0x2E8 */ OSThreadQueue queueJoin;
    /* 0x2F0 */ OSMutex* mutex;
    /* 0x2F4 */ OSMutexQueue queueMutex;
    /* 0x2FC */ OSThreadLink linkActive;
    /* 0x304 */ u32* stackBase;
    /* 0x308 */ u32* stackEnd;
    /* 0x30C */ s32 error;
    /* 0x310 */ void* specific[2];
};

#define OS_THREAD_STATE_EXITED   0
#define OS_THREAD_STATE_READY    1
#define OS_THREAD_STATE_RUNNING  2
#define OS_THREAD_STATE_WAITING  4
#define OS_THREAD_STATE_MORIBUND 8

#define OS_THREAD_ATTR_DETACH 0x0001

#define OS_PRIORITY_MIN  0
#define OS_PRIORITY_MAX 31
#define OS_PRIORITY_IDLE OS_PRIORITY_MAX

#define OS_THREAD_STACK_MAGIC 0xDEADBABE

/* OSMutex - defined here for thread/mutex linkage */
struct OSMutex {
    /* 0x00 */ OSThreadQueue queue;
    /* 0x08 */ OSThread* thread;
    /* 0x0C */ s32 count;
    /* 0x10 */ OSMutexLink link;
};

void __OSThreadInit(void);
void OSInitThreadQueue(OSThreadQueue* queue);
s32  OSDisableScheduler(void);
s32  OSEnableScheduler(void);
void __OSReschedule(void);
void OSCancelThread(OSThread* thread);
void OSClearStack(u8 val);
OSThread* OSGetCurrentThread(void);
BOOL OSIsThreadSuspended(OSThread* thread);
BOOL OSIsThreadTerminated(OSThread* thread);

BOOL OSCreateThread(OSThread* thread, void* (*func)(void*), void* param,
                    void* stack, u32 stackSize, s32 priority, u16 attr);
void OSExitThread(void* val);
BOOL OSJoinThread(OSThread* thread, u32* val);
void OSDetachThread(OSThread* thread);
s32  OSResumeThread(OSThread* thread);
s32  OSSuspendThread(OSThread* thread);
void OSSleepThread(OSThreadQueue* queue);
void OSWakeupThread(OSThreadQueue* queue);
BOOL OSSetThreadPriority(OSThread* thread, s32 priority);
s32  OSGetThreadPriority(OSThread* thread);
void OSYieldThread(void);
OSThread* OSSetIdleFunction(void (*idleFunction)(void*), void* param,
                            void* stack, u32 stackSize);
OSThread* OSGetIdleFunction(void);
s32 OSCheckActiveThreads(void);

void OSInitContext(OSContext* context, u32 pc, u32 sp);

void __OSUnlockAllMutex(OSThread* thread);
s32  __OSGetEffectivePriority(OSThread* thread);
void __OSPromoteThread(OSThread* thread, s32 priority);

#endif /* DOLPHIN_OS_OSTHREAD_H */
