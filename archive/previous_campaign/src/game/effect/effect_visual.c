/**
 * @file effect_visual.c
 * @brief Visual effect sub-modules for Pokemon Colosseum.
 *
 * This file contains the individual visual effect implementations that
 * plug into the GSeffect system (gs_effect.c). Each effect registers
 * its own start/update/render/stop callbacks via GSEffectAllocSlot
 * and GSEffectRegister.
 *
 * Address range: 0x801380D4 - 0x801402AC (approximately 0x8200 bytes)
 *
 * Sub-modules identified by rodata debug strings:
 *
 *   lightningStartEffect   (fn_801380D4)  -- Lightning bolt effect
 *     "lightningStartEffect: Could not start lightning effect -
 *      invalid model: group %d, model %d."
 *     "lightningStartEffect: Could not start lightning effect -
 *      not enough memory!"
 *     "lightningStartEffect: Could not start lightning effect!"
 *     Struct size: 0x70 bytes (passed to GSEffectAllocSlot)
 *     Callbacks: start=fn_80138630, stop=fn_801386DC,
 *                update=fn_80138680, render=fn_801387C0
 *     Core logic: _lightningRenderMain (0x4E4 bytes -- main render with
 *       GX pipeline setup: fog, lighting, material color,
 *       position/rotation matrix, GX draw calls)
 *     Helper: fn_80138838 (0x2C8 bytes -- model transform)
 *
 *   leaffxStartEffect      (0x80138B00)  -- Leaf particle effect
 *     "leaffxStartEffect: Could not start leaf effect -
 *      invalid leaf model: groupRes %d, modelRes %d!"
 *     "leaffxStartEffect: Could not start leaf effect!"
 *     "_leaffxGenerateLeafData: Could not create leaf clone model."
 *     Struct size: 0x98 bytes
 *     Callbacks: start=fn_80138CCC, stop=fn_80139074,
 *                update=fn_80138BBC, render=_leaffxGenerateLeafData
 *     Core logic: fn_80138DE4 (0x290 bytes), fn_80139074 (0x304 bytes)
 *
 *   electronStartEffect    (0x80139820)  -- Electrical arc effect
 *     "electronStartEffect: Could not start electron effect -
 *      invalid model: groupRes %d, modelRes %d!"
 *     "leaffxStartEffect: Could not start electron effect!"
 *     Struct size: 0x78 bytes
 *     Callbacks: start=fn_801398E0, stop=fn_8013A1D4,
 *                update=fn_80139934, render=fn_80139AC4
 *     Core logic: fn_80139D10 (0x170 bytes), fn_80139E80 (0x354 bytes)
 *
 *   filterStart             (fn_8013A42C)  -- Full-screen colour filter
 *     "filterStart: Could not start filter effect - invalid filter!"
 *     "filterStart: Could not start filter effect!"
 *     Struct size: 0x14 bytes
 *     Callbacks: start=fn_8013A520, stop=fn_8013AB60,
 *                update=fn_8013A49C, render=fn_8013AD9C
 *     Core logic: fn_8013A520 (0x56C bytes -- large switch on filter type)
 *
 *   surfEffectStart         (fn_8013AABC)  -- Water surface wave effect
 *     "surfEffectStart: Could not start wave effect!"
 *     Struct size: 0x28 bytes
 *
 *   seaEffectStart          (fn_8013B490)  -- Ocean/sea effect
 *     "seaEffectStart: Could not start sea effect!"
 *     Struct size: 0x2C bytes
 *     Callbacks: start=fn_8013B558, stop=fn_8013B85C,
 *                update=fn_8013B504, render=surfEffectStart
 *
 *   envMapEffectInit        (fn_8013C5A0)  -- Environment-mapped reflections
 *     "envMapEffectInit: Could not initialise env map effect!"
 *     "envMapEffectStart: Could not start env map effect!"
 *     Struct size: 0x48 bytes
 *     Callbacks: start=fn_8013C670, stop=fn_8013CBF0,
 *                update=fn_8013C614, render=seaEffectStart
 *     Core logic: fn_8013C074 (0x52C bytes), fn_8013CA48 (0x1A8 bytes),
 *                 fn_8013CE58 (0x250 bytes)
 *
 *   blurEffectStart         (fn_8013D6B8)  -- Motion blur post-process
 *     "blurEffectStart: Could not start blur effect!"
 *     Struct size: 0x24 bytes
 *     Callbacks: start=fn_8013D730, stop=fn_8013D984,
 *                update=fn_8013D7CC, render=envMapEffectInit
 *     Core logic: fn_8013D0A8 (0x55C bytes -- large GX pipeline setup)
 *
 *   auraEffectStart         (fn_8013DC18)  -- Aura glow (Shadow Pokemon)
 *     "auraEffectStart: Could not start aura effect!"
 *     Struct size: 0x20 bytes
 *     Callbacks: start=blurEffectStart, stop=fn_8013E258,
 *                update=fn_8013DC94, render=fn_8013DE6C
 *     Core logic: fn_8013DE6C (0x3EC bytes -- aura rendering)
 *
 *   distortionEffectStart   (fn_8013E4D4)  -- Screen distortion / heat-haze
 *     "distortionEffectStart: Could not start distortion effect!"
 *     "_distortionEffectUpdateMatrices: Could not project points on screen"
 *     Struct size: 0x50 bytes
 *     Uses rodata string "translate" for matrix node lookup
 *     Callbacks: start=fn_8013E5AC, stop=fn_8013E8A4,
 *                update=fn_8013E54C, render=auraEffectStart
 *     Core logic: fn_8013E6C4 (0x1E0 bytes), fn_8013EA44 (0x5BC bytes)
 *
 *   billboardEffectStart    (fn_8013F000)  -- Billboard sprite particles
 *     "billboardEffectStart: Could not start billboard effect!"
 *     Struct size: 0xB4 bytes
 *     Callbacks: start=fn_8013F114, stop=distortionEffectStart,
 *                update=fn_8013F078, render=fn_8013F410
 *     Core logic: fn_8013F80C (0x170 bytes), _distortionEffectUpdateMatrices (0x264 bytes),
 *                 fn_8013FF0C (0x22C bytes -- billboard transform setup)
 *
 *   patchiruTextureStart   (0x8013FBE0)  -- Patchiru (Spinda) texture effect
 *     "Failed to create Patchiru texture"
 *     Struct size: 0x40 bytes
 *     Uses custom texture generation for the Spinda spot pattern
 *     Callbacks: start=fn_8013FCC4, stop=billboardEffectStart,
 *                update=fn_8013FC58, render=fn_8013FD68
 *
 * External references:
 *   fn_80131428 (GSEffectAllocSlot)  -- allocate effect slot
 *   fn_80131200 (GSEffectRegister)   -- register callbacks
 *   fn_8013139C (GSEffectResetState) -- re-trigger effect
 *   fn_800F9318 (floor resource lookup)
 *   fn_800E3D98 (GSmem / resource helper)
 *   fn_800D2248, GScameraGetActiveCamera, GScameraGetPosition, GScameraGetPerspective -- matrix/vector ops
 *   fn_800DA4C4, fn_800DA2BC, fn_800DA1E8, fn_800DA028 -- GX TEV/material setup
 *   fn_800D88DC, fn_800D888C -- GX blend/alpha mode
 *   fn_800D7820 -- GX draw begin
 *   fn_800E05C0, fn_800E048C -- matrix position/rotation
 *   fn_800D7F14 -- GX load matrix
 *   fn_800D6A00 -- GX cull mode
 *   fn_800D67BC, fn_800D6680, fn_800D5CB8 -- GX position/color/texcoord
 *   fn_800B9404 -- fog setup
 *   fn_800B8DF4, fn_800B856C -- GX begin/end frame
 *   fn_800EF5A4 -- model release
 *   fn_801779EC -- camera matrix helper
 *
 * All 12 sub-modules follow the same pattern:
 *   1. xxxStart allocates a slot via fn_80131428 with a struct size
 *   2. Registers callbacks via fn_80131200
 *   3. Calls fn_8013139C to enter IDLE state
 *   4. The update callback handles per-frame logic
 *   5. The render callback sets up GX pipeline and draws geometry
 *   6. The stop callback frees resources
 */

#include "dolphin/types.h"
#include "game/effect/gs_effect.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);     /* OSReport */
extern void* memset(void* dst, int val, u32 size);

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GSbezierCalculateVector();
extern void* GSfilterCreate(void);
extern void GSmaterialSetFlags();
extern void sin();   /* MSL trig (renamed) ? referenced by asm incs */
extern void cos();   /* MSL trig (renamed) ? referenced by asm incs */

/* GSmem allocator */
extern u16   fn_800E3534(u32 size);
extern void* fn_800E27B0(u16 handle);

/* Effect system core */
extern u32   fn_80131428(void* callbacks, u16 dataSize);  /* GSEffectAllocSlot */
extern void  fn_80131200(u32 effectId,
                          GSEffectStartFunc startFunc,
                          GSEffectStopFunc  destroyFunc,
                          GSEffectStartFunc triggerFunc,
                          GSEffectStopFunc  stopFunc,
                          void* extraParam,
                          GSEffectUpdateFunc updateFunc,
                          GSEffectRenderFunc renderFunc);  /* GSEffectRegister */
extern void  fn_8013139C(u32 effectId, u32 param);        /* GSEffectResetState */

/* Floor resource system */
extern void* fn_800F9318(u32 group, u32 model);

/* Matrix / vector operations */
extern void  fn_800E3D98(void* dst, void* src);
extern void  fn_800D2248(void);
extern void* GScameraGetActiveCamera(void);
extern void  GScameraGetPosition(void* mtx, void* vec);
extern void  GScameraGetPerspective(void* mtx, void* rx, void* ry, void* rz, void* scale);
extern void  fn_800E0040(void* vecA, void* vecB);
extern void  fn_800E0168(void* dst, void* srcA, void* srcB);
extern void  fn_800E0060(void* dst, void* src);

/* GX rendering pipeline */
extern void  fn_800DA4C4(u32 a, u32 b, u32 c);
extern void  fn_800DA2BC(u32 a, u32 b, u32 c);
extern void  fn_800DA1E8(u32 a, u32 b, u32 c);
extern void  fn_800DA028(u32 a);
extern void  fn_800D88DC(u32 a);
extern void  fn_800D888C(u32 a);
extern void  fn_800D7820(void* model);
extern void  fn_800E05C0(void* dst, f32 x, f32 y, f32 z);
extern void  fn_800E048C(void* mtx, f32 x, f32 y, f32 z);
extern void  fn_800D7F14(void* mtx);
extern void  fn_800D6A00(u32 mode);
extern void  fn_800D67BC(u16 index);
extern void  fn_800D6680(f32 x, f32 y, f32 z);
extern void  fn_800D5CB8(u32 a, u8 r, u8 g, u8 b, u8 alpha);
extern void  fn_800B9404(u32 index, u32 param);
extern void  fn_800B8DF4(void);
extern void  fn_800B856C(void);
extern void  fn_800EF5A4(void* model);
extern void  fn_800D9B58(f32 a, f32 b, f32 c, f32 d);
extern void  fn_800D9ED8(u32 param);

/* Camera */
extern void* fn_801779EC(void);

/* DCFlush */
extern void  DCFlushRange(void* ptr, u32 size);

