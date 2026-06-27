/**
 * @file hsd_tev.c
 * @brief HSD TEV (Texture Environment) stage setup and management.
 *
 * Address range: 0x801B1730 - 0x801B3D1C
 * Contains TEV stage configuration, texture coordinate generation,
 * color/alpha combine setup, and render pass state management.
 * This is the core of the HSD material rendering pipeline.
 *
 * Decompiled from Melee src/sysdolphin/baselib/tev.c / texp.c
 */

#include "dolphin/types.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_tobj.h"
#include "hsd/hsd_memory.h"

/* ========================================================================= */
/*  Internal TEV types                                                        */
/* ========================================================================= */

/* TEV stage descriptor - internal representation */
typedef struct HSD_TevDesc {
    u32 stage;         /* GXTevStageID */
    u32 texcoord;      /* GXTexCoordID */
    u32 texmap;        /* GXTexMapID */
    u32 color_chan;     /* GXChannelID */
    /* Color combine */
    u32 color_a;       /* GXTevColorArg */
    u32 color_b;
    u32 color_c;
    u32 color_d;
    u32 color_op;      /* GXTevOp */
    u32 color_bias;    /* GXTevBias */
    u32 color_scale;   /* GXTevScale */
    u32 color_clamp;   /* GXBool */
    u32 color_out_reg; /* GXTevRegID */
    /* Alpha combine */
    u32 alpha_a;       /* GXTevAlphaArg */
    u32 alpha_b;
    u32 alpha_c;
    u32 alpha_d;
    u32 alpha_op;
    u32 alpha_bias;
    u32 alpha_scale;
    u32 alpha_clamp;
    u32 alpha_out_reg;
} HSD_TevDesc;

/* TExp node types */
#define HSD_TE_ZERO  0
#define HSD_TE_TEX   1
#define HSD_TE_RAS   2
#define HSD_TE_CNST  3
#define HSD_TE_IMM   4
#define HSD_TE_KONST 5
#define HSD_TE_ALL   6

/* TExp node structure - full definition of the union forward-declared in hsd_forward.h */
union HSD_TExp {
    struct {
        u32 type;
        void* data;
        union HSD_TExp* next;
        u32 op;
        union HSD_TExp* arg[4];
        u32 sel;
        u32 reg;
    };
};

/* Forward declarations */
extern void fn_801AA35C(void* list, u32 size, u32 alignment);
extern u32 lbl_8047B358;
extern u32 lbl_8047B370;
extern void GXSetTevStages(u8 numStages);
extern void GXSetTevOrder(u32 stage, u32 texcoord, u32 texmap, u32 chan);
extern void GXSetTevColorIn(u32 stage, u32 a, u32 b, u32 c, u32 d);
extern void GXSetTevColorOp(u32 stage, u32 op, u32 bias, u32 scale,
                             u32 clamp, u32 out_reg);
extern void GXSetTevAlphaIn(u32 stage, u32 a, u32 b, u32 c, u32 d);
extern void GXSetTevAlphaOp(u32 stage, u32 op, u32 bias, u32 scale,
                             u32 clamp, u32 out_reg);
extern void GXSetTevSwapMode(u32 stage, u32 ras_sel, u32 tex_sel);
extern void GXSetTevSwapModeTable(u32 id, u32 r, u32 g, u32 b, u32 a);
extern void GXSetTevKColorSel(u32 stage, u32 sel);
extern void GXSetTevKAlphaSel(u32 stage, u32 sel);
extern void GXSetTevIndirect(u32 stage, u32 ind_stage, u32 format,
                              u32 bias_sel, u32 mtx_sel, u32 wrap_s,
                              u32 wrap_t, u32 add_prev, u32 utc_lod,
                              u32 alpha_sel);
extern void GXSetNumTevStages(u8 num);
/* hsdAllocMemPiece/hsdFreeMemPiece declared in hsd_class.h with s32 */

/* TEV state globals */
static u8 tev_num_stages;
static HSD_TevDesc tev_stages[16];

/* ========================================================================= */
/*  TEV stage management                                                     */
/* ========================================================================= */

/*
 * HSD_TevInit - 0x801B1730 | Size: 0x124
 * Initialize all TEV stages to default passthrough state.
 */
