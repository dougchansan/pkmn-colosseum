/**
 * @file status.c
 * @brief Decompiled functions.
 *
 * Address range: 0x80135D10 - 0x80135F58
 *
 * Split out of the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114); see effect_util_types.h for
 * shared cross-TU declarations.
 */

#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"


/* 0x80135D10 | 0x134 */
#if 0
asm void fn_80135D10(void) {
#include "src/game/effect/effect_util_fn_80135D10.inc"
}
#else
u32 statusSetStatus(u32 kind, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5) {
    extern u32 wazaSetStatus();
    extern u32 pokemonSetStatus();
    extern u32 heroSetStatus();
    extern u32 pcboxSetStatus();
    extern u32 gamedataSetStatus();
    extern u32 fn_80142B24();
    extern u32 fightFloorSetStatus();
    extern u32 fightSideSetStatus();
    extern u32 fightTrainerSetStatus();
    u32 result = 0;

    switch ((u8)kind) {
    case 0:
        break;
    case 1:
        gamedataSetStatus(arg1, arg3, arg5);
        break;
    case 2:
        pcboxSetStatus(arg1, arg3, arg5);
        break;
    case 3:
        heroSetStatus(arg1, arg3, arg5);
        break;
    case 4:
        fn_80142B24(arg1, arg2, arg3, arg4, arg5);
        break;
    case 5:
        pokemonSetStatus(arg1, arg2, arg3, arg4, arg5);
        break;
    case 6:
        wazaSetStatus(arg1, arg2, arg3, arg4, arg5);
        break;
    case 7:
        result = fightFloorSetStatus(arg1, arg2, arg3, arg4, arg5);
        break;
    case 8:
        fightSideSetStatus(arg1, arg2, arg3, arg4, arg5);
        break;
    case 9:
        fightTrainerSetStatus(arg1, arg2, arg3, arg4, arg5);
        break;
    default:
        break;
    }
    return result;
}
#endif


/* 0x80135E44 | 0x114 */
#if 0
asm void fn_80135E44(void) {
#include "src/game/effect/effect_util_fn_80135E44.inc"
}
#else
u32 statusGetStatus(u32 kind, u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    switch ((u8)kind) {
    case 0:
        return 0;
    case 1:
        return gamedataGetStatus((void*)arg1, (u16)arg3);
    case 2:
        return pcboxGetStatus(arg1, arg3, arg4);
    case 3:
        return heroGetStatus(arg1, arg3, arg4);
    case 4:
        return itemGetStatus(arg1, arg2, arg3, arg4);
    case 5:
        return pokemonGetStatus(arg1, arg2, arg3, arg4);
    case 6:
        return wazaGetStatus(arg1, arg2, arg3, arg4);
    case 7:
        return fightFloorGetStatus(arg1, arg2, arg3, arg4);
    case 8:
        return fightSideGetStatus(arg1, arg2, arg3, arg4);
    case 9:
        return fightTrainerGetStatus(arg1, arg2, arg3, arg4);
    default:
        return 0;
    }
}
#endif
