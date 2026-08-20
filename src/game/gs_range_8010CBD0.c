/**
 * @file gs_range_8010CBD0.c
 * @brief gs-engine code, 0x8010CBD0 - 0x8010F6A0 (22 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 *
 * Boundary bug fix: the 8 functions below (0x8010E138-0x8010F6A0)
 * were physically sitting in game/gs_field_colquery.c -- outside that
 * unit's own splits.txt range (0x8010F6A0-0x801140DC) -- while this
 * unit's declared range already covers them. Relocated here during
 * the gs_field_colquery.c 6-way split so this unit scores real
 * progress instead of 0%. fn_8010E138/E53C/EB28/EFE4/F188 previously
 * carried invented "GSfield_RayCast/SphereSweep/SweepAgainstMesh/
 * LinePlaneTest/ArcTest" names/signatures from an earlier bad
 * campaign pass (same class of issue documented in
 * include/game/gs_colsys.h); reverted to the standard fn_<addr>
 * placeholder since no confirmed symbols.txt name exists yet for any
 * of them. fn_8010F4B8 and fn_8010F5A4 likewise carried invented
 * names ("GSfield_..." helpers that were never actually called under
 * those names); renamed to their confirmed symbols.txt names below.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

/* ===================================================================
 * External state and convenience aliases shared with the other colsys units
 * =================================================================== */
extern GSColSysState lbl_80404C68;

#define COL_STATE (&lbl_80404C68)
#define COL_LAYER_IDX (COL_STATE->activeLayer)
#define COL_LAYER_PTR(n) ((void*)((u8*)COL_STATE + 4 + (n) * GSCOLSYS_LAYER_SIZE))

typedef struct CcdOffsetBlock {
    void* field_00;
    u32 field_04;
    void* field_08;
    void* field_0C;
} CcdOffsetBlock;

typedef struct CcdOffsetRecord {
    u8 pad_00[0x24];
    CcdOffsetBlock* field_24;
    CcdOffsetBlock* field_28;
    CcdOffsetBlock* field_2C;
    CcdOffsetBlock* field_30;
    CcdOffsetBlock* field_34;
    CcdOffsetBlock* field_38;
    u32 field_3C;
} CcdOffsetRecord;

typedef struct CCD_FILEHEAD {
    CcdOffsetRecord* records;
    u32 count;
} CCD_FILEHEAD;

void _offsetCCD__FP12CCD_FILEHEAD(CCD_FILEHEAD* head)
{
    u8* base;
    CcdOffsetRecord* record;
    CcdOffsetBlock* block;
    u32 i;

    if (head == NULL) {
        return;
    }
    base = (u8*)head;
    head->records = (CcdOffsetRecord*)(base + (u32)head->records);
    record = head->records;

    for (i = 0; i < head->count; i++, record++) {
        if (record->field_24 != NULL) {
            record->field_24 =
                (CcdOffsetBlock*)(base + (u32)record->field_24);
            block = record->field_24;
            if (block->field_00 != NULL) {
                block->field_00 = base + (u32)block->field_00;
            }
            if (block->field_08 != NULL) {
                block->field_08 = base + (u32)block->field_08;
            }
            if (block->field_0C != NULL) {
                block->field_0C = base + (u32)block->field_0C;
            }
        }
        if (record->field_28 != NULL) {
            record->field_28 =
                (CcdOffsetBlock*)(base + (u32)record->field_28);
            block = record->field_28;
            if (block->field_00 != NULL) {
                block->field_00 = base + (u32)block->field_00;
            }
            if (block->field_08 != NULL) {
                block->field_08 = base + (u32)block->field_08;
            }
            if (block->field_0C != NULL) {
                block->field_0C = base + (u32)block->field_0C;
            }
        }
        if (record->field_2C != NULL) {
            record->field_2C =
                (CcdOffsetBlock*)(base + (u32)record->field_2C);
            block = record->field_2C;
            if (block->field_00 != NULL) {
                block->field_00 = base + (u32)block->field_00;
            }
            if (block->field_08 != NULL) {
                block->field_08 = base + (u32)block->field_08;
            }
            if (block->field_0C != NULL) {
                block->field_0C = base + (u32)block->field_0C;
            }
        }
        if (record->field_30 != NULL) {
            record->field_30 =
                (CcdOffsetBlock*)(base + (u32)record->field_30);
            block = record->field_30;
            if (block->field_00 != NULL) {
                block->field_00 = base + (u32)block->field_00;
            }
        }
        if (record->field_34 != NULL) {
            record->field_34 =
                (CcdOffsetBlock*)(base + (u32)record->field_34);
            block = record->field_34;
            if (block->field_00 != NULL) {
                block->field_00 = base + (u32)block->field_00;
            }
            if (block->field_08 != NULL) {
                block->field_08 = base + (u32)block->field_08;
            }
            if (block->field_0C != NULL) {
                block->field_0C = base + (u32)block->field_0C;
            }
        }
        if (record->field_38 != NULL) {
            record->field_38 =
                (CcdOffsetBlock*)(base + (u32)record->field_38);
            block = record->field_38;
            if (block->field_00 != NULL) {
                block->field_00 = base + (u32)block->field_00;
            }
        }
    }
}

/* 0x8010CBD0 | 0x34 */
void* GScolsys2GetCurFloor(void) {
    s32 layer;

    layer = COL_LAYER_IDX;
    if (layer < 0 || layer >= GSCOLSYS_MAX_LAYERS) {
        return NULL;
    }
    return COL_LAYER_PTR(layer);
}

