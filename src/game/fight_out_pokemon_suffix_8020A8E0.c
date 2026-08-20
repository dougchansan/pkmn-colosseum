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
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);
extern u32   statusGetStatus();

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */
/* Address: 0x8020A8E0 | Size: 0x424 | Ghidra import */

int fn_8020A8E0(u32 conditionId, u32 target)
{
    extern int _fadeEffectGetRandom__FUl();
    extern u32 fightTargetDataBiosGetStatusKid();
    extern void fightTargetDataBiosGetPtr();
    extern int fightTargetGetPtr();
    extern u32 fightFloorGetStatus();
    extern u32 fn_8020A500();
    extern u32 fn_8020A540();
    extern u32 fn_8020A580();
    extern s16 fn_8020A5C0();
    extern s16 fn_8020A630();
    extern u8 fn_8020A6A0();
    extern u16 fn_8020A710();
    extern u16 fn_8020A780();
    extern u8 fn_8020A7F0();
    extern u16 fn_8020A860();
    extern u8 fn_8020A8A0();
    u32 values[2];
    u32 row;
    u32 value;
    u32 targetId;
    u32 statusId;
    u16 next;
    s16 numerator;
    s16 denominator;
    u16 parameter;
    u8 valueMode;
    u8 scaleMode;
    u8 slot;
    u8 compareMode;
    u8 combineMode;
    int result;
    int rowResult;
    void* targetPtr;

    for (slot = 0; slot < 2; slot++) {
        value = 0;
        valueMode = fn_8020A7F0(conditionId, slot);
        targetId = fn_8020A780(conditionId, slot);
        parameter = fn_8020A710(conditionId, slot);
        numerator = fn_8020A630(conditionId, slot);
        denominator = fn_8020A5C0(conditionId, slot);
        scaleMode = fn_8020A6A0(conditionId, slot);
        switch (valueMode) {
        case 1:
            value = (u16)targetId;
            break;
        case 2:
            value = (u16)targetId +
                _fadeEffectGetRandom__FUl((u16)parameter - (u16)targetId);
            break;
        case 3:
            targetPtr = (void*)fightTargetGetPtr(
                targetId, target, (u16)fightFloorGetStatus(0, 0, 0x14, 0));
            if (targetPtr != NULL) {
                fightTargetDataBiosGetPtr(targetId);
                statusId = fightTargetDataBiosGetStatusKid();
                if (scaleMode == 0) {
                    value = statusGetStatus(statusId, targetPtr,
                                            (u16)numerator, parameter,
                                            (u16)denominator);
                } else {
                    value = statusGetStatus(statusId, targetPtr, 0,
                                            parameter, 0);
                }
            }
            break;
        }
        if (scaleMode == 1) {
            value *= numerator;
            if (denominator != 0) {
                value = (s32)value / denominator;
            }
        }
        values[slot] = value;
    }

    compareMode = fn_8020A8A0(conditionId);
    switch (compareMode) {
    case 0: result = 1; break;
    case 1: result = values[0] == values[1]; break;
    case 2: result = values[0] != values[1]; break;
    case 3: result = (s32)values[0] >= (s32)values[1]; break;
    case 4: result = (s32)values[0] <= (s32)values[1]; break;
    case 5: result = (s32)values[0] < (s32)values[1]; break;
    case 6: result = (s32)values[0] > (s32)values[1]; break;
    default: result = 0; break;
    }

    next = fn_8020A860(conditionId);
    if (next == 0) {
        return result;
    }
    do {
        row = fn_8020A540(next);
        for (slot = 0; slot < 2; slot++) {
            value = 0;
            valueMode = fn_8020A7F0(row, slot);
            targetId = fn_8020A780(row, slot);
            parameter = fn_8020A710(row, slot);
            numerator = fn_8020A630(row, slot);
            denominator = fn_8020A5C0(row, slot);
            scaleMode = fn_8020A6A0(row, slot);
            switch (valueMode) {
            case 1:
                value = (u16)targetId;
                break;
            case 2:
                value = (u16)targetId +
                    _fadeEffectGetRandom__FUl(
                        (u16)parameter - (u16)targetId);
                break;
            case 3:
                targetPtr = (void*)fightTargetGetPtr(
                    targetId, target,
                    (u16)fightFloorGetStatus(0, 0, 0x14, 0));
                if (targetPtr != NULL) {
                    fightTargetDataBiosGetPtr(targetId);
                    statusId = fightTargetDataBiosGetStatusKid();
                    if (scaleMode == 0) {
                        value = statusGetStatus(statusId, targetPtr,
                                                (u16)numerator, parameter,
                                                (u16)denominator);
                    } else {
                        value = statusGetStatus(statusId, targetPtr, 0,
                                                parameter, 0);
                    }
                }
                break;
            }
            if (scaleMode == 1) {
                value *= numerator;
                if (denominator != 0) {
                    value = (s32)value / denominator;
                }
            }
            values[slot] = value;
        }

        compareMode = fn_8020A8A0(row);
        switch (compareMode) {
        case 0: rowResult = 1; break;
        case 1: rowResult = values[0] == values[1]; break;
        case 2: rowResult = values[0] != values[1]; break;
        case 3: rowResult = (s32)values[0] >= (s32)values[1]; break;
        case 4: rowResult = (s32)values[0] <= (s32)values[1]; break;
        case 5: rowResult = (s32)values[0] < (s32)values[1]; break;
        case 6: rowResult = (s32)values[0] > (s32)values[1]; break;
        default: rowResult = 0; break;
        }

        combineMode = fn_8020A580(row);
        if (combineMode == 1) {
            result = result || rowResult;
        } else if (combineMode == 2) {
            result = result && rowResult;
        }
        next = fn_8020A500(next);
    } while (next != 0);

    return result;
}
