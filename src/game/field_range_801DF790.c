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
    extern u8 fn_801ED218(s32);
    extern u16 pokemonGetStatus(void*, u16, s32, s32);
    extern u8 fn_801ED0CC(s32, void*);
    extern s32 fn_801E075C(s32);
    extern s32 fn_801ED294(s32);
    extern void* sodateyaGetPokemonPtr(s32);
    extern u8 pokemonBiosGetLevel(void*);
    extern u32 pokemonBiosGetDp(void*);
    extern u32 heroGetStatus(s32, s32, s32);
    extern void heroDecPokedoru(void*, u32);
    extern void winMsgOpenField(u32, s32, s32, s32);
    extern void fn_80183350(u32, u32);
    extern void fn_8018C69C(u32, u32, u32);
    extern void fn_8018B76C(u32, u32, u32, u32, u32);
    extern void fn_80183018(u32, u32);
    extern void fn_801ECFE0(s32, void*);
    extern void fn_801E09E0(s32);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void _threadSwitch(void);
    extern f32 lbl_8047E3F4;
    extern f32 lbl_8047E3F8;
    extern f64 lbl_8047E400;
    extern f64 lbl_8047E408;

    s32 running = 1;
    s32 state = 0;
    s32 selection = 0;
    s32 validCount = 0;
    s32 i;
    s32 price;
    f32 timer;

    do {
        switch (state) {
        case 0:
            if ((u8)fn_801ED218(0) != 0) {
                state = 9;
            } else {
                state = 1;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x3B11, 1, 0, 2);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 8;
            }
            break;
        case 2:
            validCount = 0;
            for (i = 0; i < 6; i++) {
                void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i);
                if ((u8)pokemonCheckValid(pokemon) != 0 &&
                    pokemonGetStatus(pokemon, 0, 0x7B, 0) == 0) {
                    validCount++;
                }
            }
            if (validCount == 1) {
                state = 3;
            } else {
                state = 5;
            }
            break;
        case 3:
            winMsgOpenFieldWithSE(0x3B13, 1, 0, 2);
            state = 21;
            break;
        case 4:
            winMsgOpenFieldWithSE(0x3B15, 1, 0, 2);
            state = 21;
            break;
        case 5:
            winMsgOpenFieldWithSE(0x3B17, 1, 0, 2);
            state = 6;
            break;
        case 6:
            selection = menuPokemonOpen(6, 0, 0);
            if (selection >= 0) {
                state = 7;
            } else {
                state = 8;
            }
            break;
        case 7: {
            void* party = savedataGetStatus(0, 2);
            void* pokemon = heroBiosGetPokemonPtr(party, (u16)selection);
            void* current;
            s32 lastValid = 0;

            fn_801E075C(selection);
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(pokemon));
            winMsgOpenFieldWithSE(0x3B19, 1, 0, 2);
            state = 21;

            for (i = 0; i < 6; i++) {
                current = heroBiosGetPokemonPtr(party, (u16)i);
                if ((u8)pokemonCheckValid(current) != 0) {
                    lastValid++;
                }
            }
            if ((u8)fn_801ED0CC(0, pokemon) != 0 && selection < 6) {
                for (i = selection; i < lastValid - 1; i++) {
                    void* dst = heroBiosGetPokemonPtr(party, (u16)i);
                    void* src = heroBiosGetPokemonPtr(party, (u16)(i + 1));
                    if ((u8)pokemonCheckValid(src) == 0) {
                        break;
                    }
                    memcpy((u8*)dst, (u8*)src, 0x138);
                }
                pokemonInit(heroBiosGetPokemonPtr(party, (u16)i));
            }
            break;
        }
        case 8:
            winMsgOpenFieldWithSE(0x3B1B, 1, 0, 2);
            state = 21;
            break;
        case 9:
            winMsgOpenFieldWithSE(0x3B1C, 1, 0, 2);
            state = 10;
            break;
        case 10: {
            s32 grown = fn_801ED294(0);
            s32 level = pokemonBiosGetLevel(sodateyaGetPokemonPtr(0));
            msgctrlSetValue(0x2F, (void*)(u32)(level - (grown & 0xFF)));
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            winMsgOpenFieldWithSE(0x3B1D, 1, 0, 2);
            state = 12;
            break;
        }
        case 11:
            winMsgOpenFieldWithSE(0x3B1E, 1, 0, 2);
            if ((s8)fn_8001E184() == 0) {
                state = 13;
            } else {
                state = 4;
            }
            break;
        case 12:
            validCount = 0;
            for (i = 0; i < 6; i++) {
                if ((u8)pokemonCheckValid(heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i)) != 0) {
                    validCount++;
                }
            }
            if (validCount == 6) {
                state = 14;
            } else {
                state = 15;
            }
            break;
        case 13:
            winMsgOpenFieldWithSE(0x3B12, 1, 0, 2);
            state = 4;
            break;
        case 14:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            if (fn_801ED24C(0) != 0) {
                s32 grownDp = fn_801ED24C(0) - pokemonBiosGetDp(sodateyaGetPokemonPtr(0));
                s32 hundreds = (grownDp / 100);
                price = (hundreds != 0) ? (hundreds * 0x64) + 0x64 : 0;
            } else {
                price = 0;
            }
            msgctrlSetValue(0x4B, (void*)(u32)(((s32)selection * 0x64) + price + 0x64));
            winMsgOpenFieldWithSE(0x3B14, 1, 0, 2);
            if ((s8)fn_8001E184() == 0) {
                state = 16;
            } else {
                state = 4;
            }
            break;
        case 15:
            if (fn_801ED24C(0) != 0) {
                s32 grownDp = fn_801ED24C(0) - pokemonBiosGetDp(sodateyaGetPokemonPtr(0));
                s32 hundreds = (grownDp / 100);
                price = (hundreds != 0) ? (hundreds * 0x64) + 0x64 : 0;
            } else {
                price = 0;
            }
            price += ((s32)selection * 0x64) + 0x64;
            if ((s32)heroGetStatus(0, 0xC, 0) >= price) {
                heroDecPokedoru(savedataGetStatus(0, 2), price);
                fn_80166AB8(0x3CB, 0, 0);
                timer = lbl_8047E3F4;
                while (timer < lbl_8047E3F8) {
                    f32 frame = (f32)fn_800D3088() / (f32)fn_800D37CC();
                    timer += frame;
                    _threadSwitch();
                }
                state = 17;
            } else {
                state = 18;
            }
            break;
        case 16:
            winMsgClose(1);
            fn_801E09E0(selection);
            state = 19;
            break;
        case 17:
            winMsgOpenFieldWithSE(0x3B16, 1, 0, 2);
            state = 4;
            break;
        case 18:
            winMsgOpenFieldWithSE(0x3B18, 1, 0, 2);
            state = 20;
            break;
        case 19: {
            void* party = savedataGetStatus(0, 2);
            void* firstOpen = NULL;

            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            winMsgOpenField(0x3B1A, 1, 0, 2);
            state = 8;

            for (i = 0; i < 6; i++) {
                void* pokemon = heroBiosGetPokemonPtr(party, (u16)i);
                if ((u8)pokemonCheckValid(pokemon) == 0) {
                    firstOpen = pokemon;
                    selection = i;
                    break;
                }
            }
            fn_801ECFE0(0, firstOpen);
            fn_80183350(0x4D, 1);
            fn_8018C69C(0x4D, 1, 8);
            fn_8018B76C(0x4D, 1, 5, 0, 1);
            fn_80183018(0x4D, 1);
            break;
        }
        case 20:
            winMsgClose(1);
            running = 0;
            break;
        case 21:
            winMsgClose(1);
            running = 0;
            break;
        }
    } while (running != 0);
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
    extern u8 pokemonBiosGetCatchBallId(void* pokemon);
    extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
    extern void* pokemonDataBiosGetPtr(u16 id);
    extern u16 pokemonDataBiosGetVoice(void* data);
    extern void* floorOpenObject(u32 resource);
    extern void GSvecCopy(f32* dst, const f32* src);
    extern void GSmodelSetPosition(void* model, f32* position);
    extern void GSmodelSetScale(void* model, f32* scale);
    extern void GSmodelSetVisibility(void* model, s32 visible);
    extern void GSmodelFree(void* model);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void _threadSwitch(void);
    extern const f32 lbl_8047E3F4;
    extern const f32 lbl_8047E414;
    extern const f32 lbl_803750C8[];
    u32 ballModels[13] = {
        0x03640400, 0x03650400, 0x03640400, 0x03360400,
        0x03360400, 0x036A0400, 0x03660400, 0x03400400,
        0x03670400, 0x03690400, 0x036C0400, 0x03630400,
        0x03680400
    };
    f32 position[3] = { 0.0f, 0.0f, 0.0f };
    f32 scale[3] = { 1.0f, 1.0f, 1.0f };
    void* model = NULL;
    s32 state = 0;
    s32 running = 1;

    do {
        switch (state) {
        case 0: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), partyIndex);
            u8 ball = pokemonBiosGetCatchBallId(pokemon);
            u16 species;

            model = floorOpenObject(ballModels[ball]);
            GSvecCopy(position, lbl_803750C8);
            pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), partyIndex);
            species = pokemonBiosGetPokemonDataId(pokemon);
            if (pokemonCheckValid(pokemon) != 0) {
                void* data = pokemonDataBiosGetPtr(species);
                if (data != NULL) {
                    fn_80166AB8(pokemonDataBiosGetVoice(data), 0, 0);
                }
            }
            GSmodelSetPosition(model, position);
            GSmodelSetScale(model, scale);
            state = 1;
            break;
        }
        case 1: {
            f32 timer = lbl_8047E3F4;
            while (timer < lbl_8047E414) {
                timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
                _threadSwitch();
            }
            state = 100;
            break;
        }
        case 100:
            running = 0;
            GSmodelSetVisibility(model, 0);
            GSmodelFree(model);
            model = NULL;
            break;
        }
    } while (running != 0);
}