/* 0x8010CC04 | 0x50 */
extern void GSgfxDLFree(void* displayList);

s32 GScolsys2UnloadCCD(void) {
    COL_STATE->wzxDataPtr = NULL;
    if (COL_STATE->displayList != NULL) {
        GSgfxDLFree(COL_STATE->displayList);
        COL_STATE->displayList = NULL;
    }
    return 1;
}

typedef struct ColLayerFlagEntry {
    u8 pad_00[0x10];
    u16 flags;
    u8 pad_12[2];
} ColLayerFlagEntry;

typedef struct ColLayer {
    u8 pad_000[0xA00];
    ColLayerFlagEntry flagEntries[48];
} ColLayer;

/* 0x8010CC54 | 0x118 */
void fn_8010CC54(void)
{
    ColLayer* layer;
    u32 i;

    layer = GScolsys2GetCurFloor();
    if (layer != NULL) {
        for (i = 0; i < 48; i++) {
            layer->flagEntries[i].flags &= 0xFFFE;
        }
    }
    COL_STATE->wzxDataPtr = NULL;
}

typedef struct ColRecordVector {
    u32 x;
    u32 y;
    u32 z;
} ColRecordVector;

typedef struct ColSourceRecord {
    ColRecordVector vector[3];
    u8 pad_24[0x1C];
} ColSourceRecord;

typedef struct ColSourceDescriptor {
    ColSourceRecord* records;
    u32 count;
} ColSourceDescriptor;

typedef struct ColLayerRecord {
    ColRecordVector vector[3];
    u16 flags;
    u8 pad_26[2];
} ColLayerRecord;

/* 0x8010CD6C | 0x98 */
void fn_8010CD6C(void)
{
    ColSourceDescriptor* descriptor;
    u32 i;
    ColSourceRecord* source;
    ColLayerRecord* destination;

    descriptor = COL_STATE->wzxDataPtr;
    if (descriptor == NULL) {
        return;
    }

    i = 0;
    source = descriptor->records;
    destination = COL_LAYER_PTR(COL_LAYER_IDX);
    while (i < descriptor->count) {
        destination->vector[0] = source->vector[0];
        destination->vector[1] = source->vector[1];
        destination->vector[2] = source->vector[2];
        destination->flags = 0;
        i++;
        source++;
        destination++;
    }
}

/* 0x8010CFE4 | 0x54 */
s32 fn_8010CFE4(void* fileHead)
{
    if (COL_LAYER_IDX < 0) {
        return 0;
    }
    _offsetCCD__FP12CCD_FILEHEAD(fileHead);
    COL_STATE->wzxDataPtr = fileHead;
    return 1;
}

/* 0x8010D038 | 0x2C */
s32 fn_8010D038(void) {
    s32 layer;

    layer = COL_LAYER_IDX;
    if (layer < 0) {
        return 0;
    }
    COL_LAYER_IDX = layer - 1;
    return 1;
}

typedef union ColStateAccess {
    GSColSysState state;
    u8 bytes[sizeof(GSColSysState)];
} ColStateAccess;

typedef struct ColResetFlagEntry {
    u8 pad_00[0x12];
    u16 flags;
} ColResetFlagEntry;

typedef struct ColResetLayerView {
    u8 pad_000[0xA02];
    ColResetFlagEntry flagEntries[48];
} ColResetLayerView;

static inline void ColResetLayer(GSColSysState* state, s32 layer)
{
    ColStateAccess* access;
    ColResetLayerView* layerView;
    u32 i;

    state->wzxDataPtr = NULL;
    access = (ColStateAccess*)state;
    layerView = (ColResetLayerView*)&access->bytes[layer * GSCOLSYS_LAYER_SIZE];
    for (i = 0; i < 48; i++) {
        layerView->flagEntries[i].flags &= 0xFFFE;
    }
}

/* 0x8010D064 | 0x10C */
s32 fn_8010D064(void)
{
    GSColSysState* state;
    s32 newLayer;

    state = COL_STATE;
    newLayer = state->activeLayer + 1;
    if (newLayer >= GSCOLSYS_MAX_LAYERS) {
        return 0;
    }

    ColResetLayer(state, newLayer);
    state->activeLayer = newLayer;
    return 1;
}

/* 0x8010D170 | 0x9C */
void fn_8010D170(void)
{
    extern u8* fn_800D7894(void);
    extern void fn_800D7868(u8*, u32, u32, u32, u32, u8, u32, u8);
    u8* handle;

    COL_STATE->activeLayer = 0;
    COL_STATE->displayList = NULL;
    handle = fn_800D7894();
    COL_STATE->gfxRenderHandle = (u32)handle;
    fn_800D7868(handle, 1, 0, 1, 4, 0, 0, 0);
    fn_800D7868((u8*)COL_STATE->gfxRenderHandle, 4, 0, 6, 10, 0, 0, 0);
    COL_STATE->displayList = NULL;
}

typedef f32 ColVec3[3];
typedef f32 ColMtx[3][4];

typedef struct ColDrawGroup {
    u8* data;
    u32 count;
} ColDrawGroup;

typedef struct ColDrawObject {
    u8 pad_00[0x24];
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

typedef union ColDrawColor {
    u32 packed;
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    } channel;
} ColDrawColor;

