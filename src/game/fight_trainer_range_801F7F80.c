/**
 * @file fight_trainer_range_801F7F80.c
 * @brief game/pxdvs/app/fight/fightTrainer.cpp -- split from game/trainer.c
 *        (the XD fight-trainer bucket, 0x801F7F80-0x80201764),
 *        address range 0x801F7F80-0x801FBD10, 43 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTrainer.cpp
 * Physically split out of the trainer.c bucket by address (functions
 * bucketed via their /* 0xADDR *\/ markers), byte-identical
 * reconstruction verified against the original file.
 */
#include "game/trainer.h"
#include "game/pokemon.h"
#include "game/fight_action.h"

extern void* memcpy(void* destination, const void* source, u32 size);

/* =========================================================================
 * External declarations (duplicated verbatim from the original
 * game/trainer.c preamble into every split segment, so each new
 * TU keeps the same external visibility it had before the split)
 * ========================================================================= */

/* pokemonGetStatus - Master data table resolver
 * The most-called function in the entire game (1769 calls).
 * Takes a context pointer, slot index, table ID, and flags.
 * Returns a pointer to the resolved data, or NULL. */
extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);

/* pokemonSetStatus - Master data table writer (544 calls) */
extern u32 pokemonSetStatus(void* context, u32 slot, u16 tableId, u32 flags, u32 value);

/* pokemonGrowBasisStatus - Data table auxiliary writer */
extern void pokemonGrowBasisStatus(void* context, u32 value);

/* itemGetStatus - Secondary data accessor (169 calls) */
extern u32 itemGetStatus(u32 context, u32 param, u16 field, u32 flags);

/* fightTargetGetPtr - PokemonSlotLookup (89 calls) */
extern u32 fightTargetGetPtr(u32 type, void* ptr, u32 param);

/* Category resolution sub-dispatchers (defined with real bodies at their
 * proper address-ordered locations, possibly in a sibling segment). */
void* fightTrainerDataBiosGetPtr(u16 slot); /* Battle trainer */
void* fightTrainerPokemonPartDataBiosGetPtr(u16 slot); /* Party config */
void* fightTrainerPokemonDataBiosGetPtr(u16 slot); /* Team roster */
void* fightTrainerAiDataBiosGetPtr(u16 slot); /* Story/event data */
void* fightTrainerAiValueAddsubDataBiosGetPtr(u16 slot); /* Misc attributes */

/* Event integration (defined with real bodies, possibly in a sibling segment) */
void fightOutPokemonSetHensinPokemonStatusId(void* trainer, u16 eventId, u32 param1, u32 param2);
u8   fightOutPokemonIsUseHensinBuff(void* trainer);
void fightOutPokemonSetHensinFightPokemonStatusId(void* trainer, u16 eventId, u32 param);

/* Item/Pokemon field access helpers used by battle item flow. */
u8 pokemonGetJoutai08(void* obj, s32 arg);
u8 statusGetJoutai08(void* obj, s32 arg);

/* SDA table pointers for trainer data arrays */
extern u32* lbl_80478F08;  /* Party config header */
extern u8*  lbl_80478F0C;  /* Party config data */
extern u32* lbl_80478F10;  /* Team roster header */
extern u8*  lbl_80478F14;  /* Team roster data */
extern u32* lbl_80478F18;  /* Slot data header */
extern u8*  lbl_80478F1C;  /* Slot data */
extern u32* lbl_80478F20;  /* Battle trainer header */
extern u8*  lbl_80478F24;  /* Battle trainer data */
extern u32* lbl_80478F28;  /* Misc attributes header */
extern u8*  lbl_80478F2C;  /* Misc attributes data */
extern u32* lbl_80478F30;  /* Story/event header */
extern u8*  lbl_80478F34;  /* Story/event data */

/* Forward declarations for converted functions (defined with real bodies,
 * possibly in a sibling segment). */
u16 fn_801FCC94(u8* ptr);
u8 fn_801FCCAC(u8* ptr);
void fightTrainerSetStatus(void);
void fightTrainerGetStatus(void);

/* Built as its own unit, this file contributes only 0x801F81F8-0x801F8424.
 * Every wrapper that #includes it defines FTR_BANK_EXACT_ACTIVE and the
 * FTR_EXACT_* range it builds. */
#if !defined(FTR_BANK_EXACT_ACTIVE)
#define FTR_BANK_EXACT_ACTIVE
#define FTR_EXACT_801F81F8_801F8424
#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F7F80_801F81F8)
/* 0x801F7F80 | size: 0x80 */
void* fightTrainerSearchInvalidFightPokemonPtr(void* context, u16 count) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 index);
    extern u8 fightPokemonCheckValid(void* ptr);
    int i;
    void* result;

    if (context == NULL) {
        return NULL;
    }
    for (i = 0; (u16)i < count; i++) {
        result = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(result) == 0) {
            return result;
        }
    }
    return NULL;
}

/* 0x801F8000 | size: 0x100 */
u32 fightTrainerGetKindPrefixName(void* context) {
    extern u16 std103_battleGetResult(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 index);
    extern u8* fightTrainerKindDataBiosGetPtr(u16 val);
    extern u32 fightTrainerKindDataBiosGetPrefixName(u8* ptr);
    extern u32 GSmsgGetGSchar(u32 val);
    int valid;
    void* result;
    u16 slot;

    if (context == NULL) {
        return 0;
    }
    if (context == NULL) {
        valid = 0;
    } else {
        if ((u16)std103_battleGetResult(context) == 1) {
            valid = 0;
        } else if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
            valid = 0;
        } else {
            result = fightTrainerGetStatus(context, 0, 0x44, 0);
            if (result == NULL) {
                valid = 0;
            } else if ((u8)heroCheckValid(result) == 0) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if ((u8)valid == 0) {
        return 0;
    }
    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    result = fightTrainerGetStatus(0, slot, 0x4, 0);
    result = fightTrainerKindDataBiosGetPtr((u16)(u32)result);
    return GSmsgGetGSchar(fightTrainerKindDataBiosGetPrefixName((u8*)result));
}

/* 0x801F8100 | size: 0xF8 */
void* fightTrainerGetNamePtr(void* context) {
    extern u16 std103_battleGetResult(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 index);
    extern void* heroGetStatus(void* ptr, u32 param1, u32 param2);
    int valid;
    void* result;

    if (context == NULL) {
        return NULL;
    }
    if (context == NULL) {
        valid = 0;
    } else {
        if ((u16)std103_battleGetResult(context) == 1) {
            valid = 0;
        } else if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
            valid = 0;
        } else {
            result = fightTrainerGetStatus(context, 0, 0x44, 0);
            if (result == NULL) {
                valid = 0;
            } else if ((u8)heroCheckValid(result) == 0) {
                valid = 0;
            } else {
                valid = 1;
            }
        }
    }
    if ((u8)valid == 0) {
        return NULL;
    }
    result = fightTrainerGetStatus(context, 0, 0x44, 0);
    if (result == NULL) {
        return NULL;
    }
    return heroGetStatus(result, 1, 0);
}

#endif

/* Shared inline helpers (static inline, emit no code on their own). */
static inline void* fightTrainerSearchMotoFightOutPokemonInline(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckMotoFightPokemon(void* filter, void* pokemon);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    void* pokemon;
    u16 i;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
        if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if ((u8)fightPokemonCheckMotoFightPokemon(filter, pokemon) == 1) {
                return pokemon;
            }
        }
    }
    return NULL;
}

static inline u8 fightTrainerCheckIrekaeReserveInline(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightOutPokemonCheckIrekaeReserveFightPokemon(void* pokemon, void* filter);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    u16 i;
    void* pokemon;

    if (context == NULL) {
        return 0;
    }
    if (filter == NULL) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
        if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if ((u8)fightOutPokemonCheckIrekaeReserveFightPokemon(pokemon, filter) == 1) {
                return 1;
            }
        }
    }
    return 0;
}

static inline u32 fightTrainerCheckCanIrekaeInline(void* context, void* filter) {
    extern u8 fightPokemonCheckFightOut(void* ptr);

    if ((u8)fightPokemonCheckFightOut(filter) == 0) {
        return 1;
    }
    if (fightTrainerSearchMotoFightOutPokemonInline(context, filter) != NULL) {
        return 2;
    }
    if (fightTrainerCheckIrekaeReserveInline(context, filter) == 1) {
        return 3;
    }
    return 0;
}

static inline void* fightTrainerSearchIrekaeFightPokemonInline(void* trainer, u16* index) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    void* pokemon;

    if (trainer == NULL) {
        return NULL;
    }
    if (index == NULL) {
        return NULL;
    }
    for (; *index < 6; (*index)++) {
        pokemon = fightTrainerGetStatus(trainer, 0, 0x45, *index);
        if ((u8)fightTrainerCheckCanIrekaeInline(trainer, pokemon) == 0) {
            break;
        }
    }
    if (*index >= 6) {
        return NULL;
    }
    return pokemon;
}

static inline u8 fightTrainerCheckValidInline(void* context) {
    extern u16 std103_battleGetResult(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* result;

    if (context == NULL) {
        return 0;
    }
    if ((u16)std103_battleGetResult(context) == 1) {
        return 0;
    }
    if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
        return 0;
    }
    result = fightTrainerGetStatus(context, 0, 0x44, 0);
    if (result == NULL) {
        return 0;
    }
    if ((u8)heroCheckValid(result) == 0) {
        return 0;
    }
    return 1;
}

