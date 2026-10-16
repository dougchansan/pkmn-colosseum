/**
 * @file menuPokemon.c
 * @brief menuPokemon -- Party Pokemon menu (PC box / party list) UI.
 *
 * Address range: 0x800181C4 - 0x8001D7E4 (42 functions)
 *
 * Split from game/gs_pcbox.c (originally mislabeled -- this range is
 * actually the party Pokemon menu module, not the PC box module).
 * Corresponds to XD game/menuPokemon.cpp.
 *
 * NOTE: fn_800181C4 (0x3D0 bytes) is declared extern and called from
 * fn_800188E0 but has no recovered C body anywhere in the tree yet; it
 * remains an unimplemented hole within this address range.
 */

#include "dolphin/types.h"

/* One-function carves include this file with one of these defined. */
#if defined(MENU_POKEMON_8001D378_ONLY) || defined(MENU_POKEMON_80019B48_ONLY) || defined(MENU_POKEMON_8001BEBC_ONLY) || \
    defined(MENU_POKEMON_8001C064_ONLY) || defined(MENU_POKEMON_80019D5C_ONLY) || \
    defined(MENU_POKEMON_80019F6C_ONLY) || \
    defined(MENU_POKEMON_ISLAND_80018F30_ONLY) || \
    defined(MENU_POKEMON_ISLAND_80019938_ONLY) || defined(MENU_POKEMON_ISLAND_800195E0_ONLY) || \
    defined(MENU_POKEMON_ISLAND_8001BAC4_ONLY) || defined(MENU_POKEMON_ISLAND_80018594_ONLY) || \
    defined(MENU_POKEMON_ISLAND_800194D0_ONLY) || defined(MENU_POKEMON_SUB_ONLY) || \
    defined(MENU_POKEMON_8001C7B8_ONLY) || defined(MENU_POKEMON_80018A68_ONLY) || \
    defined(MENU_POKEMON_800181C4_ONLY) || defined(MENU_POKEMON_800194E4_ONLY) || \
    defined(MENU_POKEMON_80019754_ONLY)
#define MENU_POKEMON_CARVE_ONLY
#endif

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_80019F6C_ONLY) || \
    defined(MENU_POKEMON_8001C7B8_ONLY) || defined(MENU_POKEMON_80018A68_ONLY)

/* =========================================================================
 * External declarations (shared)
 * ========================================================================= */

/* Pokemon data */
extern void  heroItemGetItemKindToItemAryPtr(void* pokeData, u8 fieldId, u16* outCount,
                          s32 p4, s32 p5, s32 p6);
extern void  heroHizukiItemGetItemAryPtr(void* pokeData, u16* outCount, s32 p3, s32 p4, s32 p5);
extern u8    fn_801429E8(void* fieldData);
extern u16   itemBiosGetNum(void* fieldData);
extern u16   itemDataBiosGetPtr(u16 speciesId);
extern u16   itemDataBiosGetPrice(void);

/* Text formatting */
extern void  fn_8002A0B8(void* outBuf, void* fmt, s32 p3, s32 p4,
                          u16 p5, s32 p6, ...);
extern s32   heroGetStatus(void* partyData, s32 slot, s32 p3);

/* Dialog/rendering */
extern void  winMsgOpenWithSE(s32 p1, void* text, s32 p3, s32 p4, u8 p5);
extern void  winMsgClose(s32 slot);
extern void  winSeqSetMenu(void* ctx, s32 state);
extern void  menuDataBiosGetXY(s16 npcId, u16* outX, u16* outY);
extern void  menuDataBiosSetXY(s16 x, s16 y, s16 z);
extern void* menuDataBiosGetPtr(void* data);


#endif /* !MENU_POKEMON_CARVE_ONLY || MENU_POKEMON_80019F6C_ONLY */

typedef struct MenuPokemonStatus {
    u16 species;
    u8 unk2[0x18];
    u16 unk1A;
    u8 unk1C[0x8];
    u16 iconId;
    u8 unk26[0xA];
} MenuPokemonStatus;

static inline void* menuPokemonGetHero(s32 mode, void* trainer) {
    extern void* std082_flagGetValue(s32 id);
    extern void* savedataGetStatus(s32, s32);
    extern void* fn_8006AEEC(void);
    extern void* fightFloorGetGcHeroFightTrainerPtr(s32);
    extern void* fightTrainerGetStatus(void*, s32, s32, s32);

    void* hero;

    switch (mode) {
    case 0:
        if (std082_flagGetValue(0x8AE) == 0) {
            hero = savedataGetStatus(0, 2);
        } else {
            hero = fn_8006AEEC();
        }
        break;
    case 1:
        if (trainer == 0) {
            trainer = fightFloorGetGcHeroFightTrainerPtr(0);
        }
        if (trainer == 0) {
            return 0;
        }
        hero = fightTrainerGetStatus(trainer, 0, 0x44, 0);
        break;
    default:
        hero = 0;
        break;
    }
    return hero;
}

static inline void* menuPokemonGetPokemon(s32 mode, u16 index, void* trainer) {
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern void* fightFloorGetGcHeroFightTrainerPtr(s32);
    extern void* fightTrainerGetValidFightPokemonPtr(void* trainer, u16 index);
    extern void* pokemonGetStatus(void*, s32, s32, s32);
    extern u8 pokemonCheckValid(void* pokemon);
    void* pokemon = 0;
    void* hero;
    void* fightPokemon;

    switch (mode) {
    case 0:
        if (index >= 6) {
            return 0;
        }
        hero = menuPokemonGetHero(mode, trainer);
        if (hero == 0) {
            return 0;
        }
        pokemon = heroBiosGetPokemonPtr(hero, index);
        break;
    case 1:
        if (index >= 6) {
            return 0;
        }
        if (trainer == 0) {
            trainer = fightFloorGetGcHeroFightTrainerPtr(0);
        }
        if (trainer == 0) {
            return 0;
        }
        fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, index);
        if (fightPokemon == 0) {
            return 0;
        }
        pokemon = pokemonGetStatus(fightPokemon, 0, 0xCC, 0);
        break;
    case 2:
        if (index >= 30) {
            return 0;
        }
        break;
    }
    if (pokemonCheckValid(pokemon) == 0) {
        pokemon = 0;
    }
    return pokemon;
}

/* RULE-EXCEPTION(user-approved): inline copy of the real fn_8001D624 — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuPokemonGetStatusIcon(void* pokemon) {
    extern u8 pokemonGetStatus(void* pokemon, u32 a, u32 id, u32 b);
    extern u16 pokemonGetJoutaiMenuSpriteId(void* pokemon);
    extern u16 lbl_802E4EB8[8];
    u16 kind;

    if (pokemonGetStatus(pokemon, 0, 0x7B, 0) == 1) {
        kind = 1;
    } else {
        switch (pokemonGetJoutaiMenuSpriteId(pokemon)) {
        case 0x3A: kind = 2; break;
        case 0x3B: kind = 3; break;
        case 0x3C: kind = 4; break;
        case 0x3D: kind = 5; break;
        case 0x3E: kind = 6; break;
        default: kind = 0; break;
        }
    }
    return lbl_802E4EB8[kind];
}

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_8001C064_ONLY) || \
    defined(MENU_POKEMON_8001C7B8_ONLY)
/* 0x8001C064 | 0x754 */
extern u32 std082_flagGetValue();
extern void savedataGetStatus();
extern u32 fn_8006AEEC();
extern u32 fightFloorGetGcHeroFightTrainerPtr();
extern u32 fightTrainerGetStatus();
extern u32 heroBiosGetPokemonPtr();
extern u32 fightTrainerGetValidFightPokemonPtr();
extern u32 pokemonGetStatus();
extern u32 pokemonCheckValid();
extern void msgctrlSetValue();
extern void pokemonGetSoubiItemDataId(void);
extern void heroItemCheckAddItemDataId(void);
extern void winMsgOpen();
extern void heroItemAddItemDataId(void);
extern void pokemonDoItemSoubi(void);
extern void heroItemDecItemDataId(void);
extern void pokemonToMenuPokemonStatus();
extern u32 pokemonGetJoutaiMenuSpriteId();
extern void* memset(void* dst, int val, u32 n);
extern u8 lbl_803A1D40[];
extern u8 lbl_803A1C20[];
extern u8 lbl_802E4EB8[];
extern s8 fn_8001E074(u8, s16, s16, u32);
#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_8001C064_ONLY)
#if 0
asm void fn_8001C064(void) {
#include "src/game/gs_pcbox_fn_8001C064.inc"
}
#else
#if defined(MENU_POKEMON_8001C064_ONLY)
/*
 * RULE-EXCEPTION(user-approved): the one-function carve keeps a static copy
 * of fn_8001D378 so its call is inlined as retail's is; the linked
 * fn_8001D378 stays in its own unit — see docs/RULE_EXCEPTIONS.md
 */
#define MENU_POKEMON_FILL_STATUS_LINKAGE static inline
#else
#define MENU_POKEMON_FILL_STATUS_LINKAGE
#endif
MENU_POKEMON_FILL_STATUS_LINKAGE void fn_8001D378(void);

s32 fn_8001C064(s8 slot, u8 bagSlot, u16 itemId, u16* outItem) {
    extern void msgctrlSetValue();
    extern u16 pokemonGetSoubiItemDataId(void* pokemon);
    extern s32 heroItemCheckAddItemDataId(void* hero, u16 item);
    extern void heroItemAddItemDataId(void* hero, u16 item, s32 count, s32 slot);
    extern s32 heroItemDecItemDataId(void* hero, u16 item, s32 count, u8 slot);
    extern u16 pokemonDoItemSoubi(void* pokemon, u16 item, s32 flag);
    extern void winMsgOpen(s32, s32, s32, s32);
    extern void winMsgClose(s32);
    extern s32 menuSubOpenYesNo(s32, s32, s32, s32);
    void* pokemon;
    void* hero;
    u16 removed;
    s8 answer;
    u16 held;
    /* RULE-EXCEPTION(user-approved): mode read into a named local to fix retail's r5/r4 order — see docs/RULE_EXCEPTIONS.md */
    s32 mode;

    mode = *(s32*)(lbl_803A1D40 + 0x8);
    pokemon = menuPokemonGetPokemon(mode, slot, *(void**)(lbl_803A1D40 + 0xC));
    msgctrlSetValue(0x32, &((MenuPokemonStatus*)lbl_803A1C20)[(s8)lbl_803A1D40[6]]);
    held = pokemonGetSoubiItemDataId(pokemon);
    hero = menuPokemonGetHero(*(s32*)(lbl_803A1D40 + 0x8), *(void**)(lbl_803A1D40 + 0xC));
    if (hero == 0) {
        return -1;
    }

    if (itemId == 0) {
        if (held != 0) {
            if (heroItemCheckAddItemDataId(hero, held) <= 0) {
                winMsgOpen(2, 0x2B6B, 1, 1);
            } else {
                heroItemAddItemDataId(hero, held, 1, -1);
                removed = pokemonDoItemSoubi(pokemon, 0, 0);
                if (outItem != 0) {
                    *outItem = removed;
                }
                msgctrlSetValue(0x2D, removed);
                winMsgOpen(2, 0x2B69, 1, 1);
            }
        } else {
            winMsgOpen(2, 0x2B6A, 1, 1);
        }
    } else if (held != 0) {
        msgctrlSetValue(0x2D, held);
        winMsgOpen(2, 0x2B66, 1, 0);
        answer = menuSubOpenYesNo(0, -1, -1, 0);
        winMsgClose(1);
        if (answer != 0) {
            return -1;
        }
        if (heroItemDecItemDataId(hero, itemId, 1, bagSlot) != 0) {
            winMsgOpen(2, 0x2B6B, 1, 1);
        } else if (heroItemCheckAddItemDataId(hero, held) <= 0) {
            heroItemAddItemDataId(hero, itemId, 1, lbl_803A1D40[0x11]);
            winMsgOpen(2, 0x2B6B, 1, 1);
        } else {
            heroItemAddItemDataId(hero, held, 1, -1);
            removed = pokemonDoItemSoubi(pokemon, 0, 0);
            if (outItem != 0) {
                *outItem = removed;
            }
            pokemonDoItemSoubi(pokemon, itemId, 1);
            msgctrlSetValue(0x2D, removed);
            msgctrlSetValue(0x2E, itemId);
            winMsgOpen(2, 0x2B67, 1, 0);
        }
    } else if (heroItemDecItemDataId(hero, itemId, 1, bagSlot) == 0) {
        pokemonDoItemSoubi(pokemon, itemId, 1);
        msgctrlSetValue(0x2D, itemId);
        winMsgOpen(2, 0x2B68, 1, 0);
    }
    winMsgClose(1);
    fn_8001D378();
    return -1;
}
#endif
#endif /* !MENU_POKEMON_CARVE_ONLY || MENU_POKEMON_8001C064_ONLY */
#if defined(MENU_POKEMON_8001C064_ONLY)
/*
 * RULE-EXCEPTION(user-approved): the deferred-inline build generates
 * fn_8001C064 with the peephole state in force at the end of the file, which
 * in the full file is off; the carve sets it the same way — see
 * docs/RULE_EXCEPTIONS.md
 */
#pragma peephole off
#endif

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_8001C7B8_ONLY)
/* 0x8001C7B8 | 0xBC0 */
#if defined(MENU_POKEMON_8001C7B8_ONLY)
/*
 * RULE-EXCEPTION(user-approved): the one-function carve keeps a static copy
 * of fn_8001D378 so its call is inlined (the unit builds with -inline
 * auto,deferred) as retail's is; the linked fn_8001D378 stays in its own
 * unit — see docs/RULE_EXCEPTIONS.md
 */
