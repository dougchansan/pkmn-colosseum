/**
 * @file pda_exact_800499BC.c
 * @brief PDA mail screen widget callback, 0x800499BC - 0x8004A47C:
 *        fn_800499BC positions the mail widgets, sets the header tabs and
 *        captions and draws the rank icon and money digits.
 *        Copied from pda_range_80037158.c.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

typedef struct PdaPairS16 {
    s16 x;
    s16 y;
} PdaPairS16;

/* Widget layout table: 0x1C-byte records indexed by event id; +2 x, +4 y. */
extern u8 lbl_802EF0A8[];
/* Mail-screen state (PdaMailSceneState in menu_pda_mail.c): [0] selected
 * tab, [1] menu state, +0x18..+0x24 tab caption widths, +0x30 x offset,
 * +0x40 pulse phase. */
extern u8 lbl_803A6A60[];
extern PdaPairS16 lbl_8047BD70;
extern PdaPairS16 lbl_8047BD74;
extern PdaPairS16 lbl_8047BD98;
extern PdaPairS16 lbl_8047BD9C;
extern s32 lbl_804788F0;
extern u32 lbl_802E61D8[];

extern s32 spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern u32 GSmsgGetRect(s32 id);
extern u32 gamedataGetPlaytime(void);
extern void msgctrlSetValue(s32 id, u32 value);
extern void GSmsgPrintColor(s32 arg0, s32 arg1, s32 arg2, void* data);
extern void pdaTrainerDataSetCursor(s32 unused, PdaSprite* sprite, s32 messageId);
extern u8 std081_flagCheckTrue(s32 id);
extern u32 heroGetStatus(s32 a, s32 b, s32 c);
extern void menuSpriteBiosGetPtr(s32 id);
extern u32 menuGetCursorItemID(s32 menu);
extern void pdaTrainerDataAnimItem(void* work, PdaSprite* sprite);

/* Show a header tab and centre it on its measured width plus the tab's wave offset. */
static inline void pda_800499BC_PlaceTab(PdaSprite* sprite, s32 width, s16 wave)
{
    s16 left;

    sprite->flags |= 2;
    left = width - 0x36;
    left += wave;
    sprite->field_50 = left + 0xa8;
}

