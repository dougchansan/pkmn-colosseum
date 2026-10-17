/**
 * @file tev.c
 * @brief HSD TEV/channel render state: lighting channels, TEV stages, TEV
 *        registers and the render/TEV/channel allocators.
 *
 * Retail TU: .text 0x801B3168 - 0x801B42C0, with its .data (0x8036CFE8:
 * TEV register cache, channel state tables, jump tables), .bss (0x80465728:
 * allocators), .sbss (0x8047B358) and .sdata2 ("tev.c", "0" at 0x8047DE60).
 *
 * HAL sysdolphin (>= 1.3.0.0) tev.c, reconstructed with the Melee
 * decompilation (doldecomp/melee, src/sysdolphin/baselib/tev.c) as reference.
 * Functions appear in HAL source order; the library is built with
 * -inline auto,deferred, which emits them in reverse order.
 */
#include "hsd/hsd_tev.h"

#include "crt/string.h"
#include "hsd/hsd_debug.h"

extern void HSD_ObjAllocInit(HSD_ObjAllocData* data, u32 size, u32 align);

/* GX */
extern void GXSetChanAmbColor(GXChannelID chan, GXColor color); /* GXSetChanAmbColor */
extern void GXSetChanMatColor(GXChannelID chan, GXColor color); /* GXSetChanMatColor */
extern void GXSetChanCtrl(GXChannelID chan, u8 enable, GXColorSrc amb_src,
                        GXColorSrc mat_src, int light_mask,
                        GXDiffuseFn diff_fn, GXAttnFn attn_fn); /* GXSetChanCtrl */
extern void GXSetNumChans(u8 num); /* GXSetNumChans */
extern void GXSetNumTexGens(u8 num); /* GXSetNumTexGens */
extern void GXSetNumTevStages(u8 num); /* GXSetNumTevStages */
extern void GXSetTevOrder(u32 stage, u32 coord, u32 map, u32 color); /* GXSetTevOrder */
extern void GXSetTevOp(u32 stage, u32 mode);
extern void GXSetTevSwapMode(u32 stage, GXTevSwapSel ras_sel,
                        GXTevSwapSel tex_sel); /* GXSetTevSwapMode */
extern void GXSetTevSwapModeTable(GXTevSwapSel table, GXTevColorChan red,
                        GXTevColorChan green, GXTevColorChan blue,
                        GXTevColorChan alpha); /* GXSetTevSwapModeTable */
extern void GXSetTevColorOp(u32 stage, GXTevOp op, GXTevBias bias,
                        GXTevScale scale, u8 clamp,
                        GXTevRegID out_reg); /* GXSetTevColorOp */
extern void GXSetTevColorIn(u32 stage, GXTevColorArg a, GXTevColorArg b,
                        GXTevColorArg c, GXTevColorArg d); /* GXSetTevColorIn */
extern void GXSetTevAlphaOp(u32 stage, GXTevOp op, GXTevBias bias,
                        GXTevScale scale, u8 clamp,
                        GXTevRegID out_reg); /* GXSetTevAlphaOp */
extern void GXSetTevAlphaIn(u32 stage, GXTevAlphaArg a, GXTevAlphaArg b,
                        GXTevAlphaArg c, GXTevAlphaArg d); /* GXSetTevAlphaIn */
extern void GXSetTevKColorSel(u32 stage, GXTevKColorSel sel); /* GXSetTevKColorSel */
extern void GXSetTevKAlphaSel(u32 stage, GXTevKAlphaSel sel); /* GXSetTevKAlphaSel */
extern void GXSetTevColorS10(GXTevRegID id, GXColorS10 color); /* GXSetTevColorS10 */
extern void GXSetTevDirect(u32 stage); /* GXSetTevDirect */
extern void GXSetNumIndStages(u8 num); /* GXSetNumIndStages */

static struct {
    GXColorS10 a;
    int c;
} TevReg[4] = { 0 };

