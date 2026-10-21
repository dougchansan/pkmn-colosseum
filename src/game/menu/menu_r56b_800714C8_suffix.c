/**
 * @file menu_r56b_800714C8_suffix.c
 * @brief menuCB_Common.c tail 0x800714C8 - 0x800716C8: _menuPop, _menuPush,
 *        _menuPeekStackDepth, menuCB_InitMenu and fn_8007169C.
 *
 * Function-boundary carve, text only (GC/1.3 -O4,p with the peephole pass
 * off for the whole unit). The bodies are menu_range_8007109C.c's (the
 * menuCB_Common.c block, whose candidate chunks still score against that
 * file); keep the two in step. The call stack at lbl_803B6D88 is
 * {id, flag}[8] followed by the depth word at +0x40.
 *
 * _menuPop's flag clear indexes the flag column as an s32 array
 * (((s32*)(stack + 4))[depth * 2]): the byte-offset form adds the base
 * after the scaled depth and loads it into a different register (99.34%).
 * Every function here needs the peephole pass off (the range file wraps
 * each in a local pragma), so the unit is built with -opt nopeephole.
 * RULE-EXCEPTION(user-approved): per-unit compiler flag and a shaping index
 * form - see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern u8 lbl_803B6D88[0x58];
extern void floorLink(s32, s32);
extern void __assert(const char* file, u32 line, const char* msg);
extern const u8 lbl_80268708[]; /* "menuCB_Common.c" */
extern const u8 lbl_80268718[]; /* _menuPop underflow */
extern const u8 lbl_80268750[]; /* _menuPush overflow */

/* _menuPop (0x800714C8): close the top menu and pop the call stack. */
s32 _menuPop(void) {
    extern s32 windowGetActiveID(void);
    extern u32 windowSearchID(s32 id);
    extern void menuCloseCustom(s32 slot, s32 p1, s32 p2);
    s32 top;
    s32 active;
    u32 d;

    top = *(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8);
    active = windowGetActiveID();
    if (active == top) {
        menuCloseCustom(*(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8), 0, 0);
    }
    if (windowSearchID(0xBE) != 0) {
        menuCloseCustom(0xBE, 0, 1);
    }
    ((s32*)(lbl_803B6D88 + 4))[*(u32*)(lbl_803B6D88 + 0x40) * 2] = 0;
    if (*(s32*)(lbl_803B6D88 + 0x40) == 0) {
        return -1;
    }
    if (!(0 < *(s32*)(lbl_803B6D88 + 0x40))) {
        __assert((const char*)lbl_80268708, 0x5C, (const char*)lbl_80268718);
    }
    d = *(u32*)(lbl_803B6D88 + 0x40) - 1;
    *(u32*)(lbl_803B6D88 + 0x40) = d;
    return *(s32*)(lbl_803B6D88 + d * 8);
}

/* _menuPush (0x800715BC): push a menu id onto the call stack. */
void _menuPush(s32 id) {
    u32 depth;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    if (depth >= 8) {
        __assert((const char*)lbl_80268708, 0x41, (const char*)lbl_80268750);
    } else {
        *(u32*)(lbl_803B6D88 + 0x40) = depth + 1;
        *(s32*)(lbl_803B6D88 + (depth + 1) * 8) = id;
        *(s32*)(lbl_803B6D88 + *(u32*)(lbl_803B6D88 + 0x40) * 8 + 4) = 0;
    }
}

/* _menuPeekStackDepth (0x8007162C): peek the current call-stack depth slot. */
s32 _menuPeekStackDepth(void) {
    u32 depth;

    depth = *(u32*)(lbl_803B6D88 + 0x40);
    return *(s32*)(lbl_803B6D88 + depth * 8);
}

/* menuCB_InitMenu (0x80071644): clear the call stack and seed slot 0. */
void menuCB_InitMenu(s32 id) {
    u32 i;

    for (i = 0; i < 8; i++) {
        *(u32*)(lbl_803B6D88 + i * 8) = 0;
        *(u32*)(lbl_803B6D88 + i * 8 + 4) = 0;
    }
    *(s32*)(lbl_803B6D88 + 0x0) = id;
    *(u32*)(lbl_803B6D88 + 0x40) = 0;
}

/* fn_8007169C (0x8007169C): fixed diagnostic-log call, always returns 0. */
s32 fn_8007169C(void) {
    floorLink(0x385, 0);
    return 0;
}
