/**
 * @file etctool.c
 * @brief etctool, 0x801DF474 - 0x801E0FB4, with its data: .data
 * 0x803750C8-0x803751EC (the sequence-position object and four switch
 * tables) and the .sdata2 pool 0x8047E3F0-0x8047E428.
 *
 * XD's etctool unit ends with etctoolSetPokemonNakigoe, as this one does at
 * 0x801E0F78; the vtr unit starts at fn_801E0FB4 (see gs_exact_801E0FB4.c).
 * fn_801DF474 shares the unit's pool (2.0f at 0x8047E3F0).
 */
#include "dolphin/types.h"

/* One file-level prototype each for the callees all parts of the TU share
 * (peopleMoveCheck as in game/people/people.h). */
extern void peopleStartTurn(s32 groupId, s32 index, f32 yaw, f32 speed);
extern BOOL peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag);
extern void msgctrlSetValue(s32 id, void* value);
extern void* sodateyaGetPokemonPtr(s32 slot);
void fn_801E075C(s32 partyIndex);
void fn_801E09E0(s32 unused);
extern u32 GSgetFrameStep(void);
extern s32 fn_800D37CC(void);
extern void _threadSwitch(void);
extern void GSvecCopy(void*, const void*);
extern void* GSmodelGetPart(void*, s32);
extern void GSpartGetTransform(void*, void*, s32, s32);
extern void GSpartFree(void*);
extern void GSmodelSetPosition(void*, const void*);
extern void GSmodelSetScale(void*, const void*);
extern void GSmodelSetVisibility(void*, s32);
extern void GSmodelFree(void*);
extern void* fn_800F92D4(u32);
extern void* std092_getCurrentRoomID(void);
extern void floorEventCtrlDoor(void*, u32, u32);
extern void* floorOpenObject(u32);
extern u8 pokemonBiosGetCatchBallId(void*);
extern u16 pokemonBiosGetPokemonDataId(void*);
extern void* pokemonDataBiosGetPtr(u16);
extern u16 pokemonDataBiosGetVoice(void*);
extern void soundStartRaw(u32, u32, u32);
extern void std077_characterEventFlagsSet(u32, u32);
extern void std076_characterEventFlagsClear(u32, u32);
extern void std061_characterMoveToPosition(u32, u32, u32, f32, f32, f32);
extern void std172_characterGetPosition(u32, u32, void*);

typedef struct EtcToolVec {
    f32 x;
    f32 y;
    f32 z;
} EtcToolVec;

/* 0x803750C8: the etctool file statics. MWCC pools them into one .data
 * object and addresses each off its base. */
