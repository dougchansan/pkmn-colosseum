/**
 * @file gs_colsys_exact_8010D20C.c
 * @brief GScolsys2 debug draw, 0x8010D20C - 0x8010DE00, with its .sdata2
 *        pool (0x8047CEB8 - 0x8047CEE0).
 *
 * The tail of the GScolsys2 core TU. It matches XD's GScolsys2Draw.o
 * (NXXJ01.map: drawHitMdl 0x134, drawWalkMdl 0x1E8, makeDisplayListFixedObj,
 * GScolsys2Draw; drawSunMdl / drawCheckMdl / drawThruMdl UNUSED 0xCC each;
 * StarsMmd/Colo-XD-PBR-symbol-maps). colsysDrawWalkMdl is drawWalkMdl.
 * GScolsys2Draw here records the fixed objects' display list (XD
 * makeDisplayListFixedObj), and fn_8010D8D4 draws the rest and replays it
 * (XD GScolsys2Draw). The edge and face helpers take XD's names; each has
 * its own colour. GC6E01.map lists the four colours as the TU's first pool
 * entries (@1966, @1988, @2002, @2016); declared before colsysDrawWalkMdl, they
 * come out ahead of its float literals.
 *
 * What the match depends on:
 * - colsysDrawWalkMdl reads the triangle's surface, attr and layer bitfields into
 *   locals declared between the index and the triangle pointer. That
 *   numbering gives the index a low enough colouring degree that it is
 *   coloured after the vertex cursor (r26), as in retail.
 * - gfxVtxColor4u8 takes its colour components as s32 (as cursor_bios.c
 *   declares it). With u8 parameters the red byte, read straight from the
 *   colour local, is not hoisted out of the face loops.
 * - The callers test each group for NULL; the helpers do not.
 * - The declaration orders of the helpers' locals and of both callers'
 *   locals set the colouring order of the loop variables.
 * - The edge helper copies its colour through an address-taken store, so
 *   the bytes are reread from the stack after every call, as in retail.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern GSColSysState lbl_80404C68;
extern s32 GScolsys2GetObjEnable(s32 index, s32* enabled);
extern void* memset(void*, int, unsigned long);

typedef f32 ColVec3[3];
typedef f32 ColMtx[3][4];

typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;

typedef struct ColTri {
    /* 0x00 */ Vec3f verts[3];
    /* 0x24 */ Vec3f normal;
    /* 0x30 */ u8 surface : 4;
    /* 0x30 */ u8 attr : 4;
    /* 0x31 */ u8 layer : 4;
    /* 0x31 */ u8 subLayer : 4;
    /* 0x32 */ u16 id;
} ColTri;

typedef struct ColTriGroup {
    ColTri* tris;
    u32 count;
} ColTriGroup;

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


extern ColDrawScene* fn_8010CBC0(void);
extern void gfxSetCullMode(s32);
extern void gfxSetVtxDesc(void*);
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void gfxSetBlendMode(s32, s32, s32);
extern void gfxSetZMode(s32, s32, s32);
extern void fn_800D9ED8(s32);
extern void colsysMakeSRTMatrix(ColMtx out, u32 index);
extern void colsysMakeRotMatrix(ColMtx out, u32 index);
extern void PSMTXMultVec(ColMtx, const f32*, f32*);
extern void gfxSetPrimitive(s32);
extern void gfxBegin(s32);
extern void gfxVtxPos(f32, f32, f32);
extern void gfxVtxColor4u8(s32, s32, s32, s32, s32);
extern void gfxEnd(void);
extern void* GScolsys2Draw(void);
extern void GSgfxDLDraw(void*);
extern void fn_800D30AC(void);


/* The TU pool at 0x8047CEB8 starts with these four colours. */
static const GXColor lbl_8047CEB8 = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor lbl_8047CEBC = {0xFF, 0x00, 0xFF, 0xC0};
static const GXColor lbl_8047CEC0 = {0xFF, 0xFF, 0x00, 0xC0};
static const GXColor lbl_8047CEC4 = {0x00, 0xFF, 0xFF, 0xC0};

void colsysDrawWalkMdl(ColTriGroup* group, ColMtx matrix, ColMtx normalMatrix)
{
    u32 i;
    s32 surface;
    s32 attr;
    s32 level;
    s32 layer;
    ColTri* tri;
    s32 v;
    GXColor color;
    Vec3f pos;

    gfxSetPrimitive(3);
    tri = group->tris;
    for (i = 0; i < group->count; i++, tri++) {
        memset(&color, 0, sizeof(color));
        color.a = 0xC0;
        surface = tri->surface;
        attr = tri->attr;
        color.g = 127.0f * (surface / 15.0f) + 128.0f;
        level = attr + 1;
        if (level >= 16) {
            level = 0;
        }
        color.b = 255.0f * (level / 15.0f);
        layer = tri->layer;
        if (layer > 0) {
            color.r = layer * 4 + 0xC0;
        }

        gfxBegin(3);
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(matrix, &tri->verts[v].x, &pos.x);
            gfxVtxPos(pos.x, pos.y, pos.z);
            gfxVtxColor4u8(0, color.r, color.g, color.b, color.a);
        }
        gfxEnd();
    }
}