static inline void fn_8001D378(void);
#else
void fn_8001D378(void);
#endif
extern s32 menuOpen();
extern void menuClose();
extern void menuCloseCustom();
extern void fadeSet();
extern void fadeCheck();
extern void fn_80097F08(void);
extern s32 menuOpenCustom();
extern u8* windowSearchID();
extern s32 menuGetCursorItemID();
extern u32 _threadSwitch(void);
extern void winSeqCheckMove(void);
extern void pokemonReplace(void);
extern u32 fn_80019064(void);
extern void fn_80018F54();
extern void fn_8001D718(f32 target);
extern f32 lbl_8047B7C0;
extern u8 lbl_802E4E58[];
static inline void menuPokemonSetSlotSeq(s8 slot, s32 right, s32 left) {
    extern void winSeqSetMenu(s16 id, s32 seq);
    s16 x;
    s16 y;
    s16 id;

    id = *(s16*)(lbl_802E4E58 + ((s8*)lbl_803A1D40)[4] * 0x30 + slot * 8);
    menuDataBiosGetXY(id, (u16*)&x, (u16*)&y);
    winSeqSetMenu(id, (x > 0xFA) ? right : left);
}

static inline void menuPokemonWaitSlot(s8 slot) {
    extern u8 winSeqCheckMove(s16 id);
    s16 x;
    s16 y;
    s16 id;

    while (id = *(s16*)(lbl_802E4E58 + slot * 8 + ((s8*)lbl_803A1D40)[4] * 0x30),
           menuDataBiosGetXY(id, (u16*)&x, (u16*)&y), winSeqCheckMove(id) != 0) {
        _threadSwitch();
    }
}

/* RULE-EXCEPTION(user-approved): single-call inline helper (fn_8001C7B8) — see docs/RULE_EXCEPTIONS.md */
static inline s32 menuPokemonItemMenu(s8* work, s8 slot, s32* itemArg) {
    extern s32 fn_8001C064(s8 slot, s32 pocket, u16 item, u16* out);
    extern u16 fn_80018F54(s32 a, s32 b, s32 c);
    extern s32 menuOpenCustom(s32 menuId, s32 parent, void* args, s32, s32, s32, ...);
    s32 action;
    u16 item;
    u32 pocket;
    void* pokemon;
    u16 newItem;

    work[0x14] = 3;
    action = menuOpen(0x6C, 1);
    menuClose(0x6C);
    switch (action) {
    case 0:
        menuClose(0x63);
        item = fn_80018F54(2, 0, 0);
        pocket = fn_80019064();
        *itemArg = work[6];
        menuOpenCustom(0x63, 0, itemArg, 0, 0, 1, lbl_803A1C20);
        pokemon = windowSearchID(0x63);
        if (pokemon != 0) {
            ((u8*)pokemon)[0x98] = 1;
        }
        if ((u16)item == 0) {
            action = -1;
        } else {
            fn_8001C064(slot, pocket & 0xFF, item, &newItem);
            action = 0;
        }
        break;
    case 1:
        fn_8001C064(slot, -1, 0, &newItem);
        action = 0;
        break;
    case -1:
    case 2:
        action = -1;
        break;
    }
    if (action == -1) {
        return 0;
    }
    return -1;
}

#if 0
asm void fn_8001C7B8(void) {
#include "src/game/gs_pcbox_fn_8001C7B8.inc"
}
#else
/* Built with the unit-level -opt nopeephole (configure.py); retail has no
 * record-form compares here. */
s32 fn_8001C7B8(s8 slot) {
    extern void* memset(void* dst, int val, u32 n);
    extern s32 menuOpenCustom(s32 menuId, s32 parent, void* args, s32, s32, s32, ...);
    extern void pokemonToMenuPokemonStatus(void* pokemon, MenuPokemonStatus* status);
    extern u8 pokemonGetStatus(void* pokemon, u32 a, u32 id, u32 b);
    extern void pokemonReplace(void* a, void* b);
    extern void fn_80097F08(void* pokemon, void* callback, s32 arg);
    extern u16 fn_80018F54(s32 a, s32 b, s32 c);
    extern s32 fn_80019D5C();
    s32 swapArg;
    s32 summaryArg;
    s32 itemArg;
    s8* work;
    /* RULE-EXCEPTION(user-approved): single-field int carrier keeps menuOpen's result in sel's register (retail mr r23,r3) — see docs/RULE_EXCEPTIONS.md */
    struct { int value; } sel;
    s32 action;
    s32 target;
    s32 picked;
    void* pokemon;
    void* other;
    MenuPokemonStatus* status;
    u16 i;
    u16 item;
    u32 pocket;

    work = (s8*)lbl_803A1D40;
    do {
        work[0x14] = 1;
        if (std082_flagGetValue(0x8AE) == 0) {
            sel.value = menuOpen(0x6B, 1);
            menuClose(0x6B);
        } else {
            sel.value = menuOpen(0x10F, 1);
            menuClose(0x10F);
        }
        switch (sel.value) {
        case 0x3CF:
        case 0x52A:
            menuCloseCustom(0x63, 0, 1);
            pokemon = menuPokemonGetPokemon(*(s32*)(work + 0x8), slot, *(void**)(work + 0xC));
            fadeSet(3, lbl_8047B7C0);
            fadeCheck(1);
            fn_80097F08(pokemon, fn_80019D5C, 0);
            summaryArg = work[6];
            menuOpenCustom(0x63, 0, &summaryArg, 0, 0, 1, lbl_803A1C20);
            pokemon = windowSearchID(0x63);
            if (pokemon != 0) {
                ((u8*)pokemon)[0x98] = 1;
            }
            fadeSet(2, lbl_8047B7C0);
            fadeCheck(1);
            slot = work[6];
            sel.value = 0;
            break;
        case 0x3D0:
        case 0x52B:
            work[0x14] = 2;
            work[7] = slot;
            swapArg = work[6];
            picked = (s8)menuOpenCustom(0x63, 0, &swapArg, 0, 1, 1, lbl_803A1C20);
            if (((u8*)work)[1] == 0) {
                picked = -2;
            }
            target = (s8)picked;
            work[7] = -1;
            if (menuGetCursorItemID(0x63) != 0x3B6 && target != -1) {
                pokemon = menuPokemonGetPokemon(*(s32*)(work + 0x8), slot, *(void**)(work + 0xC));
                other = menuPokemonGetPokemon(*(s32*)(work + 0x8), target, *(void**)(work + 0xC));
                if (pokemon != 0 && pokemon != 0) {
                    menuPokemonSetSlotSeq(slot, 0x11A, 0x122);
                    menuPokemonSetSlotSeq(target, 0x11A, 0x122);
                    menuPokemonWaitSlot(slot);
                    pokemonReplace(pokemon, other);
                    fn_8001D378();
                    fn_8001D718(lbl_8047B7C0);
                    menuPokemonSetSlotSeq(slot, 0x116, 0x11E);
                    menuPokemonSetSlotSeq(target, 0x116, 0x11E);
                    menuPokemonWaitSlot(slot);
                }
            }
            sel.value = -1;
            break;
        case 0x3D1:
            sel.value = menuPokemonItemMenu(work, slot, &itemArg);
            break;
        case -1:
        case 0x3D2:
        case 0x52C:
            sel.value = -1;
            break;
        }
    } while (sel.value != -1);
    return 0;
}
#endif

#endif /* !MENU_POKEMON_CARVE_ONLY || MENU_POKEMON_8001C7B8_ONLY */

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_8001D378_ONLY) || \
    defined(MENU_POKEMON_8001C064_ONLY) || defined(MENU_POKEMON_8001C7B8_ONLY)
/* 0x8001D378 | 0x2AC */
#if !defined(MENU_POKEMON_8001C7B8_ONLY)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the MENU_POKEMON_8001C7B8_ONLY carve's unit flag -opt nopeephole covers its inline copy) — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#endif
#if defined(MENU_POKEMON_8001C064_ONLY) || defined(MENU_POKEMON_8001C7B8_ONLY)
static inline
#endif
void fn_8001D378(void) {
    extern u8 lbl_803A1C20[];
    extern u8 lbl_803A1D40[];
    extern void* memset(void* dst, int val, u32 n);
    extern void pokemonToMenuPokemonStatus(void* pokemon, MenuPokemonStatus* status);
    extern u8 pokemonGetStatus(void* pokemon, u32 a, u32 id, u32 b);
    /* Typed, not void*: the void* result is converted into this local instead
       of being copy-propagated, so the copy of fn_8001D378 inlined into
       fn_8001C7B8 keeps the extra coalesced pokemon copy that sets retail's
       r27-r30 order there (no effect on this function's own codegen). */
    struct Pokemon* pokemon;
    /* The O3 carve and O2 inlined caller color these locals in opposite orders. */
#if defined(MENU_POKEMON_8001D378_ONLY)
    u16 i;
    MenuPokemonStatus* status;
#else
    MenuPokemonStatus* status;
    u16 i;
#endif

    memset(lbl_803A1C20, 0, 0x120);
    for (i = 0; i < 6; i++) {
        status = &((MenuPokemonStatus*)lbl_803A1C20)[i];
        pokemon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), i,
                                        *(void**)(lbl_803A1D40 + 0xC));
        if (pokemon == 0) {
            status->species = 0;
        } else {
            pokemonToMenuPokemonStatus(pokemon, status);
            if (pokemonGetStatus(pokemon, 0, 0x7B, 0) == 1) {
                status->unk1A = 0;
            }
            status->iconId = menuPokemonGetStatusIcon(pokemon);
        }
    }
}
#if !defined(MENU_POKEMON_8001C7B8_ONLY)
#pragma pop
#endif
#endif

#if !defined(MENU_POKEMON_CARVE_ONLY)

/* 0x8001D718 | 0xCC */
extern s32 fn_800D37CC(void);
extern u32 GSgetFrameStep(void);
extern f32 lbl_8047B7C8;
extern f64 lbl_8047B7D0;
extern f64 lbl_8047B7D8;
#if 0
asm void fn_8001D718(void) {
#include "src/game/gs_pcbox_fn_8001D718.inc"
}
#else
void fn_8001D718(f32 target) {
    extern void _threadSwitch();
    f32 progress = lbl_8047B7C8;

    while (progress < target) {
        _threadSwitch();
        progress += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
    }
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_80018594_ONLY) || \
    defined(MENU_POKEMON_800181C4_ONLY)
typedef struct MenuPokemonSummaryPage {
    u8 color[4];
    s32 kind;
    u8 unk8[0x44];
} MenuPokemonSummaryPage;

/* The quantity menu's trailing argument; its column count travels separately
   (as in the event-side copy fn_80013A18). */
typedef struct MenuPokemonQuantityArgs {
    u8 color[4];
    s32 menuId;
    s32 maximum;
    s32 minimum;
    s32 unitPrice;
} MenuPokemonQuantityArgs;

