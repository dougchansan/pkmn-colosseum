/**
 * @file PAD_exact_800AB4FC.c
 * @brief Exact Dolphin PAD island, 0x800AB4FC - 0x800ABD68.
 */
#include "dolphin/types.h"
#include "dolphin/pad/Pad.h"
#include "dolphin/si/SI.h"
#include "dolphin/os/OSInterrupt.h"

extern u32 lbl_80478A14;
extern u32 lbl_80478A18;
extern u32 lbl_80478A1C;
extern u32 lbl_8047A8A4;
extern u32 lbl_8047A8B0;
extern u32 lbl_8047A8B4;
extern u32 __PADSpec;
extern PADStatus lbl_803FC5E0[4];

extern void fn_800D0338(s32 chan, u32 command);
extern void fn_800D034C(void);

void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);

#include "dolphin/pad/PAD_spec_inline.h"

void fn_800AB4FC(const u32 *commandArray) {
    BOOL enabled = OSDisableInterrupts();
    s32 chan;
    BOOL commit = FALSE;

    for (chan = 0; chan < 4; chan++, commandArray++) {
        u32 chanBit = 0x80000000u >> chan;
        if (!(lbl_8047A8A4 & chanBit))
            continue;
        if (SIGetType(chan) & 0x20000000)
            continue;

        {
            u32 command = *commandArray;
            if (lbl_80478A18 < 2 && command == 2)
                command = 0;
            fn_800D0338(chan, (lbl_80478A14 | 0x400000) | (command & 3));
            commit = TRUE;
        }
    }

    if (commit)
        fn_800D034C();

    OSRestoreInterrupts(enabled);
}

void PADSetSpec(s32 spec) {
    PADSetSpecInline(spec);
}

void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0008) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0001) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_START : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0080) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0200) ? PAD_BUTTON_START : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft)
        status->button |= PAD_TRIGGER_L;
    if (170 <= status->triggerRight)
        status->button |= PAD_TRIGGER_R;
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static inline s8 ClampS8(s8 var, s8 org) {
    if (0 < org) {
        s8 min = (s8)(-128 + org);
        if (var < min)
            var = min;
    } else if (org < 0) {
        s8 max = (s8)(127 + org);
        if (max < var)
            var = max;
    }
    return var -= org;
}

static inline u8 ClampU8(u8 var, u8 org) {
    if (var < org)
        var = org;
    return var -= org;
}

void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    PADStatus *origin;

    status->button = (u16)((data[0] >> 16) & 0x3FFF);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);

    switch (lbl_80478A14 & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0f) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0f) << 4);
        break;
    case 0x00000200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0f) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    case 0x00000300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1] >> 0);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x00000400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    origin = &lbl_803FC5E0[chan];
    status->stickX = ClampS8(status->stickX, origin->stickX);
    status->stickY = ClampS8(status->stickY, origin->stickY);
    status->substickX = ClampS8(status->substickX, origin->substickX);
    status->substickY = ClampS8(status->substickY, origin->substickY);
    status->triggerLeft = ClampU8(status->triggerLeft, origin->triggerLeft);
    status->triggerRight = ClampU8(status->triggerRight, origin->triggerRight);
}

void PADSetAnalogMode(s32 mode) {
    BOOL enabled = OSDisableInterrupts();
    u32 mask = lbl_8047A8A4;

    lbl_80478A14 = mode << 8;
    lbl_8047A8A4 &= ~mask;
    lbl_8047A8B0 &= ~mask;
    lbl_8047A8B4 &= ~mask;

    SIDisablePolling(mask);

    OSRestoreInterrupts(enabled);
}
