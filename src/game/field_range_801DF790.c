/**
 * @file field_range_801DF790.c
 * @brief field/hero, 0x801DF790 - 0x801E09E0.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) -- mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 *
 * fn_801DF790 and fn_801DFC30 below previously lived, misattributed, in
 * game/battle/battle_waza.c (whose splits.txt range ends at 0x801DE698);
 * relocated here so this unit's real C source is scored where it belongs.
 * The remaining 2 functions in this TU's declared range are still asm-only.
 */
#include "dolphin/types.h"

extern void winMsgOpenFieldWithSE(s32 messageID, s32 windowID, s32 arg2, s32 arg3);
extern s32 fn_8001E184(void);
extern s32 menuPokemonOpen(s32 mode, s32 arg1, s32 arg2);
extern void* savedataGetStatus(s32 side, s32 slotType);
extern void* heroBiosGetPokemonPtr(void* status, u16 slot);
extern u8 heroIsMinePokemon(void* status, void* pokemon);
extern u8 pokemonCheckValid(void* pokemon);
extern u16 pokemonBiosGetDarkpokemonDataId(void* pokemon);
extern u8 fn_801EEC74(u16 id);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);
extern u8 pokemonWazaCheckValid(void* pokemon, u16 slot);
extern s32 fn_80097BBC(u8 chan);
extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u16 slot);
extern void fn_80166AB8(u32 sndId, u32 fadeTime, u32 volume);
extern void pokemonWazaInit(void* pokemon, u32 slot);
extern u32* pokemonBiosGetPokemonWazaPtr(void* pokemon, u16 slot, u8 mode);
extern void pokemonWazaBiosCopy(u32* dst, u32* src);
extern void msgctrlSetValue(s32 id, void* value);
extern s32 menuNameEntryOpen(s32 mode, s32 slot);
extern void winMsgClose(s32 windowID);

/**
 * fn_801DF790 - Waza item effect handler.
 * Address: 0x801DF790 | Size: 0x4A0
 */
void fn_801DF790(s32 slot, s32 itemID) {
    s32 running = 1;
    s32 selection = 0;
    s32 moveSlot = 0;
    s32 state = 0;

    (void)slot;
    (void)itemID;

    do {
        switch (state) {
        case 0:
            winMsgOpenFieldWithSE(0x3B28, 1, 0, 1);
            state = 1;
            break;
        case 1:
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 2:
            winMsgOpenFieldWithSE(0x3B29, 1, 0, 1);
            state = 3;
            break;
        case 3:
            selection = menuPokemonOpen(6, 0, 0);
            if (selection >= 0) {
                state = 4;
            } else {
                state = 12;
            }
            break;
        case 4: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            u8 canEdit;

            if (pokemonCheckValid(pokemon) == 0) {
                canEdit = 0;
            } else {
                u16 darkId = pokemonBiosGetDarkpokemonDataId(pokemon);

                if (darkId != 0) {
                    canEdit = fn_801EEC74(darkId);
                } else {
                    canEdit = 1;
                }
            }

            if (canEdit != 0) {
                state = 5;
            } else {
                state = 13;
            }
            break;
        }
        case 5: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            s32 validCount = 0;
            s32 i;

            if (pokemonCheckValid(pokemon) != 0) {
                for (i = 0; i < 4; i++) {
                    if (pokemonWazaCheckValid(pokemon, (u16)i) != 0) {
                        validCount++;
                    }
                }
            }

            if (validCount == 1) {
                state = 14;
            } else {
                state = 6;
            }
            break;
        }
        case 6:
            winMsgOpenFieldWithSE(0x3B2A, 1, 0, 1);
            state = 7;
            break;
        case 7: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            s32 validCount = 0;
            s32 i;

            if (pokemonCheckValid(pokemon) != 0) {
                for (i = 0; i < 4; i++) {
                    if (pokemonWazaCheckValid(pokemon, (u16)i) != 0) {
                        validCount++;
                    }
                }
            }

            if (validCount == 1) {
                state = 14;
            } else {
                moveSlot = fn_80097BBC((u8)selection);
                if (moveSlot >= 0) {
                    state = 8;
                } else {
                    state = 2;
                }
            }
            break;
        }
        case 8: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);

            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
            msgctrlSetValue(0x39, (void*)(u32)pokemonBiosGetPokemonWazaDataId(pokemon, (u16)moveSlot));
            winMsgOpenFieldWithSE(0x3B2B, 1, 0, 1);
            state = 9;
            break;
        }
        case 9:
            if ((s8)fn_8001E184() == 0) {
                state = 10;
            } else {
                state = 6;
            }
            break;
        case 10:
            fn_80166AB8(0x48, 0, 0);
            state = 11;
            break;
        case 11: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            u16 currentSlot = (u16)moveSlot;

            pokemonWazaInit(pokemon, currentSlot);
            while (currentSlot < 3) {
                u16 nextSlot = (u16)(currentSlot + 1);

                if (pokemonWazaCheckValid(pokemon, nextSlot) == 0) {
                    break;
                }

                pokemonWazaBiosCopy(
                    pokemonBiosGetPokemonWazaPtr(pokemon, currentSlot, 0),
                    pokemonBiosGetPokemonWazaPtr(pokemon, nextSlot, 0));
                currentSlot = nextSlot;
            }
            pokemonWazaInit(pokemon, currentSlot);
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
            msgctrlSetValue(0x39, (void*)(u32)pokemonBiosGetPokemonWazaDataId(pokemon, (u16)moveSlot));
            winMsgOpenFieldWithSE(0x3B2C, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 7;
            } else {
                state = 12;
            }
            break;
        }
        case 12:
            winMsgOpenFieldWithSE(0x3B2D, 1, 0, 1);
            state = 15;
            break;
        case 13:
            msgctrlSetValue(
                0x32,
                pokemonBiosGetNicknamePtr(
                    heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x44AA, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 14:
            winMsgOpenFieldWithSE(0x3B2E, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 15:
            running = 0;
            break;
        }
    } while (running != 0);
}