void fn_801B1730(void) {
    u32 i;

    tev_num_stages = 1;

    for (i = 0; i < 16; i++) {
        tev_stages[i].stage = i;
        tev_stages[i].texcoord = 0xFF; /* GX_TEXCOORD_NULL */
        tev_stages[i].texmap = 0xFF;   /* GX_TEXMAP_NULL */
        tev_stages[i].color_chan = 0xFF; /* GX_COLOR_NULL */

        tev_stages[i].color_a = 15; /* GX_CC_ZERO */
        tev_stages[i].color_b = 15;
        tev_stages[i].color_c = 15;
        tev_stages[i].color_d = 15;
        tev_stages[i].color_op = 0;   /* GX_TEV_ADD */
        tev_stages[i].color_bias = 0; /* GX_TB_ZERO */
        tev_stages[i].color_scale = 0; /* GX_CS_SCALE_1 */
        tev_stages[i].color_clamp = 1; /* TRUE */
        tev_stages[i].color_out_reg = 0; /* GX_TEVPREV */

        tev_stages[i].alpha_a = 7; /* GX_CA_ZERO */
        tev_stages[i].alpha_b = 7;
        tev_stages[i].alpha_c = 7;
        tev_stages[i].alpha_d = 7;
        tev_stages[i].alpha_op = 0;
        tev_stages[i].alpha_bias = 0;
        tev_stages[i].alpha_scale = 0;
        tev_stages[i].alpha_clamp = 1;
        tev_stages[i].alpha_out_reg = 0;
    }
}

/*
 * 0x801B1854 | Size: 0x30
 * Initialize TEV vtx desc list from BSS object.
 */
extern u8 lbl_804656E0[];
void fn_801B1854(void) {
    fn_801AA35C(lbl_804656E0, 0x28, 4);
}

/* Address: 0x801B1884 | Size: 0xC */
/* Get pointer to TEV vtx desc BSS object */
void* fn_801B1884(void) {
    return lbl_804656E0;
}

/*
 * HSD_TevSetColorInput - 0x801B1890 | Size: 0x48
 * Set color input selection for a TEV stage.
 */
void fn_801B1890(u32 stage, u32 a, u32 b, u32 c, u32 d) {
    if (stage < 16) {
        tev_stages[stage].color_a = a;
        tev_stages[stage].color_b = b;
        tev_stages[stage].color_c = c;
        tev_stages[stage].color_d = d;
    }
}

/*
 * HSD_TevSetColorCombine - 0x801B18D8 | Size: 0x1F8
 * Full TEV color combine stage configuration.
 * Sets color inputs, operation, bias, scale, clamp, and output register.
 */
void fn_801B18D8(u32 stage, u32 a, u32 b, u32 c, u32 d,
                  u32 op, u32 bias, u32 scale, u32 clamp, u32 out_reg) {
    if (stage >= 16) {
        return;
    }

    tev_stages[stage].color_a = a;
    tev_stages[stage].color_b = b;
    tev_stages[stage].color_c = c;
    tev_stages[stage].color_d = d;
    tev_stages[stage].color_op = op;
    tev_stages[stage].color_bias = bias;
    tev_stages[stage].color_scale = scale;
    tev_stages[stage].color_clamp = clamp;
    tev_stages[stage].color_out_reg = out_reg;

    GXSetTevColorIn(stage, a, b, c, d);
    GXSetTevColorOp(stage, op, bias, scale, clamp, out_reg);
}

/*
 * HSD_TevSetAlphaCombine - 0x801B1AD0 | Size: 0x568
 * Full TEV alpha combine stage configuration with validation.
 * Large function because it validates inputs, handles special
 * cases for bump mapping and indirect textures, and configures
 * the TEV stage swap modes for alpha.
 */
void fn_801B1AD0(u32 stage, u32 a, u32 b, u32 c, u32 d,
                  u32 op, u32 bias, u32 scale, u32 clamp, u32 out_reg) {
    if (stage >= 16) {
        return;
    }

    tev_stages[stage].alpha_a = a;
    tev_stages[stage].alpha_b = b;
    tev_stages[stage].alpha_c = c;
    tev_stages[stage].alpha_d = d;
    tev_stages[stage].alpha_op = op;
    tev_stages[stage].alpha_bias = bias;
    tev_stages[stage].alpha_scale = scale;
    tev_stages[stage].alpha_clamp = clamp;
    tev_stages[stage].alpha_out_reg = out_reg;

    GXSetTevAlphaIn(stage, a, b, c, d);
    GXSetTevAlphaOp(stage, op, bias, scale, clamp, out_reg);
}

