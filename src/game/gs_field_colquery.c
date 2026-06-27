/**
 * @file gs_field_colquery.c
 * @brief GSfield collision queries -- ray casts, sphere sweeps, plane tests.
 *
 * Decompiled from:
 *   fn_8010E138 (GSfield_RayCast)
 *   fn_8010E53C (GSfield_SphereSweep)
 *   fn_8010EB28 (GSfield_SweepAgainstMesh)
 *   fn_8010EFE4 (GSfield_LinePlaneTest)
 *   fn_8010F188 (GSfield_ArcTest)
 *   fn_8010F320 (GSfield_ArcTestReverse)
 *   fn_8010F4B8 (GSfield_PointInBounds)
 *   fn_8010F5A4 (GSfield_TriangleBarycentric)
 *   fn_8010F6A0 (GSfield_GetNearestTriangle)
 *   fn_8010F71C (GSfield_FindGroundHeight)
 *   fn_8010FA54 (GSfield_QueryGroundType)
 *   fn_8010FAF4 (GSfield_MultiRayCast)
 *   fn_8010FDF8 (GSfield_RayTriangleIntersect)
 *   fn_8010FFC4 (GSfield_SegmentTriTest)
 *   fn_80110084 (GSfield_TriNormalFromVerts)
 *   fn_801101B4 (GSfield_BuildCollisionGrid)
 *   fn_8011069C (GSfield_GridLookup)
 *   fn_80110E64 (GSfield_WalkableSurfaceQuery)
 *   fn_80111470 (GSfield_FloorHeightAtPoint)
 *   fn_8011163C (GSfield_HeightmapSample)
 *   fn_80111864 (GSfield_TerrainProbe)
 *   fn_80111B9C (GSfield_IsPointOnFloor)
 *   fn_80111C24 (GSfield_RegionBoundsTest)
 *   fn_80111DF8 (GSfield_ClipToFloorBounds)
 *   fn_80111F2C (GSfield_ProjectPointToFloor)
 *   fn_8011207C (GSfield_FindSpawnPoint)
 *   fn_80112260 (GSfield_GetSpawnPosition)
 *   fn_80112380 (GSfield_CheckReady)
 *   fn_801123D4 (GSfield_ResourceInit)
 *   fn_80112700 (GSfield_GetActiveFloorId)
 *   fn_8011274C (GSfield_FinalizeLoad)
 *   fn_80112780 (GSfield_BeginTransition)
 *   fn_801127BC (GSfield_SetTransitionCallback)
 *   fn_80112844 (GSfield_GetTransitionState)
 *   fn_8011288C (GSfield_IsTransitioning)
 *   fn_801128A0 (GSfield_ProcessTransition)
 *   fn_801129AC (GSfield_GetFieldState)
 *   fn_801129CC (GSfield_UpdateObjects)
 *   fn_80112F8C (GSfield_ClearObjectList)
 *   fn_80112FEC (GSfield_AddObject)
 *   fn_80113248 (GSfield_RemoveObject)
 *   fn_801134E4 (GSfield_MoveObject)
 *   fn_80113778 (GSfield_SetObjectVisible)
 *   fn_80113828 (GSfield_GetObjectPosition)
 *   fn_8011388C (GSfield_SetObjectPosition)
 *   fn_8011392C (GSfield_GetObjectRotX)
 *   fn_8011393C (GSfield_GetObjectRotY)
 *   fn_8011394C (GSfield_GetObjectRotZ)
 *   fn_8011395C (GSfield_GetObjectScale)
 *   fn_8011396C (GSfield_SetObjectRotation)
 *   fn_801139BC (GSfield_SetObjectScale)
 *   fn_80113A0C (GSfield_SpawnFieldModel)
 *   fn_80113B84 (GSfield_DespawnFieldModel)
 *   fn_80113D10 (GSfield_GetModelGroup)
 *   fn_80113D34 (GSfield_GetModelIndex)
 *   fn_80113D58 (GSfield_LoadFieldModel)
 *   fn_80113F48 (GSfield_UnloadFieldModel)
 *   fn_80113F6C (GSfield_SetModelAnimation)
 *   fn_80113FB4 (GSfield_GetModelAnimFrame)
 *   fn_80113FE8 (GSfield_PlayModelAnimation)
 *   fn_801140C8 (GSfield_StopModelAnimation)
 *   fn_801140DC (GSfield_SetModelMaterial)
 *   fn_8011416C (GSfield_GetModelMaterial)
 *   fn_8011418C (GSfield_SetModelTexture)
 *   fn_801141D8 (GSfield_GetModelTexture)
 *   fn_801141F8 (GSfield_AttachToJoint)
 *   fn_80114254 (GSfield_DetachFromJoint)
 *   fn_801142B4 (GSfield_GetJointPosition)
 *   fn_801142F8 (GSfield_GetJointCount)
 *
 * This module wraps GScolsys2 collision queries for use by the field/world
 * system. It transforms world-space coordinates through the collision mesh
 * matrix, performs triangle intersection tests, and returns hit results
 * sorted by distance.
 *
 * Key patterns observed in the disassembly:
 *   - Heavy use of fn_800A37CC (MTXMultVec3) for coordinate transforms
 *   - Calls to fn_8010C7BC (GScolsys2_QueryTriVisible) for per-tri checks
 *   - Calls to fn_8010CA30 / fn_8010C8D0 for forward/inverse transforms
 *   - Calls to fn_8010DEF0 (GScolsys2_TriangleBoundsCheck)
 *   - Float comparisons with parametric t values (ray intersection)
 *   - Iteration over triangle vertex arrays (stride 0x34 per triangle)
 *   - Result arrays capped at 8 entries (GSFIELD_MAX_QUERY_RESULTS)
 *
 * Address range: 0x8010E138 - 0x80114300
 */