/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_800181C4) — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuPokemonGetBagItemNum(u8* items, u16* count, s32 slot) {
    extern u8 fn_801429E8(void* item);
    extern u16 itemBiosGetNum(void* item);
    s32 i;
    s32 validIndex;

    validIndex = -1;
    for (i = 0; i < *count; i++, items += 4) {
        if (fn_801429E8(items)) {
            validIndex++;
            if (validIndex >= slot) {
                return itemBiosGetNum(items);
            }
        }
    }
    return 0;
}

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_800181C4_ONLY)
#if !defined(MENU_POKEMON_800181C4_ONLY)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the MENU_POKEMON_800181C4_ONLY carve uses the unit's -opt nopeephole instead) — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#endif
s32 fn_800181C4(page, itemId, itemSlot)
    s32 page;
    u16 itemId;
    s32 itemSlot;
{
    extern void* itemDataBiosGetPtr();
    extern u16 itemDataBiosGetPrice(void* itemData);
    extern u32 fn_8002A0B8(u8* color, u32 messageContext, s32 group,
                           s32 category, ...);
    extern void winMsgOpenWithSE(s32, u32, s32, s32, u8);
    extern void winMsgOpen(s32, u32, s32, s32);
    extern void winMsgClose(s32);
    extern s8 menuSubOpenYesNo(s32, s32, s32, s32);
    extern void fn_8012959C(void* hero, u16 item, u16 count, s16 slot);
    extern void heroItemDecItemDataId(void* hero, u16 item, u16 count,
                                      s16 slot);
    extern void soundStartRaw(s32 sound, s32, s32);
    extern void heroAddPokedoru(void* hero, u32 amount);
    extern u8* heroItemGetItemKindToItemAryPtr(void* hero, u8 kind, u16* count,
                                               u16* total, s32, s32);
    extern u8* heroHizukiItemGetItemAryPtr(void* hero, u16* count,
                                           s32, s32, s32);
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menuId, s32 parent, void* args, s32, s32, s32, ...);
    extern void menuClose(s32 menuId);
    extern void menuCloseSync(s32 menuId, s32 wait);
    extern MenuPokemonSummaryPage lbl_80266918[];
    extern u32 lbl_8047A2BC;
    extern u32 lbl_8047A2DC;
    extern void* lbl_8047A2F8;
    extern s32 lbl_8047A2FC;
    /* RULE-EXCEPTION(user-approved): declaration order and the named row
     * offset chosen for register allocation - see docs/RULE_EXCEPTIONS.md */
    u8* items;
    s32 menuResult;
    s32 selected;
    s32 half;
    u16 total;
    u16 count;
    u32 context;
    s32 offset;
    s32* kind;
    MenuPokemonQuantityArgs args;
    s32 menuId;
    s32 price;
    s32 digits;
    s32 item;
    s32 saleValue;
    s32 quantity;
    s32 columns;
    s8 answer;
    u8 messageColor;

    /* RULE-EXCEPTION(user-approved): scalar carrier - the item id's
     * zero-extended copy for the untyped and variadic calls, chosen for
     * register allocation - see docs/RULE_EXCEPTIONS.md */
    item = itemId;
    context = lbl_8047A2BC;
    price = itemDataBiosGetPrice(itemDataBiosGetPtr(item));
    if (price <= 0) {
        winMsgOpenWithSE(2, fn_8002A0B8(&messageColor, context, 0xB, 0x2D, item, -1),
                         1, 0, messageColor);
        winMsgClose(1);
        return 0;
    }

    lbl_8047A2DC = fn_8002A0B8(&messageColor, context, 0xD, 0x2D, item, -1);
    /* RULE-EXCEPTION(user-approved): page row walked to its kind field with a
     * pointer pre-increment (one register for the row and &kind, as retail)
     * - see docs/RULE_EXCEPTIONS.md */
    offset = page * 0x4C;
    kind = (s32*)((u8*)lbl_80266918 + offset);
    heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, *++kind, 0, &total, 0, 0);
    if ((s32)total > 100) {
        digits = 3;
    } else {
        digits = 2;
    }
    if (*kind >= 0) {
        items = heroItemGetItemKindToItemAryPtr(lbl_8047A2F8, *kind, &count, 0, 0, 0);
    } else {
        items = heroHizukiItemGetItemAryPtr(lbl_8047A2F8, &count, 0, 0, 0);
    }
    quantity = menuPokemonGetBagItemNum(items, &count, itemSlot);
    if (quantity < 1) {
        selected = 0;
    } else {
        half = price / 2;
        if (half > 0) {
            if (digits == 2) {
                columns = 1;
                menuId = 0x5D;
            } else {
                columns = 2;
                menuId = 0x5E;
            }
        } else {
            if (digits == 2) {
                columns = 1;
                menuId = 0x5B;
            } else {
                columns = 2;
                menuId = 0x5C;
            }
        }
        {
            u8* entry = (u8*)lbl_80266918;
            entry += offset;
            args.color[0] = entry[0];
            args.color[1] = entry[1];
            args.color[2] = entry[2];
        }
        args.menuId = menuId;
        args.minimum = 1;
        args.maximum = quantity;
        args.unitPrice = half;
        lbl_8047A2FC = 1;

        menuResult = menuOpenCustom(menuId, windowGetActiveID(), &columns, 0, 1, 1, &args);
        menuClose(menuId);
        menuCloseSync(menuId, 1);
        if (menuResult == -1) {
            selected = -1;
        } else {
            selected = lbl_8047A2FC;
        }
    }
    if (selected < 0) {
        return 0;
    }

    saleValue = price / 2 * selected;
    winMsgOpenWithSE(2, fn_8002A0B8(&messageColor, context, 9, 0x4B, saleValue, -1),
                     1, 0, messageColor);
    answer = menuSubOpenYesNo(0, -1, -1, 0);
    if (answer == 1 || answer == -1) {
        winMsgClose(1);
        return 0;
    }

    if (*kind == -1) {
        fn_8012959C(lbl_8047A2F8, itemId, selected, itemSlot);
    } else {
        heroItemDecItemDataId(lbl_8047A2F8, itemId, selected, itemSlot);
    }
    soundStartRaw(0x3CB, 0, 0);
    heroAddPokedoru(lbl_8047A2F8, saleValue);
    winMsgOpen(2, fn_8002A0B8(&messageColor, context, 0xA, 0x2D, item, 0x4B, saleValue, -1),
               1, 0);
    winMsgClose(1);
    return 1;
}
#if !defined(MENU_POKEMON_800181C4_ONLY)
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_80018594_ONLY)
/* fn_80018594 - 0x80018594 | size: 0x34c */
extern u32 itemDataBiosGetFieldUseFunc();
extern u32 itemDataBiosGetBattleUseFunc();
extern s32 fn_80017CB8();
extern void fn_8012959C(void);
extern void menuCloseSync(); /* referenced by asm incs */
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2D8;
extern MenuPokemonSummaryPage lbl_80266918[];
#define sSummaryPageEntries ((u8*)lbl_80266918)
extern u32 lbl_8047A2DC;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2B8;
extern u16 fn_80019754(u32 color);
#if 0
asm s32 fn_80018594() {
#include "src/game/gs_pcbox_fn_80018594.inc"
}
#else
typedef struct MenuPokemonSummaryAction {
    u32 label;
    s32 (*callback)();
    u32 value;
} MenuPokemonSummaryAction;

/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_80018594) — see docs/RULE_EXCEPTIONS.md */
static inline s32 menuPokemonSelect(s32 menuId, void* arg, s32 count) {
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
    extern void menuClose(s32);
    s32 ret;

    ret = menuOpenCustom(menuId, windowGetActiveID(), 0, 0, 1, 1, arg);
    menuClose(menuId);
    menuCloseSync(menuId, 1);
    if (ret == -1) {
        return -1;
    }
    if (ret < 0 || ret >= count) {
        return -1;
    }
    return ret;
}

static inline void menuPokemonUseItem(u32 page, u16 itemId, s32 count, u32 slot) {
    extern void fn_8012959C(void* hero, u16 item, u16 count, s16 slot);
    extern void heroItemDecItemDataId(void* hero, u16 item, u16 count, s16 slot);

    if (lbl_80266918[page].kind == -1) {
        fn_8012959C((void*)lbl_8047A2F8, itemId, count, slot);
    } else {
        heroItemDecItemDataId((void*)lbl_8047A2F8, itemId, count, slot);
    }
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_80018594(boxIndex, itemId, slotIndex, outItem)
    u32 boxIndex;
    u16 itemId;
    u32 slotIndex;
    u16* outItem;
{
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
    extern void menuClose(s32);
    extern void* itemDataBiosGetPtr();
    extern void fn_8012959C(void* hero, u16 item, u16 count, s16 slot);
    extern void heroItemDecItemDataId(void* hero, u16 item, u16 count, s16 slot);
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 pad;
        MenuPokemonSummaryAction* entries;
        s32 count;
    } menuArg;
    MenuPokemonSummaryAction actions[5];
    s32 count;
    s32 choice;
    s32 callbackRet;
    s32 callbackValue;
    s32 (*menuCallback)();
    s32 (*useCallback)();
    u8* page;
    u16 selected;

    *outItem = 0;
    if ((s32)lbl_8047A2E0 == 0 && itemId == 0x21E) {
        lbl_8047A2DC = 0x2B41;
        selected = fn_80019754((lbl_80266918[lbl_8047A2D8].color[0] << 24) |
                               (lbl_80266918[lbl_8047A2D8].color[1] << 16) |
                               (lbl_80266918[lbl_8047A2D8].color[2] << 8) | 0xFF);
        if (selected == 0) {
            return 1;
        }

        *outItem = selected;
        itemDataBiosGetPtr(selected);
        if ((s32)lbl_8047A2E0 == 0) {
            useCallback = (s32 (*)())itemDataBiosGetFieldUseFunc();
        } else {
            useCallback = (s32 (*)())itemDataBiosGetBattleUseFunc();
        }
        if (useCallback == 0) {
            winMsgOpen(2, 0x4261, 1, 0);
            winMsgClose(1);
            return 0;
        }

        if (useCallback(selected, &callbackValue) == 2) {
            if (callbackValue > 0) {
                heroItemDecItemDataId((void*)lbl_8047A2F8, selected, callbackValue, -1);
            }
            return 4;
        }
        return 1;
    }

    count = fn_80017CB8(actions, 5, boxIndex, slotIndex);
    lbl_8047A2DC = 0x135;
    page = sSummaryPageEntries + lbl_8047A2D8 * 0x4C;
    menuArg.r = page[0];
    menuArg.g = page[1];
    menuArg.b = page[2];
    menuArg.entries = actions;
    menuArg.count = count;

    choice = menuPokemonSelect(0x5A, &menuArg, count);
    if (choice < 0) {
        return 1;
    }

    menuCallback = actions[choice].callback;
    if (menuCallback == 0) {
        return 1;
    }

    callbackRet = menuCallback(boxIndex, slotIndex, &callbackValue);
    switch (callbackRet) {
    case 0:
        if (callbackValue > 0) {
            menuPokemonUseItem(boxIndex, itemId, callbackValue, slotIndex);
        }
        lbl_8047A2B8 = slotIndex;
        return 0;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 1;
    case 4:
        if (callbackValue > 0) {
            menuPokemonUseItem(boxIndex, itemId, callbackValue, slotIndex);
        }
        return 4;
    default:
        return 0;
    }
}
#pragma pop
#endif


/* fn_800188E0 - 0x800188E0 | size: 0x188 */
extern u32 fn_80143FCC();
extern void fn_80017E8C();
extern u32 lbl_8047A2B8;
#if 0
asm void fn_800188E0(void) {
#include "src/game/gs_pcbox_fn_800188E0.inc"
}
#else
/* The island unit builds with -opt nopeephole (configure.py), so this
   pragma only applies to whole-file compiles. */
#if !defined(MENU_POKEMON_ISLAND_80018594_ONLY)
#pragma peephole off
#endif
s32 fn_800188E0(s32 mode, u32 ptr, u32 r5, u32 r6, u16* out) {
    u16 tmp;
    s32 ret;

    tmp = 0;
    switch (mode) {
    case 0:
    case 1: {
        ret = fn_80018594(ptr, r5, r6, &tmp);
        if (ret == 2) {
            ret = 2;
        } else if (ret == 3) {
            ret = 3;
        } else if (ret == 4) {
            ret = 1;
        } else if (mode == 0) {
            ret = 0;
        } else if (ret == 0) {
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    }
    case 2: {
        s32 r0;
        {
            extern void itemDataBiosGetPtr();
            itemDataBiosGetPtr(r5);
        }
        r0 = fn_80143FCC();
        if ((u8)r0 != 0) {
            msgctrlSetValue(0x2d, (u16)r5);
            winMsgOpen(2, 0x4262, 1, 0);
            winMsgClose(1);
            r0 = 0;
        } else {
            lbl_8047A2B8 = r6;
            r0 = 1;
        }
        if (r0 != 0) {
            ret = 1;
        } else {
            ret = 0;
        }
        break;
    }
    case 3:
        fn_800181C4(ptr, r5, r6);
        ret = 0;
        break;
    case 4:
        fn_80017E8C(ptr, r5, r6);
        ret = 0;
        break;
    default:
        break;
    }
    *out = tmp;
    return ret;
}
#if !defined(MENU_POKEMON_ISLAND_80018594_ONLY)
#pragma peephole reset
#endif
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_80018A68_ONLY)
/* fn_80018A68 - 0x80018A68 | size: 0x4c8 */
#if defined(MENU_POKEMON_80018A68_ONLY)
typedef struct MenuPokemonSummaryPage {
    u8 color[4];
    s32 kind;
    u8 unk8[0x44];
} MenuPokemonSummaryPage;
extern MenuPokemonSummaryPage lbl_80266918[];
#define sSummaryPageEntries ((u8*)lbl_80266918)
s32 fn_800188E0(s32 mode, u32 ptr, u32 r5, u32 r6, u16* out);
#endif
extern u16 itemBiosGetItemDataId();
extern void fn_800FF660(void);
extern void floorSetFadeScript();
extern u32 lbl_80478860;
extern u8 lbl_802E4DB0[];
extern u8 lbl_802EF0A8[];
extern u32 lbl_8047A2E8;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2D8;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2E4;

typedef struct MenuPokemonCursor {
    s8 base;
    s8 offset;
} MenuPokemonCursor;

typedef struct MenuPokemonPosEntry {
    u32 index;
    s16 x;
    s16 y;
    u8 unk8[4];
} MenuPokemonPosEntry;

extern MenuPokemonCursor cursorBiosGetPos(u16 id);
extern void cursorBiosSetPos(u16 id, MenuPokemonCursor* pos);

/* The file-level externs declare both getters void; the casts give their real prototypes
 * (item array pointer result, u8 kind, u16* total).
 * RULE-EXCEPTION(user-approved): codegen-only casts; an unprototyped call puts (u8)kind in an extra coalesced temp that shifts fn_80018A68's colouring — see docs/RULE_EXCEPTIONS.md */
static inline u8* menuPokemonGetItemArray(s32 kind, u16* count) {
    if (kind >= 0) {
        return ((u8* (*)(void*, u8, u16*, u16*, s32, s32))heroItemGetItemKindToItemAryPtr)((void*)lbl_8047A2F8, kind, count, 0, 0, 0);
    }
    return ((u8* (*)(void*, u16*, s32, s32, s32))heroHizukiItemGetItemAryPtr)((void*)lbl_8047A2F8, count, 0, 0, 0);
}

/* Number of usable items in a bag pocket.
 * RULE-EXCEPTION(user-approved): single-function inline helper (fn_80018A68); expanded inside menuPokemonClampCursors, so its count slot is created last (sp+0x8) — see docs/RULE_EXCEPTIONS.md */
static inline s32 menuPokemonCountItems(s32 kind) {
    u16 count;
    u8* items;
    s32 i;
    s32 n;

    items = menuPokemonGetItemArray(kind, &count);
    n = 0;
    for (i = 0; i < count; i++, items += 4) {
        if (fn_801429E8(items) != 0) {
            n++;
        }
    }
    return n;
}

/* Item id of the target-th usable item in a bag pocket, or 0.
 * RULE-EXCEPTION(user-approved): single-function inline helper (fn_80018A68); locals declared items, i, n for retail's r25/r24/r23 — see docs/RULE_EXCEPTIONS.md */
static inline u16 menuPokemonFindItem(s32 kind, s32 target) {
    u16 count;
    u8* items;
    s32 i;
    s32 n;

    items = menuPokemonGetItemArray(kind, &count);
    n = -1;
    for (i = 0; i < count; i++, items += 4) {
        if (fn_801429E8(items) != 0) {
            n++;
            if (n >= target) {
                return itemBiosGetItemDataId(items);
            }
        }
    }
    return 0;
}

