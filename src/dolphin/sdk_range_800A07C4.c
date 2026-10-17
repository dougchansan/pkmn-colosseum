/**
 * @file sdk_range_800A07C4.c
 * @brief dolphin-sdk code, 0x800A07C4 - 0x800A13E8 (19 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSThread.h"

extern u8 _stack_addr[];
extern u8 _stack_end[];

#if !defined(SDK_800A0D00_ONLY)
static OSThreadQueue RunQueue_803FB898[32];
static OSThread IdleThread;
static OSThread DefaultThread;
static OSContext IdleContext;
static volatile u32 RunQueueBits_8047A760;
static volatile int RunQueueHint_8047A764;
static s32 Reschedule_8047A768;
#endif

extern OSSwitchThreadCallback SwitchThreadCallback_804789A8;

#define OS_CURRENT_THREAD (*(OSThread**)0x800000E4)
#define OS_FPU_CONTEXT (*(OSContext**)0x800000D8)

static OSThreadQueue* const OS_ACTIVE_THREAD_QUEUE = (OSThreadQueue*)0x800000DC;

#define ADD_TAIL(queue, thread, link)              \
    do {                                            \
        OSThread* previous = (queue)->tail;         \
        if (previous == NULL) {                     \
            (queue)->head = (thread);               \
        } else {                                    \
            previous->link.next = (thread);         \
        }                                           \
        (thread)->link.prev = previous;             \
        (thread)->link.next = NULL;                 \
        (queue)->tail = (thread);                   \
    } while (0)

typedef struct SramControl {
    u8 sram[0x40];
    u32 offset;
    BOOL enabled;
    BOOL locked;
    BOOL sync;
} SramControl;

extern u32 Scb_803FB840[0x54 / sizeof(u32)];

#if !defined(SDK_800A0D00_ONLY)
static inline BOOL ReadSram(void* buffer) {
    BOOL err;
    u32 command;

    DCInvalidateRange(buffer, 0x40);
    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    command = 0x20000100;
    err = FALSE;
    err |= !EXIImm(0, &command, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !EXIDma(0, buffer, 0x40, 0, NULL);
    err |= !EXISync(0);
    err |= !EXIDeselect(0);
    EXIUnlock(0);
    return !err;
}

void __OSInitSram(void) {
    SramControl* control = (SramControl*)Scb_803FB840;

    control->locked = control->enabled = FALSE;
    control->sync = ReadSram(control->sram);
    control->offset = 0x40;
}
#endif

/* Under SDK_800A0D00_ONLY these OSRtc.c head functions (defined in
   src/dolphin/os/OSRtc.c) stay visible for inlining but are not emitted.
   RULE-EXCEPTION(user-approved): static inline copies of another unit's functions — see docs/RULE_EXCEPTIONS.md */
#if defined(SDK_800A0D00_ONLY)
#define OSRTC_HEAD_FN static inline
#else
#define OSRTC_HEAD_FN
#endif

OSRTC_HEAD_FN void* __OSLockSram(void) {
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSRestoreInterrupts(BOOL level);
    BOOL enabled;
    void* result = Scb_803FB840;

    enabled = OSDisableInterrupts();
    if ((s32)Scb_803FB840[0x12] != 0) {
        OSRestoreInterrupts(enabled);
        result = 0;
    } else {
        Scb_803FB840[0x11] = enabled;
        Scb_803FB840[0x12] = 1;
    }

    return result;
}

OSRTC_HEAD_FN void* __OSLockSramEx(void) {
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSRestoreInterrupts(BOOL level);
    BOOL enabled;
    u32* sram = Scb_803FB840;
    u32* lock;

    enabled = OSDisableInterrupts();
    lock = &sram[0x12];
    if ((s32)sram[0x12] != 0) {
        OSRestoreInterrupts(enabled);
        return 0;
    }

    sram[0x11] = enabled;
    *lock = 1;
    return (u8*)sram + 0x14;
}

OSRTC_HEAD_FN void __OSUnlockSram(BOOL commit) {
    extern void UnlockSram(BOOL commit, u32 arg);

    UnlockSram(commit, 0);
}

BOOL __OSSyncSram(void) {
    return ((u32*)Scb_803FB840)[0x13];
}

