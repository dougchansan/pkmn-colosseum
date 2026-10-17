/**
 * @file sdk_candidate_800BC8F8.c
 * @brief Dolphin SDK GXPixel.c, 0x800BC8F8 - 0x800BCEBC: GXSetFog
 *        (GXSetFog), GXInitFogAdjTable (GXInitFogAdjTable), GXSetFogRangeAdj
 *        (GXSetFogRangeAdj), GXSetBlendMode, GXSetColorUpdate (GXSetColorUpdate),
 *        GXSetAlphaUpdate (GXSetAlphaUpdate) and GXSetZMode. The rest of the file
 *        (GXSetZCompLoc onwards) is the linked GX_exact_800BCEBC.c.
 *
 * Bodies follow the SDK GXPixel.c (dolsdk2004 / Melee): the fog color is
 * packed from its b/g/r bytes and GXInitFogAdjTable uses the general
 * projection formula with the MSL sqrtf inline (volatile result). Like the
 * other SDK GX units it is built with -fp_contract off. The fog functions
 * use the shift/or register-field form, the cmode0/zmode setters the
 * __rlwimi form.
 */
#include "dolphin/gx/GX.h"

typedef struct GXData {
    /* 0x000 */ u16 field_000;
    /* 0x002 */ u16 bpSentNot;
    /* 0x004 */ u8 _004[0x1CC];
    /* 0x1D0 */ u32 cmode0;
    /* 0x1D4 */ u32 cmode1;
    /* 0x1D8 */ u32 zmode;
} GXData;

extern GXData* const gx;

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

#define GX_FIFO_U8  GXWGFifo.u8
#define GX_FIFO_U32 GXWGFifo.u32

#define GX_WRITE_RAS_REG(reg)  \
    do {                       \
        GX_FIFO_U8 = 0x61;     \
        GX_FIFO_U32 = (reg);   \
    } while (0)

extern u32 __cvt_fp2unsigned(f32 value);

typedef struct GXFogAdjTable {
    u16 r[10];
} GXFogAdjTable;

#define OLD_SET_REG_FIELD(line, reg, size, shift, val)                       \
    do {                                                                     \
        (reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) |           \
                ((u32)(val) << (shift));                                     \
    } while (0)

/* dolsdk2004 __gx.h: the release SET_REG_FIELD inserts with __rlwimi. */
#define SET_REG_FIELD(line, reg, size, shift, val)                           \
    do {                                                                     \
        (reg) = ((u32)__rlwimi((u32)(reg), (val), (shift),                   \
                               32 - (shift) - (size), 31 - (shift)));        \
    } while (0)

extern f64 __frsqrte(f64 value);

static inline f32 sqrtf(f32 x)
{
    volatile f32 y;
    f64 guess;

    if (x > 0.0f) {
        guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        y = (f32)(x * guess);
        return y;
    }
    return x;
}

void GXSetFog(u32 type, f32 startz, f32 endz, f32 nearz, f32 farz,
                 GXColor color)
{
    u32 fogclr;
    u32 fog0;
    u32 fog1;
    u32 fog2;
    u32 fog3;
    f32 A;
    f32 B;
    f32 B_mant;
    f32 C;
    f32 a;
    f32 c;
    u32 B_expn;
    u32 b_m;
    u32 b_s;
    u32 a_hex;
    u32 c_hex;
    u32 fsel;
    u32 proj;

    fogclr = 0;
    fog0 = 0;
    fog1 = 0;
    fog2 = 0;
    fog3 = 0;

    fsel = type & 7;
    proj = (type >> 3) & 1;

    if (proj) {
        if (farz == nearz || endz == startz) {
            a = 0.0f;
            c = 0.0f;
        } else {
            A = 1.0f / (endz - startz);
            a = A * (farz - nearz);
            c = A * (startz - nearz);
        }
    } else {
        if (farz == nearz || endz == startz) {
            A = 0.0f;
            B = 0.5f;
            C = 0.0f;
        } else {
            A = (farz * nearz) / ((farz - nearz) * (endz - startz));
            B = farz / (farz - nearz);
            C = startz / (endz - startz);
        }

        B_mant = B;
        B_expn = 0;
        while (B_mant > 1.0) {
            B_mant /= 2.0f;
            B_expn++;
        }
        while (B_mant > 0.0f && B_mant < 0.5) {
            B_mant *= 2.0f;
            B_expn--;
        }

        a = A / (f32)(1 << (B_expn + 1));
        b_m = 8.388638e6f * B_mant;
        b_s = B_expn + 1;
        c = C;

        OLD_SET_REG_FIELD(198, fog1, 24, 0, b_m);
        OLD_SET_REG_FIELD(198, fog1, 8, 24, 0xEF);

        OLD_SET_REG_FIELD(201, fog2, 5, 0, b_s);
        OLD_SET_REG_FIELD(201, fog2, 8, 24, 0xF0);
    }

    a_hex = *(u32*)&a;
    c_hex = *(u32*)&c;

    OLD_SET_REG_FIELD(209, fog0, 11, 0, (a_hex >> 12) & 0x7FF);
    OLD_SET_REG_FIELD(210, fog0, 8, 11, (a_hex >> 23) & 0xFF);
    OLD_SET_REG_FIELD(211, fog0, 1, 19, (a_hex >> 31));
    OLD_SET_REG_FIELD(211, fog0, 8, 24, 0xEE);

    OLD_SET_REG_FIELD(214, fog3, 11, 0, (c_hex >> 12) & 0x7FF);
    OLD_SET_REG_FIELD(215, fog3, 8, 11, (c_hex >> 23) & 0xFF);
    OLD_SET_REG_FIELD(216, fog3, 1, 19, (c_hex >> 31));
    OLD_SET_REG_FIELD(217, fog3, 1, 20, proj);
    OLD_SET_REG_FIELD(218, fog3, 3, 21, fsel);
    OLD_SET_REG_FIELD(218, fog3, 8, 24, 0xF1);

    OLD_SET_REG_FIELD(222, fogclr, 8, 0, color.b);
    OLD_SET_REG_FIELD(222, fogclr, 8, 8, color.g);
    OLD_SET_REG_FIELD(222, fogclr, 8, 16, color.r);
    OLD_SET_REG_FIELD(222, fogclr, 8, 24, 0xF2);

    GX_WRITE_RAS_REG(fog0);
    GX_WRITE_RAS_REG(fog1);
    GX_WRITE_RAS_REG(fog2);
    GX_WRITE_RAS_REG(fog3);
    GX_WRITE_RAS_REG(fogclr);

    gx->bpSentNot = 0;
}

