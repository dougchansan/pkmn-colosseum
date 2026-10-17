/** Exact shop list callbacks, 0x8002B03C - 0x8002B1A0. */
#include "dolphin/types.h"

extern u32 lbl_8047A3E4;
extern f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern f32 lbl_8047B978;

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; peephole off; optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B03C(void* r3)
{
    void* ctx;
    u8 v;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        GSmsgPrintColor(0, 0, -1, 0x2b2e);
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (pop; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma pop

#pragma peephole off
s32 fn_8002B088(void)
{
    GSmsgPrintColor(0, 0, -1, lbl_8047A3E4);
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole on

s32 fn_8002B0BC(void* r3, u8* r4)
{
    u16 hv;
    u8* ctx;
    u8 pad[8];
    hv = *(u16*)((u8*)r3 + 0x94);
    ctx = *(u8**)((u8*)r3 + 0x60);
    *(u16*)pad = hv;
    if ((s8)pad[0] + 0xa < *(s32*)(ctx + 0x8) + 1) {
        if (*(u16*)(*(void**)ctx) == 0) {
            r4[0x67] = (lbl_8047B97C - lbl_8047A3E8) * lbl_8047B978;
            goto end;
        }
    }
    r4[0x67] = 0;
end:
    return 0;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8002B134(void* r3, u8* r4)
{
    u16 hv;
    void* ctx;
    u8 pad[8];
    hv = *(u16*)((u8*)r3 + 0x94);
    ctx = *(void**)((u8*)r3 + 0x60);
    *(u16*)pad = hv;
    if ((s8)pad[0] > 0) {
        if (*(u16*)(*(void**)ctx) == 0) {
            r4[0x67] = (lbl_8047B97C - lbl_8047A3E8) * lbl_8047B978;
            goto end;
        }
    }
    r4[0x67] = 0;
end:
    return 0;
}
#pragma peephole on
