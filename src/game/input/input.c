/**
 * @file input.c
 * @brief Low-level PAD/WI input backend for Pokemon Colosseum.
 *
 * This unit is a family of small accessors and mutators over a fixed
 * 4-slot pad-entry table (base lbl_80401C10, entry stride 0x6C, entries
 * found via the shared inline lookup helper WI_FindPad). Each real
 * function reads or writes one field of the matched entry:
 * held/pressed mask, analog X/Y (raw and smoothed), rumble parameters,
 * stick mode, dead zone, connection status, etc. fn_800F8138 performs
 * the one-time table initialization and hooks up the SDK PAD callback.
 *
 * A previous pass on this file invented an entire parallel "PADInput_*"
 * wrapper API (PADInput_Init, PADInput_ReadButtons, PADInput_GetHeld,
 * ...) with its own PADStatus/PADInputState structs and GSmem-based
 * allocation. None of those names exist in config/GC6E01/symbols.txt,
 * nothing outside this file referenced them, and several of them
 * (e.g. "PADInput_Init" claimed at fn_800F7758) collided with real
 * addresses that already have genuine, differently-shaped definitions
 * further down in this same file. That block has been removed; only
 * the address-verified fn_800Fxxxx definitions remain.
 *
 * Address range: 0x800F75FC - 0x800F8268 (this unit's PAD/WI functions)
 */

#include "game/input/input.h"

/* memset is used by the table-init function (fn_800F8138) below. */
extern void* memset(void* dst, int val, u32 size);

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 4 function(s)
 * =================================================================== */

/* fn_800F760C - 0x800F760C | size: 0xD8 */
/*
 * InputRemoveAndScan - Remove an input entry from the linked list
 * and scan pad entries for matching IDs.
 *
 * Walks the linked list at (state+0x08) to find and unlink the
 * target entry. Then iterates through pad slots to mark matching
 * entries with status 3.
 *
 * 0x800F760C | size: 0xCC
 */
s32 fn_800F760C(u8* target) {
    extern u8 lbl_802712B8[];
    extern u8* lbl_80478B00;
    extern void GSlogWritef(const char* msg, ...);
    u8* state = lbl_80478B00;
    u8* prev;
    u8* cur;
    u32 offset;
    u8* padData;
    s32 i;
    u8 status;

    /* Try to unlink target from the linked list at state+0x08 */
    cur = *(u8**)(state + 0x08);
    if (cur == target) {
        /* Target is head: remove it */
        *(u32*)(state + 0x08) = *(u32*)(cur + 0x14);
    } else {
        /* Walk the list to find target */
        prev = cur;
        while ((cur = *(u8**)(prev + 0x14)) != NULL) {
            if (cur == target) {
                /* Unlink: prev->next = cur->next */
                *(u32*)(prev + 0x14) = *(u32*)(cur + 0x14);
                goto scan;
            }
            prev = cur;
        }
        if (cur == NULL) {
            /* Not found in list */
            GSlogWritef((const char*)lbl_802712B8, target);
            return -1;
        }
    }

scan:
    /* Scan pad entries and mark matching ones */
    i = 0;
    offset = 0;
    status = 3;
    while (i < (s32)*(u16*)lbl_80478B00) {
        padData = (u8*)(*(u32*)(lbl_80478B00 + 0x0C) + offset);
        if (*(u8*)(padData + 0x04) != 0) {
            u16 entryId = (u16)(*(u32*)(padData + 0x08) >> 16);
            if (*(u16*)(target + 0x00) == entryId) {
                *(u8*)(padData + 0x04) = status;
            }
        }
        offset += 0x16C;
        i++;
    }
    return 0;
}

#if !defined(INPUT_800F760C_ONLY)
/* fn_800F78A4 - 0x800F78A4 | size: 0x7C */
extern u8 lbl_80401C10[];
/* Shared 4-slot pad lookup. Defined before all readers so CW 1.3 inlines it:
 * the inlined multi-`return pad` yields the target's un-inverted `bne;b` per
 * slot, and per-function optimization_level 2 keeps the `nul` sentinel in its
 * own register instead of folding to `li rPad,0`. */
