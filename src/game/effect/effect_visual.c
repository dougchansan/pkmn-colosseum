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
 *   GSresGetResource (floor resource lookup)
 *   GSmodelGetPosition (GSmem / resource helper)
 *   _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID, GScameraGetActiveCamera, GScameraGetPosition, GScameraGetPerspective -- matrix/vector ops
 *   fn_800DA4C4, fn_800DA2BC, fn_800DA1E8, fn_800DA028 -- GX TEV/material setup
 *   fn_800D88DC, fn_800D888C -- GX blend/alpha mode
 *   fn_800D7820 -- GX draw begin
 *   fn_800E05C0, fn_800E048C -- matrix position/rotation
 *   fn_800D7F14 -- GX load matrix
 *   fn_800D6A00 -- GX cull mode
 *   fn_800D67BC, fn_800D6680, fn_800D5CB8 -- GX position/color/texcoord
 *   fn_800B9404 -- fog setup
 *   GXDrawDone, fn_800B856C -- GX begin/end frame
 *   GStextureFree -- model release
 *   cameraGetActive -- camera matrix helper
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
extern void  GSlogWrite(const char* fmt, ...);     /* OSReport */
extern void* memset(void* dst, int val, u32 size);

/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GSbezierCalculateVector();
extern void* GSfilterCreate(void);
extern void GSmaterialSetFlags();
extern double sin(double);
extern double cos(double);

/* GSmem allocator */
extern u16   _toolentryAlloc__FUl(u32 size);
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
extern void* GSresGetResource(u32 group, u32 model);

/* Matrix / vector operations */
extern void  GSmodelGetPosition(void* dst, void* src);
extern void  _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern void* GScameraGetActiveCamera(void);
extern void  GScameraGetPosition(void* mtx, void* vec);
extern void  GScameraGetPerspective(void* mtx, void* rx, void* ry, void* rz, void* scale);
extern f32   GSvecDistance(void* vecA, void* vecB);
extern void  GSvecSub(void* dst, void* srcA, void* srcB);
extern void  fn_800E0060(void* dst, void* src);
extern void  vecScale(void* dst, void* src, f32 scale);
extern void  fn_800E019C(void* dst, void* a, void* b);
extern void  fn_800E01D0(void* dst, void* src);
extern void  fn_800E01F4(void* dst, f32 x, f32 y, f32 z);

/* GX rendering pipeline */
extern void  gfxSetBlendMode(u32 a, u32 b, u32 c);
extern void  gfxSetUpdates(u32 a, u32 b, u32 c);
extern void  gfxSetZMode(u32 a, u32 b, u32 c);
extern void  gfxSetCullMode(u32 a);
extern void  gfxSetFlags(u32 a);
extern void  gfxClearFlags(u32 a);
extern void  gfxSetVtxDesc(void* model);
extern void  fn_800E05C0(void* dst, f32 x, f32 y, f32 z);
extern void  fn_800E048C(void* mtx, f32 x, f32 y, f32 z);
extern void  fn_800D7F14(void* mtx);
extern void  gfxSetPrimitive(u32 mode);
extern void  gfxBegin(u16 index);
extern void  gfxVtxPos(f32 x, f32 y, f32 z);
extern void  fn_800D5CB8(u32 a, u8 r, u8 g, u8 b, u32 alpha);
extern void  fn_800B9404(u32 index, u32 param);
extern void  GXDrawDone(void);
extern void  fn_800B856C(void);
extern void  GStextureFree(void* model);
extern void  gfxSetOrtho(f32 a, f32 b, f32 c, f32 d);
extern void  fn_800D9ED8(u32 param);
extern void* GSmodelGetPart(void* model, u32 mode);
extern void  GSpartGetTransform(void* obj, void* a, void* b, void* c);
extern void  GSpartFree(void* handle);
extern void* GSpartGetMaterial(void* handle, u32 idx);
extern void* GStextureCreate(u16 width, u16 height, u32 format, u32 tlut, u8 mip);
extern void* fn_800E3B08(u32 index);
extern void  fn_800E24B0(u16 handle);
extern void  fn_800E209C(u16 handle);
extern void  fn_800F9210(u32 group, u32 resource);
extern void  GSmodelSetVisibility(void* model, u32 visible);
extern u8    GSmodelCanTexAnimate(void* model);
extern void  GSmodelSetTexAnimIndex(void* model, u16 value);
extern void  GSmodelSetTexAnimRate(void* model, f32 value);
extern void  GSmodelSetTexAnimFrame(void* model, f32 value);
extern void  GSmodelSetTexAnimType(void* model, u32 value);
extern void  GSmodelStartTexAnimation(void* model);
extern u8    lbl_80314AE8[];
extern u8    lbl_80466BC0[];

/* Camera */
extern void* cameraGetActive(void);

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
/* Unload__13ModelSequenceFPUc: _billboardTransformHelper */
/* fn_80140190: _billboardTransformHelper2 (0x11C bytes) */
/* _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl: _billboardTransformMain (0x2DC bytes) */
extern f32 vecMag(void*);
extern void gfxEnd(void);
extern void gfxLoadTexture(u32 a, void* b);
extern f32 fn_800E0BA0(void);
extern void gfxVtxTexCoord(u32 a, f32 b, f32 c);
extern void fn_800D7E5C(void);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
/* Forward declarations for self-referencing asm blocks */
extern u32 _lightningRenderMain(void* ptr);
extern u32 fn_80138630(void* ptr);
extern u32 fn_80138680(void* ptr);
extern u32 fn_801386DC(void* ptr);
extern u32 fn_801387C0(void* ptr, u32 delta);
extern void fn_80138838(void* ptr, u32 b);
extern u32 fn_80138B74(void* ptr);
extern u32 fn_80138BBC(void* ptr);
extern u32 fn_80138CCC(void* ptr);
extern u32 fn_80138DE4(void* ptr, u32 delta);
extern void fn_80139074(void* entry, u8* leaf);
extern u8 _leaffxGenerateLeafData(void* ptr, void* entry);
extern u32 fn_80139898(void* ptr);
extern u32 fn_801398E0(void* ptr);
extern u32 fn_80139934(void* ptr);
extern BOOL fn_80139AC4(void* ptr, u32 tick);
extern u32 fn_80139D10(void* ptr);
extern void fn_80139E80(u8* e, u8* p, void* cameraPos, void* modelPos);
struct ElectronPoint;
extern void fn_8013A1D4(u8* entry, u8* parent, struct ElectronPoint* origin, u32 depth, f32 weight);
extern u32 fn_8013A49C(void* ptr);
extern u32 fn_8013A520(void* ptr);
extern u32 fn_8013AA8C(void* ptr, u16 delta);
extern u32 fn_8013AD68(void* ptr);
extern u32 fn_8013AD9C(void* ptr, u32 delta);
extern u32 fn_8013B034(void* ptr);
extern u32 fn_8013B0A0(void* ptr);
extern u16 filterStart(void* ptr);
extern void fn_8013B268(void* ptr, u8* color);
extern u32 fn_8013B504(void* ptr);
extern u32 fn_8013B558(void* ptr);
extern u16 surfEffectStart(void* ptr);
extern u32 fn_8013B85C(void* ptr, u32 delta);
extern void fn_8013BA98(void* ptr);
extern void fn_8013BC10(void* ptr, f32 t);
extern void fn_8013BE04(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale);
extern u32 fn_8013C074(void* ptr, void* arg);
extern u32 fn_8013C614(void* ptr);
extern u32 fn_8013C670(void* ptr);
extern u16 seaEffectStart(void* ptr);
extern u32 fn_8013CA48(void* ptr, u32 delta);
extern void fn_8013CBF0(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale);
extern u32 fn_8013CE58(void* inner, void* ptr);
extern u32 fn_8013D0A8(void* ptr, void* arg);
extern u32 fn_8013D730(void* ptr);
extern u32 fn_8013D7CC(void* ptr);
extern u32 envMapEffectInit(void* ptr);
extern u32 envMapEffectStart(void* ptr);
extern void fn_800D4604(u32 mode);
extern u32 fn_8013D984(void* ptr, u32 delta);
extern u32 fn_8013DC94(void* ptr);
extern u32 fn_8013DD10(void* ptr);
extern u32 fn_8013DD7C(void* ptr);
extern u32 blurEffectStart(void* ptr);
extern u32 fn_8013DE6C(void* ptr);
extern u32 fn_8013E258(void* model, void* unused, void* state);
extern u32 fn_8013E470(void* ptr, u32 delta);
extern u32 fn_8013E54C(void* ptr);
extern u32  fn_8013E5AC(u8* p);
extern u32 auraEffectStart(void* ptr);
extern u32 fn_8013E6C4(u8* ptr);
extern u32 fn_8013E8A4(void* ptr, u32 delta);
extern void fn_8013EA44(void* ptr);
extern u32 fn_8013F078(void* ptr);
extern u32 fn_8013F114(void* ptr);
extern u16 distortionEffectStart(void* ptr);
extern u32 fn_8013F410(void* ptr);
extern u32 fn_8013F80C(void* ptr, u32 delta);
extern void _distortionEffectUpdateMatrices(void* ptr);
extern u32 fn_8013FC58(void* ptr);
extern u32 fn_8013FCC4(void* ptr);
extern u32 fn_8013FD68(void* ptr);
extern u16 billboardEffectStart(void* ptr);
extern u32 fn_8013FF0C(void* ptr);

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_801380D4_8013814C)
u32 fn_801380D4(void* callbacks) {
    u32 effectId = fn_80131428(callbacks, 0x70);
    if (effectId) {
        fn_80131200(effectId,
            0,
            (GSEffectStopFunc)fn_80138630,
            (GSEffectStartFunc)fn_801386DC,
            (GSEffectStopFunc)fn_80138680,
            0,
            (GSEffectUpdateFunc)fn_801387C0,
            (GSEffectRenderFunc)_lightningRenderMain);
        fn_8013139C(effectId, 0);
    }
    return effectId;
}
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013814C)
#if 0
asm void _lightningRenderMain(void) {
#include "src/game/effect/effect_visual__lightningRenderMain.inc"
}
#else
/*
 * RULE-EXCEPTION(user-approved): _lightningRenderMain's .sdata2 pool
 * (0x8047D148-0x8047D160) as named stand-ins, defined after the function so
 * MWCC still loads them; fn_80138838's 1.0f, 0.0f and 2*pi literals are
 * redirected to these names (constant_import) — see docs/RULE_EXCEPTIONS.md
 */
extern u32 lbl_8047D148;
extern u32 lbl_8047D14C;
extern u32 lbl_8047D150;
extern u32 lbl_8047D154;
extern u32 lbl_8047D158;

/*
 * RULE-EXCEPTION(user-approved): local pragma; with named pool loads the
 * front end would otherwise merge the two reads of 1.0f into an early
 * temporary and colour the clamp differently — see docs/RULE_EXCEPTIONS.md
 */
#pragma push
#pragma opt_common_subs off
u32 _lightningRenderMain(void* ptr) {
    u8* point;
    u32 j;
    u32 segments;
    u16 i;
    u16 count;
    u8* entry;
    void* model;
    u8* p;
    void* camera;
    f32 modelPos[3];
    f32 cameraPos[3];
    f32 perspective;
    f32 unused;
    f32 distance;
    f32 scale;
    f32 width;
    f32 outer;
    f32 grow;
    f32 matrix[12];

    if (ptr != NULL) {
        p = ptr;
        count = *(u16*)(p + 0x8);
        model = GSresGetResource(*(u16*)(p + 0xA), *(u16*)(p + 0xC));
        if (model == NULL) {
            return 0;
        }

        GSmodelGetPosition(model, modelPos);
        _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
        camera = GScameraGetActiveCamera();
        GScameraGetPosition(camera, cameraPos);
        GScameraGetPerspective(camera, &perspective, &unused, &unused, &unused);
        distance = GSvecDistance(cameraPos, modelPos);
        gfxSetBlendMode(1, 6, 1);
        gfxSetUpdates(1, 1, 1);
        gfxSetZMode(1, 1, 1);
        gfxSetCullMode(0);
        scale = *(f32*)&lbl_8047D148 * vecMag(p + 0x28) / (distance * perspective);
        if (scale > *(f32*)&lbl_8047D14C) {
            scale = *(f32*)&lbl_8047D14C;
        }
        fn_800B9404((s32)(*(f32*)&lbl_8047D150 * scale), 5);
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetVtxDesc(*(void**)(p + 0x14));
        fn_800E05C0(matrix, modelPos[0], modelPos[1], modelPos[2]);
        fn_800D7F14(matrix);
        fn_800E048C(matrix, *(f32*)(p + 0x28), *(f32*)(p + 0x2C), *(f32*)(p + 0x30));
        fn_800D7F14(matrix);
        gfxSetPrimitive(2);

        entry = *(u8**)(p + 0x4);
        for (i = 0; i < count; i++, entry += 0x97C) {
            if (*(u32*)(entry + 0x978) != 0) {
                gfxBegin((u16)(*(u32*)(entry + 0x978) + 1));
                gfxVtxPos(*(f32*)(entry + 0x960), *(f32*)(entry + 0x964), *(f32*)(entry + 0x968));
                fn_800D5CB8(0, p[0x20], p[0x21], p[0x22], p[0x23]);
                segments = *(u32*)(entry + 0x978);
                point = entry;
                for (j = 0; j < segments; j++, point += 0x18) {
                    gfxVtxPos(*(f32*)(point + 0), *(f32*)(point + 4), *(f32*)(point + 8));
                    fn_800D5CB8(0, p[0x20], p[0x21], p[0x22], p[0x23]);
                }
                gfxEnd();
            }
        }

        if (*(void**)(p + 0x1C) != NULL) {
            gfxSetZMode(1, 2, 1);
            gfxSetUpdates(1, 1, 0);
            gfxSetFlags(3);
            gfxClearFlags(4);
            gfxSetVtxDesc(*(void**)(p + 0x18));
            gfxLoadTexture(0, *(void**)(p + 0x1C));
            gfxSetPrimitive(6);
            gfxBegin((u16)(count << 3));
            entry = *(u8**)(p + 4);
            grow = *(f32*)&lbl_8047D158;
            for (i = 0; i < count; i++, entry += 0x97C) {
                width = *(f32*)(p + 0x6C) * fn_800E0BA0() + *(f32*)(p + 0x68);
            gfxVtxPos(*(f32*)(entry + 0x960) - width, *(f32*)(entry + 0x964),
                        *(f32*)(entry + 0x968) - width);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D154, *(f32*)&lbl_8047D154);
            gfxVtxPos(*(f32*)(entry + 0x960) + width, *(f32*)(entry + 0x964),
                        *(f32*)(entry + 0x968) - width);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D154, *(f32*)&lbl_8047D158);
            gfxVtxPos(*(f32*)(entry + 0x960) + width, *(f32*)(entry + 0x964),
                        *(f32*)(entry + 0x968) + width);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D158, *(f32*)&lbl_8047D158);
            gfxVtxPos(*(f32*)(entry + 0x960) - width, *(f32*)(entry + 0x964),
                        *(f32*)(entry + 0x968) + width);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D158, *(f32*)&lbl_8047D154);

                outer = width * grow;
            gfxVtxPos(*(f32*)(entry + 0x96C) - outer, *(f32*)(entry + 0x970),
                        *(f32*)(entry + 0x974) - outer);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D154, *(f32*)&lbl_8047D154);
            gfxVtxPos(*(f32*)(entry + 0x96C) + outer, *(f32*)(entry + 0x970),
                        *(f32*)(entry + 0x974) - outer);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D154, *(f32*)&lbl_8047D158);
            gfxVtxPos(*(f32*)(entry + 0x96C) + outer, *(f32*)(entry + 0x970),
                        *(f32*)(entry + 0x974) + outer);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D158, *(f32*)&lbl_8047D158);
            gfxVtxPos(*(f32*)(entry + 0x96C) - outer, *(f32*)(entry + 0x970),
                        *(f32*)(entry + 0x974) + outer);
            fn_800D5CB8(0, p[0x24], p[0x25], p[0x26], p[0x27]);
            gfxVtxTexCoord(0, *(f32*)&lbl_8047D158, *(f32*)&lbl_8047D154);
            }
            gfxEnd();
        }
        fn_800D7E5C();
        fn_800D7E5C();
        return 1;
    }
    return 0;
}
#pragma pop

__declspec(section ".sdata2") u32 lbl_8047D148 = 0x42800000; /* 64.0f */
__declspec(section ".sdata2") u32 lbl_8047D14C = 0x3F800000; /* 1.0f */
__declspec(section ".sdata2") u32 lbl_8047D150 = 0x437F0000; /* 255.0f */
#pragma push
#pragma explicit_zero_data on
__declspec(section ".sdata2") u32 lbl_8047D154 = 0x00000000; /* 0.0f */
#pragma pop
__declspec(section ".sdata2") u32 lbl_8047D158 = 0x40000000; /* 2.0f */
/* fn_80138838's 2*pi: an .sdata2 object always starts 8-aligned, so the
   0x8047D15C word has to come from this object. */
