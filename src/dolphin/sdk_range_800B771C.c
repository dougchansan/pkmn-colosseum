/**
 * @file sdk_range_800B771C.c
 * @brief dolphin-sdk code, 0x800B771C - 0x800B7D3C. The GXAttr.c functions
 * from 0x800B7D3C to 0x800B856C are in sdk_range_800B771C_r40_800B7D3C.c.
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
extern void __GXXfVtxSpecs(void);
/* __GXCalculateVLim's component-count tables (.sdata). */
extern u8 lbl_80478A68[4];
extern u8 lbl_80478A6C[4];
extern u8 lbl_80478A70[4];

typedef union GXWGPipe_800B771C {
    u8 u8;
    u32 u32;
} GXWGPipe_800B771C;

volatile GXWGPipe_800B771C GXWGFifo_800B771C : 0xCC008000;

#define GX_FIFO_U8  GXWGFifo_800B771C.u8
#define GX_FIFO_U32 GXWGFifo_800B771C.u32

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

#if !defined(SDK_RANGE_800B771C_DECLS_ONLY)
/* __GXXfVtxSpecs */
#define GX_REG_FIELD_800B771C(reg, size, shift) (((reg) >> (shift)) & ((1 << (size)) - 1))

void __GXXfVtxSpecs(void)
{
    GXData_800B771C* data = (GXData_800B771C*)gx;
    u32 nCols;
    u32 nNrm;
    u32 nTex;
    u32 reg;

    reg = data->vcdLo;
    nCols = GX_REG_FIELD_800B771C(reg, 2, 13) ? 1 : 0;
    nCols += GX_REG_FIELD_800B771C(reg, 2, 15) ? 1 : 0;
    nNrm = data->hasBiNrms ? 2 : data->hasNrms ? 1 : 0;
    nTex = 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 0) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 2) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 4) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 6) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 8) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 10) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 12) ? 1 : 0;
    nTex += GX_REG_FIELD_800B771C(data->vcdHi, 2, 14) ? 1 : 0;
    reg = nCols | (nNrm << 2) | (nTex << 4);
    GX_FIFO_U8 = 0x10;
    GX_FIFO_U32 = 0x1008;
    GX_FIFO_U32 = reg;
    *(volatile u16*)((u8*)gx + 2) = 1;
}

void GXSetVtxDesc(s32 attr, s32 type)
{
    SetVcdAttr(attr, type);
    if (gx->hasNrms || gx->hasBiNrms) {
        gx->vcdLo = (gx->vcdLo & ~0x1800U) | (gx->nrmType << 11);
    } else {
        gx->vcdLo = gx->vcdLo & ~0x1800U;
    }
    gx->dirtyState |= 8;
}

void __GXSetVCD(void) {
    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = 0x50;
    GX_FIFO_U32 = gx->vcdLo;
    GX_FIFO_U8 = 0x8;
    GX_FIFO_U8 = 0x60;
    GX_FIFO_U32 = gx->vcdHi;
    __GXXfVtxSpecs();
}
#endif

/* __GXCalculateVLim (0x800B7C18) is the next object,
 * sdk_range_800B771C_r40_800B7C18_gc11p1.c, which defines
 * SDK_RANGE_800B771C_VLIM. */
#if defined(SDK_RANGE_800B771C_VLIM)
void __GXCalculateVLim(void)
{
    u32 vlim;
    u32 vcdLoReg;
    u32 vcdHiReg;
    s32 compCnt;

    if (gx->vNum == 0) {
        return;
    }

    vcdLoReg = gx->vcdLo;
    vcdHiReg = gx->vcdHi;

    compCnt = *(u32*)((u8*)gx + 0x1C);
    compCnt = (compCnt & 0x200) >> 9;

    vlim = ((vcdLoReg >> 0) & 1);
    vlim += ((vcdLoReg >> 1) & 1);
    vlim += ((vcdLoReg >> 2) & 1);
    vlim += ((vcdLoReg >> 3) & 1);
    vlim += ((vcdLoReg >> 4) & 1);
    vlim += ((vcdLoReg >> 5) & 1);
    vlim += ((vcdLoReg >> 6) & 1);
    vlim += ((vcdLoReg >> 7) & 1);
    vlim += ((vcdLoReg >> 8) & 1);

    vlim += lbl_80478A70[((vcdLoReg >> 9) & 3)];
    vlim += lbl_80478A70[((vcdLoReg >> 11) & 3)] * (compCnt == 1 ? 3 : 1);
    vlim += lbl_80478A68[((vcdLoReg >> 13) & 3)];
    vlim += lbl_80478A68[((vcdLoReg >> 15) & 3)];

    vlim += lbl_80478A6C[((vcdHiReg >> 0) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 2) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 4) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 6) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 8) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 10) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 12) & 3)];
    vlim += lbl_80478A6C[((vcdHiReg >> 14) & 3)];

    gx->vLim = vlim;
}
#endif
