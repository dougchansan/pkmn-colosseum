/**
 * @file gs_field_colquery_types.h
 * @brief Shared types/decls for the GScolsys2 collision-query unit split.
 *
 * game/gs_field_colquery.c (0x8010F6A0-0x801140DC) was a single
 * CodeCandidate bucket covering 6 distinct XD-era translation units
 * (GScolsys2Util, GScolsys2Human, GScolsys2Thru, GScolsys2Check,
 * GScolsys2Sun, floor). This header carries the typedefs, BSS/rodata
 * externs, and leaf-helper prototypes those six units share, plus
 * cross-TU forward declarations, so any split file can call into
 * another without per-file extern bookkeeping.
 *
 * Address range: 0x8010F6A0 - 0x801140DC
 */
#ifndef GAME_GS_FIELD_COLQUERY_TYPES_H
#define GAME_GS_FIELD_COLQUERY_TYPES_H

#include "game/gs_colsys.h"

/* ===== Collision triangle/query record layouts ===== */
typedef struct GScolsys2Vec3 {
    f32 x;
    f32 y;
    f32 z;
} GScolsys2Vec3;

typedef struct GScolsys2Triangle {
    GScolsys2Vec3 verts[3];      /* 0x00 */
    GScolsys2Vec3 normal;        /* 0x24 */
    u16 flags;                   /* 0x30 */
    u16 id;                      /* 0x32 */
} GScolsys2Triangle;

typedef struct GScolsys2TriangleList {
    GScolsys2Triangle* triangles; /* 0x00 */
    u32 count;                    /* 0x04 */
} GScolsys2TriangleList;

typedef struct GSfieldQueryTriangle {
    GScolsys2Vec3 verts[3];      /* 0x00 */
    GScolsys2Vec3 normal;        /* 0x24 */
    u16 id;                      /* 0x30 */
    u16 pad_32;                  /* 0x32 */
} GSfieldQueryTriangle;

typedef struct GSfieldEdgeMasks {
    u16 values[3];
} GSfieldEdgeMasks;

/* ===== String constants (rodata) ===== */
extern const char lbl_802720B0[]; /* "scene_data" */

/* ===== BSS / global state ===== */
extern u8 lbl_80404C68[];  /* GScolsys2 collision state */
extern u8 lbl_80408378[];

#define GSFIELD_COL_RESERVATION_COUNT 0x30
#define GSFIELD_COL_RESERVATION_IN_USE 0x0001
#define GSFIELD_COL_RESERVATION_DISABLED 0x0002

typedef struct GSFieldVec3f {
    f32 x;
    f32 y;
    f32 z;
} GSFieldVec3f;

typedef struct GSFieldColReservation {
    /* 0x00 */ s32 field_00;
    /* 0x04 */ s32 field_04;
    /* 0x08 */ f32 field_08;
    /* 0x0C */ f32 field_0C;
    /* 0x10 */ u16 flags;
    /* 0x12 */ u16 pad_12;
} GSFieldColReservation;

typedef struct GSFieldColLayerView {
    /* 0x000 */ u8 pad_000[0xA00];
    /* 0xA00 */ GSFieldColReservation reservations[GSFIELD_COL_RESERVATION_COUNT];
} GSFieldColLayerView;

typedef struct GSFieldWzxTriangleList {
    /* 0x00 */ void* triangles;
    /* 0x04 */ u32 triangleCount;
} GSFieldWzxTriangleList;

typedef struct GSFieldWzxRegion {
    /* 0x00 */ u8 pad_00[0x30];
    /* 0x30 */ GSFieldWzxTriangleList* floorTriangles;
    /* 0x34 */ u8 pad_34[0x04];
    /* 0x38 */ GSFieldWzxTriangleList* boundaryTriangles;
    /* 0x3C */ u8 pad_3C[0x04];
} GSFieldWzxRegion;

typedef struct GSFieldWzxData {
    /* 0x00 */ GSFieldWzxRegion* regions;
    /* 0x04 */ u32 regionCount;
} GSFieldWzxData;

typedef struct GSFieldWzxCompactTriangle {
    /* 0x00 */ GSFieldVec3f vertices[3];
    /* 0x24 */ GSFieldVec3f normal;
} GSFieldWzxCompactTriangle;

typedef struct GSFieldColqueryState {
    /* 0x00 */ u32 field_00;
    /* 0x04 */ u32 field_04;
    /* 0x08 */ u8 field_08;
    /* 0x09 */ u8 pad_09[0x03];
    /* 0x0C */ u32 field_0C;
    /* 0x10 */ u8 pad_10[0x18];
    /* 0x28 */ u32 transitionPollCallback;
    /* 0x2C */ u32 transitionBeginCallback;
    /* 0x30 */ u8 pad_30[0x0C];
    /* 0x3C */ u32 texture;
    /* 0x40 */ u32 texturePalette;
    /* 0x44 */ u8 textureSlot;
    /* 0x45 */ u8 pad_45[0x03];
    /* 0x48 */ u32 material;
    /* 0x4C */ u32 materialPalette;
    /* 0x50 */ u8 materialSlot;
    /* 0x51 */ u8 usePendingMaterial;
} GSFieldColqueryState;

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);        /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);

/* Matrix / vector math helpers */
extern void  PSMTXMultVec(void* mtxDst, void* vecSrc, void* vecDst);  /* MTXMultVec3 */
extern void  PSVECAdd(void* vecA, void* scale, void* vecOut);     /* VEC scale */
extern void  PSVECSubtract(void* a, void* b, void* out);             /* VEC diff/setup */
extern f32   PSVECMag(void* vec);                                /* VEC magnitude */
extern void  PSVECScale(void* curve, void* paramOut, f32 t);       /* VEC lerp */
f32   PSVECDotProduct(void* a, void* b);
f32   PSVECSquareDistance(void* a, void* b);

/* GScolsys2 functions */
extern void* fn_8010CBC0(void);                         /* GScolsys2_GetWZXData */
extern s32   GScolsys2GetObjEnable(u32 triIdx, void* outFlag);    /* GScolsys2_QueryTriVisible */
extern void  colsysMakeSRTMatrix(void* mtxOut, u32 layerIdx);   /* GScolsys2_BuildInverseTransform */
extern void  colsysMakeRotMatrix(void* mtxOut, u32 layerIdx);   /* GScolsys2_BuildTransform */
extern s32   fn_8010DEF0(void* result, void* origin,
                          void* verts, void* normal);    /* GScolsys2_TriangleBoundsCheck */
f32   GScolsy2UtilGetSidePlanePoint(void* normal, void* p1, void* p2);
void  GScolsy2UtilGetCpPlanePoint(void* out, void* normal, void* verts, void* point);
f32   GScolsys2UtilGetCpLinePoint(void* out, void* start, void* end, void* point);
s32   GScolsy2UtilChkInTri(void* point, void* verts, void* normal);

/* ===== Cross-TU forward declarations (this split) ===== */
s32 fn_80111864(GScolsys2Vec3* start, GScolsys2Vec3* end,
                GSfieldQueryTriangle* out); /* GScolsys2Thru, called from GScolsys2Check */

#endif /* GAME_GS_FIELD_COLQUERY_TYPES_H */