static inline u8 fightTrainerGetHeroKindInline(u16 trainerDataId) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    u16 trainerKind;

    trainerKind = (u16)(u32)fightTrainerGetStatus(NULL, trainerDataId, 4, 0);
    if (trainerKind == 1) {
        return 0;
    }
    if (trainerKind == 2 || trainerKind == 3) {
        return 1;
    }
    return 2;
}

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F81F8_801F8424)
/* 0x801F81F8 | size: 0x22C | large */
u16 fightTrainerGetHikaeFightPokemonNum(void* trainer, u16 count, u16 required) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckValid(void* ptr);
    void* pokemon;
    u16 i;
    u16 hikaeCount;
    u16 motoCount;
    u8 kind;

    if (trainer == NULL) {
        return 0;
    }
    hikaeCount = 0;
    motoCount = 0;
    for (i = 0; i < count; i++) {
        pokemon = fightTrainerGetStatus(trainer, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            kind = fightTrainerCheckCanIrekaeInline(trainer, pokemon);
            if (kind != 1) {
                if (kind == 2) {
                    motoCount++;
                } else {
                    hikaeCount++;
                }
            }
        }
    }
    while (1) {
        if (motoCount >= required) {
            break;
        }
        if (hikaeCount < 1) {
            break;
        }
        hikaeCount--;
        motoCount++;
    }
    return hikaeCount;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F8424_801F87CC)
/* 0x801F8424 | size: 0x78 */
BOOL fightTrainerIsAllyFightTargetPtr(void* arg0, void* arg1, u32 arg2) {
    u32 val0;

    if (arg0 == NULL) {
        return FALSE;
    }
    if (arg1 == NULL) {
        return FALSE;
    }
    val0 = fightTargetGetPtr(2, arg0, arg2);
    return (u8)(fightTargetGetPtr(2, arg1, arg2) - val0 == 0);
}

/* 0x801F849C | size: 0x7C */
void fightTrainerInitEnemyPokemonFightOutStatus(void* arg0, u32 arg1) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightTrainerEnemyPokemonCheckValid(void*);
    extern void fightTrainerEnemyPokemonInitFightOutStatus(void*);
    u32 mode;
    void* pokemon;
    u8 i;

    if (arg0 == NULL) {
        return;
    }
    mode = (u8)arg1;
    i = 0;
    while (i < 12) {
        pokemon = fightTrainerGetStatus(arg0, 0, 0x4e, i);
        if (mode != 1 || (u8)fightTrainerEnemyPokemonCheckValid(pokemon)) {
            fightTrainerEnemyPokemonInitFightOutStatus(pokemon);
        }
        i++;
    }
}

/* 0x801F8518 | size: 0x98 */
void* fightTrainerGetNoActionFightOutPokemonPtr(void* context, u32 index) {
    extern u8 fightOutPokemonCheckValid(void* ptr);
    extern u8 fightActionCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* pokemon;
    void* sub;

    pokemon = fightTrainerGetStatus(context, 0, 0x46, index);
    if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
        pokemon = NULL;
    }
    if (pokemon == NULL) {
        return NULL;
    }
    sub = pokemonGetStatus(pokemon, 0, 0xFE, 0);
    if (sub == NULL) {
        return NULL;
    }
    if ((u8)fightActionCheckValid(sub) == 1) {
        return NULL;
    }
    return pokemon;
}

/* 0x801F85B0 | size: 0x88 */
s32 fightTrainerGetFightOutPokemonToTemotiBanme(void* context, void* target) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    int i;

    if (context == NULL) {
        return -1;
    }
    if (target == NULL) {
        return -1;
    }
    for (i = 0; (s16)i < 2; i++) {
        if (target == fightTrainerGetStatus(context, 0, 0x46, i)) {
            return i;
        }
    }
    return -1;
}

/* 0x801F8638 | size: 0x88 */
s32 fightTrainerGetFightPokemonToTemotiBanme(void* context, void* target) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    int i;

    if (context == NULL) {
        return -1;
    }
    if (target == NULL) {
        return -1;
    }
    for (i = 0; (s16)i < 6; i++) {
        if (target == fightTrainerGetStatus(context, 0, 0x45, i)) {
            return i;
        }
    }
    return -1;
}

/* 0x801F86C0 | size: 0x10C */
void fightTrainerBackFightPokemonToTemotiPokemon(void* context, u8 flag) {
    extern void pokemonBiosCopy(void* ptr, void* pokemon);
    extern void fn_8012190C(void* ptr, u32 field, u32 val);
    extern u8 pokemonIsJoutaiDataId(void* ptr, u32 field);
    extern u16 pokemonGetSoubiItemDataId(void* pokemon);
    extern u8 fightPokemonCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    void* slot;
    u16 i;
    void* pokemon;
    void* moveData;
    u16 speciesId;

    if (context == NULL) {
        return;
    }
    for (i = 0; i < 6; i++) {
        slot = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(slot) == 0) {
            slot = NULL;
        }
        if (slot != NULL) {
            if ((pokemon = pokemonGetStatus(slot, 0, 0xCC, 0)) != NULL) {
                speciesId = pokemonGetSoubiItemDataId(pokemon);
                if ((moveData = pokemonGetStatus(slot, 0, 0xCB, 0)) != NULL) {
                    pokemonBiosCopy(moveData, pokemon);
                    if ((u8)pokemonIsJoutaiDataId(moveData, 4) == 1) {
                        fn_8012190C(moveData, 4, 1);
                    }
                    if (flag == 1) {
                        pokemonSetStatus(moveData, 0, 0x82, 0, speciesId);
                    }
                }
            }
        }
    }
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F87CC_801F8A18)
/* 0x801F87CC | size: 0x24C | large */
u16 fightTrainerGetIrekaeFightPokemonAry(void* trainer, void** pokemonList) {
    void* pokemon;
    u16 index;
    u16 count;

    for (index = 0; index < 6; index++) {
        pokemonList[index] = NULL;
    }
    count = 0;
    index = 0;
    while (1) {
        pokemon = fightTrainerSearchIrekaeFightPokemonInline(trainer, &index);
        if (pokemon == NULL) {
            break;
        }
        pokemonList[count] = pokemon;
        count++;
        index++;
    }
    return count;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F8A18_801F8D80)
/* 0x801F8A18 | size: 0x1E8 | medium */
void* fightTrainerGetIrekaeFightPokemon(void* trainer, u16* index) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    void* pokemon;

    if (trainer == NULL) {
        return NULL;
    }
    if (index == NULL) {
        return NULL;
    }
    for (; *index < 6; (*index)++) {
        pokemon = fightTrainerGetStatus(trainer, 0, 0x45, *index);
        if ((u8)fightTrainerCheckCanIrekaeInline(trainer, pokemon) == 0) {
            break;
        }
    }
    if (*index >= 6) {
        return NULL;
    }
    return pokemon;
}

/* 0x801F8C00 | size: 0x180 */
u32 fightTrainerCheckCanIrekaeFightPokemon(void* context, void* filter) {
    extern u8 fightPokemonCheckFightOut(void* ptr);

    if ((u8)fightPokemonCheckFightOut(filter) == 0) {
        return 1;
    }
    if (fightTrainerSearchMotoFightOutPokemonInline(context, filter) != NULL) {
        return 2;
    }
    return fightTrainerCheckIrekaeReserveInline(context, filter) == 1 ? 3 : 0;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F8D80_801F9130)
/* 0x801F8D80 | size: 0xB4 */
void* fightTrainerGetFightPokemonPtrToFightOutPokemonPtr(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckMotoFightPokemon(void* filter, void* pokemon);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    void* pokemon;
    u16 i;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    for (i = 0; i < 2; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
        if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if ((u8)fightPokemonCheckMotoFightPokemon(filter, pokemon) == 1) {
                return pokemon;
            }
        }
    }
    return NULL;
}

/* 0x801F8E34 | size: 0xF0 */
void* fightTrainerCheckFightPokemonFightOut(void* context, void* filter) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckMotoFightPokemon(void* filter, void* pokemon);
    extern u8 fightOutPokemonCheckFightOut(void* ptr);
    extern u8 fightOutPokemonCheckValid(void* ptr);
    u16 i;
    void* pokemon;

    if (context == NULL) {
        return NULL;
    }
    if (filter == NULL) {
        return NULL;
    }
    pokemon = NULL;
    if (context == NULL) {
        pokemon = NULL;
    } else if (filter == NULL) {
        pokemon = NULL;
    } else {
        for (i = 0; i < 2; i++) {
            pokemon = fightTrainerGetStatus(context, 0, 0x46, i);
            if ((u8)fightOutPokemonCheckValid(pokemon) == 0) {
                pokemon = NULL;
            }
            if (pokemon != NULL) {
                if ((u8)fightPokemonCheckMotoFightPokemon(filter, pokemon) == 1) {
                    if (((!filter) && (!filter)) && (!filter)) {
                        /* Preserve MWCC register allocation. */
                    }
                    goto found;
                }
            }
        }
        pokemon = NULL;
    }
found:
    if (pokemon != NULL) {
        if ((u8)fightOutPokemonCheckFightOut(pokemon) == 0) {
            pokemon = NULL;
        }
    }
    return pokemon;
}

/* 0x801F8F24 | size: 0xB4 */
void* fightTrainerGetEntryIdToFightPokemonPtr(void* context, s16 speciesId) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern u8 fightPokemonCheckValid(void* ptr);
    void* pokemon;
    u16 i;
    s16 species;

    species = speciesId;
    if (species < 0) {
        return NULL;
    }
    for (i = 0; i < 6; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(pokemon) == 0) {
            pokemon = NULL;
        }
        if (pokemon != NULL) {
            if (species == (s16)(s32)pokemonGetStatus(pokemon, 0, 0xCE, 0)) {
                return pokemon;
            }
        }
    }
    return NULL;
}

