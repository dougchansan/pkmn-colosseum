/**
 * @file pda_exact_8003D4C8.c
 * @brief PDA memo list rebuild with the colour-tab filter, 0x8003D4C8 - 0x8003D818.
 *        Standalone copy of fn_8003D4C8 (and the helpers it inlines) from
 *        pda_range_80037158.c, linked on its own.
 */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
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
extern u8 pokemonDataBiosGetColor(void* data, s32 index);

/* Memo id table loader (also inlined by fn_8003D818). */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D818_LoadIds in this standalone copy (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003D818_LoadIds(void)
{
    extern u16* lbl_8047A4E4;
    extern u16 memoDataGetPokemonID(u16* data, u32 index);
    extern u16 memoDataGetCount(u16* data);
    u32 index;
    u16 count;

    count = 0;
    for (index = 0; (u16)index < memoDataGetCount(0); index++) {
        lbl_8047A4E4[(u16)index] = memoDataGetPokemonID(0, index);
        count++;
    }
    return count;
}

/* Whether a memo entry ID has been seen (the high bit marks an unseen entry). */
static inline BOOL pdaMemoSeen(u16 id)
{
    return (id & 0x8000) == 0;
}

/* pdaLoadPokemonId for the lookup helpers, which go on to read the work slot. */
static inline u32 pdaLoadPokemonWork(u32 id)
{
    u32 rnd;
    u32 species = id;
    u32 trainerRnd;
    u32 work;

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

/* Color variant of the memo entry; bails out before loading when no work slot exists. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D4C8_Color (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u8 pda_8003D4C8_Color(u32 id)
{
    u32 work = lbl_8047A4E0;
    void* data;

    if (work != 0) {
        work = pdaLoadPokemonWork(id);
        if (work != 0) {
            data = pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(work));
            if (data != NULL) {
                return pokemonDataBiosGetColor(data, 0);
            }
        } else {
            return 0;
        }
    }
    return 0;
}

/* pdaLoadPokemonId for the colour filter: tests the work slot, then reads it again into work. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D4C8_Load (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u32 pda_8003D4C8_Load(u32 id)
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
        trainerRnd = memoDataGetPokemonTrainerRndFromID(0, species);
        pokemonBiosSetRnd(work, rnd);
        pokemonBiosSetCatchTrainerRnd(work, trainerRnd);
        return lbl_8047A4E0;
    }
    return 0;
}

/* Copy the seen entries whose colour matches the active colour tab into buf; returns the count. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D4C8_Filter (fn_8003D4C8) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003D4C8_Filter(u16* buf)
{
    u16 count;
    u16 i;
    u32 id;
    u8 color;

    count = 0;
    for (i = 0; i < lbl_8047A4E8; i++) {
        id = lbl_8047A4E4[i];
        if (pdaMemoSeen(id)) {
            pda_8003D4C8_Load(id);
            color = pda_8003D4C8_Color(id);
            if (color == *(s8*)((u8*)&lbl_803A6818 + 0x159)) {
                buf[count] = id;
                count++;
            }
        }
    }
    return count;
}

/* Rebuild the memo index table, optionally filtered by the active color tab. */
u16 pdaMemoLoadPokemon(void)
{
    extern void pdaMemoBuildNameIndex(void);
    extern void pdaMemoBuildTypeIndex(void);
    extern void pdaMemoDrawData(void);
    u16 total;
    u16 buf[500];
    u16 i;
    u16 count;

    lbl_8047A4E8 = pda_8003D818_LoadIds();
    pdaMemoBuildNameIndex();
    if (*(s8*)((u8*)&lbl_803A6818 + 0x159) != 0) {
        count = pda_8003D4C8_Filter(buf);
        for (i = 0; i < count; i++) {
            lbl_8047A4E4[i] = buf[i];
        }
        lbl_8047A4E8 = count;
    }
    pdaMemoBuildTypeIndex();
    pdaMemoDrawData();
    total = lbl_8047A4E8;
    *(s32*)((u8*)&lbl_803A6818 + 0x8) = -5;
    if (total >= 5) {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = 5;
    } else {
        *(s32*)((u8*)&lbl_803A6818 + 0xc) = total;
    }
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
    *(s32*)((u8*)&lbl_803A6818 + 0x10) = total;
    return total;
}
