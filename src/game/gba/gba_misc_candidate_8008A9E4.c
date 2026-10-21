/**
 * gba_misc 0x8008A9E4 - 0x8008AB4C: entry status query and packed status
 * send. Same code as gbaEntryCheckStatus / fn_8008AB20 in gba_misc.c, kept as a
 * standalone unit (like the gba_misc_exact_* siblings) so it can be linked.
 */
#include "dolphin/types.h"

extern s32 lbl_803FB308[4];
extern s32 lbl_803FB318[4];
extern u16 lbl_8047A67C[4];
extern u16 lbl_8047A684[4];

extern s32 _AGB_EntryGetStatus__FlPUl(s32, u32*);
extern void gbaCmdSend77();

static u32 GbaSwap32(u32 value) {
    return (value << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) |
           (value >> 24);
}

/* 0x8008A9E4 | size: 0x13C */
s32 gbaEntryCheckStatus(s32 idx, u32* out) {
    u32 status;
    s32 ret;

    *out = 0x2000000;
    ret = _AGB_EntryGetStatus__FlPUl(idx - 1, &status);
    if (ret < 0) {
        status = 0x2000000;
        goto end;
    }
    if (ret != 0) {
        *out = 0x3000000;
        lbl_803FB318[idx - 1] = 1;
        lbl_803FB308[idx - 1] = 0;
        lbl_8047A684[idx - 1] = 0;
        lbl_8047A67C[idx - 1] = 0;
        return ret;
    }
    *out = GbaSwap32(status);
    if ((*out >> 24) == 0) {
        lbl_803FB318[idx - 1] = 1;
        lbl_803FB308[idx - 1] = 0;
        lbl_8047A684[idx - 1] = 0;
        lbl_8047A67C[idx - 1] = 0;
    }
end:
    return 0;
}

/* 0x8008AB20 | size: 0x2C */
void fn_8008AB20(s32 param0, u32 param1, u32 param2) {
    u32 packed;

    packed = param2 << 24;
    param0--;
    gbaCmdSend77(param0, packed | param1);
}
