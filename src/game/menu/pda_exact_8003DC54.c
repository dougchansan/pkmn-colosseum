/**
 * @file pda_exact_8003DC54.c
 * @brief PDA memo list filter by the active type tabs, 0x8003DC54 - 0x8003E394.
 *        Standalone copy of pdaMemoBuildTypeIndex (and the helpers it inlines) from
 *        pda_range_80037158.c, linked on its own.
 */
#include "dolphin/types.h"

extern u32 GSmemGetHandleByPtr(void* p);
extern void GSmemFree(u32 handle);
extern void fn_800E24B0(u32 handle);
extern void* GSmemLock(u32 handle);
extern u32 GSmemAlloc(s32 size, s32 align);
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern u16 lbl_8047A4E8;

extern u32 gamedataGetStatus(s32 a, s32 b);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u16 pokemonBiosGetPokemonDataId(u32 work);
extern void* pokemonDataBiosGetPtr(u16 id);
extern void* pokemonDataBiosGetName(void* data);
extern s32 GSmsgGetText(u8* dst, s32 size, u8* src);

extern u32 pokemonGetStatus(u32 a, u32 b, s32 id, s32 index);

typedef struct PdaSceneWork {
    u8 pad00[0x15a];
    s8 typeTab0;
    s8 typeTab1;
} PdaSceneWork;
extern PdaSceneWork lbl_803A6818;

/* Whether a memo entry ID has been seen (the high bit marks an unseen entry). */
static inline BOOL pdaMemoSeen(u16 id)
{
    return (id & 0x8000) == 0;
}

/* Catch-trainer random value of a memo entry's species (single-type filter loader). */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003DC54_TrainerRnd (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003DC54_TrainerRnd(u32 species)
{
    u32 trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);

    return trainerRnd;
}

/* pdaLoadPokemonId for the single-type filter: tests the work slot, then reads it again into work. */
static inline u32 pdaLoadPokemonSlot(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;

    if (lbl_8047A4E0 != 0) {
        work = lbl_8047A4E0;
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = pda_8003DC54_TrainerRnd(species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Icon index of one of a memo entry's two types. */
static inline u16 pdaGetTypeIcon(u32 id, s32 slot)
{
    u32 type = (u16)pokemonGetStatus(
        0, (u16)pokemonGetStatus(pdaLoadPokemonSlot(id), 0, 0x6e, 0), 0x16, slot);

    if (type >= 9) {
        type = type - 1;
    }
    return type + 1;
}

/* Random value of a memo entry's species (both-types filter loader). */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003DC54_Rnd (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003DC54_Rnd(u32 species)
{
    u32 rnd = memoDataGetPokemonRndFromID(0, species);

    return rnd;
}

/* pdaLoadPokemonId for the both-types filter (its rnd read goes through pda_8003DC54_Rnd). */
/* RULE-EXCEPTION(user-approved): single-use inline helper copy pda_8003DC54_LoadBoth for the both-types filter (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003DC54_LoadBoth(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;

    if (lbl_8047A4E0 != 0) {
        work = lbl_8047A4E0;
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = pda_8003DC54_Rnd(species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* pdaLoadPokemonId for the both-types filter's first load, whose result is unused; the trainer
 * argument is read into a local before pokemonCreate. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003DC54_LoadSlot (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003DC54_LoadSlot(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;
    u32 trainer;

    if (lbl_8047A4E0 != 0) {
        work = lbl_8047A4E0;
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        trainer = gamedataGetStatus(0, 1);
        pokemonCreate(work, (u16)species, 10, trainer);
        rnd = pda_8003DC54_Rnd(species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
    }
}

/* pdaGetTypeIcon for the both-types filter. */
/* RULE-EXCEPTION(user-approved): single-use inline helper copy pda_8003DC54_TypeIconBoth for the both-types filter (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003DC54_TypeIconBoth(u32 id, s32 slot)
{
    u32 type = (u16)pokemonGetStatus(
        0, (u16)pokemonGetStatus(pda_8003DC54_LoadBoth(id), 0, 0x6e, 0), 0x16, slot);

    if (type >= 9) {
        type = type - 1;
    }
    return type + 1;
}

/* Copy the seen entries that have both tab types into buf; returns the count. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003DC54_FilterBoth (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003DC54_FilterBoth(u16* buf, u16 type0, u16 type1)
{
    u16 i;
    u32 id;
    u16 count;
    u16 a;
    u16 b;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            pda_8003DC54_LoadSlot(id);
            a = pda_8003DC54_TypeIconBoth(id, 0);
            b = pda_8003DC54_TypeIconBoth(id, 1);
            if ((type0 == a && type1 == b) || (type0 == b && type1 == a)) {
                buf[count++] = id;
            }
        }
    }
    return count;
}

/* Copy the seen entries that have the tab type into buf; returns the count. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pdaFilterTypes (pdaMemoBuildTypeIndex) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pdaFilterTypes(u16* buf, u16 type0)
{
    u16 i;
    u32 id;
    u16 count;
    u16 a;
    u16 b;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            pdaLoadPokemonSlot(id);
            a = pdaGetTypeIcon(id, 0);
            b = pdaGetTypeIcon(id, 1);
            if (type0 == a || type0 == b) {
                buf[count++] = id;
            }
        }
    }
    return count;
}

/* Filter the memo index table down to the active type tabs. */
void pdaMemoBuildTypeIndex(void)
{
    u8* S = (u8*)&lbl_803A6818;
    u16 buf1[500];
    u16 buf0[500];
    u16 type0;
    u16 type1;
    u16 single;
    u16 i;
    u16 count;

    if (*(s8*)(S + 0x15a) == 0 && *(s8*)(S + 0x15b) == 0) {
        return;
    }
    type0 = *(s8*)(S + 0x15a);
    type1 = *(s8*)(S + 0x15b);
    if (type0 != 0 && type1 != 0) {
        single = 0;
    } else {
        if (type0 == 0) {
            type0 = type1;
        }
        single = 1;
    }
    if (single == 0) {
        count = pda_8003DC54_FilterBoth(buf1, type0, type1);
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf1[i];
        }
        lbl_8047A4E8 = count;
    } else {
        count = pdaFilterTypes(buf0, type0);
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf0[i];
        }
        lbl_8047A4E8 = count;
    }
}
