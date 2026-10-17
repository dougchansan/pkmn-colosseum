/** Byte-exact PDA model-texture renderer, 0x800492CC - 0x800495C8. */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x54];
    s16 x;
    s16 y;
} PdaSprite;

extern u8 lbl_803A6818[];
extern u8 lbl_80314F98[];
extern s32 lbl_804788C0;
extern s16 lbl_804788C8[2];
extern s16 lbl_804788CC[2];
extern s16 lbl_804788D0[2];
extern f32 lbl_8047BCA0;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCBC;

extern u32 menuModelRender();
extern u32 gfxVtxTexCoord();
extern u32 gfxVtxColor4u8();
extern u32 gfxVtxPos2s16();
extern u32 gfxEnd();
extern u32 gfxBegin();
extern u32 gfxSetPrimitive();
extern u32 gfxSetVtxDesc();
extern u32 gfxLoadTexture();
extern u32 gfxClearFlags();
extern u32 gfxSetFlags();

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void fn_800492CC(u8* context, PdaSprite* sprite)
{
    u32 firstTexture;
    u32 secondTexture;
    s32 alpha;
    s32 mode;

    secondTexture = 0;
    if (lbl_804788C0 == 0) {
        return;
    }
    mode = *(s32*)((u8*)&lbl_803A6818 + 0x1C);
    if (mode == 0xC) {
        return;
    }
    switch (mode) {
    case 1:
    case 2:
        lbl_804788C8[0] = lbl_804788CC[0];
        lbl_804788C8[1] = lbl_804788CC[1];
        break;
    case 4:
        lbl_804788C8[0] = 0;
        lbl_804788C8[1] = 0;
        break;
    case 5:
        lbl_804788C8[0] = lbl_804788D0[0];
        lbl_804788C8[1] = lbl_804788D0[1];
        break;
    }
    if (mode == 5) {
        if (*((u8*)&lbl_803A6818 + 0x214) != 0) {
            firstTexture = menuModelRender((u8*)&lbl_803A6818 + 0x7C);
            secondTexture = menuModelRender((u8*)&lbl_803A6818 + 0xC4);
        } else {
            firstTexture = 0;
            secondTexture = 0;
        }
    } else if (*((u8*)&lbl_803A6818 + 0x214) != 0) {
        firstTexture = menuModelRender((u8*)&lbl_803A6818 + 0x7C);
    } else {
        firstTexture = 0;
    }
    alpha = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x5C);
    if (firstTexture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, firstTexture);
        gfxBegin(2);
        gfxVtxPos2s16(lbl_804788C8[0], lbl_804788C8[1]);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BC94, lbl_8047BC94);
        gfxVtxPos2s16((s16)(lbl_804788C8[0] + (s16)sprite->x),
                     (s16)(lbl_804788C8[1] + (s16)sprite->y));
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BCBC, lbl_8047BCBC);
        gfxEnd();
    }
    if (secondTexture != 0) {
        gfxSetFlags(3);
        gfxClearFlags(4);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(lbl_80314F98);
        gfxLoadTexture(0, secondTexture);
        gfxBegin(2);
        gfxVtxPos2s16(lbl_804788C8[0], lbl_804788C8[1]);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BC94, lbl_8047BC94);
        gfxVtxPos2s16((s16)(lbl_804788C8[0] + (s16)sprite->x),
                     (s16)(lbl_804788C8[1] + (s16)sprite->y));
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, alpha);
        gfxVtxTexCoord(0, lbl_8047BCBC, lbl_8047BCBC);
        gfxEnd();
    }
}
#pragma peephole reset
