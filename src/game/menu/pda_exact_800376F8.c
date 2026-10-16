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

#define PDA_FADE_STEP(name, valueOffset, velocityOffset, accelOffset, baseOffset) \
void name(void* window, volatile PdaSprite* sprite)                       \
{                                                                         \
    f32 value;                                                            \
    f32 next;                                                             \
    f32 velocity;                                                         \
    s32 alpha;                                                            \
                                                                          \
    (void)window;                                                         \
    value = *(f32*)(lbl_803A654C + valueOffset);                          \
    if (value < lbl_8047BA58) {                                           \
        alpha = (s32)value;                                               \
        if ((s16)alpha < -0xff) {                                          \
            alpha = -0xff;                                                \
        }                                                                 \
        value = 0.0f;                                                     \
        sprite->alpha = (u8)(alpha + 0xff);                               \
        next = *(f32*)(lbl_803A654C + valueOffset) +                     \
               (velocity = *(f32*)(lbl_803A654C + velocityOffset));      \
        *(f32*)(lbl_803A654C + valueOffset) = next;                       \
        *(f32*)(lbl_803A654C + velocityOffset) =                          \
            lbl_8047A494 * *(f32*)(lbl_803A654C + accelOffset) + velocity; \
        if (next > value) {                                               \
            *(f32*)(lbl_803A654C + valueOffset) = value;                 \
            sprite->field_50 = (s16)*(f32*)(lbl_803A654C + baseOffset);  \
        }                                                                 \
        sprite->field_50 = (s16)(*(f32*)(lbl_803A654C + baseOffset) +     \
                                  *(f32*)(lbl_803A654C + valueOffset));  \
    }                                                                     \
}

PDA_FADE_STEP(fn_800376F8, 0x58, 0x50, 0x4c, 0x54)
PDA_FADE_STEP(fn_800377B4, 0x40, 0x38, 0x34, 0x3c)
PDA_FADE_STEP(fn_80037870, 0x28, 0x20, 0x1c, 0x24)
PDA_FADE_STEP(fn_8003792C, 0x10, 0x08, 0x04, 0x0c)

#undef PDA_FADE_STEP
