/**
 * @file sdk_range_800B857C.c
 * @brief dolphin-sdk code, 0x800B857C - 0x800BA198 (47 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"

typedef void (*GXBreakPtCallback)(u16 token);
typedef void (*GXDrawDoneCallback)(void);

typedef struct GXData_800B857C {
    /* 0x000 */ u16 field_000;
    /* 0x002 */ u16 field_002;
    /* 0x004 */ u16 field_004;
    /* 0x006 */ u16 field_006;
    /* 0x008 */ u8 pad_008[0x74];
    /* 0x07C */ u32 field_07C;
    /* 0x080 */ u32 mtxIdx0;
    /* 0x084 */ u32 mtxIdx1;
    /* 0x088 */ u8 pad_088[0x30];
    /* 0x0B8 */ u32 field_0B8[8];
    /* 0x0D8 */ u8 pad_0D8[0xF8];
    /* 0x1D0 */ u32 field_1D0;
    /* 0x1D4 */ u32 field_1D4;
    /* 0x1D8 */ u32 field_1D8;
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
    /* 0x204 */ u32 field_204;
    /* 0x208 */ u8 pad_208[0x2E9];
    /* 0x4F1 */ u8 field_4F1;
    /* 0x4F2 */ u8 field_4F2;
    /* 0x4F3 */ u8 pad_4F3;
    /* 0x4F4 */ u32 dirtyState;
} GXData_800B857C;

extern GXData_800B857C* const gx;
extern volatile u16* __peReg;
extern void* __memReg;
extern GXBreakPtCallback lbl_8047A9C0;
extern GXDrawDoneCallback lbl_8047A9C4;
extern volatile u8 lbl_8047A9C8;
extern OSThreadQueue lbl_8047A9CC;

extern u32 fn_800B7714(void);
extern void __GXCleanGPFifo(void);
extern void __GXSetSUTexRegs(void);
extern void fn_800BC024(void);
extern void fn_800B9578(void);
extern void fn_800B7BC4(void);
extern void fn_800B8444(void);
extern void __GXCalculateVLim(void);
extern u32 __cvt_fp2unsigned(f32 value);
extern void __GetImageTileCount(s32 format, u16 width, u16 height,
                                u32* rowTiles, u32* columnTiles, u32* planes);

#define GX_FIFO_U8  (*(volatile u8*)0xCC008000)
#define GX_FIFO_U16 (*(volatile u16*)0xCC008000)
#define GX_FIFO_U32 (*(volatile u32*)0xCC008000)
#define GX_GET_MEM_REG(offset) (*(volatile u16*)((volatile u16*)__memReg + (offset)))

volatile u32 __PIRegs[12] : 0xCC003000;

#define GX_BP_REG(reg)      \
    do {                    \
        GX_FIFO_U8 = 0x61;  \
        GX_FIFO_U32 = (reg); \
    } while (0)

typedef enum GXTexCoordID_800B857C {
    GX_TEXCOORD0_800B857C,
    GX_TEXCOORD1_800B857C,
    GX_TEXCOORD2_800B857C,
    GX_TEXCOORD3_800B857C,
    GX_TEXCOORD4_800B857C,
    GX_TEXCOORD5_800B857C,
    GX_TEXCOORD6_800B857C,
    GX_TEXCOORD7_800B857C,
    GX_MAX_TEXCOORD_800B857C,
    GX_TEXCOORD_NULL_800B857C = 0xFF,
} GXTexCoordID_800B857C;

typedef enum GXTexGenType_800B857C {
    GX_TG_MTX3x4_800B857C,
    GX_TG_MTX2x4_800B857C,
    GX_TG_BUMP0_800B857C,
    GX_TG_BUMP1_800B857C,
    GX_TG_BUMP2_800B857C,
    GX_TG_BUMP3_800B857C,
    GX_TG_BUMP4_800B857C,
    GX_TG_BUMP5_800B857C,
    GX_TG_BUMP6_800B857C,
    GX_TG_BUMP7_800B857C,
    GX_TG_SRTG_800B857C,
} GXTexGenType_800B857C;

typedef enum GXTexGenSrc_800B857C {
    GX_TG_POS_800B857C,
    GX_TG_NRM_800B857C,
    GX_TG_BINRM_800B857C,
    GX_TG_TANGENT_800B857C,
    GX_TG_TEX0_800B857C,
    GX_TG_TEX1_800B857C,
    GX_TG_TEX2_800B857C,
    GX_TG_TEX3_800B857C,
    GX_TG_TEX4_800B857C,
    GX_TG_TEX5_800B857C,
    GX_TG_TEX6_800B857C,
    GX_TG_TEX7_800B857C,
    GX_TG_TEXCOORD0_800B857C,
    GX_TG_TEXCOORD1_800B857C,
    GX_TG_TEXCOORD2_800B857C,
    GX_TG_TEXCOORD3_800B857C,
    GX_TG_TEXCOORD4_800B857C,
    GX_TG_TEXCOORD5_800B857C,
    GX_TG_TEXCOORD6_800B857C,
    GX_TG_COLOR0_800B857C,
    GX_TG_COLOR1_800B857C,
} GXTexGenSrc_800B857C;

