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
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fadeEffectHookFunction_Doku_Init(s32 slot) {
    extern s32 fadeEffectHookFunction_Doku(s32 slot, f32 x, f32 y, f32 z, f32 rot, f32 scale); /* renamed from fn_801C4A44 */

    fadeSetFunctionOnly((s32)fadeEffectHookFunction_Doku);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_trainer_Init - Grid get slot Y position (symbol
 * swap fix: this body was previously misnamed fadeEffectHookFunction_
 * fadein_Init; see file header note).
 * Address: 0x801C483C | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fadeEffectHookFunction_trainer_Init(s32 slot) {
    extern void fn_801C4CB8(void);

    fadeSetFunctionOnly((s32)fn_801C4CB8);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_fadein_Init - Grid get slot Z position (symbol
 * swap fix: this body was previously misnamed fadeEffectHookFunction_
 * trainer_Init; see file header note).
 * Address: 0x801C4864 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fadeEffectHookFunction_fadein_Init(s32 slot) {
    extern f32 fn_801C54FC(void);

    fadeSetFunctionOnly((s32)fn_801C54FC);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_fadeout_in_Init - Grid set slot X position.
 * Address: 0x801C488C | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fadeEffectHookFunction_fadeout_in_Init(s32 slot, f32 x) {
    extern void fn_801C5530(void);

    fadeSetFunctionOnly((s32)fn_801C5530);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_carde_Init - Grid set slot Y position.
 * Address: 0x801C48B4 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fadeEffectHookFunction_carde_Init(s32 slot, f32 y) {
    extern f32 fadeEffectHookFunction_carde(void); /* renamed from fn_801C4C98 */

    fadeSetFunctionOnly((s32)fadeEffectHookFunction_carde);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_boss_Init - Grid set slot Z position.
 * Address: 0x801C48DC | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fadeEffectHookFunction_boss_Init(s32 slot, f32 z) {
    extern void fn_801C55D8(void);

    fadeSetFunctionOnly((s32)fn_801C55D8);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_yoko_or_tate_or_ball_Init - Grid set slot full position.
 * Address: 0x801C4904 | Size: 0x70
 */
#pragma scheduling off
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
#pragma scheduling on

/**
 * fadeEffectHookFunction_ball_Init - Grid get slot rotation.
 * Address: 0x801C4974 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fadeEffectHookFunction_ball_Init(s32 slot) {
    extern f32 fn_801C5898(void);

    fadeSetFunctionOnly((s32)fn_801C5898);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_yoko_or_tate_Init - Grid set slot rotation.
 * Address: 0x801C499C | Size: 0x58
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
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
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_tate_Init - Grid get slot scale.
 * Address: 0x801C49F4 | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
f32 fadeEffectHookFunction_tate_Init(s32 slot) {
    extern f32 fn_801C5ED0(void);

    fadeSetFunctionOnly((s32)fn_801C5ED0);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_yoko_Init - Grid set slot scale.
 * Address: 0x801C4A1C | Size: 0x28
 */
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fadeEffectHookFunction_yoko_Init(s32 slot, f32 scale) {
    extern void fn_801C5F6C(void);

    fadeSetFunctionOnly((s32)fn_801C5F6C);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on

/**
 * fadeEffectHookFunction_Doku - Grid complex slot update (position +
 * rotation + scale) (renamed from fn_801C4A44; confirmed name -- naming
 * pass 2026-07-07).
 * Address: 0x801C4A44 | Size: 0x254
 */
s32 fadeEffectHookFunction_Doku(s32 slot, f32 x, f32 y, f32 z, f32 rot, f32 scale) {
    extern const u32 lbl_8047DFD8;
    extern const f32 lbl_8047DFDC;
    extern const f32 lbl_8047DFE0;
    extern const f32 lbl_8047DFE4;
    extern const f32 lbl_8047DFE8;
    extern const f32 lbl_8047DFEC;
    extern const f32 lbl_8047DFF0;
    extern const f32 lbl_8047DFF4;
    extern const f32 lbl_8047DFF8;
    extern void* fadeEffectDokuStop(void);
    extern void fn_800D9ED8(u32 enable);
    extern void fn_800D88DC(u32 mask);
    extern void fn_800D888C(u32 mask);
    extern void fn_800D9B58(f32 left, f32 top, f32 right, f32 bottom);
    extern void fn_800DA4C4(u32 arg0, u32 arg1, u32 arg2);
    extern void fn_800DA2BC(u32 arg0, u32 arg1, u32 arg2);
    extern void fn_800DA1E8(u32 arg0, u32 arg1, u32 arg2);
    extern void fn_800DA028(u32 arg0);
    extern void fn_800D6A00(u32 arg0);
    extern void fn_800D7820(void* ptr);
    extern void fn_800D67BC(u32 count);
    extern void fn_800D6680(f32 x, f32 y, f32 z);
    extern void fn_800D5CB8(u32 arg0, u8 r, u8 g, u8 b, u8 a);
    extern void fn_800D6728(void);
    GSvec start;
    GSvec end;
    GSvec interpolated;
    f32 fade = z / rot;
    f32 progress = x / y;
    f32 t;
    u32 color = lbl_8047DFD8;
    u8 intensity;
    s32 alpha;

    if (fade <= lbl_8047DFDC) {
        start.x = start.y = start.z = lbl_8047DFE0;
        end.x = end.y = end.z = lbl_8047DFE4;
        t = lbl_8047DFE8 * fade;
        if (t >= lbl_8047DFE4) {
            t = lbl_8047DFE4;
        }
    } else {
        start.x = start.y = start.z = lbl_8047DFE4;
        end.x = end.y = end.z = lbl_8047DFE0;
        t = lbl_8047DFEC * (fade - lbl_8047DFDC);
        if (t >= lbl_8047DFE4) {
            t = lbl_8047DFE4;
        }
    }

    GSlerpGetLinearInterpolationVector(&interpolated, &start, &end, t);
    intensity = (u8)(lbl_8047DFF0 * interpolated.x);
    alpha = (s32)((f32)intensity * (lbl_8047DFE4 - progress));

    fn_800D9ED8(1);
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800D9B58(lbl_8047DFE0, lbl_8047DFE0, lbl_8047DFF4, lbl_8047DFF8);
    fn_800DA4C4(1, 6, 1);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA028(0);
    fn_800D6A00(7);
    fn_800D7820(NULL);
    fn_800D67BC(2);
    fn_800D6680(lbl_8047DFE0, lbl_8047DFE0, lbl_8047DFE0);
    fn_800D5CB8(0, ((u8*)&color)[0], ((u8*)&color)[1], ((u8*)&color)[2], alpha);
    fn_800D6680(lbl_8047DFF4, lbl_8047DFF8, lbl_8047DFE0);
    fn_800D5CB8(0, ((u8*)&color)[0], ((u8*)&color)[1], ((u8*)&color)[2], alpha);
    fn_800D6728();
    fn_800D9ED8(0);

    if (fade >= lbl_8047DFE4) {
        fadeEffectDokuStop();
        return 0;
    }
    return slot;
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