static inline void drawHitMdl(ColDrawGroup* head, ColMtx matrix)
{
    s32 vertex;
    ColVec3 transformed[3];
    u32 i;
    s32 next;
    u8* element;
    GXColor color;

    /* Retail rereads the colour bytes from the stack after every call;
     * only an address-taken copy gives that. */
    *(u32*)&color = *(const u32*)&lbl_8047CEB8;
    element = head->data;
    for (i = 0; i < head->count; i++, element += 0x34) {
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed[vertex]);
        }
        gfxSetPrimitive(1);
        for (vertex = 0; vertex < 3; vertex++) {
            next = vertex + 1;
            if (next >= 3) {
                next = 0;
            }
            gfxBegin(2);
            gfxVtxPos(transformed[vertex][0], transformed[vertex][1],
                        transformed[vertex][2]);
            gfxVtxColor4u8(0, color.r, color.g, color.b, color.a);
            gfxVtxPos(transformed[next][0], transformed[next][1],
                        transformed[next][2]);
            gfxVtxColor4u8(0, color.r, color.g, color.b, color.a);
            gfxEnd();
        }
    }
}

static inline void drawSunMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed;
    u32 i;
    s32 vertex;
    u8* element;
    GXColor color;

    color = lbl_8047CEBC;
    element = head->data;
    gfxSetPrimitive(3);
    for (i = 0; i < head->count; i++, element += 0x34) {
        gfxBegin(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            gfxVtxPos(transformed[0], transformed[1], transformed[2]);
            gfxVtxColor4u8(0, color.r, color.g, color.b, color.a);
        }
        gfxEnd();
    }
}

static inline void drawCheckMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed;
    u32 i;
    s32 vertex;
    u8* element;
    GXColor color;

    color = lbl_8047CEC0;
    element = head->data;
    gfxSetPrimitive(3);
    for (i = 0; i < head->count; i++, element += 0x34) {
        gfxBegin(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            gfxVtxPos(transformed[0], transformed[1], transformed[2]);
            gfxVtxColor4u8(0, color.r, color.g, color.b, color.a);
        }
        gfxEnd();
    }
}

static inline void drawThruMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed;
    u32 i;
    s32 vertex;
    u8* element;
    GXColor color;

    color = lbl_8047CEC4;
    element = head->data;
    gfxSetPrimitive(3);
    for (i = 0; i < head->count; i++, element += 0x30) {
        gfxBegin(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            gfxVtxPos(transformed[0], transformed[1], transformed[2]);
            gfxVtxColor4u8(0, color.r, color.g, color.b, color.a);
        }
        gfxEnd();
    }
}

/* Record the collision-debug geometry into a display list. */
void* GScolsys2Draw(void)
{
    extern u8 GSgfxDLBegin(void* buffer, u32 size);
    extern void* GSgfxDLEnd(void);
    extern s32 printf(const char*, ...);
    extern const char lbl_80272050[];
    ColDrawScene* scene;
    u32 i;
    ColMtx matrix;
    ColDrawObject* object;
    ColMtx normalMatrix;

    scene = fn_8010CBC0();
    if (scene == NULL) {
        return NULL;
    }

    gfxSetCullMode(1);
    gfxSetVtxDesc(*(void**)((u8*)&lbl_80404C68 + 0x3708));
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetBlendMode(1, 6, 7);
    gfxSetZMode(1, 2, 1);
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

        colsysMakeSRTMatrix(matrix, i);
        colsysMakeRotMatrix(normalMatrix, i);
        if (object->model != NULL) {
            colsysDrawWalkMdl(object->model, matrix, normalMatrix);
        }

        if (object->edgeGroup0 != NULL) {
            drawHitMdl(object->edgeGroup0, matrix);
        }
        if (object->faceGroup0 != NULL) {
            drawSunMdl(object->faceGroup0, matrix);
        }
        if (object->faceGroup1 != NULL) {
            drawCheckMdl(object->faceGroup1, matrix);
        }
        if (object->edgeGroup1 != NULL) {
            drawHitMdl(object->edgeGroup1, matrix);
        }
        if (object->faceGroup2 != NULL) {
            drawThruMdl(object->faceGroup2, matrix);
        }
    }

    return GSgfxDLEnd();
}

void fn_8010D8D4(void)
{
    ColDrawScene* scene;
    u32 i;
    ColMtx matrix;
    ColDrawObject* object;
    ColMtx normalMatrix;
    GSColFloorObj* floorObj;

    scene = fn_8010CBC0();
    if (scene == NULL) {
        return;
    }

    gfxSetCullMode(1);
    gfxSetVtxDesc((void*)lbl_80404C68.gfxRenderHandle);
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetBlendMode(1, 6, 7);
    gfxSetZMode(1, 2, 1);
    fn_800D9ED8(0);

    object = scene->objects;
    floorObj = lbl_80404C68.floors[lbl_80404C68.activeLayer].objs;
    for (i = 0; i < scene->count; i++, object++, floorObj++) {
        if ((floorObj->flags & 1) != 0 || (object->flags & 1) == 0) {
            continue;
        }
        colsysMakeSRTMatrix(matrix, i);
        colsysMakeRotMatrix(normalMatrix, i);
        if (object->model != NULL) {
            colsysDrawWalkMdl(object->model, matrix, normalMatrix);
        }

        if (object->edgeGroup0 != NULL) {
            drawHitMdl(object->edgeGroup0, matrix);
        }
        if (object->faceGroup0 != NULL) {
            drawSunMdl(object->faceGroup0, matrix);
        }
        if (object->faceGroup1 != NULL) {
            drawCheckMdl(object->faceGroup1, matrix);
        }
        if (object->edgeGroup1 != NULL) {
            drawHitMdl(object->edgeGroup1, matrix);
        }
        if (object->faceGroup2 != NULL) {
            drawThruMdl(object->faceGroup2, matrix);
        }
    }

    if (lbl_80404C68.displayList == NULL) {
        lbl_80404C68.displayList = GScolsys2Draw();
    }
    if (lbl_80404C68.displayList != NULL) {
        GSgfxDLDraw(lbl_80404C68.displayList);
        fn_800D30AC();
    }
}

