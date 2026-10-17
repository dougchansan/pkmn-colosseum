/**
 * @file fade_effect.c
 * @brief Battle grid fade-effect hook function table (per-effect slot
 *        position/rotation/scale accessor callbacks) and the combined
 *        slot transform updater.
 *
 * Address range: 0x801C4814 - 0x801C4CB8 (13 functions).
 *
 * Split out of the former monolithic battle_grid.c CodeCandidate bucket
 * (0x801C0F20 - 0x801C4CB8, split pass 2026-07-07). This is a distinct
 * XD translation unit (game/pxdvs/app/fade/fade_effect.cpp).
 *
 * NOTE (symbol swap fix, naming pass 2026-07-07): the previously-applied
 * names fadeEffectHookFunction_fadein_Init (0x801C483C) and
 * fadeEffectHookFunction_trainer_Init (0x801C4864) were swapped: 0x801C483C
 * registers the 0x704 body (trainer-sized; matches XD's trainer body
 * 0xC38) while 0x801C4864 registers the 0x34 body (fadein-sized; matches
 * XD's fadein body 0x70), and XD's real order is trainer_Init THEN
 * fadein_Init. The two function bodies below have been relabeled
 * accordingly (bodies unchanged, only the two names traded places) so
 * this TU is monotonic against XD. External callers (game/data/
 * data_80375938.c's fight_encount_wipe_data table, include/game/battle/
 * battle.h, include/game/battle/battle_waza_types.h) reference these
 * functions purely by name and need no changes: they now correctly
 * resolve to the swapped addresses.
 */

#include "dolphin/types.h"
#include "game/battle/battle_grid_types.h"
#include "game/gs_render.h"

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

extern void fadeSetFunctionOnly(s32 arg0); /* game/effect/fade.c, renamed from fn_801C431C */

/**
 * fadeEffectHookFunction_Doku_Init - Grid get slot X position (renamed
 * from fn_801C4814; confirmed name -- naming pass 2026-07-07). Registers
 * fadeEffectHookFunction_Doku (fn_801C4A44); fadeEffectDokuStart/Stop
 * exist in game/effect/fade.c.
 * Address: 0x801C4814 | Size: 0x28
 */
f32 fadeEffectHookFunction_Doku_Init(s32 slot) {
    extern s32 fadeEffectHookFunction_Doku(s32 slot, f32 x, f32 y, f32 z, f32 rot, f32 scale); /* renamed from fn_801C4A44 */

    fadeSetFunctionOnly((s32)fadeEffectHookFunction_Doku);
}

/**
 * fadeEffectHookFunction_trainer_Init - Grid get slot Y position (symbol
 * swap fix: this body was previously misnamed fadeEffectHookFunction_
 * fadein_Init; see file header note).
 * Address: 0x801C483C | Size: 0x28
 */
f32 fadeEffectHookFunction_trainer_Init(s32 slot) {
    extern void fn_801C4CB8(void);

    fadeSetFunctionOnly((s32)fn_801C4CB8);
}

/**
 * fadeEffectHookFunction_fadein_Init - Grid get slot Z position (symbol
 * swap fix: this body was previously misnamed fadeEffectHookFunction_
 * trainer_Init; see file header note).
 * Address: 0x801C4864 | Size: 0x28
 */
f32 fadeEffectHookFunction_fadein_Init(s32 slot) {
    extern f32 fn_801C54FC(void);

    fadeSetFunctionOnly((s32)fn_801C54FC);
}

/**
 * fadeEffectHookFunction_fadeout_in_Init - Grid set slot X position.
 * Address: 0x801C488C | Size: 0x28
 */
void fadeEffectHookFunction_fadeout_in_Init(s32 slot, f32 x) {
    extern void fn_801C5530(void);

    fadeSetFunctionOnly((s32)fn_801C5530);
}

/**
 * fadeEffectHookFunction_carde_Init - Grid set slot Y position.
 * Address: 0x801C48B4 | Size: 0x28
 */
void fadeEffectHookFunction_carde_Init(s32 slot, f32 y) {
    extern f32 fadeEffectHookFunction_carde(void); /* renamed from fn_801C4C98 */

    fadeSetFunctionOnly((s32)fadeEffectHookFunction_carde);
}

/**
 * fadeEffectHookFunction_boss_Init - Grid set slot Z position.
 * Address: 0x801C48DC | Size: 0x28
 */
void fadeEffectHookFunction_boss_Init(s32 slot, f32 z) {
    extern void fn_801C55D8(void);

    fadeSetFunctionOnly((s32)fn_801C55D8);
}

/**
 * fadeEffectHookFunction_yoko_or_tate_or_ball_Init - Grid set slot full position.
 * Address: 0x801C4904 | Size: 0x70
 */
void fadeEffectHookFunction_yoko_or_tate_or_ball_Init(s32 slot, f32 x, f32 y, f32 z) {
    extern s32 fn_801C6908(s32);
    extern void fn_801C5F6C(void);
    extern void fn_801C5ED0(void);
    extern void fn_801C5898(void);
    s32 result = fn_801C6908(3);

    switch (result) {
    case 0:
        fadeSetFunctionOnly((s32)fn_801C5F6C);
        break;
    case 1:
        fadeSetFunctionOnly((s32)fn_801C5ED0);
        break;
    case 2:
    default:
        fadeSetFunctionOnly((s32)fn_801C5898);
        break;
    }
}

/**
 * fadeEffectHookFunction_ball_Init - Grid get slot rotation.
 * Address: 0x801C4974 | Size: 0x28
 */