/* ===================================================================
 * STUBS -- Functions in this translation unit.
 *
 * These are stub declarations for the 148 functions in the range
 * 0x801380D4 - 0x801402AC.  Each corresponds to a function in the
 * auto-generated asm file auto_01_800055E0_text.s.
 *
 * Full decompilation of each is deferred; the asm files remain the
 * authoritative implementation.  The function addresses and sizes are
 * documented here for cross-reference.
 * =================================================================== */

/* ---- Lightning effect ---- */
/* fn_801380D4: lightningStartEffect -- allocate slot, register, reset */
/* _lightningRenderMain (0x8013814C, 0x4E4 bytes) -- GX pipeline + draw */
/* fn_80138630: _lightningStart callback */
/* fn_80138680: _lightningUpdate callback */
/* fn_801386DC: _lightningStop callback */
/* fn_801387C0: _lightningRender callback */
/* fn_80138838: _lightningTransformModel (0x2C8 bytes) */

/* ---- Leaf effect ---- */
/* leaffxStartEffect (0x80138B00) */
/* fn_80138B74: _leaffxHelper */
/* fn_80138BBC: _leaffxUpdate callback */
/* fn_80138CCC: _leaffxStart callback */
/* fn_80138DE4: _leaffxGenerateLeafData (0x290 bytes) */
/* fn_80139074: _leaffxStop callback (0x304 bytes) */
/* _leaffxGenerateLeafData: _leaffxRender callback (0x4A8 bytes) */

/* ---- Electron effect ---- */
/* electronStartEffect (0x80139820) */
/* fn_80139898: _electronHelper */
/* fn_801398E0: _electronStart callback */
/* fn_80139934: _electronUpdate callback (0x190 bytes) */
/* fn_80139AC4: _electronRender callback (0x24C bytes) */
/* fn_80139D10: _electronCalcArc (0x170 bytes) */
/* fn_80139E80: _electronRenderArc (0x354 bytes) */
/* fn_8013A1D4: _electronStop callback (0x258 bytes) */

/* ---- Filter effect ---- */
/* fn_8013A42C: filterAlloc */
/* fn_8013A49C: _filterUpdate callback */
/* fn_8013A520: _filterStart callback (0x56C bytes -- large switch) */
/* fn_8013AA8C: _filterHelper */
/* fn_8013AABC: surfEffectStart */
/* fn_8013AB34: _surfHelper */
/* fn_8013AB60: _filterStop callback (0x208 bytes) */
/* fn_8013AD68: _filterHelper2 */
/* fn_8013AD9C: _filterRender callback (0x298 bytes) */

/* ---- Surf / wave effect ---- */
/* fn_8013B034: _surfUpdate callback */
/* fn_8013B0A0: _surfRender callback */
/* filterStart: _surfCalcWave (0x110 bytes) */
/* fn_8013B268: _surfRenderWave (0x228 bytes) */

/* ---- Sea effect ---- */
/* fn_8013B490: seaEffectStart */
/* fn_8013B504: _seaUpdate callback */
/* fn_8013B558: _seaStart callback */
/* surfEffectStart: _seaRender callback (0x278 bytes) */
/* fn_8013B85C: _seaStop callback (0x23C bytes) */
/* fn_8013BA98: _seaCalcSurface (0x178 bytes) */
/* fn_8013BC10: _seaRenderSurface (0x1F4 bytes) */
/* fn_8013BE04: _seaRenderReflection (0x270 bytes) */

/* ---- EnvMap effect ---- */
/* fn_8013C074: _envMapRenderMain (0x52C bytes) */
/* fn_8013C5A0: envMapEffectInit */
/* fn_8013C614: _envMapUpdate callback */
/* fn_8013C670: _envMapStart callback */
/* seaEffectStart: _envMapRender callback (0x330 bytes) */
/* fn_8013CA48: _envMapCalcReflection (0x1A8 bytes) */
/* fn_8013CBF0: _envMapStop callback (0x268 bytes) */
/* fn_8013CE58: _envMapUpdateTexture (0x250 bytes) */

/* ---- Blur effect ---- */
/* fn_8013D0A8: _blurRenderMain (0x55C bytes -- large GX setup) */
/* fn_8013D604: _blurHelper */
/* fn_8013D6B8: blurEffectStart */
/* fn_8013D730: _blurStart callback */
/* fn_8013D7CC: _blurUpdate callback */
/* envMapEffectInit: _blurRender callback (0x104 bytes) */
/* envMapEffectStart: _blurCalcMotion */
/* fn_8013D984: _blurStop callback (0x1E0 bytes) */
/* fn_8013DB64: _blurCleanup */

/* ---- Aura effect (Shadow Pokemon) ---- */
/* fn_8013DC18: auraEffectStart -- allocate 0x20 bytes */
/* fn_8013DC94: _auraUpdate callback */
/* fn_8013DD10: _auraHelper */
/* fn_8013DD7C: _auraHelper2 */
/* blurEffectStart: _auraStart callback */
/* fn_8013DE6C: _auraRender callback (0x3EC bytes) */
/* fn_8013E258: _auraStop callback (0x218 bytes) */

/* ---- Distortion / heat-haze effect ---- */
/* fn_8013E470: _distortionHelper */
/* fn_8013E4D4: distortionEffectStart */
/* fn_8013E54C: _distortionUpdate callback */
/* fn_8013E5AC: _distortionStart callback */
/* auraEffectStart: _distortionRender callback */
/* fn_8013E6C4: _distortionCalcMatrices (0x1E0 bytes) */
/* fn_8013E8A4: _distortionStop callback (0x1A0 bytes) */
/* fn_8013EA44: _distortionEffectUpdateMatrices (0x5BC bytes) */

/* ---- Billboard sprite effect ---- */
/* fn_8013F000: billboardEffectStart -- allocate 0xB4 bytes */
/* fn_8013F078: _billboardUpdate callback */
/* fn_8013F114: _billboardStart callback (0x230 bytes) */
/* distortionEffectStart: _billboardStop callback */
/* fn_8013F410: _billboardRender callback (0x3FC bytes) */
/* fn_8013F80C: _billboardCalcTransform (0x170 bytes) */
/* _distortionEffectUpdateMatrices: _billboardRenderQuad (0x264 bytes) */

/* ---- Patchiru (Spinda) texture effect ---- */
/* patchiruTextureStart (0x8013FBE0) -- allocate 0x40 bytes */
/* fn_8013FC58: _patchiruUpdate callback */
/* fn_8013FCC4: _patchiruStart callback */
/* fn_8013FD68: _patchiruRender callback */
/* billboardEffectStart: _patchiruStop callback (0x13C bytes) */

/* ---- Billboard transform helpers ---- */
/* fn_8013FF0C: _billboardTransformSetup (0x22C bytes) */
/* fn_80140138: _billboardTransformHelper */
/* fn_80140190: _billboardTransformHelper2 (0x11C bytes) */
/* _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl: _billboardTransformMain (0x2DC bytes) */
extern void fn_800E008C(void);
extern void fn_800D6728(void);
extern void fn_800D85D4(u32 a, void* b);
extern void fn_800E0BA0(void);
extern void fn_800D59B8(u32 a, f32 b, f32 c);
extern void fn_800D7E5C(void);
extern u32 lbl_8047D148;
extern u32 lbl_8047D14C;
extern u32 lbl_8047D150;
extern u32 lbl_8047D158;
extern u32 lbl_8047D154;
/* Forward declarations for self-referencing asm blocks */
extern void fn_80138838(void* ptr, u32 b);
extern u32 fn_80138B74(void* ptr);
extern u32 fn_80138BBC(void* ptr);
extern u32 fn_80138CCC(void* ptr);
extern void fn_80138DE4(void);
extern void fn_80139074(void);
extern void _leaffxGenerateLeafData(void* ptr, void* entry);
extern u32 fn_80139898(void* ptr);
extern u32 fn_801398E0(void* ptr);
extern u32 fn_80139934(void* ptr);
extern BOOL fn_80139AC4(void* ptr, u32 tick);
extern u32 fn_80139D10(void* ptr);
extern void fn_80139E80(void* entry, void* parent, void* cameraPos, void* modelPos);
extern void fn_8013A1D4(void* arg0, void* arg1, void* arg2, u32 arg3, f32 arg4);
extern u32 fn_8013A49C(void* ptr);
extern void fn_8013A520(void);
extern u32 fn_8013AA8C(void* ptr, u16 delta);
extern u32 fn_8013AD68(void* ptr);
extern void fn_8013AD9C(void);
extern u32 fn_8013B034(void* ptr);
extern u32 fn_8013B0A0(void* ptr);
extern u16 filterStart(void* ptr);
extern void fn_8013B268(void* ptr, u8* color);
extern u32 fn_8013B504(void* ptr);
extern u32 fn_8013B558(void* ptr);
extern u16 surfEffectStart(void);
extern u32 fn_8013B85C(void* ptr, u32 delta);
extern void fn_8013BA98(void* ptr);
extern void fn_8013BC10(void* ptr, f32 t);
extern void fn_8013BE04(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale);
extern void fn_8013C074(void);
extern u32 fn_8013C614(void* ptr);
extern u32 fn_8013C670(void* ptr);
extern u16 seaEffectStart(void);
extern u32 fn_8013CA48(void* ptr, u32 delta);
extern void fn_8013CBF0(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale);
extern u32 fn_8013CE58(void* inner, void* ptr);
extern void fn_8013D0A8(void);
extern u32 fn_8013D730(void* ptr);
extern u32 fn_8013D7CC(void* ptr);
extern u32 envMapEffectInit(void* ptr);
extern u32 envMapEffectStart(void* ptr);
extern u32 fn_8013D984(void* ptr, u32 delta);
extern u32 fn_8013DC94(void* ptr);
extern u32 fn_8013DD10(void* ptr);
extern u32 fn_8013DD7C(void* ptr);
extern u32 blurEffectStart(void* ptr);
extern void fn_8013DE6C(void);
extern u32 fn_8013E258(void* model, void* unused, void* state);
extern u32 fn_8013E470(void* ptr, u32 delta);
extern u32 fn_8013E54C(void* ptr);
extern u32  fn_8013E5AC(u8* p);
extern u32 auraEffectStart(void* ptr);
extern u32 fn_8013E6C4(u8* ptr);
extern u32 fn_8013E8A4(void* ptr, u32 delta);
extern void fn_8013EA44();
extern u32 fn_8013F078(void* ptr);
extern u32 fn_8013F114(void* ptr);
extern u16 distortionEffectStart(void);
extern u32 fn_8013F410(void* ptr);
extern u32 fn_8013F80C(void* ptr, u32 delta);
extern void _distortionEffectUpdateMatrices(void);
extern u32 fn_8013FC58(void* ptr);
extern u32 fn_8013FCC4(void* ptr);
extern u32 fn_8013FD68(void* ptr);
extern u16 billboardEffectStart(void* ptr);
extern u32 fn_8013FF0C(void* ptr);