extern ColDrawScene* fn_8010CBC0(void);
extern void fn_800DA028(s32);
extern void fn_800D7820(void*);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800DA4C4(s32, s32, s32);
extern void fn_800DA1E8(s32, s32, s32);
extern void fn_800D9ED8(s32);
extern void fn_8010CA30(ColMtx out, u32 index);
extern void fn_8010C8D0(ColMtx out, u32 index);
extern void PSMTXMultVec(ColMtx, const ColVec3, ColVec3);
extern void fn_800D6A00(s32);
extern void fn_800D67BC(s32);
extern void fn_800D6680(f32, f32, f32);
extern void fn_800D5CB8(s32, u8, u8, u8, u8);
extern void fn_800D6728(void);
extern void* GScolsys2Draw(void);
extern void GSgfxDLDraw(void*);
extern void fn_800D30AC(void);
extern u32 lbl_8047CEB8;
extern u32 lbl_8047CEBC;
extern u32 lbl_8047CEC0;
extern u32 lbl_8047CEC4;

static inline void ColDrawSetColor(ColDrawColor color)
{
    fn_800D5CB8(0, color.channel.r, color.channel.g,
                color.channel.b, color.channel.a);
}

void fn_8010D20C(void* model, ColMtx matrix, ColMtx normalMatrix)
{
    ColDrawGroup* group;
    ColDrawColor color;
    ColVec3 transformed;
    u8* triangle;
    u32 i;
    s32 vertex;
    u8 flags;
    u32 level;

    group = model;
    triangle = group->data;
    fn_800D6A00(3);

    for (i = 0; i < group->count; i++, triangle += 0x34) {
        memset(&color, 0, sizeof(color));
        color.channel.a = 0xC0;

        flags = triangle[0x30];
        level = flags >> 4;
        color.channel.g =
            (u8)(127.0f * ((f32)level / 15.0f) + 128.0f);
        level = (flags & 0xF) + 1;
        if (level >= 16) {
            level = 0;
        }
        color.channel.b =
            (u8)(255.0f * ((f32)level / 15.0f));

        level = triangle[0x31] >> 4;
        if (level > 0) {
            color.channel.r = level * 4 + 0xC0;
        }

        fn_800D67BC(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(triangle + vertex * 12),
                         transformed);
            fn_800D6680(transformed[0], transformed[1], transformed[2]);
            ColDrawSetColor(color);
        }
        fn_800D6728();
    }
}

static inline void ColDrawEdges(ColMtx matrix, ColDrawGroup* group,
                                ColDrawColor color, u32 stride)
{
    ColVec3 transformed[3];
    u8* element;
    u32 i;
    s32 vertex;
    s32 next;

    if (group == NULL) {
        return;
    }
    element = group->data;
    for (i = 0; i < group->count; i++, element += stride) {
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed[vertex]);
        }
        fn_800D6A00(1);
        for (vertex = 0; vertex < 3; vertex++) {
            next = vertex + 1;
            if (next >= 3) {
                next = 0;
            }
            fn_800D67BC(2);
            fn_800D6680(transformed[vertex][0], transformed[vertex][1],
                        transformed[vertex][2]);
            ColDrawSetColor(color);
            fn_800D6680(transformed[next][0], transformed[next][1],
                        transformed[next][2]);
            ColDrawSetColor(color);
            fn_800D6728();
        }
    }
}

static inline void ColDrawFaces(ColMtx matrix, ColDrawGroup* group,
                                ColDrawColor color, u32 stride)
{
    ColVec3 transformed;
    u8* element;
    u32 i;
    s32 vertex;

    if (group == NULL) {
        return;
    }
    element = group->data;
    fn_800D6A00(3);
    for (i = 0; i < group->count; i++, element += stride) {
        fn_800D67BC(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            fn_800D6680(transformed[0], transformed[1], transformed[2]);
            ColDrawSetColor(color);
        }
        fn_800D6728();
    }
}

/* Record the collision-debug geometry into a display list. */
#pragma push
#pragma inline_depth(8)
#pragma inline_max_size(10000)
void* GScolsys2Draw(void)
{
    extern u8 GSgfxDLBegin(void* buffer, u32 size);
    extern void* GSgfxDLEnd(void);
    extern s32 printf(const char*, ...);
    extern const char lbl_80272050[];
    ColDrawScene* scene;
    ColDrawObject* object;
    ColMtx matrix;
    ColMtx normalMatrix;
    ColDrawColor color;
    u32 i;

    scene = fn_8010CBC0();
    if (scene == NULL) {
        return NULL;
    }

    fn_800DA028(1);
    fn_800D7820(*(void**)((u8*)&lbl_80404C68 + 0x3708));
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800DA4C4(1, 6, 7);
    fn_800DA1E8(1, 2, 1);
    fn_800D9ED8(0);

    if (GSgfxDLBegin(*(void**)((u8*)&lbl_80404C68 + 0x3708), 0x80000) == 0) {
        printf(lbl_80272050);
        return NULL;
    }

    object = scene->objects;
    for (i = 0; i < scene->count; i++, object++) {
        if ((object->flags & 1) != 0) {
            continue;
        }

        fn_8010CA30(matrix, i);
        fn_8010C8D0(normalMatrix, i);
        if (object->model != NULL) {
            fn_8010D20C(object->model, matrix, normalMatrix);
        }

        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup0, color, 0x34);
        color.packed = lbl_8047CEBC;
        ColDrawFaces(matrix, object->faceGroup0, color, 0x34);
        color.packed = lbl_8047CEC0;
        ColDrawFaces(matrix, object->faceGroup1, color, 0x34);
        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup1, color, 0x34);
        color.packed = lbl_8047CEC4;
        ColDrawFaces(matrix, object->faceGroup2, color, 0x30);
    }

    return GSgfxDLEnd();
}
#pragma pop

