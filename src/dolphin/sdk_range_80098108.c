/**
 * @file sdk_range_80098108.c
 * @brief dolphin-sdk code, 0x80098108 - 0x8009A0E0 (32 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/dvd/dvd.h"
#include "dolphin/exi/EXI.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"
#include "dolphin/db/DB.h"

typedef struct EXIRegs {
    u32 csr;
    u32 mar;
    u32 length;
    u32 cr;
    u32 data;
} EXIRegs;

typedef union EXIRegBlock {
    EXIRegs regs[3];
    u32 words[15];
} EXIRegBlock;

typedef union OSSavedRegionAddress {
    void* pointer;
    u32 address;
} OSSavedRegionAddress;

typedef struct EXIQueueEntry {
    u32 device;
    void (*callback)(s32 chan, OSContext* context);
} EXIQueueEntry;

typedef struct EXIControl {
    void (*exiCallback)(s32 chan, OSContext* context);
    void (*tcCallback)(s32 chan, OSContext* context);
    void (*extCallback)(s32 chan, OSContext* context);
    u32 state;
    s32 immLen;
    u8* immBuf;
    u32 device;
    u32 id;
    s32 idTime;
    s32 items;
    EXIQueueEntry queue[3];
} EXIControl;

typedef struct OSLowMem {
    u8 pad_0000[0x30C0];
    u32 exiProbeStartTime[2];
    u8 pad_30C8[0x1E];
    u16 dvdDeviceCode;
    u8 osDebugFlag;
    u8 padSpec;
} OSLowMem;

typedef void (*OSExceptionHandler)(u8 exception, OSContext* context, u32 dsisr, u32 dar);

extern OSBootInfo* BootInfo_8047A6A0;
extern u32* BI2DebugFlag_8047A6A4;
extern OSExceptionHandler* OSExceptionTable_8047A6C4;
#if !defined(SDK_EXI_ECB_STATIC)
extern EXIControl lbl_803FB3C8[];
#else
/* EXIBios.c's Ecb is file-static: the compiler keeps its base address in a
   register across EXIGetID's inlined helpers. */
static EXIControl lbl_803FB3C8[3];
#endif
static DVDDriveInfo DriveInfo_803FB4A0;
static DVDCommandBlock DriveBlock_803FB4C0;
extern OSSavedRegionAddress __OSSavedRegionStart;
extern OSSavedRegionAddress __OSSavedRegionEnd;

#define AT_ADDRESS(addr) : addr
#ifdef __MWERKS__
volatile u32 __EXIRegs[15] AT_ADDRESS(0xCC006800);
#else
#define __EXIRegs ((volatile u32*)0xCC006800)
#endif
u32 __OSBusClock AT_ADDRESS(0x800000F8);
s32 __EXIProbeStartTime[2] AT_ADDRESS(0x800030C0);

void __OSDBINTSTART(void);
void __OSDBINTEND(void);
void __OSDBJUMPEND(void);
extern u8 __OSEVStart[];
extern u8 __OSEVEnd[];
extern u32 __OSEVSetNumber[];
extern u32 __DBVECTOR[];

static volatile EXIRegBlock* const ExiHw = (volatile EXIRegBlock*)0xCC006800;
static volatile OSLowMem* const LowMem = (volatile OSLowMem*)0x80000000;

s32 fn_80099400(s32 chan, u32 dev, u32* id);
BOOL fn_80098790(s32 chan);
u32 fn_800986A0(s32 chan, s32 exi, s32 tc, s32 ext);
u32 __OSGetDIConfig(void);
u32 OSGetConsoleType(void);
void fn_80098110(s32 chan, EXIControl* exi);
void* memmove(void* dst, const void* src, size_t n);
void* memset(void* dst, int value, size_t n);

/* SDK_OS_SUFFIX_ONLY (sdk_r48_80099790_suffix.c) builds only OS.c's
   0x80099790 - 0x8009A0E0 run, with its .data, .sdata and .bss. */
#if !defined(SDK_OS_SUFFIX_ONLY)

