#ifndef GAME_PS_TYPES_H
#define GAME_PS_TYPES_H

#include "dolphin/types.h"

typedef struct PSGeneratorState {
    /* 0x00 */ struct PSGeneratorState* next;
    /* 0x04 */ u32 flags;
    /* 0x08 */ u8 pad08[0x04];
    /* 0x0C */ f32 lifetime;
    /* 0x10 */ u16 maxLife;
    /* 0x12 */ u16 angleFlags;
    /* 0x14 */ u8 bankIndex;
    /* 0x15 */ u8 linkNo;
    /* 0x16 */ u8 texGroup;
    /* 0x17 */ u8 pad17;
    /* 0x18 */ u16 familyId;
    /* 0x1A */ u16 particleLife;
    /* 0x1C */ void* scriptData;
    /* 0x20 */ f32 positionX;
    /* 0x24 */ f32 positionY;
    /* 0x28 */ f32 positionZ;
    /* 0x2C */ f32 velocityX;
    /* 0x30 */ f32 velocityY;
    /* 0x34 */ f32 velocityZ;
    /* 0x38 */ f32 gravity;
    /* 0x3C */ f32 friction;
    /* 0x40 */ f32 particleSize;
    /* 0x44 */ f32 radius;
    /* 0x48 */ f32 angle;
    /* 0x4C */ u32 childCount;
    /* 0x50 */ void* appSRT;
    /* 0x54 */ union {
        struct {
            f32 minAngle;
            f32 maxAngle;
            f32 height;
        } cone;
        struct {
            f32 x;
            f32 y;
            f32 z;
            f32 xx;
            f32 xy;
            f32 xz;
            f32 yx;
            f32 yy;
            f32 yz;
            f32 zx;
            f32 zy;
            f32 zz;
        } rect;
        struct {
            f32 x;
            f32 y;
            f32 z;
        } line;
    } shape;
    /* 0x84 */ u16 shapeFlags;
    /* 0x86 */ u8 pad86[0x02];
    /* 0x88 */ u16 generatorFlags;
    /* 0x8A */ u8 pad8A[0x02];
    /* 0x8C */ f32 generatorData[6];
    /* 0xA4 */ void* linkedJObj;
    /* 0xA8 */ f32 angleRadiusScale[3];
} PSGeneratorState; /* size 0xB4 */

typedef struct PSAppSRT {
    /* 0x00 */ struct PSAppSRT* next;
    /* 0x04 */ void* owner;
    /* 0x08 */ f32 rotationX;
    /* 0x0C */ f32 rotationY;
    /* 0x10 */ f32 rotationZ;
    /* 0x14 */ f32 translationX;
    /* 0x18 */ f32 translationY;
    /* 0x1C */ f32 translationZ;
    /* 0x20 */ u8 pad20[0x04];
    /* 0x24 */ f32 scaleX;
    /* 0x28 */ f32 scaleY;
    /* 0x2C */ f32 scaleZ;
    /* 0x30 */ u8 type;
    /* 0x31 */ u8 flags;
    /* 0x32 */ u16 refCount;
    /* 0x34 */ f32 matrix[3][4];
    /* 0x64 */ u8 pad64[0x08];
    /* 0x6C */ void (*destroy)(struct PSAppSRT* appSRT);
    /* 0x70 */ u16 familyId;
    /* 0x72 */ u8 active;
} PSAppSRT; /* size 0x74 */

#endif /* GAME_PS_TYPES_H */