f32 fadeEffectHookFunction_ball_Init(s32 slot) {
    extern f32 fn_801C5898(void);

    fadeSetFunctionOnly((s32)fn_801C5898);
}

/**
 * fadeEffectHookFunction_yoko_or_tate_Init - Grid set slot rotation.
 * Address: 0x801C499C | Size: 0x58
 */
void fadeEffectHookFunction_yoko_or_tate_Init(s32 slot, f32 rotation) {
    extern s32 fn_801C6908(s32);
    extern void fn_801C5F6C(void);
    extern void fn_801C5ED0(void);
    s32 result = fn_801C6908(2);
    switch (result) {
    case 0:
        fadeSetFunctionOnly((s32)fn_801C5F6C);
        break;
    case 1:
    default:
        fadeSetFunctionOnly((s32)fn_801C5ED0);
        break;
    }
}

/**
 * fadeEffectHookFunction_tate_Init - Grid get slot scale.
 * Address: 0x801C49F4 | Size: 0x28
 */
f32 fadeEffectHookFunction_tate_Init(s32 slot) {
    extern f32 fn_801C5ED0(void);

    fadeSetFunctionOnly((s32)fn_801C5ED0);
}

/**
 * fadeEffectHookFunction_yoko_Init - Grid set slot scale.
 * Address: 0x801C4A1C | Size: 0x28
 */
void fadeEffectHookFunction_yoko_Init(s32 slot, f32 scale) {
    extern void fn_801C5F6C(void);

    fadeSetFunctionOnly((s32)fn_801C5F6C);
}

/*
 * fadeEffectHookFunction_Doku (0x801C4A44 | 0x254): the poison ("doku")
 * screen flash, a FadeHook (see fade.c). A purple full-screen quad pulses
 * up over the first half of the hook and back down over the second half,
 * scaled by how far the fade has run; the hook stops the effect when it
 * completes. Instruction-exact at GC/1.3 -O4,p -opt nopeephole with natural
 * literals; it owns the head of the TU's literal pool (0x8047DFD8 -
 * 0x8047E008), so it links only with the whole fade TU
 * (docs/recon/fade_tu_d11.md).
 */
typedef struct GStexture GStexture;

typedef struct FadeColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} FadeColor;

extern void fn_800D9ED8(u32 enable);
extern void gfxSetFlags(u32 mask);
extern void gfxClearFlags(u32 mask);
extern void gfxSetOrtho(f32 left, f32 top, f32 right, f32 bottom);
extern void gfxSetBlendMode(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetUpdates(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetZMode(u32 arg0, u32 arg1, u32 arg2);
extern void gfxSetCullMode(u32 arg0);
extern void gfxSetPrimitive(u32 arg0);
extern void gfxSetVtxDesc(void* ptr);
extern void gfxBegin(u32 arg0);
extern void gfxVtxPos(f32 x, f32 y, f32 z);
extern void gfxVtxColor4u8(u32 arg0, u8 r, u8 g, u8 b, u8 a);
extern void gfxEnd(void);
extern void fadeEffectDokuStop(void);

/* 0x801C4A44 | 0x254 */
u8 fadeEffectHookFunction_Doku(u8 pending, f32 elapsed, f32 duration,
                               f32 hookElapsed, f32 hookDuration,
                               GStexture* texture)
{
    FadeColor color = {0x1F, 0x08, 0x08, 0xFF};
    GSvec out;
    GSvec from;
    GSvec to;
    f32 t;
    f32 fade;
    f32 k;
    u8 level;
    u8 alpha;

    t = hookElapsed / hookDuration;
    fade = elapsed / duration;
    if (t <= 0.5f) {
        from.x = 0.0f;
        from.y = 0.0f;
        from.z = 0.0f;
        to.x = 1.0f;
        to.y = 1.0f;
        to.z = 1.0f;
        k = 4.0f * t;
        if (k >= 1.0f) {
            k = 1.0f;
        }
    } else {
        from.x = 1.0f;
        from.y = 1.0f;
        from.z = 1.0f;
        to.x = 0.0f;
        to.y = 0.0f;
        to.z = 0.0f;
        k = 2.0f * (t - 0.5f);
        if (k >= 1.0f) {
            k = 1.0f;
        }
    }
    GSlerpGetLinearInterpolationVector(&out, &from, &to, k);
    level = 255.0f * out.x;
    alpha = level * (1.0f - fade);

    fn_800D9ED8(1);
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    gfxSetBlendMode(1, 6, 1);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetCullMode(0);
    gfxSetPrimitive(7);
    gfxSetVtxDesc(NULL);
    gfxBegin(2);
    gfxVtxPos(0.0f, 0.0f, 0.0f);
    gfxVtxColor4u8(0, color.r, color.g, color.b, alpha);
    gfxVtxPos(640.0f, 480.0f, 0.0f);
    gfxVtxColor4u8(0, color.r, color.g, color.b, alpha);
    gfxEnd();
    fn_800D9ED8(0);

    if (t >= 1.0f) {
        fadeEffectDokuStop();
        return 0;
    }
    return pending;
}

/**
 * fadeEffectHookFunction_carde - Get grid rotation callback (renamed from
 * fn_801C4C98; confirmed name -- naming pass 2026-07-07).
 * Address: 0x801C4C98 | Size: 0x20
 */
f32 fadeEffectHookFunction_carde(void) {
    extern f32 fn_801C5F6C(void);
    return fn_801C5F6C();
}

/*
 * The rest of the fade effect TU (0x801C4CB8 - 0x801C6934: the camera,
 * trail and screen-quad effects) lives in fade_range_801C4CB8.c.
 */
#define FADE_EFFECT_TU
#include "src/game/effect/fade_range_801C4CB8.c"