#include "dolphin/types.h"
#include "game/world/gs_field.h"

/* ===== External SDK / engine functions ===== */
extern void  fn_800DD970(const char* fmt, ...);        /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);

/* Matrix / vector math helpers */
extern void  fn_800A37CC(void* mtxDst, void* vecSrc, void* vecDst);  /* MTXMultVec3 */
extern void  fn_800A3A78(void* vecA, void* scale, void* vecOut);     /* VEC scale */
extern void  fn_800A3A9C(void* a, void* b, void* out);             /* VEC diff/setup */
extern f32   fn_800A3B38(void* vec);                                /* VEC magnitude */
extern void  fn_800A3AC0(void* curve, void* paramOut, f32 t);       /* VEC lerp */

/* GScolsys2 functions */
extern void* fn_8010CBC0(void);                         /* GScolsys2_GetWZXData */
extern s32   fn_8010C7BC(u32 triIdx, void* outFlag);    /* GScolsys2_QueryTriVisible */
extern void  fn_8010CA30(void* mtxOut, u32 layerIdx);   /* GScolsys2_BuildInverseTransform */
extern void  fn_8010C8D0(void* mtxOut, u32 layerIdx);   /* GScolsys2_BuildTransform */
extern s32   fn_8010DEF0(void* result, void* origin,
                          void* verts, void* normal);    /* GScolsys2_TriangleBoundsCheck */

/* ===== String constants (rodata) ===== */
extern const char lbl_802720B0[]; /* "scene_data" */

/* ===== BSS / global state ===== */
extern u8 lbl_80404C68[];  /* GScolsys2 collision state */

/* ==================================================================
 * fn_8010E138 -- GSfield_RayCast
 *
 * Cast a ray against the active collision mesh. For each visible
 * triangle (checked via GScolsys2_QueryTriVisible), transforms
 * the triangle vertices through the inverse model-view matrix,
 * then tests ray-triangle intersection.
 *
 * Results are stored in a sorted array of GSFieldHitResult structs,
 * capped at 8 entries. The function returns the number of hits.
 *
 * Register usage (from disasm):
 *   r3 = origin (Vec3f*)
 *   r4 = direction (Vec3f* -- also used as extent)
 *   r25 = WZX data pointer (from fn_8010CBC0)
 *   r26 = triangle vertex stride 0x34
 *   r24 = hit count
 *   r28 = current triangle index
 *   f1  = parametric t for current hit
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSfield_RayCast(void* origin, void* direction) {
    /* TODO: match -- 824 bytes at 0x8010E138 */
    /* Calls: fn_8010CBC0, fn_8010C7BC, fn_8010CA30, fn_8010C8D0,
     *        fn_800A37CC, fn_8010DEF0 */
}
#pragma pop

