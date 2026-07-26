/** Residual GX/SDK candidate, 0x800BB81C - 0x800BC580. */
#define SDK_800BB30C_PREFIX_ACTIVE
#include "src/dolphin/sdk_range_800BB30C.c"

void fn_800BB81C(u32 mtx_id, const f32 offset[2][3], s8 scale_exp)
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
    reg = mtx[0] | (mtx[1] << 11) | ((scale_exp & 3) << 22) |
          ((id * 3 + 6) << 24);
    GX_BP_REG(reg);

    mtx[2] = (s32) (1024.0f * offset[0][1]) & 0x7FF;
    mtx[3] = (s32) (1024.0f * offset[1][1]) & 0x7FF;
    reg = mtx[2] | (mtx[3] << 11) | (((scale_exp >> 2) & 3) << 22) |
          ((id * 3 + 7) << 24);
    GX_BP_REG(reg);

    mtx[4] = (s32) (1024.0f * offset[0][2]) & 0x7FF;
    mtx[5] = (s32) (1024.0f * offset[1][2]) & 0x7FF;
    reg = mtx[4] | (mtx[5] << 11) | (((scale_exp >> 4) & 3) << 22) |
          ((id * 3 + 8) << 24);
    GX_BP_REG(reg);
    gx->field_002 = 0;
}

void fn_800BB97C(u32 stage, u32 scale_s, u32 scale_t)
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

void fn_800BBAF8(u32 stage, u32 tex_coord, u32 tex_map)
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
    fn_800BB81C(matrix_sel, mtx, 10);
    fn_800BB780(tev_stage, ind_stage, format, bias_sel, matrix_sel, wrap_s,
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

    fn_800BB780(tev_stage, ind_stage, 0, 3, sm, 6, 6, 0, 0, 0);
    fn_800BB780(tev_stage + 1, ind_stage, 0, 3, tm, 6, 6, 1, 0, 0);
    fn_800BB780(tev_stage + 2, ind_stage, 0, 0, 0, 0, 0, 1, 0, 0);
}
