/** Exact shop status callbacks, 0x8002AB40 - 0x8002AE9C. */
#include "dolphin/types.h"

extern u8 lbl_80266E80[];
extern u32 lbl_804788F0;
extern u8 lbl_802E61D8[];
extern u32 lbl_8047A660;
extern u32 lbl_8047A664;
extern u32 GSmsgGetRect(u32 id);

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8002AB40(void* r3, u8* r4)
{
    u8* ctx;
    u32 table[4];
    u32 value;
    s32 idx;
    u32* limit;

    ctx = *(u8**)((u8*)r3 + 0x60);
    table[0] = *(u32*)(lbl_80266E80 + 0x0);
    table[1] = *(u32*)(lbl_80266E80 + 0x4);
    table[2] = *(u32*)(lbl_80266E80 + 0x8);
    table[3] = *(u32*)(lbl_80266E80 + 0xC);

    if ((ctx[0x1D] & 1) != 0) {
        r4[0x67] = 0;
        return 0;
    }
    if (ctx[0x1C] == 0 || ctx[0x1C] == 1) {
        r4[0x67] = 0;
        return 0;
    }

    switch ((s32)(u32)ctx[0x1C]) {
    case 2:
        value = (u32)heroGetStatus(0, 0xE, 0);
        break;
    case 3:
        if (ctx + 0x20 != NULL) {
            value = *(u32*)(ctx + 0x77C);
        } else {
            value = 0;
        }
        break;
    default:
        value = (u32)heroGetStatus(0, 0xE, 0);
        break;
    }

    idx = lbl_804788F0 - 1;
    limit = (u32*)(lbl_802E61D8 + idx * 4);
    while (idx >= 0) {
        if (*limit <= value) {
            break;
        }
        limit--;
        idx--;
    }
    if (idx < 0) {
        idx = 0;
    }
    if (idx >= 4) {
        idx = 3;
    }

    if ((s32)*(s16*)(r4 + 0x6) == (s32)table[idx]) {
        r4[0x67] = 0xFF;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}

s32 fn_8002ACB8(void* r3, u8* r4)
{
    u8* ctx;
    u32 value;
    u32 text_id;
    s32 x;

    ctx = *(u8**)((u8*)r3 + 0x60);
    if ((ctx[0x1D] & 1) != 0) {
        r4[0x67] = 0;
        return 0;
    }
    if (ctx[0x1C] == 0 || ctx[0x1C] == 1) {
        msgctrlSetValue(0x50, heroGetStatus(0, 0xC, 0));
        text_id = 0x151;
        x = (s32)*(s16*)(r4 + 0x54) -
            (s32)(s16)(GSmsgGetRect(text_id) >> 16);
        GSmsgPrintColor(x, 0, -1, text_id);
        goto done;
    }

    switch ((s32)(u32)ctx[0x1C]) {
    case 2:
        value = (u32)heroGetStatus(0, 0xD, 0);
        break;
    case 3:
        if (ctx + 0x20 != NULL) {
            if ((s32)*(volatile u32*)&lbl_8047A660 > 0) {
                *(u32*)(ctx + 0x778) += *(volatile u32*)&lbl_8047A660;
                *(u32*)(ctx + 0x77C) += *(volatile u32*)&lbl_8047A660;
                lbl_8047A660 = 0;
            }
            if ((s32)lbl_8047A664 > 0) {
                *(u32*)(ctx + 0x778) = 0;
                *(u32*)(ctx + 0x77C) = 0;
                lbl_8047A664 = 0;
            }
            value = *(u32*)(ctx + 0x778);
        } else {
            value = 0;
        }
        break;
    default:
        value = (u32)heroGetStatus(0, 0xD, 0);
        break;
    }

    msgctrlSetValue(0x50, (void*)value);
    text_id = 0x153;
    x = (s32)*(s16*)(r4 + 0x54) -
        (s32)(s16)(GSmsgGetRect(text_id) >> 16);
    GSmsgPrintColor(x + 6, 0, -1, text_id);
done:
    return 0;
}

s32 fn_8002AE44(void* r3, u8* r4)
{
    void* ctx;
    ctx = *(void**)((u8*)r3 + 0x60);
    if (((u8*)ctx)[0x1d] & 1) {
        r4[0x67] = 0;
    }
    return 0;
}

s32 fn_8002AE68(void* r3, u8* r4)
{
    void* ctx;
    u8 v;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        r4[0x67] = 0xcc;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
