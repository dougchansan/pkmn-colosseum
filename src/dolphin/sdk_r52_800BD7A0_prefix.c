/**
 * @file sdk_r52_800BD7A0_prefix.c
 * @brief GXTransform.c tail (GXSetScissorBoxOffset, GXSetClipMode,
 * __GXSetMatrixIndex) and GXPerf.c head (GXSetGPMetric, GXReadGPMetric), 0x800BD7A0 -
 * 0x800BE30C, with the GXPerf.c .data 0x80313628 - 0x80313770.
 *
 * Standalone carve: the range file supplies only declarations here (no
 * section guard is defined). Its bodies are the range file's, reordered to
 * address order.
 */

#include "src/dolphin/sdk_range_800BB30C.c"

void GXSetScissor(u32 xOrigin, u32 yOrigin, u32 width, u32 height) {
    GXData_800BB30C* p = gx;
    u32* scissorTL;
    u32* scissorBR;
    u32 top;
    u32 left;
    u32 bottom;
    u32 right;

    top = 0x156 + yOrigin;
    left = xOrigin + 0x156;
    bottom = top + height - 1;
    right = left + width - 1;
    scissorTL = &p->scissorTL;
    scissorBR = &p->scissorBR;
    *scissorTL = (*scissorTL & ~0x7FFU) | top;
    *scissorTL = (*scissorTL & ~0x7FF000U) | (left << 12);
    *scissorBR = (*scissorBR & ~0x7FFU) | bottom;
    *scissorBR = (*scissorBR & ~0x7FF000U) | (right << 12);
    GX_BP_REG(p->scissorTL);
    GX_BP_REG(p->scissorBR);
    p->field_002 = 0;
}

void GXSetScissorBoxOffset(u32 arg0, u32 arg1) {
    u32 reg = 0;
    u32 hx;
    u32 hy;

    hx = (arg0 + 0x156U) >> 1;
    hy = (arg1 + 0x156U) >> 1;
    reg = (reg & ~0x3FFU) | hx;
    reg = (reg & ~0xFFC00U) | (hy << 10);
    reg = (reg & 0xFFFFFFU) | 0x59000000U;
    GX_BP_REG(reg);
    gx->field_002 = 0;
}

void GXSetClipMode(u32 clipMode) {
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x1005;
    GX_FIFO_U32 = clipMode;
    gx->field_002 = 1;
}

void __GXSetMatrixIndex(s32 value) {
    u32 matrixIndex;

    if (value < 5) {
        GX_FIFO_U8 = 8;
        GX_FIFO_U8 = 0x30;
        matrixIndex = gx->mtxIdx0;
        GX_FIFO_U32 = matrixIndex;
        GX_FIFO_U8 = 0x10;
        GX_FIFO_U32 = 0x1018;
        GX_FIFO_U32 = matrixIndex;
    } else {
        GX_FIFO_U8 = 8;
        GX_FIFO_U8 = 0x40;
        matrixIndex = gx->mtxIdx1;
        GX_FIFO_U32 = matrixIndex;
        GX_FIFO_U8 = 0x10;
        GX_FIFO_U32 = 0x1019;
        GX_FIFO_U32 = matrixIndex;
    }

    gx->field_002 = 1;
}

