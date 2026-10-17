/**
 * @file tracefx.c
 * @brief TraceFX -- Trail / trace visual effects for Pokemon Colosseum.
 *
 * Decompiled from:
 *   fn_801364A8                          -- Trail resource/model load helper
 *   fn_80137114 (tracefxRender)          -- Per-frame trail rendering
 *   fn_8013735C (tracefxInit)            -- Initialise a TraceFXWork structure
 *   fn_8013757C (tracefxStartEffectImpl) -- Internal start implementation
 *   fn_80137780 (tracefxStopEffectImpl)  -- Internal stop / cleanup
 *   fn_8013796C (tracefxStartUpdate)     -- Begin trail update cycle
 *   fn_801379E4 (tracefxSetTrailParam)   -- Set a trail parameter
 *   fn_80137A2C (tracefxSetTrailColor)   -- Set trail RGBA colour
 *   tracefxStartEffect (tracefxStartEffect)     -- Public start API
 *   fn_80137D14 (tracefxAddSegment)      -- Add segments to a running trail
 *   fn_80137F58 (tracefxUpdate)          -- Per-frame trail logic update
 *
 * A prior campaign transplant had left invented duplicate definitions
 * named tracefxInit, tracefxAddSegment, and tracefxUpdate that reimplement
 * fn_8013735C, fn_80137D14, and fn_80137F58 under friendly names; none had
 * any callers anywhere in the tree (the real fn_ names are what get called
 * -- see fn_8013796C below), so they have been removed, along with the
 * tracefxStartEffect_Draft helper that only existed to call the fictional
 * tracefxInit and was itself unreferenced.
 *
 * Debug strings:
 *   "tracefxStartEffect: Could not start trail effect!"
 *       (lbl_80272B08 -- referenced when allocation fails)
 *
 * The trail effect system renders motion trails behind moving objects
 * (e.g., attack animations, Pokemon tails).  Each trail consists of a
 * chain of segments that are generated from model bone positions,
 * interpolated over time, and faded out as they age.
 *
 * A trail effect works by:
 *   1. Loading two model references: a "start bone" and an "end bone".
 *   2. Each frame, sampling the bone positions and creating a quad strip
 *      between consecutive samples.
 *   3. Applying colour fade and width taper over the segment lifetime.
 *   4. Rendering via the GS rendering pipeline (GSpart model system).
 *
 * fn_801364A8 was merged in from the former game/effect/effect_util.c
 * CodeCandidate bucket (0x8013151C - 0x80137114): it shares the
 * 60.0f/0.0f/int-to-float state and directly calls
 * fn_80137114/fn_8013735C/fn_8013757C/fn_80137780 below, so it is the
 * true first function of this translation unit.
 *
 * Address range: 0x801364A8 - 0x801380D4 (approx.)
 */

#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "game/effect/gs_effect.h"

/* ===== External engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);          /* OSReport / GSlog */
extern u32   GSgfxGetFrameCount(void);                   /* fn_800D37CC */
extern void* GSresGetResource(u32 group, u32 model);         /* GSfloor model load */
extern void* GSmodelGetPart(void* model, u16 partIdx);     /* GSpart get sub-part */
extern void  GSpartGetTransform(void* part, void* outPos,
                          void* a, void* b);             /* GSpart get position */
extern void  GSpartFree(void* part);                    /* GSpart commit */
extern void  GSvecCopy(void* dst, void* src);         /* Vec3 copy */
extern void  GSlerpGetLinearInterpolationVector(void* dst, void* srcA,
                          void* srcB, f32 t);            /* Vec3 lerp */