/* Pull each of the six pocket cursors back inside its pocket's usable item count.
 * RULE-EXCEPTION(user-approved): single-function inline helper (fn_80018A68); its slot counter i is an inline local, numbered above CountItems' n, and the indexed entry pointer becomes an induction temp, giving retail's r29/r28/r27 — see docs/RULE_EXCEPTIONS.md */
static inline void menuPokemonClampCursors(void) {
    MenuPokemonCursor pos;
    MenuPokemonCursor newPos;
    s32 i;
    s32 occupied;
    s32 excess;

    for (i = 0; i < 6; i++) {
        pos = cursorBiosGetPos(*(u32*)(sSummaryPageEntries + i * 0x4C + 0x1C));
        if (pos.offset < 0) {
            pos.offset = 0;
        }
        if (pos.base < 0) {
            pos.base = 0;
        }
        occupied = menuPokemonCountItems(*(s32*)(sSummaryPageEntries + i * 0x4C + 4));
        excess = pos.base + pos.offset - occupied;
        if (excess > 0) {
            if ((pos.offset -= (s8)excess) < 0) {
                pos.offset = 0;
            }
            excess = pos.base + pos.offset - occupied;
            if (excess > 0) {
                if ((pos.base -= (s8)excess) < 0) {
                    pos.base = 0;
                }
            }
        }
        newPos = pos;
        cursorBiosSetPos(*(u32*)(sSummaryPageEntries + i * 0x4C + 0x1C), &newPos);
    }
}

/* RULE-EXCEPTION(user-approved): single-function inline helper (fn_80018A68); the named index local gives the selected index an inline-variable number above FindItem's locals (retail r22) — see docs/RULE_EXCEPTIONS.md */
static inline s32 menuPokemonCursorIndex(MenuPokemonCursor* pos) {
    s32 index;

    index = pos->base + pos->offset;
    return index;
}

#if !defined(MENU_POKEMON_80018A68_ONLY)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma (the MENU_POKEMON_80018A68_ONLY carve uses the unit's -opt nopeephole instead) — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
#endif
u16 fn_80018A68(void) {
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32 menuId, s32 parent, void* args, s32, s32, s32, ...);
    extern void menuCloseCustom();
    u16 replacementSpecies;
    s32 targetIndex;
    MenuPokemonCursor initPos;
    MenuPokemonCursor selPos;
    s32 filterValue;
    s32 i;
    s32* filterPtr;
    u16 selectedSpecies;
    s32 ret;
    s32 done;
    s32 selectedOffset;
    s32 retrySelection;
    u16 species;

    filterPtr = 0;
    selectedOffset = 0;
    retrySelection = 0;
    done = 0;

    if (*(u8*)&lbl_80478860 != 0) {
        for (i = 0; i < 8; i++) {
            ((MenuPokemonPosEntry*)lbl_802E4DB0)[i].x = ((s16*)(lbl_802EF0A8 + 4))[((MenuPokemonPosEntry*)lbl_802E4DB0)[i].index * 14];
            ((MenuPokemonPosEntry*)lbl_802E4DB0)[i].y = ((s16*)(lbl_802EF0A8 + 8))[((MenuPokemonPosEntry*)lbl_802E4DB0)[i].index * 14];
        }
        *(u8*)&lbl_80478860 = 0;
    }

    initPos = cursorBiosGetPos(2);
    if (initPos.offset <= 0) {
        filterValue = 5;
        filterPtr = &filterValue;
    }

    while (done == 0) {
        if (retrySelection != 0) {
            lbl_8047A2E8 = selectedOffset;
        } else {
            lbl_8047A2E8 = -1;
        }

        menuPokemonClampCursors();

        lbl_8047A2D8 = -1;
        lbl_8047A2D8 = menuOpenCustom(0x59, windowGetActiveID(), filterPtr, 0, 1, 0);
        if ((s32)lbl_8047A2D8 == -1) {
            species = 0;
        } else {
            selPos = cursorBiosGetPos(*(u32*)(sSummaryPageEntries + lbl_8047A2D8 * 0x4C + 0x1C));
            targetIndex = menuPokemonCursorIndex(&selPos);
            {
                u16 item;

                /* RULE-EXCEPTION(user-approved): the item id is held in a block local so selectedOffset's copy is emitted before species' (retail mr r25,r22 then mr r5,r3) — see docs/RULE_EXCEPTIONS.md */
                item = menuPokemonFindItem(*(s32*)(sSummaryPageEntries + lbl_8047A2D8 * 0x4C + 4), targetIndex);
                selectedOffset = targetIndex;
                species = item;
            }
        }
        selectedSpecies = species;

        filterPtr = 0;
        retrySelection = 0;
        if (selectedSpecies == 0) {
            break;
        }
        /* RULE-EXCEPTION(user-approved): codegen-only cast; retail passes the u16 species unconverted (fn_800188E0 is defined with a u32 parameter) — see docs/RULE_EXCEPTIONS.md */
        ret = ((s32 (*)(s32, u32, u16, u32, u16*))fn_800188E0)(lbl_8047A2E0, lbl_8047A2D8, selectedSpecies, selectedOffset, &replacementSpecies);
        if (replacementSpecies != 0) {
            selectedSpecies = replacementSpecies;
        }
        switch (ret) {
        case 0:
            break;
        case 1:
            done = 1;
            break;
        case 2:
            retrySelection = 1;
            break;
        case 3:
            filterValue = 0;
            filterPtr = &filterValue;
            break;
        }
    }

    menuCloseCustom(0x59, 0, 1);
    if ((s32)lbl_8047A2E4 != 0) {
        fn_800FF660();
        floorSetFadeScript(0, 0);
    }
    return selectedSpecies;
}
#if !defined(MENU_POKEMON_80018A68_ONLY)
#pragma pop
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_80018F30_ONLY)
#if defined(MENU_POKEMON_ISLAND_80018F30_ONLY)
u16 fn_80018A68(void);
#endif
/* fn_80018F30 - 0x80018F30 | size: 0x24 */
extern u32 lbl_8047A2F0;
#if 0
asm void fn_80018F30(void) {
#include "src/game/gs_pcbox_fn_80018F30.inc"
}
#else
void std016_menuUnk_80018a68(void) {
    *(u16*)&lbl_8047A2F0 = fn_80018A68();
}
#endif

/* fn_80018F54 - 0x80018F54 | size: 0x34 */
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2E4;
extern u32 lbl_8047A2BC;
#if 0
asm void fn_80018F54(void) {
#include "src/game/gs_pcbox_fn_80018F54.inc"
}
#else
void fn_80018F54(u32 a, u32 b, u32 c) {
    lbl_8047A2E0 = a;
    lbl_8047A2F8 = c;
    lbl_8047A2E4 = 0;
    lbl_8047A2BC = b;
    fn_80018A68();
}
#endif

/* fn_80018F88 - 0x80018F88 | size: 0xdc */
extern u32 fightTrainer_GetHeroPtr();
extern void _flagSet();
extern void std098_mapWarpToMenuRoom();
extern u32 lbl_8047A2F4;
extern u32 lbl_8047A2F8;
extern u32 lbl_8047A2EC;
extern u32 lbl_8047A2E0;
extern u32 lbl_8047A2E4;
extern u32 lbl_8047A2F0;
#if 0
asm void fn_80018F88(void) {
#include "src/game/gs_pcbox_fn_80018F88.inc"
}
#else
u32 fn_80018F88(s32 mode, s32* ptr, u32 val) {
    if (mode == 1) {
        lbl_8047A2F4 = val;
        lbl_8047A2F8 = fightTrainer_GetHeroPtr(val);
        {
            s32 v = ptr[0];
            lbl_8047A2EC = v;
            if (v < 0 || v >= 5) {
                lbl_8047A2EC = 0;
            }
        }
    } else {
        lbl_8047A2F4 = 0;
        lbl_8047A2F8 = val;
        lbl_8047A2EC = (u32)-1;
    }
    lbl_8047A2E0 = mode;
    lbl_8047A2E4 = 1;
    _flagSet(1, 0);
    std098_mapWarpToMenuRoom(0x38f);
    floorSetFadeScript(0, 0);
    _threadSwitch();
    if (*(u16*)&lbl_8047A2F0 == 0) {
        return 0;
    }
    if (ptr != NULL) {
        ptr[0] = lbl_8047A2EC;
    }
    return *(u16*)&lbl_8047A2F0;
}
#endif

/* fn_80019064 - 0x80019064 | size: 0xc */
extern u32 lbl_8047A2B8;
#if 0
asm void fn_80019064(void) {
#include "src/game/gs_pcbox_fn_80019064.inc"
}
#else
u32 fn_80019064(void) {
    return (u8)lbl_8047A2B8;
}
#endif

/* fn_80019070 - 0x80019070 | size: 0x68 */
typedef struct MenuPokemonSpeciesCacheEntry {
    u16 species;
    u16 pad;
    u32 data;
} MenuPokemonSpeciesCacheEntry;

typedef struct MenuPokemonSpeciesCache {
    MenuPokemonSpeciesCacheEntry entries[8];
    s32 count;
} MenuPokemonSpeciesCache;