void GXInitFogAdjTable(GXFogAdjTable* table, u16 width, const f32 projmtx[4][4])
{
    f32 xi;
    f32 iw;
    f32 rangeVal;
    f32 nearZ;
    f32 sideX;
    u32 i;

    if (0.0 == projmtx[3][3]) {
        nearZ = projmtx[2][3] / (projmtx[2][2] - 1.0f);
        sideX = (nearZ * (1.0f + projmtx[0][2])) / projmtx[0][0];
    } else {
        nearZ = (1.0f + projmtx[2][3]) / projmtx[2][2];
        sideX = -(projmtx[0][3] - 1.0f) / projmtx[0][0];
    }

    iw = 2.0f / width;
    for (i = 0; i < 10; i++) {
        xi = (i + 1) << 5;
        xi *= iw;
        xi *= sideX;
        rangeVal = sqrtf(1.0f + ((xi * xi) / (nearZ * nearZ)));
        table->r[i] = (u32)(256.0f * rangeVal) & 0xFFF;
    }
}

void GXSetFogRangeAdj(GXBool enable, u16 center, const GXFogAdjTable* table)
{
    u32 i;
    u32 range_adj;
    u32 range_c;

    if (enable) {
        for (i = 0; i < 10; i += 2) {
            range_adj = 0;
            OLD_SET_REG_FIELD(338, range_adj, 12, 0, table->r[i]);
            OLD_SET_REG_FIELD(339, range_adj, 12, 12, table->r[i + 1]);
            OLD_SET_REG_FIELD(340, range_adj, 8, 24, (i >> 1) + 0xE9);
            GX_WRITE_RAS_REG(range_adj);
        }
    }
    range_c = 0;
    OLD_SET_REG_FIELD(346, range_c, 10, 0, center + 342);
    OLD_SET_REG_FIELD(347, range_c, 1, 10, enable);
    OLD_SET_REG_FIELD(348, range_c, 8, 24, 0xE8);
    GX_WRITE_RAS_REG(range_c);
    gx->bpSentNot = 0;
}

void GXSetBlendMode(u32 type, u32 src_factor, u32 dst_factor, u32 op)
{
    u32 reg;

    reg = gx->cmode0;
    SET_REG_FIELD(389, reg, 1, 11, (type == 3));
    SET_REG_FIELD(392, reg, 1, 0, type);
    SET_REG_FIELD(393, reg, 1, 1, (type == 2));
    SET_REG_FIELD(394, reg, 4, 12, op);
    SET_REG_FIELD(395, reg, 3, 8, src_factor);
    SET_REG_FIELD(396, reg, 3, 5, dst_factor);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = 0;
}

void GXSetColorUpdate(GXBool update_enable)
{
    u32 reg;

    reg = gx->cmode0;
    SET_REG_FIELD(411, reg, 1, 3, update_enable);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = 0;
}

void GXSetAlphaUpdate(GXBool update_enable)
{
    u32 reg;

    reg = gx->cmode0;
    SET_REG_FIELD(423, reg, 1, 4, update_enable);
    GX_WRITE_RAS_REG(reg);
    gx->cmode0 = reg;
    gx->bpSentNot = 0;
}

void GXSetZMode(GXBool compare_enable, u32 func, GXBool update_enable)
{
    u32 reg;

    reg = gx->zmode;
    SET_REG_FIELD(448, reg, 1, 0, compare_enable);
    SET_REG_FIELD(449, reg, 3, 1, func);
    SET_REG_FIELD(450, reg, 1, 4, update_enable);
    GX_WRITE_RAS_REG(reg);
    gx->zmode = reg;
    gx->bpSentNot = 0;
}