void fn_800BD91C(GXPerf0_800BB30C perf0, GXPerf1_800BB30C perf1) {
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
        GX_XF_REG(6, 0);
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
        GX_BP_REG(0x23000000);
        break;
    case GX_PERF0_QUAD_0CVG:
    case GX_PERF0_QUAD_NON0CVG:
    case GX_PERF0_QUAD_1CVG:
    case GX_PERF0_QUAD_2CVG:
    case GX_PERF0_QUAD_3CVG:
    case GX_PERF0_QUAD_4CVG:
    case GX_PERF0_AVG_QUAD_CNT:
        GX_BP_REG(0x24000000);
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
        GX_BP_REG(0x67000000);
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
        __cpReg[3] = 0;
        break;
    case GX_PERF1_NONE:
        break;
    }

    gx->perf0 = perf0;
    switch (gx->perf0) {
    case GX_PERF0_VERTICES:
        GX_XF_REG(6, 0x273);
        break;
    case GX_PERF0_CLIP_VTX:
        GX_XF_REG(6, 0x14A);
        break;
    case GX_PERF0_CLIP_CLKS:
        GX_XF_REG(6, 0x16B);
        break;
    case GX_PERF0_XF_WAIT_IN:
        GX_XF_REG(6, 0x84);
        break;
    case GX_PERF0_XF_WAIT_OUT:
        GX_XF_REG(6, 0xC6);
        break;
    case GX_PERF0_XF_XFRM_CLKS:
        GX_XF_REG(6, 0x210);
        break;
    case GX_PERF0_XF_LIT_CLKS:
        GX_XF_REG(6, 0x252);
        break;
    case GX_PERF0_XF_BOT_CLKS:
        GX_XF_REG(6, 0x231);
        break;
    case GX_PERF0_XF_REGLD_CLKS:
        GX_XF_REG(6, 0x1AD);
        break;
    case GX_PERF0_XF_REGRD_CLKS:
        GX_XF_REG(6, 0x1CE);
        break;
    case GX_PERF0_CLOCKS:
        GX_XF_REG(6, 0x21);
        break;
    case GX_PERF0_CLIP_RATIO:
        GX_XF_REG(6, 0x153);
        break;
    case GX_PERF0_TRIANGLES:
        GX_BP_REG(0x2300AE7F);
        break;
    case GX_PERF0_TRIANGLES_CULLED:
        GX_BP_REG(0x23008E7F);
        break;
    case GX_PERF0_TRIANGLES_PASSED:
        GX_BP_REG(0x23009E7F);
        break;
    case GX_PERF0_TRIANGLES_SCISSORED:
        GX_BP_REG(0x23001E7F);
        break;
    case GX_PERF0_TRIANGLES_0TEX:
        GX_BP_REG(0x2300AC3F);
        break;
    case GX_PERF0_TRIANGLES_1TEX:
        GX_BP_REG(0x2300AC7F);
        break;
    case GX_PERF0_TRIANGLES_2TEX:
        GX_BP_REG(0x2300ACBF);
        break;
    case GX_PERF0_TRIANGLES_3TEX:
        GX_BP_REG(0x2300ACFF);
        break;
    case GX_PERF0_TRIANGLES_4TEX:
        GX_BP_REG(0x2300AD3F);
        break;
    case GX_PERF0_TRIANGLES_5TEX:
        GX_BP_REG(0x2300AD7F);
        break;
    case GX_PERF0_TRIANGLES_6TEX:
        GX_BP_REG(0x2300ADBF);
        break;
    case GX_PERF0_TRIANGLES_7TEX:
        GX_BP_REG(0x2300ADFF);
        break;
    case GX_PERF0_TRIANGLES_8TEX:
        GX_BP_REG(0x2300AE3F);
        break;
    case GX_PERF0_TRIANGLES_0CLR:
        GX_BP_REG(0x2300A27F);
        break;
    case GX_PERF0_TRIANGLES_1CLR:
        GX_BP_REG(0x2300A67F);
        break;
    case GX_PERF0_TRIANGLES_2CLR:
        GX_BP_REG(0x2300AA7F);
        break;
    case GX_PERF0_QUAD_0CVG:
        GX_BP_REG(0x2402C0C6);
        break;
    case GX_PERF0_QUAD_NON0CVG:
        GX_BP_REG(0x2402C16B);
        break;
    case GX_PERF0_QUAD_1CVG:
        GX_BP_REG(0x2402C0E7);
        break;
    case GX_PERF0_QUAD_2CVG:
        GX_BP_REG(0x2402C108);
        break;
    case GX_PERF0_QUAD_3CVG:
        GX_BP_REG(0x2402C129);
        break;
    case GX_PERF0_QUAD_4CVG:
        GX_BP_REG(0x2402C14A);
        break;
    case GX_PERF0_AVG_QUAD_CNT:
        GX_BP_REG(0x2402C1AD);
        break;
    case GX_PERF0_NONE:
        break;
    }

    gx->perf1 = perf1;
    switch (gx->perf1) {
    case GX_PERF1_TEXELS:
        GX_BP_REG(0x67000042);
        break;
    case GX_PERF1_TX_IDLE:
        GX_BP_REG(0x67000084);
        break;
    case GX_PERF1_TX_REGS:
        GX_BP_REG(0x67000063);
        break;
    case GX_PERF1_TX_MEMSTALL:
        GX_BP_REG(0x67000129);
        break;
    case GX_PERF1_TC_MISS:
        GX_BP_REG(0x67000252);
        break;
    case GX_PERF1_CLOCKS:
        GX_BP_REG(0x67000021);
        break;
    case GX_PERF1_TC_CHECK1_2:
        GX_BP_REG(0x6700014B);
        break;
    case GX_PERF1_TC_CHECK3_4:
        GX_BP_REG(0x6700018D);
        break;
    case GX_PERF1_TC_CHECK5_6:
        GX_BP_REG(0x670001CF);
        break;
    case GX_PERF1_TC_CHECK7_8:
        GX_BP_REG(0x67000211);
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
        __cpReg[3] = 2;
        break;
    case GX_PERF1_CALL_REQ:
        __cpReg[3] = 3;
        break;
    case GX_PERF1_VC_MISS_REQ:
        __cpReg[3] = 4;
        break;
    case GX_PERF1_CP_ALL_REQ:
        __cpReg[3] = 5;
        break;
    case GX_PERF1_NONE:
        break;
    }

    gx->field_002 = 0;
}