extern void  GSmodelSetVisibility(void* model, u32 flag);        /* GSpart set visibility */
extern u16   _toolentryAlloc__FUl(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void  fn_800E24B0(u16 handle);                   /* GSmemLock/free step */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */
extern u32   wazaSequenceSysGetResID(void);                          /* Random seed generator */
extern void  loadArchivePtr(u32 memOffset, u32 resId,
                          u32 size, u32 handle);         /* GSfloor load resource */
extern void  loadModel(u32 memOffset, u32 size,
                          u32 data, u32 handle);         /* GSfloor load data */
extern void  memset(void* dst, u32 val, u32 size);
extern void  GXDrawDone(void);
extern void  fn_800B856C(void);
extern void  GStextureFree(void* p);
extern void* fn_80131428(void* owner, u32 size);
extern void  fn_80131200();
extern void  fn_8013139C(void* obj, u32 flag);

/* ===== GS immediate-mode render API (used by tracefxUpdate) ===== */
extern void  _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern void  gfxSetBlendMode(s32 a, s32 b, s32 c);
extern void  gfxSetUpdates(s32 a, s32 b, s32 c);
extern void  gfxSetZMode(s32 a, s32 b, s32 c);
extern void  gfxSetCullMode(s32 a);
extern void  gfxSetFlags(s32 a);
extern void  gfxClearFlags(s32 a);
extern void  gfxSetVtxDesc(void* p);
extern void  gfxLoadTexture(s32 a, void* p);
extern void  gfxSetPrimitive(s32 a);
extern void  gfxBegin(s32 a);
extern void  gfxVtxPos(f32 x, f32 y, f32 z);
extern void  fn_800D5CB8(s32 a, u8 r, u8 g, u8 b, u8 al);
extern void  gfxVtxTexCoord(s32 a, f32 s, f32 t);
extern void  gfxEnd(void);

/* ===== Forward declarations (vtable callbacks) ===== */
u8*   fn_80137114(u8* work, u8* params, s32 frames);
u8*   fn_8013735C(u8* work, void* params, s32 frames);
u8*   fn_8013757C(u8* work, void* params, s32 frames);
u8*   fn_80137780();
BOOL  fn_801379E4(u8* w);
BOOL  fn_80137A2C(u8* w);
BOOL  fn_80137D14(u8* work, u32 steps);
int   fn_80137F58(u8* w);
BOOL  tracefxStartEffect(u8* w);

/* ===== String constants (rodata) ===== */
extern const char lbl_80272B08[]; /* "tracefxStartEffect: Could not start trail effect!" */

/* ===== SDA21 float constants ===== */


#define TRACEFX_ALIGN32(ptr) ((u8*)(((u32)(ptr) + 0x1F) & ~0x1F))
#define TRACEFX_ALIGN32_SIZE(size) (((u32)(size) + 0x1F) & ~0x1F)
#define TRACEFX_RGBA_R(value) ((u8)(value))
#define TRACEFX_RGBA_G(value) ((u8)((value) >> 8))
#define TRACEFX_RGBA_B(value) ((u8)((value) >> 16))
#define TRACEFX_RGBA_A(value) ((u8)((value) >> 24))

/* TRACEFX_8013796C_ONLY (set by tracefx_r55_8013796C_suffix.c) builds only
 * 0x8013796C-0x801380D4, which owns the .sdata2 constants 0x8047D130-0x8047D148. */
#if !defined(TRACEFX_8013796C_ONLY)
/* RULE-EXCEPTION(user-approved): local opt_propagation off keeps retail's copies of params (desc) and the header loads through them - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_propagation off
/* 0x801364A8 | 0xC6C */
u8* fn_801364A8(u8* work, u8* params) {
    extern u8 lbl_80314638[];
    extern u8 lbl_80314AE8[];
    extern s32 fn_800D37CC(void);
    extern u16 fn_800E2C04(u32, u32);
    extern void* GStextureLoad(void*);
    extern void fn_800EFD14(void*, u16);
    extern void GStextureSetWrap(void*, s32, s32);
    extern void* memcpy(void*, const void*, u32);
    extern void fn_8013D604(u8*, s32, f32, f32);

    u8* data;
    u8* aligned;
    s32 frames;
    void* copy;
    u16 handle;
    u8* desc;
    s32 offset;
    u32 count;
    s32 i;

#define TRACEFX_FRAME_DURATION(total) \
    ((u16)(s32)(((f32)(s32)fn_800D37CC() * (f32)(s32)(total)) / 60.0f))

    desc = params;
    frames = *(s32*)(desc + 4);
    params += 0xC;
    memset(work, 0, 0xD8);
    *(u32*)(work + 0x00) = *(u32*)desc;
    *(s32*)(work + 0x04) =
        (s32)(((f32)(s32)fn_800D37CC() * (f32)frames) / 60.0f);

    switch (*(u32*)desc) {
    case 0:
        params = fn_80137780(work + 8, params, frames);
        break;

    case 1:
        memset(work + 8, 0, 0x60);
        *(u16*)(work + 0x52) = TRACEFX_FRAME_DURATION(frames);
        *(u16*)(work + 0x4C) = *(s32*)(params + 0x3C);
        *(u16*)(work + 0x4E) = *(s32*)(params + 0x40);
        *(f32*)(work + 0x48) = *(f32*)(params + 0x38);
        *(u8*)(work + 0x67) = *(u32*)((u8*)params + 0x44) >> 24;
        *(u8*)(work + 0x66) = *(u32*)((u8*)params + 0x44) >> 16;
        *(u8*)(work + 0x65) = *(u32*)((u8*)params + 0x44) >> 8;
        *(u8*)(work + 0x64) = *(u32*)((u8*)params + 0x44);
        *(u32*)(work + 0x58) = *(s32*)(params + 0x48);
        *(f32*)(work + 0x44) = *(f32*)(params + 0x34);
        *(f32*)(work + 0x40) = *(f32*)(params + 0x30);
        GSvecCopy(work + 0x10, params + 0x00);
        GSvecCopy(work + 0x34, params + 0x24);
        GSvecCopy(work + 0x1C, params + 0x0C);
        GSvecCopy(work + 0x28, params + 0x18);

        aligned = TRACEFX_ALIGN32(params + 0x54);
        handle = fn_800E2C04(TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x4C)), 0x20);
        if (handle != 0) {
            copy = fn_800E27B0(handle);
            memcpy(copy, aligned, TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x4C)));
            *(u32*)(work + 0x60) = (u32)GStextureLoad(copy);
            fn_800EFD14(*(void**)(work + 0x60), handle);
        } else {
            *(u32*)(work + 0x60) = 0;
        }
        aligned += TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x4C));
        *(u32*)(work + 0x5C) = (u32)lbl_80314AE8;
        params = aligned;
        break;

    case 2:
        memset(work + 8, 0, 0x24);
        *(f32*)(work + 0x18) = *(f32*)(params + 0x04);
        *(f32*)(work + 0x1C) = *(f32*)(params + 0x08);
        *(f32*)(work + 0x20) = (f32)(s32)*(s32*)(params + 0x0C);
        *(f32*)(work + 0x24) = (f32)(s32)*(s32*)(params + 0x10);
        *(f32*)(work + 0x28) = *(f32*)(params + 0x14);
        *(u8*)(work + 0x0B) = *(u32*)((u8*)params + 0x00) >> 24;
        *(u8*)(work + 0x0A) = *(u32*)((u8*)params + 0x00) >> 16;
        *(u8*)(work + 0x09) = *(u32*)((u8*)params + 0x00) >> 8;
        *(u8*)(work + 0x08) = *(u32*)((u8*)params + 0x00);
        *(u32*)(work + 0x0C) = (u32)lbl_80314638;
        *(u16*)(work + 0x12) = TRACEFX_FRAME_DURATION(frames);
        params += 0x1C;
        break;

    case 3:
        params = fn_8013757C(work + 8, params, frames);
        break;

    case 4:
        memset(work + 8, 0, 0x2C);
        *(u16*)(work + 0x28) = TRACEFX_FRAME_DURATION(frames);
        *(u8*)(work + 0x23) = *(u32*)((u8*)params + 0x00) >> 24;
        *(u8*)(work + 0x22) = *(u32*)((u8*)params + 0x00) >> 16;
        *(u8*)(work + 0x21) = *(u32*)((u8*)params + 0x00) >> 8;
        *(u8*)(work + 0x20) = *(u32*)((u8*)params + 0x00);
        *(u16*)(work + 0x2A) = *(s32*)(params + 0x04);
        *(u16*)(work + 0x30) = *(s32*)(params + 0x08);
        *(u16*)(work + 0x32) = *(s32*)(params + 0x0C);

        aligned = TRACEFX_ALIGN32(params + 0x18);
        handle = fn_800E2C04(TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x10)), 0x20);
        if (handle != 0) {
            copy = fn_800E27B0(handle);
            memcpy(copy, aligned, TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x10)));
            *(u32*)(work + 0x1C) = (u32)GStextureLoad(copy);
            fn_800EFD14(*(void**)(work + 0x1C), handle);
        } else {
            *(u32*)(work + 0x1C) = 0;
        }
        aligned += TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x10));
        if (*(u32*)(work + 0x1C) != 0) {
            GStextureSetWrap(*(void**)(work + 0x1C), 0, 0);
        }
        params = aligned;
        *(u32*)(work + 0x18) = (u32)lbl_80314AE8;
        break;

    case 5:
    {
        u8* cursor;

        memset(work + 8, 0, 0x4C);
        *(u16*)(work + 0x46) = TRACEFX_FRAME_DURATION(frames);
        cursor = TRACEFX_ALIGN32(params + 0x3C);
        *(f32*)(work + 0x18) = *(f32*)(params + 0x00);
        *(f32*)(work + 0x1C) = *(f32*)(params + 0x04);
        *(f32*)(work + 0x20) = *(f32*)(params + 0x08);
        *(f32*)(work + 0x24) = *(f32*)(params + 0x0C);
        *(f32*)(work + 0x28) = *(f32*)(params + 0x10);
        *(f32*)(work + 0x2C) = *(f32*)(params + 0x14);
        *(f32*)(work + 0x30) = *(f32*)(params + 0x18);
        *(f32*)(work + 0x34) = *(f32*)(params + 0x1C);
        *(f32*)(work + 0x38) = *(f32*)(params + 0x20);
        *(f32*)(work + 0x3C) = *(f32*)(params + 0x24);
        *(u16*)(work + 0x40) = *(s32*)(params + 0x28);
        *(u16*)(work + 0x42) = *(s32*)(params + 0x2C);
        *(u16*)(work + 0x52) = *(s32*)(params + 0x30);
        *(u16*)(work + 0x48) = 0x4E20;
        *(u16*)(work + 0x4A) = wazaSequenceSysGetResID();
        *(u16*)(work + 0x4C) = wazaSequenceSysGetResID();

        {
            void* model;

            loadArchivePtr((u32)cursor, *(s32*)(params + 0x34), 0x4E20, *(u16*)(work + 0x4A));
            model = GSresGetResource(0x4E20, *(u16*)(work + 0x4A));
            loadModel((u32)model, 0x4E20, 0, *(u16*)(work + 0x4C));
            model = GSresGetResource(0x4E20, *(u16*)(work + 0x4C));
            if (model != 0) {
                GSmodelSetVisibility(model, 0);
            }
        }
        /* RULE-EXCEPTION(user-approved): block-local carrier gives retail's add into a scratch register before the params copy - see docs/RULE_EXCEPTIONS.md */
        {
            u8* next = cursor + TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x34));
            params = next;
        }
        break;
    }

    case 6:
        params = fn_8013735C(work + 8, params, frames);
        break;

    case 7:
        memset(work + 8, 0, 0xD0);
        *(u16*)(work + 0xD6) = TRACEFX_FRAME_DURATION(frames);
        aligned = TRACEFX_ALIGN32(params + 0x34);
        *(f32*)(work + 0x2C) = *(f32*)(params + 0x00);
        *(f32*)(work + 0x30) = *(f32*)(params + 0x04);
        *(f32*)(work + 0x34) = *(f32*)(params + 0x08);
        *(s8*)(work + 0x47) = (s8)(*(s32*)((u8*)params + 0x0C) >> 24);
        *(u8*)(work + 0x46) = *(u32*)((u8*)params + 0x0C) >> 16;
        *(u8*)(work + 0x45) = *(u32*)((u8*)params + 0x0C) >> 8;
        *(u8*)(work + 0x44) = *(u32*)((u8*)params + 0x0C);
        *(f32*)(work + 0x48) = *(f32*)(params + 0x10);
        *(f32*)(work + 0x4C) = *(f32*)(params + 0x14);
        *(f32*)(work + 0x50) = *(f32*)(params + 0x18);
        *(u16*)(work + 0x54) = *(s32*)(params + 0x1C);
        *(u16*)(work + 0x56) = *(s32*)(params + 0x20);
        *(f32*)(work + 0xCC) = *(f32*)(params + 0x24);
        *(f32*)(work + 0xD0) = *(f32*)(params + 0x28);
        *(u32*)(work + 0x58) = 0x4E20;
        *(u32*)(work + 0x60) = wazaSequenceSysGetResID();
        *(u32*)(work + 0x5C) = wazaSequenceSysGetResID();

        {
            void* model;

            loadArchivePtr((u32)aligned, *(s32*)(params + 0x2C), 0x4E20, *(u32*)(work + 0x60));
            model = GSresGetResource(0x4E20, *(u32*)(work + 0x60));
            loadModel((u32)model, 0x4E20, 0, *(u32*)(work + 0x5C));
            model = GSresGetResource(0x4E20, *(u32*)(work + 0x5C));
            if (model != 0) {
                GSmodelSetVisibility(model, 0);
            }
        }
        /* RULE-EXCEPTION(user-approved): block-local carrier gives retail's add into a scratch register before the params copy - see docs/RULE_EXCEPTIONS.md */
        {
            u8* next = aligned + TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x2C));
            params = next;
        }
        break;

    case 8: {
        count = *(s32*)(params + 0x08);

        memset(work + 8, 0, 0x18);
        *(u32*)(work + 0x0C) = *(s32*)(params + 0x00);
        *(u32*)(work + 0x10) = *(s32*)(params + 0x04);
        switch (*(s32*)(params + 0x0C)) {
        case 2:
            data = params + 0x10;
            for (i = 0; i < (s32)count; i++, data += 0x10) {
                fn_8013D604(work + 8,
                            (s32)(((f32)(s32)fn_800D37CC() * (f32)*(s32*)(data + 0x08)) / 60.0f),
                            *(f32*)(data + 0x00),
                            *(f32*)(data + 0x04));
            }
            break;
        case 1: {
            u32 bits = *(s32*)(params + 0x08);
            f32 value = *(f32*)&bits;
            s32 duration = ((f32)(s32)fn_800D37CC() * (f32)frames) / 60.0f;
            s32 third = duration / 3;

            fn_8013D604(work + 8, third, 0.0f, value);
            fn_8013D604(work + 8, third, value, value);
            fn_8013D604(work + 8, third, 0.0f, 0.0f);
            break;
        }
        }
        /* Retail stores the cursor for every inner case; mode 1 never sets
         * it (the original source leaves data unset there). */
        params = data;
        break;
    }

    case 9:
        params = fn_80137114(work + 8, params, frames);
        break;

    case 10:
        memset(work + 8, 0, 0x34);
        *(u16*)(work + 0x3A) = TRACEFX_FRAME_DURATION(frames);
        *(f32*)(work + 0x24) = *(f32*)(params + 0x18);
        *(f32*)(work + 0x28) = *(f32*)(params + 0x1C);
        *(f32*)(work + 0x2C) = *(f32*)(params + 0x20);
        *(u8*)(work + 0x20) = *(s32*)(params + 0x04);
        *(u8*)(work + 0x21) = 0;
        if (*(s32*)(params + 0x0C) != 0) {
            *(u8*)(work + 0x21) |= 0x02;
        }
        if (*(s32*)(params + 0x14) != 0) {
            *(u8*)(work + 0x21) |= 0x08;
        }
        if (*(s32*)(params + 0x08) != 0) {
            *(u8*)(work + 0x21) |= 0x04;
        }
        if (*(s32*)(params + 0x10) != 0) {
            *(u8*)(work + 0x21) |= 0x01;
        }
        *(s8*)(work + 0x13) = (s8)(*(s32*)((u8*)params + 0x00) >> 24);
        *(u8*)(work + 0x12) = *(u32*)((u8*)params + 0x00) >> 16;
        *(u8*)(work + 0x11) = *(u32*)((u8*)params + 0x00) >> 8;
        *(u8*)(work + 0x10) = *(u32*)((u8*)params + 0x00);
        params += 0x28;
        break;

    case 11:
    {
        u8* cursor;

        memset(work + 8, 0, 0x24);
        *(u16*)(work + 0x24) = *(s32*)(params + 0x08);
        switch (*(s32*)(params + 0x10)) {
        case 0:
            *(u32*)(work + 0x20) = 0;
            break;
        case 1:
            *(u32*)(work + 0x20) = 1;
            break;
        }
        *(u16*)(work + 0x26) = *(s32*)(params + 0x0C);
        cursor = TRACEFX_ALIGN32(params + 0x18);
        *(u32*)(work + 0x08) = 0x4E20;

        if (*(s32*)(params + 0x00) != 0) {
            void* model;

            *(u32*)(work + 0x0C) = wazaSequenceSysGetResID();
            *(u32*)(work + 0x10) = wazaSequenceSysGetResID();
            loadArchivePtr((u32)cursor, *(s32*)(params + 0x00), 0x4E20, *(u32*)(work + 0x0C));
            model = GSresGetResource(0x4E20, *(u32*)(work + 0x0C));
            loadModel((u32)model, 0x4E20, 0, *(u32*)(work + 0x10));
            model = GSresGetResource(0x4E20, *(u32*)(work + 0x10));
            if (model != 0) {
                GSmodelSetVisibility(model, 0);
            }
            cursor += TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x00));
        } else {
            *(u32*)(work + 0x0C) = 0;
            *(u32*)(work + 0x10) = 0;
        }
        *(u32*)(work + 0x14) = 0;
        params = cursor;
        break;
    }

    case 12:
        offset = 0;
        memset(work + 8, 0, 0xB4);
        switch (*(s32*)(params + 0x14)) {
        case 1:
            *(f32*)(work + 0x24) = 1.0f;
            offset = -4;
            break;
        case 2:
            *(f32*)(work + 0x24) = *(f32*)(params + 0x18);
            break;
        }
        *(u16*)(work + 0xBA) = TRACEFX_FRAME_DURATION(frames);
        *(f32*)(work + 0x20) = *(f32*)(params + 0x08);
        *(f32*)(work + 0x1C) = *(f32*)(work + 0x20);
        *(f32*)(work + 0x2C) = *(f32*)(params + 0x0C);
        *(u16*)(work + 0x30) = *(s32*)(params + 0x00);
        *(u16*)(work + 0x32) = *(s32*)(params + 0x04);

        aligned = (u8*)(offset + (u32)params);
        aligned = TRACEFX_ALIGN32(aligned + 0x1C);
        handle = fn_800E2C04(TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x10)), 0x20);
        if (handle != 0) {
            copy = fn_800E27B0(handle);
            memcpy(copy, aligned, TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x10)));
            *(u32*)(work + 0x0C) = (u32)GStextureLoad(copy);
            fn_800EFD14(*(void**)(work + 0x0C), handle);
        } else {
            *(u32*)(work + 0x0C) = 0;
        }
        aligned += TRACEFX_ALIGN32_SIZE(*(s32*)(params + 0x10));
        params = aligned;
        break;

    default:
        break;
    }

    return params;