/* ==================================================================
 * fn_8010E53C -- GSfield_SphereSweep
 *
 * Swept-sphere collision test (1516 bytes). Larger than RayCast
 * because it expands each triangle by the sphere radius before
 * performing the intersection test.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSfield_SphereSweep(void* origin, void* direction, f32 radius) {
    /* TODO: match -- 1516 bytes at 0x8010E53C */
}
#pragma pop

/* ==================================================================
 * fn_8010EB28 -- GSfield_SweepAgainstMesh
 *
 * Lower-level sweep test against a specific collision mesh subset.
 * 1212 bytes, heavy float math.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSfield_SweepAgainstMesh(void* meshData, void* sweep, void* result) {
    /* TODO: match -- 1212 bytes at 0x8010EB28 */
}
#pragma pop

/* ==================================================================
 * fn_8010EFE4 -- GSfield_LinePlaneTest
 *
 * Test a line segment against a collision plane.
 * 420 bytes. Used by both RayCast and arc tests.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSfield_LinePlaneTest(void* segStart, void* segEnd,
                           void* planeNormal, s32 doubleSided) {
    /* TODO: match -- 420 bytes at 0x8010EFE4 */
}
#pragma pop

/* ==================================================================
 * fn_8010F188 -- GSfield_ArcTest
 *
 * Sweep test along a circular arc. Used for character movement on
 * curved surfaces. 408 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSfield_ArcTest(void* center, f32 radius, void* result) {
    /* TODO: match -- 408 bytes at 0x8010F188 */
}
#pragma pop

/* ==================================================================
 * fn_8010F71C -- GSfield_FindGroundHeight
 *
 * Find the ground height at a given XZ position by casting a
 * vertical ray downward. 824 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
f32 GSfield_FindGroundHeight(f32 x, f32 z, void* resultInfo) {
    /* TODO: match -- 824 bytes at 0x8010F71C */
}
#pragma pop

/* ==================================================================
 * fn_801101B4 -- GSfield_BuildCollisionGrid
 *
 * Build a spatial acceleration grid over the collision mesh for
 * faster point queries. 1256 bytes -- one of the larger functions.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSfield_BuildCollisionGrid(void* meshData) {
    /* TODO: match -- 1256 bytes at 0x801101B4 */
}
#pragma pop

/* ==================================================================
 * fn_8011069C -- GSfield_GridLookup
 *
 * Look up collision triangles in the spatial grid for a given
 * world-space position. 1992 bytes -- very large function.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GSfield_GridLookup(f32 x, f32 z, void* outTriangles) {
    /* TODO: match -- 1992 bytes at 0x8011069C */
}
#pragma pop

/* ==================================================================
 * fn_801123D4 -- GSfield_ResourceInit
 *
 * Initialize the field resource system for a new floor.
 * Sets up resource slot table, callback pointers, and allocates
 * working memory. 812 bytes.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSfield_ResourceInit(u32 floorDataEntry, u32 loadMode) {
    /* TODO: match -- 812 bytes at 0x801123D4 */
}
#pragma pop

/* ==================================================================
 * fn_801129CC -- GSfield_UpdateObjects
 *
 * Per-frame update for all field objects (NPCs, models, triggers).
 * 1472 bytes -- iterates a linked list of active objects.
 * ================================================================== */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void GSfield_UpdateObjects(void) {
    /* TODO: match -- 1472 bytes at 0x801129CC */
}
#pragma pop