#pragma push
#pragma inline_depth(8)
#pragma inline_max_size(10000)
void fn_8010D8D4(void)
{
    ColDrawScene* scene;
    ColDrawObject* object;
    ColMtx matrix;
    ColMtx normalMatrix;
    ColDrawColor color;
    u8* state;
    u8* layer;
    void* displayList;
    u32 activeLayer;
    u32 i;

    scene = fn_8010CBC0();
    if (scene == NULL) {
        return;
    }

    fn_800DA028(1);
    state = (u8*)&lbl_80404C68;
    fn_800D7820(*(void**)(state + 0x3708));
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800DA4C4(1, 6, 7);
    fn_800DA1E8(1, 2, 1);
    fn_800D9ED8(0);

    object = scene->objects;
    activeLayer = *(u32*)(state + 0x3704);
    layer = state + activeLayer * 0xDC0 + 4;
    for (i = 0; i < scene->count; i++, object++, layer += 0x28) {
        if ((*(u16*)(layer + 0x24) & 1) != 0 ||
            (object->flags & 1) == 0) {
            continue;
        }

        fn_8010CA30(matrix, i);
        fn_8010C8D0(normalMatrix, i);
        if (object->model != NULL) {
            fn_8010D20C(object->model, matrix, normalMatrix);
        }

        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup0, color, 0x34);
        color.packed = lbl_8047CEBC;
        ColDrawFaces(matrix, object->faceGroup0, color, 0x34);
        color.packed = lbl_8047CEC0;
        ColDrawFaces(matrix, object->faceGroup1, color, 0x34);
        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup1, color, 0x34);
        color.packed = lbl_8047CEC4;
        ColDrawFaces(matrix, object->faceGroup2, color, 0x30);
    }

    displayList = *(void**)(state + 0x370C);
    if (displayList == NULL) {
        displayList = GScolsys2Draw();
        *(void**)(state + 0x370C) = displayList;
    }
    if (displayList != NULL) {
        GSgfxDLDraw(displayList);
        fn_800D30AC();
    }
}
#pragma pop

typedef struct ColWalkHit {
    f32 height;
    u16 surfaceType;
    u16 surfaceId;
    u8 layer;
    u8 subLayer;
    u8 pad_0A[2];
} ColWalkHit;

extern s32 fn_8010E138(void* origin, void* results);

static inline f32 GScolsys2WalkHeightDelta(Vec3f* position, ColWalkHit* hit)
{
    return position->y - hit[0].height;
}

/* 0x8010DE00 | 0xF0 */
s32 GScolsys2WalkGetLayer(Vec3f* position, u8* layer, u8* subLayer)
{
    ColWalkHit hits[8];
    s32 count;
    s32 i;
    s32 closest;
    f32 distance;
    f32 closestDistance;

    count = fn_8010E138(position, hits);
    if (count <= 0) {
        return 0;
    }

    distance = GScolsys2WalkHeightDelta(position, hits);
    distance = distance > 0.0f ? distance : -distance;
    closest = 0;
    closestDistance = distance;
    for (i = 1; i < count; i++) {
        distance = position->y - hits[i].height;
        distance = distance > 0.0f ? distance : -distance;
        if (closestDistance > distance) {
            closest = i;
            closestDistance = distance;
        }
    }

    *layer = hits[closest].layer;
    *subLayer = hits[closest].subLayer;
    return 1;
}

s32 getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(
    Vec3f* out, Vec3f* point, Vec3f* vertices, Vec3f* plane)
{
    extern f32 lbl_8047CEE0;
    extern f32 lbl_8047CEE4;
    extern f32 lbl_8047CEE8;
    f32 minX;
    f32 minZ;
    f32 maxX;
    f32 maxZ;
    f32 cross;
    Vec3f* a;
    Vec3f* b;
    s32 i;

    minX = lbl_8047CEE4;
    minZ = lbl_8047CEE4;
    maxX = lbl_8047CEE8;
    maxZ = lbl_8047CEE8;
    for (i = 0; i < 3; i++) {
        if (minX > vertices[i].x) {
            minX = vertices[i].x;
        }
        if (minZ > vertices[i].z) {
            minZ = vertices[i].z;
        }
        if (maxX < vertices[i].x) {
            maxX = vertices[i].x;
        }
        if (maxZ < vertices[i].z) {
            maxZ = vertices[i].z;
        }
    }

    if (minX > point->x || minZ > point->z ||
        maxX < point->x || maxZ < point->z) {
        return 0;
    }

    for (i = 0; i < 3; i++) {
        a = &vertices[i];
        b = &vertices[(i + 1) % 3];
        cross = (b->x - a->x) * (point->z - a->z) -
                (b->z - a->z) * (point->x - a->x);
        if (cross > lbl_8047CEE0) {
            return 0;
        }
    }

    if (plane->y == lbl_8047CEE0) {
        return 0;
    }
    out->x = point->x;
    out->y = point->y +
             (plane->y * (point->y - vertices[0].y) +
              plane->x * (point->x - vertices[0].x) -
              plane->z * (point->z - vertices[0].z)) /
                 plane->y;
    out->z = point->z;
    return 1;
}