/* 0x80098108 | size: 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void PPCSetFpNonIEEEMode(void) {
    nofralloc
    mtfsb1 29
    blr
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
void fn_80098110(volatile s32 chan, EXIControl* exi) {
    EXIControl* exi2 = &lbl_803FB3C8[2];

    switch (chan) {
    case 0:
        if ((!exi->exiCallback && !exi2->exiCallback) || (exi->state & 0x10)) {
            __OSMaskInterrupts(0x410000);
        } else {
            __OSUnmaskInterrupts(0x410000);
        }
        break;
    case 1:
        if (!exi->exiCallback || (exi->state & 0x10)) {
            __OSMaskInterrupts(0x80000);
        } else {
            __OSUnmaskInterrupts(0x80000);
        }
        break;
    case 2:
        if (!__OSGetInterruptHandler(0x19) || (exi->state & 0x10)) {
            __OSMaskInterrupts(0x40);
        } else {
            __OSUnmaskInterrupts(0x40);
        }
        break;
    }
}
#pragma scheduling reset
#pragma pop

static inline u8* EXIGetBytes(void* buffer)
{
    return buffer;
}

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXIImm(s32 chan, void* buf, s32 len, u32 type, EXICallback callback)
{
    EXIControl* exi;
    BOOL enabled;
    u32 data;
    s32 i;

    exi = &lbl_803FB3C8[chan];
    enabled = OSDisableInterrupts();

    if ((exi->state & 3) || !(exi->state & 4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if (exi->tcCallback) {
        fn_800986A0(chan, 0, 1, 0);
        __OSUnmaskInterrupts(0x200000u >> (chan * 3));
    }

    exi->state |= 2;
    if (type != 0) {
        data = 0;
        for (i = 0; i < len; i++) {
            data |= EXIGetBytes(buf)[i] << ((3 - i) * 8);
        }
        __EXIRegs[chan * 5 + 4] = data;
    }

    exi->immBuf = buf;
    exi->immLen = type != 1 ? len : 0;
    __EXIRegs[chan * 5 + 3] = (type << 2) | 1 | ((len - 1) << 4);
    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimize_for_size on
#pragma scheduling off
BOOL fn_80098368(s32 chan, u8* buf, s32 len, volatile u32 type) {
    while (len != 0) {
        s32 xfer;

        if (len < 4) {
            xfer = len;
        } else {
            xfer = 4;
        }

        if (!EXIImm(chan, buf, xfer, type, NULL)) {
            return FALSE;
        }
        if (!EXISync(chan)) {
            return FALSE;
        }

        buf += xfer;
        len -= xfer;
    }

    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXIDma(s32 chan, void* buf, s32 len, u32 type, EXICallback callback) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    BOOL enabled = OSDisableInterrupts();

    if ((exi->state & 3) || !(exi->state & 4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if ((u32)exi->tcCallback) {
        fn_800986A0(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(0x200000u >> (chan * 3));
    }

    exi->state |= 1;
    __EXIRegs[chan * 5 + 1] = (u32)buf & 0x03FFFFE0;
    __EXIRegs[chan * 5 + 2] = len;
    __EXIRegs[chan * 5 + 3] = (type << 2) | 3;

    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma scheduling reset
#pragma pop

static inline void CompleteTransfer(s32 chan) {
    EXIControl* exi;
    u8* buf;
    u32 data;
    s32 i;
    s32 len;

    exi = &lbl_803FB3C8[chan];
    if (exi->state & 3) {
        if (exi->state & 2) {
            if ((len = exi->immLen) != 0) {
                buf = exi->immBuf;
                data = __EXIRegs[(chan * 5) + 4];
                for (i = 0; i < len; i++) {
                    *buf++ = data >> ((3 - i) * 8);
                }
            }
        }
        exi->state &= ~3;
    }
}

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXISync(s32 chan) {
    EXIControl* exi;
    s32 result;
    BOOL enabled;

    exi = &lbl_803FB3C8[chan];
    result = FALSE;

    while (exi->state & 4) {
        if (!(__EXIRegs[(chan * 5) + 3] & 1)) {
            enabled = OSDisableInterrupts();
            if (exi->state & 4) {
                CompleteTransfer(chan);

                if (__OSGetDIConfig() != 0xFF || exi->immLen != 4 ||
                    (__EXIRegs[chan * 5] & 0x70) ||
                    __EXIRegs[(chan * 5) + 4] != 0x01010000) {
                    result = TRUE;
                }
            }
            OSRestoreInterrupts(enabled);
            break;
        }
    }

    return result;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
u32 fn_800986A0(s32 chan, s32 exi, s32 tc, s32 ext) {
    u32 csr = __EXIRegs[chan * 5];
    u32 oldCsr = csr;

    csr &= 0x7F5;

    if (exi != 0) {
        csr |= 0x2;
    }
    if (tc != 0) {
        csr |= 0x8;
    }
    if (ext != 0) {
        csr |= 0x800;
    }

    __EXIRegs[chan * 5] = csr;
    return oldCsr;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
EXICallback fn_8009870C(s32 chan, EXICallback volatile callback) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    EXICallback oldCallback;
    BOOL enabled = OSDisableInterrupts();

    oldCallback = exi->exiCallback;

    exi->exiCallback = callback;
    if (chan != 2) {
        fn_80098110(chan, exi);
    } else {
        fn_80098110(0, &lbl_803FB3C8[0]);
    }
    OSRestoreInterrupts(enabled);
    return oldCallback;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL fn_80098790(s32 chan) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    BOOL enabled;
    BOOL probe;
    u32 csr;
    s32 time;

    if (chan == 2) {
        return TRUE;
    }

    probe = TRUE;
    enabled = OSDisableInterrupts();
    csr = __EXIRegs[chan * 5];
    if (!(exi->state & 8)) {
        if (csr & 0x800) {
            fn_800986A0(chan, FALSE, FALSE, TRUE);
            __EXIProbeStartTime[chan] = exi->idTime = 0;
        }

        if (csr & 0x1000) {
            time = (s32)(OSGetTime() / (__OSBusClock / 4 / 1000) / 100) + 1;
            if (__EXIProbeStartTime[chan] == 0) {
                __EXIProbeStartTime[chan] = time;
            }
            if (time - __EXIProbeStartTime[chan] < 3) {
                probe = FALSE;
            }
        } else {
            __EXIProbeStartTime[chan] = exi->idTime = 0;
            probe = FALSE;
        }
    } else if (!(csr & 0x1000) || (csr & 0x800)) {
        __EXIProbeStartTime[chan] = exi->idTime = 0;
        probe = FALSE;
    }
    OSRestoreInterrupts(enabled);

    return probe;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL fn_80098944(s32 chan) {
    BOOL probe;
    EXIControl* exi = &lbl_803FB3C8[chan];
    u32 id;

    probe = fn_80098790(chan);
    if (probe && exi->idTime == 0) {
        probe = fn_80099400(chan, 0, &id) ? TRUE : FALSE;
    }
    return probe;
}
#pragma scheduling reset
#pragma pop

static inline BOOL fn_800989C0_probe(s32 chan, EXIControl* base) {
    EXIControl* exi;
    BOOL probe;
    u32 id;

    exi = &base[chan];
    probe = fn_80098790(chan);
    if (probe && exi->idTime == 0) {
        probe = fn_80099400(chan, 0, &id) ? TRUE : FALSE;
    }
    return probe;
}

static inline BOOL fn_800989C0_attach(s32 chan, EXICallback extCallback, EXIControl* base) {
    EXIControl* exi;
    BOOL enabled;

    exi = &base[chan];
    enabled = OSDisableInterrupts();

    if ((exi->state & 8) || !fn_80098790(chan)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fn_800986A0(chan, TRUE, FALSE, FALSE);
    exi->extCallback = extCallback;
    __OSUnmaskInterrupts(0x100000u >> (chan * 3));
    exi->state |= 8;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL fn_800989C0(s32 chan, EXICallback extCallback) {
    EXIControl* base;
    EXIControl* exi;
    BOOL enabled;
    BOOL result;

    base = lbl_803FB3C8;
    exi = &base[chan];

    fn_800989C0_probe(chan, base);
    enabled = OSDisableInterrupts();
    if (exi->idTime == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    result = fn_800989C0_attach(chan, extCallback, base);
    OSRestoreInterrupts(enabled);
    return result;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL fn_80098AE8(s32 chan) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    BOOL enabled = OSDisableInterrupts();

    if (!(exi->state & 8)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    if ((exi->state & 0x10) && exi->device == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state &= ~8;
    __OSMaskInterrupts(0x700000u >> (chan * 3));
    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXISelect(s32 chan, u32 dev, u32 freq) {
    EXIControl* exi;
    u32 csr;
    BOOL enabled;

    exi = &lbl_803FB3C8[chan];
    enabled = OSDisableInterrupts();
    if ((exi->state & 4) ||
        ((chan != 2) &&
         (((dev == 0) && !(exi->state & 8) && (fn_80098790(chan) == 0)) ||
          !(exi->state & 0x10) || (exi->device != dev)))) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state |= 4;
    csr = __EXIRegs[chan * 5];
    csr &= 0x405;
    csr |= ((1 << dev) << 7) | (freq * 0x10);
    __EXIRegs[chan * 5] = csr;

    if (exi->state & 8) {
        switch (chan) {
        case 0:
            __OSMaskInterrupts(0x100000);
            break;
        case 1:
            __OSMaskInterrupts(0x20000);
            break;
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXIDeselect(s32 chan) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    u32 csr;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(exi->state & 4)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state &= ~4;
    csr = __EXIRegs[chan * 5];
    __EXIRegs[chan * 5] = csr & 0x405;

    if (exi->state & 8) {
        switch (chan) {
        case 0:
            __OSUnmaskInterrupts(0x100000);
            break;
        case 1:
            __OSUnmaskInterrupts(0x20000);
            break;
        }
    }

    OSRestoreInterrupts(enabled);

    if (chan != 2 && (csr & 0x80)) {
        return fn_80098790(chan) ? TRUE : FALSE;
    }

    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
void fn_80098DDC(volatile s16 interrupt, OSContext* context) {
    OSContext exceptionContext;
    s32 chan = (interrupt - 9) / 3;
    EXIControl* exi = &lbl_803FB3C8[chan];
    u32 csr = __EXIRegs[chan * 5];
    volatile u32 oldCsr = csr;
    EXICallback callback;

    csr &= 0x7F5;
    csr |= 2;
    __EXIRegs[chan * 5] = csr;
    callback = exi->exiCallback;
    if (callback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
void fn_80098E9C(s16 interrupt, OSContext* context) {
    OSContext exceptionContext;
    s32 chan = (interrupt - 10) / 3;
    EXIControl* exi = &lbl_803FB3C8[chan];
    EXICallback callback;

    __OSMaskInterrupts(0x80000000u >> interrupt);
    fn_800986A0(chan, 0, 1, 0);

    callback = exi->tcCallback;
    if (callback) {
        exi->tcCallback = NULL;
        CompleteTransfer(chan);

        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
void fn_80098FF8(volatile s16 interrupt, OSContext* context) {
    OSContext exceptionContext;
    s32 chan = (interrupt - 11) / 3;
    EXIControl* exi;
    EXICallback callback;

    __OSMaskInterrupts(0x700000u >> (chan * 3));
    __EXIRegs[chan * 5] = 0;
    exi = &lbl_803FB3C8[chan];
    callback = exi->extCallback;
    exi->state &= ~8;
    if (callback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        exi->extCallback = NULL;
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma scheduling off
void EXIInit(void) {
    __OSMaskInterrupts(0x7F8000);
    ExiHw->regs[0].csr = 0;
    ExiHw->regs[1].csr = 0;
    ExiHw->regs[2].csr = 0;
    ExiHw->regs[0].csr = 0x2000;
    __OSSetInterruptHandler(9, fn_80098DDC);
    __OSSetInterruptHandler(10, fn_80098E9C);
    __OSSetInterruptHandler(11, fn_80098FF8);
    __OSSetInterruptHandler(12, fn_80098DDC);
    __OSSetInterruptHandler(13, fn_80098E9C);
    __OSSetInterruptHandler(14, fn_80098FF8);
    __OSSetInterruptHandler(15, fn_80098DDC);
    __OSSetInterruptHandler(16, fn_80098E9C);

    if (OSGetConsoleType() & 0x10000000) {
        LowMem->exiProbeStartTime[0] = LowMem->exiProbeStartTime[1] = 0;
        lbl_803FB3C8[0].idTime = lbl_803FB3C8[1].idTime = 0;
        fn_80098790(0);
        fn_80098790(1);
    }
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 i;

    if (exi->state & 0x10) {
        if (unlockedCallback) {
            for (i = 0; i < exi->items; i++) {
                if (exi->queue[i].device == dev) {
                    OSRestoreInterrupts(enabled);
                    return FALSE;
                }
            }

            exi->queue[exi->items].callback = unlockedCallback;
            exi->queue[exi->items].device = dev;
            exi->items++;
        }

        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state |= 0x10;
    exi->device = dev;
    fn_80098110(chan, exi);
    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimize_for_size on
#pragma scheduling off
BOOL EXIUnlock(s32 chan) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    BOOL enabled = OSDisableInterrupts();

    if (!(exi->state & 0x10)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state &= ~0x10;
    fn_80098110(chan, exi);

    if (exi->items > 0) {
        EXICallback callback = exi->queue[0].callback;

        if (--exi->items > 0) {
            memmove(&exi->queue[0], &exi->queue[1], exi->items * sizeof(EXIQueueEntry));
        }
        callback(chan, NULL);
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma scheduling reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma peephole off
#pragma scheduling off
void OSRegisterVersion(const char* version);
u32 fn_800993A8(s32 chan) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    return exi->state;
}
#pragma scheduling reset
#pragma peephole reset
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma scheduling off
BOOL fn_800993D0(s32 chan) {
    u32 id;
    u32 unused;

    return fn_80099400(chan, 0, &id);
}
#pragma scheduling reset
#pragma pop

/*
 * EXIGetID - 0x80099400 | size: 0x390
 *
 * __EXIAttach, EXIDetach and EXIUnlock are all inlined here in the target,
 * so their bodies are repeated rather than called.
 */
