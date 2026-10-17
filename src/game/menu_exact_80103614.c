/**
 * @file menu_exact_80103614.c
 * @brief menu engine, 0x80103614-0x801038F8: _menuGetGcKeyInfo, the GC pad
 *        reader (stick direction and buttons to menu keys).
 *
 * Function-boundary carve of the menu TU (see src/game/menu.c), built with
 * the TU's flags (configure.py). It owns the TU's data that only this
 * function uses: the {1, 2, 3, 4} pad-id initializer (.rodata
 * 0x80271E00-0x80271E10) and the angle literals plus int-to-float double
 * (.sdata2 0x8047CDC8-0x8047CDE0). The 0.0f it compares against is the TU
 * pool's first entry (0x8047CDC0), shared with menuReleaseOffScreen in
 * menu_r50_80102014_prefix.c, so it is read through an extern stand-in;
 * common-subexpression elimination is off for the function so that load is
 * repeated at each use as retail's pooled literal is.
 */
#include "dolphin/types.h"

double atan2(double y, double x);
u8 GSinputIsConnected(s32 pad_id);
u32 GSinputGetLeftStickYData(s32 pad_id, s32 axis);
u32 GSinputGetLeftStickXData(s32 pad_id, s32 axis);
u32 fn_800F7BC4(s32 pad_id);

/* RULE-EXCEPTION(title-path): extern stand-in for the menu TU's pooled 0.0f - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047CDC0;

/* RULE-EXCEPTION(title-path): common-subexpression elimination off only so
 * the extern 0.0f is reloaded at each use, as the pooled literal is - see
 * docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_common_subs off
/* 0x80103614 | 0x2E4 */
u8 _menuGetGcKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 padIds[4] = { 1, 2, 3, 4 };
    u16 result = 0;
    s32 padId;
    u32 stickX;
    u32 stickY;
    u32 buttons;
    f32 angle;

    padId = padIds[port];
    if (GSinputIsConnected(padId) == 0) {
        return 0;
    }

    stickX = GSinputGetLeftStickYData(padId, 0);
    stickY = GSinputGetLeftStickXData(padId, 0);
    if (((s8)stickY < 0 ? -(s8)stickY : (s8)stickY) > 0x20 ||
        ((s8)stickX < 0 ? -(s8)stickX : (s8)stickX) > 0x20) {
        angle = atan2((s8)stickY, (s8)stickX);
        if ((angle > lbl_8047CDC0 ? angle : -angle) < 0.9599311f) {
            result |= 2;
        } else if ((angle > lbl_8047CDC0 ? angle : -angle) > 2.1816616f) {
            result |= 1;
        }
        if (0.61086524f < (angle > lbl_8047CDC0 ? angle : -angle) &&
            (angle > lbl_8047CDC0 ? angle : -angle) < 2.5307274f) {
            if (angle < lbl_8047CDC0) {
                result |= 4;
            } else {
                result |= 8;
            }
        }
    }

    buttons = fn_800F7BC4(padId);
    if (buttons & 0x8) result |= 0x1;
    if (buttons & 0x4) result |= 0x2;
    if (buttons & 0x1) result |= 0x4;
    if (buttons & 0x2) result |= 0x8;
    if (buttons & 0x100) result |= 0x10;
    if (buttons & 0x200) result |= 0x20;
    if (buttons & 0x400) result |= 0x40;
    if (buttons & 0x800) result |= 0x80;
    if (buttons & 0x10) result |= 0x100;
    if (buttons & 0x40) result |= 0x200;
    if (buttons & 0x20) result |= 0x400;
    if (buttons & 0x1000) result |= 0x800;

    *keys = result;
    return 1;
}
#pragma pop
