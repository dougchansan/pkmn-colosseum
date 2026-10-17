/**
 * @file pokemon_range_8011F77C.c
 * @brief Pokemon TU block .text 0x8011F77C-0x80120B00 with its .sdata2
 *        literal pool (0x8047CFF0-0x8047D020).
 *
 * pokemonGetDarkPokemonLevel, pokemonAddDpFormPokemonDpFilterId,
 * pokemonSetDp, pokemonGetDp, pokemonIsDarkPokemon,
 * pokemonSetDarkPokemonStatus, pokemonToMenuPokemonStatus,
 * pokemonToMenuPokemonStatusSubBar, pokemonToMenuWazaStatus,
 * pokemonCheckSetMonohiroi and pokemonAllKaihuku. The pool (0.0f, 100.0f,
 * the 80/60/40/20 dark-gauge thresholds, the int-to-float biases, -1.0f) is
 * read only by these functions, so the block links as one object; it
 * replaces the former pokemon_range_8011F77C/8011F910/8011FC14/8011FCA4
 * candidates and the pokemon_range_exact_8011FBCC/8011FC74 carves.
 *
 * Pokemon XD calls out of line what Colosseum expands here: pokemonGetDp
 * from pokemonGetDarkPokemonLevel and pokemonAddDpFormPokemonDpFilterId,
 * pokemonSetDp from pokemonSetDarkPokemonStatus, and
 * pokemonToMenuPokemonStatusSubBar from pokemonToMenuPokemonStatus
 * (GXXE01 pokemon.o: 0x8013EFA0, 0x8013F0F4 calling 0x8013F20C;
 * StarsMmd/Colo-XD-PBR-symbol-maps @ 6b51d3af, trevor403/xd-asm @ b1087f18).
 * Each callee comes after its caller in both binaries, so the callers can
 * only expand them with deferred inlining, and MWCC then emits the
 * functions in reverse source order: they are written bottom-up here and
 * come out in retail's order, with the literal pool in retail's order.
 *
 * RULE-EXCEPTION(title-path): per-unit compiler flag chosen because it
 * matches (-inline auto,deferred) and the reverse source order that follows
 * from it; pokemonCheckSetMonohiroi's table is an extern stand-in -- see
 * docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern s32 pokemonGetStatus(u8* object, u16 data_id, u32 status, u16 index);
extern void pokemonSetStatus(u8* object, u32 data_id, u32 status, u32 index, u32 value);
extern u8* pokemonSeikakuDataBiosGetPtr(u8 index);
extern u8* pokemonDpFilterDataBiosGetPtr(u16 index);
extern s32 pokemonDpFilterDataBiosGetValue(u8* data);
extern u8* itemDataBiosGetPtr(u16 item_data_id);
extern u8 itemDataBiosGetKind(u8* data);
extern u32 itemDataBiosGetBuff(u8* data);
extern s32 pokemonSeikakuDataBiosGetReliveFightout(u8* data);
extern s32 pokemonSeikakuDataBiosGetReliveWalk(u8* data);
extern s32 pokemonSeikakuDataBiosGetReliveCall(u8* data);
extern s32 pokemonSeikakuDataBiosGetReliveSodateya(u8* data);
extern s32 pokemonSeikakuDataBiosGetReliveNadenade(u8* data);
extern u8* pokemonSeikakuRateDataBiosGetPtr(u8 index);
extern u8 pokemonSeikakuRateDataBiosGetKake(u8* data);
extern u8 pokemonSeikakuRateDataBiosGetWaru(u8* data);
extern u16 darkPokemonSaveGet4A6(u16 dark_id);
extern void fn_8011B950(u8* base, u16 count);
extern u16 joutaiDataBiosGetStatusKind(u16 idx);
extern u8 fn_8011B67C(u8* obj, u16 id);
extern void fn_8011B788(u8* obj, u16 id);
extern u16 pokemonGetFaceSpriteId(u8* pokemon);
extern u8 menuSubGetPokemonSexForFightDisp(u8* pokemon);
extern void* pokemonGrowDataBiosGetPtr(u8 idx);
extern u32 pokemonGrowDataBiosGetExp(void* ptr, u8 idx);
extern void GScharCpy(u8* dst, u32 src);
extern u32 GSmsgGetGSchar(u32 id);
extern u32 fn_8010C46C(u16 id);
extern u32 fn_8010C4D4(u16 id);
extern u8 wazaGetMaxPP(u16 id, u8 up);
extern u32 wazaGetStatus(u32 obj, u16 id, u32 status, u32 index);
extern void* memset(void* dst, int val, u32 size);
extern u16 GSrand(void);
extern const u16 lbl_8027296C[20];
/* Defined below: with deferred inlining their bodies are expanded into the
 * functions written above them. */