extern u8 lbl_803A1B90[];
#if 0
asm void fn_80019070(void) {
#include "src/game/gs_pcbox_fn_80019070.inc"
}
#else
u32 fn_80019070(u16 species) {
    MenuPokemonSpeciesCache* cache;
    s32 i;
    u32 result;

    cache = (MenuPokemonSpeciesCache*)lbl_803A1B90;
    result = (u32)-1;

    for (i = 0; i < cache->count; i++) {
        if (cache->entries[i].species == species) {
            {
                /* RULE-EXCEPTION(user-approved): block-local entry pointer chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
                MenuPokemonSpeciesCacheEntry* e = (MenuPokemonSpeciesCacheEntry*)lbl_803A1B90;
                e += i;
                result = e->data;
            }
            break;
        }
    }
    cache->count = 0;
    return result;
}
#endif

/* fn_800190D8 - 0x800190D8 | size: 0x40 */
#if 0
asm void fn_800190D8(void) {
#include "src/game/gs_pcbox_fn_800190D8.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma push
#endif
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma optimization_level 4
#endif
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma peephole off
#endif
void std017_menuQueueAdd(u32 species, u32 data) {
    MenuPokemonSpeciesCache* cache;
    s32 count;

    cache = (MenuPokemonSpeciesCache*)lbl_803A1B90;
    count = cache->count;
    if (count >= 8) {
        return;
    }
    species = (u16)species;
    cache->entries[count].species = species;
    cache->entries[cache->count].data = data;
    cache->count = cache->count + 1;
}
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma pop
#endif
#endif

/* fn_80019118 - 0x80019118 | size: 0xec */
extern void winSpriteSetDisp();
extern u8 lbl_80266C10[];
extern u32 lbl_8047B7A4;
extern u32 lbl_8047A300;
extern u32 lbl_8047B7A0;
#if 0
asm void fn_80019118(void) {
#include "src/game/gs_pcbox_fn_80019118.inc"
}
#else
/* Matching trick: use if/else (not ternary) for cmpw+bne branch pattern;
   declare r30 as u8 to get clrlwi r0,r30,24 + cmplwi (not record form) */
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma peephole off
#endif
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma optimization_level 4
#endif
s32 fn_80019118(u8* a, u8* b) {
    u32 sp[4];
    u8* r4;
    s32 r7;
    u32 r0;
    u32 r30;
    s32 r4_val;

    r4 = *(u8**)(a + 0x60);
    sp[0] = *(u32*)(lbl_80266C10 + 0x0);
    sp[1] = *(u32*)(lbl_80266C10 + 0x4);
    sp[2] = *(u32*)(lbl_80266C10 + 0x8);
    sp[3] = *(u32*)(lbl_80266C10 + 0xC);
    r7 = (s32)(s8)a[0x95] + (s32)(4 - *(u32*)(r4 + 0xC));
    if (r7 < 0 || r7 >= 4) return 0;
    r0 = sp[r7];
    r4_val = (s32)(s16)*(s16*)(b + 0x6);
    if (r4_val == (s32)r0) {
        r30 = 1;
    } else {
        r30 = 0;
    }
    winSpriteSetDisp(b, r30);
    if ((u8)r30 != 0) {
        *(u8*)(b + 0x67) = *(f32*)&lbl_8047B7A0 * (*(f32*)&lbl_8047B7A4 - *(f32*)&lbl_8047A300);
    }
    return 0;
}
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma peephole reset
#endif
#endif

/* fn_80019204 - 0x80019204 | size: 0xa4 */
extern u8 lbl_80266C00[];
#if 0
asm void fn_80019204(void) {
#include "src/game/gs_pcbox_fn_80019204.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma optimization_level 4
#endif
s32 fn_80019204(u8* a, u8* b) {
#if !defined(NO_LOCAL_PRAGMAS_MENUPOKEMON_ISLAND_80018F30)
#pragma peephole off
#endif
    extern void winSpriteSetDisp();
    s32 table[4];
    u8* r5;
    s32 r8;
    s32 r0;
    s32 r6;
    u32 r4;
    r5 = *(u8**)((u8*)a + 0x60);
    table[0] = *(u32*)(lbl_80266C00 + 0x0);
    table[1] = *(u32*)(lbl_80266C00 + 0x4);
    table[2] = *(u32*)(lbl_80266C00 + 0x8);
    table[3] = *(u32*)(lbl_80266C00 + 0xC);
    r8 = (s32)((s8)*(u8*)(a + 0x95)) + (s32)(4 - *(u32*)(r5 + 0xC));
    if (r8 < 0 || r8 >= 4) return 0;
    r0 = table[r8];
    r6 = (u32)(s32)(s16)*(s16*)(b + 0x6);
    if (r6 == r0) {
        r4 = 1;
    } else {
        r4 = 0;
    }
    winSpriteSetDisp(b, r4);
    return 0;
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY)
/* fn_800192A8 - 0x800192A8 | size: 0x228 */
extern u32 itemDataBiosGetName(u32 a);
extern void fn_800FB680();
extern u32 itemDataBiosGetKind(u32 a);
extern u32 GSmsgGetRect(u32 a);
extern void fn_80142CF4(void);
extern u8 lbl_80266BF0[];

typedef struct MenuPokemonSlotIds {
    s32 ids[4];
} MenuPokemonSlotIds;

#if 0
asm void fn_800192A8(void) {
#include "src/game/gs_pcbox_fn_800192A8.inc"
}
#else
#pragma optimization_level 4
s32 fn_800192A8(u8* a, u8* b) {
    extern void* itemDataBiosGetPtr();
    extern void* heroItemGetItemKindToItemAryPtr();
    extern void msgctrlSetValue();
    extern u32 itemGetStatus();
    extern u32 itemDataBiosGetName(void* item);
    extern u32 itemDataBiosGetKind(void* item);
    s32 slots[4];
    u16 count;
    u16 total;
    u8* data;
    u32 sum;
    s16 width;
    u32 name;
    u16 item;
    u8* entry;
    s32 n;
    s32 i;

    data = *(u8**)(a + 0x60);
    *(MenuPokemonSlotIds*)slots = *(MenuPokemonSlotIds*)lbl_80266BF0;
    for (i = 0; i < 4; i++) {
        if (*(s16*)(b + 0x6) == slots[i]) {
            break;
        }
    }
    i -= 4 - *(s32*)(data + 0xC);
    if (i < 0 || i >= *(s32*)(data + 0xC)) {
        return 0;
    }
    if (*(u16*)(data + 4 + i * 2) == 0) {
        name = 0x134;
    } else {
        name = itemDataBiosGetName(itemDataBiosGetPtr(*(u16*)(data + 4 + i * 2)));
    }
    fn_800FB680(0, 0, -1, name);
    if (*(u16*)(data + 4 + i * 2) != 0) {
        heroItemGetItemKindToItemAryPtr(0, itemDataBiosGetKind(itemDataBiosGetPtr(*(u16*)(data + 4 + i * 2))), 0, &count, 0, 0);
        msgctrlSetValue(0x34, count);
        width = (s16)(GSmsgGetRect(0xCA) >> 16);
        width += (s16)(GSmsgGetRect(0x12E) >> 16);
        fn_800FB680(0xC3 - width, 0, -1, 0x12E);

        item = *(u16*)(data + 4 + i * 2);
        sum = 0;
        entry = heroItemGetItemKindToItemAryPtr(0, itemDataBiosGetKind(itemDataBiosGetPtr(item)), &total, 0, 0, 0);
        for (n = 0; n < total; n++, entry += 4) {
            if (fn_801429E8(entry) != 0 && itemGetStatus(entry, 0, 0x1B, 0) == item) {
                sum += itemBiosGetNum(entry);
            }
        }
        msgctrlSetValue(0x34, sum);
        fn_800FB680(0xC3 - (s16)(GSmsgGetRect(0xCA) >> 16), 0, -1, 0xCA);
    }
    return 0;
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_800194D0_ONLY)
/* fn_800194D0 - 0x800194D0 | size: 0x14 */
#if 0
asm void fn_800194D0(void) {
#include "src/game/gs_pcbox_fn_800194D0.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimization_level 4
s32 fn_800194D0(u8* a, u8* b) {
    u8* r5;
    s32 ret = 0;
    r5 = *(u8**)(a + 0x60);
    *(u32*)(b + 0x64) = *(u32*)r5;
    return ret;
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_800194E4_ONLY)
/* fn_800194E4 - 0x800194E4 | size: 0xfc */
extern u8 lbl_802E4E10[];
#if 0
asm void fn_800194E4(void) {
#include "src/game/gs_pcbox_fn_800194E4.inc"
}
#else
#pragma optimization_level 4
/* RULE-EXCEPTION(user-approved): local propagation control preserves the
 * consumed loop index as retail's byte offset; see docs/RULE_EXCEPTIONS.md. */
#pragma opt_propagation off
typedef struct MenuPokemonSlotPos {
    s32 id;
    s16 x;
    s16 y;
    s32 shift;
} MenuPokemonSlotPos;

s32 fn_800194E4(u8* a, u8* b) {
    u8* data;
    s32 i;
    s32 pos;
    MenuPokemonSlotPos* slot;
    s32 offset;

    data = *(u8**)(a + 0x60);
    if (*(s16*)(b + 0x6) != 0x26c) {
        *(u32*)(b + 0x64) = *(u32*)(data + 0x0);
    }
    pos = *(s32*)(data + 0xC);
    if (pos < 4) {
        slot = (MenuPokemonSlotPos*)lbl_802E4E10;
        for (i = 0; i < 6; slot++, i++) {
            if (*(s16*)(b + 0x6) == slot->id) {
                break;
            }
        }
        if (i < 6) {
            i *= sizeof(MenuPokemonSlotPos);
            offset = (4 - pos) * 0x1f;
            slot = (MenuPokemonSlotPos*)(lbl_802E4E10 + i);
            *(s16*)(b + 0x52) = offset + slot->x;
            if (slot->shift != 0) {
                *(s16*)(b + 0x56) = slot->y - offset;
            }
        }
    }
    return 0;
}
#pragma opt_propagation reset
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_800195E0_ONLY)
/* fn_800195E0 - 0x800195E0 | size: 0xa0 */
#if 0
asm void fn_800195E0(void) {
#include "src/game/gs_pcbox_fn_800195E0.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off; optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
#pragma optimization_level 4
s32 fn_800195E0(u8* a) {
    extern u8* windowGetKeyInfo();
    u8* r3;
    u8* r30;
    u32 r31;
    s32 r5;
    s32 r0;
    s32 r4;
    r30 = a;
    r31 = *(u32*)(a + 0x60);
    r3 = windowGetKeyInfo();
    r5 = *(s32*)((u8*)r31 + 0xC);
    r0 = *(u16*)(r3 + 0x6) & 0x2;
    if (r0 != 0) {
        r4 = (s32)(s8)*(u8*)(r30 + 0x95);
        r0 = r4 + 1;
        if (r0 >= r5) r0 = r5 - 1;
        *(s8*)(r30 + 0x95) = (s8)r0;
    }
    r0 = *(u16*)(r3 + 0x6) & 0x1;
    if (r0 != 0) {
        r4 = (s32)(s8)*(u8*)(r30 + 0x95);
        r0 = r4 - 1;
        if (r0 < 0) r0 = 0;
        *(s8*)(r30 + 0x95) = (s8)r0;
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019680 - 0x80019680 | size: 0xd4 */
extern u32 lbl_8047B7A8;
extern u32 lbl_8047A300;
extern u32 lbl_8047B7AC;
extern u32 lbl_8047B7A4;
#if 0
asm void fn_80019680(void) {
#include "src/game/gs_pcbox_fn_80019680.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_80019680(u8* arg) {
    s32 val;
    val = (s8)arg[1];
    switch (val) {
    case 0:
        if ((s8)arg[2] == 0) {
            winSeqSetMenu((void*)0x5f, 0x66);
            *(f32*)&lbl_8047A300 = *(f32*)&lbl_8047B7A8;
            arg[2] = 1;
        }
        break;
    case 2: {
        f32 f1;
        f1 = *(f32*)&lbl_8047A300 + *(f32*)&lbl_8047B7AC;
        *(f32*)&lbl_8047A300 = f1;
        if (f1 > *(f32*)&lbl_8047B7A4) {
            *(f32*)&lbl_8047A300 = *(f32*)&lbl_8047B7A8;
        }
        break;
    }
    case 3:
        if ((s8)arg[2] == 0) {
            winSeqSetMenu((void*)0x5f, 0x6a);
            arg[2] = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_80019754_ONLY)
/* fn_80019754 - 0x80019754 | size: 0x1e4 */
extern u32 heroItemCheckHaveItemDataId();
extern u32 lbl_80478868;
extern u32 lbl_80478BD8;
extern u8  lbl_802E4E10[];
extern u8  lbl_802EF0A8[];
#if 0
asm void fn_80019754(void) {
#include "src/game/gs_pcbox_fn_80019754.inc"
}
#else
/* RULE-EXCEPTION(user-approved): single-use inline helper - the owned kind-6 item scan is a static inline, as in retail: inlined, its count = 0 and loop counter start as `li r30,0; mr r31,r30` (the counter copies count's zero) with the counter in r31, count in r30 and the list cursor in r29; written in place the two start as separate `li 0` and the three swap registers (95.58% without) - see docs/RULE_EXCEPTIONS.md */
static inline s32 fn_80019754_collectItems(u16* ids) {
    extern void* itemDataBiosGetPtr(u16 itemId);
    extern u8 itemDataBiosGetKind(void* itemData);
    extern u8 heroItemCheckHaveItemDataId(s32 hero, u16 itemId);
    u16 i;
    s32 count;

    count = 0;
    for (i = 0; i < lbl_80478BD8 && count < 3; i++) {
        if (itemDataBiosGetKind(itemDataBiosGetPtr(i)) == 6) {
            if (heroItemCheckHaveItemDataId(0, i)) {
                ids[count] = i;
                count++;
            }
        }
    }
    return count;
}

u16 fn_80019754(u32 color) {
    extern s32 windowGetActiveID(void);
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
    extern void menuClose(s32);
    extern void menuCloseSync(s32, s32);
    struct {
        u32 color;
        u16 ids[4];
        s32 count;
    } list;
    u8* e;
    s16* p4;
    s16* p8;
    s32 count;
    s32 sel;
    s32 r4;

    if ((s32)lbl_80478868 != 0) {
        e = lbl_802E4E10;
        p4 = (s16*)(lbl_802EF0A8 + 0x4);
        p8 = (s16*)(lbl_802EF0A8 + 0x8);
        *(s16*)(e + 0x4)  = p4[*(u32*)(e + 0x0)  * 0xe];
        *(s16*)(e + 0x6)  = p8[*(u32*)(e + 0x0)  * 0xe];
        *(s16*)(e + 0x10) = p4[*(u32*)(e + 0xc)  * 0xe];
        *(s16*)(e + 0x12) = p8[*(u32*)(e + 0xc)  * 0xe];
        *(s16*)(e + 0x1c) = p4[*(u32*)(e + 0x18) * 0xe];
        *(s16*)(e + 0x1e) = p8[*(u32*)(e + 0x18) * 0xe];
        *(s16*)(e + 0x28) = p4[*(u32*)(e + 0x24) * 0xe];
        *(s16*)(e + 0x2a) = p8[*(u32*)(e + 0x24) * 0xe];
        *(s16*)(e + 0x34) = p4[*(u32*)(e + 0x30) * 0xe];
        *(s16*)(e + 0x36) = p8[*(u32*)(e + 0x30) * 0xe];
        *(s16*)(e + 0x40) = p4[*(u32*)(e + 0x3c) * 0xe];
        *(s16*)(e + 0x42) = p8[*(u32*)(e + 0x3c) * 0xe];
        lbl_80478868 = 0;
    }

    list.color = color;
    count = fn_80019754_collectItems(list.ids);
    list.ids[count] = 0;
    list.count = count + 1;

    r4 = windowGetActiveID();
    sel = menuOpenCustom(0x5f, r4, 0, 0, 1, 1, &list);
    menuClose(0x5f);
    menuCloseSync(0x5f, 1);
    if (sel >= 0 && sel < list.count) {
        /* RULE-EXCEPTION(user-approved): codegen-only index cast (sel is already known >= 0); keeps MWCC from sharing one &list.ids register between the terminator store and this load, as retail recomputes it — see docs/RULE_EXCEPTIONS.md */
        return list.ids[(u32)sel];
    }
    return 0;
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_80019938_ONLY)
/* fn_80019938 - 0x80019938 | size: 0xbc */
extern u8* windowGetAllocPtr();
#if 0
asm void fn_80019938(void) {
#include "src/game/gs_pcbox_fn_80019938.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimization_level 4) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimization_level 4
void fn_80019938(u8* a, u8* b) {
    extern void msgctrlSetValue(s32, u32);
    extern void fn_800FB680(s32, s32, s32, u32);
    u8* base;
    s16 r0;
    u32 r4;
    u32 r4v;
    base = windowGetAllocPtr();
    r0 = *(s16*)(b + 0x6);
    r4 = 0x0;
    switch (r0) {
    case (s16)0xe93:
        r4 = 0;
        break;
    case (s16)0xe94:
        r4 = 1;
        break;
    case (s16)0xe95:
        r4 = 2;
        break;
    case (s16)0xe96:
        r4 = 3;
        break;
    }
    base = base + r4 * 0xc;
    r4v = *(u32*)(base + 0x4);
    if (r4v == 0) return;
    msgctrlSetValue(0x37, r4v);
    fn_800FB680(0x0, 0x0, (s32)*(u8*)(a + 0x8b) | (s32)(-0x100), 0xe7);
}
#endif

/* fn_800199F4 - 0x800199F4 | size: 0x128 */
extern void* windowAllocMemory();
extern void menuItemBiosSetSelectFlag();
extern void* memcpy(void* dst, const void* src, u32 n);
extern u32 lbl_80478870;
#if 0
asm void fn_800199F4(void) {
#include "src/game/gs_pcbox_fn_800199F4.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_800199F4(u8* arg) {
    u8* entry;
    u16* ids;
    s32 i;
    void* dst;

    if ((s8)arg[1] == 0) {
        dst = windowAllocMemory(arg, 0x48);
        if (dst != NULL) {
            memcpy(dst, *(void**)(arg + 0x60), 0x48);
        }
    }
    {
        u8* tmp;
        tmp = windowGetAllocPtr(arg);
        i = 0;
        ids = (u16*)&lbl_80478870;
        entry = tmp;
    }
    while (i < 4) {
        if (*(u32*)(entry + 4) != 0) {
            menuItemBiosSetSelectFlag(*ids, 1);
        } else {
            menuItemBiosSetSelectFlag(*ids, 0);
        }
        entry += 0xc;
        ids++;
        i++;
    }
    {
        s32 val;
        val = menuGetCursorItemID(*(u32*)(arg + 4));
        switch (val) {
        case 0xe93:
            *(u32*)(arg + 0x80) = 0;
            break;
        case 0xe94:
            *(u32*)(arg + 0x80) = 1;
            break;
        case 0xe95:
            *(u32*)(arg + 0x80) = 2;
            break;
        case 0xe96:
            *(u32*)(arg + 0x80) = 3;
            break;
        default:
            *(s32*)(arg + 0x80) = -1;
            break;
        }
    }
    return 0;
}
#pragma peephole reset
#endif

/* fn_80019B1C - 0x80019B1C | size: 0x2c */
#if 0
asm void fn_80019B1C(void) {
#include "src/game/gs_pcbox_fn_80019B1C.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; optimization_level 4; scheduling off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void fn_80019B1C(void) {
    extern void menuCloseCustom();
    menuCloseCustom(0x42, 0x0, 0x1);
}
#pragma pop
#endif

#endif /* !MENU_POKEMON_CARVE_ONLY */

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_80019B48_ONLY)
/* fn_80019B48 - 0x80019B48 | size: 0x214 */
extern void pokemonToMenuWazaStatus(void);
extern u8 lbl_803A1BD8[];
#if 0
asm void fn_80019B48(void) {
#include "src/game/gs_pcbox_fn_80019B48.inc"
}
#else
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 4
#pragma peephole off
s32 fn_80019B48(s8 slot) {
    extern u8 lbl_803A1D40[];
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, ...);
    extern void menuCloseCustom();
    void* pokemon;
    s32 result;

    pokemon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), slot,
                                    *(void**)(lbl_803A1D40 + 0xC));
    if (pokemon == 0) {
        return -1;
    }
    ((void (*)(void*, u8*))pokemonToMenuWazaStatus)(pokemon, lbl_803A1BD8);
    result = menuOpenCustom(0x42, 0, 0, 0, 1, 1, lbl_803A1BD8);
    menuCloseCustom(0x42, 0, 1);
    return result;
}
#pragma pop
#endif
#endif

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_80019D5C_ONLY)

/* fn_80019D5C - 0x80019D5C | size: 0x210 */
#if 0
asm void fn_80019D5C(void) {
#include "src/game/gs_pcbox_fn_80019D5C.inc"
}
#else
void* fn_80019D5C(void* current, s32 dir) {
    extern u8 lbl_803A1D40[];
    s8* work = (s8*)lbl_803A1D40;
    s8 slot;
    void* pokemon;

    slot = work[6];
    switch (dir) {
    case 1:
        slot--;
        break;
    case 2:
        slot++;
        break;
    default:
        return current;
    }
    if (slot >= 6) {
        slot = 5;
    }
    if (slot < 0) {
        slot = 0;
    }
    if (slot != work[6]) {
        pokemon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), slot,
                                        *(void**)(lbl_803A1D40 + 0xC));
        if (pokemon != 0) {
            current = pokemon;
            work[6] = slot;
        }
    }
    return current;
}
#endif
#endif /* !MENU_POKEMON_CARVE_ONLY || MENU_POKEMON_80019D5C_ONLY */

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_80019F6C_ONLY)

#if defined(MENU_POKEMON_80019F6C_ONLY)
/* First declarations the full file has made by this point. */
extern u16 itemDataBiosGetPtr(u16 speciesId);
extern void msgctrlSetValue();
extern u8 lbl_803A1D40[];
extern u8 lbl_803A1C20[];
extern s32 menuGetCursorItemID();
extern u8 lbl_802E4E58[];
extern void winSpriteSetDisp();
extern void fn_800FB680();
extern u32 itemDataBiosGetKind(u32 a);
extern u32 GSmsgGetRect(u32 a);
extern u8* windowGetKeyInfo();
extern s32 windowGetParam(s32, s32);
extern u32 itemDataBiosGetWazaMachineNo();
extern u32 pokemonIsDarkPokemon();
extern u32 pokemonDataBiosGetWazaMcn();
extern void fn_800FBB34();
extern void fn_8010B9E8();
extern u32 fn_80107E78();
extern void fn_801081F8();
extern void windowDrawSprite2();
extern u32 pokemonDataBiosGetPtr();
extern void fn_8001DACC();
extern u32 GSmsgGetGSchar();
extern u32 lbl_8047A308;
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7B0;
extern u32 menuSubCalcColor();
/* The carve builds with -opt nopeephole (configure.py), the pragma state the
   full file has reached at this point. */
#endif

/* menuPokemonDrawItem - 0x80019F6C | size: 0xa18 */
extern s32 windowGetParam(s32, s32);
extern u32 itemDataBiosGetWazaMachineNo();
extern u32 pokemonIsDarkPokemon();
extern u32 pokemonDataBiosGetWazaMcn();
extern void fn_800FBB34();
extern void fn_8010B9E8();
extern u32 fn_80107E78();
extern void fn_801081F8();
extern void windowDrawSprite2();
extern u32 pokemonDataBiosGetPtr();
extern void fn_8001DACC();
extern u32 GSmsgGetGSchar();
extern u32 lbl_8047A308;
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7B0;
extern u32 menuSubCalcColor();
#if 0
asm void menuPokemonDrawItem(void) {
#include "src/game/gs_pcbox_menuPokemonDrawItem.inc"
}
#else
void menuPokemonDrawItem(u8* ctx, u8* pane) {
    extern void* itemDataBiosGetPtr(u16 itemId);
    extern u8 itemDataBiosGetKind(void* item);
    u8* entry;
    void* mon;
    u32 msgId;
    s8 state;
    u32 paneColor;
    s32 visible;
    s32 color;
    u16 index;
    void* item;
    s32 cur;
    s32 max;
    s32 grade;
    s16 width;
    s32 x;
    s32 value;
    s32 msg;

    entry = (u8*)windowGetParam((s32)ctx, 0);
    msgId = 0;
    state = 0;
    if (entry == NULL) {
        return;
    }
    if (*(u16*)(entry + 0) == 0) {
        state = -1;
    } else if (*(s16*)(entry + 0x1A) == 0) {
        state = 1;
    }

    visible = 1;
    paneColor = -1;
    if (state == -1) {
        switch (*(s16*)(pane + 6)) {
        case 0x3BA: case 0x3BB: case 0x3BC: case 0x3BD:
        case 0x3BE: case 0x3BF: case 0x3C0: case 0x3C1:
        case 0x3C4: case 0x3C9: case 0x3CA: case 0x3CB:
        case 0x545: case 0x546: case 0x547:
        case 0x12A0: case 0x12A1: case 0x12A2:
            visible = 0;
            break;
        }
    } else if (state == 1) {
        switch (*(s16*)(pane + 6)) {
        case 0x3B8: case 0x3B9:
        case 0x3C4: case 0x3C5: case 0x3C6: case 0x3C7: case 0x3C8:
        case 0x129D: case 0x129E:
        case 0x12A0:
            paneColor = 0x808080FF;
            break;
        }
    }

    winSpriteSetDisp(pane, visible);
    *(u32*)(pane + 0x64) = paneColor;
    if ((u8)visible == 0) {
        return;
    }

    color = (s32)menuSubCalcColor(ctx, pane);
    index = windowGetParam((s32)ctx, 1);

    if (lbl_803A1D40[0] == 3 || lbl_803A1D40[0] == 4) {
        item = itemDataBiosGetPtr(*(u16*)(lbl_803A1D40 + 0x12));
        if ((u8)itemDataBiosGetKind(item) == 4) {
            switch (*(s16*)(pane + 6)) {
            case 0x3BC:
            case 0x12A1:
                msgId = itemDataBiosGetWazaMachineNo(item);
                mon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), index,
                                            *(void**)(lbl_803A1D40 + 0xC));
                if ((u8)pokemonIsDarkPokemon(mon) != 0) {
                    msg = 0x2B65;
                } else if ((u8)pokemonDataBiosGetWazaMcn(pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(mon)), (u8)msgId) == 0) {
                    msg = 0x2B65;
                } else {
                    msg = 0x2B64;
                }
                fn_800FB680(0, -4, color, msg);
            case 0x3BE:
            case 0x12A2:
                winSpriteSetDisp(pane, 0);
                return;
            }
        }
    }

    switch (*(s16*)(pane + 6)) {
    case 0x3C2:
    case 0x3C5:
    case 0x3C6:
    case 0x129D:
    case 0x129E:
        if (entry[0x29] == 0) {
            winSpriteSetDisp(pane, 1);
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3B8:
    case 0x3B9:
    case 0x3C3:
    case 0x3C7:
    case 0x3C8:
        switch (entry[0x29]) {
        case 0:
            winSpriteSetDisp(pane, 0);
            break;
        case 1:
            winSpriteSetDisp(pane, 1);
            break;
        case 2:
            fn_8001DACC(ctx, pane, (f32)*(s16*)&lbl_8047A308 / *(f32*)&lbl_8047B7B0);
            winSpriteSetDisp(pane, 0);
            break;
        }
        break;
    case 0x3BA:
    case 0x3CB:
        if (*(u16*)(entry + 0x24) != 0) {
            windowDrawSprite2(0, 0, *(s16*)(pane + 0x54), *(s16*)(pane + 0x56), color, ctx, *(u16*)(entry + 0x24), 0);
        }
        break;
    case 0x3BB:
    case 0x3CA:
        if (*(u16*)(entry + 0x2A) != 0) {
            winSpriteSetDisp(pane, 1);
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3BF:
        fn_800FB680(0x2E, 0, color, 0x2BD4);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x1A));
        fn_800FBB34(0, 0, 0x2C, *(s16*)(pane + 0x56), color, 0xDE);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x18));
        fn_800FBB34(0, 0, (s16)(*(s16*)(pane + 0x54) - 2), *(s16*)(pane + 0x56), color, 0xDE);
        break;
    case 0x545:
        fn_800FB680(0x1B, 0, color, 0x195);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x1A));
        fn_800FBB34(0, 0, 0x1B, *(s16*)(pane + 0x56), color, 0xDF);
        msgctrlSetValue(0x34, *(s16*)(entry + 0x18));
        fn_800FBB34(0, 0, (s16)(*(s16*)(pane + 0x54) - 2), *(s16*)(pane + 0x56), color, 0xDF);
        break;
    case 0x3C0:
    case 0x546:
        msgctrlSetValue(0x34, entry[0x17]);
        fn_800FBB34(0, 0, *(s16*)(pane + 0x54), *(s16*)(pane + 0x56), color, 0xD3);
        break;
    case 0x3C1:
    case 0x547:
        msgctrlSetValue(0x37, entry);
        fn_800FB680(0, 0, color, 0xE7);
        x = (s16)(GSmsgGetRect(0xE7) >> 16);
        switch (entry[0x28]) {
        case 0:
            msgId = 0xD67;
            break;
        case 1:
            msgId = 0xD68;
            break;
        case 2:
            msgId = 0;
            break;
        }
        if (msgId != 0) {
            msgctrlSetValue(0x37, GSmsgGetGSchar(msgId));
            fn_800FB680(x - 2, 0, color, 0xCF);
        }
        break;
    case 0x3C4:
    case 0x12A0:
        fn_8010B9E8(ctx, pane, *(u16*)(entry + 0x26));
        break;
    case 0x3BE:
    case 0x12A2:
        cur = *(s16*)(entry + 0x1A);
        max = *(s16*)(entry + 0x18);
        if (cur <= 0) {
            grade = 0;
        } else if (cur <= max * 20 / 100) {
            grade = 0x66;
        } else if (cur <= max * 50 / 100) {
            grade = 0x65;
        } else {
            grade = 0x64;
        }
        width = (cur * *(s16*)(pane + 0x54) + (max - 1)) / max;
        if ((u16)grade != 0) {
            windowDrawSprite2(0, 0, width, *(s16*)(pane + 0x56), color, ctx, grade, 0);
        }
        break;
    case 0x543:
    case 0x12A4:
        if ((s8)lbl_803A1D40[7] >= 0 && (s8)lbl_803A1D40[7] < 6) {
            value = *(s16*)(lbl_802E4E58 + (s8)lbl_803A1D40[4] * 0x30 + (s8)lbl_803A1D40[7] * 8);
        } else {
            value = 0;
        }
        if (value == *(s32*)(ctx + 4)) {
            winSpriteSetDisp(pane, 1);
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x542:
    case 0x12A3:
        if ((s8)lbl_803A1D40[6] >= 0 && (s8)lbl_803A1D40[6] < 6) {
            value = *(s16*)(lbl_802E4E58 + (s8)lbl_803A1D40[4] * 0x30 + (s8)lbl_803A1D40[6] * 8);
        } else {
            value = 0;
        }
        if (value == *(s32*)(ctx + 4)) {
            winSpriteSetDisp(pane, 1);
            switch (lbl_803A1D40[0x14]) {
            case 0:
            case 2:
                if ((u8)fn_80107E78(ctx, (u16)*(s16*)(pane + 6), 0x2D) == 0) {
                    fn_801081F8(ctx, (u16)*(s16*)(pane + 6), 0x2D);
                }
                break;
            case 1:
            case 3:
                if ((u8)fn_80107E78(ctx, (u16)*(s16*)(pane + 6), 0x20D) == 0) {
                    fn_801081F8(ctx, (u16)*(s16*)(pane + 6), 0x20D);
                }
                break;
            }
        } else {
            winSpriteSetDisp(pane, 0);
        }
        break;
    case 0x3B6:
    case 0x3B7:
        switch (lbl_803A1D40[0x14]) {
        case 0:
        case 2:
            winSpriteSetDisp(pane, 1);
            break;
        case 1:
        case 3:
            winSpriteSetDisp(pane, 0);
            break;
        }
        break;
    }
}
#endif

