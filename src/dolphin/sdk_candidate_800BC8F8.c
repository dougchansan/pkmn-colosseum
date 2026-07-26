/** Residual GX/SDK candidate, 0x800BC8F8 - 0x800BCEBC. */
#define SDK_800BC618_SUFFIX_ACTIVE
#include "src/dolphin/sdk_range_800BB30C.c"

extern f32 sqrtf(f32 value);

void fn_800BC8F8(u32 type, GXColor_800BC2F8 color, f32 startz, f32 endz,
                 f32 nearz, f32 farz)
{
    u32 fogclr = 0;
    u32 fog0 = 0;
    u32 fog1 = 0;
    u32 fog2 = 0;
    u32 fog3 = 0;
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
    u32 rgba;

    fsel = type & 7;
    proj = (type >> 3) & 1;

    if (proj != 0) {
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

        a = A / (f32) (1 << (B_expn + 1));
        b_m = 8388638.0f * B_mant;
        b_s = B_expn + 1;
        c = C;

        fog1 = (fog1 & ~0xFFFFFFU) | (b_m & 0xFFFFFF);
        fog1 = (fog1 & 0xFFFFFFU) | 0xEF000000;
        fog2 = (fog2 & ~0x1FU) | (b_s & 0x1F);
        fog2 = (fog2 & 0xFFFFFFU) | 0xF0000000;
    }

    a_hex = *(u32*) &a;
    c_hex = *(u32*) &c;
    fog0 = (fog0 & ~0x7FFU) | ((a_hex >> 12) & 0x7FF);
    fog0 = (fog0 & ~0x7F800U) | (((a_hex >> 23) & 0xFF) << 11);
    fog0 = (fog0 & ~0x80000U) | ((a_hex >> 31) << 19);
    fog0 = (fog0 & 0xFFFFFFU) | 0xEE000000;
    fog3 = (fog3 & ~0x7FFU) | ((c_hex >> 12) & 0x7FF);
    fog3 = (fog3 & ~0x7F800U) | (((c_hex >> 23) & 0xFF) << 11);
    fog3 = (fog3 & ~0x80000U) | ((c_hex >> 31) << 19);
    fog3 = (fog3 & ~0x100000U) | (proj << 20);
    fog3 = (fog3 & ~0xE00000U) | (fsel << 21);
    fog3 = (fog3 & 0xFFFFFFU) | 0xF1000000;

    rgba = *(u32*) &color;
    fogclr = (fogclr & ~0xFFFFFFU) | (rgba >> 8);
    fogclr = (fogclr & 0xFFFFFFU) | 0xF2000000;

    GX_BP_REG(fog0);
    GX_BP_REG(fog1);
    GX_BP_REG(fog2);
    GX_BP_REG(fog3);
    GX_BP_REG(fogclr);
    gx->field_002 = 0;
}

void fn_800BCB14(GXFogAdjTable_800BCCDC* table, u16 width,
                 const f32 projmtx[4][4])
{
    f32 xi;
    f32 iw;
    f32 range;
    f32 nearZ;
    f32 sideX;
    u32 i;

    if (projmtx[3][3] == 0.0f) {
        nearZ = projmtx[2][3] / (projmtx[2][2] - 1.0f);
        sideX = nearZ / projmtx[0][0];
    } else {
        sideX = 1.0f / projmtx[0][0];
        nearZ = 1.73205f * sideX;
    }

    iw = 2.0f / width;
    for (i = 0; i < 10; i++) {
        xi = (i + 1) << 5;
        xi *= iw;
        xi *= sideX;
        range = sqrtf(1.0f + (xi * xi) / (nearZ * nearZ));
        table->r[i] = (u32) (256.0f * range) & 0xFFF;
    }
}
