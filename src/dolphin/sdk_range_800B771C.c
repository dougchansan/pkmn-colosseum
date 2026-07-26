/**
 * @file sdk_range_800B771C.c
 * @brief dolphin-sdk code, 0x800B771C - 0x800B856C (9 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef struct GXData_800B771C {
    /* 0x000 */ u8 _000[0x4];
    /* 0x004 */ u16 vNum;
    /* 0x006 */ u16 vLim;
    /* 0x008 */ u8 _008[0xC];
    /* 0x014 */ u32 vcdLo;
    /* 0x018 */ u32 vcdHi;
    /* 0x01C */ u8 _01C[0x3FC];
    /* 0x418 */ u32 nrmType;
    /* 0x41C */ u8 hasNrms;
    /* 0x41D */ u8 hasBiNrms;
    /* 0x41E */ u8 _41E[0xD6];
    /* 0x4F4 */ u32 dirtyState;
} GXData_800B771C;

extern volatile GXData_800B771C* const gx;
extern void fn_800B771C(void);
extern const u8 lbl_80478A68[];
extern const u8 lbl_80478A6C[];
extern const u8 lbl_80478A70[];

#define GX_FIFO_U8  (*(volatile u8*)0xCC008000)
#define GX_FIFO_U32 (*(volatile u32*)0xCC008000)

typedef struct GXVtxAttrFmtList_800B771C {
    s32 attr;
    s32 cnt;
    s32 type;
    u8 frac;
} GXVtxAttrFmtList_800B771C;

static inline void SetVcdAttr(s32 attr, s32 type)
{
    switch (attr) {
    case 0: gx->vcdLo = (gx->vcdLo & ~1U) | type; break;
    case 1: gx->vcdLo = (gx->vcdLo & ~2U) | (type << 1); break;
    case 2: gx->vcdLo = (gx->vcdLo & ~4U) | (type << 2); break;
    case 3: gx->vcdLo = (gx->vcdLo & ~8U) | (type << 3); break;
    case 4: gx->vcdLo = (gx->vcdLo & ~0x10U) | (type << 4); break;
    case 5: gx->vcdLo = (gx->vcdLo & ~0x20U) | (type << 5); break;
    case 6: gx->vcdLo = (gx->vcdLo & ~0x40U) | (type << 6); break;
    case 7: gx->vcdLo = (gx->vcdLo & ~0x80U) | (type << 7); break;
    case 8: gx->vcdLo = (gx->vcdLo & ~0x100U) | (type << 8); break;
    case 9: gx->vcdLo = (gx->vcdLo & ~0x600U) | (type << 9); break;
    case 10:
        if (type != 0) {
            gx->hasNrms = 1;
            gx->hasBiNrms = 0;
            gx->nrmType = type;
        } else {
            gx->hasNrms = 0;
        }
        break;
    case 25:
        if (type != 0) {
            gx->hasBiNrms = 1;
            gx->hasNrms = 0;
            gx->nrmType = type;
        } else {
            gx->hasBiNrms = 0;
        }
        break;
    case 11: gx->vcdLo = (gx->vcdLo & ~0x6000U) | (type << 13); break;
    case 12: gx->vcdLo = (gx->vcdLo & ~0x18000U) | (type << 15); break;
    case 13: gx->vcdHi = (gx->vcdHi & ~3U) | type; break;
    case 14: gx->vcdHi = (gx->vcdHi & ~0xCU) | (type << 2); break;
    case 15: gx->vcdHi = (gx->vcdHi & ~0x30U) | (type << 4); break;
    case 16: gx->vcdHi = (gx->vcdHi & ~0xC0U) | (type << 6); break;
    case 17: gx->vcdHi = (gx->vcdHi & ~0x300U) | (type << 8); break;
    case 18: gx->vcdHi = (gx->vcdHi & ~0xC00U) | (type << 10); break;
    case 19: gx->vcdHi = (gx->vcdHi & ~0x3000U) | (type << 12); break;
    case 20: gx->vcdHi = (gx->vcdHi & ~0xC000U) | (type << 14); break;
    }
}

void fn_800B771C(void)
{
    u32 nCols;
    u32 nNrms;
    u32 nTex;
    u32 vcdLo;
    u32 vcdHi;

    vcdLo = gx->vcdLo;
    if (((vcdLo >> 13) & 3) != 0) {
        nCols = 1;
    } else {
        nCols = 0;
    }
    if (((vcdLo >> 15) & 3) != 0) {
        nCols += 1;
    }

    if (gx->hasBiNrms) {
        nNrms = 2;
    } else if (gx->hasNrms) {
        nNrms = 1;
    } else {
        nNrms = 0;
    }

    nTex = 0;
    vcdHi = gx->vcdHi;
    if ((vcdHi & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 2) & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 4) & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 6) & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 8) & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 10) & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 12) & 3) != 0) {
        nTex += 1;
    }
    if (((vcdHi >> 14) & 3) != 0) {
        nTex += 1;
    }

    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x1008;
    GX_FIFO_U32 = nCols | (nNrms << 2) | (nTex << 4);
    *(volatile u16*)((u8*)gx + 2) = 1;
}

