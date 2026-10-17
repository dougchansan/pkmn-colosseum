/**
 * @file menuNameEntry_exact_80028728.c
 * @brief Pokemon model draw callback for the name-entry backdrop.
 */
#include "dolphin/types.h"

typedef struct NameEntryWindow {
    u8 pad00[0x54];
    s16 width;
    s16 height;
    u8 pad58[8];
    s32* mode;
} NameEntryWindow;

extern void* menuModelRender(void* work);
extern void gfxClearFlags(s32 flags);
extern void gfxSetFlags(s32 flags);
extern void gfxSetVtxDesc(void* resource);
extern void gfxLoadTexture(s32 slot, void* texture);
extern void gfxSetPrimitive(s32 mode);
extern void gfxBegin(s32 mode);
extern void gfxVtxPos2s16(s32 x, s32 y);
extern void gfxVtxColor4u8(s32, s32, s32, s32, s32);
extern void gfxVtxTexCoord(s32 slot, f32 xScale, f32 yScale);
extern void gfxEnd(void);
extern u8 lbl_803A2094[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;

s32 menuNameEntryBackDrawPokemonModel(NameEntryWindow* window,
                                      NameEntryWindow* output)
{
    void* texture;

    if (*window->mode != 2) {
        return 0;
    }
    texture = menuModelRender(lbl_803A2094);
    if (texture != NULL) {
        gfxClearFlags(4);
        gfxSetFlags(3);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, texture);
        gfxSetPrimitive(7);
        gfxBegin(2);
        gfxVtxPos2s16(0, 0);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047B930, lbl_8047B930);
        gfxVtxPos2s16(output->width, output->height);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxTexCoord(0, lbl_8047B934, lbl_8047B934);
        gfxEnd();
    }
    return 0;
}
