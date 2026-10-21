/**
 * @file gs_gfx_range_800DB890.c
 * @brief Exact GS render-engine suffix, 0x800DB890-0x800DC298.
 */

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"

extern void* memcpy(void* dst, const void* src, u32 n);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void gfxRecordCommand(u32, ...);
extern u32 lbl_8047AA80;
extern u8 lbl_80400B28[];
extern u16 lbl_8047AAD0;
extern u32 lbl_8047AAD8;
extern u32 lbl_8047AAD4;

/* Retail retains this otherwise-unreferenced render-state entry-point group. */
#pragma force_active on

void gfxInitDeferredPool(u32 count)
{
    u32 i;

    lbl_8047AAD8 = count;
    lbl_8047AAD0 = _toolentryAlloc__FUl(count * 0x18);
    if (lbl_8047AAD0 != 0) {
        lbl_8047AAD4 = (u32)GSmemLock(lbl_8047AAD0);
        for (i = 0; i < lbl_8047AAD8; i++) {
            ((u8*)lbl_8047AAD4)[i * 0x18] = 0;
        }
    }
}

void gfxSetIndTexMtx(u32 idx, void* src, s8 val) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x5b, 0x12, idx, src, (s32)val); }
    else {
        u32 off = (idx - 1) * 0x1c;
        u8* dst = (u8*)lbl_80400B28 + off + 0x360;
        memcpy(dst, src, 0x18);
        dst[0x18] = val;
    }
}

void gfxSetIndTexCoordScale(u32 idx, u32 a, u32 b) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x5a, 3, idx, a, b); }
    else {
        u8 *p = lbl_80400B28 + idx * 4 + 0x34d;
        p[2] = a;
        p[3] = b;
    }
}

void gfxSetIndTexOrder(u32 idx, u32 a, u32 b) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x59, 3, idx, a, b); }
    else {
        u8 *p = lbl_80400B28 + idx * 4 + 0x34d;
        *p = a;
        *(p + 1) = b;
    }
}

void gfxSetNumIndStages(u8 val) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x58, 1, (u8)val); }
    else { lbl_80400B28[0x34c] = val; }
}

void gfxSetTevIndRepeat(u32 idx) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x57, 1, idx); }
    else {
        lbl_80400B28[idx + 0x1fc] = 1;
        lbl_80400B28[idx * 0x14 + 0x20c] = 5;
    }
}

void gfxSetTevIndBumpXYZ(u32 idx, u32 a, u32 b) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x56, 3, idx, a, b); }
    else {
        u8 *p;
        lbl_80400B28[idx + 0x1fc] = 1;
        p = lbl_80400B28 + idx * 0x14 + 0x20c;
        *p = 4;
        *(p + 1) = a;
        *(p + 2) = b;
    }
}

void gfxSetTevIndBumpST(u32 idx, u32 a, u32 b) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x55, 3, idx, a, b); }
    else {
        u8 *p;
        lbl_80400B28[idx + 0x1fc] = 1;
        p = lbl_80400B28 + idx * 0x14 + 0x20c;
        *p = 3;
        *(p + 1) = a;
        *(p + 2) = b;
    }
}

void gfxSetTevIndTile(u32 idx, u32 a, u16 b, u16 c, u16 d, u16 e, u32 f, u32 g, u32 h, u32 j) {
    u8* p;

    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x54, 10, idx, a, b, c, d, e, f, g, h, j);
    } else {
        lbl_80400B28[idx + 0x1fc] = 1;
        p = lbl_80400B28 + idx * 0x14 + 0x20c;
        *p = 2;
        p[1] = a;
        *(u16*)(p + 0xc) = b;
        *(u16*)(p + 0xe) = c;
        *(u16*)(p + 0x10) = d;
        *(u16*)(p + 0x12) = e;
        p[3] = f;
        p[2] = g;
        p[4] = h;
        p[5] = j;
    }
}