/* ===== Small accessor functions (0x8011392C-0x801142F8) ===== */

/* fn_8011392C */ f32 GSfield_GetObjectRotX(void* obj)    { /* TODO: match */ }
/* fn_8011393C */ f32 GSfield_GetObjectRotY(void* obj)    { /* TODO: match */ }
/* fn_8011394C */ f32 GSfield_GetObjectRotZ(void* obj)    { /* TODO: match */ }
/* fn_8011395C */ f32 GSfield_GetObjectScale(void* obj)   { /* TODO: match */ }

/* ===================================================================
 * Generated: 0 pattern-matched + 59 stubs
 * Range: 0x8010E138 - 0x8011432C
 * =================================================================== */

/* 0x8010F320 | 0x198 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010F320(void) {
    /* TODO: match -- 408 bytes at 0x8010F320 */
}
#pragma pop

/* 0x8010F4B8 | 0xEC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010F4B8(void) {
    /* TODO: match -- 236 bytes at 0x8010F4B8 */
}
#pragma pop

/* 0x8010F5A4 | 0xFC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010F5A4(void) {
    /* TODO: match -- 252 bytes at 0x8010F5A4 */
}
#pragma pop

/* 0x8010F6A0 | 0x7C */
void fn_8010F6A0(void* arg0, void* arg1, void* arg2, f32 t) {
    f32 v[3];

    fn_800A3A9C(arg2, arg1, v);
    fn_800A3AC0(v, v, t / fn_800A3B38(v));
    fn_800A3A78(v, arg1, arg0);
}

/* 0x8010FA54 | 0xA0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010FA54(void) {
    /* TODO: match -- 160 bytes at 0x8010FA54 */
}
#pragma pop

/* 0x8010FAF4 | 0x304 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010FAF4(void) {
    /* TODO: match -- 772 bytes at 0x8010FAF4 */
}
#pragma pop

/* 0x8010FDF8 | 0x1CC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010FDF8(void) {
    /* TODO: match -- 460 bytes at 0x8010FDF8 */
}
#pragma pop

/* 0x8010FFC4 | 0xC0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8010FFC4(void) {
    /* TODO: match -- 192 bytes at 0x8010FFC4 */
}
#pragma pop

/* 0x80110084 | 0x130 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80110084(void) {
    /* TODO: match -- 304 bytes at 0x80110084 */
}
#pragma pop

/* 0x80110E64 | 0x60C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80110E64(void) {
    /* TODO: match -- 1548 bytes at 0x80110E64 */
}
#pragma pop

/* 0x80111470 | 0x1CC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80111470(void) {
    /* TODO: match -- 460 bytes at 0x80111470 */
}
#pragma pop

/* 0x8011163C | 0x228 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8011163C(void) {
    /* TODO: match -- 552 bytes at 0x8011163C */
}
#pragma pop

/* 0x80111864 | 0x338 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_80111864(void* a, void* b, void* c) {
    /* TODO: match -- 824 bytes at 0x80111864 */
    return 0;
}
#pragma pop

/* 0x80111B9C | 0x88 */
s32 fn_80111B9C(void* arg0, void* arg1, void* arg2) {
    extern f32 fn_800A3C00(void* a, void* b);
    extern f32 lbl_8047CF60;

    if (fn_8010CBC0() == 0) {
        return 0;
    }
    if (fn_800A3C00(arg1, arg0) <= lbl_8047CF60) {
        return 0;
    }
    return fn_80111864(arg0, arg1, arg2);
}

/* 0x80111C24 | 0x1D4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80111C24(void) {
    /* TODO: match -- 468 bytes at 0x80111C24 */
}
#pragma pop

/* 0x80111DF8 | 0x134 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80111DF8(void) {
    /* TODO: match -- 308 bytes at 0x80111DF8 */
}
#pragma pop

/* 0x80111F2C | 0x150 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80111F2C(void) {
    /* TODO: match -- 336 bytes at 0x80111F2C */
}
#pragma pop