#define GX_SET_REG_FIELD_800B857C(reg, size, shift, val)                    \
    do {                                                                    \
        (reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) |         \
                ((u32)(val) << (shift));                                    \
    } while (0)

extern void __GXSetMatrixIndex(u32 attr);

#if !defined(GX_EXACT_800B884C_800B8AE8)
void fn_800B857C(GXTexCoordID_800B857C dst_coord,
                 GXTexGenType_800B857C func,
                 GXTexGenSrc_800B857C src_param, u32 mtx, u8 normalize,
                 u32 pt_texmtx)
{
    u32 reg = 0;
    u32 row;
    u32 bumprow;
    u32 form;
    u32 mtx_id_attr;

    form = 0;
    row = 5;
    switch (src_param) {
    case GX_TG_POS_800B857C:
        row = 0;
        form = 1;
        break;
    case GX_TG_NRM_800B857C:
        row = 1;
        form = 1;
        break;
    case GX_TG_BINRM_800B857C:
        row = 3;
        form = 1;
        break;
    case GX_TG_TANGENT_800B857C:
        row = 4;
        form = 1;
        break;
    case GX_TG_COLOR0_800B857C:
        row = 2;
        break;
    case GX_TG_COLOR1_800B857C:
        row = 2;
        break;
    case GX_TG_TEX0_800B857C:
        row = 5;
        break;
    case GX_TG_TEX1_800B857C:
        row = 6;
        break;
    case GX_TG_TEX2_800B857C:
        row = 7;
        break;
    case GX_TG_TEX3_800B857C:
        row = 8;
        break;
    case GX_TG_TEX4_800B857C:
        row = 9;
        break;
    case GX_TG_TEX5_800B857C:
        row = 10;
        break;
    case GX_TG_TEX6_800B857C:
        row = 11;
        break;
    case GX_TG_TEX7_800B857C:
        row = 12;
        break;
    case GX_TG_TEXCOORD0_800B857C:
        bumprow;
        break;
    case GX_TG_TEXCOORD1_800B857C:
        bumprow;
        break;
    case GX_TG_TEXCOORD2_800B857C:
        bumprow;
        break;
    case GX_TG_TEXCOORD3_800B857C:
        bumprow;
        break;
    case GX_TG_TEXCOORD4_800B857C:
        bumprow;
        break;
    case GX_TG_TEXCOORD5_800B857C:
        bumprow;
        break;
    case GX_TG_TEXCOORD6_800B857C:
        bumprow;
        break;
    default:
        break;
    }

    switch (func) {
    case GX_TG_MTX2x4_800B857C:
        GX_SET_REG_FIELD_800B857C(reg, 1, 1, 0);
        GX_SET_REG_FIELD_800B857C(reg, 1, 2, form);
        GX_SET_REG_FIELD_800B857C(reg, 3, 4, 0);
        GX_SET_REG_FIELD_800B857C(reg, 5, 7, row);
        break;
    case GX_TG_MTX3x4_800B857C:
        GX_SET_REG_FIELD_800B857C(reg, 1, 1, 1);
        GX_SET_REG_FIELD_800B857C(reg, 1, 2, form);
        GX_SET_REG_FIELD_800B857C(reg, 3, 4, 0);
        GX_SET_REG_FIELD_800B857C(reg, 5, 7, row);
        break;
    case GX_TG_BUMP0_800B857C:
    case GX_TG_BUMP1_800B857C:
    case GX_TG_BUMP2_800B857C:
    case GX_TG_BUMP3_800B857C:
    case GX_TG_BUMP4_800B857C:
    case GX_TG_BUMP5_800B857C:
    case GX_TG_BUMP6_800B857C:
    case GX_TG_BUMP7_800B857C:
        GX_SET_REG_FIELD_800B857C(reg, 1, 1, 0);
        GX_SET_REG_FIELD_800B857C(reg, 1, 2, form);
        GX_SET_REG_FIELD_800B857C(reg, 3, 4, 1);
        GX_SET_REG_FIELD_800B857C(reg, 5, 7, row);
        GX_SET_REG_FIELD_800B857C(reg, 3, 12, src_param - 12);
        GX_SET_REG_FIELD_800B857C(reg, 3, 15,
                                  func - GX_TG_BUMP0_800B857C);
        break;
    case GX_TG_SRTG_800B857C:
        GX_SET_REG_FIELD_800B857C(reg, 1, 1, 0);
        GX_SET_REG_FIELD_800B857C(reg, 1, 2, form);
        if (src_param == GX_TG_COLOR0_800B857C) {
            GX_SET_REG_FIELD_800B857C(reg, 3, 4, 2);
        } else {
            GX_SET_REG_FIELD_800B857C(reg, 3, 4, 3);
        }
        GX_SET_REG_FIELD_800B857C(reg, 5, 7, 2);
        break;
    default:
        break;
    }

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = dst_coord + 0x1040;
    GX_FIFO_U32 = reg;
    reg = 0;
    GX_SET_REG_FIELD_800B857C(reg, 6, 0, pt_texmtx - 64);
    GX_SET_REG_FIELD_800B857C(reg, 1, 8, normalize);
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = dst_coord + 0x1050;
    GX_FIFO_U32 = reg;

    switch (dst_coord) {
    case GX_TEXCOORD0_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx0, 6, 6, mtx);
        break;
    case GX_TEXCOORD1_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx0, 6, 12, mtx);
        break;
    case GX_TEXCOORD2_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx0, 6, 18, mtx);
        break;
    case GX_TEXCOORD3_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx0, 6, 24, mtx);
        break;
    case GX_TEXCOORD4_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx1, 6, 0, mtx);
        break;
    case GX_TEXCOORD5_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx1, 6, 6, mtx);
        break;
    case GX_TEXCOORD6_800B857C:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx1, 6, 12, mtx);
        break;
    default:
        GX_SET_REG_FIELD_800B857C(gx->mtxIdx1, 6, 18, mtx);
        break;
    }

    mtx_id_attr = dst_coord + 1;
    __GXSetMatrixIndex(mtx_id_attr);
}
#endif