#endif /* !SDK_OS_SUFFIX_ONLY */
#ifndef SDK_EXI_PREFIX_ONLY
#if !defined(SDK_OS_SUFFIX_ONLY)
/* RULE-EXCEPTION(user-approved): local optimization_level 0, as the
   neighbouring EXIBios functions in this file use — see
   docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 0
/*
 * RULE-EXCEPTION(user-approved): inline copies of __EXIAttach, EXIUnlock
 * and EXIDetach (EXIBios.c); retail expands all three inside EXIGetID, so
 * they are repeated here as static inlines — see docs/RULE_EXCEPTIONS.md
 */
static inline int EXIGetID_Attach(s32 chan, EXICallback extCallback) {
    EXIControl* exi;
    BOOL enabled;

    exi = &lbl_803FB3C8[chan];
    enabled = OSDisableInterrupts();

    if ((exi->state & 8) || !fn_80098790(chan)) {
        OSRestoreInterrupts(enabled);
        return 0;
    }

    fn_800986A0(chan, 1, 0, 0);
    exi->extCallback = extCallback;
    __OSUnmaskInterrupts(0x100000U >> (chan * 3));
    exi->state |= 8;

    OSRestoreInterrupts(enabled);
    return 1;
}

static inline int EXIGetID_Unlock(s32 chan) {
    EXIControl* exi;
    BOOL enabled;
    EXICallback unlockedCallback;

    exi = &lbl_803FB3C8[chan];
    enabled = OSDisableInterrupts();

    if (!(exi->state & 0x10)) {
        OSRestoreInterrupts(enabled);
        return 0;
    }

    exi->state &= ~0x10;
    fn_80098110(chan, exi);
    if (exi->items > 0) {
        unlockedCallback = exi->queue[0].callback;
        if (--exi->items > 0) {
            memmove(&exi->queue[0], &exi->queue[1], exi->items * 8);
        }
        unlockedCallback(chan, 0);
    }

    OSRestoreInterrupts(enabled);
    return 1;
}

