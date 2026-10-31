/**
 * @file pda_exact_8003E394.c
 * @brief PDA memo list reorder for the active sort tab, 0x8003E394 - 0x8003F040.
 *        Standalone copy of pdaMemoDrawData (and the helpers it inlines) from
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

extern u16 pokemonDataBiosGetWeight(void* data);
extern u16 pokemonDataBiosGetHeight(void* data);
extern void pdaMemoSortPairs(u8* arr, s32 count, s32 dir);
void pdaMemoSortByName(void);

typedef struct PdaSceneWork {
    u8 pad00[0x15c];
    s8 sortTab;
} PdaSceneWork;
extern PdaSceneWork lbl_803A6818;

/* Whether a memo entry ID has been seen (the high bit marks an unseen entry). */
static inline BOOL pdaMemoSeen(u16 id)
{
    return (id & 0x8000) == 0;
}

static inline u32 pdaTrainerRnd(u32 species)
{
    u32 trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);

    return trainerRnd;
}

/* pdaLoadPokemonId for the lookup helpers, which go on to read the work slot. */
static inline u32 pdaLoadPokemonWork(u32 id)
{
    u32 trainerRnd;
    u32 rnd;
    u32 species = id;
    u32 work;

    if ((work = lbl_8047A4E0) != 0) {
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = memoDataGetPokemonRndFromID(0, species);
        trainerRnd = pdaTrainerRnd(species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003E394_Rnd (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003E394_Rnd(u32 species)
{
    u32 rnd = memoDataGetPokemonRndFromID(0, species);

    return rnd;
}

/* pdaLoadPokemonId for the lookup helpers, which go on to read the work slot. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003E394_LoadPokemonWork (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003E394_LoadPokemonWork(u32 id)
{
    u32 trainerRnd;
    u32 rnd;
    u32 species = id;
    u32 work;

    if ((work = lbl_8047A4E0) != 0) {
        if (id >= 0x8000) {
            species = id & 0x3fff;
        }
        pokemonCreate(work, (u16)species, 10, gamedataGetStatus(0, 1));
        rnd = pda_8003E394_Rnd(species);
        trainerRnd = pdaTrainerRnd(species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
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

typedef struct PdaSortPair {
    u16 index;
    u16 key;
} PdaSortPair;



static inline u16 pdaGetWeight(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetWeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003E394_GetWeight (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003E394_GetWeight(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pda_8003E394_LoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetWeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

static inline u16 pdaGetHeight(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetHeight(data);
            }
            return 0;
        }
        return 0;
    }
    return 0;
}

/* Fill the sort pairs with each seen entry's weight; returns the entry count. */
static inline u16 pdaCollectWeights(PdaSortPair* pairs, u16* ids)
{
    u16 i;
    u32 id;
    u16 count;
    u16 key;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            key = pdaGetWeight(id);
            ids[count] = id;
            pairs[count].key = key;
            pairs[count].index = i;
            count++;
        }
    }
    return count;
}

/* Fill the sort pairs with each seen entry's height; returns the entry count. */
static inline u16 pdaCollectHeights(PdaSortPair* pairs, u16* ids)
{
    u16 i;
    u32 id;
    u16 count;
    u16 key;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            key = pdaGetHeight(id);
            ids[count] = id;
            pairs[count].key = key;
            pairs[count].index = i;
            count++;
        }
    }
    return count;
}

/* Sort tab 1: collect the seen entries by weight, sort them and copy the order back
 * (the sort runs even for an empty list; i counts both loops). Returns the entry count. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003E394_SortPairs (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003E394_SortPairs(PdaSortPair* pairs, u16* ids)
{
    u16 i;
    u32 id;
    u16 count;
    u16 key;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            key = pda_8003E394_GetWeight(id);
            ids[count] = id;
            pairs[count].key = key;
            pairs[count].index = i;
            count++;
        }
    }
    pdaMemoSortPairs((u8*)pairs, count, 1);
    for (i = 0; i < count; i++) {
        lbl_8047A4E4[i] = ids[pairs[i].index];
    }
    return count;
}

/* Sort tab 1: order the seen entries by weight. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003E394_SortWeight (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003E394_SortWeight(void)
{
    u16 count;
    PdaSortPair* pairs;
    u16* ids;

    pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
    ids = (u16*)pdaAlloc(0x320, 0x20);
    count = pda_8003E394_SortPairs(pairs, ids);
    lbl_8047A4E8 = count;
    pdaFree(pairs);
    pdaFree(ids);
}

/* Reorder the memo index table for the active sort tab. */
void pdaMemoDrawData(void)
{
    u16 j;
    u16 count;
    /* RULE-EXCEPTION(user-approved): no-op store, NULL initialisers that every case overwrites (pdaMemoDrawData) - see docs/RULE_EXCEPTIONS.md */
    u16* ids = NULL;
    PdaSortPair* pairs = NULL;

    switch (lbl_803A6818.sortTab) {
    case 0:
        pdaMemoSortByName();
        break;
    case 1:
        pda_8003E394_SortWeight();
        break;
    case 2:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = pdaCollectWeights(pairs, ids);
        if (count != 0) {
            pdaMemoSortPairs((u8*)pairs, count, 0);
            for (j = 0; j < count; j++) {
                lbl_8047A4E4[j] = ids[pairs[j].index];
            }
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    case 3:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = pdaCollectHeights(pairs, ids);
        if (count != 0) {
            pdaMemoSortPairs((u8*)pairs, count, 1);
            for (j = 0; j < count; j++) {
                lbl_8047A4E4[j] = ids[pairs[j].index];
            }
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    case 4:
        pairs = (PdaSortPair*)pdaAlloc(0x640, 0x20);
        ids = (u16*)pdaAlloc(0x320, 0x20);
        count = pdaCollectHeights(pairs, ids);
        if (count != 0) {
            pdaMemoSortPairs((u8*)pairs, count, 0);
            for (j = 0; j < count; j++) {
                lbl_8047A4E4[j] = ids[pairs[j].index];
            }
        }
        lbl_8047A4E8 = count;
        pdaFree(pairs);
        pdaFree(ids);
        break;
    default:
        break;
    }
}
