/**
 * @file hsd_object.c
 * @brief Model-bound transformation and visibility helpers.
 *
 * Colosseum address: 0x80190E34 - 0x801914F4.
 */

#include "dolphin/types.h"

/* Internal bounding box structure used by fn_80191358 and fn_80191460-8019147C */
typedef struct {
    u8 field0;     /* 0x00 */
    u8 init_flag;  /* 0x01: non-zero if uninitialized (reset to 0 on first expand) */
    u8 pad[2];     /* 0x02-0x03 */
    void* field4;  /* 0x04 */
    void* field8;  /* 0x08 */
    void* fieldC;  /* 0x0C */
    f32 min[3];    /* 0x10-0x18 */
    f32 max[3];    /* 0x1C-0x24 */
    f32 field28[3];/* 0x28-0x30 */
} HSD_BBox;

typedef struct {
    u8 unk_00[4];
    f32* scale;       /* 0x04 */
    f32* rotate;      /* 0x08 */
    f32* translate;   /* 0x0C */
    f32 corner0[3];   /* 0x10 */
    f32 corner1[3];   /* 0x1C */
} HSD_ObjectTransformData;

extern void GSvecCopy(void* dst, void* src);

/* The inherited symbol name is incorrect: callsites pass a model bound and
 * receive its vector at offset 0x28. The actual HSD ObjInfoInit is
 * ObjInfoInit_801AA568, referenced by the class-info data at lbl_8036CC00. */
void ObjInfoInit(HSD_BBox* bound, f32* out) {
    GSvecCopy(out, bound->field28);
}

