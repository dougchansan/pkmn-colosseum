/**
 * @file gs_thread_hi_range_800FE35C.c
 * @brief Sprite screen environment: the whole retail TU at
 *        .text 0x800FE35C-0x800FE6DC (scissor, sprite camera/projection,
 *        screen scale and sprite origin accessors).
 *
 * Extent: GSmsg's text ends at 0x800FE35C and gs_gapp.c starts at
 * 0x800FE6DC. Its .sdata2 pool is 0x8047CD50-0x8047CD80 (the int-to-float
 * bias, then 640, 480, 0.5, the 15-degree field of view, 0.0, -1.0, 30,
 * 0.1, 30000): GSmsg's constants end at 0x8047CD50 and gapp's start at
 * 0x8047CD80, and no other code references this run. Compiled as one unit
 * this source emits exactly that pool in that order. The screen scale
 * (lbl_80478B10/14, .sdata) and sprite origin (lbl_8047AC70/72, .sbss)
 * stay extern.
 *
 * XD names (TeamOrre xd-decomp config/GXXE01/symbols.txt at 4989794e):
 * spriteClearScissor (fn_800FE35C), spriteSetScissor (fn_800FE38C),
 * spriteSetEnv, spriteGetOffset/spriteSetOffset (the origin accessors).
 * XD's copies take extra arguments, so the Colosseum symbols keep their
 * address names.
 *
 * Flags: GC/1.3, -O4,p and -opt nopeephole unit-wide. Without
 * -opt nopeephole fn_800FE35C's prologue store is scheduled after the
 * argument loads, unlike retail. All six functions are exact and the unit
 * links whole (it replaced the fn_800FE35C and accessor carves).
 *
 * fn_800FE38C (lane D9, 2026-09-29): the clipped edges reuse the result
 * variables: `right = left + w`, then `right = (f32)right * scale`.
 * Retail keeps each edge in one register web (the GPR colouring simulator
 * finds exactly one single change that gives retail: merging x1's web with
 * `right`'s). MWCC only keeps the reused variable as one web with
 * `opt_lifetimes off`. With lifetime splitting on, this form is 14
 * instructions off, and separate x0/x1 locals leave x1 in x0's dead r3
 * (2 instructions, lane U3's wall; copy chains, param reuse and other
 * pragmas do not move it). The pragma is a RULE-EXCEPTION, listed in
 * docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern f32 lbl_80478B10; /* screen x scale */
extern f32 lbl_80478B14; /* screen y scale */
extern s16 lbl_8047AC70; /* sprite origin x */
extern s16 lbl_8047AC72; /* sprite origin y */

extern f64 tan(f64 x);
extern void set__5GSvecFfff(f32* v, f32 x, f32 y, f32 z);
extern void GSmtxLookAt(f32* mtx, f32* eye, f32* up, f32* target);
extern void gfxSetPerspective(f32 fovy, f32 aspect, f32 nearZ, f32 farZ);
extern void gfxLoadIdentity(void);
extern void gfxMultMatrix(f32* mtx);
extern void gfxSetBlendMode(s32 a, s32 b, s32 c);
extern void gfxClearFlags(u32 mask);
extern void gfxSetUpdates(s32 a, s32 b, s32 c);
extern void gfxSetAlphaCompare(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void gfxSetZMode(s32 a, s32 b, s32 c);
extern void gfxSetCullMode(s32 a);
extern void gfxSetScissor(u16 left, u16 top, u16 right, u16 bottom);

void fn_800FE35C(void) {
    gfxSetScissor(0, 0, 639, 479);
}

/* RULE-EXCEPTION(title-path): local compiler control - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_lifetimes off
void fn_800FE38C(s32 x, s32 y, s32 w, s32 h) {
    s32 bottom, right, top, left;

    left = lbl_8047AC70 + x;
    top = lbl_8047AC72 + y;
    right = left + w;
    bottom = top + h;
    left = (f32)left * lbl_80478B10;
    top = (f32)top * lbl_80478B14;
    right = (f32)right * lbl_80478B10;
    bottom = (f32)bottom * lbl_80478B14;
    if (left >= 640) left = 639;
    if (top >= 480) top = 479;
    if (right >= 640) right = 639;
    if (bottom >= 480) bottom = 479;
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right < 0) right = 0;
    if (bottom < 0) bottom = 0;
    gfxSetScissor(left, top, right, bottom);
}
#pragma pop

void spriteSetEnv(void) {
    f32 eye[3];
    f32 target[3];
    f32 up[3];
    f32 mtx[12];
    f32 cx;
    f32 cy;
    f32 width;
    f32 height;
    f32 z;
    f32 fov;
    f32 half;

    width = 640.0f / lbl_80478B10;
    height = 480.0f / lbl_80478B14;
    fov = 0.2617994f;
    half = 0.5f;
    cx = width * half;
    cy = height * half;
    z = cy / (f32)tan(fov);

    set__5GSvecFfff(eye, cx - lbl_8047AC70, cy - lbl_8047AC72, z);
    set__5GSvecFfff(target, cx - lbl_8047AC70, cy - lbl_8047AC72, 0.0f);
    set__5GSvecFfff(up, 0.0f, -1.0f, 0.0f);
    GSmtxLookAt(mtx, eye, up, target);

    gfxSetPerspective(30.0f, -(width / height), 0.1f, 30000.0f);
    gfxLoadIdentity();
    gfxMultMatrix(mtx);
    gfxSetBlendMode(1, 6, 7);
    gfxClearFlags(0x80000000);
    gfxSetUpdates(2, 2, 1);
    gfxSetAlphaCompare(0, 7, 0, 1, 7, 0);
    gfxSetZMode(0, 2, 0);
    gfxSetCullMode(0);
}

void fn_800FE6A0(f32 x, f32 y) {
    lbl_80478B10 = x;
    lbl_80478B14 = y;
}

void fn_800FE6AC(s16* x, s16* y) {
    if (x != NULL) {
        *x = lbl_8047AC70;
    }
    if (y != NULL) {
        *y = lbl_8047AC72;
    }
}

void fn_800FE6D0(s16 x, s16 y) {
    lbl_8047AC70 = x;
    lbl_8047AC72 = y;
}