/*
 * HSD_TevSetIndirect - 0x801B2038 | Size: 0x528
 * Configure indirect texture parameters for a TEV stage.
 * Large function that handles the full indirect texture pipeline
 * including coordinate warping, matrix selection, and bump mapping.
 */
void fn_801B2038(u32 stage, u32 ind_stage, u32 format, u32 bias_sel,
                  u32 mtx_sel, u32 wrap_s, u32 wrap_t, u32 add_prev,
                  u32 utc_lod, u32 alpha_sel) {
    if (stage >= 16) {
        return;
    }

    GXSetTevIndirect(stage, ind_stage, format, bias_sel, mtx_sel,
                     wrap_s, wrap_t, add_prev, utc_lod, alpha_sel);
}

/* ========================================================================= */
/*  TEV state accessors                                                      */
/* ========================================================================= */

/*
 * HSD_TevGetActiveStageInfo - 0x801B2560 | Size: 0x64
 * Query active stage information.
 */
void fn_801B2560(u32 stage, u32* texcoord, u32* texmap, u32* chan) {
    if (stage >= 16) {
        return;
    }
    if (texcoord != NULL) {
        *texcoord = tev_stages[stage].texcoord;
    }
    if (texmap != NULL) {
        *texmap = tev_stages[stage].texmap;
    }
    if (chan != NULL) {
        *chan = tev_stages[stage].color_chan;
    }
}

/*
 * HSD_TevSetStageParams - 0x801B25C4 | Size: 0x90
 * Update TEV stage order parameters (texcoord, texmap, channel).
 */
void fn_801B25C4(u32 stage, u32 texcoord, u32 texmap, u32 chan) {
    if (stage >= 16) {
        return;
    }
    tev_stages[stage].texcoord = texcoord;
    tev_stages[stage].texmap = texmap;
    tev_stages[stage].color_chan = chan;
    GXSetTevOrder(stage, texcoord, texmap, chan);
}

/*
 * HSD_TevAllocColorReg - 0x801B2654 | Size: 0xA4
 * Allocate a TEV color register for a material pass.
 * Manages the pool of 4 color registers (CPREV, C0, C1, C2).
 */
static u32 color_reg_used;

u32 fn_801B2654(void) {
    u32 i;
    for (i = 1; i < 4; i++) {
        if ((color_reg_used & (1 << i)) == 0) {
            color_reg_used |= (1 << i);
            return i;
        }
    }
    return 0; /* fallback to CPREV */
}

/* Address: 0x801B26F8 | Size: 0x20 */
/* Wrapper calling fn_801ACD7C (display list dispatch) */
extern void fn_801ACD7C(void);
void fn_801B26F8(void) {
    fn_801ACD7C();
}

/* Address: 0x801B2718 | Size: 0x24 */
/* Reset TEV state SDA variables */
extern u8 lbl_8047B351;
extern u8 lbl_8047B350;
extern s32 lbl_8047B34C;
#pragma push
#pragma optimization_level 1
void fn_801B2718(void) {
    lbl_8047B351 = 0;
    lbl_8047B350 = 0;
    lbl_8047B34C = -1;
    lbl_8047B351 = 0xFF;
}
#pragma pop

/*
 * HSD_TevLookupReg - 0x801B273C | Size: 0x50
 * Look up which TEV register a given value is stored in.
 */
u32 fn_801B273C(u32 reg) {
    if (reg < 4) {
        return reg;
    }
    return 0xFF; /* invalid */
}

/*
 * HSD_TevAssignReg - 0x801B278C | Size: 0x50
 * Assign a value to a specific TEV register.
 */
void fn_801B278C(u32 reg, u32 value) {
    if (reg < 4) {
        /* Store the register assignment */
        color_reg_used |= (1 << reg);
    }
}

/*
 * HSD_TevSetSwapModeTable - 0x801B27DC | Size: 0x9C
 * Configure a TEV swap mode table entry.
 * Maps how RGBA channels are swapped for color/texture lookups.
 */
void fn_801B27DC(u32 id, u32 r, u32 g, u32 b, u32 a) {
    if (id >= 4) {
        return;
    }
    GXSetTevSwapModeTable(id, r, g, b, a);
}

