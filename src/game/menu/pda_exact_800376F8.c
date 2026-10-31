#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x50];
    s16 field_50;
    u8 pad52[0x15];
    u8 alpha;
} PdaSprite;

extern u8 lbl_803A654C[];
extern f32 lbl_8047BA58;
extern f32 lbl_8047A494;

/* Four identical fade steps over the PDA fade block (one per sprite slot);
 * written out per function (was a PDA_FADE_STEP macro). */
void pcMenuDrawItem(void* window, volatile PdaSprite* sprite)
{
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x58);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x58) +
               (velocity = *(f32*)(lbl_803A654C + 0x50));
        *(f32*)(lbl_803A654C + 0x58) = next;
        *(f32*)(lbl_803A654C + 0x50) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x4c) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x58) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x54);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x54) +
                                  *(f32*)(lbl_803A654C + 0x58));
    }
}

void pcMenuDrawItem2(void* window, volatile PdaSprite* sprite)
{
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x40);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x40) +
               (velocity = *(f32*)(lbl_803A654C + 0x38));
        *(f32*)(lbl_803A654C + 0x40) = next;
        *(f32*)(lbl_803A654C + 0x38) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x34) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x40) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x3c);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x3c) +
                                  *(f32*)(lbl_803A654C + 0x40));
    }
}

void pcMenuDrawItem3(void* window, volatile PdaSprite* sprite)
{
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x28);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x28) +
               (velocity = *(f32*)(lbl_803A654C + 0x20));
        *(f32*)(lbl_803A654C + 0x28) = next;
        *(f32*)(lbl_803A654C + 0x20) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x1c) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x28) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x24);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x24) +
                                  *(f32*)(lbl_803A654C + 0x28));
    }
}

void pcMenuDrawItem4(void* window, volatile PdaSprite* sprite)
{
    f32 value;
    f32 next;
    f32 velocity;
    s32 alpha;

    (void)window;
    value = *(f32*)(lbl_803A654C + 0x10);
    if (value < lbl_8047BA58) {
        alpha = (s32)value;
        if ((s16)alpha < -0xff) {
            alpha = -0xff;
        }
        value = 0.0f;
        sprite->alpha = (u8)(alpha + 0xff);
        next = *(f32*)(lbl_803A654C + 0x10) +
               (velocity = *(f32*)(lbl_803A654C + 0x08));
        *(f32*)(lbl_803A654C + 0x10) = next;
        *(f32*)(lbl_803A654C + 0x08) =
            lbl_8047A494 * *(f32*)(lbl_803A654C + 0x04) + velocity;
        if (next > value) {
            *(f32*)(lbl_803A654C + 0x10) = value;
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + 0x0c);
        }
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + 0x0c) +
                                  *(f32*)(lbl_803A654C + 0x10));
    }
}
