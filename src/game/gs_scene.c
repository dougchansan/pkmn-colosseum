/**
 * @file gs_scene.c
 * @brief GSscene -- Scene object lifecycle, XFB capture, and resource management.
 *
 * This module sits between the generator system (0x8017572C) and FSYS
 * (0x8017AC40) in the link order. It manages scene-level objects,
 * framebuffer captures, and resource scheduling.
 *
 * Decompiled from 78 functions in range 0x8017572C - 0x8017A5FC.
 *
 * Selected functions:
 *   fn_8017572C (GSscene_ProcessFreeList)
 *   fn_801758D8 (GSscene_UpdateActive)
 *   fn_80175A1C (GSscene_SpawnObject)
 *   fn_80175B94 (GSscene_DespawnObject)
 *   fn_80175DF0 (GSscene_FindObject)
 *   genPosUpdate (GSscene_GetObjectByHandle)
 *   fn_80175F44 (GSscene_GetObjectCount)
 *   fn_80175F6C (GSscene_SetObjectCallback)
 *   fn_801760C4 (GSscene_AttachToParent)
 *   fn_80176228 (GSscene_DetachFromParent)
 *   fn_801765F4 (GSscene_NopAccessor1)
 *   fn_80176600-80176690 (GSscene_Get/SetField accessors)
 *   fn_801766A8 (GSscene_SetPosition)
 *   fn_80176758 (GSscene_SetRotation)
 *   fn_801767E0 (GSscene_SetScale)
 *   fn_80176868 (GSscene_SetColor)
 *   fn_801768F0 (GSscene_GetTransform)
 *   fn_80176948-801769B0 (GSscene_Get X/Y/Z position)
 *   fn_801769E4 (GSscene_SetVisible)
 *   fn_80176A44 (GSscene_GetVisible)
 *   fn_80176AE4 (GSscene_ComputeWorldTransform)
 *   fn_80176B48 (GSscene_UpdateTransformHierarchy)
 *   fn_80176C04 (GSscene_GetWorldPosition)
 *   fn_80176C78 (GSscene_Render)
 *   fn_80176E0C (GSscene_RenderChildren)
 *   fn_80177004 (GSscene_SetupRenderState)
 *   fn_8017707C (GSscene_MainUpdate)
 *   fn_801773F4-80177670 (GSscene_Get/Set animation params)
 *   fn_80177760 (GSscene_PlayAnimation)
 *   0x80177830-0x801779EC (GSscene_Get/Set small accessors)
 *   fn_80177A64 (GSscene_XFBCapture -- 3064 bytes, HUGE)
 *   fn_8017865C (GSscene_XFBSetupCapture)
 *   fn_801786F4 (GSscene_XFBProcess)
 *   fn_80178AA8 (GSscene_CameraUpdate)
 *   fn_80179020 (GSscene_CameraInterpolate)
 *   fn_80179404 (GSscene_CameraSetTarget)
 *   fn_801794F0 (GSscene_CameraSetPosition)
 *   fn_80179748 (GSscene_EnvironmentUpdate)
 *   fn_80179A18 (GSscene_LightingUpdate)
 *   fn_80179BEC (GSscene_FogUpdate)
 *   fn_80179FA4 (GSscene_Init -- 1624 bytes)
 *
 * The "gs%04d.xfb" string (lbl_80273A00) indicates this module can
 * capture the current framebuffer to numbered files, likely for
 * screenshot or debug purposes. The XFB capture function at
 * fn_80177A64 (3064 bytes) is the largest in this module.
 *
 * Code patterns:
 *   - Linked-list object management (ptr at offset 0x00 = next)
 *   - Free list at lbl_8047B18C (sda21)
 *   - Active list at lbl_8047B188 (sda21)
 *   - Object count at lbl_8047B118 (sda21, u16)
 *   - Object entry size 0x50+ bytes (offsets seen up to 0x50)
 *   - fn_80169520 called for status updates
 *   - fn_8016A644 called for resource cleanup
 *   - Calls to fn_800E01F4, fn_800E0518, fn_800E019C, fn_800DFF98
 *     (GSgfx vector/matrix operations)
 *   - Camera state at lbl_80478C40 (sda21)
 *   - fn_800FF56C (GSfloor get active) called from camera code
 *   - GScameraSetPosition, GScameraLookAt, fn_800CE2D8 (trig/angle functions)
 *
 * Debug strings:
 *   "gs%04d.xfb"
 *
 * Address range: 0x8017572C - 0x8017A5FC (22KB, 78 functions)
 */