/* 0x8010E138 | 0x404 */
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

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010E138(void* origin, void* direction) {
    ColDrawScene* scene;
    GScolsys2Region* region;
    ColWalkHit temporary[8];
    ColWalkHit* tempWrite;
    ColWalkHit* tempRead;
    ColWalkHit* outHit;
    ColMtx inverse;
    ColMtx forward;
    Vec3f transformed[3];
    Vec3f plane;
    Vec3f hitPoint;
    Vec3f* point;
    GScolsys2TriangleList* list;
    GSFieldFixedMdlEventList* grid;
    GSFieldFixedMdlCell* cell;
    GScolsys2Triangle* triangle;
    u32* triangleIndices;
    s32 enabled;
    s32 totalCount;
    u32 regionIndex;
    s32 temporaryCount;
    s32 i;
    s32 duplicate;
    u8 field30;
    u8 field31;
    s32 cellX;
    s32 cellZ;
    u32 triCount;
    u32 triIdx;

    point = origin;
    outHit = direction;
    totalCount = 0;
    scene = fn_8010CBC0();
    if (scene == NULL) {
        return 0;
    }

    region = (GScolsys2Region*)scene->objects;
    for (regionIndex = 0;
         regionIndex < scene->count && totalCount < 8;
         regionIndex++, region = (GScolsys2Region*)((u8*)region + 0x40))
    {
        GScolsys2GetObjEnable(regionIndex, &enabled);
        if (enabled == 0 || region->triList == NULL) {
            continue;
        }

        temporaryCount = 0;
        tempWrite = temporary;

        if ((region->flags & 1) != 0) {
            fn_8010CA30(inverse, regionIndex);
            fn_8010C8D0(forward, regionIndex);

            list = (GScolsys2TriangleList*)region->triList;
            triangle = list->triangles;
            for (triIdx = 0;
                 triIdx < list->count && temporaryCount < 8;
                 triIdx++, triangle++)
            {
                for (i = 0; i < 3; i++) {
                    PSMTXMultVec(inverse, (const f32*)&triangle->verts[i],
                                 (f32*)&transformed[i]);
                }
                PSMTXMultVec(forward, (const f32*)&triangle->normal,
                             (f32*)&plane);
                if (getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(
                        &hitPoint, point, transformed, &plane) == 0)
                {
                    continue;
                }

                tempRead = temporary;
                for (i = 0; i < temporaryCount; i++, tempRead++) {
                    if (tempRead->height == hitPoint.y) {
                        break;
                    }
                }
                duplicate = 0;
                if (i >= temporaryCount) {
                    tempWrite->height = hitPoint.y;
                    field30 = *(u8*)((u8*)triangle + 0x30);
                    tempWrite->surfaceType =
                        ((field30 >> 4) == 0xF) ? 0xFFFF : (u16)(field30 >> 4);
                    tempWrite->surfaceId =
                        ((field30 & 0xF) == 0xF) ? 0xFFFF : (u16)(field30 & 0xF);
                    field31 = *(u8*)((u8*)triangle + 0x31);
                    tempWrite->layer = (u8)(field31 >> 4);
                    tempWrite->subLayer = (u8)(field31 & 0xF);
                    duplicate = 1;
                }
                if (duplicate != 0) {
                    tempWrite++;
                    temporaryCount++;
                }
            }
        } else {
            grid = (GSFieldFixedMdlEventList*)region->triList;
            cellX = (s32)((point->x - grid->minX) / grid->cellWidth);
            if (cellX >= 0 && cellX < (s32)grid->cellCountX) {
                cellZ = (s32)((point->z - grid->minZ) / grid->cellDepth);
                if (cellZ >= 0 && cellZ < (s32)grid->cellCountZ) {
                    cell = grid->cells + (cellX + cellZ * grid->cellCountX);
                    triangleIndices = grid->triangleIndices + cell->firstIndex;
                    triCount = cell->count;
                    for (triIdx = 0;
                         triIdx < triCount && temporaryCount < 8;
                         triIdx++, triangleIndices++)
                    {
                        triangle = grid->triangles + *triangleIndices;
                        if (getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(
                                &hitPoint, point, &triangle->verts[0],
                                &triangle->normal) == 0)
                        {
                            continue;
                        }

                        tempRead = temporary;
                        for (i = 0; i < temporaryCount; i++, tempRead++) {
                            if (tempRead->height == hitPoint.y) {
                                break;
                            }
                        }
                        duplicate = 0;
                        if (i >= temporaryCount) {
                            tempWrite->height = hitPoint.y;
                            field30 = *(u8*)((u8*)triangle + 0x30);
                            tempWrite->surfaceType =
                                ((field30 >> 4) == 0xF) ? 0xFFFF
                                                        : (u16)(field30 >> 4);
                            tempWrite->surfaceId =
                                ((field30 & 0xF) == 0xF) ? 0xFFFF
                                                         : (u16)(field30 & 0xF);
                            field31 = *(u8*)((u8*)triangle + 0x31);
                            tempWrite->layer = (u8)(field31 >> 4);
                            tempWrite->subLayer = (u8)(field31 & 0xF);
                            duplicate = 1;
                        }
                        if (duplicate != 0) {
                            tempWrite++;
                            temporaryCount++;
                        }
                    }
                }
            }
        }

        tempRead = temporary;
        for (i = 0; i < temporaryCount && totalCount < 8; i++) {
            *outHit = *tempRead;
            outHit++;
            tempRead++;
            totalCount++;
        }
    }

    return totalCount;
}
#pragma pop

/* 0x8010E53C | 0x5EC */
typedef struct GSfieldEdgeMasks {
    u16 values[3];
} GSfieldEdgeMasks;

