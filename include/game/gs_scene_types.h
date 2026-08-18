#ifndef GAME_GS_SCENE_TYPES_H
#define GAME_GS_SCENE_TYPES_H

/**
 * @file gs_scene_types.h
 * @brief Shared externs and types for the GSscene translation units split
 *        from the former game/gs_scene.c CodeCandidate bucket
 *        (0x8017572C - 0x8017A5FC): game/ps_generator_range_8017572C.c,
 *        game/gs_xfb_capture.c, game/gs_spline.c and game/camera.c.
 *
 * All functions declared here are asm-only/partially-decompiled
 * CodeCandidate units (not linked); this header exists purely so each
 * split TU can see the shared engine externs and BSS globals without
 * per-file duplication drift.
 */

#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 size);

/* GSmem */
/* _toolentryAlloc__FUl (GSmemAllocRaw) is declared per-TU: retail codegen
 * depends on whether the caller sees the u16 handle or the raw u32 return. */
/* fn_800E27B0 (GSmemGetPtr) is likewise declared per-TU. */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */

/* GSgfx math/render */
extern void  set__5GSvecFfff(void* out, f32 angle, f32 a, f32 b); /* rotation matrix */
extern void  GSmtxMakeXRotation(void* out, f32 angle);
extern void  GSmtxMakeYRotation(void* out, f32 angle);         /* angle to vector */
extern void  fn_800E0108(void* dst, void* lhs, void* rhs);
extern void  fn_800E0238(void* dst, void* src);
extern void  fn_800E02E8(void* matrix, f32 angle);
extern void  fn_800E032C(void* matrix, f32 angle);
extern void  GSvecAdd(void* out, void* a, void* b);  /* cross product */
extern void  GSvecTransform(void* out, void* a, void* b);  /* vector subtract */
extern void  GScameraSetPosition(void* obj, void* mtx);         /* set model matrix */
extern void* fn_800FF56C(void);                         /* GSfloor get active */

/* Script/generator */
extern void  fn_80169520(void* obj);                    /* status flag update */
extern void  fn_8016A644(void* obj);                    /* resource cleanup */
extern void  fn_800D305C(s32 param);                     /* gs_render_util: set render mode */
extern void  fn_800E24B0(u16 handle);                  /* GSmemLock */
extern void  GSvecCopy(void* dst, void* src);
extern u32  menuIsCheck(u32 a);
extern void menuOpenCustom(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, ...);
extern void fn_800D13C8(void* a, void* b);
extern void fn_800D258C(void* a);
extern void fn_800D1674(void* a, void* b);
extern void _threadSwitch(void);
extern void* GSresGetResource(u32 a, u32 b);
extern void* fn_800F92D4(u32 a);
extern void GScameraStopAnimation(void* a);
extern void GSmodelGetPosition(void* a, void* b);

/* ===== String constants (rodata) ===== */
extern const char lbl_80273A00[]; /* "gs%04d.xfb" */
extern const struct GSSceneVec3 lbl_80273DC8; /* vector prefix of rodata blob */

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
extern u16   lbl_8047B1AC;  /* floor-entry table alloc handle */

extern u32   lbl_80478C4C;  /* scene state word */

/* sdata2 float constants */
extern f32   lbl_8047B6A0;  /* float constant (likely 0.0 or 1.0) */

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

/* ===================================================================
 * Camera vectors shared by the camera state and its public accessors.
 * The full saved camera-state prefix is defined in camera_types.h.
 * =================================================================== */
typedef struct GSSceneVec3 {
    f32 x;
    f32 y;
    f32 z;
} GSSceneVec3;

#endif /* GAME_GS_SCENE_TYPES_H */
