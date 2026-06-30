#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/exi/EXI.h"

/*
 * OSSram.c - SRAM (battery-backed configuration memory) access.
 *
 * The GameCube SRAM stores system configuration data (language,
 * sound mode, screen position, etc.) in 64 bytes of battery-backed
 * memory on the IPL ROM chip, accessed via EXI channel 0 device 1.
 *
 * Adapted from doldecomp/melee and zeldaret/tp matching implementations.
 *
 * Matches: 0x8009C860 - 0x8009DF3C
 *   fn_8009C860 (0x3DC) - WriteSram (large EXI transfer)
 *   fn_8009CC3C (0xFC)  - WriteSramCallback
 *   fn_8009CD38 (0x154) - ReadSram
 *   fn_8009CE8C (0x684) - __OSInitSram
 *   fn_8009D510 (0x19C) - __OSLockSram
 *   fn_8009D6AC (0x174) - __OSLockSramEx
 *   fn_8009D820 (0x58)  - UnlockSram
 *   fn_8009D878 (0x8C)  - __OSUnlockSram
 *   fn_8009D904 (0x334) - __OSUnlockSramEx
 *   fn_8009DC38 (0x304) - __OSSyncSram + helpers
 */

extern void* memcpy(void* dest, const void* src, u32 n);

/* SRAM structure (64 bytes) */
typedef struct OSSram {
    u16 checkSum;       /* 0x00 */
    u16 checkSumInv;    /* 0x02 */
    u32 ead0;           /* 0x04 */
    u32 ead1;           /* 0x08 */
    u32 counterBias;    /* 0x0C */
    s8  displayOffsetH; /* 0x10 */
    u8  ntd;            /* 0x11 */
    u8  language;       /* 0x12 */
    u8  flags;          /* 0x13 */
} OSSram;

/* Extended SRAM structure (additional 64 bytes at offset 0x40) */
typedef struct OSSramEx {
    u8 flashID[4][12];  /* 0x00 */
    u32 wirelessKbID;   /* 0x30 */
    u16 wirelessPadID[4]; /* 0x34 */
    u8  dvdErrorCode;   /* 0x3C */
    u8  _pad0;          /* 0x3D */
    u8  flashIDCheckSum[4]; /* 0x3E */
    u16 _pad1;          /* 0x42 */
} OSSramEx;

/* SRAM state flags */
#define SRAM_LOCKED     0x01
#define SRAM_LOCKED_EX  0x02
#define SRAM_NEED_WRITE 0x04
#define SRAM_WRITING    0x08

static OSSram Sram;
static OSSramEx SramEx;
static u32 SramFlags;
static u32 SramOffset;
static u32 SramSize;
static u32 SramWriteCount;

static BOOL WriteSram(u32 offset, void* data, u32 size);
static void WriteSramCallback(s32 chan, OSContext* context);
static BOOL ReadSram(u32 offset, void* dest, u32 size);

static void UnlockSram(BOOL commit, u32 lockBit);

/*
 * ReadSram - Read data from SRAM via EXI.
 * 0x8009CD38 | size: 0x154
 */
static BOOL ReadSram(u32 offset, void* dest, u32 size) {
    BOOL result = FALSE;
    u32 cmd;

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    /* Send read command: address = 0x20000000 + (offset << 6) */
    cmd = 0x20000000 | (offset << 6);
    if (EXIImm(0, &cmd, 4, 1, NULL) && EXISync(0)) {
        if (EXIDma(0, dest, (s32)size, 0, NULL) && EXISync(0)) {
            result = TRUE;
        }
    }

    EXIDeselect(0);
    EXIUnlock(0);
    return result;
}

/*
 * WriteSramCallback - EXI completion callback for SRAM writes.
 * 0x8009CC3C | size: 0xFC
 */
static void WriteSramCallback(s32 chan, OSContext* context) {
    SramFlags &= ~SRAM_WRITING;
    SramWriteCount++;
    EXIDeselect(0);
    EXIUnlock(0);
}

/*
 * WriteSram - Write data to SRAM via EXI.
 * 0x8009C860 | size: 0x3DC
 *
 * Initiates an EXI write to SRAM. The write is asynchronous --
 * WriteSramCallback is called when the transfer completes.
 */
