/**
 * @file gs_colsys_candidate_8010E53C.c
 * @brief GScolsys2 sphere TU, 0x8010E53C - 0x8010F4B8, with its .sdata2
 *        pool 0x8047CEF0 - 0x8047CF08 (linked).
 *
 * This is XD's GScolsys2Hit.o (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-
 * maps). fn_8010E53C is checkHitFixedMdl (0x5EC in both games) and
 * colsysCheckHitMdl is checkHitMdl (0x4BC in both). They run three passes (face,
 * flagged edges, flagged corners) through XD's stripped getCpPointPoly /
 * getCpPointLine / getCpPointPoint inlines. fn_8010EFE4 is checkHitCollision
 * (XD 0x1AC), and GScolsys2HitNpcCollision / fn_8010F320 are GScolsys2HitNpcCollision /
 * GScolsys2HitCollision (0x198 each); they were the carve
 * gs_colsys_exact_8010EFE4.c, folded in so the unit owns its pool.
 *
 * Pool: the two {1, 2, 4} edge-mask initialisers, then 0.0f and 1.0f, as
 * literals. Retail's last entry, 0.0001f at 0x8047CF08, is named (see
 * lbl_8047CF08 below). GC/1.3 -O3 (the carve was built at -O4,p; all five
 * functions are identical at either level).
 *
 * fn_8010E53C (lane D14): building each pass's cell/index/triangle pointer
 * from its base (`p = base; p += offset;`) and declaring k, x, z in that
 * order gives retail's registers.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

typedef f32 ColMtx[3][4];

typedef struct ColDrawGroup {
    u8* data;
    u32 count;
} ColDrawGroup;

extern void PSMTXMultVec(ColMtx, const f32*, f32*);
typedef struct GSFieldFixedMdlCell {
    u32 firstIndex;
    u32 count;
} GSFieldFixedMdlCell;

typedef struct GScolsys2Triangle {
    Vec3f verts[3];
    Vec3f normal;
    u16 flags;
    u16 id;
} GScolsys2Triangle;

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

typedef struct GScolsys2TriangleList {
    GScolsys2Triangle* triangles;
    u32 count;
} GScolsys2TriangleList;

typedef struct GScolsys2Region {
    u8 pad_00[0x24];
    void* triList;
    u8 pad_28[0x14];
    u16 flags;
    u8 pad_3E[2];
} GScolsys2Region;

/* 0x8010E53C | 0x5EC */
typedef struct GSfieldEdgeMasks {
    u16 values[3];
} GSfieldEdgeMasks;

extern f32 GScolsy2UtilGetSidePlanePoint(void*, void*, void*);
extern void GScolsy2UtilGetCpPlanePoint(void*, void*, void*, void*);
extern s32 GScolsy2UtilChkInTri(void*, void*, void*);
extern void GScolsy2UtilGetPointExtentionLine(void*, void*, void*, f32);
f32 GScolsys2UtilGetCpLinePoint(Vec3f*, Vec3f*, Vec3f*, Vec3f*);

extern f32 PSVECSquareDistance(void* a, void* b);

/* RULE-EXCEPTION(title-path): extern named stand-in for this TU's own pool
 * literal 0.0001f (lbl_8047CF08) - see docs/RULE_EXCEPTIONS.md. MWCC
 * creates the float literals of this TU in code-generation order (0.0f,
 * 0.0001f, 1.0f); retail's pool (and XD GScolsys2Hit.o's) has 1.0f before
 * 0.0001f, so the extension offset is named and stays in the data unit. */
extern const f32 lbl_8047CF08;

/*
 * XD GScolsys2Hit.o's stripped helpers (NXXJ01.map, UNUSED):
 * getCpPointPoly (0xF8), getCpPointLine (0x124), getCpPointPoint (0x110).
 * Each tests the sphere at point against a triangle's face, flagged edges
 * or flagged corners and returns the closest point in *out.
 */