static inline int EXIGetID_Detach(s32 chan) {
    EXIControl* exi;
    BOOL enabled;

    exi = &lbl_803FB3C8[chan];
    enabled = OSDisableInterrupts();

    if (!(exi->state & 8)) {
        OSRestoreInterrupts(enabled);
        return 1;
    }

    if ((exi->state & 0x10) && (exi->device == 0)) {
        OSRestoreInterrupts(enabled);
        return 0;
    }

    exi->state &= ~8;
    __OSMaskInterrupts(0x700000U >> (chan * 3));

    OSRestoreInterrupts(enabled);
    return 1;
}

s32 fn_80099400(s32 chan, u32 dev, u32* id) {
    EXIControl* exi = &lbl_803FB3C8[chan];
    int err;
    u32 cmd;
    s32 startTime;
    BOOL enabled;

    if ((chan < 2) && (dev == 0)) {
        if (fn_80098790(chan) == 0) {
            return 0;
        }

        if (exi->idTime == __EXIProbeStartTime[chan]) {
            *id = exi->id;
            return exi->idTime;
        }

        if (!EXIGetID_Attach(chan, NULL)) {
            return 0;
        }

        startTime = __EXIProbeStartTime[chan];
    }

    err = !EXILock(chan, dev, (chan < 2 && dev == 0) ? (EXICallback)fn_800993D0 : NULL);
    if (err == 0) {
        err = !EXISelect(chan, dev, 0);
        if (err == 0) {
            cmd = 0;
            err |= !EXIImm(chan, &cmd, 2, 1, 0);
            err |= !EXISync(chan);
            err |= !EXIImm(chan, id, 4, 0, 0);
            err |= !EXISync(chan);
            err |= !EXIDeselect(chan);
        }

        EXIGetID_Unlock(chan);
    }

    if ((chan < 2) && (dev == 0)) {
        EXIGetID_Detach(chan);
        enabled = OSDisableInterrupts();
        err |= __EXIProbeStartTime[chan] != startTime;

        if (!err) {
            exi->id = *id;
            exi->idTime = startTime;
        }

        OSRestoreInterrupts(enabled);

        if (err) {
            return 0;
        }

        return exi->idTime;
    }

    if (err) {
        return 0;
    }

    return 1;
}
#pragma pop