static BOOL WriteSram(u32 offset, void* data, u32 size) {
    BOOL result = FALSE;
    u32 cmd;

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    /* Send write command: address = 0xA0000000 + (offset << 6) */
    cmd = 0xA0000000 | (offset << 6);
    if (EXIImm(0, &cmd, 4, 1, NULL) && EXISync(0)) {
        SramFlags |= SRAM_WRITING;
        if (EXIDma(0, data, (s32)size, 1, WriteSramCallback)) {
            result = TRUE;
        } else {
            SramFlags &= ~SRAM_WRITING;
            EXIDeselect(0);
            EXIUnlock(0);
        }
    } else {
        EXIDeselect(0);
        EXIUnlock(0);
    }

    return result;
}

/*
 * __OSInitSram - Initialize SRAM subsystem.
 * 0x8009CE8C | size: 0x684
 *
 * Reads both the primary SRAM (64 bytes) and extended SRAM from the
 * IPL ROM via EXI, validates checksums, and stores the data.
 */
void __OSInitSram(void) {
    SramFlags = 0;
    SramWriteCount = 0;

    /* Read primary SRAM (64 bytes at offset 0) */
    ReadSram(0, &Sram, sizeof(Sram));

    /* Read extended SRAM (64 bytes at offset 0x40) */
    ReadSram(0x40, &SramEx, sizeof(SramEx));

    /* Validate checksums */
    {
        u16 sum = 0;
        u16 sumInv = 0xFFFF;
        u16* data = (u16*)&Sram.ead0;
        u32 i;

        for (i = 0; i < (sizeof(Sram) - 4) / 2; i++) {
            sum += data[i];
            sumInv += ~data[i];
        }

        if (sum != Sram.checkSum || sumInv != Sram.checkSumInv) {
            /* Checksum mismatch -- reinitialize SRAM */
            Sram.checkSum = sum;
            Sram.checkSumInv = sumInv;
            SramFlags |= SRAM_NEED_WRITE;
        }
    }

    /* Sync SRAM if needed */
    if (SramFlags & SRAM_NEED_WRITE) {
        WriteSram(0, &Sram, sizeof(Sram));
    }
}

/*
 * __OSLockSram - Lock and return pointer to primary SRAM.
 * 0x8009D510 | size: 0x19C
 */
void* __OSLockSram(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Wait for any pending writes */
    while (SramFlags & SRAM_WRITING) {
        /* Spin */
    }

    SramFlags |= SRAM_LOCKED;
    OSRestoreInterrupts(enabled);
    return &Sram;
}

/*
 * __OSLockSramEx - Lock and return pointer to extended SRAM.
 * 0x8009D6AC | size: 0x174
 */
void* __OSLockSramEx(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    /* Wait for any pending writes */
    while (SramFlags & SRAM_WRITING) {
        /* Spin */
    }

    SramFlags |= SRAM_LOCKED_EX;
    OSRestoreInterrupts(enabled);
    return &SramEx;
}

/*
 * UnlockSram - Internal unlock helper.
 * 0x8009D820 | size: 0x58
 */
static void UnlockSram(BOOL commit, u32 lockBit) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (commit) {
        /* Recalculate checksum for primary SRAM */
        if (lockBit == SRAM_LOCKED) {
            u16 sum = 0;
            u16 sumInv = 0xFFFF;
            u16* data = (u16*)&Sram.ead0;
            u32 i;

            for (i = 0; i < (sizeof(Sram) - 4) / 2; i++) {
                sum += data[i];
                sumInv += ~data[i];
            }
            Sram.checkSum = sum;
            Sram.checkSumInv = sumInv;
        }
        SramFlags |= SRAM_NEED_WRITE;
    }

    SramFlags &= ~lockBit;
    OSRestoreInterrupts(enabled);
}

/*
 * __OSUnlockSram - Unlock primary SRAM.
 * 0x8009D878 | size: 0x8C
 */
BOOL __OSUnlockSram(BOOL commit) {
    UnlockSram(commit, SRAM_LOCKED);
    return TRUE;
}

/*
 * __OSUnlockSramEx - Unlock extended SRAM.
 * 0x8009D904 | size: 0x334
 */
BOOL __OSUnlockSramEx(BOOL commit) {
    UnlockSram(commit, SRAM_LOCKED_EX);
    return TRUE;
}

/*
 * __OSSyncSram - Flush any pending SRAM writes.
 * 0x8009DC38 | size: 0x304
 *
 * Returns TRUE if no write is in progress (SRAM is synced).
 */