#undef TRACEFX_FRAME_DURATION
}
#pragma pop

/* ===================================================================
 * Generated: 0 pattern-matched + 11 stubs
 * Range: 0x801364A8 - 0x801380D4
 * =================================================================== */

/* 0x80137114 | 0x248 */
u8* fn_80137114(u8* work, u8* params, s32 frames) {
    extern void fn_8013DB64(u8*, s32, f32, f32);
    s32 i;
    s32 duration;
    s32 count;
    u8* data;
    f32 value;

    count = *(s32*)params;
    memset(work, 0, 0x20);

    switch (*(s32*)(params + 4)) {
    case 3:
        *(u32*)(work + 0x10) = *(u32*)(params + 8);
        data = params + 0x0C;
        for (i = 0; i < count; i++, data += 0x10) {
            duration = ((f32)*(s32*)(data + 8) * (f32)(s32)fn_800D37CC()) / 60.0f;
            fn_8013DB64(work, duration, *(f32*)data, *(f32*)(data + 4));
        }
        break;
    case 2:
        *(u32*)(work + 0x10) = 1;
        data = params + 8;
        for (i = 0; i < count; i++, data += 0x10) {
            duration = ((f32)*(s32*)(data + 8) * (f32)(s32)fn_800D37CC()) / 60.0f;
            fn_8013DB64(work, duration, *(f32*)data, *(f32*)(data + 4));
        }
        break;
    case 1:
        *(u32*)(work + 0x10) = 1;
        {
            u32 bits = *(u32*)params;
            value = *(f32*)&bits;
        }
        frames = ((f32)frames * (f32)(s32)fn_800D37CC()) / 60.0f;
        duration = frames;
        duration = duration / 3;
        fn_8013DB64(work, duration, 0.0f, value);
        fn_8013DB64(work, duration, value, value);
        fn_8013DB64(work, duration, 0.0f, 0.0f);
        break;
    }
    return data;
}

