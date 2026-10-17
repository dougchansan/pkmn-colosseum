/**
 * @file menu_exact_80103484.c
 * @brief menu engine, 0x80103484-0x80103614: menuPlaySe and the AGB pad
 *        reader.
 *
 * Function-boundary carve of the menu TU (see src/game/menu.c), built with
 * the TU's flags (configure.py). Text only: neither function uses a pooled
 * literal, and neither inlines a function of the TU. Retail calls
 * _menuGetAgbKeyInfo from _menuUpdateKeyInfo (menu_candidate_80103614.c)
 * rather than inlining it.
 */
#include "dolphin/types.h"

typedef struct MenuData {
    u8 soundGroup;
} MenuData;

extern const MenuData* menuDataBiosGetPtr(u32 id);
extern void* menuSeBiosGetPtr(s32 group);
extern u16 menuSeTableLookup(void*, s32);
extern int soundStartSE(u16);
extern u8 fn_8008ABA0(s32);
extern s32 fn_8008AB8C(s32);

/* 0x80103484 | 0x58 */
void menuPlaySe(u32 id, s32 event) {
    void* sound = menuSeBiosGetPtr(menuDataBiosGetPtr(id)->soundGroup & 7);

    if (sound != NULL) {
        u32 se = menuSeTableLookup(sound, event);
        if (se != 0) {
            soundStartSE(se);
        }
    }
}

/* 0x801034DC | 0x138 */
u8 _menuGetAgbKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 device;
    u16 buttons;
    u16 result;

    device = port + 1;
    result = 0;
    if (fn_8008ABA0(device) == 0) {
        return 0;
    }

    buttons = fn_8008AB8C(device);
    if (buttons & 0x40) result |= 0x1;
    if (buttons & 0x80) result |= 0x2;
    if (buttons & 0x20) result |= 0x4;
    if (buttons & 0x10) result |= 0x8;
    if (buttons & 0x1) result |= 0x10;
    if (buttons & 0x2) result |= 0x20;
    if (buttons & 0x4) result |= 0x100;
    if (buttons & 0x200) result |= 0x200;
    if (buttons & 0x100) result |= 0x400;
    if (buttons & 0x8) result |= 0x800;

    *keys = result;
    return 1;
}
