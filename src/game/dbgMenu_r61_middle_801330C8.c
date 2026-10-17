#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 fn_801330C8(void)
{
    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetOrtho(lbl_8047D0F0, lbl_8047D0F0, lbl_8047D0F4, lbl_8047D0F8);
    gfxSetBlendMode(0, 6, 7);
    gfxSetUpdates(1, 1, 0);
    gfxSetZMode(0, 1, 1);
    gfxSetCullMode(0);
    gfxSetPrimitive(4);
    gfxSetVtxDesc(0);
    gfxBegin(3);
    gfxVtxPos(lbl_8047D0FC, -*(f32*)lbl_80478AC0, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0xFF, 0x80, 0x40, 0);
    gfxVtxPos(*(f32*)lbl_80478AC4, lbl_8047D100, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0x40, 0xFF, 0, 0);
    gfxVtxPos(lbl_8047D104, *(f32*)lbl_80478AC4, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0, 0x40, 0xFF, 0);
    gfxVtxPos(lbl_8047D0FC, -*(f32*)lbl_80478AC0, lbl_8047D0F0);
    gfxVtxColor4u8(0, 0xFF, 0x80, 0x40, 0);
    gfxEnd();
    return 0;
}
