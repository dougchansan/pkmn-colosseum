/**
 * @file hsd_texp.h
 * @brief HSD TExp - TEV expression trees (texp.c) and their scheduler (texpdag.c).
 *
 * Colosseum address ranges:
 *   texp.c    0x801B42C0 - 0x801B7CA0
 *   texpdag.c 0x801B7CA0 - 0x801BBAC8
 *
 * Adapted from the Melee decompilation (doldecomp/melee, sysdolphin/baselib/
 * texp.h and texpdag.h). Field offsets were checked against Colosseum's
 * code: HSD_TETev is 0x7C bytes and HSD_TECnst 0x14 (the sizes passed to
 * hsdAllocMemPiece/hsdFreeMemPiece), HSD_TExpTevDesc 0x88.
 */
#ifndef HSD_TEXP_H
#define HSD_TEXP_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "hsd/hsd_forward.h"
#include "hsd/hsd_tobj.h"

/* ------------------------------------------------------------------------ */
/* GX TEV enumerations (Dolphin SDK GXEnum.h values).                      */
/* hsd_tobj.h already defines a few of these as macros.                     */
/* ------------------------------------------------------------------------ */

typedef int GXTevOp;
typedef int GXTevBias;
typedef int GXTevScale;
typedef int GXTevColorArg;
typedef int GXTevAlphaArg;
typedef int GXTevRegID;
typedef int GXTevKColorID;
typedef int GXTevKColorSel;
typedef int GXTevKAlphaSel;
typedef int GXTevSwapSel;
typedef int GXTevClampMode;
typedef int GXChannelID;
typedef int GXTevColorChan;

#define GX_CH_RED 0
#define GX_CH_GREEN 1
#define GX_CH_BLUE 2
#define GX_CH_ALPHA 3

#define GX_TEVPREV 0
#define GX_TEVREG0 1
#define GX_TEVREG1 2
#define GX_TEVREG2 3

#define GX_KCOLOR0 0
#define GX_KCOLOR1 1
#define GX_KCOLOR2 2
#define GX_KCOLOR3 3

#define GX_CC_C0 2
#define GX_CC_A0 3
#define GX_CC_C1 4
#define GX_CC_A1 5
#define GX_CC_C2 6
#define GX_CC_A2 7
#define GX_CC_RASA 11
#define GX_CC_KONST 14

#define GX_CA_A0 1
#define GX_CA_A1 2
#define GX_CA_A2 3
#define GX_CA_RASA 5
#define GX_CA_KONST 6

#define GX_TEV_SWAP0 0
#define GX_TEV_SWAP1 1
#define GX_TEV_SWAP2 2
#define GX_TEV_SWAP3 3

#define GX_TC_LINEAR 0

#define GX_TB_ADDHALF 1
#define GX_TB_SUBHALF 2

#define GX_CS_SCALE_2 1
#define GX_CS_SCALE_4 2
#define GX_CS_DIVIDE_2 3

#define GX_TEV_KCSEL_1 0x00
#define GX_TEV_KCSEL_7_8 0x01
#define GX_TEV_KCSEL_3_4 0x02
#define GX_TEV_KCSEL_5_8 0x03
#define GX_TEV_KCSEL_1_2 0x04
#define GX_TEV_KCSEL_3_8 0x05
#define GX_TEV_KCSEL_1_4 0x06
#define GX_TEV_KCSEL_1_8 0x07
#define GX_TEV_KCSEL_K0 0x0C
#define GX_TEV_KCSEL_K1 0x0D
#define GX_TEV_KCSEL_K2 0x0E
#define GX_TEV_KCSEL_K3 0x0F
#define GX_TEV_KCSEL_K0_R 0x10
#define GX_TEV_KCSEL_K1_R 0x11
#define GX_TEV_KCSEL_K2_R 0x12
#define GX_TEV_KCSEL_K3_R 0x13
#define GX_TEV_KCSEL_K0_G 0x14
#define GX_TEV_KCSEL_K1_G 0x15
#define GX_TEV_KCSEL_K2_G 0x16
#define GX_TEV_KCSEL_K3_G 0x17
#define GX_TEV_KCSEL_K0_B 0x18
#define GX_TEV_KCSEL_K1_B 0x19
#define GX_TEV_KCSEL_K2_B 0x1A
#define GX_TEV_KCSEL_K3_B 0x1B
#define GX_TEV_KCSEL_K0_A 0x1C
#define GX_TEV_KCSEL_K1_A 0x1D
#define GX_TEV_KCSEL_K2_A 0x1E
#define GX_TEV_KCSEL_K3_A 0x1F

