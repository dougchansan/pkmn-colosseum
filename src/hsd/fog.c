/**
 * @file fog.c
 * @brief HAL sysdolphin fog.c: distance fog and fog range adjustment,
 *        0x8019B7C0-0x8019BFE8.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/fog.c) and checked against Colosseum's retail
 * code, which is the newer sysdolphin: the fog class info carries the AObj
 * update callback, the FogAdj object has a flags word selecting which of
 * its center/width/matrix fields HSD_FogSet honours, and the animation
 * callback clamps the colour channels and the adjustment fields. The
 * library is built with deferred inlining, so functions are listed in
 * HAL's order and MWCC emits them in reverse (the retail address order).
 * Functions nothing in the game references are compiled and dead-stripped
 * by the linker as in retail.
 *
 * The Dolphin GX calls are still fn_ symbols in Colosseum's symbol map;
 * their SDK names are given where they are declared.
 */
#include "dolphin/gx/GX.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_fog.h"
#include "hsd/hsd_forward.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/object.h"

void* memcpy(void* dst, const void* src, u32 n);
void* memset(void* dst, int c, u32 n);

typedef struct _GXFogAdjTable {
    u16 r[10];
} GXFogAdjTable;

/* GXSetFog */
void GXSetFog(u32 type, f32 startz, f32 endz, f32 nearz, f32 farz,
                 GXColor color);
/* GXInitFogAdjTable */
void GXInitFogAdjTable(GXFogAdjTable* table, u16 width, const f32 projmtx[4][4]);
/* GXSetFogRangeAdj */
void GXSetFogRangeAdj(u8 enable, u16 center, GXFogAdjTable* table);
/* GXGetProjectionv */
void GXGetProjectionv(f32* p);
/* GXGetViewportv */
void GXGetViewportv(f32* vp);
/* hsdNew */
void* fn_80193828(HSD_ClassInfo* info);

#define GX_FOG_NONE 0
#define GX_FOG_LIN 2
#define GX_PERSPECTIVE 0

#define FOGADJ_CENTER (1 << 0)
#define FOGADJ_WIDTH (1 << 1)
#define FOGADJ_MTX (1 << 2)
#define FOGADJ_ALL (FOGADJ_CENTER | FOGADJ_WIDTH | FOGADJ_MTX)

static void FogInfoInit(void);
static void FogAdjInfoInit(void);
void FogUpdateFunc(void* obj, u32 type, HSD_ObjData* val);

HSD_FogInfo hsdFog = { FogInfoInit };
HSD_FogAdjInfo hsdFogAdj = { FogAdjInfoInit };

/* HAL's zero fog colour (HSD_Fog_804DE6F0 in Melee: `const GXColor ... =
 * { 0 }`). Retail keeps it in .sbss2, which only the 3.x compilers emit for
 * zero-initialised consts; every 2.4.x compiler (including the library's
 * GC/1.3.2) puts it in .sdata2 between the TU's constants. It stays
 * external, in dtk's .sbss2 region, so fog.c's own .sdata2 matches. */
extern const GXColor lbl_8047E720;
#define HSD_Fog_804DE6F0 lbl_8047E720

#define HSD_FogGetFogAdj(fog) ((fog) ? (fog)->fog_adj : NULL)
#define HSD_FogAdjGetFlags(adj) ((adj) ? (adj)->flags : 0)
#define HSD_FogAdjGetCenter(adj) ((adj) ? (adj)->center : -1)
#define HSD_FogAdjGetWidth(adj) ((adj) ? (adj)->width : -1)
#define HSD_FogAdjGetMtx(adj) ((adj) ? (adj)->mtx : NULL)