__declspec(section ".sdata2") u32 lbl_8047D15C = 0x40C90FDB; /* 6.2831855f */
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80138630_801386DC)
#if 0
asm void fn_80138630(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138630.inc"
}
#else
u32 fn_80138630(void* ptr) {
    if (ptr) {
        GXDrawDone();
        fn_800B856C();
        if (*(void**)((u8*)ptr + 0x1c) != NULL) {
            GStextureFree(*(void**)((u8*)ptr + 0x1c));
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
        GXDrawDone();
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_801386DC)
extern u8 lbl_80272B40[];
#if 0
asm u32 fn_801386DC(void* ptr) {
#include "src/game/effect/effect_visual_fn_801386DC.inc"
}
#else
u32 fn_801386DC(void* ptr) {
    u8* p;
    u16 handle;

    if (ptr != NULL) {
        p = ptr;
        if (GSresGetResource(*(u16*)(p + 0xA), *(u16*)(p + 0xC)) == NULL) {
            GSlogWrite("lightningStartEffect: Could not start lightning effect - invalid model: group %d, model %d.\n",
                       *(u16*)(p + 0xA), *(u16*)(p + 0xC));
            return 0;
        }

        handle = _toolentryAlloc__FUl(*(u16*)(p + 0x8) * 0x97C);
        *(u16*)p = handle;
        if (handle == 0) {
            GSlogWrite("lightningStartEffect: Could not start lightning effect - not enough memory!\n");
            return 0;
        }

        *(void**)(p + 4) = fn_800E27B0(handle);
        memset(*(void**)(p + 4), 0, *(u16*)(p + 0x8) * 0x97C);
        fn_80138838(p, 1);
        *(u16*)(p + 0x10) = 0;
        return 1;
    }

    GSlogWrite("lightningStartEffect: Could not start lightning effect!\n");
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_801387C0_80138838)
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || defined(EFFECT_VISUAL_EXACT_80138838)
extern f32 fn_800E0BE4(void);
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void GSvecCopy(void* dst, const void* src);
extern void fn_800DFFCC(void* dst, void* src, const void* value);
extern void fn_800E0718(void*, void*, f32);
extern void GSvecTransformQuat(void* dst, void* quat, void* src);
extern void GSmtxMakeYRotation(void* mtx, f32 angle);
extern void GSvecTransform(void* dst, void* matrix, void* vec);
extern void vecScale(void* dst, void* src, f32 scale);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
#if 0
asm void fn_80138838(void* ptr, u32 b) {
#include "src/game/effect/effect_visual_fn_80138838.inc"
}
#else
void fn_80138838(void* ptr, u32 b) {
    u8* p;
    u8* current;
    u8* entry;
    u16 i;
    u16 j;
    u16 count;
    void* previous;
    f32 rotation[12];
    f32 direction[3];
    f32 noise[3];
    f32 base[3];
    f32 tangent[3];
    f32 step[4];

    p = ptr;
    entry = *(u8**)(p + 0x4);
    count = *(u16*)(p + 0x8);
    for (i = 0; i < count; i++, entry += 0x97C) {
        if (b != 0) {
            f32 angle;
            f32 radius;

            radius = *(f32*)(p + 0x58) * fn_800E0BA0() + *(f32*)(p + 0x54);
            angle = 6.2831855f * fn_800E0BE4();
            *(f32*)(entry + 0x960) = radius * (f32)cos(angle);
            *(f32*)(entry + 0x964) = 0.0f;
            *(f32*)(entry + 0x968) = radius * (f32)sin(angle);
        }

        fn_800E0060(base, entry + 0x960);
        set__5GSvecFfff(direction, base[0], 1.0f, base[2]);
        fn_800E0060(direction, direction);
        *(u32*)(entry + 0x978) = 0;
        current = entry;
        previous = entry + 0x960;

        for (j = 0; j < 100; j++, current += 0x18) {
            GSvecCopy(tangent, direction);
            tangent[1] = 0.0f;
            fn_800E0060(tangent, tangent);
            fn_800DFFCC(tangent, direction, tangent);
            fn_800E0060(tangent, tangent);

            fn_800E0718(step, tangent, *(f32*)(p + 0x4C) * fn_800E0BA0());
            GSvecTransformQuat(direction, step, direction);
            GSmtxMakeYRotation(rotation, *(f32*)(p + 0x50) * fn_800E0BA0());
            GSvecTransform(direction, rotation, direction);

            if (b != 0) {
                GSvecCopy(current + 0xC, direction);
            } else {
                vecScale(current + 0xC, current + 0xC,
                            1.0f - *(f32*)(p + 0x64));
                vecScale(direction, direction, *(f32*)(p + 0x64));
                GSvecAdd(current + 0xC, direction, current + 0xC);
                fn_800E0060(current + 0xC, current + 0xC);
            }

            vecScale(direction, current + 0xC,
                        *(f32*)(p + 0x60) * fn_800E0BA0() + *(f32*)(p + 0x5C));
            set__5GSvecFfff(noise, *(f32*)(p + 0x40) * fn_800E0BA0(),
                            *(f32*)(p + 0x44) * fn_800E0BA0(),
                            *(f32*)(p + 0x48) * fn_800E0BA0());
            GSvecAdd(noise, noise, p + 0x34);
            GSvecAdd(direction, direction, noise);
            GSvecAdd(current, previous, direction);
            GSvecCopy(direction, current + 0xC);
            *(u32*)(entry + 0x978) += 1;

            if (*(f32*)(current + 0x4) < 0.0f) {
                GSvecCopy(entry + 0x96C, previous);
                break;
            }

            previous = current;
        }
    }
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80138B00_80138BBC)
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80138BBC_80138DE4)
extern void GSmodelSetVisibility(void* a, u32 b);
extern void GSmodelFree(void* entry);
extern u32 fn_800EE0E8(void* entry);
extern s32 GSpartGetMaterialCount(void* outerP);
extern void GSmaterialSetAlpha(void* obj, f32 val);
extern void fn_800DF608(void* handle);
extern void GSpartFree(void* handle);
extern u32 lbl_8047D160;
#if 0
asm u32 fn_80138BBC(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138BBC.inc"
}
#else
u32 fn_80138BBC(void* ptr) {
    u32 j;
    u32 k;
    void* obj;
    u8* entry;
    void* part;
    u32 modelCount;
    u32 entryCount;
    void* model;
    u8* p;
    u16 handle;
    u32 i;
    u32 partCount;

    if (ptr != NULL) {
        p = ptr;
        entryCount = *(u16*)(p + 0x38);
        entry = *(u8**)p;
        handle = *(u16*)(p + 0x4);
        for (i = 0; i < entryCount; i++, entry += 0x5C) {
            if (*(void**)(entry + 0x58) != NULL) {
                GSmodelSetVisibility(*(void**)(entry + 0x58), 0);
                GSmodelFree(*(void**)(entry + 0x58));
                *(void**)(entry + 0x58) = NULL;
            }
        }

        model = *(void**)(p + 0x8);
        modelCount = fn_800EE0E8(model);
        for (k = 0; k < modelCount; k++) {
            part = GSmodelGetPart(model, k);
            if (part != NULL) {
                partCount = GSpartGetMaterialCount(part);
                for (j = 0; j < partCount; j++) {
                    obj = GSpartGetMaterial(part, j);
                    if (obj != NULL) {
                        GSmaterialSetAlpha(obj, *(f32*)&lbl_8047D160);
                        fn_800DF608(obj);
                    }
                }
                GSpartFree(part);
            }
        }

        if (handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
    return 1;
}
#endif
extern u8 lbl_80272C30[];
extern u8 lbl_80272C90[];
#if 0
asm u32 fn_80138CCC(void* ptr) {
#include "src/game/effect/effect_visual_fn_80138CCC.inc"
}
#else
u32 fn_80138CCC(void* ptr) {
    u8* entry;
    u8* p;
    u16 handle;
    u32 size;
    u32 i;
    u32 count;

    if (ptr == NULL) {
        goto allocation_error;
    }

    p = ptr;
    count = *(u16*)(p + 0x38);
    if (count > 0x20) {
        count = 0x20;
        *(u16*)(p + 0x38) = count;
    }

    *(void**)(p + 0x8) = GSresGetResource(*(u16*)(p + 0x40), *(u16*)(p + 0x44));
    if (*(void**)(p + 0x8) == NULL) {
        goto resource_error;
    }

    size = count * 0x5C;
    handle = _toolentryAlloc__FUl(size);
    *(u16*)(p + 0x4) = handle;
    if (handle == 0) {
        goto allocation_error;
    }

    entry = *(u8**)p = fn_800E27B0(handle);
    memset(*(void**)p, 0, size);
    *(u16*)(p + 0x3C) = 0;
    for (i = 0; i < count; i++, entry += 0x5C) {
        _leaffxGenerateLeafData(p, entry);
    }
    return 1;

resource_error:
    GSlogWrite((const char*)lbl_80272C30, *(u16*)(p + 0x40), *(u16*)(p + 0x44));
    return 0;

allocation_error:
    GSlogWrite((const char*)lbl_80272C90);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80138DE4)
extern void fn_800E06B8();
extern void fn_800E040C();
extern void fn_800E02C4(void* matrix, f32 x, f32 y, f32 z);
extern void fn_800E03B4();
extern void GSmodelSetMatrix();
extern u32 lbl_8047D170;
extern u32 lbl_8047D164;
extern u32 lbl_8047D178;
extern u32 lbl_8047D160;
extern const f32 lbl_8047D168;
#if 0
asm void fn_80138DE4(void) {
#include "src/game/effect/effect_visual_fn_80138DE4.inc"
}
#else
u32 fn_80138DE4(void* ptr, u32 delta) {
    u8* p;
    u32 j;
    u32 i;
    u32 modelCount;
    u16 fadeStart;
    void* obj;
    u32 partCount;
    u16 entryCount;
    void* part;
    u16 fadeEnd;
    u16 next;
    void* model;
    u8* entry;
    void* leafModel;
    u32 k;
    f32 deltaScale;
    f32 alpha;
    f32 scale;
    f32 position[3];
    f32 rotation[4];
    f32 matrix[12];

    if (ptr != NULL) {
        p = ptr;
        fadeEnd = *(u16*)(p + 0x3E);
        if (*(u16*)(p + 0x3C) < fadeEnd) {
            deltaScale = *(f32*)(p + 0x2C) * (f32)delta;
            next = *(u16*)(p + 0x3C) + delta;
            fadeStart = *(u16*)(p + 0x3A);
            entryCount = *(u16*)(p + 0x38);
            entry = *(u8**)p;
            *(u16*)(p + 0x3C) = next;

            if (next < fadeStart) {
                deltaScale *= 2.0f - ((f32)next / (f32)fadeStart);
            } else {
                model = GSresGetResource(*(u16*)(p + 0x40), *(u16*)(p + 0x44));
                alpha = 1.0f - ((f32)(next - fadeStart) / (f32)(fadeEnd - fadeStart));
                modelCount = fn_800EE0E8(model);
                for (i = 0; i < modelCount; i++) {
                    part = GSmodelGetPart(model, i);
                    if (part != NULL) {
                        partCount = GSpartGetMaterialCount(part);
                        for (j = 0; j < partCount; j++) {
                            obj = GSpartGetMaterial(part, j);
                            if (obj != NULL) {
                                GSmaterialSetAlpha(obj, alpha);
                                fn_800DF608(obj);
                            }
                        }
                        GSpartFree(part);
                    }
                }
            }

            for (k = 0; k < entryCount; k++, entry += 0x5C) {
                if (*(void**)(entry + 0x58) != NULL) {
                    *(f32*)(entry + 0x54) += deltaScale;
                    if (*(f32*)(entry + 0x54) >= 1.0f) {
                        fn_80139074(p, entry);
                    }

                    GSbezierCalculateVector(position, entry, *(f32*)(entry + 0x54));
                    if (position[1] < 0.0f) {
                        GSmodelSetVisibility(*(void**)(entry + 0x58), 0);
                        GSmodelFree(*(void**)(entry + 0x58));
                        *(void**)(entry + 0x58) = NULL;
                    } else {
                        leafModel = *(void**)(entry + 0x58);
                        scale = *(f32*)(entry + 0x50);
                        fn_800E06B8(rotation, entry + 0x30, entry + 0x40, *(f32*)(entry + 0x54));
                        fn_800E040C(matrix, rotation);
                        fn_800E02C4(matrix, scale, scale, scale);
                        fn_800E03B4(matrix, position);
                        GSmodelSetMatrix(leafModel, matrix);
                    }
                }
            }
            return 1;
        }
    }
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80139074_80139820)
extern f32 fn_800E0BE4(void);
extern void GSvecCopy(void* dst, const void* src);
extern void fn_800E0718(void*, void*, f32);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
extern void fn_800E040C();
extern void fn_800E02C4(void* matrix, f32 x, f32 y, f32 z);
extern void fn_800E03B4();
extern void GSmodelSetMatrix();
/* Quaternion copy, dst = src (src/game/gs_math_quat.c). */
extern void fn_800E076C(void* dst, void* src);
extern u32 lbl_8047D180;
extern u32 lbl_8047D184;
#if 0
asm void fn_80139074(void) {
#include "src/game/effect/effect_visual_fn_80139074.inc"
}
#else
void fn_80139074(void* entry, u8* leaf) {
    u8* config = entry;
    f32* vec;
    f32 direction[3];
    f32 farBase;
    f32 nearBase;
    f32 farRange;
    f32 nearRange;
    /* One set of locals per generated vector (0: the leaf + 0x40 axis and
       the leaf + 0x18 distance, 1: the leaf + 0x0C point, 2: the leaf + 0x24
       point); this declaration order gives retail's float register
       allocation. */
    f32 pitch0;
    f32 yaw0;
    f32 sinPitch0;
    f32 sinYaw0;
    f32 cosPitch0;
    f32 sinPitch2;
    f32 sinYaw2;
    f32 pitch2;
    f32 cosPitch2;
    f32 distance2;
    f32 yaw2;
    f32 sinPitch1;
    f32 sinYaw1;
    f32 pitch1;
    f32 cosPitch1;
    f32 distance1;
    f32 yaw1;
    f32 cosYaw0;
    f32 cosYaw1;
    f32 cosYaw2;
    f32 distance0;

    farBase = *(f32*)(config + 0x20);
    nearBase = *(f32*)(config + 0x18);
    farRange = *(f32*)(config + 0x24);
    nearRange = *(f32*)(config + 0x1C);

    *(f32*)(leaf + 0x54) -= 1.0f;
    fn_800E076C(leaf + 0x30, leaf + 0x40);

    yaw0 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch0 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch0 = sin(pitch0);
    sinYaw0 = sin(yaw0);
    cosPitch0 = cos(pitch0);
    cosYaw0 = cos(yaw0);
    set__5GSvecFfff(direction, sinPitch0 * cosYaw0, cosPitch0, sinPitch0 * sinYaw0);
    fn_800E0718(leaf + 0x40, direction,
                *(f32*)&lbl_8047D180 * fn_800E0BE4());

    GSvecCopy(leaf, leaf + 0xC);
    vec = (f32*)(leaf + 0x18);
    GSvecCopy(vec, leaf + 0x24);
    GSvecSub(vec, vec, leaf + 0xC);
    fn_800E0060(vec, vec);
    distance0 = farRange * fn_800E0BA0() + farBase;
    distance0 *= *(f32*)(leaf + 0x50);
    vecScale(vec, vec, -distance0);
    GSvecAdd(vec, vec, leaf);

    vec = (f32*)(leaf + 0xC);
    yaw1 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch1 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch1 = sin(pitch1);
    distance1 = nearRange * fn_800E0BA0() + nearBase;
    distance1 *= *(f32*)(leaf + 0x50);
    sinYaw1 = sin(yaw1);
    cosPitch1 = cos(pitch1);
    cosYaw1 = cos(yaw1);
    set__5GSvecFfff(vec, sinPitch1 * cosYaw1, cosPitch1, sinPitch1 * sinYaw1);
    vecScale(vec, vec, distance1);
    vec[1] += *(f32*)(config + 0x28);
    GSvecAdd(vec, vec, leaf);

    vec = (f32*)(leaf + 0x24);
    yaw2 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch2 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch2 = sin(pitch2);
    distance2 = farRange * fn_800E0BA0() + farBase;
    distance2 *= *(f32*)(leaf + 0x50);
    sinYaw2 = sin(yaw2);
    cosPitch2 = cos(pitch2);
    cosYaw2 = cos(yaw2);
    set__5GSvecFfff(vec, sinPitch2 * cosYaw2, cosPitch2, sinPitch2 * sinYaw2);
    vecScale(vec, vec, distance2);
    GSvecAdd(vec, vec, leaf + 0xC);
}
#endif
extern void* GSmodelClone(void* model);
extern void GSpartGetTransform(void* obj, void* a, void* b, void* c);
extern u8 lbl_80272CC4[];
/* RULE-EXCEPTION(user-approved): extern-named stand-in for the 0.0f shared
   with fn_80138DE4 (0x8047D168, in effect_visual_sdata2_8047D160) — see
   docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D168;
extern u32 lbl_8047D180;
extern u32 lbl_8047D184;
extern u32 lbl_8047D188;
#if 0
asm void _leaffxGenerateLeafData(void* ptr, void* entry) {
#include "src/game/effect/effect_visual__leaffxGenerateLeafData.inc"
}
#else
u8 _leaffxGenerateLeafData(void* ptr, void* entry) {
    void* source;
    f32* vec;
    void* part;
    u8* p;
    u8* e;
    f32 position[3];
    f32 vecA[3];
    f32 vecB[3];
    f32 matrix[12];
    f32 radiusBase;
    f32 heightBase;
    f32 radiusRange;
    f32 heightRange;
    /* One set of angle locals per generated vector (0: e+0x30 axis, 1: e+0x40
       axis, 2-4: the e+0x18, e+0x0C and e+0x24 points); this declaration
       order gives retail's float register allocation. */
    f32 pitch0;
    f32 yaw0;
    f32 sinPitch0;
    f32 sinYaw0;
    f32 cosPitch0;
    f32 sinPitch1;
    f32 sinYaw1;
    f32 pitch1;
    f32 yaw1;
    f32 cosPitch1;
    f32 sinPitch2;
    f32 sinYaw2;
    f32 pitch2;
    f32 cosPitch2;
    f32 radius2;
    f32 sinPitch3;
    f32 sinYaw3;
    f32 pitch3;
    f32 cosPitch3;
    f32 radius3;
    f32 sinPitch4;
    f32 sinYaw4;
    f32 cosPitch4;
    f32 yaw2;
    f32 yaw3;
    f32 pitch4;
    f32 radius4;
    f32 yaw4;
    f32 cosYaw0;
    f32 cosYaw1;
    f32 cosYaw2;
    f32 cosYaw3;
    f32 cosYaw4;

    p = ptr;
    e = entry;
    radiusBase = *(f32*)(p + 0x20);
    heightBase = *(f32*)(p + 0x18);
    radiusRange = *(f32*)(p + 0x24);
    heightRange = *(f32*)(p + 0x1C);
    source = GSresGetResource(*(u16*)(p + 0x46), *(u16*)(p + 0x48));
    part = NULL;
    if (source == NULL) {
        return 0;
    }

    *(void**)(e + 0x58) = GSmodelClone(*(void**)(p + 0x08));
    if (*(void**)(e + 0x58) == NULL) {
        GSlogWrite((const char*)lbl_80272CC4);
        return 0;
    }

    GSmodelSetVisibility(*(void**)(e + 0x58), 1);
    if (*(s16*)(p + 0x4A) >= 0) {
        part = GSmodelGetPart(source, *(s16*)(p + 0x4A));
    }

    if (part != NULL) {
        GSpartGetTransform(part, position, NULL, NULL);
        GSpartFree(part);
    } else {
        GSmodelGetPosition(source, position);
    }

    *(f32*)(e + 0x50) = *(f32*)(p + 0x14) * fn_800E0BA0() + *(f32*)(p + 0x10);
    *(f32*)(e + 0x50) *= *(f32*)(p + 0x0C);
    *(f32*)(e + 0x54) = lbl_8047D168;

    yaw0 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch0 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch0 = sin(pitch0);
    sinYaw0 = sin(yaw0);
    cosPitch0 = cos(pitch0);
    cosYaw0 = cos(yaw0);
    set__5GSvecFfff(vecA, sinPitch0 * cosYaw0, cosPitch0, sinPitch0 * sinYaw0);
    /* RULE-EXCEPTION(user-approved): codegen-only cast; it keeps this e + 0x30
       from being merged with the one in the final fn_800E040C call — see
       docs/RULE_EXCEPTIONS.md */
    fn_800E0718((f32*)(e + 0x30), vecA, *(f32*)&lbl_8047D180 * fn_800E0BE4());

    yaw1 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch1 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch1 = sin(pitch1);
    sinYaw1 = sin(yaw1);
    cosPitch1 = cos(pitch1);
    cosYaw1 = cos(yaw1);
    set__5GSvecFfff(vecB, sinPitch1 * cosYaw1, cosPitch1, sinPitch1 * sinYaw1);
    fn_800E0718((u8*)e + 0x40, vecB, *(f32*)&lbl_8047D180 * fn_800E0BE4());

    GSvecCopy(e, position);

    vec = (f32*)(e + 0x18);
    radius2 = radiusRange * fn_800E0BA0() + radiusBase;
    radius2 *= *(f32*)(e + 0x50);
    yaw2 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch2 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch2 = sin(pitch2);
    sinYaw2 = sin(yaw2);
    cosPitch2 = cos(pitch2);
    cosYaw2 = cos(yaw2);
    set__5GSvecFfff(vec, sinPitch2 * cosYaw2, cosPitch2, sinPitch2 * sinYaw2);
    vecScale(vec, vec, radius2);
    GSvecAdd(vec, vec, position);

    vec = (f32*)(e + 0x0C);
    yaw3 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch3 = *(f32*)&lbl_8047D188 * fn_800E0BE4();
    sinPitch3 = sin(pitch3);
    radius3 = heightRange * fn_800E0BA0() + heightBase;
    radius3 *= *(f32*)(p + 0x34) * fn_800E0BA0() + *(f32*)(p + 0x30);
    radius3 *= *(f32*)(e + 0x50);
    sinYaw3 = sin(yaw3);
    cosPitch3 = cos(pitch3);
    cosYaw3 = cos(yaw3);
    set__5GSvecFfff(vec, sinPitch3 * cosYaw3, cosPitch3, sinPitch3 * sinYaw3);
    vecScale(vec, vec, radius3);
    GSvecAdd(vec, vec, position);

    vec = (f32*)(e + 0x24);
    yaw4 = *(f32*)&lbl_8047D180 * fn_800E0BE4();
    pitch4 = *(f32*)&lbl_8047D184 * fn_800E0BE4();
    sinPitch4 = sin(pitch4);
    radius4 = radiusRange * fn_800E0BA0() + radiusBase;
    radius4 *= *(f32*)(e + 0x50);
    sinYaw4 = sin(yaw4);
    cosPitch4 = cos(pitch4);
    cosYaw4 = cos(yaw4);
    set__5GSvecFfff(vec, sinPitch4 * cosYaw4, cosPitch4, sinPitch4 * sinYaw4);
    vecScale(vec, vec, radius4);
    GSvecAdd(vec, vec, e + 0x0C);

    fn_800E040C(matrix, (u8*)e + 0x30);
    fn_800E02C4(matrix, *(f32*)(e + 0x50), *(f32*)(e + 0x50), *(f32*)(e + 0x50));
    fn_800E03B4(matrix, position);
    GSmodelSetMatrix(*(void**)(e + 0x58), matrix);
    return 1;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80139820_80139AC4)
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
        GXDrawDone();
        fn_800B856C();
        if (*(void**)((u8*)ptr + 0x58) != NULL) {
            GStextureFree(*(void**)((u8*)ptr + 0x58));
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
            GXDrawDone();
            fn_800B856C();
            fn_800E24B0(val);
            fn_800E209C(val);
        }
    }
    return 1;
}
#endif
extern u32 fn_800EE7E0(void* obj);
extern const f32 lbl_8047D190;
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
        model = GSresGetResource(*(u16*)(p + 0x4c), *(u16*)(p + 0x4e));
        if (*(void**)(p + 0x58) == NULL) {
            return 0;
        }
        _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
        if (model != NULL) {
            GSmodelGetPosition(model, pos);
            if (*(s32*)(p + 0x50) > 0) {
                part = GSmodelGetPart(model, *(u32*)(p + 0x50));
                if (part != NULL) {
                    if ((u8)fn_800EE7E0(part) != 0) {
                        GSpartGetTransform(part, pos, NULL, NULL);
                        GSpartFree(part);
                    }
                }
            }
        } else {
            set__5GSvecFfff(pos, *(f32*)&lbl_8047D190, *(f32*)&lbl_8047D190, *(f32*)&lbl_8047D190);
        }
        GScameraGetPosition(camera, cameraPos);
        gfxSetBlendMode(1, 6, 1);
        gfxSetUpdates(1, 1, 0);
        gfxSetZMode(1, 1, 1);
        gfxSetCullMode(0);
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetVtxDesc(*(void**)(p + 0x54));
        gfxLoadTexture(0, *(void**)(p + 0x58));
        gfxSetPrimitive(6);
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80139AC4_80139D10)
extern u32 lbl_8047D198;
extern u32 lbl_8047D1A0;