static inline u8* WI_FindPad(s32 padId) {
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    return nul;
}

/*
 * InputSetRumble - Find a pad entry by ID and configure rumble.
 *
 * Searches through up to 4 pad entries in lbl_80401C10 for a
 * matching pad ID. If found and non-null, sets rumble parameters.
 *
 * 0x800F78A4 | size: 0x7C
 */
void fn_800F78A4(s32 padId, u8 mode, u8 strength, u32 duration, u8 flags) {
#pragma optimization_level 2
    u8* pad;

    pad = WI_FindPad(padId);

    if (pad == NULL) { return; }
    if (mode != 0) { return; }

    /* Set rumble parameters */
    *(u32*)(pad + 0x5C) = 1;
    *(u32*)(pad + 0x60) = (u32)(strength & 0xFF) * 0xF;
    *(u32*)(pad + 0x64) = duration;
    *(u8*)(pad + 0x68) = flags;
}

/*
 * InputGetAnalogY - Get analog Y stick value for a pad by ID.
 *
 * Searches pad entries for a match and returns the Y axis value.
 * If mode == 1, reads from offset 0x5B (smoothed), else 0x29 (raw).
 *
 * 0x800F7920 | size: 0x74
 */
u8 fn_800F7920(s32 padId, s32 mode) {
#pragma optimization_level 2
    u8* pad;

    pad = WI_FindPad(padId);

    if (pad == NULL) { return 0; }

    if (mode == 1) {
        return *(u8*)(pad + 0x5B);
    }
    return *(u8*)(pad + 0x29);
}

/*
 * InputGetAnalogX - Get analog X stick value for a pad by ID.
 *
 * Same pattern as InputGetAnalogY but reads offsets 0x5A / 0x28.
 *
 * 0x800F7994 | size: 0x74
 */
u8 fn_800F7994(s32 padId, s32 mode) {
#pragma optimization_level 2
    u8* pad;

    pad = WI_FindPad(padId);

    if (pad == NULL) { return 0; }

    if (mode == 1) {
        return *(u8*)(pad + 0x5A);
    }
    return *(u8*)(pad + 0x28);
}

