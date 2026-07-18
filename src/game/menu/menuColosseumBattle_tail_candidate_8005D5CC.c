/**
 * @file menuColosseumBattle_tail_candidate_8005D5CC.c
 * @brief menuColosseumBattle.cpp candidate range, 0x8005D5CC - 0x8005D6A8.
 */
#include "dolphin/types.h"

typedef struct MenuWindow {
    u8 pad_00[0x4C];
    s32 nextMenu;
    u8 pad_50[0x10];
    void* partyState;
    u8 pad_64[0x31];
    s8 cursor;
    u8 pad_96;
    s8 previousCursor;
    u8 accepted;
    u8 canceled;
} MenuWindow;

typedef struct MenuSprite {
    u8 pad_00[6];
    s16 tag;
} MenuSprite;

extern void winSpriteSetDisp(void*, u32);
extern const s32 lbl_80267AB0[18];

/* Address: 0x8005D5CC | Size: 0xDC */
#pragma push
#pragma peephole off
#pragma optimize_for_size on
s32 fn_8005D5CC(MenuWindow* menu, MenuSprite* sprite) {
    typedef struct ColosseumSpriteTags {
        s32 values[18];
    } ColosseumSpriteTags;
    ColosseumSpriteTags tags;
    s32* p;
    s32 j;
    s32 i;
    s32 visible;

    tags = *(const ColosseumSpriteTags*)lbl_80267AB0;
    p = tags.values;
    for (i = 0; i < 6; i++) {
        j = 0;
        if (sprite->tag != p[0]) {
            j = 1;
            if (sprite->tag != p[1]) {
                j = 2;
                if (sprite->tag != p[2]) {
                    j = 3;
                }
            }
        }
        if (j < 3) break;
        p += 3;
    }
    if (i >= 6) {
        return 0;
    }
    if (menu->cursor == i) {
        visible = 1;
    } else {
        visible = 0;
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}
#pragma pop