#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);         /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 size);

/* GSmem */
extern u16   fn_800E3534(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */

/* GSgfx math/render */
extern void  fn_800E01F4(void* out, f32 angle, f32 a, f32 b); /* rotation matrix */
extern void  fn_800E0518(void* out, f32 angle);         /* angle to vector */
extern void  fn_800E019C(void* out, void* a, void* b);  /* cross product */
extern void  fn_800DFF98(void* out, void* a, void* b);  /* vector subtract */
extern void  GScameraSetPosition(void* obj, void* mtx);         /* set model matrix */
extern void  GScameraLookAt(void* obj, void* tbl, void* pos); /* set joint pos */
extern f32   fn_800CE2D8(f32 x, f32 y);                /* atan2 */
extern void* fn_800FF56C(void);                         /* GSfloor get active */

/* Script/generator */
extern void  fn_80169520(void* obj);                    /* status flag update */
extern void  fn_8016A644(void* obj);                    /* resource cleanup */
extern void  fn_800D305C(s32 param);                     /* gs_render_util: set render mode */
extern void  fn_800E24B0(u16 handle);                  /* GSmemLock */
extern void  fn_800E01D0(void* dst, void* src);
extern u32  fn_80102620(u32 a);
extern void fn_801026A4(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, ...);
extern void fn_800D13C8(void* a, void* b);
extern void fn_800D258C(void* a);
extern void fn_800D1674(void* a, void* b);
extern void GScameraSetRotation(void* a, void* b);
extern void _threadSwitch(void);
extern void* fn_800F9318(u32 a, u32 b);
extern void* fn_800F92D4(u32 a);
extern void GScameraStopAnimation(void* a);
extern void fn_800E3D98(void* a, void* b);

/* ===== String constants (rodata) ===== */
extern const char lbl_80273A00[]; /* "gs%04d.xfb" */
extern u32 lbl_80273DC8[];       /* 3-word animation param table */

/* ===== BSS / global state (sda21) ===== */
extern void* lbl_8047B188;  /* active object list head */
extern void* lbl_8047B18C;  /* free object list head */
extern void* lbl_8047B184;  /* current iteration pointer */
extern u16   lbl_8047B118;  /* active object count */
extern void* lbl_80478C40;  /* camera state pointer */
extern void* lbl_80478FB8;  /* scene render table */
extern void* lbl_8047B1A8;  /* scene object table */
extern u16   lbl_8047B1A2;  /* scene param halfword */
extern u32   lbl_8047B1A4;  /* scene param word */
extern u32   lbl_8047B1AC;  /* scene state param */

extern u32   lbl_80478C4C;  /* scene state word */

/* sdata2 float constants */
extern f32   lbl_8047B6A0;  /* float constant (likely 0.0 or 1.0) */
extern f32   lbl_8047D740;  /* float constant (sdata2) */

/* ===================================================================
 * Scene object structure (inferred from disassembly)
 * =================================================================== */

typedef struct GSSceneObject {
    /* 0x00 */ struct GSSceneObject* next;  /**< linked list next */
    /* 0x04 */ void* parent;               /**< parent object */
    /* 0x08 */ f32 posX;                   /**< local position X */
    /* 0x0C */ f32 posY;
    /* 0x10 */ f32 posZ;                   /**< local position Z */
    /* 0x12 */ u16 flags;                  /**< object flags (packed bits) */
    /* 0x14 */ f32 rotY;                   /**< Y rotation (heading) */
    /* 0x18 */ f32 pad18[2];
    /* 0x20 */ f32 animParam;
    /* 0x24 */ u8  pad24[0x28];
    /* 0x4C */ void* attachedResource;     /**< attached resource ptr */
    /* 0x50 */ void* attachedModel;        /**< attached model ptr */
} GSSceneObject;

/* ===================================================================
 * Scene render entry (0x28 bytes, used in tables)
 * =================================================================== */
typedef struct GSSceneRenderEntry {
    /* 0x00 */ u32  count;
    /* 0x04 */ void* objectPtr;
    /* 0x08 */ u8   pad[0x20];
} GSSceneRenderEntry;

/* ==================================================================
 * fn_8017572C -- GSscene_ProcessFreeList
 *
 * Process the scene object free list. Iterates through the active
 * list, checks for objects that should be freed, and moves them
 * to the free list.
 *
 * From disassembly (0x8017572C, 0x1AC bytes):
 *   lwz r31, lbl_8047B188@sda21(r0)  ; active list head
 *   ; ... iterate, compare, unlink, move to free list
 *   lhz r3, lbl_8047B118@sda21(r0)   ; decrement count
 *   subi r0, r3, 0x1
 *   sth r0, lbl_8047B118@sda21(r0)
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSscene_ProcessFreeList(void) {
    /* TODO: match -- 428 bytes at 0x8017572C */
}
#pragma pop