/* 0x8013735C | 0x220  tracefxInit */
/* 86.32%: real correct C. Documented src-dup 5-reg stmw reg-coloring (target copies
 * params->r31 so r29 reuses for arena; CW coalescer keeps 4 regs) + float-const
 * band-isolation reloc (lbl_8047D118 @nn) = un-saveable in isolated band. equivalent.txt */
u8* fn_8013735C(u8* work, void* params, s32 frames) {
    u8* p = params;
    s32 memOffset = 0;
    void* model;

    memset(work, 0, 0xac);

    switch (*(u32*)(p + 0x3c)) {
    case 1:
        *(u32*)(work + 0xa8) = 0;
        memOffset = -4;
        break;
    case 2:
    default:
        *(u32*)(work + 0xa8) = *(u32*)(p + 0x40);
        break;
    }

    *(s16*)(work + 0xa6) =
        (s16)(((f32)frames * (f32)(s32)fn_800D37CC()) / 60.0f);

    *(f32*)(work + 0x48) = *(f32*)(p + 0x00);
    *(f32*)(work + 0x4c) = *(f32*)(p + 0x04);
    *(f32*)(work + 0x50) = *(f32*)(p + 0x08);

    *(s8*)(work + 0x63) = (s8)(*(s32*)(p + 0x0c) >> 24);
    *(u8*)(work + 0x62) = (u8)(*(u32*)(p + 0x0c) >> 16);
    *(u8*)(work + 0x61) = (u8)(*(u32*)(p + 0x0c) >> 8);
    *(s8*)(work + 0x60) = (s8)(*(u32*)(p + 0x0c));

    *(f32*)(work + 0x64) = *(f32*)(p + 0x10);
    *(f32*)(work + 0x68) = *(f32*)(p + 0x14);
    *(f32*)(work + 0x6c) = *(f32*)(p + 0x18);

    *(u16*)(work + 0x70) = (u16)*(u32*)(p + 0x1c);
    *(u16*)(work + 0x72) = (u16)*(u32*)(p + 0x20);
    if (*(u16*)(work + 0x70) % 2 == 0) {
        *(u16*)(work + 0x70) += 1;
    }
    if (*(u16*)(work + 0x72) % 2 == 0) {
        *(u16*)(work + 0x72) += 1;
    }

    *(f32*)(work + 0x90) = *(f32*)(p + 0x24);
    *(f32*)(work + 0x94) = *(f32*)(p + 0x28);
    *(f32*)(work + 0x98) = *(f32*)(p + 0x2c);
    *(f32*)(work + 0x9c) = *(f32*)(p + 0x30);
    *(f32*)(work + 0xa0) = *(f32*)(p + 0x34);

    params = (u8*)(memOffset + (u32)params);
    params = TRACEFX_ALIGN32((u8*)params + 0x44);

    *(u32*)(work + 0x74) = 0x4e20;
    *(u32*)(work + 0x7c) = wazaSequenceSysGetResID();
    *(u32*)(work + 0x78) = wazaSequenceSysGetResID();

    loadArchivePtr((u32)params, *(u32*)(p + 0x38), 0x4e20, *(u32*)(work + 0x7c));
    model = GSresGetResource(0x4e20, *(u32*)(work + 0x7c));
    loadModel((u32)model, 0x4e20, 0, *(u32*)(work + 0x78));
    model = GSresGetResource(0x4e20, *(u32*)(work + 0x78));
    if (model != NULL) {
        GSmodelSetVisibility(model, 0);
    }
    /* RULE-EXCEPTION(user-approved): MWCC cast-as-lvalue keeps the advance in params' register before the return copy - see docs/RULE_EXCEPTIONS.md */
    (u8*)params += TRACEFX_ALIGN32_SIZE(*(u32*)(p + 0x38));
    return params;
}

