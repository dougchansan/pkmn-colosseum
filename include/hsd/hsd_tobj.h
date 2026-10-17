/**
 * @file hsd_tobj.h
 * @brief HSD TObj - Texture objects.
 *
 * TObj manages texture state including image data, texture coordinates,
 * filtering, wrapping, LOD, TEV color/alpha combine settings, and
 * texture animation.
 *
 * Colosseum address range: 0x801BBAC8 - 0x801BF098
 * Adapted from the Melee decompilation (doldecomp/melee).
 *
 * Layout notes (verified against the target assembly):
 *  - sizeof(HSD_TObj)     == 0xAC
 *  - sizeof(HSD_TObjInfo) == 0x4C  (Melee's + a fifth `update` method)
 *  - Colosseum keeps the descriptor's texture id in its own u16 field
 *    (`anim_id`, 0x4A) because `id` (0x0C) is overwritten with the
 *    hardware GXTexMapID by HSD_TObjAssignResources.  Melee reuses `id`
 *    for both roles.
 */
#ifndef HSD_TOBJ_H
#define HSD_TOBJ_H

#include "dolphin/mtx.h"
#include "dolphin/gx/GX.h"
#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_object.h"

/* ========================================================================= */
/*  GX constants used by TObj                                                */
/* ========================================================================= */

#define GX_TEXMAP0      0
#define GX_TEXMAP7      7
#define GX_TEXMAP_NULL  0xFF

/* "no texture id" sentinel stored in HSD_TObj::anim_id */
#define TOBJ_ID_NULL    0xFFFF

#define GX_TEXCOORD0    0
#define GX_TEXCOORD7    7

#define GX_TEXMTX0      30
#define GX_TEXMTX9      57
#define GX_IDENTITY     60

#define GX_PTTEXMTX0    64
#define GX_PTTEXMTX7    85

#define GX_MTX3x4       0
#define GX_MTX2x4       1

#define GX_TG_MTX3x4    0
#define GX_TG_MTX2x4    1
#define GX_TG_BUMP0     2
#define GX_TG_SRTG      10

#define GX_TG_POS       0
#define GX_TG_NRM       1
#define GX_TG_TEXCOORD0 12

#define GX_MIRROR       2

#define GX_TLUT0        0
#define GX_BIGTLUT0     16

#define GX_NEAR          0
#define GX_LINEAR        1
#define GX_NEAR_MIP_NEAR 2
#define GX_LIN_MIP_NEAR  3
#define GX_NEAR_MIP_LIN  4
#define GX_LIN_MIP_LIN   5

#define GX_TF_I4     0
#define GX_TF_I8     1
#define GX_TF_IA4    2
#define GX_TF_IA8    3
#define GX_TF_RGB565 4
#define GX_TF_RGB5A3 5
#define GX_TF_RGBA8  6
#define GX_TF_C4     8
#define GX_TF_C8     9
#define GX_TF_C14X2  10
#define GX_TF_CMPR   14

#define GX_FALSE   0
#define GX_TRUE    1
#define GX_DISABLE 0
#define GX_ENABLE  1

#define GX_ANISO_1 0

#define GX_COLOR_NULL 0xFF
#define GX_COLOR0A0   4

#define GX_TEV_ADD    0
#define GX_TEV_SUB    1
#define GX_TB_ZERO    0
#define GX_CS_SCALE_1 0

#define GX_CC_CPREV 0
#define GX_CC_APREV 1
#define GX_CC_TEXC  8
#define GX_CC_TEXA  9
#define GX_CC_RASC  10
#define GX_CC_ONE   12
#define GX_CC_HALF  13
#define GX_CC_ZERO  15

#define GX_CA_APREV 0
#define GX_CA_TEXA  4
#define GX_CA_ZERO  7

/* GX texture / tlut objects (opaque, sized) */
typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef struct GXTlutObj {
    u32 dummy[3];
} GXTlutObj;

/* ========================================================================= */
/*  Texture animation attribute indices                                      */
/* ========================================================================= */

#define HSD_A_T_TIMG     1
#define HSD_A_T_TRAU     2
#define HSD_A_T_TRAV     3
#define HSD_A_T_SCAU     4
#define HSD_A_T_SCAV     5
#define HSD_A_T_ROTX     6
#define HSD_A_T_ROTY     7
#define HSD_A_T_ROTZ     8
#define HSD_A_T_BLEND    9
#define HSD_A_T_TCLT     10
#define HSD_A_T_LOD_BIAS 11
#define HSD_A_T_KONST_R  12
#define HSD_A_T_KONST_G  13
#define HSD_A_T_KONST_B  14
#define HSD_A_T_KONST_A  15
#define HSD_A_T_TEV0_R   16
#define HSD_A_T_TEV0_G   17
#define HSD_A_T_TEV0_B   18
#define HSD_A_T_TEV0_A   19
#define HSD_A_T_TEV1_R   20
#define HSD_A_T_TEV1_G   21
#define HSD_A_T_TEV1_B   22
#define HSD_A_T_TEV1_A   23
#define HSD_A_T_TS_BLEND 24

