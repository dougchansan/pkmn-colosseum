/**
 * @file GScolsys2Walk_exact_8010E138.c
 * @brief GScolsys2Walk: fn_8010E138, the walk-height collector,
 *        0x8010E138 - 0x8010E53C.
 *
 * The tail of the GScolsys2Walk TU (GScolsys2Walk.c keeps
 * GScolsys2WalkGetLayer and getCpPolyVec with the TU's .sdata2 pool). It
 * uses no pool literals, so it links on its own.
 *
 * The collectors and the add-hit test are static inline: the add-hit
 * sequence is expanded twice (same order and constants), each collector's
 * count is routed through its own register and copied to the caller's
 * (mr r7), and the grid collector's early "return 0" re-clears its count
 * register on both failure paths. The grid collector loads the cell,
 * index and triangle bases into their pointers and then adds the offsets
 * (retail loads each base straight into the pointer's register), and it
 * declares z before x, which colours x first (r3), as in retail.
 */
#include "dolphin/types.h"

typedef struct GSvec {
    f32 x, y, z;
} GSvec;

typedef f32 ColMtx[3][4];

/* One collision triangle, 0x34 bytes. */
typedef struct ColTri {
    /* 0x00 */ GSvec verts[3];
    /* 0x24 */ GSvec normal;
    /* 0x30 */ u8 surface : 4;
    /* 0x30 */ u8 attr : 4;
    /* 0x31 */ u8 layer : 4;
    /* 0x31 */ u8 subLayer : 4;
    /* 0x32 */ u16 id;
} ColTri;

typedef struct ColTriList {
    ColTri* tris;
    u32 count;
} ColTriList;

typedef struct ColGridCell {
    u32 first;
    u32 count;
} ColGridCell;

/* Fixed-model triangle grid, bucketed in the x/z plane. */
typedef struct ColGrid {
    /* 0x00 */ ColTri* tris;
    /* 0x04 */ u32 triCount;
    /* 0x08 */ ColGridCell* cells;
    /* 0x0C */ u32* indices;
    /* 0x10 */ u16 cellCountX;
    /* 0x12 */ u16 cellCountZ;
    /* 0x14 */ f32 cellWidth;
    /* 0x18 */ f32 cellDepth;
    /* 0x1C */ f32 minX;
    /* 0x20 */ f32 minZ;
} ColGrid;

/* One CCD object, 0x40 bytes. */
typedef struct ColObj {
    /* 0x00 */ u8 pad_00[0x24];
    /* 0x24 */ void* walk;
    /* 0x28 */ u8 pad_28[0x14];
    /* 0x3C */ u16 flags;
    /* 0x3E */ u8 pad_3E[2];
} ColObj;

typedef struct ColScene {
    ColObj* objs;
    u32 count;
} ColScene;

/* One walkable height under a point, 0xC bytes. */
typedef struct ColWalkHit {
    f32 height;
    u16 surface;
    u16 attr;
    u8 layer;
    u8 subLayer;
} ColWalkHit;

extern ColScene* fn_8010CBC0(void);
extern s32 GScolsys2GetObjEnable(s32 index, s32* enabled);
extern void colsysMakeSRTMatrix(ColMtx out, u32 index);
extern void colsysMakeRotMatrix(ColMtx out, u32 index);
extern void PSMTXMultVec(ColMtx m, const GSvec* src, GSvec* dst);
s32 fn_8010E138(GSvec* point, ColWalkHit* out);
s32 getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(GSvec* out, GSvec* point, GSvec* vertices, GSvec* normal);

/* Append the height of a hit point to the list unless it already holds
 * it. */
static inline s32 ColWalkAddHit(ColWalkHit* list, s32 count, GSvec* cp,
                                ColTri* tri)
{
    s32 i;

    for (i = 0; i < count; i++) {
        if (cp->y == list[i].height) {
            break;
        }
    }
    if (i < count) {
        return 0;
    }

    list[count].height = cp->y;
    if (tri->surface == 15) {
        list[count].surface = 0xFFFF;
    } else {
        list[count].surface = tri->surface;
    }
    if (tri->attr == 15) {
        list[count].attr = 0xFFFF;
    } else {
        list[count].attr = tri->attr;
    }
    list[count].layer = tri->layer;
    list[count].subLayer = tri->subLayer;
    return 1;
}

/* Collect the walk heights under a point from a movable object's
 * triangle list, transformed into the object's placement. */
static inline s32 ColWalkCollectList(ColWalkHit* list, GSvec* point,
                                     ColTriList* tris, ColMtx matrix,
                                     ColMtx normalMatrix)
{
    GSvec verts[3];
    GSvec normal;
    GSvec cp;
    ColTri* tri;
    u32 j;
    s32 v;
    s32 count;

    count = 0;
    tri = tris->tris;
    for (j = 0; j < tris->count && count < 8; j++, tri++) {
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(matrix, &tri->verts[v], &verts[v]);
        }
        PSMTXMultVec(normalMatrix, &tri->normal, &normal);
        if (getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(&cp, point, verts,
                                                        &normal) != 0) {
            if (ColWalkAddHit(list, count, &cp, tri)) {
                count++;
            }
        }
    }
    return count;
}

/* Collect the walk heights under a point from a fixed model's grid. */
static inline s32 ColWalkCollectGrid(ColWalkHit* list, GSvec* point,
                                     ColGrid* grid)
{
    GSvec cp;
    ColTri* tri;
    ColGridCell* cell;
    u32* index;
    u32 j;
    s32 z;
    s32 x;
    s32 count;

    count = 0;
    x = (point->x - grid->minX) / grid->cellWidth;
    if (x < 0 || x >= grid->cellCountX) {
        return 0;
    }
    z = (point->z - grid->minZ) / grid->cellDepth;
    if (z < 0 || z >= grid->cellCountZ) {
        return 0;
    }

    cell = grid->cells;
    index = grid->indices;
    cell += x + z * grid->cellCountX;
    index += cell->first;
    for (j = 0; j < cell->count && count < 8; j++, index++) {
        tri = grid->tris;
        tri += *index;
        if (getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(&cp, point, tri->verts,
                                                        &tri->normal) != 0) {
            if (ColWalkAddHit(list, count, &cp, tri)) {
                count++;
            }
        }
    }
    return count;
}
/* 0x8010E138 | 0x404 */
s32 fn_8010E138(GSvec* point, ColWalkHit* out)
{
    ColWalkHit temp[8];
    ColMtx matrix;
    ColMtx normalMatrix;
    ColScene* scene;
    ColObj* obj;
    void* walk;
    u32 i;
    s32 total;
    s32 enabled;
    s32 k;
    s32 count;

    total = 0;
    scene = fn_8010CBC0();
    if (scene == NULL) {
        return 0;
    }

    obj = scene->objs;
    for (i = 0; i < scene->count && total < 8; i++, obj++) {
        GScolsys2GetObjEnable(i, &enabled);
        if (enabled == 0) {
            continue;
        }
        walk = obj->walk;
        if (walk == NULL) {
            continue;
        }

        if (obj->flags & 1) {
            colsysMakeSRTMatrix(matrix, i);
            colsysMakeRotMatrix(normalMatrix, i);
            count = ColWalkCollectList(temp, point, walk, matrix,
                                       normalMatrix);
        } else {
            count = ColWalkCollectGrid(temp, point, walk);
        }

        for (k = 0; k < count && total < 8; k++, total++) {
            out[total] = temp[k];
        }
    }

    return total;
}