/* 0x801F8FD8 | size: 0x5C */
void* fightTrainerCreateSequence(void* arg0) {
    extern void* wazaSequenceOwnerSetVisibility(void*, u32);
    extern void* wazaSequenceTrainerCreate(u32);
    extern void* fightTrainerGetStatus(void*, void*, u32, u32);
    void* result;

    result = wazaSequenceTrainerCreate((u16)(u32)fightTrainerGetStatus(0, arg0, 9, 0));
    if (result == NULL) {
        return NULL;
    }
    wazaSequenceOwnerSetVisibility(result, 0);
    return result;
}

/* 0x801F9034 | size: 0x90 */
u32 fightTrainerCheckCanGetExp(void* context) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    u16 slot;
    u16 type;
    u8 val;

    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    type = (u16)(u32)fightTrainerGetStatus(0, slot, 0x4, 0);
    if (type == 1) {
        val = 0;
    } else if (type == 2 || type == 3) {
        val = 1;
    } else {
        val = 2;
    }
    if ((u8)(val == 0) == 1) {
        if (((!type) && (!type)) && (!type)) {
            /* Preserve MWCC register allocation. */
        }
        return 1;
    }
    return 0;
}

/* 0x801F90C4 | size: 0x6C */
void* fightTrainerIsMineFightPokemon(void* arg0, void* arg1) {
    extern void* heroIsMinePokemon(void*, void*);
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    void* pokemon;
    void* trainerData;

    extern void* fightPokemonGetPokemonPtr(void*);
    pokemon = fightPokemonGetPokemonPtr(arg1);
    if (pokemon == NULL) {
        return NULL;
    }
    trainerData = fightTrainerGetStatus(arg0, 0, 0x44, 0);
    if (trainerData == NULL) {
        return NULL;
    }
    return heroIsMinePokemon(trainerData, pokemon);
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F9130_801F93F8)
/* 0x801F9130 | size: 0x10C */
void fightTrainerTimeOutSelectFightAction(void* unused, void* trainer, void* pokemon) {
    extern FightActionData lbl_80375CA8[];
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32 target, void* pokemon);
    extern u8 fightOutPokemonCheckFightActionWazaSelect(void* trainer, u32 mode);
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme(void* trainer, s32 slot, u32 field, u32 flags);
    extern void* fightOutPokemonGetPokemonPtr(void* trainer);
    extern u32 fightSeqSelectTarget(void* trainer, u16 moveId, void* pokemon, u32 p3, u32 p4, u32 p5, s32 p6);
    extern void* fightOutPokemonCreateFightActionAttackWaza(
        void* ctx, void* motoAction, u32 kind, u32 buff,
        FightActionData* actionData, u32 moveId, u32 targetId, s8 slot, u8 mode);
    s32 slot;
    u16 moveId;
    s8 i;

    if ((u8)fightOutPokemonCheckFightActionWazaSelect(trainer, 1) != 0) {
        return;
    }
    for (i = 0; i < 4; i++) {
        moveId = (u16)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainer), 0, 0x7F, 0);
        slot = i;
        if ((u8)fightOutPokemonCheckCanOutOkWazaBanme(trainer, slot, 1, 0) == 0) {
            moveId = (u16)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainer), 0, 0x7F, slot);
            break;
        }
    }
    fightOutPokemonCreateFightActionAttackWaza(
        trainer, NULL, 0x13, 0, lbl_80375CA8, moveId,
        fightTargetGetTragetPtrToRelativeHostSideFightTargetId(
            fightSeqSelectTarget(trainer, moveId, pokemon, 0, 1, 0, -1), pokemon),
        i, 0);
}

/* 0x801F923C | size: 0x1BC */
u32 fightTrainerSelectFightAction(void* context, void* param) {
    extern u16 std103_battleGetResult();
    extern void fightActionInit(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern void fightOutPokemonInitFightActionBuff(void* ptr);
    extern void fightTrainerAiSelectFightAction(void* ctx, void* param);
    extern u8 fightMenuFightTrainerAgbHeroOpenMenu(void* ctx, void* param);
    extern void fightMenuFightTrainerGcHeroOpenMenu(void* ctx, void* param, u32 flags);
    void* result;
    u32 slot;
    u16 i;

    result = fightTrainerGetStatus(context, 0, 0x4D, 0);
    if (result != NULL) {
        fightActionInit(result);
    }
    for (i = 0; i < 2; i++) {
        fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x46, i));
    }
    fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x47, 0));
    if ((u16)std103_battleGetResult() == 1) {
        return 0;
    }
    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    if ((u16)(u32)fightTrainerGetStatus(0, slot, 0x2, 0) != 0) {
        fightTrainerAiSelectFightAction(context, param);
    } else if (fightTrainerGetHeroKindInline(slot) == 0) {
        fightMenuFightTrainerGcHeroOpenMenu(context, param, 0);
    } else if (fightTrainerGetHeroKindInline(slot) == 1) {
        if ((u8)fightMenuFightTrainerAgbHeroOpenMenu(context, param) == 0) {
            return 0;
        }
    }
    return 1;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F93F8_801F9600)
/* 0x801F93F8 | size: 0x208 | large */
s16 fightTrainerTimeOutSelectIrekaeFightPokemon(void* trainer) {
    void* pokemon;
    u16 index;

    index = 0;
    pokemon = fightTrainerSearchIrekaeFightPokemonInline(trainer, &index);
    if (pokemon != NULL) {
        return (s16)(u32)pokemonGetStatus(pokemon, 0, 0xCE, 0);
    }
    return -1;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F9600_801F9790)
/* 0x801F9600 | size: 0x190 */
s32 fightTrainerSelectIrekaeFightPokemon(void* context, void* p1, void* p2, void* p3) {
    extern u8 dbgMenuFightGetAllManual(void);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, void* p1, void* p2, void* p3);
    extern s32 fightMenuFightTrainerAgbHeroSelectIrekaeFightPokemon(void* ctx, void* p1, void* p2, void* p3);
    extern s32 fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(void* ctx, u32 zero, void* p1, void* p2, u32 flags);
    u16 species;
    u32 slot;
    s32 result;

    result = -1;
    slot = (u16)(u32)fightTrainerGetStatus(context, 0, 0x43, 0);
    species = (u16)(u32)fightTrainerGetStatus(0, slot, 0x2, 0);
    if ((u8)dbgMenuFightGetAllManual() == 1) {
        result = fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(context, 0, p1, p2, 0);
    } else if (species != 0) {
        result = fightTrainerAiSelectIrekaeDasuFightPokemon(context, p1, p2, p3);
    } else if (fightTrainerGetHeroKindInline(slot) == 0) {
        result = fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(context, 0, p1, p2, 0);
    } else if (fightTrainerGetHeroKindInline(slot) == 1) {
        result = fightMenuFightTrainerAgbHeroSelectIrekaeFightPokemon(context, p1, p2, p3);
    }
    return result;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F9790_801F99C8)
/* 0x801F9790 | size: 0x8C */
void fightTrainerAllInitFightActionBuff(void* context) {
    extern void fightActionInit(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u16 idx);
    extern void fightOutPokemonInitFightActionBuff(void* ptr);
    void* result;
    u16 i;

    result = fightTrainerGetStatus(context, 0, 0x4D, 0);
    if (result != NULL) {
        fightActionInit(result);
    }
    for (i = 0; i < 2; i++) {
        fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x46, i));
    }
    fightOutPokemonInitFightActionBuff(fightTrainerGetStatus(context, 0, 0x47, 0));
}

/* 0x801F981C | size: 0x50 */
void* fightTrainerGetValidFightOutPokemonPtr(void* arg1, u32 arg2) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightOutPokemonCheckValid(void*);
    void* result = fightTrainerGetStatus(arg1, 0, 0x46, arg2);
    if ((u8)fightOutPokemonCheckValid(result) == 0) {
        return NULL;
    }
    return result;
    return;
}

/* 0x801F986C | size: 0x50 */
void* fightTrainerGetValidFightPokemonPtr(void* arg1, u32 arg2) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightPokemonCheckValid(void*);
    void* result = fightTrainerGetStatus(arg1, 0, 0x45, arg2);
    if ((u8)fightPokemonCheckValid(result) == 0) {
        return NULL;
    }
    return result;
}

/* 0x801F98BC | size: 0x74 */
u32 fightTrainerGetDoFightOutFightOutPokemonCount(void* arg0) {
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightOutPokemonCheckFightOut(void*);
    u8 i;
    u32 count;

    count = 0;
    for (i = 0; i < 2; i++) {
        if ((u8)fightOutPokemonCheckFightOut(fightTrainerGetStatus(arg0, 0, 0x46, i))) {
            count = ((count & 0xFF) + 1) & 0xFF;
        }
    }
    return count;
}

/* 0x801F9930 | size: 0x98 */
void* fightTrainerCheckTemotiPokemonFightEntry(void* context, void* moveData) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fightPokemonCheckValid(void* ptr);
    int i;
    void* pokemon;
    void* data;

    for (i = 0; (u16)i < 6; i++) {
        pokemon = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(pokemon) != 0) {
            data = pokemonGetStatus(pokemon, 0, 0xCB, 0);
            if (data != NULL) {
                if (moveData == data) {
                    return pokemon;
                }
            }
        }
    }
    return NULL;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F99C8_801F9F78)