static EtcToolVec etcObjectPosition = { -20.0f, 10.0f, 8.5f };
static EtcToolVec etcDoorPosition = { -15.0f, 0.0f, -12.1f };
static EtcToolVec etcOutsidePosition = { -15.0f, 0.0f, -33.1f };
static u16 etcIndices[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

typedef struct AbilityItemWeight {
    u16 item;
    u16 weight;
} AbilityItemWeight;

typedef struct AbilityVec {
    f32 x;
    f32 y;
    f32 z;
} AbilityVec;

extern u32* lbl_80478EB0;
extern AbilityItemWeight* lbl_80478EB4;

extern u16 GSrand(void);
extern void* peopleIsInGroup(s32, s32);
extern void* peopleSearchID(void*);
extern void* peopleGetPosition(void*);
extern void heroMoveGetHeroPos(void*);
extern void* peopleGetPositionPtr(void*);
extern void GSvecSub(void*, void*, void*);
extern f64 atan2(f64, f64);
extern u32 std082_flagGetValue(u32);
extern void _flagSet(u32, s32);
extern u8 std081_flagCheckTrue(u32);
extern void winMsgOpenFieldWithSE(s32, s32, s32, s32);
extern void std130_soundUnk_801653cc(s32, s32, s32);

extern void winMsgOpen(s32, s32, s32, s32);
extern s32 heroItemAddItemDataId(void*, u32, u32, s32);
extern void pcboxDelItem(s32, u32, u32);

void std087_sequenceUnk87(s32 slot, s32 abilityID) {
    AbilityItemWeight* weights;
    AbilityVec direction;
    AbilityVec heroPosition;
    u32 state;
    s32 running;
    u32 totalWeight;
    u32 i;
    s32 selectedItem;
    u32 selectedData;
    u32 cumulative;
    u32 randomValue;
    u32 count;
    s32 result;
    void* person;

    state = 0;
    running = 1;
    totalWeight = 0;
    count = *lbl_80478EB0;
    weights = lbl_80478EB4;
    for (i = 0; i < count; i++) {
        totalWeight += (weights++)->weight;
    }

    randomValue = (u16)GSrand() % totalWeight;
    cumulative = 0;
    {
        AbilityItemWeight* entry = lbl_80478EB4;

        for (i = 0; i < count; i++, entry++) {
            cumulative += entry->weight;
            if (randomValue < cumulative) {
                selectedItem = entry->item;
                selectedData = entry->item;
                break;
            }
        }
    }

    do {
        switch (state) {
        case 0:
            person = peopleSearchID(peopleIsInGroup(slot, abilityID));
            if (person != NULL) {
                peopleGetPosition(person);
                heroMoveGetHeroPos(&heroPosition);
                GSvecSub(&direction, &heroPosition,
                            peopleGetPositionPtr(person));
                peopleStartTurn(slot, abilityID,
                            (f32)atan2(direction.x, direction.z),
                            2.0f);
                peopleMoveCheck(slot, abilityID, 1);
            }

            if (std082_flagGetValue(0xD0) != 0) {
                if (std082_flagGetValue(0xD1) >= 500) {
                    _flagSet(0xD1, 0);
                    result = 1;
                } else if (std081_flagCheckTrue(0xAFE) != 0) {
                    _flagSet(0xAFE, 0);
                    result = -1;
                } else {
                    result = 0;
                }
            } else {
                _flagSet(0xD0, 1);
                result = 1;
            }

            if (result > 0) {
                state = 1;
            } else if (result < 0) {
                state = 3;
            } else {
                state = 4;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x5571, 1, 0, 1);
            state = 2;
            break;
        case 2:
            std130_soundUnk_801653cc(0x3CA, 0, 0xFF);
            msgctrlSetValue(0x2D, (void*)(u32)selectedItem);
            winMsgOpen(3, 0x3CB8, 1, 0);
            result = heroItemAddItemDataId(NULL, selectedData, 1, -1);
            if (result != 0 && result > 0) {
                /* Masks rather than casts keep MWCC's preheader order retail. */
                pcboxDelItem(0, selectedData & 0xFFFF, result & 0xFFFF);
            }
            state = 7;
            break;
        case 3:
            winMsgOpenFieldWithSE(0x5573, 1, 0, 1);
            state = 7;
            break;
        case 4:
            winMsgOpenFieldWithSE(0x5574, 1, 0, 1);
            state = 7;
            break;
        case 7:
            running = 0;
            break;
        case 5:
        case 6:
        default:
            break;
        }
    } while (running != 0);
}

extern void winMsgOpenFieldWithSE(s32 messageID, s32 windowID, s32 arg2, s32 arg3);
extern s32 std011_dialogDisplayYesNoMenu(void);
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
extern void soundStartRaw(u32 sndId, u32 fadeTime, u32 volume);
extern void pokemonWazaInit(void* pokemon, u32 slot);
extern u32* pokemonBiosGetPokemonWazaPtr(void* pokemon, u16 slot, u8 mode);
extern void pokemonWazaBiosCopy(u32* dst, u32* src);
extern s32 menuNameEntryOpen(s32 mode, s32 slot);
extern void winMsgClose(s32 windowID);

static inline u8 fieldWazaCanForget(u16 selection)
{
    void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), selection);
    u16 darkId;

    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    darkId = pokemonBiosGetDarkpokemonDataId(pokemon);
    if (darkId != 0) {
        if (fn_801EEC74(darkId) != 0) {
            return 1;
        }
        return 0;
    }
    return 1;
}

/* The invalid-Pokemon path assigns 0 and falls through rather than returning
 * early: this is the only form that keeps retail's register order in
 * fn_801DF790 (the early return adds a result temp that reorders its
 * callee-saved locals). */