static HSD_Chan prev_ch[4] = {
    { NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
    { NULL, GX_COLOR1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
    { NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
    { NULL, GX_ALPHA1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
};

static HSD_ObjAllocData render_alloc_data;
static HSD_ObjAllocData tevreg_alloc_data;
static HSD_ObjAllocData chan_alloc_data;

static int current_tev;
static int prev_amb_invalid[2];
static int prev_mat_invalid[2];
static int prev_num_chans;
static int num_tex_gens;

void HSD_RenderInitAllocData(void)
{
    HSD_ObjAllocInit(&render_alloc_data, 28, 4);
    HSD_ObjAllocInit(&tevreg_alloc_data, 20, 4);
    HSD_ObjAllocInit(&chan_alloc_data, sizeof(HSD_Chan), 4);
}

HSD_ObjAllocData* HSD_RenderGetAllocData(void)
{
    return &render_alloc_data;
}

HSD_ObjAllocData* HSD_TevRegGetAllocData(void)
{
    return &tevreg_alloc_data;
}

HSD_ObjAllocData* HSD_ChanGetAllocData(void)
{
    return &chan_alloc_data;
}

static inline BOOL CompareRGB(GXColor* c0, GXColor* c1)
{
    u32* d0 = (u32*) c0;
    u32* d1 = (u32*) c1;
    return ((*d0 ^ *d1) & 0xFFFFFF00) != 0;
}

static inline BOOL CompareRGBA(GXColor* c0, GXColor* c1)
{
    u32* d0 = (u32*) c0;
    u32* d1 = (u32*) c1;
    return *d0 != *d1;
}

static inline void CopyRGB(GXColor* dst, GXColor* src)
{
    u32* d = (u32*) dst;
    u32* s = (u32*) src;
    *d = (*d & 0xff) | (*s & 0xffffff00);
}

/* HSD_SetupChannel */
void HSD_SetupChannel(HSD_Chan* ch)
{
    int idx;
    GXChannelID chan;
    int no;

    if (ch == NULL || ch->chan == GX_COLOR_NULL) {
        return;
    }

    chan = ch->chan;
    idx = chan & 3;
    no = chan & 1;
    if (ch->enable != GX_DISABLE && ch->amb_src == GX_SRC_REG) {
        if (prev_amb_invalid[no] != 0) {
            prev_amb_invalid[no] = 0;
            GXSetChanAmbColor(no + 4, ch->amb_color);
            prev_ch[no].amb_color = ch->amb_color;
        } else if (chan == GX_COLOR0A0 || chan == GX_COLOR1A1) {
            if (CompareRGBA(&ch->amb_color, &prev_ch[no].amb_color)) {
                prev_ch[no].amb_color = ch->amb_color;
                goto set_amb;
            }
        } else if (chan == GX_COLOR0 || chan == GX_COLOR1) {
            if (CompareRGB(&ch->amb_color, &prev_ch[no].amb_color)) {
                CopyRGB(&prev_ch[no].amb_color, &ch->amb_color);
                goto set_amb;
            }
        } else if (ch->amb_color.a != prev_ch[no].amb_color.a) {
            prev_ch[no].amb_color.a = ch->amb_color.a;
        set_amb:
            GXSetChanAmbColor(chan, ch->amb_color);
        }
    }

    if (ch->mat_src == GX_SRC_REG) {
        if (prev_mat_invalid[no] != 0) {
            prev_mat_invalid[no] = 0;
            GXSetChanMatColor(no + 4, ch->mat_color);
            prev_ch[no].mat_color = ch->mat_color;
        } else if (chan == GX_COLOR0A0 || chan == GX_COLOR1A1) {
            if (CompareRGBA(&ch->mat_color, &prev_ch[no].mat_color)) {
                prev_ch[no].mat_color = ch->mat_color;
                goto set_mat;
            }
        } else if (chan == GX_COLOR0 || chan == GX_COLOR1) {
            if (CompareRGB(&ch->mat_color, &prev_ch[no].mat_color)) {
                CopyRGB(&prev_ch[no].mat_color, &ch->mat_color);
                goto set_mat;
            }
        } else if (ch->mat_color.a != prev_ch[no].mat_color.a) {
            prev_ch[no].mat_color.a = ch->mat_color.a;
        set_mat:
            GXSetChanMatColor(chan, ch->mat_color);
        }
    }

    if ((ch->enable != prev_ch[idx].enable) ||
        (ch->amb_src != prev_ch[idx].amb_src) ||
        (ch->mat_src != prev_ch[idx].mat_src) ||
        (ch->light_mask != prev_ch[idx].light_mask) ||
        (ch->diff_fn != prev_ch[idx].diff_fn) ||
        (ch->attn_fn != prev_ch[idx].attn_fn))
    {
        GXSetChanCtrl(chan, ch->enable, ch->amb_src, ch->mat_src,
                    ch->light_mask, ch->diff_fn, ch->attn_fn);
        prev_ch[idx].enable = ch->enable;
        prev_ch[idx].amb_src = ch->amb_src;
        prev_ch[idx].mat_src = ch->mat_src;
        prev_ch[idx].light_mask = ch->light_mask;
        prev_ch[idx].diff_fn = ch->diff_fn;
        prev_ch[idx].attn_fn = ch->attn_fn;
        if (chan == GX_COLOR0A0 || chan == GX_COLOR1A1) {
            prev_ch[idx + 2].enable = ch->enable;
            prev_ch[idx + 2].amb_src = ch->amb_src;
            prev_ch[idx + 2].mat_src = ch->mat_src;
            prev_ch[idx + 2].light_mask = ch->light_mask;
            prev_ch[idx + 2].diff_fn = ch->diff_fn;
            prev_ch[idx + 2].attn_fn = ch->attn_fn;
        }
    }
}

/* Turns the lighting of a channel off, keeping its other settings. */
void fn_801B3AE8(GXChannelID chan)
{
    int idx = chan & 3;
    GXColorSrc amb_src;
    GXColorSrc mat_src;
    GXDiffuseFn diff_fn;
    GXAttnFn attn_fn;

    switch (chan) {
    case GX_COLOR0A0:
    case GX_COLOR1A1:
        if (prev_ch[idx].enable != GX_DISABLE ||
            prev_ch[idx + 2].enable != GX_DISABLE)
        {
            prev_ch[idx].enable = prev_ch[idx + 2].enable = GX_DISABLE;
            prev_ch[idx].light_mask = prev_ch[idx + 2].light_mask = 0;
            amb_src = prev_ch[idx].amb_src;
            mat_src = prev_ch[idx].mat_src;
            diff_fn = prev_ch[idx].diff_fn;
            attn_fn = prev_ch[idx].attn_fn;
            prev_ch[idx + 2].amb_src = amb_src;
            prev_ch[idx + 2].mat_src = mat_src;
            prev_ch[idx + 2].diff_fn = diff_fn;
            prev_ch[idx + 2].attn_fn = attn_fn;
            GXSetChanCtrl(chan, GX_DISABLE, amb_src, mat_src, 0, diff_fn,
                        attn_fn);
        }
        break;
    default:
        if (prev_ch[idx].enable != GX_DISABLE) {
            prev_ch[idx].enable = GX_DISABLE;
            prev_ch[idx].light_mask = 0;
            amb_src = prev_ch[idx].amb_src;
            mat_src = prev_ch[idx].mat_src;
            diff_fn = prev_ch[idx].diff_fn;
            attn_fn = prev_ch[idx].attn_fn;
            GXSetChanCtrl(chan, GX_DISABLE, amb_src, mat_src, 0, diff_fn,
                        attn_fn);
        }
        break;
    }
}

void HSD_StateSetNumChans(int num)
{
    if (prev_num_chans != num) {
        GXSetNumChans(num);
        prev_num_chans = num;
    }
}

static inline int HSD_Channel2Num(int chan);

/* HSD_SetupChannelAll */
void HSD_SetupChannelAll(HSD_Chan* ch)
{
    int num = 0;

    while (ch != NULL) {
        int n = HSD_Channel2Num(ch->chan);
        if (n > num) {
            num = n;
        }
        HSD_SetupChannel(ch);
        ch = ch->next;
    }
    HSD_StateSetNumChans((u8) num);
}

static inline int HSD_TexCoordID2Num(int id);

void HSD_StateRegisterTexGen(int coord)
{
    int num = HSD_TexCoordID2Num(coord);
    if (num > num_tex_gens) {
        num_tex_gens = num;
    }
}

/* HSD_StateSetNumTexGens */
void HSD_StateSetNumTexGens(void)
{
    GXSetNumTexGens(num_tex_gens);
    num_tex_gens = 0;
}

/* HSD_StateInitTev */
void HSD_StateInitTev(void)
{
    current_tev = 0;
}

/* HSD_StateGetNumTevStages */
int HSD_StateGetNumTevStages(void)
{
    return current_tev;
}

int HSD_StateAssignTev(void)
{
    return HSD_Index2TevStage(current_tev++);
}

/* HSD_StateSetNumTevStages */
void HSD_StateSetNumTevStages(void)
{
    GXSetNumTevStages(current_tev);
    current_tev = 0;
}

/* HSD_SetupTevStage */
void HSD_SetupTevStage(HSD_TevDesc* desc)
{
    GXSetTevOrder(desc->stage, desc->coord, desc->map, desc->color);
    if (desc->flags == 0) {
        GXSetTevOp(desc->stage, desc->u.tevconf.clr_op);
        GXSetTevSwapMode(desc->stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
        return;
    }
    GXSetTevColorOp(desc->stage, desc->u.tevconf.clr_op,
                desc->u.tevconf.clr_bias, desc->u.tevconf.clr_scale,
                desc->u.tevconf.clr_clamp, desc->u.tevconf.clr_out_reg);
    GXSetTevColorIn(desc->stage, desc->u.tevconf.clr_a, desc->u.tevconf.clr_b,
                desc->u.tevconf.clr_c, desc->u.tevconf.clr_d);
    GXSetTevAlphaOp(desc->stage, desc->u.tevconf.alpha_op,
                desc->u.tevconf.alpha_bias, desc->u.tevconf.alpha_scale,
                desc->u.tevconf.alpha_clamp, desc->u.tevconf.alpha_out_reg);
    GXSetTevAlphaIn(desc->stage, desc->u.tevconf.alpha_a, desc->u.tevconf.alpha_b,
                desc->u.tevconf.alpha_c, desc->u.tevconf.alpha_d);
    GXSetTevSwapModeTable(desc->u.tevconf.ras_swap, desc->u.tevconf.swap_red,
                desc->u.tevconf.swap_green, desc->u.tevconf.swap_blue,
                desc->u.tevconf.swap_alpha);
    if (desc->u.tevconf.tex_swap != desc->u.tevconf.ras_swap) {
        GXSetTevSwapModeTable(desc->u.tevconf.tex_swap, desc->u.tevconf.swap_red,
                    desc->u.tevconf.swap_green, desc->u.tevconf.swap_blue,
                    desc->u.tevconf.swap_alpha);
    }
    GXSetTevSwapMode(desc->stage, desc->u.tevconf.ras_swap,
                desc->u.tevconf.tex_swap);
    GXSetTevKColorSel(desc->stage, desc->u.tevconf.kcsel);
    GXSetTevKAlphaSel(desc->stage, desc->u.tevconf.kasel);
}

static inline int HSD_TevStage2Num(int stage);

static inline int setupTevStages(HSD_TevDesc* desc)
{
    int num = 0;

    while (desc != NULL) {
        int n = HSD_TevStage2Num(desc->stage);
        if (n > num) {
            num = n;
        }
        HSD_SetupTevStage(desc);
        desc = desc->next;
    }
    return num;
}

/* HSD_SetupTevStageAll */
void HSD_SetupTevStageAll(HSD_TevDesc* desc)
{
    current_tev = setupTevStages(desc);
    HSD_StateSetNumTevStages();
}

static inline int HSD_Channel2Num(int chan)
{
    switch (chan) {
    case GX_COLOR0:
        return 1;
    case GX_COLOR1:
        return 2;
    case GX_ALPHA0:
        return 1;
    case GX_ALPHA1:
        return 2;
    case GX_COLOR0A0:
        return 1;
    case GX_COLOR1A1:
        return 2;
    case GX_COLOR_NULL:
        return 0;
    default:
        HSD_ASSERT(753, 0);
        return 0;
    }
}

int HSD_Index2TevStage(int idx)
{
    switch (idx) {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    case 5:
        return 5;
    case 6:
        return 6;
    case 7:
        return 7;
    case 8:
        return 8;
    case 9:
        return 9;
    case 10:
        return 10;
    case 11:
        return 11;
    case 12:
        return 12;
    case 13:
        return 13;
    case 14:
        return 14;
    case 15:
        return 15;
    default:
        HSD_ASSERT(806, 0);
        return 15;
    }
}

static inline int HSD_TevStage2Num(int stage)
{
    switch (stage) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    case 4:
        return 5;
    case 5:
        return 6;
    case 6:
        return 7;
    case 7:
        return 8;
    case 8:
        return 9;
    case 9:
        return 10;
    case 10:
        return 11;
    case 11:
        return 12;
    case 12:
        return 13;
    case 13:
        return 14;
    case 14:
        return 15;
    case 15:
        return 16;
    default:
        HSD_ASSERT(890, 0);
        return 0;
    }
}

/*
 * Maps a TevReg[] slot to its GX register. Retail copies the colour argument
 * before evaluating the register switch, so the switch sits in its own
 * (expanded) function.
 */
static inline GXTevRegID TevRegIndex2ID(u32 idx)
{
    switch (idx) {
    case 0:
        return GX_TEVREG0;
    case 1:
        return GX_TEVREG1;
    case 2:
        return GX_TEVREG2;
    case 3:
        return GX_TEVPREV;
    default:
        return GX_TEVREG0;
    }
}

/* HSD_SetTevRegAll */
void HSD_SetTevRegAll(void)
{
    u32 i;

    for (i = 0; i < 4; i++) {
        if (TevReg[i].c != 0) {
            GXSetTevColorS10(TevRegIndex2ID(i), TevReg[i].a);
            TevReg[i].c = 0;
        }
    }
}

static inline int HSD_TexCoordID2Num(int id)
{
    switch (id) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    case 4:
        return 5;
    case 5:
        return 6;
    case 6:
        return 7;
    case 7:
        return 8;
    case 0xFF:
        return 0;
    default:
        HSD_ASSERT(1107, 0);
        return 0;
    }
}

static HSD_Chan invalid_prev_ch[4] = {
    { NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
    { NULL, GX_COLOR1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
    { NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
    { NULL, GX_ALPHA1, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0xFF, 0, 0, 0,
      GX_DF_NONE, GX_AF_NONE, NULL },
};

/* _HSD_StateInvalidateColorChannel */
void _HSD_StateInvalidateColorChannel(void)
{
    memcpy(&prev_ch, &invalid_prev_ch, sizeof(prev_ch));
    prev_amb_invalid[0] = 1;
    prev_amb_invalid[1] = 1;
    prev_mat_invalid[0] = 1;
    prev_mat_invalid[1] = 1;
    prev_num_chans = -1;
}

/* _HSD_StateInvalidateTevStage */
void _HSD_StateInvalidateTevStage(void)
{
    int i;

    for (i = 0; i < 16; i++) {
        GXSetTevDirect(i);
    }
    GXSetNumIndStages(0);
    current_tev = 0;
}

/* _HSD_StateInvalidateTevRegister */
void _HSD_StateInvalidateTevRegister(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        TevReg[i].c = 0;
    }
}

/* _HSD_StateInvalidateTexCoordGen */
void _HSD_StateInvalidateTexCoordGen(void)
{
    num_tex_gens = 0;
}
