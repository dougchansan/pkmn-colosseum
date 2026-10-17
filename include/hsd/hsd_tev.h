/**
 * @file hsd_tev.h
 * @brief HSD TEV/channel render state (tev.c).
 *
 * Colosseum address range: 0x801B3168 - 0x801B42C0.
 * Adapted from the Melee decompilation (doldecomp/melee, sysdolphin/baselib/
 * tev.h); HSD_Chan's layout (0x30 bytes) was checked against Colosseum's
 * channel code and prev_ch table.
 */
#ifndef HSD_TEV_H
#define HSD_TEV_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "hsd/hsd_objalloc.h"
#include "hsd/hsd_texp.h"

typedef struct _GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;

typedef int GXColorSrc;
typedef int GXDiffuseFn;
typedef int GXAttnFn;

#define GX_SRC_REG 0

#define GX_COLOR0 0
#define GX_COLOR1 1
#define GX_ALPHA0 2
#define GX_ALPHA1 3
#define GX_COLOR1A1 5

#define GX_DF_NONE 0
#define GX_AF_NONE 2

typedef struct HSD_Chan HSD_Chan;
struct HSD_Chan {
    HSD_Chan* next;
    GXChannelID chan;
    u32 flags;
    GXColor amb_color;
    GXColor mat_color;
    u8 enable;
    GXColorSrc amb_src;
    GXColorSrc mat_src;
    int light_mask; /* GXLightID */
    GXDiffuseFn diff_fn;
    GXAttnFn attn_fn;
    HSD_AObj* aobj;
};

/* Functions still carrying address names in symbols.txt:
 *   _HSD_StateInvalidateTexCoordGen _HSD_StateInvalidateTexCoordGen
 *   _HSD_StateInvalidateTevRegister _HSD_StateInvalidateTevRegister
 *   _HSD_StateInvalidateTevStage _HSD_StateInvalidateTevStage
 *   _HSD_StateInvalidateColorChannel _HSD_StateInvalidateColorChannel
 *   HSD_SetTevRegAll HSD_SetTevRegAll
 *   HSD_SetupTevStageAll HSD_SetupTevStageAll    fn_801B3638 HSD_SetupTevStage
 *   HSD_StateSetNumTevStages HSD_StateSetNumTevStages
 *   HSD_StateGetNumTevStages HSD_StateGetNumTevStages HSD_StateInitTev HSD_StateInitTev
 *   HSD_StateSetNumTexGens HSD_StateSetNumTexGens
 *   HSD_SetupChannelAll HSD_SetupChannelAll     HSD_SetupChannel HSD_SetupChannel
 *   fn_801B3AE8 (disables a channel's lighting; no Melee counterpart)
 */
void HSD_RenderInitAllocData(void);
HSD_ObjAllocData* HSD_RenderGetAllocData(void);
HSD_ObjAllocData* HSD_TevRegGetAllocData(void);
HSD_ObjAllocData* HSD_ChanGetAllocData(void);
void HSD_SetupChannel(HSD_Chan* ch);
void fn_801B3AE8(GXChannelID chan);
void HSD_StateSetNumChans(int num);
void HSD_SetupChannelAll(HSD_Chan* ch);
void HSD_StateRegisterTexGen(int coord);
void HSD_StateSetNumTexGens(void);
void HSD_StateInitTev(void);
int HSD_StateGetNumTevStages(void);
int HSD_StateAssignTev(void);
void HSD_StateSetNumTevStages(void);
void HSD_SetupTevStage(HSD_TevDesc* desc);
void HSD_SetupTevStageAll(HSD_TevDesc* desc);
int HSD_Index2TevStage(int idx);
void HSD_SetTevRegAll(void);
void _HSD_StateInvalidateColorChannel(void);
void _HSD_StateInvalidateTevStage(void);
void _HSD_StateInvalidateTevRegister(void);
void _HSD_StateInvalidateTexCoordGen(void);

#endif /* HSD_TEV_H */