/* ==================================================================
 * fn_80175B94 -- GSscene_SpawnObject
 *
 * Spawn a new scene object. Allocates from the free list, initializes
 * fields, and adds to the active list. 604 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void* GSscene_SpawnObject(u32 type, u32 param) {
    /* TODO: match -- 604 bytes at 0x80175B94 */
}
#pragma pop

/* ==================================================================
 * fn_80177A64 -- GSscene_XFBCapture
 *
 * Capture the current framebuffer to a file. At 3064 bytes, this is
 * the largest function in the scene system. Uses the "gs%04d.xfb"
 * format string to generate numbered filenames.
 *
 * This is likely a debug/development feature for capturing screenshots.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSscene_XFBCapture(u32 captureIndex) {
    /* TODO: match -- 3064 bytes at 0x80177A64 */
}
#pragma pop

/* ==================================================================
 * fn_80178AA8 -- GSscene_CameraUpdate
 *
 * Update the scene camera. Processes camera position, target,
 * interpolation, and constraint calculations. 1400 bytes.
 *
 * Extensively references the camera state at lbl_80478C40:
 *   - offset 0x04-0x10: camera direction vector
 *   - offset 0x14: Y rotation angle
 *   - offset 0x1C-0x28: position components
 *   - offset 0x40: horizontal angle
 *   - offset 0x44: vertical angle
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSscene_CameraUpdate(void* sceneObj) {
    /* TODO: match -- 1400 bytes at 0x80178AA8 */
}
#pragma pop

/* ==================================================================
 * fn_80179020 -- GSscene_CameraInterpolate
 *
 * Interpolate the camera between two states. 996 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSscene_CameraInterpolate(void* camera) {
    /* TODO: match -- 996 bytes at 0x80179020 */
}
#pragma pop

/* ==================================================================
 * fn_80179FA4 -- GSscene_Init
 *
 * Initialize the scene system. Second-largest function at 1624 bytes.
 * Sets up the object free list, active list, camera state, render
 * tables, and initial scene configuration.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSscene_Init(void) {
    /* TODO: match -- 1624 bytes at 0x80179FA4 */
}
#pragma pop

/* ===== Small accessor/setter functions ===== */

/* fn_801765F4 -- nop accessor, 0xC bytes */
void* GSscene_NopAccessor1(void) {
    return (void*)0;
}

/* fn_80175F44 -- get object count, 0x28 bytes */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 GSscene_GetObjectCount(void) {
    /* TODO: match -- 40 bytes at 0x80175F44 */
}
#pragma pop

/* ==================================================================
 * Position accessors (fn_80176948-801769B0)
 *
 * Three 0x34-byte functions that return X, Y, Z position of a scene
 * object. Pattern:
 *   load object pointer from table
 *   lfs fX, offset(r3)
 *   blr
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off

f32 GSscene_GetPositionX(void* obj) {
    /* TODO: match -- 52 bytes at 0x80176948 */
}

f32 GSscene_GetPositionY(void* obj) {
    /* TODO: match -- 52 bytes at 0x8017697C */
}

f32 GSscene_GetPositionZ(void* obj) {
    /* TODO: match -- 52 bytes at 0x801769B0 */
}

#pragma pop

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 2 functions matched
 * =================================================================== */

extern u8 lbl_8047B1A0;

/* Address: 0x80175FFC | Size: 0x8 | Pattern: sda_getter */
u8 fn_80175FFC(void) {
    return lbl_8047B1A0;
}

