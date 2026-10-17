/**
 * @file GScolsys2Thru.c
 * @brief GScolsys2Thru: the "thru" (pass-through) collision event queries.
 *
 * The whole GScolsys2Thru TU: .text 0x801101B4 - 0x80111864 and its .sdata2
 * literal pool 0x8047CF48 - 0x8047CF60 (the two {1, 2, 4} edge-mask
 * initialisers of getCpPointLine / getCpPointPoint, then 0.0f and 1.0f).
 * Functions: fn_801101B4 (XD GScolsys2ThruPassEventID, with
 * getMdlEventListPass inlined), GScolsys2ThruGetFixedMdlEventList,
 * GScolsys2ThruGetMdlEventList, GScolsys2ThruGetEventList and
 * GScolsys2ThruGetEventID. The layout follows XD's GScolsys2Thru.o
 * (NXXJ01.map lines 6907-6919: stripped getCpPointPoint, getCpPointLine,
 * getCpPointPoly, addList, getCpSegPoly and getFixedMdlEventListPass).
 * fn_80110084 belongs to GScolsys2Human and fn_80111864 to the next TU,
 * which has its own 0.0f/1.0f pool at 0x8047CF60.
 *
 * History: lanes D10/D13/D14 brought the functions to 99%+ (see
 * docs/recon/thru_d14.md). Lane D23 fixed the last wall, the preheader of
 * the vertex-transform loop in fn_801101B4 and GetMdlEventList (see
 * docs/recon/thru_d23.md):
 *  - Order. MWCC's pre-RA list scheduler breaks ties with a per-opcode byte
 *    (mr 0, addi 2, li 4), so `mr src` always issued before `addi dst`.
 *    Retail's addi comes first because it feeds a copy in the same block
 *    (the dst pointer is a copy of an address temporary), which wins on
 *    release count. The copy only survives when the temporary is an inline
 *    local (a coalescable web) that is also read, d-form, by addList after
 *    the loop; the register allocator then coalesces it away.
 *  - Registers. MWCC pops the low-degree webs in descending vreg order, and
 *    inline temporaries are numbered in reverse creation order. The source
 *    pointer has to be created after the dst web (and, in fn_801101B4,
 *    after the fixed pass's checkPolyLine temporary), which fixes where its
 *    declaration sits.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

/* XD addList (UNUSED 0x70): append one triangle to the result list. */
static inline void addList(GSfieldQueryTriangle* out, GScolsys2Vec3* verts,
                           GScolsys2Vec3* normal, u16 id)
{
    out->verts[0] = verts[0];
    out->verts[1] = verts[1];
    out->verts[2] = verts[2];
    out->normal = *normal;
    out->id = id;
}

/*
 * XD GScolsys2Thru.o's stripped helpers (NXXJ01.map lines 6908-6913, all
 * UNUSED): getCpPointPoly (0xF8), getCpPointLine (0x124) and
 * getCpPointPoint (0x110). Each tests one triangle against the point for
 * the face, the flagged edges and the flagged corners, respectively.
 */
static inline s32 getCpPointPoly(GScolsys2Vec3* cp, GScolsys2Vec3* point, f32 radiusSq,
                                 GScolsys2Vec3* verts, GScolsys2Vec3* normal)
{
    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    GScolsy2UtilGetCpPlanePoint(cp, normal, verts, point);
    if (PSVECSquareDistance(cp, point) >= radiusSq) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(cp, verts, normal) == 0) {
        return 0;
    }
    return 1;
}

static inline s32 getCpPointLine(GScolsys2Vec3* cp, GScolsys2Vec3* point, f32 radiusSq,
                                 GScolsys2Vec3* verts, GScolsys2Vec3* normal, u16 flags)
{
    u16 masks[3] = {1, 2, 4};
    s32 i;
    s32 next;
    f32 t;

    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (flags & masks[i]) {
            next = i + 1;
            if (next >= 3) {
                next = 0;
            }
            t = GScolsys2UtilGetCpLinePoint(cp, &verts[i], &verts[next], point);
            if (t < 0.0f || t > 1.0f) {
                continue;
            }
            if (PSVECSquareDistance(cp, point) < radiusSq) {
                return 1;
            }
        }
    }
    return 0;
}