#endif /* !SDK_OS_SUFFIX_ONLY */

/* OS.c. Hand-written Dolphin SDK asm (zeroes the FPRs, paired singles
   too when HID2[PSE] is set); evidence: docs/asm_evidence/os_primitives.md */
extern f64 ZeroF_8047A6B0;
extern f32 ZeroPS_8047A6B8[2];

asm void __OSFPRInit(void) {
    nofralloc
    mfmsr   r3
    ori     r3, r3, 0x2000
    mtmsr   r3
    mfspr   r3, 920 /* HID2 */
    extrwi. r3, r3, 1, 2
    beq     _regular
    lis     r3, ZeroPS_8047A6B8@ha
    addi    r3, r3, ZeroPS_8047A6B8@l
    psq_l   f0, 0(r3), 0, 0
    ps_mr   f1, f0
    ps_mr   f2, f0
    ps_mr   f3, f0
    ps_mr   f4, f0
    ps_mr   f5, f0
    ps_mr   f6, f0
    ps_mr   f7, f0
    ps_mr   f8, f0
    ps_mr   f9, f0
    ps_mr   f10, f0
    ps_mr   f11, f0
    ps_mr   f12, f0
    ps_mr   f13, f0
    ps_mr   f14, f0
    ps_mr   f15, f0
    ps_mr   f16, f0
    ps_mr   f17, f0
    ps_mr   f18, f0
    ps_mr   f19, f0
    ps_mr   f20, f0
    ps_mr   f21, f0
    ps_mr   f22, f0
    ps_mr   f23, f0
    ps_mr   f24, f0
    ps_mr   f25, f0
    ps_mr   f26, f0
    ps_mr   f27, f0
    ps_mr   f28, f0
    ps_mr   f29, f0
    ps_mr   f30, f0
    ps_mr   f31, f0
_regular:
    lfd     f0, ZeroF_8047A6B0
    fmr     f1, f0
    fmr     f2, f0
    fmr     f3, f0
    fmr     f4, f0
    fmr     f5, f0
    fmr     f6, f0
    fmr     f7, f0
    fmr     f8, f0
    fmr     f9, f0
    fmr     f10, f0
    fmr     f11, f0
    fmr     f12, f0
    fmr     f13, f0
    fmr     f14, f0
    fmr     f15, f0
    fmr     f16, f0
    fmr     f17, f0
    fmr     f18, f0
    fmr     f19, f0
    fmr     f20, f0
    fmr     f21, f0
    fmr     f22, f0
    fmr     f23, f0
    fmr     f24, f0
    fmr     f25, f0
    fmr     f26, f0
    fmr     f27, f0
    fmr     f28, f0
    fmr     f29, f0
    fmr     f30, f0
    fmr     f31, f0
    mtfsf   0xFF, f0
    blr
}