BOOL __OSSyncSram(void) {
    BOOL synced;

    if (SramFlags & SRAM_NEED_WRITE) {
        SramFlags &= ~SRAM_NEED_WRITE;
        WriteSram(0, &Sram, sizeof(Sram));
    }

    synced = !(SramFlags & SRAM_WRITING);
    return synced;
}

/*
 * OSSetWirelessID - Set the wireless controller ID for a channel.
 * Part of SRAM extended functionality.
 */
void OSSetWirelessID(s32 chan, u16 id) {
    OSSramEx* sramEx;

    sramEx = (OSSramEx*)__OSLockSramEx();
    sramEx->wirelessPadID[chan] = id;
    __OSUnlockSramEx(TRUE);
}

/*
 * OSGetWirelessID - Get the wireless controller ID for a channel.
 */
u16 OSGetWirelessID(s32 chan) {
    OSSramEx* sramEx;
    u16 id;

    sramEx = (OSSramEx*)__OSLockSramEx();
    id = sramEx->wirelessPadID[chan];
    __OSUnlockSramEx(FALSE);
    return id;
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800A09B0 - 0x800A09B0 | size: 0x308 */
void fn_800A09B0(void) {
    u8 sp[0x20];
    extern void fn_80098368();
    extern void fn_800A064C();
    extern u32 Scb_803FB840;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)Scb_803FB840;
    r31 = (u32)Scb_803FB840;
    if ((s32)r3 != 0) {
        if (r4 == 0) {
            r3 = *(u8*)((u8*)r31 + 0x13);
            tmp = r3 & 0x3;
            if (tmp > 2) {
                /* clrrwi tmp, r3, 2 */;
                *(u8*)((u8*)r31 + 0x13) = tmp;
            }
            tmp = 0x0;
            *(u16*)((u8*)r31 + 0x2) = tmp;
            r5 = r31 + 0x14;
            r6 = r31 + 0xc;
            r3 = r5 + 0x1;
            *(u16*)((u8*)r31 + 0x0) = tmp;
            r3 = r3 - r6;
            r3 = (u32)r3 >> 1;
            if (r6 < r5) {
                /* srwi. tmp, r3, 3 */;
                ctr_fn = (void(*)(void))tmp;
                if (r6 != r5) {
                    do {
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0x0);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0x0);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0x2);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0x2);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0x4);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0x4);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0x6);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0x6);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0x8);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0x8);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0xA);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0xA);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0xC);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0xC);
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0xE);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0xE);
                        r6 = r6 + 0x10;
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                    } while (--ctr != 0);
                    r3 = r3 & 0x7;
                }
                if (r6 != r5) {
                    ctr_fn = (void(*)(void))r3;
                    do {
                        r5 = *(u16*)((u8*)r31 + 0x0);
                        tmp = *(u16*)((u8*)r6 + 0x0);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x0) = tmp;
                        tmp = *(u16*)((u8*)r6 + 0x0);
                        r6 = r6 + 0x2;
                        r5 = *(u16*)((u8*)r31 + 0x2);
                        tmp = ~(tmp | tmp);
                        tmp = r5 + tmp;
                        *(u16*)((u8*)r31 + 0x2) = tmp;
                    } while (--ctr != 0);
                }
            }
        }
        /* L_800A0B84 */
        r30 = r31 + 0x40;
        tmp = *(u32*)((u8*)r31 + 0x40);
        if (r4 < tmp) {
            *(u32*)((u8*)r30 + 0x0) = r4;
        }
        r29 = *(u32*)((u8*)r30 + 0x0);
        r3 = (u32)fn_800A064C;
        r5 = (u32)fn_800A064C;
        r27 = 0x40 - r29;
        r28 = r31 + r29;
        r3 = 0x0;
        r4 = 0x1;
        EXILock(r3, r4, 0);
        if ((s32)r3 == 0) {
            tmp = 0x0;
        } else {
            r3 = 0x0;
            r4 = 0x1;
            r5 = 0x3;
            EXISelect(r3, r4, r5);
            if ((s32)r3 == 0) {
                r3 = 0x0;
                EXIUnlock(r3);
                tmp = 0x0;
            } else {
                r3 = r29 << 6;
                tmp = r3 + 0x100;
                tmp = tmp | (0xa000 << 16);
                r4 = (u32)sp + 0x10;
                r3 = 0x0;
                r5 = 0x4;
                r6 = 0x1;
                r7 = 0x0;
                EXIImm(r3, (void*)r4, r5, 0, 0);
                tmp = __cntlzw(r3);
                r29 = (u32)tmp >> 5;
                r3 = 0x0;
                EXISync(r3);
                tmp = __cntlzw(r3);
                tmp = (u32)tmp >> 5;
                r4 = r28 + 0x0;
                r5 = r27 + 0x0;
                r29 = r29 | tmp;
                r3 = 0x0;
                r6 = 0x1;
                fn_80098368();
                tmp = __cntlzw(r3);
                tmp = (u32)tmp >> 5;
                r29 = r29 | tmp;
                r3 = 0x0;
                EXIDeselect(r3);
                tmp = __cntlzw(r3);
                tmp = (u32)tmp >> 5;
                r29 = r29 | tmp;
                r3 = 0x0;
                EXIUnlock(r3);
                tmp = __cntlzw(r29);
                tmp = (u32)tmp >> 5;
            }
        }
        /* L_800A0C78 */
        *(u32*)((u8*)r31 + 0x4C) = tmp;
        tmp = *(u32*)((u8*)r31 + 0x4C);
        if ((s32)tmp != 0) {
            tmp = 0x40;
            *(u32*)((u8*)r30 + 0x0) = tmp;
        }
    }
    /* L_800A0C90 */
    tmp = 0x0;
    *(u32*)((u8*)r31 + 0x48) = tmp;
    r3 = *(u32*)((u8*)r31 + 0x44);
    OSRestoreInterrupts(r3);
    r3 = *(u32*)((u8*)r31 + 0x4C);
    return;
}

