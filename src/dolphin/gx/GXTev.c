/**
 * @file GXTev.c
 * @brief Dolphin SDK GXTev.c, 0x800BC114 - 0x800BC8F8 (GXSetTevOp through
 *        GXSetNumTevStages), with its .data: the four TEV op preset tables
 *        and GXSetTevOrder's channel-to-raster table (0x80313590 -
 *        0x80313608).
 *
 * Function bodies were matched in the earlier sdk_range_800BB81C,
 * GX_exact_800BC580, sdk_range_800BC618 and GX_exact_800BC8C8 carves and
 * follow zeldaret/tww's GXTev.c.
 */
#include "dolphin/types.h"

typedef union GXStatus_800BB30C {
    /* 0x000 */ u32 word;
    struct {
        /* 0x000 */ u16 field_000;
        /* 0x002 */ u16 field_002;
    } half;
} GXStatus_800BB30C;

typedef enum GXTevAlphaArg_800BB30C {
    GX_TEV_ALPHA_ARG_0,
    GX_TEV_ALPHA_ARG_1,
    GX_TEV_ALPHA_ARG_2,
    GX_TEV_ALPHA_ARG_3,
    GX_TEV_ALPHA_ARG_4,
    GX_TEV_ALPHA_ARG_5,
    GX_TEV_ALPHA_ARG_6,
    GX_TEV_ALPHA_ARG_7,
} GXTevAlphaArg_800BB30C;

typedef enum GXPerf0_800BB30C {
    GX_PERF0_VERTICES,
    GX_PERF0_CLIP_VTX,
    GX_PERF0_CLIP_CLKS,
    GX_PERF0_XF_WAIT_IN,
    GX_PERF0_XF_WAIT_OUT,
    GX_PERF0_XF_XFRM_CLKS,
    GX_PERF0_XF_LIT_CLKS,
    GX_PERF0_XF_BOT_CLKS,
    GX_PERF0_XF_REGLD_CLKS,
    GX_PERF0_XF_REGRD_CLKS,
    GX_PERF0_CLIP_RATIO,
    GX_PERF0_TRIANGLES,
    GX_PERF0_TRIANGLES_CULLED,
    GX_PERF0_TRIANGLES_PASSED,
    GX_PERF0_TRIANGLES_SCISSORED,
    GX_PERF0_TRIANGLES_0TEX,
    GX_PERF0_TRIANGLES_1TEX,
    GX_PERF0_TRIANGLES_2TEX,
    GX_PERF0_TRIANGLES_3TEX,
    GX_PERF0_TRIANGLES_4TEX,
    GX_PERF0_TRIANGLES_5TEX,
    GX_PERF0_TRIANGLES_6TEX,
    GX_PERF0_TRIANGLES_7TEX,
    GX_PERF0_TRIANGLES_8TEX,
    GX_PERF0_TRIANGLES_0CLR,
    GX_PERF0_TRIANGLES_1CLR,
    GX_PERF0_TRIANGLES_2CLR,
    GX_PERF0_QUAD_0CVG,
    GX_PERF0_QUAD_NON0CVG,
    GX_PERF0_QUAD_1CVG,
    GX_PERF0_QUAD_2CVG,
    GX_PERF0_QUAD_3CVG,
    GX_PERF0_QUAD_4CVG,
    GX_PERF0_AVG_QUAD_CNT,
    GX_PERF0_CLOCKS,
    GX_PERF0_NONE,
} GXPerf0_800BB30C;

typedef enum GXPerf1_800BB30C {
    GX_PERF1_TEXELS,
    GX_PERF1_TX_IDLE,
    GX_PERF1_TX_REGS,
    GX_PERF1_TX_MEMSTALL,
    GX_PERF1_TC_CHECK1_2,
    GX_PERF1_TC_CHECK3_4,
    GX_PERF1_TC_CHECK5_6,
    GX_PERF1_TC_CHECK7_8,
    GX_PERF1_TC_MISS,
    GX_PERF1_VC_ELEMQ_FULL,
    GX_PERF1_VC_MISSQ_FULL,
    GX_PERF1_VC_MEMREQ_FULL,
    GX_PERF1_VC_STATUS7,
    GX_PERF1_VC_MISSREP_FULL,
    GX_PERF1_VC_STREAMBUF_LOW,
    GX_PERF1_VC_ALL_STALLS,
    GX_PERF1_VERTICES,
    GX_PERF1_FIFO_REQ,
    GX_PERF1_CALL_REQ,
    GX_PERF1_VC_MISS_REQ,
    GX_PERF1_CP_ALL_REQ,
    GX_PERF1_CLOCKS,
    GX_PERF1_NONE,
} GXPerf1_800BB30C;