/* Address: 0x80179DFC | Size: 0x8 | Pattern: return_constant */
u32 fn_80179DFC(void) { return 428; }
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80176004(void) {
#include "src/game/gs_scene_fn_80176004.inc"
}
#else
void fn_80176004(void) {
    lbl_8047B1A0 = 0;
    fn_800D305C(1);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80176030(void) {
#include "src/game/gs_scene_fn_80176030.inc"
}
#else
#pragma optimization_level 4
#pragma optimization_level 4
void fn_80176030(u32 param) {
    lbl_8047B1A2 = param;
    lbl_8047B1A0 = 1;
    lbl_8047B1A4 = 0;
    fn_800D305C(0);
}
#pragma optimization_level 0
#pragma optimization_level 0
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80176068(void) {
#include "src/game/gs_scene_fn_80176068.inc"
}
#else
#pragma optimization_level 4
void fn_80176068(u8* ptr) {
    u16 handle;
    if (ptr != NULL) {
        handle = *(u16*)(ptr + 0x22);
        fn_800E24B0(handle);
        fn_800E209C(handle);
        handle = *(u16*)(ptr + 0x20);
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
}
#endif
#pragma pop
#if 0
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void fn_8017662C(void) {
#include "src/game/gs_scene_fn_8017662C.inc"
}
#pragma pop
#else
void fn_8017662C(void* src) {
    fn_800E01D0((u8*)lbl_80478C40 + 0xE4, src);
}
#endif
#if 0
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void fn_80176658(void) {
#include "src/game/gs_scene_fn_80176658.inc"
}
#pragma pop
#else
void fn_80176658(void* src) {
    fn_800E01D0((u8*)lbl_80478C40 + 0xD8, src);
}
#endif
#if 0
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void fn_80176684(void) {
#include "src/game/gs_scene_fn_80176684.inc"
}
#pragma pop
#else
f32 fn_80176684(void) {
    return *(f32*)((u8*)lbl_80478C40 + 0x14);
}
#endif
#if 0
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void fn_8017669C(void) {
#include "src/game/gs_scene_fn_8017669C.inc"
}
#pragma pop
#else
f32 fn_8017669C(void) {
    return *(f32*)((u8*)lbl_80478C40 + 0x40);
}
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80176A94(void) {
#include "src/game/gs_scene_fn_80176A94.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_80176A94(void) {
    void* p;
    void* r;
    p = lbl_80478C40;
    r = fn_800F9318(*(u32*)((u8*)p + 0xD0), *(u32*)((u8*)p + 0xD4));
    if (r == 0)
        r = fn_800F92D4(*(u32*)((u8*)p + 0xD4));
    if (r != 0)
        GScameraStopAnimation(r);
}
#pragma pop
#endif
#if 0
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void fn_80176F68(void) {
#include "src/game/gs_scene_fn_80176F68.inc"
}
#pragma pop
#else
void fn_80176F68(void) {
    *((u8*)lbl_80478C40 + 0x4C) = 0;
    *((u8*)lbl_80478C40 + 0x4D) = 0;
    *((u8*)lbl_80478C40 + 0x4E) = 0;
    *((u8*)lbl_80478C40 + 0x4F) = 0;
    *((u8*)lbl_80478C40 + 0x01) = 0;
}
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80176F98(void) {
#include "src/game/gs_scene_fn_80176F98.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
u32 fn_80176F98(u8 param) {
    void* p;
    for (;;) {
        p = lbl_80478C40;
        if (*((u8*)p + 0x4C) != 0) goto nonzero;
        if (*((u8*)p + 0x4E) != 0) goto nonzero;
        if (*((u8*)p + 0x4F) != 0) goto nonzero;
        return 0;
    nonzero:
        if (param != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80177478(void) {
#include "src/game/gs_scene_fn_80177478.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_80177478(void* unused, void* src, f32 param) {
    void* p;
    p = lbl_80478C40;
    *((u8*)p + 0x01) = 1;
    p = lbl_80478C40;
    fn_800E01D0((u8*)p + 0xB0, src);
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0xCC) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0xC8) = param;
        q = lbl_80478C40;
        *((u8*)q + 0x4F) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0xBC, (u8*)q + 0x10);
    }
}
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801774F0(void) {
#include "src/game/gs_scene_fn_801774F0.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_801774F0(f32 a, f32 b, f32 c, f32 angle) {
    f32 tmp[3];
    void* p;
    fn_800E01F4(tmp, a, b, c);
    p = lbl_80478C40;
    *((u8*)p + 0x01) = 1;
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x90, tmp);
    }
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0xAC) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0xA8) = angle;
        q = lbl_80478C40;
        *((u8*)q + 0x4E) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x9C, (u8*)q + 0x4);
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80177574(void) {
#include "src/game/gs_scene_fn_80177574.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_80177574(void* unused, void* src, f32 param) {
    void* p;
    p = lbl_80478C40;
    *((u8*)p + 0x01) = 1;
    p = lbl_80478C40;
    fn_800E01D0((u8*)p + 0x90, src);
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0xAC) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0xA8) = param;
        q = lbl_80478C40;
        *((u8*)q + 0x4E) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x9C, (u8*)q + 0x4);
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801775EC(void) {
#include "src/game/gs_scene_fn_801775EC.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_801775EC(f32 a, f32 b, f32 c, f32 angle) {
    f32 tmp[3];
    void* p;
    fn_800E01F4(tmp, a, b, c);
    p = lbl_80478C40;
    *((u8*)p + 0x01) = 1;
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x50, tmp);
    }
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0x6C) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0x68) = angle;
        q = lbl_80478C40;
        *((u8*)q + 0x4C) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x5C, (u8*)q + 0x1C);
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801776E8(void) {
#include "src/game/gs_scene_fn_801776E8.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_801776E8(void* unused, void* src, f32 param) {
    void* p;
    p = lbl_80478C40;
    *((u8*)p + 0x01) = 1;
    p = lbl_80478C40;
    fn_800E01D0((u8*)p + 0x50, src);
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0x6C) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0x68) = param;
        q = lbl_80478C40;
        *((u8*)q + 0x4C) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x5C, (u8*)q + 0x1C);
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80177670(void) {
#include "src/game/gs_scene_fn_80177670.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_80177670(void* unused, void* src, f32 param) {
    void* p;
    p = lbl_80478C40;
    *((u8*)p + 0x01) = 1;
    p = lbl_80478C40;
    fn_800E01D0((u8*)p + 0x70, src);
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0x8C) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0x88) = param;
        q = lbl_80478C40;
        *((u8*)q + 0x4D) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x7C, (u8*)q + 0x28);
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80177760(void) {
#include "src/game/gs_scene_fn_80177760.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_80177760(void* unused, u32 a, u32 b, f32 param) {
    u32 local[3];
    void* handle;
    local[0] = lbl_80273DC8[0];
    local[1] = lbl_80273DC8[1];
    local[2] = lbl_80273DC8[2];
    {
        void* p = lbl_80478C40;
        *(u32*)((u8*)p + 0x34) = a;
        p = lbl_80478C40;
        *(u32*)((u8*)p + 0x38) = b;
        p = lbl_80478C40;
        *(u32*)((u8*)p + 0x3C) = (u32)-1;
    }
    handle = fn_800F9318(a, b);
    if (handle != 0) {
        fn_800E3D98(handle, local);
    }
    {
        void* p = lbl_80478C40;
        *((u8*)p + 0x01) = 1;
        p = lbl_80478C40;
        fn_800E01D0((u8*)p + 0x50, local);
    }
    {
        void* q = lbl_80478C40;
        *(f32*)((u8*)q + 0x6C) = lbl_8047D740;
        q = lbl_80478C40;
        *(f32*)((u8*)q + 0x68) = param;
        q = lbl_80478C40;
        *((u8*)q + 0x4C) = 1;
    }
    {
        void* q = lbl_80478C40;
        fn_800E01D0((u8*)q + 0x5C, (u8*)q + 0x1C);
    }
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_GetCameraRotationVector(void) {
#include "src/game/gs_scene_fn_80177830.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_GetCameraRotationVector(void* dst) {
    fn_800E01D0(dst, (u8*)lbl_80478C40 + 0x10);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_SetCameraRotationVector(void) {
#include "src/game/gs_scene_fn_80177858.inc"
}
#else
void GSscene_SetCameraRotationVector(void* src) {
    void* handle;
    handle = fn_800F9318(0, 0);
    fn_800E01D0((u8*)lbl_80478C40 + 0x10, src);
    GScameraSetRotation(handle, src);
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_GetCameraDirectionVector(void) {
#include "src/game/gs_scene_fn_801778B4.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_GetCameraDirectionVector(void* dst) {
    fn_800E01D0(dst, (u8*)lbl_80478C40 + 0x4);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_SetCameraDirectionVector(void) {
#include "src/game/gs_scene_fn_801778DC.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_SetCameraDirectionVector(void* src) {
    fn_800E01D0((u8*)lbl_80478C40 + 0x4, src);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_GetCameraPositionVector(void) {
#include "src/game/gs_scene_fn_80177908.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_GetCameraPositionVector(void* dst) {
    fn_800E01D0(dst, (u8*)lbl_80478C40 + 0x1C);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_SetCameraPositionVector(void) {
#include "src/game/gs_scene_fn_80177930.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_SetCameraPositionVector(void* src) {
    fn_800E01D0((u8*)lbl_80478C40 + 0x1C, src);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_GetCameraViewVector(void) {
#include "src/game/gs_scene_fn_8017795C.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_GetCameraViewVector(void* dst) {
    fn_800E01D0(dst, (u8*)lbl_80478C40 + 0x28);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_SetCameraViewVector(void) {
#include "src/game/gs_scene_fn_80177984.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void GSscene_SetCameraViewVector(void* src) {
    fn_800E01D0((u8*)lbl_80478C40 + 0x28, src);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801779B0(void) {
#include "src/game/gs_scene_fn_801779B0.inc"
}
#else
#pragma push
#pragma optimization_level 1
void fn_801779B0(u32 a, u32 b, u32 c) {
    *(u32*)((u8*)lbl_80478C40 + 0x34) = a;
    *(u32*)((u8*)lbl_80478C40 + 0x38) = b;
    *(u32*)((u8*)lbl_80478C40 + 0x3C) = c;
}
#pragma pop
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_801779CC(void) {
#include "src/game/gs_scene_fn_801779CC.inc"
}
#else
#pragma push
#pragma optimization_level 1
void fn_801779CC(u32 a, u32 b) {
    *(u32*)((u8*)lbl_80478C40 + 0x34) = a;
    *(u32*)((u8*)lbl_80478C40 + 0x38) = b;
    *(u32*)((u8*)lbl_80478C40 + 0x3C) = (u32)-1;
}
#pragma pop
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void GSscene_GetMode(void) {
#include "src/game/gs_scene_fn_80177A38.inc"
}
#else
u32 GSscene_GetMode(void) {
    return *(u8*)lbl_80478C40;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
#if 0
asm void GSscene_SetMode(void) {
#include "src/game/gs_scene_fn_80177A44.inc"
}
#else
u32 GSscene_SetMode(u32 val) {
    u8* state = lbl_80478C40;
    u32 old;
    if (state[0] == (u8)val) return val;
    old = state[0];
    state[0] = val;
    return old;
}
#endif
#pragma pop
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80179E04(void) {
#include "src/game/gs_scene_fn_80179E04.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
#pragma push
#pragma optimization_level 1
void fn_80179E04(void* dst) {
    void* ptr;
    void* out;
    void* handle;
    u32 mode;
    out = dst;
    handle = fn_800F9318(0, 0);
    ptr = lbl_80478C40;
    mode = *(u8*)ptr;
    if (mode == 4 || mode == 8) {
        void* next;
        next = fn_800F9318(*(u32*)((u8*)ptr + 0xD0), *(u32*)((u8*)ptr + 0xD4));
        if (next == 0)
            next = fn_800F92D4(*(u32*)((u8*)ptr + 0xD4));
        handle = next;
    }
    memcpy(out, lbl_80478C40, 0xFC);
    fn_800D1674(handle, (u8*)out + 0xFC);
}
#pragma pop
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80179EA4(void) {
#include "src/game/gs_scene_fn_80179EA4.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 4
void fn_80179EA4(void* src) {
    u32 src_val;
    void* saved_src;
    void* ptr;
    void* handle;
    void* next;
    u8 r0;
    src_val = (u32)src;
    saved_src = (void*)src_val;
    memcpy(lbl_80478C40, saved_src, 0xFC);
    handle = fn_800F9318(0, 0);
    ptr = lbl_80478C40;
    r0 = *(u8*)ptr;
    if (r0 == 4 || r0 == 8) {
        next = fn_800F9318(*(u32*)((u8*)ptr + 0xD0), *(u32*)((u8*)ptr + 0xD4));
        if (next == 0) {
            next = fn_800F92D4(*(u32*)((u8*)ptr + 0xD4));
        }
        handle = next;
    }
    fn_800D13C8(handle, (u8*)saved_src + 0xFC);
    fn_800D258C(handle);
}
#pragma pop
#endif
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80179F4C(void) {
#include "src/game/gs_scene_fn_80179F4C.inc"
}
#pragma pop
#else
#pragma pop
#pragma push
#pragma optimization_level 1
void fn_80179F4C(u32 param) {
    volatile u32* saved = &param;

    lbl_80478C4C = *saved;
    if ((u8)fn_80102620(0xFE) == 0) {
        fn_801026A4(0xFE, 0, 0, 0, 0, 0);
    }
}
#pragma pop
#endif
