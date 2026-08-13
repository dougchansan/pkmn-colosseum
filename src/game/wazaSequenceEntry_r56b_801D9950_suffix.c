/* Score instrumentation only; not evidence of a retail TU boundary. */
#define fn_801D9950 fn_801D9950_stub
#include "src/game/wazaSequenceEntry_candidate_801D97F0.c"
#undef fn_801D9950

typedef struct WazaSequenceScaleCtx {
    u8 pad_00[0x2C];
    u16 field_2C;
    u16 field_2E;
} WazaSequenceScaleCtx;

void fn_801D9950(void* owner, f32* scale, s32 selector)
{
    extern void fn_800E013C(void* dst, void* src, f32 scale);
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
    f32 value;

    switch (selector) {
    case -2: value = lbl_8047E348; break;
    case -1: value = lbl_8047E354; break;
    case 1: value = lbl_8047E358; break;
    case 2: value = lbl_8047E35C; break;
    case 3: value = lbl_8047E360; break;
    default: value = lbl_8047E34C; break;
    }
    set__5GSvecFfff(scale, value, value, value);

    switch (ctx->field_2C) {
    case 0x8F:
        if (ctx->field_2E == 2) fn_800E013C(scale, scale, lbl_8047E364);
        break;
    case 0xB1:
        fn_800E013C(scale, scale, lbl_8047E348);
        break;
    case 0xC4:
        if (ctx->field_2E == 1) fn_800E013C(scale, scale, lbl_8047E368);
        else if (ctx->field_2E == 2) fn_800E013C(scale, scale, lbl_8047E348);
        break;
    case 0xFA:
        if (ctx->field_2E == 1) fn_800E013C(scale, scale, lbl_8047E36C);
        else if (ctx->field_2E == 2) fn_800E013C(scale, scale, lbl_8047E370);
        break;
    case 0x119:
        fn_800E013C(scale, scale, lbl_8047E374);
        break;
    case 0x133:
    case 0x13B:
        if (ctx->field_2E == 2) fn_800E013C(scale, scale, lbl_8047E374);
        break;
    case 0x143:
        if (ctx->field_2E == 2) fn_800E013C(scale, scale, lbl_8047E378);
        break;
    case 0x149:
    case 0x162:
        if (ctx->field_2E == 2) fn_800E013C(scale, scale, lbl_8047E37C);
        break;
    }
}
