/**
 * @file sdk_candidate_800BD454.c
 * @brief GXTransform.c run 0x800BD454 - 0x800BD554: GXGetProjectionv (the
 *        projection read-back, GXGetProjectionv), GXLoadPosMtxImm and
 *        GXLoadNrmMtxImm.
 */
#include "dolphin/gx/GXInternal.h"

void GXGetProjectionv(f32* projection)
{
    GXData* data = gx;

    projection[0] = data->field_420;
    projection[1] = data->field_424;
    projection[2] = data->field_428;
    projection[3] = data->field_42C;
    projection[4] = data->field_430;
    projection[5] = data->field_434;
    projection[6] = data->field_438;
}

/* The FIFO copies are the vendor's paired-single inline asm helpers;
   evidence: docs/asm_evidence/gx_transform.md */
static inline void WriteMTXPS4x3(register volatile void* dst, register f32 src[3][4])
{
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm {
        psq_l   ps_0, 0(src), 0, 0
        psq_l   ps_1, 8(src), 0, 0
        psq_l   ps_2, 16(src), 0, 0
        psq_l   ps_3, 24(src), 0, 0
        psq_l   ps_4, 32(src), 0, 0
        psq_l   ps_5, 40(src), 0, 0
        psq_st  ps_0, 0(dst), 0, 0
        psq_st  ps_1, 0(dst), 0, 0
        psq_st  ps_2, 0(dst), 0, 0
        psq_st  ps_3, 0(dst), 0, 0
        psq_st  ps_4, 0(dst), 0, 0
        psq_st  ps_5, 0(dst), 0, 0
    }
}

void GXLoadPosMtxImm(f32 mtx[3][4], u32 id)
{
    u32 reg;
    u32 addr;

    addr = id * 4;
    reg = addr | 0xB0000;
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = reg;
    WriteMTXPS4x3(&GX_FIFO_U32, mtx);
}

/* Paired-single inline asm helper; evidence: docs/asm_evidence/gx_transform.md */
static inline void WriteMTXPS3x3(register volatile void* dst, register f32 src[3][4])
{
    register f32 ps_0, ps_1, ps_2, ps_3, ps_4, ps_5;

    asm {
        psq_l   ps_0, 0(src), 0, 0
        lfs     ps_1, 8(src)
        psq_l   ps_2, 16(src), 0, 0
        lfs     ps_3, 24(src)
        psq_l   ps_4, 32(src), 0, 0
        lfs     ps_5, 40(src)
        psq_st  ps_0, 0(dst), 0, 0
        stfs    ps_1, 0(dst)
        psq_st  ps_2, 0(dst), 0, 0
        stfs    ps_3, 0(dst)
        psq_st  ps_4, 0(dst), 0, 0
        stfs    ps_5, 0(dst)
    }
}

void GXLoadNrmMtxImm(f32 mtx[3][4], u32 id)
{
    u32 reg;
    u32 addr;

    addr = id * 3 + 0x400;
    reg = addr | 0x80000;
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = reg;
    WriteMTXPS3x3(&GX_FIFO_U32, mtx);
}