void fn_800B884C(u8 count) {
    u32 n = count;
    GXData_800B857C* p = gx;

    p->field_204 = (p->field_204 & ~0xFU) | n;
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x103F;
    GX_FIFO_U32 = n;
    p->dirtyState |= 4;
}

void GXSetMisc(s32 token, u32 value) {
    switch (token) {
    case 0:
        break;
    case 1:
        gx->field_004 = value;
        gx->field_000 = (u16)((u32)__cntlzw(gx->field_004) >> 5);
        gx->field_002 = 1;
        if (gx->field_004 != 0) {
            gx->dirtyState |= 8;
        }
        break;
    case 2:
        gx->field_4F1 = (value != 0);
        break;
    case 3:
        gx->field_4F2 = (value != 0);
        break;
    }
}

void fn_800B91EC(void);
void __GXSendFlushPrim(void);

void GXFlush(void) {
    if (gx->dirtyState != 0) {
        fn_800B91EC();
    }

    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    PPCSync();
}

static inline u32 __GXReadMEMCounterU32(u32 regAddrL, u32 regAddrH) {
    u32 ctrH0;
    u32 ctrH1;
    u32 ctrL;

    ctrH0 = GX_GET_MEM_REG(regAddrH);

    do {
        ctrH1 = ctrH0;
        ctrL = GX_GET_MEM_REG(regAddrL);
        ctrH0 = GX_GET_MEM_REG(regAddrH);
    } while (ctrH0 != ctrH1);

    return (ctrH0 << 16) | ctrL;
}

#if defined(GX_EXACT_800B884C_800B8AE8)
#define GX_ABORT_STATIC static inline
#else
#define GX_ABORT_STATIC static
#endif

GX_ABORT_STATIC void __GXAbortWait(u32 clocks) {
    OSTime time0;
    OSTime time1;

    time0 = OSGetTime();
    do {
        time1 = OSGetTime();
    } while (time1 - time0 <= (clocks / 4));
}

GX_ABORT_STATIC void __GXAbortWaitPECopyDone(void) {
    u32 peCnt0;
    u32 peCnt1;

    peCnt0 = __GXReadMEMCounterU32(0x28, 0x27);
    do {
        peCnt1 = peCnt0;
        __GXAbortWait(32);
        peCnt0 = __GXReadMEMCounterU32(0x28, 0x27);
    } while (peCnt0 != peCnt1);
}

void __GXAbort(void) {
    if (gx->field_4F2 && fn_800B7714() != 0) {
        __GXAbortWaitPECopyDone();
    }

    __PIRegs[0x18 / 4] = 1;
    __GXAbortWait(200);
    __PIRegs[0x18 / 4] = 0;
    __GXAbortWait(20);
}

#undef GX_ABORT_STATIC

#if !defined(GX_EXACT_800B884C_800B8AE8)
void fn_800B8C58(u16 token) {
    BOOL enabled;
    u32 reg;

    enabled = OSDisableInterrupts();
    reg = token | 0x48000000;
    GX_BP_REG(reg);
    reg = (reg & ~0xFFFFU) | token;
    reg = (reg & 0xFFFFFFU) | 0x47000000;
    GX_BP_REG(reg);
    GXFlush();
    OSRestoreInterrupts(enabled);
    gx->field_002 = 0;
}

void GXSetDrawDone(void) {
    BOOL enabled;
    u8 cmd = 0x61;
    GXData_800B857C* p;

    enabled = OSDisableInterrupts();
    p = gx;
    GX_FIFO_U8 = cmd;
    GX_FIFO_U32 = 0x45000002;
    if (p->dirtyState != 0) {
        fn_800B91EC();
    }

    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    GX_FIFO_U32 = 0;
    PPCSync();
    lbl_8047A9C8 = 0;
    OSRestoreInterrupts(enabled);
}

void fn_800B8DA8(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    while (lbl_8047A9C8 == 0) {
        OSSleepThread(&lbl_8047A9CC);
    }
    OSRestoreInterrupts(enabled);
}