#define TOBJ_ANIM (1 << 4)

/* ========================================================================= */
/*  TObj TEV input selectors                                                 */
/* ========================================================================= */

#define TOBJ_TEV_CC_KONST_RGB (0x01 << 7 | 0)
#define TOBJ_TEV_CC_KONST_RRR (0x01 << 7 | 1)
#define TOBJ_TEV_CC_KONST_GGG (0x01 << 7 | 2)
#define TOBJ_TEV_CC_KONST_BBB (0x01 << 7 | 3)
#define TOBJ_TEV_CC_KONST_AAA (0x01 << 7 | 4)
#define TOBJ_TEV_CC_TEX0_RGB  (0x01 << 7 | 5)
#define TOBJ_TEV_CC_TEX0_AAA  (0x01 << 7 | 6)
#define TOBJ_TEV_CC_TEX1_RGB  (0x01 << 7 | 7)
#define TOBJ_TEV_CC_TEX1_AAA  (0x01 << 7 | 8)

#define TOBJ_TEV_CA_KONST_R (0x01 << 6 | 0)
#define TOBJ_TEV_CA_KONST_G (0x01 << 6 | 1)
#define TOBJ_TEV_CA_KONST_B (0x01 << 6 | 2)
#define TOBJ_TEV_CA_KONST_A (0x01 << 6 | 3)
#define TOBJ_TEV_CA_TEX0_A  (0x01 << 6 | 4)
#define TOBJ_TEV_CA_TEX1_A  (0x01 << 6 | 5)

#define TOBJ_TEVREG_ACTIVE_COLOR_TEV (0x01 << 30)
#define TOBJ_TEVREG_ACTIVE_ALPHA_TEV (0x01U << 31)

/* ========================================================================= */
/*  Texture coordinate source types                                          */
/* ========================================================================= */

#define TEX_COORD_UV         0
#define TEX_COORD_REFLECTION 1
#define TEX_COORD_HILIGHT    2
#define TEX_COORD_SHADOW     3
#define TEX_COORD_TOON       4
#define TEX_COORD_GRADATION  5
#define TEX_COORD_BACKLIGHT  6
#define TEX_COORD_MASK       (0x0f)
#define tobj_coord(T) ((T)->flags & TEX_COORD_MASK)

/* ========================================================================= */
/*  Texture color mapping modes                                              */
/* ========================================================================= */

#define TEX_COLORMAP_NONE       (0 << 16)
#define TEX_COLORMAP_ALPHA_MASK (1 << 16)
#define TEX_COLORMAP_RGB_MASK   (2 << 16)
#define TEX_COLORMAP_BLEND      (3 << 16)
#define TEX_COLORMAP_MODULATE   (4 << 16)
#define TEX_COLORMAP_REPLACE    (5 << 16)
#define TEX_COLORMAP_PASS       (6 << 16)
#define TEX_COLORMAP_ADD        (7 << 16)
#define TEX_COLORMAP_SUB        (8 << 16)
#define TEX_COLORMAP_MASK       (0x0f << 16)
#define tobj_colormap(T) ((T)->flags & TEX_COLORMAP_MASK)

/* ========================================================================= */
/*  Texture alpha mapping modes                                              */
/* ========================================================================= */

#define TEX_ALPHAMAP_NONE       (0 << 20)
#define TEX_ALPHAMAP_ALPHA_MASK (1 << 20)
#define TEX_ALPHAMAP_BLEND      (2 << 20)
#define TEX_ALPHAMAP_MODULATE   (3 << 20)
#define TEX_ALPHAMAP_REPLACE    (4 << 20)
#define TEX_ALPHAMAP_PASS       (5 << 20)
#define TEX_ALPHAMAP_ADD        (6 << 20)
#define TEX_ALPHAMAP_SUB        (7 << 20)
#define TEX_ALPHAMAP_MASK       (0x0f << 20)
#define tobj_alphamap(T) ((T)->flags & TEX_ALPHAMAP_MASK)

/* ========================================================================= */
/*  Texture light map flags                                                  */
/* ========================================================================= */

#define TEX_LIGHTMAP_DIFFUSE  (0x1 << 4)
#define TEX_LIGHTMAP_SPECULAR (0x1 << 5)
#define TEX_LIGHTMAP_AMBIENT  (0x1 << 6)
#define TEX_LIGHTMAP_EXT      (0x1 << 7)
#define TEX_LIGHTMAP_SHADOW   (0x1 << 8)
#define TEX_LIGHTMAP_MASK \
    (TEX_LIGHTMAP_DIFFUSE | TEX_LIGHTMAP_SPECULAR | TEX_LIGHTMAP_AMBIENT | \
     TEX_LIGHTMAP_EXT | TEX_LIGHTMAP_SHADOW)
