/**
 * @file fight_out_pokemon_candidate_80202810.c
 * @brief fightOutPokemon + fightPokemon candidate prefix, address range
 *        0x80202810-0x8020355C, 9 functions.
 *
 * OutPokemon/Pokemon field accessors, sequence/status writers, and
 * damage-calc support the seq/waza layers call into (statusGetStatus,
 * fadeEffectGetRandom callers, etc). Corresponds to XD's
 * fight.cpp fightOutPokemon+fightPokemon cluster (0x80200644-0x80208288).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

typedef struct StatusIdTable7 {
    u16 id[7];
} StatusIdTable7;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  joutaiDataBiosGetStatusKind(void);
extern void  pokemonIsJoutaiDataId(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

/* 0x80202810 | size: 0x188 | medium */
/* 0x80202810 | size: 0x188 */
void fightOutPokemonWriteJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern void fn_8011B788();
    extern void pokemonInitPersistentJoutai();
    extern void fn_801DA36C();
    void* eeData;

    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    if ((u16)(u32)typeObj == 0) {
        if (eeData != NULL) {
            fn_801DA36C(eeData, 1);
            fn_801DA36C(eeData, 2);
        }
    } else {
        if (eeData != NULL) {
            if ((u16)(u32)typeObj == 8) {
                fn_801DA36C(eeData, 1);
            }
            if ((u16)(u32)typeObj == 7) {
                fn_801DA36C(eeData, 2);
            }
        }
    }
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8 || joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        eeData = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
            if (eeData == NULL) {
                eeData = NULL;
            } else {
                eeData = pokemonGetStatus(eeData, 0, 0xCC, 0);
            }
            pokemonInitPersistentJoutai(eeData, typeObj);
        } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
            fn_8011B788(eeData, typeObj);
        }
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xD8) {
        fn_8011B788(ctx, typeObj);
    }
}

/* 0x80202998 | size: 0x94 */
void fightOutPokemonResetSeqStatus(void* ctx, u16 mode) {
    extern void fn_801DA36C();
    void* obj;
    u16 modeVal;
    obj = pokemonGetStatus(ctx, 0, 0xEE, 0);
    modeVal = mode;
    if (modeVal == 0) {
        if (obj != NULL) {
            fn_801DA36C(obj, 1);
            fn_801DA36C(obj, 2);
        }
    } else {
        if (obj != NULL) {
            if (modeVal == 8) {
                fn_801DA36C(obj, 1);
            }
            modeVal = mode;
            if (modeVal == 7) {
                fn_801DA36C(obj, 2);
            }
        }
    }
}

/* 0x80202A2C | size: 0xB0 */
void fightPokemonWriteJoutaiDataId(void* ctx, void* typeObj, u32 param) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern void statusSetJoutaiStack();
    extern void fn_8012190C();
    void* resolved;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
        }
        fn_8012190C(resolved, typeObj, param);
    } else if (joutaiDataBiosGetStatusKind(typeObj) == 0xCD) {
        statusSetJoutaiStack(ctx, typeObj, param);
    }
}

/* 0x80202ADC | size: 0xAC */
u32 fightPokemonCheckWriteJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u32 fn_8011B67C();
    extern u32 pokemonIsJoutaiDataId();
    void* resolved;
    u32 result;
    if (joutaiDataBiosGetStatusKind(typeObj) == 0x7C || joutaiDataBiosGetStatusKind(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
        }
        result = pokemonIsJoutaiDataId(resolved, typeObj);
    } else if (joutaiDataBiosGetStatusKind(typeObj) != 0xCD) {
        result = 0;
    } else {
        result = fn_8011B67C(ctx, typeObj);
    }
    return result;
}

/* 0x80202B88 | size: 0x94 */
u32 fightOutPokemonIsAlly(void* obj1, void* obj2) {
    extern u32 fightTargetGetPtr();
    extern u16 fightFloorGetStatus();
    u16 tableId;
    u32 val1;
    u32 val2;
    tableId = 0xFFFF & fightFloorGetStatus(NULL, 0, 0x14, 0);
    if (obj1 == NULL) {
        return 0;
    }
    if (obj2 == NULL) {
        return 0;
    }
    val1 = fightTargetGetPtr(2, obj1, tableId);
    val2 = fightTargetGetPtr(2, obj2, tableId);
    return (u8)(val1 == val2);
}

/* Address: 0x80202C1C | Size: 0x57c | Ghidra import */

/* fightPokemonGetPokemonPtr (0x80205BE8), fightPokemonCheckValid (0x80206A04),
 * fightPokemonCheckFightOut (0x80206608), fightOutPokemonCheckValid
 * (0x80206780) and fightOutPokemonCheckFightOut (0x802062FC), expanded here
 * as in fightOutPokemonSetMeetEnemyFightPokemonEnemySideAll
 * (fight_out_pokemon.c), the sibling this function mirrors. */
