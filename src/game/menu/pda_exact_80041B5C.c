/**
 * @file pda_exact_80041B5C.c
 * @brief PDA people-screen sprite callbacks, 0x80041B5C - 0x80041E48:
 *        pdaPeopleUpdateAlpha (alpha update and event filter) and pdaPeopleDrawTypeIcons (the
 *        highlighted Pokemon's type icons). Copied from pda_range_80037158.c.
 */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;

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
} PdaSceneWork;

typedef struct PdaEvent {
    u8 pad00[0x6];
    s16 messageId;
} PdaEvent;

extern u8 lbl_802EF0A8[];
extern PdaSceneWork lbl_803A6818;
extern u32 lbl_8047A4E0;
extern u16* lbl_8047A4E4;
extern f32 lbl_8047BCA0;
extern u32 gamedataGetStatus(s32 a, s32 b);
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern u32 memoDataGetPokemonRndFromID(s32 a, u32 id);
extern u32 memoDataGetPokemonTrainerRndFromID(s32 a, u32 id);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetCatchTrainerRnd(u32 work, u32 rnd);
extern u32 pokemonGetStatus(u32 a, u32 b, s32 id, s32 index);
extern void windowDrawSprite(s16 x, s16 y, PdaSprite* sprite, u16 id, s32 arg4);

void pdaPeopleUpdateAlpha(PdaSprite* sprite, PdaEvent* event)
{
    extern f32 lbl_8047BCA0;
    extern void pdaMemoDrawPokemonDetail(PdaSprite* sprite, PdaEvent* event);

    sprite->alphaByte = lbl_8047BCA0 * *(f32*)((u8*)&lbl_803A6818 + 0x4c);
    switch (event->messageId) {
    case 0x331:
    case 0x759:
    case 0x76a:
    case 0xfbe:
        break;
    default:
        pdaMemoDrawPokemonDetail(sprite, event);
        break;
    }
}

/* Reload the scratch Pokemon slot from a memo entry ID. */
static inline u32 pdaLoadPokemon(s32 index)
{
    u32 work = lbl_8047A4E0;
    u32 rnd;
    u32 species;
    u32 trainerRnd;

    if (work != 0) {
        species = lbl_8047A4E4[index];
        if (species >= 0x8000) {
            species = species & 0x3fff;
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

/* People-screen: draw the currently highlighted Pokemon's type icons. */
void pdaPeopleDrawTypeIcons(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern u16 lbl_802E554C[];
    s32 pokemon;
    u16 type0;
    u16 type1;
    u8 seen;

    pokemon = pdaLoadPokemon(lbl_803A6818.currentIndex);

    alphaSprite->alphaByte = lbl_8047BCA0 * lbl_803A6818.alphaScale;
    seen = (lbl_8047A4E4[lbl_803A6818.currentIndex] & 0x8000) ? 0 : 1;
    if (seen != 0) {
        type0 = pokemonGetStatus(
            0, (u16)pokemonGetStatus(pokemon, 0, 0x6e, 0), 0x16, 0);
        windowDrawSprite(
            (s16)(*(s16*)(lbl_802EF0A8 + 0x5996) - sprite->field_50),
            (s16)(*(s16*)(lbl_802EF0A8 + 0x5998) - sprite->field_52),
            alphaSprite, lbl_802E554C[type0], 0);
        type1 = pokemonGetStatus(
            0, (u16)pokemonGetStatus(pokemon, 0, 0x6e, 0), 0x16, 1);
        if (type0 != type1) {
            windowDrawSprite(
                (s16)(*(s16*)(lbl_802EF0A8 + 0x59b2) - sprite->field_50),
                (s16)(*(s16*)(lbl_802EF0A8 + 0x59b4) - sprite->field_52),
                alphaSprite, lbl_802E554C[type1], 0);
        }
    } else {
        windowDrawSprite((s16)(*(s16*)(lbl_802EF0A8 + 0x5996) - sprite->field_50),
                         (s16)(*(s16*)(lbl_802EF0A8 + 0x5998) - sprite->field_52),
                         alphaSprite, 0x5d, 0);
        windowDrawSprite((s16)(*(s16*)(lbl_802EF0A8 + 0x59b2) - sprite->field_50),
                         (s16)(*(s16*)(lbl_802EF0A8 + 0x59b4) - sprite->field_52),
                         alphaSprite, 0x5d, 0);
    }
}