void gfxSetTevIndWarp(u32 idx, u32 a, u8 b, u8 c, u32 d) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x53, 5, idx, a, (u32)b, (u32)c, d); }
    else {
        u8 *p;
        lbl_80400B28[idx + 0x1fc] = 1;
        p = lbl_80400B28 + idx * 0x14 + 0x20c;
        *p = 1;
        *(p + 1) = a;
        *(p + 0xa) = b;
        *(p + 0xb) = c;
        *(p + 2) = d;
    }
}

void gfxSetTevIndirect(u32 idx, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u8 g, u8 h, u32 j) {
    u8* p;

    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x52, 10, idx, a, b, c, d, e, f, g, h, j);
    } else {
        lbl_80400B28[idx + 0x1fc] = 1;
        p = lbl_80400B28 + idx * 0x14 + 0x20c;
        *p = 0;
        p[1] = a;
        p[3] = b;
        p[4] = c;
        p[2] = d;
        p[6] = e;
        p[7] = f;
        p[8] = g;
        p[9] = h;
        p[5] = j;
    }
}

void gfxSetTevDirect(u32 idx) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x51, 1, idx); }
    else { lbl_80400B28[idx + 0x1fc] = 0; }
}

void gfxSetTevKColor(u32 idx, GXColor color) {
    if (*(s32*)lbl_8047AA80 == 1) {
        gfxRecordCommand(0x50, 0x14, idx, color);
    } else {
        *(u32*)(lbl_80400B28 + idx * 4 + 0x1ec) = *(u32*)&color;
    }
}

void gfxSetTevKAlphaSel(u32 idx, u32 val) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x4f, 2, idx, val); }
    else {
        u32* base = (u32*)(lbl_80400B28 + 0x1ac);
        base[idx] = val;
    }
}

void gfxSetTevKColorSel(u32 idx, u32 val) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x4e, 2, idx, val); }
    else {
        u32* base = (u32*)(lbl_80400B28 + 0x16c);
        base[idx] = val;
    }
}

void gfxSetTevColorIn(u32 idx, u32 a, u32 b, u32 c, u32 d) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x4d, 5, idx, a, b, c, d); }
    else {
        u8 *p = lbl_80400B28 + idx * 4 + 0x12b;
        *p = a;
        *(p + 1) = b;
        *(p + 2) = c;
        *(p + 3) = d;
    }
}

void gfxSetTevColorOp(u32 idx, u32 a, u32 b, u32 c, u8 d, u32 e) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x4c, 6, idx, a, b, c, (u32)d, e); }
    else {
        u8 *p = lbl_80400B28 + idx * 5 + 0x9b;
        *p = a;
        *(p + 1) = b;
        *(p + 2) = c;
        *(p + 3) = d;
        *(p + 4) = e;
    }
}

void gfxSetTevAlphaIn(u32 idx, u32 a, u32 b, u32 c, u32 d) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x4b, 5, idx, a, b, c, d); }
    else {
        u8 *p = lbl_80400B28 + idx * 4 + 0xeb;
        *p = a;
        *(p + 1) = b;
        *(p + 2) = c;
        *(p + 3) = d;
    }
}

void gfxSetTevAlphaOp(u32 idx, u32 a, u32 b, u32 c, u8 d, u32 e) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x4a, 6, idx, a, b, c, (u32)d, e); }
    else {
        u8 *p = lbl_80400B28 + idx * 5 + 0x4b;
        *p = a;
        *(p + 1) = b;
        *(p + 2) = c;
        *(p + 3) = d;
        *(p + 4) = e;
    }
}

void gfxSetNumTevStages(u8 val) {
    if (*(s32*)lbl_8047AA80 == 1) { gfxRecordCommand(0x49, 1, (u8)val); }
    else { lbl_80400B28[0x1a] = val; }
}

void gfxSetTevStage(u32 idx, u32 a, u32 b, u32 c, u32 d)
{
    u8* state = (u8*)lbl_8047AA80;

    if (*(s32*)state == 1) {
        gfxRecordCommand(0x48, 5, idx, a, b, c, d);
    } else {
        u8* dst = state + idx * 4 + 0x42E;
        dst[0] = a;
        dst[1] = b;
        dst[2] = c;
        dst[3] = d;
    }
}
