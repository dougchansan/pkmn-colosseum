/**
 * @file hsd_fog.c
 * @brief HSD Fog and FogAdj implementation.
 *
 * Colosseum address: 0x8019B7C0 (HSD_FogAdjInit)
 * Adapted from doldecomp/melee src/sysdolphin/baselib/fog.c
 */

#include "hsd/hsd_fog.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_object.h"

extern void* memcpy(void* dst, const void* src, u32 size);

static void FogInfoInit(void);
static void FogAdjInfoInit(void);

static HSD_FogInfo hsdFog = { FogInfoInit };
static HSD_FogAdjInfo hsdFogAdj = { FogAdjInfoInit };

/* ========================================================================= */
/*  FogAdj                                                                   */
/* ========================================================================= */

void HSD_FogAdjInit(HSD_FogAdj* fogadj, HSD_FogAdjDesc* desc)
{
    if (fogadj == NULL || desc == NULL) {
        return;
    }
    fogadj->center = (s16) desc->center;
    fogadj->width = desc->width;
    memcpy(fogadj->mtx, desc->mtx, sizeof(f32) * 16);
}

HSD_FogAdj* HSD_FogAdjLoadDesc(HSD_FogAdjDesc* desc)
{
    HSD_FogAdj* fogadj;

    if (desc == NULL) {
        return NULL;
    }

    fogadj = HSD_FogAdjAlloc();
    HSD_FogAdjInit(fogadj, desc);
    return fogadj;
}

HSD_FogAdj* HSD_FogAdjAlloc(void)
{
    HSD_FogAdj* fogadj;
    fogadj = (HSD_FogAdj*) hsdNew(&hsdFogAdj.parent.parent);
    HSD_ASSERT(0, fogadj);
    return fogadj;
}

/* ========================================================================= */
/*  Fog                                                                      */
/* ========================================================================= */

void HSD_FogInit(HSD_Fog* fog, HSD_FogDesc* desc)
{
    if (fog == NULL || desc == NULL) {
        return;
    }
    fog->type = desc->type;
    fog->start = desc->start;
    fog->end = desc->end;
    fog->color = desc->color;
    fog->fog_adj = HSD_FogAdjLoadDesc(desc->fogadjdesc);
}

HSD_Fog* HSD_FogLoadDesc(HSD_FogDesc* desc)
{
    HSD_Fog* fog;

    if (desc == NULL) {
        return NULL;
    }

    fog = HSD_FogAlloc();
    HSD_FogInit(fog, desc);
    return fog;
}

HSD_Fog* HSD_FogAlloc(void)
{
    HSD_Fog* fog;
    fog = (HSD_Fog*) hsdNew(&hsdFog.parent.parent);
    HSD_ASSERT(0, fog);
    return fog;
}

void HSD_FogReqAnim(HSD_Fog* fog, f32 frame)
{
    if (fog != NULL) {
        HSD_AObjReqAnim(fog->aobj, frame);
    }
}

void HSD_FogInterpretAnim(HSD_Fog* fog)
{
    if (fog == NULL) {
        return;
    }
    /* Interpret AObj -> update fog parameters */
}

/* ========================================================================= */
/*  Class init                                                               */
/* ========================================================================= */

static void FogRelease(HSD_Class* o)
{
    HSD_Fog* fog = (HSD_Fog*) o;
    HSD_AObjRemove(fog->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdFog)->release(o);
}

static void FogInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdFog), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_fog",
                     sizeof(HSD_FogInfo), sizeof(HSD_Fog));
    HSD_CLASS_INFO(&hsdFog)->release = FogRelease;
}

static void FogAdjRelease(HSD_Class* o)
{
    HSD_FogAdj* fogadj = (HSD_FogAdj*) o;
    HSD_AObjRemove(fogadj->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdFogAdj)->release(o);
}

static void FogAdjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdFogAdj), HSD_CLASS_INFO(&hsdObj),
                     "sysdolphin_base_library", "hsd_fogadj",
                     sizeof(HSD_FogAdjInfo), sizeof(HSD_FogAdj));
    HSD_CLASS_INFO(&hsdFogAdj)->release = FogAdjRelease;
}

/* ===================================================================
 * Generated: 0 pattern-matched + 13 stubs
 * Range: 0x8019B7C0 - 0x8019C690
 * =================================================================== */