/* fn_800A0D10 - 0x800A0D10 | size: 0x124 */
/*
 * ReadSramDma - Read SRAM data via EXI DMA transfer.
 *
 * Invalidates the destination cache range, then performs a full
 * EXI lock/select/command/DMA/deselect/unlock sequence to read
 * SRAM data at the given offset.
 *
 * 0x800A0D10 | size: 0x124
 */
BOOL fn_800A0D10(void* dest, u32 size, u32 offset) {
    u32 cmd;
    BOOL err = FALSE;

    DCInvalidateRange(dest, size);

    if (!EXILock(0, 1, NULL)) {
        return FALSE;
    }

    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return FALSE;
    }

    /* Send read command: address with offset shifted */
    cmd = offset << 6;
    if (!EXIImm(0, &cmd, 4, 1, NULL)) { err = TRUE; }
    if (!EXISync(0)) { err = TRUE; }
    if (!EXIDma(0, dest, (s32)size, 0, NULL)) { err = TRUE; }
    if (!EXISync(0)) { err = TRUE; }
    if (!EXIDeselect(0)) { err = TRUE; }
    EXIUnlock(0);

    return !err;
}

/*
 * SramGetFlag04 - Lock SRAM and return whether flag bit 0x04 is set.
 *
 * Acquires the SRAM lock (Scb+0x48), reads the flags byte at offset 0x13,
 * checks bit 2 (0x04), then releases via fn_800A09B0.
 *
 * 0x800A0E34 | size: 0x80
 */
u32 fn_800A0E34(void) {
    extern void fn_800A09B0(u32 commit, u32 lockBit);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    BOOL enabled;
    u32 result;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
    }

    result = (*(u8*)(sram + 0x13) & 0x04) ? 1 : 0;

    fn_800A09B0(0, 0);
    return result;
}

/*
 * SramSetFlag04 - Lock SRAM and set/clear flag bit 0x04.
 *
 * Acquires the SRAM lock, reads flags at offset 0x13. If the current
 * value of bit 0x04 matches the requested value (r30), does nothing.
 * Otherwise, clears the old bit, sets the new value, and commits.
 *
 * 0x800A0EB4 | size: 0xA4
 */
void fn_800A0EB4(u32 newVal) {
    extern void fn_800A09B0(u32 commit, u32 lockBit);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    BOOL enabled;
    u8 flags;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
    }

    flags = *(u8*)(sram + 0x13);
    if (newVal == (u32)(flags & 0x04)) {
        fn_800A09B0(0, 0);
    } else {
        *(u8*)(sram + 0x13) = flags & ~0x04;
        flags = *(u8*)(sram + 0x13);
        *(u8*)(sram + 0x13) = flags | (u8)newVal;
        fn_800A09B0(1, 0);
    }
}