extern u8* lbl_80478B00;
s32 fn_800F75FC(u32 val) {
    *(u32*)(lbl_80478B00 + 0x10) = val;
    return 0;
}
extern u8* lbl_80478B00;
#if 0
asm void fn_800F76E4(void) {
#include "src/game/input/input_fn_800F76E4.inc"
}
#else
void fn_800F76E4(u8* arg) {
    extern u8* lbl_80478B00;
    u8* head;
    u32 off;
    u32* tbl;
    s32 i;

    *(u32*)(arg + 0x14) = 0;
    head = (u8*)*(u32*)(lbl_80478B00 + 0x8);
    if (head == NULL) {
        *(u32*)(lbl_80478B00 + 0x8) = (u32)arg;
    } else {
        *(u32*)(lbl_80478B00 + 0x8) = (u32)arg;
        *(u32*)(arg + 0x14) = (u32)head;
    }
    if (*(u8*)(arg + 0xa) != 0) return;
    tbl = (u32*)(arg + *(u32*)(arg + 0xc));
    i = 0;
    while (i < (s32)*(u16*)(arg + 0x6)) {
        off = *tbl;
        tbl += 1;
        i += 1;
        *(u32*)(arg + off) += (u32)arg;
    }
    *(u8*)(arg + 0xa) = 1;
}
#endif
extern void _toolentryAlloc__FUl(void);
extern void GSlogWritef(void);
extern u8 lbl_80401BF8[];
extern u8* lbl_80478B00;
extern u8 lbl_802712E4[];
#if 1
s32 fn_800F7758(u32 count) {
    extern void* memset(void* dst, int val, u32 size);
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* fn_800E27B0(u16 handle);
    extern void GSlogWritef(const char* msg, ...);
    extern u8 lbl_80401BF8[];
    extern u8* lbl_80478B00;
    extern u8 lbl_802712E4[];
    s32 count16;
    u8* item;
    s32 i;

    memset(lbl_80401BF8, 0, 0x14);
    count16 = (u16)count;
    lbl_80478B00 = lbl_80401BF8;
    *(u16*)(lbl_80478B00 + 2) = _toolentryAlloc__FUl(count16 * 0x16c);
    if (*(u16*)(lbl_80478B00 + 2) == 0) {
        GSlogWritef((const char*)lbl_802712E4);
        return -1;
    }

    *(u32*)(lbl_80478B00 + 0xc) = (u32)fn_800E27B0(*(u16*)(lbl_80478B00 + 2));
    item = *(u8**)(lbl_80478B00 + 0xc);
    for (i = 0; i < count16; i++) {
        *(u32*)(item + 0) = 0;
        *(u8*)(item + 4) = 0;
        item += 0x16c;
    }
    *(u16*)lbl_80478B00 = count;
    return 0;
}
#else
void fn_800F7758(void) { /* TODO */ }
#endif
#if 0
asm void fn_800F7A08(void) {
#include "src/game/input/input_fn_800F7A08.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7A08(s32 key, s32 sel) {
    u8* pad = WI_FindPad(key);
    if (pad == NULL) return 0;
    if (sel == 1) return *(u8*)(pad + 0x59);
    return *(u8*)(pad + 0x27);
}
#pragma pop
#endif
#if 0
asm void fn_800F7A7C(void) {
#include "src/game/input/input_fn_800F7A7C.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7A7C(s32 key, s32 sel) {
    u8* pad = WI_FindPad(key);
    if (pad == NULL) return 0;
    if (sel == 1) return *(u8*)(pad + 0x58);
    return *(u8*)(pad + 0x26);
}
#pragma pop
#endif
#if 0
asm void fn_800F7AF0(void) {
#include "src/game/input/input_fn_800F7AF0.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7AF0(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return (u32)*(u16*)(pad + 0x24) ^ *(u32*)(pad + 0x30);
}
#pragma pop
#endif
#if 0
asm void fn_800F7B5C(void) {
#include "src/game/input/input_fn_800F7B5C.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7B5C(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return ~(u32)*(u16*)(pad + 0x24);
}
#pragma pop
#endif
#if 0
asm void fn_800F7BC4(void) {
#include "src/game/input/input_fn_800F7BC4.inc"
}
#else
#pragma push
#pragma optimization_level 2
u16 fn_800F7BC4(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return *(u16*)(pad + 0x24);
}
#pragma pop
#endif
#if 0
asm void fn_800F7C28(void) {
#include "src/game/input/input_fn_800F7C28.inc"
}
#else
#pragma push
#pragma optimization_level 2
u32 fn_800F7C28(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 2;
    return *(u32*)(pad + 0x4);
}
#pragma pop
#endif
extern u32 lbl_8047CCC8;
#if 0
asm void fn_800F7C8C(void) {
#include "src/game/input/input_fn_800F7C8C.inc"
}
#else
#pragma push
#pragma optimization_level 3
void fn_800F7C8C(s32 padId, s8 stickX, s8 stickY) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad != padId) goto try1;
    goto found;
try1:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try2;
    goto found;
try2:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try3;
    goto found;
try3:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto miss;
    goto found;
miss:
    pad = nul;
found:
    if (pad == NULL) return;
    *(s8*)(pad + 0x36) = stickX;
    *(s8*)(pad + 0x37) = stickY;
    *(f32*)(pad + 0x50) = (f32)(s32)stickX;
    *(f32*)(pad + 0x54) = (f32)(s32)stickY;
}
#pragma pop
#endif
extern u32 lbl_8047CCC8;
#if 0
asm void fn_800F7D38(void) {
#include "src/game/input/input_fn_800F7D38.inc"
}
#else
#pragma push
#pragma optimization_level 3
void fn_800F7D38(s32 padId, s8 stickX, s8 stickY) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad != padId) goto try1;
    goto found;
try1:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try2;
    goto found;
