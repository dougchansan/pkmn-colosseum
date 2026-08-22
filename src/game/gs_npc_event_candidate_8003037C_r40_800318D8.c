#include "dolphin/types.h"
#include "game/win_sprite.h"

extern void _threadSwitch(void);
extern u32 fn_8010B560(void);
extern void fn_801CB9D8(u32);

extern u32 lbl_8047A418;
extern u32 lbl_8047A420;
extern u32 lbl_8047A424;
extern u32 lbl_8047A42C;
extern u8 lbl_80266E90[];

void fn_800318D8(s32 unused, u8* tgt)
{
    u8* p = lbl_80266E90;
    s32 group = 0;
    s32 sub = 0;
    s32 i;

    for (i = 0; i < 2; i++) {
        if ((s32) * (s16*) (tgt + 6) == (s32) * (u16*) (p + 0xE)) {
            group = p[0];
            sub = p[1];
        }
        p += 0x12;
        if ((s32) * (s16*) (tgt + 6) == (s32) * (u16*) (p + 0xE)) {
            group = p[0];
            sub = p[1];
        }
        p += 0x12;
        if ((s32) * (s16*) (tgt + 6) == (s32) * (u16*) (p + 0xE)) {
            group = p[0];
            sub = p[1];
        }
        p += 0x12;
        if ((s32) * (s16*) (tgt + 6) == (s32) * (u16*) (p + 0xE)) {
            group = p[0];
            sub = p[1];
        }
        p += 0x12;
        if ((s32) * (s16*) (tgt + 6) == (s32) * (u16*) (p + 0xE)) {
            group = p[0];
            sub = p[1];
        }
        p += 0x12;
        if ((s32) * (s16*) (tgt + 6) == (s32) * (u16*) (p + 0xE)) {
            group = p[0];
            sub = p[1];
        }
        p += 0x12;
    }
    if (group == 1 && (s32) lbl_8047A424 == sub) {
        winSpriteSetDisp(tgt, 1);
        return;
    }
    if (group == 2 && (s32) lbl_8047A420 == sub) {
        winSpriteSetDisp(tgt, 1);
        return;
    }
    winSpriteSetDisp(tgt, 0);
}

void fn_80031A1C(void* r3, void* r4)
{
    s32 val = (s32) lbl_8047A42C;

    switch (val) {
    case 7:
    case 2:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}

void fn_80031A70(void* r3, void* r4)
{
    s32 val = (s32) lbl_8047A42C;

    switch (val) {
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}

void fn_80031AC0(void* r3, void* r4)
{
    s32 val = (s32) lbl_8047A42C;

    switch (val) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        winSpriteSetDisp(r4, 1);
        break;
    default:
        winSpriteSetDisp(r4, 0);
        break;
    }
}

void fn_80031B10(void)
{
    while ((u8) fn_8010B560() != 0) {
        _threadSwitch();
    }
    fn_801CB9D8(lbl_8047A418);
}