static inline void* fightPokemonGetPokemonPtrInline(void* fp)
{
    void* p;

    if (fp == NULL) {
        p = NULL;
    } else {
        /* RULE-EXCEPTION(user-approved): result carrier, as in fight_out_pokemon.c's copy of this inline (99.62% without) — see docs/RULE_EXCEPTIONS.md */
        void* tmp = pokemonGetStatus(fp, 0, 0xCC, 0);
        p = tmp;
    }
    return p;
}

static inline u8 fightPokemonCheckValidInline(void* fp)
{
    extern u8 pokemonCheckValid(void*);
    extern u16 std103_battleGetResult();
    void* p;

    if (fp == NULL) {
        return 0;
    }
    if (std103_battleGetResult() == 1) {
        return 0;
    }
    p = pokemonGetStatus(fp, 0, 0xcb, 0);
    if (p == NULL) {
        return 0;
    }
    if (pokemonCheckValid(p) == 0) {
        return 0;
    }
    p = fightPokemonGetPokemonPtrInline(fp);
    if (p == NULL) {
        return 0;
    }
    if (pokemonCheckValid(p) == 0) {
        return 0;
    }
    if ((s32)pokemonGetStatus(fp, 0, 0xce, 0) < 0) {
        return 0;
    }
    return 1;
}

static inline u8 fightPokemonCheckFightOutInline(void* fp)
{
    extern u8 pokemonCheckFightOut();
    void* p;

    if (fp == NULL) {
        return 0;
    }
    if (fightPokemonCheckValidInline(fp) == 0) {
        return 0;
    }
    if ((s32)pokemonGetStatus(fp, 0, 0xd2, 0) == 1) {
        return 0;
    }
    p = fightPokemonGetPokemonPtrInline(fp);
    if (p == NULL) {
        return 0;
    }
    if (pokemonCheckFightOut(p) == 0) {
        return 0;
    }
    return 1;
}

static inline u8 fightOutPokemonCheckValidInline(void* fo)
{
    extern u16 std103_battleGetResult();
    void* fp;

    if (fo == NULL) {
        return 0;
    }
    if (std103_battleGetResult() == 1) {
        return 0;
    }
    fp = pokemonGetStatus(fo, 0, 0xd6, 0);
    if (fp == NULL) {
        return 0;
    }
    if (fightPokemonCheckValidInline(fp) == 0) {
        return 0;
    }
    return 1;
}

static inline u8 fightOutPokemonCheckFightOutInline(void* fo)
{
    void* fp;

    if (fo == NULL) {
        return 0;
    }
    if (fightOutPokemonCheckValidInline(fo) == 0) {
        return 0;
    }
    if ((s32)pokemonGetStatus(fo, 0, 0x120, 0) == 1) {
        return 0;
    }
    fp = pokemonGetStatus(fo, 0, 0xd6, 0);
    if (fightPokemonCheckFightOutInline(fp) == 0) {
        return 0;
    }
    return 1;
}

/* Adds target to fo's 4-entry enemy array (status 0x122) unless it is
 * already there: the per-pokemon step of this SideAll walk, kept as an
 * inline like XD's per-pokemon fightOutPokemonSetMeetEnemyFightPokemon in
 * the sibling. */
/* RULE-EXCEPTION(user-approved): single-use inline helper without a named counterpart (open-coded 99.74%) — see docs/RULE_EXCEPTIONS.md */
static inline void fightOutPokemonCreateEnemyInline(void* fo, void* target)
{
    extern void* fightOutPokemonEnemySearchAry();
    extern u8 fightOutPokemonEnemyCheckValid();
    extern void fightOutPokemonEnemyCreate();
    void* ary;
    void* enemy;
    u16 k;

    if (fo == NULL || fightOutPokemonCheckValidInline(target) == 0) {
        return;
    }
    ary = pokemonGetStatus(fo, 0, 0x122, 0);
    if (fightOutPokemonEnemySearchAry(ary, 4, target) == NULL) {
        for (k = 0; k < 4; k++) {
            enemy = (void*)((u32)ary + k * 0xC);
            if (fightOutPokemonEnemyCheckValid(enemy) == 0) {
                fightOutPokemonEnemyCreate(enemy, target);
                break;
            }
        }
    }
}