/* menuPokemonDrawHelp - 0x8001A984 | size: 0x114 */
#if 0
asm void menuPokemonDrawHelp(void) {
#include "src/game/gs_pcbox_menuPokemonDrawHelp.inc"
}
#else
#if !defined(MENU_POKEMON_80019F6C_ONLY)
#pragma optimization_level 4
#endif
void menuPokemonDrawHelp(u8* a) {
    extern u8 lbl_803A1D40[];
    extern u8 lbl_803A1C20[];
    extern u32 itemDataBiosGetPtr();
    extern void msgctrlSetValue();
    extern void fn_800FB680();
    u32 msg;

    msg = 0;
    switch (*(u8*)(lbl_803A1D40 + 0x14)) {
    case 0:
        switch (*(u8*)(lbl_803A1D40 + 0x0)) {
        case 3:
        case 4:
            if ((itemDataBiosGetKind(itemDataBiosGetPtr(*(u16*)(lbl_803A1D40 + 0x12))) & 0xFF) == 4) {
                msg = 0x2b63;
            } else {
                msg = 0x2b61;
            }
            break;
        case 5:
            msg = 0x2b62;
            break;
        case 1:
        case 2:
        case 6:
        case 7:
        default:
            msg = 0x2b5d;
            break;
        }
        break;
    case 1:
        msgctrlSetValue(0x32, lbl_803A1C20 + (s8)*(u8*)(lbl_803A1D40 + 0x6) * 0x30);
        msg = 0x2b5e;
        break;
    case 2:
        msg = 0x2b5f;
        break;
    case 3:
        msg = 0x2b60;
        break;
    }
    if (msg != 0) {
        fn_800FB680(0, 0, *(u8*)(a + 0x8b) | -0x100, msg);
    }
}
#endif