/* 0x8019B808 | 0x6C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_8019B874(HSD_Fog* o);   /* inferred signature (glm5): FogRelease virtual */
extern void fn_8019B948(HSD_Fog* fog, s32 type, f32* value);   /* inferred glm6: FogInterpretAnim vtable slot (0x3C) */
extern u8 lbl_8036C7E8[];
extern u8 lbl_8036CC00[];
extern u8 lbl_802747B8[];
extern u8 lbl_8047DA60;
void fn_8019B808(void) {
    hsdInitClassInfo((HSD_ClassInfo*) lbl_8036C7E8,
                     (HSD_ClassInfo*) lbl_8036CC00, (char*) lbl_802747B8,
                     (char*) &lbl_8047DA60, 0x40, 0x20);
    ((void**)lbl_8036C7E8)[0x30/4] = (void*)fn_8019B874;
    ((void**)lbl_8036C7E8)[0x3c/4] = (void*)fn_8019B948;
}

/* 0x8019B874 | 0xD4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_801C25E4(HSD_AObj* aobj);   /* inferred == HSD_AObjRemove */
#if 0
asm void fn_8019B874(void) {
#include "src/hsd/hsd_fog_fn_8019B874.inc"
}
#else
/* decompiled glm5: functional (TU not byte-measurable) */
/* FogRelease virtual (vtable 0x30). Signature inferred: arg0 is the HSD_Fog. */
void fn_8019B874(HSD_Fog* o)
{
    HSD_FogAdj* fog_adj = o->fog_adj;

    if (fog_adj != NULL) {
        if (ref_DEC(fog_adj)) {
            hsdDelete(fog_adj);
        }
    }
    fn_801C25E4(o->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdFog)->release((HSD_Class*)o);
}
#endif
#pragma pop

/* 0x8019B948 | 0x230 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void jumptable_8036C864();
extern u32 lbl_8047DA68;
extern u32 lbl_8047DA6C;
extern u32 lbl_8047DA70;
#if 0
asm void fn_8019B948(void) {
#include "src/hsd/hsd_fog_fn_8019B948.inc"
}
#else
/* decompiled glm6: functional (W-SDA-WRAPPER wall caps this TU <100% even for the
 * asm reference: 0x8019B948=91%, 0x8019BB78=95%, 0x8019BD18=96%; byte-exact is
 * unattainable, so faithful readable C is the target) */
/*
 * Fog AObj interpret callback. Writes one animated attribute (selected by
 * `type`, 0..0x15) into the fog object. Inferred signature:
 *   void fn_8019B948(HSD_Fog* fog, s32 type, f32* value)
 *   0 start | 1 end | 2..5 color R/G/B/A | 6 fog_adj.center | 7 fog_adj.width
 *   8..0x15 are no-ops (jumptable falls through to the return).
 *
 * NOTE: HSD_FogAdj in include/hsd/hsd_fog.h is out of sync with the binary.
 * The real layout (consistent across 0x8019B948/BB78/BD18) is
 *   flags@0x08  center@0x0C  width@0x0E  mtx[4][4]@0x10
 * whereas the header has center@0x08/width@0x0A/mtx@0x0C and no flags field.
 * FogAdj fields are therefore accessed by binary offset below so the code is
 * faithful; the header should be corrected separately.
 */
void fn_8019B948(HSD_Fog* fog, s32 type, f32* value)
{
    if (fog == NULL || (u32)type > 0x15) {
        return;
    }

    switch (type) {
    case 0: /* start */
        fog->start = *value;
        break;

    case 1: /* end */
        fog->end = *value;
        break;

    case 2: /* color R */
    case 3: /* color G */
    case 4: /* color B */
    case 5: { /* color A */
        f32 v = *value;
        if (v <= 0.0f) {
            v = 0.0f;
        } else if (v >= 1.0f) {
            v = 1.0f;
        }
        ((u8*)&fog->color)[type - 2] = (u8)(255.0f * v);
        break;
    }

    case 6: { /* fog_adj.center (clamped to [-320, 320]) */
        s32 v = (s32)*value;
        HSD_FogAdj* adj = fog->fog_adj;
        if (adj != NULL) {
            if (v < -0x140) {
                v = -0x140;
            } else if (v > 0x140) {
                v = 0x140;
            }
            *(s16*)((u8*)adj + 0x0C) = (s16)v;
        }
        break;
    }

    case 7: { /* fog_adj.width (clamped to [0, 640]) */
        s32 v = (s32)*value;
        HSD_FogAdj* adj = fog->fog_adj;
        if (adj != NULL) {
            if (v < 0) {
                v = 0;
            } else if (v > 0x280) {
                v = 0x280;
            }
            *(s16*)((u8*)adj + 0x0E) = (s16)v;
        }
        break;
    }

    default:
        break;
    }
}
#endif
#pragma pop