#if 1
asm void _lightningRenderMain(void) {
#include "src/game/effect/effect_visual__lightningRenderMain.inc"
}
#else
void _lightningRenderMain(void) { /* TODO */ }
#endif
#if 0
asm void fn_80138630(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138630.inc"
}
#else
u32 fn_80138630(void* ptr) {
    if (ptr) {
        fn_800B8DF4();
        fn_800B856C();
        if (*(void**)((u8*)ptr + 0x1c) != NULL) {
            fn_800EF5A4(*(void**)((u8*)ptr + 0x1c));
        }
        return 1;
    }
    return 0;
}
#endif
extern void fn_800E24B0(u16 a);
extern void fn_800E209C(u16 a);
#if 0
asm void fn_80138680(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138680.inc"
}
#else
u32 fn_80138680(void* ptr) {
    u32 val;
    u32 ret;
    if (ptr) {
        val = *(u16*)ptr;
        fn_800B8DF4();
        fn_800B856C();
        if (val) {
            fn_800E24B0(val);
            fn_800E209C(val);
        }
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
#endif
extern u8 lbl_80272B40[];
#if 0
asm u32 fn_801386DC(void* ptr) {
#include "src/game/effect/effect_visual_fn_801386DC.inc"
}
#else
u32 fn_801386DC(void* ptr) {
    u8* p;
    u16 handle;

    if (ptr == NULL) {
        fn_800DD970((const char*)lbl_80272B40 + 0xB0);
        return 0;
    }

    p = ptr;
    if (fn_800F9318(*(u16*)(p + 0xA), *(u16*)(p + 0xC)) == NULL) {
        fn_800DD970((const char*)lbl_80272B40, *(u16*)(p + 0xA), *(u16*)(p + 0xC));
        return 0;
    }

    handle = fn_800E3534(*(u16*)(p + 0x8) * 0x97C);
    *(u16*)p = handle;
    if (handle == 0) {
        fn_800DD970((const char*)lbl_80272B40 + 0x60);
        return 0;
    }

    *(void**)(p + 4) = fn_800E27B0(handle);
    memset(*(void**)(p + 4), 0, *(u16*)(p + 0x8) * 0x97C);
    fn_80138838(p, 1);
    *(u16*)(p + 0x10) = 0;
    return 1;
}
#endif
#if 0
asm void fn_801387C0(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_801387C0.inc"
}
#else
u32 fn_801387C0(void* ptr, u32 delta) {
    u8* p;
    u32 d;
    u16 limit;
    d = delta;
    if (ptr == NULL) {
        goto fail;
    }
    p = ptr;
    limit = *(u16*)(p + 0x12);
    if (limit != 0 && *(u16*)(p + 0x10) >= limit) {
        goto fail;
    }
    fn_80138838(p, 0);
    *(u16*)(p + 0x10) = *(u16*)(p + 0x10) + d;
    return 1;
fail:
    return 0;
}
#endif
extern void fn_800E0BE4(void);
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern void fn_800E01F4(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E01D0(void);
extern void fn_800DFFCC(void);
extern void fn_800E0718(void);
extern void fn_800DFEEC(void);
extern void fn_800E0518(void);
extern void fn_800DFF98(void);
extern void fn_800E013C(void* dst, void* src, f32 scale);
extern void fn_800E019C(void* dst, void* srcA, void* srcB);
extern u32 lbl_8047D154;
extern u32 lbl_8047D15C;
extern u32 lbl_8047D14C;
#if 1
asm void fn_80138838(void* ptr, u32 b) {
#include "src/game/effect/effect_visual_fn_80138838.inc"
}
#else
void fn_80138838(void* ptr, u32 b) { /* TODO */ }
#endif
#if 0
asm void leaffxStartEffect(void) {
#include "src/game/effect/effect_visual_leaffxStartEffect.inc"
}
#else
u32 leaffxStartEffect(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x4c);
    if (effectId) {
        fn_80131200(effectId,
            0,
            (GSEffectStopFunc)fn_80138B74,
            (GSEffectStartFunc)fn_80138CCC,
            (GSEffectStopFunc)fn_80138BBC,
            0,
            (GSEffectUpdateFunc)fn_80138DE4,
            0);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
extern void fn_800F9210(u32 a, u32 b);
#if 0
asm void fn_80138B74(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138B74.inc"
}
#else
u32 fn_80138B74(void* ptr) {
    if (ptr) {
        fn_800F9210(*(u16*)((u8*)ptr + 0x40), *(u16*)((u8*)ptr + 0x44));
        fn_800F9210(*(u16*)((u8*)ptr + 0x40), *(u16*)((u8*)ptr + 0x42));
    }
    return 1;
}
#endif
extern void fn_800E4014(void* a, u32 b);
extern void fn_800E4BF4(void* entry);
extern u32 fn_800EE0E8(void* entry);
extern void* fn_800EE150(void* model, u32 mode);
extern s32 fn_800EE758(void* outerP);
extern void* fn_800EE6B4(void* handle, u32 idx);
extern void fn_800DF21C(void* obj, f32 val);
extern void fn_800DF608(void* handle);
extern void fn_800EE828(void* handle);
extern u32 lbl_8047D160;
#if 1
asm u32 fn_80138BBC(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138BBC.inc"
}
#else
u32 fn_80138BBC(void* ptr) { /* TODO */ }
#endif
extern u8 lbl_80272C30[];
extern u8 lbl_80272C90[];
#if 1
asm u32 fn_80138CCC(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138CCC.inc"
}
#else
u32 fn_80138CCC(void* ptr) { /* TODO */ }
#endif
extern void GSbezierCalculateVector(void);
extern void fn_800E06B8(void);
extern void fn_800E040C(void);
extern void fn_800E02C4(void);
extern void fn_800E03B4(void);
extern void fn_800E4598(void);
extern u32 lbl_8047D170;
extern u32 lbl_8047D164;
extern u32 lbl_8047D178;
extern u32 lbl_8047D160;
extern u32 lbl_8047D168;
#if 1
asm void fn_80138DE4(void) {
#include "src/game/effect/effect_visual_fn_80138DE4.inc"
}
#else
void fn_80138DE4(void) { /* TODO */ }
#endif
extern void fn_800E076C(void);
extern u32 lbl_8047D160;
extern u32 lbl_8047D180;
extern u32 lbl_8047D184;
#if 1
asm void fn_80139074(void) {
#include "src/game/effect/effect_visual_fn_80139074.inc"
}
#else
void fn_80139074(void) { /* TODO */ }
#endif
extern void fn_800E4C98(void);
extern void fn_800EE3BC(void* obj, void* a, void* b, void* c);
extern u8 lbl_80272CC4[];
extern u32 lbl_8047D168;
extern u32 lbl_8047D180;
extern u32 lbl_8047D184;
extern u32 lbl_8047D188;
#if 1
asm void _leaffxGenerateLeafData(void* ptr, void* entry) {
#include "src/game/effect/effect_visual__leaffxGenerateLeafData.inc"
}
#else
void _leaffxGenerateLeafData(void* ptr, void* entry) { /* TODO */ }
#endif
#if 0
asm void electronStartEffect(void) {
#include "src/game/effect/effect_visual_electronStartEffect.inc"
}
#else
u32 electronStartEffect(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x60);
    if (effectId) {
        fn_80131200(effectId,
            0,
            (GSEffectStopFunc)fn_80139898,
            (GSEffectStartFunc)fn_80139D10,
            (GSEffectStopFunc)fn_801398E0,
            0,
            (GSEffectUpdateFunc)fn_80139AC4,
            (GSEffectRenderFunc)fn_80139934);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm void fn_80139898(void* ptr) {
#include "src/game/effect/effect_visual_fn_80139898.inc"
}
#else
u32 fn_80139898(void* ptr) {
    if (ptr) {
        fn_800B8DF4();
        fn_800B856C();
        if (*(void**)((u8*)ptr + 0x58) != NULL) {
            fn_800EF5A4(*(void**)((u8*)ptr + 0x58));
        }
    }
    return 1;
}
#endif
#if 0
asm void fn_801398E0(void* ptr) {
#include "src/game/effect/effect_visual_fn_801398E0.inc"
}
#else
u32 fn_801398E0(void* ptr) {
    u32 val;
    if (ptr) {
        val = *(u16*)((u8*)ptr + 0x4);
        if (val) {
            fn_800B8DF4();
            fn_800B856C();
            fn_800E24B0(val);
            fn_800E209C(val);
        }
    }
    return 1;
}
#endif
extern u32 fn_800EE7E0(void* obj);
extern u32 lbl_8047D190;
#if 1
u32 fn_80139934(void* ptr) {
    f32 cameraPos[3];
    f32 pos[3];
    u8* p;
    void* camera;
    u16 i;
    u16 count;
    u8* entry;
    void* model;
    void* part;

    if (ptr != NULL) {
        p = ptr;
        camera = GScameraGetActiveCamera();
        count = *(u16*)(p + 0x44);
        entry = *(void**)p;
        model = fn_800F9318(*(u16*)(p + 0x4c), *(u16*)(p + 0x4e));
        if (*(void**)(p + 0x58) == NULL) {
            return 0;
        }
        fn_800D2248();
        if (model != NULL) {
            fn_800E3D98(model, pos);
            if (*(s32*)(p + 0x50) > 0) {
                part = fn_800EE150(model, *(u32*)(p + 0x50));
                if (part != NULL) {
                    if ((u8)fn_800EE7E0(part) != 0) {
                        fn_800EE3BC(part, pos, NULL, NULL);
                        fn_800EE828(part);
                    }
                }
            }
        } else {
            fn_800E01F4(pos, *(f32*)&lbl_8047D190, *(f32*)&lbl_8047D190, *(f32*)&lbl_8047D190);
        }
        GScameraGetPosition(camera, cameraPos);
        fn_800DA4C4(1, 6, 1);
        fn_800DA2BC(1, 1, 0);
        fn_800DA1E8(1, 1, 1);
        fn_800DA028(0);
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D7820(*(void**)(p + 0x54));
        fn_800D85D4(0, *(void**)(p + 0x58));
        fn_800D6A00(6);
        i = 0;
        while (i < count) {
            if (*(u16*)(entry + 0x12e0) != 0) {
                fn_80139E80(entry, p, cameraPos, pos);
            }
            i++;
            entry += 0x12e4;
        }
        return 1;
    }
    return 0;
}
#else
u32 fn_80139934(void* ptr) { /* TODO */ }
#endif
extern u32 lbl_8047D198;
extern u32 lbl_8047D1A0;
#if 1
asm BOOL fn_80139AC4(void* ptr, u32 tick) {
#include "src/game/effect/effect_visual_fn_80139AC4.inc"
}
#else
BOOL fn_80139AC4(void* ptr, u32 tick) { /* TODO */ }
#endif
extern u8 lbl_80272D08[];
extern u32 lbl_8047D190;
extern u8 lbl_80272D54[];
#if 1
asm u32 fn_80139D10(void* ptr) {
#include "src/game/effect/effect_visual_fn_80139D10.inc"
}
#else
u32 fn_80139D10(void* ptr) { /* TODO */ }
#endif
extern void fn_800E0108(void);
extern void fn_800E0560(void);
extern void fn_800E042C(void);
extern void fn_800E00AC(void);
extern u32 lbl_8047D1A0;
extern u32 lbl_8047D1A8;
extern u32 lbl_8047D190;
extern u32 lbl_8047D1AC;
extern u32 lbl_8047D1B0;
#if 1
asm void fn_80139E80(void* entry, void* parent, void* cameraPos, void* modelPos) {
#include "src/game/effect/effect_visual_fn_80139E80.inc"
}
#else
void fn_80139E80(void* entry, void* parent, void* cameraPos, void* modelPos) { /* TODO */ }
#endif
extern u32 lbl_8047D1B4;
extern u32 lbl_8047D190;
extern u32 lbl_8047D1A0;
extern u32 lbl_8047D1A8;
extern u32 lbl_8047D1AC;
#if 1
asm void fn_8013A1D4(void* arg0, void* arg1, void* arg2, u32 arg3, f32 arg4) {
#include "src/game/effect/effect_visual_fn_8013A1D4.inc"
}
#else
void fn_8013A1D4(void* arg0, void* arg1, void* arg2, u32 arg3, f32 arg4) { /* TODO */ }
#endif
#if 0
asm void fn_8013A42C(void) {
#include "src/game/effect/effect_visual_fn_8013A42C.inc"
}
#else
u32 fn_8013A42C(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x24);
    if (effectId) {
        fn_80131200(effectId,
            0,
            0,
            (GSEffectStartFunc)fn_8013A49C,
            0,
            0,
            (GSEffectUpdateFunc)fn_8013AA8C,
            (GSEffectRenderFunc)fn_8013A520);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
extern u8 lbl_80272D90[];
extern u8 lbl_80272DF4[];
#if 0
asm u32 fn_8013A49C(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013A49C.inc"
}
#else
u32 fn_8013A49C(void* ptr) {
    void* res;
    if (ptr) {
        res = fn_800F9318(*(u16*)((u8*)ptr + 0xc), *(u16*)((u8*)ptr + 0xe));
        if (res == NULL) {
            fn_800DD970((const char*)lbl_80272D90, *(u16*)((u8*)ptr + 0xc), *(u16*)((u8*)ptr + 0xe));
            return 0;
        }
        *(u16*)((u8*)ptr + 0x8) = 0;
        return 1;
    }
    fn_800DD970((const char*)lbl_80272DF4);
    return 0;
}
#endif
extern void fn_800C46B0(void);
extern u32 lbl_8047D1B8;
extern u32 lbl_8047D1BC;
extern u32 lbl_8047D1C0;
extern u32 lbl_8047D1C4;
extern u32 lbl_8047D1C8;
extern u32 lbl_8047D1CC;
extern u32 lbl_8047D1D0;
extern u32 lbl_8047D1D8;
#if 1
asm void fn_8013A520(void) {
#include "src/game/effect/effect_visual_fn_8013A520.inc"
}
#else
void fn_8013A520(void) { /* TODO */ }
#endif
#if 0
asm u32 fn_8013AA8C(void* ptr, u16 delta) {
#include "src/game/effect/effect_visual_fn_8013AA8C.inc"
}
#else
u32 fn_8013AA8C(void* ptr, u16 delta) {
    u16 cur;
    u16 max;
    if (ptr) {
        cur = *(u16*)((u8*)ptr + 0x8);
        max = *(u16*)((u8*)ptr + 0xa);
        if (cur < max) {
            *(u16*)((u8*)ptr + 0x8) = cur + delta;
            return 1;
        }
    }
    return 0;
}
#endif
#if 0
asm void fn_8013AABC(void) {
#include "src/game/effect/effect_visual_fn_8013AABC.inc"
}
#else
u32 fn_8013AABC(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x5c);
    if (effectId) {
        fn_80131200(effectId,
            0,
            (GSEffectStopFunc)fn_8013B034,
            (GSEffectStartFunc)filterStart,
            (GSEffectStopFunc)fn_8013B0A0,
            (void*)fn_8013AD68,
            (GSEffectUpdateFunc)fn_8013AD9C,
            0);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm u32 fn_8013AB34(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013AB34.inc"
}
#else
u32 fn_8013AB34(void* ptr) {
    void* inner;
    u32 val;
    inner = *(void**)((u8*)ptr + 0x54);
    if (inner) {
        val = *(u32*)((u8*)inner + 0x8);
        if (val == 0xFFFFFFFF) {
            return 1;
        }
    }
    return 0;
}
#endif
extern u32 lbl_8047D1E8;
extern u32 lbl_8047D1E0;
#if 1
asm u32 fn_8013AB60(void* ptr, u8* src, u8* dst, u32 alpha) {
#include "src/game/effect/effect_visual_fn_8013AB60.inc"
}
#else
u32 fn_8013AB60(void* ptr, u8* src, u8* dst, u32 alpha) { /* TODO */ }
#endif
#if 0
asm u32 fn_8013AD68(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013AD68.inc"
}
#else
u32 fn_8013AD68(void* ptr) {
    void* ptr2;
    if (ptr) {
        ptr2 = *(void**)((u8*)ptr + 0x54);
        if (ptr2) {
            *(void**)((u8*)ptr + 0x54) = *(void**)((u8*)ptr2 + 0x10);
            *(u32*)((u8*)ptr + 0x58) = 0;
            return 1;
        }
    }
    return 0;
}
#endif
extern void fn_80168408(void);
extern u32 lbl_8047D1E8;
extern u32 lbl_8047D1F0;
extern u32 lbl_8047D1F4;
#if 1
asm void fn_8013AD9C(void) {
#include "src/game/effect/effect_visual_fn_8013AD9C.inc"
}
#else
void fn_8013AD9C(void) { /* TODO */ }
#endif
#if 0
asm void fn_8013B034(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013B034.inc"
}
#else
u32 fn_8013B034(void* ptr) {
    void* next;
    u32 val;
    u32 ret;
    if (ptr) {
        ptr = *(void**)((u8*)ptr + 0x50);
        while (ptr != NULL) {
            val = *(u16*)((u8*)ptr + 0xc);
            next = *(void**)((u8*)ptr + 0x10);
            fn_800E24B0(val);
            fn_800E209C(val);
            ptr = next;
        }
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
#endif
extern void fn_800E5A74(void* texture);
extern void fn_800E638C(void* texture);
extern void fn_801684F0(void* texture);
#if 0
asm void fn_8013B0A0(void) {
#include "src/game/effect/effect_visual_fn_8013B0A0.inc"
}
#else
u32 fn_8013B0A0(void* ptr) {
    u8 flag2;
    u8 flag1;
    u16 i;
    u32 count;
    u8* p;

    if (ptr == NULL) {
        goto fail;
    }

    if (*(u8*)((u8*)ptr + 0x4c) != 0) {
        p = (u8*)ptr + 4;
        count = *(u32*)((u8*)ptr + 0x48) & 0xffff;
        flag1 = *(u8*)((u8*)ptr + 0x4e);
        flag2 = *(u8*)((u8*)ptr + 0x4f);
        fn_800B8DF4();
        fn_800B856C();
        for (i = 0; i < count; i++, p += 4) {
            if (*(void**)p != NULL) {
                if (flag2 == 0) {
                    fn_800E5A74(*(void**)p);
                }
                if (flag1 != 0) {
                    fn_800E638C(*(void**)p);
                }
            }
        }
    } else {
        void* tex = *(void**)ptr;
        if (tex != NULL) {
            fn_801684F0(tex);
        }
    }
    return 1;

fail:
    return 0;
}
#endif
extern void fn_800E6478(void* texture, void* name);
extern void fn_800E5BE0(void* texture, void* filter);
extern void fn_80168570(void);
extern u8 lbl_80363CA8[];
extern u8 lbl_80272E30[];
extern u8 lbl_80272E70[];
#if 0
asm u16 filterStart(void) {
#include "src/game/effect/effect_visual_filterStart.inc"
}
#else
u16 filterStart(void* arg) {
    u8 flag2;
    u8 flag1;
    u16 i;
    u32 count;
    u8* p;
    void* ptr = arg;

    if (ptr == NULL) {
        goto report_null;
    }

    if (*(void**)((u8*)arg + 0x50) == NULL) {
        return 0;
    }

    if (*(u8*)((u8*)ptr + 0x4c) != 0) {
        p = (u8*)ptr + 4;
        count = *(u32*)((u8*)ptr + 0x48) & 0xffff;
        flag1 = *(u8*)((u8*)ptr + 0x4e);
        flag2 = *(u8*)((u8*)ptr + 0x4f);
        for (i = 0; i < count; i++, p += 4) {
            if (*(void**)p != NULL) {
                if (flag1 != 0) {
                    fn_800E6478(*(void**)p, lbl_80363CA8);
                }
                if (flag2 == 0) {
                    fn_800E5BE0(*(void**)p, *(void**)((u8*)ptr + 0x50));
                }
            }
        }
    } else {
        *(void**)ptr = GSfilterCreate();
        if (*(void**)ptr == NULL) {
            fn_800DD970((const char*)lbl_80272E30);
            return 0;
        }
    }

    *(void**)((u8*)ptr + 0x54) = *(void**)((u8*)ptr + 0x50);
    *(u32*)((u8*)ptr + 0x58) = 0;
    return 1;

report_null:
    fn_800DD970((const char*)lbl_80272E70);
    return 0;
}
#endif
extern void fn_800E5B68(void* obj, void* param);
extern u32 lbl_8047D1E8;
extern u32 lbl_8047D1F8;
extern u32 lbl_8047D1F0;
extern u32 lbl_8047D1FC;
#if 1
asm void fn_8013B268(void* ptr, u8* color) {
#include "src/game/effect/effect_visual_fn_8013B268.inc"
}
#else
void fn_8013B268(void* ptr, u8* color) { /* TODO */ }
#endif
#if 0
asm void fn_8013B490(void) {
#include "src/game/effect/effect_visual_fn_8013B490.inc"
}
#else
u32 fn_8013B490(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0xd0);
    if (effectId) {
        fn_80131200(effectId,
            0,
            (GSEffectStopFunc)fn_8013B504,
            (GSEffectStartFunc)surfEffectStart,
            (GSEffectStopFunc)fn_8013B558,
            0,
            (GSEffectUpdateFunc)fn_8013B85C,
            0);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm u32 fn_8013B504(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013B504.inc"
}
#else
u32 fn_8013B504(void* ptr) {
    if (ptr) {
        if (*(s32*)((u8*)ptr + 0x58) != 0) {
            fn_800F9210(*(u32*)((u8*)ptr + 0x50), *(u32*)((u8*)ptr + 0x54));
            fn_800F9210(*(u32*)((u8*)ptr + 0x50), *(u32*)((u8*)ptr + 0x58));
        }
    }
    return 1;
}
#endif
#if 0
asm u32 fn_8013B558(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013B558.inc"
}
#else
u32 fn_8013B558(void* ptr) {
    u16 val;
    if (ptr) {
        if (*(void**)ptr != NULL) {
            fn_800E4014(*(void**)ptr, 0);
        }
        fn_800B8DF4();
        fn_800B856C();
        val = *(u16*)((u8*)ptr + 0x20);
        if (val) {
            fn_800E24B0(val);
            fn_800E209C(val);
        }
        val = *(u16*)((u8*)ptr + 0xc0);
        if (val) {
            fn_800E24B0(val);
            fn_800E209C(val);
        }
    }
    return 1;
}
#endif
extern void fn_800E2C04(void);
extern void fn_800E0E14(void);
extern void fn_800E4170(void);
extern u8 fn_800EC1B0(void* model);
extern void fn_800EC35C(void* model, u16 value);
extern void fn_800EC308(void* model, f32 value);
extern void fn_800EC2A4(void* model, f32 value);
extern void fn_800EC208(void* model, u32 value);
extern void fn_800EC1E4(void* model);
extern u32 lbl_8047D200;
extern u32 lbl_8047D204;
extern u8 lbl_80272EA0[];
#if 1
asm u16 surfEffectStart(void) {
#include "src/game/effect/effect_visual_surfEffectStart.inc"
}
#else
u16 surfEffectStart(void) { /* TODO */ }
#endif
extern void fn_800E09E8(void* dst, void* src, u32 count);
extern u32 lbl_8047D208;
#if 1
asm u32 fn_8013B85C(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013B85C.inc"
}
#else
u32 fn_8013B85C(void* ptr, u32 delta) { /* TODO */ }
#endif
extern u8 lbl_8031554C[];
extern u8 lbl_80315540[];
extern u32 lbl_8047D200;
#if 1
asm void fn_8013BA98(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013BA98.inc"
}
#else
void fn_8013BA98(void* ptr) { /* TODO */ }
#endif
extern void fn_800E0204(void);
extern u32 lbl_8047D200;
extern u32 lbl_8047D210;
extern u32 lbl_8047D214;
extern u32 lbl_8047D218;
extern u32 lbl_8047D21C;
#if 1
asm void fn_8013BC10(void* ptr, f32 t) {
#include "src/game/effect/effect_visual_fn_8013BC10.inc"
}
#else
void fn_8013BC10(void* ptr, f32 t) { /* TODO */ }
#endif
extern u32 lbl_8047D220;
extern u32 lbl_8047D204;
extern u32 lbl_8047D228;
extern u32 lbl_8047D21C;
extern u32 lbl_8047D208;
#if 1
asm void fn_8013BE04(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) {
#include "src/game/effect/effect_visual_fn_8013BE04.inc"
}
#else
void fn_8013BE04(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) { /* TODO */ }
#endif
extern void fn_8019FF48(void);
extern void HSD_MObjSetFlags(void);
extern void fn_801A6FF0(void);
#if 1
asm void fn_8013C074(void) {
#include "src/game/effect/effect_visual_fn_8013C074.inc"
}
#else
void fn_8013C074(void) { /* TODO */ }
#endif
#if 0
asm void fn_8013C5A0(void) {
#include "src/game/effect/effect_visual_fn_8013C5A0.inc"
}
#else
u32 fn_8013C5A0(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0xac);
    if (effectId) {
        fn_80131200(effectId,
            0,
            (GSEffectStopFunc)fn_8013C614,
            (GSEffectStartFunc)seaEffectStart,
            (GSEffectStopFunc)fn_8013C670,
            0,
            (GSEffectUpdateFunc)fn_8013CA48,
            0);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm u32 fn_8013C614(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013C614.inc"
}
#else
u32 fn_8013C614(void* ptr) {
    if (ptr) {
        fn_800B8DF4();
        fn_800B856C();
        if (*(s32*)((u8*)ptr + 0x7c) != 0) {
            fn_800F9210(*(u32*)((u8*)ptr + 0x74), *(u32*)((u8*)ptr + 0x78));
            fn_800F9210(*(u32*)((u8*)ptr + 0x74), *(u32*)((u8*)ptr + 0x7c));
        }
    }
    return 1;
}
#endif
extern void fn_800EC1D4(void* a);
#if 0
asm void fn_8013C670(void) {
#include "src/game/effect/effect_visual_fn_8013C670.inc"
}
#else
#pragma push
#pragma global_optimizer off
u32 fn_8013C670(void* arg) {
    void* ptr;
    void* inner;

    if (arg != 0) {
        ptr = arg;
        inner = *(void**)ptr;
        fn_800EC1D4(inner);
        if (inner != 0) {
            fn_800E4014(inner, 0);
        }
        fn_800B8DF4();
        fn_800B856C();
        fn_8013CE58(inner, ptr);

        {
            u16 val;
            val = *(u16*)((u8*)ptr + 0x1c);
            if (val != 0) {
                fn_800E24B0(val);
                fn_800E209C(val);
            }
        }

        ptr = (void*)(u32)*(u16*)((u8*)ptr + 0x8c);
        if (ptr != 0) {
            fn_800E24B0((u32)ptr);
            fn_800E209C((u32)ptr);
        }
    }
    return 1;
}
#pragma pop
#endif
extern void fn_800EC188(void);
extern u32 lbl_8047D230;
extern u32 lbl_8047D234;
extern u32 lbl_8047D238;
extern u32 lbl_8047D23C;
extern u8 lbl_80272ED0[];
#if 1
asm u16 seaEffectStart(void) {
#include "src/game/effect/effect_visual_seaEffectStart.inc"
}
#else
u16 seaEffectStart(void) { /* TODO */ }
#endif
extern void fn_800E0CA0(void);
extern u32 lbl_8047D248;
extern u32 lbl_8047D23C;
extern u8 lbl_80363CB8[];
extern u32 lbl_8047D240;
extern u32 lbl_8047D244;
extern u32 lbl_8047D250;
#if 1
asm u32 fn_8013CA48(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013CA48.inc"
}
#else
u32 fn_8013CA48(void* ptr, u32 delta) { /* TODO */ }
#endif
extern u32 lbl_8047D23C;
extern u32 lbl_8047D240;
extern u32 lbl_8047D258;
extern u32 lbl_8047D250;
extern u32 lbl_8047D248;
#if 1
asm void fn_8013CBF0(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) {
#include "src/game/effect/effect_visual_fn_8013CBF0.inc"
}
#else
void fn_8013CBF0(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) { /* TODO */ }
#endif
#if 1
asm u32 fn_8013CE58(void* inner, void* ptr) {
#include "src/game/effect/effect_visual_fn_8013CE58.inc"
}
#else
u32 fn_8013CE58(void* inner, void* ptr) { return 0; }
#endif
#if 1
asm void fn_8013D0A8(void) {
#include "src/game/effect/effect_visual_fn_8013D0A8.inc"
}
#else
void fn_8013D0A8(void) { /* TODO */ }
#endif
#if 0
asm void fn_8013D604(void* ptr, u32 val, f32 f1, f32 f2) {
#include "src/game/effect/effect_visual_fn_8013D604.inc"
}
#else
u32 fn_8013D604(void* ptr, u32 val, f32 f1, f32 f2) {
    u16 handle;
    void* node;
    void* cur;
    handle = fn_800E3534(0x14);
    if (handle) {
        node = fn_800E27B0(handle);
        *(u16*)((u8*)node + 0xc) = handle;
        *(f32*)((u8*)node + 0x0) = f1;
        *(f32*)((u8*)node + 0x4) = f2;
        *(u32*)((u8*)node + 0x8) = val;
        cur = *(void**)((u8*)ptr + 0xc);
        if (cur != NULL) {
            while (*(void**)((u8*)cur + 0x10) != NULL) {
                cur = *(void**)((u8*)cur + 0x10);
            }
            *(void**)((u8*)cur + 0x10) = node;
        } else {
            *(void**)((u8*)ptr + 0xc) = node;
        }
        *(void**)((u8*)node + 0x10) = NULL;
    }
    return 0;
}
#endif
#if 0
asm void fn_8013D6B8(void) {
#include "src/game/effect/effect_visual_fn_8013D6B8.inc"
}
#else
u32 fn_8013D6B8(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x18);
    if (effectId) {
        fn_80131200(effectId,
            (GSEffectStartFunc)envMapEffectInit,
            (GSEffectStopFunc)fn_8013D730,
            (GSEffectStartFunc)envMapEffectStart,
            (GSEffectStopFunc)fn_8013D7CC,
            0,
            (GSEffectUpdateFunc)fn_8013D984,
            0);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
extern u32 lbl_8047AEE0;
extern u16 lbl_8047AEE4;
#if 0
asm void fn_8013D730(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013D730.inc"
}
#else
u32 fn_8013D730(void* ptr) {
    void* next;
    u32 val;
    u32 ret;
    if (ptr) {
        next = *(void**)((u8*)ptr + 0xc);
        if (lbl_8047AEE0 != 0) {
            lbl_8047AEE4 -= 1;
            if (lbl_8047AEE4 == 0) {
                fn_800B8DF4();
                fn_800EF5A4((void*)lbl_8047AEE0);
                lbl_8047AEE0 = 0;
            }
        }
        while (next != NULL) {
            val = *(u16*)((u8*)next + 0xc);
            next = *(void**)((u8*)next + 0x10);
            fn_800E24B0(val);
            fn_800E209C(val);
        }
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}
#endif
extern void fn_800E5FFC(void* ptr);
#if 0
asm u32 fn_8013D7CC(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013D7CC.inc"
}
#else
u32 fn_8013D7CC(void* ptr) {
    void* inner;
    if (ptr != NULL) {
        inner = *(void**)ptr;
        if (inner != NULL) {
            fn_800E5FFC(inner);
        }
    }
    return 1;
}
#endif
extern void* GStextureCreate(u32 a, u32 b, u32 size, u32 d, u32 e);
extern u32 lbl_8047AEE0;
extern u8 lbl_80466BC0[];
extern u16 lbl_8047AEE4;
extern u8 lbl_80272F00[];
#if 0
asm u32 envMapEffectInit(void* ptr) {
#include "src/game/effect/effect_visual_envMapEffectInit.inc"
}
#else
u32 envMapEffectInit(void* ptr) {
    void* next;
    u32 val;
    if (ptr) {
        if (lbl_8047AEE0 == 0) {
            lbl_8047AEE4 = 0;
            lbl_8047AEE0 = (u32)GStextureCreate(*(u16*)(lbl_80466BC0 + 4), *(u16*)(lbl_80466BC0 + 6), 0x44, 0, 0);
            if (lbl_8047AEE0 == 0) {
                if (ptr) {
                    ptr = *(void**)((u8*)ptr + 0xc);
                    if (lbl_8047AEE0 != 0) {
                        lbl_8047AEE4 -= 1;
                        if (lbl_8047AEE4 == 0) {
                            fn_800B8DF4();
                            fn_800EF5A4((void*)lbl_8047AEE0);
                            lbl_8047AEE0 = 0;
                        }
                    }
                    while (ptr != NULL) {
                        val = *(u16*)((u8*)ptr + 0xc);
                        ptr = *(void**)((u8*)ptr + 0x10);
                        fn_800E24B0(val);
                        fn_800E209C(val);
                    }
                }
                return 0;
            }
        }
        lbl_8047AEE4 = lbl_8047AEE4 + 1;
        return 1;
    }
    fn_800DD970((const char*)lbl_80272F00);
    return 0;
}
#endif
extern void fn_800E61BC(void* a, void* b, void* c, void* d, f32 e);
extern u32 lbl_8047AEE0;
extern u8 lbl_80272F38[];
#if 0
asm u32 envMapEffectStart(void* ptr) {
#include "src/game/effect/effect_visual_envMapEffectStart.inc"
}
#else
u32 envMapEffectStart(void* ptr) {
    void* inner;
    if (ptr) {
        *(void**)((u8*)ptr + 0x10) = *(void**)((u8*)ptr + 0xc);
        *(u32*)((u8*)ptr + 0x14) = 0;
        inner = *(void**)ptr;
        if (inner == NULL) { goto log; }
        if (lbl_8047AEE0 == 0) { goto log; }
        fn_800E61BC(inner, *(void**)((u8*)ptr + 0x4), *(void**)((u8*)ptr + 0x8),
                    (void*)lbl_8047AEE0, *(f32*)*(void**)((u8*)ptr + 0x10));
        return 1;
    }
log:
    fn_800DD970((const char*)lbl_80272F38);
    return 0;
}
#endif
extern s32 fn_800E3B3C(void);
extern void fn_800D4604(u32 mode);
extern void fn_800D377C(u32 a);
extern void fn_800D3410(void* texture, u32 a);
extern void* fn_800E3B08(u32 index);
extern void fn_800E3C64(void);
extern void fn_800E3760(void* obj, u32 flags);
extern void fn_800D3190(void);
extern void fn_800E5FAC(void);
extern void fn_800E60F0(void);
extern u32 lbl_8047AEE0;
extern u32 lbl_8047D260;
#if 1
asm u32 fn_8013D984(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013D984.inc"
}
#else
u32 fn_8013D984(void* ptr, u32 delta) { /* TODO */ }
#endif
#if 0
asm void fn_8013DB64(void* ptr, u32 val, f32 f1, f32 f2) {
#include "src/game/effect/effect_visual_fn_8013DB64.inc"
}
#else
u32 fn_8013DB64(void* ptr, u32 val, f32 f1, f32 f2) {
    u16 handle;
    void* node;
    void* cur;
    handle = fn_800E3534(0x14);
    if (handle) {
        node = fn_800E27B0(handle);
        *(u16*)((u8*)node + 0xc) = handle;
        *(f32*)((u8*)node + 0x0) = f1;
        *(f32*)((u8*)node + 0x4) = f2;
        *(u32*)((u8*)node + 0x8) = val;
        cur = *(void**)((u8*)ptr + 0x14);
        if (cur != NULL) {
            while (*(void**)((u8*)cur + 0x10) != NULL) {
                cur = *(void**)((u8*)cur + 0x10);
            }
            *(void**)((u8*)cur + 0x10) = node;
        } else {
            *(void**)((u8*)ptr + 0x14) = node;
        }
        *(void**)((u8*)node + 0x10) = NULL;
    }
    return 0;
}
#endif
#if 0
asm void fn_8013DC18(void) {
#include "src/game/effect/effect_visual_fn_8013DC18.inc"
}
#else
u32 fn_8013DC18(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x20);
    if (effectId) {
        fn_80131200(effectId,
            (GSEffectStartFunc)fn_8013DD10,
            (GSEffectStopFunc)fn_8013DC94,
            (GSEffectStartFunc)blurEffectStart,
            (GSEffectStopFunc)fn_8013DD7C,
            0,
            (GSEffectUpdateFunc)fn_8013E470,
            (GSEffectRenderFunc)fn_8013DE6C);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm u32 fn_8013DC94(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013DC94.inc"
}
#else
u32 fn_8013DC94(void* ptr) {
    void* node;
    u16 val;
    if (ptr) {
        node = *(void**)((u8*)ptr + 0x14);
        if (*(void**)((u8*)ptr + 0xc) != NULL) {
            fn_800B8DF4();
            fn_800B856C();
            fn_800EF5A4(*(void**)((u8*)ptr + 0xc));
        }
        while (node) {
            val = *(u16*)((u8*)node + 0xc);
            node = *(void**)((u8*)node + 0x10);
            fn_800E24B0(val);
            fn_800E209C(val);
        }
    }
    return 1;
}
#endif
#if 0
asm u32 fn_8013DD10(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013DD10.inc"
}
#else
u32 fn_8013DD10(void* ptr) {
    if (ptr) {
        if (*(s32*)((u8*)ptr + 0x10) != 0) {
            *(void**)((u8*)ptr + 0xc) = GStextureCreate(0, 0, 0x44, 0, 0);
            if (*(void**)((u8*)ptr + 0xc) == NULL) {
                return 0;
            }
        }
    }
    return 1;
}
#endif
extern void GSgfxEndBackFBCapture(void* a);
extern void fn_800E5790(void* a);
#if 0
asm u32 fn_8013DD7C(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013DD7C.inc"
}
#else
u32 fn_8013DD7C(void* ptr) {
    void* inner;
    if (ptr) {
        if (*(s32*)((u8*)ptr + 0x10) != 0) {
            GSgfxEndBackFBCapture(*(void**)((u8*)ptr + 0xc));
        } else {
            inner = *(void**)ptr;
            if (inner) {
                fn_800E5790(inner);
            }
        }
    }
    return 1;
}
#endif
extern void GSgfxBeginBackFBCapture(void* handle, void* callback, void* ctx);
extern void* fn_800E584C(void* a, void* b);
extern u8 lbl_80272F70[];
#if 0
asm u16 blurEffectStart(void) {
#include "src/game/effect/effect_visual_blurEffectStart.inc"
}
#else
u32 blurEffectStart(void* ptr) {
    if (ptr) {
        *(u32*)((u8*)ptr + 0x18) = *(u32*)((u8*)ptr + 0x14);
        *(u32*)((u8*)ptr + 0x1c) = 0;
        if (*(s32*)((u8*)ptr + 0x10) != 0) {
            if (*(void**)((u8*)ptr + 0xc) != NULL) {
                GSgfxBeginBackFBCapture(*(void**)((u8*)ptr + 0xc), (void*)fn_8013E258, ptr);
                return 1;
            }
        } else {
            if (*(void**)ptr != NULL) {
                *(void**)((u8*)ptr + 0x4) = fn_800E584C(*(void**)ptr, (u8*)ptr + 0x8);
                return 1;
            }
        }
    }
    fn_800DD970((const char*)lbl_80272F70);
    return 0;
}
#endif
extern void fn_800EC960(void);
extern void fn_800D3068(void);
extern void GSmodelGetAnimFrame(void);
extern void fn_800EC570(void);
extern void fn_800D45F8(void);
extern void fn_800DF3F0(void);
extern void GSmaterialSetFlags(void);
extern void fn_800DF188(void);
extern void fn_800EC990(void* model);
extern void fn_800ECA78(void* model, f32 value);
extern void fn_800EC134(void);
extern void fn_800DF140(void);
extern void fn_800DF504(void);
extern u32 lbl_8047D288;
extern u32 lbl_8047D26C;
extern u32 lbl_8047D268;
extern u32 lbl_8047D270;
extern u32 lbl_8047D274;
extern u32 lbl_8047D278;
extern u32 lbl_8047D290;
extern u32 lbl_8047D27C;
extern u8 lbl_80363CC8[];
extern u32 lbl_8047D280;
#if 1
asm void fn_8013DE6C(void) {
#include "src/game/effect/effect_visual_fn_8013DE6C.inc"
}
#else
void fn_8013DE6C(void) { /* TODO */ }
#endif
extern u32 lbl_8047D27C;
extern u32 lbl_8047D288;
extern u32 lbl_8047D298;
extern u32 lbl_8047D29C;
extern u32 lbl_8047D2A0;
extern u8 lbl_80314AE8[];
extern u32 lbl_8047D280;
#if 1
asm u32 fn_8013E258(void* model, void* unused, void* state) {
#include "src/game/effect/effect_visual_fn_8013E258.inc"
}
#else
u32 fn_8013E258(void* model, void* unused, void* state) { /* TODO */ }
#endif
#if 0
asm void fn_8013E470(void) {
#include "src/game/effect/effect_visual_fn_8013E470.inc"
}
#else
u32 fn_8013E470(void* ptr, u32 delta) {
    void* node;
    if (ptr == NULL) {
        goto ret0;
    }
    node = *(void**)((u8*)ptr + 0x18);
    if (node == NULL) {
        goto ret0;
    }
    while (*(u32*)((u8*)ptr + 0x1c) >= *(u32*)((u8*)node + 0x8)) {
        *(u32*)((u8*)ptr + 0x1c) -= *(u32*)((u8*)node + 0x8);
        node = *(void**)((u8*)node + 0x10);
        *(void**)((u8*)ptr + 0x18) = node;
        if (node == NULL) {
            return 0;
        }
    }
    *(u32*)((u8*)ptr + 0x1c) += delta;
    return 1;
ret0:
    return 0;
}
#endif
#if 0
asm void fn_8013E4D4(void) {
#include "src/game/effect/effect_visual_fn_8013E4D4.inc"
}
#else
u32 fn_8013E4D4(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x34);
    if (effectId) {
        fn_80131200(effectId,
            (GSEffectStartFunc)fn_8013E5AC,
            (GSEffectStopFunc)fn_8013E54C,
            (GSEffectStartFunc)auraEffectStart,
            0,
            0,
            (GSEffectUpdateFunc)fn_8013E8A4,
            (GSEffectRenderFunc)fn_8013E6C4);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm void fn_8013E54C(void) {
#include "src/game/effect/effect_visual_fn_8013E54C.inc"
}
#else
#pragma push
#pragma global_optimizer off
u32 fn_8013E54C(void* arg) {
    void* ptr;
    if (arg != 0) {
        ptr = arg;
        if (*(void**)((u8*)ptr + 4) != 0) {
            fn_800B8DF4();
            fn_800B856C();
            fn_800EF5A4(*(void**)((u8*)ptr + 4));
        }
        memset(ptr, 0, 0x34);
    }
    return 1;
}
#pragma pop
#endif
#if 0
asm void fn_8013E5AC(void) {
#include "src/game/effect/effect_visual_fn_8013E5AC.inc"
}
#else
u32 fn_8013E5AC(u8* p) {
    extern u8 lbl_80466BC0[];
    extern void* GStextureCreate(u32 a, u32 b, u32 size, u32 d, u32 e);
    extern void* fn_800EF548(void* buf, u32 b);
    extern void fn_800EF504(void* buf);
    u8* tbl;
    void* buf;
    if (p != 0) {
        tbl = lbl_80466BC0;
        if (p[0x18] > 0x1e) {
            p[0x18] = 0x1e;
        }
        buf = GStextureCreate(*(u16*)(tbl + 4), *(u16*)(tbl + 6), 0xa0, 0, 0);
        if (buf != 0) {
            *(void**)(p + 4) = buf;
            memset(fn_800EF548(buf, 0), 0, *(u16*)(tbl + 4) * *(u16*)(tbl + 6));
            fn_800EF504(buf);
            return 1;
        }
    }
    return 0;
}
#endif
extern u8 lbl_80272FA0[];
#if 0
asm u32 auraEffectStart(void* ptr) {
#include "src/game/effect/effect_visual_auraEffectStart.inc"
}
#else
u32 auraEffectStart(void* ptr) {
    if (ptr == NULL) { goto log; }
    *(u16*)((u8*)ptr + 0x30) = 0;
    if (*(void**)ptr == NULL) { goto log; }
    if (*(u8*)((u8*)ptr + 0x18) == 0) { goto log; }
    if (*(void**)((u8*)ptr + 0x4) == NULL) { goto log; }
    return 1;
log:
    fn_800DD970((const char*)lbl_80272FA0);
    return 0;
}
#endif
extern u32 lbl_8047D2AC;
extern u32 lbl_8047D2B0;
extern u32 lbl_8047D2B4;
extern u32 lbl_8047D2A8;
#if 1
u32 fn_8013E6C4(u8* ptr)
{
    void* model;

    if (ptr != NULL) {
        model = *(void**)(ptr + 4);
        if (model == NULL) {
            return 0;
        }
        fn_8013EA44(ptr);
        fn_800D9B58(*(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2AC,
                    *(f32*)&lbl_8047D2B0, *(f32*)&lbl_8047D2B4);
        if (ptr[0x19] & 4) {
            fn_800DA4C4(1, 6, 7);
        } else {
            fn_800DA4C4(1, 6, 1);
        }
        fn_800DA1E8(0, 7, 0);
        fn_800DA2BC(1, 1, 0);
        fn_800DA028(0);
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D7820(lbl_80314AE8);
        fn_800D6A00(4);
        fn_800D85D4(0, model);
        fn_800D67BC(4);
        fn_800D6680(*(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2AC,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        fn_800D59B8(0, *(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2AC);
        fn_800D6680(*(f32*)&lbl_8047D2B0, *(f32*)&lbl_8047D2AC,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        fn_800D59B8(0, *(f32*)&lbl_8047D2A8, *(f32*)&lbl_8047D2AC);
        fn_800D6680(*(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2B4,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        fn_800D59B8(0, *(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2A8);
        fn_800D6680(*(f32*)&lbl_8047D2B0, *(f32*)&lbl_8047D2B4,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        fn_800D59B8(0, *(f32*)&lbl_8047D2A8, *(f32*)&lbl_8047D2A8);
        fn_800D6728();
        return 1;
    }
    return 0;
}
#else
void fn_8013E6C4(void) { /* TODO */ }
#endif
extern void fn_800CE318(void);
extern u32 lbl_8047D2B8;
extern u32 lbl_8047D2A8;
extern u32 lbl_8047D2BC;
extern u32 lbl_8047D2D8;
extern u32 lbl_8047D2C0;
extern u32 lbl_8047D2C8;
extern u32 lbl_8047D2D0;
extern u32 lbl_8047D2D4;
#if 1
asm u32 fn_8013E8A4(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013E8A4.inc"
}
#else
u32 fn_8013E8A4(void* ptr, u32 delta) { /* TODO */ }
#endif
extern void __assert(void);
extern void* fn_800E3C5C(void* model);
extern void fn_800E4514(void);
extern void fn_800E69C4(void);
extern void fn_800E66B8(void);
extern void fn_8019D620(void);
extern void fn_800E6804(void);
extern void fn_800E65CC(void);
extern void fn_800E68D8(void);
extern u8 lbl_80363CD8[];
extern u8 lbl_8047D2E0[];
extern u8 lbl_8047D2E8[];
extern u8 lbl_80272FD0[];
extern u8 lbl_8047D2F0[];
extern u32 lbl_8047D2D4;
extern u32 lbl_8047D2F8;
extern u32 lbl_8047D2A8;
#if 1
asm void fn_8013EA44(void) {
#include "src/game/effect/effect_visual_fn_8013EA44.inc"
}
#else
void fn_8013EA44(void) { /* TODO */ }
#endif
#if 0
asm void fn_8013F000(void) {
#include "src/game/effect/effect_visual_fn_8013F000.inc"
}
#else
u32 fn_8013F000(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0xb4);
    if (effectId) {
        fn_80131200(effectId,
            (GSEffectStartFunc)fn_8013F114,
            (GSEffectStopFunc)fn_8013F078,
            (GSEffectStartFunc)distortionEffectStart,
            0,
            0,
            (GSEffectUpdateFunc)fn_8013F80C,
            (GSEffectRenderFunc)fn_8013F410);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
extern void fn_800D75F4(void* a);
extern u32 lbl_8047AEE8;
extern u32 lbl_8047AEEC;
#if 0
asm void fn_8013F078(void) {
#include "src/game/effect/effect_visual_fn_8013F078.inc"
}
#else
u32 fn_8013F078(void* ptr) {
    if (ptr) {
        fn_800B8DF4();
        fn_800B856C();
        if (*(void**)((u8*)ptr + 0x4) != NULL) {
            fn_800EF5A4(*(void**)((u8*)ptr + 0x4));
            *(u32*)((u8*)ptr + 0x4) = 0;
        }
        if (*(void**)ptr != NULL) {
            fn_800D75F4(*(void**)ptr);
            *(u32*)ptr = 0;
        }
        if (lbl_8047AEE8 != 0) {
            *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC - 1;
            if (*(u16*)&lbl_8047AEEC == 0) {
                fn_800B8DF4();
                fn_800EF5A4((void*)lbl_8047AEE8);
                lbl_8047AEE8 = 0;
            }
        }
    }
    return 1;
}
#endif
extern void* fn_800D7894(void);
extern void fn_800D7868(void* handle, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
extern u32 lbl_8047AEE8;
extern u32 lbl_8047AEEC;
#if 1
u32 fn_8013F114(void* ptr) {
    u8* p;

    p = ptr;
    if (p != NULL) {
        if (*(void**)(p + 0x4) == NULL) {
            return 0;
        }
        if (lbl_8047AEE8 == 0) {
            *(u16*)&lbl_8047AEEC = 0;
            lbl_8047AEE8 = (u32)GStextureCreate(*(u16*)(lbl_80466BC0 + 4), *(u16*)(lbl_80466BC0 + 6), 0x45, 0, 0);
            if (lbl_8047AEE8 == 0) {
                if (p != NULL) {
                    fn_800B8DF4();
                    fn_800B856C();
                    if (*(void**)(p + 0x4) != NULL) {
                        fn_800EF5A4(*(void**)(p + 0x4));
                        *(u32*)(p + 0x4) = 0;
                    }
                    if (*(void**)p != NULL) {
                        fn_800D75F4(*(void**)p);
                        *(u32*)p = 0;
                    }
                    if (lbl_8047AEE8 != 0) {
                        *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC - 1;
                        if (*(u16*)&lbl_8047AEEC == 0) {
                            fn_800B8DF4();
                            fn_800EF5A4((void*)lbl_8047AEE8);
                            lbl_8047AEE8 = 0;
                        }
                    }
                }
                return 0;
            }
        }
        *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC + 1;
        *(void**)p = fn_800D7894();
        if (*(void**)p == NULL) {
            if (p != NULL) {
                fn_800B8DF4();
                fn_800B856C();
                if (*(void**)(p + 0x4) != NULL) {
                    fn_800EF5A4(*(void**)(p + 0x4));
                    *(u32*)(p + 0x4) = 0;
                }
                if (*(void**)p != NULL) {
                    fn_800D75F4(*(void**)p);
                    *(u32*)p = 0;
                }
                if (lbl_8047AEE8 != 0) {
                    *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC - 1;
                    if (*(u16*)&lbl_8047AEEC == 0) {
                        fn_800B8DF4();
                        fn_800EF5A4((void*)lbl_8047AEE8);
                        lbl_8047AEE8 = 0;
                    }
                }
            }
            return 0;
        }
        fn_800D7868(*(void**)p, 1, 0, 1, 4, 0, 0, 0);
        fn_800D7868(*(void**)p, 4, 0, 5, 6, 0, 0, 0);
        fn_800D7868(*(void**)p, 6, 0, 8, 4, 0, 0, 0);
        fn_800D7868(*(void**)p, 7, 0, 8, 4, 0, 0, 0);
        return 1;
    }
    return 0;
}
#else
u32 fn_8013F114(void* ptr) { /* TODO */ }
#endif
extern u32 lbl_8047AEE8;
extern u32 lbl_8047D300;
extern u8 lbl_80272FE0[];
#if 1
asm u16 distortionEffectStart(void) {
#include "src/game/effect/effect_visual_distortionEffectStart.inc"
}
#else
u16 distortionEffectStart(void) { /* TODO */ }
#endif
extern u8 fn_800E3D08(void* obj);
extern void fn_80118104(u32 a, u32 b);
extern void fn_800D848C(u32 a, u32 b, u32 c, void* d);
extern void fn_800E064C(void* dst);
extern void fn_800DC1D4(u32 a);
extern void fn_800DBA54(u32 a);
extern void fn_800DB9F0(u32 a, u32 b, u32 c);
extern void fn_800DB988(u32 a, u32 b, u32 c);
extern void fn_800DB900(u32 a, void* b, u32 c);
extern void fn_800DBCE4(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800DC224(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800DC14C(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void fn_800DC0D4(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800DC04C(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void fn_800DBFD4(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800D5C18(u32 a, u32 r, u32 g, u32 b);
extern void fn_800DBE5C(u32 a);
extern u32 lbl_8047AEE8;
extern u32 lbl_8047AEF0;
extern u32 lbl_8047D304;
extern u32 lbl_8047D300;
extern u32 lbl_8047D308;
#if 1
u32 fn_8013F410(void* ptr) {
    f32 mtx[12];
    u8* p;
    s32 count;
    s32 i;
    void* obj;

    if (ptr != NULL) {
        p = ptr;
        if (*(void**)(p + 0x4) == NULL || lbl_8047AEE8 == 0) {
            return 0;
        }
        _distortionEffectUpdateMatrices();
        if (lbl_8047AEE8 != 0) {
            count = fn_800E3B3C();
            if (count != 0) {
                fn_800D4604(2);
                fn_800D2248();
                fn_800D377C(1);
                fn_800D3410((void*)lbl_8047AEE8, 0);
                for (i = 0; i < count; i++) {
                    obj = fn_800E3B08(i);
                    if (obj != NULL && fn_800E3D08(obj) != 0) {
                        fn_800E3760(obj, 0x3010);
                    }
                }
                if (*(u16*)(p + 0x28) != 0) {
                    fn_80118104(0x2000, 0);
                    fn_80118104(0x2000, 1);
                }
                fn_800D3190();
                fn_800D4604(1);
            }
        }
        fn_800DA1E8(0, 2, 0);
        fn_800DA028(0);
        fn_800D88DC(0x80000003);
        fn_800D888C(4);
        fn_800DA4C4(0, 6, 7);
        fn_800D9ED8(1);
        fn_800DA2BC(1, 1, 0);
        fn_800D85D4(0, (void*)lbl_8047AEE8);
        fn_800D85D4(1, *(void**)(p + 0x4));
        fn_800D848C(0, 2, 4, p + 0x68);
        fn_800E064C(mtx);
        fn_800D848C(1, 2, 5, mtx);
        fn_800DC1D4(1);
        fn_800DBA54(1);
        fn_800DB9F0(0, 1, 1);
        fn_800DB988(0, lbl_8047AEF0, lbl_8047AEF0);
        fn_800DB900(1, p + 0x98, 1);
        fn_800DBCE4(0, 0, 1, 0, 1);
        fn_800DC224(0, 0, 0, 0, 2);
        fn_800DC14C(0, 0, 0, 0, 1, 0);
        fn_800DC0D4(0, 0xf, 0xf, 0xf, 8);
        fn_800DC04C(0, 0, 0, 0, 1, 0);
        fn_800DBFD4(0, 7, 7, 7, 4);
        fn_800D7820(*(void**)p);
        fn_800D6A00(6);
        fn_800D7F14(p + 0x38);
        fn_800D67BC(4);

        fn_800D6680(*(f32*)&lbl_8047D304, *(f32*)&lbl_8047D304, *(f32*)&lbl_8047D300);
        fn_800D5C18(0, 0xff, 0xff, 0xff);
        fn_800D59B8(0, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D300);
        fn_800D59B8(1, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D300);

        fn_800D6680(*(f32*)&lbl_8047D308, *(f32*)&lbl_8047D304, *(f32*)&lbl_8047D300);
        fn_800D5C18(0, 0xff, 0xff, 0xff);
        fn_800D59B8(0, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);
        fn_800D59B8(1, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);

        fn_800D6680(*(f32*)&lbl_8047D308, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);
        fn_800D5C18(0, 0xff, 0xff, 0xff);
        fn_800D59B8(0, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D308);
        fn_800D59B8(1, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D308);

        fn_800D6680(*(f32*)&lbl_8047D304, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);
        fn_800D5C18(0, 0xff, 0xff, 0xff);
        fn_800D59B8(0, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D308);
        fn_800D59B8(1, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D308);

        fn_800D6728();
        fn_800D7E5C();
        fn_800D848C(0, 2, 4, mtx);
        fn_800DBE5C(0);
        fn_800DBA54(0);
        fn_800D888C(0x80000002);
        fn_800D9ED8(0);
        return 1;
    }
    return 0;
}
#else
u32 fn_8013F410(void* ptr) { /* TODO */ }
#endif
extern u32 lbl_8047AEE8;
extern u32 lbl_8047D310;
extern u32 lbl_8047D300;
extern u32 lbl_8047D308;
#if 1
asm u32 fn_8013F80C(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013F80C.inc"
}
#else
u32 fn_8013F80C(void* ptr, u32 delta) { /* TODO */ }
#endif
extern void fn_800D7BF8(void);
extern void GScameraGetLookAt(void* mtx, void* lookAt, void* eye);
extern void fn_800E0628(void);
extern void fn_800E0238(void);
extern void fn_800D2DE8(void);
extern void fn_800EF4FC(void);
extern void fn_800EF4F4(void);
extern void fn_800E03E8(void);
extern u32 lbl_8047D300;
extern u32 lbl_8047D304;
extern u32 lbl_8047D308;
extern u8 lbl_8027301C[];
extern u32 lbl_8047D318;
extern u32 lbl_8047D31C;
extern u32 lbl_8047D320;
extern u32 lbl_8047AEE8;
extern u32 lbl_8047D310;
extern u32 lbl_8047D324;
#if 1
asm void _distortionEffectUpdateMatrices(void) {
#include "src/game/effect/effect_visual__distortionEffectUpdateMatrices.inc"
}
#else
void _distortionEffectUpdateMatrices(void) { /* TODO */ }
#endif
#if 0
asm void patchiruTextureStart(void) {
#include "src/game/effect/effect_visual_patchiruTextureStart.inc"
}
#else
u32 patchiruTextureStart(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x24);
    if (effectId) {
        fn_80131200(effectId,
            (GSEffectStartFunc)fn_8013FCC4,
            (GSEffectStopFunc)fn_8013FC58,
            (GSEffectStartFunc)billboardEffectStart,
            (GSEffectStopFunc)fn_8013FD68,
            0,
            (GSEffectUpdateFunc)fn_8013FF0C,
            0);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif
#if 0
asm void fn_8013FC58(void) {
#include "src/game/effect/effect_visual_fn_8013FC58.inc"
}
#else
u32 fn_8013FC58(void* ptr) {
    if (ptr) {
        if (*(u32*)((u8*)ptr + 0x10) != 0) {
            fn_800F9210(*(u32*)ptr, *(u32*)((u8*)ptr + 0x8));
            fn_800F9210(*(u32*)ptr, *(u32*)((u8*)ptr + 0x4));
        }
        if (*(u32*)((u8*)ptr + 0x14) != 0) {
            fn_800F9210(*(u32*)ptr, *(u32*)((u8*)ptr + 0xc));
        }
    }
    return 1;
}
#endif
extern void* fn_801195AC(void* a);
#if 0
asm void fn_8013FCC4(void) {
#include "src/game/effect/effect_visual_fn_8013FCC4.inc"
}
#else
u32 fn_8013FCC4(void* ptr) {
    void* res;
    if (ptr) {
        if (*(u32*)((u8*)ptr + 0x4) != 0 && *(u32*)((u8*)ptr + 0x8) != 0) {
            *(void**)((u8*)ptr + 0x10) = fn_800F9318(*(u32*)ptr, *(u32*)((u8*)ptr + 0x8));
        } else {
            *(void**)((u8*)ptr + 0x10) = NULL;
        }
        if (*(u32*)((u8*)ptr + 0xc) != 0) {
            res = fn_800F9318(*(u32*)ptr, *(u32*)((u8*)ptr + 0xc));
            if (res != NULL) {
                *(void**)((u8*)ptr + 0x14) = fn_801195AC(res);
            } else {
                *(void**)((u8*)ptr + 0x14) = NULL;
            }
        } else {
            *(void**)((u8*)ptr + 0x14) = NULL;
        }
        return 1;
    }
    return 0;
}
#endif
extern void fn_80118874(void* a, u32 b);
extern void fn_800EC96C(void* a);
#if 0
asm u32 fn_8013FD68(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013FD68.inc"
}
#else
u32 fn_8013FD68(void* ptr) {
    void* inner;
    if (ptr) {
        inner = *(void**)((u8*)ptr + 0x14);
        if (inner) {
            fn_80118874(inner, 0);
        }
        if (*(void**)((u8*)ptr + 0x10)) {
            fn_800EC96C(*(void**)((u8*)ptr + 0x10));
            fn_800EC1D4(*(void**)((u8*)ptr + 0x10));
            fn_800E4014(*(void**)((u8*)ptr + 0x10), 0);
        }
    }
    return 1;
}
#endif
extern void* fn_801190DC(void* ptr, u16 arg, u32 flag);
extern void fn_800E3CC8(void* model, u32 flag);
extern u8 fn_800EC1BC(void* model);
extern void fn_800ECCA8(void* model, u16 value);
extern void fn_800ECB74(void* model, u32 value);
extern void fn_800EC9DC(void* model, f32 value);
extern u32 lbl_8047D328;
extern u32 lbl_8047D32C;
extern u8 lbl_80273078[];
#if 0
asm u16 billboardEffectStart(void) {
#include "src/game/effect/effect_visual_billboardEffectStart.inc"
}
#else
u16 billboardEffectStart(void* ptr) {
    void* model;

    if (ptr == NULL) {
        goto report_null;
    }

    model = *(void**)((u8*)ptr + 0x10);
    if (model == NULL && *(void**)((u8*)ptr + 0x14) == NULL) {
        return 0;
    }

    if (*(void**)((u8*)ptr + 0x14) != NULL) {
        *(void**)((u8*)ptr + 0x20) = fn_801190DC(*(void**)((u8*)ptr + 0x14), *(u16*)((u8*)ptr + 0x1e), 1);
    }

    if (model != NULL) {
        fn_800E4014(model, 1);
        fn_800E3CC8(model, 1);
        if (fn_800EC1BC(model)) {
            fn_800ECCA8(model, *(u16*)((u8*)ptr + 0x1c));
            fn_800ECB74(model, *(u32*)((u8*)ptr + 0x18));
            fn_800EC9DC(model, *(f32*)&lbl_8047D328);
            fn_800ECA78(model, *(f32*)&lbl_8047D32C);
            fn_800EC990(model);
        }
        if (fn_800EC1B0(model)) {
            fn_800EC35C(model, *(u16*)((u8*)ptr + 0x1c));
            fn_800EC208(model, *(u32*)((u8*)ptr + 0x18));
            fn_800EC308(model, *(f32*)&lbl_8047D328);
            fn_800EC2A4(model, *(f32*)&lbl_8047D32C);
            fn_800EC1E4(model);
        }
    }
    return 1;

report_null:
    fn_800DD970((const char*)lbl_80273078);
    return 0;
}
#endif
extern f64 fn_800CE220(f64 x);
extern u32 fn_80118DA8(void* ptr);
extern void fn_80118F04(void* arg1, void* arg2);
extern u8 fn_800EC954(void* model);
extern u8 fn_800EC1C8(void* model);
extern void fn_800E43A4(void* entry, void* param);
extern void fn_800E407C(void* entry, void* param);
extern u8 lbl_80273060[];
extern u8 lbl_8027306C[];
extern u32 lbl_8047D334;
extern u32 lbl_8047D328;
extern u32 lbl_8047D330;
extern u32 lbl_8047D338;
extern u32 lbl_8047D32C;
#if 1
asm u32 fn_8013FF0C(void* ptr) {
#include "src/game/effect/effect_visual_fn_8013FF0C.inc"
}
#else
u32 fn_8013FF0C(void* ptr) { /* TODO */ }
#endif
#if 0
asm void fn_80140138(void) {
#include "src/game/effect/effect_visual_fn_80140138.inc"
}
#else
void fn_80140138(u8* ptr) {
    extern void fn_800DF608(void* handle);
    if (*(u32*)(ptr) != 0) {
        fn_800DF608(*(void**)(ptr));
        *(u32*)(ptr) = 0;
    }
    if (*(u32*)(ptr + 0x4) != 0) {
        fn_800EF5A4(*(void**)(ptr + 0x4));
        *(u32*)(ptr + 0x4) = 0;
    }
}
#endif
extern void* _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl(void* tex1, void* tex2, u32 arg);
extern void fn_800DF028(void* material, void* texture);
#if 0
asm void fn_80140190(void) {
#include "src/game/effect/effect_visual_fn_80140190.inc"
}
#else
u32 fn_80140190(void** out, void* model, u32 arg) {
    u32 ret;
    void* handle;
    void* tex1;
    void* tex2;

    ret = 0;
    if (model == NULL) {
        return 0;
    }

    handle = fn_800EE150(model, 2);
    if (handle == NULL) {
        return 0;
    }

    out[0] = NULL;
    out[1] = NULL;
    out[0] = fn_800EE6B4(handle, 0);
    if (out[0] != NULL) {
        tex1 = fn_800F9318(0x387, 0x10561200);
        if (tex1 != NULL) {
            tex2 = fn_800F9318(0x387, 0x10571200);
            if (tex2 != NULL) {
                out[1] = _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl(tex1, tex2, arg);
                if (out[1] != NULL) {
                    fn_800DF028(out[0], out[1]);
                    ret = 1;
                }
            }
        }
    }

    if ((u8)ret == 0) {
        if (out[1] != NULL) {
            fn_800EF5A4(out[1]);
            out[1] = NULL;
        }
        if (out[0] != NULL) {
            fn_800DF608(out[0]);
            out[0] = NULL;
        }
    }

    fn_800EE828(handle);
    return ret;
}
#endif

#if 0
asm void fn_801436F0(void) {
#include "src/game/effect/effect_visual_fn_801436F0.inc"
}
#else
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
u32 fn_801436F0(void* ptr) {
    s32 v;
    if (ptr == NULL) return 0;
    v = !!((((u8*)ptr)[0x4] >> 3) & 1);
    return v;
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#endif

#if 0
asm void fn_80143718(void) {
#include "src/game/effect/effect_visual_fn_80143718.inc"
}
#else
unsigned int fn_80143718(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return (unsigned int)(unsigned char)p[0xF];
}
#endif

#if 0
asm void fn_80143730(void) {
#include "src/game/effect/effect_visual_fn_80143730.inc"
}
#else
unsigned int fn_80143730(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return (unsigned int)(unsigned char)p[0xE];
}
#endif

#if 0
asm void fn_80143748(void) {
#include "src/game/effect/effect_visual_fn_80143748.inc"
}
#else
unsigned int fn_80143748(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return (unsigned int)(unsigned char)p[0xD];
}
#endif

#if 0
asm void fn_80143778(void) {
#include "src/game/effect/effect_visual_fn_80143778.inc"
}
#else
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
unsigned int fn_80143778(const void *ptr) {
    s32 v;
    if (ptr == NULL) return 0;
    v = !!((((u8*)ptr)[0x4] >> 4) & 1);
    return v;
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#endif

#if 0
asm void fn_801437D8(void) {
#include "src/game/effect/effect_visual_fn_801437B8.inc"
}
#else
unsigned int fn_801437D8(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return ((p[4] & 0xFE) != 0);
}
#endif

#if 0
asm void fn_80143ABC(void) {
#include "src/game/effect/effect_visual_fn_80143ABC.inc"
}
#else
int fn_80143ABC(void* obj, u32 index) {
    s8* new_var;
    u32 masked_index;
    if (obj == NULL) return 0;
    masked_index = (u16)index;
    if (masked_index >= 0x19) return 0;
    new_var = (s8*)obj;
    return (int)(s8)*(new_var + masked_index + 4);
}
#endif