static inline s32 fieldWazaCountValid(u16 selection)
{
    void* pokemon;
    s32 i;
    s32 count;

    count = 0;
    pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), selection);

    if (pokemonCheckValid(pokemon) == 0) {
        count = 0;
    } else {
        for (i = 0; i < 4; i++) {
            if (pokemonWazaCheckValid(pokemon, (u16)i) != 0) {
                count++;
            }
        }
    }
    return count;
}

/* RULE-EXCEPTION(title-path): single-use inline helper whose only evidence
 * is register allocation - see docs/RULE_EXCEPTIONS.md. Retail routes the move
 * id through r4 before copying it to wazaId, which only an inline return does. */
static inline u32 fieldWazaGetId(u16 selection, u16 slot)
{
    return pokemonBiosGetPokemonWazaDataId(
        heroBiosGetPokemonPtr(savedataGetStatus(0, 2), selection), slot);
}

/* pokemonWazaCopy and pokemonWazaForget follow the XD sister title
 * (TeamOrre/xd-decomp symbols.txt: pokemonWazaForget 0x8013E9A0,
 * pokemonWazaCopy 0x8013EA24 and the local _pokemonWazaCopy__FP7PokemonUsUs;
 * trevor403/xd-asm FUN_8013e9a0/FUN_8013ea24 have the same loop and calls).
 * Here both are expanded inline in fn_801DF790's state 11. */
static inline void pokemonWazaCopy(void* pokemon, u16 dstSlot, u16 srcSlot)
{
    if (pokemon != NULL) {
        u32* dst = pokemonBiosGetPokemonWazaPtr(pokemon, dstSlot, 0);
        pokemonWazaBiosCopy(dst, pokemonBiosGetPokemonWazaPtr(pokemon, srcSlot, 0));
    }
}

static inline void pokemonWazaForget(void* pokemon, u16 slot)
{
    u16 i;
    pokemonWazaInit(pokemon, slot);
    for (i = slot; i < 3; i++) {
        if (pokemonWazaCheckValid(pokemon, i + 1) == 0) {
            break;
        }
        pokemonWazaCopy(pokemon, i, i + 1);
    }
    pokemonWazaInit(pokemon, i);
}

/**
 * fn_801DF790 - Waza item effect handler.
 * Address: 0x801DF790 | Size: 0x4A0
 */
