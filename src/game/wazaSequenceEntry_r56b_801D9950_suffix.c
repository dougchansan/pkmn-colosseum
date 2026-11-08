#include "game/battle/battle_waza_types.h"

typedef struct WazaSequenceScaleCtx {
    u8 pad_00[0x2C];
    u16 field_2C;
    u16 field_2E;
} WazaSequenceScaleCtx;

void wazaSequenceGetScaleVec(void* owner, f32* scale, s32 selector)
{
    extern void vecScale(void* dst, void* src, f32 scale);
    extern const f32 lbl_8047E348;
    extern const f32 lbl_8047E34C;
    extern const f32 lbl_8047E354;
    extern const f32 lbl_8047E358;
    extern const f32 lbl_8047E35C;
    extern const f32 lbl_8047E360;
    extern const f32 lbl_8047E364;
    extern const f32 lbl_8047E368;
    extern const f32 lbl_8047E36C;
    extern const f32 lbl_8047E370;
    extern const f32 lbl_8047E374;
    extern const f32 lbl_8047E378;
    extern const f32 lbl_8047E37C;
    WazaSequenceScaleCtx* ctx = owner;
    u16 variant;

    switch (selector) {
    case -2:
        set__5GSvecFfff(scale, lbl_8047E348, lbl_8047E348, lbl_8047E348);
        break;
    case -1:
        set__5GSvecFfff(scale, lbl_8047E354, lbl_8047E354, lbl_8047E354);
        break;
    case 1:
        set__5GSvecFfff(scale, lbl_8047E358, lbl_8047E358, lbl_8047E358);
        break;
    case 2:
        set__5GSvecFfff(scale, lbl_8047E35C, lbl_8047E35C, lbl_8047E35C);
        break;
    case 3:
        set__5GSvecFfff(scale, lbl_8047E360, lbl_8047E360, lbl_8047E360);
        break;
    default:
        set__5GSvecFfff(scale, lbl_8047E34C, lbl_8047E34C, lbl_8047E34C);
        break;
    }

    switch (ctx->field_2C) {
    case 0x8F:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E364);
        }
        return;
    case 0xB1:
        vecScale(scale, scale, lbl_8047E348);
        return;
    case 0xC4:
        variant = ctx->field_2E;
        if (variant == 1) {
            vecScale(scale, scale, lbl_8047E368);
            return;
        }
        if (variant == 2) {
            vecScale(scale, scale, lbl_8047E348);
            return;
        }
        break;
    case 0xFA:
        variant = ctx->field_2E;
        if (variant == 1) {
            vecScale(scale, scale, lbl_8047E36C);
            return;
        }
        if (variant == 2) {
            vecScale(scale, scale, lbl_8047E370);
            return;
        }
        break;
    case 0x119:
        vecScale(scale, scale, lbl_8047E374);
        return;
    case 0x133:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E374);
            return;
        }
        break;
    case 0x13B:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E374);
            return;
        }
        break;
    case 0x143:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E378);
            return;
        }
        break;
    case 0x149:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E37C);
            return;
        }
        break;
    case 0x162:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E37C);
        }
        break;
    }
}