BOOL fn_80139AC4(void* ptr, u32 tick) {
    u8* p;
    u32 frame;
    u16 endFrame;
    u16 i;
    u32 count;
    u8* entry;

    if (ptr != NULL) {
        p = ptr;
    frame = *(u16*)(p + 0x48);
    if (*(void**)(p + 0x58) == NULL) {
        return 0;
    }

    endFrame = *(u16*)(p + 0x4A);
    if (frame < endFrame) {
    count = *(u16*)(p + 0x44);
    entry = *(u8**)p;
    for (i = 0; i < count; i++, entry += 0x12E4) {
        if (frame >= *(u16*)(entry + 0x12DC) &&
            frame < *(u16*)(entry + 0x12DE)) {
                f32 random_offset[3];
                f32 offset[3];
                f32 fade;
                u32 segment_count;
                u32 segment;
                u8* point;

                GSvecCopy(offset, p + 8);
                segment_count = *(u16*)(entry + 0x12E2);
                point = entry;
                for (segment = 0; segment < segment_count; segment++, point += 0x18) {
                    set__5GSvecFfff(random_offset,
                                    offset[0] * fn_800E0BA0(),
                                    offset[1] * fn_800E0BA0(),
                                    offset[2] * fn_800E0BA0());
                    GSvecAdd(point, point, random_offset);
                }

                if (*(u16*)(p + 0x48) >
                    ((*(u16*)(entry + 0x12DE) +
                      *(u16*)(entry + 0x12DC)) >> 1)) {
                    fade = (f32)(*(u16*)(entry + 0x12DE) -
                                 *(u16*)(p + 0x48)) /
                           (f32)((*(u16*)(entry + 0x12DE) -
                                  *(u16*)(entry + 0x12DC)) >> 1);
                    *(u8*)(entry + 0x12D8) = (u8)(p[0x5C] * fade);
                    *(u8*)(entry + 0x12D9) = (u8)(p[0x5D] * fade);
                    *(u8*)(entry + 0x12DA) = (u8)(p[0x5E] * fade);
                    *(u8*)(entry + 0x12DB) = (u8)(p[0x5F] * fade);
                }
                *(u16*)(entry + 0x12E0) = 1;
        } else {
            *(u16*)(entry + 0x12E0) = 0;
        }
    }

    *(u16*)(p + 0x48) = *(u16*)(p + 0x48) + tick;
        return 1;
    }
    }
    return 0;
}
extern u8 lbl_80272D08[];
extern const f32 lbl_8047D190;
extern u8 lbl_80272D54[];
#if 0
asm u32 fn_80139D10(void* ptr) {
#include "src/game/effect/effect_visual_fn_80139D10.inc"
}
#else
u32 fn_80139D10(void* ptr) {
    u16 handle;
    u32 size;
    u8* entry;
    u8* p;
    u16 span;
    u16 step;
    u16 count;
    u16 i;

    if (ptr != NULL) {
        p = ptr;
        count = *(u16*)(p + 0x44);
        if (*(void**)(p + 0x58) == NULL) {
            return 0;
        }

        if (count > 1) {
            span = *(u16*)(p + 0x4A) / (count - 1);
            step = (*(u16*)(p + 0x4A) - span) / count;
        } else {
            step = 0;
            span = *(u16*)(p + 0x4A);
        }

        size = count * 0x12E4;
        handle = _toolentryAlloc__FUl(size);
        if (handle == 0) {
            GSlogWrite((const char*)lbl_80272D08);
            return 0;
        }

        *(u16*)(p + 0x4) = handle;
        entry = *(u8**)p = fn_800E27B0(handle);
        memset(entry, 0, size);
        for (i = 0; i < count; i++, entry += 0x12E4) {
            *(u16*)(entry + 0x12E2) = 0;
            set__5GSvecFfff(entry + 0x12C0, 0.0f, *(f32*)(p + 0x38), 0.0f);
            fn_8013A1D4(entry, p, (struct ElectronPoint*)(entry + 0x12C0), 0, *(f32*)(p + 0x3C));
            entry[0x12D8] = p[0x5C];
            entry[0x12D9] = p[0x5D];
            entry[0x12DA] = p[0x5E];
            entry[0x12DB] = p[0x5F];
            *(u16*)(entry + 0x12DC) = i * step;
            *(u16*)(entry + 0x12DE) = *(u16*)(entry + 0x12DC) + span;
        }
        *(u16*)(p + 0x48) = 0;
        return 1;
    }

    GSlogWrite((const char*)lbl_80272D54);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || defined(EFFECT_VISUAL_EXACT_80139E80)
extern void fn_800E0108(void);
extern void fn_800E0560(void);
extern void fn_800E042C(void);
extern void vecNormalize(void);
extern u32 lbl_8047D1A0;
extern const f32 lbl_8047D1A8;
extern const f32 lbl_8047D190;
extern const f32 lbl_8047D1AC;
extern const f32 lbl_8047D1B0;
#if 0
asm void fn_80139E80(void* entry, void* parent, void* cameraPos, void* modelPos) {
#include "src/game/effect/effect_visual_fn_80139E80.inc"
}
#else
void fn_80139E80(u8* e, u8* p, void* cameraPos, void* modelPos) {
    extern void fn_800D7E5C(void);
    extern void fn_800D7F14(void*);
    extern void fn_800DFFCC(void*, void*, void*);
    extern void fn_800E0060(f32*, f32*);
    extern f32 vecMag(f32*);
    extern void fn_800E0108(f32*, f32*, f32*, u32, u32);
    extern void fn_800E0560(f32 (*)[4], f32*);
    extern void fn_800E042C(f32 (*)[4], f32*);
    extern void vecNormalize(f32*, f32*);
    /* RULE-EXCEPTION(user-approved): store-only carrier for register allocation — see docs/RULE_EXCEPTIONS.md */
    short spanEnd;
    u16 i;
    u16 count;
    u8* pointEntry;
    f32 vertex[3];
    f32 normal[3];
    f32 direction[3];
    f32 cameraVector[3];
    f32 interpolated[3];
    f32 matrix[3][4];
    f32 threshold;
    u16 current;
    u16 start;
    u16 halfSpan;
    u16 visible;

    start = *(u16*)(e + 0x12DC);
    current = *(u16*)(p + 0x48) - start;
    halfSpan = (u16)(spanEnd = *(u16*)(e + 0x12DE) - start) >> 1;
    fn_800E0108(interpolated, (f32*)(e + 0x12CC), (f32*)(p + 0x20), *(u16*)(p + 0x48) - start,
                start);
    GSvecSub(interpolated, modelPos, interpolated);
    if (current < halfSpan) {
        threshold = *(f32*)(p + 0x38);
        threshold *= 1.0f - (f32)current / (f32)halfSpan;
    } else {
        threshold = 0.0f;
    }

    count = *(u16*)(e + 0x12E2);
    visible = 0;
    pointEntry = e;
    for (i = 0; i < count; i++, pointEntry += 0x18) {
        if (*(f32*)(*(u8**)(pointEntry + 0xC) + 4) > threshold) {
            visible++;
        }
    }

    fn_800E0560(matrix, interpolated);
    fn_800D7F14(matrix);
    fn_800E042C(matrix, (f32*)(p + 0x20));
    fn_800D7F14(matrix);
    gfxBegin((visible * 4) & 0xFFFC);

    count = *(u16*)(e + 0x12E2);
    pointEntry = e;
    for (i = 0; i < count; i++, pointEntry += 0x18) {
        if (*(f32*)(*(u8**)(pointEntry + 0xC) + 4) > threshold) {
        GSvecSub(direction, pointEntry, *(u8**)(pointEntry + 0xC));
        fn_800E0060(direction, direction);
        GSvecAdd(cameraVector, pointEntry, *(u8**)(pointEntry + 0xC));
        vecScale(cameraVector, cameraVector, 0.5f);
        GSvecSub(cameraVector, cameraPos, cameraVector);
        fn_800E0060(cameraVector, cameraVector);
        fn_800DFFCC(normal, cameraVector, direction);
        if (vecMag(normal) > 0.001f) {
            vecNormalize(normal, normal);
        }

        vecScale(direction, normal,
                    0.5f * *(f32*)(pointEntry + 0x10));
        GSvecAdd(vertex, *(u8**)(pointEntry + 0xC), direction);
        gfxVtxPos(vertex[0], vertex[1], vertex[2]);
        fn_800D5CB8(0, e[0x12D8], e[0x12D9], e[0x12DA], e[0x12DB]);
        gfxVtxTexCoord(0, 0.0f, 0.0f);
        GSvecSub(vertex, *(u8**)(pointEntry + 0xC), direction);
        gfxVtxPos(vertex[0], vertex[1], vertex[2]);
        fn_800D5CB8(0, e[0x12D8], e[0x12D9], e[0x12DA], e[0x12DB]);
        gfxVtxTexCoord(0, 1.0f, 0.0f);

        vecScale(direction, normal,
                    0.5f * *(f32*)(pointEntry + 0x14));
        GSvecSub(vertex, pointEntry, direction);
        gfxVtxPos(vertex[0], vertex[1], vertex[2]);
        fn_800D5CB8(0, e[0x12D8], e[0x12D9], e[0x12DA], e[0x12DB]);
        gfxVtxTexCoord(0, 1.0f, 1.0f);
        GSvecAdd(vertex, pointEntry, direction);
        gfxVtxPos(vertex[0], vertex[1], vertex[2]);
        fn_800D5CB8(0, e[0x12D8], e[0x12D9], e[0x12DA], e[0x12DB]);
        gfxVtxTexCoord(0, 0.0f, 1.0f);
        }
    }
    gfxEnd();
    fn_800D7E5C();
    fn_800D7E5C();
}
#endif
#endif
#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || defined(EFFECT_VISUAL_EXACT_8013A1D4)
#if defined(EFFECT_VISUAL_EXACT_8013A1D4)
extern f32 fn_800E0BE4(void);
extern void GSvecCopy(void* dst, const void* src);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
#endif
extern const f32 lbl_8047D1B4;
extern const f32 lbl_8047D190;
extern u32 lbl_8047D1A0;
extern const f32 lbl_8047D1A8;
extern const f32 lbl_8047D1AC;
typedef struct ElectronPoint {
    f32 x;
    f32 y;
    f32 z;
    struct ElectronPoint* prev;
    f32 startWeight;
    f32 endWeight;
} ElectronPoint;

void fn_8013A1D4(u8* entry, u8* parent, ElectronPoint* origin, u32 depth, f32 weight) {
    ElectronPoint* first;
    u32 i;
    u32 j;
    u32 startCount;
    ElectronPoint* point;
    ElectronPoint* previous;
    ElectronPoint* current;
    u32 count;
    f32 spawnChance;
    f32 branchChance;
    f32 stopHeight;
    f32 branchDecay;
    f32 spread[3];

    previous = origin;
    GSvecCopy(spread, parent + 0x14);
    startCount = *(u16*)(entry + 0x12E2);
    if (depth != 0) {
        stopHeight = 0.25f * origin->y;
    } else {
        stopHeight = 0.0f;
    }

    spawnChance = 1.0f / (f32)(depth + 1);
    first = (ElectronPoint*)(entry + startCount * sizeof(ElectronPoint));
    current = first;
    for (i = startCount; i < 0xC8; i++, current++) {
        current->prev = previous;
        point = previous;
        previous = current;

        set__5GSvecFfff(
            current,
            spread[0] * fn_800E0BA0() + *(f32*)(parent + 0x2C),
            spread[1] * fn_800E0BA0() + *(f32*)(parent + 0x30),
            spread[2] * fn_800E0BA0() + *(f32*)(parent + 0x34));
        GSvecAdd(current, current, point);

        (*(u16*)(entry + 0x12E2))++;
        if (current->y < stopHeight || spawnChance < fn_800E0BE4()) {
            if (depth == 0) {
                GSvecCopy(entry + 0x12CC, current);
            }
            break;
        }
    }

    if (depth >= *(u16*)(parent + 0x46)) {
        return;
    }

    branchChance = *(f32*)(parent + 0x40) *
                  (f32)(*(u16*)(parent + 0x46) - depth) /
                  (f32)*(u16*)(parent + 0x46);
    count = *(u16*)(entry + 0x12E2);
    if (depth != 0) {
        branchDecay = weight / (f32)(count - startCount);
    } else {
        branchDecay = 0.0f;
    }

    for (j = startCount; j < count; j++, first++) {
        first->startWeight = weight;
        weight = first->endWeight = weight - branchDecay;
        if (branchChance > fn_800E0BE4()) {
            fn_8013A1D4(entry, parent, first->prev, depth + 1,
                        0.5f * weight);
        }
    }
}
#endif /* 8013A1D4 */
#if !defined(EFFECT_VISUAL_BANK_ACTIVE)
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013A42C_8013A520)
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
        res = GSresGetResource(*(u16*)((u8*)ptr + 0xc), *(u16*)((u8*)ptr + 0xe));
        if (res == NULL) {
            GSlogWrite((const char*)lbl_80272D90, *(u16*)((u8*)ptr + 0xc), *(u16*)((u8*)ptr + 0xe));
            return 0;
        }
        *(u16*)((u8*)ptr + 0x8) = 0;
        return 1;
    }
    GSlogWrite((const char*)lbl_80272DF4);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || defined(EFFECT_VISUAL_EXACT_8013A520)
