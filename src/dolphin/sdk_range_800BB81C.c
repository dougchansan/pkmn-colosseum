/**
 * @file sdk_range_800BB81C.c
 * @brief dolphin-sdk code, 0x800BB81C - 0x800BC580 (23 functions), with the
 *        TEV op presets (.data 0x80313590 - 0x803135E0) and the literal pool
 *        0x8047C370 - 0x8047C388 (1024.0f, 1/1024, 0.0f and the u32
 *        int-to-float bias).
 *
 * Merged from three candidate chunks (sdk_candidate_800BB81C,
 * sdk_candidate_800BB97C_gc13, sdk_candidate_800BBC0C) so that one object
 * owns the pool: GXSetIndTexMtx reads 1024.0f and fn_800BBCE0 the rest, and no
 * other code reads 0x8047C370 - 0x8047C388. The middle chunk was scored with
 * GC/1.3 (91-93%); under this range's GC/1.2.5n its two functions are exact.
 * Bodies copied, in address order, from sdk_range_800BB30C.c and the former
 * sdk_candidate_800BB81C.c (lane D18).
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


void GXSetIndTexMtx(u32 mtx_id, const f32 offset[2][3], s8 scale_exp)
{
    s32 mtx[6];
    u32 reg;
    u32 id;

    switch (mtx_id) {
    case 1:
    case 2:
    case 3:
        id = mtx_id - 1;
        break;
    case 5:
    case 6:
    case 7:
        id = mtx_id - 5;
        break;
    case 9:
    case 10:
    case 11:
        id = mtx_id - 9;
        break;
    default:
        id = 0;
        break;
    }

    mtx[0] = (s32) (1024.0f * offset[0][0]) & 0x7FF;
    mtx[1] = (s32) (1024.0f * offset[1][0]) & 0x7FF;
    scale_exp += 17;
    reg = 0;
    reg = (reg & ~0x7FFU) | (mtx[0] & 0x7FF);
    reg = (reg & ~0x3FF800U) | ((mtx[1] << 11) & 0x3FF800);
    reg = (reg & ~0xC00000U) | ((scale_exp & 3) << 22);
    reg = (reg & 0xFFFFFFU) | ((id * 3 + 6) << 24);
    GX_BP_REG(reg);

    mtx[2] = (s32) (1024.0f * offset[0][1]) & 0x7FF;
    mtx[3] = (s32) (1024.0f * offset[1][1]) & 0x7FF;
    reg = 0;
    reg = (reg & ~0x7FFU) | (mtx[2] & 0x7FF);
    reg = (reg & ~0x3FF800U) | ((mtx[3] << 11) & 0x3FF800);
    reg = (reg & ~0xC00000U) | (((scale_exp >> 2) & 3) << 22);
    reg = (reg & 0xFFFFFFU) | ((id * 3 + 7) << 24);
    GX_BP_REG(reg);

    mtx[4] = (s32) (1024.0f * offset[0][2]) & 0x7FF;
    mtx[5] = (s32) (1024.0f * offset[1][2]) & 0x7FF;
    reg = 0;
    reg = (reg & ~0x7FFU) | (mtx[4] & 0x7FF);
    reg = (reg & ~0x3FF800U) | ((mtx[5] << 11) & 0x3FF800);
    reg = (reg & ~0xC00000U) | (((scale_exp >> 4) & 3) << 22);
    reg = (reg & 0xFFFFFFU) | ((id * 3 + 8) << 24);
    GX_BP_REG(reg);
    gx->field_002 = 0;
}

void GXSetIndTexCoordScale(u32 stage, u32 scale_s, u32 scale_t)
{
    switch (stage) {
    case 0:
        gx->indTexScale0 =
            (gx->indTexScale0 & ~0xFU) | scale_s;
        gx->indTexScale0 =
            (gx->indTexScale0 & ~0xF0U) | (scale_t << 4);
        gx->indTexScale0 =
            (gx->indTexScale0 & 0xFFFFFFU) | 0x25000000;
        GX_BP_REG(gx->indTexScale0);
        break;
    case 1:
        gx->indTexScale0 =
            (gx->indTexScale0 & ~0xF00U) | (scale_s << 8);
        gx->indTexScale0 =
            (gx->indTexScale0 & ~0xF000U) | (scale_t << 12);
        gx->indTexScale0 =
            (gx->indTexScale0 & 0xFFFFFFU) | 0x25000000;
        GX_BP_REG(gx->indTexScale0);
        break;
    case 2:
        gx->indTexScale1 =
            (gx->indTexScale1 & ~0xFU) | scale_s;
        gx->indTexScale1 =
            (gx->indTexScale1 & ~0xF0U) | (scale_t << 4);
        gx->indTexScale1 =
            (gx->indTexScale1 & 0xFFFFFFU) | 0x26000000;
        GX_BP_REG(gx->indTexScale1);
        break;
    case 3:
        gx->indTexScale1 =
            (gx->indTexScale1 & ~0xF00U) | (scale_s << 8);
        gx->indTexScale1 =
            (gx->indTexScale1 & ~0xF000U) | (scale_t << 12);
        gx->indTexScale1 =
            (gx->indTexScale1 & 0xFFFFFFU) | 0x26000000;
        GX_BP_REG(gx->indTexScale1);
        break;
    }
    gx->field_002 = 0;
}

void GXSetIndTexOrder(u32 stage, u32 tex_coord, u32 tex_map)
{
    switch (stage) {
    case 0:
        gx->iref = (gx->iref & ~0x7U) | tex_map;
        gx->iref = (gx->iref & ~0x38U) | (tex_coord << 3);
        break;
    case 1:
        gx->iref = (gx->iref & ~0x1C0U) | (tex_map << 6);
        gx->iref = (gx->iref & ~0xE00U) | (tex_coord << 9);
        break;
    case 2:
        gx->iref = (gx->iref & ~0x7000U) | (tex_map << 12);
        gx->iref = (gx->iref & ~0x38000U) | (tex_coord << 15);
        break;
    case 3:
        gx->iref = (gx->iref & ~0x1C0000U) | (tex_map << 18);
        gx->iref = (gx->iref & ~0xE00000U) | (tex_coord << 21);
        break;
    }
    GX_BP_REG(gx->iref);
    gx->dirtyState |= 3;
    gx->field_002 = 0;
}

void GXSetNumIndStages(u32 nChans) {
    GXData_800BB30C* p = gx;

    p->genMode = (p->genMode & ~0x70000U) | ((nChans & 0xFF) << 16);
    p->dirtyState |= 6;
}

void GXSetTevDirect(u32 dstCoord) {
    GXSetTevIndirect(dstCoord, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void fn_800BBC7C(u32 dstCoord, u32 func, u8 normalize, u8 color, u32 postMtx) {
    u32 colorSel = color != 0 ? 6 : 0;

    GXSetTevIndirect(dstCoord, func, 0, normalize != 0 ? 7 : 0, postMtx, colorSel, colorSel, 0, 0, 0);
}

void fn_800BBCE0(u32 tev_stage, u32 ind_stage, u16 tilesize_s,
                 u16 tilesize_t, u16 tilespacing_s, u16 tilespacing_t,
                 u32 format, u32 matrix_sel, u32 bias_sel, u32 alpha_sel)
{
    u32 wrap_s;
    u32 wrap_t;
    f32 mtx[2][3];

    switch (tilesize_s) {
    case 256: wrap_s = 1; break;
    case 128: wrap_s = 2; break;
    case 64: wrap_s = 3; break;
    case 32: wrap_s = 4; break;
    case 16: wrap_s = 5; break;
    default: wrap_s = 0; break;
    }
    switch (tilesize_t) {
    case 256: wrap_t = 1; break;
    case 128: wrap_t = 2; break;
    case 64: wrap_t = 3; break;
    case 32: wrap_t = 4; break;
    case 16: wrap_t = 5; break;
    default: wrap_t = 0; break;
    }

    mtx[0][0] = tilespacing_s / 1024.0f;
    mtx[0][1] = mtx[0][2] = 0.0f;
    mtx[1][1] = tilespacing_t / 1024.0f;
    mtx[1][0] = mtx[1][2] = 0.0f;
    GXSetIndTexMtx(matrix_sel, mtx, 10);
    GXSetTevIndirect(tev_stage, ind_stage, format, bias_sel, matrix_sel, wrap_s,
                wrap_t, 0, 1, alpha_sel);
}

void fn_800BBE8C(u32 tev_stage, u32 ind_stage, u32 matrix_sel)
{
    u32 sm;
    u32 tm;

    switch (matrix_sel) {
    case 1:
        sm = 5;
        tm = 9;
        break;
    case 2:
        sm = 6;
        tm = 10;
        break;
    case 3:
        sm = 7;
        tm = 11;
        break;
    }

    GXSetTevIndirect(tev_stage, ind_stage, 0, 3, sm, 6, 6, 0, 0, 0);
    GXSetTevIndirect(tev_stage + 1, ind_stage, 0, 3, tm, 6, 6, 1, 0, 0);
    GXSetTevIndirect(tev_stage + 2, ind_stage, 0, 0, 0, 0, 0, 1, 0, 0);
}

void fn_800BBF98(u32 dstCoord, u32 func, u32 normalize) {
    GXSetTevIndirect(dstCoord, func, 0, 7, normalize, 0, 0, 0, 0, 0);
}

void fn_800BBFDC(u32 dstCoord) {
    GXSetTevIndirect(dstCoord, 0, 0, 0, 0, 6, 6, 1, 0, 0);
}

void __GXUpdateBPMask(void) {
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

void __GXFlushTextureState(void) {
    GX_BP_REG(gx->field_124);
    gx->field_002 = 0;
}