#define GX_TEV_KASEL_1 0x00
#define GX_TEV_KASEL_7_8 0x01
#define GX_TEV_KASEL_3_4 0x02
#define GX_TEV_KASEL_5_8 0x03
#define GX_TEV_KASEL_1_2 0x04
#define GX_TEV_KASEL_3_8 0x05
#define GX_TEV_KASEL_1_4 0x06
#define GX_TEV_KASEL_1_8 0x07
#define GX_TEV_KASEL_K0_R 0x10
#define GX_TEV_KASEL_K1_R 0x11
#define GX_TEV_KASEL_K2_R 0x12
#define GX_TEV_KASEL_K3_R 0x13
#define GX_TEV_KASEL_K0_G 0x14
#define GX_TEV_KASEL_K1_G 0x15
#define GX_TEV_KASEL_K2_G 0x16
#define GX_TEV_KASEL_K3_G 0x17
#define GX_TEV_KASEL_K0_B 0x18
#define GX_TEV_KASEL_K1_B 0x19
#define GX_TEV_KASEL_K2_B 0x1A
#define GX_TEV_KASEL_K3_B 0x1B
#define GX_TEV_KASEL_K0_A 0x1C
#define GX_TEV_KASEL_K1_A 0x1D
#define GX_TEV_KASEL_K2_A 0x1E
#define GX_TEV_KASEL_K3_A 0x1F

/* ------------------------------------------------------------------------ */

#define HSD_TEXP_RAS ((HSD_TExp*) -2)
#define HSD_TEXP_TEX ((HSD_TExp*) -1)
#define HSD_TEXP_ZERO ((HSD_TExp*) 0)

typedef enum _HSD_TEInput {
    HSD_TE_END = 0,
    HSD_TE_RGB = 1,
    HSD_TE_R = 2,
    HSD_TE_G = 3,
    HSD_TE_B = 4,
    HSD_TE_A = 5,
    HSD_TE_X = 6,
    HSD_TE_0 = 7,
    HSD_TE_1 = 8,
    HSD_TE_1_8 = 9,
    HSD_TE_2_8 = 10,
    HSD_TE_3_8 = 11,
    HSD_TE_4_8 = 12,
    HSD_TE_5_8 = 13,
    HSD_TE_6_8 = 14,
    HSD_TE_7_8 = 15,
    HSD_TE_INPUT_MAX = 16,
    HSD_TE_UNDEF = 0xFF
} HSD_TEInput;

typedef enum _HSD_TEType {
    HSD_TE_U8 = 0,
    HSD_TE_U16 = 1,
    HSD_TE_U32 = 2,
    HSD_TE_F32 = 3,
    HSD_TE_F64 = 4,
    HSD_TE_COMP_TYPE_MAX = 5
} HSD_TEType;

typedef enum _HSD_TExpType {
    HSD_TE_ZERO = 0,
    HSD_TE_TEV = 1,
    HSD_TE_TEX = 2,
    HSD_TE_RAS = 3,
    HSD_TE_CNST = 4,
    HSD_TE_IMM = 5,
    HSD_TE_KONST = 6,
    HSD_TE_ALL = 7,
    HSD_TE_TYPE_MAX = 8
} HSD_TExpType;