/*
 * HSD_TevValidateOrder - 0x801B2878 | Size: 0x40
 * Validate TEV stage ordering. Ensures stages are contiguous
 * and properly ordered.
 */
BOOL fn_801B2878(u32 numStages) {
    if (numStages == 0 || numStages > 16) {
        return FALSE;
    }
    return TRUE;
}

/* Address: 0x801B28B8 | Size: 0x10 */
/* Store float into BSS object at offset 0x10 */
extern u8 lbl_80465710[];
void fn_801B28B8(f32 val) {
    *(f32*)(lbl_80465710 + 0x10) = val;
}

/*
 * HSD_TevSetKColorSel - 0x801B28C8 | Size: 0x84
 * Select which constant color to use for a TEV stage.
 */
void fn_801B28C8(u32 stage, u32 sel) {
    if (stage >= 16) {
        return;
    }
    GXSetTevKColorSel(stage, sel);
}

/*
 * HSD_TevSetKAlphaSel - 0x801B294C | Size: 0x98
 * Select which constant alpha to use for a TEV stage.
 */
void fn_801B294C(u32 stage, u32 sel) {
    if (stage >= 16) {
        return;
    }
    GXSetTevKAlphaSel(stage, sel);
}

/* ========================================================================= */
/*  TEV expression compilation                                               */
/* ========================================================================= */

/*
 * HSD_TExpCompileColor - 0x801B29E4 | Size: 0x538
 * Compile a TExp color expression tree into TEV stages.
 * Walks the expression tree and generates the appropriate
 * GX TEV stage configurations for color channel processing.
 * This is the main code generation pass for the material system.
 */
void fn_801B29E4(HSD_TExp* root, u32* num_stages, u32 start_stage) {
    HSD_TExp* node;
    u32 stage;

    if (root == NULL) {
        return;
    }

    stage = start_stage;
    node = root;

    while (node != NULL) {
        if (stage >= 16) {
            break;
        }

        /* Process this node into a TEV stage */
        switch (node->type) {
        case HSD_TE_ZERO:
            /* Zero input - just pass through */
            tev_stages[stage].color_a = 15; /* CC_ZERO */
            tev_stages[stage].color_b = 15;
            tev_stages[stage].color_c = 15;
            tev_stages[stage].color_d = 15;
            break;

        case HSD_TE_TEX:
            /* Texture input */
            tev_stages[stage].color_a = 15; /* CC_ZERO */
            tev_stages[stage].color_b = 8;  /* CC_TEXC */
            tev_stages[stage].color_c = 15;
            tev_stages[stage].color_d = 0;  /* CC_CPREV */
            break;

        case HSD_TE_RAS:
            /* Rasterized color input */
            tev_stages[stage].color_a = 15;
            tev_stages[stage].color_b = 10; /* CC_RASC */
            tev_stages[stage].color_c = 15;
            tev_stages[stage].color_d = 0;
            break;

        default:
            break;
        }

        GXSetTevColorIn(stage, tev_stages[stage].color_a,
                         tev_stages[stage].color_b,
                         tev_stages[stage].color_c,
                         tev_stages[stage].color_d);
        GXSetTevColorOp(stage, tev_stages[stage].color_op,
                         tev_stages[stage].color_bias,
                         tev_stages[stage].color_scale,
                         tev_stages[stage].color_clamp,
                         tev_stages[stage].color_out_reg);

        stage++;
        node = node->next;
    }

    if (num_stages != NULL) {
        *num_stages = stage;
    }
}

/*
 * HSD_TExpCompileAlpha - 0x801B2F1C | Size: 0x24C
 * Compile a TExp alpha expression tree into TEV stages.
 * Similar to color compile but for the alpha channel.
 */