#pragma dont_inline on
void GXDrawDone(void) {
    BOOL enabled;
    u32 scratch[2];

    enabled = OSDisableInterrupts();
    GX_FIFO_U8 = 0x61;
    GX_FIFO_U32 = 0x45000002;
    GXFlush();
    lbl_8047A9C8 = 0;
    OSRestoreInterrupts(enabled);

    enabled = OSDisableInterrupts();
    while (lbl_8047A9C8 == 0) {
        OSSleepThread(&lbl_8047A9CC);
    }
    OSRestoreInterrupts(enabled);
}
#pragma dont_inline reset

void fn_800B8E74(void) {
    GXData_800B857C* p = gx;

    GX_FIFO_U8 = 0x61;
    GX_FIFO_U32 = p->field_1DC;
    p->field_002 = 0;
}

void fn_800B8E98(u32 token, u32 value) {
    __peReg[3] = (u16)((token << 8) | (value & 0xFF));
}

void fn_800B8EAC(u32 value) {
    __peReg[4] = (u16)((value & ~4U) | 4);
}

void fn_800B8EC0(u32 enable) {
    volatile u16* reg = &__peReg[1];
    *reg = (u16)((*reg & ~0x10U) | ((enable & 0xFF) << 4));
}

void fn_800B8EDC(s32 type, u32 srcFactor, u32 dstFactor, u32 op) {
    volatile u16* regp = &__peReg[1];
    u32 reg = *regp;
    u32 enable = (type == 1 || type == 3);
    u32 subtract = (type == 3);
    u32 logic = (type == 2);

    reg = (reg & ~1U) | enable;
    reg = (reg & ~0x800U) | (subtract << 11);
    reg = (reg & ~2U) | (logic << 1);
    reg = (reg & ~0xF000U) | (op << 12);
    reg = (reg & ~0x700U) | (srcFactor << 8);
    reg = (reg & ~0xE0U) | (dstFactor << 5);
    reg = (reg & 0xFFFFFFU) | 0x41000000;
    *regp = (u16)reg;
}

void fn_800B8F64(u32 enable) {
    volatile u16* reg = &__peReg[1];
    *reg = (u16)((*reg & ~0x8U) | ((enable & 0xFF) << 3));
}

void fn_800B8F80(u8 func, u32 threshold) {
    u32 reg;

    reg = (threshold & 0xFF) | (func << 8);
    __peReg[2] = reg;
}

void fn_800B8F94(u32 enable) {
    volatile u16* reg = &__peReg[1];
    *reg = (u16)((*reg & ~0x4U) | ((enable & 0xFF) << 2));
}

void fn_800B8FB0(u32 arg0, u32 arg1, u32 arg2) {
    u32 reg = arg0 & 0xFF;

    reg = (reg & ~0xEU) | (arg1 << 1);
    reg = (reg & ~0x10U) | ((arg2 & 0xFF) << 4);
    __peReg[0] = (u16)reg;
}

GXBreakPtCallback fn_800B8FD8(GXBreakPtCallback callback) {
    GXBreakPtCallback old = lbl_8047A9C0;
    BOOL enabled = OSDisableInterrupts();

    lbl_8047A9C0 = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

void fn_800B901C(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;
    u16 token = __peReg[7];

    if (lbl_8047A9C0 != NULL) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        lbl_8047A9C0(token);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }

    __peReg[5] = (__peReg[5] & ~4U) | 4;
}

GXDrawDoneCallback fn_800B90A4(GXDrawDoneCallback callback) {
    GXDrawDoneCallback old = lbl_8047A9C4;
    BOOL enabled = OSDisableInterrupts();

    lbl_8047A9C4 = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

void fn_800B90E8(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptionContext;

    __peReg[5] = (__peReg[5] & ~8U) | 8;
    lbl_8047A9C8 = 1;

    if (lbl_8047A9C4 != NULL) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        lbl_8047A9C4();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }

    OSWakeupThread(&lbl_8047A9CC);
}

void __GXPEInit(void) {
    volatile u16* regp;
    u32 reg;

    __OSSetInterruptHandler(0x12, fn_800B901C);
    __OSSetInterruptHandler(0x13, fn_800B90E8);
    OSInitThreadQueue(&lbl_8047A9CC);
    __OSUnmaskInterrupts(0x2000);
    __OSUnmaskInterrupts(0x1000);

    regp = &__peReg[5];
    reg = *regp;
    reg = (reg & ~4U) | 4;
    reg = (reg & ~8U) | 8;
    reg = (reg & ~1U) | 1;
    reg = (reg & ~2U) | 2;
    *regp = (u16)reg;
}

void fn_800B91EC(void) {
    if (gx->dirtyState & 1) {
        __GXSetSUTexRegs();
    }
    if (gx->dirtyState & 2) {
        fn_800BC024();
    }
    if (gx->dirtyState & 4) {
        fn_800B9578();
    }
    if (gx->dirtyState & 8) {
        fn_800B7BC4();
    }
    if (gx->dirtyState & 0x10) {
        fn_800B8444();
    }
    if (gx->dirtyState & 0x18) {
        __GXCalculateVLim();
    }
    gx->dirtyState = 0;
}

void fn_800B928C(u32 primitive, u32 vertexFormat, u16 vertexCount) {
    if (gx->dirtyState != 0) {
        fn_800B91EC();
    }

    if (*(u32*)&gx->field_000 == 0) {
        __GXSendFlushPrim();
    }

    GX_FIFO_U8 = vertexFormat | primitive;
    GX_FIFO_U16 = vertexCount;
}