typedef struct _HSD_TevConf {
    GXTevOp clr_op;
    GXTevColorArg clr_a;
    GXTevColorArg clr_b;
    GXTevColorArg clr_c;
    GXTevColorArg clr_d;
    GXTevScale clr_scale;
    GXTevBias clr_bias;
    u8 clr_clamp;
    GXTevRegID clr_out_reg;
    GXTevOp alpha_op;
    GXTevAlphaArg alpha_a;
    GXTevAlphaArg alpha_b;
    GXTevAlphaArg alpha_c;
    GXTevAlphaArg alpha_d;
    GXTevScale alpha_scale;
    GXTevBias alpha_bias;
    u8 alpha_clamp;
    GXTevRegID alpha_out_reg;
    GXTevClampMode mode;
    GXTevSwapSel ras_swap;
    GXTevSwapSel tex_swap;
    GXTevKColorSel kcsel;
    GXTevKAlphaSel kasel;
    GXTevColorChan swap_red; /* swap table used for ras_swap/tex_swap */
    GXTevColorChan swap_green;
    GXTevColorChan swap_blue;
    GXTevColorChan swap_alpha;
} HSD_TevConf;

typedef struct HSD_TExpRes {
    int failed;
    int texmap;
    int cnst_remain;
    struct {
        u8 color;
        u8 alpha;
    } reg[8];
    u8 c_ref[4];
    u8 a_ref[4];
    u8 c_use[4];
    u8 a_use[4];
} HSD_TExpRes;

typedef struct _HSD_TevDesc {
    struct _HSD_TevDesc* next;
    u32 flags;
    u32 stage;
    u32 coord;
    u32 map;
    u32 color;
    union {
        HSD_TevConf tevconf;
        struct {
            u32 tevmode;
        } tevop;
    } u;
} HSD_TevDesc;

struct _HSD_TExpTevDesc {
    struct _HSD_TevDesc desc;
    HSD_TObj* tobj;
};
typedef struct _HSD_TExpTevDesc HSD_TExpTevDesc;

typedef struct _HSD_TECommon {
    HSD_TExpType type;
    HSD_TExp* next;
} HSD_TECommon;

typedef struct _HSD_TECnst {
    HSD_TExpType type;
    HSD_TExp* next;
    void* val;
    u8 comp;  /* HSD_TEInput */
    u8 ctype; /* HSD_TEType */
    u8 reg;
    u8 idx;
    u8 ref;
    u8 range;
} HSD_TECnst;

typedef struct _HSD_TEArg {
    u8 type;
    u8 sel;
    u8 arg;
    HSD_TExp* exp;
} HSD_TEArg;

typedef struct _HSD_TETev {
    HSD_TExpType type;
    HSD_TExp* next;
    s32 c_ref;
    u8 c_dst;
    u8 c_op;
    u8 c_clamp;
    u8 c_bias;
    u8 c_scale;
    u8 c_range;
    s32 a_ref;
    u8 a_dst;
    u8 a_op;
    u8 a_clamp;
    u8 a_bias;
    u8 a_scale;
    u8 a_range;
    u8 tex_swap;
    u8 ras_swap;
    u8 kcsel;
    u8 kasel;
    GXTevColorChan swap_red;
    GXTevColorChan swap_green;
    GXTevColorChan swap_blue;
    GXTevColorChan swap_alpha;
    HSD_TEArg c_in[4];
    HSD_TEArg a_in[4];
    HSD_TObj* tex;
    u8 chan;
} HSD_TETev;

union HSD_TExp {
    HSD_TExpType type;
    struct _HSD_TECommon comm;
    struct _HSD_TETev tev;
    struct _HSD_TECnst cnst;
};

typedef struct HSD_TExpDag HSD_TExpDag;
struct HSD_TExpDag {
    struct _HSD_TETev* tev;
    u8 idx;
    u8 nb_dep;
    u8 nb_ref;
    u8 dist;
    HSD_TExpDag* depend[8];
};