/* 0x8019BB78 | 0x1A0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void* fn_80193828(HSD_ClassInfo* info);   /* inferred glm6: == hsdNew */
extern void fn_800BD768(void* dst);              /* inferred glm6: writes viewport/projection scratch */
extern void* memset(void* dst, int val, u32 n);
extern char lbl_8047DA74;   /* "fog.c" */
extern u8 lbl_8047DA80[4];   /* "fog"  */
extern u8 lbl_8047DA7C[4];   /* "adj"  */
#if 0
asm void fn_8019BB78(void) {
#include "src/hsd/hsd_fog_fn_8019BB78.inc"
}
#else
/* decompiled glm6: functional (W-SDA-WRAPPER wall caps this TU <100% even for the
 * asm reference: 0x8019B948=91%, 0x8019BB78=95%, 0x8019BD18=96%; byte-exact is
 * unattainable, so faithful readable C is the target) */
/*
 * Allocate and initialise an HSD_Fog (plus its HSD_FogAdj) from a descriptor.
 * Inferred signature: HSD_Fog* fn_8019BB78(HSD_FogDesc* desc).
 *
 * This is the monolithic binary form of what the reference code above splits
 * into HSD_FogAlloc + HSD_FogInit (+ FogAdj alloc/init): it hsdNew()s the fog,
 * copies type/start/end/color from the desc (or zero-inits defaults when the
 * desc is NULL), then hsdNew()s a FogAdj from desc->fogadjdesc and links it.
 *
 * HSD_FogAdj binary layout differs from the header (see fn_8019B948 note);
 * FogAdj fields use binary offsets. HSD_FogAdjDesc binary layout is likewise
 *   flags@0x00  center@0x04  width@0x06  mtx[4][4]@0x08
 * (header has center@0x00/width@0x02/mtx@0x04, no flags).
 */
HSD_Fog* fn_8019BB78(HSD_FogDesc* desc)
{
    HSD_Fog* fog;
    HSD_FogAdj* adj;

    fog = (HSD_Fog*)fn_80193828((HSD_ClassInfo*)&hsdFog);
    if (fog == NULL) {
        __assert((char*)&lbl_8047DA74, 0xa1, (char*)lbl_8047DA80);
    }
    if (fog == NULL) {
        __assert((char*)&lbl_8047DA74, 0xae, (char*)lbl_8047DA80);
    }

    if (fog != NULL) {
        if (desc != NULL) {
            fog->type = desc->type;
            fog->start = desc->start;
            fog->end = desc->end;
            fog->color = desc->color;
        } else {
            u8 cobj[0x18];
            fn_800BD768(cobj);
            fog->type = 2;
            fog->start = *(f32*)(cobj + 0x10);
            fog->end = *(f32*)(cobj + 0x14);
            ((u8*)&fog->color)[0] = 0xff;
            ((u8*)&fog->color)[1] = 0xff;
            ((u8*)&fog->color)[2] = 0xff;
            ((u8*)&fog->color)[3] = 0xff;
        }
    }

    if (desc != NULL && desc->fogadjdesc != NULL) {
        HSD_FogAdjDesc* fadesc = desc->fogadjdesc;
        adj = (HSD_FogAdj*)fn_80193828((HSD_ClassInfo*)&hsdFogAdj);
        if (adj == NULL) {
            __assert((char*)&lbl_8047DA74, 0xf9, (char*)lbl_8047DA7C);
        }
        if (adj == NULL) {
            __assert((char*)&lbl_8047DA74, 0x109, (char*)lbl_8047DA7C);
        }
        if (adj != NULL) {
            *(u32*)((u8*)adj + 0x08) = *(u32*)((u8*)fadesc + 0x00); /* flags  */
            *(s16*)((u8*)adj + 0x0C) = *(s16*)((u8*)fadesc + 0x04); /* center */
            *(s16*)((u8*)adj + 0x0E) = *(s16*)((u8*)fadesc + 0x06); /* width  */
            memcpy((u8*)adj + 0x10, (u8*)fadesc + 0x08, 0x40);      /* mtx4x4 */
        }
        fog->fog_adj = adj;
    }

    return fog;
}
#endif
#pragma pop