static inline s32 getCpPointPoly(Vec3f* out, Vec3f* point, f32 radiusSq,
                                 Vec3f* verts, Vec3f* normal)
{
    Vec3f cp;

    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    GScolsy2UtilGetCpPlanePoint(&cp, normal, verts, point);
    if (PSVECSquareDistance(&cp, point) >= radiusSq) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(&cp, verts, normal) == 0) {
        return 0;
    }
    *out = cp;
    return 1;
}

static inline s32 getCpPointLine(Vec3f* out, Vec3f* point, f32 radiusSq,
                                 Vec3f* verts, Vec3f* normal, u16 flags)
{
    u16 masks[3] = {1, 2, 4};
    Vec3f cp;
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
            t = GScolsys2UtilGetCpLinePoint(&cp, &verts[i], &verts[next], point);
            if (t < 0.0f || t > 1.0f) {
                continue;
            }
            if (PSVECSquareDistance(&cp, point) < radiusSq) {
                *out = cp;
                return 1;
            }
        }
    }
    return 0;
}

static inline s32 getCpPointPoint(Vec3f* out, Vec3f* point, f32 radiusSq,
                                  Vec3f* verts, Vec3f* normal, u16 flags)
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
                *out = verts[i];
                return 1;
            }
        }
    }
    return 0;
}

/* 0x8010E53C | 0x5EC: XD checkHitFixedMdl__FP5GSvecfP15CCD_HITMDL_HEADP5GSvec. */
s32 colsysWallSphere(Vec3f* point, void* data, f32 radius, Vec3f* result) {
    GSFieldFixedMdlEventList* head = (GSFieldFixedMdlEventList*)data;
    Vec3f hitPoint;
    GSFieldFixedMdlCell* cell;
    u32* index;
    GScolsys2Triangle* tri;
    f32 radiusSq;
    Vec3f min;
    Vec3f max;
    s32 x0;
    s32 z0;
    s32 x1;
    s32 z1;
    u32 k;
    s32 x;
    s32 z;
    s32 hit;

    min.x = point->x - radius;
    min.z = point->z - radius;
    max.x = point->x + radius;
    max.z = point->z + radius;
    x0 = (min.x - head->minX) / head->cellWidth;
    if (x0 < 0) {
        x0 = 0;
    }
    z0 = (min.z - head->minZ) / head->cellDepth;
    if (z0 < 0) {
        z0 = 0;
    }
    x1 = (max.x - head->minX) / head->cellWidth;
    if (x1 > head->cellCountX - 1) {
        x1 = head->cellCountX - 1;
    }
    z1 = (max.z - head->minZ) / head->cellDepth;
    if (z1 > head->cellCountZ - 1) {
        z1 = head->cellCountZ - 1;
    }
    radiusSq = radius * radius;

    hit = 0;
    for (z = z0; z <= z1 && !hit; z++) {
        cell = head->cells;
        cell += z * head->cellCountX + x0;
        for (x = x0; x <= x1 && !hit; x++, cell++) {
            index = head->triangleIndices;
            index += cell->firstIndex;
            for (k = 0; k < cell->count && !hit; k++, index++) {
                tri = head->triangles;
                tri += *index;
                hit = getCpPointPoly(&hitPoint, point, radiusSq, tri->verts, &tri->normal);
            }
        }
    }
    if (hit) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &hitPoint, point, lbl_8047CF08 + radius);
        return 1;
    }

    hit = 0;
    for (z = z0; z <= z1 && !hit; z++) {
        cell = head->cells;
        cell += z * head->cellCountX + x0;
        for (x = x0; x <= x1 && !hit; x++, cell++) {
            index = head->triangleIndices;
            index += cell->firstIndex;
            for (k = 0; k < cell->count && !hit; k++, index++) {
                tri = head->triangles;
                tri += *index;
                if (!(tri->flags & 7)) {
                    continue;
                }
                hit = getCpPointLine(&hitPoint, point, radiusSq, tri->verts, &tri->normal, tri->flags);
            }
        }
    }
    if (hit) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &hitPoint, point, lbl_8047CF08 + radius);
        return 1;
    }

    hit = 0;
    for (z = z0; z <= z1 && !hit; z++) {
        cell = head->cells;
        cell += z * head->cellCountX + x0;
        for (x = x0; x <= x1 && !hit; x++, cell++) {
            index = head->triangleIndices;
            index += cell->firstIndex;
            for (k = 0; k < cell->count && !hit; k++, index++) {
                tri = head->triangles;
                tri += *index;
                if (!(tri->flags & 7)) {
                    continue;
                }
                hit = getCpPointPoint(&hitPoint, point, radiusSq, tri->verts, &tri->normal, tri->flags);
            }
        }
    }
    if (hit) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &hitPoint, point, lbl_8047CF08 + radius);
        return 1;
    }
    return 0;
}