/* texp.c */
HSD_TExpType HSD_TExpGetType(HSD_TExp* texp);
HSD_TExp* HSD_TExpTev(HSD_TExp** list);
HSD_TExp* HSD_TExpCnst(void* val, HSD_TEInput comp, HSD_TEType type,
                       HSD_TExp** list);
/* Functions still carrying address names in symbols.txt:
 *   fn_801B707C HSD_TExpTev       fn_801B5E40 HSD_TExpOrder
 *   fn_801B6E74 HSD_TExpColorOp   fn_801B64EC HSD_TExpColorIn
 *   fn_801B6CD8 HSD_TExpAlphaOp   fn_801B5F08 HSD_TExpAlphaIn
 *   HSD_TExpFreeList HSD_TExpFreeList  fn_801B45A4 HSD_TExpSetupTev
 *   HSD_TExpRef HSD_TExpRef       HSD_TExpUnref HSD_TExpUnref
 *   fn_801B6DC0 (swap-table setter, no Melee counterpart)
 */
HSD_TExp* HSD_TExpTev(HSD_TExp** list);
void HSD_TExpOrder(HSD_TExp* texp, HSD_TObj* tex, GXChannelID chan);
void HSD_TExpColorOp(HSD_TExp* texp, GXTevOp op, GXTevBias bias,
                 GXTevScale scale, u8 clamp);
void fn_801B6DC0(HSD_TExp* texp, GXTevColorChan red, GXTevColorChan green,
                 GXTevColorChan blue, GXTevColorChan alpha);
void HSD_TExpColorIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a,
                 HSD_TEInput sel_b, HSD_TExp* exp_b, HSD_TEInput sel_c,
                 HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d);
void HSD_TExpAlphaOp(HSD_TExp* texp, GXTevOp op, GXTevBias bias,
                 GXTevScale scale, u8 clamp);
void HSD_TExpAlphaIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a,
                 HSD_TEInput sel_b, HSD_TExp* exp_b, HSD_TEInput sel_c,
                 HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d);
void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tdesc);
HSD_TExp* HSD_TExpFreeList(HSD_TExp* texp_list, HSD_TExpType type, s32 all);
int HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tevdesc,
                    HSD_TExp** texp_list);
void HSD_TExpSetupTev(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp);
void HSD_TExpRef(HSD_TExp* texp, u8 sel);
void HSD_TExpUnref(HSD_TExp* texp, u8 sel);
void HSD_TExpSetReg(HSD_TExp* texp);

/* texpdag.c (HSD_TExpSchedule HSD_TExpSchedule, HSD_TExpSimplify2 HSD_TExpSimplify2) */
int HSD_TExpMakeDag(HSD_TExp* root, HSD_TExpDag* list);
void HSD_TExpSchedule(int num, HSD_TExpDag* list, HSD_TExp** result,
                 HSD_TExpRes* resource);
int HSD_TExpSimplify(HSD_TExp* texp);
int HSD_TExpSimplify2(HSD_TExp* texp);
void fn_801B9048(HSD_TETev* tev);

static inline BOOL IsThroughColor(HSD_TExp* texp)
{
    return texp->tev.c_op == GX_TEV_ADD && texp->tev.c_in[0].sel == HSD_TE_0 &&
           texp->tev.c_in[1].sel == HSD_TE_0 && texp->tev.c_bias == 0 &&
           texp->tev.c_scale == 0;
}

static inline BOOL IsThroughAlpha(HSD_TExp* texp)
{
    return texp->tev.a_op == GX_TEV_ADD && texp->tev.a_in[0].sel == HSD_TE_0 &&
           texp->tev.a_in[1].sel == HSD_TE_0 && texp->tev.a_bias == 0 &&
           texp->tev.a_scale == 0;
}

#endif /* HSD_TEXP_H */