f32 pokemonGetDp(u8* ptr);
void pokemonSetDp(u8* ptr, f32 dp);
void pokemonToMenuPokemonStatusSubBar(u8* pokemon, struct MenuPokemonStatus* menu);

/* Static copies of pokemon-TU helpers linked from other objects (XD keeps
 * them out of line: pokemonGetLevelToExp 0x801405F8, pokemonIsJoutaiDataId
 * 0x8013FDA4, pokemonGetTokuseiDataId 0x801416A4, pokemonWazaCheckValid
 * 0x80141114, pokemonWazaGetMaxPP 0x801411E4); every caller here expands
 * them. */
static inline u32 pokemonGrowGetLevelToExp(u8 growId, u8 level)
{
    void* growData;

    growData = pokemonGrowDataBiosGetPtr(growId);
    if (growData == NULL) {
        return 0;
    }
    return pokemonGrowDataBiosGetExp(growData, level);
}

static inline u32 pokemonGetLevelToExp(u8* pokemon, u8 level)
{
    if (pokemon == NULL) {
        return 0;
    }
    return pokemonGrowGetLevelToExp(
        (u8)pokemonGetStatus(NULL, (u16)pokemonGetStatus(pokemon, 0, 0x6E, 0), 0x11, 0), level);
}

static inline u8 pokemonGetJoutai(u8* pokemon, u16 id)
{
    if (joutaiDataBiosGetStatusKind(id) != 0x7C && joutaiDataBiosGetStatusKind(id) != 0xC8) {
        return 0;
    }
    return fn_8011B67C(pokemon, id);
}

static inline u16 pokemonGetJoutaiIcon(u8* pokemon)
{
    if (pokemon == NULL) {
        return 0;
    }
    if (pokemonGetJoutai(pokemon, 3) == 1) {
        return 0x3A;
    }
    if (pokemonGetJoutai(pokemon, 4) == 1) {
        return 0x3A;
    }
    if (pokemonGetJoutai(pokemon, 5) == 1) {
        return 0x3B;
    }
    if (pokemonGetJoutai(pokemon, 6) == 1) {
        return 0x3C;
    }
    if (pokemonGetJoutai(pokemon, 7) == 1) {
        return 0x3D;
    }
    if (pokemonGetJoutai(pokemon, 8) == 1) {
        return 0x3E;
    }
    return 0;
}

typedef struct MenuPokemonStatus {
    u8 name[0x17];
    u8 level;
    u16 maxHp;
    u16 hp;
    union {
        u32 integer;
        f32 real;
    } progressMax;
    union {
        u32 integer;
        f32 real;
    } progress;
    u16 conditionIcon;
    u16 displayValue;
    u8 sex;
    u8 darkState;
    u16 status82;
} MenuPokemonStatus;

/* Colosseum's own pokemonWazaCheckValid (0x80123CD4, (Pokemon*, u16)) and
 * pokemonWazaGetMaxPP (0x80123E70, which adds 4 to the move slot for the
 * PP-up entries). */
static inline u8 pokemonWazaSlotIsValid(u8* pokemon, u16 slot)
{
    if (pokemon == NULL) {
        return 0;
    }
    if ((s32)pokemonGetStatus(pokemon, 0, 0x7F, slot) == 0) {
        return 0;
    }
    if ((s32)pokemonGetStatus(pokemon, 0, 0x7F, slot) == 0x163) {
        return 0;
    }
    return 1;
}