/* 0x8010EB28 | 0x4BC: XD checkHitMdl__FP5GSvecfP15CCD_HITMDL_HEADP5GSmtxP5GSmtxP5GSvec. */
s32 colsysCheckHitMdl(Vec3f* point, void* data, ColMtx inverse,
                ColMtx forward, f32 radius, Vec3f* result) {
    GScolsys2TriangleList* list = (GScolsys2TriangleList*)data;
    Vec3f verts[3];
    Vec3f normal;
    Vec3f hitPoint;
    GScolsys2Triangle* tri;
    f32 radiusSq;
    u32 i;
    s32 v;
    s32 hit;

    radiusSq = radius * radius;
    tri = list->triangles;
    for (i = 0, hit = 0; i < list->count && !hit; i++, tri++) {
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(inverse, &tri->verts[v].x, &verts[v].x);
        }
        PSMTXMultVec(forward, &tri->normal.x, &normal.x);
        hit = getCpPointPoly(&hitPoint, point, radiusSq, verts, &normal);
    }
    if (hit) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &hitPoint, point, lbl_8047CF08 + radius);
        return 1;
    }

    tri = list->triangles;
    for (i = 0, hit = 0; i < list->count && !hit; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(inverse, &tri->verts[v].x, &verts[v].x);
        }
        PSMTXMultVec(forward, &tri->normal.x, &normal.x);
        hit = getCpPointLine(&hitPoint, point, radiusSq, verts, &normal, tri->flags);
    }
    if (hit) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &hitPoint, point, lbl_8047CF08 + radius);
        return 1;
    }

    tri = list->triangles;
    for (i = 0, hit = 0; i < list->count && !hit; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(inverse, &tri->verts[v].x, &verts[v].x);
        }
        PSMTXMultVec(forward, &tri->normal.x, &normal.x);
        hit = getCpPointPoint(&hitPoint, point, radiusSq, verts, &normal, tri->flags);
    }
    if (hit) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &hitPoint, point, lbl_8047CF08 + radius);
        return 1;
    }
    return 0;
}

/* One CCD object record, 0x40 bytes. */
typedef struct ColDrawObject {
    Vec3f trans;
    Vec3f rot;
    Vec3f scale;
    void* model;
    ColDrawGroup* edgeGroup0;
    ColDrawGroup* faceGroup0;
    ColDrawGroup* faceGroup1;
    ColDrawGroup* edgeGroup1;
    ColDrawGroup* faceGroup2;
    u16 flags;
    u8 pad_3E[2];
} ColDrawObject;

typedef struct ColDrawScene {
    ColDrawObject* objects;
    u32 count;
} ColDrawScene;

extern ColDrawScene* fn_8010CBC0(void);
extern s32 GScolsys2GetObjEnable(s32 index, s32* enabled);
extern s32 colsysMakeSRTMatrix(ColMtx out, u32 index);
extern s32 colsysMakeRotMatrix(ColMtx out, u32 index);