static inline s32 getCpPointPoint(GScolsys2Vec3* point, f32 radiusSq,
                                  GScolsys2Vec3* verts, GScolsys2Vec3* normal, u16 flags)
{
    u16 masks[3] = {1, 2, 4};
    s32 i;
    s32 prev;

    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        prev = i + 2;
        if (prev >= 3) {
            prev -= 3;
        }
        if ((flags & masks[i]) && (flags & masks[prev])) {
            if (PSVECSquareDistance(&verts[i], point) < radiusSq) {
                return 1;
            }
        }
    }
    return 0;
}


/* 0x801101B4 | 0x4E8 */
typedef struct GSFieldWzxRegionLocal {
    u8 pad_00[0x2C];
    GScolsys2TriangleList* triangles;
    u8 pad_30[0x0C];
    u16 flags;
    u8 pad_3E[2];
} GSFieldWzxRegionLocal;

/* The segment test of one triangle: the plane crossing must lie on the
 * segment and inside the triangle. */
static inline s32 checkPolyLine(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                GScolsys2Vec3* verts, GScolsys2Vec3* normal)
{
    GScolsys2Vec3 hitPoint;
    f32 t;

    if (GScolsys2UtilGetCpPlaneLine((Vec3f*)&hitPoint, &t, (Vec3f*)normal,
                                    (Vec3f*)verts, (Vec3f*)start,
                                    (Vec3f*)end) == 0) {
        return 0;
    }
    if (t < 0.0f || t > 1.0f) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(&hitPoint, verts, normal) == 0) {
        return 0;
    }
    return 1;
}

/*
 * RULE-EXCEPTION(title-path): shaping inlines - see docs/RULE_EXCEPTIONS.md.
 * The vertex transform of getMdlEventListPass. The source pointer is a
 * local two inline levels down: the inliner expands level by level, so this
 * makes it a later temporary than the fixed pass's checkPolyLine operands,
 * which gives retail's r16 source / r17 fixed-pass normal. `out` is the
 * caller's `dst` copy of verts.
 */
static inline void mdlXformVertsBody(f32* m, GScolsys2Triangle* tri, GScolsys2Vec3* out)
{
    GScolsys2Vec3* src;
    s32 v;

    src = tri->verts;
    for (v = 0; v < 3; v++, src++) {
        PSMTXMultVec(m, src, &out[v]);
    }
}

static inline void mdlXformVerts(f32* m, GScolsys2Triangle* tri, GScolsys2Vec3* out)
{
    mdlXformVertsBody(m, tri, out);
}

/* XD getMdlEventListPass (0x200): the transformed-model pass. */
static inline s32 getMdlEventListPass(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                      GScolsys2TriangleList* list, f32* mtxInv,
                                      f32* mtxFwd, GSfieldQueryTriangle* out)
{
    GScolsys2Vec3 normal;
    GScolsys2Vec3 dirVec;
    GScolsys2Vec3 verts[3];
    GSfieldQueryTriangle* top;
    GScolsys2Vec3* dst;
    GSfieldQueryTriangle* write;
    GSfieldQueryTriangle* scan;
    GScolsys2Triangle* tri;
    u32 i;
    s32 j;
    s32 count;

    count = 0;
    PSVECSubtract(end, start, &dirVec);
    top = out;
    tri = list->triangles;
    write = top;
    for (i = 0; i < list->count && count < 4; i++, tri++) {
        for (j = 0, scan = top; j < count; scan++, j++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (j < count) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, &dirVec) >= 0.0f) {
            continue;
        }
        /* RULE-EXCEPTION(title-path): dst copy for the loop and addList - see docs/RULE_EXCEPTIONS.md */
        dst = verts;
        mdlXformVerts(mtxInv, tri, dst);
        if (checkPolyLine(start, end, verts, &normal)) {
            addList(write, dst, &normal, tri->id);
            write++;
            count++;
        }
    }
    return count;
}