typedef struct GXData_800BB30C {
    /* 0x000 */ GXStatus_800BB30C status;
    /* 0x004 */ u8 pad_004[0x78];
    /* 0x07C */ u32 lpSize;
    /* 0x080 */ u32 mtxIdx0;
    /* 0x084 */ u32 mtxIdx1;
    /* 0x088 */ u8 pad_088[0x30];
    /* 0x0B8 */ u32 suTs0[8];
    /* 0x0D8 */ u32 suTs1[8];
    /* 0x0F8 */ u32 scissorTL;
    /* 0x0FC */ u32 scissorBR;
    /* 0x100 */ u32 tref[8];
    /* 0x120 */ u32 iref;
    /* 0x124 */ u32 field_124;
    /* 0x128 */ u32 indTexScale0;
    /* 0x12C */ u32 indTexScale1;
    /* 0x130 */ u32 tevColorEnv[16];
    /* 0x170 */ u32 tevAlphaEnv[16];
    /* 0x1B0 */ u32 field_1B0[8];
    /* 0x1D0 */ u32 field_1D0;
    /* 0x1D4 */ u32 dstAlpha;
    /* 0x1D8 */ u32 zMode;
    /* 0x1DC */ u32 field_1DC;
    /* 0x1E0 */ u32 field_1E0;
    /* 0x1E4 */ u32 field_1E4;
    /* 0x1E8 */ u32 field_1E8;
    /* 0x1EC */ u32 field_1EC;
    /* 0x1F0 */ u32 field_1F0;
    /* 0x1F4 */ u32 field_1F4;
    /* 0x1F8 */ u32 field_1F8;
    /* 0x1FC */ u32 field_1FC;
    /* 0x200 */ u8 field_200;
    /* 0x201 */ u8 pad_201[3];
    /* 0x204 */ u32 genMode;
    /* 0x208 */ u8 pad_208[0x218];
    /* 0x420 */ u32 field_420;
    /* 0x424 */ f32 field_424;
    /* 0x428 */ f32 field_428;
    /* 0x42C */ f32 field_42C;
    /* 0x430 */ f32 field_430;
    /* 0x434 */ f32 field_434;
    /* 0x438 */ f32 field_438;
    /* 0x43C */ f32 projection[6];
    /* 0x454 */ u8 pad_454[0x8];
    /* 0x45C */ u32 texMapSize[8];
    /* 0x47C */ u32 texMapWrap[8];
    /* 0x49C */ u32 texmapId[16];
    /* 0x4DC */ u32 tcsManEnab;
    /* 0x4E0 */ u32 tevTcEnab;
    /* 0x4E4 */ GXPerf0_800BB30C perf0;
    /* 0x4E8 */ GXPerf1_800BB30C perf1;
    /* 0x4EC */ u32 perfSel;
    /* 0x4F0 */ u8 pad_4F0[4];
    /* 0x4F4 */ u32 dirtyState;
} GXData_800BB30C;

#define field_002 status.half.field_002

extern GXData_800BB30C* const gx;
extern volatile u16* __cpReg;
extern u32 lbl_80313608[];

extern void GXSetTevIndirect(u32 dstCoord, u32 func, u32 srcParam, u32 mtx,
                        u32 normalize, u32 postMtx, u32 normalizeColor,
                        u8 bias, u8 arg8, u32 arg9);
extern void GXSetViewportJitter(f32 left, f32 top, f32 width, f32 height, f32 nearz,
                        f32 farz, u32 field);
extern void __GXSetMatrixIndex(s32 value);
extern void fn_800BE164(u32* hi, u32* lo);
extern void __GXSetDirtyState(void);
extern void __GXSendFlushPrim(void);
extern u32 __cvt_fp2unsigned(f32 value);
extern s32 TRKReleaseBuffer(s32 bufferIndex);

typedef struct TRKEvent {
    /* 0x00 */ s32 type;
    /* 0x04 */ s32 unused;
    /* 0x08 */ s32 bufferIndex;
} TRKEvent;

typedef union PPCWGPipe_800BB30C {
    u8 u8;
    u16 u16;
    u32 u32;
    f32 f32;
} PPCWGPipe_800BB30C;

volatile PPCWGPipe_800BB30C GXWGFifo_800BB30C : 0xCC008000;

#define GX_FIFO_U8  GXWGFifo_800BB30C.u8
#define GX_FIFO_U16 GXWGFifo_800BB30C.u16
#define GX_FIFO_U32 GXWGFifo_800BB30C.u32
#define GX_FIFO_F32 GXWGFifo_800BB30C.f32