/* 0x8011207C | 0x1E4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8011207C(void) {
    /* TODO: match -- 484 bytes at 0x8011207C */
}
#pragma pop

/* 0x80112260 | 0x120 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112260(void) {
    /* TODO: match -- 288 bytes at 0x80112260 */
}
#pragma pop

/* 0x80112380 | 0x54 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112380(void) {
    /* TODO: match -- 84 bytes at 0x80112380 */
}
#pragma pop

/* 0x80112700 | 0x4C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112700(void) {
    /* TODO: match -- 76 bytes at 0x80112700 */
}
#pragma pop

/* 0x8011274C | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void fn_8011274C(void) {
    extern void fn_801C40F0(s32);
    extern void fn_800D3074(s32);
    extern u8 lbl_80478DD0;

    fn_801C40F0(1);
    lbl_80478DD0 = 0;
    fn_800D3074(1);
}
#pragma peephole on
#pragma pop

/* 0x80112780 | 0x3C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112780(void) {
    /* TODO: match -- 60 bytes at 0x80112780 */
}
#pragma pop

/* 0x801127BC | 0x88 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801127BC(void) {
    /* TODO: match -- 136 bytes at 0x801127BC */
}
#pragma pop

/* 0x80112844 | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112844(void) {
    /* TODO: match -- 72 bytes at 0x80112844 */
}
#pragma pop

extern u8 lbl_80408378[];

/* 0x8011288C | 0x14 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_8011288C(u32 a, u32 b) {
    *(u32*)(lbl_80408378 + 0x28) = b;
    *(u32*)(lbl_80408378 + 0x2C) = a;
}
#pragma pop

/* 0x801128A0 | 0x10C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801128A0(void) {
    /* TODO: match -- 268 bytes at 0x801128A0 */
}
#pragma pop

/* 0x801129AC | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801129AC(void) {
    extern void fn_801D23C0(void);
    fn_801D23C0();
}
#pragma pop

/* 0x80112F8C | 0x60 */
#pragma push
#pragma peephole off
void fn_80112F8C(void) {
    extern void* fn_801157B0(void);
    extern u32 fn_800FF560(void);
    extern void GSthreadCreate(s32 a, u32 b, u32 c, u32 d, u32 e, void* f);
    extern void fn_800FF0A0(void (*callback)(void));
    void* obj;

    obj = fn_801157B0();
    if (obj == (void*)0) {
        return;
    }
    GSthreadCreate(1, fn_800FF560(), 0x4000, 1, 1, obj);
    fn_800FF0A0(fn_80112F8C);
}
#pragma pop

/* 0x80112FEC | 0x25C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112FEC(void) {
    /* TODO: match -- 604 bytes at 0x80112FEC */
}
#pragma pop

/* 0x80113248 | 0x29C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113248(void) {
    /* TODO: match -- 668 bytes at 0x80113248 */
}
#pragma pop

/* 0x801134E4 | 0x294 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801134E4(void) {
    /* TODO: match -- 660 bytes at 0x801134E4 */
}
#pragma pop

/* 0x80113778 | 0xB0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113778(void) {
    /* TODO: match -- 176 bytes at 0x80113778 */
}
#pragma pop

/* 0x80113828 | 0x64 */
#pragma push
#pragma peephole off
void fn_80113828(u32 arg0, s32 arg1) {
    extern u32 fn_800FF56C(void);
    extern void fn_800FF58C(s32);

    if (arg0 != 0) {
        *(u32*)(lbl_80408378 + 0x0) = fn_800FF56C();
        *(s32*)(lbl_80408378 + 0x4) = arg0;
        *(u8*)(lbl_80408378 + 0x8) = 1;
        *(s32*)(lbl_80408378 + 0xC) = arg1;
        fn_800FF58C(arg0);
    }
}
#pragma peephole on
#pragma pop

/* 0x8011388C | 0xA0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8011388C(void) {
    /* TODO: match -- 160 bytes at 0x8011388C */
}
#pragma pop