extern f32 GScolsy2UtilGetSidePlanePoint(void*, void*, void*);
extern void GScolsy2UtilGetCpPlanePoint(void*, void*, void*, void*);
extern s32 GScolsy2UtilChkInTri(void*, void*, void*);
extern void GScolsy2UtilGetPointExtentionLine(void*, void*, void*, f32);
f32 GScolsys2UtilGetCpLinePoint(Vec3f*, Vec3f*, Vec3f*, Vec3f*);

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010E53C(Vec3f* point, void* data, f32 radius, Vec3f* result) {
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    extern const f32 lbl_8047CF08[];
    extern f32 PSVECSquareDistance(void* a, void* b);
    GSFieldFixedMdlEventList* grid;
    GSFieldFixedMdlCell* cell;
    GScolsys2Triangle* tri;
    GSfieldEdgeMasks edgeMasksA;
    GSfieldEdgeMasks edgeMasksB;
    Vec3f planePoint;
    Vec3f cp;
    Vec3f lineCp;
    s32 startX;
    s32 startZ;
    s32 endX;
    s32 endZ;
    s32 x;
    s32 z;
    s32 hit;
    s32 edge;
    f32 radiusSq;

    grid = data;
    startX = (s32)((point->x - radius - grid->minX) / grid->cellWidth);
    if (startX < 0) {
        startX = 0;
    }
    startZ = (s32)((point->z - radius - grid->minZ) / grid->cellDepth);
    if (startZ < 0) {
        startZ = 0;
    }
    endX = (s32)((point->x + radius - grid->minX) / grid->cellWidth);
    if (endX > (s32)grid->cellCountX - 1) {
        endX = (s32)grid->cellCountX - 1;
    }
    endZ = (s32)((point->z + radius - grid->minZ) / grid->cellDepth);
    if (endZ > (s32)grid->cellCountZ - 1) {
        endZ = (s32)grid->cellCountZ - 1;
    }

    radiusSq = radius * radius;
    edgeMasksA.values[0] = 1;
    edgeMasksA.values[1] = 2;
    edgeMasksA.values[2] = 4;
    edgeMasksB = edgeMasksA;

    for (z = startZ; z <= endZ; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX; x++, cell++) {
            u32* triIndexPtr;
            u32 cellTriIdx;

            triIndexPtr = grid->triangleIndices + cell->firstIndex;
            cellTriIdx = 0;
            while (cellTriIdx < cell->count) {
                tri = grid->triangles + (*triIndexPtr);
                if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                    lbl_8047CF00)
                {
                    hit = 0;
                } else {
                    GScolsy2UtilGetCpPlanePoint(&cp, &tri->normal,
                                                (Vec3f*)tri, point);
                    if (PSVECSquareDistance(&cp, point) >= radiusSq) {
                        hit = 0;
                    } else if (GScolsy2UtilChkInTri(&cp, tri, &tri->normal) ==
                               0)
                    {
                        hit = 0;
                    } else {
                        lineCp = cp;
                        hit = 1;
                    }
                }
                if (hit != 0) {
                    if (result == NULL) {
                        return 1;
                    }
                    GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                                     lbl_8047CF08[0] + radius);
                    return 1;
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    for (z = startZ; z <= endZ; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX; x++, cell++) {
            u32* triIndexPtr;
            u32 cellTriIdx;

            triIndexPtr = grid->triangleIndices + cell->firstIndex;
            cellTriIdx = 0;
            while (cellTriIdx < cell->count) {
                tri = grid->triangles + (*triIndexPtr);
                if ((tri->flags & 7) == 0) {
                    hit = 0;
                } else if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri,
                                                         point) <
                           lbl_8047CF00)
                {
                    hit = 0;
                } else {
                    hit = 0;
                    for (edge = 0; edge < 3; edge++) {
                        s32 next = edge + 1;
                        f32 lineT;

                        if (next >= 3) {
                            next = 0;
                        }
                        if ((tri->flags & edgeMasksA.values[edge]) == 0) {
                            continue;
                        }
                        lineT = GScolsys2UtilGetCpLinePoint(
                            &lineCp, &tri->verts[edge], &tri->verts[next],
                            point);
                        if (lineT < lbl_8047CF00 || lineT > lbl_8047CF04) {
                            continue;
                        }
                        if (PSVECSquareDistance(&lineCp, point) < radiusSq) {
                            hit = 1;
                            break;
                        }
                    }
                }
                if (hit != 0) {
                    if (result == NULL) {
                        return 1;
                    }
                    GScolsy2UtilGetPointExtentionLine(
                        result, &lineCp, point, lbl_8047CF08[0] + radius);
                    return 1;
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    for (z = startZ; z <= endZ; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX; x++, cell++) {
            u32* triIndexPtr;
            u32 cellTriIdx;

            triIndexPtr = grid->triangleIndices + cell->firstIndex;
            cellTriIdx = 0;
            while (cellTriIdx < cell->count) {
                tri = grid->triangles + (*triIndexPtr);
                if ((tri->flags & 7) == 0) {
                    hit = 0;
                } else if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri,
                                                         point) <
                           lbl_8047CF00)
                {
                    hit = 0;
                } else {
                    hit = 0;
                    for (edge = 0; edge < 3; edge++) {
                        s32 next = edge + 2;

                        if (next >= 3) {
                            next -= 3;
                        }
                        if ((tri->flags & edgeMasksB.values[edge]) == 0 ||
                            (tri->flags & edgeMasksB.values[next]) == 0)
                        {
                            continue;
                        }
                        if (PSVECSquareDistance(&tri->verts[edge], point) <
                            radiusSq)
                        {
                            lineCp = tri->verts[edge];
                            hit = 1;
                            break;
                        }
                    }
                }
                if (hit != 0) {
                    if (result == NULL) {
                        return 1;
                    }
                    GScolsy2UtilGetPointExtentionLine(
                        result, &lineCp, point, lbl_8047CF08[0] + radius);
                    return 1;
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    return 0;
}
#pragma pop

/* 0x8010EB28 | 0x4BC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010EB28(Vec3f* point, void* data, ColMtx inverse,
                ColMtx forward, f32 radius, Vec3f* result) {
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    extern const f32 lbl_8047CF08[];
    extern f32 PSVECSquareDistance(void* a, void* b);
    ColDrawGroup* group;
    GScolsys2Triangle* tri;
    Vec3f transformedVerts[3];
    Vec3f transformedNormal;
    Vec3f planePoint;
    Vec3f cp;
    Vec3f lineCp;
    Vec3f* currentVert;
    Vec3f* otherVert;
    u16 edgeMasksA[3];
    u16 edgeMasksB[3];
    u32 i;
    s32 edge;
    s32 next;
    s32 hit;
    f32 radiusSq;
    f32 lineT;

    group = data;
    tri = (GScolsys2Triangle*)group->data;
    radiusSq = radius * radius;

    i = 0;
    hit = 0;
    for (; i < group->count && hit == 0;
         i++, tri = (GScolsys2Triangle*)((u8*)tri + 0x34)) {
        currentVert = tri->verts;
        otherVert = transformedVerts;
        for (edge = 0; edge < 3; edge++, currentVert++, otherVert++) {
            PSMTXMultVec(inverse, (const f32*)currentVert,
                         (f32*)otherVert);
        }
        PSMTXMultVec(forward, (const f32*)&tri->normal, (f32*)&transformedNormal);

        if (GScolsy2UtilGetSidePlanePoint(&transformedNormal, transformedVerts,
                                          point) < lbl_8047CF00)
        {
            hit = 0;
        } else {
            GScolsy2UtilGetCpPlanePoint(&cp, &transformedNormal,
                                        transformedVerts, point);
            if (PSVECSquareDistance(&cp, point) >= radiusSq) {
                hit = 0;
            } else if (GScolsy2UtilChkInTri(&cp, transformedVerts,
                                            &transformedNormal) == 0)
            {
                hit = 0;
            } else {
                lineCp = cp;
                hit = 1;
            }
        }
    }
    if (hit != 0) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                         lbl_8047CF08[0] + radius);
        return 1;
    }

    edgeMasksA[0] = 1;
    edgeMasksA[1] = 2;
    edgeMasksA[2] = 4;

    i = 0;
    tri = (GScolsys2Triangle*)group->data;
    hit = 0;
    for (; i < group->count && hit == 0;
         i++, tri = (GScolsys2Triangle*)((u8*)tri + 0x34)) {
        if ((tri->flags & 7) == 0) {
            continue;
        }
        for (edge = 0; edge < 3; edge++) {
            PSMTXMultVec(inverse, (const f32*)&tri->verts[edge],
                         (f32*)&transformedVerts[edge]);
        }
        PSMTXMultVec(forward, (const f32*)&tri->normal, (f32*)&transformedNormal);

        if (GScolsy2UtilGetSidePlanePoint(&transformedNormal, transformedVerts,
                                          point) < lbl_8047CF00)
        {
            hit = 0;
        } else {
            hit = 0;
            currentVert = transformedVerts;
            for (edge = 0; edge < 3; edge++, currentVert++) {
                next = edge + 1;
                if (next >= 3) {
                    next = 0;
                }
                if ((tri->flags & edgeMasksA[edge]) == 0) {
                    continue;
                }
                otherVert = &transformedVerts[next];
                lineT = GScolsys2UtilGetCpLinePoint(&lineCp, currentVert,
                                                    otherVert, point);
                if (lineT < lbl_8047CF00 || lineT > lbl_8047CF04) {
                    continue;
                }
                if (PSVECSquareDistance(&lineCp, point) < radiusSq) {
                    hit = 1;
                    break;
                }
            }
        }
    }
    if (hit != 0) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                         lbl_8047CF08[0] + radius);
        return 1;
    }

    edgeMasksB[0] = 1;
    edgeMasksB[1] = 2;
    edgeMasksB[2] = 4;

    i = 0;
    tri = (GScolsys2Triangle*)group->data;
    hit = 0;
    for (; i < group->count && hit == 0;
         i++, tri = (GScolsys2Triangle*)((u8*)tri + 0x34)) {
        if ((tri->flags & 7) == 0) {
            continue;
        }
        for (edge = 0; edge < 3; edge++) {
            PSMTXMultVec(inverse, (const f32*)&tri->verts[edge],
                         (f32*)&transformedVerts[edge]);
        }
        PSMTXMultVec(forward, (const f32*)&tri->normal, (f32*)&transformedNormal);

        if (GScolsy2UtilGetSidePlanePoint(&transformedNormal, transformedVerts,
                                          point) < lbl_8047CF00)
        {
            hit = 0;
        } else {
            hit = 0;
            for (edge = 0; edge < 3; edge++) {
                next = edge + 2;
                if (next >= 3) {
                    next -= 3;
                }
                if ((tri->flags & edgeMasksB[edge]) == 0 ||
                    (tri->flags & edgeMasksB[next]) == 0)
                {
                    continue;
                }
                if (PSVECSquareDistance(&transformedVerts[edge], point) <
                    radiusSq)
                {
                    lineCp = transformedVerts[edge];
                    hit = 1;
                    break;
                }
            }
        }
    }
    if (hit != 0) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                         lbl_8047CF08[0] + radius);
        return 1;
    }

    return 0;
}
#pragma pop

/* 0x8010EFE4 | 0x1A4 */
s32 fn_8010EFE4(Vec3f* segStart, Vec3f* segEnd, f32 radius,
                Vec3f* result, s32 useAlternate)
{
    ColDrawScene* scene;
    ColDrawObject* object;
    Vec3f current;
    Vec3f hitPoint;
    Vec3f* hitPointPtr;
    ColMtx inverse;
    ColMtx forward;
    s32 pass;

    current = *segEnd;
    scene = fn_8010CBC0();
    hitPointPtr = result != NULL ? &hitPoint : NULL;
    pass = 0;

    do {
        s32 hits;
        u32 index;

        object = scene->objects;
        hits = 0;
        for (index = 0; index < scene->count; index++, object++) {
            ColDrawGroup* group;
            s32 enabled;
            s32 collided;

            GScolsys2GetObjEnable(index, &enabled);
            if (enabled == 0) {
                continue;
            }
            group = useAlternate ? object->edgeGroup1 : object->edgeGroup0;
            if (group == NULL) {
                continue;
            }
            if ((object->flags & 1) != 0) {
                fn_8010CA30(inverse, index);
                fn_8010C8D0(forward, index);
                collided = fn_8010EB28(&current, group, inverse, forward,
                                       radius, hitPointPtr);
            } else {
                collided =
                    fn_8010E53C(&current, group, radius, hitPointPtr);
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
        pass++;
    } while (pass < 10);

    if (pass <= 0) {
        return 0;
    }
    *result = current;
    return 1;
}

/* 0x8010F188 | 0x198 */
s32 fn_8010F188(Vec3f* start, Vec3f* end, f32 radius, Vec3f* result)
{
    extern f32 PSVECDistance(const Vec3f*, const Vec3f*);
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    Vec3f direction;
    Vec3f segmentStart;
    Vec3f segmentEnd;
    f32 distance;
    f32 step;
    f32 position;
    f32 next;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    distance = PSVECDistance(start, end);
    step = lbl_8047CF00;
    if (distance > step) {
        step = radius / distance;
        if (step > lbl_8047CF04) {
            step = lbl_8047CF04;
        }
    }

    PSVECSubtract(end, start, &direction);
    position = lbl_8047CF00;
    while (position < lbl_8047CF04) {
        next = position + step;
        if (next > lbl_8047CF04) {
            next = lbl_8047CF04;
        }
        PSVECScale(&direction, &segmentStart, position);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&direction, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (fn_8010EFE4(&segmentStart, &segmentEnd, radius, result, 1)) {
            return 1;
        }
        if (step <= lbl_8047CF00) {
            break;
        }
        position += step;
    }
    return 0;
}

/* 0x8010F320 | 0x198 */
s32 fn_8010F320(Vec3f* start, Vec3f* end, f32 radius, Vec3f* result)
{
    extern f32 PSVECDistance(const Vec3f*, const Vec3f*);
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    Vec3f direction;
    Vec3f segmentStart;
    Vec3f segmentEnd;
    f32 distance;
    f32 step;
    f32 position;
    f32 next;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    distance = PSVECDistance(start, end);
    step = lbl_8047CF00;
    if (distance > step) {
        step = radius / distance;
        if (step > lbl_8047CF04) {
            step = lbl_8047CF04;
        }
    }

    PSVECSubtract(end, start, &direction);
    position = lbl_8047CF00;
    while (position < lbl_8047CF04) {
        next = position + step;
        if (next > lbl_8047CF04) {
            next = lbl_8047CF04;
        }
        PSVECScale(&direction, &segmentStart, position);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&direction, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (fn_8010EFE4(&segmentStart, &segmentEnd, radius, result, 0)) {
            return 1;
        }
        if (step <= lbl_8047CF00) {
            break;
        }
        position += step;
    }
    return 0;
}

/* 0x8010F4B8 | 0xEC */
s32 GScolsys2UtilGetCpPlaneLine(Vec3f* out, f32* tOut,
                               const Vec3f* normal,
                               const Vec3f* planePoint,
                               const Vec3f* lineStart,
                               const Vec3f* lineEnd) {
    Vec3f direction;
    f32 denominator;
    f32 t;
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    extern const f32 lbl_8047CF10;

    PSVECSubtract(lineEnd, lineStart, &direction);
    if ((denominator = normal->x * direction.x + normal->y * direction.y
                     + normal->z * direction.z) == lbl_8047CF10) {
        return 0;
    }

    t = (normal->x * (planePoint->x - lineStart->x)
       + normal->y * (planePoint->y - lineStart->y)
       + normal->z * (planePoint->z - lineStart->z)) / denominator;
    PSVECScale(&direction, &direction, t);
    PSVECAdd(&direction, lineStart, out);
    *tOut = t;
    return 1;
}

/* 0x8010F5A4 | 0xFC */
f32 GScolsys2UtilGetCpLinePoint(Vec3f* out, Vec3f* start, Vec3f* end,
                               Vec3f* point) {
    Vec3f direction;
    f32 lengthSquared;
    f32 t;
    extern void PSVECSubtract(void*, void*, void*);
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);
    extern const f32 lbl_8047CF10;

    PSVECSubtract(end, start, &direction);
    lengthSquared = direction.x * direction.x + direction.y * direction.y
                  + direction.z * direction.z;
    if (lbl_8047CF10 == lengthSquared) {
        out->x = start->x;
        out->y = start->y;
        out->z = start->z;
        return lbl_8047CF10;
    }

    t = (direction.x * (point->x - start->x)
       + direction.y * (point->y - start->y)
       + direction.z * (point->z - start->z)) / lengthSquared;
    PSVECScale(&direction, &direction, t);
    PSVECAdd(&direction, start, out);
    return t;
}