void __GXSendFlushPrim(void) {
    GXData_800B857C* p = gx;
    u16 nverts = p->field_004;
    u16 stride = p->field_006;
    u32 bytes = nverts * stride;
    u32 i;

    GX_FIFO_U8 = 0x98;
    GX_FIFO_U16 = nverts;

    for (i = 0; i < bytes; i += 4) {
        GX_FIFO_U32 = 0;
    }

    gx->field_002 = 1;
}

void fn_800B9404(u32 left, u32 top) {
    GXData_800B857C* p = gx;

    p->field_07C = (p->field_07C & ~0xFFU) | (left & 0xFF);
    p->field_07C = (p->field_07C & ~0x70000U) | (top << 16);
    GX_BP_REG(p->field_07C);
    p->field_002 = 0;
}

void fn_800B944C(u32 right, u32 bottom) {
    GXData_800B857C* p = gx;

    p->field_07C = (p->field_07C & ~0xFF00U) | ((right & 0xFF) << 8);
    p->field_07C = (p->field_07C & ~0x380000U) | (bottom << 19);
    GX_BP_REG(p->field_07C);
    p->field_002 = 0;
}

void fn_800B9494(u32 chan, u32 enable0, u32 enable1) {
    gx->field_0B8[chan] = (gx->field_0B8[chan] & ~0x40000U) | ((enable0 & 0xFF) << 18);
    gx->field_0B8[chan] = (gx->field_0B8[chan] & ~0x80000U) | ((enable1 & 0xFF) << 19);
    GX_BP_REG(gx->field_0B8[chan]);
    gx->field_002 = 0;
}

void fn_800B94F0(s32 value) {
    switch (value) {
    case 1:
        value = 2;
        break;
    case 2:
        value = 1;
        break;
    }

    gx->field_204 = (gx->field_204 & ~0xC000U) | (value << 14);
    gx->dirtyState |= 4;
}

void fn_800B953C(u32 value) {
    GXData_800B857C* p = gx;

    p->field_204 = (p->field_204 & ~0x80000U) | ((value & 0xFF) << 19);
    GX_BP_REG(0xFE080000);
    GX_BP_REG(p->field_204);
}

void fn_800B9578(void) {
    GXData_800B857C* p = gx;

    GX_FIFO_U8 = 0x61;
    GX_FIFO_U32 = p->field_204;
    p->field_002 = 0;
}

void fn_800B959C(u16 left, u16 top, u16 width, u16 height) {
    gx->field_1E0 = 0;
    gx->field_1E0 = (gx->field_1E0 & ~0x3FFU) | left;
    gx->field_1E0 = (gx->field_1E0 & ~0xFFC00U) | (top << 10);
    gx->field_1E0 = (gx->field_1E0 & 0xFFFFFFU) | 0x49000000;

    gx->field_1E4 = 0;
    gx->field_1E4 = (gx->field_1E4 & ~0x3FFU) | (width - 1);
    gx->field_1E4 = (gx->field_1E4 & ~0xFFC00U) | ((height - 1) << 10);
    gx->field_1E4 = (gx->field_1E4 & 0xFFFFFFU) | 0x4A000000;
}

void fn_800B962C(u16 left, u16 top, u16 width, u16 height) {
    gx->field_1F0 = 0;
    gx->field_1F0 = (gx->field_1F0 & ~0x3FFU) | left;
    gx->field_1F0 = (gx->field_1F0 & ~0xFFC00U) | (top << 10);
    gx->field_1F0 = (gx->field_1F0 & 0xFFFFFFU) | 0x49000000;

    gx->field_1F4 = 0;
    gx->field_1F4 = (gx->field_1F4 & ~0x3FFU) | (width - 1);
    gx->field_1F4 = (gx->field_1F4 & ~0xFFC00U) | ((height - 1) << 10);
    gx->field_1F4 = (gx->field_1F4 & 0xFFFFFFU) | 0x4A000000;
}

void fn_800B96BC(u32 value) {
    GXData_800B857C* p;
    u32* regp;

    p = *(GXData_800B857C**)&gx;
    p->field_1E8 = 0;
    regp = &p->field_1E8;
    *regp = (*regp & ~0x3FFU) |
            (((s32)((value & 0x7FFFU) << 1) << 16) >> 21);
    *regp = (*regp & 0xFFFFFFU) | 0x4D000000U;
}

