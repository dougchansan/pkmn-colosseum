/**
 * @file pda_exact_8004A7A8.c
 * @brief PDA mail screen sprite positions and caption pulses,
 *        0x8004A7A8 - 0x8004B278: pdaTrainerDataAnimItem offsets the mail widgets by the
 *        eased x/y offsets, fades the captions and pulses their alpha.
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

/* Widget layout table: 0x1C-byte records indexed by event id; +2 x, +4 y. */
extern u8 lbl_802EF0A8[];
/* Mail-screen state (PdaMailSceneState in menu_pda_mail.c): +0x30/+0x34
 * widget x/y offsets, +0x40 pulse phase. */
extern u8 lbl_803A6A60[];
extern f32 lbl_804788D8;
extern s32 spriteSetOrigin(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern f64 sin(f64 x);

/* Fade the caption with the widgets' horizontal offset, then pulse its alpha
 * and grey level with a sine wave; `k` staggers the phase per caption. */
static inline void pda_8004A7A8_Pulse(PdaSprite* sprite, f32 k)
{
    s32 fade;
    f32 pulse;

    if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
        fade = 0xff;
    } else {
        fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
    }
    sprite->alpha = 0xff - fade;
    pulse = sin(3.1415927f * (2.0f * *(f32*)(lbl_803A6A60 + 0x40)) + lbl_804788D8 * k);
    pulse += 1.0f;
    pulse *= 0.5f;
    pulse *= 256.0f;
    if (pulse > 255.0f) {
        pulse = 255.0f;
    }
    if (pulse < 0.0f) {
        pulse = 0.0f;
    }
    sprite->alpha = pulse;
    sprite->colorR = pulse;
    sprite->colorG = pulse;
    sprite->colorB = pulse;
}

/* Backdrop alpha ramp and the staggered caption pulses on the mail screen. */
void pdaTrainerDataAnimItem(void* work, PdaSprite* sprite)
{
    s32 fade;
    s16* pos;

    switch (sprite->eventId) {
    case 0x6e0:
    case 0x6e2:
    case 0x751:
    case 0x752:
    case 0x753:
    case 0x754:
    case 0x755:
    case 0x756:
    case 0x757:
    case 0x758:
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_50 = pos[1] + (s32)*(f32*)(lbl_803A6A60 + 0x30);
        spriteSetEnv(spriteSetOrigin(
            (s16)(*(s16*)((u8*)work + 0x84) + sprite->field_50),
            (s16)(*(s16*)((u8*)work + 0x86) + sprite->field_52)));
        break;
    case 0x432:
    case 0x433:
    case 0x434:
    case 0x435:
    case 0x436:
    case 0x6dd:
        pos = (s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c);
        sprite->field_52 = pos[2] + (s32)*(f32*)(lbl_803A6A60 + 0x34);
        break;
    case 0x437:
    case 0x438:
    case 0x439:
    case 0x43a:
    case 0x777:
        if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6ea:
        pda_8004A7A8_Pulse(sprite, 0.0f);
        break;
    case 0x6eb:
        pda_8004A7A8_Pulse(sprite, 1.0f);
        break;
    case 0x6ec:
        pda_8004A7A8_Pulse(sprite, 2.0f);
        break;
    case 0x6ed:
        pda_8004A7A8_Pulse(sprite, 3.0f);
        break;
    case 0x6ee:
        pda_8004A7A8_Pulse(sprite, 4.0f);
        break;
    case 0x6ef:
        pda_8004A7A8_Pulse(sprite, 0.0f);
        break;
    case 0x6f0:
        pda_8004A7A8_Pulse(sprite, 1.0f);
        break;
    case 0x6f1:
        pda_8004A7A8_Pulse(sprite, 2.0f);
        break;
    case 0x6f2:
        pda_8004A7A8_Pulse(sprite, 3.0f);
        break;
    case 0x6f3:
        pda_8004A7A8_Pulse(sprite, 4.0f);
        break;
    case 0x6e7:
    case 0x6e8:
    case 0x6e9:
    case 0x6f4:
    case 0x6f5:
    case 0x6f6:
        if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6e3:
    case 0x6e4:
    case 0x6e5:
    case 0x6e6:
        if (*(f32*)(lbl_803A6A60 + 0x30) > 255.0f) {
            fade = 0xff;
        } else {
            fade = (s32)*(f32*)(lbl_803A6A60 + 0x30);
        }
        sprite->alpha = 0xff - fade;
        break;
    case 0x6f7:
    case 0x6f8:
        sprite->flags &= ~2;
        break;
    default:
        break;
    }
}