BOOL __OSReadROM(void* buffer, s32 length, s32 offset) {
    BOOL err;
    u32 command;

    DCInvalidateRange(buffer, (u32)length);

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    command = (u32)(offset << 6);
    err = FALSE;
    err |= !EXIImm(0, &command, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !EXIDma(0, buffer, length, 0, NULL);
    err |= !EXISync(0);
    err |= !EXIDeselect(0);
    EXIUnlock(0);

    return !err;
}

OSRTC_HEAD_FN BOOL __OSUnlockSramEx(BOOL commit) {
    extern BOOL UnlockSram(BOOL commit, u32 arg);

    return UnlockSram(commit, 0x14);
}

s32 OSGetSoundMode(void) {
    u8* sram;
    u32 flags;
    s32 mode;

    sram = __OSLockSram();
    flags = sram[0x13];
    if (flags & 4) {
        mode = 1;
    } else {
        mode = 0;
    }

    __OSUnlockSram(FALSE);
    return mode;
}

void OSSetSoundMode(u32 mode) {
    u8* sram;

    mode = (mode & 1) << 2;
    sram = __OSLockSram();
    if (mode == (sram[0x13] & 4)) {
        __OSUnlockSram(FALSE);
    } else {
        sram[0x13] &= ~4;
        sram[0x13] |= mode;
        __OSUnlockSram(TRUE);
    }
}

u32 OSGetProgressiveMode(void) {
    u8* sram;
    u32 mode;

    sram = __OSLockSram();
    mode = (sram[0x13] >> 7) & 1;
    __OSUnlockSram(FALSE);
    return mode;
}

void OSSetProgressiveMode(u32 mode) {
    u8* sram;

    mode = (mode & 1) << 7;
    sram = __OSLockSram();
    if (mode == (sram[0x13] & 0x80)) {
        __OSUnlockSram(FALSE);
    } else {
        sram[0x13] &= ~0x80;
        sram[0x13] |= mode;
        __OSUnlockSram(TRUE);
    }
}

u8 OSGetLanguage(void) {
    u8* sram;
    u8 language;

    sram = __OSLockSram();
    language = sram[0x12];
    __OSUnlockSram(FALSE);
    return language;
}

u16 OSGetWirelessID(s32 chan) {
    typedef struct OSSramEx {
        u8 flashID[2][12];
        u32 wirelessKbID;
        u16 wirelessPadID[4];
    } OSSramEx;
    OSSramEx* sram;
    s32 index;
    u16 id;

    index = chan;
    sram = __OSLockSramEx();
    id = sram->wirelessPadID[index];
    __OSUnlockSramEx(FALSE);
    return id;
}

void OSSetWirelessID(s32 chan, u16 id) {
    typedef struct OSSramEx {
        u8 flashID[2][12];
        u32 wirelessKbID;
        u16 wirelessPadID[4];
    } OSSramEx;
    OSSramEx* sram;

    sram = __OSLockSramEx();
    if (sram->wirelessPadID[chan] != id) {
        sram->wirelessPadID[chan] = id;
        __OSUnlockSramEx(TRUE);
    } else {
        __OSUnlockSramEx(FALSE);
    }
}

/* OSSync.c. Hand-written Dolphin SDK asm (SystemCallVector, copied to
   0x80000C00 by __OSInitSystemCall); evidence: docs/asm_evidence/os_primitives.md */
void __OSSystemCallVectorStart(void);
void __OSSystemCallVectorEnd(void);

asm void fn_800A1208(void) {
    nofralloc
entry __OSSystemCallVectorStart
    mfspr   r9, HID0
    ori     r10, r9, 0x8
    mtspr   HID0, r10
    isync
    sync
    mtspr   HID0, r9
    rfi
entry __OSSystemCallVectorEnd
    nop
}

void __OSInitSystemCall(void) {
    extern void __OSSystemCallVectorStart(void);
    extern void __OSSystemCallVectorEnd(void);
    extern void* memcpy(void* dst, const void* src, u32 size);
    extern void DCFlushRangeNoSync(void* addr, u32 size);
    extern void ICInvalidateRange(void* addr, u32 size);
    void* vector;

    vector = (void*)0x80000C00;
    memcpy(vector, __OSSystemCallVectorStart, (u32)__OSSystemCallVectorEnd - (u32)__OSSystemCallVectorStart);
    DCFlushRangeNoSync(vector, 0x100);
    __sync();
    ICInvalidateRange(vector, 0x100);
}

#if !defined(SDK_800A0D00_ONLY)
void DefaultSwitchThreadCallback(void) {
}

void __OSThreadInit(void) {
    OSThread* thread = &DefaultThread;
    s32 priority;

    thread->state = OS_THREAD_STATE_RUNNING;
    thread->attr = OS_THREAD_ATTR_DETACH;
    thread->priority = thread->base = 16;
    thread->suspend = 0;
    thread->val = (u32)-1;
    thread->mutex = NULL;

    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    OS_FPU_CONTEXT = &thread->context;
    OSClearContext(&thread->context);
    OSSetCurrentContext(&thread->context);
    thread->stackBase = (u32*)_stack_addr;
    thread->stackEnd = (u32*)_stack_end;
    *thread->stackEnd = OS_THREAD_STACK_MAGIC;

    SwitchThreadCallback_804789A8(OS_CURRENT_THREAD, thread);
    OS_CURRENT_THREAD = thread;
    OSClearStack(0);

    RunQueueBits_8047A760 = 0;
    RunQueueHint_8047A764 = FALSE;
    for (priority = OS_PRIORITY_MIN; priority <= OS_PRIORITY_MAX; priority++) {
        OSInitThreadQueue(&RunQueue_803FB898[priority]);
    }

    OSInitThreadQueue(OS_ACTIVE_THREAD_QUEUE);
    ADD_TAIL(OS_ACTIVE_THREAD_QUEUE, thread, linkActive);

    OSClearContext(&IdleContext);
    Reschedule_8047A768 = 0;
}
#endif /* !SDK_800A0D00_ONLY */