/* 0x8019BD18 | 0x2D0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_800BC8F8(u32 type, void* color, f32 start, f32 end, f32 znear, f32 zfar); /* inferred glm6: GXSetFog */
extern HSD_CObj* HSD_CObjGetCurrent(void);
extern void HSD_Panic(char* file, u32 line, char* msg);
extern f32 HSD_CObjGetFar(HSD_CObj* cobj);
extern f32 HSD_CObjGetNear(HSD_CObj* cobj);
extern void fn_800BD454(void* dst);                /* inferred glm6: writes 7-float projection desc */
extern void fn_800BCB14(void* table, u32 width, void* projmtx); /* inferred glm6: GXInitFogAdjTable */
extern void fn_800BCCDC(u32 enable, u32 center, void* table);   /* inferred glm6: GXSetFogRangeAdj */
extern u32 lbl_8047E720;   /* default fog colour (read-only) */
extern u8 lbl_802747DC[];  /* HSD_Panic message */
#if 0
asm void fn_8019BD18(void) {
#include "src/hsd/hsd_fog_fn_8019BD18.inc"
}
#else
/* decompiled glm6: functional (W-SDA-WRAPPER wall caps this TU <100% even for the
 * asm reference: 0x8019B948=91%, 0x8019BB78=95%, 0x8019BD18=96%; byte-exact is
 * unattainable, so faithful readable C is the target) */
/*
 * Apply a fog object to the GPU: GXSetFog from the fog params, then optionally
 * configure fog-range adjustment (GXInitFogAdjTable + GXSetFogRangeAdj) from the
 * fog's HSD_FogAdj. Inferred signature: void fn_8019BD18(HSD_Fog* fog).
 *
 * fn_800BD768 fills viewport/projection scratch (reads [0x00] and [0x08]).
 * HSD_FogAdj flags @0x08 select behaviour (master gate = any of bits 29..31):
 *   bit31 -> centre derived from fogadj->center, else centre = proj[0]+proj[8]/2
 *   bit30 -> width  from fogadj->width,  else width  = (s32)proj[8]
 *   bit29 -> matrix from fogadj->mtx,    else matrix rebuilt from fn_800BD454
 * FogAdj binary layout differs from the header (see fn_8019B948 note), so its
 * fields are accessed by binary offset below.
 */