#define GX_BP_REG(reg)     \
    GX_FIFO_U8 = 0x61;     \
    GX_FIFO_U32 = (reg)

#define GX_XF_REG(addr, reg)          \
    GX_FIFO_U8 = 0x10;                \
    GX_FIFO_U32 = 0x1000 + (addr);    \
    GX_FIFO_U32 = (reg)

#define GX_CP_REG(addr, reg) \
    GX_FIFO_U8 = 8;          \
    GX_FIFO_U8 = (addr);     \
    GX_FIFO_U32 = (reg)


static u32 TEVCOpTableST0[5] = { /* 0x80313590 */
    0xC008F8AF, 0xC008A89F, 0xC008AC8F, 0xC008FFF8, 0xC008FFFA,
};
static u32 TEVCOpTableST1[5] = { /* 0x803135A4 */
    0xC008F80F, 0xC008089F, 0xC0080C8F, 0xC008FFF8, 0xC008FFF0,
};
static u32 TEVAOpTableST0[5] = { /* 0x803135B8 */
    0xC108F2F0, 0xC108FFD0, 0xC108F2F0, 0xC108FFC0, 0xC108FFD0,
};
static u32 TEVAOpTableST1[5] = { /* 0x803135CC */
    0xC108F070, 0xC108FF80, 0xC108F070, 0xC108FFC0, 0xC108FF80,
};


void GXSetTevOp(s32 stage, u32 mode) {
    u32* color;
    u32* alpha;
    u32 reg;

    if (stage == 0) {
        color = TEVCOpTableST0 + mode;
        alpha = TEVAOpTableST0 + mode;
    } else {
        color = TEVCOpTableST1 + mode;
        alpha = TEVAOpTableST1 + mode;
    }

    reg = gx->tevColorEnv[stage];
    reg = (*color & ~0xFF000000) | (reg & 0xFF000000);
    GX_BP_REG(reg);
    gx->tevColorEnv[stage] = reg;

    reg = gx->tevAlphaEnv[stage];
    reg = (*alpha & ~0xFF00000F) | (reg & 0xFF00000F);
    GX_BP_REG(reg);
    gx->tevAlphaEnv[stage] = reg;
    gx->field_002 = 0;
}

void GXSetTevColorIn(u32 stage, u32 a, u32 b, u32 c, u32 d) {
    GXData_800BB30C* p = gx;
    u32 reg = p->tevColorEnv[stage];

    reg = __rlwimi(reg, a, 12, 16, 19);
    reg = __rlwimi(reg, b, 8, 20, 23);
    reg = __rlwimi(reg, c, 4, 24, 27);
    reg = __rlwimi(reg, d, 0, 28, 31);
    GX_BP_REG(reg);
    p->tevColorEnv[stage] = reg;
    p->field_002 = 0;
}

void GXSetTevAlphaIn(u32 stage, GXTevAlphaArg_800BB30C a,
                 GXTevAlphaArg_800BB30C b, GXTevAlphaArg_800BB30C c,
                 GXTevAlphaArg_800BB30C d) {
    GXData_800BB30C* p = gx;
    u32 reg = p->tevAlphaEnv[stage];

    reg = __rlwimi(reg, a, 13, 16, 18);
    reg = __rlwimi(reg, b, 10, 19, 21);
    reg = __rlwimi(reg, c, 7, 22, 24);
    reg = __rlwimi(reg, d, 4, 25, 27);
    GX_BP_REG(reg);
    p->tevAlphaEnv[stage] = reg;
    p->field_002 = 0;
}

void GXSetTevColorOp(u32 stage, s32 op, u32 bias, u32 scale, u32 clamp,
                 u32 outReg) {
    u32 reg;

    reg = gx->tevColorEnv[stage];
    reg = __rlwimi(reg, op & 1, 18, 13, 13);
    if (op <= 1) {
        reg = __rlwimi(reg, scale, 20, 10, 11);
        reg = __rlwimi(reg, bias, 16, 14, 15);
    } else {
        reg = __rlwimi(reg, (op >> 1) & 3, 20, 10, 11);
        reg = __rlwimi(reg, 3, 16, 14, 15);
    }
    reg = __rlwimi(reg, clamp & 0xFF, 19, 12, 12);
    reg = __rlwimi(reg, outReg, 22, 8, 9);
    GX_BP_REG(reg);
    gx->tevColorEnv[stage] = reg;
    gx->field_002 = 0;
}

