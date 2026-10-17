/**
 * @file toolentry.c
 * @brief game/pxdvs/app/toolentry/toolentry.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x8025CD64-0x8025DBD4, 34 fns.
 *
 * XD source unit: game/pxdvs/app/toolentry/toolentry.cpp
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void battleCameraSelect();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

typedef struct ToolentrySpeciesList {
    u16 speciesId[1];
} ToolentrySpeciesList;

static inline u16 toolentryCountValidPokemon(s32 player) {
    extern void* savedataBattleGetEntryPtr(void*);
    extern u8 pokemonCheckValid(void*);
    extern void* heroBiosGetPokemonPtr(void*, u32);
    u16 count;
    s32 i;
    u8 valid;

    count = 0;
    for (i = 0; i < 6; i++) {
        valid = pokemonCheckValid(heroBiosGetPokemonPtr((u8*)savedataBattleGetEntryPtr((void*)player) + 0xb44, (u16)i)) != 0;
        if (valid) {
            count++;
        }
    }
    return count;
}

static inline u16 toolentryEntryPokemonNum(s32 player) {
    extern u32 fn_8006B1D4(void);
    u16 limit;
    u16 num;

    limit = fn_8006B1D4();
    num = toolentryCountValidPokemon(player);
    if (num < limit) {
        return num;
    }
    return limit;
}

static inline void* toolentryHeroPokemonPtr(s32 player, s32 index) {
    extern u8* savedataBattleGetEntryPtr(void*);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    return heroBiosGetPokemonPtr(savedataBattleGetEntryPtr((void*)player) + 0xb44, index);
}

static inline void* toolentryEntryPokemonPtr(s32 player, s32 index) {
    extern u8* savedataBattleGetEntryPtr(void*);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    return heroBiosGetPokemonPtr(savedataBattleGetEntryPtr((void*)player) + 0x2c, index);
}

#if !defined(TOOLENTRY_8025D788_ONLY) && !defined(TOOLENTRY_8025D164_ONLY) && !defined(TOOLENTRY_8025D3F4_ONLY) && !defined(TOOLENTRY_8025D644_ONLY)

/* Address: 0x8025CD64 | Size: 0x54 | Pattern: field_accessor */
#ifdef TOOLENTRY_DEBUG_ONLY
static inline
#endif
void toolentryTaisenFreePokemonData(void* ctx, u32 slot, u32 param) {
    extern u32 lbl_8047B650;
    extern u32 GSmemGetHandleByPtr();
    extern void GSmemFree();
    extern void fn_800E24B0();
    u32 handle;
    u32 result;
    handle = lbl_8047B650;
    if (handle != 0) {
        result = GSmemGetHandleByPtr(handle);
        if ((result & 0xFFFF) != 0) {
            fn_800E24B0(result);
            GSmemFree(result);
        }
        lbl_8047B650 = 0;
    }
}

/* RULE-EXCEPTION(user-approved): inline helper used by one function — see docs/RULE_EXCEPTIONS.md */
static inline void toolentryCopyHeroData(void* dst, void* src) {
    extern void* memcpy(void*, const void*, u32);
    if (src != 0) {
        memcpy(dst, src, 0xb18);
    }
}

/* RULE-EXCEPTION(user-approved): one-use inline boundary preserves register allocation;
 * see docs/RULE_EXCEPTIONS.md and docs/recon/toolentry_debug_pokemon_residual_20261008.md. */
static inline void toolentryCreateDebugPokemonList(u8* pokemon) {
    extern s32 lbl_80478D98;
    extern void* GSmsgGetGSchar(u32);
    extern void pokemonAllKaihuku(void*);
    extern void pokemonSetCatchStatus(void*, u32, u32, u32, u32, u32, void*);
    extern void pokemonCreate(void*, u16, u32, void*);
    extern void* gamedataGetStatus(u32, u32);
    s32 i;

    for (i = 0; i < lbl_80478D98; i++) {
        pokemonCreate(pokemon, i + 1, 10, gamedataGetStatus(0, 1));
        pokemonSetCatchStatus(pokemon, 0, 8, 1, 0, 0, GSmsgGetGSchar(i + 0x1004));
        pokemonAllKaihuku(pokemon);
        pokemon += 0x138;
    }
}

/* Address: 0x8025CDB8 | Size: 0x2B4 (692 bytes) */
void toolentryDebugPokemonCreate(void) {
    extern s32 lbl_80478D98;
    extern void* lbl_8047B650;
    extern u8* lbl_8047B654;
    extern u8* savedataBattleGetEntryPtr(void*);
    extern void* GSmsgGetGSchar(u32);
    extern void pokemonAllKaihuku(void*);
    extern void pokemonSetCatchStatus(void*, u32, u32, u32, u32, u32, void*);
    extern void pokemonCreate(void*, u16, u32, void*);
    extern void pokemonInit(void*);
    extern void* savedataGetStatus(u32, u32);
    extern void heroInit(void*);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    extern void* gamedataGetStatus(u32, u32);
    extern void* memcpy(void*, const void*, u32);
    extern u32 GSmemGetHandleByPtr(void*);
    extern void GSmemFree(u32);
    extern void fn_800E24B0(u32);
    extern u32 GSmemAlloc(u32, u32);
    extern void* GSmemLock(u32);
    s32 i;
    s32 j;
    u32 handle;
    void* buffer;
    u8* src;
    void* dst;
    s32 offset;
    s32 k;

    lbl_8047B654 = 0;
    handle = GSmemAlloc((lbl_80478D98 * 0x138 + 0x1f) & ~0x1f, 0x20);
    if ((u16)handle != 0) {
        buffer = GSmemLock(handle);
    } else {
        buffer = 0;
    }
    lbl_8047B654 = buffer;
    toolentryCreateDebugPokemonList(buffer);

    toolentryTaisenFreePokemonData(0, 0, 0);
    handle = GSmemAlloc(0x80, 0x20);
    if ((u16)handle != 0) {
        buffer = GSmemLock(handle);
    } else {
        buffer = 0;
    }
    lbl_8047B650 = buffer;

    for (j = 0; j < 4; j++) {
        heroInit(savedataBattleGetEntryPtr((void*)j) + 0xb44);
        for (i = 0; i < 6; i++) {
            pokemonInit(heroBiosGetPokemonPtr(savedataBattleGetEntryPtr((void*)j) + 0xb44, i));
        }
    }
    for (j = 0; j < 4; j++) {
        heroInit(savedataBattleGetEntryPtr((void*)j) + 0x2c);
        for (i = 0; i < 6; i++) {
            pokemonInit(heroBiosGetPokemonPtr(savedataBattleGetEntryPtr((void*)j) + 0x2c, i));
        }
    }
    /* RULE-EXCEPTION(user-approved): empty loop kept for retail's six-iteration CTR loop — see docs/RULE_EXCEPTIONS.md */
    for (i = 0; i < 6; i++) {
    }

    toolentryCopyHeroData(savedataBattleGetEntryPtr(0) + 0xb44, savedataGetStatus(0, 2));
    offset = 0;
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            toolentryCopyHeroData(savedataBattleGetEntryPtr((void*)i) + 0xb44, savedataGetStatus(0, 2));
        } else {
            for (k = 0; k < 6; k++) {
                src = lbl_8047B654 + offset;
                dst = heroBiosGetPokemonPtr(savedataBattleGetEntryPtr((void*)i) + 0xb44, k);
                if (dst != 0) {
                    memcpy(dst, src, 0x138);
                }
                offset += 0x138;
            }
        }
    }

    if (lbl_8047B654 != 0) {
        handle = GSmemGetHandleByPtr(lbl_8047B654);
        if ((u16)handle != 0) {
            fn_800E24B0(handle);
            GSmemFree(handle);
        }
        lbl_8047B654 = 0;
    }
}

/* Address: 0x8025D06C | Size: 0x3c | Ghidra import */
#ifndef TOOLENTRY_DEBUG_ONLY
u32 couponAddFromResult(void)
{
    extern u32 savedataBattleGetCouponRun();
    extern void savedataBattleGetSavedHero();
    extern void* fn_8006B5A8();
    extern void heroAddPokecoupon();
    u32 uVar1;
  fn_8006B5A8();
  savedataBattleGetSavedHero();
  uVar1 = savedataBattleGetCouponRun();
  heroAddPokecoupon(0,uVar1);
  return 0;
}


/* Address: 0x8025D0A8 | Size: 0xBC */
f32 heroCountPartyInList(void* ctx, u32 param1, u32 param2) {
    extern ToolentrySpeciesList* lbl_80478EAC;
    extern f32 lbl_8047E658;
    extern f32 lbl_8047E65C;
    extern u16 pokemonBiosGetPokemonDataId(void*);
    extern u8 pokemonCheckValid(void*);
    extern void* savedataGetStatus(u32, u32);
    extern void* heroBiosGetPokemonPtr(void*, u32);
    u32 i;
    u32 count;
    void* member;
    void* party;
    ToolentrySpeciesList* speciesList;
    u32 offset;
    u16 idx;
    u16 species;
    u16 entry;
    f32 scale;
    f32 factor;

    count = 0;
    if ((party = ctx) == 0) {
        party = savedataGetStatus(0, 2);
    }
    for (i = 0; (s32)i < 6; i++) {
        member = heroBiosGetPokemonPtr(party, i & 0xFFFF);
        if (pokemonCheckValid(member) != 0) {
            species = pokemonBiosGetPokemonDataId(member);
            speciesList = lbl_80478EAC;
            offset = 0;
            while (1) {
                entry = speciesList->speciesId[offset];
                if (entry == 0) {
                    break;
                }
                if (species == entry) {
                    count++;
                }
                offset++;
            }
        }
    }
    scale = lbl_8047E658;
    factor = lbl_8047E65C;
    while ((s32)count > 0) {
        scale *= factor;
        count--;
    }
    return scale;
}
#endif

#endif /* carve guards */

#if !defined(TOOLENTRY_8025D788_ONLY) && !defined(TOOLENTRY_DEBUG_ONLY) && !defined(TOOLENTRY_8025D3F4_ONLY) && !defined(TOOLENTRY_8025D644_ONLY)
/* Address: 0x8025D164 | Size: 0x128 (296 bytes) */
s32 couponCalcEarned(void) {
    extern f32 lbl_8039A648[];
    extern f32 lbl_8039A664[];
    extern ToolentrySpeciesList* lbl_80478EAC;
    extern f32 lbl_8047E658;
    extern f32 lbl_8047E65C;
    extern u8* savedataBattleGetEntryPtr(void*);
    extern void* fn_8006B5A8();
    extern u16 pokemonBiosGetPokemonDataId(void*);
    extern u8 pokemonCheckValid(void*);
    extern void* heroBiosGetPokemonPtr(void*, u16);
    void* member;
    s32 wins;
    s32 i;
    s32 count;
    s32 level;
    s32 type;
    s32 rank;
    ToolentrySpeciesList* speciesList;
    u32 offset;
    u16 species;
    u16 entry;
    f32 scale;
    f32 factor;

    count = 0;
    level = *(s32*)((u8*)fn_8006B5A8() + 0xc);
    type = *(s32*)fn_8006B5A8();
    wins = *(s32*)((u8*)fn_8006B5A8() + 0x14);
    for (i = 0; i < 6; i++) {
        member = heroBiosGetPokemonPtr(savedataBattleGetEntryPtr(0) + 0xb44, i);
        if (pokemonCheckValid(member)) {
            species = pokemonBiosGetPokemonDataId(member);
            speciesList = lbl_80478EAC;
            offset = 0;
            while (1) {
                entry = speciesList->speciesId[offset];
                if (entry == 0) {
                    break;
                }
                if (species == entry) {
                    count++;
                }
                offset++;
            }
        }
    }
    scale = lbl_8047E658;
    factor = lbl_8047E65C;
    while (count > 0) {
        scale *= factor;
        count--;
    }
    if (type == 1) {
        rank = (wins + 1) / 10;
        if (rank > 10) {
            rank = 10;
        }
        scale *= lbl_8039A664[rank];
    } else {
        if (level >= 6) {
            level = 6;
        }
        scale *= lbl_8039A648[level];
    }
    return (s32)scale;
}

#endif /* TOOLENTRY_8025D788_ONLY */

/* Address: 0x8025D28C | Size: 0x24 | Pattern: null_check_getter */
extern void* savedataBattleGetEntryPtr(void*);

#if !defined(TOOLENTRY_8025D788_ONLY) && !defined(TOOLENTRY_8025D164_ONLY) && !defined(TOOLENTRY_DEBUG_ONLY)
#if !defined(TOOLENTRY_8025D3F4_ONLY) && !defined(TOOLENTRY_8025D644_ONLY)
u16 toolentryTaisenGetTrainerDataID(void* ctx) { return *(u16*)savedataBattleGetEntryPtr(ctx); }

/* Address: 0x8025D2B0 | Size: 0x24 | Pattern: null_check_getter */
u32 toolentryTaisenGetControlerType(void* ctx) { return *(u32*)((u8*)savedataBattleGetEntryPtr(ctx) + 0x24); }

/* Address: 0x8025D2D4 | Size: 0x90 */
u32 toolentryGetTrainerSamllFaceResID(void* ctx, u32 param1, u32 param2) {
    extern u32 lbl_80478E04;
    extern void* savedataBattleGetEntryPtr(void*);
    extern u32 fn_801FCBA4(void);
    extern void fightTrainerDataBiosGetPtr(u32);
    u32 id;
    u32 base;
    u32 offset;
    u32* entry;
    u32 ret;

    id = *(u16*)savedataBattleGetEntryPtr(ctx);
    if (id == 0) {
        return 0;
    }
    fightTrainerDataBiosGetPtr(id);
    offset = fn_801FCBA4();
    offset *= 0x14;
    base = lbl_80478E04;
    entry = (u32*)(base + offset);
    if ((s32)param1 == 0) {
        ret = entry[3];
        if (ret == 0) {
            return 0xf941200;
        }
        return ret;
    }
    ret = entry[4];
    if (ret == 0) {
        return 0xf8f1200;
    }
    return ret;
}

/* Address: 0x8025D364 | Size: 0x90 */
u32 toolentryGetTrainerBicFaceResID(void* ctx, u32 param1, u32 param2) {
    extern u32 lbl_80478E04;
    extern void* savedataBattleGetEntryPtr(void*);
    extern u32 fn_801FCBA4(void);
    extern void fightTrainerDataBiosGetPtr(u32);
    u16 id16;
    u32 id;
    u32 base;
    u32 offset;
    u32* entry;
    u32 ret;

    if ((s32)ctx != 0) {
        id16 = *(u16*)savedataBattleGetEntryPtr(ctx);
    } else {
        id16 = *(u16*)savedataBattleGetEntryPtr(ctx);
    }
    id = id16;
    if (id == 0) {
        return 0;
    }
    fightTrainerDataBiosGetPtr(id);
    offset = fn_801FCBA4();
    offset *= 0x14;
    base = lbl_80478E04;
    entry = (u32*)(base + offset);
    if ((s32)param1 == 0) {
        return entry[1];
    }
    ret = entry[2];
    if (ret == 0) {
        return 0xf991200;
    }
    return ret;
}

#endif /* !TOOLENTRY_8025D3F4_ONLY */

#if !defined(TOOLENTRY_8025D644_ONLY)
/* Address: 0x8025D3F4 | Size: 0x16C (364 bytes) */
void toolentryTaisenEntryPokemon(void* ctx) {
    typedef struct ToolentryHeroData {
        u32 words[0xB18 / 4];
    } ToolentryHeroData;
    typedef struct ToolentryPokemonData {
        u32 words[0x138 / 4];
    } ToolentryPokemonData;
    extern u8* savedataBattleGetEntryPtr(void*);
    ToolentryHeroData* hero;
    ToolentryPokemonData* src;
    ToolentryPokemonData* dst;
    s32 i;
    s32 order;
    s32 num;

    /* RULE-EXCEPTION(user-approved): codegen-only (void*)(s32) casts on ctx and local order chosen for register allocation — see docs/RULE_EXCEPTIONS.md */
    num = toolentryEntryPokemonNum((s32)ctx);
    hero = (ToolentryHeroData*)(savedataBattleGetEntryPtr((void*)(s32)ctx) + 0x2c);
    *hero = *(ToolentryHeroData*)(savedataBattleGetEntryPtr((void*)(s32)ctx) + 0xb44);
    for (i = 0; i < num; i++) {
        order = *(s32*)(savedataBattleGetEntryPtr((void*)(s32)ctx) + 8 + i * 4);
        src = toolentryHeroPokemonPtr((s32)ctx, order);
        toolentryEntryPokemonPtr((s32)ctx, i);
        dst = toolentryEntryPokemonPtr((s32)ctx, i);
        if (dst != 0 && src != 0) {
            *dst = *src;
        }
    }
}
#endif /* !TOOLENTRY_8025D644_ONLY */

#if !defined(TOOLENTRY_8025D3F4_ONLY)
#if !defined(TOOLENTRY_8025D644_ONLY)
/* Address: 0x8025D560 | Size: 0x24 | Pattern: null_check_getter */
u32 toolentryTaisengetEtnryPokemonOrderNum(void* ctx) { return *(u32*)((u8*)savedataBattleGetEntryPtr(ctx) + 0x20); }

/* Address: 0x8025D584 | Size: 0x5C | Pattern: field_accessor */
u32 toolentryTaisenDeleteEtnryPokemonOrder(void* ctx, u32 slot, u32 param) {
    typedef struct BattleFieldAccessor {
        u8 unk_00[8];
        u32 values[6];
        u32 count;
    } BattleFieldAccessor;
    extern BattleFieldAccessor* savedataBattleGetEntryPtr(void*);
    BattleFieldAccessor* entry;
    s32 index;

    entry = savedataBattleGetEntryPtr(ctx);
    index = entry->count - 1;
    if ((index < 0) || (index > 6)) {
        return 0;
    }
    entry->values[index] = (u32)-1;
    entry->count--;
    return entry->count;
}

/* Address: 0x8025D5E0 | Size: 0x64 | Pattern: field_accessor */
u32 toolentryTaisenSetEtnryPokemonOrderGBA(void* ctx, s32 count, u32* src) {
    typedef struct BattleFieldAccessor {
        u8 unk_00[8];
        u32 values[6];
        u32 count;
    } BattleFieldAccessor;
    extern BattleFieldAccessor* savedataBattleGetEntryPtr(void*);
    BattleFieldAccessor* dst;
    s32 i;

    dst = savedataBattleGetEntryPtr(ctx);
    for (i = 0; i < count; i++) {
        dst->values[i] = src[i];
    }
    dst->count = i;
    return i;
}

#endif /* !TOOLENTRY_8025D644_ONLY */

/* Address: 0x8025D644 | Size: 0x100 (256 bytes) */
/* RULE-EXCEPTION(user-approved): unsigned player parameter, a long copy of it,
 * and a do/while(0) around the duplicate check, chosen for codegen (permuter
 * farm) — see docs/RULE_EXCEPTIONS.md */
s32 toolentryTaisenSetEtnryPokemonOrder(unsigned int player, s32 order) {
    long playerId;
    typedef struct BattleFieldAccessor {
        u8 unk_00[8];
        s32 values[6];
        s32 count;
    } BattleFieldAccessor;
    extern BattleFieldAccessor* savedataBattleGetEntryPtr(void*);
    BattleFieldAccessor* entry;
    s32 entryCount;
    s32 i;

    playerId = player;
    entry = savedataBattleGetEntryPtr((void*)playerId);
    entryCount = entry->count;
    if (entry->count >= toolentryEntryPokemonNum(playerId)) {
        return -1;
    }
    for (i = 0; i < entryCount; i++) {
        do {
            if (entry->values[i] == order) {
                return -1;
            }
        } while (0);
    }
    entry->values[entryCount] = order;
    entry->count++;
    return entryCount;
}

#if !defined(TOOLENTRY_8025D644_ONLY)
/* Address: 0x8025D744 | Size: 0x44 | Pattern: field_accessor */
u32 toolentryTaisenInitPokemonOrder(void* ctx, u32 slot, u32 param) {
    extern void* savedataBattleGetEntryPtr();
    u8* base;
    u32 i;
    base = (u8*)savedataBattleGetEntryPtr(ctx);
    *(u32*)(base + 0x20) = 0;
    for (i = 0; i < 6; i++) {
        *(u32*)(base + 0x8 + i * 4) = (u32)-1;
    }
    return (u32)base;
}
#endif /* !TOOLENTRY_8025D644_ONLY */

#endif /* !TOOLENTRY_8025D3F4_ONLY */
#endif /* carve guards */

#if !defined(TOOLENTRY_8025D164_ONLY) && !defined(TOOLENTRY_DEBUG_ONLY) && !defined(TOOLENTRY_8025D3F4_ONLY) && !defined(TOOLENTRY_8025D644_ONLY)
/* Address: 0x8025D788 | Size: 0x80 | Pattern: field_accessor */
void toolentryCopyHero(void* ctx, u32 slot, u32 param) {
    typedef struct {
        u32 words[0x2C6];
    } BattleCopyBlock;
    BattleCopyBlock* src;
    s32 i;
    BattleCopyBlock* dst;

    i = 0;
    do {
        src = (BattleCopyBlock*)((u8*)savedataBattleGetEntryPtr((void*)i) + 0xb44);
        dst = (BattleCopyBlock*)((u8*)savedataBattleGetEntryPtr((void*)i) + 0x2c);
        if ((src != NULL) && (dst != NULL)) {
            *dst = *src;
        }
        i++;
    } while ((s32)i < 4);
}

/* Address: 0x8025D808 | Size: 0x94 */
u16 toolentryTaisenGetEntryPokemonNum(void* ctx, u32 param1, u32 param2) {
    u16 limit;
    u16 num;

    extern u32 fn_8006B1D4(void);
    limit = fn_8006B1D4();
    num = toolentryCountValidPokemon((s32)ctx);
    if (num < limit) {
        return num;
    }
    return limit;
}

/* Address: 0x8025D89C | Size: 0x78 | Pattern: field_accessor */
u32 toolentryTaisenGetPokemonNum(void* ctx, u32 slot, u32 param) {
    extern void* savedataBattleGetEntryPtr(void*);
    extern u32 pokemonCheckValid(void*);
    extern void* heroBiosGetPokemonPtr(void*, u32);
    u8 sp[0x20];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r31 = 0;
    u32 r30 = 0;
    u32 r29 = 0;
    u32 r4 = slot;

    r30 = 0x0;
    r31 = r3;
    r29 = 0x0;
    do {
        r3 = (u32)savedataBattleGetEntryPtr((void *)r31);
        r4 = r29 & 0xFFFF;
        r3 = (u32)heroBiosGetPokemonPtr((void *)(r3 + 0xb44), r4);
        r3 = pokemonCheckValid((void *)r3);
        r3 = r3 & 0xFF;
        r0 = r3 != 0;
        r0 = r0 & 0xFF;
        if ((s32)r0 != (s32)0) {
            r3 = r30 & 0xFFFF;
            r0 = r3 + 0x1;
            r30 = r0 & 0xFFFF;
        }
        r29 = r29 + 0x1;
    } while ((s32)r29 < (s32)0x6);
    return r30;
}

#endif /* TOOLENTRY_8025D164_ONLY */

#if !defined(TOOLENTRY_8025D788_ONLY) && !defined(TOOLENTRY_8025D164_ONLY) && !defined(TOOLENTRY_DEBUG_ONLY) && !defined(TOOLENTRY_8025D3F4_ONLY) && !defined(TOOLENTRY_8025D644_ONLY)

/* Address: 0x8025D914 | Size: 0x24 | Pattern: null_check_getter */
void* toolentryTaisenGetHeroPtr(void* ctx) { return (u8*)savedataBattleGetEntryPtr(ctx) + 0xb44; }

/* Address: 0x8025D938 | Size: 0x38 | Ghidra import */
u32 toolentryTaisenGetEntryPokemonPtr(u32 r3, u16 r4)
{
    extern void* savedataBattleGetEntryPtr();
    extern void heroBiosGetPokemonPtr();
    int iVar1;
  iVar1 = (int)savedataBattleGetEntryPtr();
  heroBiosGetPokemonPtr(iVar1 + 0x2c, r4);
}


/* Address: 0x8025D970 | Size: 0x38 | Ghidra import */
u32 toolentryTaisenGetPokemonPtr(u32 r3, u16 r4)
{
    extern void* savedataBattleGetEntryPtr();
    extern void heroBiosGetPokemonPtr();
    int iVar1;
  iVar1 = (int)savedataBattleGetEntryPtr();
  heroBiosGetPokemonPtr(iVar1 + 0xb44, r4);
}


/* Address: 0x8025D9A8 | Size: 0x24 | Pattern: null_check_getter */
extern void* fn_8006B5A8(void*);
u32 battleResultGet00(void* ctx) { return *(u32*)fn_8006B5A8(ctx); }

/* Address: 0x8025D9CC | Size: 0x24 | Pattern: null_check_getter */
u32 battleResultGet10(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x10); }

/* Address: 0x8025D9F0 | Size: 0x28 | Ghidra import */
u32 toolentryTaisenGetHomePlace(void)
{
    extern u32 fn_8006A7E8();
    extern void* savedataBattleGetEntryPtr();
    u16 uVar1;
  savedataBattleGetEntryPtr();
  uVar1 = fn_8006A7E8();
  return uVar1;
}


/* Address: 0x8025DA18 | Size: 0x24 | Pattern: accessor */
u32 toolentryTaisenGetBattlePlayerID(void* ctx) {
    extern void* savedataBattleGetEntryPtr(void*);
    extern u32 fn_8006A7D8(void);

    savedataBattleGetEntryPtr(ctx);
    return fn_8006A7D8();
}

/* toolentryTaisenGetEntryPlayerNum | Size: 0x4C | Get battle party size based on mode */
u32 toolentryTaisenGetEntryPlayerNum(void) {
    s32 mode;
    u32 res;
    extern void* fn_8006B5A8(void);
    void* result = fn_8006B5A8();
    res = 2;
    mode = *(s32*)((u8*)result + 0x4);
    switch (mode) {
        case 0:
        case 1:
            res = 2;
            break;
        case 2:
            res = 4;
            break;
    }
    return res;
}

/* Address: 0x8025DA88 | Size: 0x24 | Pattern: null_check_getter */
u32 toolentryTaisenGetBattleType(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x4); }

/* Address: 0x8025DAAC | Size: 0x24 | Pattern: null_check_getter */
u32 battleResultGet0C(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0xc); }

/* Address: 0x8025DAD0 | Size: 0x24 | Pattern: null_check_getter */
u32 battleResultGet08(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x8); }

/* Address: 0x8025DAF4 | Size: 0x38 | Ghidra import */
u32 battleResultDec18(void)
{
    extern void* fn_8006B5A8();
    u32 uVar1;
    uVar1 = (u32)fn_8006B5A8();
    if (*(u32 *)(uVar1 + 0x18) != 0) {
        *(u32 *)(uVar1 + 0x18) = *(u32 *)(uVar1 + 0x18) - 1;
    }
    return *(u32 *)(uVar1 + 0x18);
}


/* Address: 0x8025DB2C | Size: 0x30 | Ghidra import */
u32 battleResultInc18(void)
{
    extern void* fn_8006B5A8();
    int iVar1;
  iVar1 = (int)fn_8006B5A8();
  *(int *)(iVar1 + 0x18) = *(int *)(iVar1 + 0x18) + 1;
  return *(u32 *)(iVar1 + 0x18);
}


/* Address: 0x8025DB5C | Size: 0x24 | Pattern: null_check_getter */
u32 battleResultGet18(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x18); }

/* Address: 0x8025DB80 | Size: 0x30 | Ghidra import */
u32 battleResultInc14(void)
{
    extern void* fn_8006B5A8();
    int iVar1;
  iVar1 = (int)fn_8006B5A8();
  *(int *)(iVar1 + 0x14) = *(int *)(iVar1 + 0x14) + 1;
  return *(u32 *)(iVar1 + 0x14);
}


/* Address: 0x8025DBB0 | Size: 0x24 | Pattern: null_check_getter */
u32 battleResultGet14(void* ctx) { return *(u32*)((u8*)fn_8006B5A8(ctx) + 0x14); }

#endif /* carve guards */