/* 0x801F99C8 | size: 0x2F4 | large */
typedef struct FightTrainerPokemonCopy {
    u8 data[0x138];
} FightTrainerPokemonCopy;

void fightTrainerSortFightTrainerDataIdToHeroTemotiPokemon(
    void* trainer, u16 selectionCount, u16 selectionMode) {
    extern u8 pokemonCheckValid(void*);
    extern void pokemonInit(void*);
    extern void* heroGetStatus(void*, s32, s32);
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern void fightTrainerSortTemoti(void*, void*, void**, u16, u16);
    void* selected[6];
    FightTrainerPokemonCopy sortedPokemon[6];
    void* hero;
    FightTrainerPokemonCopy* pokemon;
    u32 trainerDataId;
    u16 trainerPokemonCount;
    u8 i;
    u8 validCount;

    if (trainer == NULL) {
        return;
    }
    if (fightTrainerCheckValidInline(trainer) == 0) {
        return;
    }
    trainerDataId = (u16)(u32)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerPokemonCount =
        (u16)(u32)fightTrainerGetStatus(NULL, trainerDataId, 2, 0);
    hero = fightTrainerGetStatus(trainer, 0, 0x44, 0);
    if (trainerPokemonCount == 0) {
        return;
    }
    if (fightTrainerGetHeroKindInline(trainerDataId) == 0 || fightTrainerGetHeroKindInline(trainerDataId) == 1) {
        return;
    }

    for (i = 0; i < 6; i++) {
        selected[i] = NULL;
        pokemonInit(&sortedPokemon[i]);
    }

    if (selectionCount > 6) {
        selectionCount = 6;
    }
    fightTrainerSortTemoti(trainer, hero, selected, selectionCount, selectionMode);

    validCount = 0;
    for (i = 0; i < selectionCount; i++) {
        pokemon = selected[i];
        if (pokemon != NULL && pokemonCheckValid(pokemon)) {
            sortedPokemon[validCount] = *pokemon;
            validCount++;
        }
    }

    for (i = 0; i < 6; i++) {
        pokemon = heroGetStatus(hero, 3, i);
        if (pokemon != NULL) {
            *pokemon = sortedPokemon[i];
        }
    }
}

static inline u8 fightTrainerCheckTrainerDataIdValidInline(u16 trainerDataId, s32 inputDevice) {
    extern void* fightTrainerGetStatus(void*, u16, u32, u16);
    u32 trainerPokemonCount;

    trainerPokemonCount = (u16)(u32)fightTrainerGetStatus(NULL, trainerDataId, 2, 0);
    if (trainerDataId == 0) {
        return FALSE;
    }
    if (trainerPokemonCount == 0 && inputDevice == 0) {
        return FALSE;
    }
    return TRUE;
}

static inline u8 fightTrainerGetTrainerDataKindInline(u16 trainerDataId) {
    extern void* fightTrainerGetStatus(void*, u16, u32, u16);
    u16 trainerKind;

    trainerKind = (u16)(u32)fightTrainerGetStatus(NULL, trainerDataId, 4, 0);
    if (trainerKind == 1) {
        return 0;
    }
    if (trainerKind == 2 || trainerKind == 3) {
        return 1;
    }
    return 2;
}

/* 0x801F9CBC | size: 0x2BC | large */
void fightTrainerCreateFightTrainerDataIdToHero(
    u16 trainerDataId, s32 inputDevice, void* hero) {
    extern u32 fn_800896B8(void);
    extern void* fn_800896C8(void);
    extern void* GSmsgGetGSchar(u32);
    extern void* savedataGetStatus(u32, u32);
    extern s32 heroItemAddItemDataId(void*, u16, u32, s32);
    extern s32 heroCatchPokemon(void*, void*, u32, u16, u8);
    extern void heroCreate(void*, void*, u8);
    extern void heroSetStatus(void*, u32, u32);
    extern void* heroGetStatus(void*, u32, u32);
    extern void heroBiosCopy(void*, void*);
    extern u8 fightTrainerCreateFightTrainerPokemonDataIdToPokemon(
        u16, void*, void*);
    extern void* fightTrainerGetStatus(void*, u16, u32, u16);
    u8 pokemon[0x138];
    void* savedHero;
    u16 pokemonDataId;
    u16 itemDataId;
    u8 sex;
    u16 i;
    void* name;

    if (hero == NULL) {
        return;
    }
    if (fightTrainerCheckTrainerDataIdValidInline(trainerDataId, inputDevice) == 0) {
        return;
    }
    if (fightTrainerGetTrainerDataKindInline(trainerDataId) == 0) {
        savedHero = savedataGetStatus(0, 2);
        if (savedHero != NULL) {
            heroBiosCopy(hero, savedHero);
        }
        return;
    }

    (void)fightTrainerGetStatus(NULL, trainerDataId, 4, 0);
    sex = (u8)(u32)fightTrainerGetStatus(NULL, trainerDataId, 1, 0);
    name = GSmsgGetGSchar(
        (u32)fightTrainerGetStatus(NULL, trainerDataId, 3, 0));
    if (trainerDataId == fn_800896B8()) {
        name = fn_800896C8();
    }
    heroCreate(hero, name, sex);

    pokemonDataId =
        (u16)(u32)fightTrainerGetStatus(NULL, trainerDataId, 5, 0);
    for (;;) {
        if (!fightTrainerCreateFightTrainerPokemonDataIdToPokemon(
                pokemonDataId, pokemon, heroGetStatus(hero, 2, 0))) {
            break;
        }
        pokemonSetStatus(pokemon, 0, 0xC9, 0, pokemonDataId);
        if ((s16)heroCatchPokemon(
                hero, pokemon, 0,
                (u16)(u32)fightTrainerGetStatus(
                    NULL, pokemonDataId, 0x12, 0),
                0) < 0) {
            break;
        }
        pokemonDataId++;
    }

    for (i = 0; i < 8; i++) {
        itemDataId =
            (u16)(u32)fightTrainerGetStatus(NULL, trainerDataId, 6, i);
        if (itemDataId != 0 &&
            heroItemAddItemDataId(hero, itemDataId, 1, -1) < 0) {
            break;
        }
    }

    heroSetStatus(hero, 0xF, 0);
    heroSetStatus(hero, 0x10, 0);
    heroSetStatus(hero, 0x11, 0);
    heroSetStatus(hero, 0x12, 0);
    heroSetStatus(hero, 0x13, 0);
    heroSetStatus(hero, 0x14, 0);
    heroSetStatus(hero, 0x15, 0);
    heroSetStatus(hero, 0x16, 0);
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801F9F78_801FA4B4)
/* 0x801F9F78 | size: 0x53C | large */
typedef struct FightTrainerStatusIds {
    u16 id[6];
} FightTrainerStatusIds;

/* RULE-EXCEPTION(user-approved): signed trainer-ID local copy, explicit
 * conversions and a no-op held-item OR chosen for register allocation — see
 * docs/RULE_EXCEPTIONS.md and
 * docs/recon/trainer_pokemon_match_checkpoint_20261008.md. */