void _fightFloorCreateFightPokemonEnemyArySub(void* fo, void* side)
{
    extern u32 fightFloorGetStatus();
    extern void* fightSideGetStatus(void*, u32, u32, u16);
    extern u8 fightTrainerCheckValid();
    extern void* fightTrainerGetStatus(void*, u32, u32, u16);
    u16 trainers;
    u16 pokemons;
    u16 p;
    u16 t;
    void* trainer;
    void* target;

    fightFloorGetStatus(0, 0, 0x14, 0);
    trainers = fightFloorGetStatus(0, 0, 0x16, 0);
    pokemons = fightFloorGetStatus(0, 0, 0x18, 0);
    for (t = 0; t < trainers; t++) {
        trainer = fightSideGetStatus(side, 0, 7, t);
        if (fightTrainerCheckValid(trainer) == 0) {
            continue;
        }
        for (p = 0; p < pokemons; p++) {
            target = fightTrainerGetStatus(trainer, 0, 0x46, p);
            if (fightOutPokemonCheckFightOutInline(target) != 0) {
                fightOutPokemonCreateEnemyInline(fo, target);
            }
        }
    }
}

/* 0x80203198 | size: 0x14C | medium */
/* 0x80203198 | size: 0x14C */
extern u16 fightOutPokemonEnemyBiosGetOumuWazaDataId();
extern void* fightOutPokemonEnemySearchAry();
extern u8 fightOutPokemonEnemyCheckValid();
extern void fightOutPokemonEnemyInit();

static inline u8 fn_80203198_countOumu(void* ctx) {
    void* entryPtr;
    void* entry;
    u8 count;
    u8 i;

    if (ctx == NULL) {
        return 0;
    }
    entryPtr = pokemonGetStatus(ctx, 0, 0x122, 0);
    for (i = 0; i < 4; i++) {}
    count = 0;
    for (i = 0; i < 4; i++) {
        entry = (void*)((u32)entryPtr + i * 0xC);
        if (fightOutPokemonEnemyCheckValid(entry) != 0) {
            u16 oumu = fightOutPokemonEnemyBiosGetOumuWazaDataId(entry);
            if (oumu != 0 && oumu != 0x165) {
                count++;
            }
        }
    }
    return count;
}

void fightOutPokemonEnemyClearOumu(void* ctx, u32 param) {
    void* entry;
    u16 species;

    if (ctx == NULL) {
        return;
    }
    entry = fightOutPokemonEnemySearchAry(pokemonGetStatus(ctx, 0, 0x122, 0), 4, param);
    if (entry == NULL) {
        return;
    }
    species = fightOutPokemonEnemyBiosGetOumuWazaDataId(entry);
    fightOutPokemonEnemyInit(entry);
    if (species == 0) {
        return;
    }
    if (species == 0x165) {
        return;
    }
    if (species == 0xFFFF) {
        return;
    }
    if ((s32)(u32)pokemonGetStatus(ctx, 0, 0xF7, 0) != 0) {
        return;
    }
    if (fn_80203198_countOumu(ctx) == 0) {
        pokemonSetStatus(ctx, 0, 0xF7, 0, (u32)species);
    }
}

/* 0x802032E4 | size: 0x138 */
static inline void* fn_802032E4_getCC(void* ctx) {
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

void fightPokemonGetFriendFormPokemonFriendFilterId(void* ctx, u32 param) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u8 fn_8011B67C();
    extern u8 pokemonIsJoutaiDataId();
    extern void pokemonGetFriendFormPokemonFriendFilterId();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* resolvedPtr;
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_802032E4_getCC(ctx); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_802032E4_getCC(ctx); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (joutaiDataBiosGetStatusKind(0x3D) == 0x7C || joutaiDataBiosGetStatusKind(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { resolvedPtr = pokemonGetStatus(ctx, 0, 0xCC, 0); tmp = resolvedPtr; }
            result = pokemonIsJoutaiDataId(tmp, 0x3D);
        } else {
            if (joutaiDataBiosGetStatusKind(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(ccCtx);
        }
    }
    pokemonGetFriendFormPokemonFriendFilterId(ccData, value, param);
}

/* 0x8020341C | size: 0x140 */
static inline void* fn_8020341C_resolveCcData(void* ctx)
{
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

static inline void* fn_8020341C_resolveCcCtx(void* ctx)
{
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

void fightPokemonGetEffortFromPokemon(void* ctx, u32 param1, u32 param2) {
    extern u16 joutaiDataBiosGetStatusKind();
    extern u8 fn_8011B67C();
    extern u8 pokemonIsJoutaiDataId();
    extern void pokemonGetEffortFromPokemon();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_8020341C_resolveCcData(ctx); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_8020341C_resolveCcCtx(ctx); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (joutaiDataBiosGetStatusKind(0x3D) == 0x7C || joutaiDataBiosGetStatusKind(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { tmp = pokemonGetStatus(ctx, 0, 0xCC, 0); }
            result = pokemonIsJoutaiDataId(tmp, 0x3D);
        } else {
            if (joutaiDataBiosGetStatusKind(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(ccCtx);
        }
    }
    pokemonGetEffortFromPokemon(ccData, value, param1, param2);
}