#if defined(EFFECT_VISUAL_EXACT_8013A520)
extern f32 fn_800E0BE4(void);
extern void GSvecCopy(void* dst, const void* src);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
extern u32 fn_800EE7E0(void* obj);
#endif
extern void __cvt_fp2unsigned(void);
extern const f32 lbl_8047D1B8;
extern const f32 lbl_8047D1BC;
extern const f32 lbl_8047D1C0;
extern const f32 lbl_8047D1C4;
extern const f32 lbl_8047D1C8;
extern const f32 lbl_8047D1CC;
extern const f32 lbl_8047D1D0;
extern u32 lbl_8047D1D8;
#if 0
asm void fn_8013A520(void) {
#include "src/game/effect/effect_visual_fn_8013A520.inc"
}
#else
u32 fn_8013A520(void* ptr) {
    extern u32 fn_800EE0E8(void*);
    extern f64 sin(f64);
    extern f64 cos(f64);
    u32 segment;
    u32 firstIndex;
    u32 secondIndex;
    u8* p = ptr;
    f32 ctrl[4][3];
    f32 center[3];
    f32 cameraPos[3];
    f32 firstPos[3];
    f32 secondPos[3];
    f32 offset[3];
    f32 point[3];
    f32 jitter[3];
    f32 perspective;
    f32 unused;
    void* resource;
    void* first;
    void* camera;
    u32 count;
    void* second;
    u32 segments;
    /* Float locals of the function and of its three random-offset blocks
       (B1: ctrl[2] offset, B2: ctrl[3] offset, B3: per-segment jitter); this
       declaration order gives retail's float register allocation. */
    f32 cosYawB2;
    f32 step;
    f32 t;
    f32 yawB1;
    f32 pitchB1;
    f32 chance;
    f32 cosYawB1;
    f32 lengthB3;
    f32 scale;
    f32 cosPitchB1;
    f32 distance;
    f32 lengthB1;
    f32 cosPitchB2;
    f32 pitchB3;
    f32 yawB3;
    f32 sinPitchB1;
    f32 sinYawB1;
    f32 sinPitchB2;
    f32 sinYawB2;
    f32 sinPitchB3;
    f32 sinYawB3;
    f32 cosYawB3;
    f32 yawB2;
    f32 pitchB2;
    f32 radius;
    f32 lengthB2;
    f32 cosPitchB3;

    if (ptr != NULL) {
    chance = *(f32*)(p + 0x20);
    resource = GSresGetResource(*(u16*)(p + 0xC), *(u16*)(p + 0xE));
    if (resource == NULL) {
        return 0;
    }

    count = fn_800EE0E8(resource);
    GSmodelGetPosition(resource, center);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    camera = GScameraGetActiveCamera();
    GScameraGetPosition(camera, cameraPos);
    GScameraGetPerspective(camera, &perspective, &unused, &unused, &unused);
    distance = GSvecDistance(cameraPos, center);
    gfxSetBlendMode(1, 6, 1);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(1, 1, 1);
    gfxSetCullMode(0);
    scale = (64.0f * *(f32*)(p + 0x10)) /
            (distance * perspective);
    if (scale > 1.0f) {
        scale = 1.0f;
    }
    fn_800B9404((s32)(255.0f * scale), 5);
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetVtxDesc(*(void**)(p + 4));
    gfxSetPrimitive(2);

    for (firstIndex = 1; firstIndex < count; firstIndex++) {
        first = GSmodelGetPart(resource, firstIndex);
        if (first == NULL) {
            continue;
        }
        if ((u8)fn_800EE7E0(first) != 0) {
            GSpartGetTransform(first, firstPos, 0, 0);
            if (GSvecDistance(firstPos, center) > 0.1f) {
                for (secondIndex = firstIndex + 1; secondIndex < count;
                     secondIndex++) {
                    second = GSmodelGetPart(resource, secondIndex);
                    if (second == NULL) {
                        continue;
                    }
                    if ((u8)fn_800EE7E0(second) != 0 && chance > fn_800E0BE4()) {
                        GSpartGetTransform(second, secondPos, 0, 0);
                        if (GSvecDistance(firstPos, center) > 0.1f) {
                            GSvecCopy(ctrl[0], firstPos);
                            GSvecCopy(ctrl[1], secondPos);
                            radius = 0.5f *
                                     GSvecDistance(firstPos, secondPos);

                            {
                                yawB1 = 6.2831855f * fn_800E0BE4();
                                pitchB1 = 3.1415927f * fn_800E0BE4();
                                sinPitchB1 = sin(pitchB1);
                                lengthB1 = radius * fn_800E0BE4();
                                sinYawB1 = sin(yawB1);
                                cosPitchB1 = cos(pitchB1);
                                cosYawB1 = cos(yawB1);
                                set__5GSvecFfff(offset, sinPitchB1 * (lengthB1 * cosYawB1),
                                                lengthB1 * cosPitchB1,
                                                sinPitchB1 * (lengthB1 * sinYawB1));
                            }
                            GSvecAdd(ctrl[2], firstPos, offset);

                            {
                                yawB2 = 6.2831855f * fn_800E0BE4();
                                pitchB2 = 3.1415927f * fn_800E0BE4();
                                sinPitchB2 = sin(pitchB2);
                                lengthB2 = radius * fn_800E0BE4();
                                sinYawB2 = sin(yawB2);
                                cosPitchB2 = cos(pitchB2);
                                cosYawB2 = cos(yawB2);
                                set__5GSvecFfff(offset, sinPitchB2 * (lengthB2 * cosYawB2),
                                                lengthB2 * cosPitchB2,
                                                sinPitchB2 * (lengthB2 * sinYawB2));
                            }
                            GSvecAdd(ctrl[3], secondPos, offset);

                            segments = *(f32*)(p + 0x10) *
                                       (*(f32*)(p + 0x1C) * fn_800E0BA0() +
                                        *(f32*)(p + 0x18));
                            step = 1.0f / (f32)segments;
                            gfxBegin((u16)(segments + 1));
                            gfxVtxPos(ctrl[0][0], ctrl[0][1], ctrl[0][2]);
                            fn_800D5CB8(0, p[0], p[1], p[2], p[3]);
                            t = step;
                            for (segment = 1; segment < segments; segment++) {
                                GSbezierCalculateVector(point, ctrl, t);
                                {
                                    yawB3 = 6.2831855f * fn_800E0BE4();
                                    pitchB3 = 3.1415927f * fn_800E0BE4();
                                    sinPitchB3 = sin(pitchB3);
                                    lengthB3 = *(f32*)(p + 0x14) * *(f32*)(p + 0x10) *
                                             fn_800E0BA0();
                                    sinYawB3 = sin(yawB3);
                                    cosPitchB3 = cos(pitchB3);
                                    cosYawB3 = cos(yawB3);
                                    set__5GSvecFfff(jitter, sinPitchB3 * (lengthB3 * cosYawB3),
                                                    lengthB3 * cosPitchB3,
                                                    sinPitchB3 * (lengthB3 * sinYawB3));
                                }
                                GSvecAdd(point, point, jitter);
                                gfxVtxPos(point[0], point[1], point[2]);
                                fn_800D5CB8(0, p[0], p[1], p[2], p[3]);
                                t += step;
                            }
                            gfxVtxPos(ctrl[1][0], ctrl[1][1], ctrl[1][2]);
                            fn_800D5CB8(0, p[0], p[1], p[2], p[3]);
                            gfxEnd();
                        }
                    }
                    GSpartFree(second);
                }
            }
        }
        GSpartFree(first);
    }
    return 1;
    }
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013AA8C_8013AB60)
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013AB60)
extern u32 lbl_8047D1E8;
extern const f32 lbl_8047D1E0;
#if 0
asm u32 fn_8013AB60(void* ptr, u8* src, u8* dst, u32 alpha) {
#include "src/game/effect/effect_visual_fn_8013AB60.inc"
}
#else
u32 fn_8013AB60(void* ptr, u8* src, u8* dst, u32 alpha) {
    u8* p;
    u8* node;
    u8* tail;
    u16 handle;
    f32 start;
    f32 end;

    p = ptr;
    handle = _toolentryAlloc__FUl(0x14);
    if (handle != 0) {
        node = fn_800E27B0(handle);
        *(u16*)(node + 0xC) = handle;
        if (alpha == 0xFFFFFFFF) {
            start = src[0];
            end = dst[0];
            node[0] = start + 0.5f * (end - start);
            start = src[1];
            end = dst[1];
            node[1] = start + 0.5f * (end - start);
            start = src[2];
            end = dst[2];
            node[2] = start + 0.5f * (end - start);
            start = src[3];
            end = dst[3];
            node[3] = start + 0.5f * (end - start);
            node[4] = node[0];
            node[5] = node[1];
            node[6] = node[2];
            node[7] = node[3];
        } else {
            node[0] = src[0];
            node[1] = src[1];
            node[2] = src[2];
            node[3] = src[3];
            node[4] = dst[0];
            node[5] = dst[1];
            node[6] = dst[2];
            node[7] = dst[3];
        }
        *(u32*)(node + 0x8) = alpha;
        tail = *(u8**)(p + 0x50);
        if (tail != NULL) {
            while (*(void**)(tail + 0x10) != NULL) {
                tail = *(u8**)(tail + 0x10);
            }
            *(void**)(tail + 0x10) = node;
        } else {
            *(void**)(p + 0x50) = node;
        }
        *(void**)(node + 0x10) = NULL;
    }
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013AD68_8013AD9C)
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013AD9C)
extern void fn_80168408(void* filter, u8* color);
extern u32 lbl_8047D1E8;
extern const f32 lbl_8047D1F0;
extern const f32 lbl_8047D1F4;
u32 fn_8013AD9C(void* ptr, u32 delta) {
    u8* p;
    u8* node;
    u8* previous;
    u8 color[4];
    void* filter;
    f32 ratio;
    f32 start;
    f32 end;

    if (ptr == NULL) {
        goto done;
    }

    p = ptr;
    node = *(u8**)(p + 0x54);
    if (node == NULL) {
        goto done;
    }

    if (*(u32*)(node + 0x8) == 0xFFFFFFFF) {
        *(u32*)(p + 0x58) = 0;
    }
    while (*(u32*)(p + 0x58) >= *(u32*)(node + 0x8)) {
        *(u32*)(p + 0x58) -= *(u32*)(node + 0x8);
        previous = node;
        node = *(u8**)(node + 0x10);
        *(void**)(p + 0x54) = node;
        if (node == NULL) {
            if (*(u8*)(p + 0x4C) != 0) {
                fn_8013B268(p, previous + 4);
            } else {
                filter = *(void**)p;
                if (filter != NULL) {
                    fn_80168408(filter, previous + 4);
                }
            }
            return 0;
        }
        if (*(u32*)(node + 0x8) == 0xFFFFFFFF) {
            *(u32*)(p + 0x58) = 0;
            break;
        }
    }

    ratio = (f32)*(u32*)(p + 0x58) / (f32)*(u32*)(node + 0x8);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    if (ratio > 1.0f) {
        ratio = 1.0f;
    }

    start = node[0];
    end = node[4];
    color[0] = start + ratio * (end - start);
    start = node[1];
    end = node[5];
    color[1] = start + ratio * (end - start);
    start = node[2];
    end = node[6];
    color[2] = start + ratio * (end - start);
    start = node[3];
    end = node[7];
    color[3] = start + ratio * (end - start);

    if (*(u8*)(p + 0x4C) != 0) {
        fn_8013B268(p, color);
    } else {
        filter = *(void**)p;
        if (filter != NULL) {
            fn_80168408(filter, color);
        } else {
            return 0;
        }
    }
    *(u32*)(p + 0x58) += delta;
    return 1;

done:
    return 0;
}
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013B034_8013B268)
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
extern void GSmodelDisableModulation(void* texture);
extern void GSmodelResetPEdescr(void* texture);
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
        GXDrawDone();
        fn_800B856C();
        for (i = 0; i < count; i++, p += 4) {
            if (*(void**)p != NULL) {
                if (flag2 == 0) {
                    GSmodelDisableModulation(*(void**)p);
                }
                if (flag1 != 0) {
                    GSmodelResetPEdescr(*(void**)p);
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
extern void GSmodelSetPEdescr(void* texture, void* name);
extern void GSmodelEnableModulation(void* texture, void* filter);
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
    u8* ptr = (u8*)arg;

    if (ptr == NULL) {
        goto report_null;
    }

    arg = *(void**)((u8*)ptr + 0x50);
    if (arg == NULL) {
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
                    GSmodelSetPEdescr(*(void**)p, lbl_80363CA8);
                }
                if (flag2 == 0) {
                    GSmodelEnableModulation(*(void**)p, *(void**)((u8*)ptr + 0x50));
                }
            }
        }
    } else {
        *(void**)ptr = GSfilterCreate();
        if (*(void**)ptr == NULL) {
            GSlogWrite((const char*)lbl_80272E30);
            return 0;
        }
    }

    *(void**)((u8*)ptr + 0x54) = *(void**)((u8*)ptr + 0x50);
    *(u32*)((u8*)ptr + 0x58) = 0;
    return 1;

report_null:
    GSlogWrite((const char*)lbl_80272E70);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013B268)
extern void GSmodelSetModulationColor(void* obj, void* param);
extern u32 lbl_8047D1E8;
extern const f32 lbl_8047D1F8;
extern const f32 lbl_8047D1F0;
extern const f32 lbl_8047D1FC;

static inline u8 effectColorAdd(u8 base, u8 color) {
    f32 result = (f32)base + (f32)color - 128.0f;

    if (result < 0.0f) {
        result = 0.0f;
    } else if (result > 255.0f) {
        result = 255.0f;
    }
    return result;
}

static inline u8 effectColorMultiply(u8 base, u8 color) {
    return ((f32)base * (f32)color) / 255.0f;
}

#if 0
asm void fn_8013B268(void* ptr, u8* color) {
#include "src/game/effect/effect_visual_fn_8013B268.inc"
}
#else
void fn_8013B268(void* ptr, u8* color) {
    u8* p;
    void** textures;
    u16 count;
    u16 i;
    u8 useBaseColor;
    u8* baseColor;
    u8 adjusted[4];

    p = ptr;
    count = (u16)*(u32*)(p + 0x48);
    textures = (void**)(p + 0x4);
    useBaseColor = *(u8*)(p + 0x4F);
    if (useBaseColor != 0) {
        baseColor = p + 0x44;
    }
    for (i = 0; i < count; i++, textures++) {
        if (*textures != NULL) {
            if (useBaseColor != 0) {
                adjusted[0] = effectColorAdd(baseColor[0], color[0]);
                adjusted[1] = effectColorAdd(baseColor[1], color[1]);
                adjusted[2] = effectColorAdd(baseColor[2], color[2]);
                adjusted[3] = effectColorMultiply(baseColor[3], color[3]);
                GSmodelSetModulationColor(*textures, adjusted);
            } else {
                GSmodelSetModulationColor(*textures, color);
            }
        }
    }
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013B490_8013B5E4)
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
            GSmodelSetVisibility(*(void**)ptr, 0);
        }
        GXDrawDone();
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013B5E4)
#if defined(EFFECT_VISUAL_BANK_ACTIVE)
extern f32 fn_800E0BE4(void);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void GSvecCopy(void* dst, const void* src);
extern void fn_800DFFCC(void* dst, void* src, const void* value);
extern void fn_800E0718(void*, void*, f32);
extern void GSvecTransformQuat(void* dst, void* quat, void* src);
extern void GSmtxMakeYRotation(void* mtx, f32 angle);
extern void GSvecTransform(void* dst, void* matrix, void* vec);
extern void vecScale(void* dst, void* src, f32 scale);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
#endif
extern u16 fn_800E2C04(u32 size, u32 alignment);
extern u8 fn_800E0E14();
extern void GSmodelSetRotation(void* model, void* rotation);
extern u8 GSmodelCanTexAnimate(void* model);
extern void GSmodelSetTexAnimIndex(void* model, u16 value);
extern void GSmodelSetTexAnimRate(void* model, f32 value);
extern void GSmodelSetTexAnimFrame(void* model, f32 value);
extern void GSmodelSetTexAnimType(void* model, u32 value);
extern void GSmodelStartTexAnimation(void* model);
extern const f32 lbl_8047D200;
extern const f32 lbl_8047D204;
extern u8 lbl_80272EA0[];
/* RULE-EXCEPTION(user-approved): single-function inline helper — see docs/RULE_EXCEPTIONS.md */
static inline u8 surfAllocMesh(u8* p, void* model) {
    u16 columns;
    u16 rows;
    u32 totalSize;
    u8* cursor;
    u8* data;
    u16 handle;
    u32 pointCount;
    u32 vectorSize;
    u32 texcoordSize;
    u32 colorSize;

    columns = *(u16*)(p + 0x4E);
    rows = *(u16*)(p + 0x4C);
    *(void**)p = model;
    *(u16*)(p + 0x1C) = rows + 1;
    *(u16*)(p + 0x1E) = columns + 1;
    pointCount = *(u16*)(p + 0x1C) * *(u16*)(p + 0x1E);
    vectorSize = (pointCount * 0xC + 0x1F) & ~0x1F;
    colorSize = (pointCount * 4 + 0x1F) & ~0x1F;
    texcoordSize = (pointCount * 8 + 0x1F) & ~0x1F;
    *(u32*)(p + 0x18) = (columns * (*(u16*)(p + 0x1C) * 0x10 + 3) + 0x1F) & ~0x1F;
    totalSize = vectorSize + vectorSize + colorSize + texcoordSize + *(u32*)(p + 0x18);

    handle = fn_800E2C04(totalSize, 0x20);
    if (handle != 0) {
        data = fn_800E27B0(handle);
        *(u16*)(p + 0x20) = handle;
        memset(data, 0, totalSize);
        *(u8**)(p + 0x14) = data;
        cursor = data + *(u32*)(p + 0x18);
        *(u8**)(p + 0x4) = cursor;
        cursor += vectorSize;
        *(u8**)(p + 0x8) = cursor;
        cursor += vectorSize;
        *(u8**)(p + 0x10) = cursor;
        cursor += colorSize;
        *(u8**)(p + 0xC) = cursor;
        fn_800E0E14(0, 0);
        return TRUE;
    }
    return FALSE;
}

/* RULE-EXCEPTION(user-approved): single-function inline helper — see docs/RULE_EXCEPTIONS.md */
static inline u8 surfAllocColumn(u8* p) {
    u16 handle;

    handle = _toolentryAlloc__FUl((*(u16*)(p + 0x4E) + 1) * 0xC);
    if (handle != 0) {
        *(u16*)(p + 0xC0) = handle;
        *(void**)(p + 0xBC) = fn_800E27B0(handle);
        return TRUE;
    }
    return FALSE;
}

u16 surfEffectStart(void* ptr) {
    u8* p;
    void* model;
    u16 handle;

    if (ptr == NULL) {
        goto fail;
    }

    p = ptr;
    *(u16*)(p + 0xCC) = 0;
    memset(p, 0, 0x24);
    if (*(s32*)(p + 0x54) == 0) {
        goto fail;
    }

    model = GSresGetResource(*(u32*)(p + 0x50), *(u32*)(p + 0x54));
    if (model == NULL) {
        goto fail;
    }

    fn_800B856C();
    if (!surfAllocMesh(p, model)) {
        goto fail;
    }

    fn_8013BE04(p, p + 0x24, p + 0x3C, *(f32*)(p + 0x40), *(f32*)(p + 0x44),
                *(f32*)(p + 0x48));
    if (!(u8)fn_8013C074(model, p)) {
        goto cleanup;
    }
    if (!surfAllocColumn(p)) {
        goto cleanup;
    }

    GSmodelSetVisibility(model, 0);
    GSmodelSetRotation(model, p + 0x30);
    if (GSmodelCanTexAnimate(model)) {
        GSmodelSetTexAnimIndex(model, 0);
        GSmodelSetTexAnimRate(model, 0.5f);
        GSmodelSetTexAnimFrame(model, 0.0f);
        GSmodelSetTexAnimType(model, 1);
        GSmodelStartTexAnimation(model);
    }
    return 1;

cleanup:
    handle = *(u16*)(p + 0x20);
    *(u16*)(p + 0x20) = 0;
    fn_800E24B0(handle);
    fn_800E209C(handle);
    handle = *(u16*)(p + 0xC0);
    *(u16*)(p + 0xC0) = 0;
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
fail:
    GSlogWrite((const char*)lbl_80272EA0);
    return 0;
}
extern void fn_800E09E8(void* dst, void* src, u32 count);
extern u32 lbl_8047D208;
#if 0
asm u32 fn_8013B85C(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013B85C.inc"
}
#else
u32 fn_8013B85C(void* ptr, u32 delta) {
    u8* p;
    s32 half;
    s32 column;
    f32 t;
    s32 row;
    u8* source;
    u8* points;
    s32 firstCount;
    s32 count;
    s32 columns;
    s32 evenHalf;

    if (ptr != NULL) {
        p = ptr;
        t = (f32)*(u16*)(p + 0xCC) / (f32)*(u16*)(p + 0xCE);
        GSmodelSetVisibility(*(void**)p, 1);
        fn_8013BC10(p, t);

        count = *(u16*)(p + 0x1E);
        if (count % 2 != 0) {
            half = count >> 1;
            firstCount = half + 1;
            fn_800E09E8(*(void**)(p + 0xBC), p + 0x5C, firstCount);
            fn_800E09E8(*(u8**)(p + 0xBC) + firstCount * 0xC, p + 0x8C, half);
        } else {
            evenHalf = count >> 1;
            fn_800E09E8(*(void**)(p + 0xBC), p + 0x5C, evenHalf);
            fn_800E09E8(*(u8**)(p + 0xBC) + evenHalf * 0xC, p + 0x8C, evenHalf);
        }

        count = *(u16*)(p + 0x1C);
        columns = *(u16*)(p + 0x1E);
        points = *(u8**)(p + 0x4);
        for (row = 0; row < count; row++) {
            source = *(u8**)(p + 0xBC);
            for (column = 0; column < columns; column++, source += 0xC, points += 0xC) {
                *(f32*)(points + 0x4) = *(f32*)(source + 0x4);
                *(f32*)(points + 0x8) = *(f32*)(source + 0x8);
            }
        }

        fn_8013BA98(p);
        *(u16*)(p + 0xCC) = *(u16*)(p + 0xCC) + delta;
        return (*(u16*)(p + 0xCC) < *(u16*)(p + 0xCE));
    }
    return 0;
}
#endif
extern u8 lbl_8031554C[];
extern u8 lbl_80315540[];
extern const f32 lbl_8047D200;
/* RULE-EXCEPTION(user-approved): single-function inline helper — see docs/RULE_EXCEPTIONS.md */
static inline void surfCopyColumn(u8* column, s32 depth, s32 width) {
    s32 row;

    for (row = 0; row < depth; row++) {
        GSvecCopy(column + row * width * 0xC, column);
    }
}

void fn_8013BA98(void* ptr) {
    s32 width;
    s32 depth;
    u8* p;
    u8* previous;
    s32 column;
    u8* point;
    u8* points;
    u8* vectors;
    s32 row;
    f32 previousDelta[3];
    f32 nextDelta[3];

    p = ptr;
    points = *(u8**)(p + 0x4);
    vectors = *(u8**)(p + 0x8);
    width = *(u16*)(p + 0x1E);
    depth = *(u16*)(p + 0x1C);
    point = points + 0xC;

    GSvecCopy(vectors, lbl_8031554C);
    for (row = 0; row < depth; row++) {
        GSvecCopy(vectors + row * width * 0xC, vectors);
    }

    for (column = 1; column < width - 1; column++) {
        previous = points;
        points = point;
        vectors += 0xC;
        point += 0xC;
        GSvecSub(previousDelta, points, previous);
        GSvecSub(nextDelta, point, points);
        fn_800DFFCC(previousDelta, previousDelta, lbl_80315540);
        fn_800DFFCC(nextDelta, nextDelta, lbl_80315540);
        GSvecAdd(vectors, nextDelta, previousDelta);
        vecScale(vectors, vectors, 0.5f);
        fn_800E0060(vectors, vectors);
        for (row = 0; row < depth; row++) {
            GSvecCopy(vectors + row * width * 0xC, vectors);
        }
    }

    GSvecCopy(vectors + 0xC, lbl_8031554C);
    surfCopyColumn(vectors + 0xC, depth, width);
}
extern void clear__5GSvecFv(void* vec);
extern const f32 lbl_8047D200;
extern const f32 lbl_8047D210;
extern const f32 lbl_8047D214;
extern const f32 lbl_8047D218;
extern const f32 lbl_8047D21C;
void fn_8013BC10(void* ptr, f32 t) {
    u8* p;
    f32 upperLeft[3];
    f32 upperRight[3];
    f32 lowerRight[3];
    f32 offset[3];
    f32 lowerLeft[3];
    f32 lowerFar[3];
    f32 halfWidth;

    p = ptr;
    halfWidth = 0.5f * *(f32*)(p + 0x44);
    clear__5GSvecFv(upperLeft);
    clear__5GSvecFv(upperRight);
    clear__5GSvecFv(lowerRight);
    clear__5GSvecFv(offset);
    clear__5GSvecFv(lowerLeft);
    clear__5GSvecFv(lowerFar);

    lowerLeft[2] = *(f32*)(p + 0xC4);
    lowerFar[2] = *(f32*)(p + 0xC4);
    upperLeft[2] = -halfWidth;
    lowerRight[2] = halfWidth;
    upperRight[1] = *(f32*)(p + 0xC8);

    if (t < 0.33333334f) {
        t *= 3.0f;
        lowerRight[2] = halfWidth * t;
        upperRight[2] = 0.5f * (upperLeft[2] + lowerRight[2]);
        upperRight[1] = *(f32*)(p + 0xC8) * t;
    } else if (t < 0.6666667f) {
        t -= 0.33333334f;
        t *= 3.0f;
        upperRight[2] = halfWidth * t;
        lowerFar[2] = *(f32*)(p + 0xC4) * (1.0f - t);
        offset[2] = upperRight[2];
    } else {
        t -= 0.6666667f;
        t *= 3.0f;
        upperRight[2] = halfWidth;
        upperRight[1] = *(f32*)(p + 0xC8) * (1.0f - t);
        lowerFar[2] = *(f32*)(p + 0xC4) * -t;
        offset[2] = halfWidth * (1.0f + t);
    }

    GSvecAdd(offset, offset, p + 0x24);
    GSvecAdd(upperLeft, upperLeft, offset);
    GSvecAdd(upperRight, upperRight, offset);
    GSvecAdd(lowerRight, lowerRight, offset);
    GSvecCopy(p + 0x5C, upperLeft);
    GSvecCopy(p + 0x68, upperRight);
    GSvecCopy(p + 0x8C, upperRight);
    GSvecCopy(p + 0x98, lowerRight);
    GSvecCopy(p + 0x74, lowerLeft);
    GSvecCopy(p + 0x80, lowerLeft);
    GSvecCopy(p + 0xA4, lowerFar);
    GSvecCopy(p + 0xB0, lowerLeft);
}
extern const f32 lbl_8047D220;
extern const f32 lbl_8047D204;
extern u32 lbl_8047D228;
extern const f32 lbl_8047D21C;
extern u32 lbl_8047D208;
#if 0
asm void fn_8013BE04(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) {
#include "src/game/effect/effect_visual_fn_8013BE04.inc"
}
#else
void fn_8013BE04(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) {
    u8* p = ptr;
    u16 row;
    u16 rows = *(u16*)(p + 0x1C);
    u16 column;
    u16 cols = *(u16*)(p + 0x1E);
    u8* positions = *(u8**)(p + 4);
    u8* normals = *(u8**)(p + 8);
    u8* texcoords = *(u8**)(p + 0xC);
    u8* colors = *(u8**)(p + 0x10);
    f32 base[3];
    f32 current[3];
    f32 rowStep[3];
    f32 colStep[3];
    f32 inverseBaseX2;
    f32 inverseBaseZ2;

    set__5GSvecFfff(&base, -0.5f * x, 0.0f, -0.5f * z);
    set__5GSvecFfff(&rowStep, x / (rows - 1), 0.0f, 0.0f);
    set__5GSvecFfff(&colStep, 0.0f, 0.0f, z / (cols - 1));
    inverseBaseX2 = 1.0f / (base[0] * base[0]);
    inverseBaseZ2 = 1.0f / (base[2] * base[2]);
    GSvecCopy(&current, &base);

    for (row = 0; row < rows; row++) {
        for (column = 0; column < cols; column++, positions += 0xC, normals += 0xC,
                                           texcoords += 8, colors += 4) {
            GSvecAdd(positions, mtx, &current);
            GSvecCopy(normals, lbl_8031554C);
            *(f32*)(texcoords + 4) = scale * (current[0] - base[0]);
            *(f32*)(texcoords + 0) = scale * (current[2] - base[2]);
            colors[0] = color[0];
            colors[1] = color[1];
            colors[2] = color[2];
            colors[3] = (f32)color[3] *
                        (1.0f - current[0] * current[0] * inverseBaseX2) *
                        (1.0f - current[2] * current[2] * inverseBaseZ2);
            GSvecAdd(&current, &current, &colStep);
        }
        GSvecAdd(&current, &current, &rowStep);
        current[2] = base[2];
    }
    fn_800E0E14(0, 0);
}
#endif
extern void* fn_8019FF48(void* model);
extern void HSD_MObjSetFlags(void* mobj, u32 flags);
extern void HSD_MObjCompileTev(void* mobj);
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || defined(EFFECT_VISUAL_EXACT_8013C074)
#if defined(EFFECT_VISUAL_EXACT_8013C074)
extern u8 fn_800E0E14();
extern void* fn_8019FF48(void* model);
extern void HSD_MObjSetFlags(void* mobj, u32 flags);
extern void HSD_MObjCompileTev(void* mobj);
#endif
#if 0
asm void fn_8013C074(void) {
#include "src/game/effect/effect_visual_fn_8013C074.inc"
}
#else
typedef struct SurfRenderStage {
    s32 kind;
    u32 field_04;
    s32 field_08;
    s32 field_0C;
    u16 field_10;
    u16 field_12;
    u32 field_14;
} SurfRenderStage;
typedef struct SurfRenderPObj {
    u8 pad_00[8];
    SurfRenderStage* stages;
    u16 flags;
    u16 display_count;
    u8* display;
} SurfRenderPObj;
typedef struct SurfRenderDObj {
    u8 pad_00[8];
    void* mobj;
    SurfRenderPObj* pobj;
} SurfRenderDObj;
typedef struct SurfRenderState {
    void* model;
    u8* positions;
    u8* normals;
    u8* texcoords;
    u8* colors;
    u8* display;
    s32 display_size;
    u16 rows;
    u16 cols;
} SurfRenderState;