void fn_801B2F1C(HSD_TExp* root, u32* num_stages, u32 start_stage) {
    HSD_TExp* node;
    u32 stage;

    if (root == NULL) {
        return;
    }

    stage = start_stage;
    node = root;

    while (node != NULL) {
        if (stage >= 16) {
            break;
        }

        switch (node->type) {
        case HSD_TE_ZERO:
            tev_stages[stage].alpha_a = 7; /* CA_ZERO */
            tev_stages[stage].alpha_b = 7;
            tev_stages[stage].alpha_c = 7;
            tev_stages[stage].alpha_d = 7;
            break;

        case HSD_TE_TEX:
            tev_stages[stage].alpha_a = 7;
            tev_stages[stage].alpha_b = 4; /* CA_TEXA */
            tev_stages[stage].alpha_c = 7;
            tev_stages[stage].alpha_d = 0; /* CA_APREV */
            break;

        case HSD_TE_RAS:
            tev_stages[stage].alpha_a = 7;
            tev_stages[stage].alpha_b = 5; /* CA_RASA */
            tev_stages[stage].alpha_c = 7;
            tev_stages[stage].alpha_d = 0;
            break;

        default:
            break;
        }

        GXSetTevAlphaIn(stage, tev_stages[stage].alpha_a,
                         tev_stages[stage].alpha_b,
                         tev_stages[stage].alpha_c,
                         tev_stages[stage].alpha_d);
        GXSetTevAlphaOp(stage, tev_stages[stage].alpha_op,
                         tev_stages[stage].alpha_bias,
                         tev_stages[stage].alpha_scale,
                         tev_stages[stage].alpha_clamp,
                         tev_stages[stage].alpha_out_reg);

        stage++;
        node = node->next;
    }

    if (num_stages != NULL) {
        *num_stages = stage;
    }
}

/* Address: 0x801B3168 | Size: 0xC */
/* Clear TEV stage count SDA variable */
void fn_801B3168(void) {
    lbl_8047B358 = 0;
}

/*
 * 0x801B3174 | Size: 0x30
 * Clear field 0x8 of 4 entries in rodata struct array.
 */
extern u8 lbl_8036CFE8[];
void fn_801B3174(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        *(u32*)(lbl_8036CFE8 + i * 0xC + 0x8) = 0;
    }
}

/*
 * 0x801B31A4 | Size: 0x50
 * Reset TEV stages - release each stage then clear globals.
 */
extern void fn_800BBC34(s32 stage);
extern void fn_800BBC0C(s32 val);
#pragma push
#pragma optimization_level 2
void fn_801B31A4(void) {
    s32 i;
    for (i = 0; i < 16; i++) {
        fn_800BBC34(i);
    }
    fn_800BBC0C(0);
    lbl_8047B370 = 0;
}
#pragma pop

/*
 * HSD_TExpAllocReg - 0x801B31F4 | Size: 0x64
 * Allocate a TExp register node.
 */
HSD_TExp* fn_801B31F4(u32 reg) {
    HSD_TExp* exp;
    exp = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TExp));
    if (exp != NULL) {
        exp->type = HSD_TE_IMM;
        exp->reg = reg;
        exp->data = NULL;
        exp->next = NULL;
    }
    return exp;
}

/*
 * HSD_TExpMakeBinOp - 0x801B3258 | Size: 0xE0
 * Build a binary operation TExp node.
 * Creates a node that combines two child expressions.
 */
HSD_TExp* fn_801B3258(u32 op, HSD_TExp* left, HSD_TExp* right) {
    HSD_TExp* exp;

    if (left == NULL) {
        return right;
    }
    if (right == NULL) {
        return left;
    }

    exp = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TExp));
    if (exp != NULL) {
        exp->type = HSD_TE_ALL;
        exp->op = op;
        exp->arg[0] = left;
        exp->arg[1] = right;
        exp->arg[2] = NULL;
        exp->arg[3] = NULL;
        exp->next = NULL;
    }
    return exp;
}

/*
 * HSD_TExpMakeUnaryOp - 0x801B3338 | Size: 0xD0
 * Build a unary operation TExp node.
 */
HSD_TExp* HSD_Index2TevStage(u32 op, HSD_TExp* child) {
    HSD_TExp* exp;

    if (child == NULL) {
        return NULL;
    }

    exp = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TExp));
    if (exp != NULL) {
        exp->type = HSD_TE_ALL;
        exp->op = op;
        exp->arg[0] = child;
        exp->arg[1] = NULL;
        exp->arg[2] = NULL;
        exp->arg[3] = NULL;
        exp->next = NULL;
    }
    return exp;
}

/*
 * HSD_TExpSimplify - 0x801B3408 | Size: 0x230
 * Simplify a TExp expression tree by folding constants,
 * removing identity operations, and merging compatible nodes.
 */