u32 OSGetConsoleType(void) {
    if (BootInfo_8047A6A0 == NULL || BootInfo_8047A6A0->consoleType == 0) {
        return 0x10000002;
    }
    return BootInfo_8047A6A0->consoleType;
}

extern u32 BOOT_REGION_START : 0x812FDFF0;
extern u32 BOOT_REGION_END : 0x812FDFEC;

static void ClearArena(void) {
    if (OSGetResetCode() != 0x80000000) {
        __OSSavedRegionStart.address = 0;
        __OSSavedRegionEnd.address = 0;
        memset(OSGetArenaLo(), 0U, (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
        return;
    }

    __OSSavedRegionStart.address = BOOT_REGION_START;
    __OSSavedRegionEnd.address = BOOT_REGION_END;
    if (BOOT_REGION_START == 0U) {
        memset(OSGetArenaLo(), 0U, (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
        return;
    }

    if ((u32)OSGetArenaLo() < __OSSavedRegionStart.address) {
        if ((u32)OSGetArenaHi() <= __OSSavedRegionStart.address) {
            memset(OSGetArenaLo(), 0U,
                   (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
            return;
        }

        memset(OSGetArenaLo(), 0U,
               __OSSavedRegionStart.address - (u32)OSGetArenaLo());

        {
            void* savedRegionEnd;

            if ((u32)OSGetArenaHi() >
                (u32)(savedRegionEnd = __OSSavedRegionEnd.pointer)) {
                memset(savedRegionEnd, 0,
                       (u32)OSGetArenaHi() - (u32)savedRegionEnd);
            }
        }
    }
}

static void InquiryCallback(s32 result, DVDCommandBlock* block) {
    (void)result;

    switch (block->state) {
    case 0:
        LowMem->dvdDeviceCode = DriveInfo_803FB4A0.deviceCode | 0x8000;
        break;
    default:
        LowMem->dvdDeviceCode = 1;
        break;
    }
}

static void OSExceptionInit(void);

const char* __OSVersion =
    "<< Dolphin SDK - OS\trelease build: Mar 17 2003 04:20:41 (0x2301) >>";

/* 0x80099A44 | size: 0x3D8 */
void OSInit(void) {
    extern void OSDisableInterrupts(void);
    extern void OSEnableInterrupts(void);
    extern s64 __OSGetSystemTime(void);
    extern void PPCMtmmcr0(u32);
    extern void PPCMtmmcr1(u32);
    extern void PPCMtpmc1(u32);
    extern void PPCMtpmc2(u32);
    extern void PPCMtpmc3(u32);
    extern void PPCMtpmc4(u32);
    extern void PPCDisableSpeculation(void);
    extern u32 PPCMfhid2(void);
    extern void PPCMthid2(u32);
    extern void OSSetArenaLo(void* addr);
    extern void OSSetArenaHi(void* addr);
    extern void __OSInitSystemCall(void);
    extern void OSInitAlarm(void);
    extern void __OSModuleInit(void);
    extern void __OSInterruptInit(void);
    extern void __OSSetInterruptHandler(u32 interrupt, void* handler);
    extern void __OSResetSWInterruptHandler(void);
    extern void __OSContextInit(void);
    extern void __OSCacheInit(void);
    extern void SIInit(void);
    extern void __OSInitSram(void);
    extern void __OSThreadInit(void);
    extern void __OSInitAudioSystem(void);
    extern void __OSInitMemoryProtection(void);
    extern void OSReport(const char* format, ...);
    extern void OSRegisterVersion(const char* version);
    extern void EnableMetroTRKInterrupts(void);
    extern void DVDInit(void);
    extern void DCInvalidateRange(void* addr, u32 nBytes);
    extern void DVDInquiryAsync(DVDCommandBlock* block, DVDDriveInfo* info,
                                void* callback);
    extern s64 __OSStartTime;
    extern BOOL AreWeInitialized_8047A6C0;
    extern u32 BI2DebugFlagHolder_8047A6A8;
    extern u32 __PADSpec;
    extern u32 __DVDLongFileNameFlag;
    extern u8 __ArenaLo[];
    extern u8 __ArenaHi[];
    extern u8 _stack_addr[];

    u32 consoleType;
    u32* debugInfo;
    void* debugArenaLo;

    if (AreWeInitialized_8047A6C0 == FALSE) {
        AreWeInitialized_8047A6C0 = TRUE;
        __OSStartTime = __OSGetSystemTime();
        OSDisableInterrupts();

        PPCMtmmcr0(0);
        PPCMtmmcr1(0);
        PPCMtpmc1(0);
        PPCMtpmc2(0);
        PPCMtpmc3(0);
        PPCMtpmc4(0);
        PPCDisableSpeculation();
        PPCSetFpNonIEEEMode();

        BI2DebugFlag_8047A6A4 = 0;
        BootInfo_8047A6A0 = (OSBootInfo*)0x80000000;
        __DVDLongFileNameFlag = 0;

        debugInfo = (u32*)(*(u32*)0x800000F4);
        if (debugInfo != NULL) {
            BI2DebugFlag_8047A6A4 = &debugInfo[3];
            __PADSpec = debugInfo[9];
            *(u8*)0x800030E8 = (u8)*BI2DebugFlag_8047A6A4;
            *(u8*)0x800030E9 = (u8)__PADSpec;
        } else if (BootInfo_8047A6A0->arenaHi) {
            BI2DebugFlagHolder_8047A6A8 = (u32)*(u8*)0x800030E8;
            BI2DebugFlag_8047A6A4 = &BI2DebugFlagHolder_8047A6A8;
            __PADSpec = (u32)*(u8*)0x800030E9;
        }

        __DVDLongFileNameFlag = 1;

        OSSetArenaLo((BootInfo_8047A6A0->arenaLo == 0) ? (void*)__ArenaLo
                                                          : (void*)BootInfo_8047A6A0->arenaLo);
        if ((BootInfo_8047A6A0->arenaLo == 0) && (BI2DebugFlag_8047A6A4 != NULL) &&
            (*BI2DebugFlag_8047A6A4 < 2)) {
            debugArenaLo = (char*)(((u32)_stack_addr + 0x1f) & ~0x1f);
            OSSetArenaLo(debugArenaLo);
        }
        OSSetArenaHi((BootInfo_8047A6A0->arenaHi == 0) ? (void*)__ArenaHi
                                                          : (void*)BootInfo_8047A6A0->arenaHi);

        OSExceptionInit();
        __OSInitSystemCall();
        OSInitAlarm();
        __OSModuleInit();
        __OSInterruptInit();
        __OSSetInterruptHandler(0x16, &__OSResetSWInterruptHandler);
        __OSContextInit();
        __OSCacheInit();
        EXIInit();
        SIInit();
        __OSInitSram();
        __OSThreadInit();
        __OSInitAudioSystem();
        PPCMthid2(PPCMfhid2() & 0xBFFFFFFF);

        if (__OSInIPL == FALSE) {
            __OSInitMemoryProtection();
        }

        OSReport("\nDolphin OS\n");
        OSReport("Kernel built : %s %s\n", "Mar 17 2003", "04:20:41");
        OSReport("Console Type : ");

        consoleType = OSGetConsoleType();
        switch (consoleType & 0xF0000000) {
        case 0x00000000:
            OSReport("Retail %d\n", consoleType);
            break;
        case 0x10000000:
        case 0x20000000:
            switch (consoleType & 0x0FFFFFFF) {
            case 0x10000000:
                OSReport("Mac Emulator\n");
                break;
            case 0x10000001:
                OSReport("PC Emulator\n");
                break;
            case 0x10000002:
                OSReport("EPPC Arthur\n");
                break;
            case 0x10000003:
                OSReport("EPPC Minnow\n");
                break;
            default:
                OSReport("Development HW%d (%08x)\n",
                         (consoleType & 0x0FFFFFFF) - 3, consoleType);
                break;
            }
            break;
        default:
            OSReport("%08x\n", consoleType);
            break;
        }

        OSReport("Memory %d MB\n", (u32)BootInfo_8047A6A0->memorySize >> 0x14U);
        OSReport("Arena : 0x%x - 0x%x\n", OSGetArenaLo(), OSGetArenaHi());
        OSRegisterVersion(__OSVersion);

        if (BI2DebugFlag_8047A6A4 && ((*BI2DebugFlag_8047A6A4) >= 2)) {
            EnableMetroTRKInterrupts();
        }

        ClearArena();
        OSEnableInterrupts();

        if (__OSInIPL == FALSE) {
            DVDInit();

            if (__OSIsGcam) {
                LowMem->dvdDeviceCode = 0x9000;
                return;
            }

            DCInvalidateRange(&DriveInfo_803FB4A0, sizeof(DriveInfo_803FB4A0));
            DVDInquiryAsync(&DriveBlock_803FB4C0, &DriveInfo_803FB4A0,
                            InquiryCallback);
        }
    }
}

static u32 __OSExceptionLocations[] = {
    0x00000100, 0x00000200, 0x00000300, 0x00000400, 0x00000500,
    0x00000600, 0x00000700, 0x00000800, 0x00000900, 0x00000C00,
    0x00000D00, 0x00000F00, 0x00001300, 0x00001400, 0x00001700,
};

static void OSExceptionInit(void) {
    u8 exception;
    u32* destAddr;
    u32* opCodeAddr;
    u32 oldOpCode;
    u8* handlerStart;
    u32 handlerSize;

    opCodeAddr = __OSEVSetNumber;
    oldOpCode = *opCodeAddr;
    handlerStart = __OSEVStart;
    handlerSize = (u32)(__OSEVEnd - __OSEVStart);

    destAddr = (u32*)0x80000060;
    if (*destAddr == 0) {
        DBPrintf("Installing OSDBIntegrator\n");
        memcpy(destAddr, (u8*)__OSDBINTSTART, (u32)((u8*)__OSDBINTEND - (u8*)__OSDBINTSTART));
        DCFlushRangeNoSync(destAddr, (u32)((u8*)__OSDBINTEND - (u8*)__OSDBINTSTART));
        __sync();
        ICInvalidateRange(destAddr, (u32)((u8*)__OSDBINTEND - (u8*)__OSDBINTSTART));
    }

    for (exception = 0; exception < OS_EXCEPTION_MAX; exception++) {
        if (BI2DebugFlag_8047A6A4 && *BI2DebugFlag_8047A6A4 >= 2 &&
            __DBIsExceptionMarked(exception)) {
            DBPrintf(">>> OSINIT: exception %d commandeered by TRK\n", exception);
            continue;
        }

        *opCodeAddr = oldOpCode | exception;

        if (__DBIsExceptionMarked(exception)) {
            DBPrintf(">>> OSINIT: exception %d vectored to debugger\n", exception);
            memcpy(__DBVECTOR, (u8*)__OSDBINTEND, (u32)((u8*)__OSDBJUMPEND - (u8*)__OSDBINTEND));
        } else {
            u32* ops = __DBVECTOR;
            int cb;

            for (cb = 0; cb < (u32)((u8*)__OSDBJUMPEND - (u8*)__OSDBINTEND); cb += sizeof(u32)) {
                *ops++ = 0x60000000;
            }
        }

        destAddr = (u32*)(__OSExceptionLocations[exception] + 0x80000000);
        memcpy(destAddr, handlerStart, handlerSize);
        DCFlushRangeNoSync(destAddr, handlerSize);
        __sync();
        ICInvalidateRange(destAddr, handlerSize);
    }

    OSExceptionTable_8047A6C4 = (OSExceptionHandler*)0x80003000;

    for (exception = 0; exception < OS_EXCEPTION_MAX; exception++) {
        __OSSetExceptionHandler(exception, OSDefaultExceptionHandler);
    }

    *opCodeAddr = oldOpCode;
    DBPrintf("Exceptions initialized...\n");
}

/* OS.c. Hand-written Dolphin SDK asm (the debugger integrator and its jump,
   copied by OSExceptionInit); evidence: docs/asm_evidence/os_primitives.md */
asm void __OSDBIntegrator(void) {
    nofralloc
entry __OSDBINTSTART
    li      r5, 0x40
    mflr    r3
    stw     r3, 0xC(r5)
    lwz     r3, 0x8(r5)
    oris    r3, r3, 0x8000
    mtlr    r3
    li      r3, 0x30
    mtmsr   r3
    blr
entry __OSDBINTEND
}

/* OS.c's __OSDBJump; evidence: docs/asm_evidence/os_primitives.md */
asm void fn_8009A0C0(void) {
    nofralloc
    bla     0x60
entry __OSDBJUMPEND
}

OSExceptionHandler __OSSetExceptionHandler(u8 exception, OSExceptionHandler handler) {
    OSExceptionHandler* entry = &OSExceptionTable_8047A6C4[exception];
    OSExceptionHandler old = *entry;

    *entry = handler;
    return old;
}
#endif

