/**
 * @file OSRtc.c
 * @brief Dolphin SDK OSRtc.c, 0x800A064C - 0x800A1208 (linked: 0x800A064C - 0x800A0D00): the SRAM control
 *        block (Scb) and its read/write/lock/unlock, plus the SRAM-backed
 *        sound, progressive, language and wireless-ID settings.
 *
 * Follows the SDK as decompiled in zeldaret/tww (src/dolphin/os/OSRtc.c),
 * with this build's extras (__OSReadROM, OSGetLanguage) and EXI helper
 * names. Scb is the file's static, so the whole file links with its .bss
 * (0x803FB840-0x803FB894).
 */
#include "dolphin/types.h"
#include "dolphin/exi/EXI.h"
#include "dolphin/os/OSCache.h"

#define RTC_SRAM_SIZE 0x40

typedef struct SramControlBlock {
    u8 sram[RTC_SRAM_SIZE];
    u32 offset;
    BOOL enabled;
    BOOL locked;
    BOOL sync;
    void (*callback)(void);
} SramControlBlock;

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKbID;
    u16 wirelessPadID[4];
} OSSramEx;

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern BOOL EXIImmEx(s32 chan, void* buffer, s32 length, s32 mode); /* EXIImmEx */

static void WriteSramCallback(s32 chan, OSContext* context);
static BOOL WriteSram(void* buffer, u32 offset, u32 size);

/* Retail's local Scb; global here (under the extracted objects' name) so the
 * file's tail, which still links from sdk_range_800A0D00, can reach it.
 * RULE-EXCEPTION(user-approved): static data and UnlockSram made global for a
 * split file - see docs/RULE_EXCEPTIONS.md */
SramControlBlock Scb_803FB840 __attribute__((aligned(32)));
#define Scb Scb_803FB840

static void WriteSramCallback(s32 chan, OSContext* context) {
    Scb.sync = WriteSram(Scb.sram + Scb.offset, Scb.offset, RTC_SRAM_SIZE - Scb.offset);
    if (Scb.sync) {
        Scb.offset = RTC_SRAM_SIZE;
    }
}