u8 fightTrainerCreateFightTrainerPokemonDataIdToPokemon(
    u16 pokemonDataId, void* pokemon, u32 trainerId) {
    extern FightTrainerStatusIds lbl_80279C48;
    extern FightTrainerStatusIds lbl_80279C54;
    extern u32 GSmsgGetGSchar(u32 msgId);
    extern u32 gamedataGetStatus(u32 kind, u32 field);
    extern void pokemonCreate(void* pokemon, u16 species, u8 level, u32 trainerId);
    extern void pokemonSetDarkPokemonStatus(void* pokemon, u16 darkId);
    /* pokemonSetStatus returns void (see its definition in pokemon_range_8011F5FC.c);
     * the file-scope u32 prototype adds a result block after each call. */
    extern void pokemonSetStatus(void* context, u32 slot, u16 tableId, u32 flags, u32 value);
    extern u32 pokemonDoItemSoubi(void* pokemon, u16 item, u8 flag);
    extern void pokemonWazaCreate(void* pokemon, u32 slot, u16 waza);
    extern void pokemonWazaInit(void* pokemon, s32 slot);
    extern u32 pokemonCreateRndFit(void* pokemon, s8 sex, s8 seikaku, s8 flag, u32 trainerId);
    extern void pokemonSetTokuseiFlag(void* pokemon, u8 flag);
    extern void pokemonResetBasisStatus(void* pokemon);
    extern u32 darkPokemonSaveGet8(u16 darkId);
    extern void darkPokemonSaveSet8(u16 darkId, u32 value);
    extern s8 fn_801EE824(u16 darkId, u16 index);
    extern void darkPokemonSetFlag24(u16 darkId, u16 index, s8 value);
    extern u8 darkPokemonCheckFlag16(u16 darkId);
    extern s32 fightTrainerGetStatus(void* ctx, u16 slot, u32 field, u16 idx);
    u8 wazaPoint[4];
    s8 individual[6];
    s16 effort[6];
    s32 waza[4];
    FightTrainerStatusIds individualIds;
    FightTrainerStatusIds effortIds;
    u8 darkId;
    s8 sex;
    s8 seikaku;
    s8 tokusei;
    s16 friend;
    s32 trainer; /* RULE-EXCEPTION(user-approved): signed local copy of trainerId for allocation — see docs/RULE_EXCEPTIONS.md */
    s32 item;
    s32 itemValue;
    u16 darkIndex;
    u16 species;
    u32 nameId;
    u8 stat;
    u8 level;
    u8 i;

    trainer = trainerId;
    individualIds = lbl_80279C48;
    effortIds = lbl_80279C54;
    species = fightTrainerGetStatus(NULL, pokemonDataId, 0x15, 0);
    if (species == 0) {
        return 0;
    }
    darkId = fightTrainerGetStatus(NULL, pokemonDataId, 0x13, 0);
    level = fightTrainerGetStatus(NULL, pokemonDataId, 0x11, 0);
    nameId = fightTrainerGetStatus(NULL, pokemonDataId, 0xE, 0);
    for (i = 0; i < 6; i++) {
        individual[i] = fightTrainerGetStatus(NULL, pokemonDataId, 0xF, i);
    }
    for (i = 0; i < 6; i++) {
        effort[i] = fightTrainerGetStatus(NULL, pokemonDataId, 0x10, i);
    }
    tokusei = fightTrainerGetStatus(NULL, pokemonDataId, 0x14, 0);
    /* RULE-EXCEPTION(user-approved): no-op `item |= itemValue` keeps the
     * named item web (retail r24); a plain assignment is copy-propagated into
     * the call-result temporary — see docs/RULE_EXCEPTIONS.md */
    item = itemValue = fightTrainerGetStatus(NULL, pokemonDataId, 0x16, 0);
    item |= itemValue;
    for (i = 0; i < 4; i++) {
        waza[i] = fightTrainerGetStatus(NULL, pokemonDataId, 0x17, i);
        wazaPoint[i] = fightTrainerGetStatus(NULL, pokemonDataId, 0x18, i);
    }
    friend = fightTrainerGetStatus(NULL, pokemonDataId, 0x19, 0);
    sex = fightTrainerGetStatus(NULL, pokemonDataId, 0x1A, 0);
    seikaku = fightTrainerGetStatus(NULL, pokemonDataId, 0x1B, 0);

    pokemonCreate(pokemon, species, level, gamedataGetStatus(0, 1));
    if (nameId != 0) {
        pokemonSetStatus(pokemon, 0, 0x77, 0, GSmsgGetGSchar(nameId));
    }
    darkIndex = darkId;
    for (stat = 0; stat < 6; stat++) {
        if (darkId != 0 && (u8)darkPokemonCheckFlag16((u8)darkId) == 1) {
            pokemonSetStatus(pokemon, 0, individualIds.id[stat], 0,
                             (u16)fn_801EE824((u8)darkId, stat));
        } else if (individual[stat] >= 0) {
            pokemonSetStatus(pokemon, 0, individualIds.id[stat], 0, individual[stat]);
        }
    }
    for (i = 0; i < 6; i++) {
        if (effort[i] >= 0) {
            pokemonSetStatus(pokemon, 0, effortIds.id[i], 0, effort[i]);
        }
    }
    if (tokusei >= 0) {
        pokemonSetTokuseiFlag(pokemon, tokusei);
    }
    if (item >= 0) {
        if (item == 0) {
            pokemonDoItemSoubi(pokemon, 0, 0);
        } else {
            pokemonDoItemSoubi(pokemon, (u16)item, 1);
        }
    }
    for (i = 0; i < 4; i++) {
        if (waza[i] >= 0) {
            if (waza[i] == 0 && waza[i] == 0x164 && waza[i] == 0x165 &&
                waza[i] == 0x163) {
                pokemonWazaInit(pokemon, i);
            } else {
                pokemonWazaCreate(pokemon, (u8)i, waza[i]);
                pokemonSetStatus(pokemon, 0, 0x81, (u8)i, wazaPoint[i]);
            }
        }
    }
    if (friend >= 0) {
        pokemonSetStatus(pokemon, 0, 0x99, 0, friend);
    }
    if (darkId != 0 && darkPokemonCheckFlag16(darkIndex) == 1) {
        pokemonSetStatus(pokemon, 0, 0x6F, 0, darkPokemonSaveGet8(darkIndex));
    } else {
        pokemonSetStatus(pokemon, 0, 0x6F, 0,
                         pokemonCreateRndFit(pokemon, sex, seikaku, 0, trainer));
    }
    if (darkId != 0) {
        pokemonSetDarkPokemonStatus(pokemon, darkIndex);
        if (darkPokemonCheckFlag16(darkIndex) == 0) {
            darkPokemonSaveSet8(darkIndex, (u32)pokemonGetStatus(pokemon, 0, 0x6F, 0));
            for (i = 0; i < 6; i++) {
                u16 value = (u32)pokemonGetStatus(pokemon, 0, individualIds.id[i], 0);
                darkPokemonSetFlag24(darkIndex, i, value);
            }
        }
    }
    pokemonResetBasisStatus(pokemon);
    return 1;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FA4B4_801FA524)
/* 0x801FA4B4 | size: 0x70 */
BOOL fightTrainerCheckTrainerDataIdValid(u32 arg0, s32 arg1) {
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    u32 result;

    result = (u16)fightTrainerGetStatus(0, arg0, 2, 0);
    if ((u16)arg0 == 0) {
        return FALSE;
    }
    if (result == 0 && arg1 == 0) {
        return FALSE;
    }
    return TRUE;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FA524_801FA634)
static inline u8 fightTrainerGetFightOutCountInline(void* context) {
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern u8 fightPokemonCheckFightOut(void* ptr);
    u8 i;
    u8 count;

    count = 0;
    for (i = 0; i < 6; i++) {
        if ((u8)fightPokemonCheckFightOut(fightTrainerGetStatus(context, 0, 0x45, i)) != 0) {
            count++;
        }
    }
    return count;
}

/* 0x801FA524 | size: 0x110 */
u32 fightTrainerCheckDoFight(void* context) {
    if (fightTrainerCheckValidInline(context) == 0) {
        return 0;
    }
    return fightTrainerGetFightOutCountInline(context) != 0;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FA634_801FAA58)
/* 0x801FA634 | size: 0xA4 */
u32 fightTrainerCheckValid(void* context) {
    extern u16 std103_battleGetResult(void* ctx);
    extern u8 heroCheckValid(void* ptr);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* result;

    if (context == NULL) {
        return 0;
    }
    if ((u16)std103_battleGetResult(context) == 1) {
        return 0;
    }
    if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
        return 0;
    }
    result = fightTrainerGetStatus(context, 0, 0x44, 0);
    if (result == NULL) {
        return 0;
    }
    return (u8)heroCheckValid(result) != 0;
}

/* 0x801FA6D8 | size: 0x1F4 | medium */
void fightTrainerCreate(void* trainer, void* arg1, u16 arg2, u32 arg3, u32 arg4) {
    extern void heroInit(void* ptr);
    extern void heroBiosCopy(void* ptr, void* arg);
    extern void fightActionInit(void* ptr);
    extern void fightFloorSetShadow(void);
    extern void fightTrainerSetStatus(void* ctx, u32 slot, u32 field, u32 idx, u32 val);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fightOutPokemonInitAry(void* ptr, u32 count);
    extern void fightOutPokemonInit(void* ptr);
    extern void fightPokemonInitAry(void* ptr, u32 count);
    extern void fightTrainerEnemyPokemonInitAry(void* ptr, u32 count);
    void* result;

    if (trainer != NULL && arg1 != NULL) {
        if (trainer != NULL) {
            heroInit(fightTrainerGetStatus(trainer, 0, 0x44, 0));
            fightTrainerSetStatus(trainer, 0, 0x43, 0, 0);
            fightPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x45, 0), 6);
            fightOutPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x46, 0), 2);
            fightOutPokemonInit(fightTrainerGetStatus(trainer, 0, 0x47, 0));
            fightTrainerSetStatus(trainer, 0, 0x48, 0, 1);
            fightTrainerSetStatus(trainer, 0, 0x49, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4A, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4B, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4C, 0, 0);
            result = fightTrainerGetStatus(trainer, 0, 0x4D, 0);
            if (result != NULL) {
                fightActionInit(result);
            }
            fightTrainerEnemyPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x4E, 0), 0xC);
        }
        result = fightTrainerGetStatus(trainer, 0, 0x44, 0);
        if (result != NULL) {
            heroBiosCopy(result, arg1);
            fightTrainerSetStatus(trainer, 0, 0x43, 0, arg2);
            fightTrainerSetStatus(trainer, 0, 0x4B, 0, arg3);
            if (arg4 != 0) {
                fightTrainerSetStatus(trainer, 0, 0x4C, 0, arg4);
                fightFloorSetShadow();
            }
        }
    }
}