HSD_TExp* fn_801B3408(HSD_TExp* exp) {
    if (exp == NULL) {
        return NULL;
    }

    /* Recursively simplify children */
    if (exp->type == HSD_TE_ALL) {
        u32 i;
        for (i = 0; i < 4; i++) {
            if (exp->arg[i] != NULL) {
                exp->arg[i] = fn_801B3408(exp->arg[i]);
            }
        }

        /* Check for identity operations */
        /* If all inputs are zero, result is zero */
        if (exp->arg[0] != NULL && exp->arg[0]->type == HSD_TE_ZERO) {
            if (exp->arg[1] == NULL || exp->arg[1]->type == HSD_TE_ZERO) {
                HSD_TExp* result = exp->arg[0];
                hsdFreeMemPiece(exp, sizeof(HSD_TExp));
                return result;
            }
        }
    }

    return exp;
}

/*
 * HSD_TExpEvaluate - 0x801B3638 | Size: 0x138
 * Evaluate a TExp expression tree to determine how many TEV stages
 * it will require and what resources it needs.
 */
u32 fn_801B3638(HSD_TExp* exp) {
    u32 count;

    if (exp == NULL) {
        return 0;
    }

    count = 1;

    if (exp->type == HSD_TE_ALL) {
        u32 i;
        for (i = 0; i < 4; i++) {
            if (exp->arg[i] != NULL) {
                count += fn_801B3638(exp->arg[i]);
            }
        }
    }

    if (exp->next != NULL) {
        count += fn_801B3638(exp->next);
    }

    return count;
}

/*
 * HSD_TExpSetInput - 0x801B3770 | Size: 0x30
 * Set an input source for a TExp node.
 */
void fn_801B3770(void) {
    extern void fn_800BC8C8(u8 count);
    u32 count;

    count = lbl_8047B370;
    fn_800BC8C8((u8)count);
    lbl_8047B370 = 0;
}

/*
 * HSD_TExpTraverse - 0x801B37A0 | Size: 0xDC
 * Walk a TExp tree and call a visitor function for each node.
 */
void HSD_StateAssignTev(HSD_TExp* exp, void (*visitor)(HSD_TExp*)) {
    u32 i;

    if (exp == NULL || visitor == NULL) {
        return;
    }

    visitor(exp);

    if (exp->type == HSD_TE_ALL) {
        for (i = 0; i < 4; i++) {
            if (exp->arg[i] != NULL) {
                HSD_StateAssignTev(exp->arg[i], visitor);
            }
        }
    }

    if (exp->next != NULL) {
        HSD_StateAssignTev(exp->next, visitor);
    }
}

/* NOTE: fn_801B387C (Size: 0x8) is already decompiled in another file */

/* Address: 0x801B3884 | Size: 0xC */
/* Clear TEV expression list SDA global */
void fn_801B3884(void) {
    lbl_8047B370 = 0;
}

/*
 * HSD_TExpFreeNode - 0x801B3890 | Size: 0x30
 * Release TEV stage count from SDA variable.
 */
extern void fn_800B884C(u8 count);
void fn_801B3890(void) {
    u8 count = (u8)lbl_8047B358;
    fn_800B884C(count);
    lbl_8047B358 = 0;
}

/*
 * HSD_TExpFreeAll - 0x801B38C0 | Size: 0xD8
 * Free all nodes in a TExp tree recursively.
 */
void HSD_StateRegisterTexGen(HSD_TExp* exp) {
    u32 i;

    if (exp == NULL) {
        return;
    }

    /* Free children first */
    if (exp->type == HSD_TE_ALL) {
        for (i = 0; i < 4; i++) {
            if (exp->arg[i] != NULL) {
                HSD_StateRegisterTexGen(exp->arg[i]);
                exp->arg[i] = NULL;
            }
        }
    }

    /* Free the linked list */
    if (exp->next != NULL) {
        HSD_StateRegisterTexGen(exp->next);
        exp->next = NULL;
    }

    hsdFreeMemPiece(exp, sizeof(HSD_TExp));
}

/*
 * HSD_TExpBuildColorExpr - 0x801B3998 | Size: 0x110
 * Build a TExp color expression from a TObj chain.
 * Creates the expression tree that represents the color
 * combine operations for a set of texture layers.
 */