void HSD_FogSet(HSD_Fog* fog)
{
    GXFogAdjTable tbl;
    f32 v[6];
    HSD_CObj* cobj;
    HSD_FogAdj* adj;
    u32 flags;
    s32 range;
    s32 width;
    f32(*mtx)[4];
    struct {
        f32 x0;
        f32 v[6];
    } proj;
    f32 m[4][4];

    if (fog == NULL) {
        GXColor color = HSD_Fog_804DE6F0;
        GXSetFog(GX_FOG_NONE, 0.0F, 0.0F, 0.0F, 0.0F, color);
        return;
    }
    cobj = HSD_CObjGetCurrent();
    if (cobj == NULL) {
        HSD_Panic(__FILE__, 88, "You must specify CObj first.\n");
    }
    GXSetFog(fog->type, fog->start, fog->end, HSD_CObjGetNear(cobj),
                HSD_CObjGetFar(cobj), fog->color);

    adj = HSD_FogGetFogAdj(fog);
    flags = HSD_FogAdjGetFlags(adj);
    if (adj != NULL && (flags & FOGADJ_ALL)) {
        GXGetViewportv(v);
        if (flags & FOGADJ_CENTER) {
            range = v[0] + v[2] * (HSD_FogAdjGetCenter(adj) + 320) / 640.0F;
        } else {
            range = v[0] + v[2] / 2;
        }
        if (flags & FOGADJ_WIDTH) {
            width = HSD_FogAdjGetWidth(adj);
        } else {
            width = v[2];
        }
        if (flags & FOGADJ_MTX) {
            mtx = HSD_FogAdjGetMtx(adj);
        } else {
            mtx = m;
            memset(m, 0, sizeof(f32[4][4]));
            GXGetProjectionv((f32*) &proj);
            switch ((s32) proj.x0) {
            case GX_PERSPECTIVE:
                m[0][0] = proj.v[0];
                m[0][2] = proj.v[1];
                m[1][1] = proj.v[2];
                m[1][2] = proj.v[3];
                m[2][2] = proj.v[4];
                m[2][3] = proj.v[5];
                m[3][2] = -1.0F;
                break;
            default:
                m[0][0] = proj.v[0];
                m[0][3] = proj.v[1];
                m[1][1] = proj.v[2];
                m[1][3] = proj.v[3];
                m[2][2] = proj.v[4];
                m[2][3] = proj.v[5];
                m[3][3] = 1.0F;
                break;
            }
        }
        GXInitFogAdjTable(&tbl, width, mtx);
        GXSetFogRangeAdj(1, range, &tbl);
    } else {
        GXSetFogRangeAdj(0, 0, NULL);
    }
}

HSD_Fog* HSD_FogAlloc(void)
{
    HSD_Fog* fog = fn_80193828(HSD_CLASS_INFO(&hsdFog));
    HSD_ASSERT(161, fog);
    return fog;
}

void HSD_FogInit(HSD_Fog* fog, HSD_FogDesc* desc);
HSD_FogAdj* HSD_FogAdjLoadDesc(HSD_FogAdjDesc* desc);

HSD_Fog* HSD_FogLoadDesc(HSD_FogDesc* desc)
{
    HSD_Fog* fog = HSD_FogAlloc();
    HSD_ASSERT(174, fog);
    HSD_FogInit(fog, desc);
    if (desc->fogadjdesc != NULL) {
        fog->fog_adj = HSD_FogAdjLoadDesc(desc->fogadjdesc);
    }
    return fog;
}

void HSD_FogInit(HSD_Fog* fog, HSD_FogDesc* desc)
{
    if (fog != NULL) {
        if (desc != NULL) {
            fog->type = desc->type;
            fog->start = desc->start;
            fog->end = desc->end;
            fog->color = desc->color;
        } else {
            f32 v[6];
            GXGetViewportv(v);
            fog->type = GX_FOG_LIN;
            fog->start = v[4];
            fog->end = v[5];
            fog->color.r = 0xFF;
            fog->color.g = 0xFF;
            fog->color.b = 0xFF;
            fog->color.a = 0xFF;
        }
    }
}

HSD_FogAdj* HSD_FogAdjAlloc(void)
{
    HSD_FogAdj* adj = fn_80193828(HSD_CLASS_INFO(&hsdFogAdj));
    HSD_ASSERT(249, adj);
    return adj;
}

void HSD_FogAdjInit(HSD_FogAdj* adj, HSD_FogAdjDesc* desc);

HSD_FogAdj* HSD_FogAdjLoadDesc(HSD_FogAdjDesc* desc)
{
    HSD_FogAdj* adj = HSD_FogAdjAlloc();
    HSD_ASSERT(265, adj);
    HSD_FogAdjInit(adj, desc);
    return adj;
}

void HSD_FogAdjInit(HSD_FogAdj* adj, HSD_FogAdjDesc* desc)
{
    if (adj != NULL) {
        if (desc != NULL) {
            adj->flags = desc->flags;
            adj->width = desc->width;
            adj->center = desc->center;
            memcpy(adj->mtx, desc->mtx, sizeof(f32[4][4]));
        } else {
            adj->flags = 0;
            adj->width = 0;
            adj->center = 0;
            memset(adj->mtx, 0, sizeof(f32[4][4]));
        }
    }
}