void GXSetTevAlphaOp(u32 stage, s32 op, u32 bias, u32 scale, u32 clamp,
                 u32 outReg) {
    u32 reg;

    reg = gx->tevAlphaEnv[stage];
    reg = __rlwimi(reg, op & 1, 18, 13, 13);
    if (op <= 1) {
        reg = __rlwimi(reg, scale, 20, 10, 11);
        reg = __rlwimi(reg, bias, 16, 14, 15);
    } else {
        reg = __rlwimi(reg, (op >> 1) & 3, 20, 10, 11);
        reg = __rlwimi(reg, 3, 16, 14, 15);
    }
    reg = __rlwimi(reg, clamp & 0xFF, 19, 12, 12);
    reg = __rlwimi(reg, outReg, 22, 8, 9);
    GX_BP_REG(reg);
    gx->tevAlphaEnv[stage] = reg;
    gx->field_002 = 0;
}

typedef struct GXColor_800BC2F8 {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor_800BC2F8;

typedef struct GXColorS10_800BC36C {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10_800BC36C;

typedef struct GXFogAdjTable_800BCCDC {
    u16 r[10];
} GXFogAdjTable_800BCCDC;


void GXSetTevColor(u32 id, GXColor_800BC2F8 color) {
    u32 reg0;
    u32 reg1;

    reg0 = 0;
    reg0 = (reg0 & ~0xFFU) | color.r;
    reg0 = (reg0 & ~0xFF000U) | (color.a << 12);
    reg0 = (reg0 & 0xFFFFFFU) | ((id * 2 + 0xE0) << 24);

    reg1 = 0;
    reg1 = (reg1 & ~0xFFU) | color.b;
    reg1 = (reg1 & ~0xFF000U) | (color.g << 12);
    reg1 = (reg1 & 0xFFFFFFU) | ((id * 2 + 0xE1) << 24);

    GX_BP_REG(reg0);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    gx->field_002 = 0;
}

void GXSetTevColorS10(u32 id, GXColorS10_800BC36C color) {
    u32 reg0;
    u32 reg1;

    reg0 = 0;
    reg0 = (reg0 & ~0x7FFU) | (color.r & 0x7FF);
    reg0 = (reg0 & ~0x7FF000U) | ((color.a & 0x7FF) << 12);
    reg0 = (reg0 & 0xFFFFFFU) | ((id * 2 + 0xE0) << 24);

    reg1 = 0;
    reg1 = (reg1 & ~0x7FFU) | (color.b & 0x7FF);
    reg1 = (reg1 & ~0x7FF000U) | ((color.g & 0x7FF) << 12);
    reg1 = (reg1 & 0xFFFFFFU) | ((id * 2 + 0xE1) << 24);

    GX_BP_REG(reg0);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    gx->field_002 = 0;
}

void GXSetTevKColor(u32 id, GXColor_800BC2F8 color) {
    u32 reg0;
    u32 reg1;

    reg0 = 0;
    reg0 = (reg0 & ~0xFFU) | color.r;
    reg0 = (reg0 & ~0xFF000U) | (color.a << 12);
    reg0 = (reg0 & ~0xF00000U) | 0x800000U;
    reg0 = (reg0 & 0xFFFFFFU) | ((id * 2 + 0xE0) << 24);

    reg1 = 0;
    reg1 = (reg1 & ~0xFFU) | color.b;
    reg1 = (reg1 & ~0xFF000U) | (color.g << 12);
    reg1 = (reg1 & ~0xF00000U) | 0x800000U;

    reg1 = (reg1 & 0xFFFFFFU) | ((id * 2 + 0xE1) << 24);
    GX_BP_REG(reg0);
    GX_BP_REG(reg1);
    gx->field_002 = 0;
}

void GXSetTevKColorSel(s32 stage, u32 value) {
    GXData_800BB30C* p = gx;
    u32* reg = &p->field_1B0[stage >> 1];

    if (stage & 1) {
        *reg = (*reg & ~0x7C000U) | (value << 14);
    } else {
        *reg = (*reg & ~0x1F0U) | (value << 4);
    }

    GX_BP_REG(*reg);
    gx->field_002 = 0;
}

void GXSetTevKAlphaSel(s32 stage, u32 value) {
    GXData_800BB30C* p = gx;
    u32* reg = &p->field_1B0[stage >> 1];

    if (stage & 1) {
        *reg = (*reg & ~0xF80000U) | (value << 19);
    } else {
        *reg = (*reg & ~0x3E00U) | (value << 9);
    }

    GX_BP_REG(*reg);
    gx->field_002 = 0;
}

void GXSetTevSwapMode(u32 stage, u32 rasSel, u32 texSel) {
    GXData_800BB30C* p = gx;
    u32* reg = &p->tevAlphaEnv[stage];

    *reg = (*reg & ~3U) | rasSel;
    *reg = (*reg & ~0xCU) | (texSel << 2);
    GX_BP_REG(*reg);
    p->field_002 = 0;
}

void GXSetTevSwapModeTable(u32 table, u32 red, u32 green, u32 blue, u32 alpha) {
    u32 index = table * 2;
    GXData_800BB30C* p = gx;
    u32* reg0 = &p->field_1B0[index];
    u32* reg1;

    *reg0 = (*reg0 & ~3U) | red;
    *reg0 = (*reg0 & ~0xCU) | (green << 2);
    GX_BP_REG(*reg0);
    reg1 = &p->field_1B0[index + 1];

    *reg1 = (*reg1 & ~3U) | blue;
    *reg1 = (*reg1 & ~0xCU) | (alpha << 2);
    GX_BP_REG(*reg1);
    p->field_002 = 0;
}

void GXSetAlphaCompare(u32 comp0, u8 ref0, u32 op, u32 comp1, u8 ref1) {
    u32 reg = ref0;

    reg |= 0xF3000000U;
    reg = (reg & ~0xFF00U) | (ref1 << 8);
    reg = (reg & ~0x70000U) | (comp0 << 16);
    reg = (reg & ~0x380000U) | (comp1 << 19);
    reg = (reg & ~0xC00000U) | (op << 22);
    GX_BP_REG(reg);
    gx->field_002 = 0;
}

#define ZTEX_SET_REG(field, pos, size, value) \
    (field) = ((field) & ~(((1 << (size)) - 1) << (31 - (pos) - (size) + 1))) | \
              ((int)(value) << (31 - (pos) - (size) + 1))

/* GXSetZTexture */
void GXSetZTexture(int op, int format, u32 bias) {
    u32 val1;
    u32 val2;
    u32 val3;

    val1 = 0;
    ZTEX_SET_REG(val1, 8, 24, bias);
    ZTEX_SET_REG(val1, 0, 8, 0xF4);

    val2 = 0;
    switch (format) {
    case 0x11:
        val3 = 0;
        break;
    case 0x13:
        val3 = 1;
        break;
    case 0x16:
        val3 = 2;
        break;
    default:
        val3 = 2;
        break;
    }

    ZTEX_SET_REG(val2, 30, 2, val3);
    ZTEX_SET_REG(val2, 28, 2, op);
    ZTEX_SET_REG(val2, 0, 8, 0xF5);

    GX_BP_REG(val1);
    GX_BP_REG(val2);

    gx->field_002 = 0;
}

#define TEV_SET_REG(field, pos, size, value) \
    (field) = ((field) & ~(((1 << (size)) - 1) << (31 - (pos) - (size) + 1))) | \
              ((int)(value) << (31 - (pos) - (size) + 1))

/* GXSetTevOrder */
void GXSetTevOrder(int stage, int coord, int map, int color)
{
    static int c2r[] = {0, 1, 0, 1, 0, 1, 7, 5, 6};

    u32* reg;
    u32 tempMap;
    u32 tempCoord;

    reg = &gx->tref[stage / 2];
    gx->texmapId[stage] = map;

    tempMap = map & ~0x100;
    tempMap = (tempMap >= 8) ? 0 : tempMap;

    if (coord >= 8) {
        tempCoord = 0;
        gx->tevTcEnab = gx->tevTcEnab & ~(1 << stage);
    } else {
        tempCoord = coord;
        gx->tevTcEnab = gx->tevTcEnab | (1 << stage);
    }

    if (stage & 1) {
        TEV_SET_REG(*reg, 17, 3, tempMap);
        TEV_SET_REG(*reg, 14, 3, tempCoord);
        TEV_SET_REG(*reg, 10, 3, (color == 0xFF ? 7 : c2r[color]));
        TEV_SET_REG(*reg, 13, 1, ((map != 0xFF) && !(map & 0x100)));
    } else {
        TEV_SET_REG(*reg, 29, 3, tempMap);
        TEV_SET_REG(*reg, 26, 3, tempCoord);
        TEV_SET_REG(*reg, 22, 3, (color == 0xFF ? 7 : c2r[color]));
        TEV_SET_REG(*reg, 25, 1, ((map != 0xFF) && !(map & 0x100)));
    }

    GX_BP_REG(*reg);
    gx->field_002 = 0;
    gx->dirtyState |= 1;
}

void GXSetNumTevStages(u32 nStages) {
    GXData_800BB30C* data = gx;

    data->genMode = (data->genMode & ~0x3C00U) |
                    (((nStages & 0xFF) - 1) << 10);
    data->dirtyState |= 4;
}
