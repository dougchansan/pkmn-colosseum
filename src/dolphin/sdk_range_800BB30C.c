/**
 * @file sdk_range_800BB30C.c
 * @brief dolphin-sdk code, 0x800BB30C - 0x800BE464 (68 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
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

extern GXData_800BB30C* gx;
extern volatile u16* __cpReg;
extern u32 lbl_80313590[];
extern u32 lbl_80313608[];

extern void fn_800BB780(u32 dstCoord, u32 func, u32 srcParam, u32 mtx,
                        u32 normalize, u32 postMtx, u32 normalizeColor,
                        u8 bias, u8 arg8, u32 arg9);
extern void fn_800BD640(f32 left, f32 top, f32 width, f32 height, f32 nearz,
                        f32 farz, u32 field);
extern void __GXSetMatrixIndex(s32 value);
extern void fn_800BE164(u32* hi, u32* lo);
extern void fn_800B91EC(void);
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

#if defined(SDK_800BC618_SUFFIX_ACTIVE)
volatile PPCWGPipe_800BB30C GXWGFifo_800BB30C : 0xCC008000;

#define GX_FIFO_U8  GXWGFifo_800BB30C.u8
#define GX_FIFO_U16 GXWGFifo_800BB30C.u16
#define GX_FIFO_U32 GXWGFifo_800BB30C.u32
#define GX_FIFO_F32 GXWGFifo_800BB30C.f32
#else
#define GX_FIFO_U8  (*(volatile u8*)0xCC008000)
#define GX_FIFO_U16 (*(volatile u16*)0xCC008000)
#define GX_FIFO_U32 (*(volatile u32*)0xCC008000)
#define GX_FIFO_F32 (*(volatile f32*)0xCC008000)
#endif

#define GX_BP_REG(reg)       \
    do {                     \
        GX_FIFO_U8 = 0x61;   \
        GX_FIFO_U32 = (reg); \
    } while (0)

#define GX_XF_REG(addr, reg)       \
    do {                           \
        GX_FIFO_U8 = 0x10;         \
        GX_FIFO_U32 = 0x1000 + (addr); \
        GX_FIFO_U32 = (reg);       \
    } while (0)

#define GX_CP_REG(addr, reg) \
    do {                     \
        GX_FIFO_U8 = 8;      \
        GX_FIFO_U8 = (addr); \
        GX_FIFO_U32 = (reg); \
    } while (0)

#if defined(SDK_800BB30C_PREFIX_ACTIVE)
void fn_800BB30C(u32 texMap, u32 texCoord) {
    GXData_800BB30C* p = gx;
    u32 size = p->texMapSize[texMap];
    u32 wrap = p->texMapWrap[texMap];

    p->suTs0[texCoord] = (p->suTs0[texCoord] & 0xFFFF0000U) | (size & 0x3FF);
    p->suTs1[texCoord] = (p->suTs1[texCoord] & 0xFFFF0000U) | ((size >> 10) & 0x3FF);
    p->suTs0[texCoord] = (p->suTs0[texCoord] & ~0x10000U) | (((wrap & 3) == 1) << 16);
    p->suTs1[texCoord] = (p->suTs1[texCoord] & ~0x10000U) | ((((wrap >> 2) & 3) == 1) << 16);

    GX_BP_REG(p->suTs0[texCoord]);
    GX_BP_REG(p->suTs1[texCoord]);
    p->field_002 = 0;
}

void __GXSetSUTexRegs(void) {
    extern void fn_800BB30C(u32 texMap, u32 texCoord);
    u32 nStages;
    u32 nIndStages;
    u32 i;
    u32 map;
    u32 texMap;
    u32 texCoord;
    u32* tref;

    if (gx->tcsManEnab != 0xFF) {
        nStages = ((gx->genMode >> 10) & 0xF) + 1;
        nIndStages = (gx->genMode >> 16) & 7;
        for (i = 0; i < nIndStages; i++) {
            switch (i) {
            case 0:
                texMap = gx->iref & 7;
                texCoord = (gx->iref >> 3) & 7;
                break;
            case 1:
                texMap = (gx->iref >> 6) & 7;
                texCoord = (gx->iref >> 9) & 7;
                break;
            case 2:
                texMap = (gx->iref >> 12) & 7;
                texCoord = (gx->iref >> 15) & 7;
                break;
            case 3:
                texMap = (gx->iref >> 18) & 7;
                texCoord = (gx->iref >> 21) & 7;
                break;
            }
            if (!(gx->tcsManEnab & (1 << texCoord))) {
                fn_800BB30C(texMap, texCoord);
            }
        }

        for (i = 0; i < nStages; i++) {
            tref = &gx->tref[i / 2];
            map = gx->texmapId[i];
            texMap = map & ~0x100U;
            if (i & 1) {
                texCoord = (*tref >> 15) & 7;
            } else {
                texCoord = (*tref >> 3) & 7;
            }
            if (texMap != 0xFF &&
                !(gx->tcsManEnab & (1 << texCoord)) &&
                (gx->tevTcEnab & (1 << i))) {
                fn_800BB30C(texMap, texCoord);
            }
        }
    }
}

void __GXSetTmemConfig(u32 config) {
    switch (config) {
    case 2:
        GX_BP_REG(0x8C0D8000);
        GX_BP_REG(0x900DC000);
        GX_BP_REG(0x8D0D8800);
        GX_BP_REG(0x910DC800);
        GX_BP_REG(0x8E0D9000);
        GX_BP_REG(0x920DD000);
        GX_BP_REG(0x8F0D9800);
        GX_BP_REG(0x930DD800);
        GX_BP_REG(0xAC0DA000);
        GX_BP_REG(0xB00DC400);
        GX_BP_REG(0xAD0DA800);
        GX_BP_REG(0xB10DCC00);
        GX_BP_REG(0xAE0DB000);
        GX_BP_REG(0xB20DD400);
        GX_BP_REG(0xAF0DB800);
        GX_BP_REG(0xB30DDC00);
        break;
    case 1:
        GX_BP_REG(0x8C0D8000);
        GX_BP_REG(0x900DC000);
        GX_BP_REG(0x8D0D8800);
        GX_BP_REG(0x910DC800);
        GX_BP_REG(0x8E0D9000);
        GX_BP_REG(0x920DD000);
        GX_BP_REG(0x8F0D9800);
        GX_BP_REG(0x930DD800);
        GX_BP_REG(0xAC0DA000);
        GX_BP_REG(0xB00DE000);
        GX_BP_REG(0xAD0DA800);
        GX_BP_REG(0xB10DE800);
        GX_BP_REG(0xAE0DB000);
        GX_BP_REG(0xB20DF000);
        GX_BP_REG(0xAF0DB800);
        GX_BP_REG(0xB30DF800);
        break;
    default:
        GX_BP_REG(0x8C0D8000);
        GX_BP_REG(0x900DC000);
        GX_BP_REG(0x8D0D8400);
        GX_BP_REG(0x910DC400);
        GX_BP_REG(0x8E0D8800);
        GX_BP_REG(0x920DC800);
        GX_BP_REG(0x8F0D8C00);
        GX_BP_REG(0x930DCC00);
        GX_BP_REG(0xAC0D9000);
        GX_BP_REG(0xB00DD000);
        GX_BP_REG(0xAD0D9400);
        GX_BP_REG(0xB10DD400);
        GX_BP_REG(0xAE0D9800);
        GX_BP_REG(0xB20DD800);
        GX_BP_REG(0xAF0D9C00);
        GX_BP_REG(0xB30DDC00);
        break;
    }
}

void fn_800BB780(u32 dstCoord, u32 func, u32 srcParam, u32 mtx,
                 u32 normalize, u32 postMtx, u32 normalizeColor,
                 u8 bias, u8 arg8, u32 arg9) {
    GXData_800BB30C* p = gx;
    u32 command;
    u32 value = func;

    value = (value & ~0xCU) | (srcParam << 2);
    value = (value & ~0x70U) | (mtx << 4);
    value = (value & ~0x180U) | (arg9 << 7);
    value = (value & ~0x1E00U) | (normalize << 9);
    value = (value & ~0xE000U) | (postMtx << 13);
    dstCoord += 0x10;
    value = (value & ~0x70000U) | (normalizeColor << 16);
    value = (value & ~0x80000U) | (arg8 << 19);
    value = (value & ~0x100000U) | (bias << 20);

    command = value & 0xFFFFFF;
    command |= dstCoord << 24;
    GX_BP_REG(command);
    p->field_002 = 0;
}

void fn_800BBC0C(u32 nChans) {
    GXData_800BB30C* p = gx;

    p->genMode = (p->genMode & ~0x70000U) | ((nChans & 0xFF) << 16);
    p->dirtyState |= 6;
}

void fn_800BBC34(u32 dstCoord) {
    fn_800BB780(dstCoord, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void fn_800BBC7C(u32 dstCoord, u32 func, u8 normalize, u8 color, u32 postMtx) {
    u32 colorSel = color != 0 ? 6 : 0;

    fn_800BB780(dstCoord, func, 0, normalize != 0 ? 7 : 0, postMtx, colorSel, colorSel, 0, 0, 0);
}

void fn_800BBF98(u32 dstCoord, u32 func, u32 normalize) {
    fn_800BB780(dstCoord, func, 0, 7, normalize, 0, 0, 0, 0, 0);
}

void fn_800BBFDC(u32 dstCoord) {
    fn_800BB780(dstCoord, 0, 0, 0, 0, 6, 6, 1, 0, 0);
}

void fn_800BC024(void) {
    u32 nIndStages;
    u32 i;
    u32 texMap;
    u32 mask = 0;

    nIndStages = (gx->genMode >> 16) & 7;
    for (i = 0; i < nIndStages; i++) {
        switch (i) {
        case 0:
            texMap = gx->iref & 7;
            break;
        case 1:
            texMap = (gx->iref >> 6) & 7;
            break;
        case 2:
            texMap = (gx->iref >> 12) & 7;
            break;
        case 3:
            texMap = (gx->iref >> 18) & 7;
            break;
        }
        mask |= 1 << texMap;
    }

    if ((gx->field_124 & 0xFF) != mask) {
        gx->field_124 = (gx->field_124 & ~0xFFU) | mask;
        GX_BP_REG(gx->field_124);
        gx->field_002 = 0;
    }
}

#pragma dont_inline on
#pragma peephole off
void __GXFlushTextureState(void) {
    GXData_800BB30C* p = gx;

    GX_BP_REG(p->field_124);
    p->field_002 = 0;
}
#pragma peephole on
#pragma dont_inline reset

void GXSetTevOp(s32 stage, u32 mode) {
    u32* color;
    u32* alpha;
    u32* table = lbl_80313590;
    GXData_800BB30C* p;
    u32 colorReg;
    u32 alphaReg;

    if (stage == 0) {
        color = &table[mode];
        alpha = color + 10;
    } else {
        u32* base = &table[mode];
        color = base + 5;
        alpha = base + 15;
    }

    p = gx;
    colorReg = p->tevColorEnv[stage];
    colorReg = (colorReg & 0xFF000000) | (*color & 0xFFFFFF);
    GX_BP_REG(colorReg);
    p->tevColorEnv[stage] = colorReg;

    alphaReg = p->tevAlphaEnv[stage];
    alphaReg = (alphaReg & 0xFF00000F) | (*alpha & 0x00FFFFF0);
    GX_BP_REG(alphaReg);
    p->tevAlphaEnv[stage] = alphaReg;
    p->field_002 = 0;
}

void fn_800BC1A0(u32 stage, u32 a, u32 b, u32 c, u32 d) {
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

void fn_800BC1E4(u32 stage, GXTevAlphaArg_800BB30C a,
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

void fn_800BC228(u32 stage, s32 op, u32 bias, u32 scale, u32 clamp,
                 u32 outReg) {
    u32 reg;
    GXData_800BB30C* p;

    reg = gx->tevColorEnv[stage];
    reg = __rlwimi(reg, op, 18, 13, 13);
    if (op <= 1) {
        reg = __rlwimi(reg, scale, 20, 10, 11);
        reg = __rlwimi(reg, bias, 16, 14, 15);
    } else {
        reg = __rlwimi(reg, op, 19, 10, 11);
        reg = __rlwimi(reg, 3, 16, 14, 15);
    }
    reg = __rlwimi(reg, clamp, 19, 12, 12);
    reg = __rlwimi(reg, outReg, 22, 8, 9);
    GX_FIFO_U8 = 0x61;
    p = gx;
    GX_FIFO_U32 = reg;
    p->tevColorEnv[stage] = reg;
    p->field_002 = 0;
}

void fn_800BC290(u32 stage, s32 op, u32 bias, u32 scale, u32 clamp,
                 u32 outReg) {
    u32 reg;
    GXData_800BB30C* p;

    reg = gx->tevAlphaEnv[stage];
    reg = __rlwimi(reg, op, 18, 13, 13);
    if (op <= 1) {
        reg = __rlwimi(reg, scale, 20, 10, 11);
        reg = __rlwimi(reg, bias, 16, 14, 15);
    } else {
        reg = __rlwimi(reg, op, 19, 10, 11);
        reg = __rlwimi(reg, 3, 16, 14, 15);
    }
    reg = __rlwimi(reg, clamp, 19, 12, 12);
    reg = __rlwimi(reg, outReg, 22, 8, 9);
    GX_FIFO_U8 = 0x61;
    p = gx;
    GX_FIFO_U32 = reg;
    p->tevAlphaEnv[stage] = reg;
    p->field_002 = 0;
}
#endif

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

#if defined(SDK_800BB30C_PREFIX_ACTIVE)
void fn_800BC2F8(u32 id, GXColor_800BC2F8 color) {
    u32 reg0;
    u32 reg1;
    GXData_800BB30C* p;

    reg0 = 0;
    reg0 = (reg0 & ~0xFFU) | color.r;
    reg0 = (reg0 & ~0xFF000U) | (color.a << 12);
    reg0 = (reg0 & 0xFFFFFFU) | ((id * 2 + 0xE0) << 24);

    p = gx;
    reg1 = 0;
    reg1 = (reg1 & ~0xFFU) | color.b;
    reg1 = (reg1 & ~0xFF000U) | (color.g << 12);
    reg1 = (reg1 & 0xFFFFFFU) | ((id * 2 + 0xE1) << 24);

    GX_BP_REG(reg0);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    p->field_002 = 0;
}

void fn_800BC36C(u32 id, GXColorS10_800BC36C color) {
    GXData_800BB30C* p;
    u32 reg0;
    u32 reg1;

    reg0 = 0;
    reg0 = (reg0 & ~0x7FFU) | (color.r & 0x7FF);
    reg0 = (reg0 & ~0x7FF000U) | ((color.a << 12) & 0x7FF000U);
    reg0 = (reg0 & 0xFFFFFFU) | ((id * 2 + 0xE0) << 24);

    p = gx;
    reg1 = 0;
    reg1 = (reg1 & ~0x7FFU) | (color.b & 0x7FF);
    reg1 = (reg1 & ~0x7FF000U) | ((color.g << 12) & 0x7FF000U);

    GX_BP_REG(reg0);
    reg1 = (reg1 & 0xFFFFFFU) | ((id * 2 + 0xE1) << 24);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    GX_BP_REG(reg1);
    p->field_002 = 0;
}

void fn_800BC3E0(u32 id, GXColor_800BC2F8 color) {
    u32 reg0;
    u32 reg1;
    GXData_800BB30C* p;

    reg0 = 0;
    reg0 = (reg0 & ~0xFFU) | color.r;
    reg0 = (reg0 & ~0xFF000U) | (color.a << 12);
    reg0 = (reg0 & ~0xF00000U) | 0x800000U;
    reg0 = (reg0 & 0xFFFFFFU) | ((id * 2 + 0xE0) << 24);

    p = gx;
    reg1 = 0;
    reg1 = (reg1 & ~0xFFU) | color.b;
    reg1 = (reg1 & ~0xFF000U) | (color.g << 12);
    reg1 = (reg1 & ~0xF00000U) | 0x800000U;

    GX_BP_REG(reg0);
    reg1 = (reg1 & 0xFFFFFFU) | ((id * 2 + 0xE1) << 24);
    GX_BP_REG(reg1);
    p->field_002 = 0;
}

void fn_800BC454(s32 stage, u32 value) {
    GXData_800BB30C* p = gx;
    u32* reg = &p->field_1B0[stage >> 1];

    if (stage & 1) {
        *reg = (*reg & ~0x7C000U) | (value << 14);
    } else {
        *reg = (*reg & ~0x1F0U) | (value << 4);
    }

    GX_BP_REG(*reg);
    p->field_002 = 0;
}

void fn_800BC4C0(s32 stage, u32 value) {
    GXData_800BB30C* p = gx;
    u32* reg = &p->field_1B0[stage >> 1];

    if (stage & 1) {
        *reg = (*reg & ~0xF80000U) | (value << 19);
    } else {
        *reg = (*reg & ~0x3E00U) | (value << 9);
    }

    GX_BP_REG(*reg);
    p->field_002 = 0;
}

void fn_800BC52C(u32 stage, u32 rasSel, u32 texSel) {
    GXData_800BB30C* p = gx;
    u32* reg = &p->tevAlphaEnv[stage];

    *reg = (*reg & ~3U) | rasSel;
    *reg = (*reg & ~0xCU) | (texSel << 2);
    GX_BP_REG(*reg);
    p->field_002 = 0;
}
#endif

#if defined(SDK_EXACT_800BC580)
void fn_800BC580(u32 table, u32 red, u32 green, u32 blue, u32 alpha) {
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
#endif

#if defined(SDK_800BC618_SUFFIX_ACTIVE)
void fn_800BC618(u32 comp0, u8 ref0, u32 op, u32 comp1, u8 ref1) {
    u32 reg = ref0;
    GXData_800BB30C* p;

    reg |= 0xF3000000U;
    reg = (reg & ~0xFF00U) | (ref1 << 8);
    reg = (reg & ~0x70000U) | (comp0 << 16);
    reg = (reg & ~0x380000U) | (comp1 << 19);
    reg = (reg & ~0xC00000U) | (op << 22);
    p = gx;
    GX_BP_REG(reg);
    p->field_002 = 0;
}

void fn_800BC66C(u32 op, u32 format, u32 bias) {
    u32 zFormat;
    u32 reg;
    GXData_800BB30C* p;

    reg = (bias & 0xFFFFFFU) | 0xF4000000U;
    switch (format) {
    case 0x11:
        zFormat = 0;
        break;
    case 0x13:
        zFormat = 1;
        break;
    case 0x16:
        zFormat = 2;
        break;
    default:
        zFormat = 2;
        break;
    }

    p = gx;
    GX_BP_REG(reg);
    reg = (zFormat & 3) | (op << 2);
    reg = (reg & 0xFFFFFFU) | 0xF5000000U;
    GX_BP_REG(reg);
    p->field_002 = 0;
}

void fn_800BC8C8(u32 nStages) {
    GXData_800BB30C* p = gx;

    p->genMode = (p->genMode & ~0x3C00U) | (((nStages & 0xFF) - 1) << 10);
    p->dirtyState |= 4;
}

void fn_800BCCDC(u8 enable, u16 center, GXFogAdjTable_800BCCDC* table) {
    GXData_800BB30C* p;
    u16* values;
    u32 i;
    u32 reg;

    if (enable) {
        values = table->r;
        for (i = 0; i < 5; i++, values += 2) {
            reg = values[1] << 12;
            reg = __rlwimi(reg, values[0], 0, 20, 31);
            reg = (reg & 0xFFFFFFU) | ((0xE9 + i) << 24);
            GX_BP_REG(reg);
        }
    }

    reg = center + 342;
    p = gx;
    reg = (reg & ~0x400U) | (enable << 10);
    GX_BP_REG((reg & 0xFFFFFFU) | 0xE8000000U);
    p->field_002 = 0;
}

void fn_800BCEBC(u32 value) {
    GXData_800BB30C* p = gx;

    p->field_1DC = (p->field_1DC & ~0x40U) | ((value & 0xFF) << 6);
    GX_BP_REG(p->field_1DC);
    p->field_002 = 0;
}

void fn_800BD0F8(void) {}

void GXCallDisplayList(void* list, u32 nbytes) {
    if (gx->dirtyState != 0) {
        fn_800B91EC();
    }

    if (gx->status.word == 0) {
        __GXSendFlushPrim();
    }

    GX_FIFO_U8 = 0x40;
    GX_FIFO_U32 = (u32)list;
    GX_FIFO_U32 = nbytes;
}

void fn_800BD2E0(f32* projection, s32 type) {
    GXData_800BB30C* p = gx;

    p->field_420 = type;
    p->field_424 = projection[0];
    p->field_42C = projection[5];
    p->field_434 = projection[10];
    p->field_438 = projection[11];
    if (type == 1) {
        p->field_428 = projection[3];
        p->field_430 = projection[7];
    } else {
        p->field_428 = projection[2];
        p->field_430 = projection[6];
    }

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x00061020;
    GX_FIFO_F32 = p->field_424;
    GX_FIFO_F32 = p->field_428;
    GX_FIFO_F32 = p->field_42C;
    GX_FIFO_F32 = p->field_430;
    GX_FIFO_F32 = p->field_434;
    GX_FIFO_F32 = p->field_438;
    GX_FIFO_U32 = p->field_420;
    p->field_002 = 1;
}

void fn_800BD394(f32* projection) {
    GXData_800BB30C* p;
    u32 type;

    type = __cvt_fp2unsigned(projection[0]);
    p = gx;
    p->field_420 = type;
    p->field_424 = projection[1];
    p->field_428 = projection[2];
    p->field_42C = projection[3];
    p->field_430 = projection[4];
    p->field_434 = projection[5];
    p->field_438 = projection[6];

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x00061020;
    GX_FIFO_F32 = p->field_424;
    GX_FIFO_F32 = p->field_428;
    GX_FIFO_F32 = p->field_42C;
    GX_FIFO_F32 = p->field_430;
    GX_FIFO_F32 = p->field_434;
    GX_FIFO_F32 = p->field_438;
    GX_FIFO_U32 = p->field_420;
    p->field_002 = 1;
}

void fn_800BD454(f32* projection) {
    GXData_800BB30C* p = gx;

    projection[0] = p->field_420;
    projection[1] = p->field_424;
    projection[2] = p->field_428;
    projection[3] = p->field_42C;
    projection[4] = p->field_430;
    projection[5] = p->field_434;
    projection[6] = p->field_438;
}

void GXLoadPosMtxImm(f32 mtx[3][4], u32 id) {
    f64* pairs = (f64*)mtx;
    volatile f64* fifo = (volatile f64*)0xCC008000;
    u32 addr = id * 4;
    u32 reg = addr | 0xB0000;

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = reg;
    *fifo = pairs[0];
    *fifo = pairs[1];
    *fifo = pairs[2];
    *fifo = pairs[3];
    *fifo = pairs[4];
    *fifo = pairs[5];
}

void GXLoadNrmMtxImm(f32 mtx[3][4], u32 id) {
    u32 addr = id * 3 + 0x400;

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = addr | 0x80000;
    GX_FIFO_F32 = mtx[0][0];
    GX_FIFO_F32 = mtx[0][1];
    GX_FIFO_F32 = mtx[0][2];
    GX_FIFO_F32 = mtx[1][0];
    GX_FIFO_F32 = mtx[1][1];
    GX_FIFO_F32 = mtx[1][2];
    GX_FIFO_F32 = mtx[2][0];
    GX_FIFO_F32 = mtx[2][1];
    GX_FIFO_F32 = mtx[2][2];
}

void fn_800BD554(u32 index) {
    GXData_800BB30C* p = gx;

    p->mtxIdx0 = (p->mtxIdx0 & ~0x3FU) | index;
    __GXSetMatrixIndex(0);
}

void GXLoadTexMtxImm(f32 mtx[3][4], u32 id, s32 type) {
    u32 addr;
    u32 count;
    u32 length;

    if (id >= 0x40) {
        addr = (id - 0x40) * 4 + 0x500;
    } else {
        addr = id * 4;
    }

    if (type == 1) {
        count = 8;
    } else {
        count = 12;
    }

    length = (count - 1) << 16;
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = addr | length;

    if (type == 0) {
        f64* pairs = (f64*)mtx;
        volatile f64* fifo = (volatile f64*)0xCC008000;

        *fifo = pairs[0];
        *fifo = pairs[1];
        *fifo = pairs[2];
        *fifo = pairs[3];
        *fifo = pairs[4];
        *fifo = pairs[5];
    } else {
        f64* pairs = (f64*)mtx;
        volatile f64* fifo = (volatile f64*)0xCC008000;

        *fifo = pairs[0];
        *fifo = pairs[1];
        *fifo = pairs[2];
        *fifo = pairs[3];
    }
}

void fn_800BD640(f32 left, f32 top, f32 width, f32 height, f32 nearz,
                 f32 farz, u32 field) {
    GXData_800BB30C* p;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 ox;
    f32 oy;
    f32 oz;

    if (field == 0) {
        top -= 0.5f;
    }

    p = gx;
    p->projection[0] = left;
    p->projection[1] = top;
    p->projection[2] = width;
    p->projection[3] = height;
    p->projection[4] = nearz;
    p->projection[5] = farz;

    sx = width * 0.5f;
    sy = -height * 0.5f;
    ox = 342.0f + left + sx;
    oy = 342.0f + top + height * 0.5f;
    sz = 16777215.0f * farz - 16777215.0f * nearz;
    oz = 16777215.0f * farz;

    if (p->pad_454[0] != 0) {
        fn_800BD0F8();
    }

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x5101A;
    GX_FIFO_F32 = sx;
    GX_FIFO_F32 = sy;
    GX_FIFO_F32 = sz;
    GX_FIFO_F32 = ox;
    GX_FIFO_F32 = oy;
    GX_FIFO_F32 = oz;
    p->field_002 = 1;
}

void fn_800BD744(void) {
    fn_800BD640(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1);
}

void fn_800BD768(f32* projection) {
    GXData_800BB30C* p = gx;

    projection[0] = p->projection[0];
    projection[1] = p->projection[1];
    projection[2] = p->projection[2];
    projection[3] = p->projection[3];
    projection[4] = p->projection[4];
    projection[5] = p->projection[5];
}

void fn_800BD7A0(u32 xOrigin, u32 yOrigin, u32 width, u32 height) {
    GXData_800BB30C* p = gx;
    u32 x0;
    u32 y0;
    u32 x1;
    u32 y1;

    x0 = xOrigin + 0x156;
    y0 = yOrigin + 0x156;
    x1 = x0 + width - 1;
    y1 = y0 + height - 1;
    p->scissorTL = (p->scissorTL & ~0x7FFU) | y0;
    p->scissorTL = (p->scissorTL & ~0x7FF000U) | (x0 << 12);
    p->scissorBR = (p->scissorBR & ~0x7FFU) | y1;
    p->scissorBR = (p->scissorBR & ~0x7FF000U) | (x1 << 12);
    GX_BP_REG(p->scissorTL);
    GX_BP_REG(p->scissorBR);
    p->field_002 = 0;
}

void __GXSetMatrixIndex(s32 value) {
    GXData_800BB30C* p = gx;
    u32 matrixIndex;

    if (value < 5) {
        GX_FIFO_U8 = 8;
        GX_FIFO_U8 = 0x30;
        matrixIndex = p->mtxIdx0;
        GX_FIFO_U32 = matrixIndex;
        GX_FIFO_U8 = 0x10;
        GX_FIFO_U32 = 0x1018;
        GX_FIFO_U32 = matrixIndex;
    } else {
        GX_FIFO_U8 = 8;
        GX_FIFO_U8 = 0x40;
        matrixIndex = p->mtxIdx1;
        GX_FIFO_U32 = matrixIndex;
        GX_FIFO_U8 = 0x10;
        GX_FIFO_U32 = 0x1019;
        GX_FIFO_U32 = matrixIndex;
    }

    gx->field_002 = 1;
}

void fn_800BD91C(GXPerf0_800BB30C perf0, GXPerf1_800BB30C perf1) {
    u32 reg;

    switch (gx->perf0) {
    case GX_PERF0_VERTICES:
    case GX_PERF0_CLIP_VTX:
    case GX_PERF0_CLIP_CLKS:
    case GX_PERF0_XF_WAIT_IN:
    case GX_PERF0_XF_WAIT_OUT:
    case GX_PERF0_XF_XFRM_CLKS:
    case GX_PERF0_XF_LIT_CLKS:
    case GX_PERF0_XF_BOT_CLKS:
    case GX_PERF0_XF_REGLD_CLKS:
    case GX_PERF0_XF_REGRD_CLKS:
    case GX_PERF0_CLIP_RATIO:
    case GX_PERF0_CLOCKS:
        reg = 0;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_TRIANGLES:
    case GX_PERF0_TRIANGLES_CULLED:
    case GX_PERF0_TRIANGLES_PASSED:
    case GX_PERF0_TRIANGLES_SCISSORED:
    case GX_PERF0_TRIANGLES_0TEX:
    case GX_PERF0_TRIANGLES_1TEX:
    case GX_PERF0_TRIANGLES_2TEX:
    case GX_PERF0_TRIANGLES_3TEX:
    case GX_PERF0_TRIANGLES_4TEX:
    case GX_PERF0_TRIANGLES_5TEX:
    case GX_PERF0_TRIANGLES_6TEX:
    case GX_PERF0_TRIANGLES_7TEX:
    case GX_PERF0_TRIANGLES_8TEX:
    case GX_PERF0_TRIANGLES_0CLR:
    case GX_PERF0_TRIANGLES_1CLR:
    case GX_PERF0_TRIANGLES_2CLR:
        reg = 0x23000000;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_0CVG:
    case GX_PERF0_QUAD_NON0CVG:
    case GX_PERF0_QUAD_1CVG:
    case GX_PERF0_QUAD_2CVG:
    case GX_PERF0_QUAD_3CVG:
    case GX_PERF0_QUAD_4CVG:
    case GX_PERF0_AVG_QUAD_CNT:
        reg = 0x24000000;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_NONE:
        break;
    }

    switch (gx->perf1) {
    case GX_PERF1_TEXELS:
    case GX_PERF1_TX_IDLE:
    case GX_PERF1_TX_REGS:
    case GX_PERF1_TX_MEMSTALL:
    case GX_PERF1_TC_CHECK1_2:
    case GX_PERF1_TC_CHECK3_4:
    case GX_PERF1_TC_CHECK5_6:
    case GX_PERF1_TC_CHECK7_8:
    case GX_PERF1_TC_MISS:
    case GX_PERF1_CLOCKS:
        reg = 0x67000000;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_VC_ELEMQ_FULL:
    case GX_PERF1_VC_MISSQ_FULL:
    case GX_PERF1_VC_MEMREQ_FULL:
    case GX_PERF1_VC_STATUS7:
    case GX_PERF1_VC_MISSREP_FULL:
    case GX_PERF1_VC_STREAMBUF_LOW:
    case GX_PERF1_VC_ALL_STALLS:
    case GX_PERF1_VERTICES:
        gx->perfSel &= 0xFFFFFF0F;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_FIFO_REQ:
    case GX_PERF1_CALL_REQ:
    case GX_PERF1_VC_MISS_REQ:
    case GX_PERF1_CP_ALL_REQ:
        reg = 0;
        __cpReg[3] = reg;
        break;
    case GX_PERF1_NONE:
        break;
    }

    gx->perf0 = perf0;
    switch (gx->perf0) {
    case GX_PERF0_VERTICES:
        reg = 0x273;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_CLIP_VTX:
        reg = 0x14A;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_CLIP_CLKS:
        reg = 0x16B;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_WAIT_IN:
        reg = 0x84;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_WAIT_OUT:
        reg = 0xC6;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_XFRM_CLKS:
        reg = 0x210;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_LIT_CLKS:
        reg = 0x252;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_BOT_CLKS:
        reg = 0x231;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_REGLD_CLKS:
        reg = 0x1AD;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_XF_REGRD_CLKS:
        reg = 0x1CE;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_CLOCKS:
        reg = 0x21;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_CLIP_RATIO:
        reg = 0x153;
        GX_XF_REG(6, reg);
        break;
    case GX_PERF0_TRIANGLES:
        reg = 0x2300AE7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_CULLED:
        reg = 0x23008E7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_PASSED:
        reg = 0x23009E7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_SCISSORED:
        reg = 0x23001E7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_0TEX:
        reg = 0x2300AC3F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_1TEX:
        reg = 0x2300AC7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_2TEX:
        reg = 0x2300ACBF;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_3TEX:
        reg = 0x2300ACFF;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_4TEX:
        reg = 0x2300AD3F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_5TEX:
        reg = 0x2300AD7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_6TEX:
        reg = 0x2300ADBF;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_7TEX:
        reg = 0x2300ADFF;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_8TEX:
        reg = 0x2300AE3F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_0CLR:
        reg = 0x2300A27F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_1CLR:
        reg = 0x2300A67F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_TRIANGLES_2CLR:
        reg = 0x2300AA7F;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_0CVG:
        reg = 0x2402C0C6;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_NON0CVG:
        reg = 0x2402C16B;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_1CVG:
        reg = 0x2402C0E7;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_2CVG:
        reg = 0x2402C108;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_3CVG:
        reg = 0x2402C129;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_QUAD_4CVG:
        reg = 0x2402C14A;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_AVG_QUAD_CNT:
        reg = 0x2402C1AD;
        GX_BP_REG(reg);
        break;
    case GX_PERF0_NONE:
        break;
    }

    gx->perf1 = perf1;
    switch (gx->perf1) {
    case GX_PERF1_TEXELS:
        reg = 0x67000042;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TX_IDLE:
        reg = 0x67000084;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TX_REGS:
        reg = 0x67000063;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TX_MEMSTALL:
        reg = 0x67000129;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TC_MISS:
        reg = 0x67000252;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_CLOCKS:
        reg = 0x67000021;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TC_CHECK1_2:
        reg = 0x6700014B;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TC_CHECK3_4:
        reg = 0x6700018D;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TC_CHECK5_6:
        reg = 0x670001CF;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_TC_CHECK7_8:
        reg = 0x67000211;
        GX_BP_REG(reg);
        break;
    case GX_PERF1_VC_ELEMQ_FULL:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x20;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VC_MISSQ_FULL:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x30;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VC_MEMREQ_FULL:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x40;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VC_STATUS7:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x50;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VC_MISSREP_FULL:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x60;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VC_STREAMBUF_LOW:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x70;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VC_ALL_STALLS:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x90;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_VERTICES:
        gx->perfSel = (gx->perfSel & 0xFFFFFF0F) | 0x80;
        GX_CP_REG(0x20, gx->perfSel);
        break;
    case GX_PERF1_FIFO_REQ:
        reg = 2;
        __cpReg[3] = reg;
        break;
    case GX_PERF1_CALL_REQ:
        reg = 3;
        __cpReg[3] = reg;
        break;
    case GX_PERF1_VC_MISS_REQ:
        reg = 4;
        __cpReg[3] = reg;
        break;
    case GX_PERF1_CP_ALL_REQ:
        reg = 5;
        __cpReg[3] = reg;
        break;
    case GX_PERF1_NONE:
        break;
    }

    gx->field_002 = 0;
}

void fn_800BE30C(void) {
    __cpReg[2] = 4;
}

u32 fn_800BE31C(void) {
    u32 hi;
    u32 lo;

    fn_800BE164(&hi, &lo);
    return hi;
}

void GXSetBlendMode(u32 type, u32 srcFactor, u32 dstFactor, u32 op) {
    GXData_800BB30C* p = gx;
    u32 value = p->field_1D0;

    value = __rlwimi(value, type == 3, 11, 20, 20);
    value = __rlwimi(value, type, 0, 31, 31);
    value = __rlwimi(value, type == 2, 1, 30, 30);
    value = __rlwimi(value, op, 12, 16, 19);
    value = __rlwimi(value, srcFactor, 8, 21, 23);
    value = __rlwimi(value, dstFactor, 5, 24, 26);
    GX_BP_REG(value);
    p->field_1D0 = value;
    p->field_002 = 0;
}

void fn_800BCE30(u32 zCompLoc) {
    GXData_800BB30C* p = gx;
    u32 value = p->field_1D0;

    value = __rlwimi(value, zCompLoc, 3, 28, 28);
    GX_BP_REG(value);
    p->field_1D0 = value;
    p->field_002 = 0;
}

void fn_800BCE5C(u32 zCompLoc) {
    GXData_800BB30C* p = gx;
    u32 value = p->field_1D0;

    value = __rlwimi(value, zCompLoc, 4, 27, 27);
    GX_BP_REG(value);
    p->field_1D0 = value;
    p->field_002 = 0;
}

void GXSetZMode(u32 compareEnable, u32 func, u32 updateEnable) {
    GXData_800BB30C* p = gx;
    u32 value = p->zMode;

    value &= 0x87FFFFFFU;
    value |= compareEnable << 31;
    value |= func << 28;
    value |= updateEnable << 27;
    GX_BP_REG(value);
    p->zMode = value;
    p->field_002 = 0;
}

void fn_800BCEF4(s32 pixelFmt, u32 zFmt) {
    GXData_800BB30C* p = gx;
    u32 old = p->field_1DC;
    u32 value;
    u8 isY8;

    p->field_1DC = (p->field_1DC & ~7U) | lbl_80313608[pixelFmt];
    p->field_1DC = (p->field_1DC & ~0x38U) | (zFmt << 3);

    value = p->field_1DC;
    if (old != value) {
        GX_BP_REG(value);
        if (pixelFmt == 2) {
            isY8 = 1;
        } else {
            isY8 = 0;
        }
        p = gx;
        p->genMode = (p->genMode & ~0x200U) | (isY8 << 9);
        p->dirtyState |= 4;
    }

    if (lbl_80313608[pixelFmt] == 4) {
        p = gx;
        p->dstAlpha = (p->dstAlpha & ~0x600U) |
                      (((pixelFmt - 4) << 9) & 0x600U);
        p->dstAlpha = (p->dstAlpha & 0xFFFFFFU) | 0x42000000U;
        GX_BP_REG(p->dstAlpha);
    }

    gx->field_002 = 0;
}

void fn_800BCFDC(u32 zCompLoc) {
    GXData_800BB30C* p = gx;
    u32 value = p->field_1D0;

    value = __rlwimi(value, zCompLoc, 2, 29, 29);
    GX_BP_REG(value);
    p->field_1D0 = value;
    p->field_002 = 0;
}

void GXSetDstAlpha(u32 enable, u32 alpha) {
    GXData_800BB30C* p = gx;
    u32 value = p->dstAlpha;

    value = __rlwimi(value, alpha & 0xFFU, 0, 24, 31);
    value = __rlwimi(value, enable & 0xFFU, 8, 23, 23);

    GX_BP_REG(value);
    p->dstAlpha = value;
    p->field_002 = 0;
}

#pragma optimize_for_size off
void GXSetClipMode(u32 clipMode) {
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x1005;
    GX_FIFO_U32 = clipMode;
    gx->field_002 = 1;
}

#pragma optimize_for_size reset

void fn_800BD044(u32 arg0, u32 arg1) {
    GXData_800BB30C* p = gx;
    u32 value = arg1;

    value &= 0xFFU;
    value |= (arg0 & 0xFFU) << 1U;
    value &= 0xFFU;
    GX_BP_REG(0x44000000U | value);
    p->field_002 = 0;
}

void fn_800BD07C(u32 fieldMode, u32 halfAspectRatio) {
    GXData_800BB30C* p = gx;

    p->lpSize = (p->lpSize & ~0x400000U) |
                ((halfAspectRatio & 0xFFU) << 22);
    GX_BP_REG(p->lpSize);
    __GXFlushTextureState();
    GX_BP_REG(0x68000000U | (fieldMode & 0xFFU));
    __GXFlushTextureState();
}

void fn_800BD830(u32 arg0, u32 arg1) {
    GXData_800BB30C* p = gx;
    u32 value = arg0 + 0x156U;
    u32 cmd = arg1 + 0x156U;

    value = (value >> 1U) & 0xFFC00FFFU;
    cmd = ((cmd << 9U) | (cmd >> 23U)) & 0x003FFFFFU;
    value |= cmd;
    value &= 0x00FFFFFFU;

    GX_BP_REG(0x59000000U | value);
    p->field_002 = 0;
}

void TRKNubMainLoop(void) {
    extern s32 TRKGetNextEvent(TRKEvent* event);
    extern void* TRKGetBuffer(s32 bufferIndex);
    extern s32 TRKDispatchMessage(void* buffer);
    extern void TRKTargetInterrupt(TRKEvent* event);
    extern void TRKTargetSupportRequest(void);
    extern s32 TRKTargetStopped(void);
    extern s32 TRKTargetContinue(void);
    extern void TRKGetInput(void);
    extern void TRKDestructEvent(TRKEvent* event);
    extern void* gTRKInputPendingPtr;
    s32 done = 0;
    s32 inputActivated = 0;
    TRKEvent event;

    while (!done) {
        if (TRKGetNextEvent(&event)) {
            inputActivated = 0;
            switch (event.type) {
            case 2:
                TRKDispatchMessage(TRKGetBuffer(event.bufferIndex));
                break;
            case 1:
                done = 1;
                break;
            case 3:
            case 4:
                TRKTargetInterrupt(&event);
                break;
            case 5:
                TRKTargetSupportRequest();
                break;
            case 0:
                break;
            }
            TRKDestructEvent(&event);
        } else {
            if (inputActivated != 0) {
                u8** inputPendingPtr = (u8**)&gTRKInputPendingPtr;

                if (**inputPendingPtr == 0) {
                    goto no_input;
                }
            }
            inputActivated = 1;
            TRKGetInput();
            continue;

        no_input:
            if (!TRKTargetStopped()) {
                TRKTargetContinue();
            }
            inputActivated = 0;
        }
    }
}

void TRKDestructEvent(TRKEvent* event) {
    TRKReleaseBuffer(event->bufferIndex);
}
#endif