void std086_sequenceJump86(s32 slot, s32 itemID) {
    s32 selection;
    s32 moveSlot;
    u32 wazaId;
    s32 running;
    s32 state;

    running = 1;
    moveSlot = 0;
    wazaId = 0;
    state = 0;

    (void)slot;
    (void)itemID;

    do {
        switch (state) {
        case 0:
            winMsgOpenFieldWithSE(0x3B28, 1, 0, 1);
            state = 1;
            break;
        case 1:
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
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
        case 4:
            if (fieldWazaCanForget((u16)selection) != 0) {
                state = 5;
            } else {
                state = 13;
            }
            break;
        case 5:
            if (fieldWazaCountValid((u16)selection) == 1) {
                state = 14;
            } else {
                state = 6;
            }
            break;
        case 6:
            winMsgOpenFieldWithSE(0x3B2A, 1, 0, 1);
            state = 7;
            break;
        case 7:
            if (fieldWazaCountValid((u16)selection) == 1) {
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
        case 8:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            wazaId = fieldWazaGetId((u16)selection, (u16)moveSlot);
            msgctrlSetValue(0x39, (void*)(u32)wazaId);
            winMsgOpenFieldWithSE(0x3B2B, 1, 0, 1);
            state = 9;
            break;
        case 9:
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
                state = 10;
            } else {
                state = 6;
            }
            break;
        case 10:
            soundStartRaw(0x48, 0, 0);
            state = 11;
            break;
        case 11:
            pokemonWazaForget(heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection), (u16)moveSlot);
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            msgctrlSetValue(0x39, (void*)(u32)wazaId);
            winMsgOpenFieldWithSE(0x3B2C, 1, 0, 1);
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
                state = 7;
            } else {
                state = 12;
            }
            break;
        case 12:
            winMsgOpenFieldWithSE(0x3B2D, 1, 0, 1);
            state = 15;
            break;
        case 13:
            winMsgOpenFieldWithSE(0x3B2E, 1, 0, 1);
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 14:
            msgctrlSetValue(
                0x32,
                pokemonBiosGetNicknamePtr(
                    heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x44AA, 1, 0, 1);
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
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

typedef struct FieldPokemonData {
    u8 data[0x138];
} FieldPokemonData;

static inline u16 fieldCountValidPokemon(void)
{
    u16 count;
    void* party;
    u16 i;

    count = 0;
    party = savedataGetStatus(0, 2);
    for (i = 0; i < 6; i++) {
        if (pokemonCheckValid(heroBiosGetPokemonPtr(party, i)) != 0) {
            count++;
        }
    }
    return count;
}

extern u32 pokemonBiosGetDp(void*);
extern u32 fn_801ED24C(s32);

extern u8 pokemonBiosGetLevel(void*);
extern s32 fn_801ED294(s32);

static inline s32 sodateyaGetPokemonLevelUpValue(void)
{
    s32 storedLevel = fn_801ED294(0);
    u8 level = pokemonBiosGetLevel(sodateyaGetPokemonPtr(0));

    return (s32)(level - (u8)storedLevel);
}

static inline s32 sodateyaGetPokemonDPValue(void)
{
    void* pokemon = sodateyaGetPokemonPtr(0);

    return (s32)(fn_801ED24C(0) - pokemonBiosGetDp(sodateyaGetPokemonPtr(0))) / 100;
}

static inline s32 sodateyaCalcPrice(s32 levels)
{
    s32 price;
    s32 value;

    if (fn_801ED24C(0) != 0) {
        value = sodateyaGetPokemonDPValue();
        if (value != 0) {
            price = value * 100 + 100;
        } else {
            price = 0;
        }
    } else {
        price = 0;
    }
    return 100 + levels * 100 + price;
}

extern u8 pokemonGetStatus(void*, u16, s32, s32);

/* RULE-EXCEPTION(title-path): single-use inline helper whose only evidence
 * is register allocation - see docs/RULE_EXCEPTIONS.md. */
static inline s32 fieldCountAvailablePokemon(void)
{
    s32 i;
    void* pokemon;
    s32 count;

    count = 0;
    for (i = 0; i < 6; i++) {
        pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i);
        if ((u8)pokemonCheckValid(pokemon) != 0 &&
            pokemonGetStatus(pokemon, 0, 0x7B, 0) == 0) {
            count++;
        }
    }
    return count;
}

/* RULE-EXCEPTION(title-path): single-use inline helper whose only evidence
 * is register allocation - see docs/RULE_EXCEPTIONS.md. */
static inline s32 fieldCountPartyPokemon(void)
{
    s32 i;
    s32 count;

    count = 0;
    for (i = 0; i < 6; i++) {
        if ((u8)pokemonCheckValid(heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i)) != 0) {
            count++;
        }
    }
    return count;
}

/* RULE-EXCEPTION(title-path): single-use inline helper whose only evidence
 * is register allocation - see docs/RULE_EXCEPTIONS.md. */
static inline s32 fieldFindEmptySlot(void)
{
    s32 i;

    for (i = 0; i < 6; i++) {
        void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i);

        if ((u8)pokemonCheckValid(pokemon) == 0) {
            return i;
        }
        pokemonBiosGetDp(pokemon);
    }
    return -1;
}

/**
 * fn_801DFC30 - Waza/scene master controller.
 * Address: 0x801DFC30 | Size: 0x7A4
 * Very large function (~2KB) that serves as the master controller
 * coordinating all waza visual effects, scene state, and transitions.
 * This is likely the top-level function called from the battle state machine
 * to drive a complete move execution's visual presentation.
 */