static inline u8 pokemonGetWazaMaxPP(u8* pokemon, u16 waza)
{
    u16 id;

    if (pokemon == NULL) {
        return 0;
    }
    id = (u16)pokemonGetStatus(pokemon, 0, 0x7F, waza + 4);
    return wazaGetMaxPP(id, (u8)pokemonGetStatus(pokemon, 0, 0x81, waza + 4));
}

static inline u16 pokemonGetTokusei(u8* pokemon)
{
    u16 species;

    if (pokemon == NULL) {
        return 0;
    }
    species = (u16)pokemonGetStatus(pokemon, 0, 0x6E, 0);
    if (pokemonGetStatus(NULL, species, 0x17, 1) == 0) {
        return (u16)pokemonGetStatus(NULL, species, 0x17, 0);
    }
    return (u16)pokemonGetStatus(NULL, species, 0x17, (u8)pokemonGetStatus(pokemon, 0, 0xB7, 0));
}

static inline void pokemonClearJoutai(u8* pokemon, u16 id)
{
    if (joutaiDataBiosGetStatusKind(id) == 0x7C || joutaiDataBiosGetStatusKind(id) == 0xC8) {
        fn_8011B788(pokemon, id);
    }
}

void pokemonAllKaihuku(u8* pokemon)
{
    u8 i;

    if (pokemon == NULL) {
        return;
    }
    pokemonSetStatus(pokemon, 0, 0x83, 0, (u16)pokemonGetStatus(pokemon, 0, 0x87, 0));
    for (i = 0; i < 4; i++) {
        if (pokemonWazaSlotIsValid(pokemon, i) == 1) {
            pokemonSetStatus(pokemon, 0, 0x80, i, pokemonGetWazaMaxPP(pokemon, i));
        }
    }
    pokemonClearJoutai(pokemon, 3);
    pokemonClearJoutai(pokemon, 4);
    pokemonClearJoutai(pokemon, 5);
    pokemonClearJoutai(pokemon, 6);
    pokemonClearJoutai(pokemon, 7);
    pokemonClearJoutai(pokemon, 8);
}

typedef struct MonohiroiTable {
    u16 entry[20];
} MonohiroiTable;

s32 pokemonCheckSetMonohiroi(u8* pokemon)
{
    u16 table[20];
    u16 item;
    u16 i;
    u32 roll;

    /* RULE-EXCEPTION(title-path): extern named stand-in for this TU's
     * .rodata table -- see docs/RULE_EXCEPTIONS.md. The original is a local
     * initializer ({0x16, 30}, {0x17, 40} ... {0xBB, 1}: item, cumulative
     * percent); as compiler-owned data it would start this object's .rodata,
     * which MWCC aligns to 8, at 0x8027296C (4-aligned), after the
     * pokemonGetEffortFromPokemon table 0x80272948 of the same TU. */
    *(MonohiroiTable*)table = *(const MonohiroiTable*)lbl_8027296C;
    if (pokemonGetTokusei(pokemon) == 0x35 && (u16)pokemonGetStatus(pokemon, 0, 0x82, 0) == 0 &&
        GSrand() % 10 == 0) {
        roll = GSrand() % 100;
        item = 0;
        for (i = 0; (s32)i < 20; i += 2) {
            if (table[i + 1] > roll) {
                item = table[i];
                break;
            }
        }
        if (item != 0) {
            if (pokemon != NULL) {
                pokemonSetStatus(pokemon, 0, 0x82, 0, item);
            }
            return 1;
        }
    }
    return 0;
}

void pokemonToMenuWazaStatus(u8* ptr, u8* out)
{
    u8 i;
    u8* slot;

    memset(out, 0, 0x48);
    *(u32*)out = pokemonGetStatus(ptr, 0, 0x77, 0);
    for (i = 0; i < 4; i++) {
        slot = out + i * 0xC + 4;
        if (pokemonWazaSlotIsValid(ptr, i) == 0) {
            *(u32*)(slot + 0x0) = 0;
            *(u32*)(slot + 0x4) = 0;
            *(u8*)(slot + 0xA) = 0;
            *(u8*)(slot + 0xB) = 0;
        } else {
            u16 id;
            u16 resolved;

            id = (u16)pokemonGetStatus(ptr, 0, 0x7F, i);
            resolved = (u16)wazaGetStatus(0, id, 3, 0);
            *(u32*)(slot + 0x0) = GSmsgGetGSchar(wazaGetStatus(0, id, 1, 0));
            *(u32*)(slot + 0x4) = GSmsgGetGSchar(fn_8010C4D4(resolved));
            *(u16*)(slot + 0x8) = (u16)fn_8010C46C(resolved);
            *(u8*)(slot + 0xA) = pokemonGetWazaMaxPP(ptr, i);
            *(u8*)(slot + 0xB) = (u8)pokemonGetStatus(ptr, 0, 0x80, i);
        }
    }
}