/* menuPokemonButton - 0x8001AA98 | size: 0xd8 */
extern void soundStartSE(void);
extern void menuButtonNormal(void);
#if 0
asm void menuPokemonButton(void) {
#include "src/game/gs_pcbox_menuPokemonButton.inc"
}
#else
#if !defined(MENU_POKEMON_80019F6C_ONLY)
#pragma push
#pragma optimization_level 4
#pragma peephole off
#endif
void menuPokemonButton(u8* a) {
    extern u8 lbl_803A1D40[];
    extern u32 windowGetKeyInfo();
    extern u32 menuGetCursorItemID();
    extern void soundStartSE(u32);
    extern void menuButtonNormal(u8*);
    s32 r3;
    u32 r31;
    u32 r3b;
    r31 = 0;
    if (*(u8*)(lbl_803A1D40 + 0x1) == 0) {
        *(u8*)(a + 0x98) = 0x1;
        return;
    }
    r3 = windowGetKeyInfo();
    r3 = *(u16*)((u8*)r3 + 0x4);
    if ((r3 & 0x10) != 0) {
        r3b = menuGetCursorItemID(*(u32*)(a + 0x4));
        if ((r3b & 0xFFFF) == 0x3b6) r31 = 1;
    } else if ((r3 & 0x20) != 0) {
        r31 = 1;
    }
    if ((u8)r31 != 0) {
        if (*(u8*)(lbl_803A1D40 + 0x15) == 0) {
            soundStartSE(0x26);
            return;
        }
        *(u8*)(a + 0x98) = 0x1;
        *(u8*)(a + 0x99) = 0x1;
        return;
    }
    menuButtonNormal(a);
}
#if !defined(MENU_POKEMON_80019F6C_ONLY)
#pragma pop
#endif
#endif

/* fn_8001AB70 - 0x8001AB70 | size: 0x3d4 */
extern u8 menuDataBiosGetType(void*);
#if 0
asm void fn_8001AB70(void) {
#include "src/game/gs_pcbox_fn_8001AB70.inc"
}
#else
void fn_8001AB70(u8* ctx) {
    extern u8* windowGetKeyInfo();
    u8 valid[8];
    s32 i;
    s8 cursor;
    s8 step;
    u16 buttons;
    s32 count;

    cursor = *(s8*)(ctx + 0x95);
    step = 0;
    for (i = 0; i < 6; i++) {
        if (menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), i,
                                  *(void**)(lbl_803A1D40 + 0xC)) != 0) {
            valid[i] = 1;
        } else {
            valid[i] = 0;
        }
    }
    valid[i] = 1;

    buttons = *(u16*)(windowGetKeyInfo() + 6);
    if ((buttons & 8) != 0) {
        step = 1;
        if (((s8*)lbl_803A1D40)[4] == 0) {
            if (cursor == 0) {
                if (((s8*)lbl_803A1D40)[5] == 3) {
                    cursor = ((s8*)lbl_803A1D40)[5];
                } else {
                    cursor = 2;
                }
            }
        } else if (cursor == 0) {
            if (((s8*)lbl_803A1D40)[5] == 3) {
                cursor = ((s8*)lbl_803A1D40)[5];
            } else {
                cursor = 2;
            }
        } else if (cursor == 1) {
            if (((s8*)lbl_803A1D40)[5] == 5) {
                cursor = ((s8*)lbl_803A1D40)[5];
            } else {
                cursor = 4;
            }
        }
    } else if ((buttons & 4) != 0) {
        step = -1;
        if (((s8*)lbl_803A1D40)[4] == 0) {
            switch (cursor) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                cursor = 0;
                break;
            }
        } else if (((s8*)lbl_803A1D40)[4] == 1) {
            switch (cursor) {
            case 2:
            case 3:
                cursor = 0;
                break;
            case 4:
            case 5:
                cursor = 1;
                break;
            }
        }
    }

    if ((buttons & 1) != 0) {
        cursor--;
        step = -1;
        if (cursor < 0) {
            cursor = 6;
        }
    } else if ((buttons & 2) != 0) {
        step = 1;
        count = menuDataBiosGetType(*(void**)(ctx + 4));
        cursor++;
        if (cursor >= count) {
            cursor = 0;
        }
    }

    if (step != 0) {
        while (valid[cursor] == 0) {
            cursor += step;
            if (cursor < 0 || cursor >= 7) {
                cursor = 7;
                break;
            }
        }
    }

    if (*(s8*)(ctx + 0x95) != cursor) {
        ((s8*)lbl_803A1D40)[5] = *(s8*)(ctx + 0x95);
        ((s8*)lbl_803A1D40)[6] = cursor;
        *(s8*)(ctx + 0x95) = cursor;
    }
}
#endif
#endif /* !MENU_POKEMON_CARVE_ONLY || MENU_POKEMON_80019F6C_ONLY */

#if !defined(MENU_POKEMON_CARVE_ONLY)

/* menuPokemonCtrl - 0x8001AF44 | size: 0x240 */
extern void menuItemBiosSetXY(s16 x, s16 y, s16 z);
extern u32 lbl_8047A308;
#if 0
asm void menuPokemonCtrl(void) {
#include "src/game/gs_pcbox_fn_8001AF44.inc"
}
#else
#pragma push
#pragma peephole off
s32 menuPokemonCtrl(s32 ctx) {
    extern u8 lbl_803A1D40[];
    extern u8 lbl_802E4E58[];
    extern s32 menuOpenCustom(s32, s32, s32, s32, s32, s32, void*, ...);
    u32 result;
    s32 byte_off;
    u8* iter;
    s32 i;

    result = windowGetParam(ctx, 0);
    if (result == 0) return 0;

    if ((s8)*((u8*)ctx + 1) == 0) {
        *((s8*)ctx + 0x97) = -1;
        if ((s8)*((u8*)ctx + 2) == 0) {
            i = 0;
            byte_off = 0;
            iter = (u8*)result;
            for (; i < 6; i++) {
                s16 sy, sx;
                s16 npcId;
                u8* slot;
                s32 state;

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + byte_off;
                menuItemBiosSetXY(*(s16*)(slot + 2), *(s16*)(slot + 4), *(s16*)(slot + 6));
                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30 + byte_off;
                menuDataBiosSetXY(*(s16*)(slot + 0), *(s16*)(slot + 4), *(s16*)(slot + 6));

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                menuOpenCustom((s32)*(s16*)(slot + byte_off), 0x63, 0, 0, 0, 2, (void*)iter, i);

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                npcId = *(s16*)(slot + byte_off);
                menuDataBiosGetXY(npcId, (u16*)&sx, (u16*)&sy);
                if (sx > 0xfa) state = 0x116;
                else state = 0x11e;
                winSeqSetMenu((void*)(s32)npcId, state);

                byte_off += 8;
                iter += 0x30;
            }
            winSeqSetMenu(*(void**)((u8*)ctx + 4), 1);
            *((s8*)ctx + 2) = 1;
        }
    } else if ((s8)*((u8*)ctx + 1) == 3) {
        if ((s8)*((u8*)ctx + 2) == 0) {
            byte_off = 0;
            for (i = byte_off; byte_off < 6; byte_off++) {
                s16 sy, sx;
                s16 npcId;
                u8* slot;
                s32 state;

                slot = lbl_802E4E58 + (s32)(s8)lbl_803A1D40[4] * 0x30;
                npcId = *(s16*)(slot + i);
                menuDataBiosGetXY(npcId, (u16*)&sx, (u16*)&sy);
                if (sx > 0xfa) state = 0x11a;
                else state = 0x122;
                winSeqSetMenu((void*)(s32)npcId, state);

                i += 8;
            }
            winSeqSetMenu(*(void**)((u8*)ctx + 4), 7);
            *((s8*)ctx + 2) = 1;
        }
    }
    *(s16*)&lbl_8047A308 = (s16)(((s32)*(s16*)&lbl_8047A308 + 1) % 1000);
    return 0;
}
#pragma pop
#endif