void fn_800B96F8(u16 width, u16 height, s32 format, u8 mipmap) {
    u32 rowTiles;
    u32 columnTiles;
    u32 planes;
    u32 peFormat;
    u32 peFormatHigh;

    gx->field_200 = 0;
    peFormat = format & 0xF;

    if (format == 0x13) {
        peFormat = 0xB;
    }

    switch (format) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 0x26:
        gx->field_1FC = (gx->field_1FC & ~0x18000U) | 0x18000;
        break;
    default:
        gx->field_1FC = (gx->field_1FC & ~0x18000U) | 0x10000;
        break;
    }

    gx->field_200 = (0x10 == (format & 0x10));
    peFormatHigh = (peFormat >> 3) & 1;
    !peFormat;
    gx->field_1FC = (gx->field_1FC & ~8U) | (peFormatHigh << 3);
    peFormat &= 7;

    __GetImageTileCount(format, width, height, &rowTiles, &columnTiles, &planes);

    gx->field_1F8 = 0;
    gx->field_1F8 = (gx->field_1F8 & ~0x3FFU) | (rowTiles * planes);
    gx->field_1F8 = (gx->field_1F8 & 0xFFFFFFU) | 0x4D000000U;
    gx->field_1FC = (gx->field_1FC & ~0x200U) | ((mipmap & 0xFF) << 9);
    gx->field_1FC = (gx->field_1FC & ~0x70U) | (peFormat << 4);
}

void fn_800B984C(u32 value) {
    gx->field_1EC = (gx->field_1EC & ~0x3000U) | (value << 12);
    gx->field_1FC = gx->field_1FC & ~0x3000U;
}

void fn_800B9874(u32 value) {
    GXData_800B857C* p = gx;
    u32 bit0;
    u32 bit1;
    u8 out0;
    u8 out1;

    bit0 = value & 1;
    bit1 = value & 2;
    out0 = (bit0 == 1);
    out1 = (bit1 == 2);
    p->field_1EC = (p->field_1EC & ~1U) | out0;
    p->field_1EC = (p->field_1EC & ~2U) | (out1 << 1);
    p->field_1FC = (p->field_1FC & ~1U) | out0;
    p->field_1FC = (p->field_1FC & ~2U) | (out1 << 1);
}

static u32 __GXGetNumXfbLines(u32 efbHt, u32 iScale) {
    u32 count;
    u32 realHt;
    u32 iScaleD;

    count = (efbHt - 1) * 0x100;
    realHt = (count / iScale) + 1;

    iScaleD = iScale;

    if (iScaleD > 0x80 && iScaleD < 0x100) {
        while (iScaleD % 2 == 0) {
            iScaleD /= 2;
        }

        if (efbHt % iScaleD == 0) {
            realHt++;
        }
    }

    if (realHt > 0x400) {
        realHt = 0x400;
    }

    return realHt;
}

f32 GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight) {
    f32 fScale;
    f32 yScale;
    u32 iScale;
    u32 tgtHt;
    u32 realHt;

    tgtHt = xfbHeight;
    yScale = (f32)xfbHeight / (f32)efbHeight;
    iScale = (u32)(256.0f / yScale) & 0x1FF;
    realHt = __GXGetNumXfbLines(efbHeight, iScale);

    while (realHt > xfbHeight) {
        tgtHt--;
        yScale = (f32)tgtHt / (f32)efbHeight;
        iScale = (u32)(256.0f / yScale) & 0x1FF;
        realHt = __GXGetNumXfbLines(efbHeight, iScale);
    }

    fScale = yScale;
    while (realHt < xfbHeight) {
        fScale = yScale;
        tgtHt++;
        yScale = (f32)tgtHt / (f32)efbHeight;
        iScale = (u32)(256.0f / yScale) & 0x1FF;
        realHt = __GXGetNumXfbLines(efbHeight, iScale);
    }

    return fScale;
}

static inline u32 gxGetNumXfbLines(u32 height, u32 scale) {
    u32 count;
    u32 result;
    u32 scaleDivisor;

    count = (height - 1) * 0x100;
    result = (count / scale) + 1;
    scaleDivisor = scale;

    if (scaleDivisor > 0x80 && scaleDivisor < 0x100) {
        while ((scaleDivisor & 1) == 0) {
            scaleDivisor >>= 1;
        }
        if ((height % scaleDivisor) == 0) {
            result++;
        }
    }
    if (result > 0x400) {
        result = 0x400;
    }
    return result;
}

u32 fn_800B9B14(f32 scale) {
    u32 yScale;
    u8 nonUnityScale;
    u32 height;
    GXData_800B857C* p = gx;

    yScale = __cvt_fp2unsigned(256.0F / scale) & 0x1FF;
    GX_BP_REG(0x4E000000 | yScale);
    p->field_002 = 0;
    nonUnityScale = (yScale != 0x100);
    p->field_1EC = (p->field_1EC & ~0x400U) | (nonUnityScale << 10);
    height = ((p->field_1E4 >> 10) & 0x3FF) + 1;
    return gxGetNumXfbLines(height, yScale);
}