void HSD_FogAddAnim(HSD_Fog* fog, HSD_AObjDesc* desc)
{
    if (fog != NULL) {
        if (fog->aobj != NULL) {
            HSD_AObjRemove(fog->aobj);
        }
        fog->aobj = HSD_AObjLoadDesc(desc);
    }
}

void HSD_FogReqAnimByFlags(HSD_Fog* fog, u32 flags, f32 frame);

void HSD_FogReqAnim(HSD_Fog* fog, f32 frame)
{
    HSD_FogReqAnimByFlags(fog, 0x7FF, frame);
}

void HSD_FogReqAnimByFlags(HSD_Fog* fog, u32 flags, f32 frame)
{
    if (fog == NULL || !(flags & 0x200)) {
        return;
    }
    HSD_AObjReqAnim(fog->aobj, frame);
}

void HSD_FogInterpretAnim(HSD_Fog* fog)
{
    if (fog != NULL) {
        HSD_AObjInterpretAnim(fog->aobj, fog, FogUpdateFunc);
    }
}

static inline void HSD_FogAdjSetCenter(HSD_FogAdj* adj, s32 center)
{
    if (adj != NULL) {
        if (center <= -320) {
            adj->center = -320;
        } else if (center >= 320) {
            adj->center = 320;
        } else {
            adj->center = center;
        }
    }
}

static inline void HSD_FogAdjSetWidth(HSD_FogAdj* adj, s32 width)
{
    if (adj != NULL) {
        if (width <= 0) {
            adj->width = 0;
        } else if (width >= 640) {
            adj->width = 640;
        } else {
            adj->width = width;
        }
    }
}

void FogUpdateFunc(void* obj, u32 type, HSD_ObjData* val)
{
    HSD_Fog* fog = obj;
    f32 fv;

    if (fog != NULL) {
        switch (type) {
        case 1:
            fog->start = val->fv;
            break;
        case 2:
            fog->end = val->fv;
            break;
        case 5:
            fv = val->fv;
            if (fv <= 0.0F) {
                fv = 0.0F;
            } else if (fv >= 1.0F) {
                fv = 1.0F;
            }
            fog->color.r = 255.0F * fv;
            break;
        case 6:
            fv = val->fv;
            if (fv <= 0.0F) {
                fv = 0.0F;
            } else if (fv >= 1.0F) {
                fv = 1.0F;
            }
            fog->color.g = 255.0F * fv;
            break;
        case 7:
            fv = val->fv;
            if (fv <= 0.0F) {
                fv = 0.0F;
            } else if (fv >= 1.0F) {
                fv = 1.0F;
            }
            fog->color.b = 255.0F * fv;
            break;
        case 8:
            fv = val->fv;
            if (fv <= 0.0F) {
                fv = 0.0F;
            } else if (fv >= 1.0F) {
                fv = 1.0F;
            }
            fog->color.a = 255.0F * fv;
            break;
        case 20:
            HSD_FogAdjSetCenter(HSD_FogGetFogAdj(fog), val->fv);
            break;
        case 21:
            HSD_FogAdjSetWidth(HSD_FogGetFogAdj(fog), val->fv);
            break;
        }
    }
}

static void FogRelease(HSD_Fog* fog)
{
    HSD_FogAdj* adj = fog->fog_adj;

    if (adj != NULL) {
        if (ref_DEC(adj)) {
            hsdDelete(adj);
        }
    }
    HSD_AObjRemove(fog->aobj);
    HSD_OBJECT_PARENT_INFO(&hsdFog)->release((HSD_Class*) fog);
}

static void FogInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdFog), &hsdObj,
                     "sysdolphin_base_library", "hsd_fog", sizeof(HSD_FogInfo),
                     sizeof(HSD_Fog));
    HSD_CLASS_INFO(&hsdFog)->release = (void*) FogRelease;
    hsdFog.update = FogUpdateFunc;
}

static void FogAdjInfoInit(void)
{
    hsdInitClassInfo(HSD_CLASS_INFO(&hsdFogAdj), &hsdObj,
                     "sysdolphin_base_library", "hsd_fogadj",
                     sizeof(HSD_FogAdjInfo), sizeof(HSD_FogAdj));
}