void fn_8019BD18(HSD_Fog* fog)
{
    HSD_CObj* cobj;
    u32 color;
    f32 znear;
    f32 zfar;

    if (fog == NULL) {
        color = lbl_8047E720;
        fn_800BC8F8(0, &color, 0.0f, 0.0f, 0.0f, 0.0f);
        return;
    }

    cobj = HSD_CObjGetCurrent();
    if (cobj == NULL) {
        HSD_Panic((char*)&lbl_8047DA74, 0x58, (char*)lbl_802747DC);
    }

    color = fog->color;
    zfar = HSD_CObjGetFar(cobj);
    znear = HSD_CObjGetNear(cobj);
    fn_800BC8F8(fog->type, &color, fog->start, fog->end, znear, zfar);

    {
        HSD_FogAdj* adj = fog->fog_adj;
        u32 flags = (adj != NULL) ? *(u32*)((u8*)adj + 0x08) : 0;

        if (adj != NULL && (flags & 0xE0000000) != 0) {
            u8 proj[0x10];
            u8 local_mtx[0x40];
            u8 fogadj_table[0x14];
            f32 pp[7];
            f32 p0;
            f32 p8;
            s32 center_i;
            s32 width_i;
            void* mtx;
            f32* m;

            fn_800BD768(proj);
            p0 = *(f32*)(proj + 0x00);
            p8 = *(f32*)(proj + 0x08);

            if ((flags & 0x80000000) != 0) {
                /* centre from fogadj->center (+320, scaled by proj w / 640) */
                s32 c = (s32)*(s16*)((u8*)adj + 0x0C);
                center_i = (s32)(p0 + p8 * (c + 320) / 640.0f);
            } else {
                /* centre = proj[0] + proj[8]/2 */
                center_i = (s32)(p0 + p8 * 0.5f);
            }

            if ((flags & 0x40000000) != 0) {
                width_i = (s32)*(u16*)((u8*)adj + 0x0E);
            } else {
                width_i = (s32)p8;
            }

            if ((flags & 0x20000000) != 0) {
                mtx = (void*)((u8*)adj + 0x10);
            } else {
                memset(local_mtx, 0, 0x40);
                fn_800BD454(pp);
                m = (f32*)local_mtx;
                if ((s32)pp[0] == 0) {
                    m[0]  = pp[1];
                    m[2]  = pp[2];
                    m[5]  = pp[3];
                    m[6]  = pp[4];
                    m[10] = pp[5];
                    m[11] = pp[6];
                    m[14] = -1.0f;
                } else {
                    m[0]  = pp[1];
                    m[3]  = pp[2];
                    m[5]  = pp[3];
                    m[7]  = pp[4];
                    m[10] = pp[5];
                    m[11] = pp[6];
                    m[15] = 1.0f;
                }
                mtx = local_mtx;
            }

            fn_800BCB14(fogadj_table, (u32)(u16)width_i, mtx);
            fn_800BCCDC(1, (u32)(u16)center_i, fogadj_table);
        } else {
            fn_800BCCDC(0, 0, 0);
        }
    }
}
#endif
#pragma pop

/* 0x8019BFE8 | 0x110 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u8 lbl_80274800[];
extern char lbl_8047DA98;
#if 0
asm void HSD_HashSearch(void) {
#include "src/hsd/hsd_fog_HSD_HashSearch.inc"
}
#else
/* decompiled glm5: functional (TU not byte-measurable) */
/*
 * Generic class-based hash-table lookup. No typed header struct exists; layout
 * inferred from the .inc (same raw-access style as HSD_IDGetDataFromTable):
 *   table @0x00 HSD_ClassInfo* class_info ; @0x04 void** buckets ; @0x08 u32 nb
 *   node  @0x00 node* next               ; @0x04 void* key       ; @0x08 void* value
 *   class_info virtuals: hash @0x3C (idx = hash(table,key)) ;
 *                        cmp  @0x40 (cmp(table,a,b) == 0 means equal)
 * Signature inferred (glm5): returns the matched node's value (NULL if absent);
 * *found = 1 on hit / 0 on miss (only written when found != NULL).
 */
typedef u32 (*HashKeyFn)(void* table, void* key);
typedef u32 (*HashCmpFn)(void* table, void* a, void* b);

void* HSD_HashSearch(void* table, void* key, u32* found)
{
    HSD_ClassInfo* info = *(HSD_ClassInfo**)table;
    u32 idx = ((HashKeyFn) *(void**) ((u8*)info + 0x3C))(table, key);
    u8* node;

    if (idx >= *(u32*)((u8*)table + 0x08)) {
        __assert((const char*) &lbl_8047DA98, 0x71, (const char*) lbl_80274800);
    }

    /* walk the collision chain for buckets[idx] */
    node = (*(u8***) ((u8*)table + 0x04))[idx];
    while (node != NULL) {
        if (((HashCmpFn) *(void**) ((u8*)info + 0x40))
                (table, *(void**) (node + 0x04), key) == 0) {
            break;   /* equal key -> found */
        }
        node = *(u8**) node;   /* next */
    }

    if (found != NULL) {
        *found = (node != NULL);
    }
    return (node != NULL) ? *(void**) (node + 0x08) : NULL;
}
#endif
#pragma pop