/* 0x8013757C | 0x204 */
u8* fn_8013757C(u8* work, void* params, s32 frames) {
    extern u16 fn_800E2C04(u32, u32);
    extern void* GStextureLoad(void*);
    extern void fn_800EFD14(void*, u16);
    extern void GStextureSetWrap(void*, s32, s32);
    extern u8 lbl_80314638[];
    extern u8 lbl_80314AE8[];
    u8* p = params;
    u16 handle;

    memset(work, 0, 0x70);

    *(u16*)(work + 0x12) =
        (u16)(((f32)frames * (f32)(s32)fn_800D37CC()) / 60.0f);

    work[0x23] = *(u32*)(p + 0) >> 24;
    work[0x22] = *(u32*)(p + 0) >> 16;
    work[0x21] = *(u32*)(p + 0) >> 8;
    work[0x20] = *(u32*)(p + 0);
    work[0x27] = *(u32*)(p + 4) >> 24;
    work[0x26] = *(u32*)(p + 4) >> 16;
    work[0x25] = *(u32*)(p + 4) >> 8;
    work[0x24] = *(u32*)(p + 4);
    GSvecCopy(work + 0x34, p + 0x0C);
    GSvecCopy(work + 0x40, p + 0x18);
    *(u16*)(work + 8) = *(u32*)(p + 8);
    *(f32*)(work + 0x4C) = *(f32*)(p + 0x24);
    *(f32*)(work + 0x50) = *(f32*)(p + 0x28);
    *(f32*)(work + 0x54) = *(f32*)(p + 0x2C);
    *(f32*)(work + 0x58) = *(f32*)(p + 0x30);
    *(f32*)(work + 0x5C) = *(f32*)(p + 0x34);
    *(f32*)(work + 0x60) = *(f32*)(p + 0x38);
    *(f32*)(work + 0x64) = *(f32*)(p + 0x3C);
    *(f32*)(work + 0x68) = *(f32*)(p + 0x40);
    *(f32*)(work + 0x6C) = *(f32*)(p + 0x44);
    *(u16*)(work + 0x0E) = *(u32*)(p + 0x48);

    params = TRACEFX_ALIGN32((u8*)params + 0x54);
    handle = fn_800E2C04(TRACEFX_ALIGN32_SIZE(*(u32*)(p + 0x4C)), 0x20);
    if (handle != 0) {
        void* copy = fn_800E27B0(handle);
        memcpy(copy, params, TRACEFX_ALIGN32_SIZE(*(u32*)(p + 0x4C)));
        *(void**)(work + 0x1C) = GStextureLoad(copy);
        fn_800EFD14(*(void**)(work + 0x1C), handle);
    } else {
        *(void**)(work + 0x1C) = NULL;
    }
    params = (u8*)params + TRACEFX_ALIGN32_SIZE(*(u32*)(p + 0x4C));
    if (*(void**)(work + 0x1C) != NULL) {
        GStextureSetWrap(*(void**)(work + 0x1C), 2, 2);
    }
    *(void**)(work + 0x14) = lbl_80314638;
    *(void**)(work + 0x18) = lbl_80314AE8;
    return params;
}