/* RULE-EXCEPTION(user-approved): single-use inline helper (an HSD_DObjGetMObj-style
   accessor, as in fn_8013D0A8) chosen for register allocation — see
   docs/RULE_EXCEPTIONS.md */
static inline void* surfDObjGetMObj(SurfRenderDObj* dobj) {
    return dobj ? dobj->mobj : NULL;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper (the strip loop, as
   blurBuildStrips in fn_8013D0A8) chosen for register allocation — see
   docs/RULE_EXCEPTIONS.md */
static inline void surfBuildStrips(SurfRenderState* state) {
    s32 top;
    u8* out;
    u16* dst;
    s32 strip;
    s32 row;
    s32 last;
    s32 rows;
    s32 span;

    span = state->cols;
    rows = state->rows;
    out = state->display;

    last = span - 1;
    for (strip = 0; strip < last; strip++) {
        *(u8*) out = 0x98;
        *(u16*) (out + 1) = rows * 2;
        out += 3;

        dst = (u16*)out;
        for (row = 0; row < rows; row++) {
            top = strip + span * row + 1;

            *dst++ = top;
            *dst++ = top;
            *dst++ = top;
            *dst++ = top;
            *dst++ = top - 1;
            *dst++ = top - 1;
            *dst++ = top - 1;
            *dst++ = top - 1;
        }
        out = (u8*)dst;
    }
}

u32 fn_8013C074(void* ptr, void* arg) {
    SurfRenderState* state = arg;
    SurfRenderDObj* dobj = fn_8019FF48(*(void**)((u8*)ptr + 0x8));
    SurfRenderPObj* pobj;
    void* mobj;
    SurfRenderStage* stage;
    s32 found9 = 0;
    s32 found10 = 0;
    s32 found13 = 0;
    s32 found11 = 0;
    s32 kind;
    s32 stage_ordinal;

    if (dobj == NULL) {
        return 0;
    }
    pobj = dobj ? dobj->pobj : NULL;
    mobj = surfDObjGetMObj(dobj);
    if (mobj != NULL) {
        HSD_MObjSetFlags(mobj, 0x40000000);
        HSD_MObjCompileTev(mobj);
    }
    if (pobj == NULL) {
        return 0;
    }
    stage = pobj->stages;
    if (stage == NULL) {
        return 0;
    }

    for (stage_ordinal = 1; (kind = stage->kind) != 0xFF; stage++, stage_ordinal++) {
        switch (kind) {
        case 9:
            found9 = stage_ordinal;
            if (stage->field_08 != 1) {
                return 0;
            }
            if (stage->field_0C != 4) {
                return 0;
            }
            if (stage->field_12 != 12) {
                return 0;
            }
            break;
        case 10:
            found10 = stage_ordinal;
            if (stage->field_08 != 0) {
                return 0;
            }
            if (stage->field_0C != 4) {
                return 0;
            }
            if (stage->field_12 != 12) {
                return 0;
            }
            break;
        case 11:
            found11 = stage_ordinal;
            if (stage->field_08 != 1) {
                return 0;
            }
            if (stage->field_0C != 5) {
                return 0;
            }
            if (stage->field_12 != 4) {
                return 0;
            }
            break;
        case 13:
            found13 = stage_ordinal;
            if (stage->field_08 != 1) {
                return 0;
            }
            if (stage->field_0C != 4) {
                return 0;
            }
            if (stage->field_12 != 8) {
                return 0;
            }
            break;
        default:
            return 0;
        }
    }

    if (!found9 || !found10 || !found11 || !found13) {
        return 0;
    }

    for (stage = pobj->stages; stage->kind != 0xFF; stage++) {
        stage->field_04 = 3;
        switch (stage->kind) {
        case 9:
            stage->field_14 = (u32) state->positions;
            break;
        case 10:
            stage->field_14 = (u32) state->normals;
            break;
        case 11:
            stage->field_14 = (u32) state->colors;
            break;
        case 13:
            stage->field_14 = (u32) state->texcoords;
            break;
        }
    }

    surfBuildStrips(state);

    fn_800E0E14(0, 0);
    pobj->display = state->display;
    pobj->display_count = state->display_size >> 5;
    return 1;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013C5A0_8013C670)
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
        GXDrawDone();
        fn_800B856C();
        if (*(s32*)((u8*)ptr + 0x7c) != 0) {
            fn_800F9210(*(u32*)((u8*)ptr + 0x74), *(u32*)((u8*)ptr + 0x78));
            fn_800F9210(*(u32*)((u8*)ptr + 0x74), *(u32*)((u8*)ptr + 0x7c));
        }
    }
    return 1;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE)
extern void GSmodelStopTexAnimation(void* a);
#if 0
asm void fn_8013C670(void) {
#include "src/game/effect/effect_visual_fn_8013C670.inc"
}
#else
u32 fn_8013C670(void* arg) {
    void* ptr;
    void* inner;

    if (arg != 0) {
        ptr = arg;
        inner = *(void**)ptr;
        GSmodelStopTexAnimation(inner);
        if (inner != 0) {
            GSmodelSetVisibility(inner, 0);
        }
        GXDrawDone();
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
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013C718)
#if defined(EFFECT_VISUAL_BANK_ACTIVE)
extern u16 fn_800E2C04(u32 size, u32 alignment);
extern u8 fn_800E0E14();
extern void GSmodelSetRotation(void* model, void* rotation);
extern u8 GSmodelCanTexAnimate(void* model);
extern void GSmodelSetTexAnimIndex(void* model, u16 value);
extern void GSmodelSetTexAnimRate(void* model, f32 value);
extern void GSmodelSetTexAnimFrame(void* model, f32 value);
extern void GSmodelSetTexAnimType(void* model, u32 value);
extern void GSmodelStartTexAnimation(void* model);
extern f32 fn_800E0BE4(void);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void GSvecCopy(void* dst, const void* src);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
#endif
extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
extern const f32 lbl_8047D230;
extern const f32 lbl_8047D234;
extern const f32 lbl_8047D238;
extern const f32 lbl_8047D23C;
extern u8 lbl_80272ED0[];
#if 0
asm u16 seaEffectStart(void) {
#include "src/game/effect/effect_visual_seaEffectStart.inc"
}
#else
/* RULE-EXCEPTION(user-approved): single-function inline helper — see docs/RULE_EXCEPTIONS.md */
static inline u8 seaAllocMesh(u8* p, void* model) {
    u16 columns;
    u16 rows;
    u8* cursor;
    u8* data;
    u16 handle;
    u32 pointCount;
    u32 vectorSize;
    u32 texcoordSize;
    u32 colorSize;

    columns = *(u16*)(p + 0x72);
    rows = *(u16*)(p + 0x70);
    *(void**)p = model;
    *(u16*)(p + 0x18) = rows + 1;
    *(u16*)(p + 0x1A) = columns + 1;
    pointCount = *(u16*)(p + 0x18) * *(u16*)(p + 0x1A);
    *(u32*)(p + 0x14) = (columns * (*(u16*)(p + 0x18) * 0x10 + 3) + 0x1F) & ~0x1F;
    vectorSize = (pointCount * 0xC + 0x1F) & ~0x1F;
    colorSize = (pointCount * 4 + 0x1F) & ~0x1F;
    texcoordSize = (pointCount * 8 + 0x1F) & ~0x1F;
    /* RULE-EXCEPTION(user-approved): pointCount reused for the total size,
       which gives retail's callee-saved register — see
       docs/RULE_EXCEPTIONS.md */
    pointCount = vectorSize + colorSize + texcoordSize + *(u32*)(p + 0x14);

    handle = fn_800E2C04(pointCount, 0x20);
    if (handle != 0) {
        data = fn_800E27B0(handle);
        *(u16*)(p + 0x1C) = handle;
        memset(data, 0, pointCount);
        *(u8**)(p + 0x10) = data;
        cursor = data + *(u32*)(p + 0x14);
        *(u8**)(p + 0x4) = cursor;
        cursor += vectorSize;
        *(u8**)(p + 0xC) = cursor;
        cursor += colorSize;
        *(u8**)(p + 0x8) = cursor;
        return TRUE;
    }
    return FALSE;
}

/* RULE-EXCEPTION(user-approved): single-function inline helper — see docs/RULE_EXCEPTIONS.md */
static inline u8 seaAllocRandomTable(u8* p) {
    s32 rowCount;
    f32* randomX;
    f32* randomZ;
    f32* randomAngle;
    s32 i;
    u16 handle;
    f32 low;

    rowCount = *(u16*)(p + 0x70) + 1;
    handle = _toolentryAlloc__FUl(rowCount * 3 * sizeof(f32));
    if (handle != 0) {
        *(u16*)(p + 0x8C) = handle;
        randomX = *(f32**)(p + 0x80) = fn_800E27B0(handle);
        randomZ = randomX + rowCount;
        randomAngle = randomZ + rowCount;
        *(f32**)(p + 0x84) = randomZ;
        *(f32**)(p + 0x88) = randomAngle;
        for (i = 0; i < rowCount; randomX++, i++, randomZ++, randomAngle++) {
            low = *(f32*)(p + 0x90);
            *randomX = low + (*(f32*)(p + 0x94) - low) * fn_800E0BE4();
            low = *(f32*)(p + 0x98);
            *randomZ = low + (*(f32*)(p + 0x9C) - low) * fn_800E0BE4();
            *randomAngle = *(f32*)&lbl_8047D230 * fn_800E0BE4();
        }
        return TRUE;
    }
    return FALSE;
}

u16 seaEffectStart(void* ptr) {
    u8* p;
    void* model;
    u16 handle;

    if (ptr == NULL) {
        goto fail;
    }

    p = ptr;
    *(u16*)(p + 0xA4) = 0;
    memset(p, 0, 0x48);
    if (*(s32*)(p + 0x78) == 0) {
        goto fail;
    }

    model = GSresGetResource(*(u32*)(p + 0x74), *(u32*)(p + 0x78));
    if (model == NULL) {
        goto fail;
    }

    GXDrawDone();
    fn_800B856C();
    if (!seaAllocMesh(p, model)) {
        goto fail;
    }

    fn_8013CBF0(p, p + 0x48, p + 0x60, *(f32*)(p + 0x64), *(f32*)(p + 0x68),
                *(f32*)(p + 0x6C));
    if (!(u8)fn_8013D0A8(model, p)) {
        goto cleanup;
    }
    if (!seaAllocRandomTable(p)) {
        goto cleanup;
    }

    GSmodelSetVisibility(model, 1);
    *(f32*)(p + 0x58) += *(f32*)&lbl_8047D234;
    GSmodelSetRotation(model, p + 0x54);
    GSmodelLinkTexAnimToAnim(model, 0);
    if (GSmodelCanTexAnimate(model)) {
        GSmodelSetTexAnimIndex(model, 0);
        GSmodelSetTexAnimRate(model, *(f32*)&lbl_8047D238);
        GSmodelSetTexAnimFrame(model, lbl_8047D23C);
        GSmodelSetTexAnimType(model, 1);
        GSmodelStartTexAnimation(model);
    }
    return 1;

cleanup:
    GSmodelSetVisibility(model, 0);
    handle = *(u16*)(p + 0x1C);
    *(u16*)(p + 0x1C) = 0;
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
    handle = *(u16*)(p + 0x8C);
    *(u16*)(p + 0x8C) = 0;
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
fail:
    GSlogWrite((const char*)lbl_80272ED0);
    return 0;
}
#endif
#if defined(EFFECT_VISUAL_EXACT_8013C718)
/*
 * RULE-EXCEPTION(user-approved): seaEffectStart's .sdata2 pool
 * (0x8047D230-0x8047D240) as named constants defined after the function, so
 * the linked fn_8013CA48 unit still resolves the shared 0.0f by name — see
 * docs/RULE_EXCEPTIONS.md
 */
__declspec(section ".sdata2") const f32 lbl_8047D230 = 6.2831855f;
__declspec(section ".sdata2") const f32 lbl_8047D234 = 1.5707964f;
__declspec(section ".sdata2") const f32 lbl_8047D238 = 0.5f;
#pragma push
#pragma explicit_zero_data on
__declspec(section ".sdata2") const f32 lbl_8047D23C = 0.0f;
#pragma pop
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013CA48)
#if defined(EFFECT_VISUAL_BANK_ACTIVE)
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void GSvecCopy(void* dst, const void* src);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
#endif
extern f32 fn_800E0CA0(f32 angle);
extern u32 lbl_8047D248;
/* RULE-EXCEPTION(user-approved): extern-named stand-in for the 0.0f shared
   with seaEffectStart (0x8047D23C), which stays in the data unit — see
   docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D23C;
extern u8 lbl_80363CB8[];
extern const f32 lbl_8047D240;
extern const f32 lbl_8047D244;
extern u32 lbl_8047D250;
#if 0
asm u32 fn_8013CA48(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013CA48.inc"
}
#else
u32 fn_8013CA48(void* ptr, u32 delta) {
    u8* p;
    f32 angleStep;
    f32 rowAngleStep;
    f32 angle;
    f32 amplitude;
    f32 span;
    f32* grid;
    s32 columns;
    s32 rows;
    s32 column;
    s32 row;
    f32* randomAngle;
    f32* randomZ;
    f32* randomX;

    p = ptr;
    if (p != NULL) {
        if (*(u16*)(p + 0xA4) >= *(u16*)(p + 0xA6)) {
            return 0;
        }

        amplitude = (f32)*(u16*)(p + 0xA4) / (f32)*(u16*)(p + 0xA6);
        randomX = *(f32**)(p + 0x80);
        randomZ = *(f32**)(p + 0x84);
        randomAngle = *(f32**)(p + 0x88);
        rows = *(u16*)(p + 0x18);
        columns = *(u16*)(p + 0x1A);

        GXDrawDone();
        fn_800B856C();
        set__5GSvecFfff(lbl_80363CB8, lbl_8047D23C, 1.0f,
                        lbl_8047D23C);

        span = 360.0f * *(f32*)(p + 0x68);
        amplitude = amplitude * (span * *(f32*)(p + 0xA0));
        angleStep = span / (f32)(columns - 1);
        grid = *(f32**)(p + 0x4);
        for (row = 0; row < rows; row++, randomX++, randomZ++, randomAngle++) {
            rowAngleStep = angleStep / *randomZ;
            angle = *randomAngle + amplitude / *randomZ;
            for (column = 0; column < columns; column++) {
                grid[1] = *(f32*)(p + 0x4C) + *randomX * fn_800E0CA0(angle);
                angle += rowAngleStep;
                grid += 3;
            }
        }

        *(u16*)(p + 0xA4) = *(u16*)(p + 0xA4) + delta;
        return 1;
    }
    return 0;
}
#endif
extern u32 lbl_8047D250;
extern u32 lbl_8047D248;
#if 0
asm void fn_8013CBF0(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) {
#include "src/game/effect/effect_visual_fn_8013CBF0.inc"
}
#else
void fn_8013CBF0(void* ptr, void* mtx, u8* color, f32 x, f32 z, f32 scale) {
    u8* p = ptr;
    u32 rows = *(u16*)(p + 0x18);
    u32 columns = *(u16*)(p + 0x1A);
    f32* positions = *(f32**)(p + 0x4);
    f32* texcoords = *(f32**)(p + 0x8);
    u8* colors = *(u8**)(p + 0xC);
    f32 offset[3];
    f32 current[3];
    f32 rowStep[3];
    f32 columnStep[3];
    f32 inverseX;
    f32 inverseZ;
    f32 factorX;
    f32 factorZ;
    u16 row;
    u16 column;

    GXDrawDone();
    fn_800B856C();
    set__5GSvecFfff(lbl_80363CB8, lbl_8047D23C, 1.0f, lbl_8047D23C);
    set__5GSvecFfff(offset, -0.5f * x, lbl_8047D23C, -0.5f * z);
    /* RULE-EXCEPTION(user-approved): the second zero is spelled through a
       cast so the front end does not hoist one shared load ahead of the
       conversion — see docs/RULE_EXCEPTIONS.md */
    set__5GSvecFfff(rowStep, x / (f32)(s32)(rows - 1), lbl_8047D23C, *(f32*)&lbl_8047D23C);
    set__5GSvecFfff(columnStep, lbl_8047D23C, *(f32*)&lbl_8047D23C, z / (f32)(s32)(columns - 1));
    inverseX = 1.0f / (offset[0] * offset[0]);
    inverseZ = 1.0f / (offset[2] * offset[2]);
    GSvecCopy(current, offset);

    for (row = 0; row < rows; row++) {
        for (column = 0; column < columns; column++, positions += 3, texcoords += 2,
             colors += 4) {
            GSvecAdd(positions, mtx, current);
            texcoords[1] = scale * (current[0] - offset[0]);
            texcoords[0] = scale * (current[2] - offset[2]);
            colors[0] = color[0];
            colors[1] = color[1];
            colors[2] = color[2];
            colors[3] = (u8)((f32)color[3] * (1.0f - inverseX * (current[0] * current[0])) *
                             (1.0f - inverseZ * (current[2] * current[2])));
            GSvecAdd(current, current, columnStep);
        }
        GSvecAdd(current, current, rowStep);
        current[2] = offset[2];
    }
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE)
#if 0
asm u32 fn_8013CE58(void* inner, void* ptr) {
#include "src/game/effect/effect_visual_fn_8013CE58.inc"
}
#else
u32 fn_8013CE58(void* inner, void* ptr) {
    u8* p = ptr;
    u8* displayObject;
    u8* material;
    u8* stages;
    u8* stage;
    s32 op;
    s32 found9;
    s32 found10;
    s32 found13;
    s32 found11;
    s32 stageIndex;

    displayObject = fn_8019FF48(*(void**)((u8*)inner + 0x8));
    found9 = 0;
    found10 = 0;
    found13 = 0;
    found11 = 0;
    if (p[0x46] == 0) {
        return 1;
    }
    if (displayObject == NULL) {
        return 0;
    }

    material = displayObject != NULL ? *(u8**)(displayObject + 0xC) : NULL;
    if (material == NULL) {
        return 0;
    }
    stages = *(u8**)(material + 0x8);
    stage = stages;
    if (stages == NULL) {
        return 0;
    }

    stageIndex = 1;
    for (; (op = *(s32*)stage) != 0xFF; stage += 0x18, stageIndex++) {
        switch (op) {
        case 9:
            found9 = stageIndex;
            if (*(s32*)(stage + 0x8) != 1) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 4) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 12) {
                return 0;
            }
            break;
        case 10:
            found10 = stageIndex;
            if (*(s32*)(stage + 0x8) != 0) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 4) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 12) {
                return 0;
            }
            break;
        case 11:
            found11 = stageIndex;
            if (*(s32*)(stage + 0x8) != 1) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 5) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 4) {
                return 0;
            }
            break;
        case 13:
            found13 = stageIndex;
            if (*(s32*)(stage + 0x8) != 1) {
                return 0;
            }
            if (*(s32*)(stage + 0xC) != 4) {
                return 0;
            }
            if (*(u16*)(stage + 0x12) != 8) {
                return 0;
            }
            break;
        default:
            return 0;
        }
    }

    if (!found9 || !found10 || !found11 || !found13) {
        return 0;
    }

    stageIndex = 0;
    while (*(s32*)stages != 0xFF) {
        *(u32*)(stages + 0x4) = *(u32*)(p + 0x20 + stageIndex * 4);
        *(u32*)(stages + 0x14) = *(u32*)(p + 0x30 + stageIndex * 4);
        stageIndex++;
        stages += 0x18;
    }
    *(u32*)(material + 0x10) = *(u32*)(p + 0x40);
    *(u16*)(material + 0xE) = *(u16*)(p + 0x44);
    return 1;
}
#endif
#endif /* 8013CE58 */

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || defined(EFFECT_VISUAL_EXACT_8013D0A8)
#if defined(EFFECT_VISUAL_EXACT_8013D0A8)
extern void* fn_8019FF48(void* model);
extern void HSD_MObjSetFlags(void* mobj, u32 flags);
extern void HSD_MObjCompileTev(void* mobj);
extern u8 lbl_80363CB8[];
#endif
#if 0
asm void fn_8013D0A8(void) {
#include "src/game/effect/effect_visual_fn_8013D0A8.inc"
}
#else
typedef struct BlurRenderStage {
    s32 kind;
    u32 field_04;
    s32 field_08;
    s32 field_0C;
    u16 field_10;
    u16 field_12;
    void* field_14;
} BlurRenderStage;