/* 0x8011392C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_8011392C(void) {
    return *(u32*)(lbl_80408378 + 0xC);
}
#pragma pop

/* 0x8011393C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_8011393C(void) {
    return *(u32*)(lbl_80408378 + 0x4);
}
#pragma pop

/* 0x8011394C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_8011394C(void) {
    return *(u32*)(lbl_80408378 + 0x0);
}
#pragma pop

/* 0x8011395C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void fn_8011395C(u32 value) {
    asm {
        lis r4, lbl_80408378@ha
        addi r4, r4, lbl_80408378@l
        stw r3, 0(r4)
    }
}
#pragma pop

/* 0x8011396C | 0x50 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8011396C(void) {
    /* TODO: match -- 80 bytes at 0x8011396C */
}
#pragma pop

/* 0x801139BC | 0x50 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801139BC(void) {
    extern void fn_8018B76C(s32, s32, s32, s32, s32);
    extern void fn_80117154(void);

    fn_8018B76C(0, 0x64, 1, 0, 1);
    fn_8018B76C(0, 0x65, 1, 0, 1);
    fn_80117154();
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
#pragma pop

/* 0x80113A0C | 0x178 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113A0C(void) {
    /* TODO: match -- 376 bytes at 0x80113A0C */
}
#pragma pop

/* 0x80113B84 | 0x18C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113B84(void) {
    /* TODO: match -- 396 bytes at 0x80113B84 */
}
#pragma pop

/* 0x80113D10 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
s32 fn_80113D10(void) {
    extern void fn_800E4BF4(void);

    fn_800E4BF4();
    return 1;
}
#pragma scheduling on
#pragma scheduling on
#pragma pop

/* 0x80113D34 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
void fn_80113D34(u32 unused, u32 modelIndex) {
#pragma optimization_level 4
    extern void fn_80113D58();

    fn_80113D58(modelIndex);
}
#pragma scheduling on
#pragma scheduling on
#pragma pop

/* 0x80113D58 | 0x1F0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113D58(void) {
    /* TODO: match -- 496 bytes at 0x80113D58 */
}
#pragma pop

/* 0x80113F48 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113F48(void) {
    extern void fn_80115BD8(void);
    extern void fn_80115A80(void);

    fn_80115BD8();
    fn_80115A80();
}
#pragma pop

/* 0x80113F6C | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113F6C(void) {
    /* TODO: match -- 72 bytes at 0x80113F6C */
}
#pragma pop

/* 0x80113FB4 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113FB4(void) {
    /* TODO: match -- 52 bytes at 0x80113FB4 */
}
#pragma pop

/* 0x80113FE8 | 0xE0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113FE8(void) {
    /* TODO: match -- 224 bytes at 0x80113FE8 */
}
#pragma pop

/* 0x801140C8 | 0x14 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801140C8(void) {
    *(u8*)(lbl_80408378 + 0x51) = 0;
}
#pragma pop

/* 0x801140DC | 0x90 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801140DC(void) {
    /* TODO: match -- 144 bytes at 0x801140DC */
}
#pragma pop

/* 0x8011416C | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8011416C(void) {
    /* TODO: match -- 32 bytes at 0x8011416C */
}
#pragma pop

/* 0x8011418C | 0x4C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_8011418C(void) {
    /* TODO: match -- 76 bytes at 0x8011418C */
}
#pragma pop

/* 0x801141D8 | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801141D8(void) {
    /* TODO: match -- 32 bytes at 0x801141D8 */
}
#pragma pop

/* 0x801141F8 | 0x5C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801141F8(void) {
    /* TODO: match -- 92 bytes at 0x801141F8 */
}
#pragma pop

/* 0x80114254 | 0x60 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80114254(void) {
    /* TODO: match -- 96 bytes at 0x80114254 */
}
#pragma pop

/* 0x801142B4 | 0x44 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801142B4(void) {
    /* TODO: match -- 68 bytes at 0x801142B4 */
}
#pragma pop

/* 0x801142F8 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801142F8(void) {
    /* TODO: match -- 52 bytes at 0x801142F8 */
}
#pragma pop
