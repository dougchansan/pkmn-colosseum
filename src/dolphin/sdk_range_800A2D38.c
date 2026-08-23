/**
 * @file sdk_range_800A2D38.c
 * @brief dolphin-sdk code, 0x800A2D38 - 0x800A3EB0 (37 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). Functions not present here remain asm-only until
 * matched; the range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef f32 Mtx[3][4];
typedef f32 Mtx44[4][4];

typedef struct Vec {
    f32 x;
    f32 y;
    f32 z;
} Vec;

typedef struct Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Quaternion;

extern const f32 lbl_8047C288;
extern const f32 lbl_8047C28C;
extern const f32 lbl_8047C290;
extern const f32 lbl_8047C294;
extern const f32 lbl_8047C2A8;
extern const f32 lbl_8047C2AC;
extern const f32 lbl_8047C2B0;
extern const f32 lbl_8047C2B4;
extern const f32 lbl_8047C2B8;
extern const f32 lbl_8047C2BC;
extern const f32 lbl_804789B0[];

extern f32 tanf(f32 x);

#if !defined(SDK_800A37CC_SUFFIX_ACTIVE)
#if !defined(SDK_MTXLOOKAT_BEFORE_EXCLUDE)
#if !defined(SDK_MTX_IDENTITY_COPY_EXCLUDE)
void PSMTXIdentity(register Mtx m)
{
    register f32 zero = lbl_8047C28C;
    register f32 one = lbl_8047C288;
    register f32 zeroOne;
    register f32 oneZero;

    asm {
        psq_st zero, 8(m), 0, 0
        ps_merge01 zeroOne, zero, one
        psq_st zero, 24(m), 0, 0
        ps_merge10 oneZero, one, zero
        psq_st zero, 32(m), 0, 0
        psq_st zeroOne, 16(m), 0, 0
        psq_st oneZero, 0(m), 0, 0
        psq_st oneZero, 40(m), 0, 0
    }
}

asm void PSMTXCopy(const register Mtx src, register Mtx dst)
{
    nofralloc
    psq_l f0, 0(src), 0, 0
    psq_st f0, 0(dst), 0, 0
    psq_l f1, 8(src), 0, 0
    psq_st f1, 8(dst), 0, 0
    psq_l f2, 16(src), 0, 0
    psq_st f2, 16(dst), 0, 0
    psq_l f3, 24(src), 0, 0
    psq_st f3, 24(dst), 0, 0
    psq_l f4, 32(src), 0, 0
    psq_st f4, 32(dst), 0, 0
    psq_l f5, 40(src), 0, 0
    psq_st f5, 40(dst), 0, 0
    blr
}

asm void PSMTXConcat(const register Mtx mA, const register Mtx mB,
                     register Mtx mAB)
{
    nofralloc
    stwu r1, -64(r1)
    psq_l f0, 0(mA), 0, 0
    stfd f14, 8(r1)
    psq_l f6, 0(mB), 0, 0
    lis r6, lbl_804789B0@ha
    psq_l f7, 8(mB), 0, 0
    stfd f15, 16(r1)
    addi r6, r6, lbl_804789B0@l
    stfd f31, 40(r1)
    psq_l f8, 16(mB), 0, 0
    ps_muls0 f12, f6, f0
    psq_l f2, 16(mA), 0, 0
    ps_muls0 f13, f7, f0
    psq_l f31, 0(r6), 0, 0
    ps_muls0 f14, f6, f2
    psq_l f9, 24(mB), 0, 0
    ps_muls0 f15, f7, f2
    psq_l f1, 8(mA), 0, 0
    ps_madds1 f12, f8, f0, f12
    psq_l f3, 24(mA), 0, 0
    ps_madds1 f14, f8, f2, f14
    psq_l f10, 32(mB), 0, 0
    ps_madds1 f13, f9, f0, f13
    psq_l f11, 40(mB), 0, 0
    ps_madds1 f15, f9, f2, f15
    psq_l f4, 32(mA), 0, 0
    psq_l f5, 40(mA), 0, 0
    ps_madds0 f12, f10, f1, f12
    ps_madds0 f13, f11, f1, f13
    ps_madds0 f14, f10, f3, f14
    ps_madds0 f15, f11, f3, f15
    psq_st f12, 0(mAB), 0, 0
    ps_muls0 f2, f6, f4
    ps_madds1 f13, f31, f1, f13
    ps_muls0 f0, f7, f4
    psq_st f14, 16(mAB), 0, 0
    ps_madds1 f15, f31, f3, f15
    psq_st f13, 8(mAB), 0, 0
    ps_madds1 f2, f8, f4, f2
    ps_madds1 f0, f9, f4, f0
    ps_madds0 f2, f10, f5, f2
    lfd f14, 8(r1)
    psq_st f15, 24(mAB), 0, 0
    ps_madds0 f0, f11, f5, f0
    psq_st f2, 32(mAB), 0, 0
    ps_madds1 f0, f31, f5, f0
    lfd f15, 16(r1)
    psq_st f0, 40(mAB), 0, 0
    lfd f31, 40(r1)
    addi r1, r1, 64
    blr
}

void PSMTXTranspose(const register Mtx src, register Mtx xPose)
{
    register f32 zero = lbl_8047C28C;

    asm {
        psq_l f1, 0(src), 0, 0
        stfs zero, 44(xPose)
        psq_l f2, 16(src), 0, 0
        ps_merge00 f4, f1, f2
        psq_l f3, 8(src), 1, 0
        ps_merge11 f5, f1, f2
        psq_l f2, 24(src), 1, 0
        psq_st f4, 0(xPose), 0, 0
        psq_l f1, 32(src), 0, 0
        ps_merge00 f2, f3, f2
        psq_st f5, 16(xPose), 0, 0
        ps_merge00 f4, f1, zero
        psq_st f2, 32(xPose), 0, 0
        ps_merge10 f5, f1, zero
        psq_st f4, 8(xPose), 0, 0
        lfs f3, 40(src)
        psq_st f5, 24(xPose), 0, 0
        stfs f3, 40(xPose)
    }
}

asm u32 PSMTXInverse(const register Mtx src, register Mtx inv)
{
    nofralloc
    psq_l f0, 0(src), 1, 0
    psq_l f1, 4(src), 0, 0
    psq_l f2, 16(src), 1, 0
    ps_merge10 f6, f1, f0
    psq_l f3, 20(src), 0, 0
    psq_l f4, 32(src), 1, 0
    ps_merge10 f7, f3, f2
    psq_l f5, 36(src), 0, 0
    ps_mul f11, f3, f6
    ps_mul f13, f5, f7
    ps_merge10 f8, f5, f4
    ps_msub f11, f1, f7, f11
    ps_mul f12, f1, f8
    ps_msub f13, f3, f8, f13
    ps_mul f10, f3, f4
    ps_msub f12, f5, f6, f12
    ps_mul f9, f0, f5
    ps_mul f8, f1, f2
    ps_sub f6, f6, f6
    ps_msub f10, f2, f5, f10
    ps_mul f7, f0, f13
    ps_msub f9, f1, f4, f9
    ps_madd f7, f2, f12, f7
    ps_msub f8, f0, f3, f8
    ps_madd f7, f4, f11, f7
    ps_cmpo0 cr0, f7, f6
    bne regular
    addi r3, 0, 0
    blr
regular:
    fres f0, f7
    ps_add f6, f0, f0
    ps_mul f5, f0, f0
    ps_nmsub f0, f7, f5, f6
    lfs f1, 12(src)
    ps_muls0 f13, f13, f0
    lfs f2, 28(src)
    ps_muls0 f12, f12, f0
    lfs f3, 44(src)
    ps_muls0 f11, f11, f0
    ps_merge00 f5, f13, f12
    ps_muls0 f10, f10, f0
    ps_merge11 f4, f13, f12
    ps_muls0 f9, f9, f0
    psq_st f5, 0(inv), 0, 0
    ps_mul f6, f13, f1
    psq_st f4, 16(inv), 0, 0
    ps_muls0 f8, f8, f0
    ps_madd f6, f12, f2, f6
    psq_st f10, 32(inv), 1, 0
    ps_nmadd f6, f11, f3, f6
    psq_st f9, 36(inv), 1, 0
    ps_mul f7, f10, f1
    ps_merge00 f5, f11, f6
    psq_st f8, 40(inv), 1, 0
    ps_merge11 f4, f11, f6
    psq_st f5, 8(inv), 0, 0
    ps_madd f7, f9, f2, f7
    psq_st f4, 24(inv), 0, 0
    ps_nmadd f7, f8, f3, f7
    addi r3, 0, 1
    psq_st f7, 44(inv), 1, 0
    blr
}

asm u32 PSMTXInvXpose(const register Mtx src, register Mtx invX)
{
    nofralloc
    psq_l f0, 0(src), 1, 0
    psq_l f1, 4(src), 0, 0
    psq_l f2, 16(src), 1, 0
    ps_merge10 f6, f1, f0
    psq_l f3, 20(src), 0, 0
    psq_l f4, 32(src), 1, 0
    ps_merge10 f7, f3, f2
    psq_l f5, 36(src), 0, 0
    ps_mul f11, f3, f6
    ps_merge10 f8, f5, f4
    ps_mul f13, f5, f7
    ps_msub f11, f1, f7, f11
    ps_mul f12, f1, f8
    ps_msub f13, f3, f8, f13
    ps_msub f12, f5, f6, f12
    ps_mul f10, f3, f4
    ps_mul f9, f0, f5
    ps_mul f8, f1, f2
    ps_msub f10, f2, f5, f10
    ps_msub f9, f1, f4, f9
    ps_msub f8, f0, f3, f8
    ps_mul f7, f0, f13
    ps_sub f1, f1, f1
    ps_madd f7, f2, f12, f7
    ps_madd f7, f4, f11, f7
    ps_cmpo0 cr0, f7, f1
    bne regular_x
    addi r3, 0, 0
    blr
regular_x:
    fres f0, f7
    psq_st f1, 12(invX), 1, 0
    ps_add f6, f0, f0
    ps_mul f5, f0, f0
    psq_st f1, 28(invX), 1, 0
    ps_nmsub f0, f7, f5, f6
    psq_st f1, 44(invX), 1, 0
    ps_muls0 f13, f13, f0
    ps_muls0 f12, f12, f0
    ps_muls0 f11, f11, f0
    psq_st f13, 0(invX), 0, 0
    psq_st f12, 16(invX), 0, 0
    ps_muls0 f10, f10, f0
    ps_muls0 f9, f9, f0
    psq_st f11, 32(invX), 0, 0
    psq_st f10, 8(invX), 1, 0
    ps_muls0 f8, f8, f0
    addi r3, 0, 1
    psq_st f9, 24(invX), 1, 0
    psq_st f8, 40(invX), 1, 0
    blr
}
#endif

#pragma peephole off
#if !defined(SDK_PSMTXROTRAD_EXCLUDE)
void PSMTXRotRad(Mtx m, char axis, f32 rad)
{
    extern f32 sinf(f32 x);
    extern f32 cosf(f32 x);
    extern void PSMTXRotTrig(Mtx m, char axis, f32 sinA, f32 cosA);
    f32 sinA;
    f32 cosA;

    sinA = sinf(rad);
    cosA = cosf(rad);
    PSMTXRotTrig(m, axis, sinA, cosA);
}
#endif
#pragma peephole reset

#if !defined(SDK_PSMTXROTTRIG_EXCLUDE)
void PSMTXRotTrig(register Mtx m, register char axis, register f32 sinA,
                  register f32 cosA)
{
    register f32 sin = sinA;
    register f32 cos = cosA;
    register f32 zero = lbl_8047C28C;
    register f32 one = lbl_8047C288;

    asm {
    ori r0, axis, 32
    ps_neg f2, sin
    cmplwi r0, 120
    beq rot_x
    cmplwi r0, 121
    beq rot_y
    cmplwi r0, 122
    beq rot_z
    b rot_done
rot_x:
    psq_st one, 0(m), 1, 0
    psq_st zero, 4(m), 0, 0
    ps_merge00 f3, sin, cos
    psq_st zero, 12(m), 0, 0
    ps_merge00 one, cos, f2
    psq_st zero, 28(m), 0, 0
    psq_st zero, 44(m), 1, 0
    psq_st f3, 36(m), 0, 0
    psq_st one, 20(m), 0, 0
    b rot_done
rot_y:
    ps_merge00 f3, cos, zero
    ps_merge00 one, zero, one
    psq_st zero, 24(m), 0, 0
    psq_st f3, 0(m), 0, 0
    ps_merge00 f2, f2, zero
    ps_merge00 zero, sin, zero
    psq_st f3, 40(m), 0, 0
    psq_st one, 16(m), 0, 0
    psq_st zero, 8(m), 0, 0
    psq_st f2, 32(m), 0, 0
    b rot_done
rot_z:
    psq_st zero, 8(m), 0, 0
    ps_merge00 f3, sin, cos
    ps_merge00 f2, cos, f2
    psq_st zero, 24(m), 0, 0
    psq_st zero, 32(m), 0, 0
    ps_merge00 one, one, zero
    psq_st f3, 16(m), 0, 0
    psq_st f2, 0(m), 0, 0
    psq_st one, 40(m), 0, 0
rot_done:
    }
}
#endif

#if !defined(SDK_PSMTXROTAXIS_INTERNAL_EXCLUDE)
void __PSMTXRotAxisRadInternal(register Mtx m, const register Vec* axis,
                              register f32 sinA, register f32 cosA)
{
    register f32 three;
    register f32 half;

    half = lbl_8047C290;
    three = lbl_8047C294;

    asm {
        frsp f11, cosA
        psq_l f2, 0(axis), 0, 0
        frsp f12, sinA
        lfs f3, 8(axis)
        ps_mul f4, f2, f2
        fadds f8, half, half
        ps_madd f5, f3, f3, f4
        fsubs f1, half, half
        ps_sum0 f6, f5, f3, f4
        fsubs f0, f8, f11
        frsqrte f7, f6
        fmuls f4, f7, f7
        fmuls f5, f7, half
        fnmsubs f4, f4, f6, three
        fmuls f7, f4, f5
        ps_merge00 f11, f11, f11
        ps_muls0 f2, f2, f7
        ps_muls0 f3, f3, f7
        ps_muls0 f6, f2, f0
        ps_muls0 half, f2, f12
        ps_muls0 f7, f3, f0
        ps_muls1 f5, f6, f2
        ps_muls0 f4, f6, f2
        ps_muls0 f6, f6, f3
        fnmsubs f0, f3, f12, f5
        fmadds f8, f3, f12, f5
        ps_neg f2, half
        ps_sum0 three, f6, f1, half
        ps_sum0 f4, f4, f0, f11
        ps_sum1 f5, f11, f8, f5
        ps_sum0 f0, f2, f1, f6
        psq_st three, 8(m), 0, 0
        ps_sum0 f2, f6, f6, f2
        psq_st f4, 0(m), 0, 0
        ps_muls0 f7, f7, f3
        psq_st f5, 16(m), 0, 0
        ps_sum1 f6, half, f2, f6
        psq_st f0, 24(m), 0, 0
        ps_sum0 f7, f7, f1, f11
        psq_st f6, 32(m), 0, 0
        psq_st f7, 40(m), 0, 0
    }
}
#endif

#pragma peephole off
#if !defined(SDK_PSMTXROTAXISRAD_EXCLUDE)
void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad)
{
    extern f32 sinf(f32 x);
    extern f32 cosf(f32 x);
    f32 sinA;
    f32 cosA;

    sinA = sinf(rad);
    cosA = cosf(rad);
    __PSMTXRotAxisRadInternal(m, axis, sinA, cosA);
}
#endif
#pragma peephole reset

#if !defined(SDK_PSMTXTRANS_EXCLUDE)
void PSMTXTrans(register Mtx m, register f32 xT, register f32 yT,
                register f32 zT)
{
    register f32 zero = lbl_8047C28C;
    register f32 one = lbl_8047C288;

    asm {
        stfs xT, 12(m)
        stfs yT, 28(m)
        psq_st zero, 4(m), 0, 0
        psq_st zero, 32(m), 0, 0
        stfs zero, 16(m)
        stfs one, 20(m)
        stfs zero, 24(m)
        stfs one, 40(m)
        stfs zT, 44(m)
        stfs one, 0(m)
    }
}
#endif

#if !defined(SDK_PSMTXTRANSAPPLY_EXCLUDE)
asm void PSMTXTransApply(const register Mtx src, register Mtx dst,
                         register f32 xT, register f32 yT, register f32 zT)
{
    nofralloc
    psq_l f4, 0(src), 0, 0
    frsp xT, xT
    psq_l f5, 8(src), 0, 0
    frsp yT, yT
    psq_l f7, 24(src), 0, 0
    frsp zT, zT
    psq_l f8, 40(src), 0, 0
    psq_st f4, 0(dst), 0, 0
    ps_sum1 f5, xT, f5, f5
    psq_l f6, 16(src), 0, 0
    psq_st f5, 8(dst), 0, 0
    ps_sum1 f7, yT, f7, f7
    psq_l f9, 32(src), 0, 0
    psq_st f6, 16(dst), 0, 0
    ps_sum1 f8, zT, f8, f8
    psq_st f7, 24(dst), 0, 0
    psq_st f9, 32(dst), 0, 0
    psq_st f8, 40(dst), 0, 0
    blr
}
#endif

#if !defined(SDK_PSMTXSCALE_EXCLUDE)
void PSMTXScale(register Mtx m, register f32 xS, register f32 yS,
                register f32 zS)
{
    register f32 zero = lbl_8047C28C;

    asm {
        stfs xS, 0(m)
        psq_st zero, 4(m), 0, 0
        psq_st zero, 12(m), 0, 0
        stfs yS, 20(m)
        psq_st zero, 24(m), 0, 0
        psq_st zero, 32(m), 0, 0
        stfs zS, 40(m)
        stfs zero, 44(m)
    }
}
#endif

#if !defined(SDK_PSMTXSCALEAPPLY_EXCLUDE)
asm void PSMTXScaleApply(const register Mtx src, register Mtx dst,
                         register f32 xS, register f32 yS, register f32 zS)
{
    nofralloc
    frsp xS, xS
    psq_l f4, 0(src), 0, 0
    frsp yS, yS
    psq_l f5, 8(src), 0, 0
    frsp zS, zS
    ps_muls0 f4, f4, xS
    psq_l f6, 16(src), 0, 0
    ps_muls0 f5, f5, xS
    psq_l f7, 24(src), 0, 0
    ps_muls0 f6, f6, yS
    psq_l f8, 32(src), 0, 0
    psq_st f4, 0(dst), 0, 0
    ps_muls0 f7, f7, yS
    psq_l f2, 40(src), 0, 0
    psq_st f5, 8(dst), 0, 0
    ps_muls0 f8, f8, zS
    psq_st f6, 16(dst), 0, 0
    ps_muls0 f2, f2, zS
    psq_st f7, 24(dst), 0, 0
    psq_st f8, 32(dst), 0, 0
    psq_st f2, 40(dst), 0, 0
    blr
}
#endif

#if !defined(SDK_PSMTXQUAT_EXCLUDE)
void PSMTXQuat(register Mtx m, const register Quaternion* q)
{
    register f32 zero;
    register f32 one = 1.0f;
    register f32 two;
    register f32 scale;
    register f32 tmp0;
    register f32 tmp1;
    register f32 tmp2;
    register f32 tmp3;
    register f32 tmp4;
    register f32 tmp5;
    register f32 tmp6;
    register f32 tmp7;
    register f32 tmp8;
    register f32 tmp9;

    asm {
        psq_l tmp0, 0(q), 0, 0
        psq_l tmp1, 8(q), 0, 0
        fsubs zero, one, one
        fadds two, one, one
        ps_mul tmp2, tmp0, tmp0
        ps_merge10 tmp5, tmp0, tmp0
        ps_madd tmp4, tmp1, tmp1, tmp2
        ps_mul tmp3, tmp1, tmp1
        ps_sum0 scale, tmp4, tmp4, tmp4
        ps_muls1 tmp7, tmp5, tmp1
        fres tmp9, scale
        ps_sum1 tmp4, tmp3, tmp4, tmp2
        ps_nmsub scale, scale, tmp9, two
        ps_muls1 tmp6, tmp1, tmp1
        ps_mul scale, tmp9, scale
        ps_sum0 tmp2, tmp2, tmp2, tmp2
        fmuls scale, scale, two
        ps_madd tmp8, tmp0, tmp5, tmp6
        ps_msub tmp6, tmp0, tmp5, tmp6
        psq_st zero, 12(m), 1, 0
        ps_nmsub tmp2, tmp2, scale, one
        ps_nmsub tmp4, tmp4, scale, one
        psq_st zero, 44(m), 1, 0
        ps_mul tmp8, tmp8, scale
        ps_mul tmp6, tmp6, scale
        psq_st tmp2, 40(m), 1, 0
        ps_madds0 tmp5, tmp0, tmp1, tmp7
        ps_merge00 tmp1, tmp8, tmp4
        ps_nmsub tmp7, tmp7, two, tmp5
        ps_merge10 tmp0, tmp4, tmp6
        psq_st tmp1, 16(m), 0, 0
        ps_mul tmp5, tmp5, scale
        ps_mul tmp7, tmp7, scale
        psq_st tmp0, 0(m), 0, 0
        psq_st tmp5, 8(m), 1, 0
        ps_merge10 tmp3, tmp7, zero
        ps_merge01 tmp9, tmp7, tmp5
        psq_st tmp3, 24(m), 0, 0
        psq_st tmp9, 32(m), 0, 0
    }
}
#endif
#endif

#if !defined(SDK_MTXLOOKAT_EXCLUDE)
void C_MTXLookAt(Mtx m, const Vec* cameraPosition, const Vec* cameraUp,
                 const Vec* target)
{
    extern void PSVECNormalize(const Vec* source, Vec* destination);
    extern void PSVECCrossProduct(const Vec* first, const Vec* second, Vec* destination);
    Vec look;
    Vec right;
    Vec up;

    look.x = cameraPosition->x - target->x;
    look.y = cameraPosition->y - target->y;
    look.z = cameraPosition->z - target->z;
    PSVECNormalize(&look, &look);
    PSVECCrossProduct(cameraUp, &look, &right);
    PSVECNormalize(&right, &right);
    PSVECCrossProduct(&look, &right, &up);

    m[0][0] = right.x;
    m[0][1] = right.y;
    m[0][2] = right.z;
    m[0][3] = -(cameraPosition->x * right.x + cameraPosition->y * right.y +
                  cameraPosition->z * right.z);

    m[1][0] = up.x;
    m[1][1] = up.y;
    m[1][2] = up.z;
    m[1][3] = -(cameraPosition->x * up.x + cameraPosition->y * up.y +
                  cameraPosition->z * up.z);

    m[2][0] = look.x;
    m[2][1] = look.y;
    m[2][2] = look.z;
    m[2][3] = -(cameraPosition->x * look.x + cameraPosition->y * look.y +
                  cameraPosition->z * look.z);
}
#endif

#if !defined(SDK_MTXLOOKAT_AFTER_EXCLUDE)
#pragma peephole off
void C_MTXLightFrustum(Mtx m, f32 top, f32 bottom, f32 left, f32 right, f32 near,
                       f32 scaleS, f32 scaleT, f32 transS, f32 transT)
{
    extern const f32 lbl_8047C298;
    extern const f32 lbl_8047C29C;
    f32 tmp;

    tmp = lbl_8047C288 / (right - left);
    m[0][0] = (lbl_8047C298 * near) * tmp * scaleS;
    m[0][1] = lbl_8047C28C;
    m[0][2] = (right + left) * tmp * scaleS - transS;
    m[0][3] = lbl_8047C28C;

    tmp = lbl_8047C288 / (top - bottom);
    m[1][0] = lbl_8047C28C;
    m[1][1] = (lbl_8047C298 * near) * tmp * scaleT;
    m[1][2] = (top + bottom) * tmp * scaleT - transT;
    m[1][3] = lbl_8047C28C;

    m[2][0] = lbl_8047C28C;
    m[2][1] = lbl_8047C28C;
    m[2][2] = lbl_8047C29C;
    m[2][3] = lbl_8047C28C;
}
#pragma peephole reset

#pragma peephole off
void C_MTXLightPerspective(Mtx m, f32 fovY, f32 aspect, f32 scaleS, f32 scaleT,
                           f32 transS, f32 transT)
{
    extern const f32 lbl_8047C290;
    extern const f32 lbl_8047C29C;
    extern const f32 lbl_8047C2A0;
    f32 cot;

    fovY = lbl_8047C290 * fovY;
    fovY = lbl_8047C2A0 * fovY;
    cot = lbl_8047C288 / tanf(fovY);

    m[0][0] = scaleS * (cot / aspect);
    m[0][1] = lbl_8047C28C;
    m[0][2] = -transS;
    m[0][3] = lbl_8047C28C;

    m[1][0] = lbl_8047C28C;
    m[1][1] = cot * scaleT;
    m[1][2] = -transT;
    m[1][3] = lbl_8047C28C;

    m[2][0] = lbl_8047C28C;
    m[2][1] = lbl_8047C28C;
    m[2][2] = lbl_8047C29C;
    m[2][3] = lbl_8047C28C;
}
#pragma peephole reset
#endif
#endif

#if defined(SDK_800A37CC_SUFFIX_ACTIVE)
#if !defined(SDK_CQUATSLERP_ONLY)
#if !defined(SDK_MTXVEC_PREFIX_EXCLUDE)
asm void PSMTXMultVec(const register Mtx m, const register Vec* src,
                      register Vec* dst)
{
    nofralloc
    psq_l f0, 0(src), 0, 0
    psq_l f2, 0(m), 0, 0
    psq_l f1, 8(src), 1, 0
    ps_mul f4, f2, f0
    psq_l f3, 8(m), 0, 0
    ps_madd f5, f3, f1, f4
    psq_l f8, 16(m), 0, 0
    ps_sum0 f6, f5, f6, f5
    psq_l f9, 24(m), 0, 0
    ps_mul f10, f8, f0
    psq_st f6, 0(dst), 1, 0
    ps_madd f11, f9, f1, f10
    psq_l f2, 32(m), 0, 0
    ps_sum0 f12, f11, f12, f11
    psq_l f3, 40(m), 0, 0
    ps_mul f4, f2, f0
    psq_st f12, 4(dst), 1, 0
    ps_madd f5, f3, f1, f4
    ps_sum0 f6, f5, f6, f5
    psq_st f6, 8(dst), 1, 0
    blr
}

asm void PSMTXMultVecSR(const register Mtx m, const register Vec* src,
                        register Vec* dst)
{
    nofralloc
    psq_l f0, 0(m), 0, 0
    psq_l f6, 0(src), 0, 0
    psq_l f2, 16(m), 0, 0
    ps_mul f8, f0, f6
    psq_l f4, 32(m), 0, 0
    ps_mul f10, f2, f6
    psq_l f7, 8(src), 1, 0
    ps_mul f12, f4, f6
    psq_l f3, 24(m), 0, 0
    ps_sum0 f8, f8, f8, f8
    psq_l f5, 40(m), 0, 0
    ps_sum0 f10, f10, f10, f10
    psq_l f1, 8(m), 0, 0
    ps_sum0 f12, f12, f12, f12
    ps_madd f9, f1, f7, f8
    psq_st f9, 0(dst), 1, 0
    ps_madd f11, f3, f7, f10
    psq_st f11, 4(dst), 1, 0
    ps_madd f13, f5, f7, f12
    psq_st f13, 8(dst), 1, 0
    blr
}
#endif

#if !defined(SDK_POST_MTXVEC_EXCLUDE)

#if !defined(SDK_PROJECTION_PREFIX_EXCLUDE)
void C_MTXFrustum(Mtx44 m, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far)
{
    f32 scaledNear;
    f32 depthScale;
    f32 verticalScale;
    f32 horizontalScale;

    horizontalScale = lbl_8047C2A8 / (right - left);
    scaledNear = lbl_8047C2AC * near;
    verticalScale = lbl_8047C2A8 / (top - bottom);
    depthScale = lbl_8047C2A8 / (far - near);

    m[0][0] = scaledNear * horizontalScale;
    m[0][1] = lbl_8047C2B0;
    m[0][2] = horizontalScale * (right + left);
    m[0][3] = lbl_8047C2B0;

    m[1][0] = lbl_8047C2B0;
    m[1][1] = scaledNear * verticalScale;
    m[1][2] = verticalScale * (top + bottom);
    m[1][3] = lbl_8047C2B0;

    m[2][0] = lbl_8047C2B0;
    m[2][1] = lbl_8047C2B0;
    m[2][2] = -near * depthScale;
    m[2][3] = depthScale * -(far * near);

    m[3][0] = lbl_8047C2B0;
    m[3][1] = lbl_8047C2B0;
    m[3][2] = lbl_8047C2B4;
    m[3][3] = lbl_8047C2B0;
}

void C_MTXPerspective(Mtx44 m, f32 fovY, f32 aspect, f32 near, f32 far)
{
    f32 cot;
    f32 tmp;

    fovY = lbl_8047C2B8 * fovY;
    fovY = lbl_8047C2BC * fovY;
    cot = lbl_8047C2A8 / tanf(fovY);
    tmp = lbl_8047C2A8 / (far - near);

    m[0][0] = cot / aspect;
    m[0][1] = lbl_8047C2B0;
    m[0][2] = lbl_8047C2B0;
    m[0][3] = lbl_8047C2B0;

    m[1][0] = lbl_8047C2B0;
    m[1][1] = cot;
    m[1][2] = lbl_8047C2B0;
    m[1][3] = lbl_8047C2B0;

    m[2][0] = lbl_8047C2B0;
    m[2][1] = lbl_8047C2B0;
    m[2][2] = -near * tmp;
    m[2][3] = -(far * near) * tmp;

    m[3][0] = lbl_8047C2B0;
    m[3][1] = lbl_8047C2B0;
    m[3][2] = lbl_8047C2B4;
    m[3][3] = lbl_8047C2B0;
}

void C_MTXOrtho(Mtx44 m, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far)
{
    f32 tmp;

    tmp = lbl_8047C2A8 / (right - left);
    m[0][0] = lbl_8047C2AC * tmp;
    m[0][1] = lbl_8047C2B0;
    m[0][2] = lbl_8047C2B0;
    m[0][3] = -(right + left) * tmp;

    tmp = lbl_8047C2A8 / (top - bottom);
    m[1][0] = lbl_8047C2B0;
    m[1][1] = lbl_8047C2AC * tmp;
    m[1][2] = lbl_8047C2B0;
    m[1][3] = -(top + bottom) * tmp;

    tmp = lbl_8047C2A8 / (far - near);
    m[2][0] = lbl_8047C2B0;
    m[2][1] = lbl_8047C2B0;
    m[2][2] = lbl_8047C2B4 * tmp;
    m[2][3] = -far * tmp;

    m[3][0] = lbl_8047C2B0;
    m[3][1] = lbl_8047C2B0;
    m[3][2] = lbl_8047C2B0;
    m[3][3] = lbl_8047C2A8;
}
#endif

#if !defined(SDK_VECTOR_PREFIX_EXCLUDE)
asm void PSVECAdd(const register Vec* lhs, const register Vec* rhs,
                  register Vec* dst)
{
    nofralloc
    psq_l f2, 0(lhs), 0, 0
    psq_l f4, 0(rhs), 0, 0
    ps_add f6, f2, f4
    psq_st f6, 0(dst), 0, 0
    psq_l f3, 8(lhs), 1, 0
    psq_l f5, 8(rhs), 1, 0
    ps_add f7, f3, f5
    psq_st f7, 8(dst), 1, 0
    blr
}

asm void PSVECSubtract(const register Vec* lhs, const register Vec* rhs,
                       register Vec* dst)
{
    nofralloc
    psq_l f2, 0(lhs), 0, 0
    psq_l f4, 0(rhs), 0, 0
    ps_sub f6, f2, f4
    psq_st f6, 0(dst), 0, 0
    psq_l f3, 8(lhs), 1, 0
    psq_l f5, 8(rhs), 1, 0
    ps_sub f7, f3, f5
    psq_st f7, 8(dst), 1, 0
    blr
}

asm void PSVECScale(const register Vec* src, register Vec* dst,
                    register f32 scale)
{
    nofralloc
    psq_l f0, 0(src), 0, 0
    psq_l f2, 8(src), 1, 0
    ps_muls0 f0, f0, scale
    psq_st f0, 0(dst), 0, 0
    ps_muls0 f0, f2, scale
    psq_st f0, 8(dst), 1, 0
    blr
}

void PSVECNormalize(Vec* src, Vec* dst)
{
    extern f32 lbl_8047C2C0;
    extern f32 lbl_8047C2C4;
    register Vec* srcReg = src;
    register Vec* dstReg = dst;
    register f32 half = lbl_8047C2C0;
    register f32 three = lbl_8047C2C4;
    register f32 pair;
    register f32 z;
    register f32 sum;
    register f32 estimate;
    register f32 temp;

    asm {
        psq_l pair, 0(srcReg), 0, 0
        ps_mul estimate, pair, pair
        psq_l z, 8(srcReg), 1, 0
        ps_madd sum, z, z, estimate
        ps_sum0 sum, sum, z, estimate
        frsqrte estimate, sum
        fmuls temp, estimate, estimate
        fmuls half, estimate, half
        fnmsubs temp, temp, sum, three
        fmuls estimate, temp, half
        ps_muls0 pair, pair, estimate
        psq_st pair, 0(dstReg), 0, 0
        ps_muls0 z, z, estimate
        psq_st z, 8(dstReg), 1, 0
    }
}

asm f32 PSVECSquareMag(const register Vec* src)
{
    nofralloc
    psq_l f0, 0(src), 0, 0
    ps_mul f0, f0, f0
    lfs f1, 8(src)
    ps_madd f1, f1, f1, f0
    ps_sum0 f1, f1, f0, f0
    blr
}

f32 PSVECMag(const register Vec* src)
{
    extern f32 lbl_8047C2C0;
    extern f32 lbl_8047C2C4;
    register f32 pair;
    register f32 square;
    register f32 estimate;
    register f32 temp;
    register f32 three;
    register f32 half = lbl_8047C2C0;
    register f32 zero;

    asm {
        psq_l pair, 0(src), 0, 0
        ps_mul pair, pair, pair
        lfs square, 8(src)
        fsubs zero, half, half
        ps_madd square, square, square, pair
        ps_sum0 square, square, pair, pair
    }
    if (square != zero) {
        asm {
            frsqrte estimate, square
        }
        three = lbl_8047C2C4;
        asm {
            fmuls temp, estimate, estimate
            fmuls estimate, estimate, half
            fnmsubs temp, temp, square, three
            fmuls estimate, temp, estimate
            fmuls square, square, estimate
        }
    }
    return square;
}

asm f32 PSVECDotProduct(const register Vec* lhs, const register Vec* rhs)
{
    nofralloc
    psq_l f2, 4(lhs), 0, 0
    psq_l f3, 4(rhs), 0, 0
    ps_mul f2, f2, f3
    psq_l f5, 0(lhs), 0, 0
    psq_l f4, 0(rhs), 0, 0
    ps_madd f3, f5, f4, f2
    ps_sum0 f1, f3, f2, f2
    blr
}

asm void PSVECCrossProduct(const register Vec* lhs, const register Vec* rhs,
                           register Vec* dst)
{
    nofralloc
    psq_l f1, 0(rhs), 0, 0
    lfs f2, 8(lhs)
    psq_l f0, 0(lhs), 0, 0
    ps_merge10 f6, f1, f1
    lfs f3, 8(rhs)
    ps_mul f4, f1, f2
    ps_muls0 f7, f1, f0
    ps_msub f5, f0, f3, f4
    ps_msub f8, f0, f6, f7
    ps_merge11 f9, f5, f5
    ps_merge01 f10, f5, f8
    psq_st f9, 0(dst), 1, 0
    ps_neg f10, f10
    psq_st f10, 4(dst), 0, 0
    blr
}

asm f32 PSVECSquareDistance(const register Vec* lhs,
                            const register Vec* rhs)
{
    nofralloc
    psq_l f0, 4(lhs), 0, 0
    psq_l f1, 4(rhs), 0, 0
    ps_sub f2, f0, f1
    psq_l f0, 0(lhs), 0, 0
    psq_l f1, 0(rhs), 0, 0
    ps_mul f2, f2, f2
    ps_sub f0, f0, f1
    ps_madd f1, f0, f0, f2
    ps_sum0 f1, f1, f2, f2
    blr
}

f32 PSVECDistance(const register Vec* lhs, const register Vec* rhs)
{
    extern f32 lbl_8047C2C0;
    extern f32 lbl_8047C2C4;
    register f32 lhsPair;
    register f32 rhsPair;
    register f32 delta;
    register f32 square;
    register f32 estimate;
    register f32 temp;
    register f32 half;
    register f32 three;
    register f32 zero;

    asm {
        psq_l lhsPair, 4(lhs), 0, 0
        psq_l rhsPair, 4(rhs), 0, 0
        ps_sub delta, lhsPair, rhsPair
        psq_l lhsPair, 0(lhs), 0, 0
        psq_l rhsPair, 0(rhs), 0, 0
        ps_mul delta, delta, delta
        ps_sub lhsPair, lhsPair, rhsPair
    }
    half = lbl_8047C2C0;
    asm {
        ps_madd square, lhsPair, lhsPair, delta
        fsubs zero, half, half
        ps_sum0 square, square, delta, delta
    }
    if (zero != square) {
        three = lbl_8047C2C4;
        asm {
            frsqrte estimate, square
            fmuls temp, estimate, estimate
            fmuls estimate, estimate, half
            fnmsubs temp, temp, square, three
            fmuls estimate, temp, estimate
            fmuls square, square, estimate
        }
    }
    return square;
}
#endif

#if !defined(SDK_QUAT_SUFFIX_EXCLUDE)

#if !defined(SDK_PSQUAT_MULTIPLY_EXCLUDE)
asm void PSQUATMultiply(const register Quaternion* lhs,
                        const register Quaternion* rhs,
                        register Quaternion* dst)
{
    nofralloc
    psq_l f0, 0(lhs), 0, 0
    psq_l f1, 8(lhs), 0, 0
    psq_l f2, 0(rhs), 0, 0
    ps_neg f5, f0
    psq_l f3, 8(rhs), 0, 0
    ps_neg f6, f1
    ps_merge01 f4, f5, f0
    ps_muls0 f7, f1, f2
    ps_muls0 f5, f5, f2
    ps_merge01 f1, f6, f1
    ps_muls1 f8, f4, f2
    ps_madds0 f7, f4, f3, f7
    ps_muls1 f2, f1, f2
    ps_madds0 f5, f1, f3, f5
    ps_madds1 f8, f6, f3, f8
    ps_merge10 f7, f7, f7
    ps_madds1 f2, f0, f3, f2
    ps_merge10 f5, f5, f5
    ps_add f7, f7, f2
    psq_st f7, 0(dst), 0, 0
    ps_sub f5, f5, f8
    psq_st f5, 8(dst), 0, 0
    blr
}
#endif

#if !defined(SDK_CQUATROT_EXCLUDE)
#pragma dont_inline on
void C_QUATRotAxisRad(Quaternion* quat, const Vec* axis, f32 rad)
{
    extern f32 sinf(f32 x);
    extern f32 cosf(f32 x);
    extern const f32 lbl_8047C2D0;
    Vec normalized;
    f32 sinHalf;
    f32 cosHalf;

    PSVECNormalize((Vec*)axis, &normalized);
    rad = lbl_8047C2D0 * rad;
    sinHalf = sinf(rad);
    cosHalf = cosf(rad);
    quat->x = sinHalf * normalized.x;
    quat->y = sinHalf * normalized.y;
    quat->z = sinHalf * normalized.z;
    quat->w = cosHalf;
}
#pragma dont_inline reset
#endif
#endif
#endif
#endif

#if !defined(SDK_POST_MTXVEC_EXCLUDE)
#if !defined(SDK_CQUATSLERP_EXCLUDE)
void C_QUATSlerp(const Quaternion* p, const Quaternion* q, Quaternion* r, f32 t)
{
    extern f32 acosf(f32 x);
    extern f32 sinf(f32 x);
    extern const f32 lbl_8047C2C8;
    extern const f32 lbl_8047C2CC;
    extern const f32 lbl_8047C2D4;
    f32 theta;
    f32 sinTheta;
    f32 cosTheta;
    f32 pScale;
    f32 qScale;

    cosTheta = p->x * q->x + p->y * q->y + p->z * q->z + p->w * q->w;
    qScale = lbl_8047C2CC;

    if (cosTheta < lbl_8047C2C8) {
        cosTheta = -cosTheta;
        qScale = -qScale;
    }

    if (cosTheta <= lbl_8047C2D4) {
        theta = acosf(cosTheta);
        sinTheta = sinf(theta);
        pScale = sinf((lbl_8047C2CC - t) * theta) / sinTheta;
        qScale *= sinf(t * theta) / sinTheta;
    } else {
        pScale = lbl_8047C2CC - t;
        qScale *= t;
    }

    r->x = pScale * p->x + qScale * q->x;
    r->y = pScale * p->y + qScale * q->y;
    r->z = pScale * p->z + qScale * q->z;
    r->w = pScale * p->w + qScale * q->w;
}
#endif
#endif
#endif
