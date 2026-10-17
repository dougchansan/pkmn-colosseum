/**
 * @file fight_trainer_range_801F7954.c
 * @brief Head of fightTrainer.cpp: menu ball status and item helpers.
 *
 * Split out of the former game/pokemon.c CodeCandidate bucket
 * (0x801F000C-0x801F7F80), which was mislabeled "pokemon" but is
 * entirely the XD-era fight-engine cluster. Address range covered by
 * this translation unit: 0x801F7954-0x801F7F80 (5 functions), per
 * config/GC6E01/splits.txt.
 */

#include "game/pokemon_fight_types.h"

static inline u32 fightTrainerCheckValidInline(u8* context) {
    extern u16 std103_battleGetResult(void* context);
    extern u8 heroCheckValid(void* hero);
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    u8* hero;

    if (context == NULL) {
        return 0;
    }
    if ((u16)std103_battleGetResult(context) == 1) {
        return 0;
    }
    if ((s32)fightTrainerGetStatus(context, 0, 0x43, 0) == 0) {
        return 0;
    }
    hero = fightTrainerGetStatus(context, 0, 0x44, 0);
    if (hero == NULL) {
        return 0;
    }
    if ((u8)heroCheckValid(hero) == 0) {
        return 0;
    }
    return 1;
}

static inline u8* fightTrainerFindPokemonInline(u8* context, u8* pokemon) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8 fightPokemonCheckValid(u8*);
    extern u32 pokemonGetStatus(u8*, u32, u16, u32);
    u32 i;
    u8* candidate;

    for (i = 0; (u16)i < 6; i++) {
        candidate = fightTrainerGetStatus(context, 0, 0x45, i);
        if ((u8)fightPokemonCheckValid(candidate) != 0) {
            u32 owner = pokemonGetStatus(candidate, 0, 0xCB, 0);
            if (owner != 0 && pokemon == (u8*)owner) {
                return candidate;
            }
        }
    }
    return NULL;
}

static inline void clearUsedItemIdsInline(u16* itemIds) {
    u32 index = 0;
    u16 empty = (u16)index;

    while ((u16)index < 2) {
        itemIds[(u16)index] = empty;
        index++;
    }
}

static inline u32 collectUsedItemIdsInline(u8* context, u16* itemIds) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8 fightOutPokemonCheckFightOut(u8*);
    extern u16 fightOutPokemonGetFightActionUseItemDataId(u8*);
    u32 count = 0;
    u32 index = count;

    while ((u16)index < 2) {
        u8* fightOut = fightTrainerGetStatus(context, 0, 0x46, index);
        if ((u8)fightOutPokemonCheckFightOut(fightOut) != 0) {
            u16 itemId = fightOutPokemonGetFightActionUseItemDataId(fightOut);
            if (itemId != 0) {
                itemIds[(u16)count] = itemId;
                count++;
            }
        }
        index++;
    }
    return count;
}

/* 0x801F7954 | size: 0x21C | large */
void fightTrainerToMenuBallStatus(u8* ptr, u8* arr) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8* heroGetStatus(u8*, u32, u16);
    extern u8 pokemonCheckValid(void);
    extern u32 pokemonGetStatus(u8*, u32, u16, u32);
    extern u8 fightPokemonCheckValid(u8*);
    extern u8 fightPokemonCheckFightOut(u8*);
    extern u8* fightPokemonGetPokemonPtr(u8*);
    extern u8 pokemonIsJoutaiNormal(u8*);
    u8 r0;
    u32 r31;
    u8* r30;
    u8* r27;
    u8* r26;

    if (ptr == NULL) return;
    if ((u8)fightTrainerCheckValidInline(ptr) == 0) return;
    r30 = fightTrainerGetStatus(ptr, 0, 0x44, 0);
    {
        u32 j;
        j = 0;
        while ((u16)j < 6) {
            arr[(u16)j] = 0;
            j = j + 1;
        }
    }
    r31 = 0;
    while ((u16)r31 < 6) {
        r27 = heroGetStatus(r30, 3, (u16)r31);
        r0 = pokemonCheckValid();
        if ((u8)r0 != 0) {
            r26 = fightTrainerFindPokemonInline(ptr, r27);
            if (r26 == NULL) goto L_next;
            if (fightPokemonCheckValid(r26) == 0) goto L_next;
            if ((s32)pokemonGetStatus(r26, 0, 0xd2, 0) == 1) goto L_next;
            if (fightPokemonCheckFightOut(r26) == 0) {
                arr[(u16)r31] = 3;
                goto L_next;
            }
            if (pokemonIsJoutaiNormal(fightPokemonGetPokemonPtr(r26)) == 0) {
                arr[(u16)r31] = 2;
                goto L_next;
            }
            arr[(u16)r31] = 1;
        }
L_next:
        r31 = r31 + 1;
    }
}

