/**
 * @file win_msg.c
 * @brief winMsgDraw (0x801058CC - 0x80105A3C), the head of the winMsg TU
 *        (0x801058CC - 0x80106F98; XD's winMsg.cpp). The rest of the TU,
 *        winMsgCtrl through winMsgOpen, is win_msg_exact_80105A3C.c, which
 *        also documents the TU flags (-O4,p, "-opt nopeephole", GC/2.5).
 *
 * Exact (2026-09-30). The item is XD's menu item, a pointer to const
 * (winMsg.cpp's callers pass PC13MENU_ITEM_dd), and its size is read into
 * s16 locals. That is what puts retail's copies in the non-0x50 branch
 * ("lha r3,0x54(r27); lha r0,0x56(r27); mr r29,r3; mr r28,r0"): MWCC treats
 * a const s16 field read into an s16 variable as a conversion, loads it
 * into a temporary and copies it into the variable's home, while the 0x50
 * branch reads the window's own work area, which is not const, and loads
 * straight into the homes. The same rule is written up on
 * _windowCreateItemSprite (window_r50_80104A94_o2.c), which shows the same
 * pair. With the sizes s16, fn_8001EC08 taking them as s32 gives retail's
 * "extsh r5,r29; extsh r6,r28" at both calls; as s32 locals or s16
 * parameters those are plain moves. Every other shape (intermediate locals,
 * accessors, out-parameter inlines, struct and array temporaries, a switch,
 * ternaries) either leaves the loads direct or goes to the stack.
 */
#include "dolphin/types.h"

extern void spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void msgWinDrawBackground(s32, s32, s32, s32, u8, s32);
extern void winMsgDrawMessage(void* message, u32 state, u8 flag);
extern void* windowGetFreeWork(void* window);

typedef struct WinMsgItem {
    u8 pad_00[0x50];
    s16 x;
    s16 y;
    s16 width;
    s16 height;
} WinMsgItem;

/* 0x801058CC | 0x170 */
s32 winMsgDraw(u8* window, const WinMsgItem* item)
{
    u8* work = windowGetFreeWork(window);
    s16 width;
    s16 height;
    s16 offset;

    if (*(s32*)(window + 0x04) == 0x50) {
        width = *(s16*)(work + 0x0C);
        height = *(s16*)(work + 0x0E);
    } else {
        width = item->width;
        height = item->height;
    }

    offset = item->height - 4 - height;
    spriteSetOrigin(*(s16*)(window + 0x84) + item->x,
                *(s16*)(window + 0x86) + item->y + offset);
    spriteSetEnv();

    switch (*(s32*)(window + 0x04)) {
    case 0x40:
    case 0x50:
    case 0x51:
        msgWinDrawBackground(0, 2, width, height, window[0x8B], 1);
        break;
    case 0x10C:
        msgWinDrawBackground(0, 2, width, height, window[0x8B], 0);
        break;
    }

    switch ((s8)window[1]) {
    case 2:
        if (*(s32*)(window + 0x04) != 0x51) {
            winMsgDrawMessage(*(void**)work, *(u32*)(work + 0x08),
                        ((s8)window[0] & 4) == 0);
            *(u32*)(work + 0x08) = 0;
        }
        break;
    case 0:
    case 1:
    case 3:
    case 5:
        break;
    }
    return 0;
}