#define tobj_lightmap(T) ((T)->flags & TEX_LIGHTMAP_MASK)

#define TEX_BUMP      (0x1 << 24)
#define tobj_bump(T)  ((T)->flags & TEX_BUMP)
#define TEX_MTX_DIRTY (1U << 31)

/* ========================================================================= */
/*  Quaternion                                                               */
/* ========================================================================= */

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

/* ========================================================================= */
/*  Texture LUT                                                              */
/* ========================================================================= */

typedef struct _HSD_Tlut {
    void* lut;     /* 0x00 */
    u32 fmt;       /* 0x04  GXTlutFmt */
    u32 tlut_name; /* 0x08 */
    u16 n_entries; /* 0x0C */
} HSD_Tlut;        /* 0x10 */

typedef struct _HSD_TlutDesc {
    void* lut;
    u32 fmt;
    u32 tlut_name;
    u16 n_entries;
} HSD_TlutDesc;

/* ========================================================================= */
/*  Texture LOD descriptor                                                   */
/* ========================================================================= */

typedef struct _HSD_TexLODDesc {
    u32 minFilt;        /* 0x00  GXTexFilter */
    f32 LODBias;        /* 0x04 */
    u8 bias_clamp;      /* 0x08 */
    u8 edgeLODEnable;   /* 0x09 */
    u32 max_anisotropy; /* 0x0C  GXAnisotropy */
} HSD_TexLODDesc;       /* 0x10 */

/* ========================================================================= */
/*  Per-TObj TEV stage                                                       */
/* ========================================================================= */

typedef struct _HSD_TObjTev {
    u8 color_op;    /* 0x00 */
    u8 alpha_op;    /* 0x01 */
    u8 color_bias;  /* 0x02 */
    u8 alpha_bias;  /* 0x03 */
    u8 color_scale; /* 0x04 */
    u8 alpha_scale; /* 0x05 */
    u8 color_clamp; /* 0x06 */
    u8 alpha_clamp; /* 0x07 */
    u8 color_a;     /* 0x08 */
    u8 color_b;
    u8 color_c;
    u8 color_d;
    u8 alpha_a;   /* 0x0C */
    u8 alpha_b;
    u8 alpha_c;
    u8 alpha_d;
    GXColor konst; /* 0x10 */
    GXColor tev0;  /* 0x14 */
    GXColor tev1;  /* 0x18 */
    u32 active;    /* 0x1C */
} HSD_TObjTev;     /* 0x20 */

typedef HSD_TObjTev HSD_TObjTevDesc;

/* ========================================================================= */
/*  Image descriptor                                                         */
/* ========================================================================= */

struct HSD_ImageDesc {
    void* image_ptr; /* 0x00 */
    u16 width;       /* 0x04 */
    u16 height;      /* 0x06 */
    u32 format;      /* 0x08  GXTexFmt */
    u32 mipmap;      /* 0x0C */
    f32 minLOD;      /* 0x10 */
    f32 maxLOD;      /* 0x14 */
};                   /* 0x18 */

/* ========================================================================= */
/*  TObj structure                                                           */
/* ========================================================================= */

struct HSD_TObj {
    HSD_Obj parent;           /* 0x00 */
    HSD_TObj* next;           /* 0x08 */
    s32 id;                   /* 0x0C  GXTexMapID (assigned at setup) */
    s32 src;                  /* 0x10  GXTexGenSrc */
    s32 mtxid;                /* 0x14  GXTexMtx */
    Quaternion rotate;        /* 0x18 */
    Vec scale;                /* 0x28 */
    Vec translate;            /* 0x34 */
    s32 wrap_s;               /* 0x40  GXTexWrapMode */
    s32 wrap_t;               /* 0x44  GXTexWrapMode */
    u8 repeat_s;              /* 0x48 */
    u8 repeat_t;              /* 0x49 */
    u16 anim_id;              /* 0x4A  id as authored, for TexAnim lookup */
    u32 flags;                /* 0x4C */
    f32 blending;             /* 0x50 */
    s32 magFilt;              /* 0x54  GXTexFilter */
    HSD_ImageDesc* imagedesc; /* 0x58 */
    HSD_Tlut* tlut;           /* 0x5C */
    HSD_TexLODDesc* lod;      /* 0x60 */
    HSD_AObj* aobj;           /* 0x64 */
    HSD_ImageDesc** imagetbl; /* 0x68 */
    HSD_Tlut** tluttbl;       /* 0x6C */
    u8 tlut_no;               /* 0x70 */
    Mtx mtx;                  /* 0x74 */
    s32 coord;                /* 0xA4  GXTexCoordID */
    HSD_TObjTev* tev;         /* 0xA8 */
};                            /* 0xAC */

