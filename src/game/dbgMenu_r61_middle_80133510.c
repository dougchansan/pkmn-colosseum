/**
 * @file dbgMenu_r61_middle_80133510.c
 * @brief dbgMenu carve, 0x80133510 - 0x80133630: debugMenuNodrawArea,
 *        dbgMenuTogglePartner (party member toggle) and debugMenuChangeMemInfo.
 *
 * Text only, GC/1.3 -O4,p, no pragmas. The three entries are debug-menu
 * callbacks referenced only from unlinked data, so they are listed in
 * configure.py's force_active_symbols.
 */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

u32 debugMenuNodrawArea(void)
{
    if (menuIsCheck(0x7) & 0xFF) {
        menuClose(0x7);
    } else {
        menuOpenCustom(0x7, 0, 0, 0, 0, 0);
    }
    return 0;
}

u32 dbgMenuTogglePartner(u32 arg1, s32 arg2)
{
    extern u32 heroMoveIsMember(s32 id);
    extern void heroMoveDismissMember(s32 id);
    extern void partnerSetFollowing(s32 id);
    s32 id;

    id = -1;
    switch (arg2) {
    case 0:
        id = 1;
        break;
    }
    if (id >= 0) {
        if (heroMoveIsMember(id) & 0xFF) {
            heroMoveDismissMember(id);
        } else {
            partnerSetFollowing(id);
        }
    }
    return 0;
}

u32 debugMenuChangeMemInfo(void)
{
    if (menuIsCheck(0x4) & 0xFF) {
        menuClose(0x4);
    } else {
        menuOpenCustom(0x4, 0, 0, 0, 0, 0);
    }
    return 0;
}