/* 0x801FA8CC | size: 0x18C */
void fightTrainerInit(void* base, u16 count) {
    extern void heroInit(void* ptr);
    extern void fightActionInit(void* ptr);
    extern void fightTrainerSetStatus(void* ctx, u32 slot, u32 field, u32 idx, u32 val);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern void fightOutPokemonInitAry(void* ptr, u32 count);
    extern void fightOutPokemonInit(void* ptr);
    extern void fightPokemonInitAry(void* ptr, u32 count);
    extern void fightTrainerEnemyPokemonInitAry(void* ptr, u32 count);
    u8* trainer;
    void* result;
    u16 i;

    if (base == NULL) {
        return;
    }
    for (i = 0; i < count; i++) {
        trainer = (u8*)base + i * 0x28E4;
        if (trainer != NULL) {
            heroInit(fightTrainerGetStatus(trainer, 0, 0x44, 0));
            fightTrainerSetStatus(trainer, 0, 0x43, 0, 0);
            fightPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x45, 0), 6);
            fightOutPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x46, 0), 2);
            fightOutPokemonInit(fightTrainerGetStatus(trainer, 0, 0x47, 0));
            fightTrainerSetStatus(trainer, 0, 0x48, 0, 1);
            fightTrainerSetStatus(trainer, 0, 0x49, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4A, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4B, 0, 0);
            fightTrainerSetStatus(trainer, 0, 0x4C, 0, 0);
            result = fightTrainerGetStatus(trainer, 0, 0x4D, 0);
            if (result != NULL) {
                fightActionInit(result);
            }
            fightTrainerEnemyPokemonInitAry(fightTrainerGetStatus(trainer, 0, 0x4E, 0), 0xC);
        }
    }
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801FAA58 | size: 0x768 | large */
void fightTrainerSetStatus(void) {
    extern void fightTrainerGetStatus();
    extern void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag();
    extern void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag();
    extern void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetStoreTokuseiData();
    extern void fightTrainerEnemyPokemonBiosSetTokuseiFlag();
    extern void fn_801FBDF4();
    extern void fightTrainerAiValueAddsubDataBiosSetPrefixName();
    extern void fightTrainerAiValueAddsubDataBiosSetKoudouName();
    extern void fightTrainerAiValueAddsubDataBiosSetName();
    extern void fightTrainerAiValueAddsubDataBiosSetValue();
    extern void fightTrainerAiDataBiosSetLastValueRevise();
    extern void fightTrainerAiDataBiosSetWazaNokoriPpValue();
    extern void fightTrainerAiDataBiosSetWazaRiskFlag();
    extern void fightTrainerAiDataBiosSetWazaAvgValue();
    extern void fightTrainerAiDataBiosSetWazaHitFlag();
    extern void fightTrainerAiDataBiosSetWazaTypeReviseValue();
    extern void fightTrainerAiDataBiosSetWazaTypeReviseTypeDataId();
    extern void fightTrainerAiDataBiosSetPartFlag();
    extern void fightTrainerAiDataBiosSetWazaInitValueFlag();
    extern void fightTrainerAiDataBiosSetWazaRndSelectFlag();
    extern void fightTrainerAiDataBiosSetDefensePokemonRndSelectFlag();
    extern void fightTrainerAiDataBiosSetAbicntMinValue();
    extern void fightTrainerAiDataBiosSetAbicntMaxValue();
    extern void fightTrainerAiDataBiosSetWazaDamageFlag();
    extern void fightTrainerAiDataBiosSetNokoriHpValue();
    extern void fightTrainerAiDataBiosSetTokuseiCheckFlag();
    extern void fightTrainerAiDataBiosSetZokuseiReviseValue();
    extern void fightTrainerAiDataBiosSetZokuseiReviseZokuseiDataId();
    extern void fightTrainerAiDataBiosSetZokuseiCheckFlag();
    extern void fightTrainerAiDataBiosSetParamStoreFlag();
    extern void fightTrainerAiDataBiosSetParamExpectFlag();
    extern void fightTrainerAiDataBiosSetItemValue();
    extern void fightTrainerAiDataBiosSetIrekaeValue();
    extern void fightTrainerAiDataBiosSetComboValue();
    extern void fightTrainerAiDataBiosSetPokemonJoutaiFlag();
    extern void fightTrainerAiDataBiosSetKeyPlayerFlag();
    extern void fightTrainerAiDataBiosSetPokemonDataOrderAceBossFlag();
    extern void fightTrainerAiDataBiosSetPokemonDataOrderOutFlag();
    extern void fightTrainerAiDataBiosSetPokemonSelectWeakPointFlag();
    extern void fightTrainerAiDataBiosSetPokemonSelectRandomFlag();
    extern void fightTrainerPokemonDataBiosSetPartDataId();
    extern void fightTrainerPokemonDataBiosSetKeyPlayerFlag();
    extern void fightTrainerPokemonDataBiosSetSeikakuDataId();
    extern void fightTrainerPokemonDataBiosSetSexDataId();
    extern void fightTrainerPokemonDataBiosSetFriend();
    extern void fightTrainerPokemonDataBiosSetPpCnt();
    extern void fightTrainerPokemonDataBiosSetWazaDataId();
    extern void fightTrainerPokemonDataBiosSetItemDataId();
    extern void fightTrainerPokemonDataBiosSetPokemonDataId();
    extern void fightTrainerPokemonDataBiosSetTokuseiFlag();
    extern void fightTrainerPokemonDataBiosSetDarkPokemonFlag();
    extern void fightTrainerPokemonDataBiosSetItemBallId();
    extern void fightTrainerPokemonDataBiosSetLevel();
    extern void fightTrainerPokemonDataBiosSetStatusEffort();
    extern void fightTrainerPokemonDataBiosSetStatusRnd();
    extern void fightTrainerPokemonDataBiosSetNickname();
    extern void fightTrainerPokemonPartDataBiosSetWazaTypeRevise();
    extern void fightTrainerPokemonPartDataBiosSetName();
    extern void fightTrainerSet10();
    extern void fightTrainerSet24Arr();
    extern void fightTrainerSet0C();
    extern void fightTrainerSetU16Arr14();
    extern void fightTrainerSet04();
    extern void fightTrainerSet08();
    extern void fightTrainerSet06();
    extern void fightTrainerSet00();
    extern void fightTrainerDataBiosSetKindDataId();
    extern void fightTrainer_SetControllerId();
    extern void fightTrainer_SetNigeruCount();
    extern void fightTrainer_SetKoban();
    extern void fightTrainer_SetOkaneBai();
    extern void fightTrainer_SetFightTrainerDataId();
    extern void fightTrainer_SetSequencePtr();
    extern void fightTrainerEnemyPokemonEraseAry();
    extern void fightTrainerEnemyPokemonRegistAry();
    extern void fightTrainerEnemyPokemonSearchAry();
    extern void fightTrainerEnemyPokemonInitFightOutStatus();
    extern u8 jumptable_80375670[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r0 = r5 & 0xFFFF;
    r30 = r5;
    r28 = r4;
    r29 = r6;
    r31 = r7;
    if ((s32)r0 == (s32)0) return;
    if (r0 >= (u32)0x5b) {
        return;
    }
    if (r0 < (u32)0xa) {
        r3 = r28;
        ((void(*)(void))fightTrainerDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0xd) {
        r3 = r28;
        ((void(*)(void))fightTrainerPokemonPartDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x1e) {
        r3 = r28;
        ((void(*)(void))fightTrainerPokemonDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x3d) {
        r3 = r28;
        ((void(*)(void))fightTrainerAiDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;

    }
    if (r0 < (u32)0x42) {
        r3 = r28;
        ((void(*)(void))fightTrainerAiValueAddsubDataBiosGetPtr)();
        if (r3 == (u32)0x0) return;
    }
    if (r3 == (u32)0x0) return;
    r0 = r30 & 0xFFFF;
    if (r0 > (u32)0x59) return;
    r4 = (u32)jumptable_80375670;
    r0 = r0 << 2;
    r4 = (u32)jumptable_80375670;
    r0 = *(u32*)(r4 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    r4 = r31 & 0xFF;
    fightTrainerSet00();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerSet06();
    return;
    r4 = r31;
    fightTrainerSet08();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerDataBiosSetKindDataId();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerSet04();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFFFF;
    fightTrainerSetU16Arr14();
    return;
    r4 = r31;
    fightTrainerSet0C();
    return;
    r5 = r31;
    r4 = r29 & 0xFF;
    fightTrainerSet24Arr();
    return;
    r4 = r31;
    fightTrainerSet10();
    return;
    r4 = r31;
    fightTrainerPokemonPartDataBiosSetName();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFF;
    fightTrainerPokemonPartDataBiosSetWazaTypeRevise();
    return;
    r4 = r31;
    fightTrainerPokemonDataBiosSetNickname();
    return;
    r4 = r29 & 0xFF;
    r5 = (s8)r31;
    fightTrainerPokemonDataBiosSetStatusRnd();
    return;
    r4 = r29 & 0xFF;
    r5 = (s16)r31;
    fightTrainerPokemonDataBiosSetStatusEffort();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetLevel();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerPokemonDataBiosSetItemBallId();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetDarkPokemonFlag();
    return;
    r4 = (s8)r31;
    fightTrainerPokemonDataBiosSetTokuseiFlag();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainerPokemonDataBiosSetPokemonDataId();
    return;
    r4 = r31;
    fightTrainerPokemonDataBiosSetItemDataId();
    return;
    r5 = r31;
    r4 = r29 & 0xFF;
    fightTrainerPokemonDataBiosSetWazaDataId();
    return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetPpCnt();
    return;
    r4 = (s16)r31;
    fightTrainerPokemonDataBiosSetFriend();
    return;
    r4 = (s8)r31;
    fightTrainerPokemonDataBiosSetSexDataId();
    return;
    r4 = (s8)r31;
    fightTrainerPokemonDataBiosSetSeikakuDataId();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetKeyPlayerFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerPokemonDataBiosSetPartDataId();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonSelectRandomFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonSelectWeakPointFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonDataOrderOutFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonDataOrderAceBossFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetKeyPlayerFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPokemonJoutaiFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetComboValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetIrekaeValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetItemValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetParamExpectFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetParamStoreFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetZokuseiCheckFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetTokuseiCheckFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetNokoriHpValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaDamageFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetAbicntMaxValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetAbicntMinValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetDefensePokemonRndSelectFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaRndSelectFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaInitValueFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetPartFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaHitFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaAvgValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaRiskFlag();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaNokoriPpValue();
    return;
    r4 = r31 & 0xFF;
    fightTrainerAiDataBiosSetLastValueRevise();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetZokuseiReviseZokuseiDataId();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetZokuseiReviseValue();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaTypeReviseTypeDataId();
    return;
    r4 = r29;
    r5 = r31 & 0xFF;
    fightTrainerAiDataBiosSetWazaTypeReviseValue();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetValue();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetName();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetKoudouName();
    return;
    r4 = r31;
    fightTrainerAiValueAddsubDataBiosSetPrefixName();
    return;
    r4 = r31 & 0xFFFF;
    fightTrainer_SetFightTrainerDataId();
    return;
    r4 = r31 & 0xFF;
    fightTrainer_SetOkaneBai();
    return;
    r4 = r31;
    fightTrainer_SetKoban();
    return;
    r4 = r31 & 0xFF;
    fightTrainer_SetNigeruCount();
    return;
    r4 = r31 & 0xFF;
    fightTrainer_SetControllerId();
    return;
    r4 = r31;
    fightTrainer_SetSequencePtr();
    return;
    r30 = (s16)r28;
    if (r0 >= (u32)0x59) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r29 & 0xFF;
    r5 = r31 & 0xFFFF;
    fn_801FBDF4();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFFFF;
    fightTrainerEnemyPokemonBiosSetTokuseiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFFFF;
    fightTrainerEnemyPokemonBiosSetStoreTokuseiData();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    r4 = r31 & 0xFF;
    fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag();
    return;
    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 == (u32)0x0) return;
    r5 = (s16)r31;
    r4 = 0xc;
    fightTrainerEnemyPokemonRegistAry();
    return;
    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 == (u32)0x0) return;
    r5 = (s16)r31;
    r4 = 0xc;
    fightTrainerEnemyPokemonEraseAry();
    return;
    r30 = (s16)r28;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r30;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) return;
    fightTrainerEnemyPokemonInitFightOutStatus();

    return;
}

/* 0x801FB1C0 | size: 0x738 | large */
void fightTrainerGetStatus(void) {
    extern void fightTrainerGetStatus();
    extern void fightTrainerEnemyPokemonBiosGetParam1bantakaiFlag();
    extern void fightTrainerEnemyPokemonBiosGetBadwazaHaveFlag();
    extern void fightTrainerEnemyPokemonBiosGetDefense1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosGetLv1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosGetNowhp1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosGetStoreTokuseiData();
    extern void fightTrainerEnemyPokemonBiosGetTokuseiFlag();
    extern void fightTrainerGetU16Arr();
    extern void fightTrainerAiValueAddsubDataBiosGetPrefixName();
    extern void fightTrainerAiValueAddsubDataBiosGetKoudouName();
    extern void fightTrainerAiValueAddsubDataBiosGetName();
    extern void fightTrainerAiValueAddsubDataBiosGetValue();
    extern void fightTrainerAiDataBiosGetLastValueRevise();
    extern void fightTrainerAiDataBiosGetWazaNokoriPpValue();
    extern void fightTrainerAiDataBiosGetWazaRiskFlag();
    extern void fightTrainerAiDataBiosGetWazaAvgValue();
    extern void fightTrainerAiDataBiosGetWazaHitFlag();
    extern void fightTrainerAiDataBiosGetWazaTypeReviseValue();
    extern void fightTrainerAiDataBiosGetWazaTypeReviseTypeDataId();
    extern void fightTrainerAiDataBiosGetPartFlag();
    extern void fightTrainerAiDataBiosGetWazaInitValueFlag();
    extern void fightTrainerAiDataBiosGetWazaRndSelectFlag();
    extern void fightTrainerAiDataBiosGetDefensePokemonRndSelectFlag();
    extern void fightTrainerAiDataBiosGetAbicntMinValue();
    extern void fightTrainerAiDataBiosGetAbicntMaxValue();
    extern void fightTrainerAiDataBiosGetWazaDamageFlag();
    extern void fightTrainerAiDataBiosGetNokoriHpValue();
    extern void fightTrainerAiDataBiosGetTokuseiCheckFlag();
    extern void fightTrainerAiDataBiosGetZokuseiReviseValue();
    extern void fightTrainerAiDataBiosGetZokuseiReviseZokuseiDataId();
    extern void fightTrainerAiDataBiosGetZokuseiCheckFlag();
    extern void fightTrainerAiDataBiosGetParamStoreFlag();
    extern void fightTrainerAiDataBiosGetParamExpectFlag();
    extern void fightTrainerAiDataBiosGetItemValue();
    extern void fightTrainerAiDataBiosGetIrekaeValue();
    extern void fightTrainerAiDataBiosGetComboValue();
    extern void fightTrainerAiDataBiosGetPokemonJoutaiFlag();
    extern void fightTrainerAiDataBiosGetKeyPlayerFlag();
    extern void fightTrainerAiDataBiosGetPokemonDataOrderAceBossFlag();
    extern void fightTrainerAiDataBiosGetPokemonDataOrderOutFlag();
    extern void fightTrainerAiDataBiosGetPokemonSelectWeakPointFlag();
    extern void fightTrainerAiDataBiosGetPokemonSelectRandomFlag();
    extern void fightTrainerPokemonDataBiosGetPartDataId();
    extern void fightTrainerPokemonDataBiosGetKeyPlayerFlag();
    extern void fightTrainerPokemonDataBiosGetSeikakuDataId();
    extern void fightTrainerPokemonDataBiosGetSexDataId();
    extern void fightTrainerPokemonDataBiosGetFriend();
    extern void fightTrainerPokemonDataBiosGetPpCnt();
    extern void fightTrainerPokemonDataBiosGetWazaDataId();
    extern void fightTrainerPokemonDataBiosGetItemDataId();
    extern void fightTrainerPokemonDataBiosGetPokemonDataId();
    extern void fightTrainerPokemonDataBiosGetTokuseiFlag();
    extern void fightTrainerPokemonDataBiosGetDarkPokemonFlag();
    extern void fightTrainerPokemonDataBiosGetItemBallId();
    extern void fightTrainerPokemonDataBiosGetLevel();
    extern void fightTrainerPokemonDataBiosGetStatusEffort();
    extern void fightTrainerPokemonDataBiosGetStatusRnd();
    extern void fightTrainerPokemonDataBiosGetNickname();
    extern void fightTrainerPokemonPartDataBiosGetWazaTypeRevise();
    extern void fightTrainerPokemonPartDataBiosGetName();
    extern void fn_801FCBA4();
    extern void fn_801FCBBC();
    extern void fn_801FCBF0();
    extern void fn_801FCC08();
    extern void fn_801FCC3C();
    extern void fightTrainerDataBiosGetKindDataId();
    extern void fn_801FCC7C();
    extern void fn_801FCC94();
    extern void fn_801FCCAC();
    extern void fightTrainer_GetFightActionBuffPtr();
    extern void fightTrainer_GetFightTrainerEnemyPokemonAryPtr();
    extern void fightTrainer_GetSequencePtr();
    extern void fightTrainer_GetControllerId();
    extern void fightTrainer_GetNigeruCount();
    extern void fightTrainer_GetKoban();
    extern void fightTrainer_GetOkaneBai();
    extern void fightTrainer_GetFightoutPokemonBuffPtr();
    extern void fightTrainer_GetFightoutPokemonPtr();
    extern void fightTrainer_GetFightPokemonPtr();
    extern void fightTrainer_GetFightTrainerDataId();
    extern void fightTrainer_GetHeroPtr();
    extern void fightTrainerEnemyPokemonSearchAry();
    extern u8 jumptable_803757D8[];
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r1 = (u32)sp;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r0 = r5 & 0xFFFF;
    r31 = r5;
    r29 = r4;
    r30 = r6;
    if ((s32)r0 == (s32)0) { r3 = 0x0; return; }
    if (r0 >= (u32)0x5b) {

        r3 = 0x0;
        return;
    }
    if (r0 < (u32)0xa) {
        r3 = r29;
        ((void(*)(void))fightTrainerDataBiosGetPtr)();
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            return;
        }
        if (r0 < (u32)0xd) {
            r3 = r29;
            ((void(*)(void))fightTrainerPokemonPartDataBiosGetPtr)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                return;
            }
            if (r0 < (u32)0x1e) {
                r3 = r29;
                ((void(*)(void))fightTrainerPokemonDataBiosGetPtr)();
                if (r3 == (u32)0x0) {
                    r3 = 0x0;
                    return;
                }
                if (r0 < (u32)0x3d) {
                    r3 = r29;
                    ((void(*)(void))fightTrainerAiDataBiosGetPtr)();
        }
        }
        }
        if (r3 == (u32)0x0) {
            r3 = 0x0;
            return;
        }
        if (r0 < (u32)0x42) {
            r3 = r29;
            ((void(*)(void))fightTrainerAiValueAddsubDataBiosGetPtr)();
            if (r3 == (u32)0x0) {
                r3 = 0x0;
                return;
        }
        }
        }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r0 = r31 & 0xFFFF;
    if (r0 > (u32)0x56) { r3 = 0x0; return; }
    r4 = (u32)jumptable_803757D8;
    r0 = r0 << 2;
    r4 = (u32)jumptable_803757D8;
    r0 = *(u32*)(r4 + r0);
    ctr_fn = (void(*)(void))r0;
    /* indirect jump via ctr */;
    fn_801FCCAC();
    r3 = r3 & 0xFF;
    return;
    fn_801FCC94();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCC7C();
    return;
    fightTrainerDataBiosGetKindDataId();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCC3C();
    r3 = r3 & 0xFFFF;
    return;
    r4 = r30 & 0xFF;
    fn_801FCC08();
    r3 = r3 & 0xFFFF;
    return;
    fn_801FCBF0();
    return;
    r4 = r30 & 0xFF;
    fn_801FCBBC();
    return;
    fn_801FCBA4();
    return;
    fightTrainerPokemonPartDataBiosGetName();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonPartDataBiosGetWazaTypeRevise();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetNickname();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetStatusRnd();
    r3 = (s8)r3;
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetStatusEffort();
    r3 = (s16)r3;
    return;
    fightTrainerPokemonDataBiosGetLevel();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetItemBallId();
    r3 = r3 & 0xFFFF;
    return;
    fightTrainerPokemonDataBiosGetDarkPokemonFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetTokuseiFlag();
    r3 = (s8)r3;
    return;
    fightTrainerPokemonDataBiosGetPokemonDataId();
    r3 = r3 & 0xFFFF;
    return;
    fightTrainerPokemonDataBiosGetItemDataId();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetWazaDataId();
    return;
    r4 = r30 & 0xFF;
    fightTrainerPokemonDataBiosGetPpCnt();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetFriend();
    r3 = (s16)r3;
    return;
    fightTrainerPokemonDataBiosGetSexDataId();
    r3 = (s8)r3;
    return;
    fightTrainerPokemonDataBiosGetSeikakuDataId();
    r3 = (s8)r3;
    return;
    fightTrainerPokemonDataBiosGetKeyPlayerFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerPokemonDataBiosGetPartDataId();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonSelectRandomFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonSelectWeakPointFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonDataOrderOutFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonDataOrderAceBossFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetKeyPlayerFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPokemonJoutaiFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetComboValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetIrekaeValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetItemValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetParamExpectFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetParamStoreFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetZokuseiCheckFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetTokuseiCheckFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetNokoriHpValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaDamageFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetAbicntMaxValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetAbicntMinValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetDefensePokemonRndSelectFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaRndSelectFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaInitValueFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetPartFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaHitFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaAvgValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaRiskFlag();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetWazaNokoriPpValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiDataBiosGetLastValueRevise();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetZokuseiReviseZokuseiDataId();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetZokuseiReviseValue();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetWazaTypeReviseTypeDataId();
    r3 = r3 & 0xFF;
    return;
    r4 = r30;
    fightTrainerAiDataBiosGetWazaTypeReviseValue();
    r3 = r3 & 0xFF;
    return;
    fightTrainerAiValueAddsubDataBiosGetValue();
    return;
    fightTrainerAiValueAddsubDataBiosGetName();
    return;
    fightTrainerAiValueAddsubDataBiosGetKoudouName();
    return;
    fightTrainerAiValueAddsubDataBiosGetPrefixName();
    return;
    fightTrainer_GetHeroPtr();
    return;
    fightTrainer_GetFightTrainerDataId();
    r3 = r3 & 0xFFFF;
    return;
    r4 = r30;
    fightTrainer_GetFightPokemonPtr();
    return;
    r4 = r30;
    fightTrainer_GetFightoutPokemonPtr();
    return;
    fightTrainer_GetFightoutPokemonBuffPtr();
    return;
    fightTrainer_GetOkaneBai();
    r3 = r3 & 0xFF;
    return;
    fightTrainer_GetKoban();
    return;
    fightTrainer_GetNigeruCount();
    r3 = r3 & 0xFF;
    return;
    fightTrainer_GetControllerId();
    r3 = r3 & 0xFF;
    return;
    fightTrainer_GetFightActionBuffPtr();
    return;
    fightTrainer_GetSequencePtr();
    return;
    r4 = r30 & 0xFF;
    fightTrainer_GetFightTrainerEnemyPokemonAryPtr();
    return;
    r31 = (s16)r29;
    if (r0 >= (u32)0x56) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    r4 = r30 & 0xFF;
    fightTrainerGetU16Arr();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetTokuseiFlag();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetStoreTokuseiData();
    r3 = r3 & 0xFFFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetNowhp1banhikuiFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetLv1banhikuiFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetDefense1banhikuiFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetBadwazaHaveFlag();
    r3 = r3 & 0xFF;
    return;
    r31 = (s16)r29;
    if (r3 >= (u32)0x0) {

    r4 = 0x0;
    r5 = 0x4e;
    r6 = 0x0;
    fightTrainerGetStatus();
    if (r3 != (u32)0x0) {

        r5 = r31;
        r4 = 0xc;
        fightTrainerEnemyPokemonSearchAry();
    }
    }
    if (r3 == (u32)0x0) {
        r3 = 0x0;
        return;
    }
    fightTrainerEnemyPokemonBiosGetParam1bantakaiFlag();
    r3 = r3 & 0xFF;
    return;

    r3 = 0x0;

    return;
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE) || \
    defined(FTR_EXACT_801FB8F8_801FBAD4)
/* 0x801FB8F8 | size: 0x7C */
u8 fightTrainerIsGcHero(void* arg0) {
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    u32 val;
    u32 category;

    val = (u16)fightTrainerGetStatus(arg0, 0, 0x43, 0);
    val = (u16)fightTrainerGetStatus(0, val, 4, 0);
    if (val == 1) {
        category = 0;
    } else if (val == 2 || val == 3) {
        category = 1;
    } else {
        category = 2;
    }
    return (u8)category == 0;
}

/* 0x801FB974 | size: 0xB0 */
void fightTrainerWazaSeq57(void* context, u8 mode) {
    extern void _threadSwitch(void);
    extern void wazaSequenceSysStopWaza(void* ptr, u32 field, u32 size);
    extern u8 wazaSequenceSysIsPlayingWaza(void* ptr, u32 field, u32 size);
    extern void wazaSequenceSysStartWaza(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    if (mode == 0) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 1) {
        wazaSequenceSysStartWaza(data, 0x57, 4);
    } else if (mode == 2) {
        do {
            if ((u8)wazaSequenceSysIsPlayingWaza(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        wazaSequenceSysStopWaza(data, 0x57, 4);
    }
}

/* 0x801FBA24 | size: 0xB0 */
void fightTrainerWazaSeq74(void* context, u8 mode) {
    extern void _threadSwitch(void);
    extern void wazaSequenceSysStopWaza(void* ptr, u32 field, u32 size);
    extern u8 wazaSequenceSysIsPlayingWaza(void* ptr, u32 field, u32 size);
    extern void wazaSequenceSysStartWaza(void* ptr, u32 field, u32 size);
    extern void fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    if (mode == 0) {
        fn_801DDD28(data, 0x74, 4, 0);
    } else if (mode == 1) {
        wazaSequenceSysStartWaza(data, 0x74, 4);
    } else if (mode == 2) {
        do {
            if ((u8)wazaSequenceSysIsPlayingWaza(data, 0x74, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        wazaSequenceSysStopWaza(data, 0x74, 4);
    }
}

#endif

#if !defined(FTR_BANK_EXACT_ACTIVE)
/* 0x801FBAD4 | size: 0x14C */
void fightTrainerHokakuThrowEffect(void* context, u32 param, u8 mode) {
    extern void _threadSwitch(void);
    extern void wazaSequenceSysStopWaza(void* ptr, u16 field, u32 size);
    extern u8 wazaSequenceSysIsPlayingWaza(void* ptr, u16 field, u32 size);
    extern void wazaSequenceSysStartWaza(void* ptr, u16 field, u32 size);
    extern void fn_801DDD28(void* ptr, u16 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    void* data;
    u32 animId;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    animId = itemGetStatus(0, param, 0x12, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        wazaSequenceSysStartWaza(data, animId, 4);
        do {
            if ((u8)wazaSequenceSysIsPlayingWaza(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 2) {
        wazaSequenceSysStopWaza(data, animId, 4);
    } else if (mode == 3) {
        fn_801DDD28(data, 0x57, 4, 0);
    } else if (mode == 4) {
        wazaSequenceSysStartWaza(data, 0x57, 4);
    } else if (mode == 5) {
        do {
            if ((u8)wazaSequenceSysIsPlayingWaza(data, 0x57, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 6) {
        wazaSequenceSysStopWaza(data, 0x57, 4);
    }
}

/* 0x801FBC20 | size: 0xF0 */
void fightTrainerBallThrowEffect(void* context, void* trainerCtx, u8 mode) {
    extern void _threadSwitch(void);
    extern void wazaSequenceSysStopWaza(void* ptr, u16 field, u32 size);
    extern u8 wazaSequenceSysIsPlayingWaza(void* ptr, u16 field, u32 size);
    extern void wazaSequenceSysStartWaza(void* ptr, u16 field, u32 size);
    extern void fn_801DDD28(void* ptr, u16 field, u32 size, u32 flags);
    extern void* fightTrainerGetStatus(void* ctx, u32 slot, u32 field, u32 idx);
    extern void* fightOutPokemonGetPokemonPtr(void* ctx);
    void* data;
    u8 typeId;
    u32 animId;

    if ((data = fightTrainerGetStatus(context, 0, 0x4C, 0)) == NULL) {
        return;
    }
    typeId = (u8)(u32)pokemonGetStatus(fightOutPokemonGetPokemonPtr(trainerCtx), 0, 0x73, 0);
    animId = itemGetStatus(0, typeId, 0x11, 0);
    if (mode == 0) {
        fn_801DDD28(data, animId, 4, 0);
    } else if (mode == 1) {
        wazaSequenceSysStartWaza(data, animId, 4);
    } else if (mode == 2) {
        do {
            if ((u8)wazaSequenceSysIsPlayingWaza(data, animId, 4) == 0) {
                return;
            }
            _threadSwitch();
        } while (1);
    } else if (mode == 3) {
        wazaSequenceSysStopWaza(data, animId, 4);
    }
}
#endif

#undef FTR_BANK_EXACT_ACTIVE