/* 0x801F7B70 | size: 0xE4 | medium */
s32 fightTrainerCheckItemKind(u8* ptr) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8* heroItemGetItemKindToItemAryPtr(u8*, u32, u16*, u32, u32, u32);
    extern u8 fn_801429E8(u8*);
    extern u16 itemGetStatus(u8*, u32, u16, u32);
    u16 count;
    u8* r31;
    u8* r30;
    u32 r29;
    u8* p44;
    p44 = fightTrainerGetStatus(ptr, 0, 0x44, 0);
    if (p44 == NULL) return 0;
    r31 = heroItemGetItemKindToItemAryPtr(p44, 1, &count, 0, 0, 0);
    if (r31 == NULL) return 0;
    r29 = 0;
    while ((u16)r29 < count) {
        r30 = r31 + (u16)r29 * 4;
        if (fn_801429E8(r30) != 0) {
            if (itemGetStatus(r30, 0, 0x1b, 0) != 0) {
                if (itemGetStatus(r30, 0, 0x1c, 0) != 0) {
                    return 1;
                }
            }
        }
        r29++;
    }
    return 0;
}

/* 0x801F7C54 | size: 0x20C | large */
u32 fightTrainerGetTemotiNormalItemDataIdAry(u8* ptr, u16* out, u16 count, u8 mode) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8* heroItemGetItemKindToItemAryPtr(u8*, u32, u16*, u32, u32, u32);
    extern u8 fn_801429E8(u8*);
    extern u32 itemGetStatus(u8*, u32, u16, u32);
    extern u8 fightOutPokemonCheckFightOut(u8*);
    extern u16 fightOutPokemonGetFightActionUseItemDataId(u8*);
    u16 itemCount;
    u16 usedItemIds[2];
    u8* itemArray;
    u32 usedCount;
    u8* fightOut;
    u8* item;
    u32 itemIndex;
    u32 outputCount;
    u32 fightOutIndex;
    u16 itemId;

    {
        u8* hero = fightTrainerGetStatus(ptr, 0, 0x44, 0);
        if (hero == NULL) {
            return 0;
        }
        itemArray = heroItemGetItemKindToItemAryPtr(hero, 2, &itemCount, 0, 0, 0);
    }
    if (itemArray == NULL) {
        return 0;
    }

    clearUsedItemIdsInline(usedItemIds);

    usedCount = collectUsedItemIdsInline(ptr, usedItemIds);

    {
        u32 clearIndex;

        for (clearIndex = 0; (u16)clearIndex < count; clearIndex++) {
            out[(u16)clearIndex] = 0;
        }
    }

    outputCount = 0;
    for (itemIndex = 0; (u16)itemIndex < itemCount; itemIndex++) {
        item = itemArray + (u16)itemIndex * 4;
        if ((u8)fn_801429E8(item) != 0) {
            itemId = itemGetStatus(item, 0, 0x1B, 0);
            if (itemId != 0) {
                u16 amount = itemGetStatus(item, 0, 0x1C, 0);
                if (amount != 0) {
                    if (mode == 1) {
                        u32 usedIndex;

                        for (usedIndex = 0; (u16)usedIndex < (u16)usedCount; usedIndex++) {
                            if (itemId == usedItemIds[(u16)usedIndex] && amount != 0) {
                                amount--;
                            }
                        }
                        if (amount == 0) {
                            continue;
                        }
                    }
                    if ((u16)outputCount < count) {
                        out[(u16)outputCount] = itemId;
                        outputCount++;
                    }
                }
            }
        }
    }
    return outputCount;
}

/* 0x801F7E60 | size: 0x90 | medium */
s32 fightTrainerIsUsedItem(u8* ptr) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8 fightOutPokemonCheckFightOut(u8*);
    extern u8 fightOutPokemonIsFightActionUseItemKind(u8*, u32, u32, u32);
    u8* r29;
    u8* r31;
    u32 r30;
    r29 = ptr;
    for (r30 = 0; (u16)r30 < 2; r30++) {
        r31 = fightTrainerGetStatus(r29, 0, 0x46, r30);
        if (fightOutPokemonCheckFightOut(r31) != 0) {
            if (fightOutPokemonIsFightActionUseItemKind(r31, 1, 0, 0) == 1) {
                return 1;
            }
        }
    }
    return 0;
}

/* 0x801F7EF0 | size: 0x90 | medium */
s32 fightTrainerIsSelectedItemBall(u8* ptr) {
    extern u8* fightTrainerGetStatus(u8*, u32, u16, u32);
    extern u8 fightOutPokemonCheckFightOut(u8*);
    extern u8 fightOutPokemonIsFightActionUseItemKind(u8*, u32, u32, u32);
    u8* r29;
    u8* r31;
    u32 r30;
    r29 = ptr;
    for (r30 = 0; (u16)r30 < 2; r30++) {
        r31 = fightTrainerGetStatus(r29, 0, 0x46, r30);
        if (fightOutPokemonCheckFightOut(r31) != 0) {
            if (fightOutPokemonIsFightActionUseItemKind(r31, 1, 1, 0) == 1) {
                return 1;
            }
        }
    }
    return 0;
}