/* 0x80137780 | 0x1EC */
u8* fn_80137780(u8* work, u8* params) {
    /* RULE-EXCEPTION(user-approved): int stride local and combined advance chosen for register allocation (permuter farm) — see docs/RULE_EXCEPTIONS.md */
    int stride;
    typedef struct TraceColor {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    } TraceColor;
    extern void fn_8013AB60(u8*, TraceColor*, TraceColor*, s32);
    s32 i;
    TraceColor first;
    s32 count;
    s32 offset;
    TraceColor second;
    u8 useAlpha;
    u8 useRgb;
    u8* data;

    useAlpha = FALSE;
    useRgb = TRUE;
    count = *(s32*)(params + 4);
    memset(work, 0, 0x5C);
    work[0x4C] = *(u32*)params;
    switch (*(s32*)(params + 8)) {
    case 1:
    case 2:
        offset = -4;
        break;
    case 3:
    default:
        if ((*(s32*)(params + 0x0C) & 4) == 4) {
            useRgb = FALSE;
        }
        if ((*(s32*)(params + 0x0C) & 1) == 1) {
            useAlpha = TRUE;
        }
        offset = 0;
        break;
    }
    if (*(s32*)params == 0) {
        useRgb = TRUE;
        useAlpha = TRUE;
    }
    params = (u8*)(offset + (u32)params);
    work[0x4E] = useAlpha;
    stride = 0x10;
    data = (params += stride);

    for (i = 0; i < count; i++, data += 0x10) {
        if (useRgb) {
            first.b = *(u32*)(data + 0) >> 16;
            first.g = *(u32*)(data + 0) >> 8;
            first.r = *(u32*)(data + 0);
            second.b = *(u32*)(data + 4) >> 16;
            second.g = *(u32*)(data + 4) >> 8;
            second.r = *(u32*)(data + 4);
        } else {
            first.r = first.g = first.b = 0x7F;
            second.r = second.g = second.b = 0x7F;
        }
        if (useAlpha) {
            first.a = *(u32*)(data + 0) >> 24;
            second.a = *(u32*)(data + 4) >> 24;
        } else {
            first.a = 0xFF;
            second.a = 0xFF;
        }
        if (*(s32*)(data + 8) < 0) {
            fn_8013AB60(work, &first, &second, -1);
        } else {
            fn_8013AB60(work, &first, &second,
                        ((f32)*(s32*)(data + 8) * (f32)(s32)fn_800D37CC()) / 60.0f);
        }
    }
    return data;
}

