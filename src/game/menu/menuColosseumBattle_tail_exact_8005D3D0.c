/**
 * @file menuColosseumBattle_tail_exact_8005D3D0.c
 * @brief Byte-exact menuColosseumBattle.cpp range, 0x8005D3D0 - 0x8005D5CC.
 */
#include "dolphin/types.h"

extern void menuClose();
extern s32 menuCloseSync(s32, s32);
extern void winSpriteSetDisp(void*, u32);

extern u8 lbl_8047A5A8;
extern const u32 lbl_8047BF38;
extern const u32 lbl_8047BF3C;

/* Address: 0x8005D3D0 | Size: 0xDC */
#pragma push
#pragma peephole off
s32 fn_8005D3D0(s32 target) {
    extern s32 menuOpenCustom(s32, s32, void*, s32, s32, s32, ...);
    extern s32 windowGetActiveID(void);
    s32 i;
    s32 results[2];
    s32 menuResult;

    results[0] = lbl_8047BF38;
    results[1] = lbl_8047BF3C;
    for (i = 0; i < 2; i++) {
        if (target == results[i]) {
            break;
        }
    }
    if (i >= 2) {
        i = 0;
    }
    menuResult = menuOpenCustom(0xA7, windowGetActiveID(), &i, 0, 1, 0);
    menuClose(0xA7);
    menuCloseSync(0xA7, 1);
    if (menuResult <= -1 || menuResult >= 2) {
        return 1;
    }
    return results[menuResult];
}
#pragma pop

/* Address: 0x8005D4AC | Size: 0x48 */
#pragma push
#pragma peephole off
s32 fn_8005D4AC(void* menu, void* sprite) {
    s32 visible;
    u8 flags = lbl_8047A5A8;

    (void)menu;
    if ((flags & 0x8) != 0) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

/* Address: 0x8005D4F4 | Size: 0x48 */
#pragma push
#pragma peephole off
s32 fn_8005D4F4(void* menu, void* sprite) {
    s32 visible;
    u8 flags = lbl_8047A5A8;

    (void)menu;
    if ((flags & 0x4) != 0) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

/* Address: 0x8005D53C | Size: 0x48 */
#pragma push
#pragma peephole off
s32 fn_8005D53C(void* menu, void* sprite) {
    s32 visible;
    u8 flags = lbl_8047A5A8;

    (void)menu;
    if ((flags & 0x2) != 0) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

/* Address: 0x8005D584 | Size: 0x48 */
#pragma push
#pragma peephole off
s32 fn_8005D584(void* menu, void* sprite) {
    s32 visible;
    u8 flags = lbl_8047A5A8;

    (void)menu;
    if ((flags & 0x1) != 0) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop
