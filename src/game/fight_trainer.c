/**
 * @file fight_trainer.c
 * @brief fightTrainer section (fightTrainerEnemyPokemon cluster tail) -- split from colosseum_event.c (the fight
 *        engine bucket, 0x80202810-0x80211A00), address range
 *        0x8020E4E8-0x8020EE1C, 12 fns.
 *
 * fightTrainerEnemyPokemon Create/InitAry/Init/Erase/Regist/SearchAry/
 * CheckValid family plus the trainer action-buff/DB accessor head.
 * Corresponds to XD's fightTrainer section tail
 * (0x801FC350-0x802004EC, anchors at 0x80200288-0x80200424).
 *
 * fightTrainerEnemyPokemonEraseAry/RegistAry expand the same unit's
 * SearchAry, CheckValid and entry-initialisation bodies (the static inline
 * copies below), which they precede in the binary.
 *
 * The per-function `#pragma peephole on` blocks this file used to carry were
 * no-ops: -O4,s already runs the peephole pass.
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

typedef struct FightAbicntRatio {
    u8 numerator;
    u8 denominator;
} FightAbicntRatio;

/* fightOutPokemonEnemy DB row (0xC bytes); fields accessed via the
 * fightOutPokemonEnemyBios* getter/setter family in fight_trainer_db_range_801FBD10.c */
typedef struct FightOutPokemonEnemyEntry {
    u32 targetFightOutPokemonPtr; /* 0x0 */
    u16 oumuWazaDataId;           /* 0x4 */
    u16 initHp;                   /* 0x6 */
    u16 damage;                   /* 0x8 */
} FightOutPokemonEnemyEntry;

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

/* Ability-count numerator/denominator table (16 entries) */
extern FightAbicntRatio lbl_80375D10[];
extern u32 lbl_80478D68; /* table entry count */

/* Address: 0x8020E4E8 | Size: 0x94 | Ghidra import */
/* val * kake / waru of ability-count entry id. The repeated NULL tests are
 * the expansions of the table lookup and its field getters (0 kake, 1 waru
 * for an id past the table); the entry address is formed twice, once per
 * lookup, as in retail. */
u32 fightAbicntDoKakeWaru(u16 id, u32 val)
{
    u8 den;
    u8* numRatio;
    u32 denAddress;
    u8 num;
    u32 cnt = lbl_80478D68;

    numRatio = (u8*)&lbl_80375D10[(u16)id];
    if ((u16)id >= cnt) {
        numRatio = NULL;
    }
    if (numRatio == NULL) {
        num = 0;
    } else if (numRatio == NULL) {
        num = 0;
    } else {
        num = numRatio[0];
    }

    denAddress = (u32)lbl_80375D10 + (id & 0xffffU) * sizeof(FightAbicntRatio);
    if ((u16)id >= cnt) {
        denAddress = 0;
    }
    den = (denAddress == 0) ? 1 : ((denAddress == 0) ? 1 : ((u8*)denAddress)[1]);

    return (val * num) / den;
}

/* Address: 0x8020E57C | Size: 0x98 | Ghidra import */
FightOutPokemonEnemyEntry* fightOutPokemonEnemySearchAry(FightOutPokemonEnemyEntry* ctx, u16 count, u32 matchVal)
{
    extern u32 fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(FightOutPokemonEnemyEntry* ptr);
    FightOutPokemonEnemyEntry* entry;
    u32 target;
    u16 i;

    if (ctx == NULL) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        entry = &ctx[i];
        {
            u8 valid = -fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(entry) != 0;
            if (valid) {
                target = fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(entry);
                if (target != matchVal) {
                    continue;
                }
                return entry;
            }
        }
    }
    return NULL;
}

/* fightOutPokemonEnemyCheckValid | Size: 0x2C | Check if fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr returns non-zero */
BOOL fightOutPokemonEnemyCheckValid(void) {
    extern s32 fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(void);
    return -fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr() != 0;
}

/* Address: 0x8020E640 | Size: 0x94 | Ghidra import */
void fightOutPokemonEnemyCreate(FightOutPokemonEnemyEntry* ctx, u32 pokemonSlot)
{
    extern void fightOutPokemonEnemyBiosSetDamage(FightOutPokemonEnemyEntry* ptr, u16 val);
    extern void fightOutPokemonEnemyBiosSetInitHp(FightOutPokemonEnemyEntry* ptr, u16 val);
    extern void fightOutPokemonEnemyBiosSetOumuWazaDataId(FightOutPokemonEnemyEntry* ptr, u16 val);
    extern void fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(FightOutPokemonEnemyEntry* ptr, u32 val);
    extern u32 fightOutPokemonGetPokemonPtr(u32 slot);
    u32 pokePtr;
    u16 hp;

    if ((ctx != NULL) && (pokemonSlot != 0)) {
        fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(ctx, 0);
        fightOutPokemonEnemyBiosSetOumuWazaDataId(ctx, 0);
        fightOutPokemonEnemyBiosSetInitHp(ctx, 0);
        fightOutPokemonEnemyBiosSetDamage(ctx, 0);
        fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(ctx, pokemonSlot);
        pokePtr = fightOutPokemonGetPokemonPtr(pokemonSlot);
        hp = (u16)pokemonGetStatus(pokePtr, 0, 0x83, 0);
        fightOutPokemonEnemyBiosSetInitHp(ctx, hp);
    }
    return;
}

/* Address: 0x8020E6D4 | Size: 0x84 | Ghidra import */
void fightOutPokemonEnemyInitAry(FightOutPokemonEnemyEntry* ctx, u16 count)
{
    extern void fightOutPokemonEnemyBiosSetDamage(FightOutPokemonEnemyEntry* ptr, u16 val);
    extern void fightOutPokemonEnemyBiosSetInitHp(FightOutPokemonEnemyEntry* ptr, u16 val);
    extern void fightOutPokemonEnemyBiosSetOumuWazaDataId(FightOutPokemonEnemyEntry* ptr, u16 val);
    extern void fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(FightOutPokemonEnemyEntry* ptr, u32 val);
    FightOutPokemonEnemyEntry* entry;
    u16 i;

    if (ctx != NULL) {
        for (i = 0; i < count; i++) {
            entry = &ctx[i];
            fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(entry, 0);
            fightOutPokemonEnemyBiosSetOumuWazaDataId(entry, 0);
            fightOutPokemonEnemyBiosSetInitHp(entry, 0);
            fightOutPokemonEnemyBiosSetDamage(entry, 0);
        }
    }
    return;
}

/* Address: 0x8020E758 | Size: 0x54 | Ghidra import */
void fightOutPokemonEnemyInit(u32 r3)
{
    extern void fightOutPokemonEnemyBiosSetDamage();
    extern void fightOutPokemonEnemyBiosSetInitHp();
    extern void fightOutPokemonEnemyBiosSetOumuWazaDataId();
    extern void fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr();
    fightOutPokemonEnemyBiosSetTargetFightOutPokemonPtr(r3, 0);
    fightOutPokemonEnemyBiosSetOumuWazaDataId(r3, 0);
    fightOutPokemonEnemyBiosSetInitHp(r3, 0);
    fightOutPokemonEnemyBiosSetDamage(r3, 0);
}

/* Address: 0x8020E7AC | Size: 0x1b0 | Ghidra import */
extern s16 fightTrainerEnemyPokemonBiosGetFightEntryeId(void* enemy);
extern void fightTrainerEnemyPokemonBiosSetFightEntryeId(void* enemy, s16 id);
extern void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(void* enemy, u32 flag);
extern void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(void* enemy, u32 flag);
extern void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(void* enemy, u32 flag);
extern void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(void* enemy, u32 flag);
extern void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(void* enemy, u32 flag);
extern void fightTrainerEnemyPokemonBiosSetStoreTokuseiData(void* enemy, u32 data);
extern void fightTrainerEnemyPokemonBiosSetTokuseiFlag(void* enemy, u32 flag);
extern void fn_801FBDF4(void* enemy, u8 index, u32 value);

static inline u8 fightTrainerEnemyCheckValid(void* enemy)
{
    if (enemy == NULL) {
        return 0;
    }
    if (fightTrainerEnemyPokemonBiosGetFightEntryeId(enemy) < 0) {
        return 0;
    }
    return 1;
}

static inline void* fightTrainerEnemySearch(void* ary, u16 count, s16 id)
{
    void* enemy;
    u16 i;

    if (ary == NULL) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        enemy = (u8*)ary + i * 0x14;
        if (id < 0) {
            if (fightTrainerEnemyCheckValid(enemy) == 0) {
                return enemy;
            }
        } else if (fightTrainerEnemyCheckValid(enemy) &&
                   id == fightTrainerEnemyPokemonBiosGetFightEntryeId(enemy)) {
            return enemy;
        }
    }
    return NULL;
}

static inline void fightTrainerEnemyInit(void* enemy)
{
    u8 j;

    fightTrainerEnemyPokemonBiosSetFightEntryeId(enemy, -1);
    for (j = 0; j < 4; j++) {
        fn_801FBDF4(enemy, j, 0);
    }
    fightTrainerEnemyPokemonBiosSetTokuseiFlag(enemy, 0);
    fightTrainerEnemyPokemonBiosSetStoreTokuseiData(enemy, 0);
    fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(enemy, 0);
    fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(enemy, 0);
    fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(enemy, 0);
    fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(enemy, 0);
    fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(enemy, 0);
}

u32 fightTrainerEnemyPokemonEraseAry(void* ary, u16 count, s16 id)
{
    void* enemy;

    if (ary == NULL) {
        return 0;
    }
    if (id < 0) {
        return 0;
    }
    enemy = fightTrainerEnemySearch(ary, count, id);
    if (enemy == NULL) {
        return 0;
    }
    fightTrainerEnemyInit(enemy);
    return 1;
}

/* Address: 0x8020E95C | Size: 0x24c | Ghidra import */
static inline void fightTrainerEnemyCreate(void* enemy, s16 id)
{
    if (enemy == NULL) {
        return;
    }
    if (id < 0) {
        return;
    }
    fightTrainerEnemyInit(enemy);
    fightTrainerEnemyPokemonBiosSetFightEntryeId(enemy, id);
}

u32 fightTrainerEnemyPokemonRegistAry(void* ary, u16 count, s16 id)
{
    void* enemy;

    if (ary == NULL) {
        return 0;
    }
    if (id < 0) {
        return 0;
    }
    if (fightTrainerEnemySearch(ary, count, id) != NULL) {
        return 0;
    }
    enemy = fightTrainerEnemySearch(ary, count, -1);
    if (enemy == NULL) {
        return 0;
    }
    fightTrainerEnemyCreate(enemy, id);
    return 1;
}

/* Address: 0x8020EBA8 | Size: 0xfc | Ghidra import */
int fightTrainerEnemyPokemonSearchAry(void* p1, u16 p2, s16 p3) {
    extern s16 fightTrainerEnemyPokemonBiosGetFightEntryeId();
    u8 bVar1;
    s16 sVar2;
    void* iVar4;
    u16 uVar3;

    if (p1 == 0) return 0;
    for (uVar3 = 0; uVar3 < p2; uVar3++) {
        iVar4 = (void*)((u32)p1 + (u32)uVar3 * 0x14);
        if (p3 < 0) {
            if (iVar4 == 0) {
                bVar1 = 0;
            } else {
                sVar2 = fightTrainerEnemyPokemonBiosGetFightEntryeId(iVar4);
                if (sVar2 < 0) {
                    bVar1 = 0;
                } else {
                    bVar1 = 1;
                }
            }
            if (bVar1 == 0) {
                return (s32)iVar4;
            }
        } else {
            if (iVar4 == 0) {
                bVar1 = 0;
            } else {
                sVar2 = fightTrainerEnemyPokemonBiosGetFightEntryeId(iVar4);
                if (sVar2 < 0) {
                    bVar1 = 0;
                } else {
                    bVar1 = 1;
                }
            }
            if (bVar1 && p3 == (sVar2 = fightTrainerEnemyPokemonBiosGetFightEntryeId(iVar4))) {
                return (s32)iVar4;
            }
        }
    }
    return 0;
}

/* 0x8020ECA4 | size: 0x3C | small */
u32 fightTrainerEnemyPokemonCheckValid(void* obj) {
    extern s16 fightTrainerEnemyPokemonBiosGetFightEntryeId();
    s16 val;
    if (obj == 0) return 0;
    val = fightTrainerEnemyPokemonBiosGetFightEntryeId(obj);
    return (val >= 0) ? 1 : 0;
}

/* Address: 0x8020ECE0 | Size: 0xdc | Ghidra import */
void fightTrainerEnemyPokemonInitAry(void* ctx, u16 count) {
    extern void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag();
    extern void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag();
    extern void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetStoreTokuseiData();
    extern void fightTrainerEnemyPokemonBiosSetTokuseiFlag();
    extern void fn_801FBDF4(void* entry, u8 idx, u32 zero);
    extern void fightTrainerEnemyPokemonBiosSetFightEntryeId();
    u8 j;
    void* entry;
    u16 i;
    if (ctx == NULL) { return; }
    for (i = 0; i < count; i++) {
        entry = (void*)((u32)ctx + (u32)i * 0x14);
        fightTrainerEnemyPokemonBiosSetFightEntryeId(entry, (void*)0xffffffff);
        for (j = 0; j < 4; j++) {
            fn_801FBDF4(entry, j, 0);
        }
        fightTrainerEnemyPokemonBiosSetTokuseiFlag(entry, 0);
        fightTrainerEnemyPokemonBiosSetStoreTokuseiData(entry, 0);
        fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(entry, 0);
        fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(entry, 0);
        fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(entry, 0);
        fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(entry, 0);
        fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(entry, 0);
    }
}

/* 0x8020EDBC | size: 0x60 */
void fightTrainerEnemyPokemonInitFightOutStatus(void* ctx) {
    extern void fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag();
    extern void fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag();
    extern void fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag();
    extern void fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag();
    fightTrainerEnemyPokemonBiosSetNowhp1banhikuiFlag(ctx, 0);
    fightTrainerEnemyPokemonBiosSetLv1banhikuiFlag(ctx, 0);
    fightTrainerEnemyPokemonBiosSetDefense1banhikuiFlag(ctx, 0);
    fightTrainerEnemyPokemonBiosSetBadwazaHaveFlag(ctx, 0);
    fightTrainerEnemyPokemonBiosSetParam1bantakaiFlag(ctx, 0);
}
