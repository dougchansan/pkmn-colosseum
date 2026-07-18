/**
 * @file PAD_suffix_800ABD68.c
 * @brief Dolphin PAD suffix, 0x800ABD68 - 0x800AC02C.
 */
#include "dolphin/types.h"
#include "dolphin/pad/Pad.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"

extern s32 lbl_80478A0C;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8A8;
extern u32 lbl_8047A8AC;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 lbl_8047A8B8;
extern PADSamplingCallback lbl_8047A8BC;
extern u32 lbl_8047A8C0;
extern PADStatus lbl_803FC5E0[4];

extern u32 SIDisablePolling(u32 poll);
extern void SIGetTypeAsync(s32 chan, SITypeAndStatusCallback cb);
extern BOOL fn_800CF708(void);
extern void PADTypeAndStatusCallback(s32 chan, u32 type);

BOOL fn_800ABD68(BOOL final) {
    BOOL wasSampling;
    BOOL sync;

    wasSampling = (lbl_8047A8BC != 0);
    if (wasSampling) {
        PADSetSamplingCallback(0);
    }

    if (final == 0) {
        sync = (lbl_8047A8A8 == 0) && (lbl_80478A0C == 0x20);
        if (sync) {
            sync = fn_800CF708();
        }

        if (lbl_8047A8C0 == 0 && sync) {
            BOOL enabled = OSDisableInterrupts();
            u32 mask = 0xF0000000;

            mask |= lbl_8047A8B8;
            lbl_8047A8B8 = 0;
            mask &= ~(lbl_8047A8B0 | lbl_8047A8B4);
            lbl_8047A8A8 |= mask;
            lbl_8047A8A4 &= ~mask;
            lbl_8047A8AC |= mask;
            SIDisablePolling(lbl_8047A8A8);

            if (lbl_80478A0C == 0x20) {
                lbl_80478A0C = __cntlzw(lbl_8047A8A8);
                if (lbl_80478A0C != 0x20) {
                    u32 chanBit = 0x80000000u >> lbl_80478A0C;
                    lbl_8047A8A8 &= ~chanBit;
                    memset(&lbl_803FC5E0[lbl_80478A0C], 0, sizeof(PADStatus));
                    SIGetTypeAsync(lbl_80478A0C, (SITypeAndStatusCallback)PADTypeAndStatusCallback);
                }
            }

            OSRestoreInterrupts(enabled);
            lbl_8047A8C0 = 1;
            return FALSE;
        }
        return sync;
    }

    lbl_8047A8C0 = 0;
    return TRUE;
}

static void SamplingHandler(__OSInterrupt interrupt, OSContext *context) {
    OSContext newContext;
    if (lbl_8047A8BC != NULL) {
        OSClearContext(&newContext);
        OSSetCurrentContext(&newContext);
        lbl_8047A8BC();
        OSClearContext(&newContext);
        OSSetCurrentContext(context);
    }
}

PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback callback) {
    PADSamplingCallback old = lbl_8047A8BC;
    lbl_8047A8BC = callback;
    if (callback != NULL) {
        SIRegisterPollingHandler((__OSInterruptHandler)SamplingHandler);
    } else {
        SIUnregisterPollingHandler((__OSInterruptHandler)SamplingHandler);
    }
    return old;
}

BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL old;
    BOOL enabled = OSDisableInterrupts();
    int flags;

    old = (*(volatile u8 *)0x800030E3 & 0x40) ? TRUE : FALSE;
    flags = *(volatile u8 *)0x800030E3;
    flags &= 0xbf;
    *(volatile u8 *)0x800030E3 = flags;
    if (disable) {
        *(volatile u8 *)0x800030E3 = *(volatile u8 *)0x800030E3 | 0x40;
    }

    OSRestoreInterrupts(enabled);
    return old;
}
