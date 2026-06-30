/**
 * @file hsd_object.c
 * @brief HSD base object class initialization.
 *
 * Colosseum address: part of HSD library section (0x80190E34+)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/object.c
 */

#include "hsd/hsd_object.h"
#include "hsd/hsd_class.h"

HSD_ClassInfo hsdObj = { ObjInfoInit };
extern HSD_ClassInfo hsdClass;

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

void ObjInfoInit(void)
{
    hsdInitClassInfo(&hsdObj, &hsdClass, "sysdolphin_base_library", "hsd_obj",
                     sizeof(HSD_ObjInfo), sizeof(HSD_Obj));
}

/* 0x80190E60 | 0x2B8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_800E053C(void);
extern void fn_800E0518(void);
extern void fn_800E04F4(void);
extern void fn_800E048C(void);
extern void fn_800E0560(void);
extern void fn_800E0290(void);
extern void fn_800DFF98(void);
extern void fn_800D2248(void);
extern void fn_800D88DC(void);
extern void fn_800D888C(void);
extern void fn_800DA028(void);
extern void fn_800DA4C4(void);
extern void fn_800D7820(void);
extern void fn_800D6A00(void);
extern void fn_800D67BC(void);
extern void fn_800D6680(void);
extern void fn_800D5CB8(void);
extern void fn_800D6728(void);
extern u8 lbl_80314638[];
#if 1
asm void fn_80190E60(void) {
#include "src/hsd/hsd_object_fn_80190E60.inc"
}
#else
void fn_80190E60(void) { /* TODO */ }
#endif
#pragma pop

/* 0x80191118 | 0x240 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_800D2F34(void);
#if 1
asm void fn_80191118(void) {
#include "src/hsd/hsd_object_fn_80191118.inc"
}
#else
void fn_80191118(void) { /* TODO */ }
#endif
#pragma pop

/* 0x80191358 | 0x108 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_800E01F4(void* vec, f32 x, f32 y, f32 z);
extern void fn_800E0168(void* dst, void* max, void* min);
#if 0
asm void fn_80191358(void) {
#include "src/hsd/hsd_object_fn_80191358.inc"
}
#else
#pragma optimization_level 4
void fn_80191358(HSD_BBox* bbox, f32 x, f32 y, f32 z) {
    if (bbox->init_flag != 0) {
        bbox->init_flag = 0;
        fn_800E01F4(bbox->min, x, y, z);
        fn_800E01F4(bbox->max, x, y, z);
    } else {
        if (x < bbox->min[0]) { bbox->min[0] = x; }
        if (y < bbox->min[1]) { bbox->min[1] = y; }
        if (z < bbox->min[2]) { bbox->min[2] = z; }
        if (x > bbox->max[0]) { bbox->max[0] = x; }
        if (y > bbox->max[1]) { bbox->max[1] = y; }
        if (z > bbox->max[2]) { bbox->max[2] = z; }
    }
    fn_800E0168(bbox->field28, bbox->max, bbox->min);
}
#endif
#pragma pop

/* 0x80191460 | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80191460(void) {
#include "src/hsd/hsd_object_fn_80191460.inc"
}
#else
#pragma optimization_level 4
void fn_80191460(HSD_BBox* bbox) {
    bbox->init_flag = 1;
}
#endif
#pragma pop

/* 0x8019146C | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8019146C(void) {
#include "src/hsd/hsd_object_fn_8019146C.inc"
}
#else
#pragma optimization_level 4
void fn_8019146C(HSD_BBox* bbox, void* val) {
    bbox->fieldC = val;
}
#endif
#pragma pop

/* 0x80191474 | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_80191474(void) {
#include "src/hsd/hsd_object_fn_80191474.inc"
}
#else
#pragma optimization_level 4
void fn_80191474(HSD_BBox* bbox, void* val) {
    bbox->field8 = val;
}
#endif
#pragma pop

/* 0x8019147C | 0x8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void fn_8019147C(void) {
#include "src/hsd/hsd_object_fn_8019147C.inc"
}
#else
#pragma optimization_level 4
void fn_8019147C(HSD_BBox* bbox, void* val) {
    bbox->field4 = val;
}
#endif
#pragma pop

/* 0x80191484 | 0x70 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u32   fn_800E3534(u32 size);   /* GSmemAllocRaw (returns u16 handle) */
extern void* fn_800E27B0(u16 handle); /* GSmemGetPtr */
extern u16  lbl_8047B208; /* GSmem handle for object instance pool */
extern void* lbl_8047B20C; /* resolved pointer to object instance pool */
extern u32  lbl_8047B210; /* object instance count */
#if 0
asm void fn_80191484(void) {
#include "src/hsd/hsd_object_fn_80191484.inc"
}
#else
#pragma optimization_level 4
void fn_80191484(u32 count) {
    u32 handle;
    u32 i;

    lbl_8047B210 = count;
    handle = fn_800E3534(count * 0x34);
    lbl_8047B208 = handle;
    if ((u16)handle != 0) {
        lbl_8047B20C = fn_800E27B0((u16)handle);
        for (i = 0; i < lbl_8047B210; i++) {
            ((u8*)lbl_8047B20C)[i * 0x34] = 0;
        }
    }
}
#endif
#pragma pop