void pokemonToMenuPokemonStatusSubBar(u8* pokemon, MenuPokemonStatus* menu)
{
    u8 level;
    u32 baseExp;

    level = (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
    baseExp = pokemonGetLevelToExp(pokemon, level);
    if ((u8)pokemonGetStatus(pokemon, 0, 0xC2, 0) == 0) {
        menu->progressMax.integer = pokemonGetLevelToExp(pokemon, level + 1) - baseExp;
        menu->progress.integer = pokemonGetStatus(pokemon, 0, 0x79, 0) - baseExp;
    } else {
        menu->progressMax.real = darkPokemonSaveGet4A6((u16)pokemonGetStatus(pokemon, 0, 0xC3, 0));
        menu->progress.real = pokemonGetDp(pokemon);
    }
}

void pokemonToMenuPokemonStatus(u8* pokemon, MenuPokemonStatus* menu)
{
    pokemonGetStatus(pokemon, 0, 0x6E, 0);
    GScharCpy(menu->name, pokemonGetStatus(pokemon, 0, 0x77, 0));
    menu->level = (u8)pokemonGetStatus(pokemon, 0, 0x7A, 0);
    menu->maxHp = (u16)pokemonGetStatus(pokemon, 0, 0x87, 0);
    menu->hp = (u16)pokemonGetStatus(pokemon, 0, 0x83, 0);

    pokemonToMenuPokemonStatusSubBar(pokemon, menu);

    menu->conditionIcon = pokemonGetJoutaiIcon(pokemon);
    menu->displayValue = pokemonGetFaceSpriteId(pokemon);
    menu->sex = menuSubGetPokemonSexForFightDisp(pokemon);

    if ((u8)pokemonGetStatus(pokemon, 0, 0xC2, 0) == 0) {
        menu->darkState = 0;
    } else if (pokemonGetJoutai(pokemon, 0x3E) == 0) {
        menu->darkState = 1;
    } else {
        menu->darkState = 2;
    }
    menu->status82 = (u16)pokemonGetStatus(pokemon, 0, 0x82, 0);
}

void pokemonSetDarkPokemonStatus(u8* pokemon, u16 dark_id)
{
    if (pokemon == NULL) {
        return;
    }
    pokemonSetStatus(pokemon, 0, 0xC3, 0, 0);
    pokemonSetDp(pokemon, -1.0f);
    pokemonSetStatus(pokemon, 0, 0xC6, 0, 0);
    pokemonSetStatus(pokemon, 0, 0xC7, 0, 0);
    fn_8011B950((u8*)pokemonGetStatus(pokemon, 0, 0xC8, 0), 1);
    pokemonSetStatus(pokemon, 0, 0xC3, 0, dark_id);
    pokemonSetDp(pokemon, darkPokemonSaveGet4A6(dark_id));
}

u8 pokemonIsDarkPokemon(u8* pokemon)
{
    return (u8)pokemonGetStatus(pokemon, 0, 0xc2, 0);
}

f32 pokemonGetDp(u8* ptr)
{
    s32 value;

    if (ptr == NULL) {
        return 0.0f;
    }
    value = pokemonGetStatus(ptr, 0, 0xc5, 0);
    return (f32)value / 100.0f;
}

void pokemonSetDp(u8* ptr, f32 dp)
{
    if (ptr == NULL) {
        return;
    }
    pokemonSetStatus(ptr, 0, 0xc5, 0, (u32)(s32)(100.0f * dp));
}

void pokemonAddDpFormPokemonDpFilterId(u8* ptr, u16 item_data_id, u16 filter_id)
{
    u8* seikaku_data;
    u8* rate_data;
    u8* item_data;
    s32 rate_data_id;
    u8 kake;
    u8 waru;
    f32 dp;
    f32 new_dp;

    if ((u8)pokemonGetStatus(ptr, 0, 0xc2, 0) == 0) {
        return;
    }
    seikaku_data = pokemonSeikakuDataBiosGetPtr((u8)pokemonGetStatus(ptr, 0, 0xbf, 0));
    if (seikaku_data == NULL) {
        return;
    }
    dp = (f32)(s8)pokemonDpFilterDataBiosGetValue(pokemonDpFilterDataBiosGetPtr(filter_id));
    if (filter_id == 4) {
        item_data = itemDataBiosGetPtr(item_data_id);
        if (item_data == NULL) {
            return;
        }
        if (itemDataBiosGetKind(item_data) != 6) {
            return;
        }
        dp *= (f32)itemDataBiosGetBuff(item_data);
    }
    if (filter_id == 5) {
        dp = -1.0f * pokemonGetDp(ptr);
    }
    if (filter_id == 0) {
        rate_data_id = pokemonSeikakuDataBiosGetReliveFightout(seikaku_data);
    } else if (filter_id == 1) {
        rate_data_id = pokemonSeikakuDataBiosGetReliveWalk(seikaku_data);
    } else if (filter_id == 2) {
        rate_data_id = pokemonSeikakuDataBiosGetReliveCall(seikaku_data);
    } else if (filter_id == 3) {
        rate_data_id = pokemonSeikakuDataBiosGetReliveSodateya(seikaku_data);
    } else if (filter_id == 4) {
        rate_data_id = pokemonSeikakuDataBiosGetReliveNadenade(seikaku_data);
    }
    rate_data = pokemonSeikakuRateDataBiosGetPtr((u8)rate_data_id);
    if (rate_data == NULL) {
        return;
    }
    kake = pokemonSeikakuRateDataBiosGetKake(rate_data);
    waru = pokemonSeikakuRateDataBiosGetWaru(rate_data);
    if (waru != 0) {
        dp *= (f32)kake;
        dp /= (f32)waru;
    } else {
        return;
    }
    if (ptr != NULL) {
        new_dp = pokemonGetDp(ptr);
        new_dp += dp;
        if (new_dp < 0.0f) {
            new_dp = 0.0f;
        }
        pokemonSetDp(ptr, new_dp);
    }
}

s32 pokemonGetDarkPokemonLevel(u8* pokemon)
{
    u16 divisor;
    f32 level;

    if (pokemon == NULL) {
        return 7;
    }
    if ((u8)pokemonGetStatus(pokemon, 0, 0xC2, 0) == 1) {
        divisor = (u16)pokemonGetStatus(pokemon, 0, 0xC4, 0);
        if (divisor == 0) {
            divisor = 1;
        }
        level = pokemonGetDp(pokemon);
        if (level < 0.0f) {
            level = 0.0f;
        } else {
            level = (100.0f * level) / (f32)divisor;
        }
        if (level >= 100.0f) {
            return 0;
        }
        if (level >= 80.0f) {
            return 1;
        }
        if (level >= 60.0f) {
            return 2;
        }
        if (level >= 40.0f) {
            return 3;
        }
        if (level >= 20.0f) {
            return 4;
        }
        if (level > 0.0f) {
            return 5;
        }
        return 6;
    }
    return 7;
}