try2:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto try3;
    goto found;
try3:
    pad += 0x6c;
    if (*(s32*)pad != padId) goto miss;
    goto found;
miss:
    pad = nul;
found:
    if (pad == NULL) return;
    *(s8*)(pad + 0x34) = stickX;
    *(s8*)(pad + 0x35) = stickY;
    *(f32*)(pad + 0x48) = (f32)(s32)stickX;
    *(f32*)(pad + 0x4c) = (f32)(s32)stickY;
}
#pragma pop
#endif
static inline u8* PADInput_FindPad(s32 padId) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;
    if (*(s32*)pad == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    if (*(s32*)(pad += 0x6c) == padId) return pad;
    return nul;
}
#if 0
asm void fn_800F7DE4(void) {
#include "src/game/input/input_fn_800F7DE4.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_800F7DE4(s32 padId, u32 val) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return;
    *(u32*)(pad + 0x14) = val;
}
#pragma pop
#endif
#if 0
asm void fn_800F7E40(void) {
#include "src/game/input/input_fn_800F7E40.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_800F7E40(s32 padId, u8 val) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return;
    *(u8*)(pad + 0x10) = val;
}
#pragma pop
#endif
#if 0
asm void fn_800F7E9C(void) {
#include "src/game/input/input_fn_800F7E9C.inc"
}
#else
#pragma push
#pragma optimization_level 2
void fn_800F7E9C(s32 padId, u32 val) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return;
    *(u32*)(pad + 0x8) = val;
}
#pragma pop
#endif
#if 0
asm void fn_800F7EF8(void) {
#include "src/game/input/input_fn_800F7EF8.inc"
}
#else
#pragma push
#pragma optimization_level 2
u8 fn_800F7EF8(s32 padId) {
    u8* pad = WI_FindPad(padId);
    if (pad == NULL) return 0;
    return *(u32*)(pad + 0xc) == 0;
}
#pragma pop
#endif
extern void fn_800D3094(void);
extern void fn_800AAD34(void);
extern void fn_800F8654(void);
extern void fn_800F8A54(void);
extern void OSDisableInterrupts();
extern void* memcpy(void* dst, const void* src, u32 n);
extern void OSRestoreInterrupts();
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
#if 1
#pragma push
#pragma optimization_level 2
u32 fn_800F7F64(s32 padId) {
    extern u32 OSDisableInterrupts(void);
    extern void OSRestoreInterrupts(u32);
    extern u32 fn_800D3094(void);
    extern void fn_800AAD34(u32);
    extern void* memcpy(void* dst, const void* src, u32 n);
    extern void fn_800F8654(u8*, u8, u8, u8*, u8*, u8*, u8*, f32*, f32*);
    extern void fn_800F8A54(u8*);
    extern u32 lbl_8047AC4C;
    extern u32 lbl_8047AC50;
    u8* pad;
    u32 irq;
    u32 token;

    pad = WI_FindPad(padId);
    if (pad == NULL) {
        return 2;
    }

    *(u32*)(pad + 0x30) = *(u16*)(pad + 0x24);
    irq = OSDisableInterrupts();
    token = fn_800D3094();
    if (*(volatile u32*)&lbl_8047AC4C != 0 && token != lbl_8047AC50) {
        fn_800AAD34(*(volatile u32*)&lbl_8047AC4C);
        lbl_8047AC50 = token;
    }
    memcpy(pad + 0x24, pad + 0x18, 0xc);
    OSRestoreInterrupts(irq);
    fn_800F8654(pad, *(u8*)(pad + 0x26), *(u8*)(pad + 0x27),
                pad + 0x34, pad + 0x35, pad + 0x38, pad + 0x3c,
                (f32*)(pad + 0x48), (f32*)(pad + 0x4c));
    fn_800F8654(pad, *(u8*)(pad + 0x28), *(u8*)(pad + 0x29),
                pad + 0x36, pad + 0x37, pad + 0x40, pad + 0x44,
                (f32*)(pad + 0x50), (f32*)(pad + 0x54));
    fn_800F8A54(pad);
    return *(u32*)(pad + 0xc);
}
#pragma pop
#else
void fn_800F7F64(void) { /* TODO */ }
#endif
#if 0
asm void fn_800F80B0(void) {
#include "src/game/input/input_fn_800F80B0.inc"
}
#else
s32 fn_800F80B0(s32 padIdx) {
    extern u8 lbl_80401C10[];
    u8* pad = &lbl_80401C10[0];
    u8* nul = NULL;

    if (*(s32*)pad != 0) {
        pad += 0x6C;
        if (*(s32*)pad != 0) {
            pad += 0x6C;
            if (*(s32*)pad != 0) {
                pad += 0x6C;
                if (*(s32*)pad != 0) {
                    pad = nul;
                }
            }
        }
    }
    if (pad == NULL) {
        return 4;
    }
    if (padIdx < 5 && padIdx >= 1) {
        *(u32*)pad = padIdx;
        return 0;
    }
    return 1;
}
#endif
extern void PADSetAnalogMode(void);
extern void fn_800AAF38(void);
extern void fn_800AB4FC(void);
extern void fn_800AAE34(void);
extern void fn_800F8268(void);
extern void fn_800ABF5C(void);
extern void SISetSamplingRate();
extern u32 lbl_8047AC48;
extern u32 lbl_8047CCD0;
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
#if 0
asm void fn_800F8138(void) {
#include "src/game/input/input_fn_800F8138.inc"
}
#else
#pragma push
#pragma scheduling on
void fn_800F8138(void) {
    extern u8 lbl_80401C10[];
    extern u32 lbl_8047AC48;
    extern u32 lbl_8047CCD0;
    extern u32 lbl_8047AC4C;
    extern u32 lbl_8047AC50;
    extern void PADSetAnalogMode(s32);
    extern void fn_800AAF38(void);
    extern void fn_800AB4FC(void*);
    extern void fn_800AAE34(u32);
    extern void fn_800ABF5C(void*);
    extern void SISetSamplingRate(s32);
    u8* base;
    u8* pad;
    u32* arr1c0;
    u32* arr1b0;
    s32 i;
    f32 zero_f;

    lbl_8047AC48 = 0;
    base = lbl_80401C10;
    lbl_8047AC4C = 0xF0000000;
    pad = base;
    zero_f = *(f32*)&lbl_8047CCD0;
    arr1c0 = (u32*)(base + 0x1c0);
    arr1b0 = (u32*)(base + 0x1b0);
    lbl_8047AC50 = 0;

    for (i = 0; i < 4; i++) {
        *(u32*)(pad + 0x00) = 0;
        *(u32*)(pad + 0x08) = 0;
        *(u32*)(pad + 0x0C) = 3;
        *(u8*) (pad + 0x10) = 0;
        *(u32*)(pad + 0x14) = 0;
        *(u32*)(pad + 0x30) = 0;
        memset(pad + 0x18, 0, 12);
        memset(pad + 0x24, 0, 12);
        *(u8*) (pad + 0x34) = 0;
        *(u8*) (pad + 0x35) = 0;
        *(u8*) (pad + 0x36) = 0;
        *(u8*) (pad + 0x37) = 0;
        *(f32*)(pad + 0x48) = zero_f;
        *(f32*)(pad + 0x4C) = zero_f;
        *(f32*)(pad + 0x50) = zero_f;
        *(f32*)(pad + 0x54) = zero_f;
        *(u32*)(pad + 0x5C) = 3;
        *(u32*)(pad + 0x60) = 0;
        *(u32*)(pad + 0x64) = 0;
        *(u8*) (pad + 0x68) = 0;
        pad += 0x6c;
        *arr1c0++ = 2;
        *arr1b0++ = 0;
    }

    PADSetAnalogMode(0);
    fn_800AAF38();
    fn_800AB4FC(base + 0x1c0);
    fn_800AAE34(lbl_8047AC4C);
    SISetSamplingRate(0xb);
    fn_800ABF5C((void*)fn_800F8268);
}
#pragma pop
#endif
#endif