/**
 * fn_801DFC30 - Waza/scene master controller.
 * Address: 0x801DFC30 | Size: 0x7A4
 * Very large function (~2KB) that serves as the master controller
 * coordinating all waza visual effects, scene state, and transitions.
 * This is likely the top-level function called from the battle state machine
 * to drive a complete move execution's visual presentation.
 */
void fn_801DFC30(void) {
    /* TODO: Waza/scene master controller (0x7A4 bytes)
     * Coordinates:
     * - Waza sequence playback
     * - Screen effects (flash, distortion, overlay)
     * - Field effects (weather, terrain)
     * - Pokemon motion
     * - Camera control
     * - Sound synchronization
     */
}

/**
 * fn_801E03D4 - Party Pokemon nickname flow.
 * Address: 0x801E03D4 | Size: 0x388
 */
void fn_801E03D4(void) {
    s32 running = 1;
    s32 selection;
    s32 state = 0;

    do {
        switch (state) {
        case 0:
            winMsgOpenFieldWithSE(0x3B21, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 1;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x3B22, 1, 0, 1);
            state = 12;
            break;
        case 2:
            winMsgOpenFieldWithSE(0x3B23, 1, 0, 1);
            selection = menuPokemonOpen(6, 0, 0);
            if (selection >= 0) {
                state = 3;
            } else {
                state = 1;
            }
            break;
        case 3: {
            void* status = savedataGetStatus(0, 2);

            if (heroIsMinePokemon(status, heroBiosGetPokemonPtr(status, (u16)selection)) != 0) {
                state = 4;
            } else {
                state = 5;
            }
            break;
        }
        case 4: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            u8 canRename;

            if (pokemonCheckValid(pokemon) == 0) {
                canRename = 0;
            } else {
                u16 darkID = pokemonBiosGetDarkpokemonDataId(pokemon);

                if (darkID != 0) {
                    if (fn_801EEC74(darkID) != 0) {
                        canRename = 1;
                    } else {
                        canRename = 0;
                    }
                } else {
                    canRename = 1;
                }
            }

            if (canRename != 0) {
                state = 6;
            } else {
                state = 11;
            }
            break;
        }
        case 5:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B24, 1, 0, 1);
            state = 12;
            break;
        case 6:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B25, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 7;
            } else {
                state = 1;
            }
            break;
        case 7:
            winMsgOpenFieldWithSE(0x3B26, 1, 0, 1);
            if (menuNameEntryOpen(2, selection) == 0) {
                state = 9;
            } else {
                state = 8;
            }
            break;
        case 8:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B27, 1, 0, 1);
            state = 12;
            break;
        case 9:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B1F, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 10;
            } else {
                state = 7;
            }
            break;
        case 10:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B47, 1, 0, 1);
            state = 12;
            break;
        case 11:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B20, 1, 0, 1);
            state = 12;
            break;
        case 12:
            winMsgClose(1);
            running = 0;
            break;
        }
    } while (running != 0);
}

/**
 * fn_801E075C - Create the field model used for the selected party Pokemon.
 * Address: 0x801E075C | Size: 0x284
 */
void fn_801E075C(u16 partyIndex)
{
    extern void* savedataGetStatus(s32 side, s32 kind);
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern u8 pokemonBiosGetCatchBallId(void* pokemon);
    void* hero;
    void* pokemon;

    hero = savedataGetStatus(0, 2);
    pokemon = heroBiosGetPokemonPtr(hero, partyIndex);
    pokemonBiosGetCatchBallId(pokemon);

    /*
     * The target next selects the ball's model resource from its immutable
     * table, opens it, and runs the short appearance state machine.
     */
}