static inline BOOL ReadSram(void* buffer) {
    BOOL err;
    u32 cmd;

    DCInvalidateRange(buffer, RTC_SRAM_SIZE);

    if (!EXILock(0, 1, 0)) {
        return FALSE;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    cmd = 0x20000000 | 0x100;
    err = FALSE;
    err |= !EXIImm(0, &cmd, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !EXIDma(0, buffer, RTC_SRAM_SIZE, 0, NULL);
    err |= !EXISync(0);
    err |= !EXIDeselect(0);
    EXIUnlock(0);

    return !err;
}

static BOOL WriteSram(void* buffer, u32 offset, u32 size) {
    BOOL err;
    u32 cmd;

    if (!EXILock(0, 1, WriteSramCallback)) {
        return FALSE;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    offset <<= 6;
    cmd = 0xA0000000 | 0x100 + offset;
    err = FALSE;
    err |= !EXIImm(0, &cmd, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !EXIImmEx(0, buffer, (s32)size, 1);
    err |= !EXIDeselect(0);
    EXIUnlock(0);

    return !err;
}

void __OSInitSram(void) {
    Scb.locked = Scb.enabled = FALSE;
    Scb.sync = ReadSram(Scb.sram);
    Scb.offset = RTC_SRAM_SIZE;
}

static void* LockSram(u32 offset) {
    BOOL enabled;
    enabled = OSDisableInterrupts();

    if (Scb.locked != FALSE) {
        OSRestoreInterrupts(enabled);
        return NULL;
    }

    Scb.enabled = enabled;
    Scb.locked = TRUE;

    return Scb.sram + offset;
}

OSSram* __OSLockSram(void) {
    return LockSram(0);
}

OSSramEx* __OSLockSramEx(void) {
    return LockSram(sizeof(OSSram));
}

/* UnlockSram (global: the file's tail, linked from sdk_range_800A0D00, calls it) */
BOOL UnlockSram(BOOL commit, u32 offset) {
    u16* p;

    if (commit) {
        if (offset == 0) {
            OSSram* sram = (OSSram*)Scb.sram;

            if (2u < (sram->flags & 3)) {
                sram->flags &= ~3;
            }

            sram->checkSum = sram->checkSumInv = 0;
            for (p = (u16*)&sram->counterBias; p < (u16*)(Scb.sram + sizeof(OSSram)); p++) {
                sram->checkSum += *p;
                sram->checkSumInv += ~*p;
            }
        }

        if (offset < Scb.offset) {
            Scb.offset = offset;
        }

        Scb.sync = WriteSram(Scb.sram + Scb.offset, Scb.offset, RTC_SRAM_SIZE - Scb.offset);
        if (Scb.sync) {
            Scb.offset = RTC_SRAM_SIZE;
        }
    }
    Scb.locked = FALSE;
    OSRestoreInterrupts(Scb.enabled);
    return Scb.sync;
}

BOOL __OSUnlockSram(BOOL commit) {
    return UnlockSram(commit, 0);
}

BOOL __OSUnlockSramEx(BOOL commit) {
    return UnlockSram(commit, sizeof(OSSram));
}

/*
 * The rest of the file (0x800A0D00-0x800A1208) links from
 * sdk_range_800A0D00.c for now: ending this unit at the system-call vector
 * (0x800A1208) makes mwld hang, so the split stays at 0x800A0D00.
 * Define OSRTC_WHOLE_FILE to compile it here.
 */
#if defined(OSRTC_WHOLE_FILE)
BOOL __OSSyncSram(void) {
    return Scb.sync;
}

BOOL __OSReadROM(void* buffer, s32 length, s32 offset) {
    BOOL err;
    u32 cmd;

    DCInvalidateRange(buffer, (u32)length);

    if (!EXILock(0, 1, 0)) {
        return FALSE;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    cmd = (u32)(offset << 6);
    err = FALSE;
    err |= !EXIImm(0, &cmd, 4, 1, NULL);
    err |= !EXISync(0);
    err |= !EXIDma(0, buffer, length, 0, NULL);
    err |= !EXISync(0);
    err |= !EXIDeselect(0);
    EXIUnlock(0);

    return !err;
}

u32 OSGetSoundMode(void) {
    OSSram* sram;
    u32 flags;
    u32 mode;

    sram = __OSLockSram();
    flags = sram->flags;
    if (flags & 4) {
        mode = 1;
    } else {
        mode = 0;
    }

    __OSUnlockSram(FALSE);
    return mode;
}

/* OSSetSoundMode */
void OSSetSoundMode(u32 mode) {
    OSSram* sram;

    mode <<= 2;
    mode &= 4;

    sram = __OSLockSram();
    if (mode == (sram->flags & 4)) {
        __OSUnlockSram(FALSE);
        return;
    }

    sram->flags &= ~4;
    sram->flags |= mode;
    __OSUnlockSram(TRUE);
}

u32 OSGetProgressiveMode(void) {
    OSSram* sram;
    u32 mode;

    sram = __OSLockSram();
    mode = (sram->flags & 0x80) >> 7;
    __OSUnlockSram(FALSE);
    return mode;
}

/* OSSetProgressiveMode */
void OSSetProgressiveMode(u32 mode) {
    OSSram* sram;

    mode <<= 7;
    mode &= 0x80;

    sram = __OSLockSram();
    if (mode == (sram->flags & 0x80)) {
        __OSUnlockSram(FALSE);
        return;
    }

    sram->flags &= ~0x80;
    sram->flags |= mode;
    __OSUnlockSram(TRUE);
}

u8 OSGetLanguage(void) {
    OSSram* sram;
    u8 language;

    sram = __OSLockSram();
    language = sram->language;
    __OSUnlockSram(FALSE);
    return language;
}

u16 OSGetWirelessID(s32 chan) {
    OSSramEx* sram;
    u16 id;

    sram = __OSLockSramEx();
    id = sram->wirelessPadID[chan];
    __OSUnlockSramEx(FALSE);
    return id;
}

void OSSetWirelessID(s32 chan, u16 id) {
    OSSramEx* sram;

    sram = __OSLockSramEx();
    if (sram->wirelessPadID[chan] != id) {
        sram->wirelessPadID[chan] = id;
        __OSUnlockSramEx(TRUE);
        return;
    }

    __OSUnlockSramEx(FALSE);
}
#endif /* OSRTC_WHOLE_FILE */