void fn_800B7874(s32 attr, s32 type)
{
    SetVcdAttr(attr, type);
    if (gx->hasNrms || gx->hasBiNrms) {
        gx->vcdLo = (gx->vcdLo & ~0x1800U) | (gx->nrmType << 11);
    } else {
        gx->vcdLo = gx->vcdLo & ~0x1800U;
    }
    gx->dirtyState |= 8;
}

void __GXCalculateVLim(void)
{
    u32 normalCount;
    u32 limit;
    u32 multiplier;
    u32 vcdLo;
    u32 vcdHi;
    u32 vatA;

    if (gx->vNum != 0) {
        vcdLo = gx->vcdLo;
        vcdHi = gx->vcdHi;
        vatA = *(volatile u32*)((u8*)gx + 0x1C);
        normalCount = (vatA >> 9) & 1;

        limit = (vcdLo >> 0) & 1;
        limit += (u8)((vcdLo >> 1) & 1);
        limit += (u8)((vcdLo >> 2) & 1);
        limit += (u8)((vcdLo >> 3) & 1);
        limit += (u8)((vcdLo >> 4) & 1);
        limit += (u8)((vcdLo >> 5) & 1);
        limit += (u8)((vcdLo >> 6) & 1);
        limit += (u8)((vcdLo >> 7) & 1);
        limit += (u8)((vcdLo >> 8) & 1);
        limit += lbl_80478A70[(vcdLo >> 9) & 3];

        multiplier = normalCount == 1 ? 3 : 1;
        limit += lbl_80478A70[(vcdLo >> 11) & 3] * multiplier;
        limit += lbl_80478A68[(vcdLo >> 13) & 3];
        limit += lbl_80478A68[(vcdLo >> 15) & 3];
        limit += lbl_80478A6C[(vcdHi >> 0) & 3];
        limit += lbl_80478A6C[(vcdHi >> 2) & 3];
        limit += lbl_80478A6C[(vcdHi >> 4) & 3];
        limit += lbl_80478A6C[(vcdHi >> 6) & 3];
        limit += lbl_80478A6C[(vcdHi >> 8) & 3];
        limit += lbl_80478A6C[(vcdHi >> 10) & 3];
        limit += lbl_80478A6C[(vcdHi >> 12) & 3];
        limit += lbl_80478A6C[(vcdHi >> 14) & 3];
        gx->vLim = limit;
    }
}

static inline void SetVat(u32* va, u32* vb, u32* vc, s32 attr, s32 cnt,
                          s32 type, u8 frac)
{
    switch (attr) {
    case 9:
        *va = (*va & ~1U) | cnt;
        *va = (*va & ~0xEU) | (type << 1);
        *va = (*va & ~0x1F0U) | (frac << 4);
        break;
    case 10:
    case 25:
        *va = (*va & ~0x1C00U) | (type << 10);
        if (cnt == 2) {
            *va |= 0x200U;
            *va |= 0x80000000U;
        } else {
            *va = (*va & ~0x200U) | (cnt << 9);
            *va &= ~0x80000000U;
        }
        break;
    case 11:
        *va = (*va & ~0x2000U) | (cnt << 13);
        *va = (*va & ~0x1C000U) | (type << 14);
        break;
    case 12:
        *va = (*va & ~0x20000U) | (cnt << 17);
        *va = (*va & ~0x1C0000U) | (type << 18);
        break;
    case 13:
        *va = (*va & ~0x200000U) | (cnt << 21);
        *va = (*va & ~0x1C00000U) | (type << 22);
        *va = (*va & ~0x3E000000U) | (frac << 25);
        break;
    case 14:
        *vb = (*vb & ~1U) | cnt;
        *vb = (*vb & ~0xEU) | (type << 1);
        *vb = (*vb & ~0x1F0U) | (frac << 4);
        break;
    case 15:
        *vb = (*vb & ~0x200U) | (cnt << 9);
        *vb = (*vb & ~0x1C00U) | (type << 10);
        *vb = (*vb & ~0x3E000U) | (frac << 13);
        break;
    case 16:
        *vb = (*vb & ~0x40000U) | (cnt << 18);
        *vb = (*vb & ~0x380000U) | (type << 19);
        *vb = (*vb & ~0x7C00000U) | (frac << 22);
        break;
    case 17:
        *vb = (*vb & ~0x8000000U) | (cnt << 27);
        *vb = (*vb & ~0x70000000U) | (type << 28);
        *vc = (*vc & ~0x1FU) | frac;
        break;
    case 18:
        *vc = (*vc & ~0x20U) | (cnt << 5);
        *vc = (*vc & ~0x1C0U) | (type << 6);
        *vc = (*vc & ~0x3E00U) | (frac << 9);
        break;
    case 19:
        *vc = (*vc & ~0x4000U) | (cnt << 14);
        *vc = (*vc & ~0x38000U) | (type << 15);
        *vc = (*vc & ~0x7C0000U) | (frac << 18);
        break;
    case 20:
        *vc = (*vc & ~0x800000U) | (cnt << 23);
        *vc = (*vc & ~0x7000000U) | (type << 24);
        *vc = (*vc & ~0xF8000000U) | (frac << 27);
        break;
    }
}