/* The fixed-model pass (inline in XD GScolsys2ThruPassEventID). */
static inline s32 getFixedMdlEventListPass(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                           GScolsys2TriangleList* list,
                                           GSfieldQueryTriangle* out)
{
    GScolsys2Vec3 dirVec;
    GScolsys2Triangle* tri;
    u32 i;
    s32 count;
    GSfieldQueryTriangle* write;
    GSfieldQueryTriangle* top;
    GSfieldQueryTriangle* scan;
    s32 j;

    count = 0;
    PSVECSubtract(end, start, &dirVec);
    top = out;
    tri = list->triangles;
    write = top;
    for (i = 0; i < list->count && count < 4; i++, tri++) {
        for (j = 0, scan = top; j < count; scan++, j++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (j < count) {
            continue;
        }
        if (PSVECDotProduct(&tri->normal, &dirVec) >= 0.0f) {
            continue;
        }
        if (checkPolyLine(start, end, tri->verts, &tri->normal)) {
            addList(write, tri->verts, &tri->normal, tri->id);
            write++;
            count++;
        }
    }
    return count;
}

/* XD GScolsys2ThruPassEventID (0x3B0 + getMdlEventListPass 0x200). */
s32 colsysPassGetEventID(GScolsys2Vec3* start, GScolsys2Vec3* end,
                GSfieldQueryTriangle* out)
{
    GSfieldQueryTriangle temp[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    GSFieldWzxData* wzx;
    GSFieldWzxRegionLocal* region;
    GScolsys2TriangleList* list;
    GSfieldQueryTriangle* scan;
    GSfieldQueryTriangle* read;
    s32 j;
    s32 k;
    s32 enabled;
    u32 i;
    s32 tempCount;
    s32 outCount;

    outCount = 0;
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    if (wzx == NULL) {
        return 0;
    }
    region = (GSFieldWzxRegionLocal*)wzx->regions;
    for (i = 0; i < wzx->regionCount && outCount < 4; i++, region++) {
        GScolsys2GetObjEnable(i, &enabled);
        if (enabled == 0) {
            continue;
        }
        list = region->triangles;
        if (list == NULL) {
            continue;
        }
        if ((region->flags & 1) != 0) {
            colsysMakeSRTMatrix(mtxInv, i);
            colsysMakeRotMatrix(mtxFwd, i);
            tempCount = getMdlEventListPass(start, end, list, mtxInv, mtxFwd, temp);
        } else {
            tempCount = getFixedMdlEventListPass(start, end, list, temp);
        }
        for (read = temp, j = 0; j < tempCount && outCount < 4; read++, j++) {
            for (scan = out, k = 0; k < outCount; scan++, k++) {
                if (scan->id == read->id) {
                    break;
                }
            }
            if (k >= outCount) {
                out[outCount++] = *read;
            }
        }
    }
    return outCount;
}

/* 0x8011069C | 0x7C8 */
typedef struct GSFieldFixedMdlCell {
    u32 firstIndex;
    u32 count;
} GSFieldFixedMdlCell;

/* The fixed model's triangle grid (the walk grid's layout, GScolsys2Walk.c). */
typedef struct GSFieldFixedMdlEventList {
    /* 0x00 */ GScolsys2Triangle* triangles;
    /* 0x04 */ u8 pad_04[4];
    /* 0x08 */ GSFieldFixedMdlCell* cells;
    /* 0x0C */ u32* triangleIndices;
    /* 0x10 */ u16 cellCountX;
    /* 0x12 */ u16 cellCountZ;
    /* 0x14 */ f32 cellWidth;
    /* 0x18 */ f32 cellDepth;
    /* 0x1C */ f32 minX;
    /* 0x20 */ f32 minZ;
} GSFieldFixedMdlEventList;

/* XD getFixedMdlEventList (0x7C8): the three passes of
 * GScolsys2ThruGetMdlEventList over the grid cells the sphere touches. */
s32 GScolsys2ThruGetFixedMdlEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
    GScolsys2TriangleList* triList, GSfieldQueryTriangle* outTris)
{
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    GSFieldFixedMdlEventList* grid;
    GSfieldQueryTriangle* scan;
    f32 radiusSq;
    GScolsys2Vec3 lo;
    GScolsys2Vec3 hi;
    s32 startX;
    s32 startZ;
    s32 endX;
    s32 endZ;
    GScolsys2Triangle* tri;
    u32 j;
    GSFieldFixedMdlCell* cell;
    u32* index;
    s32 x;
    s32 z;
    s32 k;
    s32 outCount;

    grid = (GSFieldFixedMdlEventList*)triList;
    outCount = 0;
    lo.x = point->x - radius;
    lo.z = point->z - radius;
    hi.x = point->x + radius;
    hi.z = point->z + radius;
    startX = (lo.x - grid->minX) / grid->cellWidth;
    if (startX < 0) {
        startX = 0;
    }
    startZ = (lo.z - grid->minZ) / grid->cellDepth;
    if (startZ < 0) {
        startZ = 0;
    }
    endX = (hi.x - grid->minX) / grid->cellWidth;
    if (endX > grid->cellCountX - 1) {
        endX = grid->cellCountX - 1;
    }
    endZ = (hi.z - grid->minZ) / grid->cellDepth;
    if (endZ > grid->cellCountZ - 1) {
        endZ = grid->cellCountZ - 1;
    }
    radiusSq = radius * radius;

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells;
        cell += startX + z * grid->cellCountX;
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            index = grid->triangleIndices;
            index += cell->firstIndex;
            for (j = 0; j < cell->count && outCount < 4; j++, index++) {
                tri = grid->triangles;
                tri += *index;
                for (k = 0; k < outCount; k++) {
                    if (tri->id == outTris[k].id) {
                        break;
                    }
                }
                if (k < outCount) {
                    continue;
                }
                if (PSVECDotProduct(&tri->normal, dirVec) >= 0.0f) {
                    continue;
                }
                if (getCpPointPoly(&cp, point, radiusSq, tri->verts, &tri->normal)) {
                    addList(&outTris[outCount], tri->verts, &tri->normal, tri->id);
                    outCount++;
                }
            }
        }
    }

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells;
        cell += startX + z * grid->cellCountX;
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            index = grid->triangleIndices;
            index += cell->firstIndex;
            for (j = 0; j < cell->count && outCount < 4; j++, index++) {
                tri = grid->triangles;
                tri += *index;
                if (!(tri->flags & 7)) {
                    continue;
                }
                for (k = 0; k < outCount; k++) {
                    if (tri->id == outTris[k].id) {
                        break;
                    }
                }
                if (k < outCount) {
                    continue;
                }
                if (PSVECDotProduct(&tri->normal, dirVec) >= 0.0f) {
                    continue;
                }
                if (getCpPointLine(&lineCp, point, radiusSq, tri->verts, &tri->normal,
                                   tri->flags)) {
                    addList(&outTris[outCount], tri->verts, &tri->normal, tri->id);
                    outCount++;
                }
            }
        }
    }

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells;
        cell += startX + z * grid->cellCountX;
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            index = grid->triangleIndices;
            index += cell->firstIndex;
            for (j = 0; j < cell->count && outCount < 4; j++, index++) {
                tri = grid->triangles;
                tri += *index;
                if (!(tri->flags & 7)) {
                    continue;
                }
                for (k = 0; k < outCount; k++) {
                    if (tri->id == outTris[k].id) {
                        break;
                    }
                }
                if (k < outCount) {
                    continue;
                }
                if (PSVECDotProduct(&tri->normal, dirVec) >= 0.0f) {
                    continue;
                }
                if (getCpPointPoint(point, radiusSq, tri->verts, &tri->normal,
                                    tri->flags)) {
                    addList(&outTris[outCount], tri->verts, &tri->normal, tri->id);
                    outCount++;
                }
            }
        }
    }
    return outCount;
}

