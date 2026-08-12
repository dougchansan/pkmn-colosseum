/**
 * @file fight_range_80051710.c
 * @brief fight, 0x80051710 - 0x8005344C.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) — mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

typedef struct DebugTrainerPartData {
    u32 words[5];
} DebugTrainerPartData;

s32 fn_80051710(u32 trainerId)
{
    extern DebugTrainerPartData* fightTrainerPokemonPartDataBiosGetPtr(u16);
    extern s32 menuOpenCustom(s32, ...);
    extern void menuCloseCustom(s32, s32, s32);
    extern s32 menuOpen(s32, s32);
    extern s32 menuGetCursorItemID(s32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u8 fn_8001E224(u32, s32*, u32, u32, u32, u32);
    extern void fightTrainerSetStatus(u32, u32, u32, u32, s32);
    extern void menuSubCloseNumberInput(void);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    DebugTrainerPartData saved;
    DebugTrainerPartData* data;
    u16 id;
    s32 result;
    s32 item;

    id = trainerId;
    if (id == 0) {
        return 1;
    }

    data = fightTrainerPokemonPartDataBiosGetPtr(id);
    saved = *data;
    for (;;) {
        result = menuOpenCustom(0x8D, 0, 0, 0, 1, 1, id);
        if (result == -1) {
            menuCloseCustom(0x8D, 0, 1);
            *data = saved;
            return -1;
        }
        if (result == -2) {
            if (menuOpen(0x44, 1) == 0) {
                menuCloseCustom(0x44, 0, 1);
                break;
            }
            menuCloseCustom(0x44, 0, 1);
            continue;
        }

        item = menuGetCursorItemID(0x8D);
        switch (item) {
        case 0x5FE: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 1), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 1, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x5FF: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 2), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 2, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x600: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 3), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 3, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x601: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 5), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 5, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x602: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 6), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 6, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x603: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 7), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 7, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x604: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 8), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 8, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x605: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 9), &value,
                            0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 9, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x606: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 0xA),
                            &value, 0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 0xA, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0x607: {
            s32 value;

            if (fn_8001E224(fightTrainerGetStatus(0, trainerId, 0xC, 0xB),
                            &value, 0, 0x32, 0x32, 0) == 1)
            {
                if (value > 0xFF) {
                    value = 0xFF;
                }
                if (value < 0) {
                    value = 0;
                }
                fightTrainerSetStatus(0, trainerId, 0xC, 0xB, value);
            }
            menuSubCloseNumberInput();
            continue;
        }
        case 0xFED:
            result = menuSubOpenYesNo(
                0x7F, -1, -1,
                fightTrainerGetStatus(0, trainerId, 0xC, 4) == 0);
            if (result == 0) {
                fightTrainerSetStatus(0, trainerId, 0xC, 4, 1);
            } else if (result == 1) {
                fightTrainerSetStatus(0, trainerId, 0xC, 4, 0);
            }
            continue;
        }
    }

    menuCloseCustom(0x8D, 0, 1);
    return 1;
}

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

typedef struct DebugTrainerData {
    u32 words[10];
} DebugTrainerData;

static void dbgMenuFightTrainerToggleStatus(u16 trainerId, u32 field)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightTrainerSetStatus(u32, u32, u32, u32, s32);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    s32 result;

    result = menuSubOpenYesNo(0x7F, -1, -1,
                              fightTrainerGetStatus(0, trainerId, field, 0) == 0);
    if (result == 0) {
        fightTrainerSetStatus(0, trainerId, field, 0, 1);
    } else if (result == 1) {
        fightTrainerSetStatus(0, trainerId, field, 0, 0);
    }
}

static void dbgMenuFightTrainerSelectStatus08518(u16 trainerId, u32 field,
                                                 u32 index)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightTrainerSetStatus(u32, u32, u32, u32, s32);
    extern s32 fn_80008518(void);
    s32 value;

    fightTrainerGetStatus(0, trainerId, field, index);
    value = fn_80008518();
    if (value >= 0) {
        fightTrainerSetStatus(0, trainerId, field, index, value);
    }
}

static void dbgMenuFightTrainerSelectStatus08460(u16 trainerId, u32 field,
                                                 u32 index)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightTrainerSetStatus(u32, u32, u32, u32, s32);
    extern s32 fn_80008460(void);
    s32 value;

    fightTrainerGetStatus(0, trainerId, field, index);
    value = fn_80008460();
    if (value >= 0) {
        fightTrainerSetStatus(0, trainerId, field, index, value);
    }
}

void fn_80051E38(u16 trainerId)
{
    extern DebugTrainerData* fn_801FC658(void);
    extern s32 fn_801026A4(s32, s32, s32, s32, s32, s32, u32);
    extern void fn_80102568(s32, s32, s32);
    extern s32 fn_8010264C(s32, s32);
    extern s32 fn_801022B8(s32);
    DebugTrainerData saved;
    DebugTrainerData* trainer;
    s32 result;
    s32 item;

    if (trainerId == 0) {
        return;
    }

    trainer = fn_801FC658();
    saved = *trainer;
    for (;;) {
        result = fn_801026A4(0x86, 0, 0, 0, 1, 1, trainerId);
        if (result == -1) {
            fn_80102568(0x86, 0, 1);
            *trainer = saved;
            return;
        }
        if (result == -2) {
            result = fn_8010264C(0x44, 1);
            fn_80102568(0x44, 0, 1);
            if (result != 0) {
                continue;
            }
            fn_80102568(0x86, 0, 1);
            return;
        }

        item = fn_801022B8(0x86);
        switch (item) {
        case 0x5A0:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x25, 0, 0x64, 0);
            break;
        case 0x5A1:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x26, 0, 0x64, 0);
            break;
        case 0x5A2:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x27, 0, 0x64, 0);
            break;
        case 0x5A3:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x2C, 0, 0x64, 0);
            break;
        case 0x5A4:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x37, 0, 0x64, 0);
            break;
        case 0x5A5:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x1F);
            break;
        case 0x5A6:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x20);
            break;
        case 0x5A7:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x23);
            break;
        case 0x5A8:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x24);
            break;
        case 0x5A9:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x28);
            break;
        case 0x5AA:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x29);
            break;
        case 0x5AB:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x2A);
            break;
        case 0x5AC:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x2B);
            break;
        case 0x5AD:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x2D);
            break;
        case 0x5AE:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x30);
            break;
        case 0x5AF:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x31);
            break;
        case 0x5B0:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x32);
            break;
        case 0x5B1:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x33);
            break;
        case 0x5B2:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x34);
            break;
        case 0x5B3:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x36);
            break;
        case 0x5B4:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x35, 0, 0x64, 0);
            break;
        case 0x5B5:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x2E, 0, 0x54, 0);
            break;
        case 0x5B6:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x2F, 0, 0x54, 0);
            break;
        case 0x608:
            dbgMenuFightTrainerSelectStatus08518(trainerId, 0x39, 0);
            break;
        case 0x609:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x3A, 0, 0xFF, 0);
            break;
        case 0x60A:
            dbgMenuFightTrainerSelectStatus08518(trainerId, 0x39, 1);
            break;
        case 0x60B:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x3A, 1, 0xFF, 0);
            break;
        case 0x60C:
            dbgMenuFightTrainerSelectStatus08460(trainerId, 0x3B, 0);
            break;
        case 0x60D:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x3C, 0, 0xFF, 0);
            break;
        case 0x60E:
            dbgMenuFightTrainerSelectStatus08460(trainerId, 0x3B, 1);
            break;
        case 0x60F:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x3C, 1, 0xFF, 0);
            break;
        case 0xFC7:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x21);
            break;
        case 0xFC8:
            dbgMenuFightTrainerToggleStatus(trainerId, 0x22);
            break;
        case 0x1197:
            dbgMenuFightTrainerDataStatusInputDigit(trainerId, 0x38, 1, 0xFF, 0);
            break;
        }
    }
}

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