/* Per-widget update for the mail screen's sprites and captions. */
s32 pdaTrainerDataDraw(void* work, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    extern void windowDrawSprite2(s16 x, s16 y, s16 w, s16 h, u32 color,
                                  void* work, s32 id, s32 flag);
    PdaPairS16 wave[2];
    s32* rank;
    s32 idx;
    PdaPairS16 icon[2];
    s16* pos;
    s32 i;
    s32 id;
    u16 cursor;
    s32 msg;
    u8* row;
    u32 value;
    s32 n;
    s32 x;
    u8* layout;
    u32 digit;
    s16 left;
    u32 status;

    pdaTrainerDataAnimItem(work, sprite);
    switch (sprite->eventId) {
    case 0x6e1:
        icon[0] = lbl_8047BD70;
        icon[1] = lbl_8047BD74;
        pos = (s16*)(lbl_802EF0A8 + 0xc09e);
        status = heroGetStatus(0, 0xe, 0);
        for (i = lbl_804788F0 - 1; i >= 0; i--) {
            if (lbl_802E61D8[i] <= status) {
                break;
            }
        }
        if (i < 0) {
            i = 0;
        }
        rank = &idx;
        *rank = i;
        if (i > 4) {
            *rank = 0;
        }
        id = ((u16*)icon)[*rank];
        menuSpriteBiosGetPtr(id);
        windowDrawSprite2(
            (s16)((s32)*(f32*)(lbl_803A6A60 + 0x30) +
                  (pos[0] - sprite->field_50)),
            (s16)(pos[1] - sprite->field_52),
            sprite->x, sprite->y, sprite->alpha | -0x100, work, id, 0);
        break;
    case 0x435:
        row = *(u8**)((u8*)work + 0x60);
        sprite->messageId = **(s32**)(row + 4);
        sprite->colorR = row[0];
        sprite->colorG = row[1];
        sprite->colorB = row[2];
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 1:
            if (*(s8*)(lbl_803A6A60 + 1) == 2) {
                sprite->flags &= ~2;
            } else {
                sprite->flags |= 2;
            }
            break;
        }
        break;
    case 0x433:
    case 0x436:
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 1:
            if (*(s8*)(lbl_803A6A60 + 1) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
            break;
        default:
            sprite->flags &= ~2;
            break;
        }
        break;
    case 0x438:
        if (std081_flagCheckTrue(0x3f0) != 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        *(s32*)(lbl_803A6A60 + 0x1c) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x437:
        *(s32*)(lbl_803A6A60 + 0x18) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x43a:
        *(s32*)(lbl_803A6A60 + 0x24) = GSmsgGetRect(sprite->messageId) >> 16;
        cursor = menuGetCursorItemID(*(s32*)((u8*)work + 4));
        if (std081_flagCheckTrue(0x3f0) != 0) {
            switch (cursor) {
            case 0x437:
                lbl_803A6A60[0] = 0;
                break;
            case 0x438:
                lbl_803A6A60[0] = 1;
                break;
            case 0x439:
                lbl_803A6A60[0] = 2;
                break;
            case 0x43a:
                lbl_803A6A60[0] = 3;
                break;
            default:
                break;
            }
        } else {
            switch (cursor) {
            case 0x437:
                lbl_803A6A60[0] = 0;
                break;
            case 0x438:
                lbl_803A6A60[0] = 1;
                break;
            case 0x439:
                lbl_803A6A60[0] = 2;
                break;
            case 0x43a:
                lbl_803A6A60[0] = 3;
                break;
            default:
                break;
            }
        }
        break;
    case 0x439:
        *(s32*)(lbl_803A6A60 + 0x20) = GSmsgGetRect(sprite->messageId) >> 16;
        break;
    case 0x6e7:
    case 0x6e8:
    case 0x6e9:
    case 0x75d:
    case 0x75e:
    case 0x75f:
    case 0x760:
    case 0x761:
    case 0x762:
    case 0x763:
    case 0x764:
    case 0x765:
        pdaTrainerDataSetCursor((s32)work, sprite, sprite->eventId);
        break;
    case 0x6dd:
    case 0x6e2:
        break;
    case 0x754:
        /* RULE-EXCEPTION(user-approved): scalar reuse of digit for the play time in fn_800499BC - see docs/RULE_EXCEPTIONS.md */
        digit = gamedataGetPlaytime();
        msgctrlSetValue(0x4c, digit);
        break;
    case 0x755:
        msgctrlSetValue(0x50, heroGetStatus(0, 0xd, 0));
        break;
    case 0x756:
        msgctrlSetValue(0x50, heroGetStatus(0, 0xc, 0));
        break;
    case 0x757:
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_50 = pos[1] + (s32)*(f32*)(lbl_803A6A60 + 0x30);
        spriteSetEnv(spriteSetOrigin(
            (s16)(*(s16*)((u8*)work + 0x84) + sprite->field_50),
            (s16)(*(s16*)((u8*)work + 0x86) + sprite->field_52)));
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_50 = pos[1] + (s32)*(f32*)(lbl_803A6A60 + 0x30);
        value = heroGetStatus(0, 2, 0) & 0xffff;
        x = sprite->x - 0x20;
        for (n = 0; n < 5; n++) {
            x -= 0xd;
            digit = value % 10;
            value = value / 10;
            msgctrlSetValue(0x34, digit);
            GSmsgPrintColor(x + 0x24, 0, sprite->alpha | -0x100, (void*)0xca);
        }
        break;
    case 0x777:
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 0:
            msg = 0x36d6;
            break;
        case 1:
            msg = 0x36d7;
            break;
        case 2:
            msg = 0x36d8;
            break;
        case 3:
            msg = 0x36d9;
            break;
        default:
            msg = 1;
            break;
        }
        GSmsgPrintColor(-0x50, -4, sprite->alpha | -0x100, (void*)msg);
        break;
    case 0x6e3:
    case 0x6e4:
    case 0x6e5:
    case 0x6e6:
        wave[0] = lbl_8047BD98;
        wave[1] = lbl_8047BD9C;
        layout = lbl_802EF0A8 + sprite->eventId * 0x1c;
        switch (*(s8*)(lbl_803A6A60 + 0)) {
        case 0:
            if (sprite->eventId == 0x6e3) {
                sprite->field_50 = (
                    4.0f *
                        (f32)sin(3.1415927f *
                                 (2.0f *
                                  (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(layout + 2));
                wave[0].x = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[0].x = 0;
            }
            wave[0].x -= 0x1e;
            break;
        case 1:
            if (sprite->eventId == 0x6e4) {
                sprite->field_50 = (
                    4.0f *
                        (f32)sin(3.1415927f *
                                 (2.0f *
                                  (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(layout + 2));
                wave[0].y = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[0].y = 0;
            }
            wave[0].y -= 0x1e;
            break;
        case 2:
            if (sprite->eventId == 0x6e5) {
                wave[1].x = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[1].x = 0;
            }
            wave[1].x -= 0x1e;
            break;
        case 3:
            if (sprite->eventId == 0x6e6) {
                sprite->field_50 = (
                    4.0f *
                        (f32)sin(3.1415927f *
                                 (2.0f *
                                  (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))) +
                    (f32)*(s16*)(layout + 2));
                wave[1].y = (
                    4.0f *
                    (f32)sin(3.1415927f *
                             (2.0f *
                              (2.0f * *(f32*)(lbl_803A6A60 + 0x40)))));
            } else {
                sprite->field_50 = *(s16*)(layout + 2);
                wave[1].y = 0;
            }
            wave[1].y -= 0x1e;
            break;
        default:
            break;
        }
        if (sprite->eventId == 0x6e5) {
            sprite->flags |= 2;
            left = *(s32*)(lbl_803A6A60 + 0x20) - 0x36;
            left += wave[1].x;
            sprite->field_50 = left + 0xa8;
        }
        if (sprite->eventId == 0x6e3) {
            pda_800499BC_PlaceTab(sprite, *(s32*)(lbl_803A6A60 + 0x18), wave[0].x);
        }
        if (sprite->eventId == 0x6e4) {
            pda_800499BC_PlaceTab(sprite, *(s32*)(lbl_803A6A60 + 0x1c), wave[0].y);
        }
        if (sprite->eventId == 0x6e6) {
            pda_800499BC_PlaceTab(sprite, *(s32*)(lbl_803A6A60 + 0x24), wave[1].y);
        }
        if (sprite->eventId == 0x6e4) {
            if (std081_flagCheckTrue(0x3f0) != 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}
