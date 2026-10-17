/**
 * @file fight_out_pokemon_suffix_8020A8E0.c
 * @brief fightOutPokemon + fightPokemon final suffix, address range
 *        0x8020A8E0-0x8020AE30, 1 function.
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
/* Address: 0x8020A8E0 | Size: 0x550
 *
 * Evaluates a jouken (condition) row and its linked rows. Each row's test is
 * the same inline body (retail keeps one value array per inlined copy). */

extern int _fadeEffectGetRandom__FUl();
extern u32 fightTargetDataBiosGetStatusKid();
extern void fightTargetDataBiosGetPtr(u16 targetId);
extern void* fightTargetGetPtr(u16 targetId, void* target, u16 floorStatus);
extern u32 statusGetStatus(u32 statusId, void* ptr, u16 numerator,
                           u16 parameter, u16 denominator);
extern u32 fightFloorGetStatus();
extern u16 joukenDataGet4(u16 idx);
extern u32 joukenDataGet2(u16 idx);
extern u8 joukenDataGet0();
extern s16 fn_8020A5C0();
extern s16 fn_8020A630();
extern u8 fn_8020A6A0();
extern u16 fn_8020A710();
extern u16 fn_8020A780();
extern u8 fn_8020A7F0();
extern u16 fn_8020A860();
extern u8 fightJoukenDataBiosGetType();

/* RULE-EXCEPTION(user-approved): single-use inline helper - the per-slot value read is its own inline with an int slot parameter, which gives the two inlined fightJoukenCheck copies retail's conversion-web order - see docs/RULE_EXCEPTIONS.md */
static inline s32 fightJoukenGetValue(u32 id, int slot, void* target)
{
    s32 value;
    u16 targetId;
    u8 scaleMode;
    u8 valueMode;
    u32 statusId;
    u16 parameter;
    s16 denominator;
    s16 numerator;
    void* targetPtr;

    value = 0;
    valueMode = fn_8020A7F0(id, slot);
    targetId = fn_8020A780(id, slot);
    parameter = fn_8020A710(id, slot);
    numerator = fn_8020A630(id, slot);
    denominator = fn_8020A5C0(id, slot);
    scaleMode = fn_8020A6A0(id, slot);
    switch (valueMode) {
    case 0:
        break;
    case 1:
        value = targetId;
        break;
    case 2:
        value = targetId + _fadeEffectGetRandom__FUl(parameter - targetId);
        break;
    case 3:
        targetPtr = fightTargetGetPtr(
            targetId, target, (u16)fightFloorGetStatus(0, 0, 0x14, 0));
        if (targetPtr == NULL) {
            return 0;
        }
        fightTargetDataBiosGetPtr(targetId);
        statusId = fightTargetDataBiosGetStatusKid();
        if (scaleMode == 0) {
            value = statusGetStatus(statusId, targetPtr, (u16)numerator,
                                    parameter, (u16)denominator);
        } else {
            value = statusGetStatus(statusId, targetPtr, 0, parameter, 0);
        }
        break;
    }
    if (scaleMode == 1) {
        value *= numerator;
        if (denominator != 0) {
            value /= denominator;
        }
    }
    return value;
}

static inline u8 fightJoukenCheck(u32 id, void* target)
{
    s32 values[2];
    u8 result;
    u8 slot;

    for (slot = 0; slot < 2; slot++) {
        values[slot] = fightJoukenGetValue(id, slot, target);
    }

    result = 0;
    switch (fightJoukenDataBiosGetType(id)) {
    case 0:
        result = 1;
        break;
    case 1:
        if (values[0] == values[1]) {
            result = 1;
        }
        break;
    case 2:
        if (values[0] != values[1]) {
            result = 1;
        }
        break;
    case 3:
        if (values[0] >= values[1]) {
            result = 1;
        }
        break;
    case 4:
        if (values[0] <= values[1]) {
            result = 1;
        }
        break;
    case 5:
        if (values[0] < values[1]) {
            result = 1;
        }
        break;
    case 6:
        if (values[0] > values[1]) {
            result = 1;
        }
        break;
    }
    return result;
}

u8 fn_8020A8E0(u32 conditionId, void* target)
{
    u8 result;
    u8 rowResult;
    u32 row;
    u16 next;

    result = fightJoukenCheck(conditionId, target);
    next = fn_8020A860(conditionId);
    if (next == 0) {
        return result;
    }
    while (1) {
        row = joukenDataGet2(next);
        rowResult = fightJoukenCheck(row, target);
        switch (joukenDataGet0(row)) {
        case 1:
            if (result || rowResult) {
                result = 1;
            } else {
                result = 0;
            }
            break;
        case 2:
            if (result && rowResult) {
                result = 1;
            } else {
                result = 0;
            }
            break;
        }
        next = joukenDataGet4(next);
        if (next == 0) {
            return result;
        }
    }
}