typedef struct BlurRenderPObj {
    u8 pad_00[8];
    BlurRenderStage* stages;
    u16 flags;
    u16 display_count;
    u8* display;
} BlurRenderPObj;

typedef struct BlurRenderDObj {
    u8 pad_00[8];
    void* mobj;
    BlurRenderPObj* pobj;
} BlurRenderDObj;

typedef struct BlurRenderState {
    void* model;
    u8* positions;
    u8* texcoords;
    u8* colors;
    u8* display;
    s32 display_size;
    u16 rows;
    u16 cols;
    u32 pad_1C;
    u32 saved_field_04[4];
    u32 saved_field_14[4];
    u8* old_display;
    u16 old_display_count;
    u8 applied;
} BlurRenderState;

/* RULE-EXCEPTION(user-approved): single-use inline helper (an HSD_DObjGetMObj-style
   accessor) chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
static inline void* blurDObjGetMObj(BlurRenderDObj* dobj) {
    return dobj ? dobj->mobj : NULL;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper chosen for register
   allocation — see docs/RULE_EXCEPTIONS.md */
static inline void blurBuildStrips(BlurRenderState* state) {
    s32 top;
    u8* out;
    u16* dst;
    s32 strip;
    s32 row;
    s32 last;
    s32 rows;
    s32 span;

    span = state->cols;
    rows = state->rows;
    out = state->display;

    last = span - 1;
    for (strip = 0; strip < last; strip++) {
        *(u8*) out = 0x98;
        *(u16*) (out + 1) = rows * 2;
        out += 3;

        dst = (u16*)out;
        for (row = 0; row < rows; row++) {
            top = strip + span * row + 1;

            *dst++ = top;
            *dst++ = 0;
            *dst++ = top;
            *dst++ = top;
            *dst++ = top - 1;
            *dst++ = 0;
            *dst++ = top - 1;
            *dst++ = top - 1;
        }
        out = (u8*)dst;
    }
}

u32 fn_8013D0A8(void* ptr, void* arg) {
    BlurRenderState* state = arg;
    BlurRenderDObj* dobj = fn_8019FF48(*(void**)((u8*)ptr + 0x8));
    BlurRenderPObj* pobj;
    void* mobj;
    BlurRenderStage* stage;
    s32 found9 = 0;
    s32 found10 = 0;
    s32 found13 = 0;
    s32 found11 = 0;
    s32 kind;
    s32 stage_ordinal;
    u8* saved_cursor;

    if (dobj == NULL) {
        return 0;
    }
    pobj = dobj ? dobj->pobj : NULL;
    mobj = blurDObjGetMObj(dobj);
    if (mobj != NULL) {
        HSD_MObjSetFlags(mobj, 0x40000000);
        HSD_MObjCompileTev(mobj);
    }
    if (pobj == NULL) {
        return 0;
    }
    stage = pobj->stages;
    if (stage == NULL) {
        return 0;
    }

    for (stage_ordinal = 1; (kind = stage->kind) != 0xFF; stage++, stage_ordinal++) {
        switch (kind) {
        case 9:
            found9 = stage_ordinal;
            if (stage->field_08 != 1) {
                return 0;
            }
            if (stage->field_0C != 4) {
                return 0;
            }
            if (stage->field_12 != 12) {
                return 0;
            }
            break;
        case 10:
            found10 = stage_ordinal;
            if (stage->field_08 != 0) {
                return 0;
            }
            if (stage->field_0C != 4) {
                return 0;
            }
            if (stage->field_12 != 12) {
                return 0;
            }
            break;
        case 11:
            found11 = stage_ordinal;
            if (stage->field_08 != 1) {
                return 0;
            }
            if (stage->field_0C != 5) {
                return 0;
            }
            if (stage->field_12 != 4) {
                return 0;
            }
            break;
        case 13:
            found13 = stage_ordinal;
            if (stage->field_08 != 1) {
                return 0;
            }
            if (stage->field_0C != 4) {
                return 0;
            }
            if (stage->field_12 != 8) {
                return 0;
            }
            break;
        default:
            return 0;
        }
    }

    if (!found9 || !found10 || !found11 || !found13) {
        return 0;
    }

    for (stage = pobj->stages, saved_cursor = (u8*)state; stage->kind != 0xFF;
         stage++, saved_cursor += sizeof(u32)) {
        *(u32*)(saved_cursor + 0x20) = stage->field_04;
        *(void**)(saved_cursor + 0x30) = stage->field_14;
        stage->field_04 = 3;

        switch (stage->kind) {
        case 9:
            stage->field_14 = state->positions;
            break;
        case 10:
            stage->field_14 = lbl_80363CB8;
            break;
        case 11:
            stage->field_14 = state->colors;
            break;
        case 13:
            stage->field_14 = state->texcoords;
            break;
        }
    }

    blurBuildStrips(state);

    state->old_display = pobj->display;
    state->old_display_count = pobj->display_count;
    pobj->display = state->display;
    pobj->display_count = state->display_size >> 5;
    state->applied = 1;
    return 1;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013D604_8013D984)
#if 0
asm void fn_8013D604(void* ptr, u32 val, f32 f1, f32 f2) {
#include "src/game/effect/effect_visual_fn_8013D604.inc"
}
#else
u32 fn_8013D604(void* ptr, u32 val, f32 f1, f32 f2) {
    u16 handle;
    void* node;
    void* cur;
    handle = _toolentryAlloc__FUl(0x14);
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
                GXDrawDone();
                GStextureFree((void*)lbl_8047AEE0);
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
extern void GSmodelDisableEnvMap(void* ptr);
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
            GSmodelDisableEnvMap(inner);
        }
    }
    return 1;
}
#endif
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
                            GXDrawDone();
                            GStextureFree((void*)lbl_8047AEE0);
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
    GSlogWrite((const char*)lbl_80272F00);
    return 0;
}
#endif
extern void GSmodelEnableEnvMap(void* a, void* b, void* c, void* d, f32 e);
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
        GSmodelEnableEnvMap(inner, *(void**)((u8*)ptr + 0x4), *(void**)((u8*)ptr + 0x8),
                    (void*)lbl_8047AEE0, *(f32*)*(void**)((u8*)ptr + 0x10));
        return 1;
    }
log:
    GSlogWrite((const char*)lbl_80272F38);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013D984)
extern s32 fn_800E3B3C(void);
extern void fn_800D377C(u32 a);
extern void fn_800D3410(void* texture, u32 a);
extern u8 fn_800E3C64(void* model);
extern void GSmodelDrawModel(void* obj, u32 flags);
extern void fn_800D3190(void);
extern u8 GSmodelIsEnvMapEnabled(void* model);
extern void GSmodelSetEnvMapBlendValue(void* model, f32 value);
extern u32 lbl_8047AEE0;
extern u32 lbl_8047D260;
#if 0
asm u32 fn_8013D984(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013D984.inc"
}
#else
u32 fn_8013D984(void* ptr, u32 delta) {
    u8* p;
    u8* node;
    void* model;
    s32 modelCount;
    s32 i;
    void* otherModel;
    f32 blend;

    if (ptr != NULL) {
        p = ptr;
        node = *(u8**)(p + 0x10);
        if (node != NULL) {
            if (*(u32*)(node + 0x8) == 0xFFFFFFFF) {
                *(u32*)(p + 0x14) = 0;
            }
            while (*(u32*)(p + 0x14) >= *(u32*)(node + 0x8)) {
                *(u32*)(p + 0x14) -= *(u32*)(node + 0x8);
                node = *(u8**)(node + 0x10);
                *(void**)(p + 0x10) = node;
                if (node == NULL) {
                    return 0;
                }
                if (*(u32*)(node + 0x8) == 0xFFFFFFFF) {
                    *(u32*)(p + 0x14) = 0;
                    break;
                }
            }

            blend = *(f32*)node +
                    ((f32)*(u32*)(p + 0x14) / (f32)*(u32*)(node + 0x8)) *
                        (*(f32*)(node + 0x4) - *(f32*)node);
            model = *(void**)p;
            if (lbl_8047AEE0 != 0 && model != NULL) {
                modelCount = fn_800E3B3C();
                if (modelCount != 0) {
                    fn_800D4604(2);
                    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
                    fn_800D377C(1);
                    fn_800D3410((void*)lbl_8047AEE0, 0);
                    for (i = 0; i < modelCount; i++) {
                        otherModel = fn_800E3B08(i);
                        if (otherModel != NULL && otherModel != model &&
                            fn_800E3C64(otherModel)) {
                            GSmodelDrawModel(otherModel, 0x3010);
                        }
                    }
                    fn_800D3190();
                    fn_800D4604(1);
                }
            }

            if (!GSmodelIsEnvMapEnabled(*(void**)p)) {
                GSmodelEnableEnvMap(*(void**)p, *(void**)(p + 0x4), *(void**)(p + 0x8),
                                    (void*)lbl_8047AEE0, blend);
            } else {
                GSmodelSetEnvMapBlendValue(*(void**)p, blend);
            }
            *(u32*)(p + 0x14) += delta;
            return 1;
        }
    }
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013DB64_8013DE6C)
#if 0
asm void fn_8013DB64(void* ptr, u32 val, f32 f1, f32 f2) {
#include "src/game/effect/effect_visual_fn_8013DB64.inc"
}
#else
u32 fn_8013DB64(void* ptr, u32 val, f32 f1, f32 f2) {
    u16 handle;
    void* node;
    void* cur;
    handle = _toolentryAlloc__FUl(0x14);
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
            GXDrawDone();
            fn_800B856C();
            GStextureFree(*(void**)((u8*)ptr + 0xc));
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
    GSlogWrite((const char*)lbl_80272F70);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013DE6C)
extern u8 GSmodelIsAnimating(void* model);
extern s32 fn_800D3068(void);
extern f32 GSmodelGetAnimFrame(void* model);
extern f32 GSmodelGetAnimRate(void* model);
extern s32 fn_800D45F8(void);
extern u32 GSmaterialGetFlags(void* material);
extern void GSmaterialSetFlags(void* material, u32 flags);
extern void GSmaterialStoreAlpha(void* material);
extern void GSmodelStartAnimation(void* model);
extern void GSmodelSetAnimFrame(void* model, f32 value);
extern void fn_800EC134(void*);
extern void GSmaterialResetAlpha(void* material);
extern void GSmaterialResetFlags(void* material);
extern u32 lbl_8047D288;
extern const f32 lbl_8047D26C;
extern const f32 lbl_8047D268;
extern const f32 lbl_8047D270;
extern const f32 lbl_8047D274;
extern const f32 lbl_8047D278;
extern u32 lbl_8047D290;
extern const f32 lbl_8047D27C;
extern u8 lbl_80363CC8[];
extern const f32 lbl_8047D280;
#if 0
asm void fn_8013DE6C(void) {
#include "src/game/effect/effect_visual_fn_8013DE6C.inc"
}
#else
u32 fn_8013DE6C(void* ptr) {
    extern void fn_800D4604(s32);
    u8* timing;
    f32 halfRate;
    f32 alpha;
    f32 animFrame;
    f32 animRate;
    void** materials;
    s32 j;
    u8* p = ptr;
    f32 blend;
    void* model;
    f32 alphaStep;
    s32 i;
    s32 copies;
    f32 startFrame;
    u32 frame;
    s32 materialCount;
    s32 oldMode;

    if (ptr != NULL) {
        model = *(void**)p;
        timing = *(u8**)(p + 0x18);
        materialCount = *(s32*)(p + 8);
        if (*(s32*)(p + 0x10) != 0 || model == NULL) {
            return 1;
        }
        if (GSmodelIsAnimating(model) == 0) {
            return 1;
        }

        frame = *(u32*)(p + 0x1C);
        blend = (f32)frame / (f32)*(u32*)(timing + 8);
        blend = blend * (*(f32*)(timing + 4) - *(f32*)timing) + *(f32*)timing;
        switch (fn_800D3068()) {
        case 1:
            copies = 60.0f * blend + 0.5f;
            break;
        case 2:
            copies = 30.0f * blend + 0.5f;
            break;
        case 3:
            copies = 20.0f * blend + 0.5f;
            break;
        case 4:
            copies = 15.0f * blend + 0.5f;
            break;
        }
        if (copies == 0) {
            return 1;
        }

        animFrame = GSmodelGetAnimFrame(model);
        animRate = GSmodelGetAnimRate(model);
        startFrame = animFrame - animRate * copies;
        if (startFrame < 0.0f) {
            copies = animFrame / animRate;
            startFrame = 0.0f;
        }
        if (copies == 0) {
            return 1;
        }
        if (startFrame >= animFrame) {
            return 1;
        }

        GSmodelSetPEdescr(model, lbl_80363CC8);
        oldMode = fn_800D45F8();
        fn_800D4604(2);
        _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
        materials = *(void***)(p + 4);
        for (i = 0; i < materialCount; i++, materials++) {
            if (*materials != NULL) {
                GSmaterialSetFlags(*materials, GSmaterialGetFlags(*materials) | 0x20);
                GSmaterialStoreAlpha(*materials);
            }
        }

        GSmodelStartAnimation(model);
        alphaStep = 1.0f / copies;
        halfRate = 0.5f * animRate;
        j = 1;
        while (startFrame + halfRate < animFrame) {
            alpha = j++ * alphaStep;
            GSmodelSetAnimFrame(model, startFrame);
            fn_800EC134(model);
            startFrame += animRate;
            alpha = 0.5f * (alpha * alpha);
            materials = *(void***)(p + 4);
            for (i = 0; i < materialCount; i++, materials++) {
                if (*materials != NULL) {
                    GSmaterialSetAlpha(*materials, alpha);
                }
            }
            GSmodelDrawModel(model, 0x3010);
        }

        materials = *(void***)(p + 4);
        for (i = 0; i < materialCount; i++, materials++) {
            if (*materials != NULL) {
                GSmaterialResetAlpha(*materials);
                GSmaterialResetFlags(*materials);
            }
        }
        GSmodelResetPEdescr(model);
        GSmodelSetAnimFrame(model, animFrame);
        fn_800EC134(model);
        GSmodelDrawModel(model, 0x3010);
        fn_800D4604(oldMode);
        return 1;
    }
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013E258_ONLY)
extern const f32 lbl_8047D27C;
extern u32 lbl_8047D288;
extern const f32 lbl_8047D298;
extern const f32 lbl_8047D29C;
extern const f32 lbl_8047D2A0;
extern u8 lbl_80314AE8[];
extern const f32 lbl_8047D280;
#if 0
asm u32 fn_8013E258(void* model, void* unused, void* state) {
#include "src/game/effect/effect_visual_fn_8013E258.inc"
}
#else
u32 fn_8013E258(void* model, void* unused, void* state) {
    struct EffectVisualState {
        f32 start;
        f32 end;
        u32 duration;
    } *visual;
    s32 alpha;
    f32 amount;

    (void)unused;
    if (state == NULL) {
        goto ret0;
    }

    visual = *(struct EffectVisualState**)((u8*)state + 0x18);
    amount = visual->start +
        ((f32)*(u32*)((u8*)state + 0x1C) / (f32)visual->duration) *
        (visual->end - visual->start);
    alpha = (s32)(*(f32*)&lbl_8047D298 * amount);
    gfxSetOrtho(0.0f, 0.0f,
                *(f32*)&lbl_8047D29C, *(f32*)&lbl_8047D2A0);
    gfxSetBlendMode(1, 6, 7);
    gfxSetZMode(0, 7, 0);
    fn_800D9ED8(1);
    gfxSetUpdates(1, 1, 0);
    gfxSetCullMode(0);
    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxSetVtxDesc(lbl_80314AE8);
    gfxLoadTexture(0, model);
    gfxSetPrimitive(4);
    gfxBegin(4);
    gfxVtxPos(0.0f, 0.0f, 0.0f);
    fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 0.0f, 0.0f);
    gfxVtxPos(*(f32*)&lbl_8047D29C, 0.0f,
                0.0f);
    fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 1.0f, 0.0f);
    gfxVtxPos(0.0f, *(f32*)&lbl_8047D2A0,
                0.0f);
    fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 0.0f, 1.0f);
    gfxVtxPos(*(f32*)&lbl_8047D29C, *(f32*)&lbl_8047D2A0,
                0.0f);
    fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, alpha);
    gfxVtxTexCoord(0, 1.0f, 1.0f);
    gfxEnd();
    fn_800D9ED8(0);
    return 1;

ret0:
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013E470_8013E54C)
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE)
#if 0
asm void fn_8013E54C(void) {
#include "src/game/effect/effect_visual_fn_8013E54C.inc"
}
#else
u32 fn_8013E54C(void* arg) {
    void* ptr;
    if (arg != 0) {
        ptr = arg;
        if (*(void**)((u8*)ptr + 4) != 0) {
            GXDrawDone();
            fn_800B856C();
            GStextureFree(*(void**)((u8*)ptr + 4));
        }
        memset(ptr, 0, 0x34);
    }
    return 1;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013E5AC_8013E8A4)