/* 0x8010EFE4 | 0x1A4 */
s32 colsysWallAll(Vec3f* segStart, Vec3f* segEnd, f32 radius,
                Vec3f* result, s32 useAlternate)
{
    ColDrawScene* scene;
    ColDrawObject* object;
    Vec3f current;
    Vec3f hitPoint;
    ColMtx inverse;
    ColMtx forward;
    s32 pass;
    s32 hits;
    u32 index;
    Vec3f* hitPointPtr;
    s32 enabled;
    s32 collided;
    ColDrawGroup* group;

    current = *segEnd;
    scene = fn_8010CBC0();
    hitPointPtr = result == NULL ? NULL : &hitPoint;

    for (pass = 0; pass < 10; pass++) {
        object = scene->objects;
        hits = 0;
        for (index = 0; index < scene->count; index++, object++) {
            GScolsys2GetObjEnable(index, &enabled);
            if (enabled == 0) {
                continue;
            }
            if (useAlternate) {
                group = object->edgeGroup1;
                if (group == NULL) {
                    continue;
                }
            } else {
                group = object->edgeGroup0;
                if (group == NULL) {
                    continue;
                }
            }
            if ((object->flags & 1) != 0) {
                colsysMakeSRTMatrix(inverse, index);
                colsysMakeRotMatrix(forward, index);
                collided = colsysCheckHitMdl(&current, group, inverse, forward,
                                       radius, hitPointPtr);
            } else {
                collided = colsysWallSphere(&current, group, radius, hitPointPtr);
            }
            if (collided != 0) {
                if (result == NULL) {
                    return 1;
                }
                hits++;
                if (hitPointPtr != NULL) {
                    current = *hitPointPtr;
                }
            }
        }
        if (hits <= 0) {
            break;
        }
    }

    if (pass <= 0) {
        return 0;
    }
    *result = current;
    return 1;
}

/* 0x8010F188 | 0x198 */
s32 GScolsys2HitNpcCollision(Vec3f* start, Vec3f* end, f32 radius, Vec3f* result)
{
    extern f32 PSVECDistance(const Vec3f*, const Vec3f*);
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    Vec3f segmentEnd;
    Vec3f segmentStart;
    Vec3f direction;
    f32 distance;
    f32 step;
    f32 next;
    f32 position;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    distance = PSVECDistance(start, end);
    step = 0.0f;
    if (distance > step) {
        step = radius / distance;
        if (step > 1.0f) {
            step = 1.0f;
        }
    }

    PSVECSubtract(end, start, &direction);
    position = 0.0f;
    while (position < 1.0f) {
        next = position + step;
        if (next > 1.0f) {
            next = 1.0f;
        }
        PSVECScale(&direction, &segmentStart, position);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&direction, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (colsysWallAll(&segmentStart, &segmentEnd, radius, result, 1)) {
            return 1;
        }
        if (step <= 0.0f) {
            break;
        }
        position += step;
    }
    return 0;
}

/* 0x8010F320 | 0x198 */
s32 colsysWallSweep(Vec3f* start, Vec3f* end, f32 radius, Vec3f* result)
{
    extern f32 PSVECDistance(const Vec3f*, const Vec3f*);
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    Vec3f segmentEnd;
    Vec3f segmentStart;
    Vec3f direction;
    f32 distance;
    f32 step;
    f32 next;
    f32 position;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    distance = PSVECDistance(start, end);
    step = 0.0f;
    if (distance > step) {
        step = radius / distance;
        if (step > 1.0f) {
            step = 1.0f;
        }
    }

    PSVECSubtract(end, start, &direction);
    position = 0.0f;
    while (position < 1.0f) {
        next = position + step;
        if (next > 1.0f) {
            next = 1.0f;
        }
        PSVECScale(&direction, &segmentStart, position);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&direction, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (colsysWallAll(&segmentStart, &segmentEnd, radius, result, 0)) {
            return 1;
        }
        if (step <= 0.0f) {
            break;
        }
        position += step;
    }
    return 0;
}
