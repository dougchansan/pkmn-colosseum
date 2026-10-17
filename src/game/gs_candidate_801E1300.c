/** Exact GSvtr texture-display setup and draw callback, 0x801E1300 - 0x801E16D0. */
#include "dolphin/types.h"

#define GS_SDATA2 __declspec(section ".sdata2")
extern GS_SDATA2 const char lbl_8047E458[];
extern GS_SDATA2 const char lbl_8047E45C[];
extern GS_SDATA2 const char lbl_8047E464[];
extern GS_SDATA2 const char lbl_8047E46C[];
extern GS_SDATA2 const char lbl_8047E474[];

void _vtrTexDispFunc__Fv(void);

void vtrInit(void)
{
    extern u32 lbl_80467CF8[];
    extern u8 lbl_8047B420;
    extern u32 lbl_8047B424;
    extern u32 lbl_8047B42C;
    extern u8 lbl_8047B434;
    extern u32 lbl_8047B438;
    extern u8 lbl_8047B43C;
    extern void *memset(void *dst, int value, u32 size);
    extern u32 GSgappCreate(s32 state, u8 priority, u32 param, void *func);

    lbl_8047B420 = 0;
    lbl_8047B424 = 0;
    lbl_8047B42C = 0;
    lbl_8047B438 = 0;
    lbl_8047B434 = 1;
    lbl_8047B43C = 0;
    memset(lbl_80467CF8, 0, 0x10);
    GSgappCreate(1, 0xFD, 10, _vtrTexDispFunc__Fv);
}

void _vtrTexDispFunc__Fv(void)
{
    extern u8 lbl_8047B420;
    extern u32 lbl_8047B424;
    extern u32 lbl_8047B428;
    extern u8 lbl_8047B434;
    extern u8 lbl_8047B435;
    extern void* lbl_8047B438;
    extern f32 lbl_8047E428;
    extern f32 lbl_8047E42C;
    extern f32 lbl_8047E430;
    extern f32 lbl_8047E434;
    extern f32 lbl_8047E438;
    extern f32 lbl_8047E43C;
    extern f32 lbl_8047E440;
    extern f32 lbl_8047E444;
    extern f32 lbl_8047E448;
    extern f32 lbl_8047E44C;
    extern f32 lbl_8047E450;
    extern f32 lbl_8047E454;
    extern u8 lbl_80314958[];
    extern u8 lbl_80314C78[];
    extern void gfxSetFlags(s32 mode);
    extern void gfxClearFlags(s32 mode);
    extern void gfxSetOrtho(f32, f32, f32, f32);
    extern void gfxSetBlendMode(s32, s32, s32);
    extern void gfxSetUpdates(s32, s32, s32);
    extern void gfxSetZMode(s32, s32, s32);
    extern void gfxSetCullMode(s32);
    extern void fn_800D9ED8(s32);
    extern void gfxSetPrimitive(s32 primitive);
    extern void gfxSetVtxDesc(void* format);
    extern void gfxBegin(s32 count);
    extern void gfxVtxPos(f32 x, f32 y, f32 z);
    extern void gfxVtxColor(s32 index, s32 red, s32 green, s32 blue);
    extern void gfxLoadTexture(s32 index, void* texture);
    extern void gfxVtxTexCoord(s32 index, f32 u, f32 v);
    extern void gfxEnd(void);
    extern void GSmsgPrintf(s32 x, s32 y, s32 color, const char* text,
                            ...);

    if (!lbl_8047B420 || !lbl_8047B434) {
        return;
    }
    if (lbl_8047B438 == NULL) {
        return;
    }

    gfxSetFlags(1);
    gfxClearFlags(4);
    gfxSetOrtho(lbl_8047E428, lbl_8047E428, lbl_8047E42C,
                lbl_8047E430);
    gfxSetBlendMode(0, 1, 1);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetCullMode(0);
    fn_800D9ED8(1);
    gfxSetPrimitive(4);

    if (lbl_8047B435) {
        gfxClearFlags(2);
        gfxSetVtxDesc(lbl_80314958);
        gfxBegin(4);
        gfxVtxPos(lbl_8047E434, lbl_8047E438, lbl_8047E428);
        gfxVtxColor(0, 0xFF, 0, 0);
        gfxVtxPos(lbl_8047E43C, lbl_8047E438, lbl_8047E428);
        gfxVtxColor(0, 0xFF, 0, 0);
        gfxVtxPos(lbl_8047E434, lbl_8047E440, lbl_8047E428);
        gfxVtxColor(0, 0xFF, 0, 0);
        gfxVtxPos(lbl_8047E43C, lbl_8047E440, lbl_8047E428);
        gfxVtxColor(0, 0xFF, 0, 0);
        gfxEnd();
    }

    gfxSetFlags(2);
    gfxSetVtxDesc(lbl_80314C78);
    gfxLoadTexture(0, lbl_8047B438);
    gfxBegin(4);
    gfxVtxPos(lbl_8047E444, lbl_8047E448, lbl_8047E428);
    gfxVtxColor(0, 0xFF, 0xFF, 0xFF);
    gfxVtxTexCoord(0, lbl_8047E428, lbl_8047E428);
    gfxVtxPos(lbl_8047E44C, lbl_8047E448, lbl_8047E428);
    gfxVtxColor(0, 0xFF, 0xFF, 0xFF);
    gfxVtxTexCoord(0, lbl_8047E450, lbl_8047E428);
    gfxVtxPos(lbl_8047E444, lbl_8047E454, lbl_8047E428);
    gfxVtxColor(0, 0xFF, 0xFF, 0xFF);
    gfxVtxTexCoord(0, lbl_8047E428, lbl_8047E450);
    gfxVtxPos(lbl_8047E44C, lbl_8047E454, lbl_8047E428);
    gfxVtxColor(0, 0xFF, 0xFF, 0xFF);
    gfxVtxTexCoord(0, lbl_8047E450, lbl_8047E450);
    gfxEnd();

    switch (lbl_8047B424) {
    case 0:
        GSmsgPrintf(0x230, 0x2C, -1, lbl_8047E458);
        break;
    case 1:
        GSmsgPrintf(0x230, 0x2C, -1, lbl_8047E45C);
        break;
    case 2:
        GSmsgPrintf(0x230, 0x2C, -1, lbl_8047E464);
        break;
    case 3:
        switch (lbl_8047B428) {
        case 0:
        case 1:
            GSmsgPrintf(0x230, 0x2C, -1, lbl_8047E46C);
            break;
        case 2:
        case 3:
            GSmsgPrintf(0x230, 0x2C, -1, lbl_8047E45C);
            break;
        }
        break;
    case 4:
        GSmsgPrintf(0x230, 0x2C, -1, lbl_8047E474);
        break;
    }
    fn_800D9ED8(0);
}