#endif

/* TRACEFX_801364A8_ONLY (set by tracefx_r55_801364A8_gc13_o1.c) stops here:
 * that unit is 0x801364A8-0x8013796C. */
#if !defined(TRACEFX_801364A8_ONLY)

/* 0x8013796C | 0x78 */
void* fn_8013796C(void* owner) {
    void* obj = fn_80131428(owner, 0x2C);
    if (obj != (void*)0) {
        fn_80131200(obj, 0, fn_801379E4, tracefxStartEffect, fn_80137A2C, 0,
                    fn_80137D14, fn_80137F58);
        fn_8013139C(obj, 0);
    }
    return obj;
}

/* 0x801379E4 | 0x48 */
BOOL fn_801379E4(u8* w) {
    if (w != (void*)0) {
        GXDrawDone();
        fn_800B856C();
        if (*(void**)(w + 0x14) != (void*)0) {
            GStextureFree(*(void**)(w + 0x14));
        }
    }
    return TRUE;
}

/* 0x80137A2C | 0x78 */
BOOL fn_80137A2C(u8* w) {
    u16 handle;
    if (w != (void*)0) {
        GXDrawDone();
        fn_800B856C();
        handle = *(u16*)(w + 0x0C);
        if (handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
        handle = *(u16*)(w + 0x0E);
        if (handle != 0) {
            fn_800E24B0(handle);
            fn_800E209C(handle);
        }
    }
    return TRUE;
}

/* 0x80137AA4 | 0x270 */
BOOL tracefxStartEffect(u8* w) {
    typedef struct TraceNode {
        Vec start;
        Vec end;
        struct TraceNode* next;
        struct TraceNode* prev;
    } TraceNode;
    s32 last;
    u32 node_bytes;
    void* part;
    u8* weights;
    u16 max_count;
    u16 k;
    u16 handle;
    TraceNode* nodes;
    u32 count32;
    f32 step;
    void* model;
    u16 count;

    if (w == (void*)0) {
        goto fail;
    }

    max_count = *(u16*)(w + 0x20);
    count = *(u16*)(w + 0x22);
    model = GSresGetResource(*(u16*)(w + 0x24), *(u16*)(w + 0x26));

    if (*(void**)(w + 0x14) == (void*)0) {
        return FALSE;
    }
    if (count == 0 || max_count == 0) {
        return FALSE;
    }
    if (model == (void*)0) {
        return FALSE;
    }

    part = GSmodelGetPart(model, *(u16*)(w + 0x28));
    if (part == (void*)0) {
        return FALSE;
    }
    GSpartFree(part);

    part = GSmodelGetPart(model, *(u16*)(w + 0x2A));
    if (part == (void*)0) {
        return FALSE;
    }
    GSpartFree(part);

    if (count > (max_count >> 1)) {
        count = *(u16*)(w + 0x22) = max_count >> 1;
    }

    node_bytes = count << 5;
    count32 = count;
    handle = _toolentryAlloc__FUl(node_bytes);
    if (handle == 0) {
        return FALSE;
    }

    *(u16*)(w + 0x0C) = handle;
    nodes = (TraceNode*)fn_800E27B0(handle);
    *(TraceNode**)(w + 0x04) = nodes;
    *(TraceNode**)(w + 0x00) = nodes;
    memset(nodes, 0, node_bytes);

    last = count32 - 1;
    for (k = 0; k < count; k++) {
        if (k == 0) {
            nodes[k].prev = &nodes[last];
        } else {
            nodes[k].prev = &nodes[k - 1];
        }
        if (k == last) {
            nodes[k].next = &nodes[0];
        } else {
            nodes[k].next = &nodes[k + 1];
        }
    }

    handle = _toolentryAlloc__FUl(count32 << 4);
    if (handle == 0) {
        fn_800E24B0(*(u16*)(w + 0x0C));
        fn_800E209C(*(u16*)(w + 0x0C));
        return FALSE;
    }

    *(u16*)(w + 0x0E) = handle;
    *(u8**)(w + 0x08) = (u8*)fn_800E27B0(handle);
    *(u16*)(w + 0x1C) = 0;
    *(u16*)(w + 0x1E) = 0;

    weights = *(u8**)(w + 0x08);
    count32 = *(u16*)(w + 0x22);
    step = 1.0f / (f32)(s32)(count32 - 1);
    for (k = 0; k < count32; k++) {
        f32 t = (f32)k * step;
        *(f32*)(weights + 0x00) = t;
        *(f32*)(weights + 0x04) = 0.0f;
        *(f32*)(weights + 0x08) = t;
        *(f32*)(weights + 0x0C) = 1.0f;
        weights += 0x10;
    }

    return TRUE;

fail:
    GSlogWrite(lbl_80272B08);
    return FALSE;
}

/* 0x80137D14 | 0x244 */
BOOL fn_80137D14(u8* work, u32 steps) {
    u8* node;
    void* model;
    u32 i;
    void* part;
    Vec start;
    Vec end;
    Vec previousStart;
    Vec previousEnd;
    f32 fraction;

    if (*(u16*)(work + 0x1C) < *(u16*)(work + 0x20)) {
        model = GSresGetResource(*(u16*)(work + 0x24),
                                 *(u16*)(work + 0x26));
        if (model == NULL) {
            return FALSE;
        }
        if (*(void**)(work + 0x14) == NULL) {
            return FALSE;
        }
        part = GSmodelGetPart(model, *(u16*)(work + 0x28));
        if (part == NULL) {
            return FALSE;
        }
        GSpartGetTransform(part, &start, NULL, NULL);
        GSpartFree(part);
        part = GSmodelGetPart(model, *(u16*)(work + 0x2A));
        if (part == NULL) {
            return FALSE;
        }
        GSpartGetTransform(part, &end, NULL, NULL);
        GSpartFree(part);

        node = *(u8**)work;
        if (*(u16*)(work + 0x1C) != 0) {
            GSvecCopy(&previousStart, node);
            GSvecCopy(&previousEnd, node + 0x0C);
            fraction = 1.0f / (f32)steps;
        }
        for (i = 0; i < steps; i++) {
            node = *(u8**)(node + 0x18);
            if (*(u16*)(work + 0x1C) != 0) {
                f32 t = fraction * (f32)(i + 1);
                GSlerpGetLinearInterpolationVector(
                    node, &previousStart, &start, t);
                GSlerpGetLinearInterpolationVector(
                    node + 0x0C, &previousEnd, &end, t);
            } else {
                GSvecCopy(node, &start);
                GSvecCopy(node + 0x0C, &end);
            }
        }
        *(u8**)work = node;
        *(u16*)(work + 0x1C) += steps;
        *(u16*)(work + 0x1E) += steps;
        if (*(u16*)(work + 0x1C) > *(u16*)(work + 0x20)) {
            *(u16*)(work + 0x1C) = *(u16*)(work + 0x20);
        }
        if (*(u16*)(work + 0x1E) > *(u16*)(work + 0x22)) {
            *(u16*)(work + 0x1E) = *(u16*)(work + 0x22);
        }
        if (*(u16*)(work + 0x1E) >
            *(u16*)(work + 0x20) - *(u16*)(work + 0x1C)) {
            *(u16*)(work + 0x1E) =
                *(u16*)(work + 0x20) - *(u16*)(work + 0x1C);
        }
        return TRUE;
    }
    return FALSE;
}

/* 0x80137F58 | 0x17C */
int fn_80137F58(u8* w) {
    u16 count1;
    u16 count2;
    u8* node;
    u8* weight;
    u32 i;

    count1 = *(u16*)(w + 0x1C);
    count2 = *(u16*)(w + 0x1E);
    if (*(void**)(w + 0x14) == (void*)0) {
        return 0;
    }
    if (count1 <= 1) goto ret0;
    if (count2 <= 1) goto ret0;

    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    gfxSetBlendMode(1, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(1, 2, 1);
    gfxSetCullMode(0);
    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxSetVtxDesc(*(void**)(w + 0x10));
    gfxLoadTexture(0, *(void**)(w + 0x14));
    gfxSetPrimitive(4);
    gfxBegin((count2 & 0x7FFF) << 1);

    node = *(u8**)(w + 0x00);
    weight = *(u8**)(w + 0x08) + (*(u16*)(w + 0x22) - count2) * 16;
    for (i = 0; (u16)i < count2; i++) {
        gfxVtxPos(*(f32*)(node + 0x00), *(f32*)(node + 0x04), *(f32*)(node + 0x08));
        fn_800D5CB8(0, w[0x18], w[0x19], w[0x1A], w[0x1B]);
        gfxVtxTexCoord(0, *(f32*)(weight + 0x00), *(f32*)(weight + 0x04));
        gfxVtxPos(*(f32*)(node + 0x0C), *(f32*)(node + 0x10), *(f32*)(node + 0x14));
        fn_800D5CB8(0, w[0x18], w[0x19], w[0x1A], w[0x1B]);
        gfxVtxTexCoord(0, *(f32*)(weight + 0x08), *(f32*)(weight + 0x0C));
        node = *(u8**)(node + 0x1C);
        weight += 0x10;
    }
    gfxEnd();
    return 1;
ret0:
    return 0;
}

#endif