#if 0
asm void fn_8013E5AC(void) {
#include "src/game/effect/effect_visual_fn_8013E5AC.inc"
}
#else
u32 fn_8013E5AC(u8* p) {
    extern u8 lbl_80466BC0[];
    extern void* GStextureCreate(u16 width, u16 height, u32 format, u32 tlut, u8 mip);
    extern void* GStextureLockImage(void* buf, u32 b);
    extern void GStextureUnlockImage(void* buf);
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
            memset(GStextureLockImage(buf, 0), 0, *(u16*)(tbl + 4) * *(u16*)(tbl + 6));
            GStextureUnlockImage(buf);
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
    GSlogWrite((const char*)lbl_80272FA0);
    return 0;
}
#endif
extern const f32 lbl_8047D2AC;
extern const f32 lbl_8047D2B0;
extern const f32 lbl_8047D2B4;
extern const f32 lbl_8047D2A8;
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
        gfxSetOrtho(*(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2AC,
                    *(f32*)&lbl_8047D2B0, *(f32*)&lbl_8047D2B4);
        if (ptr[0x19] & 4) {
            gfxSetBlendMode(1, 6, 7);
        } else {
            gfxSetBlendMode(1, 6, 1);
        }
        gfxSetZMode(0, 7, 0);
        gfxSetUpdates(1, 1, 0);
        gfxSetCullMode(0);
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetVtxDesc(lbl_80314AE8);
        gfxSetPrimitive(4);
        gfxLoadTexture(0, model);
        gfxBegin(4);
        gfxVtxPos(*(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2AC,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2AC);
        gfxVtxPos(*(f32*)&lbl_8047D2B0, *(f32*)&lbl_8047D2AC,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D2A8, *(f32*)&lbl_8047D2AC);
        gfxVtxPos(*(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2B4,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D2AC, *(f32*)&lbl_8047D2A8);
        gfxVtxPos(*(f32*)&lbl_8047D2B0, *(f32*)&lbl_8047D2B4,
                    *(f32*)&lbl_8047D2AC);
        fn_800D5CB8(0, ptr[8], ptr[9], ptr[0xA], ptr[0xB]);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D2A8, *(f32*)&lbl_8047D2A8);
        gfxEnd();
        return 1;
    }
    return 0;
}
#else
void fn_8013E6C4(void) { /* TODO */ }
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013E8A4)
extern double fmod(double value, double modulus);
extern const f32 lbl_8047D2B8;
/* RULE-EXCEPTION(user-approved): extern-named stand-in for the 1.0f this
   function shares with the linked fn_8013E6C4 (0x8047D2A8) — see
   docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D2A8;
extern const f32 lbl_8047D2BC;
extern u32 lbl_8047D2D8;
extern const f32 lbl_8047D2C0;
extern u32 lbl_8047D2C8;
extern const f32 lbl_8047D2D0;
extern const f32 lbl_8047D2D4;
#if 0
asm u32 fn_8013E8A4(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013E8A4.inc"
}
#else
u32 fn_8013E8A4(void* ptr, u32 delta) {
    u8* p;
    u8 flags;
    f32 period;
    f32 range;
    f32 base;
    f32 phase;
    f32 wave;
    f32 frame;

    if (ptr != NULL) {
        p = ptr;
        flags = p[0x19];
        period = 60.0f / *(f32*)(p + 0x24);
        base = *(f32*)(p + 0x1C);
        range = *(f32*)(p + 0x20);

        if (flags & 1) {
            *(f32*)(p + 0x2C) = lbl_8047D2A8;
            /* RULE-EXCEPTION(user-approved): phase reused as the 1 + base
               temporary for retail's operand order — see
               docs/RULE_EXCEPTIONS.md */
            phase = lbl_8047D2A8 + base;
            *(f32*)(p + 0x28) = phase + range;
        } else {
            if (flags & 2) {
                frame = *(u16*)(p + 0x30);
                phase = fmod(frame, 2.0f * period);
                phase = fmod(3.1415927f * phase / period, 3.1415927410125732);
            } else {
                frame = *(u16*)(p + 0x30);
                phase = fmod(frame, period);
                phase = 6.2831855f * phase / period;
            }
            wave = cos(phase);
            if (flags & 8) {
                *(f32*)(p + 0x2C) = lbl_8047D2A8;
            } else {
                *(f32*)(p + 0x2C) = 0.5f * (lbl_8047D2A8 + wave);
            }
            *(f32*)(p + 0x28) = lbl_8047D2A8 + (base - range * wave);
        }

        *(u16*)(p + 0x30) += delta;
        return *(u16*)(p + 0x32) == -1 || *(u16*)(p + 0x30) < *(u16*)(p + 0x32);
    }
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013EA44)
extern void __assert();
extern void* GSmodelGetBound(void* model);
extern void* GSmodelGetMatrixPtr(void* model);
extern void GSmodelSetRenderFlags(void* model, u32 flags);
extern void GSmodelInitMaterialAlpha(void* model);
extern void fn_8019D620(void* jobj);
extern void GSmodelSetMaterialAlpha(void* model, f32 alpha);
extern void GSmodelResetMaterialAlpha(void* model);
extern void GSmodelResetRenderFlags(void* model);
extern void GSmodelDrawModel(void* model, u32 flags);
extern void GSmodelSetPEdescr(void* model, void* peDescr);
extern u8 lbl_80363CD8[];
/* RULE-EXCEPTION(user-approved): the jobj.h assert strings as named .sdata2
   arrays, placed between fn_8013E8A4's pool and fn_8013EA44's conversion
   constant as in retail — see docs/RULE_EXCEPTIONS.md */
const char lbl_8047D2E0[7] = "jobj.h";
const char lbl_8047D2E8[5] = "jobj";
extern char lbl_80272FD0[10];
const char lbl_8047D2F0[6] = "scale";
extern const f32 lbl_8047D2A8;
#if 0
asm void fn_8013EA44(void) {
#include "src/game/effect/effect_visual_fn_8013EA44.inc"
}
#else
typedef struct LayerVec {
    f32 x;
    f32 y;
    f32 z;
} LayerVec;

#define LAYER_JOBJ_FLUSH(jobj)                                                  \
    do {                                                                        \
        if (!(*(u32*)((jobj) + 0x14) & 0x02000000)) {                           \
            if ((jobj) != NULL) {                                               \
                s32 dirty_;                                                     \
                u32 flags_;                                                     \
                if ((jobj) == NULL) {                                           \
                    __assert(lbl_8047D2E0, 0x25D, lbl_8047D2E8);                \
                }                                                               \
                flags_ = *(u32*)((jobj) + 0x14);                                \
                dirty_ = 0;                                                     \
                if (!(flags_ & 0x00800000) && (flags_ & 0x40)) {                \
                    dirty_ = 1;                                                 \
                }                                                               \
                if (dirty_ == 0) {                                              \
                    fn_8019D620(jobj);                                          \
                }                                                               \
            }                                                                   \
        }                                                                       \
    } while (0)

#define LAYER_JOBJ_SET_SCALE(jobj, value)                                       \
    do {                                                                        \
        if ((jobj) == NULL) {                                                   \
            __assert(lbl_8047D2E0, 0x316, lbl_8047D2E8);                        \
        }                                                                       \
        if ((value) == NULL) {                                                  \
            __assert(lbl_8047D2E0, 0x317, lbl_8047D2F0);                        \
        }                                                                       \
        *(LayerVec*)((jobj) + 0x2C) = *(value);                                 \
        LAYER_JOBJ_FLUSH(jobj);                                                 \
    } while (0)

#define LAYER_JOBJ_SET_TRANSLATE(jobj, value)                                   \
    do {                                                                        \
        if ((jobj) == NULL) {                                                   \
            __assert(lbl_8047D2E0, 0x3A9, lbl_8047D2E8);                        \
        }                                                                       \
        if ((value) == NULL) {                                                  \
            __assert(lbl_8047D2E0, 0x3AA, lbl_80272FD0);                        \
        }                                                                       \
        *(LayerVec*)((jobj) + 0x38) = *(value);                                 \
        LAYER_JOBJ_FLUSH(jobj);                                                 \
    } while (0)