/* 0x8019C0F8 | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern u8 lbl_804653A8[];
#if 0
asm void _HSD_IDForgetMemory(void) {
#include "src/hsd/hsd_fog__HSD_IDForgetMemory.inc"
}
#else
void _HSD_IDForgetMemory(void) {
    memset(lbl_804653A8, 0, 0x194);
}
#endif
#pragma pop

/* 0x8019C128 | 0x88 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void HSD_IDGetDataFromTable(void) {
#include "src/hsd/hsd_fog_HSD_IDGetDataFromTable.inc"
}
#else
#pragma optimization_level 4
void* HSD_IDGetDataFromTable(u32* table, u32 key, u32* found) {
    u32** bucket;
    u32** node;

    if (table == NULL) {
        table = (u32*)lbl_804653A8;
    }
    bucket = (u32**)(table + key % 101);
    node = (u32**)*bucket;
    while (node != NULL) {
        if ((u32)node[1] == key) {
            if (found != NULL) {
                *found = 1;
            }
            return node[2];
        }
        node = (u32**)node[0];
    }
    if (found != NULL) {
        *found = 0;
    }
    return NULL;
}
#endif
#pragma pop

/* 0x8019C1B0 | 0xB4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void HSD_ObjFree(void* list, void* data);
extern u8 lbl_8046553C[];
#if 1
asm void fn_8019C1B0(void) {
#include "src/hsd/hsd_fog_fn_8019C1B0.inc"
}
#else
#pragma optimization_level 4
void fn_8019C1B0(u32* table, u32 key) {
    u32** bucket;
    u32** node;
    u32** prev;
    u32 hash;

    if (table == NULL) {
        table = (u32*)lbl_804653A8;
    }
    hash = key % 101;
    bucket = (u32**)(table + hash);
    node = (u32**)*bucket;
    prev = NULL;
    while (node != NULL) {
        if ((u32)node[1] == key) {
            if (prev != NULL) {
                prev[0] = (u32*)node[0];
            } else {
                *bucket = (u32*)node[0];
            }
            HSD_ObjFree(lbl_8046553C, node);
            return;
        }
        prev = node;
        node = (u32**)node[0];
    }
}
#endif
#pragma pop

/* 0x8019C264 | 0xF4 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void* HSD_ObjAlloc(void* list);
extern char lbl_8047DAA0;
extern char lbl_8047DAA8;
#if 1
asm void HSD_IDInsertToTable(void) {
#include "src/hsd/hsd_fog_HSD_IDInsertToTable.inc"
}
#else
#pragma optimization_level 4
void HSD_IDInsertToTable(u32* table, u32 key, u32 value) {
    u32** node;
    u32** newnode;
    u32 hash;

    if (table == NULL) {
        table = (u32*)lbl_804653A8;
    }
    hash = key % 101;
    node = (u32**)*((u32**)(table + hash));
    while (node != NULL) {
        if ((u32)node[1] == key) {
            break;
        }
        node = (u32**)node[0];
    }
    if (node != NULL) {
        node[1] = (u32*)key;
        node[2] = (u32*)value;
    } else {
        newnode = (u32**)HSD_ObjAlloc(lbl_8046553C);
        if (newnode == NULL) {
            __assert((char*)lbl_8047DAA0, 0x43, (char*)lbl_8047DAA8);
        }
        memset(newnode, 0, 0xc);
        newnode[1] = (u32*)key;
        newnode[2] = (u32*)value;
        newnode[0] = (u32*)*((u32**)(table + hash));
        *((u32**)(table + hash)) = (u32*)newnode;
    }
}
#endif
#pragma pop

/* 0x8019C358 | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_IDSetup(void) {
#include "src/hsd/hsd_fog_HSD_IDSetup.inc"
}
#else
void HSD_IDSetup(void) {
    memset(lbl_804653A8, 0, 0x194);
}
#endif
#pragma pop

/* 0x8019C388 | 0x30 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
extern void fn_801AA35C(void* list, u32 size, u32 alignment);
#if 0
asm void HSD_IDInitAllocData(void) {
#include "src/hsd/hsd_fog_HSD_IDInitAllocData.inc"
}
#else
void HSD_IDInitAllocData(void) {
    fn_801AA35C(lbl_8046553C, 0xC, 4);
}
#endif
#pragma pop

/* 0x8019C3B8 | 0xC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 0
asm void HSD_IDGetAllocData(void) {
#include "src/hsd/hsd_fog_HSD_IDGetAllocData.inc"
}
#else
void* HSD_IDGetAllocData(void) {
    return lbl_8046553C;
}
#endif
#pragma pop