/* 0x80190E60 | 0x2B8 */
extern void GSmtxMakeXRotation(void* dst, f32 angle);
extern void GSmtxMakeYRotation(void* dst, f32 angle);
extern void GSmtxMakeZRotation(void* dst, f32 angle);
extern void fn_800E048C(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E0560(void* dst, f32* src);
extern void GSmtxConcat(void* dst, void* lhs, void* rhs);
extern void GSvecTransform(void* dst, void* mtx, void* vec);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern void gfxSetFlags(s32 mode);
extern void gfxClearFlags(s32 mode);
extern void gfxSetCullMode(s32 mode);
extern void gfxSetBlendMode(s32 a, s32 b, s32 c);
extern void gfxSetVtxDesc(void* handle);
extern void gfxSetPrimitive(s32 prim_type);
extern void gfxBegin(s32 count);
extern void gfxVtxPos(f32 x, f32 y, f32 z);
extern void gfxVtxColor4u8(s32 idx, s32 r, s32 g, s32 b, s32 a);
extern void gfxEnd(void);
extern u8 lbl_80314638[];
void fn_80190E60(HSD_ObjectTransformData* data) {
    f32 corner0[3];
    f32 corner1[3];
    f32 mtx_scale[3][4];
    f32 mtx_translate[3][4];
    f32 mtx_z[3][4];
    f32 mtx_y[3][4];
    f32 mtx[3][4];

    GSmtxMakeXRotation(mtx, data->rotate[0]);
    GSmtxMakeYRotation(mtx_y, data->rotate[1]);
    GSmtxMakeZRotation(mtx_z, data->rotate[2]);
    fn_800E048C(mtx_translate, data->translate[0], data->translate[1],
                data->translate[2]);
    fn_800E0560(mtx_scale, data->scale);
    GSmtxConcat(mtx, mtx_y, mtx);
    GSmtxConcat(mtx, mtx_z, mtx);
    GSmtxConcat(mtx, mtx_translate, mtx);
    GSmtxConcat(mtx, mtx_scale, mtx);
    GSvecTransform(corner0, mtx, data->corner0);
    GSvecTransform(corner1, mtx, data->corner1);

    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetCullMode(0);
    gfxSetBlendMode(1, 6, 7);
    gfxSetVtxDesc(lbl_80314638);
    gfxSetPrimitive(4);
    gfxBegin(10);

    gfxVtxPos(corner0[0], corner0[1], corner0[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner0[0], corner1[1], corner0[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner1[0], corner0[1], corner0[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner1[0], corner1[1], corner0[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner1[0], corner0[1], corner1[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner1[0], corner1[1], corner1[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner0[0], corner0[1], corner1[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner0[0], corner1[1], corner1[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner0[0], corner0[1], corner0[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxVtxPos(corner0[0], corner1[1], corner0[2]);
    gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0x80);
    gfxEnd();
}

/* 0x80191118 | 0x240 */
#pragma push
extern s32 gfxWorldToScreen(void* pos, void* out);
extern const f32 lbl_8047D8B8;
extern const f32 lbl_8047D8BC;
extern const f32 lbl_8047D8C0;
/* Retail reloads the -100/740/580 bounds for every test, which is what MWCC
 * does for float literals owned by this TU's .sdata2. Written with literals,
 * this function matches without the pragma below apart from relocation names;
 * the constants currently live in game/data/sdata2_8047D8A8.c, so the extern
 * form still needs CSE disabled. */
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (opt_common_subs off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma opt_common_subs off
s32 fn_80191118(HSD_ObjectTransformData* data) {
    f32 corner0[3];
    f32 corner1[3];
    f32 screen0[3];
    f32 screen1[3];
    f32 mtx_scale[3][4];
    f32 mtx_translate[3][4];
    f32 mtx_z[3][4];
    f32 mtx_y[3][4];
    f32 mtx[3][4];
    f32 screen0x;
    f32 screen0y;
    f32 screen1x;
    f32 screen1y;
    s32 result0;
    s32 result1;

    GSmtxMakeXRotation(mtx, data->rotate[0]);
    GSmtxMakeYRotation(mtx_y, data->rotate[1]);
    GSmtxMakeZRotation(mtx_z, data->rotate[2]);
    fn_800E048C(mtx_translate, data->translate[0], data->translate[1],
                data->translate[2]);
    fn_800E0560(mtx_scale, data->scale);
    GSmtxConcat(mtx, mtx_y, mtx);
    GSmtxConcat(mtx, mtx_z, mtx);
    GSmtxConcat(mtx, mtx_translate, mtx);
    GSmtxConcat(mtx, mtx_scale, mtx);
    GSvecTransform(corner0, mtx, data->corner0);
    GSvecTransform(corner1, mtx, data->corner1);

    result0 = gfxWorldToScreen(corner0, screen0);
    result1 = gfxWorldToScreen(corner1, screen1);
    if (result0 == 0 || result1 == 0) {
        return 2;
    }
    if (result0 == 1 && result1 == 1) {
        return 0;
    }
    screen0x = screen0[0];
    if (screen0x < lbl_8047D8B8 && screen1[0] < lbl_8047D8B8) {
        return 0;
    }
    if (screen0x > lbl_8047D8BC && screen1[0] > lbl_8047D8BC) {
        return 0;
    }
    screen0y = screen0[1];
    if (screen0y < lbl_8047D8B8 && screen1[1] < lbl_8047D8B8) {
        return 0;
    }
    if (screen0y > lbl_8047D8C0 && screen1[1] > lbl_8047D8C0) {
        return 0;
    }
    if (result0 == 1 || result1 == 1) {
        return 1;
    }
    if (screen0x < lbl_8047D8B8 ||
        (screen1x = screen1[0]) < lbl_8047D8B8) {
        return 1;
    }
    if (screen0x > lbl_8047D8BC || screen1x > lbl_8047D8BC) {
        return 1;
    }
    if (screen0y < lbl_8047D8B8 ||
        (screen1y = screen1[1]) < lbl_8047D8B8) {
        return 1;
    }
    if (screen0y > lbl_8047D8C0 || screen1y > lbl_8047D8C0) {
        return 1;
    }
    return 2;
}
#pragma pop

/* 0x80191358 | 0x108 */
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void GSvecSub(void* dst, void* max, void* min);
void fn_80191358(HSD_BBox* bbox, f32 x, f32 y, f32 z) {
    if (bbox->init_flag != 0) {
        bbox->init_flag = 0;
        set__5GSvecFfff(bbox->min, x, y, z);
        set__5GSvecFfff(bbox->max, x, y, z);
    } else {
        if (x < bbox->min[0]) { bbox->min[0] = x; }
        if (y < bbox->min[1]) { bbox->min[1] = y; }
        if (z < bbox->min[2]) { bbox->min[2] = z; }
        if (x > bbox->max[0]) { bbox->max[0] = x; }
        if (y > bbox->max[1]) { bbox->max[1] = y; }
        if (z > bbox->max[2]) { bbox->max[2] = z; }
    }
    GSvecSub(bbox->field28, bbox->max, bbox->min);
}

/* 0x80191460 | 0xC */
void fn_80191460(HSD_BBox* bbox) {
    bbox->init_flag = 1;
}

/* 0x8019146C | 0x8 */
void fn_8019146C(HSD_BBox* bbox, void* val) {
    bbox->fieldC = val;
}

/* 0x80191474 | 0x8 */
void fn_80191474(HSD_BBox* bbox, void* val) {
    bbox->field8 = val;
}

/* 0x8019147C | 0x8 */
void fn_8019147C(HSD_BBox* bbox, void* val) {
    bbox->field4 = val;
}

/* 0x80191484 | 0x70 */
extern u32   _toolentryAlloc__FUl(u32 size);   /* GSmemAllocRaw (returns u16 handle) */
extern void* GSmemLock(u16 handle); /* GSmemGetPtr */
extern u16  lbl_8047B208; /* GSmem handle for object instance pool */
extern void* lbl_8047B20C; /* resolved pointer to object instance pool */
extern u32  lbl_8047B210; /* object instance count */
void fn_80191484(u32 count) {
    u32 handle;
    u32 i;

    lbl_8047B210 = count;
    handle = _toolentryAlloc__FUl(count * 0x34);
    lbl_8047B208 = handle;
    if ((u16)handle != 0) {
        lbl_8047B20C = GSmemLock((u16)handle);
        for (i = 0; i < lbl_8047B210; i++) {
            ((u8*)lbl_8047B20C)[i * 0x34] = 0;
        }
    }
}