void fn_8013EA44(void* ptr) {
    extern void fn_800EC134(void*);
    u8* p;
    void* model;
    void* texture;
    u8* part;
    void* bound;
    void* mtx;
    LayerVec baseScale;
    LayerVec scale;
    LayerVec baseTranslate;
    LayerVec translate;
    LayerVec mid;
    LayerVec offset;
    f32 layerAlpha;
    f32 layerAlphaStep;
    f32 alpha;
    f32 alphaStep;
    s32 i;
    s32 count;

    p = ptr;
    texture = *(void**)(p + 4);
    model = *(void**)p;
    count = p[0x18];
    if (texture == NULL || model == NULL) {
        return;
    }

    part = *(u8**)((u8*)model + 8);
    if (part == NULL) {
        return;
    }

    fn_800EC134(model);
    GSmodelSetPEdescr(model, lbl_80363CD8);

    if (part == NULL) {
        __assert(lbl_8047D2E0, 0x3E4, lbl_8047D2E8);
    }
    if (&baseTranslate == NULL) {
        __assert(lbl_8047D2E0, 0x3E5, lbl_80272FD0);
    }
    baseTranslate = *(LayerVec*)(part + 0x38);

    if (part == NULL) {
        __assert(lbl_8047D2E0, 0x351, lbl_8047D2E8);
    }
    if (&baseScale == NULL) {
        __assert(lbl_8047D2E0, 0x352, lbl_8047D2F0);
    }
    baseScale = *(LayerVec*)(part + 0x2C);

    bound = GSmodelGetBound(model);
    GSvecAdd(&mid, (u8*)bound + 0x1C, (u8*)bound + 0x10);
    vecScale(&mid, &mid, 0.5f);
    mtx = GSmodelGetMatrixPtr(model);
    GSvecTransform(&mid, mtx, &mid);
    GSvecSub(&mid, &mid, &baseTranslate);

    fn_800D377C(2);
    fn_800D4604(2);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    fn_800D3410(texture, 0);

    alpha = *(f32*)(p + 0x28);
    layerAlphaStep = lbl_8047D2A8 / (f32)count;
    alphaStep = (alpha - lbl_8047D2A8) / (f32)count;
    layerAlpha = lbl_8047D2A8 - (f32)count * layerAlphaStep;

    GSmodelSetRenderFlags(model, 0x20);
    GSmodelInitMaterialAlpha(model);
    lbl_80363CD8[4] = 1;
    lbl_80363CD8[5] = 4;
    lbl_80363CD8[6] = 5;

    for (i = 0; i < count; i++) {
        alpha -= alphaStep;
        layerAlpha += layerAlphaStep;

        vecScale(&scale, &baseScale, alpha);
        offset.x = mid.x * (scale.x - baseScale.x);
        offset.y = mid.y * (scale.y - baseScale.y);
        offset.z = mid.z * (scale.z - baseScale.z);
        GSvecSub(&translate, &baseTranslate, &offset);

        LAYER_JOBJ_SET_SCALE(part, &scale);
        LAYER_JOBJ_SET_TRANSLATE(part, &translate);

        GSmodelSetMaterialAlpha(model, layerAlpha);
        GSmodelDrawModel(model, 0x3010);
    }

    LAYER_JOBJ_SET_TRANSLATE(part, &baseTranslate);
    LAYER_JOBJ_SET_SCALE(part, &baseScale);

    lbl_80363CD8[4] = 2;
    lbl_80363CD8[5] = 5;
    lbl_80363CD8[6] = 0;
    GSmodelSetMaterialAlpha(model, lbl_8047D2A8);
    GSmodelDrawModel(model, 0x3010);
    GSmodelResetMaterialAlpha(model);
    GSmodelResetRenderFlags(model);
    GSmodelResetMaterialAlpha(model);
    fn_800D3190();
    fn_800D4604(1);
    fn_800D377C(1);
    GSmodelResetPEdescr(model);
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013F000_8013F344)
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
        GXDrawDone();
        fn_800B856C();
        if (*(void**)((u8*)ptr + 0x4) != NULL) {
            GStextureFree(*(void**)((u8*)ptr + 0x4));
            *(u32*)((u8*)ptr + 0x4) = 0;
        }
        if (*(void**)ptr != NULL) {
            fn_800D75F4(*(void**)ptr);
            *(u32*)ptr = 0;
        }
        if (lbl_8047AEE8 != 0) {
            *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC - 1;
            if (*(u16*)&lbl_8047AEEC == 0) {
                GXDrawDone();
                GStextureFree((void*)lbl_8047AEE8);
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
                    GXDrawDone();
                    fn_800B856C();
                    if (*(void**)(p + 0x4) != NULL) {
                        GStextureFree(*(void**)(p + 0x4));
                        *(u32*)(p + 0x4) = 0;
                    }
                    if (*(void**)p != NULL) {
                        fn_800D75F4(*(void**)p);
                        *(u32*)p = 0;
                    }
                    if (lbl_8047AEE8 != 0) {
                        *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC - 1;
                        if (*(u16*)&lbl_8047AEEC == 0) {
                            GXDrawDone();
                            GStextureFree((void*)lbl_8047AEE8);
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
                GXDrawDone();
                fn_800B856C();
                if (*(void**)(p + 0x4) != NULL) {
                    GStextureFree(*(void**)(p + 0x4));
                    *(u32*)(p + 0x4) = 0;
                }
                if (*(void**)p != NULL) {
                    fn_800D75F4(*(void**)p);
                    *(u32*)p = 0;
                }
                if (lbl_8047AEE8 != 0) {
                    *(u16*)&lbl_8047AEEC = *(u16*)&lbl_8047AEEC - 1;
                    if (*(u16*)&lbl_8047AEEC == 0) {
                        GXDrawDone();
                        GStextureFree((void*)lbl_8047AEE8);
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
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013F344_8013F410)
extern u32 lbl_8047AEE8;
/* RULE-EXCEPTION(user-approved): extern-named stand-ins for constants shared with linked code — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D300;
extern u8 lbl_80272FE0[];
#if 0
asm u16 distortionEffectStart(void) {
#include "src/game/effect/effect_visual_distortionEffectStart.inc"
}
#else
typedef struct DistortionState {
    void* unk_00;
    void* texture;   /* 0x04 */
    void* model;     /* 0x08 */
    u32   partIdx;   /* 0x0C */
    f32   unk_10;    /* 0x10 */
    f32   unk_14;    /* 0x14 */
    u8    pad_18[4];
    f32   unk_1C;    /* 0x1C */
    f32   unk_20;    /* 0x20 */
    f32   unk_24;    /* 0x24 */
    u8    pad_28[2];
    u16   unk_2A;
    f32   pos[3];    /* 0x2C */
    u8    pad_38[0x78];
    u16   frame;     /* 0xB0 */
    u16   duration;  /* 0xB2 */
} DistortionState;

u16 distortionEffectStart(void* ptr) {
    DistortionState* s = ptr;
    void* part;
    f32 tmp;

    if (s != NULL) {
        if (s->texture == NULL || lbl_8047AEE8 == 0) {
            return 0;
        }
        if (s->model == NULL) {
            return 0;
        }

        part = GSmodelGetPart(s->model, s->partIdx);
        if (part != NULL) {
            GSpartGetTransform(part, s->pos, NULL, NULL);
            GSpartFree(part);
        } else {
            GSmodelGetPosition(s->model, s->pos);
        }
        s->frame = 0;
        tmp = *(f32*)&lbl_8047D300;
        s->unk_10 = tmp;
        return 1;
    }
    GSlogWrite((const char*)lbl_80272FE0);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013F410_8013F80C)
extern u8 GSmodelGetVisibility(void* obj);
extern void gsParticleDraw(u32 a, u32 b);
extern void fn_800D848C(u32 a, u32 b, u32 c, void* d);
extern void fn_800E064C(void* dst);
extern void fn_800DC1D4(u32 a);
extern void fn_800DBA54(u32 a);
extern void fn_800DB9F0(u32 a, u32 b, u32 c);
extern void fn_800DB988(u32 a, u32 b, u32 c);
extern void fn_800DB900(u32 a, void* b, u32 c);
extern void fn_800DBCE4(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void gfxSetTevStage(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800DC14C(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void fn_800DC0D4(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void fn_800DC04C(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void fn_800DBFD4(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void gfxVtxColor(u32 a, u32 r, u32 g, u32 b);
extern void fn_800DBE5C(u32 a);
extern u32 lbl_8047AEE8;
extern u32 lbl_8047AEF0;
/* RULE-EXCEPTION(user-approved): extern-named stand-ins for constants shared with linked code — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D304;
extern const f32 lbl_8047D300;
extern const f32 lbl_8047D308;
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
        _distortionEffectUpdateMatrices(ptr);
        if (lbl_8047AEE8 != 0) {
            count = fn_800E3B3C();
            if (count != 0) {
                fn_800D4604(2);
                _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
                fn_800D377C(1);
                fn_800D3410((void*)lbl_8047AEE8, 0);
                for (i = 0; i < count; i++) {
                    obj = fn_800E3B08(i);
                    if (obj != NULL && GSmodelGetVisibility(obj) != 0) {
                        GSmodelDrawModel(obj, 0x3010);
                    }
                }
                if (*(u16*)(p + 0x28) != 0) {
                    gsParticleDraw(0x2000, 0);
                    gsParticleDraw(0x2000, 1);
                }
                fn_800D3190();
                fn_800D4604(1);
            }
        }
        gfxSetZMode(0, 2, 0);
        gfxSetCullMode(0);
        gfxSetFlags(0x80000003);
        gfxClearFlags(4);
        gfxSetBlendMode(0, 6, 7);
        fn_800D9ED8(1);
        gfxSetUpdates(1, 1, 0);
        gfxLoadTexture(0, (void*)lbl_8047AEE8);
        gfxLoadTexture(1, *(void**)(p + 0x4));
        fn_800D848C(0, 2, 4, p + 0x68);
        fn_800E064C(mtx);
        fn_800D848C(1, 2, 5, mtx);
        fn_800DC1D4(1);
        fn_800DBA54(1);
        fn_800DB9F0(0, 1, 1);
        fn_800DB988(0, lbl_8047AEF0, lbl_8047AEF0);
        fn_800DB900(1, p + 0x98, 1);
        fn_800DBCE4(0, 0, 1, 0, 1);
        gfxSetTevStage(0, 0, 0, 0, 2);
        fn_800DC14C(0, 0, 0, 0, 1, 0);
        fn_800DC0D4(0, 0xf, 0xf, 0xf, 8);
        fn_800DC04C(0, 0, 0, 0, 1, 0);
        fn_800DBFD4(0, 7, 7, 7, 4);
        gfxSetVtxDesc(*(void**)p);
        gfxSetPrimitive(6);
        fn_800D7F14(p + 0x38);
        gfxBegin(4);

        gfxVtxPos(*(f32*)&lbl_8047D304, *(f32*)&lbl_8047D304, *(f32*)&lbl_8047D300);
        gfxVtxColor(0, 0xff, 0xff, 0xff);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D300);
        gfxVtxTexCoord(1, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D300);

        gfxVtxPos(*(f32*)&lbl_8047D308, *(f32*)&lbl_8047D304, *(f32*)&lbl_8047D300);
        gfxVtxColor(0, 0xff, 0xff, 0xff);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);
        gfxVtxTexCoord(1, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);

        gfxVtxPos(*(f32*)&lbl_8047D308, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);
        gfxVtxColor(0, 0xff, 0xff, 0xff);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D308);
        gfxVtxTexCoord(1, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D308);

        gfxVtxPos(*(f32*)&lbl_8047D304, *(f32*)&lbl_8047D308, *(f32*)&lbl_8047D300);
        gfxVtxColor(0, 0xff, 0xff, 0xff);
        gfxVtxTexCoord(0, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D308);
        gfxVtxTexCoord(1, *(f32*)&lbl_8047D300, *(f32*)&lbl_8047D308);

        gfxEnd();
        fn_800D7E5C();
        fn_800D848C(0, 2, 4, mtx);
        fn_800DBE5C(0);
        fn_800DBA54(0);
        gfxClearFlags(0x80000002);
        fn_800D9ED8(0);
        return 1;
    }
    return 0;
}
#else
u32 fn_8013F410(void* ptr) { /* TODO */ }
#endif
#endif

/* fn_8013F80C's own object does not see distortionEffectStart's block. */
#if defined(EFFECT_VISUAL_EXACT_8013F80C) && \
    !defined(EFFECT_VISUAL_EXACT_8013F344_8013F410)
typedef struct DistortionState {
    void* unk_00;
    void* texture;   /* 0x04 */
    void* model;     /* 0x08 */
    u32   partIdx;   /* 0x0C */
    f32   unk_10;    /* 0x10 */
    f32   unk_14;    /* 0x14 */
    u8    pad_18[4];
    f32   unk_1C;    /* 0x1C */
    f32   unk_20;    /* 0x20 */
    f32   unk_24;    /* 0x24 */
    u8    pad_28[2];
    u16   unk_2A;
    f32   pos[3];    /* 0x2C */
    u8    pad_38[0x78];
    u16   frame;     /* 0xB0 */
    u16   duration;  /* 0xB2 */
} DistortionState;
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013F80C)
extern u32 lbl_8047AEE8;
extern u32 lbl_8047D310;
/* RULE-EXCEPTION(user-approved): extern-named stand-ins for constants shared with linked code — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D300;
extern const f32 lbl_8047D308;
#if 0
asm u32 fn_8013F80C(void* ptr, u32 delta) {
#include "src/game/effect/effect_visual_fn_8013F80C.inc"
}
#else
u32 fn_8013F80C(void* ptr, u32 delta) {
    DistortionState* s = ptr;
    void* part;
    f32 threshold;
    f32 progress;

    if (s != NULL) {
        if (s->texture == NULL || lbl_8047AEE8 == 0) {
            return 0;
        }

        if (s->unk_2A != 0) {
            part = GSmodelGetPart(s->model, s->partIdx);
            if (part != NULL) {
                GSpartGetTransform(part, s->pos, NULL, NULL);
                GSpartFree(part);
            }
        }

        s->unk_10 = s->unk_14 * s->duration;
        progress = (f32)s->frame / (f32)s->duration;
        threshold = s->unk_1C;
        if (threshold != *(f32*)&lbl_8047D300 && progress <= threshold) {
            s->unk_10 *= progress / threshold;
        } else {
            s->unk_10 *= (*(f32*)&lbl_8047D308 - progress) / (*(f32*)&lbl_8047D308 - threshold);
        }
        s->unk_20 = s->unk_24 * (*(f32*)&lbl_8047D308 - progress);
        s->frame += delta;
        return s->frame < s->duration;
    }
    return 0;
}
#endif
extern void* gfxGetCameraMatrix(u32 index);
extern void GScameraGetLookAt(void* mtx, void* lookAt, void* eye);
extern void fn_800E0628(void* dst, void* src);
extern void fn_800E0238(void* dst, void* src);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern s32 fn_800D2DE8(void* input, void* output, s32 count);
extern void GSlogWrite(const char* fmt, ...);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void fn_800E03E8(void* matrix, f32 x, f32 y, f32 z);
/* RULE-EXCEPTION(user-approved): extern-named stand-ins for constants shared with linked code — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D300;
extern const f32 lbl_8047D304;
extern const f32 lbl_8047D308;
extern u8 lbl_8027301C[];
extern const f32 lbl_8047D318;
extern const f32 lbl_8047D31C;
extern const f32 lbl_8047D320;
extern u32 lbl_8047AEE8;
extern u32 lbl_8047D310;
extern const f32 lbl_8047D324;
#if 0
asm void _distortionEffectUpdateMatrices(void) {
#include "src/game/effect/effect_visual__distortionEffectUpdateMatrices.inc"
}
#else
void _distortionEffectUpdateMatrices(void* ptr) {
    void* camera;
    f32* matrix;
    void* cameraMatrix;
    f32 viewMatrix[12];
    f32 corners[2][3];
    f32 projected[2][3];
    f32 eye[3];
    f32 lookAt[3];
    f32 xScale;
    f32 yScale;

    camera = GScameraGetActiveCamera();
    matrix = (f32*)((u8*)ptr + 0x38);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    cameraMatrix = gfxGetCameraMatrix(0);
    fn_800E064C(matrix);
    if (camera != NULL) {
        GScameraGetLookAt(camera, lookAt, eye);
        fn_800E0628(viewMatrix, cameraMatrix);
        viewMatrix[11] = lbl_8047D300;
        viewMatrix[7] = lbl_8047D300;
        viewMatrix[3] = lbl_8047D300;
        fn_800E0238(matrix, viewMatrix);
    }
    fn_800E02C4(matrix, *(f32*)((u8*)ptr + 0x10),
                *(f32*)((u8*)ptr + 0x10), *(f32*)((u8*)ptr + 0x10));
    matrix[3] = *(f32*)((u8*)ptr + 0x2C);
    matrix[7] = *(f32*)((u8*)ptr + 0x30);
    matrix[11] = *(f32*)((u8*)ptr + 0x34);
    set__5GSvecFfff(corners[0], lbl_8047D304, lbl_8047D304,
                    lbl_8047D300);
    GSvecTransform(corners[0], matrix, corners[0]);
    set__5GSvecFfff(corners[1], lbl_8047D308, lbl_8047D308,
                    lbl_8047D300);
    GSvecTransform(corners[1], matrix, corners[1]);
    if (fn_800D2DE8(corners, projected, 2) != 2) {
        GSlogWrite((const char*)lbl_8027301C);
        return;
    }
    projected[0][0] /= 6.4e+02f;
    projected[0][1] /= 4.8e+02f;
    projected[1][0] /= 6.4e+02f;
    projected[1][1] /= 4.8e+02f;
    xScale = 0.5f * (*(f32*)((u8*)ptr + 0x20) - lbl_8047D308);
    *(f32*)((u8*)ptr + 0x98) = 0.5f;
    *(f32*)((u8*)ptr + 0x9C) = lbl_8047D300;
    *(f32*)((u8*)ptr + 0xA0) = xScale;
    *(f32*)((u8*)ptr + 0xA4) = lbl_8047D300;
    *(f32*)((u8*)ptr + 0xA8) = 0.5f;
    *(f32*)((u8*)ptr + 0xAC) = xScale;
    fn_800E048C((u8*)ptr + 0x68, projected[1][0] - projected[0][0],
                 projected[1][1] - projected[0][1], lbl_8047D300);
    xScale = 128.0f /
             (f32)GStextureGetXsize((void*)lbl_8047AEE8);
    yScale = 128.0f /
             (f32)GStextureGetYsize((void*)lbl_8047AEE8);
    xScale *= *(f32*)((u8*)ptr + 0x20);
    yScale *= *(f32*)((u8*)ptr + 0x20);
    /* RULE-EXCEPTION(user-approved): integer-cast address stops MWCC sharing it with the earlier call — see docs/RULE_EXCEPTIONS.md */
    fn_800E03E8((void*)((u32)ptr + 0x68), xScale + projected[0][0],
                yScale + projected[0][1], lbl_8047D300);
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013FBE0_8013FF0C)
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
extern void* gsParticleBankLoad(void* a);
#if 0
asm void fn_8013FCC4(void) {
#include "src/game/effect/effect_visual_fn_8013FCC4.inc"
}
#else
u32 fn_8013FCC4(void* ptr) {
    void* res;
    if (ptr) {
        if (*(u32*)((u8*)ptr + 0x4) != 0 && *(u32*)((u8*)ptr + 0x8) != 0) {
            *(void**)((u8*)ptr + 0x10) = GSresGetResource(*(u32*)ptr, *(u32*)((u8*)ptr + 0x8));
        } else {
            *(void**)((u8*)ptr + 0x10) = NULL;
        }
        if (*(u32*)((u8*)ptr + 0xc) != 0) {
            res = GSresGetResource(*(u32*)ptr, *(u32*)((u8*)ptr + 0xc));
            if (res != NULL) {
                *(void**)((u8*)ptr + 0x14) = gsParticleBankLoad(res);
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
extern void GSmodelStopAnimation(void* a);
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
            GSmodelStopAnimation(*(void**)((u8*)ptr + 0x10));
            GSmodelStopTexAnimation(*(void**)((u8*)ptr + 0x10));
            GSmodelSetVisibility(*(void**)((u8*)ptr + 0x10), 0);
        }
    }
    return 1;
}
#endif
extern void* gsParticleCreate(void* ptr, u16 arg, u32 flag);
extern void fn_800E3CC8(void* model, u32 flag);
extern u8 GSmodelCanAnimate(void* model);
extern void GSmodelSetAnimIndex(void* model, u16 value);
extern void GSmodelSetAnimType(void* model, u32 value);
extern void GSmodelSetAnimRate(void* model, f32 value);
extern const f32 lbl_8047D328;
extern const f32 lbl_8047D32C;
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
        *(void**)((u8*)ptr + 0x20) = gsParticleCreate(*(void**)((u8*)ptr + 0x14), *(u16*)((u8*)ptr + 0x1e), 1);
    }

    if (model != NULL) {
        GSmodelSetVisibility(model, 1);
        fn_800E3CC8(model, 1);
        if (GSmodelCanAnimate(model)) {
            GSmodelSetAnimIndex(model, *(u16*)((u8*)ptr + 0x1c));
            GSmodelSetAnimType(model, *(u32*)((u8*)ptr + 0x18));
            GSmodelSetAnimRate(model, *(f32*)&lbl_8047D328);
            GSmodelSetAnimFrame(model, *(f32*)&lbl_8047D32C);
            GSmodelStartAnimation(model);
        }
        if (GSmodelCanTexAnimate(model)) {
            GSmodelSetTexAnimIndex(model, *(u16*)((u8*)ptr + 0x1c));
            GSmodelSetTexAnimType(model, *(u32*)((u8*)ptr + 0x18));
            GSmodelSetTexAnimRate(model, *(f32*)&lbl_8047D328);
            GSmodelSetTexAnimFrame(model, *(f32*)&lbl_8047D32C);
            GSmodelStartTexAnimation(model);
        }
    }
    return 1;

report_null:
    GSlogWrite((const char*)lbl_80273078);
    return 0;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_8013FF0C_80140138)
extern f64 tan(f64 x);
extern u8 GSmodelCanAnimate(void* model);
extern void* GSmodelGetBound(void* model);
extern void GScameraGetLookAt(void* mtx, void* lookAt, void* eye);
extern void GSvecAdd(void* dst, void* srcA, void* srcB);
extern u32 fn_80118DA8(void* ptr);
extern void fn_80118F04(void* arg1, void* arg2);
extern u8 GSmodelHasAnimationEnded(void* model);
extern u8 GSmodelHasTexAnimationEnded(void* model);
extern void GSmodelSetPosition(void* entry, void* param);
extern void GSmodelSetScale(void* entry, void* param);
extern u8 lbl_80273060[];
extern u8 lbl_8027306C[];
extern const f32 lbl_8047D334;
extern const f32 lbl_8047D328;
extern const f32 lbl_8047D330;
extern const f32 lbl_8047D338;
extern const f32 lbl_8047D32C;
typedef struct BillboardVec {
    f32 x;
    f32 y;
    f32 z;
} BillboardVec;

u32 fn_8013FF0C(void* ptr) {
    u8* p;
    void* camera;
    void* model;
    void* material;
    u32 updated;
    BillboardVec position;
    BillboardVec billboardScale;
    f32 interest[3];
    f32 up[3];
    f32 fov;
    f32 nearPlane;
    f32 aspect;
    f32 farPlane;

    if (ptr != NULL) {
        p = ptr;
        model = *(void**)(p + 0x10);
        camera = cameraGetActive();
        position = *(BillboardVec*)lbl_80273060;
        billboardScale = *(BillboardVec*)lbl_8027306C;
        updated = 0;
        if (camera != NULL) {
            _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
            GScameraGetPosition(camera, &position);
            GScameraGetLookAt(camera, up, interest);
            GSvecSub(up, interest, &position);
            fn_800E0060(up, up);
            GScameraGetPerspective(camera, &fov, &aspect, &nearPlane, &farPlane);
            nearPlane *= *(f32*)&lbl_8047D330;
            fov = *(f32*)&lbl_8047D334 * fov;
            fov = fov * *(f32*)&lbl_8047D328;
            billboardScale.y = *(f32*)&lbl_8047D338 * nearPlane * (f32)tan(fov);
            billboardScale.x = billboardScale.y * aspect;
            billboardScale.z = *(f32*)&lbl_8047D32C;
            vecScale(up, up, nearPlane);
            GSvecAdd(&position, &position, up);
        }

        if (*(void**)(p + 0x20) != NULL &&
            fn_80118DA8(*(void**)(p + 0x20)) != 0) {
            fn_80118F04(*(void**)(p + 0x20), &position);
            updated = 1;
        }

        if (*(void**)(p + 0x10) != NULL &&
            ((GSmodelCanAnimate(model) && !GSmodelHasAnimationEnded(model)) ||
             (GSmodelCanTexAnimate(model) && !GSmodelHasTexAnimationEnded(model)))) {
            material = GSmodelGetBound(model);
            billboardScale.x /= *(f32*)((u8*)material + 0x28);
            billboardScale.y /= *(f32*)((u8*)material + 0x2C);
            GSmodelSetPosition(model, &position);
            GSmodelSetScale(model, &billboardScale);
            updated = 1;
        }
        return updated;
    }
    return 0;
}
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE) || \
    defined(EFFECT_VISUAL_EXACT_80140138_801402AC)
#if 0
asm void Unload__13ModelSequenceFPUc(void) {
#include "src/game/effect/effect_visual_fn_80140138.inc"
}
#else
void Unload__13ModelSequenceFPUc(u8* ptr) {
    extern void fn_800DF608(void* handle);
    if (*(u32*)(ptr) != 0) {
        fn_800DF608(*(void**)(ptr));
        *(u32*)(ptr) = 0;
    }
    if (*(u32*)(ptr + 0x4) != 0) {
        GStextureFree(*(void**)(ptr + 0x4));
        *(u32*)(ptr + 0x4) = 0;
    }
}
#endif
extern void* _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl(void* tex1, void* tex2, u32 arg);
extern void GSmaterialSetTexture(void* material, void* texture);
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

    handle = GSmodelGetPart(model, 2);
    if (handle == NULL) {
        return 0;
    }

    out[0] = NULL;
    out[1] = NULL;
    out[0] = GSpartGetMaterial(handle, 0);
    if (out[0] != NULL) {
        tex1 = GSresGetResource(0x387, 0x10561200);
        if (tex1 != NULL) {
            tex2 = GSresGetResource(0x387, 0x10571200);
            if (tex2 != NULL) {
                out[1] = _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl(tex1, tex2, arg);
                if (out[1] != NULL) {
                    GSmaterialSetTexture(out[0], out[1]);
                    ret = 1;
                }
            }
        }
    }

    if ((u8)ret == 0) {
        if (out[1] != NULL) {
            GStextureFree(out[1]);
            out[1] = NULL;
        }
        if (out[0] != NULL) {
            fn_800DF608(out[0]);
            out[0] = NULL;
        }
    }

    GSpartFree(handle);
    return ret;
}
#endif
#endif

#if !defined(EFFECT_VISUAL_BANK_ACTIVE)
void* _pachiruEffectCreateTexture__FP9GStextureP9GStextureUl(void* baseTexture,
                                                             void* spotTexture,
                                                             u32 pattern)
{
    extern u8 GStextureGetMiplevels(void* texture);
    extern u32 GStextureGetTlutFormat(void* texture);
    extern u32 GStextureGetFormat(void* texture);
    extern void* GStextureLockImage(void* texture, u32 level);
    extern void GStextureUnlockImage(void* texture);
    extern void GStextureConvertToHW(void* texture);
    extern void fn_800EF098(void* texture);
    extern void* memcpy(void* dst, const void* src, u32 size);
    extern const f32 lbl_8047D340;
    extern const u8 lbl_80478BA8[8];
    extern const char lbl_802730B8[];
    u16* sourcePixels;
    void* outputTexture;
    s16 heightTemp;
    u32 image;
    s32 baseWidth;
    s32 baseHeight;
    s32 pixelCount;
    s32 scratch;

    sourcePixels = NULL;
    baseWidth = (s16)GStextureGetXsize(baseTexture);
    heightTemp = (s16)GStextureGetYsize(baseTexture);
    baseHeight = heightTemp;
    outputTexture = GStextureCreate(baseWidth, baseHeight,
                                    GStextureGetFormat(baseTexture),
                                    GStextureGetTlutFormat(baseTexture),
                                    GStextureGetMiplevels(baseTexture));
    image = (u32)GStextureLockImage(outputTexture, 0);
    if (image == 0) {
        goto fail;
    }

    sourcePixels = GStextureLockImage(baseTexture, 0);
    if (sourcePixels == NULL) {
        goto fail;
    }

    pixelCount = baseWidth * baseHeight;
    scratch = pixelCount * (s32)sizeof(u16);
    memcpy((void*)image, sourcePixels, scratch);
    DCFlushRange((void*)image, scratch);
    GStextureUnlockImage(baseTexture);

    fn_800EF098(outputTexture);
    fn_800EF098(spotTexture);
    sourcePixels = GStextureLockImage(spotTexture, 0);
    if (sourcePixels == NULL) {
        goto fail;
    }

    {
    u16* lastOutputPixel;
    s16 spotHeight;
    s32 row;
    s32 spot;
    s32 x;
    s32 startPixel;
    const u8* placement;
    s32 outputIndex;
    s16 sourceIndex;
    s32 y;
    s32 xOffset;
    s32 yOffset;

    scratch = (s16)GStextureGetXsize(spotTexture);
    spotHeight = (s16)GStextureGetYsize(spotTexture);
    if (scratch > (baseWidth >> 2) || spotHeight > (baseHeight >> 2)) {
        goto fail;
    }

    lastOutputPixel = (u16*)image + (pixelCount - 1);
    spot = 0;
    placement = lbl_80478BA8;
    for (; (s16)spot < 4;) {
        yOffset = (s16)((s16)((pattern >> 4) & 0xF) - 8);
        outputIndex = (s16)(pattern & 0xF) - 8;
        xOffset = (s16)outputIndex;
        pattern >>= 8;
        y = (s32)(lbl_8047D340 * (f32)yOffset) + placement[1];
        x = (s32)(lbl_8047D340 * (f32)xOffset);
        yOffset = y * baseWidth;
        x = x + placement[0];
        startPixel = (s16)(x + yOffset);

        for (row = 0; (s16)row < spotHeight; row++) {
            sourceIndex = (s16)(row * scratch);
            outputIndex = (s16)(startPixel + row * baseWidth);
            if (outputIndex >= 0) {
                u16* source;
                u16* output;
                s32 column;

                output = (u16*)image + outputIndex;

                if (output <= lastOutputPixel) {
                    source = sourcePixels + sourceIndex;

                    for (column = 0; (s16)column < scratch;) {
                        if (output <= lastOutputPixel && *source > *output) {
                            *output = *source;
                        }
                        column++;
                        output++;
                        source++;
                    }
                }
            }
        }
        placement += 2;
        spot++;
    }

    GStextureUnlockImage(outputTexture);
    GStextureUnlockImage(spotTexture);
    GStextureConvertToHW(spotTexture);
    GStextureConvertToHW(outputTexture);
    return outputTexture;
    }

fail:
    GSlogWrite(lbl_802730B8);
    if (image != 0) {
        GStextureUnlockImage(outputTexture);
    }
    if (sourcePixels != NULL) {
        GStextureUnlockImage(spotTexture);
    }
    if (outputTexture != NULL) {
        GStextureFree(outputTexture);
    }
    return NULL;
}

#if 0
asm void itemParamGetPPMaxFullFlag(void) {
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
u32 itemParamGetPPMaxFullFlag(void* ptr) {
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
#endif

#if 0
asm void itemParamGetSpAttackEffortUp(void) {
#include "src/game/effect/effect_visual_fn_80143718.inc"
}
#else
unsigned int itemParamGetSpAttackEffortUp(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return (unsigned int)(unsigned char)p[0xF];
}
#endif

#if 0
asm void itemParamGetSpDefenceEffortUp(void) {
#include "src/game/effect/effect_visual_fn_80143730.inc"
}
#else
unsigned int itemParamGetSpDefenceEffortUp(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return (unsigned int)(unsigned char)p[0xE];
}
#endif

#if 0
asm void itemParamGetQuickEffortUp(void) {
#include "src/game/effect/effect_visual_fn_80143748.inc"
}
#else
unsigned int itemParamGetQuickEffortUp(const void *ptr) {
    const unsigned char *p = (const unsigned char*)ptr;
    if (p == NULL) return 0;
    return (unsigned int)(unsigned char)p[0xD];
}
#endif

#if 0
asm void itemParamGetEvolutionFlag(void) {
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
unsigned int itemParamGetEvolutionFlag(const void *ptr) {
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
asm void tasteDataGetAisyou(void) {
#include "src/game/effect/effect_visual_fn_80143ABC.inc"
}
#else
int tasteDataGetAisyou(void* obj, u32 index) {
    s8* new_var;
    u32 masked_index;
    if (obj == NULL) return 0;
    masked_index = (u16)index;
    if (masked_index >= 0x19) return 0;
    new_var = (s8*)obj;
    return (int)(s8)*(new_var + masked_index + 4);
}
#endif
#endif
