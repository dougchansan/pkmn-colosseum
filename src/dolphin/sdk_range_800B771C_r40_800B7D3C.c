/**
 * @file sdk_range_800B771C_r40_800B7D3C.c
 * @brief GXAttr.c middle, 0x800B7D3C - 0x800B856C (GXClearVtxDesc ..
 * GXSetArray).
 *
 * Standalone carve: SDK_RANGE_800B771C_DECLS_ONLY takes only the range
 * file's types, so this object emits these five functions in address order
 * and owns their two switch tables (.data 0x80312B48-0x80312BD0).
 */
#define SDK_RANGE_800B771C_DECLS_ONLY
#include "src/dolphin/sdk_range_800B771C.c"

/* GXClearVtxDesc */
void GXClearVtxDesc(void) {
    gx->vcdLo = 0;
    gx->vcdLo = (gx->vcdLo & ~0x600) | 0x200;
    gx->vcdHi = 0;
    gx->hasNrms = 0;
    gx->hasBiNrms = 0;
    gx->dirtyState |= 8;
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
            *va = (*va & ~0x200U) | 0x200U;
            *va = (*va & ~0x80000000U) | 0x80000000U;
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

void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac)
{
    u32* va = (u32*)((u8*)gx + 0x1C + vtxfmt * 4);
    u32* vb = (u32*)((u8*)gx + 0x3C + vtxfmt * 4);
    u32* vc = (u32*)((u8*)gx + 0x5C + vtxfmt * 4);

    SetVat(va, vb, vc, attr, cnt, type, frac);
    gx->dirtyState |= 0x10;
    *((u8*)gx + 0x4F3) |= (u8)(1 << (u8)vtxfmt);
}

void GXSetVtxAttrFmtv(s32 vtxfmt, const GXVtxAttrFmtList_800B771C* list)
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

/* RULE-EXCEPTION(user-approved): single-use inline helper evidenced only by
 * register colouring -- see docs/RULE_EXCEPTIONS.md. Its s32 parameter gives
 * the (u8) index its own temporary, numbered ahead of the hoisted gx load. */
static inline void WriteVatRegs(s32 idx) {
    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = idx | 0x70;
    GX_FIFO_U32 = *(volatile u32*)((u8*)gx + 0x1C + idx * 4);
    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = idx | 0x80;
    GX_FIFO_U32 = *(volatile u32*)((u8*)gx + 0x3C + idx * 4);
    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = idx | 0x90;
    GX_FIFO_U32 = *(volatile u32*)((u8*)gx + 0x5C + idx * 4);
}

/* __GXSetVAT: flush the dirty vertex attribute formats. */
void __GXSetVAT(void) {
    u8 i;

    for (i = 0; i < 8; i++) {
        if (((u8*)gx)[0x4F3] & (1 << i)) {
            WriteVatRegs(i);
        }
    }
    ((u8*)gx)[0x4F3] = 0;
}

void GXSetArray(s32 attr, u32 value, u8 value2) {
    s32 idx;
    s32 j;

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
        volatile u32* gx32 = (u32*)gx;
        gx32[0x22 + j] = value;
    }

    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = idx | 0xB0;
    GX_FIFO_U32 = value2;
    j = idx - 0xC;
    if (j >= 0 && j < 4) {
        volatile u32* gx32 = (u32*)gx;
        gx32[0x26 + j] = value2;
    }
}