/*
 * RULE-EXCEPTION(title-path): shaping inline - see docs/RULE_EXCEPTIONS.md.
 * GetMdlEventList's first-pass vertex transform. `copy` is the address
 * temporary the destination pointer is copied from; the caller hands it to
 * addList, whose d-form reads keep it alive until the backend folds them.
 * Declaration order (count, dst, src, copy) and increment order (dst
 * before src) give retail's r21 count / r20 dst / r19 src.
 */
static inline GScolsys2Vec3* mdlXformVertsCopy(void* m, GScolsys2Triangle* tri,
                                               GScolsys2Vec3* out)
{
    s32 count;
    GScolsys2Vec3* dst;
    GScolsys2Vec3* src;
    GScolsys2Vec3* copy;

    copy = out;
    src = tri->verts;
    dst = copy;
    for (count = 0; count < 3; count++, dst++, src++) {
        PSMTXMultVec(m, src, dst);
    }
    return copy;
}

/* 0x80110E64 | 0x60C */
s32 GScolsys2ThruGetMdlEventList(GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
                GScolsys2TriangleList* triList, void* mtxInv, void* mtxFwd,
                GSfieldQueryTriangle* outTris) {
    GScolsys2Vec3 verts[3];
    GScolsys2Vec3 normal;
    GScolsys2Vec3* xformed;
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    GScolsys2Triangle* tri;
    GSfieldQueryTriangle* scan;
    f32 radiusSq;
    s32 outCount;
    u32 i;
    s32 k;
    s32 v;

    radiusSq = radius * radius;
    outCount = 0;
    tri = triList->triangles;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        for (k = 0; k < outCount; k++) {
            if (tri->id == outTris[k].id) {
                break;
            }
        }
        if (k < outCount) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= 0.0f) {
            continue;
        }
        xformed = mdlXformVertsCopy(mtxInv, tri, verts);
        if (getCpPointPoly(&cp, point, radiusSq, verts, &normal)) {
            addList(&outTris[outCount], xformed, &normal, tri->id);
            outCount++;
        }
    }

    tri = triList->triangles;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (k = 0; k < outCount; k++) {
            if (tri->id == outTris[k].id) {
                break;
            }
        }
        if (k < outCount) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= 0.0f) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(mtxInv, &tri->verts[v], &verts[v]);
        }
        if (getCpPointLine(&lineCp, point, radiusSq, verts, &normal, tri->flags)) {
            addList(&outTris[outCount], verts, &normal, tri->id);
            outCount++;
        }
    }

    tri = triList->triangles;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (k = 0; k < outCount; k++) {
            if (tri->id == outTris[k].id) {
                break;
            }
        }
        if (k < outCount) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= 0.0f) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(mtxInv, &tri->verts[v], &verts[v]);
        }
        if (getCpPointPoint(point, radiusSq, verts, &normal, tri->flags)) {
            addList(&outTris[outCount], verts, &normal, tri->id);
            outCount++;
        }
    }
    return outCount;
}