/* RULE-EXCEPTION(user-approved): named stand-in for another function's
 * compiler switch table - see docs/RULE_EXCEPTIONS.md.
 * fn_800BE164's switch table (GXPerf.c, unlinked). It starts at 0x80313714,
 * four-byte aligned only after this object's two tables, so it is defined
 * here rather than in its own 8-aligned data object. */
void* jumptable_80313714[23] = {
    (void*)((u8*)fn_800BE164 + 0x128),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x134),
    (void*)((u8*)fn_800BE164 + 0x144),
    (void*)((u8*)fn_800BE164 + 0x158),
    (void*)((u8*)fn_800BE164 + 0x16C),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x188),
    (void*)((u8*)fn_800BE164 + 0x188),
    (void*)((u8*)fn_800BE164 + 0x188),
    (void*)((u8*)fn_800BE164 + 0x188),
    (void*)((u8*)fn_800BE164 + 0x180),
    (void*)((u8*)fn_800BE164 + 0x190),
};

/* GXReadGPMetric */
static inline u32 ReadCPCounter(u32 regAddrL, u32 regAddrH)
{
    volatile u16* hi;
    volatile u16* lo;
    u32 ctrH0;
    u32 ctrH1;
    u32 ctrL;

    hi = &__cpReg[regAddrH];
    ctrH0 = *hi;
    lo = &__cpReg[regAddrL];
    do {
        ctrH1 = ctrH0;
        ctrL = *lo;
        ctrH0 = *hi;
    } while (ctrH0 != ctrH1);
    return (ctrH0 << 0x10) | ctrL;
}

/* RULE-EXCEPTION(user-approved): local scheduling off, as upstream GXPerf.c
   wraps its counter readers — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma scheduling off
void fn_800BE164(u32* count0, u32* count1)
{
    u32 cpCounter0;
    u32 cpCounter1;
    u32 cpCounter2;
    u32 cpCounter3;

    cpCounter0 = ReadCPCounter(32, 33);
    cpCounter1 = ReadCPCounter(34, 35);
    cpCounter2 = ReadCPCounter(36, 37);
    cpCounter3 = ReadCPCounter(38, 39);

    switch (gx->perf0) {
    case GX_PERF0_CLIP_RATIO:
        *count0 = cpCounter1 * 1000 / cpCounter0;
        break;
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
    case GX_PERF0_QUAD_0CVG:
    case GX_PERF0_QUAD_NON0CVG:
    case GX_PERF0_QUAD_1CVG:
    case GX_PERF0_QUAD_2CVG:
    case GX_PERF0_QUAD_3CVG:
    case GX_PERF0_QUAD_4CVG:
    case GX_PERF0_AVG_QUAD_CNT:
    case GX_PERF0_CLOCKS:
        *count0 = cpCounter0;
        break;
    case GX_PERF0_NONE:
        *count0 = 0;
        break;
    default:
        *count0 = 0;
        break;
    }

    switch (gx->perf1) {
    case GX_PERF1_TEXELS:
        *count1 = cpCounter3 * 4;
        break;
    case GX_PERF1_TC_CHECK1_2:
        *count1 = cpCounter2 + cpCounter3 * 2;
        break;
    case GX_PERF1_TC_CHECK3_4:
        *count1 = cpCounter2 * 3 + cpCounter3 * 4;
        break;
    case GX_PERF1_TC_CHECK5_6:
        *count1 = cpCounter2 * 5 + cpCounter3 * 6;
        break;
    case GX_PERF1_TC_CHECK7_8:
        *count1 = cpCounter2 * 7 + cpCounter3 * 8;
        break;
    case GX_PERF1_TX_IDLE:
    case GX_PERF1_TX_REGS:
    case GX_PERF1_TX_MEMSTALL:
    case GX_PERF1_TC_MISS:
    case GX_PERF1_VC_ELEMQ_FULL:
    case GX_PERF1_VC_MISSQ_FULL:
    case GX_PERF1_VC_MEMREQ_FULL:
    case GX_PERF1_VC_STATUS7:
    case GX_PERF1_VC_MISSREP_FULL:
    case GX_PERF1_VC_STREAMBUF_LOW:
    case GX_PERF1_VC_ALL_STALLS:
    case GX_PERF1_VERTICES:
    case GX_PERF1_CLOCKS:
        *count1 = cpCounter3;
        break;
    case GX_PERF1_FIFO_REQ:
    case GX_PERF1_CALL_REQ:
    case GX_PERF1_VC_MISS_REQ:
    case GX_PERF1_CP_ALL_REQ:
        *count1 = cpCounter2;
        break;
    case GX_PERF1_NONE:
        *count1 = 0;
        break;
    default:
        *count1 = 0;
        break;
    }
}
#pragma pop
