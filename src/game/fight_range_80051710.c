/**
 * @file fight_range_80051710.c
 * @brief fight, 0x80051710 - 0x8005344C.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) — mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

void dbgMenuFightTrainerDataStatusInputDigit(u32 trainerId, u32 field, u32 index,
                                             s32 maximum, s32 minimum) {
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u8 fn_8001E224(u32, s32*, u32, u32, u32, u32);
    extern void fightTrainerSetStatus(u32, u32, u32, u32, s32);
    extern void menuSubCloseNumberInput(void);
    s32 value;
    u8 result;

    result = fn_8001E224(fightTrainerGetStatus(0, trainerId, field, index),
                         &value, 0, 0x32, 0x32, 0);
    if (result == 1) {
        if (value > maximum) {
            value = maximum;
        }
        if (value < minimum) {
            value = minimum;
        }
        fightTrainerSetStatus(0, trainerId, field, index, value);
    }
    menuSubCloseNumberInput();
}

typedef struct DebugMoveData {
    u32 words[14];
} DebugMoveData;

s32 dbgMenuFightWazaEditSub(u16 moveId)
{
    extern DebugMoveData* wazaDataBiosGetPtr(u16);
    extern s32 menuOpenCustom(s32, ...);
    extern s32 menuOpen(s32, s32);
    extern void menuCloseCustom(s32, s32, s32);
    extern s32 menuGetCursorItemID(s32);
    extern u32 wazaGetStatus(s32, u16, s32, s32);
    extern void wazaSetStatus(s32, u16, s32, s32, s32);
    extern s32 dbgMenuFightGetWazaTypeId(u8);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    DebugMoveData saved;
    DebugMoveData* move;
    s32 result;
    s32 item;
    s32 value;
    s32 field;

    if (moveId == 0 || moveId >= 0x163) {
        return 1;
    }

    move = wazaDataBiosGetPtr(moveId);
    saved = *move;
    for (;;) {
        result = menuOpenCustom(0x8E, 0, 0, 0, 1, 1, moveId);
        if (result == -1) {
            menuCloseCustom(0x8E, 0, 1);
            *move = saved;
            return -1;
        }
        if (result == -2) {
            result = menuOpen(0x44, 1);
            menuCloseCustom(0x44, 0, 1);
            if (result == 0) {
                break;
            }
            continue;
        }

        item = menuGetCursorItemID(0x8E);
        if (item >= 0x612 && item <= 0x614) {
            field = item - 0x612;
            value = dbgMenuFightGetWazaTypeId(
                (u8)wazaGetStatus(0, moveId, 0x1A, field));
            if (value >= 0) {
                wazaSetStatus(0, moveId, 0x1A, field, value);
            }
        } else if (item == 0x611) {
            result = menuSubOpenYesNo(
                0x7F, -1, -1, wazaGetStatus(0, moveId, 0x1B, 0) == 0);
            if (result == 0) {
                wazaSetStatus(0, moveId, 0x1B, 0, 1);
            } else if (result == 1) {
                wazaSetStatus(0, moveId, 0x1B, 0, 0);
            }
        }
    }

    menuCloseCustom(0x8E, 0, 1);
    return 1;
}