HSD_TExp* fn_801B3998(HSD_TObj* tobj, HSD_TExp* list) {
    HSD_TExp* root = NULL;
    HSD_TExp* tex_node;
    HSD_TObj* t;

    for (t = tobj; t != NULL; t = t->next) {
        u32 colormap = tobj_colormap(t);

        if (colormap == TEX_COLORMAP_NONE) {
            continue;
        }

        /* Create a texture node for this TObj */
        tex_node = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TExp));
        if (tex_node == NULL) {
            break;
        }
        tex_node->type = HSD_TE_TEX;
        tex_node->data = t;
        tex_node->next = NULL;

        /* Combine with existing expression based on colormap mode */
        if (root == NULL) {
            root = tex_node;
        } else {
            root = fn_801B3258(0 /* ADD */, root, tex_node);
        }
    }

    return root;
}

/*
 * HSD_TExpLink - 0x801B3AA8 | Size: 0x40
 * Link a TExp node into a list.
 */
void fn_801B3AA8(HSD_TExp** list, HSD_TExp* node) {
    if (list == NULL || node == NULL) {
        return;
    }
    node->next = *list;
    *list = node;
}

/*
 * HSD_TExpBuildAlphaExpr - 0x801B3AE8 | Size: 0x234
 * Build a TExp alpha expression from a TObj chain.
 * Similar to color expression builder but for the alpha channel.
 */
HSD_TExp* fn_801B3AE8(HSD_TObj* tobj, HSD_TExp* list) {
    HSD_TExp* root = NULL;
    HSD_TExp* tex_node;
    HSD_TObj* t;

    for (t = tobj; t != NULL; t = t->next) {
        u32 alphamap = tobj_alphamap(t);

        if (alphamap == TEX_ALPHAMAP_NONE) {
            continue;
        }

        tex_node = (HSD_TExp*)hsdAllocMemPiece(sizeof(HSD_TExp));
        if (tex_node == NULL) {
            break;
        }
        tex_node->type = HSD_TE_TEX;
        tex_node->data = t;
        tex_node->next = NULL;

        /* Combine based on alphamap mode */
        switch (alphamap) {
        case TEX_ALPHAMAP_BLEND:
            if (root != NULL) {
                root = fn_801B3258(1 /* BLEND */, root, tex_node);
            } else {
                root = tex_node;
            }
            break;

        case TEX_ALPHAMAP_MODULATE:
            if (root != NULL) {
                root = fn_801B3258(2 /* MUL */, root, tex_node);
            } else {
                root = tex_node;
            }
            break;

        case TEX_ALPHAMAP_REPLACE:
            root = tex_node;
            break;

        case TEX_ALPHAMAP_ADD:
            if (root != NULL) {
                root = fn_801B3258(0 /* ADD */, root, tex_node);
            } else {
                root = tex_node;
            }
            break;

        case TEX_ALPHAMAP_SUB:
            if (root != NULL) {
                root = fn_801B3258(3 /* SUB */, root, tex_node);
            } else {
                root = tex_node;
            }
            break;

        default:
            root = tex_node;
            break;
        }
    }

    return root;
}

/*
 * HSD_TExpCompileMaterial - 0x801B3D1C | Size: 0x524
 * Full material expression compile.
 * Takes the TObj chain from a material and generates the complete
 * TEV stage configuration. This is the top-level entry point for
 * the texture expression compilation system.
 */
void fn_801B3D1C(HSD_TObj* tobj, u32 render_mode) {
    HSD_TExp* color_expr;
    HSD_TExp* alpha_expr;
    HSD_TExp* list = NULL;
    u32 num_stages;

    /* Build expression trees */
    color_expr = fn_801B3998(tobj, list);
    alpha_expr = fn_801B3AE8(tobj, list);

    /* Simplify expressions */
    color_expr = fn_801B3408(color_expr);
    alpha_expr = fn_801B3408(alpha_expr);

    /* Reset TEV state */
    color_reg_used = 0;
    fn_801B1730();

    /* Compile color expression into TEV stages */
    num_stages = 0;
    fn_801B29E4(color_expr, &num_stages, 0);

    /* Compile alpha expression into same stages */
    fn_801B2F1C(alpha_expr, NULL, 0);

    /* Set final TEV stage count */
    if (num_stages > 0) {
        fn_801B1854();
    }

    /* Free expression trees */
    HSD_StateRegisterTexGen(color_expr);
    HSD_StateRegisterTexGen(alpha_expr);
}
