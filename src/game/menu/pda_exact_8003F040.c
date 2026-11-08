/**
 * @file pda_exact_8003F040.c
 * @brief PDA memo list sort by species name, 0x8003F040 - 0x8003F2DC.
 *        Standalone copy of pdaMemoSortByName (and the helpers it inlines) from
 *        pda_range_80037158.c, linked on its own.
 */
#include "dolphin/types.h"

extern u32 GSmemGetHandleByPtr(void* p);
extern void GSmemFree(u32 handle);
extern void GSmemUnlock(u32 handle);
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

/* As pdaLoadPokemon, but from an entry ID the caller already has in hand. */
static inline u32 pdaLoadPokemonId(u32 id)
{
    u32 work;
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;

    if ((work = lbl_8047A4E0) != 0) {
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Name of the memo entry as a wide string, or NULL when unavailable. */
static inline void* pdaGetPokemonName(u16 id)
{
    u32 work = pdaLoadPokemonId(id);

    if (work != 0) {
        return pokemonDataBiosGetName(
            pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work)));
    }
    return NULL;
}


static inline void* pdaAlloc(s32 size, s32 align)
{
    u32 h = GSmemAlloc(size, align);

    if ((u16)h != 0) {
        return GSmemLock(h);
    }
    return NULL;
}

static inline void pdaFree(void* p)
{
    u32 h;

    if (p != NULL) {
        h = GSmemGetHandleByPtr(p);
        if ((u16)h != 0) {
            GSmemUnlock(h);
            GSmemFree(h);
        }
    }
}

typedef struct PdaNamePair {
    u16 index;
    u16 name[12];
} PdaNamePair;

/* Shell sort of the name pairs by name (array indexing; the compiler derives the row pointers). */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003F040_Sort (pdaMemoSortByName) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003F040_Sort(PdaNamePair* pairs, s32 count)
{
    extern s32 GScharCmp(u16* a, u16* b);
    extern void* memcpy(void* dst, const void* src, u32 size);
    PdaNamePair tmp;
    s32 gap;
    s32 j;
    s32 k;

    for (gap = count / 2; gap > 0; gap /= 2) {
        for (j = gap; j < count; j++) {
            for (k = j - gap; k >= 0 && GScharCmp(pairs[k].name, pairs[k + gap].name) >= 0; k -= gap) {
                memcpy(&tmp, &pairs[k], 0x1a);
                memcpy(&pairs[k], &pairs[k + gap], 0x1a);
                memcpy(&pairs[k + gap], &tmp, 0x1a);
            }
        }
    }
}

/* Sort the memo index table by species name. */
void pdaMemoSortByName(void)
{
    u16 i;
    PdaNamePair* pairs;
    u16* ids;

    pairs = (PdaNamePair*)pdaAlloc(0x28a0, 0x20);
    ids = (u16*)pdaAlloc(0x320, 0x20);
    for (i = 0; i < lbl_8047A4E8; i++) {
        u32 id = lbl_8047A4E4[i];
        ids[i] = id;
        GSmsgGetText((u8*)pairs[i].name, 0x18, (u8*)pdaGetPokemonName(id));
        pairs[i].index = i;
    }
    pda_8003F040_Sort(pairs, lbl_8047A4E8);
    for (i = 0; i < lbl_8047A4E8; i++) {
        lbl_8047A4E4[i] = ids[pairs[i].index];
    }
    pdaFree(pairs);
    pdaFree(ids);
}
