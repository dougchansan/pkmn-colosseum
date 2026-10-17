/**
 * @file gapp_exact_80101B90.c
 * @brief gapp load meter: fn_80101B90 (frame-time bar), fn_80101D5C and
 *        fn_80101D8C (meter frame and scale lines), 0x80101B90 - 0x80101FB8.
 *
 * One object because fn_80101B90 and fn_80101D8C share this translation
 * unit's literal pool (.sdata2 0x8047CD80 - 0x8047CDC0: 0.0f, 640.0f,
 * 480.0f, 685000.0f, 450.0f, 452.0f, the unsigned int-to-float bias,
 * 2740000.0f, 454.0f, 456.0f, 448.0f, 458.0f and the signed bias), which
 * the unit owns. The constants are literals: as extern variables MWCC
 * cannot schedule their loads the way retail does. gfxVtxColor4u8 takes u8
 * colour components (it is the GSgfx 4u8 colour setter); with that
 * prototype and masked u8 components, MWCC extracts the colour into
 * scratch registers before fn_800D67BC(2) and copies them to their
 * callee-saved homes, as retail does.
 */
#include "dolphin/types.h"

extern u32 lbl_8047ACF0;
extern u32 lbl_8047ACF8;
extern u32 lbl_8047ACEC;
extern u32 lbl_8047ACE8;
extern u8 lbl_80478B28;

extern u32 OSGetTick(void);
extern void GXSetDrawSync(s32);
extern void fn_800BE30C(void);
extern void fn_800D9ED8(s32);
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void gfxSetOrtho(f32, f32, f32, f32);
extern void gfxSetBlendMode(s32, s32, s32);
extern void gfxSetUpdates(s32, s32, s32);
extern void gfxSetZMode(s32, s32, s32);
extern void gfxSetCullMode(s32);
extern void gfxSetPrimitive(s32);
extern void gfxSetVtxDesc(s32);
extern void gfxBegin(s32);
extern void gfxVtxPos(f32, f32, f32);
extern void gfxVtxColor4u8(u32, u8, u8, u8, u8);
extern void gfxEnd(void);

void fn_80101B90(u32 color) {
    u32 tick;
    u8 red;
    u8 green;
    u8 blue;

    if ((s32)lbl_8047ACF0 != 0) {
        tick = OSGetTick();
        fn_800D9ED8(1);
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
        gfxSetBlendMode(1, 6, 7);
        gfxSetUpdates(2, 1, 0);
        gfxSetZMode(1, 1, 1);
        gfxSetCullMode(0);
        gfxSetPrimitive(7);
        gfxSetVtxDesc(0);

        red = (color >> 16) & 0xFF;
        green = (color >> 8) & 0xFF;
        blue = color & 0xFF;
        gfxBegin(2);
        gfxVtxPos(640.0f * (((f32)(lbl_8047ACEC - lbl_8047ACE8) / (f32)lbl_80478B28) /
                                    685000.0f),
                    450.0f, 0.0f);
        gfxVtxColor4u8(0, red, green, blue, 0xFF);
        gfxVtxPos(640.0f * (((f32)(tick - lbl_8047ACE8) / (f32)lbl_80478B28) /
                                    685000.0f),
                    452.0f, 0.0f);
        gfxVtxColor4u8(0, red, green, blue, 0xFF);
        gfxEnd();
        lbl_8047ACEC = tick;
    }
}

void fn_80101D5C(void) {
    if ((s32)lbl_8047ACF0 != 0) {
        GXSetDrawSync(3);
    }
}

void fn_80101D8C(void)
{
    f32 x;
    s32 offset;
    s8 line;
    u32 tick;

    if ((s32)lbl_8047ACF0 == 0) {
        return;
    }

    gfxSetFlags(1);
    gfxClearFlags(6);
    gfxSetOrtho(0.0f, 0.0f, 640.0f, 480.0f);
    gfxSetBlendMode(1, 6, 7);
    gfxSetUpdates(2, 1, 0);
    gfxSetZMode(1, 1, 1);
    gfxSetCullMode(0);

    x = 640.0f * (((f32)lbl_8047ACF8 / 2740000.0f) / (f32)lbl_80478B28);
    gfxSetPrimitive(7);
    gfxSetVtxDesc(0);
    gfxBegin(2);
    gfxVtxPos(0.0f, 454.0f, 0.0f);
    gfxVtxColor4u8(0, 0, 0xFF, 0, 0xFF);
    gfxVtxPos(x, 456.0f, 0.0f);
    gfxVtxColor4u8(0, 0, 0xFF, 0, 0xFF);
    gfxEnd();

    gfxSetPrimitive(1);
    gfxSetVtxDesc(0);
    line = 0;
    offset = 0;
    for (; line <= lbl_80478B28; line++) {
        x = (f32)(offset / lbl_80478B28);
        gfxBegin(2);
        gfxVtxPos(x, 448.0f, 0.0f);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxVtxPos(x, 458.0f, 0.0f);
        gfxVtxColor4u8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        gfxEnd();
        offset += 0x280;
    }

    tick = OSGetTick();
    lbl_8047ACE8 = tick;
    lbl_8047ACEC = tick;
    fn_800BE30C();
}