/* 0x80111470 | 0x1CC */
s32 GScolsys2ThruGetEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec,
    GSfieldQueryTriangle* out, f32 radius) {
    GSfieldQueryTriangle temporary[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    s32 enabled;
    GSFieldWzxData* wzx;
    GSFieldWzxRegion* region;
    GScolsys2TriangleList* list;
    u32 regionIndex;
    s32 temporaryCount;
    s32 outCount;
    s32 i;
    s32 j;

    outCount = 0;
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    region = wzx->regions;
    for (regionIndex = 0; regionIndex < wzx->regionCount && outCount < 4;
         regionIndex++, region++) {
        GScolsys2GetObjEnable(regionIndex, &enabled);
        if (enabled == 0) {
            continue;
        }
        list = *(GScolsys2TriangleList**)((u8*)region + 0x2C);
        if (list == NULL) {
            continue;
        }
        if ((*(u16*)((u8*)region + 0x3C) & 1) != 0) {
            colsysMakeSRTMatrix(mtxInv, regionIndex);
            colsysMakeRotMatrix(mtxFwd, regionIndex);
            temporaryCount = GScolsys2ThruGetMdlEventList(
                point, dirVec, radius, list, mtxInv, mtxFwd, temporary);
        } else {
            temporaryCount = GScolsys2ThruGetFixedMdlEventList(
                point, dirVec, radius, list, temporary);
        }
        for (i = 0; i < temporaryCount && outCount < 4; i++) {
            for (j = 0; j < outCount; j++) {
                if (out[j].id == temporary[i].id) {
                    break;
                }
            }
            if (j >= outCount) {
                out[outCount++] = temporary[i];
            }
        }
    }
    return outCount;
}

/* 0x8011163C | 0x228 */
s32 GScolsys2ThruGetEventID(
    GScolsys2Vec3* start, GScolsys2Vec3* end,
    f32 radius, GSfieldQueryTriangle* out)
{
    extern f32 PSVECMag(void*);
    GSfieldQueryTriangle temporary[4];
    GScolsys2Vec3 direction;
    GScolsys2Vec3 point;
    f32 length;
    f32 position;
    f32 step;
    f32 sample;
    s32 outCount;

    outCount = 0;
    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    PSVECSubtract(end, start, &direction);
    length = PSVECMag(&direction);
    if (length <= 0.0f) {
        return 0;
    }
    step = radius / length;
    if (step > 1.0f) {
        step = 1.0f;
    }

    position = 0.0f;
    while (position < 1.0f && outCount < 4) {
        s32 temporaryCount;
        s32 i;

        sample = position + step;
        if (sample > 1.0f) {
            sample = 1.0f;
        }
        PSVECScale(&direction, &point, sample);
        PSVECAdd(&point, start, &point);
        temporaryCount =
            GScolsys2ThruGetEventList(&point, &direction, temporary, radius);

        for (i = 0; i < temporaryCount && outCount < 4; i++) {
            s32 j;
            for (j = 0; j < outCount; j++) {
                if (temporary[i].id == out[j].id) {
                    break;
                }
            }
            if (j >= outCount) {
                out[outCount++] = temporary[i];
            }
        }
        if (step <= 0.0f) {
            break;
        }
        position += step;
    }
    return outCount;
}