void std085_sequenceJump85(void) {
    extern u8 fn_801ED218(s32);
    extern u8 fn_801ED0CC(s32, void*);
    extern u32 heroGetStatus(s32, s32, s32);
    extern void heroDecPokedoru(void*, u32);
    extern void winMsgOpenField(u32, s32, s32);
    extern void std076_characterEventFlagsClear(u32, u32);
    extern void peopleClearFlagsByIndex(u32, u32, u32);
    extern void std051_characterSetAnimation(u32, u32, u32, u32, u32);
    extern void std077_characterEventFlagsSet(u32, u32);
    extern void fn_801ECFE0(s32, void*);
    extern s32 fn_800D37CC(void);
    extern u32 GSgetFrameStep(void);
    extern void _threadSwitch(void);

    s32 i;
    s32 selection;
    s32 levels;
    s32 running;
    s32 lastValid;
    void* dst;
    void* party;
    void* pokemon;
    s32 state;
    s32 validCount;
    s32 price;
    f32 timer;

    state = 0;
    running = 1;

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
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
                state = 2;
            } else {
                state = 8;
            }
            break;
        case 2:
            if (fieldCountAvailablePokemon() == 1) {
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

            fn_801E075C(selection);
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B19, 1, 0, 2);
            state = 21;

            party = savedataGetStatus(0, 2);
            pokemon = heroBiosGetPokemonPtr(party, (u16)selection);
            lastValid = fieldCountValidPokemon();
            if ((u8)fn_801ED0CC(0, pokemon) != 0 && selection < 6) {
                for (i = selection; i < lastValid - 1; i++) {
                    dst = heroBiosGetPokemonPtr(party, (u16)i);
                    pokemon = heroBiosGetPokemonPtr(party, (u16)(i + 1));
                    if ((u8)pokemonCheckValid(pokemon) == 0) {
                        break;
                    }
                    *(FieldPokemonData*)dst = *(FieldPokemonData*)pokemon;
                }
                pokemonInit(pokemon);
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
        case 10:
            levels = sodateyaGetPokemonLevelUpValue();
            if (levels) {
                state = 11;
            } else {
                state = 12;
            }
            break;
        case 11: {
            msgctrlSetValue(0x2F, (void*)sodateyaGetPokemonLevelUpValue());
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            winMsgOpenFieldWithSE(0x3B1D, 1, 0, 2);
            state = 12;
            break;
        }
        case 12:
            winMsgOpenFieldWithSE(0x3B1E, 1, 0, 2);
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
                state = 13;
            } else {
                state = 4;
            }
            break;
        case 13:
            if (fieldCountPartyPokemon() == 6) {
                state = 14;
            } else {
                state = 15;
            }
            break;
        case 14:
            winMsgOpenFieldWithSE(0x3B12, 1, 0, 2);
            state = 4;
            break;
        case 15:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            msgctrlSetValue(0x4B, (void*)sodateyaCalcPrice(levels));
            winMsgOpenFieldWithSE(0x3B14, 1, 0, 2);
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
                state = 16;
            } else {
                state = 4;
            }
            break;
        case 16:
            price = sodateyaCalcPrice(levels);
            if ((s32)heroGetStatus(0, 0xC, 0) >= price) {
                heroDecPokedoru(savedataGetStatus(0, 2), price);
                soundStartRaw(0x3CB, 0, 0);
                timer = 0.0f;
                while (timer < 0.7f) {
                    timer += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
                    _threadSwitch();
                }
                state = 17;
            } else {
                state = 18;
            }
            break;
        case 17:
            winMsgClose(1);
            fn_801E09E0(selection);
            state = 19;
            break;
        case 18:
            winMsgOpenFieldWithSE(0x3B16, 1, 0, 2);
            state = 4;
            break;
        case 19:
            winMsgOpenFieldWithSE(0x3B18, 1, 0, 2);
            state = 20;
            break;
        case 20:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            winMsgOpenField(0x3B1A, 1, 0);
            state = 8;
            fn_801ECFE0(0, heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)fieldFindEmptySlot()));
            std076_characterEventFlagsClear(0x4D, 1);
            peopleClearFlagsByIndex(0x4D, 1, 8);
            std051_characterSetAnimation(0x4D, 1, 5, 0, 1);
            std077_characterEventFlagsSet(0x4D, 1);
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
void std084_sequenceJump84(void) {
    s32 running = 1;
    s32 selection;
    s32 state = 0;

    do {
        switch (state) {
        case 0:
            winMsgOpenFieldWithSE(0x3B21, 1, 0, 1);
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
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
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
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
            if ((s8)std011_dialogDisplayYesNoMenu() == 0) {
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


typedef struct EtcToolSequenceData {
    u32 objectIds[13];
    EtcToolVec objectPosition;
    EtcToolVec objectScale;
    EtcToolVec partyPosition;
    EtcToolVec partyScale;
} EtcToolSequenceData;

extern const EtcToolSequenceData lbl_80279A00;

/* RULE-EXCEPTION(user-approved): reconstructed linker-stripped function — see docs/RULE_EXCEPTIONS.md
 * Retail's pool has 1.0f (0x8047E410) before fn_801E075C's 1.5f, so a
 * function compiled between fn_801E03D4 and fn_801E075C used 1.0f first; it
 * has no code in retail. This stand-in reproduces the pool order and is
 * dead-stripped by the linker. */
f32 etctoolStrippedUnitScale(void)
{
    return 1.0f;
}

/**
 * fn_801E075C - Show the selected party Pokemon's ball model.
 * Address: 0x801E075C | Size: 0x284
 */
void fn_801E075C(s32 partyIndex)
{
    EtcToolVec position;
    EtcToolVec scale;
    s32 running;
    void* model;
    s32 state;
    const EtcToolSequenceData* data = &lbl_80279A00;

    position = data->partyPosition;
    scale = data->partyScale;
    model = NULL;
    state = 0;
    running = 1;

    do {
        switch (state) {
        case 0: {
            u8 ball;
            void* pokemon;
            u16 species;
            u32 objectIds[13];

            ball = pokemonBiosGetCatchBallId(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)partyIndex));
            objectIds[0] = data->objectIds[0];
            objectIds[1] = data->objectIds[1];
            objectIds[2] = data->objectIds[2];
            objectIds[3] = data->objectIds[3];
            objectIds[4] = data->objectIds[4];
            objectIds[5] = data->objectIds[5];
            objectIds[6] = data->objectIds[6];
            objectIds[7] = data->objectIds[7];
            objectIds[8] = data->objectIds[8];
            objectIds[9] = data->objectIds[9];
            objectIds[10] = data->objectIds[10];
            objectIds[11] = data->objectIds[11];
            objectIds[12] = data->objectIds[12];
            model = floorOpenObject(objectIds[ball]);
            GSvecCopy(&position, &etcObjectPosition);
            pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)partyIndex);
            species = pokemonBiosGetPokemonDataId(pokemon);
            if (pokemonCheckValid(pokemon) != 0) {
                void* pokemonData = pokemonDataBiosGetPtr(species);

                if (pokemonData != NULL) {
                    soundStartRaw(pokemonDataBiosGetVoice(pokemonData), 0, 0);
                }
            }
            GSmodelSetPosition(model, &position);
            GSmodelSetScale(model, &scale);
            state = 1;
            break;
        }
        case 1: {
            f32 timer = 0.0f;
            f32 endTime = 1.5f;

            while (timer < endTime) {
                timer += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
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


#define ETCTOOL_WAIT(duration)                                             \
    do {                                                                   \
        f32 elapsed = 0.0f;                                        \
        f32 limit = (duration);                                            \
        while (elapsed < limit) {                                          \
            elapsed += (f32)GSgetFrameStep() / (f32)fn_800D37CC();            \
            _threadSwitch();                                               \
        }                                                                  \
    } while (0)

void fn_801E09E0(s32 unused)
{
    EtcToolVec position;
    EtcToolVec savedPosition;
    EtcToolVec objectPosition;
    EtcToolVec objectScale;
    const EtcToolSequenceData* data = &lbl_80279A00;
    void* object;
    BOOL running;
    void* resource;
    void* pokemon;
    void* pokemonData;
    void* part;
    u32 objectIds[13];
    u32 state;
    u8 ballId;

    objectPosition = data->objectPosition;
    objectScale = data->objectScale;
    object = NULL;
    state = 0;
    running = TRUE;
    resource = std092_getCurrentRoomID();

    do {
        switch (state) {
        case 0:
            std172_characterGetPosition(0x4D, 1, &savedPosition);
            std076_characterEventFlagsClear(0x4D, 1);
            part = GSmodelGetPart(fn_800F92D4(0x01DA1002), 0);
            GSpartGetTransform(part, &position, 0, 0);
            GSpartFree(part);
            GSvecCopy(&position, &etcDoorPosition);
            std061_characterMoveToPosition(0x4D, 1, 1, position.x, position.y, position.z);
            peopleMoveCheck(0x4D, 1, 1);
            floorEventCtrlDoor(resource, 0x2C, 0);
            ETCTOOL_WAIT(0.6f);
            state = 1;
            break;

        case 1:
            GSvecCopy(&position, &etcOutsidePosition);
            std061_characterMoveToPosition(0x4D, 1, 1, position.x, position.y, position.z);
            peopleMoveCheck(0x4D, 1, 1);
            floorEventCtrlDoor(resource, 0x2C, 2);
            ETCTOOL_WAIT(2.0f);

            std061_characterMoveToPosition(0x4D, 1, 1, position.x, position.y,
                        position.z + 1.0f);
            peopleMoveCheck(0x4D, 1, 1);
            floorEventCtrlDoor(resource, 0x2C, 0);

            GSvecCopy(&position, &etcDoorPosition);
            std061_characterMoveToPosition(0x4D, 1, 1, position.x, position.y, position.z);
            peopleMoveCheck(0x4D, 1, 1);
            floorEventCtrlDoor(resource, 0x2C, 2);
            ETCTOOL_WAIT(0.5f);

            std061_characterMoveToPosition(0x4D, 1, 1, savedPosition.x, savedPosition.y,
                        savedPosition.z);
            peopleMoveCheck(0x4D, 1, 1);
            peopleStartTurn(0x4D, 1, 0.0f, 1.0f);
            state = 10;
            ETCTOOL_WAIT(0.8f);
            break;

        case 10:
            pokemon = sodateyaGetPokemonPtr(0);
            ballId = pokemonBiosGetCatchBallId(pokemon);
            objectIds[0] = data->objectIds[0];
            objectIds[1] = data->objectIds[1];
            objectIds[2] = data->objectIds[2];
            objectIds[3] = data->objectIds[3];
            objectIds[4] = data->objectIds[4];
            objectIds[5] = data->objectIds[5];
            objectIds[6] = data->objectIds[6];
            objectIds[7] = data->objectIds[7];
            objectIds[8] = data->objectIds[8];
            objectIds[9] = data->objectIds[9];
            objectIds[10] = data->objectIds[10];
            objectIds[11] = data->objectIds[11];
            objectIds[12] = data->objectIds[12];
            object = floorOpenObject(objectIds[ballId]);
            GSvecCopy(&objectPosition, &etcObjectPosition);

            pokemon = sodateyaGetPokemonPtr(0);
            if (pokemon != NULL) {
                pokemonData = pokemonDataBiosGetPtr(
                    pokemonBiosGetPokemonDataId(pokemon));
                if (pokemonData != NULL) {
                    soundStartRaw(pokemonDataBiosGetVoice(pokemonData), 0, 0);
                }
            }

            GSmodelSetPosition(object, &objectPosition);
            GSmodelSetScale(object, &objectScale);
            state = 2;
            break;

        case 2:
            ETCTOOL_WAIT(1.5f);
            state = 100;
            break;

        case 100:
            running = FALSE;
            std077_characterEventFlagsSet(0x4D, 1);
            GSmodelSetVisibility(object, 0);
            GSmodelFree(object);
            object = NULL;
            break;
        }
    } while (running);
}

void etctoolSetPokemonNakigoe(void)
{
    void *pokemonData;
    u16 voice;

    /* RULE-EXCEPTION(user-approved): contradicting block-scope prototype — see docs/RULE_EXCEPTIONS.md
     * Retail calls pokemonDataBiosGetPtr here without setting r3. */
    extern void *pokemonDataBiosGetPtr(void);
    extern u16 pokemonDataBiosGetVoice(void *);
    extern void soundStartRaw(u32, u32, u32);

    pokemonData = pokemonDataBiosGetPtr();
    if (pokemonData != NULL) {
        voice = pokemonDataBiosGetVoice(pokemonData);
        soundStartRaw((u32)voice, 0, 0);
    }
}