typedef struct GXColor_800B9BDC {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor_800B9BDC;

void fn_800B9BDC(GXColor_800B9BDC color, u32 clearZ) {
    u32 reg;
    GXData_800B857C* p = gx;

    reg = 0;
    reg = (reg & ~0xFFU) | color.r;
    reg = (reg & ~0xFF00U) | (color.a << 8);
    reg = (reg & 0xFFFFFFU) | 0x4F000000U;
    GX_BP_REG(reg);
    reg = 0;
    reg = (reg & ~0xFFU) | color.b;
    reg = (reg & ~0xFF00U) | (color.g << 8);
    reg = (reg & 0xFFFFFFU) | 0x50000000U;
    GX_BP_REG(reg);
    reg = 0;
    reg = (reg & ~0xFFFFFFU) | (clearZ & 0xFFFFFFU);
    reg = (reg & 0xFFFFFFU) | 0x51000000U;
    GX_BP_REG(reg);
    p->field_002 = 0;
}

#define GX_GET_REG_FIELD(reg, size, shift) \
    ((s32)((reg) >> (shift)) & ((1 << (size)) - 1))
#define GX_SET_REG_FIELD(reg, size, shift, value)                                  \
    ((reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) |                   \
             ((u32)(value) << (shift)))

void fn_800B9C44(u8 aa, const u8 samplePattern[12][2], u8 vf,
                 const u8 vfilter[7]) {
    u32 msLoc[4];
    u32 coeff0;
    u32 coeff1;

    if (aa != 0) {
        msLoc[0] = 0;
        GX_SET_REG_FIELD(msLoc[0], 4, 0, samplePattern[0][0]);
        GX_SET_REG_FIELD(msLoc[0], 4, 4, samplePattern[0][1]);
        GX_SET_REG_FIELD(msLoc[0], 4, 8, samplePattern[1][0]);
        GX_SET_REG_FIELD(msLoc[0], 4, 12, samplePattern[1][1]);
        GX_SET_REG_FIELD(msLoc[0], 4, 16, samplePattern[2][0]);
        GX_SET_REG_FIELD(msLoc[0], 4, 20, samplePattern[2][1]);
        GX_SET_REG_FIELD(msLoc[0], 8, 24, 1);

        msLoc[1] = 0;
        GX_SET_REG_FIELD(msLoc[1], 4, 0, samplePattern[3][0]);
        GX_SET_REG_FIELD(msLoc[1], 4, 4, samplePattern[3][1]);
        GX_SET_REG_FIELD(msLoc[1], 4, 8, samplePattern[4][0]);
        GX_SET_REG_FIELD(msLoc[1], 4, 12, samplePattern[4][1]);
        GX_SET_REG_FIELD(msLoc[1], 4, 16, samplePattern[5][0]);
        GX_SET_REG_FIELD(msLoc[1], 4, 20, samplePattern[5][1]);
        GX_SET_REG_FIELD(msLoc[1], 8, 24, 2);

        msLoc[2] = 0;
        GX_SET_REG_FIELD(msLoc[2], 4, 0, samplePattern[6][0]);
        GX_SET_REG_FIELD(msLoc[2], 4, 4, samplePattern[6][1]);
        GX_SET_REG_FIELD(msLoc[2], 4, 8, samplePattern[7][0]);
        GX_SET_REG_FIELD(msLoc[2], 4, 12, samplePattern[7][1]);
        GX_SET_REG_FIELD(msLoc[2], 4, 16, samplePattern[8][0]);
        GX_SET_REG_FIELD(msLoc[2], 4, 20, samplePattern[8][1]);
        GX_SET_REG_FIELD(msLoc[2], 8, 24, 3);

        msLoc[3] = 0;
        GX_SET_REG_FIELD(msLoc[3], 4, 0, samplePattern[9][0]);
        GX_SET_REG_FIELD(msLoc[3], 4, 4, samplePattern[9][1]);
        GX_SET_REG_FIELD(msLoc[3], 4, 8, samplePattern[10][0]);
        GX_SET_REG_FIELD(msLoc[3], 4, 12, samplePattern[10][1]);
        GX_SET_REG_FIELD(msLoc[3], 4, 16, samplePattern[11][0]);
        GX_SET_REG_FIELD(msLoc[3], 4, 20, samplePattern[11][1]);
        GX_SET_REG_FIELD(msLoc[3], 8, 24, 4);
    } else {
        msLoc[0] = 0x01666666;
        msLoc[1] = 0x02666666;
        msLoc[2] = 0x03666666;
        msLoc[3] = 0x04666666;
    }

    GX_BP_REG(msLoc[0]);
    GX_BP_REG(msLoc[1]);
    GX_BP_REG(msLoc[2]);
    GX_BP_REG(msLoc[3]);

    coeff0 = 0;
    GX_SET_REG_FIELD(coeff0, 8, 24, 0x53);
    coeff1 = 0;
    GX_SET_REG_FIELD(coeff1, 8, 24, 0x54);

    if (vf != 0) {
        GX_SET_REG_FIELD(coeff0, 6, 0, vfilter[0]);
        GX_SET_REG_FIELD(coeff0, 6, 6, vfilter[1]);
        GX_SET_REG_FIELD(coeff0, 6, 12, vfilter[2]);
        GX_SET_REG_FIELD(coeff0, 6, 18, vfilter[3]);
        GX_SET_REG_FIELD(coeff1, 6, 0, vfilter[4]);
        GX_SET_REG_FIELD(coeff1, 6, 6, vfilter[5]);
        GX_SET_REG_FIELD(coeff1, 6, 12, vfilter[6]);
    } else {
        GX_SET_REG_FIELD(coeff0, 6, 0, 0);
        GX_SET_REG_FIELD(coeff0, 6, 6, 0);
        GX_SET_REG_FIELD(coeff0, 6, 12, 21);
        GX_SET_REG_FIELD(coeff0, 6, 18, 22);
        GX_SET_REG_FIELD(coeff1, 6, 0, 21);
        GX_SET_REG_FIELD(coeff1, 6, 6, 0);
        GX_SET_REG_FIELD(coeff1, 6, 12, 0);
    }

    {
        GXData_800B857C* p = gx;

        GX_BP_REG(coeff0);
        GX_BP_REG(coeff1);
        p->field_002 = 0;
    }
}

void fn_800B9E6C(u32 value) {
    GXData_800B857C* p = gx;

    p->field_1EC = (p->field_1EC & ~0x180U) | (value << 7);
}

void fn_800B9E88(void* dest, u8 clear) {
    u32 reg;
    u32 tempPeCtrl;
    u32 phyAddr;
    u8 changePeCtrl;

    if (clear) {
        reg = gx->field_1D8;
        GX_SET_REG_FIELD(reg, 1, 0, 1);
        GX_SET_REG_FIELD(reg, 3, 1, 7);
        GX_BP_REG(reg);

        reg = gx->field_1D0;
        GX_SET_REG_FIELD(reg, 1, 0, 0);
        GX_SET_REG_FIELD(reg, 1, 1, 0);
        GX_BP_REG(reg);
    }

    changePeCtrl = 0;

    if ((clear || (u32)GX_GET_REG_FIELD(gx->field_1DC, 3, 0) == 3) &&
        (u32)GX_GET_REG_FIELD(gx->field_1DC, 1, 6) == 1) {
        changePeCtrl = 1;
        tempPeCtrl = gx->field_1DC;
        GX_SET_REG_FIELD(tempPeCtrl, 1, 6, 0);
        GX_BP_REG(tempPeCtrl);
    }

    GX_BP_REG(gx->field_1E0);
    GX_BP_REG(gx->field_1E4);
    GX_BP_REG(gx->field_1E8);

    phyAddr = (u32)dest & 0x3FFFFFFF;
    reg = 0;
    GX_SET_REG_FIELD(reg, 21, 0, phyAddr >> 5);
    GX_SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_BP_REG(reg);

    GX_SET_REG_FIELD(gx->field_1EC, 1, 11, clear);
    GX_SET_REG_FIELD(gx->field_1EC, 1, 14, 1);
    GX_SET_REG_FIELD(gx->field_1EC, 8, 24, 0x52);
    GX_BP_REG(gx->field_1EC);

    if (clear) {
        GX_BP_REG(gx->field_1D8);
        GX_BP_REG(gx->field_1D0);
    }

    if (changePeCtrl) {
        GXData_800B857C* p = gx;
        GX_BP_REG(p->field_1DC);
    }

    gx->field_002 = 0;
}

void fn_800B9FE4(void* dest, u8 clear) {
    u32 reg;
    u32 tempPeCtrl;
    u32 physicalAddress;
    u8 changePeCtrl;

    if (clear) {
        reg = gx->field_1D8;
        GX_SET_REG_FIELD(reg, 1, 0, 1);
        GX_SET_REG_FIELD(reg, 3, 1, 7);
        GX_BP_REG(reg);

        reg = gx->field_1D0;
        GX_SET_REG_FIELD(reg, 1, 0, 0);
        GX_SET_REG_FIELD(reg, 1, 1, 0);
        GX_BP_REG(reg);
    }

    changePeCtrl = 0;
    tempPeCtrl = gx->field_1DC;

    if (gx->field_200 && ((tempPeCtrl & 7) != 3)) {
        changePeCtrl = 1;
        GX_SET_REG_FIELD(tempPeCtrl, 3, 0, 3);
    }

    if ((clear || ((tempPeCtrl & 7) == 3)) &&
        (((tempPeCtrl >> 6) & 1) == 1)) {
        changePeCtrl = 1;
        GX_SET_REG_FIELD(tempPeCtrl, 1, 6, 0);
    }

    if (changePeCtrl) {
        GX_BP_REG(tempPeCtrl);
    }

    GX_BP_REG(gx->field_1F0);
    GX_BP_REG(gx->field_1F4);
    GX_BP_REG(gx->field_1F8);

    physicalAddress = (u32)dest & 0x3FFFFFFF;
    reg = 0;
    GX_SET_REG_FIELD(reg, 21, 0, physicalAddress >> 5);
    GX_SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_BP_REG(reg);

    GX_SET_REG_FIELD(gx->field_1FC, 1, 11, clear);
    GX_SET_REG_FIELD(gx->field_1FC, 1, 14, 0);
    GX_SET_REG_FIELD(gx->field_1FC, 8, 24, 0x52);
    GX_BP_REG(gx->field_1FC);

    if (clear) {
        GX_BP_REG(gx->field_1D8);
        GX_BP_REG(gx->field_1D0);
    }

    if (changePeCtrl) {
        GXData_800B857C* p = gx;
        GX_BP_REG(p->field_1DC);
    }

    gx->field_002 = 0;
}

void GXClearBoundingBox(void) {
    GXData_800B857C* p = gx;

    GX_FIFO_U8 = 0x61;
    GX_FIFO_U32 = 0x550003FF;
    GX_FIFO_U8 = 0x61;
    GX_FIFO_U32 = 0x560003FF;
    p->field_002 = 0;
}
#endif