/* menuPokemonClose - 0x8001B184 | size: 0x68 */
#if 0
asm void menuPokemonClose(void) {
#include "src/game/gs_pcbox_fn_8001B184.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling off
void menuPokemonClose(void) {
    extern u8 lbl_803A1D40[];
    extern void menuCloseCustom();
    extern void fn_800FF660();
    extern void floorSetFadeScript();
    extern void _threadSwitch();
    menuCloseCustom(0x63, 0x0, 0x1);
    if (*(u8*)(lbl_803A1D40 + 0x2) != 0) {
        fn_800FF660();
        if (*(u8*)(lbl_803A1D40 + 0x3) != 0x1) floorSetFadeScript(0x0, 0x0);
        _threadSwitch();
    }
}
#pragma pop
#endif

#endif /* !MENU_POKEMON_CARVE_ONLY */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_SUB_ONLY)
/* menuPokemonSub - 0x8001B1EC | size: 0x8d8 */
#if defined(MENU_POKEMON_SUB_ONLY)
extern u8 lbl_803A1D40[];
extern u8 lbl_803A1C20[];
extern s32 menuOpen();
extern void menuClose();
extern void menuCloseCustom();
extern void fadeSet();
extern void fadeCheck();
extern u8* windowSearchID();
extern s32 menuGetCursorItemID();
extern u32 _threadSwitch(void);
extern u32 pokemonIsDarkPokemon();
extern void winMsgOpen();
extern void winMsgClose(s32);
extern s32 fn_8001C7B8(s8);
extern s32 fn_8001C064(s8, u8, u16, u16*);
#endif
extern void fn_8010B01C();
extern u32 menuPokemonCheckPokemonChange();
extern void fn_80097E58();
extern f32 pokemonGetDp();
extern u32 fn_8010B560();
extern void* fn_8001BEBC(u16*);
extern u16 lbl_8047A30A;
extern f32 lbl_8047B7C0;
extern u32 lbl_8047B7C4;
#if 0
asm void menuPokemonSub(void) {
#include "src/game/gs_pcbox_fn_8001B1EC.inc"
}
#else
/* RULE-EXCEPTION(user-approved): summary-path inline copy and scalar carriers
 * preserve retail register allocation; see docs/RULE_EXCEPTIONS.md. */
static inline void* menuPokemonGetSummaryPokemon(s32 mode, u16 index, void* trainer) {
    extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
    extern void* fightFloorGetGcHeroFightTrainerPtr(s32);
    extern void* fightTrainerGetValidFightPokemonPtr(void* trainer, u16 index);
    extern void* pokemonGetStatus(void*, s32, s32, s32);
    extern u8 pokemonCheckValid(void* pokemon);
    struct { void* value; } pokemon;
    void* hero;
    void* fightPokemon;

    pokemon.value = 0;
    switch (mode) {
    case 0:
        if (index >= 6) {
            return 0;
        }
        hero = menuPokemonGetHero(mode, trainer);
        if (hero == 0) {
            return 0;
        }
        pokemon.value = heroBiosGetPokemonPtr(hero, index);
        break;
    case 1:
        if (index >= 6) {
            return 0;
        }
        if (trainer == 0) {
            trainer = fightFloorGetGcHeroFightTrainerPtr(0);
        }
        if (trainer == 0) {
            return 0;
        }
        fightPokemon = fightTrainerGetValidFightPokemonPtr(trainer, index);
        if (fightPokemon == 0) {
            return 0;
        }
        pokemon.value = pokemonGetStatus(fightPokemon, 0, 0xCC, 0);
        break;
    case 2:
        if (index >= 30) {
            return 0;
        }
        break;
    }
    if (pokemonCheckValid(pokemon.value) == 0) {
        pokemon.value = 0;
    }
    return pokemon.value;
}

s32 menuPokemonSub() {
    extern void* memset(void* dst, int val, u32 n);
    extern s32 menuOpenCustom(s32 menuId, s32 parent, void* args, s32, s32, s32, ...);
    extern void pokemonToMenuPokemonStatus(void* pokemon, MenuPokemonStatus* status);
    extern u8 pokemonGetStatus(void* pokemon, u32 a, u32 id, u32 b);
    extern s32 fn_80019D5C();
    s32 cursorArg;
    s32 summaryArg;
    u16 newItem;
    s8* work;
    s8* cursor;
    MenuPokemonStatus* status;
    void* pokemon;
    struct { u16 value; } slot;
    s8 selection;
    s32 result;

    work = (s8*)lbl_803A1D40;
    /* RULE-EXCEPTION(user-approved): cursor reached by pre-increment so it stays a register pointer as in retail — see docs/RULE_EXCEPTIONS.md */
    cursor = work + 5;
    *++cursor = 0;
    work[5] = 0;
    work[7] = -1;
    if (((u8*)work)[1] == 0) {
        *cursor = -1;
    }

    memset(lbl_803A1C20, 0, 0x120);
    for (slot.value = 0; slot.value < 6; slot.value++) {
        status = &((MenuPokemonStatus*)lbl_803A1C20)[slot.value];
        pokemon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), slot.value,
                                        *(void**)(lbl_803A1D40 + 0xC));
        if (pokemon == 0) {
            status->species = 0;
        } else {
            pokemonToMenuPokemonStatus(pokemon, status);
            if (pokemonGetStatus(pokemon, 0, 0x7B, 0) == 1) {
                status->unk1A = 0;
            }
            status->iconId = menuPokemonGetStatusIcon(pokemon);
        }
    }

    lbl_8047A30A = 0;
    fn_8010B01C(0, fn_8001BEBC, &lbl_8047A30A);

    while (1) {
        lbl_803A1D40[0x14] = 0;
        cursorArg = *cursor;
        selection = menuOpenCustom(0x63, 0, &cursorArg, 0, 1, 1, lbl_803A1C20);
        if (((u8*)work)[1] == 0) {
            selection = -2;
        }
        *cursor = selection;
        if (selection == -2) {
            break;
        }
        if (menuGetCursorItemID(0x63) == 0x3B6 || selection == -1) {
            *cursor = -1;
            break;
        }

        switch (lbl_803A1D40[0]) {
        case 1:
            result = fn_8001C7B8(selection);
            break;
        case 2: {
            struct { s32 value; } action;
            lbl_803A1D40[0x14] = 1;
            while (1) {
                if (lbl_803A1D40[0x15] != 0) {
                    action.value = menuOpen(0x6D, 1);
                    menuClose(0x6D);
                } else {
                    action.value = menuOpen(0x108, 1);
                    menuClose(0x108);
                }
                switch (action.value) {
                case 0:
                    if ((u8)menuPokemonCheckPokemonChange(*(void**)(lbl_803A1D40 + 0x18),
                                                          *(void**)(lbl_803A1D40 + 0xC),
                                                          selection) != 0) {
                        action.value = -1;
                    } else {
                        action.value = 1;
                    }
                    break;
                case 1:
                    menuCloseCustom(0x63, 0, 1);
                    pokemon = menuPokemonGetSummaryPokemon(*(s32*)(lbl_803A1D40 + 0x8), selection,
                                                           *(void**)(lbl_803A1D40 + 0xC));
                    fadeSet(3, lbl_8047B7C0);
                    fadeCheck(1);
                    fn_80097E58(*(void**)(lbl_803A1D40 + 0xC), pokemon, fn_80019D5C, 0);
                    summaryArg = *cursor;
                    menuOpenCustom(0x63, 0, &summaryArg, 0, 0, 1, lbl_803A1C20);
                    pokemon = windowSearchID(0x63);
                    if (pokemon != 0) {
                        ((u8*)pokemon)[0x98] = 1;
                    }
                    fadeSet(2, lbl_8047B7C0);
                    fadeCheck(1);
                    selection = *cursor;
                    continue;
                case -1:
                case 2:
                    action.value = 1;
                    break;
                }
                break;
            }
            result = action.value;
            break;
        }
        case 3:
        case 4:
            result = -1;
            break;
        case 5:
            result = ((s32 (*)(s8, u8, u16, u16*))fn_8001C064)(selection, lbl_803A1D40[0x11],
                                                              *(u16*)(lbl_803A1D40 + 0x12), &newItem);
            break;
        case 6:
            result = -1;
            break;
        case 7: {
            pokemon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), selection,
                                            *(void**)(lbl_803A1D40 + 0xC));
            if (pokemon == 0) {
                continue;
            }
            if ((u8)pokemonIsDarkPokemon(pokemon) == 0) {
                winMsgOpen(2, 0x44DD, 1, 0);
                winMsgClose(1);
                continue;
            }
            if (*(f32*)&lbl_8047B7C4 != pokemonGetDp(pokemon)) {
                winMsgOpen(2, 0x44DE, 1, 0);
                winMsgClose(1);
                continue;
            }
            result = -1;
            break;
        }
        }

        if (result < 0) {
            break;
        }
    }

    while ((u8)fn_8010B560() != 0) {
        _threadSwitch();
    }
    *(s32*)(lbl_803A1D40 + 0x1C) = *cursor;
    return result;
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_ISLAND_8001BAC4_ONLY)
/* menuPokemonOpenSub - 0x8001BAC4 | size: 0x228 */
extern void menuCreateOffScreen();
extern void menuReleaseOffScreen();
extern u32 lbl_8047B7B8;
extern u32 lbl_8047B7C4;
#if 0
asm void menuPokemonOpenSub(void) {
#include "src/game/gs_pcbox_fn_8001BAC4.inc"
}
#else
u32 menuPokemonOpenSub(u8 a0, u8 a1, u8 a2, u16 a3, u32 a4, u8 a5) {
    extern u8 lbl_803A1D40[];
    extern s32 fn_800D37CC(void);
    extern void _flagSet();
    extern void std098_mapWarpToMenuRoom();
    extern void floorSetFadeScript();
    extern void _threadSwitch();
    extern s32 menuPokemonSub(u8, u16, u32);
    s32 v;

    *(u8*)(lbl_803A1D40 + 0x0) = a0;
    *(u8*)(lbl_803A1D40 + 0x10) = a1;
    *(u8*)(lbl_803A1D40 + 0x11) = a2;
    *(u16*)(lbl_803A1D40 + 0x12) = a3;
    *(u32*)(lbl_803A1D40 + 0xC) = a4;
    *(u8*)(lbl_803A1D40 + 0x1) = a5;
    *(u32*)(lbl_803A1D40 + 0x1C) = 0;
    *(u8*)(lbl_803A1D40 + 0x15) = 1;

    switch ((s32)(u8)a0) {
    case 2:
    case 4:
        *(u32*)(lbl_803A1D40 + 0x8) = 1;
        break;
    case 3:
    default:
        *(u32*)(lbl_803A1D40 + 0x8) = 0;
        break;
    }

    v = (s32)*(u8*)(lbl_803A1D40 + 0x0);
    switch (v) {
    case 3:
    case 4:
    case 5:
        *(u8*)(lbl_803A1D40 + 0x2) = 0;
        *(u8*)(lbl_803A1D40 + 0x3) = 0;
        break;
    case 1:
        *(u8*)(lbl_803A1D40 + 0x2) = 1;
        *(u8*)(lbl_803A1D40 + 0x3) = 0;
        break;
    case 2:
        *(u8*)(lbl_803A1D40 + 0x2) = 1;
        *(u8*)(lbl_803A1D40 + 0x3) = 1;
        if ((u16)a3 == 0) {
            *(u8*)(lbl_803A1D40 + 0x15) = 0;
        }
    case 6:
    case 7:
    default:
        menuCreateOffScreen(*(f32*)&lbl_8047B7C4 / (f32)fn_800D37CC());
        *(u8*)(lbl_803A1D40 + 0x2) = 1;
        *(u8*)(lbl_803A1D40 + 0x3) = 2;
        break;
    }
    if (*(u8*)(lbl_803A1D40 + 0x2) == 1) {
        _flagSet(1, 1);
        std098_mapWarpToMenuRoom(0x38f);
        if (*(u8*)(lbl_803A1D40 + 0x3) != 1) {
            floorSetFadeScript(0, 0);
        }
        _threadSwitch();
        if (*(u8*)(lbl_803A1D40 + 0x3) == 2) {
            menuReleaseOffScreen(*(f32*)&lbl_8047B7C4 / (f32)fn_800D37CC());
        }
    } else {
        menuPokemonSub(a0, (u16)a3, a4);
    }
    *(u8*)(lbl_803A1D40 + 0x4) = 1;
    return *(u32*)(lbl_803A1D40 + 0x1C);
}
#endif

#endif /* guard split */
#if !defined(MENU_POKEMON_CARVE_ONLY)
/* menuPokemonOpenItemGive - 0x8001BCEC | size: 0x50 */
#if 0
asm void menuPokemonOpenItemGive(void) {
#include "src/game/gs_pcbox_fn_8001BCEC.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpenItemGive(u32 a, u32 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub(u32, u32, u32, u32, u32, u32);
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    menuPokemonOpenSub(0x5, a, b, c, d, 0x1);
}
#pragma pop
#endif

/* menuPokemonOpenItemUse - 0x8001BD3C | size: 0x44 */
#if 0
asm void menuPokemonOpenItemUse(void) {
#include "src/game/gs_pcbox_fn_8001BD3C.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpenItemUse(u32 a, u32 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub();
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    menuPokemonOpenSub(a, 0x0, 0x0, c, d, b);
}
#pragma pop
#endif

/* menuPokemonOpenFight - 0x8001BD80 | size: 0x74 */
#if 0
asm void menuPokemonOpenFight(void) {
#include "src/game/gs_pcbox_fn_8001BD80.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpenFight(u8 a, u8 b, u32 c, u32 d) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub(u32, u32, u32, u32, u32, u32);
    if (a == 0x1) {
        *(u8*)(lbl_803A1D40 + 0x4) = 0x0;
    } else {
        *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    }
    *(u32*)(lbl_803A1D40 + 0x18) = d;
    menuPokemonOpenSub(0x2, 0x0, 0x0, b, c, 0x1);
}
#pragma pop
#endif

/* menuPokemonOpen - 0x8001BDF4 | size: 0x44 */
#if 0
asm void menuPokemonOpen(void) {
#include "src/game/gs_pcbox_fn_8001BDF4.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
void menuPokemonOpen(u32 a, u32 b, u32 c) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonOpenSub(u32, u32, u32, u32, u32, u32);
    *(u8*)(lbl_803A1D40 + 0x4) = 0x1;
    menuPokemonOpenSub(a, 0x0, 0x0, b, c, 0x1);
}
#pragma pop
#endif

/* menuPokemonMain - 0x8001BE38 | size: 0x84 */
#if 0
asm void menuPokemonMain(void) {
#include "src/game/gs_pcbox_fn_8001BE38.inc"
}
#else
#pragma push
#pragma scheduling off
#pragma optimization_level 4
s32 menuPokemonMain(void) {
    extern u8 lbl_803A1D40[];
    extern void menuPokemonSub();
    extern void menuCloseCustom();
    extern void fn_800FF660();
    extern void floorSetFadeScript();
    extern void _threadSwitch();
    u8 r3;
    u16 r4;
    u32 r5;
    r3 = *(u8*)(lbl_803A1D40 + 0x0);
    r4 = *(u16*)(lbl_803A1D40 + 0x12);
    r5 = *(u32*)(lbl_803A1D40 + 0xC);
    menuPokemonSub((u32)r3, (u32)r4, r5);
    menuCloseCustom(0x63, 0x0, 0x1);
    if (*(u8*)(lbl_803A1D40 + 0x2) != 0) {
        fn_800FF660();
        if (*(u8*)(lbl_803A1D40 + 0x3) != 0x1) floorSetFadeScript(0x0, 0x0);
        _threadSwitch();
    }
    return 0;
}
#pragma pop
#endif

#endif /* !MENU_POKEMON_CARVE_ONLY */

#if !defined(MENU_POKEMON_CARVE_ONLY) || defined(MENU_POKEMON_8001BEBC_ONLY)
/* fn_8001BEBC - 0x8001BEBC | size: 0x1a8 */
#if 0
asm void fn_8001BEBC(void) {
#include "src/game/gs_pcbox_fn_8001BEBC.inc"
}
#else
void* fn_8001BEBC(u16* index) {
    extern u8 lbl_803A1D40[];
    void* pokemon;

    pokemon = menuPokemonGetPokemon(*(s32*)(lbl_803A1D40 + 0x8), *index,
                                    *(void**)(lbl_803A1D40 + 0xC));
    (*index)++;
    return pokemon;
}
#endif
#endif

#if !defined(MENU_POKEMON_CARVE_ONLY)

/* fn_8001D624 - 0x8001D624 | size: 0xf4 */
extern u8 lbl_802E4EC8[];
#if 0
asm void fn_8001D624(void) {
#include "src/game/gs_pcbox_fn_8001D624.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
u16 fn_8001D624(void* a, u8 b) {
    extern u32 pokemonGetStatus();
    extern u32 pokemonGetJoutaiMenuSpriteId();
    u32 r3 = pokemonGetStatus(a, 0x0, 0x7b, 0x0);
    if ((u8)r3 == 0x1) {
        r3 = 0x1;
    } else {
        r3 = pokemonGetJoutaiMenuSpriteId(a);
        switch ((u16)r3) {
        case 0x3a: r3 = 0x2; break;
        case 0x3b: r3 = 0x3; break;
        case 0x3c: r3 = 0x4; break;
        case 0x3d: r3 = 0x5; break;
        case 0x3e: r3 = 0x6; break;
        default: r3 = 0x0; break;
        }
    }
    if ((u8)b == 0) {
        return *(u16*)(lbl_802E4EB8 + (u16)r3 * 2);
    } else {
        return *(u16*)(lbl_802E4EC8 + (u16)r3 * 2);
    }
}
#pragma pop
#endif
#endif /* !MENU_POKEMON_CARVE_ONLY */