void fn_800B7D74(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac)
{
    u32* va = (u32*)((u8*)gx + 0x1C + vtxfmt * 4);
    u32* vb = (u32*)((u8*)gx + 0x3C + vtxfmt * 4);
    u32* vc = (u32*)((u8*)gx + 0x5C + vtxfmt * 4);

    SetVat(va, vb, vc, attr, cnt, type, frac);
    gx->dirtyState |= 0x10;
    *(volatile u8*)((u8*)gx + 0x4F3) |= 1 << vtxfmt;
}

void fn_800B80CC(s32 vtxfmt, const GXVtxAttrFmtList_800B771C* list)
{
    u32* va = (u32*)((u8*)gx + 0x1C + vtxfmt * 4);
    u32* vb = (u32*)((u8*)gx + 0x3C + vtxfmt * 4);
    u32* vc = (u32*)((u8*)gx + 0x5C + vtxfmt * 4);

    while (list->attr != 0xFF) {
        SetVat(va, vb, vc, list->attr, list->cnt, list->type, list->frac);
        list++;
    }
    gx->dirtyState |= 0x10;
    *((u8*)gx + 0x4F3) |= (u8)(1 << (u8)vtxfmt);
}

void fn_800B7BC4(void) {
    volatile u32* gx32;

    GX_FIFO_U8 = 0x8;
    gx32 = (u32*)gx;
    GX_FIFO_U8 = 0x50;
    GX_FIFO_U32 = gx32[0x5];
    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = 0x60;
    GX_FIFO_U32 = gx32[0x6];
    fn_800B771C();
}

#pragma optimize_for_size on
#pragma peephole off
void fn_800B7D3C(void) {
    u32 vcdLo;

    gx->vcdLo = 0;
    vcdLo = *(volatile u32*)((u32)gx + 0x14);
    gx->vcdLo = (vcdLo & 0xFFFFF9FF) | 0x200;
    gx->vcdHi = 0;
    gx->hasNrms = 0;
    gx->hasBiNrms = 0;
    gx->dirtyState |= 8;
}
#pragma peephole reset
#pragma optimize_for_size reset

#pragma optimize_for_size on
#pragma opt_common_subs off
void fn_800B8444(void) {
    u8 i;
    u32 off;
    volatile u8* gx8 = (u8*)gx;

    off = 0;
    for (i = 0; i < 8; i++) {
        if (gx8[0x4F3] & (1 << i)) {
            GX_FIFO_U8 = 0x8;
            GX_FIFO_U8 = i | 0x70;
            GX_FIFO_U32 = *(volatile u32*)(gx8 + off + 0x1C);
            GX_FIFO_U8 = 0x8;
            GX_FIFO_U8 = i | 0x80;
            GX_FIFO_U32 = *(volatile u32*)(gx8 + off + 0x3C);
            GX_FIFO_U8 = 0x8;
            GX_FIFO_U8 = i | 0x90;
            GX_FIFO_U32 = *(volatile u32*)(gx8 + off + 0x5C);
        }
        off += 4;
    }
    ((volatile u8*)gx)[0x4F3] = 0;
}
#pragma opt_common_subs reset
#pragma optimize_for_size reset

void fn_800B84E0(s32 attr, u32 value, u8 value2) {
    s32 idx;
    s32 j;
    volatile u32* gx32 = (u32*)gx;

    if (attr == 0x19) {
        attr = 0xA;
    }
    idx = attr - 9;

    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = idx | 0xA0;
    value &= 0x3FFFFFFF;
    GX_FIFO_U32 = value;
    j = idx - 0xC;
    if (j >= 0 && j < 4) {
        gx32[0x22 + j] = value;
    }

    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = idx | 0xB0;
    GX_FIFO_U32 = value2;
    j = idx - 0xC;
    if (j >= 0 && j < 4) {
        gx32[0x26 + j] = value2;
    }
}