/*
 * SramGetFlag80 - Lock SRAM and return whether flag bit 0x80 is set.
 *
 * 0x800A0F58 | size: 0x70
 */
u32 fn_800A0F58(void) {
    extern void fn_800A09B0(u32 commit, u32 lockBit);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    BOOL enabled;
    u32 result;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
    }

    result = (*(u8*)(sram + 0x13) >> 7) & 1;

    fn_800A09B0(0, 0);
    return result;
}

/*
 * SramSetFlag80 - Lock SRAM and set/clear flag bit 0x80.
 *
 * Same pattern as SramSetFlag04 but operates on bit 7 (0x80).
 *
 * 0x800A0FC8 | size: 0xA4
 */
void fn_800A0FC8(u32 newVal) {
    extern void fn_800A09B0(u32 commit, u32 lockBit);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    BOOL enabled;
    u8 flags;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
    }

    flags = *(u8*)(sram + 0x13);
    if (newVal == (u32)(flags & 0x80)) {
        fn_800A09B0(0, 0);
    } else {
        *(u8*)(sram + 0x13) = flags & ~0x80;
        flags = *(u8*)(sram + 0x13);
        *(u8*)(sram + 0x13) = flags | (u8)newVal;
        fn_800A09B0(1, 0);
    }
}

/* fn_800A106C - 0x800A106C | size: 0x6C
 * __OSLockSramEx2 - Lock the SRAM structure and read a byte field.
 * Acquires the lock at Scb+0x48, stores interrupt state at Scb+0x44.
 * Reads byte at offset 0x12 of the SRAM data. Returns the byte value,
 * or 0 if the lock is already held.
 */
u32 fn_800A106C(void) {
    extern void fn_800A09B0(u32 a, u32 b);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    BOOL enabled;
    u32 result;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
        result = 0;
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
        result = *(u8*)(sram + 0x12);
    }

    fn_800A09B0(0, 0);
    return result;
}

/*
 * SramGetPadID - Lock SRAM and read a wireless pad ID.
 *
 * Locks the SRAM, reads a 16-bit pad ID from the SramEx structure
 * at offset 0x14 + 0x1C + (index * 2), then releases.
 *
 * 0x800A10D8 | size: 0x84
 */
u16 fn_800A10D8(u32 padIndex) {
    extern void fn_800A09B0(u32 commit, u32 lockBit);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    u8* sramEx;
    BOOL enabled;
    u16 result;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
        sramEx = NULL;
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
        sramEx = sram + 0x14;
    }

    result = *(u16*)(sramEx + 0x1C + (padIndex * 2));

    fn_800A09B0(0, 0x14);
    return result;
}

/*
 * SramSetPadID - Lock SRAM and write a wireless pad ID.
 *
 * If the new value differs from the current one, writes it and
 * commits the SRAM. Otherwise, just releases the lock.
 *
 * 0x800A115C | size: 0xAC
 */
void fn_800A115C(u32 padIndex, u16 newId) {
    extern void fn_800A09B0(u32 commit, u32 lockBit);
    extern u32 Scb_803FB840;
    u8* sram = (u8*)(u32)Scb_803FB840;
    u8* sramEx;
    BOOL enabled;
    u16 curId;
    u8* entry;

    enabled = OSDisableInterrupts();

    if (*(s32*)(sram + 0x48) != 0) {
        OSRestoreInterrupts(enabled);
        sramEx = NULL;
    } else {
        *(u32*)(sram + 0x44) = (u32)enabled;
        *(u32*)(sram + 0x48) = 1;
        sramEx = sram + 0x14;
    }

    entry = sramEx + 0x1C + (padIndex * 2);
    curId = *(u16*)entry;
    if (curId != (newId & 0xFFFF)) {
        *(u16*)entry = newId;
        fn_800A09B0(1, 0x14);
    } else {
        fn_800A09B0(0, 0x14);
    }
}

/* fn_800A1208 - 0x800A1208 | size: 0x20 */
void fn_800A1208(void) {
    u32 r9 = 0;
    u32 r10 = 0;

    r9 = 0; /* mfspr HID0 */;
    r10 = r9 | 0x8;
    /* mtspr HID0, r10 */;
    /* isync */;
    /* sync */;
    /* mtspr HID0, r9 */;
    /* rfi */; return;
    /* nop  */;
}