/* ========================================================================= */
/*  TObj descriptor (data format)                                            */
/* ========================================================================= */

typedef struct _HSD_TObjDesc {
    char* class_name;           /* 0x00 */
    struct _HSD_TObjDesc* next; /* 0x04 */
    u32 id;                     /* 0x08  GXTexMapID */
    u32 src;                    /* 0x0C  GXTexGenSrc */
    Vec rotate;                 /* 0x10 */
    Vec scale;                  /* 0x1C */
    Vec translate;              /* 0x28 */
    u32 wrap_s;                 /* 0x34 */
    u32 wrap_t;                 /* 0x38 */
    u8 repeat_s;                /* 0x3C */
    u8 repeat_t;                /* 0x3D */
    u32 blend_flags;            /* 0x40 */
    f32 blending;               /* 0x44 */
    u32 magFilt;                /* 0x48 */
    HSD_ImageDesc* imagedesc;   /* 0x4C */
    HSD_TlutDesc* tlutdesc;     /* 0x50 */
    HSD_TexLODDesc* lod;        /* 0x54 */
    HSD_TObjTevDesc* tev;       /* 0x58 */
} HSD_TObjDesc;                 /* 0x5C */

/* ========================================================================= */
/*  TObj class info                                                          */
/* ========================================================================= */

typedef struct _HSD_TObjInfo {
    HSD_ObjInfo parent;                              /* 0x00 */
    void (*make_mtx)(HSD_TObj* tobj);                /* 0x3C */
    int (*load)(HSD_TObj* tobj, HSD_TObjDesc* desc); /* 0x40 */
    void (*make_texp)(HSD_TObj* tobj, u32 lightmap, u32 lightmap_done,
                      HSD_TExp** c, HSD_TExp** a, HSD_TExp** list); /* 0x44 */
    void (*update)(void* obj, u32 type, HSD_ObjData* val);          /* 0x48 */
} HSD_TObjInfo;                                                     /* 0x4C */

/* ========================================================================= */
/*  Texture animation                                                        */
/* ========================================================================= */

typedef struct _HSD_TexAnim {
    struct _HSD_TexAnim* next; /* 0x00 */
    u32 id;                    /* 0x04  GXTexMapID */
    HSD_AObjDesc* aobjdesc;    /* 0x08 */
    HSD_ImageDesc** imagetbl;  /* 0x0C */
    HSD_TlutDesc** tluttbl;    /* 0x10 */
    u16 n_imagetbl;            /* 0x14 */
    u16 n_tluttbl;             /* 0x16 */
} HSD_TexAnim;                 /* 0x18 */

/* ========================================================================= */
/*  Globals and macros                                                       */
/* ========================================================================= */

#define HSD_TOBJ(o) ((HSD_TObj*) (o))
#define HSD_TOBJ_INFO(i) ((HSD_TObjInfo*) (i))
#define HSD_TOBJ_METHOD(o) HSD_TOBJ_INFO(HSD_OBJECT_METHOD(o))

/* ========================================================================= */
/*  Function declarations                                                    */
/* ========================================================================= */

/* These addresses are not yet conservatively renamed in symbols.txt. */
void HSD_TObjAnimAll(HSD_TObj* tobj);
void HSD_TObjReqAnimAllByFlags(HSD_TObj* tobj, f32 startframe, u32 flags);
void fn_801BEEDC(HSD_TObj* tobj, HSD_TexAnim* texanim);
void HSD_TObjSetupVolatileTev(HSD_TObj* tobj, u32 rendermode);
void HSD_TObjSetupTextureCoordGen(HSD_TObj* tobj);

#define HSD_TObjAnimAll HSD_TObjAnimAll
#define HSD_TObjReqAnimAllByFlags HSD_TObjReqAnimAllByFlags
#define HSD_TObjAddAnimAll fn_801BEEDC

HSD_TObj* HSD_TObjLoadDesc(HSD_TObjDesc* td);
HSD_TObj* _HSD_TObjGetCurrentByType(HSD_TObj* from, u32 mapping);
void HSD_TObjRemove(HSD_TObj* tobj);
void HSD_TObjRemoveAll(HSD_TObj* tobj);
HSD_TObj* HSD_TObjAlloc(void);
HSD_ImageDesc* HSD_ImageDescAlloc(void);
void HSD_ImageDescFree(HSD_ImageDesc* idesc);
void HSD_ImageDescRemove(HSD_ImageDesc* idesc);
u32 HSD_Index2TexMtx(u32 index);
s32 HSD_TObjAssignResources(HSD_TObj* tobj_top);
void HSD_TObjSetup(HSD_TObj* tobj);

#endif /* HSD_TOBJ_H */
