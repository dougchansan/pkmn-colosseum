/**
 * @file battle_logic.c
 * @brief Battle engine logic -- moves, types, status, damage, AI, shadow (merged TU).
 *
 * This is a single translation unit containing all interleaved battle logic
 * functions. Link order analysis showed that battle_move.c, battle_type.c,
 * battle_status.c, battle_damage.c, battle_ai.c, and battle_shadow.c had
 * interleaved functions throughout the 0x801E03D4-0x801EF02C address range,
 * confirming they were originally compiled as one TU.
 *
 * battle_main.c remains separate (address range 0x801EF02C-0x801F000C).
 *
 * Subsystems:
 *   - Move execution:    accuracy, effects, turn order, priority
 *   - Type effectiveness: Gen III type chart + Shadow type
 *   - Status effects:     primary/volatile status, counters, immunities
 *   - Damage calculation: Gen III formula, STAB, crits, stat stages
 *   - AI:                 scoring-based move/target selection
 *   - Shadow Pokemon:     Hyper Mode, Shadow Rush, snagging, Call action
 *
 * Address range: 0x801E03D4 - 0x801EF02C
 */

#include "game/battle/battle.h"

/* Use inline lmw/stmw for prologue/epilogue (all functions in this TU use stmw, not savegpr stubs) */
#pragma use_lmw_stmw on

/* =========================================================================
 * External function declarations
 * ========================================================================= */

/* Random number generation */
extern s32  fn_800D37CC(void);   /* GSrandom_Get */
extern void fn_800D3074(s32 seed); /* GSrandom_Seed */

/* Debug logging */
extern void fn_800DD970(const char* fmt, ...);  /* GSlog_Print */

/* Battle message display */
extern void fn_80106698(s32 msgID, s32 arg1, s32 arg2, s32 arg3);

/* Pokemon data access (People/NPC system) */
extern s32  fn_80129280(s32 side, s32 slotType);    /* get battle party */
extern s32  fn_8012AC08(s32 party, u16 slotIdx);    /* get pokemon from party */
extern s32  fn_80129D64(s32 pokemon, s32 move);      /* check move validity */
extern s32  fn_8011EE40(s32 pokemon);                /* get pokemon HP */
extern s32  fn_8011F4F0(s32 pokemon);                /* get pokemon species */

/* Waza (move animation) system */
extern void fn_801DAEF8(s32 count);      /* load waza data */
extern void fn_801D7464(void);           /* wazaSequenceLoad */
extern void fn_801D7B94(void);           /* wazaSequenceUpdate */
extern void wazaSequenceEntryStart(void);           /* wazaSequenceEntryStart */

/* Battle state checks */
extern s32  fn_8001E184(void);           /* wait for event completion */
extern s32  fn_8001BDF4(s32 a, s32 b, s32 c); /* check condition */

/* Move data lookup */
extern void fn_80132A38(s32 msgType, s32 species);  /* display move name */

/* Shadow-related checks from the state machine */
extern u8   fn_801EEC74(void);         /* check shadow pokemon state */
extern void fn_801EECD8(s32 slot, s32 arg); /* set slot shadow state */
extern void fn_801EEB34(s32 slot, s32 arg); /* reset slot shadow anim */
extern void fn_801EE958(s32 slot, s32 arg); /* reset slot shadow effect */

/* =========================================================================
 * Type Effectiveness Table (from battle_type.c)
 * ========================================================================= */

/**
 * Gen III type effectiveness chart.
 * Each entry: { attacking_type, defending_type, effectiveness }
 * Only non-neutral matchups are listed (neutral is the default).
 * Terminated by 0xFF in the attacking_type field.
 */
static const TypeMatchup sTypeChart[] = {
    /* Normal attacking */
    { TYPE_NORMAL,   TYPE_ROCK,     TYPE_EFF_NOT_VERY },
    { TYPE_NORMAL,   TYPE_STEEL,    TYPE_EFF_NOT_VERY },
    { TYPE_NORMAL,   TYPE_GHOST,    TYPE_EFF_IMMUNE   },

    /* Fighting attacking */
    { TYPE_FIGHTING, TYPE_NORMAL,   TYPE_EFF_SUPER    },
    { TYPE_FIGHTING, TYPE_ICE,      TYPE_EFF_SUPER    },
    { TYPE_FIGHTING, TYPE_ROCK,     TYPE_EFF_SUPER    },
    { TYPE_FIGHTING, TYPE_DARK,     TYPE_EFF_SUPER    },
    { TYPE_FIGHTING, TYPE_STEEL,    TYPE_EFF_SUPER    },
    { TYPE_FIGHTING, TYPE_FLYING,   TYPE_EFF_NOT_VERY },
    { TYPE_FIGHTING, TYPE_POISON,   TYPE_EFF_NOT_VERY },
    { TYPE_FIGHTING, TYPE_BUG,      TYPE_EFF_NOT_VERY },
    { TYPE_FIGHTING, TYPE_PSYCHIC,  TYPE_EFF_NOT_VERY },
    { TYPE_FIGHTING, TYPE_GHOST,    TYPE_EFF_IMMUNE   },

    /* Flying attacking */
    { TYPE_FLYING,   TYPE_GRASS,    TYPE_EFF_SUPER    },
    { TYPE_FLYING,   TYPE_FIGHTING, TYPE_EFF_SUPER    },
    { TYPE_FLYING,   TYPE_BUG,      TYPE_EFF_SUPER    },
    { TYPE_FLYING,   TYPE_ELECTRIC, TYPE_EFF_NOT_VERY },
    { TYPE_FLYING,   TYPE_ROCK,     TYPE_EFF_NOT_VERY },
    { TYPE_FLYING,   TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Poison attacking */
    { TYPE_POISON,   TYPE_GRASS,    TYPE_EFF_SUPER    },
    { TYPE_POISON,   TYPE_POISON,   TYPE_EFF_NOT_VERY },
    { TYPE_POISON,   TYPE_GROUND,   TYPE_EFF_NOT_VERY },
    { TYPE_POISON,   TYPE_ROCK,     TYPE_EFF_NOT_VERY },
    { TYPE_POISON,   TYPE_GHOST,    TYPE_EFF_NOT_VERY },
    { TYPE_POISON,   TYPE_STEEL,    TYPE_EFF_IMMUNE   },

    /* Ground attacking */
    { TYPE_GROUND,   TYPE_FIRE,     TYPE_EFF_SUPER    },
    { TYPE_GROUND,   TYPE_ELECTRIC, TYPE_EFF_SUPER    },
    { TYPE_GROUND,   TYPE_POISON,   TYPE_EFF_SUPER    },
    { TYPE_GROUND,   TYPE_ROCK,     TYPE_EFF_SUPER    },
    { TYPE_GROUND,   TYPE_STEEL,    TYPE_EFF_SUPER    },
    { TYPE_GROUND,   TYPE_GRASS,    TYPE_EFF_NOT_VERY },
    { TYPE_GROUND,   TYPE_BUG,      TYPE_EFF_NOT_VERY },
    { TYPE_GROUND,   TYPE_FLYING,   TYPE_EFF_IMMUNE   },

    /* Rock attacking */
    { TYPE_ROCK,     TYPE_FIRE,     TYPE_EFF_SUPER    },
    { TYPE_ROCK,     TYPE_ICE,      TYPE_EFF_SUPER    },
    { TYPE_ROCK,     TYPE_FLYING,   TYPE_EFF_SUPER    },
    { TYPE_ROCK,     TYPE_BUG,      TYPE_EFF_SUPER    },
    { TYPE_ROCK,     TYPE_FIGHTING, TYPE_EFF_NOT_VERY },
    { TYPE_ROCK,     TYPE_GROUND,   TYPE_EFF_NOT_VERY },
    { TYPE_ROCK,     TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Bug attacking */
    { TYPE_BUG,      TYPE_GRASS,    TYPE_EFF_SUPER    },
    { TYPE_BUG,      TYPE_PSYCHIC,  TYPE_EFF_SUPER    },
    { TYPE_BUG,      TYPE_DARK,     TYPE_EFF_SUPER    },
    { TYPE_BUG,      TYPE_FIRE,     TYPE_EFF_NOT_VERY },
    { TYPE_BUG,      TYPE_FIGHTING, TYPE_EFF_NOT_VERY },
    { TYPE_BUG,      TYPE_FLYING,   TYPE_EFF_NOT_VERY },
    { TYPE_BUG,      TYPE_POISON,   TYPE_EFF_NOT_VERY },
    { TYPE_BUG,      TYPE_GHOST,    TYPE_EFF_NOT_VERY },
    { TYPE_BUG,      TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Ghost attacking */
    { TYPE_GHOST,    TYPE_PSYCHIC,  TYPE_EFF_SUPER    },
    { TYPE_GHOST,    TYPE_GHOST,    TYPE_EFF_SUPER    },
    { TYPE_GHOST,    TYPE_DARK,     TYPE_EFF_NOT_VERY },
    { TYPE_GHOST,    TYPE_STEEL,    TYPE_EFF_NOT_VERY },
    { TYPE_GHOST,    TYPE_NORMAL,   TYPE_EFF_IMMUNE   },

    /* Steel attacking */
    { TYPE_STEEL,    TYPE_ICE,      TYPE_EFF_SUPER    },
    { TYPE_STEEL,    TYPE_ROCK,     TYPE_EFF_SUPER    },
    { TYPE_STEEL,    TYPE_FIRE,     TYPE_EFF_NOT_VERY },
    { TYPE_STEEL,    TYPE_WATER,    TYPE_EFF_NOT_VERY },
    { TYPE_STEEL,    TYPE_ELECTRIC, TYPE_EFF_NOT_VERY },
    { TYPE_STEEL,    TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Fire attacking */
    { TYPE_FIRE,     TYPE_GRASS,    TYPE_EFF_SUPER    },
    { TYPE_FIRE,     TYPE_ICE,      TYPE_EFF_SUPER    },
    { TYPE_FIRE,     TYPE_BUG,      TYPE_EFF_SUPER    },
    { TYPE_FIRE,     TYPE_STEEL,    TYPE_EFF_SUPER    },
    { TYPE_FIRE,     TYPE_FIRE,     TYPE_EFF_NOT_VERY },
    { TYPE_FIRE,     TYPE_WATER,    TYPE_EFF_NOT_VERY },
    { TYPE_FIRE,     TYPE_ROCK,     TYPE_EFF_NOT_VERY },
    { TYPE_FIRE,     TYPE_DRAGON,   TYPE_EFF_NOT_VERY },

    /* Water attacking */
    { TYPE_WATER,    TYPE_FIRE,     TYPE_EFF_SUPER    },
    { TYPE_WATER,    TYPE_GROUND,   TYPE_EFF_SUPER    },
    { TYPE_WATER,    TYPE_ROCK,     TYPE_EFF_SUPER    },
    { TYPE_WATER,    TYPE_WATER,    TYPE_EFF_NOT_VERY },
    { TYPE_WATER,    TYPE_GRASS,    TYPE_EFF_NOT_VERY },
    { TYPE_WATER,    TYPE_DRAGON,   TYPE_EFF_NOT_VERY },

    /* Grass attacking */
    { TYPE_GRASS,    TYPE_WATER,    TYPE_EFF_SUPER    },
    { TYPE_GRASS,    TYPE_GROUND,   TYPE_EFF_SUPER    },
    { TYPE_GRASS,    TYPE_ROCK,     TYPE_EFF_SUPER    },
    { TYPE_GRASS,    TYPE_FIRE,     TYPE_EFF_NOT_VERY },
    { TYPE_GRASS,    TYPE_GRASS,    TYPE_EFF_NOT_VERY },
    { TYPE_GRASS,    TYPE_POISON,   TYPE_EFF_NOT_VERY },
    { TYPE_GRASS,    TYPE_FLYING,   TYPE_EFF_NOT_VERY },
    { TYPE_GRASS,    TYPE_BUG,      TYPE_EFF_NOT_VERY },
    { TYPE_GRASS,    TYPE_DRAGON,   TYPE_EFF_NOT_VERY },
    { TYPE_GRASS,    TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Electric attacking */
    { TYPE_ELECTRIC, TYPE_WATER,    TYPE_EFF_SUPER    },
    { TYPE_ELECTRIC, TYPE_FLYING,   TYPE_EFF_SUPER    },
    { TYPE_ELECTRIC, TYPE_ELECTRIC, TYPE_EFF_NOT_VERY },
    { TYPE_ELECTRIC, TYPE_GRASS,    TYPE_EFF_NOT_VERY },
    { TYPE_ELECTRIC, TYPE_DRAGON,   TYPE_EFF_NOT_VERY },
    { TYPE_ELECTRIC, TYPE_GROUND,   TYPE_EFF_IMMUNE   },

    /* Psychic attacking */
    { TYPE_PSYCHIC,  TYPE_FIGHTING, TYPE_EFF_SUPER    },
    { TYPE_PSYCHIC,  TYPE_POISON,   TYPE_EFF_SUPER    },
    { TYPE_PSYCHIC,  TYPE_PSYCHIC,  TYPE_EFF_NOT_VERY },
    { TYPE_PSYCHIC,  TYPE_STEEL,    TYPE_EFF_NOT_VERY },
    { TYPE_PSYCHIC,  TYPE_DARK,     TYPE_EFF_IMMUNE   },

    /* Ice attacking */
    { TYPE_ICE,      TYPE_GRASS,    TYPE_EFF_SUPER    },
    { TYPE_ICE,      TYPE_GROUND,   TYPE_EFF_SUPER    },
    { TYPE_ICE,      TYPE_FLYING,   TYPE_EFF_SUPER    },
    { TYPE_ICE,      TYPE_DRAGON,   TYPE_EFF_SUPER    },
    { TYPE_ICE,      TYPE_FIRE,     TYPE_EFF_NOT_VERY },
    { TYPE_ICE,      TYPE_WATER,    TYPE_EFF_NOT_VERY },
    { TYPE_ICE,      TYPE_ICE,      TYPE_EFF_NOT_VERY },
    { TYPE_ICE,      TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Dragon attacking */
    { TYPE_DRAGON,   TYPE_DRAGON,   TYPE_EFF_SUPER    },
    { TYPE_DRAGON,   TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Dark attacking */
    { TYPE_DARK,     TYPE_PSYCHIC,  TYPE_EFF_SUPER    },
    { TYPE_DARK,     TYPE_GHOST,    TYPE_EFF_SUPER    },
    { TYPE_DARK,     TYPE_FIGHTING, TYPE_EFF_NOT_VERY },
    { TYPE_DARK,     TYPE_DARK,     TYPE_EFF_NOT_VERY },
    { TYPE_DARK,     TYPE_STEEL,    TYPE_EFF_NOT_VERY },

    /* Shadow attacking (Colosseum-exclusive) */
    { TYPE_SHADOW,   TYPE_SHADOW,   TYPE_EFF_NOT_VERY },

    /* Sentinel */
    { 0xFF, 0xFF, 0xFF }
};

/* =========================================================================
 * Damage Calculation Constants (from battle_damage.c)
 * ========================================================================= */

/**
 * Stat stage multiplier table.
 * Index 0 = stage -6, index 6 = stage 0, index 12 = stage +6.
 * Stored as numerator/denominator pairs.
 */
static const s32 sStatStageNumerator[13] = {
    2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8
};

static const s32 sStatStageDenominator[13] = {
    8, 7, 6, 5, 4, 3, 2, 2, 2, 2, 2, 2, 2
};

/**
 * Physical/special split by type (Gen III rules).
 * TRUE = physical, FALSE = special.
 */
static const u8 sTypeIsPhysical[TYPE_COUNT] = {
    /* Normal   */ 1,
    /* Fighting */ 1,
    /* Flying   */ 1,
    /* Poison   */ 1,
    /* Ground   */ 1,
    /* Rock     */ 1,
    /* Bug      */ 1,
    /* Ghost    */ 1,
    /* Steel    */ 1,
    /* Fire     */ 0,
    /* Water    */ 0,
    /* Grass    */ 0,
    /* Electric */ 0,
    /* Psychic  */ 0,
    /* Ice      */ 0,
    /* Dragon   */ 0,
    /* Dark     */ 0,
    /* Shadow   */ 1,  /* Shadow Rush is treated as physical */
};

/* =========================================================================
 * AI Constants (from battle_ai.c)
 * ========================================================================= */

#define AI_SCORE_BASE          100
#define AI_SCORE_TYPE_BONUS     40   /* Bonus for super-effective moves */
#define AI_SCORE_TYPE_PENALTY  -20   /* Penalty for not-very-effective moves */
#define AI_SCORE_KO_BONUS       60   /* Bonus if move would KO target */
#define AI_SCORE_STAB_BONUS     15   /* Bonus for STAB moves */
#define AI_SCORE_STATUS_BONUS   30   /* Bonus for status moves on healthy targets */
#define AI_SCORE_ALLY_HIT_PENALTY -80 /* Penalty for hitting ally with spread move */
#define AI_SCORE_RANDOM_RANGE   20   /* Random variance for less predictable AI */

/* =========================================================================
 * Shadow Pokemon Constants (from battle_shadow.c)
 * ========================================================================= */

#define SHADOW_RUSH_POWER     90
#define SHADOW_RUSH_RECOIL_DIV 16   /* 1/16 max HP recoil */
#define CALL_GAUGE_REDUCTION   100  /* Heart gauge reduction per Call */
#define HYPER_MODE_BASE_CHANCE 25   /* Base % chance per turn to enter Hyper Mode */

/* #######################################################################
 * MOVE EXECUTION (from battle_move.c)
 * ####################################################################### */

/**
 * Check if a move hits based on accuracy.
 *
 * Gen III accuracy formula:
 *   hitChance = moveAccuracy * accStage / evaStage
 */
BOOL battle_CheckAccuracy(BattlePokemon* attacker, BattlePokemon* defender,
                          const MoveData* move) {
    s32 accuracy;
    s32 accStage;
    s32 roll;

    /* Accuracy of 0 means the move always hits (e.g., Swift, Aerial Ace) */
    if (move->accuracy == 0) {
        return TRUE;
    }

    accuracy = move->accuracy;

    /* Apply accuracy/evasion stage modifiers */
    accStage = attacker->statStages.accuracy - defender->statStages.evasion;

    /* Clamp to [-6, +6] */
    if (accStage < -6) accStage = -6;
    if (accStage >  6) accStage =  6;

    /* Apply stage modifier using the accuracy/evasion table */
    if (accStage >= 0) {
        accuracy = accuracy * (3 + accStage) / 3;
    } else {
        accuracy = accuracy * 3 / (3 - accStage);
    }

    /* Roll for hit */
    roll = ((u32)fn_800D37CC() % 100) + 1;

    return (roll <= accuracy);
}

/**
 * Apply a move's secondary effect to the target.
 */
void battle_ApplyMoveEffect(BattlePokemon* target, const MoveData* move) {
    s32 roll;

    /* No effect to apply */
    if (move->effect == 0) {
        return;
    }

    /* Check effect chance (0 means always, otherwise percentage) */
    if (move->effectChance > 0) {
        roll = ((u32)fn_800D37CC() % 100) + 1;
        if (roll > move->effectChance) {
            return;
        }
    }

    switch (move->effect) {
        /* Stat lowering effects */
        case 1:
            if (target->statStages.attack > -6) target->statStages.attack--;
            break;
        case 2:
            if (target->statStages.defense > -6) target->statStages.defense--;
            break;
        case 3:
            if (target->statStages.speed > -6) target->statStages.speed--;
            break;
        case 4:
            if (target->statStages.spAttack > -6) target->statStages.spAttack--;
            break;
        case 5:
            if (target->statStages.spDefense > -6) target->statStages.spDefense--;
            break;
        case 6:
            if (target->statStages.accuracy > -6) target->statStages.accuracy--;
            break;

        /* Volatile status effects */
        case 30:
            target->volatileStatus |= VSTATUS_FLINCH;
            break;

        /* Primary status effects */
        case 31:
            battle_TryInflictStatus(target, STATUS_BURN, 100);
            break;
        case 32:
            battle_TryInflictStatus(target, STATUS_FREEZE, 100);
            break;
        case 33:
            battle_TryInflictStatus(target, STATUS_PARALYSIS, 100);
            break;
        case 34:
            battle_TryInflictStatus(target, STATUS_POISON, 100);
            break;
        case 35:
            target->volatileStatus |= VSTATUS_CONFUSION;
            break;

        default:
            break;
    }
}

/**
 * Execute a move from one battle slot against another.
 * Stub until full state machine is decompiled.
 */
s32 battle_ExecuteMove(s32 attackerSlot, s32 targetSlot, u16 moveID) {
    return 0;
}

/**
 * Determine turn order for all pending actions.
 */
void battle_DetermineTurnOrder(TurnAction actions[], s32 count) {
    s32 i, j;
    TurnAction temp;

    /* Simple insertion sort by priority (descending), then speed (descending) */
    for (i = 1; i < count; i++) {
        temp = actions[i];
        j = i - 1;

        while (j >= 0 && battle_ComparePriority(&actions[j], &temp) < 0) {
            actions[j + 1] = actions[j];
            j--;
        }
        actions[j + 1] = temp;
    }
}

/**
 * Compare two turn actions for priority ordering.
 */
s32 battle_ComparePriority(const TurnAction* a, const TurnAction* b) {
    /* Higher priority goes first */
    if (a->priority != b->priority) {
        return a->priority - b->priority;
    }

    /* Same priority: higher speed goes first */
    if (a->speedValue != b->speedValue) {
        return a->speedValue - b->speedValue;
    }

    /* Speed tie: random tiebreaker */
    return ((u32)fn_800D37CC() & 1) ? 1 : -1;
}

/* #######################################################################
 * TYPE EFFECTIVENESS (from battle_type.c)
 * ####################################################################### */

/**
 * Look up type effectiveness for a single type matchup.
 */
u8 battle_CalcTypeMatchup(u8 atkType, u8 defType) {
    const TypeMatchup* entry;

    /* Shadow type has special handling */
    if (atkType == TYPE_SHADOW) {
        if (defType == TYPE_SHADOW) {
            return TYPE_EFF_NOT_VERY;
        }
        return TYPE_EFF_SUPER;
    }

    /* Search the type chart for this matchup */
    for (entry = sTypeChart; entry->attackType != 0xFF; entry++) {
        if (entry->attackType == atkType && entry->defendType == defType) {
            return entry->effectiveness;
        }
    }

    /* Default: neutral effectiveness */
    return TYPE_EFF_NORMAL;
}

/**
 * Calculate combined type effectiveness against a dual-typed defender.
 */
u8 battle_GetTypeEffectiveness(u8 attackType, u8 defType1, u8 defType2) {
    u8 eff1, eff2;

    eff1 = battle_CalcTypeMatchup(attackType, defType1);

    /* If immune to one type, total is immune */
    if (eff1 == TYPE_EFF_IMMUNE) {
        return TYPE_EFF_IMMUNE;
    }

    /* Single-typed Pokemon (type2 == type1 or type2 is TYPE_NORMAL placeholder) */
    if (defType1 == defType2) {
        return eff1;
    }

    eff2 = battle_CalcTypeMatchup(attackType, defType2);

    if (eff2 == TYPE_EFF_IMMUNE) {
        return TYPE_EFF_IMMUNE;
    }

    /* Multiply and normalize */
    return (u8)((s32)eff1 * (s32)eff2 / TYPE_EFF_NORMAL);
}

/* #######################################################################
 * STATUS EFFECTS (from battle_status.c)
 * ####################################################################### */

/**
 * Apply end-of-turn status damage to a Pokemon.
 */
void battle_ApplyStatusDamage(BattlePokemon* pokemon) {
    s32 damage;

    if (pokemon->currentHP == 0) {
        return;  /* Fainted Pokemon don't take status damage */
    }

    /* Poison: 1/8 max HP */
    if (pokemon->statusCondition & STATUS_POISON) {
        damage = pokemon->maxHP / 8;
        if (damage < 1) damage = 1;

        if (pokemon->currentHP <= (u16)damage) {
            pokemon->currentHP = 0;
        } else {
            pokemon->currentHP -= (u16)damage;
        }
    }

    /* Burn: 1/8 max HP */
    if (pokemon->statusCondition & STATUS_BURN) {
        damage = pokemon->maxHP / 8;
        if (damage < 1) damage = 1;

        if (pokemon->currentHP <= (u16)damage) {
            pokemon->currentHP = 0;
        } else {
            pokemon->currentHP -= (u16)damage;
        }
    }

    /* Toxic: N/16 max HP, where N increments each turn */
    if (pokemon->statusCondition & STATUS_TOXIC) {
        s32 toxicCount = (pokemon->statusCondition >> 8) & 0xF;
        toxicCount++;
        if (toxicCount > 15) toxicCount = 15;

        damage = pokemon->maxHP * toxicCount / 16;
        if (damage < 1) damage = 1;

        if (pokemon->currentHP <= (u16)damage) {
            pokemon->currentHP = 0;
        } else {
            pokemon->currentHP -= (u16)damage;
        }

        /* Update toxic counter */
        pokemon->statusCondition = (pokemon->statusCondition & 0xFFFFF0FF) |
                                    ((u32)toxicCount << 8);
    }

    /* Clear flinch at end of turn (it only lasts one turn) */
    pokemon->volatileStatus &= ~VSTATUS_FLINCH;
}

/**
 * Check if a status condition prevents the Pokemon from moving.
 */
BOOL battle_CheckStatusPreventsMove(BattlePokemon* pokemon) {
    s32 roll;

    /* Check flinch (cleared after this check) */
    if (pokemon->volatileStatus & VSTATUS_FLINCH) {
        return TRUE;
    }

    /* Check freeze: 20% chance to thaw */
    if (pokemon->statusCondition & STATUS_FREEZE) {
        roll = (u32)fn_800D37CC() % 5;
        if (roll == 0) {
            pokemon->statusCondition &= ~STATUS_FREEZE;
            return FALSE;
        }
        return TRUE;
    }

    /* Check sleep: decrement counter */
    if (pokemon->statusCondition & STATUS_SLEEP) {
        s32 sleepCount = pokemon->statusCondition & 0x07;
        if (sleepCount > 0) {
            sleepCount--;
            pokemon->statusCondition = (pokemon->statusCondition & ~0x07) | sleepCount;
            if (sleepCount == 0) {
                return FALSE;
            }
            return TRUE;
        }
    }

    /* Check paralysis: 25% chance of full paralysis */
    if (pokemon->statusCondition & STATUS_PARALYSIS) {
        roll = (u32)fn_800D37CC() % 4;
        if (roll == 0) {
            return TRUE;
        }
    }

    /* Check confusion: 50% chance of hitting self */
    if (pokemon->volatileStatus & VSTATUS_CONFUSION) {
        roll = (u32)fn_800D37CC() % 2;
        if (roll == 0) {
            s32 selfDamage;
            selfDamage = ((2 * pokemon->level / 5 + 2) * 40 *
                           pokemon->attack / pokemon->defense) / 50 + 2;
            if (selfDamage < 1) selfDamage = 1;

            if (pokemon->currentHP <= (u16)selfDamage) {
                pokemon->currentHP = 0;
            } else {
                pokemon->currentHP -= (u16)selfDamage;
            }
            return TRUE;
        }
    }

    /* Check attract: 50% chance of being immobilized */
    if (pokemon->volatileStatus & VSTATUS_ATTRACT) {
        roll = (u32)fn_800D37CC() % 2;
        if (roll == 0) {
            return TRUE;
        }
    }

    /* Check Shadow Pokemon Hyper Mode */
    if (pokemon->isShadow && pokemon->shadowMode == SHADOW_HYPER_MODE) {
        roll = (u32)fn_800D37CC() % 4;  /* ~25% chance to disobey */
        if (roll == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Tick status effect counters at end of turn.
 */
void battle_TickStatusCounters(BattlePokemon* pokemon) {
    /* Confusion counter: lasts 1-4 turns */
    if (pokemon->volatileStatus & VSTATUS_CONFUSION) {
        /* The confusion counter could be stored in upper bits of volatileStatus. */
    }

    /* Flinch is always cleared at end of turn */
    pokemon->volatileStatus &= ~VSTATUS_FLINCH;
}

/**
 * Attempt to inflict a status condition on a target Pokemon.
 */
void battle_TryInflictStatus(BattlePokemon* target, u32 status, u8 chance) {
    s32 roll;

    /* Cannot inflict if Pokemon already has a primary status */
    if (target->statusCondition != STATUS_NONE) {
        return;
    }

    /* Cannot inflict on fainted Pokemon */
    if (target->currentHP == 0) {
        return;
    }

    /* Type immunity checks */
    switch (status) {
        case STATUS_BURN:
            if (target->type1 == TYPE_FIRE || target->type2 == TYPE_FIRE) {
                return;
            }
            break;

        case STATUS_FREEZE:
            if (target->type1 == TYPE_ICE || target->type2 == TYPE_ICE) {
                return;
            }
            break;

        case STATUS_PARALYSIS:
            break;

        case STATUS_POISON:
        case STATUS_TOXIC:
            if (target->type1 == TYPE_POISON || target->type2 == TYPE_POISON ||
                target->type1 == TYPE_STEEL  || target->type2 == TYPE_STEEL) {
                return;
            }
            break;

        default:
            break;
    }

    /* Check chance */
    if (chance < 100) {
        roll = ((u32)fn_800D37CC() % 100) + 1;
        if (roll > chance) {
            return;
        }
    }

    /* Inflict the status */
    if (status == STATUS_SLEEP) {
        s32 sleepTurns = ((u32)fn_800D37CC() % 5) + 1;
        target->statusCondition = (u32)sleepTurns;
    } else {
        target->statusCondition = status;
    }
}

/* #######################################################################
 * DAMAGE CALCULATION (from battle_damage.c)
 * ####################################################################### */

/**
 * Apply a stat stage modifier to a base stat value.
 */
s32 battle_ApplyStatStage(s32 baseStat, s8 stage) {
    s32 idx;

    /* Clamp stage to valid range */
    if (stage < -6) stage = -6;
    if (stage >  6) stage =  6;

    idx = stage + 6;
    return (baseStat * sStatStageNumerator[idx]) / sStatStageDenominator[idx];
}

/**
 * Check if a move gets STAB (Same-Type Attack Bonus).
 */
BOOL battle_IsSTAB(BattlePokemon* attacker, u8 moveType) {
    return (attacker->type1 == moveType || attacker->type2 == moveType);
}

/**
 * Calculate whether a critical hit occurs.
 */
u8 battle_CalcCriticalHit(BattlePokemon* attacker, const MoveData* move) {
    s32 critStage = 0;
    s32 random;
    s32 threshold;

    if (move->flags & 0x01) {
        critStage += 1;
    }

    if (attacker->volatileStatus & VSTATUS_FOCUS) {
        critStage += 2;
    }

    switch (critStage) {
        case 0: threshold = 16; break;
        case 1: threshold = 8;  break;
        case 2: threshold = 4;  break;
        case 3: threshold = 3;  break;
        default: threshold = 2; break;
    }

    random = (u32)fn_800D37CC() % threshold;
    return (random == 0) ? 1 : 0;
}

/**
 * Get a random damage factor between 85 and 100 (inclusive).
 */
s32 battle_GetRandomDamageFactor(void) {
    return 85 + ((u32)fn_800D37CC() % 16);
}

/**
 * Calculate damage using the Gen III formula.
 */
s32 battle_CalcDamage(BattlePokemon* attacker, BattlePokemon* defender,
                      const MoveData* move, u8 isCritical) {
    s32 level;
    s32 power;
    s32 attack;
    s32 defense;
    s32 damage;
    u8  moveType;
    u8  effectiveness;
    s32 randomFactor;

    level = attacker->level;
    power = move->basePower;
    moveType = move->type;

    /* A move with 0 base power does no damage (status move) */
    if (power == 0) {
        return 0;
    }

    /* Determine physical or special based on move type (Gen III type-based split) */
    if (sTypeIsPhysical[moveType]) {
        attack = attacker->attack;
        defense = defender->defense;

        if (isCritical) {
            if (attacker->statStages.attack > 0) {
                attack = battle_ApplyStatStage(attack, attacker->statStages.attack);
            }
            if (defender->statStages.defense < 0) {
                defense = battle_ApplyStatStage(defense, defender->statStages.defense);
            }
        } else {
            attack = battle_ApplyStatStage(attack, attacker->statStages.attack);
            defense = battle_ApplyStatStage(defense, defender->statStages.defense);
        }
    } else {
        attack = attacker->spAttack;
        defense = defender->spDefense;

        if (isCritical) {
            if (attacker->statStages.spAttack > 0) {
                attack = battle_ApplyStatStage(attack, attacker->statStages.spAttack);
            }
            if (defender->statStages.spDefense < 0) {
                defense = battle_ApplyStatStage(defense, defender->statStages.spDefense);
            }
        } else {
            attack = battle_ApplyStatStage(attack, attacker->statStages.spAttack);
            defense = battle_ApplyStatStage(defense, defender->statStages.spDefense);
        }
    }

    /* Prevent division by zero */
    if (defense == 0) defense = 1;
    if (attack == 0) attack = 1;

    /* Core damage formula */
    damage = ((2 * level / 5 + 2) * power * attack / defense) / 50 + 2;

    /* Apply STAB */
    if (battle_IsSTAB(attacker, moveType)) {
        damage = damage * 3 / 2;
    }

    /* Apply type effectiveness (both defending types) */
    effectiveness = battle_GetTypeEffectiveness(moveType,
                                                 defender->type1,
                                                 defender->type2);
    if (effectiveness == TYPE_EFF_IMMUNE) {
        return 0;
    }
    damage = damage * effectiveness / 100;

    /* Apply critical hit multiplier */
    if (isCritical) {
        damage *= 2;
    }

    /* Apply random factor (85-100) */
    randomFactor = battle_GetRandomDamageFactor();
    damage = damage * randomFactor / 100;

    /* Minimum 1 damage for any damaging move that isn't type-immune */
    if (damage < 1) {
        damage = 1;
    }

    return damage;
}

/* #######################################################################
 * AI (from battle_ai.c)
 * ####################################################################### */

/**
 * Evaluate a single move against a single target.
 */
s32 battle_AIEvaluateMove(s32 aiSlot, s32 targetSlot, u16 moveID) {
    s32 score;

    score = AI_SCORE_BASE;

    /* Add random variance */
    score += ((u32)fn_800D37CC() % AI_SCORE_RANDOM_RANGE) -
             (AI_SCORE_RANDOM_RANGE / 2);

    return score;
}

/**
 * Choose an action for an AI-controlled trainer's Pokemon.
 */
void battle_AIChooseAction(s32 trainerSlot, TurnAction* outAction) {
    s32 bestScore;
    s32 score;
    s32 bestMove;
    s32 bestTarget;
    s32 moveIdx;
    s32 targetIdx;

    bestScore = -9999;
    bestMove = 0;
    bestTarget = BATTLE_POS_PLAYER_LEFT;

    for (moveIdx = 0; moveIdx < 4; moveIdx++) {
        for (targetIdx = 0; targetIdx < BATTLE_TOTAL_POKEMON; targetIdx++) {
            if (trainerSlot == 1) {
                if (targetIdx >= BATTLE_POS_ENEMY_LEFT) {
                    continue;
                }
            } else {
                if (targetIdx < BATTLE_POS_ENEMY_LEFT) {
                    continue;
                }
            }

            score = battle_AIEvaluateMove(trainerSlot, targetIdx, 0);

            if (score > bestScore) {
                bestScore = score;
                bestMove = moveIdx;
                bestTarget = targetIdx;
            }
        }
    }

    /* Fill in the action */
    outAction->actionType = 0;  /* Fight */
    outAction->moveIndex = (u8)bestMove;
    outAction->targetSlot = (u8)bestTarget;
    outAction->priority = 0;
    outAction->moveID = 0;
    outAction->speedValue = 0;
}

/* #######################################################################
 * SHADOW POKEMON (from battle_shadow.c)
 * ####################################################################### */

/**
 * Check if a Pokemon is a Shadow Pokemon.
 */
BOOL battle_IsShadowPokemon(BattlePokemon* pokemon) {
    return (pokemon->isShadow != 0);
}

/**
 * Attempt to put a Shadow Pokemon into Hyper Mode.
 */
void battle_EnterHyperMode(BattlePokemon* pokemon) {
    s32 roll;
    s32 chance;

    if (battle_IsShadowPokemon(pokemon) == FALSE) {
        return;
    }

    /* Already in Hyper Mode */
    if (pokemon->shadowMode == SHADOW_HYPER_MODE) {
        return;
    }

    /* Calculate Hyper Mode chance based on heart gauge */
    if (pokemon->shadowGaugeMax == 0) {
        return;
    }

    chance = HYPER_MODE_BASE_CHANCE * pokemon->shadowGauge / pokemon->shadowGaugeMax;
    if (chance < 1) chance = 1;

    roll = ((u32)fn_800D37CC() % 100) + 1;

    if (roll <= chance) {
        pokemon->shadowMode = SHADOW_HYPER_MODE;
    }
}

/**
 * Exit Hyper Mode (via the "Call" action).
 */
void battle_ExitHyperMode(BattlePokemon* pokemon) {
    if (battle_IsShadowPokemon(pokemon) == FALSE) {
        return;
    }

    if (pokemon->shadowMode != SHADOW_HYPER_MODE) {
        return;
    }

    /* Exit Hyper Mode */
    pokemon->shadowMode = SHADOW_NORMAL;

    /* Reduce heart gauge (contributes to purification) */
    if (pokemon->shadowGauge > CALL_GAUGE_REDUCTION) {
        pokemon->shadowGauge -= CALL_GAUGE_REDUCTION;
    } else {
        pokemon->shadowGauge = 0;
    }
}

/**
 * Process the "Call" battle command for Shadow Pokemon.
 */
void battle_CallPokemon(BattlePokemon* pokemon) {
    if (battle_IsShadowPokemon(pokemon) == FALSE) {
        return;
    }

    if (pokemon->shadowMode == SHADOW_HYPER_MODE) {
        battle_ExitHyperMode(pokemon);
    } else {
        s32 reduction = CALL_GAUGE_REDUCTION / 2;
        if (pokemon->shadowGauge > (u16)reduction) {
            pokemon->shadowGauge -= (u16)reduction;
        } else {
            pokemon->shadowGauge = 0;
        }
    }
}

/**
 * Calculate Shadow Rush damage.
 */
s32 battle_CalcShadowRushDamage(BattlePokemon* attacker, BattlePokemon* defender) {
    s32 level;
    s32 power;
    s32 attack;
    s32 defense;
    s32 damage;
    s32 randomFactor;
    u8  effectiveness;

    level = attacker->level;
    power = SHADOW_RUSH_POWER;

    /* Hyper Mode boost: 1.5x power */
    if (attacker->shadowMode == SHADOW_HYPER_MODE) {
        power = power * 3 / 2;
    }

    /* Shadow Rush is physical */
    attack = battle_ApplyStatStage(attacker->attack, attacker->statStages.attack);
    defense = battle_ApplyStatStage(defender->defense, defender->statStages.defense);

    if (defense == 0) defense = 1;
    if (attack == 0) attack = 1;

    /* Core damage formula */
    damage = ((2 * level / 5 + 2) * power * attack / defense) / 50 + 2;

    /* Type effectiveness: Shadow type mechanics */
    if (defender->isShadow) {
        damage = damage * TYPE_EFF_NOT_VERY / TYPE_EFF_NORMAL;
    } else {
        effectiveness = battle_GetTypeEffectiveness(TYPE_SHADOW,
                                                     defender->type1,
                                                     defender->type2);
        damage = damage * effectiveness / TYPE_EFF_NORMAL;
    }

    /* Random factor (85-100) */
    randomFactor = battle_GetRandomDamageFactor();
    damage = damage * randomFactor / 100;

    if (damage < 1) damage = 1;

    /* Apply recoil to attacker (1/16 max HP) */
    {
        s32 recoil = attacker->maxHP / SHADOW_RUSH_RECOIL_DIV;
        if (recoil < 1) recoil = 1;

        if (attacker->currentHP <= (u16)recoil) {
            attacker->currentHP = 0;
        } else {
            attacker->currentHP -= (u16)recoil;
        }
    }

    return damage;
}

/**
 * Check if a Pokemon can be snagged.
 */
BOOL battle_CanSnag(BattlePokemon* target) {
    if (battle_IsShadowPokemon(target) == FALSE) {
        return FALSE;
    }

    if (target->currentHP == 0) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Process the snagging sequence for a target Pokemon.
 */
void battle_ProcessSnagging(s32 targetSlot) {
    /* Stub -- full implementation requires item/party/save systems */
}

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 8 functions matched
 * =================================================================== */

extern u8 lbl_8047B420;
extern u32 lbl_8047B424;
extern u32 lbl_8047B428;
extern u32 lbl_8047B42C;
extern u32 lbl_8047B430;
extern u8 lbl_8047B434;
extern u32 lbl_8047B438;
extern u8 lbl_8047B440;
extern u8 lbl_8047B441;
extern u32 lbl_8047B444;
extern u32 lbl_8047B450;
extern u32 lbl_8047B454;
extern u32 lbl_8047B458;
extern u32 lbl_8047B45C;
extern u32 lbl_8047B460;
extern u32 lbl_8047B468;
extern u8 lbl_8047B5C1;

extern void* fn_800F92D4(u32 size);
extern s32 fn_801E25C8(void);

/* Address: 0x801E11CC | Size: 0x8 | Pattern: sda_getter */
u8 fn_801E11CC(void) {
    return lbl_8047B434;
}

/* Address: 0x801E11E0 | Size: 0x8 | Pattern: sda_getter */
u32 fn_801E11E0(void) {
    return lbl_8047B424;
}

/* Address: 0x801E11E8 | Size: 0x8 | Pattern: sda_getter */
u8 fn_801E11E8(void) {
    return lbl_8047B420;
}

/* Address: 0x801ED640 | Size: 0x8 | Pattern: sda_setter */
void fn_801ED640(u8 val) {
    lbl_8047B5C1 = val;
}

/* Address: 0x801EE034 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fn_801EE034(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x801EE04C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801EE04C(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x1]);
}

/* Address: 0x801EE064 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fn_801EE064(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* Address: 0x801EE468 | Size: 0x8 | Pattern: return_constant */
u32 fn_801EE468(void) { return 48; }

/* #######################################################################
 * COVERAGE STUBS: battle state machine functions (0x801E03D4 - 0x801EF02C)
 * 138 functions remaining for full coverage of battle_logic.c TU.
 * These are pragma stubs for linker coverage -- replace with real
 * decompilations as disassembly analysis proceeds.
 * ####################################################################### */



/* ============================================================ */
/* Declarations for asm wrappers (auto-generated)               */
/* ============================================================ */

/* Standard library and system functions */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern u32   OSGetTick(void);
extern u32   OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(u32 level);
extern void  OSRegisterVersion(const char* version);
extern void  OSCancelThread(void* thread);
extern u32   PPCMfhid2(void);
extern u32   VIGetTvFormat(void);
extern void  DCFlushRange(void* addr, u32 size);
extern void  DCInvalidateRange(void* addr, u32 size);
extern u32   __mod2u(u32 a, u32 b);

/* Forward declarations for intra-TU non-standard sig functions */
void fn_801E1B54(void* val);
u32 fn_801E1B84(void);
void fn_801E1BB8(void* val);
u32 fn_801E1BE8(void);
u8* fn_801ED2DC(u8* data);

/* Forward declarations for intra-TU functions */
void fn_80029760(void);
void fn_8009B388(void);
void fn_8009B55C(void);
void fn_8009B614(void);
void fn_8009F1D0(void);
void fn_800A19CC(void);
void fn_800A1F94(void);
void fn_800A221C(void);
void fn_800A2D38(void);
void fn_800A39E0(void);
void fn_800A501C(void);
void fn_800A50E4(void);
void fn_800A541C(void);
void DVDCancel(void);
void fn_800A8850(void);
void fn_800AA2F0(void);
void fn_800B7874(void);
void fn_800B7D3C(void);
void fn_800B7D74(void);
void fn_800B857C(void);
void fn_800B884C(void);
void fn_800B928C(void);
void fn_800B9E6C(void);
void fn_800BA6B0(void);
void fn_800BA9E4(void);
void fn_800BACA0(void);
void fn_800BAFFC(void);
void fn_800BB29C(void);
void fn_800BC114(void);
void fn_800BC1A0(void);
void fn_800BC1E4(void);
void fn_800BC228(void);
void fn_800BC290(void);
void fn_800BC36C(void);
void fn_800BC3E0(void);
void fn_800BC454(void);
void fn_800BC4C0(void);
void fn_800BC52C(void);
void fn_800BC580(void);
void fn_800BC6F0(void);
void fn_800BC8C8(void);
void fn_800BCDDC(void);
void fn_800BCE30(void);
void fn_800BCE5C(void);
void fn_800BCE88(void);
void fn_800BCEF4(void);
void fn_800BD2E0(void);
void fn_800BD4B4(void);
void fn_800BD554(void);
void fn_800BD744(void);
void fn_800BD7A0(void);
void fn_800C4928(void);
void fn_800C4C50(void);
void fn_800C4C98(void);
void fn_800CA7FC(void);
void GScameraGetPosition(void);
void GScameraGetActiveCamera(void);
void fn_800D2F34(void);
void fn_800D3088(void);
void fn_800D3190(void);
void fn_800D3410(void);
void fn_800D3FA4(void);
void fn_800D59B8(void);
void fn_800D5C18(void);
void fn_800D6680(void);
void fn_800D6728(void);
void fn_800D67BC(void);
void fn_800D6A00(void);
void fn_800D7820(void);
void fn_800D85D4(void);
void fn_800D888C(void);
void fn_800D88DC(void);
void fn_800D9B58(void);
void fn_800D9ED8(void);
void fn_800DA028(void);
void fn_800DA1E8(void);
void fn_800DA2BC(void);
void fn_800DA4C4(void);
void fn_800E008C(void);
void fn_800E00AC(void);
void fn_800E013C(void);
void fn_800E01D0(void);
void fn_800E202C(void);
void fn_800E209C(void);
void fn_800E24B0(void);
void fn_800E27B0(void);
void fn_800E2B00(void);
void fn_800E4014(void);
void fn_800E407C(void);
void fn_800E43A4(void);
void fn_800E4BF4(void);
void fn_800EE150(void);
void fn_800EE3BC(void);
void fn_800EE828(void);
void fn_800EE928(void);
void fn_800EE9BC(void);
void fn_800EF4F4(void);
void fn_800EF4FC(void);
void fn_800EFD3C(void);
void _threadSwitch(void);
void fn_800F0470(void);
void fn_800F9E70(void);
void fn_800FAEF8(void);
void fn_800FE6DC(void);
void fn_800FE6F8(void);
void fn_800FE834(void);
void fn_800FF56C(void);
void fn_801069FC(void);
void fn_80111C24(void);
void fn_80113D58(void);
void fn_80116958(void);
void fn_8011E15C(void);
void fn_8011E778(void);
void fn_8011ED68(void);
void fn_8011EE28(void);
void fn_8011F4A8(void);
void fn_8011F4C0(void);
void fn_8011F550(void);
void fn_8011F5C8(void);
void fn_8011F910(void);
void fn_80121ADC(void);
void fn_80121B4C(void);
void fn_8012361C(void);
void fn_80123FBC(void);
void fn_80124A60(void);
void fn_80125424(void);
u32 fn_8012BDE0(u32, u32);
void fn_8014E9B4(void);
void fn_8014EE40(void);
void fn_8014F2DC(void);
void sndStreamFree(void);
void fn_8014FF0C(void);
void fn_80150564(void);
void fn_80166AB8(void);
void fn_80183018(void);
void fn_80183350(void);
void fn_80185EE8(void);
void fn_8018805C(void);
void fn_8018A280(void);
void fn_8018BDF4(void);
void fn_801902E0(void);
void fn_801906A0(void);
void fn_8019075C(void);
void fn_801E1D0C(void);
void fn_801E1D48(void);
void fn_801E1D7C(void);
void fn_801E1E1C(void);
void fn_801E1FF8();
void fn_801E24B0(void);
s32 fn_801E25C8(void);
void fn_801E2B74(void);
void fn_801E2CA8(void);
void fn_801E34F0(void);
void fn_801E3858(u32 *out0, u32 *out1);
void fn_801E386C(void);
u8 fn_801E38D8(void);
s32 fn_801E38E8(void *dst);
s32 fn_801E3930(void *dst);
void fn_801E3978(void);
void fn_801E3F54(void);
s32 fn_801E4058(void);
void fn_801E40F8(void);
void fn_801E446C(void *arg);
void fn_801E449C(void);
int fn_801E4650(void);
int fn_801E4724(void);
void fn_801E4778(void);
void fn_801E4A6C(void);
u32 fn_801E4AC4(u32 arg);
void fn_801E4B08();
void fn_801E4DAC(void);
void fn_801E4DE8(void);
void fn_801E4E1C(void);
u32 fn_801E4EF0(u32 arg);
void fn_801E4F34(void *arg);
void fn_801E5400(void);
void fn_801E543C(void);
void fn_801E5470(void);
void fn_801E5548(void);
void fn_801E578C(void);
void fn_801E57D0(void);
void fn_801E590C(void);
void fn_801E5A28(void);
void fn_801E5DE4(void);
void fn_801E5FC4(void);
void fn_801E60B4(void);
void fn_801E611C(void);
void fn_801E62D8(void);
void fn_801E632C(void);
void fn_801E6578(void);
void fn_801E6684(void);
void fn_801E810C(void);
void fn_801E9B98(void);
void fn_801EB644(void);
void fn_801EBCC0(void);
void fn_801EC368(void);
void fn_801ECA10(void);
void fn_801ECAB0(void);
void fn_801ECF14(void);
void fn_801ECFA4(u8 *obj, u8 *src);
u16 fn_801EE470(u16 arg);
void fn_801EE4DC(u16 arg, u16 val);
void fn_801EF1E4(void);

/* ============================================================ */
/* 0x801E03D4 | size: 0x388 | large */
#if 0
asm void fn_801E03D4(void) {
#include "src/game/battle/battle_logic_fn_801E03D4.inc"
}
#else
void fn_801E03D4(void) {
    extern void fn_80029760();
    extern void fn_801069FC();
    extern void fn_80123FBC();
    extern u8 jumptable_803751B8[];
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;

    r31 = 0x1;
    r29 = 0x0;
    do {
        if (r29 <= 0xc) {
            switch (r29) {
            case 0x0:
                r3 = 0x3b21; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                ((void(*)(void))fn_8001E184)();
                tmp = (s8)r3;
                r29 = (r29 != 0xc) ? 0x1 : 0x2;
                break;
            case 0x1:
                r3 = 0x3b22; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r29 = 0xc;
                break;
            case 0x2:
                r3 = 0x3b23; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r3 = 0x6; r4 = 0x0; r5 = 0x0;
                ((void(*)(void))fn_8001BDF4)();
                r29 = (r29 < 0xc) ? 0x1 : 0x3;
                break;
            case 0x3:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF; r29 = r3;
                ((void(*)(void))fn_8012AC08)();
                r4 = r3; r3 = r29;
                ((void(*)(void))fn_80129D64)();
                tmp = r3 & 0xFF;
                r29 = (r29 == 0xc) ? 0x5 : 0x4;
                break;
            case 0x4:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                r29 = r3;
                fn_80123FBC();
                tmp = r3 & 0xFF;
                if (r29 == 0xc) { tmp = 0x0; }
                else {
                    r3 = r29;
                    ((void(*)(void))fn_8011EE40)();
                    tmp = r3 & 0xFFFF;
                    if (r29 == 0xc) { tmp = 0x1; }
                    else {
                        ((void(*)(void))fn_801EEC74)();
                        tmp = r3 & 0xFF;
                        tmp = (r29 == 0xc) ? 0x0 : 0x1;
                    }
                }
                tmp = tmp & 0xFF;
                r29 = (r29 == 0xc) ? 0xb : 0x6;
                break;
            case 0x5:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                ((void(*)(void))fn_8011F4F0)();
                r4 = r3; r3 = 0x32;
                ((void(*)(void))fn_80132A38)();
                r3 = 0x3b24; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r29 = 0xc;
                break;
            case 0x6:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                ((void(*)(void))fn_8011F4F0)();
                r4 = r3; r3 = 0x32;
                ((void(*)(void))fn_80132A38)();
                r3 = 0x3b25; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                ((void(*)(void))fn_8001E184)();
                tmp = (s8)r3;
                r29 = (r29 != 0xc) ? 0x1 : 0x7;
                break;
            case 0x7:
                r3 = 0x3b26; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r4 = r30; r3 = 0x2;
                fn_80029760();
                r29 = ((s32)r3 != 0) ? 0x8 : 0x9;
                break;
            case 0x8:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                ((void(*)(void))fn_8011F4F0)();
                r4 = r3; r3 = 0x32;
                ((void(*)(void))fn_80132A38)();
                r3 = 0x3b27; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r29 = 0xc;
                break;
            case 0x9:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                ((void(*)(void))fn_8011F4F0)();
                r4 = r3; r3 = 0x32;
                ((void(*)(void))fn_80132A38)();
                r3 = 0x3b1f; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                ((void(*)(void))fn_8001E184)();
                tmp = (s8)r3;
                r29 = ((s32)r3 != 0) ? 0x7 : 0xa;
                break;
            case 0xa:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                ((void(*)(void))fn_8011F4F0)();
                r4 = r3; r3 = 0x32;
                ((void(*)(void))fn_80132A38)();
                r3 = 0x3b47; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r29 = 0xc;
                break;
            case 0xb:
                r3 = 0x0; r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r30 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                ((void(*)(void))fn_8011F4F0)();
                r4 = r3; r3 = 0x32;
                ((void(*)(void))fn_80132A38)();
                r3 = 0x3b20; r4 = 0x1; r5 = 0x0; r6 = 0x1;
                ((void(*)(void))fn_80106698)();
                r29 = 0xc;
                break;
            case 0xc:
                r3 = 0x1;
                fn_801069FC();
                r31 = 0x0;
                break;
            default:
                break;
            }
        }
    } while ((s32)r31 != 0);
    return;
}
#endif

/* 0x801E075C | size: 0x284 | large */
#if 0
asm void fn_801E075C(void) {
#include "src/game/battle/battle_logic_fn_801E075C.inc"
}
#else
void fn_801E075C(void) {
    extern u8 lbl_80279A00[];
    extern u8 lbl_803750C8[];
    extern f32 lbl_8047E3F4;
    extern f64 lbl_8047E400;
    extern f64 lbl_8047E408;
    extern f32 lbl_8047E414;
    extern void fn_800D3088();
    extern void fn_800E01D0();
    extern void fn_800E4014();
    extern void fn_800E407C();
    extern void fn_800E43A4();
    extern void fn_800E4BF4();
    extern void _threadSwitch();
    extern void fn_80113D58();
    extern void fn_8011E15C();
    extern void fn_8011E778();
    extern void fn_8011F550();
    extern void fn_8011F5C8();
    extern void fn_80123FBC();
    extern void fn_80166AB8();
    u8 sp[0xE0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r4 = (u32)lbl_80279A00;
    r29 = r3;
    r31 = (u32)lbl_80279A00;
    r25 = 0x0;
    r7 = *(u32*)((u8*)r31 + 0x4C);
    r26 = 0x0;
    r6 = *(u32*)((u8*)r31 + 0x50);
    r30 = 0x1;
    r5 = *(u32*)((u8*)r31 + 0x54);
    r4 = *(u32*)((u8*)r31 + 0x58);
    r3 = *(u32*)((u8*)r31 + 0x5C);
    tmp = *(u32*)((u8*)r31 + 0x60);
    *(u32*)(sp + 0x10) = tmp;
    do {
    do {
        if ((s32)r26 != 1) {
            if ((s32)r26 < 1) {
                if ((s32)r26 < 0) {
                    break;
                }
                if ((s32)r26 != 0x64) {
                    break;
                    }
                r3 = 0x0;
                r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r29 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                fn_8011F550();
                r25 = *(u32*)((u8*)r31 + 0x0);
                r26 = *(u32*)((u8*)r31 + 0x4);
                r3 = (u32)sp + 0x20;
                r27 = *(u32*)((u8*)r31 + 0x8);
                r28 = *(u32*)((u8*)r31 + 0xC);
                r12 = *(u32*)((u8*)r31 + 0x10);
                r11 = *(u32*)((u8*)r31 + 0x14);
                r10 = *(u32*)((u8*)r31 + 0x18);
                r9 = *(u32*)((u8*)r31 + 0x1C);
                r8 = *(u32*)((u8*)r31 + 0x20);
                r7 = *(u32*)((u8*)r31 + 0x24);
                r6 = *(u32*)((u8*)r31 + 0x28);
                r5 = *(u32*)((u8*)r31 + 0x2C);
                r4 = *(u32*)((u8*)r31 + 0x30);
                r3 = *(u32*)(r3 + tmp);
                fn_80113D58();
                tmp = r3;
                r4 = (u32)lbl_803750C8;
                r4 = (u32)lbl_803750C8;
                r3 = (u32)sp + 0x14;
                r25 = tmp;
                fn_800E01D0();
                r3 = 0x0;
                r4 = 0x2;
                ((void(*)(void))fn_80129280)();
                r4 = r29 & 0xFFFF;
                ((void(*)(void))fn_8012AC08)();
                r26 = r3;
                fn_8011F5C8();
                tmp = r3;
                r3 = r26;
                r26 = tmp;
                fn_80123FBC();
                tmp = r3 & 0xFF;
                if ((s32)r26 != 0x64) {
                    r3 = r26;
                    fn_8011E778();
                    if (r3 != 0) {
                        fn_8011E15C();
                        r3 = r3 & 0xFFFF;
                        r4 = 0x0;
                        r5 = 0x0;
                        fn_80166AB8();
                }
                }
                r3 = r25;
                r4 = (u32)sp + 0x14;
                fn_800E43A4();
                r3 = r25;
                r4 = (u32)sp + 0x8;
                fn_800E407C();
                r26 = 0x1;
                break;
            }
            f27 = lbl_8047E3F4;
            r28 = 0x43300000;
            f28 = lbl_8047E400;
            f30 = lbl_8047E408;
            f31 = lbl_8047E414;
            while (f27 < f31) {

                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x5C) = tmp;
                f29 = f0 - f28;
                fn_800D3088();
                f0 = f0 - f30;
                f0 = f0 / f29;
                f27 = f27 + f0;
                _threadSwitch();

            }
            r26 = 0x64;
            break;
                }
        r3 = r25;
        r30 = 0x0;
        r4 = 0x0;
        fn_800E4014();
        r3 = r25;
        fn_800E4BF4();
        r25 = 0x0;
    } while (0);
        ;
    } while ((s32)r30 != 0);
    return;
}
#endif

/* 0x801E09E0 | size: 0x598 | large */
#if 0
asm void fn_801E09E0(void) {
#include "src/game/battle/battle_logic_fn_801E09E0.inc"
}
#else
void fn_801E09E0(void) {
    extern u8 lbl_80279A00[];
    extern u8 lbl_803750C8[];
    extern f32 lbl_8047E3F0;
    extern f32 lbl_8047E3F4;
    extern f64 lbl_8047E400;
    extern f64 lbl_8047E408;
    extern f32 lbl_8047E410;
    extern f32 lbl_8047E414;
    extern f32 lbl_8047E418;
    extern f32 lbl_8047E41C;
    extern f32 lbl_8047E420;
    extern void fn_800D3088();
    extern void fn_800E01D0();
    extern void fn_800E4014();
    extern void fn_800E407C();
    extern void fn_800E43A4();
    extern void fn_800E4BF4();
    extern void fn_800EE150();
    extern void fn_800EE3BC();
    extern void fn_800EE828();
    extern void _threadSwitch();
    extern void fn_800FF56C();
    extern void fn_80113D58();
    extern void fn_80116958();
    extern void fn_8011E15C();
    extern void fn_8011E778();
    extern void fn_8011F550();
    extern void fn_8011F5C8();
    extern void fn_80166AB8();
    extern void fn_80183018();
    extern void fn_80183350();
    extern void fn_80185EE8();
    extern void fn_8018805C();
    extern void fn_8018A280();
    extern void fn_8018BDF4();
    extern void fn_801ED2DC();
    u8 sp[0xF0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    r3 = (u32)lbl_80279A00;
    r4 = (u32)lbl_803750C8;
    r31 = (u32)lbl_80279A00;
    r29 = 0x0;
    r7 = *(u32*)((u8*)r31 + 0x34);
    r30 = (u32)lbl_803750C8;
    r6 = *(u32*)((u8*)r31 + 0x38);
    r25 = 0x0;
    r5 = *(u32*)((u8*)r31 + 0x3C);
    r28 = 0x1;
    r4 = *(u32*)((u8*)r31 + 0x40);
    r3 = *(u32*)((u8*)r31 + 0x44);
    tmp = *(u32*)((u8*)r31 + 0x48);
    *(u32*)(sp + 0x10) = tmp;
    fn_800FF56C();
    r27 = r3;
    do {
    do {
        if ((s32)r25 != 0xa) {
            if ((s32)r25 < 0xa) {
                if ((s32)r25 != 1) {
                    if ((s32)r25 < 1) {
                        if ((s32)r25 < 0) {
                            break;
                        }
                        if ((s32)r25 >= 3) break;
                        goto L_801E0EAC;
                    }
                    if ((s32)r25 == 0x64) goto L_801E0F0C;
                    break;
                        }
                r5 = (u32)sp + 0x20;
                r3 = 0x4d;
                r4 = 0x1;
                fn_8018BDF4();
                r3 = 0x4d;
                r4 = 0x1;
                fn_80183350();
                r3 = 0x1DA0000;
                r3 = r3 + 0x1002;
                ((void(*)(void))fn_800F92D4)();
                r4 = 0x0;
                fn_800EE150();
                r4 = (u32)sp + 0x2c;
                r24 = r3;
                r5 = 0x0;
                r6 = 0x0;
                fn_800EE3BC();
                r3 = r24;
                fn_800EE828();
                r3 = (u32)sp + 0x2c;
                r4 = r30 + 0xc;
                fn_800E01D0();
                f1 = *(f32*)(sp + 0x2C);
                r3 = 0x4d;
                f2 = *(f32*)(sp + 0x30);
                r4 = 0x1;
                f3 = *(f32*)(sp + 0x34);
                r5 = 0x1;
                fn_80185EE8();
                r3 = 0x4d;
                r4 = 0x1;
                r5 = 0x1;
                fn_8018A280();
                r3 = r27;
                r4 = 0x2c;
                r5 = 0x0;
                fn_80116958();
                f27 = lbl_8047E3F4;
                r24 = 0x43300000;
                f31 = lbl_8047E400;
                f29 = lbl_8047E408;
                f28 = lbl_8047E418;
                while (f27 < f28) {

                    ((void(*)(void))fn_800D37CC)();
                    *(u32*)(sp + 0x74) = tmp;
                    f30 = f0 - f31;
                    fn_800D3088();
                    f0 = f0 - f29;
                    f0 = f0 / f30;
                    f27 = f27 + f0;
                    _threadSwitch();

                }
                r25 = 0x1;
                break;
                }
            r3 = (u32)sp + 0x2c;
            r4 = r30 + 0x18;
            fn_800E01D0();
            f1 = *(f32*)(sp + 0x2C);
            r3 = 0x4d;
            f2 = *(f32*)(sp + 0x30);
            r4 = 0x1;
            f3 = *(f32*)(sp + 0x34);
            r5 = 0x1;
            fn_80185EE8();
            r3 = 0x4d;
            r4 = 0x1;
            r5 = 0x1;
            fn_8018A280();
            r3 = r27;
            r4 = 0x2c;
            r5 = 0x2;
            fn_80116958();
            f27 = lbl_8047E3F4;
            r24 = 0x43300000;
            f31 = lbl_8047E400;
            f29 = lbl_8047E408;
            f28 = lbl_8047E3F0;
            while (f27 < f28) {

                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x7C) = tmp;
                f30 = f0 - f31;
                fn_800D3088();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;
                _threadSwitch();

            }
            f2 = lbl_8047E410;
            r3 = 0x4d;
            f0 = *(f32*)(sp + 0x34);
            r4 = 0x1;
            f1 = *(f32*)(sp + 0x2C);
            r5 = 0x1;
            f3 = f2 + f0;
            f2 = *(f32*)(sp + 0x30);
            fn_80185EE8();
            r3 = 0x4d;
            r4 = 0x1;
            r5 = 0x1;
            fn_8018A280();
            r3 = r27;
            r4 = 0x2c;
            r5 = 0x0;
            fn_80116958();
            r3 = (u32)sp + 0x2c;
            r4 = r30 + 0xc;
            fn_800E01D0();
            f1 = *(f32*)(sp + 0x2C);
            r3 = 0x4d;
            f2 = *(f32*)(sp + 0x30);
            r4 = 0x1;
            f3 = *(f32*)(sp + 0x34);
            r5 = 0x1;
            fn_80185EE8();
            r3 = 0x4d;
            r4 = 0x1;
            r5 = 0x1;
            fn_8018A280();
            r3 = r27;
            r4 = 0x2c;
            r5 = 0x2;
            fn_80116958();
            f27 = lbl_8047E3F4;
            r24 = 0x43300000;
            f31 = lbl_8047E400;
            f29 = lbl_8047E408;
            f28 = lbl_8047E41C;
            while (f27 < f28) {

                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x7C) = tmp;
                f30 = f0 - f31;
                fn_800D3088();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;
                _threadSwitch();

            }
            f1 = *(f32*)(sp + 0x20);
            r3 = 0x4d;
            f2 = *(f32*)(sp + 0x24);
            r4 = 0x1;
            f3 = *(f32*)(sp + 0x28);
            r5 = 0x1;
            fn_80185EE8();
            r3 = 0x4d;
            r4 = 0x1;
            r5 = 0x1;
            fn_8018A280();
            f1 = lbl_8047E3F4;
            r3 = 0x4d;
            f2 = lbl_8047E410;
            r4 = 0x1;
            fn_8018805C();
            f27 = lbl_8047E3F4;
            r25 = 0xa;
            f31 = lbl_8047E400;
            r24 = 0x43300000;
            f29 = lbl_8047E408;
            f28 = lbl_8047E420;
            while (f27 < f28) {

                ((void(*)(void))fn_800D37CC)();
                *(u32*)(sp + 0x7C) = tmp;
                f30 = f0 - f31;
                fn_800D3088();
                f0 = f0 - f29;
                f0 = f0 / f30;
                f27 = f27 + f0;
                _threadSwitch();

            }
            break;
        }
        r3 = 0x0;
        fn_801ED2DC();
        fn_8011F550();
        r24 = *(u32*)((u8*)r31 + 0x0);
        r25 = *(u32*)((u8*)r31 + 0x4);
        r3 = (u32)sp + 0x38;
        r29 = *(u32*)((u8*)r31 + 0x8);
        r26 = *(u32*)((u8*)r31 + 0xC);
        r12 = *(u32*)((u8*)r31 + 0x10);
        r11 = *(u32*)((u8*)r31 + 0x14);
        r10 = *(u32*)((u8*)r31 + 0x18);
        r9 = *(u32*)((u8*)r31 + 0x1C);
        r8 = *(u32*)((u8*)r31 + 0x20);
        r7 = *(u32*)((u8*)r31 + 0x24);
        r6 = *(u32*)((u8*)r31 + 0x28);
        r5 = *(u32*)((u8*)r31 + 0x2C);
        r4 = *(u32*)((u8*)r31 + 0x30);
        r3 = *(u32*)(r3 + tmp);
        fn_80113D58();
        tmp = r3;
        r3 = (u32)sp + 0x14;
        r29 = tmp;
        r4 = r30 + 0x0;
        fn_800E01D0();
        r3 = 0x0;
        fn_801ED2DC();
        if (r3 != 0) {
            fn_8011F5C8();
            fn_8011E778();
            if (r3 != 0) {
                fn_8011E15C();
                r3 = r3 & 0xFFFF;
                r4 = 0x0;
                r5 = 0x0;
                fn_80166AB8();
        }
        }
        r3 = r29;
        r4 = (u32)sp + 0x14;
        fn_800E43A4();
        r3 = r29;
        r4 = (u32)sp + 0x8;
        fn_800E407C();
        r25 = 0x2;
        break;
    L_801E0EAC:
        f27 = lbl_8047E3F4;
        r26 = 0x43300000;
        f28 = lbl_8047E400;
        f30 = lbl_8047E408;
        f31 = lbl_8047E414;
        while (f27 < f31) {

            ((void(*)(void))fn_800D37CC)();
            *(u32*)(sp + 0x7C) = tmp;
            f29 = f0 - f28;
            fn_800D3088();
            f0 = f0 - f30;
            f0 = f0 / f29;
            f27 = f27 + f0;
            _threadSwitch();

        }
        r25 = 0x64;
        break;
    L_801E0F0C:
        r28 = 0x0;
        r3 = 0x4d;
        r4 = 0x1;
        fn_80183018();
        r3 = r29;
        r4 = 0x0;
        fn_800E4014();
        r3 = r29;
        fn_800E4BF4();
        r29 = 0x0;
    } while (0);
        ;
    } while ((s32)r28 != 0);
    return;
}
#endif

/* 0x801E0F78 | size: 0x3C | small */
#if 0
asm void fn_801E0F78(void) {
#include "src/game/battle/battle_logic_fn_801E0F78.inc"
}
#else
void fn_801E0F78(void) {
    extern u32 fn_8011E778(void);
    extern u32 fn_8011E15C(void);
    extern void fn_80166AB8(u32, u32, u32);
    u32 r3;
    r3 = fn_8011E778();
    if (r3 != 0) {
        r3 = fn_8011E15C();
        r3 = r3 & 0xFFFF;
        fn_80166AB8(r3, 0, 0);
    }
}
#endif

/* 0x801E0FB4 | size: 0x1BC | medium */
#if 0
asm void fn_801E0FB4(void) {
#include "src/game/battle/battle_logic_fn_801E0FB4.inc"
}
#else
void fn_801E0FB4(void) {
    extern u8 lbl_80467CF8[];
    extern u8 lbl_8047B43C;
    extern void fn_800D3190();
    extern void fn_800D3410();
    extern void fn_800D3FA4();
    extern void fn_800F0470();
    extern void fn_800FE6DC();
    extern void fn_800FE6F8();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f5 = 0.0f;

    r29 = r3;
    r30 = r4;
    r31 = r5;
    tmp = *(u8*)&lbl_8047B420;
    if (tmp == 0) {
        r27 = 0x1;
        goto L_801E112C;
    }
    tmp = *(u32*)&lbl_8047B428;
    if ((s32)tmp != 2) {
        if ((s32)tmp < 2) {
            if ((s32)tmp != 0) {
                if ((s32)tmp < 0) {
                    goto L_801E10A0;
                }
                if ((s32)tmp >= 4) goto L_801E10A0;
                goto L_801E1064;
                }
            r27 = 0x1;
            goto L_801E10A0;
                }
        tmp = 0x2;
        r3 = (u32)lbl_80467CF8;
        *(u32*)&lbl_8047B428 = tmp;
        r28 = (u32)lbl_80467CF8;
        r27 = 0x0;
        while (1) {
            tmp = *(u32*)&lbl_8047B42C;
            if (r27 >= tmp) break;
            r3 = *(u32*)((u8*)r28 + 0x0);
            if (r3 != 0) {
                fn_800FE6F8();
            }
            r27 = r27 + 0x1;
            r28 = r28 + 0x4;


        }
        r27 = 0x1;
        goto L_801E10A0;
    }
    r27 = 0x0;
    goto L_801E10A0;
L_801E1064:
    r28 = 0x0;
    r3 = (u32)lbl_80467CF8;
    *(u32*)&lbl_8047B428 = r28;
    r27 = (u32)lbl_80467CF8;
    while (1) {
        tmp = *(u32*)&lbl_8047B42C;
        if (r28 >= tmp) break;
        r3 = *(u32*)((u8*)r27 + 0x0);
        if (r3 != 0) {
            fn_800FE6DC();
        }
        r28 = r28 + 0x1;
        r27 = r27 + 0x4;


    }
    r27 = 0x0;
L_801E10A0:
do {
    tmp = *(u32*)&lbl_8047B424;
    if ((s32)tmp != 3) {
        if ((s32)tmp < 3) {
            break;
        }
        if ((s32)tmp >= 5) break;

    } else {
    tmp = *(u32*)&lbl_8047B428;
    if ((s32)tmp == 2) break;
    tmp = 0x1;
    *(u32*)&lbl_8047B428 = tmp;
    break;
    }
    tmp = *(u32*)&lbl_8047B428;
    r4 = 0x2;
    if ((s32)tmp == 0) {
        r4 = 0x1;
    }
    r3 = *(u32*)&lbl_8047B430;
    *(u32*)&lbl_8047B428 = r4;
    tmp = r3 + 0x1;
    *(u32*)&lbl_8047B430 = tmp;
    if (tmp < 5) break;
    r3 = 0x3;
    tmp = 0x0;
    *(u32*)&lbl_8047B428 = r3;
    *(u32*)&lbl_8047B430 = tmp;
} while (0);
    tmp = *(u32*)&lbl_8047B428;
    if ((s32)tmp == 2) {
        r3 = 0xE3900000;
        fn_800F0470();
    }
L_801E112C:
    tmp = lbl_8047B43C;
    if (tmp != 0) {
        r27 = 0x0;
    }
    r4 = r27;
    r3 = 0x0;
    fn_800D3410();
    r3 = r29;
    r4 = r30;
    r5 = r31;
    fn_800D3FA4();
    fn_800D3190();
    return;
}
#endif

/* 0x801E1170 | size: 0x1C */
#if 0
asm void fn_801E1170(void) {
#include "src/game/battle/battle_logic_fn_801E1170.inc"
}
#else
void fn_801E1170(void) {
    lbl_8047B424 = 4;
    lbl_8047B428 = 3;
    lbl_8047B430 = 0;
}
#endif

/* 0x801E118C | size: 0x10 | tiny */
#if 0
asm void fn_801E118C(void) {
#include "src/game/battle/battle_logic_fn_801E118C.inc"
}
#else
void fn_801E118C(void) {
    lbl_8047B424 = 3;
    lbl_8047B428 = 3;
}
#endif

/* 0x801E119C | size: 0x14 | tiny */
#if 0
asm void fn_801E119C(void) {
#include "src/game/battle/battle_logic_fn_801E119C.inc"
}
#else
void fn_801E119C(void) {
    lbl_8047B424 = 2;
    lbl_8047B428 = 3;
}
#endif

/* 0x801E11B0 | size: 0x1C */
#if 0
asm void fn_801E11B0(void) {
#include "src/game/battle/battle_logic_fn_801E11B0.inc"
}
#else
void fn_801E11B0(void) {
    u32 prev = lbl_8047B428;
    lbl_8047B424 = 1;
    if ((s32)prev == 2) { return; }
    lbl_8047B428 = 1;
}
#endif

/* 0x801E11D4 | size: 0xC | tiny */
#if 0
asm void fn_801E11D4(void) {
#include "src/game/battle/battle_logic_fn_801E11D4.inc"
}
#else
void fn_801E11D4(u8 arg0, u8 arg1) {
    extern u8 lbl_8047B435;
    lbl_8047B434 = arg0;
    lbl_8047B435 = arg1;
}
#endif

/* 0x801E11F0 | size: 0x68 | small */
#if 0
asm void fn_801E11F0(void) {
#include "src/game/battle/battle_logic_fn_801E11F0.inc"
}
#else
void fn_801E11F0(void) {
    extern u32 lbl_80467CF8[];
    extern void fn_800FE6DC(u32);
    u32 *ptr;
    u32 i;
    u32 entry;
    ptr = lbl_80467CF8;
    i = 0;
    lbl_8047B424 = i;
    lbl_8047B420 = (u8)i;
    while (i < lbl_8047B42C) {
        entry = *ptr;
        if (entry != 0) {
            fn_800FE6DC(entry);
        }
        i++;
        ptr++;
    }
}
#endif

/* 0x801E1258 | size: 0x1C */
#if 0
asm void fn_801E1258(void) {
#include "src/game/battle/battle_logic_fn_801E1258.inc"
}
#else
void fn_801E1258(void) {
    lbl_8047B420 = 1;
    lbl_8047B424 = 2;
    lbl_8047B428 = 3;
}
#endif

/* 0x801E1274 | size: 0x2C */
#if 0
asm void fn_801E1274(void) {
#include "src/game/battle/battle_logic_fn_801E1274.inc"
}
#else
void fn_801E1274(void) {
    lbl_8047B438 = (u32)fn_800F92D4(0x0B521200);
}
#endif

/* 0x801E12A0 | size: 0x60 | small */
#if 0
asm void fn_801E12A0(void) {
#include "src/game/battle/battle_logic_fn_801E12A0.inc"
}
#else
s32 fn_801E12A0(u32 arg) {
    extern u32 lbl_80467CF8[];
    u32 *ptr;
    u32 n;
    if (lbl_8047B42C + 1 >= 4) return 0;
    ptr = lbl_80467CF8;
    for (n = 4; n != 0; n--) {
        if (*ptr == 0) {
            *ptr = arg;
            lbl_8047B42C++;
            return 1;
        }
        ptr++;
    }
    return 0;
}
#endif

/* 0x801E1300 | size: 0x68 | small */
#if 0
asm void fn_801E1300(void) {
#include "src/game/battle/battle_logic_fn_801E1300.inc"
}
#else
void fn_801E1300(void) {
    extern u8 lbl_80467CF8[];
    extern u8 lbl_8047B43C;
    extern void fn_800FE834(u32, u32, u32, void *);
    extern void _vtrTexDispFunc__Fv(void);
    lbl_8047B420 = 0;
    lbl_8047B424 = 0;
    lbl_8047B42C = 0;
    lbl_8047B438 = 0;
    lbl_8047B434 = 1;
    lbl_8047B43C = 0;
    memset(lbl_80467CF8, 0, 0x10);
    fn_800FE834(1, 0xfd, 0xa, _vtrTexDispFunc__Fv);
}
#endif

/* 0x801E1368 | size: 0x368 | large */
#if 0
asm void _vtrTexDispFunc__Fv(void) {
#include "src/game/battle/battle_logic__vtrTexDispFunc__Fv.inc"
}
#else
void _vtrTexDispFunc__Fv(void) {
    extern u8 lbl_80314958[];
    extern u8 lbl_80314C78[];
    extern u8 lbl_8047B435;
    extern f32 lbl_8047E428;
    extern f32 lbl_8047E42C;
    extern f32 lbl_8047E430;
    extern f32 lbl_8047E434;
    extern f32 lbl_8047E438;
    extern f32 lbl_8047E43C;
    extern f32 lbl_8047E440;
    extern f32 lbl_8047E444;
    extern f32 lbl_8047E448;
    extern f32 lbl_8047E44C;
    extern f32 lbl_8047E450;
    extern f32 lbl_8047E454;
    extern u8 lbl_8047E458[];
    extern u8 lbl_8047E45C[];
    extern u8 lbl_8047E464[];
    extern u8 lbl_8047E46C[];
    extern u8 lbl_8047E474[];
    extern void fn_800D59B8();
    extern void fn_800D5C18();
    extern void fn_800D6680();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D85D4();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800D9B58();
    extern void fn_800D9ED8();
    extern void fn_800DA028();
    extern void fn_800DA1E8();
    extern void fn_800DA2BC();
    extern void fn_800DA4C4();
    extern void fn_800FAEF8();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;

    tmp = *(u8*)&lbl_8047B420;
    if (tmp == 0) return;
    tmp = *(u8*)&lbl_8047B434;
    if (tmp == 0) {
        return;
    }
    tmp = *(u32*)&lbl_8047B438;
    if (tmp == 0) return;
    r3 = 0x1;
    fn_800D88DC();
    r3 = 0x4;
    fn_800D888C();
    f1 = lbl_8047E428;
    f3 = lbl_8047E42C;
    f2 = f1;
    f4 = lbl_8047E430;
    fn_800D9B58();
    r3 = 0x0;
    r4 = 0x1;
    r5 = 0x1;
    fn_800DA4C4();
    r3 = 0x1;
    r4 = 0x1;
    r5 = 0x0;
    fn_800DA2BC();
    r3 = 0x0;
    r4 = 0x1;
    r5 = 0x1;
    fn_800DA1E8();
    r3 = 0x0;
    fn_800DA028();
    r3 = 0x1;
    fn_800D9ED8();
    r3 = 0x4;
    fn_800D6A00();
    tmp = lbl_8047B435;
    if (tmp != 0) {
        r3 = 0x2;
        fn_800D888C();
        r3 = (u32)lbl_80314958;
        r3 = (u32)lbl_80314958;
        fn_800D7820();
        r3 = 0x4;
        fn_800D67BC();
        f1 = lbl_8047E434;
        f2 = lbl_8047E438;
        f3 = lbl_8047E428;
        fn_800D6680();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x0;
        r6 = 0x0;
        fn_800D5C18();
        f1 = lbl_8047E43C;
        f2 = lbl_8047E438;
        f3 = lbl_8047E428;
        fn_800D6680();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x0;
        r6 = 0x0;
        fn_800D5C18();
        f1 = lbl_8047E434;
        f2 = lbl_8047E440;
        f3 = lbl_8047E428;
        fn_800D6680();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x0;
        r6 = 0x0;
        fn_800D5C18();
        f1 = lbl_8047E43C;
        f2 = lbl_8047E440;
        f3 = lbl_8047E428;
        fn_800D6680();
        r3 = 0x0;
        r4 = 0xff;
        r5 = 0x0;
        r6 = 0x0;
        fn_800D5C18();
        fn_800D6728();
    }
    r3 = 0x2;
    fn_800D88DC();
    r3 = (u32)lbl_80314C78;
    r3 = (u32)lbl_80314C78;
    fn_800D7820();
    r4 = *(u32*)&lbl_8047B438;
    r3 = 0x0;
    fn_800D85D4();
    r3 = 0x4;
    fn_800D67BC();
    f1 = lbl_8047E444;
    f2 = lbl_8047E448;
    f3 = lbl_8047E428;
    fn_800D6680();
    r3 = 0x0;
    r4 = 0xff;
    r5 = 0xff;
    r6 = 0xff;
    fn_800D5C18();
    f1 = lbl_8047E428;
    r3 = 0x0;
    f2 = f1;
    fn_800D59B8();
    f1 = lbl_8047E44C;
    f2 = lbl_8047E448;
    f3 = lbl_8047E428;
    fn_800D6680();
    r3 = 0x0;
    r4 = 0xff;
    r5 = 0xff;
    r6 = 0xff;
    fn_800D5C18();
    f1 = lbl_8047E450;
    r3 = 0x0;
    f2 = lbl_8047E428;
    fn_800D59B8();
    f1 = lbl_8047E444;
    f2 = lbl_8047E454;
    f3 = lbl_8047E428;
    fn_800D6680();
    r3 = 0x0;
    r4 = 0xff;
    r5 = 0xff;
    r6 = 0xff;
    fn_800D5C18();
    f1 = lbl_8047E428;
    r3 = 0x0;
    f2 = lbl_8047E450;
    fn_800D59B8();
    f1 = lbl_8047E44C;
    f2 = lbl_8047E454;
    f3 = lbl_8047E428;
    fn_800D6680();
    r3 = 0x0;
    r4 = 0xff;
    r5 = 0xff;
    r6 = 0xff;
    fn_800D5C18();
    f1 = lbl_8047E450;
    r3 = 0x0;
    f2 = f1;
    fn_800D59B8();
    fn_800D6728();
    tmp = *(u32*)&lbl_8047B424;
    if ((s32)tmp != 2) {
        if ((s32)tmp < 2) {
            if ((s32)tmp != 0) {
                if ((s32)tmp < 0) {
                    goto L_801E16B8;
                }
                if ((s32)tmp != 4) {
                    if ((s32)tmp >= 4) goto L_801E16B8;
                    goto L_801E1644;
                    }
                r3 = 0x230;
                r4 = 0x2c;
                r5 = -0x1;
                r6 = (u32)lbl_8047E458;
                fn_800FAEF8();
                goto L_801E16B8;
                    }
            r3 = 0x230;
            r4 = 0x2c;
            r5 = -0x1;
            r6 = (u32)lbl_8047E45C;
            fn_800FAEF8();
            goto L_801E16B8;
        }
        r3 = 0x230;
        r4 = 0x2c;
        r5 = -0x1;
        r6 = (u32)lbl_8047E464;
        fn_800FAEF8();
        goto L_801E16B8;
    L_801E1644:
        tmp = *(u32*)&lbl_8047B428;
        if ((s32)tmp < 2) {
            if ((s32)tmp < 0) {
                goto L_801E16B8;
            }
            if ((s32)tmp >= 4) goto L_801E16B8;

            } else {
        r3 = 0x230;
        r4 = 0x2c;
        r5 = -0x1;
        r6 = (u32)lbl_8047E46C;
        fn_800FAEF8();
        goto L_801E16B8;
            }
        r3 = 0x230;
        r4 = 0x2c;
        r5 = -0x1;
        r6 = (u32)lbl_8047E45C;
        fn_800FAEF8();
        goto L_801E16B8;
                }
    r3 = 0x230;
    r4 = 0x2c;
    r5 = -0x1;
    r6 = (u32)lbl_8047E474;
    fn_800FAEF8();
L_801E16B8:
    r3 = 0x0;
    fn_800D9ED8();

    return;
}
#endif

/* 0x801E16D0 | size: 0x20 */
#if 0
asm void fn_801E16D0(void) {
#include "src/game/battle/battle_logic_fn_801E16D0.inc"
}
#else
void fn_801E16D0(void) {
    fn_801E25C8();
}
#endif

/* 0x801E16F0 | size: 0xB8 | medium */
#if 0
asm void fn_801E16F0(void) {
#include "src/game/battle/battle_logic_fn_801E16F0.inc"
}
#else
void fn_801E16F0(void) {
    extern u32 fn_800E202C(u32 arg);
    extern void fn_800E209C(u32 arg);
    extern void fn_800E24B0(void);
    extern void fn_800EE928(void);
    extern void fn_801E386C(void);
    extern s32 fn_801E38D8(void);
    extern void fn_801E3F54(void);
    extern int fn_801E4724(void);
    s32 r3b;
    u32 r3;
    u32 tmp;

    if (lbl_8047B440 == 0) {
        goto false_case;
    }
    if (lbl_8047B441 != 0) {
        goto true_case;
    }
false_case:
    tmp = 0;
    goto test_case;
true_case:
    tmp = 1;
test_case:
    if ((u8)tmp == 0) return;
    fn_801E386C();
    r3b = fn_801E38D8();
    if (r3b == 4) return;
    if (r3b < 4) {
        if (r3b < 3) return;
    } else {
        if (r3b >= 6) return;
    }
    if (lbl_8047B441 == 0) return;
    fn_801E3F54();
    fn_801E4724();
    r3 = fn_800E202C(lbl_8047B450);
    if (r3 & 0xFFFF) {
        fn_800E24B0();
        fn_800E209C(r3);
    }
    lbl_8047B441 = 0;
    fn_800EE928();
}
#endif

/* 0x801E17A8 | size: 0x68 | small */
#if 0
asm void fn_801E17A8(void) {
#include "src/game/battle/battle_logic_fn_801E17A8.inc"
}
#else
void fn_801E17A8(void) {
    extern u32 lbl_80469030[];
    extern u32 lbl_8047B454;
    extern u32 lbl_8047B458;
    extern u32 lbl_8047B45C;
    extern void fn_801E3978(u32, u32, u32, u32, u32);
    u32 tmp;

    if (lbl_8047B440 == 0) {
        goto false_case;
    }
    if (lbl_8047B441 != 0) {
        goto true_case;
    }
false_case:
    tmp = 0;
    goto test_case;
true_case:
    tmp = 1;
test_case:
    if ((u8)tmp != 0) {
        fn_801E3978(lbl_8047B45C, lbl_8047B458, lbl_8047B454, lbl_80469030[0], lbl_80469030[1]);
    }
}
#endif

/* 0x801E1810 | size: 0x64 | small */
#if 0
asm void fn_801E1810(void) {
#include "src/game/battle/battle_logic_fn_801E1810.inc"
}
#else
void fn_801E1810(void) {
    extern u32 lbl_8047B450;
    extern u32 fn_800E202C(u32 arg);
    extern void fn_800E209C(u32 arg);
    extern void fn_800E24B0(void);
    extern void fn_800EE928(void);
    extern void fn_801E3F54(void);
    extern int fn_801E4724(void);
    extern u8 lbl_8047B441;
    u32 r3;

    if (lbl_8047B441 != 0) {
        fn_801E3F54();
        fn_801E4724();
        r3 = fn_800E202C(lbl_8047B450);
        if (r3 & 0xFFFF) {
            fn_800E24B0();
            fn_800E209C(r3);
        }
        lbl_8047B441 = 0;
        fn_800EE928();
    }
}
#endif

/* 0x801E1874 | size: 0x28 */
extern u8 lbl_8047B440;
extern u8 lbl_8047B441;
u32 fn_801E1874(void) {
    if (lbl_8047B440 == 0 || lbl_8047B441 == 0) { return 0; }
    return 1;
}

/* 0x801E189C | size: 0x88 | medium */
#if 0
asm void fn_801E189C(void) {
#include "src/game/battle/battle_logic_fn_801E189C.inc"
}
#else
void fn_801E189C(void) {
    extern u8 lbl_80467D08[];
    extern u8 lbl_80468020[];
    extern u32 lbl_8047B444;
    extern u8 lbl_8047B448[];
    extern void fn_800A19CC();
    extern void fn_800A1F94();
    extern void _threadSwitch();
    extern void fn_801E1924();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;

    r5 = (u32)lbl_80468020;
    r7 = (u32)lbl_80467D08;
    tmp = 0x0;
    r9 = r4 & 0xFF;
    r8 = (u32)lbl_8047B448;
    *(u32*)((u8*)r8 + 0x4) = r9;
    r6 = (u32)fn_801E1924;
    r5 = (u32)lbl_80468020;
    r8 = 0x10;
    lbl_8047B444 = tmp;
    tmp = (u32)lbl_80467D08;
    r4 = (u32)fn_801E1924;
    r6 = r5 + 0xffc;
    *(u32*)lbl_8047B448 = r3;
    r3 = tmp;
    r5 = (u32)lbl_8047B448;
    r7 = 0x1000;
    r9 = 0x1;
    fn_800A19CC();
    r3 = (u32)lbl_80467D08;
    r3 = (u32)lbl_80467D08;
    fn_800A1F94();
    while (1) {
        tmp = lbl_8047B444;
        if ((s32)tmp != 0) break;
        _threadSwitch();


    }
    return;
}
#endif

/* 0x801E1924 | size: 0x208 | large */
#if 0
asm void fn_801E1924(void) {
#include "src/game/battle/battle_logic_fn_801E1924.inc"
}
#else
void fn_801E1924(void) {
    extern u8 lbl_80279A68[];
    extern u8 lbl_80466BC0[];
    extern u8 lbl_80469020[];
    extern u8 lbl_80469030[];
    extern u32 lbl_8047B444;
    extern u32 lbl_8047B450;
    extern u32 lbl_8047B454;
    extern u32 lbl_8047B458;
    extern u32 lbl_8047B45C;
    extern void fn_800E202C();
    extern void fn_800E209C();
    extern void fn_800E24B0();
    extern void fn_800E27B0();
    extern void fn_800E2B00();
    extern void fn_800EE928();
    extern void fn_800EE9BC();
    extern void fn_8014F2DC();
    extern void fn_801E3858();
    extern void fn_801E38E8();
    extern void fn_801E3930();
    extern void fn_801E4058();
    extern void fn_801E40F8();
    extern void fn_801E449C();
    extern int fn_801E4650();
    extern void fn_801E4778();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r4 = (u32)lbl_80279A68;
    r31 = (u32)lbl_80279A68;
    tmp = *(u32*)((u8*)r3 + 0x4);
    r30 = *(u32*)((u8*)r3 + 0x0);
    r29 = tmp & 0xFF;
    fn_800EE9BC();
    r3 = r30;
    r4 = r29;
    fn_801E4778();
    if ((s32)r3 == 0) {
        r4 = r30;
        r3 = r31 + 0x0;
        ((void(*)(void))fn_800DD970)();
        fn_800EE928();
        tmp = 0x1;
        r3 = 0x0;
        lbl_8047B444 = tmp;
        return;
    }
    r3 = (u32)lbl_80469030;
    r3 = (u32)lbl_80469030;
    fn_801E3930();
    r3 = (u32)lbl_80469020;
    r3 = (u32)lbl_80469020;
    fn_801E38E8();
    r4 = (u32)lbl_80466BC0;
    r3 = (u32)lbl_80469030;
    r5 = (u32)lbl_80466BC0;
    r4 = *(u32*)lbl_80469030;
    lbl_8047B45C = r5;
    r3 = *(u32*)((u8*)r3 + 0x4);
    tmp = *(u16*)((u8*)r5 + 0x4);
    tmp = tmp - r4;
    tmp = (u32)tmp >> 1;
    lbl_8047B458 = tmp;
    tmp = *(u16*)((u8*)r5 + 0x6);
    tmp = tmp - r3;
    tmp = (u32)tmp >> 1;
    lbl_8047B454 = tmp;
    fn_801E4650();
    r4 = 0x20;
    fn_800E2B00();
    tmp = r3 & 0xFFFF;
    if ((s32)r3 != 0) {
        fn_800E27B0();
    } else {

        r3 = 0x0;
    }
    lbl_8047B450 = r3;
    if (r3 == 0) {
        r3 = r31 + 0x30;
        ((void(*)(void))fn_800DD970)();
        fn_800EE928();
        tmp = 0x1;
        r3 = 0x0;
        lbl_8047B444 = tmp;
        return;
    }
    fn_801E449C();
    r3 = (u32)lbl_80469020;
    r30 = (u32)lbl_80469020;
    tmp = *(u32*)((u8*)r30 + 0xC);
    if (tmp != 1) {
        OSGetTick();
        r4 = *(u32*)((u8*)r30 + 0xC);
        tmp = (u32)r3 / (u32)r4;
        tmp = tmp * r4;
        r5 = r3 - tmp;
    } else {

        r5 = 0x0;
    }
    r3 = 0x0;
    r4 = 0x0;
    fn_801E40F8();
    if ((s32)r3 == 0) {
        r3 = r31 + 0x5c;
        ((void(*)(void))fn_800DD970)();
        r3 = lbl_8047B450;
        fn_800E202C();
        tmp = r3 & 0xFFFF;
        r30 = r3;
        if ((s32)r3 != 0) {
            fn_800E24B0();
            r3 = r30;
            fn_800E209C();
        }
        fn_800EE928();
        tmp = 0x1;
        r3 = 0x0;
        lbl_8047B444 = tmp;
        return;
    }
    r3 = (u32)sp + 0xc;
    r4 = (u32)sp + 0x8;
    fn_801E3858();
    tmp = r3 + (0x1 << 16);
    if (tmp != 0xffff) {
        r4 = 0x0;
        r5 = 0x0;
        r6 = 0x0;
        r7 = 0x7f;
        r8 = 0x0;
        fn_8014F2DC();
    }
    tmp = r3 + (0x1 << 16);
    if (tmp != 0xffff) {
        r4 = 0x0;
        r5 = 0x7f;
        r6 = 0x0;
        r7 = 0x7f;
        r8 = 0x0;
        fn_8014F2DC();
    }
    fn_801E4058();
    tmp = 0x1;
    r3 = 0x0;
    *(u8*)&lbl_8047B441 = tmp;
    lbl_8047B444 = tmp;

    return;
}
#endif

/* 0x801E1B2C | size: 0x28 */
extern void fn_801E4A6C(void);
#if 0
asm void fn_801E1B2C(void) {
#include "src/game/battle/battle_logic_fn_801E1B2C.inc"
}
#else
void fn_801E1B2C(void) {
    fn_801E4A6C();
    lbl_8047B440 = 1;
}
#endif

/* 0x801E1B54 | size: 0x30 */
extern u8 lbl_8046A3D0[];
extern void fn_8009F230(void*, void*, u32);
void fn_801E1B54(void* val) {
    fn_8009F230(lbl_8046A3D0, val, 1);
}

/* 0x801E1B84 | size: 0x34 */
extern void fn_8009F2F8(void*, void*, u32);
u32 fn_801E1B84(void) {
    u32 result;
    fn_8009F2F8(lbl_8046A3D0, &result, 1);
    return result;
}

/* 0x801E1BB8 | size: 0x30 */
extern u8 lbl_8046A410[];
void fn_801E1BB8(void* val) {
    fn_8009F230(lbl_8046A410, val, 1);
}

/* 0x801E1BE8 | size: 0x34 */
extern u8 lbl_8046A3F0[];
u32 fn_801E1BE8(void) {
    u32 result;
    fn_8009F2F8(lbl_8046A3F0, &result, 1);
    return result;
}

/* 0x801E1C1C | size: 0xF0 | medium */
#if 0
asm void fn_801E1C1C(void) {
#include "src/game/battle/battle_logic_fn_801E1C1C.inc"
}
#else
void fn_801E1C1C(void) {
    extern u8 lbl_80469040[];
    extern u8 lbl_8046AC60[];
    extern s32 fn_8009F2F8(void *, void *, u32);
    extern void fn_8009F230(void *, void *, u32);
    extern s32 fn_800A541C(void *, u32, u32, u32, u32);
    extern void fn_800A221C(void *);
    extern void fn_801E446C(u32);
    u8 *base;
    u8 *base2;
    u8 sp[8];
    u32 counter;
    u32 val_bc;
    u32 val_b8;
    u32 *item;
    u32 result;
    u32 c0val;
    u32 div;
    u32 r3;

    base = lbl_8046AC60;
    base2 = lbl_80469040;
    counter = 0;
    val_b8 = *(u32 *)(base + 0xb8);
    val_bc = *(u32 *)(base + 0xbc);
    while (1) {
        fn_8009F2F8(base2 + 0x13d0, sp, 1);
        item = *(u32 **)sp;
        result = fn_800A541C(base, item[0], val_bc, val_b8, 2);
        if ((s32)result != (s32)val_bc) {
            if ((s32)result == -1) {
                *(u32 *)(base + 0xa8) = (u32)-1;
            }
            if ((s32)counter == 0) {
                fn_801E446C(0);
            }
            fn_800A221C(base2 + 0x1000);
        }
        item[1] = counter;
        fn_8009F230(base2 + 0x13b0, item, 1);
        c0val = *(u32 *)(base + 0xc0);
        val_b8 += val_bc;
        div = *(u32 *)(base + 0x50);
        r3 = (counter + c0val) % div;
        val_bc = *(u32 *)(item[0]);
        if (r3 == div - 1) {
            if (*(u8 *)(base + 0xa6) & 1) {
                val_b8 = *(u32 *)(base + 0x64);
            } else {
                fn_800A221C(base2 + 0x1000);
            }
        }
        counter++;
    }
}
#endif

/* 0x801E1D0C | size: 0x3C | small */
/* Cancel the battle thread if it is active. */
#if 0
asm void fn_801E1D0C(void) {
#include "src/game/battle/battle_logic_fn_801E1D0C.inc"
}
#else
void fn_801E1D0C(void) {
    extern u8 lbl_8046A040[];
    extern u32 lbl_8047B460;

    if ((s32)lbl_8047B460 != 0) {
        OSCancelThread((void*)lbl_8046A040);
        lbl_8047B460 = 0;
    }
}
#endif

/* 0x801E1D48 | size: 0x34
 * Resume the battle thread if it is active.
 */
#if 0
asm void fn_801E1D48(void) {
#include "src/game/battle/battle_logic_fn_801E1D48.inc"
}
#else
void fn_801E1D48(void) {
    extern u8 lbl_8046A040[];
    extern u32 lbl_8047B460;
    extern void fn_800A1F94(void* thread);

    if ((s32)lbl_8047B460 != 0) {
        fn_800A1F94((void*)lbl_8046A040);
    }
}
#endif

/* 0x801E1D7C | size: 0xA0 | medium */
#if 0
asm void fn_801E1D7C(void) {
#include "src/game/battle/battle_logic_fn_801E1D7C.inc"
}
#else
void fn_801E1D7C(void) {
    extern u8 lbl_80469040[];
    extern u32 lbl_8047B460;
    extern void fn_8009F1D0();
    extern void fn_800A19CC();
    extern void fn_801E1C1C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r31 = 0;

    r4 = (u32)fn_801E1C1C;
    r5 = (u32)lbl_80469040;
    r4 = (u32)fn_801E1C1C;
    r8 = r3;
    r7 = 0x1000;
    r31 = (u32)lbl_80469040;
    r6 = r31 + 0x0;
    r5 = 0x0;
    r3 = r31 + 0x1000;
    r9 = 0x1;
    r6 = r6 + 0x1000;
    fn_800A19CC();
    if ((s32)r3 == 0) {
        r3 = 0x0;
    } else {

        r3 = r31 + 0x13d0;
        r4 = r31 + 0x1368;
        r5 = 0xa;
        fn_8009F1D0();
        r3 = r31 + 0x13b0;
        r4 = r31 + 0x1340;
        r5 = 0xa;
        fn_8009F1D0();
        r3 = r31 + 0x1390;
        r4 = r31 + 0x1318;
        r5 = 0xa;
        fn_8009F1D0();
        tmp = 0x1;
        r3 = 0x1;
        lbl_8047B460 = tmp;
    }
    return;
}
#endif

/* 0x801E1E1C | size: 0x1DC | medium */
#if 0
asm void fn_801E1E1C(void) {
#include "src/game/battle/battle_logic_fn_801E1E1C.inc"
}
#else
void fn_801E1E1C(void) {
    extern f32 lbl_8047E494;
    extern void fn_800B928C();
    extern void fn_800BA9E4();
    extern void fn_800BACA0();
    extern void fn_800BAFFC();
    u8 sp[0x90];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;

    r24 = r4;
    r25 = r5;
    r30 = r6;
    r31 = r7;
    r26 = r10;
    r4 = r3;
    r27 = *(s16*)((u8*)(u32)sp + 0x9A);
    r29 = r8;
    r5 = r8 & 0xFFFF;
    r28 = r9;
    r6 = r9 & 0xFFFF;
    r3 = (u32)sp + 0x48;
    r7 = 0x1;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_800BA9E4();
    f1 = lbl_8047E494;
    r3 = (u32)sp + 0x48;
    r4 = 0x0;
    r5 = 0x0;
    f2 = f1;
    r6 = 0x0;
    f3 = f1;
    r7 = 0x0;
    r8 = 0x0;
    fn_800BACA0();
    r3 = (u32)sp + 0x48;
    r4 = 0x0;
    fn_800BAFFC();
    r3 = (s16)r29;
    tmp = (s16)r28;
    r28 = (s32)r3 >> 1;
    r4 = r24;
    r29 = (s32)tmp >> 1;
    r3 = (u32)sp + 0x28;
    r5 = r28 & 0xFFFF;
    r7 = 0x1;
    r6 = r29 & 0xFFFF;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_800BA9E4();
    f1 = lbl_8047E494;
    r3 = (u32)sp + 0x28;
    r4 = 0x0;
    r5 = 0x0;
    f2 = f1;
    r6 = 0x0;
    f3 = f1;
    r7 = 0x0;
    r8 = 0x0;
    fn_800BACA0();
    r3 = (u32)sp + 0x28;
    r4 = 0x1;
    fn_800BAFFC();
    r4 = r25;
    r3 = (u32)sp + 0x8;
    r5 = r28 & 0xFFFF;
    r6 = r29 & 0xFFFF;
    r7 = 0x1;
    r8 = 0x0;
    r9 = 0x0;
    r10 = 0x0;
    fn_800BA9E4();
    f1 = lbl_8047E494;
    r3 = (u32)sp + 0x8;
    r4 = 0x0;
    r5 = 0x0;
    f2 = f1;
    r6 = 0x0;
    f3 = f1;
    r7 = 0x0;
    r8 = 0x0;
    fn_800BACA0();
    r3 = (u32)sp + 0x8;
    r4 = 0x2;
    fn_800BAFFC();
    r3 = 0x80;
    r4 = 0x7;
    r5 = 0x4;
    fn_800B928C();
    r4 = 0xCC010000;
    tmp = (s16)r31;
    *(u16*)((u8*)r4 + (-32768)) = r30;
    r6 = tmp + r27;
    r3 = 0x0;
    r5 = (s16)r30;
    *(u16*)((u8*)r4 + (-32768)) = r31;
    tmp = (s16)r26;
    r5 = r5 + tmp;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    tmp = 0x1;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = r5;
    *(u16*)((u8*)r4 + (-32768)) = r31;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = tmp;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = r5;
    *(u16*)((u8*)r4 + (-32768)) = r6;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = tmp;
    *(u16*)((u8*)r4 + (-32768)) = tmp;
    *(u16*)((u8*)r4 + (-32768)) = r30;
    *(u16*)((u8*)r4 + (-32768)) = r6;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = r3;
    *(u16*)((u8*)r4 + (-32768)) = tmp;
    return;
}
#endif

/* 0x801E1FF8 | size: 0x4B8 | large */
#if 0
asm void fn_801E1FF8(void) {
#include "src/game/battle/battle_logic_fn_801E1FF8.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801E1FF8(void *arg0) {
    extern u32 lbl_8047E480;
    extern u32 lbl_8047E484;
    extern u32 lbl_8047E488;
    extern u32 lbl_8047E48C;
    extern u32 lbl_8047E490;
    extern f32 lbl_8047E494;
    extern f32 lbl_8047E498;
    extern f32 lbl_8047E49C;
    extern void fn_800A2D38();
    extern void fn_800A39E0(void *, f32, f32, f32, f32, f32, f32);
    extern void fn_800B7874();
    extern void fn_800B7D3C();
    extern void fn_800B7D74();
    extern void fn_800B857C();
    extern void fn_800B884C();
    extern void fn_800B9E6C();
    extern void fn_800BA6B0();
    extern void fn_800BB29C();
    extern void fn_800BC1A0();
    extern void fn_800BC1E4();
    extern void fn_800BC228();
    extern void fn_800BC290();
    extern void fn_800BC36C();
    extern void fn_800BC3E0();
    extern void fn_800BC454();
    extern void fn_800BC4C0();
    extern void fn_800BC52C();
    extern void fn_800BC580();
    extern void fn_800BC6F0();
    extern void fn_800BC8C8();
    extern void fn_800BCDDC();
    extern void fn_800BCE30();
    extern void fn_800BCE5C();
    extern void fn_800BCE88();
    extern void fn_800BCEF4();
    extern void fn_800BD2E0();
    extern void fn_800BD4B4();
    extern void fn_800BD554();
    extern void fn_800BD744(f32, f32, f32, f32, f32, f32);
    extern void fn_800BD7A0();
    u32 v0;
    u32 v1;
    u32 v2;
    u32 pair[2];
    u8 mtxB[0x30];
    u8 mtxA[0x44];
    int field4;
    int field6;

    field4 = *(u16 *)((u8 *)arg0 + 4);
    field6 = *(u16 *)((u8 *)arg0 + 6);
    fn_800BCEF4(0, 0);
    fn_800A39E0(mtxA, lbl_8047E494, (f32)field6, lbl_8047E494, (f32)field4, lbl_8047E494,
                lbl_8047E498);
    fn_800BD2E0(mtxA, 1);
    fn_800BD744(lbl_8047E494, lbl_8047E494, (f32)field4, (f32)field6, lbl_8047E494,
                lbl_8047E49C);
    fn_800BD7A0(0, 0, field4, field6);
    fn_800A2D38(mtxB);
    fn_800BD4B4(mtxB, 0);
    fn_800BD554(0);
    fn_800BCE88(1, 7, 0);
    fn_800BCDDC(0, 1, 0, 0);
    fn_800BCE30(1);
    fn_800BCE5C(0);
    fn_800B9E6C(0);
    fn_800BA6B0(0);
    fn_800B884C(2);
    fn_800B857C(0, 1, 4, 0x3c, 0, 0x7d);
    fn_800B857C(1, 1, 4, 0x3c, 0, 0x7d);
    fn_800BB29C();
    fn_800B7D3C();
    fn_800B7874(9, 1);
    fn_800B7874(0xd, 1);
    fn_800B7D74(7, 9, 1, 3, 0);
    fn_800B7D74(7, 0xd, 1, 2, 0);
    fn_800BC8C8(4);
    fn_800BC6F0(0, 1, 1, 0xff);
    fn_800BC1A0(0, 0xf, 8, 0xe, 2);
    fn_800BC228(0, 0, 0, 0, 0, 0);
    fn_800BC1E4(0, 7, 4, 6, 1);
    fn_800BC290(0, 1, 0, 0, 0, 0);
    fn_800BC454(0, 0xc);
    fn_800BC4C0(0, 0x1c);
    fn_800BC52C(0, 0, 0);
    fn_800BC6F0(1, 1, 2, 0xff);
    fn_800BC1A0(1, 0xf, 8, 0xe, 0);
    fn_800BC228(1, 0, 0, 1, 0, 0);
    fn_800BC1E4(1, 7, 4, 6, 0);
    fn_800BC290(1, 1, 0, 0, 0, 0);
    fn_800BC454(1, 0xd);
    fn_800BC4C0(1, 0x1d);
    fn_800BC52C(1, 0, 0);
    fn_800BC6F0(2, 0, 0, 0xff);
    fn_800BC1A0(2, 0xf, 8, 0xc, 0);
    fn_800BC228(2, 0, 0, 0, 1, 0);
    fn_800BC1E4(2, 4, 7, 7, 0);
    fn_800BC290(2, 0, 0, 0, 1, 0);
    fn_800BC52C(2, 0, 0);
    fn_800BC6F0(3, 0xff, 0xff, 0xff);
    fn_800BC1A0(3, 1, 0, 0xe, 0xf);
    fn_800BC228(3, 0, 0, 0, 1, 0);
    fn_800BC1E4(3, 7, 7, 7, 7);
    fn_800BC290(3, 0, 0, 0, 1, 0);
    fn_800BC52C(3, 0, 0);
    fn_800BC454(3, 0xe);
    pair[0] = lbl_8047E480;
    pair[1] = lbl_8047E484;
    fn_800BC36C(1, pair);
    v0 = lbl_8047E488;
    fn_800BC3E0(0, &v0);
    v1 = lbl_8047E48C;
    fn_800BC3E0(1, &v1);
    v2 = lbl_8047E490;
    fn_800BC3E0(2, &v2);
    fn_800BC580(0, 0, 1, 2, 3);
}
#pragma pop
#endif

/* 0x801E24B0 | size: 0x118 | medium */
#if 0
asm void fn_801E24B0(void) {
#include "src/game/battle/battle_logic_fn_801E24B0.inc"
}
#else
void fn_801E24B0(void) {
    extern void fn_800B884C(u32);
    extern void fn_800BA6B0(u32);
    extern void fn_800BC114(u32, u32);
    extern void fn_800BC52C(u32, u32, u32);
    extern void fn_800BC580(u32, u32, u32, u32, u32);
    extern void fn_800BC6F0(u32, u32, u32, u32);
    extern void fn_800BC8C8(u32);
    extern void fn_800BCDDC(u32, u32, u32, u32);
    extern void fn_800BCE88(u32, u32, u32);
    fn_800BCE88(1, 7, 0);
    fn_800BCDDC(0, 1, 0, 0xf);
    fn_800B884C(1);
    fn_800BA6B0(0);
    fn_800BC8C8(1);
    fn_800BC6F0(0, 0, 0, 0xff);
    fn_800BC114(0, 3);
    fn_800BC52C(0, 0, 0);
    fn_800BC52C(1, 0, 0);
    fn_800BC52C(2, 0, 0);
    fn_800BC52C(3, 0, 0);
    fn_800BC580(0, 0, 1, 2, 3);
    fn_800BC580(1, 0, 0, 0, 3);
    fn_800BC580(2, 1, 1, 1, 3);
    fn_800BC580(3, 2, 2, 2, 3);
}
#endif

/* 0x801E25C8 | size: 0x44 | small */
#if 0
asm void fn_801E25C8(void) {
#include "src/game/battle/battle_logic_fn_801E25C8.inc"
}
#else
s32 fn_801E25C8(void) {
    extern u8 lbl_8046AC60[];
    u8 *base = lbl_8046AC60;
    u8 *ptr;
    if ((s32)*(u32*)(base + 0xa0) != 0) {
        if (*(u8*)(base + 0xa4) != 0) {
            ptr = *(u8**)(base + 0xe8);
            if (ptr != 0) {
                return *(u32*)(ptr + 0xc) + *(u32*)(base + 0xc0);
            }
        }
    }
    return -1;
}
#endif

/* 0x801E260C | size: 0x568 | large */
#if 0
asm void fn_801E260C(void) {
#include "src/game/battle/battle_logic_fn_801E260C.inc"
}
#else
void fn_801E260C(void) {
    extern u8 lbl_8046A440[];
    extern u8 lbl_8046AC60[];
    extern u32 lbl_80478D00;
    extern u32 lbl_80478D04;
    extern u32 lbl_8047B470;
    extern u32 lbl_8047B474;
    extern void fn_8014E9B4();
    extern void fn_801E2B74();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r5 = (u32)lbl_8046AC60;
    r5 = (u32)lbl_8046AC60;
    tmp = 0x3e8;
    r7 = *(u32*)((u8*)r5 + 0x90);
    r7 = r7 * 0x28;
    r28 = (u32)r7 / (u32)tmp;
    if (r7 == 0) { r3 = 0x0; return; }
    r7 = (u32)lbl_8046A440;
    r8 = 0x0;
    r31 = (u32)lbl_8046A440;
    r7 = *(u32*)((u8*)r31 + 0x34);
    tmp = *(u32*)((u8*)r31 + 0x30);
    r7 = r7 + r4;
    r10 = r7 + r6;
    *(u32*)((u8*)r31 + 0x3C) = r10;
    *(u32*)((u8*)r31 + 0x38) = r9;
    while (1) {
        r7 = *(u32*)((u8*)r31 + 0x28);
        tmp = *(u32*)((u8*)r31 + 0x2C);
        if ((s32)r7 == (s32)tmp) break;
        tmp = r7 << 3;
        r7 = r31 + tmp;
        tmp = *(u32*)((u8*)r7 + 0x4);
        r7 = *(u32*)((u8*)r7 + 0x0);
        tmp = r10 - tmp;
        tmp = r9 - r7; /* -borrow */;
        tmp = r3 - r3; /* -borrow */;
        /* neg. tmp, tmp */;
        if (r7 != 0) break;
        r7 = *(u32*)((u8*)r31 + 0x28);
        tmp = r7 + 0x1;
        *(u32*)((u8*)r31 + 0x28) = tmp;
        if ((s32)tmp >= 5) {
            *(u32*)((u8*)r31 + 0x28) = r8;
        }
        r7 = *(u32*)((u8*)r5 + 0xE4);
        tmp = r7 + 0x1;
        *(u32*)((u8*)r5 + 0xE4) = tmp;


    }

    tmp = r4 + r6;
    r30 = (u32)r28 >> 1;
    if (tmp < r30) { r3 = 0x0; return; }
    r29 = lbl_8047B470;
    if (r3 == r29) {
        r3 = (u32)lbl_8046AC60;
        r27 = (u32)lbl_8046AC60;
        tmp = *(u32*)((u8*)r27 + 0x8C);
        if (tmp == 2) {
            r3 = (u32)lbl_8046A440;
            r25 = lbl_8047B474;
            r26 = (u32)lbl_8046A440;
            r24 = r30;
            r23 = *(u32*)((u8*)r26 + 0x40);
            r22 = *(u32*)((u8*)r26 + 0x44);
        L_801E270C:
            r3 = r29;
            r4 = r25;
            r5 = r24;
            r6 = (u32)sp + 0x14;
            fn_801E2B74();
            r22 = r22 + r3;
            do {
                if ((s32)tmp == 0) break;
                if ((s32)tmp == 1) {
                    tmp = r3 << 1;
                    r24 = r24 - r3;
                    r29 = r29 + tmp;
                    if (r25 != 0) {
                        r25 = r25 + tmp;
                    }
                    tmp = *(u32*)((u8*)r31 + 0x2C);
                    tmp = tmp << 3;
                    r3 = r26 + tmp;
                    *(u32*)((u8*)r3 + 0x4) = r22;
                    *(u32*)((u8*)r3 + 0x0) = r23;
                    r3 = *(u32*)((u8*)r31 + 0x2C);
                    tmp = r3 + 0x1;
                    *(u32*)((u8*)r31 + 0x2C) = tmp;
                    if ((s32)tmp < 5) goto L_801E270C;
                    tmp = 0x0;
                    *(u32*)((u8*)r31 + 0x2C) = tmp;
                    goto L_801E270C;
                }
                r21 = r24 << 1;
                r3 = r29;
                r5 = r21;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
                if (r25 == 0) break;
                r3 = r25;
                r5 = r21;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
            } while (0);
            r3 = *(u32*)((u8*)r26 + 0x44);
            tmp = *(u32*)((u8*)r26 + 0x40);
            r3 = r3 + r30;
            *(u32*)((u8*)r26 + 0x44) = r3;
            *(u32*)((u8*)r26 + 0x40) = tmp;
            goto L_801E28A8;
        }
        r3 = (u32)lbl_8046A440;
        r23 = r30;
        r26 = (u32)lbl_8046A440;
        r22 = 0x0;
        r24 = *(u32*)((u8*)r26 + 0x40);
        r25 = *(u32*)((u8*)r26 + 0x44);
    L_801E27E8:
        r3 = r29;
        r4 = r22;
        r5 = r23;
        r6 = (u32)sp + 0x10;
        fn_801E2B74();
        r25 = r25 + r3;
        do {
            if ((s32)tmp == 0) break;
            if ((s32)tmp == 1) {
                tmp = r3 << 1;
                r23 = r23 - r3;
                r29 = r29 + tmp;
                if (r22 != 0) {
                    r22 = r22 + tmp;
                }
                tmp = *(u32*)((u8*)r31 + 0x2C);
                tmp = tmp << 3;
                r3 = r26 + tmp;
                *(u32*)((u8*)r3 + 0x4) = r25;
                *(u32*)((u8*)r3 + 0x0) = r24;
                r3 = *(u32*)((u8*)r31 + 0x2C);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                if ((s32)tmp < 5) goto L_801E27E8;
                tmp = 0x0;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                goto L_801E27E8;
            }
            r21 = r23 << 1;
            r3 = r29;
            r5 = r21;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r22 == 0) break;
            r3 = r22;
            r5 = r21;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
        } while (0);
        r3 = *(u32*)((u8*)r26 + 0x44);
        tmp = *(u32*)((u8*)r26 + 0x40);
        r3 = r3 + r30;
        *(u32*)((u8*)r26 + 0x44) = r3;
        *(u32*)((u8*)r26 + 0x40) = tmp;
    L_801E28A8:
        r3 = lbl_8047B470;
        r4 = r28;
        DCFlushRange((void*)r3, r4);
        r3 = lbl_80478D00;
        r5 = r30;
        r4 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        fn_8014E9B4();
        tmp = *(u32*)((u8*)r27 + 0x8C);
        if (tmp == 2) {
            r3 = lbl_8047B474;
            r4 = r28;
            DCFlushRange((void*)r3, r4);
            r3 = lbl_80478D04;
            r5 = r30;
            r4 = 0x0;
            r6 = 0x0;
            r7 = 0x0;
            fn_8014E9B4();
        }
        goto L_801E2B3C;
    }
    r3 = (u32)lbl_8046AC60;
    r26 = (u32)lbl_8046AC60;
    tmp = *(u32*)((u8*)r26 + 0x8C);
    if (tmp == 2) {
        r3 = (u32)lbl_8046A440;
        r4 = r30 << 1;
        r27 = (u32)lbl_8046A440;
        tmp = lbl_8047B474;
        r25 = *(u32*)((u8*)r27 + 0x40);
        r24 = r30;
        r21 = *(u32*)((u8*)r27 + 0x44);
        r23 = r29 + r4;
        r22 = tmp + r4;
    L_801E2938:
        r3 = r23;
        r4 = r22;
        r5 = r24;
        r6 = (u32)sp + 0xc;
        fn_801E2B74();
        r21 = r21 + r3;
        do {
            if ((s32)tmp == 0) break;
            if ((s32)tmp == 1) {
                tmp = r3 << 1;
                r24 = r24 - r3;
                r23 = r23 + tmp;
                if (r22 != 0) {
                    r22 = r22 + tmp;
                }
                tmp = *(u32*)((u8*)r31 + 0x2C);
                tmp = tmp << 3;
                r3 = r27 + tmp;
                *(u32*)((u8*)r3 + 0x4) = r21;
                *(u32*)((u8*)r3 + 0x0) = r25;
                r3 = *(u32*)((u8*)r31 + 0x2C);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                if ((s32)tmp < 5) goto L_801E2938;
                tmp = 0x0;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                goto L_801E2938;
            }
            r21 = r24 << 1;
            r3 = r23;
            r5 = r21;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r22 == 0) break;
            r3 = r22;
            r5 = r21;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
        } while (0);
        r3 = *(u32*)((u8*)r27 + 0x44);
        tmp = *(u32*)((u8*)r27 + 0x40);
        r3 = r3 + r30;
        *(u32*)((u8*)r27 + 0x44) = r3;
        *(u32*)((u8*)r27 + 0x40) = tmp;
        goto L_801E2ADC;
    }
    r3 = (u32)lbl_8046A440;
    tmp = r30 << 1;
    r27 = (u32)lbl_8046A440;
    r21 = r30;
    r22 = *(u32*)((u8*)r27 + 0x40);
    r24 = r29 + tmp;
    r25 = *(u32*)((u8*)r27 + 0x44);
    r23 = 0x0;
L_801E2A1C:
    r3 = r24;
    r4 = r23;
    r5 = r21;
    r6 = (u32)sp + 0x8;
    fn_801E2B74();
    r25 = r25 + r3;
    do {
        if ((s32)tmp == 0) break;
        if ((s32)tmp == 1) {
            tmp = r3 << 1;
            r21 = r21 - r3;
            r24 = r24 + tmp;
            if (r23 != 0) {
                r23 = r23 + tmp;
            }
            tmp = *(u32*)((u8*)r31 + 0x2C);
            tmp = tmp << 3;
            r3 = r27 + tmp;
            *(u32*)((u8*)r3 + 0x4) = r25;
            *(u32*)((u8*)r3 + 0x0) = r22;
            r3 = *(u32*)((u8*)r31 + 0x2C);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x2C) = tmp;
            if ((s32)tmp < 5) goto L_801E2A1C;
            tmp = 0x0;
            *(u32*)((u8*)r31 + 0x2C) = tmp;
            goto L_801E2A1C;
        }
        r21 = r21 << 1;
        r3 = r24;
        r5 = r21;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
        if (r23 == 0) break;
        r3 = r23;
        r5 = r21;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
    } while (0);
    r3 = *(u32*)((u8*)r27 + 0x44);
    tmp = *(u32*)((u8*)r27 + 0x40);
    r3 = r3 + r30;
    *(u32*)((u8*)r27 + 0x44) = r3;
    *(u32*)((u8*)r27 + 0x40) = tmp;
L_801E2ADC:
    tmp = lbl_8047B470;
    r21 = r30 << 1;
    r4 = r28;
    r3 = tmp + r21;
    DCFlushRange((void*)r3, r4);
    r3 = lbl_80478D00;
    r4 = r30;
    r5 = r30;
    r6 = 0x0;
    r7 = 0x0;
    fn_8014E9B4();
    tmp = *(u32*)((u8*)r26 + 0x8C);
    if (tmp == 2) {
        tmp = lbl_8047B474;
        r4 = r30;
        r3 = tmp + r21;
        DCFlushRange((void*)r3, r4);
        r3 = lbl_80478D04;
        r4 = r30;
        r5 = r30;
        r6 = 0x0;
        r7 = 0x0;
        fn_8014E9B4();
    }
L_801E2B3C:
    r4 = *(u32*)((u8*)r31 + 0x34);
    r3 = r30;
    tmp = *(u32*)((u8*)r31 + 0x30);
    r4 = r4 + r30;
    *(u32*)((u8*)r31 + 0x34) = r4;
    *(u32*)((u8*)r31 + 0x30) = tmp;
    return;


}
#endif

/* 0x801E2B74 | size: 0x134 | medium */
#if 0
asm void fn_801E2B74(void) {
#include "src/game/battle/battle_logic_fn_801E2B74.inc"
}
#else
void fn_801E2B74(void) {
    extern u8 lbl_8046AC60[];
    extern void fn_801E4AC4();
    extern void fn_801E4B08();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r7 = (u32)lbl_8046AC60;
    r29 = (u32)lbl_8046AC60;
    r26 = r3;
    r27 = r4;
    r28 = r5;
    r30 = r6;
    tmp = *(u32*)((u8*)r29 + 0xEC);
    if (tmp == 0) {
        r3 = 0x0;
        fn_801E4AC4();
        *(u32*)((u8*)r29 + 0xEC) = r3;
        if (r3 == 0) {
            tmp = 0x2;
            r3 = 0x0;
            *(u32*)((u8*)r30 + 0x0) = tmp;
            return;
    }
    }
    r3 = *(u32*)((u8*)r29 + 0xEC);
    tmp = *(u32*)((u8*)r3 + 0x8);
    if (tmp == 0) { r3 = r31; return; }
    if (tmp >= r28) {
        tmp = r28;
    }
    r4 = *(u32*)((u8*)r3 + 0x4);
    r31 = tmp;
    if (r27 == 0) {
        ctr_fn = (void(*)(void))tmp;
        if (tmp > 0) {
            do {
                tmp = *(s16*)((u8*)r4 + 0x2);
                r4 = r4 + 0x4;
                *(u16*)((u8*)r26 + 0x0) = tmp;
                r26 = r26 + 0x2;
            } while (--ctr != 0);
        }
    } else {
        ctr_fn = (void(*)(void))tmp;
        if (tmp > 0) {
            do {
                tmp = *(s16*)((u8*)r4 + 0x0);
                *(u16*)((u8*)r27 + 0x0) = tmp;
                r27 = r27 + 0x2;
                tmp = *(s16*)((u8*)r4 + 0x2);
                r4 = r4 + 0x4;
                *(u16*)((u8*)r26 + 0x0) = tmp;
                r26 = r26 + 0x2;
            } while (--ctr != 0);
        }
    }
    r3 = *(u32*)((u8*)r29 + 0xEC);
    tmp = *(u32*)((u8*)r3 + 0x8);
    tmp = tmp - r31;
    *(u32*)((u8*)r3 + 0x8) = tmp;
    r3 = *(u32*)((u8*)r29 + 0xEC);
    *(u32*)((u8*)r3 + 0x4) = r4;
    r3 = *(u32*)((u8*)r29 + 0xEC);
    tmp = *(u32*)((u8*)r3 + 0x8);
    if (tmp == 0) {
        fn_801E4B08();
        r3 = 0x0;
        tmp = 0x1;
        *(u32*)((u8*)r29 + 0xEC) = r3;
        *(u32*)((u8*)r30 + 0x0) = tmp;
        r3 = r31;
        return;
    }
    tmp = 0x0;
    *(u32*)((u8*)r30 + 0x0) = tmp;

    r3 = r31;

    return;
}
#endif

/* 0x801E2CA8 | size: 0x848 | massive */
#if 0
asm void fn_801E2CA8(void) {
#include "src/game/battle/battle_logic_fn_801E2CA8.inc"
}
#else
void fn_801E2CA8(void) {
    extern u8 lbl_8046A440[];
    extern u32 lbl_80478D00;
    extern u32 lbl_80478D04;
    extern u32 lbl_8047B470;
    extern u32 lbl_8047B474;
    extern void fn_8014E9B4();
    extern void fn_801E2B74();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r17 = 0;
    u32 r18 = 0;
    u32 r19 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r3 = (u32)lbl_8046A440;
    r5 = 0x3e8;
    r30 = (u32)lbl_8046A440;
    r31 = r30 + 0x0;
    r7 = r30 + 0x820;
    tmp = *(u32*)((u8*)r7 + 0x90);
    r3 = *(u32*)((u8*)r31 + 0x38);
    r6 = tmp * 0x28;
    r18 = *(u32*)((u8*)r31 + 0x40);
    r4 = *(u32*)((u8*)r31 + 0x3C);
    r19 = *(u32*)((u8*)r31 + 0x44);
    tmp = r3 ^ r18;
    r29 = (u32)r6 / (u32)r5;
    r5 = r4 ^ r19;
    /* or. tmp, r5, tmp */;
    if ((s32)tmp == 0) {
        tmp = *(u32*)((u8*)r7 + 0x8C);
        r17 = 0x0;
        *(u32*)((u8*)r31 + 0x44) = r17;
        *(u32*)((u8*)r31 + 0x40) = r17;
        if (tmp == 2) {
            r20 = 0x0;
            r18 = lbl_8047B470;
            r17 = lbl_8047B474;
            r19 = r29;
            r21 = r20;
        L_801E2D24:
            r3 = r18;
            r4 = r17;
            r5 = r19;
            r6 = (u32)sp + 0x1c;
            fn_801E2B74();
            r21 = r21 + r3;
            do {
                if ((s32)tmp == 0) break;
                if ((s32)tmp == 1) {
                    tmp = r3 << 1;
                    r19 = r19 - r3;
                    r18 = r18 + tmp;
                    if (r17 != 0) {
                        r17 = r17 + tmp;
                    }
                    tmp = *(u32*)((u8*)r31 + 0x2C);
                    tmp = tmp << 3;
                    r3 = r31 + tmp;
                    *(u32*)((u8*)r3 + 0x4) = r21;
                    *(u32*)((u8*)r3 + 0x0) = r20;
                    r3 = *(u32*)((u8*)r31 + 0x2C);
                    tmp = r3 + 0x1;
                    *(u32*)((u8*)r31 + 0x2C) = tmp;
                    if ((s32)tmp < 5) goto L_801E2D24;
                    tmp = 0x0;
                    *(u32*)((u8*)r31 + 0x2C) = tmp;
                    goto L_801E2D24;
                }
                r19 = r19 << 1;
                r3 = r18;
                r5 = r19;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
                if (r17 == 0) break;
                r3 = r17;
                r5 = r19;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
            } while (0);
            r3 = *(u32*)((u8*)r31 + 0x44);
            tmp = *(u32*)((u8*)r31 + 0x40);
            r3 = r3 + r29;
            *(u32*)((u8*)r31 + 0x44) = r3;
            *(u32*)((u8*)r31 + 0x40) = tmp;
            goto L_801E3468;
        }
        r20 = 0x0;
        r18 = lbl_8047B470;
        r19 = r29;
        r21 = r20;
    L_801E2DF8:
        r3 = r18;
        r4 = r17;
        r5 = r19;
        r6 = (u32)sp + 0x18;
        fn_801E2B74();
        r21 = r21 + r3;
        do {
            if ((s32)tmp == 0) break;
            if ((s32)tmp == 1) {
                tmp = r3 << 1;
                r19 = r19 - r3;
                r18 = r18 + tmp;
                if (r17 != 0) {
                    r17 = r17 + tmp;
                }
                tmp = *(u32*)((u8*)r31 + 0x2C);
                tmp = tmp << 3;
                r3 = r31 + tmp;
                *(u32*)((u8*)r3 + 0x4) = r21;
                *(u32*)((u8*)r3 + 0x0) = r20;
                r3 = *(u32*)((u8*)r31 + 0x2C);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                if ((s32)tmp < 5) goto L_801E2DF8;
                tmp = 0x0;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                goto L_801E2DF8;
            }
            r19 = r19 << 1;
            r3 = r18;
            r5 = r19;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r17 == 0) break;
            r3 = r17;
            r5 = r19;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
        } while (0);
        r3 = *(u32*)((u8*)r31 + 0x44);
        tmp = *(u32*)((u8*)r31 + 0x40);
        r3 = r3 + r29;
        *(u32*)((u8*)r31 + 0x44) = r3;
        *(u32*)((u8*)r31 + 0x40) = tmp;
        goto L_801E3468;
    }
    r24 = r29;
    r6 = r29;
    r5 = 0x0;
    __mod2u(r3, r4);
    r28 = r4;
    r3 = r18;
    r4 = r19;
    r6 = r24;
    r5 = 0x0;
    __mod2u(r3, r4);
    /* mr. r26, r4 */;
    if (r17 == 0) {
        r26 = r29;
    }
    if (r28 < r26) {
        tmp = r26 - r28;
        r3 = lbl_8047B470;
        r17 = r28 << 1;
        r19 = tmp << 1;
        r4 = r3 + r17;
        r5 = r19;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r18 = r30 + 0x820;
        tmp = *(u32*)((u8*)r18 + 0x8C);
        if (tmp == 2) {
            r3 = lbl_8047B474;
            r5 = r19;
            r4 = r3 + r17;
            memcpy((void*)r3, (const void*)r4, (u32)r5);
        }
        r17 = r30 + 0x0;
        r21 = 0x0;
        r20 = *(u32*)((u8*)r17 + 0x28);
        while ((s32)r20 != (s32)tmp) {
                tmp = *(u32*)((u8*)r17 + 0x2C);
                tmp = r20 << 3;
                r22 = r30 + 0x0;
                r22 = r22 + tmp;
                r6 = r24;
                r3 = *(u32*)((u8*)r22 + 0x0);
                r5 = 0x0;
                r4 = *(u32*)((u8*)r22 + 0x4);
                __mod2u(r3, r4);
                tmp = r4 - r28;
                r20 = r20 + 0x1;
                *(u32*)((u8*)r22 + 0x4) = tmp;
                tmp = r3 - r21; /* -borrow */;
                *(u32*)((u8*)r22 + 0x0) = tmp;
                if ((s32)r20 >= 5) {
                    r20 = 0x0;
                }
        }
        r3 = (u32)r19 >> 1;
        tmp = *(u32*)((u8*)r18 + 0x8C);
        r20 = r29 - r3;
        r21 = 0x0;
        r18 = r29 - r20;
        tmp = lbl_8047B470;
        /* clrrwi r3, r19, 1 */;
        *(u32*)((u8*)r31 + 0x44) = r18;
        r4 = tmp + r3;
        *(u32*)((u8*)r31 + 0x40) = r21;
        if (tmp == 2) {
            tmp = lbl_8047B474;
            r23 = r20;
            r22 = r4;
            r19 = r30 + 0x0;
            r21 = tmp + r3;
            r24 = 0x0;
        L_801E2FD8:
            r3 = r22;
            r4 = r21;
            r5 = r23;
            r6 = (u32)sp + 0x14;
            fn_801E2B74();
            r18 = r18 + r3;
            do {
                if ((s32)tmp == 0) break;
                if ((s32)tmp == 1) {
                    tmp = r3 << 1;
                    r23 = r23 - r3;
                    r22 = r22 + tmp;
                    if (r21 != 0) {
                        r21 = r21 + tmp;
                    }
                    tmp = *(u32*)((u8*)r17 + 0x2C);
                    tmp = tmp << 3;
                    r3 = r19 + tmp;
                    *(u32*)((u8*)r3 + 0x4) = r18;
                    *(u32*)((u8*)r3 + 0x0) = r24;
                    r3 = *(u32*)((u8*)r17 + 0x2C);
                    tmp = r3 + 0x1;
                    *(u32*)((u8*)r17 + 0x2C) = tmp;
                    if ((s32)tmp < 5) goto L_801E2FD8;
                    tmp = 0x0;
                    *(u32*)((u8*)r17 + 0x2C) = tmp;
                    goto L_801E2FD8;
                }
                r17 = r23 << 1;
                r3 = r22;
                r5 = r17;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
                if (r21 == 0) break;
                r3 = r21;
                r5 = r17;
                r4 = 0x0;
                memset((void*)r3, (int)r4, (u32)r5);
            } while (0);
            r3 = *(u32*)((u8*)r31 + 0x44);
            tmp = *(u32*)((u8*)r31 + 0x40);
            r3 = r3 + r20;
            *(u32*)((u8*)r31 + 0x44) = r3;
            *(u32*)((u8*)r31 + 0x40) = tmp;
            goto L_801E3468;
        }
        r22 = r20;
        r19 = r4;
        r24 = r30 + 0x0;
        r23 = 0x0;
    L_801E30AC:
        r3 = r19;
        r4 = r21;
        r5 = r22;
        r6 = (u32)sp + 0x10;
        fn_801E2B74();
        r18 = r18 + r3;
        do {
            if ((s32)tmp == 0) break;
            if ((s32)tmp == 1) {
                tmp = r3 << 1;
                r22 = r22 - r3;
                r19 = r19 + tmp;
                if (r21 != 0) {
                    r21 = r21 + tmp;
                }
                tmp = *(u32*)((u8*)r17 + 0x2C);
                tmp = tmp << 3;
                r3 = r24 + tmp;
                *(u32*)((u8*)r3 + 0x4) = r18;
                *(u32*)((u8*)r3 + 0x0) = r23;
                r3 = *(u32*)((u8*)r17 + 0x2C);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r17 + 0x2C) = tmp;
                if ((s32)tmp < 5) goto L_801E30AC;
                tmp = 0x0;
                *(u32*)((u8*)r17 + 0x2C) = tmp;
                goto L_801E30AC;
            }
            r17 = r22 << 1;
            r3 = r19;
            r5 = r17;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r21 == 0) break;
            r3 = r21;
            r5 = r17;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
        } while (0);
        r3 = *(u32*)((u8*)r31 + 0x44);
        tmp = *(u32*)((u8*)r31 + 0x40);
        r3 = r3 + r20;
        *(u32*)((u8*)r31 + 0x44) = r3;
        *(u32*)((u8*)r31 + 0x40) = tmp;
        goto L_801E3468;
    }
    r18 = (u32)r29 >> 2;
    r4 = lbl_8047B470;
    r5 = r18;
    r3 = r30 + 0xa0;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r23 = r29 - r28;
    r3 = lbl_8047B470;
    r25 = r28 << 1;
    r20 = r23 << 1;
    r4 = r3 + r25;
    r5 = r20;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    tmp = lbl_8047B470;
    /* clrrwi r19, r20, 1 */;
    r5 = r18;
    r4 = r30 + 0xa0;
    r3 = tmp + r19;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r22 = r30 + 0x820;
    tmp = *(u32*)((u8*)r22 + 0x8C);
    if (tmp == 2) {
        r4 = lbl_8047B474;
        r5 = r18;
        r3 = r30 + 0xa0;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        r3 = lbl_8047B474;
        r5 = r20;
        r4 = r3 + r25;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        tmp = lbl_8047B474;
        r5 = r18;
        r4 = r30 + 0xa0;
        r3 = tmp + r19;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
    }
    r27 = r30 + 0x0;
    r21 = (u32)r29 >> 1;
    r18 = *(u32*)((u8*)r27 + 0x28);
    r20 = 0x0;
    while ((s32)r18 != (s32)tmp) {
            tmp = *(u32*)((u8*)r27 + 0x2C);
            tmp = r18 << 3;
            r19 = r30 + 0x0;
            r19 = r19 + tmp;
            r6 = r24;
            r3 = *(u32*)((u8*)r19 + 0x0);
            r5 = 0x0;
            r4 = *(u32*)((u8*)r19 + 0x4);
            __mod2u(r3, r4);
            tmp = r21 - r4;
            tmp = r20 - r3; /* -borrow */;
            tmp = r17 - r17; /* -borrow */;
            /* neg. tmp, tmp */;
            if (tmp != 2) {
                tmp = r4 - r28;
                *(u32*)((u8*)r19 + 0x4) = tmp;
                tmp = r3 - r20; /* -borrow */;
                *(u32*)((u8*)r19 + 0x0) = tmp;
            } else {

                tmp = r4 + r23;
                *(u32*)((u8*)r19 + 0x4) = tmp;
                tmp = r3 + r20; /* +carry */;
                *(u32*)((u8*)r19 + 0x0) = tmp;
            }
            r18 = r18 + 0x1;
            if ((s32)r18 >= 5) {
                r18 = 0x0;
            }
    }
    tmp = *(u32*)((u8*)r22 + 0x8C);
    r17 = r28 - r26;
    r3 = lbl_8047B470;
    r4 = r29 << 1;
    r18 = r29 - r17;
    r21 = 0x0;
    r3 = r3 + r4;
    *(u32*)((u8*)r31 + 0x44) = r18;
    tmp = r3 - r25;
    r3 = r26 << 1;
    *(u32*)((u8*)r31 + 0x40) = r21;
    r19 = tmp + r3;
    if (tmp == 2) {
        tmp = lbl_8047B474;
        r22 = r17;
        r20 = r30 + 0x0;
        r23 = 0x0;
        tmp = tmp + r4;
        tmp = tmp - r25;
        r21 = tmp + r3;
    L_801E32D8:
        r3 = r19;
        r4 = r21;
        r5 = r22;
        r6 = (u32)sp + 0xc;
        fn_801E2B74();
        r18 = r18 + r3;
        do {
            if ((s32)tmp == 0) break;
            if ((s32)tmp == 1) {
                tmp = r3 << 1;
                r22 = r22 - r3;
                r19 = r19 + tmp;
                if (r21 != 0) {
                    r21 = r21 + tmp;
                }
                tmp = *(u32*)((u8*)r27 + 0x2C);
                tmp = tmp << 3;
                r3 = r20 + tmp;
                *(u32*)((u8*)r3 + 0x4) = r18;
                *(u32*)((u8*)r3 + 0x0) = r23;
                r3 = *(u32*)((u8*)r27 + 0x2C);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r27 + 0x2C) = tmp;
                if ((s32)tmp < 5) goto L_801E32D8;
                tmp = 0x0;
                *(u32*)((u8*)r27 + 0x2C) = tmp;
                goto L_801E32D8;
            }
            r18 = r22 << 1;
            r3 = r19;
            r5 = r18;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r21 == 0) break;
            r3 = r21;
            r5 = r18;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
        } while (0);
        r3 = *(u32*)((u8*)r31 + 0x44);
        tmp = *(u32*)((u8*)r31 + 0x40);
        r3 = r3 + r17;
        *(u32*)((u8*)r31 + 0x44) = r3;
        *(u32*)((u8*)r31 + 0x40) = tmp;
        goto L_801E3468;
    }
    r22 = r17;
    r20 = r30 + 0x0;
    r23 = 0x0;
L_801E33A8:
    r3 = r19;
    r4 = r21;
    r5 = r22;
    r6 = (u32)sp + 0x8;
    fn_801E2B74();
    r18 = r18 + r3;
    do {
        if ((s32)tmp == 0) break;
        if ((s32)tmp == 1) {
            tmp = r3 << 1;
            r22 = r22 - r3;
            r19 = r19 + tmp;
            if (r21 != 0) {
                r21 = r21 + tmp;
            }
            tmp = *(u32*)((u8*)r27 + 0x2C);
            tmp = tmp << 3;
            r3 = r20 + tmp;
            *(u32*)((u8*)r3 + 0x4) = r18;
            *(u32*)((u8*)r3 + 0x0) = r23;
            r3 = *(u32*)((u8*)r27 + 0x2C);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r27 + 0x2C) = tmp;
            if ((s32)tmp < 5) goto L_801E33A8;
            tmp = 0x0;
            *(u32*)((u8*)r27 + 0x2C) = tmp;
            goto L_801E33A8;
        }
        r18 = r22 << 1;
        r3 = r19;
        r5 = r18;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
        if (r21 == 0) break;
        r3 = r21;
        r5 = r18;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
    } while (0);
    r3 = *(u32*)((u8*)r31 + 0x44);
    tmp = *(u32*)((u8*)r31 + 0x40);
    r3 = r3 + r17;
    *(u32*)((u8*)r31 + 0x44) = r3;
    *(u32*)((u8*)r31 + 0x40) = tmp;
L_801E3468:
    tmp = 0x0;
    r5 = r30 + 0x0;
    r17 = r29 << 1;
    *(u32*)((u8*)r5 + 0x34) = tmp;
    r3 = lbl_8047B470;
    r4 = r17;
    *(u32*)((u8*)r5 + 0x30) = tmp;
    *(u32*)((u8*)r31 + 0x3C) = tmp;
    *(u32*)((u8*)r31 + 0x38) = tmp;
    DCFlushRange((void*)r3, r4);
    r3 = lbl_80478D00;
    r5 = r29;
    r4 = 0x0;
    r6 = 0x0;
    r7 = 0x0;
    fn_8014E9B4();
    r3 = r30 + 0x820;
    tmp = *(u32*)((u8*)r3 + 0x8C);
    if (tmp == 2) {
        r3 = lbl_8047B474;
        r4 = r17;
        DCFlushRange((void*)r3, r4);
        r3 = lbl_80478D04;
        r5 = r29;
        r4 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        fn_8014E9B4();
    }
    return;
}
#endif

/* 0x801E34F0 | size: 0x368 | large */
#if 0
asm void fn_801E34F0(void) {
#include "src/game/battle/battle_logic_fn_801E34F0.inc"
}
#else
void fn_801E34F0(void) {
    extern u8 lbl_8046A440[];
    extern u8 lbl_8046AC60[];
    extern u32 lbl_80478D00;
    extern u32 lbl_80478D04;
    extern u32 lbl_8047B470;
    extern u32 lbl_8047B474;
    extern void fn_8014E9B4();
    extern void fn_8014EE40();
    extern void sndStreamFree();
    extern void fn_801E2B74();
    extern void fn_801E260C();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r3 = (u32)fn_801E260C;
    r4 = (u32)lbl_8046AC60;
    r7 = 0x3e8;
    r9 = 0x0;
    r3 = (u32)fn_801E260C;
    r30 = (u32)lbl_8046AC60;
    r4 = 0x30000;
    tmp = 0x1;
    r10 = 0x0;
    r6 = *(u32*)((u8*)r30 + 0x90);
    r5 = *(u32*)((u8*)r30 + 0x8C);
    r8 = r6 * 0x28;
    r29 = (u32)r8 / (u32)r7;
    r5 = 0x40;
    r4 = r7 - r7; /* -borrow */;
    r8 = r5 & ~r4;
    r5 = r29;
    r3 = 0xff;
    *(u32*)(sp + 0x18) = tmp;
    r7 = 0x7f;
    r9 = 0x0;
    r4 = lbl_8047B470;
    fn_8014EE40();
    tmp = r3 + (0x1 << 16);
    lbl_80478D00 = r3;
    if (tmp == 0xffff) {
        r3 = 0x0;
        return;
    }
    tmp = *(u32*)((u8*)r30 + 0x8C);
    if (tmp == 2) {
        r6 = 0x0;
        r3 = (u32)fn_801E260C;
        r4 = 0x30000;
        tmp = (u32)fn_801E260C;
        r5 = r29;
        r3 = 0xff;
        r7 = 0x7f;
        r8 = 0x7f;
        r9 = 0x0;
        r10 = 0x0;
        *(u32*)(sp + 0x14) = tmp;
        r4 = lbl_8047B474;
        r6 = *(u32*)((u8*)r30 + 0x90);
        fn_8014EE40();
        tmp = r3 + (0x1 << 16);
        lbl_80478D04 = r3;
        if (tmp == 0xffff) {
            r3 = lbl_80478D00;
            sndStreamFree();
            r3 = 0x0;
            return;
    }
    }
    tmp = *(u32*)((u8*)r30 + 0x8C);
    r3 = (u32)lbl_8046A440;
    r31 = (u32)lbl_8046A440;
    r24 = 0x0;
    *(u32*)((u8*)r31 + 0x28) = r24;
    *(u32*)((u8*)r31 + 0x2C) = r24;
    *(u32*)((u8*)r31 + 0x34) = r24;
    *(u32*)((u8*)r31 + 0x30) = r24;
    *(u32*)((u8*)r31 + 0x3C) = r24;
    *(u32*)((u8*)r31 + 0x38) = r24;
    *(u32*)((u8*)r31 + 0x44) = r24;
    *(u32*)((u8*)r31 + 0x40) = r24;
    if (tmp == 2) {
        r25 = 0x0;
        r27 = lbl_8047B470;
        r28 = lbl_8047B474;
        r26 = r29;
        r24 = r25;
    L_801E3654:
        r3 = r27;
        r4 = r28;
        r5 = r26;
        r6 = (u32)sp + 0x24;
        fn_801E2B74();
        r24 = r24 + r3;
        do {
            if ((s32)tmp == 0) break;
            if ((s32)tmp == 1) {
                tmp = r3 << 1;
                r26 = r26 - r3;
                r27 = r27 + tmp;
                if (r28 != 0) {
                    r28 = r28 + tmp;
                }
                tmp = *(u32*)((u8*)r31 + 0x2C);
                tmp = tmp << 3;
                r3 = r31 + tmp;
                *(u32*)((u8*)r3 + 0x4) = r24;
                *(u32*)((u8*)r3 + 0x0) = r25;
                r3 = *(u32*)((u8*)r31 + 0x2C);
                tmp = r3 + 0x1;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                if ((s32)tmp < 5) goto L_801E3654;
                tmp = 0x0;
                *(u32*)((u8*)r31 + 0x2C) = tmp;
                goto L_801E3654;
            }
            r24 = r26 << 1;
            r3 = r27;
            r5 = r24;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
            if (r28 == 0) break;
            r3 = r28;
            r5 = r24;
            r4 = 0x0;
            memset((void*)r3, (int)r4, (u32)r5);
        } while (0);
        r3 = *(u32*)((u8*)r31 + 0x44);
        tmp = *(u32*)((u8*)r31 + 0x40);
        r3 = r3 + r29;
        *(u32*)((u8*)r31 + 0x44) = r3;
        *(u32*)((u8*)r31 + 0x40) = tmp;
        goto L_801E37E8;
    }
    r27 = 0x0;
    r25 = lbl_8047B470;
    r26 = r29;
    r28 = r27;
L_801E3728:
    r3 = r25;
    r4 = r24;
    r5 = r26;
    r6 = (u32)sp + 0x20;
    fn_801E2B74();
    r28 = r28 + r3;
    do {
        if ((s32)tmp == 0) break;
        if ((s32)tmp == 1) {
            tmp = r3 << 1;
            r26 = r26 - r3;
            r25 = r25 + tmp;
            if (r24 != 0) {
                r24 = r24 + tmp;
            }
            tmp = *(u32*)((u8*)r31 + 0x2C);
            tmp = tmp << 3;
            r3 = r31 + tmp;
            *(u32*)((u8*)r3 + 0x4) = r28;
            *(u32*)((u8*)r3 + 0x0) = r27;
            r3 = *(u32*)((u8*)r31 + 0x2C);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r31 + 0x2C) = tmp;
            if ((s32)tmp < 5) goto L_801E3728;
            tmp = 0x0;
            *(u32*)((u8*)r31 + 0x2C) = tmp;
            goto L_801E3728;
        }
        r26 = r26 << 1;
        r3 = r25;
        r5 = r26;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
        if (r24 == 0) break;
        r3 = r24;
        r5 = r26;
        r4 = 0x0;
        memset((void*)r3, (int)r4, (u32)r5);
    } while (0);
    r3 = *(u32*)((u8*)r31 + 0x44);
    tmp = *(u32*)((u8*)r31 + 0x40);
    r3 = r3 + r29;
    *(u32*)((u8*)r31 + 0x44) = r3;
    *(u32*)((u8*)r31 + 0x40) = tmp;
L_801E37E8:
    r24 = r29 << 1;
    r3 = lbl_8047B470;
    r4 = r24;
    DCFlushRange((void*)r3, r4);
    r3 = lbl_80478D00;
    r5 = r29;
    r4 = 0x0;
    r6 = 0x0;
    r7 = 0x0;
    fn_8014E9B4();
    tmp = *(u32*)((u8*)r30 + 0x8C);
    if (tmp == 2) {
        r3 = lbl_8047B474;
        r4 = r24;
        DCFlushRange((void*)r3, r4);
        r3 = lbl_80478D04;
        r5 = r29;
        r4 = 0x0;
        r6 = 0x0;
        r7 = 0x0;
        fn_8014E9B4();
    }
    r3 = 0x1;

    return;
}
#endif

/* 0x801E3858 | size: 0x14 | tiny */
#if 0
asm void fn_801E3858(void) {
#include "src/game/battle/battle_logic_fn_801E3858.inc"
}
#else
void fn_801E3858(u32 *out0, u32 *out1) {
    extern u32 lbl_80478D00;
    extern u32 lbl_80478D04;
    *out0 = lbl_80478D00;
    *out1 = lbl_80478D04;
}
#endif

/* 0x801E386C | size: 0x6C | small */
#if 0
asm void fn_801E386C(void) {
#include "src/game/battle/battle_logic_fn_801E386C.inc"
}
#else
void fn_801E386C(void) {
    extern u8 lbl_8046A494[];
    extern s32 fn_8009F2F8(void*, void*, u32);
    extern void fn_801E4F34(u32);
    u8 sp[8];
    u32 item;
    if ((s32)lbl_8047B468 == 0) return;
    while (1) {
        if (fn_8009F2F8(lbl_8046A494, sp, 0) == 1) {
            item = *(u32 *)sp;
        } else {
            item = 0;
        }
        if (item == 0) return;
        fn_801E4F34(item);
    }
}
#endif

/* 0x801E38D8 | size: 0x10 | tiny */
#if 0
asm void fn_801E38D8(void) {
#include "src/game/battle/battle_logic_fn_801E38D8.inc"
}
#else
u8 fn_801E38D8(void) {
    extern u8 lbl_8046AC60[];
    return lbl_8046AC60[0xa4];
}
#endif

/* 0x801E38E8 | size: 0x48 | small */
#if 0
asm void fn_801E38E8(void) {
#include "src/game/battle/battle_logic_fn_801E38E8.inc"
}
#else
s32 fn_801E38E8(void *dst) {
    extern u8 lbl_8046AC60[];

    if ((s32)*(u32*)(lbl_8046AC60 + 0xa0) != 0) {
        memcpy(dst, lbl_8046AC60 + 0x8c, 0x10);
        return 1;
    }
    return 0;
}
#endif

/* 0x801E3930 | size: 0x48 | small */
#if 0
asm void fn_801E3930(void) {
#include "src/game/battle/battle_logic_fn_801E3930.inc"
}
#else
s32 fn_801E3930(void *dst) {
    extern u8 lbl_8046AC60[];

    if ((s32)*(u32*)(lbl_8046AC60 + 0xa0) != 0) {
        memcpy(dst, lbl_8046AC60 + 0x80, 0xc);
        return 1;
    }
    return 0;
}
#endif

/* 0x801E3978 | size: 0xD8 | medium */
#if 0
asm void fn_801E3978(void) {
#include "src/game/battle/battle_logic_fn_801E3978.inc"
}
#else
void fn_801E3978(void) {
    extern u8 lbl_8046AC60[];
    extern void fn_801E1E1C();
    extern void fn_801E1FF8();
    extern void fn_801E24B0();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r8 = (u32)lbl_8046AC60;
    r31 = (u32)lbl_8046AC60;
    r27 = r4;
    r28 = r5;
    r29 = r6;
    r30 = r7;
    tmp = *(u32*)((u8*)r31 + 0xA0);
    if ((s32)tmp != 0) {
        tmp = *(u8*)((u8*)r31 + 0xA4);
        if (tmp != 0) {
            tmp = *(u32*)((u8*)r31 + 0xE8);
            if (tmp != 0) {
                fn_801E1FF8();
                r5 = *(u32*)((u8*)r31 + 0xE8);
                tmp = (s16)r30;
                r3 = (u32)lbl_8046AC60;
                r6 = (s16)r27;
                *(u32*)(sp + 0x8) = tmp;
                r3 = (u32)lbl_8046AC60;
                r7 = (s16)r28;
                r10 = (s16)r29;
                r4 = *(u32*)((u8*)r3 + 0x80);
                tmp = *(u32*)((u8*)r3 + 0x84);
                r3 = *(u32*)((u8*)r5 + 0x0);
                r8 = (s16)r4;
                r4 = *(u32*)((u8*)r5 + 0x4);
                r9 = (s16)tmp;
                r5 = *(u32*)((u8*)r5 + 0x8);
                fn_801E1E1C();
                fn_801E24B0();
                r4 = *(u32*)((u8*)r31 + 0xE8);
                r3 = (u32)lbl_8046AC60;
                r3 = (u32)lbl_8046AC60;
                r4 = *(u32*)((u8*)r4 + 0xC);
                tmp = *(u32*)((u8*)r3 + 0xC0);
                r3 = *(u32*)((u8*)r3 + 0x50);
                r4 = r4 + tmp;
                tmp = (u32)r4 / (u32)r3;
                tmp = tmp * r3;
                r3 = r4 - tmp;
                return;
    }
    }
    }
    r3 = -0x1;

    return;
}
#endif

/* 0x801E3A50 | size: 0x504 | large */
#if 0
asm void fn_801E3A50(void) {
#include "src/game/battle/battle_logic_fn_801E3A50.inc"
}
#else
void fn_801E3A50(void) {
    extern u8 lbl_8046A494[];
    extern u8 lbl_8046AC60[];
    extern u32 lbl_80478D00;
    extern u32 lbl_80478D04;
    extern u32 lbl_8047B46C;
    extern f32 lbl_8047E4A8;
    extern void fn_800AA2F0();
    extern void fn_800C4928();
    extern void fn_8014FF0C();
    extern void fn_80150564();
    extern void fn_801E4EF0();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r12 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    void (*ctr_fn)(void) = 0;

    r12 = lbl_8047B46C;
    if (r12 != 0) {
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
    }
    r3 = (u32)lbl_8046AC60;
    r30 = -0x1;
    r31 = (u32)lbl_8046AC60;
    tmp = *(u32*)((u8*)r31 + 0xA0);
    if ((s32)tmp == 0) return;
    tmp = *(u8*)((u8*)r31 + 0xA4);
    if (tmp != 2) return;
    tmp = *(u32*)((u8*)r31 + 0xA8);
    if ((s32)tmp == 0) {
        tmp = *(u32*)((u8*)r31 + 0xAC);
        if ((s32)tmp != 0) {
        }
        r3 = (u32)lbl_8046AC60;
        tmp = 0x5;
        r3 = (u32)lbl_8046AC60;
        *(u8*)((u8*)r31 + 0xA4) = tmp;
        *(u8*)((u8*)r3 + 0xA5) = tmp;
        return;
        }
    r3 = *(u32*)((u8*)r31 + 0xCC);
    r7 = 0x1;
    tmp = *(u32*)((u8*)r31 + 0xC8);
    r6 = 0x0;
    r5 = r3 + r7;
    r4 = tmp + r6; /* +carry */;
    *(u32*)((u8*)r31 + 0xCC) = r5;
    r3 = r5 ^ r6;
    tmp = r4 ^ r6;
    *(u32*)((u8*)r31 + 0xC8) = r4;
    /* or. tmp, r3, tmp */;
    if ((s32)tmp == 0) {
    do {
        r3 = *(u32*)((u8*)r31 + 0x88);
        tmp = r3 & 0x1;
        if ((s32)tmp != 0) {
            fn_800AA2F0();
            if (r3 == 0) {
                r7 = 0x1;
                break;
            }
            tmp = r3 & 0x00000002;
            if (r3 == 0) break;
            fn_800AA2F0();
            if (r3 == 1) {
                r7 = 0x1;
                break;
                break;
            }
            }
        r7 = 0x0;
    } while (0);
        if ((s32)r7 != 0) {
            r3 = (u32)lbl_8046AC60;
            r29 = (u32)lbl_8046AC60;
            tmp = *(u8*)((u8*)r29 + 0xA7);
            if (tmp != 0) {
                r3 = *(u32*)((u8*)r29 + 0xE4);
                tmp = *(u32*)((u8*)r29 + 0xE0);
                tmp = tmp - r3;
                if ((s32)tmp <= 1) {
                    r3 = 0x0;
                    fn_801E4EF0();
                    r5 = (u32)lbl_8046AC60;
                    r4 = *(u32*)((u8*)r29 + 0xE0);
                    r6 = (u32)lbl_8046AC60;
                    r30 = r3;
                    r5 = *(u32*)((u8*)r6 + 0xD8);
                    tmp = r4 + 0x1;
                    *(u32*)((u8*)r29 + 0xE0) = tmp;
                    *(u32*)((u8*)r6 + 0xD8) = tmp;
                    goto L_801E3E60;
                }
                r3 = lbl_80478D00;
                tmp = r3 + (0x1 << 16);
                do {
                    if (tmp == 0xffff) break;
                    fn_8014FF0C();
                    tmp = r3 & 0xFF;
                    if (tmp == 0xffff) break;
                    r3 = lbl_80478D04;
                    tmp = r3 + (0x1 << 16);
                    if (tmp != 0xffff) {
                        fn_8014FF0C();
                        tmp = r3 & 0xFF;
                        if (tmp != 0xffff) {
                            tmp = 0x1;
                            break;
                        }
                        r3 = lbl_80478D00;
                        fn_80150564();
                        break;
                    }
                    tmp = 0x1;
                    break;
                } while (0);
                tmp = 0x0;

                if ((s32)tmp == 0) {
                    r3 = (u32)lbl_8046AC60;
                    tmp = 0x5;
                    r3 = (u32)lbl_8046AC60;
                    *(u8*)((u8*)r31 + 0xA4) = tmp;
                    *(u8*)((u8*)r3 + 0xA5) = tmp;
                    return;
                }
                r3 = (u32)lbl_8046AC60;
                tmp = 0x2;
                r3 = (u32)lbl_8046AC60;
                *(u8*)((u8*)r3 + 0xA5) = tmp;
                goto L_801E3E60;
            }
            r3 = 0x0;
            fn_801E4EF0();
            r30 = r3;
            goto L_801E3E60;
        }
        tmp = -0x1;
        *(u32*)((u8*)r31 + 0xCC) = tmp;
        *(u32*)((u8*)r31 + 0xC8) = tmp;
        goto L_801E3E60;
    }
    tmp = *(u8*)((u8*)r31 + 0xA7);
    do {
        if (tmp == 0) break;
        r3 = r5 ^ r7;
        tmp = r4 ^ r6;
        /* or. tmp, r3, tmp */;
        if (tmp != 0) break;
        tmp = *(u8*)((u8*)r31 + 0xA5);
        if (tmp == 2) break;
        r3 = lbl_80478D00;
        tmp = r3 + (0x1 << 16);
        do {
        if (tmp == 0xffff) break;
            fn_8014FF0C();
            tmp = r3 & 0xFF;
            if (tmp == 0xffff) break;
            r3 = lbl_80478D04;
            tmp = r3 + (0x1 << 16);
            if (tmp != 0xffff) {
                fn_8014FF0C();
                tmp = r3 & 0xFF;
                if (tmp != 0xffff) {
                    tmp = 0x1;
                    break;
                }
                r3 = lbl_80478D00;
                fn_80150564();
                break;
            }
            tmp = 0x1;
            break;
        } while (0);
        tmp = 0x0;

        if ((s32)tmp == 0) {
            tmp = 0x5;
            *(u8*)((u8*)r31 + 0xA5) = tmp;
            *(u8*)((u8*)r31 + 0xA4) = tmp;
            return;
        }
        tmp = 0x2;
        *(u8*)((u8*)r31 + 0xA5) = tmp;
    } while (0);
    r3 = (u32)lbl_8046AC60;
    r3 = (u32)lbl_8046AC60;
    r4 = *(u32*)((u8*)r3 + 0x88);
    tmp = r4 & 0x1;
    if ((s32)tmp != 0) {
        fn_800AA2F0();
        if (r3 == 0) {
            tmp = 0x1;
            goto L_801E3DF4;
        }
        tmp = r4 & 0x00000002;
        if (r3 != 0) {
            fn_800AA2F0();
        }
        if (r3 == 1) {
            tmp = 0x1;
            goto L_801E3DF4;
        }
        f1 = lbl_8047E4A8;
        f0 = *(f32*)((u8*)r3 + 0x4C);
        f0 = f1 * f0;
        f0 = (f64)(s32)f0;
        VIGetTvFormat();
        if (r3 == 1) {
            r8 = *(u32*)((u8*)r31 + 0xCC);
            tmp = (s32)r29 >> 31;
            r4 = *(u32*)((u8*)r31 + 0xC8);
            r5 = 0x0;
            r3 = (u32)((u64)r8 * (u64)r29 >> 32);
            r6 = 0x1388;
            r7 = r4 * r29;
            tmp = r8 * tmp;
            r3 = r3 + r7;
            r4 = r8 * r29;
            r3 = r3 + tmp;
            fn_800C4928();
            r3 = (u32)lbl_8046AC60;
            r3 = (u32)lbl_8046AC60;
            *(u32*)((u8*)r3 + 0xD4) = r4;
        } else {

            r8 = *(u32*)((u8*)r31 + 0xCC);
            tmp = (s32)r29 >> 31;
            r4 = *(u32*)((u8*)r31 + 0xC8);
            r5 = 0x0;
            r3 = (u32)((u64)r8 * (u64)r29 >> 32);
            r6 = 0x176a;
            r7 = r4 * r29;
            tmp = r8 * tmp;
            r3 = r3 + r7;
            r4 = r8 * r29;
            r3 = r3 + tmp;
            fn_800C4928();
            r3 = (u32)lbl_8046AC60;
            r3 = (u32)lbl_8046AC60;
            *(u32*)((u8*)r3 + 0xD4) = r4;
        }
        r3 = (u32)lbl_8046AC60;
        r3 = (u32)lbl_8046AC60;
        tmp = *(u32*)((u8*)r3 + 0xD0);
        r4 = *(u32*)((u8*)r3 + 0xD4);
        if ((s32)tmp != (s32)r4) {
            *(u32*)((u8*)r3 + 0xD0) = r4;
            tmp = 0x1;
            goto L_801E3DF4;
        }
        }
    tmp = 0x0;
L_801E3DF4:
    if ((s32)tmp == 0) goto L_801E3E60;
    tmp = *(u8*)((u8*)r31 + 0xA7);
    if (tmp != 0) {
        r3 = (u32)lbl_8046AC60;
        r29 = (u32)lbl_8046AC60;
        r3 = *(u32*)((u8*)r29 + 0xE4);
        tmp = *(u32*)((u8*)r29 + 0xE0);
        tmp = tmp - r3;
        if ((s32)tmp <= 1) {
            r3 = 0x0;
            fn_801E4EF0();
            r5 = (u32)lbl_8046AC60;
            r4 = *(u32*)((u8*)r29 + 0xE0);
            r6 = (u32)lbl_8046AC60;
            r30 = r3;
            r5 = *(u32*)((u8*)r6 + 0xD8);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r29 + 0xE0) = tmp;
            *(u32*)((u8*)r6 + 0xD8) = tmp;
        }
        goto L_801E3E60;
    }
    r3 = 0x0;
    fn_801E4EF0();
    r30 = r3;
L_801E3E60:
    if (r30 != 0) {
        tmp = r30 + (0x1 << 16);
        if (tmp != 0xffff) {
            r3 = (u32)lbl_8046AC60;
            r29 = (u32)lbl_8046AC60;
            r4 = *(u32*)((u8*)r29 + 0xE8);
            if (r4 != 0) {
                r3 = (u32)lbl_8046A494;
                r5 = 0x0;
                r3 = (u32)lbl_8046A494;
                ((void(*)(void))fn_8009F230)();
            }
            *(u32*)((u8*)r29 + 0xE8) = r30;
    }
    }
    r3 = (u32)lbl_8046AC60;
    r5 = (u32)lbl_8046AC60;
    tmp = *(u8*)((u8*)r5 + 0xA6);
    tmp = tmp & 0x1;
    if (r4 != 0) return;
    tmp = *(u8*)((u8*)r5 + 0xA7);
    if (tmp != 0) {
        r4 = *(u32*)((u8*)r5 + 0xE4);
        r3 = *(u32*)((u8*)r5 + 0xC0);
        tmp = *(u32*)((u8*)r5 + 0x50);
        r3 = r4 + r3;
        if (r3 != tmp) return;
        tmp = *(u32*)((u8*)r5 + 0xEC);
        if (tmp != 0) return;
        tmp = 0x3;
        *(u8*)((u8*)r5 + 0xA5) = tmp;
        *(u8*)((u8*)r31 + 0xA4) = tmp;
        return;
    }
    r3 = *(u32*)((u8*)r5 + 0xE8);
    if (r3 != 0) {
        r3 = *(u32*)((u8*)r3 + 0xC);
        tmp = *(u32*)((u8*)r5 + 0xC0);
        r5 = r3 + tmp;
    } else {

        r3 = *(u32*)((u8*)r5 + 0xC0);
    }
    r3 = (u32)lbl_8046AC60;
    r4 = (u32)lbl_8046AC60;
    r3 = *(u32*)((u8*)r4 + 0x50);
    if (r5 != tmp) return;
    if (r30 != 0) return;
    tmp = 0x3;
    *(u8*)((u8*)r4 + 0xA5) = tmp;
    *(u8*)((u8*)r31 + 0xA4) = tmp;

    return;
}
#endif

/* 0x801E3F54 | size: 0x104 | medium */
#if 0
asm void fn_801E3F54(void) {
#include "src/game/battle/battle_logic_fn_801E3F54.inc"
}
#else
void fn_801E3F54(void) {
    extern u8 lbl_8046A494[];
    extern u8 lbl_8046AC60[];
    extern u32 lbl_80478D00;
    extern u32 lbl_80478D04;
    extern u32 lbl_8047B46C;
    extern void DVDCancel(void);
    extern void fn_800A8850(u32);
    extern void sndStreamFree(u32);
    extern void fn_801E1D0C(void);
    extern void fn_801E4DAC(void);
    extern void fn_801E5400(void);
    extern s32 fn_8009F2F8(void *, void *, u32);
    u8 sp[8];
    u8 *queue;
    u32 r0;

    if ((s32)*(u32*)(lbl_8046AC60 + 0xa0) == 0) return;
    if (*(u8*)(lbl_8046AC60 + 0xa4) == 0) return;
    *(u8*)(lbl_8046AC60 + 0xa5) = 0;
    *(u8*)(lbl_8046AC60 + 0xa4) = 0;
    fn_800A8850(lbl_8047B46C);
    if ((s32)*(u32*)(lbl_8046AC60 + 0xb0) == 0) {
        DVDCancel();
        fn_801E1D0C();
    }
    fn_801E5400();
    if (*(u8*)(lbl_8046AC60 + 0xa7) != 0) {
        sndStreamFree(lbl_80478D00);
        r0 = lbl_80478D04;
        lbl_80478D00 = -1;
        if ((r0 + 0x10000) != 0xffff) {
            sndStreamFree(r0);
            lbl_80478D04 = -1;
        }
        fn_801E4DAC();
    }
    do {
        if (fn_8009F2F8(lbl_8046A494, sp, 0) == 1) {
            r0 = *(u32 *)sp;
        } else {
            r0 = 0;
        }
    } while (r0 != 0);
    *(u32*)(lbl_8046AC60 + 0xa8) = 0;
    *(u32*)(lbl_8046AC60 + 0xac) = 0;
}
#endif

/* 0x801E4058 | size: 0xA0 | medium */
#if 0
asm void fn_801E4058(void) {
#include "src/game/battle/battle_logic_fn_801E4058.inc"
}
#else
s32 fn_801E4058(void) {
    extern u8 lbl_8046AC60[];
    extern void fn_801E2CA8(void);
    u8 *r31 = lbl_8046AC60;
    u8 a4;
    if ((s32)*(u32*)(r31 + 0xa0) != 0) {
        a4 = *(u8*)(r31 + 0xa4);
        if (a4 == 1 || a4 == 4) {
            if (a4 == 4 && *(u8*)(lbl_8046AC60 + 0xa7) != 0) {
                fn_801E2CA8();
            }
            *(u8*)(r31 + 0xa4) = 2;
            *(u32*)(lbl_8046AC60 + 0xd0) = 0;
            *(u32*)(lbl_8046AC60 + 0xd4) = 0;
            *(u32*)(lbl_8046AC60 + 0xcc) = -1;
            *(u32*)(lbl_8046AC60 + 0xc8) = -1;
            return 1;
        }
    }
    return 0;
}
#endif

/* 0x801E40F8 | size: 0x374 | large */
#if 0
asm void fn_801E40F8(void) {
#include "src/game/battle/battle_logic_fn_801E40F8.inc"
}
#else
void fn_801E40F8(void) {
    extern u8 lbl_8046A440[];
    extern u32 lbl_8047B46C;
    extern u8 lbl_8047B478[];
    extern void fn_8009F1D0();
    extern void fn_800A541C();
    extern void fn_800A8850();
    extern void fn_801E1BB8();
    extern void fn_801E1D48();
    extern void fn_801E1D7C();
    extern void fn_801E34F0();
    extern void fn_801E4B08();
    extern void fn_801E4DE8();
    extern void fn_801E4E1C();
    extern void fn_801E4F34();
    extern void fn_801E543C();
    extern void fn_801E5470();
    extern void fn_801E3A50();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;

    r6 = (u32)lbl_8046A440;
    r29 = (u32)lbl_8046A440;
    r30 = r29 + 0x820;
    r28 = r3;
    r27 = r4;
    r31 = r5;
    tmp = *(u32*)((u8*)r30 + 0xA0);
    if ((s32)tmp == 0) { r3 = 0x0; return; }
    tmp = *(u8*)((u8*)r30 + 0xA4);
    if (tmp != 0) { r3 = 0x0; return; }
    if ((s32)r28 > 0) {
        r5 = *(u32*)((u8*)r30 + 0x60);
        if (r5 == 0) {
            r3 = 0x0;
            return;
        }
        tmp = *(u32*)((u8*)r30 + 0x50);
        if (tmp <= r28) { r3 = 0x0; return; }
        r3 = r30;
        tmp = tmp << 2;
        r4 = r29 + 0xa0;
        r6 = r5 + tmp;
        r5 = 0x20;
        r7 = 0x2;
        fn_800A541C();
        if ((s32)r3 < 0) {
            r3 = 0x0;
            return;
        }
        r3 = r29 + 0xa0;
        r4 = *(u32*)((u8*)r30 + 0x64);
        r6 = *(s16*)((u8*)r29 + 0xA0);
        tmp = *(s16*)((u8*)r3 + 0x2);
        r3 = r4 + r6;
        *(u32*)((u8*)r30 + 0xC0) = r28;
        tmp = tmp - r6;
        *(u32*)((u8*)r30 + 0xB8) = r3;
        *(u32*)((u8*)r30 + 0xBC) = tmp;


    } else {
    r3 = *(u32*)((u8*)r30 + 0x64);
    tmp = *(u32*)((u8*)r30 + 0x54);
    *(u32*)((u8*)r30 + 0xB8) = r3;
    *(u32*)((u8*)r30 + 0xBC) = tmp;
    *(u32*)((u8*)r30 + 0xC0) = r28;
    }
    r28 = r29 + 0x820;
    tmp = *(u8*)((u8*)r28 + 0xA7);
    if (tmp != 0) {
        if ((s32)r31 < 0) { r3 = 0x0; return; }
        tmp = *(u32*)((u8*)r28 + 0x98);
        if (r31 >= tmp) {

            r3 = 0x0;
            return;
        }
        *(u32*)((u8*)r28 + 0xDC) = r31;
    }
    r31 = r29 + 0x820;
    r4 = r27 & 0x1;
    tmp = *(u32*)((u8*)r31 + 0xB0);
    r3 = 0x0;
    *(u8*)((u8*)r31 + 0xA6) = r4;
    *(u32*)((u8*)r31 + 0xD8) = r3;
    if ((s32)tmp != 0) {
        r4 = *(u32*)((u8*)r31 + 0xB4);
        r3 = r31;
        r5 = *(u32*)((u8*)r31 + 0x58);
        r7 = 0x2;
        r6 = *(u32*)((u8*)r31 + 0x64);
        fn_800A541C();
        if ((s32)r3 < 0) {
            r3 = 0x0;
            return;
        }
        r4 = *(u32*)((u8*)r31 + 0xB4);
        r3 = 0x14;
        tmp = *(u32*)((u8*)r31 + 0xB8);
        r5 = *(u32*)((u8*)r31 + 0x64);
        tmp = r4 + tmp;
        r25 = tmp - r5;
        r4 = r25;
        fn_801E5470();
        tmp = *(u8*)((u8*)r28 + 0xA7);
        if (tmp != 0) {
            r4 = r25;
            r3 = 0xc;
            fn_801E4E1C();
        }

    } else {
    r3 = 0x14;
    r4 = 0x0;
    fn_801E5470();
    tmp = *(u8*)((u8*)r28 + 0xA7);
    if (tmp != 0) {
        r3 = 0xc;
        r4 = 0x0;
        fn_801E4E1C();
    }
    r3 = 0x8;
    fn_801E1D7C();
    }
do {
    tmp = *(u32*)((u8*)r31 + 0xB0);
    if ((s32)tmp == 0) {
        r25 = 0x0;
        r26 = r29 + 0x820;
        r27 = r25;
        do {
            r3 = r27 + 0xf0;
            r3 = r26 + r3;
            fn_801E1BB8();
            r25 = r25 + 0x1;
            r27 = r27 + 0xc;
        } while ((s32)r25 < 0xa);
    }
    r26 = r29 + 0x820;
    r25 = 0x0;
    r27 = 0x0;
    do {
        r3 = r27 + 0x168;
        r3 = r26 + r3;
        fn_801E4F34();
        r25 = r25 + 0x1;
        r27 = r27 + 0x10;
    } while ((s32)r25 < 3);
    tmp = *(u8*)((u8*)r28 + 0xA7);
    if (tmp != 0) {
        r26 = r29 + 0x820;
        r25 = 0x0;
        r27 = 0x0;
        do {
            r3 = r27 + 0x198;
            r3 = r26 + r3;
            fn_801E4B08();
            r25 = r25 + 0x1;
            r27 = r27 + 0xc;
        } while ((s32)r25 < 3);
    }
    r3 = r29 + 0x74;
    r4 = (u32)lbl_8047B478;
    r5 = 0x2;
    fn_8009F1D0();
    fn_801E543C();
    tmp = *(u8*)((u8*)r28 + 0xA7);
    if (tmp != 0) {
        fn_801E4DE8();
    }
    tmp = *(u32*)((u8*)r31 + 0xB0);
    if ((s32)tmp == 0) {
        fn_801E1D48();
    }
    tmp = *(u8*)((u8*)r28 + 0xA7);
    if (tmp != 0) {
        r3 = r29 + 0x74;
        r4 = (u32)sp + 0x8;
        r5 = 0x1;
        ((void(*)(void))fn_8009F2F8)();
        r3 = r29 + 0x74;
        r4 = (u32)sp + 0xc;
        r5 = 0x1;
        ((void(*)(void))fn_8009F2F8)();
        if ((s32)tmp != 0 && (s32)tmp != 0) {

            tmp = 0x1;
            break;
        }
        tmp = 0x0;
        break;
    }
    r3 = r29 + 0x74;
    r4 = (u32)sp + 0x8;
    r5 = 0x1;
    ((void(*)(void))fn_8009F2F8)();
    if ((s32)tmp != 0) {
        tmp = 0x1;
        break;
    }
    tmp = 0x0;
} while (0);
    if ((s32)tmp == 0) {
        r3 = 0x0;
        return;
    }
    tmp = *(u8*)((u8*)r28 + 0xA7);
    r6 = 0x1;
    r4 = r29 + 0x820;
    r5 = 0x0;
    r3 = -0x1;
    *(u8*)((u8*)r30 + 0xA4) = r6;
    *(u8*)((u8*)r4 + 0xA5) = r5;
    *(u32*)((u8*)r4 + 0xE8) = r5;
    *(u32*)((u8*)r4 + 0xEC) = r5;
    *(u32*)((u8*)r4 + 0xE0) = r3;
    *(u32*)((u8*)r4 + 0xE4) = r5;
    if (tmp != 0) {
        fn_801E34F0();
    }
    r3 = (u32)fn_801E3A50;
    r3 = (u32)fn_801E3A50;
    fn_800A8850();
    lbl_8047B46C = r3;
    r3 = 0x1;
    return;


}
#endif

/* 0x801E446C | size: 0x30 | small */
#if 0
asm void fn_801E446C(void) {
#include "src/game/battle/battle_logic_fn_801E446C.inc"
}
#else
void fn_801E446C(void *arg) {
    extern u8 lbl_8046A4B4[];
    extern void fn_8009F230(void *, void *, u32);
    fn_8009F230(lbl_8046A4B4, arg, 1);
}
#endif

/* 0x801E449C | size: 0x1B4 | medium */
#if 0
asm void fn_801E449C(void) {
#include "src/game/battle/battle_logic_fn_801E449C.inc"
}
#else
void fn_801E449C(void) {
    extern u8 lbl_8046AC60[];
    extern u32 lbl_8047B470;
    extern u32 lbl_8047B474;
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f4 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = (u32)lbl_8046AC60;
    r5 = (u32)lbl_8046AC60;
    tmp = *(u32*)((u8*)r5 + 0xA0);
    if ((s32)tmp != 0) {
        tmp = *(u8*)((u8*)r5 + 0xA4);
        if (tmp == 0) {
            tmp = *(u32*)((u8*)r5 + 0xB0);
            r31 = r3;
            if ((s32)tmp != 0) {
                tmp = *(u32*)((u8*)r5 + 0x58);
                *(u32*)((u8*)r5 + 0xB4) = r3;
                r31 = r31 + tmp;
            } else {

                tmp = 0xa;
                r3 = 0x0;
                ctr_fn = (void(*)(void))tmp;
                do {
                    r4 = r5 + r3;
                    r3 = r3 + 0xc;
                    *(u32*)((u8*)r4 + 0xF0) = r31;
                    r4 = *(u32*)((u8*)r5 + 0x44);
                    tmp = r4 + 0x1f;
                    /* clrrwi tmp, tmp, 5 */;
                    r31 = r31 + tmp;
                } while (--ctr != 0);
            }
            r3 = (u32)lbl_8046AC60;
            r27 = 0x0;
            r29 = (u32)lbl_8046AC60;
            r30 = 0x0;
            r3 = *(u32*)((u8*)r29 + 0x80);
            tmp = *(u32*)((u8*)r29 + 0x84);
            r4 = r3 * tmp;
            r3 = (u32)r4 >> 2;
            r4 = r4 + 0x1f;
            tmp = r3 + 0x1f;
            /* clrrwi r26, r4, 5 */;
            /* clrrwi r25, tmp, 5 */;
            do {
                r28 = r29 + r30;
                r3 = r31;
                *(u32*)((u8*)r28 + 0x168) = r31;
                r4 = r26;
                DCInvalidateRange((void*)r3, r4);
                r31 = r31 + r26;
                r4 = r25;
                *(u32*)((u8*)r28 + 0x16C) = r31;
                r3 = r31;
                DCInvalidateRange((void*)r3, r4);
                r31 = r31 + r25;
                r4 = r25;
                *(u32*)((u8*)r28 + 0x170) = r31;
                r3 = r31;
                DCInvalidateRange((void*)r3, r4);
                r27 = r27 + 0x1;
                r31 = r31 + r25;
                r30 = r30 + 0x10;
            } while (r27 < 3);
            r3 = (u32)lbl_8046AC60;
            r5 = (u32)lbl_8046AC60;
            tmp = *(u8*)((u8*)r5 + 0xA7);
            if (tmp != 0) {
                tmp = 0x3;
                r6 = 0x0;
                r3 = 0x0;
                ctr_fn = (void(*)(void))tmp;
                do {
                    r4 = r5 + r3;
                    r3 = r3 + 0xc;
                    *(u32*)((u8*)r4 + 0x198) = r31;
                    *(u32*)((u8*)r4 + 0x19C) = r31;
                    *(u32*)((u8*)r4 + 0x1A0) = r6;
                    tmp = *(u32*)((u8*)r5 + 0x48);
                    r4 = tmp << 2;
                    tmp = r4 + 0x1f;
                    /* clrrwi tmp, tmp, 5 */;
                    r31 = r31 + tmp;
                } while (--ctr != 0);
                r4 = (u32)lbl_8046AC60;
                r3 = 0x1f4;
                r5 = (u32)lbl_8046AC60;
                lbl_8047B470 = r31;
                r4 = *(u32*)((u8*)r5 + 0x90);
                tmp = *(u32*)((u8*)r5 + 0x8C);
                r4 = r4 * 0x28;
                r3 = (u32)r4 / (u32)r3;
                tmp = r3 + 0x1f;
                /* clrrwi tmp, tmp, 5 */;
                r31 = r31 + tmp;
                if (tmp == 2) {
                    lbl_8047B474 = r31;
                    r31 = r31 + tmp;
            }
            }
            r4 = (u32)lbl_8046AC60;
            r3 = 0x1;
            r4 = (u32)lbl_8046AC60;
            *(u32*)((u8*)r4 + 0x9C) = r31;
            return;
    }
    }
    r3 = 0x0;

    return;
}
#endif

/* 0x801E4650 | size: 0xD4 | medium */
#if 0
asm void fn_801E4650(void) {
#include "src/game/battle/battle_logic_fn_801E4650.inc"
}
#else
#pragma push
#pragma optimize_for_size on
int fn_801E4650(void) {
    extern u8 lbl_8046AC60[];
    s32 total;
    u32 area;

    if (*(s32 *)(lbl_8046AC60 + 0xA0) != 0) {
        if (*(s32 *)(lbl_8046AC60 + 0xB0) != 0) {
            total = (*(s32 *)(lbl_8046AC60 + 0x58) + 0x1F) & ~0x1F;
        } else {
            total = ((*(s32 *)(lbl_8046AC60 + 0x44) + 0x1F) & ~0x1F) * 0xA;
        }
        area = *(u32 *)(lbl_8046AC60 + 0x80) * *(u32 *)(lbl_8046AC60 + 0x84);
        total += ((area + 0x1F) & ~0x1F) * 3;
        total += (((area >> 2) + 0x1F) & ~0x1F) * 3;
        total += (((area >> 2) + 0x1F) & ~0x1F) * 3;
        if (*(u8 *)(lbl_8046AC60 + 0xA7) != 0) {
            total += (((*(u32 *)(lbl_8046AC60 + 0x48) << 2) + 0x1F) & ~0x1F) * 3;
            total += *(s32 *)(lbl_8046AC60 + 0x8C) *
                     (s32)(((*(u32 *)(lbl_8046AC60 + 0x90) * 0x28 / 0x1F4) + 0x1F) & ~0x1F);
        }
        return total + 0x1000;
    }
    return 0;
}
#pragma pop
#endif

/* 0x801E4724 | size: 0x54 | small */
#if 0
asm void fn_801E4724(void) {
#include "src/game/battle/battle_logic_fn_801E4724.inc"
}
#else
int fn_801E4724(void) {
    extern u8 lbl_8046AC60[];
    extern void fn_800A50E4(void);
    u8 *base = lbl_8046AC60;
    if ((s32)*(u32*)(base + 0xa0) != 0 && *(u8*)(base + 0xa4) == 0) {
        *(u32*)(base + 0xa0) = 0;
        fn_800A50E4();
        return 1;
    }
    return 0;
}
#endif

/* 0x801E4778 | size: 0x2F4 | large */
#if 0
asm void fn_801E4778(void) {
#include "src/game/battle/battle_logic_fn_801E4778.inc"
}
#else
void fn_801E4778(void) {
    extern u8 lbl_8046A4E0[];
    extern u8 lbl_8046AC60[];
    extern u32 lbl_8047B468;
    extern u8 lbl_8047E4AC[];
    extern void fn_800A501C();
    extern void fn_800A50E4();
    extern void fn_800A541C();
    extern void fn_800CA7FC();
    extern void fn_801ECA10();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r24 = r3;
    r31 = r4;
    fn_801ECA10();
    if ((s32)r3 == 0) {
        r3 = 0x0;
        return;
    }
    tmp = lbl_8047B468;
    if ((s32)tmp == 0) {
        r3 = 0x0;
        return;
    }
    r3 = (u32)lbl_8046AC60;
    r30 = (u32)lbl_8046AC60;
    tmp = *(u32*)((u8*)r30 + 0xA0);
    if ((s32)tmp != 0) {
        r3 = 0x0;
        return;
    }
    r29 = r30 + 0x80;
    r4 = 0x0;
    r3 = r29;
    r5 = 0xc;
    memset((void*)r3, (int)r4, (u32)r5);
    r3 = (u32)lbl_8046AC60;
    r4 = 0x0;
    r3 = (u32)lbl_8046AC60;
    r5 = 0x10;
    r28 = r3 + 0x8c;
    r3 = r28;
    memset((void*)r3, (int)r4, (u32)r5);
    r4 = (u32)lbl_8046AC60;
    r3 = r24;
    r4 = (u32)lbl_8046AC60;
    fn_800A501C();
    if ((s32)r3 == 0) {
        r3 = 0x0;
        return;
    }
    r3 = (u32)lbl_8046AC60;
    r4 = (u32)lbl_8046A4E0;
    r3 = (u32)lbl_8046AC60;
    r5 = 0x40;
    r4 = (u32)lbl_8046A4E0;
    r6 = 0x0;
    r7 = 0x2;
    fn_800A541C();
    if ((s32)r3 < 0) {
        r3 = (u32)lbl_8046AC60;
        r3 = (u32)lbl_8046AC60;
        fn_800A50E4();
        r3 = 0x0;
        return;
    }
    r4 = (u32)lbl_8046AC60;
    r3 = (u32)lbl_8046A4E0;
    r4 = (u32)lbl_8046AC60;
    r5 = 0x30;
    r24 = r4 + 0x3c;
    r4 = (u32)lbl_8046A4E0;
    r3 = r24;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = r24;
    r4 = (u32)lbl_8047E4AC;
    fn_800CA7FC();
    if ((s32)r3 != 0) {
        r3 = (u32)lbl_8046AC60;
        r3 = (u32)lbl_8046AC60;
        fn_800A50E4();
        r3 = 0x0;
        return;
    }
    r3 = (u32)lbl_8046AC60;
    r3 = (u32)lbl_8046AC60;
    r4 = *(u32*)((u8*)r3 + 0x40);
    /* subis tmp, r4, 0x1 */;
    if (tmp != 0x1000) {
        fn_800A50E4();
        r3 = 0x0;
        return;
    }
    r24 = *(u32*)((u8*)r3 + 0x5C);
    r4 = (u32)lbl_8046A4E0;
    r4 = (u32)lbl_8046A4E0;
    r5 = 0x20;
    r6 = r24;
    r7 = 0x2;
    fn_800A541C();
    if ((s32)r3 < 0) {
        r3 = (u32)lbl_8046AC60;
        r3 = (u32)lbl_8046AC60;
        fn_800A50E4();
        r3 = 0x0;
        return;
    }
    r4 = (u32)lbl_8046AC60;
    r3 = (u32)lbl_8046A4E0;
    r4 = (u32)lbl_8046AC60;
    r5 = 0x14;
    r26 = r4 + 0x6c;
    r4 = (u32)lbl_8046A4E0;
    r3 = r26;
    memcpy((void*)r3, (const void*)r4, (u32)r5);
    r3 = (u32)lbl_8046AC60;
    r25 = r24 + 0x14;
    tmp = 0x0;
    r24 = 0x0;
    r27 = (u32)lbl_8046AC60;
    *(u8*)((u8*)r27 + 0xA7) = tmp;
    while (1) {
        tmp = *(u32*)((u8*)r26 + 0x0);
        if (r24 >= tmp) break;
        r3 = r27 + r24;
        tmp = *(u8*)((u8*)r3 + 0x70);
        if ((s32)tmp != 1) {
            if ((s32)tmp >= 1) { r3 = 0x0; return; }
            if ((s32)tmp < 0) {
                r3 = 0x0;
                return;
            }
            r3 = (u32)lbl_8046AC60;
            r4 = (u32)lbl_8046A4E0;
            r3 = (u32)lbl_8046AC60;
            r6 = r25;
            r4 = (u32)lbl_8046A4E0;
            r5 = 0x20;
            r7 = 0x2;
            fn_800A541C();
            if ((s32)r3 < 0) {
                r3 = (u32)lbl_8046AC60;
                r3 = (u32)lbl_8046AC60;
                fn_800A50E4();
                r3 = 0x0;
                return;
            }
            r4 = (u32)lbl_8046A4E0;
            r3 = r29;
            r4 = (u32)lbl_8046A4E0;
            r5 = 0xc;
            memcpy((void*)r3, (const void*)r4, (u32)r5);
            r25 = r25 + 0xc;

        } else {
        r3 = (u32)lbl_8046AC60;
        r4 = (u32)lbl_8046A4E0;
        r3 = (u32)lbl_8046AC60;
        r6 = r25;
        r4 = (u32)lbl_8046A4E0;
        r5 = 0x20;
        r7 = 0x2;
        fn_800A541C();
        if ((s32)r3 < 0) {
            r3 = (u32)lbl_8046AC60;
            r3 = (u32)lbl_8046AC60;
            fn_800A50E4();
            r3 = 0x0;
            return;
        }
        r4 = (u32)lbl_8046A4E0;
        r3 = r28;
        r4 = (u32)lbl_8046A4E0;
        r5 = 0x10;
        memcpy((void*)r3, (const void*)r4, (u32)r5);
        tmp = 0x1;
        r25 = r25 + 0x10;
        *(u8*)((u8*)r27 + 0xA7) = tmp;


        }
        r24 = r24 + 0x1;


    }
    r3 = (u32)lbl_8046AC60;
    tmp = 0x1;
    r4 = (u32)lbl_8046AC60;
    r5 = 0x0;
    *(u8*)((u8*)r4 + 0xA5) = r5;
    r3 = 0x1;
    *(u8*)((u8*)r4 + 0xA4) = r5;
    *(u8*)((u8*)r4 + 0xA6) = r5;
    *(u32*)((u8*)r4 + 0xB0) = r31;
    *(u32*)((u8*)r30 + 0xA0) = tmp;

    return;
}
#endif

/* 0x801E4A6C | size: 0x58 | small */
#if 0
asm void fn_801E4A6C(void) {
#include "src/game/battle/battle_logic_fn_801E4A6C.inc"
}
#else
void fn_801E4A6C(void) {
    extern u8 lbl_8046A440[];
    extern void fn_8009F1D0(void *, void *, u32);
    u8 *r31;
    r31 = lbl_8046A440;
    memset(r31 + 0x820, 0, 0x1c0);
    fn_8009F1D0(r31 + 0x54, r31 + 0x48, 3);
    lbl_8047B468 = 1;
}
#endif

/* 0x801E4AC4 | size: 0x44 | small */
#if 0
asm void fn_801E4AC4(void) {
#include "src/game/battle/battle_logic_fn_801E4AC4.inc"
}
#else
u32 fn_801E4AC4(u32 arg) {
    extern u8 lbl_8046AE38[];
    extern s32 fn_8009F2F8(u8* queue, u32* out, u32 arg);
    u32 out;

    if (fn_8009F2F8(lbl_8046AE38, &out, arg) == 1) {
        return out;
    }
    return 0;
}
#endif

/* 0x801E4B08 | size: 0x30 | small */
#if 0
asm void fn_801E4B08(void) {
#include "src/game/battle/battle_logic_fn_801E4B08.inc"
}
#else
void fn_801E4B08(void* val) {
    extern u8 lbl_8046AE58[];
    fn_8009F230(lbl_8046AE58, val, 0);
}
#endif

/* 0x801E4B38 | size: 0x148 | medium */
#if 0
asm void fn_801E4B38(void) {
#include "src/game/battle/battle_logic_fn_801E4B38.inc"
}
#else
void fn_801E4B38(void) {
    extern u8 lbl_8046AC60[];
    extern u8 lbl_8046AE20[];
    extern void fn_800A221C();
    extern void fn_801E446C();
    extern void fn_801ECAB0();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = (u32)lbl_8046AC60;
    r31 = (u32)lbl_8046AC60;
    r4 = (u32)lbl_8046AE20;
    r29 = r3;
    r30 = (u32)lbl_8046AE20;
    r27 = 0x0;
    r28 = *(u32*)((u8*)r31 + 0xBC);
    do {
        tmp = *(u32*)((u8*)r31 + 0x6C);
        r25 = r29 + 0x8;
        r3 = r30 + 0x38;
        r4 = (u32)sp + 0x8;
        r6 = tmp << 2;
        r5 = 0x1;
        r26 = r6 + 0x8;
        r26 = r29 + r26;
        ((void(*)(void))fn_8009F2F8)();
        tmp = *(u32*)((u8*)r31 + 0x6C);
        r4 = 0x0;
        ctr_fn = (void(*)(void))tmp;
        if (tmp > 0) {
            do {
                r3 = r31 + r4;
                tmp = *(u8*)((u8*)r3 + 0x70);
                if ((s32)tmp != 1) {

                } else {
                r4 = *(u32*)((u8*)r25 + 0x0);
                r5 = 0x0;
                tmp = *(u32*)((u8*)r31 + 0xDC);
                r3 = *(u32*)((u8*)r24 + 0x0);
                tmp = r4 * tmp;
                r4 = r26 + tmp;
                fn_801ECAB0();
                *(u32*)((u8*)r24 + 0x8) = r3;
                r4 = r24;
                r3 = r30 + 0x18;
                r5 = 0x1;
                tmp = *(u32*)((u8*)r24 + 0x0);
                *(u32*)((u8*)r24 + 0x4) = tmp;
                ((void(*)(void))fn_8009F230)();
                break;
                }
                tmp = *(u32*)((u8*)r25 + 0x0);
                r25 = r25 + 0x4;
                r4 = r4 + 0x1;
                r26 = r26 + tmp;
            } while (--ctr != 0);
        }
        tmp = *(u32*)((u8*)r31 + 0xC0);
        r5 = *(u32*)((u8*)r31 + 0x50);
        r4 = r27 + tmp;
        r3 = (u32)r4 / (u32)r5;
        r3 = r3 * r5;
        r3 = r4 - r3;
        if (r3 == tmp) {
            tmp = *(u8*)((u8*)r31 + 0xA6);
            tmp = tmp & 0x1;
            if (r3 != tmp) {
                r28 = *(u32*)((u8*)r29 + 0x0);
                r29 = *(u32*)((u8*)r31 + 0xB4);
                goto L_801E4C68;
            }
            if ((s32)r27 < 2) {
                r3 = 0x1;
                fn_801E446C();
            }
            r3 = r30 + 0x1058;
            fn_800A221C();
            goto L_801E4C68;
        }
        tmp = *(u32*)((u8*)r29 + 0x0);
        r29 = r29 + r28;
        r28 = tmp;
    L_801E4C68:
        if ((s32)r27 == 2) {
            r3 = 0x1;
            fn_801E446C();
        }
        r27 = r27 + 0x1;
    } while (1);
}
#endif

/* 0x801E4C80 | size: 0x12C | medium */
#if 0
asm void fn_801E4C80(void) {
#include "src/game/battle/battle_logic_fn_801E4C80.inc"
}
#else
void fn_801E4C80(void) {
    extern u8 lbl_8046AC60[];
    extern u8 lbl_8046AE38[];
    extern u8 lbl_8046AE58[];
    extern void fn_801E1B54();
    extern void fn_801E1BE8();
    extern void fn_801E446C();
    extern void fn_801ECAB0();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = (u32)lbl_8046AC60;
    r3 = (u32)lbl_8046AE58;
    r30 = (u32)lbl_8046AC60;
    r28 = 0x0;
    r31 = (u32)lbl_8046AE58;
    do {
        fn_801E1BE8();
        tmp = *(u32*)((u8*)r30 + 0x6C);
        r29 = r3;
        r6 = *(u32*)((u8*)r29 + 0x0);
        r3 = r31;
        r4 = tmp << 2;
        r5 = 0x1;
        r27 = r4 + 0x8;
        r26 = r6 + 0x8;
        r4 = (u32)sp + 0x8;
        r27 = r6 + r27;
        ((void(*)(void))fn_8009F2F8)();
        tmp = *(u32*)((u8*)r30 + 0x6C);
        r4 = 0x0;
        ctr_fn = (void(*)(void))tmp;
        if (tmp > 0) {
            do {
                r3 = r30 + r4;
                tmp = *(u8*)((u8*)r3 + 0x70);
                if ((s32)tmp != 1) {

                } else {
                r4 = *(u32*)((u8*)r26 + 0x0);
                r5 = 0x0;
                tmp = *(u32*)((u8*)r30 + 0xDC);
                r3 = *(u32*)((u8*)r25 + 0x0);
                tmp = r4 * tmp;
                r4 = r27 + tmp;
                fn_801ECAB0();
                *(u32*)((u8*)r25 + 0x8) = r3;
                r3 = (u32)lbl_8046AE38;
                r3 = (u32)lbl_8046AE38;
                r4 = r25;
                tmp = *(u32*)((u8*)r25 + 0x0);
                r5 = 0x1;
                *(u32*)((u8*)r25 + 0x4) = tmp;
                ((void(*)(void))fn_8009F230)();
                break;
                }
                tmp = *(u32*)((u8*)r26 + 0x0);
                r26 = r26 + 0x4;
                r4 = r4 + 0x1;
                r27 = r27 + tmp;
            } while (--ctr != 0);
        }
        if ((s32)r28 < 2) {
            tmp = *(u8*)((u8*)r30 + 0xA6);
            tmp = tmp & 0x1;
            if ((s32)r28 == 2) {
                r3 = *(u32*)((u8*)r30 + 0x50);
                r5 = *(u32*)((u8*)r29 + 0x4);
                r4 = *(u32*)((u8*)r30 + 0xC0);
                r3 = r5 + r4;
                if (r3 == tmp) {
                    r3 = 0x1;
                    fn_801E446C();
        }
        }
        }
        if ((s32)r28 == 2) {
            r3 = 0x1;
            fn_801E446C();
        }
        r3 = r29;
        fn_801E1B54();
        r28 = r28 + 0x1;
    } while (1);
}
#endif

/* 0x801E4DAC | size: 0x3C | small */
/* Cancel battle sub-thread 2 if active. */
#if 0
asm void fn_801E4DAC(void) {
#include "src/game/battle/battle_logic_fn_801E4DAC.inc"
}
#else
void fn_801E4DAC(void) {
    extern u8 lbl_8046BE78[];
    extern u32 lbl_8047B480;

    if ((s32)lbl_8047B480 != 0) {
        OSCancelThread((void*)lbl_8046BE78);
        lbl_8047B480 = 0;
    }
}
#endif

/* 0x801E4DE8 | size: 0x34
 * Resume battle sub-thread 2 if active.
 */
#if 0
asm void fn_801E4DE8(void) {
#include "src/game/battle/battle_logic_fn_801E4DE8.inc"
}
#else
void fn_801E4DE8(void) {
    extern u8 lbl_8046BE78[];
    extern u32 lbl_8047B480;
    extern void fn_800A1F94(void* thread);

    if ((s32)lbl_8047B480 != 0) {
        fn_800A1F94((void*)lbl_8046BE78);
    }
}
#endif

/* 0x801E4E1C | size: 0xD4 | medium */
#if 0
asm void fn_801E4E1C(void) {
#include "src/game/battle/battle_logic_fn_801E4E1C.inc"
}
#else
void fn_801E4E1C(void) {
    extern u8 lbl_8046AE20[];
    extern u32 lbl_8047B480;
    extern void fn_8009F1D0();
    extern void fn_800A19CC();
    extern void fn_801E4B38();
    extern void fn_801E4C80();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r31 = 0;

    r5 = (u32)lbl_8046AE20;
    r31 = (u32)lbl_8046AE20;
    if (r4 != 0) {
        r5 = (u32)fn_801E4B38;
        r6 = r31 + 0x58;
        tmp = (u32)fn_801E4B38;
        r8 = r3;
        r5 = r4;
        r3 = r31 + 0x1058;
        r4 = tmp;
        r7 = 0x1000;
        r9 = 0x1;
        r6 = r6 + 0x1000;
        fn_800A19CC();
    } else {
        r4 = (u32)fn_801E4C80;
        r6 = r31 + 0x58;
        r8 = r3;
        r3 = r31 + 0x1058;
        r4 = (u32)fn_801E4C80;
        r5 = 0x0;
        r7 = 0x1000;
        r9 = 0x1;
        r6 = r6 + 0x1000;
        fn_800A19CC();
    }
    if ((s32)r3 == 0) {
        r3 = 0x0;
        return;
    }
    r3 = r31 + 0x38;
    r4 = r31 + 0xc;
    r5 = 0x3;
    fn_8009F1D0();
    r3 = r31 + 0x18;
    r4 = r31 + 0x0;
    r5 = 0x3;
    fn_8009F1D0();
    tmp = 0x1;
    r3 = 0x1;
    lbl_8047B480 = tmp;

    return;
}
#endif

/* 0x801E4EF0 | size: 0x44 | small */
#if 0
asm void fn_801E4EF0(void) {
#include "src/game/battle/battle_logic_fn_801E4EF0.inc"
}
#else
u32 fn_801E4EF0(u32 arg) {
    extern u8 lbl_8046C1A8[];
    extern s32 fn_8009F2F8(void *, void *, u32);
    u8 sp[8];

    if (fn_8009F2F8(lbl_8046C1A8, sp, arg) == 1) {
        return *(u32 *)sp;
    }
    return 0;
}
#endif

/* 0x801E4F34 | size: 0x30 | small */
#if 0
asm void fn_801E4F34(void) {
#include "src/game/battle/battle_logic_fn_801E4F34.inc"
}
#else
void fn_801E4F34(void *arg) {
    extern u8 lbl_8046C1C8[];
    extern void fn_8009F230(void *, void *, u32);
    fn_8009F230(lbl_8046C1C8, arg, 0);
}
#endif

/* 0x801E4F64 | size: 0x1F0 | medium */
#if 0
asm void fn_801E4F64(void) {
#include "src/game/battle/battle_logic_fn_801E4F64.inc"
}
#else
void fn_801E4F64(void) {
    extern u8 lbl_8046AC60[];
    extern u8 lbl_8046C190[];
    extern u32 lbl_8047B48C;
    extern void fn_800A221C();
    extern void fn_801E446C();
    extern void fn_801E5548();
    u8 sp[0x40];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r4 = (u32)lbl_8046AC60;
    r31 = (u32)lbl_8046AC60;
    r4 = (u32)lbl_8046C190;
    r29 = r3;
    r30 = (u32)lbl_8046C190;
    r27 = 0x0;
    r28 = *(u32*)((u8*)r31 + 0xBC);
    do {
        tmp = *(u8*)((u8*)r31 + 0xA7);
        if (tmp != 0) {
            while (1) {
                tmp = *(u32*)((u8*)r31 + 0xD8);
            if ((s32)tmp >= 0) break;
                r3 = OSDisableInterrupts();
                r4 = *(u32*)((u8*)r31 + 0xD8);
                tmp = r4 + 0x1;
                *(u32*)((u8*)r31 + 0xD8) = tmp;
                OSRestoreInterrupts(r3);
                tmp = *(u32*)((u8*)r31 + 0xC0);
                r5 = *(u32*)((u8*)r31 + 0x50);
                r4 = r27 + tmp;
                r3 = (u32)r4 / (u32)r5;
                r3 = r3 * r5;
                r3 = r4 - r3;
                if (r3 == tmp) {
                    tmp = *(u8*)((u8*)r31 + 0xA6);
                    tmp = tmp & 0x1;
                    if (r3 == tmp) break;
                    r28 = *(u32*)((u8*)r29 + 0x0);
                    r29 = *(u32*)((u8*)r31 + 0xB4);

                } else {
                tmp = *(u32*)((u8*)r29 + 0x0);
                r29 = r29 + r28;
                r28 = tmp;
                }
                r27 = r27 + 0x1;


            }
        }
        tmp = *(u32*)((u8*)r31 + 0x6C);
        r25 = r29 + 0x8;
        r3 = r30 + 0x38;
        r4 = (u32)sp + 0x8;
        r6 = tmp << 2;
        r5 = 0x1;
        r26 = r6 + 0x8;
        r26 = r29 + r26;
        ((void(*)(void))fn_8009F2F8)();
        r24 = 0x0;
        while (1) {
            tmp = *(u32*)((u8*)r31 + 0x6C);
            if (r24 >= tmp) break;
            r3 = r31 + r24;
            tmp = *(u8*)((u8*)r3 + 0x70);
            if ((s32)tmp != 0) {
            } else {

                r4 = *(u32*)((u8*)r23 + 0x0);
                r3 = r26;
                r5 = *(u32*)((u8*)r23 + 0x4);
                r6 = *(u32*)((u8*)r23 + 0x8);
                r7 = *(u32*)((u8*)r31 + 0x9C);
                fn_801E5548();
                *(u32*)((u8*)r31 + 0xAC) = r3;
                if ((s32)r3 != 0) {
                    tmp = lbl_8047B48C;
                    if ((s32)tmp != 0) {
                        r3 = 0x0;
                        fn_801E446C();
                        tmp = 0x0;
                        lbl_8047B48C = tmp;
                    }
                    r3 = r30 + 0x1058;
                    fn_800A221C();
                }
                *(u32*)((u8*)r23 + 0xC) = r27;
                r4 = r23;
                r3 = r30 + 0x18;
                r5 = 0x1;
                ((void(*)(void))fn_8009F230)();
                r3 = OSDisableInterrupts();
                r4 = *(u32*)((u8*)r31 + 0xD8);
                tmp = r4 + 0x1;
                *(u32*)((u8*)r31 + 0xD8) = tmp;
                OSRestoreInterrupts(r3);
            }
            tmp = *(u32*)((u8*)r25 + 0x0);
            r25 = r25 + 0x4;
            r24 = r24 + 0x1;
            r26 = r26 + tmp;


        }
        tmp = lbl_8047B48C;
        if ((s32)tmp != 0) {
            r3 = 0x1;
            fn_801E446C();
            tmp = 0x0;
            lbl_8047B48C = tmp;
        }
        tmp = *(u32*)((u8*)r31 + 0xC0);
        r5 = *(u32*)((u8*)r31 + 0x50);
        r4 = r27 + tmp;
        r3 = (u32)r4 / (u32)r5;
        r3 = r3 * r5;
        r3 = r4 - r3;
        if (r3 == tmp) {
            tmp = *(u8*)((u8*)r31 + 0xA6);
            tmp = tmp & 0x1;
            if (r3 != tmp) {
                r28 = *(u32*)((u8*)r29 + 0x0);
                r29 = *(u32*)((u8*)r31 + 0xB4);
                goto L_801E514C;
            }
            r3 = r30 + 0x1058;
            fn_800A221C();
            goto L_801E514C;
        }
        tmp = *(u32*)((u8*)r29 + 0x0);
        r29 = r29 + r28;
        r28 = tmp;
    L_801E514C:
        r27 = r27 + 0x1;
    } while (1);
}
#endif

/* 0x801E5154 | size: 0x2AC | large */
#if 0
asm void fn_801E5154(void) {
#include "src/game/battle/battle_logic_fn_801E5154.inc"
}
#else
void fn_801E5154(void) {
    extern u8 lbl_8046AC60[];
    extern u8 lbl_8046C190[];
    extern u32 lbl_8047B48C;
    extern void fn_800A221C();
    extern void fn_801E1B84();
    extern void fn_801E1BB8();
    extern void fn_801E1BE8();
    extern void fn_801E446C();
    extern void fn_801E5548();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r4 = (u32)lbl_8046C190;
    r3 = (u32)lbl_8046AC60;
    r30 = (u32)lbl_8046C190;
    r31 = (u32)lbl_8046AC60;
    do {
        tmp = *(u8*)((u8*)r31 + 0xA7);
        if (tmp != 0) {
            while (1) {
                tmp = *(u32*)((u8*)r31 + 0xD8);
                if ((s32)tmp >= 0) break;
                fn_801E1B84();
                r28 = r3;
                r5 = *(u32*)((u8*)r31 + 0x50);
                r3 = *(u32*)((u8*)r31 + 0xC0);
                r4 = *(u32*)((u8*)r28 + 0x4);
                r4 = r4 + r3;
                r3 = (u32)r4 / (u32)r5;
                r3 = r3 * r5;
                r3 = r4 - r3;
                do {
                    if (r3 != tmp) break;
                    tmp = *(u8*)((u8*)r31 + 0xA6);
                    tmp = tmp & 0x1;
                    if (r3 != tmp) break;
                    tmp = *(u32*)((u8*)r31 + 0x6C);
                    r3 = r30 + 0x38;
                    r7 = *(u32*)((u8*)r28 + 0x0);
                    r4 = (u32)sp + 0xc;
                    r6 = tmp << 2;
                    r5 = 0x1;
                    r26 = r6 + 0x8;
                    r25 = r7 + 0x8;
                    r26 = r7 + r26;
                    ((void(*)(void))fn_8009F2F8)();
                    r3 = (u32)lbl_8046AC60;
                    r27 = (u32)lbl_8046AC60;
                    r24 = 0x0;
                    while (1) {
                        tmp = *(u32*)((u8*)r31 + 0x6C);
                        if (r24 >= tmp) break;
                        r3 = r27 + r24;
                        tmp = *(u8*)((u8*)r3 + 0x70);
                        if ((s32)tmp != 0) {
                        } else {

                            r4 = *(u32*)((u8*)r29 + 0x0);
                            r3 = r26;
                            r5 = *(u32*)((u8*)r29 + 0x4);
                            r6 = *(u32*)((u8*)r29 + 0x8);
                            r7 = *(u32*)((u8*)r31 + 0x9C);
                            fn_801E5548();
                            *(u32*)((u8*)r31 + 0xAC) = r3;
                            if ((s32)r3 != 0) {
                                tmp = lbl_8047B48C;
                                if ((s32)tmp != 0) {
                                    r3 = 0x0;
                                    fn_801E446C();
                                    tmp = 0x0;
                                    lbl_8047B48C = tmp;
                                }
                                r3 = r30 + 0x1058;
                                fn_800A221C();
                            }
                            tmp = *(u32*)((u8*)r28 + 0x4);
                            r4 = r29;
                            r3 = r30 + 0x18;
                            r5 = 0x1;
                            *(u32*)((u8*)r29 + 0xC) = tmp;
                            ((void(*)(void))fn_8009F230)();
                            r3 = OSDisableInterrupts();
                            r4 = *(u32*)((u8*)r31 + 0xD8);
                            tmp = r4 + 0x1;
                            *(u32*)((u8*)r31 + 0xD8) = tmp;
                            OSRestoreInterrupts(r3);
                        }
                        tmp = *(u32*)((u8*)r25 + 0x0);
                        r25 = r25 + 0x4;
                        r24 = r24 + 0x1;
                        r26 = r26 + tmp;


                    }
                    tmp = lbl_8047B48C;
                    if ((s32)tmp == 0) break;
                    r3 = 0x1;
                    fn_801E446C();
                    tmp = 0x0;
                    lbl_8047B48C = tmp;
                } while (0);
                r3 = r28;
                fn_801E1BB8();
                r3 = OSDisableInterrupts();
                r4 = *(u32*)((u8*)r31 + 0xD8);
                tmp = r4 + 0x1;
                *(u32*)((u8*)r31 + 0xD8) = tmp;
                OSRestoreInterrupts(r3);


            }
        }
        tmp = *(u8*)((u8*)r31 + 0xA7);
        if (tmp != 0) {
            fn_801E1B84();
        } else {

            fn_801E1BE8();
        }
        tmp = *(u32*)((u8*)r31 + 0x6C);
        r29 = r3;
        r6 = *(u32*)((u8*)r3 + 0x0);
        r3 = r30 + 0x38;
        r5 = tmp << 2;
        r4 = (u32)sp + 0x8;
        r24 = r5 + 0x8;
        r25 = r6 + 0x8;
        r24 = r6 + r24;
        r5 = 0x1;
        ((void(*)(void))fn_8009F2F8)();
        r26 = 0x0;
        while (1) {
            tmp = *(u32*)((u8*)r31 + 0x6C);
            if (r26 >= tmp) break;
            r3 = r31 + r26;
            tmp = *(u8*)((u8*)r3 + 0x70);
            if ((s32)tmp != 0) {
            } else {

                r4 = *(u32*)((u8*)r27 + 0x0);
                r3 = r24;
                r5 = *(u32*)((u8*)r27 + 0x4);
                r6 = *(u32*)((u8*)r27 + 0x8);
                r7 = *(u32*)((u8*)r31 + 0x9C);
                fn_801E5548();
                *(u32*)((u8*)r31 + 0xAC) = r3;
                if ((s32)r3 != 0) {
                    tmp = lbl_8047B48C;
                    if ((s32)tmp != 0) {
                        r3 = 0x0;
                        fn_801E446C();
                        tmp = 0x0;
                        lbl_8047B48C = tmp;
                    }
                    r3 = r30 + 0x1058;
                    fn_800A221C();
                }
                tmp = *(u32*)((u8*)r29 + 0x4);
                r4 = r27;
                r3 = r30 + 0x18;
                r5 = 0x1;
                *(u32*)((u8*)r27 + 0xC) = tmp;
                ((void(*)(void))fn_8009F230)();
                r3 = OSDisableInterrupts();
                r4 = *(u32*)((u8*)r31 + 0xD8);
                tmp = r4 + 0x1;
                *(u32*)((u8*)r31 + 0xD8) = tmp;
                OSRestoreInterrupts(r3);
            }
            tmp = *(u32*)((u8*)r25 + 0x0);
            r25 = r25 + 0x4;
            r26 = r26 + 0x1;
            r24 = r24 + tmp;


        }
        tmp = lbl_8047B48C;
        if ((s32)tmp != 0) {
            r3 = 0x1;
            fn_801E446C();
            tmp = 0x0;
            lbl_8047B48C = tmp;
        }
        r3 = r29;
        fn_801E1BB8();
    } while (1);
}
#endif

/* 0x801E5400 | size: 0x3C | small */
/* Cancel battle sub-thread 3 if active. */
#if 0
asm void fn_801E5400(void) {
#include "src/game/battle/battle_logic_fn_801E5400.inc"
}
#else
void fn_801E5400(void) {
    extern u8 lbl_8046D1E8[];
    extern u32 lbl_8047B488;

    if ((s32)lbl_8047B488 != 0) {
        OSCancelThread((void*)lbl_8046D1E8);
        lbl_8047B488 = 0;
    }
}
#endif

/* 0x801E543C | size: 0x34
 * Resume battle sub-thread 3 if active.
 */
#if 0
asm void fn_801E543C(void) {
#include "src/game/battle/battle_logic_fn_801E543C.inc"
}
#else
void fn_801E543C(void) {
    extern u8 lbl_8046D1E8[];
    extern u32 lbl_8047B488;
    extern void fn_800A1F94(void* thread);

    if ((s32)lbl_8047B488 != 0) {
        fn_800A1F94((void*)lbl_8046D1E8);
    }
}
#endif

/* 0x801E5470 | size: 0xD8 | medium */
#if 0
asm void fn_801E5470(void) {
#include "src/game/battle/battle_logic_fn_801E5470.inc"
}
#else
void fn_801E5470(void) {
    extern u8 lbl_8046C190[];
    extern u32 lbl_8047B488;
    extern u32 lbl_8047B48C;
    extern void fn_8009F1D0();
    extern void fn_800A19CC();
    extern void fn_801E4F64();
    extern void fn_801E5154();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r31 = 0;

    r5 = (u32)lbl_8046C190;
    r31 = (u32)lbl_8046C190;
    if (r4 != 0) {
        r5 = (u32)fn_801E4F64;
        r6 = r31 + 0x58;
        tmp = (u32)fn_801E4F64;
        r8 = r3;
        r5 = r4;
        r3 = r31 + 0x1058;
        r4 = tmp;
        r7 = 0x1000;
        r9 = 0x1;
        r6 = r6 + 0x1000;
        fn_800A19CC();
    } else {
        r4 = (u32)fn_801E5154;
        r6 = r31 + 0x58;
        r8 = r3;
        r3 = r31 + 0x1058;
        r4 = (u32)fn_801E5154;
        r5 = 0x0;
        r7 = 0x1000;
        r9 = 0x1;
        r6 = r6 + 0x1000;
        fn_800A19CC();
    }
    if ((s32)r3 == 0) {
        r3 = 0x0;
        return;
    }
    r3 = r31 + 0x38;
    r4 = r31 + 0xc;
    r5 = 0x3;
    fn_8009F1D0();
    r3 = r31 + 0x18;
    r4 = r31 + 0x0;
    r5 = 0x3;
    fn_8009F1D0();
    tmp = 0x1;
    r3 = 0x1;
    lbl_8047B488 = tmp;
    lbl_8047B48C = tmp;

    return;
}
#endif

/* 0x801E5548 | size: 0x244 | large */
#if 0
asm void fn_801E5548(void) {
#include "src/game/battle/battle_logic_fn_801E5548.inc"
}
#else
void fn_801E5548(void) {
    extern u32 lbl_8047B5AC;
    extern u32 lbl_8047B5B0;
    extern u32 lbl_8047B5B4;
    extern void fn_8009B388();
    extern void fn_801E578C();
    extern void fn_801E57D0();
    extern void fn_801E590C();
    extern void fn_801E5A28();
    extern void fn_801E5DE4();
    extern void fn_801E62D8();
    extern void fn_801E6578();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* mr. r27, r3 */;
    r28 = r4 + 0x0;
    r29 = r5 + 0x0;
    r30 = r6 + 0x0;
    r31 = r7 + 0x0;
    if ((s32)tmp == 0) { r3 = 0x19; return; }
    if (r28 == 0) { r3 = 0x1b; return; }
    if (r29 == 0) { r3 = 0x1b; return; }
    if (r30 == 0) { r3 = 0x1b; return; }
    if (r31 == 0) { r3 = 0x1a; return; }
    PPCMfhid2();
    tmp = r3 & 0x10000000;
    if (r31 == 0) { r3 = 0x1c; return; }
    tmp = lbl_8047B5B4;
    if ((s32)tmp == 0) { r3 = 0x1d; return; }
    lbl_8047B5AC = r31;
    r4 = 0x6bc;
    r3 = lbl_8047B5AC;
    tmp = r3 + 0x1f;
    /* clrrwi r3, tmp, 5 */;
    lbl_8047B5B0 = r3;
    tmp = r3 + 0x6bc;
    lbl_8047B5AC = tmp;
    r3 = lbl_8047B5B0;
    fn_8009B388();
    r3 = lbl_8047B5B0;
    r4 = 0x21;
    tmp = 0x0;
    *(u32*)((u8*)r3 + 0x6A4) = r4;
    r31 = 0x0;
    r3 = lbl_8047B5B0;
    *(u16*)((u8*)r3 + 0x698) = tmp;
    r3 = lbl_8047B5B0;
    *(u32*)((u8*)r3 + 0x69C) = r27;
    do {
    do {
        r4 = lbl_8047B5B0;
        r3 = *(u32*)((u8*)r4 + 0x69C);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r4 + 0x69C) = tmp;
        tmp = *(u8*)((u8*)r3 + 0x0);
        if (tmp != 0xff) { r3 = 0x3; return; }
        while (1) {
            r3 = lbl_8047B5B0;
            r4 = r3 + 0x69c;
            r3 = *(u32*)((u8*)r3 + 0x69C);
            tmp = *(u8*)((u8*)r3 + 0x0);
            if (tmp != 0xff) break;
            r3 = *(u32*)((u8*)r4 + 0x0);
            tmp = r3 + 0x1;
            *(u32*)((u8*)r4 + 0x0) = tmp;


        }
        tmp = r3 + 0x1;
        *(u32*)((u8*)r4 + 0x0) = tmp;
        tmp = *(u8*)((u8*)r3 + 0x0);
        if (tmp <= 0xd7) {
            if (tmp == 0xc4) {
                fn_801E5DE4();
                tmp = r3 & 0xFF;
                if (tmp != 0xc4) { r3 = r3 & 0xFF; return; }
                break;
            }
            if (tmp != 0xc0) { r3 = 0xb; return; }
            fn_801E57D0();
            tmp = r3 & 0xFF;
            if (tmp != 0xc0) { r3 = r3 & 0xFF; return; }
            break;
        }
        if (tmp >= 0xd8 && tmp <= 0xdf) {

            if (tmp == 0xdd) {
                fn_801E62D8();

            } else if (tmp == 0xdb) {
                fn_801E5A28();
                tmp = r3 & 0xFF;
                if (tmp != 0xdb) { r3 = r3 & 0xFF; return; }

            } else if (tmp == 0xda) {
                fn_801E590C();
                tmp = r3 & 0xFF;
                if (tmp != 0xda) { r3 = r3 & 0xFF; return; }
                r31 = 0x1;

            }
            if (tmp == 0xd8) break;
            r3 = 0xb;
            return;
        }
        if (tmp < 0xe0) break;
        if (tmp >= 0xe0) {
            if (tmp > 0xef) {
            }
            if (tmp != 0xfe) { r3 = 0xb; return; }
            }
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r3 + 0x69C);
        r5 = r3 + 0x69c;
        r3 = *(u8*)((u8*)r4 + 0x0);
        tmp = *(u8*)((u8*)r4 + 0x1);
        tmp = (tmp & ~0x0000FF00) | (((r3 << 8) | (r3 >> 24)) & 0x0000FF00);
        tmp = r4 + tmp;
        *(u32*)((u8*)r5 + 0x0) = tmp;
    } while (0);
        tmp = r31 & 0xFF;
    } while (tmp == 0xfe);
    fn_801E578C();
    r3 = r28 + 0x0;
    r4 = r29 + 0x0;
    r5 = r30 + 0x0;
    fn_801E6578();
    r3 = 0x0;
    return;


}
#endif

/* 0x801E578C | size: 0x44 | small */
#if 0
asm void fn_801E578C(void) {
#include "src/game/battle/battle_logic_fn_801E578C.inc"
}
#else
void fn_801E578C(void) {
    extern u8 lbl_8046D618[];
    extern u32 lbl_8047B5AC;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;

    r4 = lbl_8047B5AC;
    r3 = (u32)lbl_8046D618;
    r5 = (u32)lbl_8046D618;
    tmp = r4 + 0x1f;
    /* clrrwi r6, tmp, 5 */;
    *(u32*)((u8*)r5 + 0x0) = r6;
    r3 = r6 + 0x80;
    tmp = r6 + 0x100;
    *(u32*)((u8*)r5 + 0x4) = r3;
    r4 = r6 + 0x180;
    r3 = r6 + 0x200;
    *(u32*)((u8*)r5 + 0x8) = tmp;
    tmp = r6 + 0x280;
    *(u32*)((u8*)r5 + 0xC) = r4;
    *(u32*)((u8*)r5 + 0x10) = r3;
    *(u32*)((u8*)r5 + 0x14) = tmp;
    return;
}
#endif

/* 0x801E57D0 | size: 0x13C | medium */
#if 0
asm void fn_801E57D0(void) {
#include "src/game/battle/battle_logic_fn_801E57D0.inc"
}
#else
void fn_801E57D0(void) {
    extern u32 lbl_8047B5B0;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;

    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x2;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x1;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    tmp = *(u8*)((u8*)r3 + 0x0);
    if (tmp != 8) {
        r3 = 0xa;
        return;
    }
    r5 = lbl_8047B5B0;
    r4 = *(u32*)((u8*)r5 + 0x69C);
    r3 = *(u8*)((u8*)r4 + 0x0);
    tmp = *(u8*)((u8*)r4 + 0x1);
    tmp = (tmp & ~0x0000FF00) | (((r3 << 8) | (r3 >> 24)) & 0x0000FF00);
    *(u16*)((u8*)r5 + 0x694) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x2;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    r5 = lbl_8047B5B0;
    r4 = *(u32*)((u8*)r5 + 0x69C);
    r3 = *(u8*)((u8*)r4 + 0x0);
    tmp = *(u8*)((u8*)r4 + 0x1);
    tmp = (tmp & ~0x0000FF00) | (((r3 << 8) | (r3 >> 24)) & 0x0000FF00);
    *(u16*)((u8*)r5 + 0x692) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x2;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x1;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    tmp = *(u8*)((u8*)r3 + 0x0);
    if (tmp != 3) {
        r3 = 0xc;
        return;
    }
    r7 = 0x0;
    r6 = 0x0;
    while (1) {
        tmp = r7 & 0xFF;
        if (tmp >= 3) break;
        r4 = lbl_8047B5B0;
        tmp = r7 & 0xFF;
        r3 = *(u32*)((u8*)r4 + 0x69C);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r4 + 0x69C) = tmp;
        r4 = lbl_8047B5B0;
        r3 = *(u32*)((u8*)r4 + 0x69C);
        tmp = r3 + 0x1;
        *(u32*)((u8*)r4 + 0x69C) = tmp;
        r3 = *(u8*)((u8*)r3 + 0x0);
        if (tmp == 3) {
            if (r3 != 0x22) { r3 = 0x13; return; }
        }
        tmp = r7 & 0xFF;
        if ((r3 != 0x22) && (r3 != 0x11)) {


            r3 = 0x13;
            return;
        }
        r5 = lbl_8047B5B0;
        tmp = r6 + 0x680;
        r6 = r6 + 0x6;
        r4 = *(u32*)((u8*)r5 + 0x69C);
        r7 = r7 + 0x1;
        r3 = r4 + 0x1;
        *(u32*)((u8*)r5 + 0x69C) = r3;
        r4 = *(u8*)((u8*)r4 + 0x0);
        r3 = lbl_8047B5B0;
        *(u8*)(r3 + tmp) = r4;


    }
    r3 = 0x0;
    return;
}
#endif

/* 0x801E590C | size: 0x11C | medium */
#if 0
asm void fn_801E590C(void) {
#include "src/game/battle/battle_logic_fn_801E590C.inc"
}
#else
void fn_801E590C(void) {
    extern u32 lbl_8047B5B0;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;

    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x2;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x1;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    tmp = *(u8*)((u8*)r3 + 0x0);
    if (tmp != 3) {
        r3 = 0xc;
        return;
    }
    r9 = 0x0;
    r8 = 0x0;
    while (1) {
        tmp = r9 & 0xFF;
        if (tmp >= 3) break;
        r6 = lbl_8047B5B0;
        tmp = r8 + 0x681;
        r4 = r8 + 0x682;
        r5 = *(u32*)((u8*)r6 + 0x69C);
        r3 = 0x1;
        r5 = r5 + 0x1;
        *(u32*)((u8*)r6 + 0x69C) = r5;
        r7 = lbl_8047B5B0;
        r6 = *(u32*)((u8*)r7 + 0x69C);
        r5 = r6 + 0x1;
        *(u32*)((u8*)r7 + 0x69C) = r5;
        r7 = *(u8*)((u8*)r6 + 0x0);
        r5 = lbl_8047B5B0;
        r6 = (s32)r7 >> 4;
        *(u8*)(r5 + tmp) = r6;
        r7 = r7 & 0xF;
        tmp = r3 << r6;
        r5 = lbl_8047B5B0;
        *(u8*)(r5 + r4) = r7;
        r4 = lbl_8047B5B0;
        r4 = *(u8*)((u8*)r4 + 0x6A8);
        /* and. tmp, r4, tmp */;
        if (tmp == 3) {
            r3 = 0xf;
            return;
        }
        tmp = r7 + 0x1;
        tmp = r3 << tmp;
        /* and. tmp, r4, tmp */;
        if (tmp == 3) {
            r3 = 0xf;
            return;
        }
        r8 = r8 + 0x6;
        r9 = r9 + 0x1;


    }
    r5 = lbl_8047B5B0;
    tmp = 0x0;
    r3 = 0x0;
    r4 = *(u32*)((u8*)r5 + 0x69C);
    r4 = r4 + 0x3;
    *(u32*)((u8*)r5 + 0x69C) = r4;
    r5 = lbl_8047B5B0;
    r4 = *(u16*)((u8*)r5 + 0x692);
    r4 = r4 + 0xf;
    r4 = (s32)r4 >> 4;
    *(u16*)((u8*)r5 + 0x696) = r4;
    r4 = lbl_8047B5B0;
    *(u16*)((u8*)r4 + 0x684) = tmp;
    r4 = lbl_8047B5B0;
    *(u16*)((u8*)r4 + 0x68A) = tmp;
    r4 = lbl_8047B5B0;
    *(u16*)((u8*)r4 + 0x690) = tmp;
    return;
}
#endif

/* 0x801E5A28 | size: 0x3BC | large */
#if 0
asm void fn_801E5A28(void) {
#include "src/game/battle/battle_logic_fn_801E5A28.inc"
}
#else
void fn_801E5A28(void) {
    extern u8 lbl_80279AE8[];
    extern u32 lbl_8047B5B0;
    extern f64 lbl_8047E4B0;
    u8 sp[0x188];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;

    r4 = (u32)lbl_80279AE8;
    r4 = (u32)lbl_80279AE8;
    r6 = r4 + 0x50;
    r3 = lbl_8047B5B0;
    r5 = *(u32*)((u8*)r3 + 0x69C);
    r8 = r3 + 0x69c;
    r3 = (u32)sp + 0x14;
    r7 = *(u8*)((u8*)r5 + 0x0);
    tmp = r5 + 0x2;
    r5 = *(u8*)((u8*)r5 + 0x1);
    r5 = (r5 & ~0x0000FF00) | (((r7 << 8) | (r7 >> 24)) & 0x0000FF00);
    *(u32*)((u8*)r8 + 0x0) = tmp;
    r7 = r5 & 0xFFFF;
    tmp = 0x43300000;
    f0 = lbl_8047E4B0;
    do {
        r11 = lbl_8047B5B0;
        r5 = r4 + 0x0;
        r9 = 0x0;
        r10 = *(u32*)((u8*)r11 + 0x69C);
        r8 = r10 + 0x1;
        *(u32*)((u8*)r11 + 0x69C) = r8;
        r8 = *(u8*)((u8*)r10 + 0x0);
        while (1) {
            r10 = r9 & 0xFFFF;
            if (r10 >= 0x40) break;
            r30 = lbl_8047B5B0;
            r28 = r9 + 0x1;
            r29 = r9 + 0x2;
            r27 = *(u32*)((u8*)r30 + 0x69C);
            r12 = r9 + 0x3;
            r11 = r9 + 0x4;
            r10 = r27 + 0x1;
            *(u32*)((u8*)r30 + 0x69C) = r10;
            r10 = r9 + 0x5;
            r28 = r28 & 0xFFFF;
            r30 = *(u8*)((u8*)r27 + 0x0);
            r29 = r29 & 0xFFFF;
            r27 = *(u8*)((u8*)r5 + 0x0);
            r12 = r12 & 0xFFFF;
            r27 = r27 << 2;
            *(u32*)(sp + 0x150) = tmp;
            r11 = r11 & 0xFFFF;
            r10 = r10 & 0xFFFF;
            f1 = f1 - f0;
            *(f32*)(r3 + r27) = f1;
            r27 = lbl_8047B5B0;
            r31 = *(u32*)((u8*)r27 + 0x69C);
            r30 = r31 + 0x1;
            *(u32*)((u8*)r27 + 0x69C) = r30;
            r30 = *(u8*)((u8*)r31 + 0x0);
            r28 = *(u8*)(r4 + r28);
            r28 = r28 << 2;
            *(u32*)(sp + 0x148) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r28) = f1;
            r28 = lbl_8047B5B0;
            r31 = *(u32*)((u8*)r28 + 0x69C);
            r30 = r31 + 0x1;
            *(u32*)((u8*)r28 + 0x69C) = r30;
            r30 = *(u8*)((u8*)r31 + 0x0);
            r29 = *(u8*)(r4 + r29);
            r29 = r29 << 2;
            *(u32*)(sp + 0x140) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r29) = f1;
            r31 = lbl_8047B5B0;
            r30 = *(u32*)((u8*)r31 + 0x69C);
            r29 = r30 + 0x1;
            *(u32*)((u8*)r31 + 0x69C) = r29;
            r29 = *(u8*)((u8*)r30 + 0x0);
            r12 = *(u8*)(r4 + r12);
            r12 = r12 << 2;
            *(u32*)(sp + 0x138) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r12) = f1;
            r30 = lbl_8047B5B0;
            r29 = *(u32*)((u8*)r30 + 0x69C);
            r12 = r29 + 0x1;
            *(u32*)((u8*)r30 + 0x69C) = r12;
            r12 = *(u8*)((u8*)r29 + 0x0);
            r11 = *(u8*)(r4 + r11);
            r11 = r11 << 2;
            *(u32*)(sp + 0x130) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r11) = f1;
            r29 = lbl_8047B5B0;
            r12 = *(u32*)((u8*)r29 + 0x69C);
            r11 = r12 + 0x1;
            *(u32*)((u8*)r29 + 0x69C) = r11;
            r11 = *(u8*)((u8*)r12 + 0x0);
            r10 = *(u8*)(r4 + r10);
            r10 = r10 << 2;
            *(u32*)(sp + 0x128) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r10) = f1;
            r30 = lbl_8047B5B0;
            r11 = r9 + 0x6;
            r10 = r9 + 0x7;
            r29 = *(u32*)((u8*)r30 + 0x69C);
            r11 = r11 & 0xFFFF;
            r10 = r10 & 0xFFFF;
            r12 = r29 + 0x1;
            *(u32*)((u8*)r30 + 0x69C) = r12;
            r5 = r5 + 0x8;
            r9 = r9 + 0x8;
            r12 = *(u8*)((u8*)r29 + 0x0);
            r11 = *(u8*)(r4 + r11);
            r11 = r11 << 2;
            *(u32*)(sp + 0x120) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r11) = f1;
            r29 = lbl_8047B5B0;
            r12 = *(u32*)((u8*)r29 + 0x69C);
            r11 = r12 + 0x1;
            *(u32*)((u8*)r29 + 0x69C) = r11;
            r11 = *(u8*)((u8*)r12 + 0x0);
            r10 = *(u8*)(r4 + r10);
            r10 = r10 << 2;
            *(u32*)(sp + 0x118) = tmp;
            f1 = f1 - f0;
            *(f32*)(r3 + r10) = f1;


        }
        r29 = lbl_8047B5B0;
        r27 = r6 + 0x0;
        r28 = r8 << 8;
        r5 = 0x0;
        r8 = 0x0;
        while (1) {
            r9 = r8 & 0xFFFF;
            if (r9 >= 8) break;
            f1 = *(f64*)((u8*)r27 + 0x0);
            f2 = *(f32*)(r3 + r11);
            r9 = r5 + 0x1;
            f3 = *(f64*)((u8*)r6 + 0x0);
            f1 = f2 * f1;
            r9 = r5 + 0x2;
            r9 = r5 + 0x3;
            f1 = f3 * f1;
            r10 = r5 + 0x4;
            f1 = (f32)f1;
            r9 = r29 + r11;
            r10 = r5 + 0x7;
            *(f32*)(r28 + r9) = f1;
            r9 = r5 + 0x5;
            f2 = *(f32*)(r3 + r12);
            r9 = r5 + 0x6;
            f1 = *(f64*)((u8*)r27 + 0x0);
            r9 = r29 + r12;
            f1 = f2 * f1;
            f3 = *(f64*)((u8*)r6 + 0x8);
            r30 = r29 + r26;
            r31 = r29 + r25;
            f1 = f3 * f1;
            r12 = r29 + r24;
            r11 = r29 + r23;
            r10 = r29 + r22;
            r5 = r5 + 0x8;
            f1 = (f32)f1;
            r8 = r8 + 0x1;
            *(f32*)(r28 + r9) = f1;
            r9 = r29 + r21;
            f2 = *(f32*)(r3 + r26);
            f1 = *(f64*)((u8*)r27 + 0x0);
            f3 = *(f64*)((u8*)r6 + 0x10);
            f1 = f2 * f1;
            f1 = f3 * f1;
            f1 = (f32)f1;
            *(f32*)(r28 + r30) = f1;
            f2 = *(f32*)(r3 + r25);
            f1 = *(f64*)((u8*)r27 + 0x0);
            f3 = *(f64*)((u8*)r6 + 0x18);
            f1 = f2 * f1;
            f1 = f3 * f1;
            f1 = (f32)f1;
            *(f32*)(r28 + r31) = f1;
            f2 = *(f32*)(r3 + r24);
            f1 = *(f64*)((u8*)r27 + 0x0);
            f3 = *(f64*)((u8*)r6 + 0x20);
            f1 = f2 * f1;
            f1 = f3 * f1;
            f1 = (f32)f1;
            *(f32*)(r28 + r12) = f1;
            f2 = *(f32*)(r3 + r23);
            f1 = *(f64*)((u8*)r27 + 0x0);
            f3 = *(f64*)((u8*)r6 + 0x28);
            f1 = f2 * f1;
            f1 = f3 * f1;
            f1 = (f32)f1;
            *(f32*)(r28 + r11) = f1;
            f2 = *(f32*)(r3 + r22);
            f1 = *(f64*)((u8*)r27 + 0x0);
            f3 = *(f64*)((u8*)r6 + 0x30);
            f1 = f2 * f1;
            f1 = f3 * f1;
            f1 = (f32)f1;
            *(f32*)(r28 + r10) = f1;
            f1 = *(f64*)((u8*)r27 + 0x0);
            r27 = r27 + 0x8;
            f2 = *(f32*)(r3 + r21);
            f3 = *(f64*)((u8*)r6 + 0x38);
            f1 = f2 * f1;
            f1 = f3 * f1;
            f1 = (f32)f1;
            *(f32*)(r28 + r9) = f1;


        }
        r5 = r7 & 0xFFFF;
    } while (r9 != 8);
    r3 = 0x0;
    return;
}
#endif

/* 0x801E5DE4 | size: 0x1E0 | medium */
#if 0
asm void fn_801E5DE4(void) {
#include "src/game/battle/battle_logic_fn_801E5DE4.inc"
}
#else
void fn_801E5DE4(void) {
    extern u32 lbl_8047B544;
    extern u32 lbl_8047B548;
    extern u32 lbl_8047B54C;
    extern u32 lbl_8047B5AC;
    extern u32 lbl_8047B5B0;
    extern void fn_801E5FC4();
    extern void fn_801E60B4();
    extern void fn_801E611C();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r4 = lbl_8047B5AC;
    r3 = lbl_8047B5B0;
    tmp = r4 + 0x101;
    lbl_8047B548 = r4;
    r5 = r3 + 0x69c;
    lbl_8047B54C = tmp;
    r3 = *(u32*)((u8*)r3 + 0x69C);
    r4 = *(u8*)((u8*)r3 + 0x0);
    tmp = r3 + 0x2;
    r3 = *(u8*)((u8*)r3 + 0x1);
    r3 = (r3 & ~0x0000FF00) | (((r4 << 8) | (r4 >> 24)) & 0x0000FF00);
    r30 = r3 & 0xFFFF;
    *(u32*)((u8*)r5 + 0x0) = tmp;
    do {
        r5 = lbl_8047B5B0;
        r29 = 0x0;
        r3 = 0x0;
        r4 = *(u32*)((u8*)r5 + 0x69C);
        tmp = r4 + 0x1;
        *(u32*)((u8*)r5 + 0x69C) = tmp;
        r6 = *(u8*)((u8*)r4 + 0x0);
        r4 = lbl_8047B5B0;
        tmp = (s32)r6 >> 4;
        r5 = *(u32*)((u8*)r4 + 0x69C);
        tmp = tmp & 0xFF;
        tmp = r4 + tmp;
        lbl_8047B544 = r5;
        r31 = tmp & 0xFF;
        while (1) {
            tmp = r3 & 0xFF;
            if (tmp >= 0x10) break;
            r5 = lbl_8047B5B0;
            r3 = r3 + 0x8;
            r4 = *(u32*)((u8*)r5 + 0x69C);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            r5 = lbl_8047B5B0;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r4 = *(u32*)((u8*)r5 + 0x69C);
            r29 = r29 + tmp;
            tmp = r4 + 0x1;
            *(u32*)((u8*)r5 + 0x69C) = tmp;
            tmp = *(u8*)((u8*)r4 + 0x0);
            r29 = r29 + tmp;


        }
        r3 = r31 * 0xe0;
        r5 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r5 + 0x69C);
        tmp = r3 + 0x340;
        *(u32*)(r5 + tmp) = r4;
        tmp = r29 & 0xFFFF;
        r4 = lbl_8047B5B0;
        r3 = *(u32*)((u8*)r4 + 0x69C);
        tmp = r3 + tmp;
        *(u32*)((u8*)r4 + 0x69C) = tmp;
        fn_801E5FC4();
        fn_801E60B4();
        r3 = r31;
        fn_801E611C();
        r5 = lbl_8047B5B0;
        tmp = r29 + 0x11;
        r30 = r30 - tmp;
        tmp = 0x1;
        r4 = *(u8*)((u8*)r5 + 0x6A8);
        r3 = tmp << r31;
        r3 = r4 | r3;
        tmp = r30 & 0xFFFF;
        *(u8*)((u8*)r5 + 0x6A8) = r3;
    } while (tmp != 0x10);
    r3 = 0x0;
    return;
}
#endif

/* 0x801E5FC4 | size: 0xF0 | medium */
#if 0
asm void fn_801E5FC4(void) {
#include "src/game/battle/battle_logic_fn_801E5FC4.inc"
}
#else
void fn_801E5FC4(void) {
    extern u32 lbl_8047B544;
    extern u32 lbl_8047B548;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r6 = 0x0;
    r7 = 0x1;
    do {
        r3 = lbl_8047B544;
        r5 = r7 & 0xFF;
        r8 = *(u8*)(r3 + tmp);
        r3 = r8 + 0x0;
        do {
            if ((s32)r8 == 0) break;
            /* srwi. tmp, r3, 3 */;
            ctr_fn = (void(*)(void))tmp;
            if ((s32)r8 != 0) {
                do {
                    r4 = lbl_8047B548;
                    tmp = r6;
                    r6 = r6 + 0x1;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                    tmp = r6 + 0x0;
                    r6 = r6 + 0x1;
                    r4 = lbl_8047B548;
                    *(u8*)(r4 + tmp) = r5;
                } while (--ctr != 0);
                r3 = r3 & 0x7;
                if ((s32)r8 == 0) break;
            }
            ctr_fn = (void(*)(void))r3;
            do {
                r4 = lbl_8047B548;
                tmp = r6;
                r6 = r6 + 0x1;
                *(u8*)(r4 + tmp) = r5;
            } while (--ctr != 0);
        } while (0);
        r7 = r7 + 0x1;
    } while ((s32)r7 <= 0x10);
    r3 = lbl_8047B548;
    tmp = 0x0;
    *(u8*)(r3 + r6) = tmp;
    return;
}
#endif

/* 0x801E60B4 | size: 0x68 | small */
#if 0
asm void fn_801E60B4(void) {
#include "src/game/battle/battle_logic_fn_801E60B4.inc"
}
#else
void fn_801E60B4(void) {
    extern u32 lbl_8047B548;
    extern u32 lbl_8047B54C;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;

    r6 = lbl_8047B548;
    r8 = 0x0;
    r5 = lbl_8047B54C;
    r9 = 0x0;
    r7 = *(u8*)((u8*)r6 + 0x0);
    r3 = 0x1;
    while (1) {
        tmp = r8 & 0xFFFF;
        tmp = *(u8*)(r6 + tmp);
        if (tmp == 0) break;
        r4 = r7 & 0xFF;
        while (1) {
            tmp = r8 & 0xFFFF;
            tmp = *(u8*)(r6 + tmp);
            if (r4 != tmp) break;
            *(u16*)(r5 + tmp) = r9;
            r8 = r8 + 0x1;
            r9 = r9 + 0x1;


        }
        tmp = r9 & 0xFFFF;
        tmp = tmp << r3;
        r9 = tmp & 0xFFFF;
        r7 = r7 + 0x1;


    }
    return;
}
#endif

/* 0x801E611C | size: 0x1BC | medium */
#if 0
asm void fn_801E611C(void) {
#include "src/game/battle/battle_logic_fn_801E611C.inc"
}
#else
void fn_801E611C(void) {
    extern u32 lbl_8047B544;
    extern u32 lbl_8047B54C;
    extern u32 lbl_8047B5B0;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    tmp = r3 & 0xFF;
    r4 = lbl_8047B5B0;
    r3 = tmp * 0xe0;
    r7 = r3 + 0x300;
    tmp = 0x4;
    r7 = r4 + r7;
    ctr_fn = (void(*)(void))tmp;
    r6 = r7 + 0x4;
    r8 = 0x0;
    r9 = 0x1;
    do {
        r3 = lbl_8047B544;
        tmp = *(u8*)(r3 + r5);
        if (tmp != 0) {
            r3 = lbl_8047B54C;
            tmp = r8 << 1;
            tmp = *(u16*)(r3 + tmp);
            tmp = r8 - tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
            r3 = lbl_8047B544;
            r4 = lbl_8047B54C;
            tmp = *(u8*)(r3 + r5);
            r8 = r8 + tmp;
            r3 = r8 << 1;
            tmp = *(u16*)(r4 + tmp);
            *(u32*)((u8*)r6 + 0x44) = tmp;
        } else {

            tmp = -0x1;
            *(u32*)((u8*)r6 + 0x44) = tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
        }
        r3 = lbl_8047B544;
        r5 = r9 + 0x0;
        r9 = r9 + 0x1;
        tmp = *(u8*)(r3 + r5);
        r6 = r6 + 0x4;
        if (tmp != 0) {
            r3 = lbl_8047B54C;
            tmp = r8 << 1;
            tmp = *(u16*)(r3 + tmp);
            tmp = r8 - tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
            r3 = lbl_8047B544;
            r4 = lbl_8047B54C;
            tmp = *(u8*)(r3 + r5);
            r8 = r8 + tmp;
            r3 = r8 << 1;
            tmp = *(u16*)(r4 + tmp);
            *(u32*)((u8*)r6 + 0x44) = tmp;
        } else {

            tmp = -0x1;
            *(u32*)((u8*)r6 + 0x44) = tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
        }
        r3 = lbl_8047B544;
        r5 = r9 + 0x0;
        r9 = r9 + 0x1;
        tmp = *(u8*)(r3 + r5);
        r6 = r6 + 0x4;
        if (tmp != 0) {
            r3 = lbl_8047B54C;
            tmp = r8 << 1;
            tmp = *(u16*)(r3 + tmp);
            tmp = r8 - tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
            r3 = lbl_8047B544;
            r4 = lbl_8047B54C;
            tmp = *(u8*)(r3 + r5);
            r8 = r8 + tmp;
            r3 = r8 << 1;
            tmp = *(u16*)(r4 + tmp);
            *(u32*)((u8*)r6 + 0x44) = tmp;
        } else {

            tmp = -0x1;
            *(u32*)((u8*)r6 + 0x44) = tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
        }
        r3 = lbl_8047B544;
        r5 = r9 + 0x0;
        r9 = r9 + 0x1;
        tmp = *(u8*)(r3 + r5);
        r6 = r6 + 0x4;
        if (tmp != 0) {
            r3 = lbl_8047B54C;
            tmp = r8 << 1;
            tmp = *(u16*)(r3 + tmp);
            tmp = r8 - tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
            r3 = lbl_8047B544;
            r4 = lbl_8047B54C;
            tmp = *(u8*)(r3 + r5);
            r8 = r8 + tmp;
            r3 = r8 << 1;
            tmp = *(u16*)(r4 + tmp);
            *(u32*)((u8*)r6 + 0x44) = tmp;
        } else {

            tmp = -0x1;
            *(u32*)((u8*)r6 + 0x44) = tmp;
            *(u32*)((u8*)r6 + 0x8C) = tmp;
        }
        r6 = r6 + 0x4;
        r9 = r9 + 0x1;
    } while (--ctr != 0);
    r3 = 0x100000;
    *(u32*)((u8*)r7 + 0x88) = tmp;
    return;
}
#endif

/* 0x801E62D8 | size: 0x54 | small */
#if 0
asm void fn_801E62D8(void) {
#include "src/game/battle/battle_logic_fn_801E62D8.inc"
}
#else
void fn_801E62D8(void) {
    extern u32 lbl_8047B5B0;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    r3 = lbl_8047B5B0;
    tmp = 0x1;
    *(u8*)((u8*)r3 + 0x6A9) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x2;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    r5 = lbl_8047B5B0;
    r4 = *(u32*)((u8*)r5 + 0x69C);
    r3 = *(u8*)((u8*)r4 + 0x0);
    tmp = *(u8*)((u8*)r4 + 0x1);
    tmp = (tmp & ~0x0000FF00) | (((r3 << 8) | (r3 >> 24)) & 0x0000FF00);
    *(u16*)((u8*)r5 + 0x6AA) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x69C);
    tmp = r3 + 0x2;
    *(u32*)((u8*)r4 + 0x69C) = tmp;
    r3 = lbl_8047B5B0;
    tmp = *(u16*)((u8*)r3 + 0x6AA);
    *(u16*)((u8*)r3 + 0x6AC) = tmp;
    return;
}
#endif

/* 0x801E632C | size: 0x24C | large */
#if 0
asm void fn_801E632C(void) {
#include "src/game/battle/battle_logic_fn_801E632C.inc"
}
#else
void fn_801E632C(void) {
    extern u32 lbl_8047B4A0;
    extern u32 lbl_8047B4C0;
    extern u32 lbl_8047B4E0;
    extern u32 lbl_8047B500;
    extern u32 lbl_8047B520;
    extern u32 lbl_8047B540;
    extern u32 lbl_8047B5B0;
    u8 sp[0x18];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = lbl_8047B5B0;
    tmp = *(u32*)((u8*)r3 + 0x69C);
    r4 = r3 + 0x6a4;
    r3 = *(u32*)((u8*)r3 + 0x6A4);
    /* clrrwi r5, tmp, 2 */;
    tmp = tmp & 0x3;
    if (r3 != 0x21) {
        tmp = 0x3 - tmp;
        tmp = tmp << 3;
        tmp = r3 - tmp;
        *(u32*)((u8*)r4 + 0x0) = tmp;
    } else {

        r3 = tmp << 3;
        tmp = r3 + 0x1;
        *(u32*)((u8*)r4 + 0x0) = tmp;
    }
    r4 = lbl_8047B5B0;
    r3 = 0x0;
    r8 = 0x0;
    *(u32*)((u8*)r4 + 0x69C) = r5;
    tmp = *(u32*)((u8*)r5 + 0x0);
    r4 = lbl_8047B5B0;
    *(u32*)((u8*)r4 + 0x6A0) = tmp;
    do {
        r4 = lbl_8047B5B0;
        tmp = 0x1;
        tmp = tmp << r3;
        r4 = *(u8*)((u8*)r4 + 0x6A8);
        /* and. tmp, r4, tmp */;
        if (r3 != 0x21) {
            tmp = 0x10;
            ctr_fn = (void(*)(void))tmp;
            r7 = r8 + 0x0;
            r12 = 0x0;
            do {
                tmp = lbl_8047B5B0;
                r5 = 0xff;
                r31 = 0x0;
                r4 = tmp + r12;
                tmp = r4 + 0x300;
                *(u8*)(r8 + tmp) = r5;
                while (r31 < 5) {

                    r11 = lbl_8047B5B0;
                    r4 = 0x4 - r31;
                    tmp = r31 << 2;
                    r5 = r8 + r11;
                    r10 = r5 + tmp;
                    tmp = *(u32*)((u8*)r10 + 0x348);
                    r9 = (u32)r12 >> r4;
                    if ((s32)r9 <= (s32)tmp) {
                        r6 = *(u32*)((u8*)r5 + 0x340);
                        r5 = r31 + 0x1;
                        r4 = *(u32*)((u8*)r10 + 0x390);
                        tmp = r11 + 0x300;
                        r31 = 0x63;
                        r4 = r4 + r6;
                        r4 = *(u8*)(r9 + r4);
                        *(u8*)(r7 + tmp) = r4;
                        r4 = lbl_8047B5B0;
                        tmp = r4 + 0x320;
                        *(u8*)(r7 + tmp) = r5;
                    }
                    r31 = r31 + 0x1;

                }
                tmp = lbl_8047B5B0;
                r12 = r12 + 0x1;
                r5 = 0xff;
                r4 = tmp + r12;
                tmp = r4 + 0x300;
                *(u8*)(r8 + tmp) = r5;
                r31 = 0x0;
                r7 = r7 + 0x1;
                while (r31 < 5) {

                    r11 = lbl_8047B5B0;
                    r4 = 0x4 - r31;
                    tmp = r31 << 2;
                    r5 = r8 + r11;
                    r10 = r5 + tmp;
                    tmp = *(u32*)((u8*)r10 + 0x348);
                    r9 = (u32)r12 >> r4;
                    if ((s32)r9 <= (s32)tmp) {
                        r6 = *(u32*)((u8*)r5 + 0x340);
                        r5 = r31 + 0x1;
                        r4 = *(u32*)((u8*)r10 + 0x390);
                        tmp = r11 + 0x300;
                        r31 = 0x63;
                        r4 = r4 + r6;
                        r4 = *(u8*)(r9 + r4);
                        *(u8*)(r7 + tmp) = r4;
                        r4 = lbl_8047B5B0;
                        tmp = r4 + 0x320;
                        *(u8*)(r7 + tmp) = r5;
                    }
                    r31 = r31 + 0x1;

                }
                r7 = r7 + 0x1;
                r12 = r12 + 0x1;
            } while (--ctr != 0);
        }
        r3 = r3 + 0x1;
        r8 = r8 + 0xe0;
    } while (r3 < 4);
    r9 = lbl_8047B5B0;
    r4 = *(u8*)((u8*)r9 + 0x682);
    tmp = *(u8*)((u8*)r9 + 0x688);
    r3 = *(u8*)((u8*)r9 + 0x68E);
    r5 = r4 << 1;
    r7 = *(u8*)((u8*)r9 + 0x687);
    r4 = tmp << 1;
    r6 = *(u8*)((u8*)r9 + 0x68D);
    tmp = *(u8*)((u8*)r9 + 0x681);
    r3 = r3 << 1;
    r7 = r7 << 1;
    r6 = r6 << 1;
    r5 = r5 + 0x1;
    r4 = r4 + 0x1;
    r3 = r3 + 0x1;
    tmp = tmp << 1;
    r8 = tmp * 0xe0;
    r7 = r7 * 0xe0;
    r6 = r6 * 0xe0;
    r5 = r5 * 0xe0;
    r4 = r4 * 0xe0;
    r3 = r3 * 0xe0;
    r8 = r8 + 0x300;
    r7 = r7 + 0x300;
    r6 = r6 + 0x300;
    r5 = r5 + 0x300;
    r4 = r4 + 0x300;
    tmp = r3 + 0x300;
    r8 = r9 + r8;
    r3 = r9 + r7;
    lbl_8047B4A0 = r8;
    r6 = r9 + r6;
    r5 = r9 + r5;
    lbl_8047B4C0 = r3;
    r3 = r9 + r4;
    tmp = r9 + tmp;
    lbl_8047B4E0 = r6;
    lbl_8047B500 = r5;
    lbl_8047B520 = r3;
    lbl_8047B540 = tmp;
    return;
}
#endif

/* 0x801E6578 | size: 0x10C | medium */
#if 0
asm void fn_801E6578(void) {
#include "src/game/battle/battle_logic_fn_801E6578.inc"
}
#else
void fn_801E6578(void) {
    extern u32 lbl_8047B5A4;
    extern u32 lbl_8047B5A8;
    extern u32 lbl_8047B5B0;
    extern void fn_801E632C();
    extern void fn_801E6684();
    extern void fn_801E810C();
    extern void fn_801E9B98();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r6 = lbl_8047B5B0;
    *(u32*)((u8*)r6 + 0x6B0) = r3;
    r6 = lbl_8047B5B0;
    *(u32*)((u8*)r6 + 0x6B4) = r4;
    r4 = lbl_8047B5B0;
    *(u32*)((u8*)r4 + 0x6B8) = r5;
    r4 = lbl_8047B5B0;
    r31 = *(u16*)((u8*)r4 + 0x698);
    r30 = *(u16*)((u8*)r4 + 0x694);
    r4 = 0; /* mfspr GQR5 */;
    tmp = 0; /* mfspr GQR6 */;
    lbl_8047B5A4 = r4;
    lbl_8047B5A8 = tmp;
    r3 = 0x7;
    r3 = r3 | (0x7 << 16);
    /* mtspr GQR5, r3 */;
    r3 = 0x3d04;
    r3 = r3 | (0x3d04 << 16);
    /* mtspr GQR6, r3 */;
    fn_801E632C();
    r4 = lbl_8047B5B0;
    tmp = *(u16*)((u8*)r4 + 0x692);
    if (tmp == 0x200 && r30 == 0x1c0) {

        while (1) {
            tmp = r31 & 0xFFFF;
            if (tmp >= r30) break;
            fn_801E6684();
            r31 = r31 + 0x10;

        }
        goto L_801E665C;
    }
    r4 = lbl_8047B5B0;
    tmp = *(u16*)((u8*)r4 + 0x692);
    if (tmp == 0x280 && r30 == 0x1e0) {

        while (1) {
            tmp = r31 & 0xFFFF;
            if (tmp >= r30) break;
            fn_801E810C();
            r31 = r31 + 0x10;

        }
        goto L_801E665C;
    }
    while (tmp < r30) {
            tmp = r31 & 0xFFFF;
            fn_801E9B98();
            r31 = r31 + 0x10;
    }
L_801E665C:
    r4 = lbl_8047B5A4;
    tmp = lbl_8047B5A8;
    /* mtspr GQR5, r4 */;
    /* mtspr GQR6, tmp */;
    return;
}
#endif

/* 0x801E6684 | size: 0x1A88 | massive */
#if 0
asm void fn_801E6684(void) {
#include "src/game/battle/battle_logic_fn_801E6684.inc"
}
#else
void fn_801E6684(void) {
    extern u8 lbl_8046D500[];
    extern u32 lbl_8047B560;
    extern u32 lbl_8047B580;
    extern u32 lbl_8047B5A0;
    extern u32 lbl_8047B5B0;
    extern f32 lbl_8047E4B8;
    extern f32 lbl_8047E4BC;
    extern f32 lbl_8047E4C0;
    extern f32 lbl_8047E4C4;
    extern f32 lbl_8047E4C8;
    extern void fn_8009B55C();
    extern void fn_8009B614();
    extern void fn_801EB644();
    extern void fn_801EBCC0();
    extern void fn_801EC368();
    u8 sp[0x100];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;
    f32 f12 = 0.0f;
    f32 f13 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)lbl_8046D500;
    r31 = (u32)lbl_8046D500;
    r3 = 0x3;
    fn_8009B614();
    f27 = lbl_8047E4B8;
    r30 = 0x0;
    f28 = lbl_8047E4BC;
    f29 = lbl_8047E4C0;
    f30 = lbl_8047E4C4;
    f31 = lbl_8047E4C8;
    while (1) {
        r3 = lbl_8047B5B0;
        r4 = r30 & 0xFF;
        tmp = *(u16*)((u8*)r3 + 0x696);
        if ((s32)r4 >= (s32)tmp) break;
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x118);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x11C);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x120);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x124);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x128);
        fn_801EBCC0();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x12C);
        fn_801EC368();
        r3 = *(u32*)((u8*)r31 + 0x100);
        tmp = 0x200;
        r4 = lbl_8047B5B0;
        lbl_8047B560 = r3;
        lbl_8047B580 = tmp;
        tmp = *(u8*)((u8*)r4 + 0x680);
        tmp = tmp << 8;
        tmp = r4 + tmp;
        lbl_8047B5A0 = tmp;
        r8 = *(u32*)((u8*)r31 + 0x118);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E675C:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E675C;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r4 = r3 << 2;
        r6 = tmp << 2;
        r7 = r4;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r31 + 0x11C);
        r7 = lbl_8047B5A0;
        r10 = r3 + 0x8;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E6B6C:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E6B6C;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r10 = r10 << 2;
        r6 = tmp << 2;
        r7 = r10;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r31 + 0x120);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E6F78:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        r6 = r6 | tmp;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E6F78;
                        break;
                    }
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x10;
                    /* ps_merge00 f2, f7, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_sub f1, f28, f29 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r7 = tmp << 3;
        r4 = r3 << 2;
        r6 = tmp << 2;
        /* ps_add f9, f7, f6 */;
        r7 = r7 + r4;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r31 + 0x124);
        r7 = lbl_8047B5A0;
        r9 = r3 + 0x8;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E738C:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        r6 = r6 | tmp;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r10), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E738C;
                        break;
                    }
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x10;
                    /* ps_merge00 f2, f7, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_sub f1, f28, f29 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r10), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r10), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r10), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r10), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r10), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r10), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r10), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r10), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r10), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r10), 0, qr0 */;
            /* psq_stu f10, 0x8(r10), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r10), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r10 = r31;
        tmp = lbl_8047B580;
        r7 = tmp << 3;
        r9 = r9 << 2;
        r6 = tmp << 2;
        /* ps_add f9, f7, f6 */;
        r7 = r7 + r9;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r10 = r10 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r4 = *(u32*)((u8*)r31 + 0x104);
        tmp = 0x100;
        r5 = lbl_8047B5B0;
        lbl_8047B560 = r4;
        r3 = (u32)r3 >> 1;
        lbl_8047B580 = tmp;
        tmp = *(u8*)((u8*)r5 + 0x686);
        tmp = tmp << 8;
        tmp = r5 + tmp;
        lbl_8047B5A0 = tmp;
        r8 = *(u32*)((u8*)r31 + 0x128);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E77C4:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E77C4;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r4 = r3 << 2;
        r6 = tmp << 2;
        r7 = r4;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        tmp = *(u32*)((u8*)r31 + 0x108);
        r4 = lbl_8047B5B0;
        lbl_8047B560 = tmp;
        tmp = *(u8*)((u8*)r4 + 0x68C);
        tmp = tmp << 8;
        tmp = r4 + tmp;
        lbl_8047B5A0 = tmp;
        r9 = *(u32*)((u8*)r31 + 0x12C);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E7BEC:
        r6 = *(u32*)((u8*)r9 + 0xC);
        tmp = *(u32*)((u8*)r9 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r9 + 0x4);
        r4 = *(u16*)((u8*)r9 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r8), 0, qr0 */;
                        r9 = r9 + 0x10;
                        if (--ctr != 0) goto L_801E7BEC;
                        break;
                    }
                    r9 = r9 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r9 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r9 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r9 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r9 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r8), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r8), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r8), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r8), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r9 = r9 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r9 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r9 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r9 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r9 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r8), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r8), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r8), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r8), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r9 = r9 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r9 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r9 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r9 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r9 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r8), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r8), 0, qr0 */;
            /* psq_stu f10, 0x8(r8), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r8), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r7 = lbl_8047B560;
        r8 = r31;
        tmp = lbl_8047B580;
        r3 = r3 << 2;
        r5 = tmp << 2;
        r6 = r3;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r5 = r6 + r5;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r4 = r7 + r6;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r7 + r5;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r8 = r8 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r6 = r6 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r5 = r5 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r4 = r7 + r6;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r7 + r5;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r4 = lbl_8047B5B0;
        tmp = *(u8*)((u8*)r4 + 0x6A9);
        if (tmp != 0) {
            r3 = *(u16*)((u8*)r4 + 0x6AC);
            tmp = r3 & 0xFFFF;
            *(u16*)((u8*)r4 + 0x6AC) = r3;
            if (tmp == 0) {
                r3 = lbl_8047B5B0;
                tmp = *(u16*)((u8*)r3 + 0x6AA);
                *(u16*)((u8*)r3 + 0x6AC) = tmp;
                r4 = lbl_8047B5B0;
                r3 = *(u32*)((u8*)r4 + 0x6A4);
                tmp = r3 + 0x6;
                /* clrrwi r3, tmp, 3 */;
                tmp = r3 + 0x1;
                *(u32*)((u8*)r4 + 0x6A4) = tmp;
                r3 = lbl_8047B5B0;
                tmp = *(u32*)((u8*)r3 + 0x6A4);
                if (tmp > 0x21) {
                    tmp = 0x21;
                    *(u32*)((u8*)r3 + 0x6A4) = tmp;
                }
                r3 = lbl_8047B5B0;
                tmp = 0x0;
                *(u16*)((u8*)r3 + 0x684) = tmp;
                r3 = lbl_8047B5B0;
                *(u16*)((u8*)r3 + 0x68A) = tmp;
                r3 = lbl_8047B5B0;
                *(u16*)((u8*)r3 + 0x690) = tmp;
        }
        }
        r30 = r30 + 0x1;


    }
    r3 = *(u32*)((u8*)r3 + 0x6B0);
    r5 = 0x2000;
    r4 = *(u32*)((u8*)r31 + 0x100);
    fn_8009B55C();
    r3 = lbl_8047B5B0;
    r5 = 0x800;
    r4 = *(u32*)((u8*)r31 + 0x104);
    r3 = *(u32*)((u8*)r3 + 0x6B4);
    fn_8009B55C();
    r3 = lbl_8047B5B0;
    r5 = 0x800;
    r4 = *(u32*)((u8*)r31 + 0x108);
    r3 = *(u32*)((u8*)r3 + 0x6B8);
    fn_8009B55C();
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x6B0);
    tmp = r3 + 0x2000;
    *(u32*)((u8*)r4 + 0x6B0) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x6B4);
    tmp = r3 + 0x800;
    *(u32*)((u8*)r4 + 0x6B4) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x6B8);
    tmp = r3 + 0x800;
    *(u32*)((u8*)r4 + 0x6B8) = tmp;
    f31 = *(f64*)((u8*)(u32)sp + 0x30);
    f30 = *(f64*)((u8*)(u32)sp + 0x28);
    f29 = *(f64*)((u8*)(u32)sp + 0x20);
    f28 = *(f64*)((u8*)(u32)sp + 0x18);
    f27 = *(f64*)((u8*)(u32)sp + 0x10);
    return;
}
#endif

/* 0x801E810C | size: 0x1A8C | massive */
#if 0
asm void fn_801E810C(void) {
#include "src/game/battle/battle_logic_fn_801E810C.inc"
}
#else
void fn_801E810C(void) {
    extern u8 lbl_8046D500[];
    extern u32 lbl_8047B560;
    extern u32 lbl_8047B580;
    extern u32 lbl_8047B5A0;
    extern u32 lbl_8047B5B0;
    extern f32 lbl_8047E4B8;
    extern f32 lbl_8047E4BC;
    extern f32 lbl_8047E4C0;
    extern f32 lbl_8047E4C4;
    extern f32 lbl_8047E4C8;
    extern void fn_8009B55C();
    extern void fn_8009B614();
    extern void fn_801EB644();
    extern void fn_801EBCC0();
    extern void fn_801EC368();
    u8 sp[0x100];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;
    f32 f12 = 0.0f;
    f32 f13 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = (u32)lbl_8046D500;
    r31 = (u32)lbl_8046D500;
    r3 = 0x3;
    fn_8009B614();
    f27 = lbl_8047E4B8;
    r30 = 0x0;
    f28 = lbl_8047E4BC;
    f29 = lbl_8047E4C0;
    f30 = lbl_8047E4C4;
    f31 = lbl_8047E4C8;
    while (1) {
        r3 = lbl_8047B5B0;
        r4 = r30 & 0xFF;
        tmp = *(u16*)((u8*)r3 + 0x696);
        if ((s32)r4 >= (s32)tmp) break;
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x118);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x11C);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x120);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x124);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x128);
        fn_801EBCC0();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r31 + 0x12C);
        fn_801EC368();
        r3 = *(u32*)((u8*)r31 + 0x10C);
        tmp = 0x280;
        r4 = lbl_8047B5B0;
        lbl_8047B560 = r3;
        lbl_8047B580 = tmp;
        tmp = *(u8*)((u8*)r4 + 0x680);
        tmp = tmp << 8;
        tmp = r4 + tmp;
        lbl_8047B5A0 = tmp;
        r8 = *(u32*)((u8*)r31 + 0x118);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E81E4:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E81E4;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r4 = r3 << 2;
        r6 = tmp << 2;
        r7 = r4;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r31 + 0x11C);
        r7 = lbl_8047B5A0;
        r10 = r3 + 0x8;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E85F4:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E85F4;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r10 = r10 << 2;
        r6 = tmp << 2;
        r7 = r10;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r31 + 0x120);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E8A00:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        r6 = r6 | tmp;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E8A00;
                        break;
                    }
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x10;
                    /* ps_merge00 f2, f7, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_sub f1, f28, f29 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r7 = tmp << 3;
        r4 = r3 << 2;
        r6 = tmp << 2;
        /* ps_add f9, f7, f6 */;
        r7 = r7 + r4;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r31 + 0x124);
        r7 = lbl_8047B5A0;
        r9 = r3 + 0x8;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E8E14:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        r6 = r6 | tmp;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r10), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E8E14;
                        break;
                    }
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x10;
                    /* ps_merge00 f2, f7, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_sub f1, f28, f29 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r10), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r10), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r10), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r10), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r10), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r10), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r10), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r10), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r10), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r10), 0, qr0 */;
            /* psq_stu f10, 0x8(r10), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r10), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r10 = r31;
        tmp = lbl_8047B580;
        r7 = tmp << 3;
        r9 = r9 << 2;
        r6 = tmp << 2;
        /* ps_add f9, f7, f6 */;
        r7 = r7 + r9;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r10 = r10 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r4 = *(u32*)((u8*)r31 + 0x110);
        tmp = 0x140;
        r5 = lbl_8047B5B0;
        lbl_8047B560 = r4;
        r3 = (u32)r3 >> 1;
        lbl_8047B580 = tmp;
        tmp = *(u8*)((u8*)r5 + 0x686);
        tmp = tmp << 8;
        tmp = r5 + tmp;
        lbl_8047B5A0 = tmp;
        r8 = *(u32*)((u8*)r31 + 0x128);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E924C:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r4 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801E924C;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r31;
        tmp = lbl_8047B580;
        r4 = r3 << 2;
        r6 = tmp << 2;
        r7 = r4;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r4 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r4 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        tmp = *(u32*)((u8*)r31 + 0x114);
        r4 = lbl_8047B5B0;
        lbl_8047B560 = tmp;
        tmp = *(u8*)((u8*)r4 + 0x68C);
        tmp = tmp << 8;
        tmp = r4 + tmp;
        lbl_8047B5A0 = tmp;
        r9 = *(u32*)((u8*)r31 + 0x12C);
        r7 = lbl_8047B5A0;
        r4 = 0x8;
        ctr_fn = (void(*)(void))r4;
    L_801E9674:
        r6 = *(u32*)((u8*)r9 + 0xC);
        tmp = *(u32*)((u8*)r9 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r9 + 0x4);
        r4 = *(u16*)((u8*)r9 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r4 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r8), 0, qr0 */;
                        r9 = r9 + 0x10;
                        if (--ctr != 0) goto L_801E9674;
                        break;
                    }
                    r9 = r9 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r9 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r4 = *(u16*)((u8*)r9 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r9 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r9 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r8), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r8), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r8), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r8), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r9 = r9 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r9 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r9 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r9 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r4 = *(u16*)((u8*)r9 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r8), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r8), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r8), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r8), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r9 = r9 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r9 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r9 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r9 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r4 = *(u16*)((u8*)r9 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r8), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r8), 0, qr0 */;
            /* psq_stu f10, 0x8(r8), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r8), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r7 = lbl_8047B560;
        r8 = r31;
        tmp = lbl_8047B580;
        r3 = r3 << 2;
        r5 = tmp << 2;
        r6 = r3;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r5 = r6 + r5;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r4 = r7 + r6;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r7 + r5;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r8 = r8 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r6 = r6 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r5 = r5 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r4 = r7 + r6;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r7 + r5;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r4 = lbl_8047B5B0;
        tmp = *(u8*)((u8*)r4 + 0x6A9);
        if (tmp != 0) {
            r3 = *(u16*)((u8*)r4 + 0x6AC);
            *(u16*)((u8*)r4 + 0x6AC) = tmp;
            r3 = lbl_8047B5B0;
            tmp = *(u16*)((u8*)r3 + 0x6AC);
            if (tmp == 0) {
                tmp = *(u16*)((u8*)r3 + 0x6AA);
                *(u16*)((u8*)r3 + 0x6AC) = tmp;
                r4 = lbl_8047B5B0;
                r3 = *(u32*)((u8*)r4 + 0x6A4);
                tmp = r3 + 0x6;
                /* clrrwi r3, tmp, 3 */;
                tmp = r3 + 0x1;
                *(u32*)((u8*)r4 + 0x6A4) = tmp;
                r3 = lbl_8047B5B0;
                tmp = *(u32*)((u8*)r3 + 0x6A4);
                if (tmp > 0x20) {
                    tmp = 0x21;
                    *(u32*)((u8*)r3 + 0x6A4) = tmp;
                }
                r3 = lbl_8047B5B0;
                tmp = 0x0;
                *(u16*)((u8*)r3 + 0x684) = tmp;
                r3 = lbl_8047B5B0;
                *(u16*)((u8*)r3 + 0x68A) = tmp;
                r3 = lbl_8047B5B0;
                *(u16*)((u8*)r3 + 0x690) = tmp;
        }
        }
        r30 = r30 + 0x1;


    }
    r3 = *(u32*)((u8*)r3 + 0x6B0);
    r5 = 0x2800;
    r4 = *(u32*)((u8*)r31 + 0x10C);
    fn_8009B55C();
    r3 = lbl_8047B5B0;
    r5 = 0xa00;
    r4 = *(u32*)((u8*)r31 + 0x110);
    r3 = *(u32*)((u8*)r3 + 0x6B4);
    fn_8009B55C();
    r3 = lbl_8047B5B0;
    r5 = 0xa00;
    r4 = *(u32*)((u8*)r31 + 0x114);
    r3 = *(u32*)((u8*)r3 + 0x6B8);
    fn_8009B55C();
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x6B0);
    tmp = r3 + 0x2800;
    *(u32*)((u8*)r4 + 0x6B0) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x6B4);
    tmp = r3 + 0xa00;
    *(u32*)((u8*)r4 + 0x6B4) = tmp;
    r4 = lbl_8047B5B0;
    r3 = *(u32*)((u8*)r4 + 0x6B8);
    tmp = r3 + 0xa00;
    *(u32*)((u8*)r4 + 0x6B8) = tmp;
    f31 = *(f64*)((u8*)(u32)sp + 0x30);
    f30 = *(f64*)((u8*)(u32)sp + 0x28);
    f29 = *(f64*)((u8*)(u32)sp + 0x20);
    f28 = *(f64*)((u8*)(u32)sp + 0x18);
    f27 = *(f64*)((u8*)(u32)sp + 0x10);
    return;
}
#endif

/* 0x801E9B98 | size: 0x1AAC | massive */
#if 0
asm void fn_801E9B98(void) {
#include "src/game/battle/battle_logic_fn_801E9B98.inc"
}
#else
void fn_801E9B98(void) {
    extern u8 lbl_8046D500[];
    extern u32 lbl_8047B560;
    extern u32 lbl_8047B580;
    extern u32 lbl_8047B5A0;
    extern u32 lbl_8047B5B0;
    extern f32 lbl_8047E4B8;
    extern f32 lbl_8047E4BC;
    extern f32 lbl_8047E4C0;
    extern f32 lbl_8047E4C4;
    extern f32 lbl_8047E4C8;
    extern void fn_8009B55C();
    extern void fn_8009B614();
    extern void fn_801EB644();
    extern void fn_801EBCC0();
    extern void fn_801EC368();
    u8 sp[0x100];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f6 = 0.0f;
    f32 f7 = 0.0f;
    f32 f8 = 0.0f;
    f32 f9 = 0.0f;
    f32 f10 = 0.0f;
    f32 f11 = 0.0f;
    f32 f12 = 0.0f;
    f32 f13 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r4 = (u32)lbl_8046D500;
    r30 = (u32)lbl_8046D500;
    r3 = lbl_8047B5B0;
    r28 = *(u16*)((u8*)r3 + 0x692);
    r3 = 0x3;
    fn_8009B614();
    f27 = lbl_8047E4B8;
    r31 = (u32)r28 >> 1;
    f28 = lbl_8047E4BC;
    r29 = 0x0;
    f29 = lbl_8047E4C0;
    f30 = lbl_8047E4C4;
    f31 = lbl_8047E4C8;
    while (1) {
        r3 = lbl_8047B5B0;
        r4 = r29 & 0xFF;
        tmp = *(u16*)((u8*)r3 + 0x696);
        if ((s32)r4 >= (s32)tmp) break;
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r30 + 0x118);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r30 + 0x11C);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r30 + 0x120);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r30 + 0x124);
        fn_801EB644();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r30 + 0x128);
        fn_801EBCC0();
        r3 = lbl_8047B5B0;
        r4 = *(u32*)((u8*)r30 + 0x12C);
        fn_801EC368();
        tmp = *(u32*)((u8*)r30 + 0x10C);
        r5 = lbl_8047B5B0;
        lbl_8047B560 = tmp;
        lbl_8047B580 = r28;
        tmp = *(u8*)((u8*)r5 + 0x680);
        tmp = tmp << 8;
        tmp = r5 + tmp;
        lbl_8047B5A0 = tmp;
        r9 = *(u32*)((u8*)r30 + 0x118);
        r8 = lbl_8047B5A0;
        r5 = 0x8;
        ctr_fn = (void(*)(void))r5;
    L_801E9C80:
        r7 = *(u32*)((u8*)r9 + 0xC);
        tmp = *(u32*)((u8*)r9 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r6 = *(u32*)((u8*)r9 + 0x4);
        r5 = *(u16*)((u8*)r9 + 0x2);
        /* or. r7, r7, tmp */;
    while (1) {
            if ((s32)r7 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r6 == 0) {
                    if ((s32)r5 == 0) {
                        r8 = r8 + 0x20;
                        /* psq_stu f4, 0x20(r3), 0, qr0 */;
                        r9 = r9 + 0x10;
                        if (--ctr != 0) goto L_801E9C80;
                        break;
                    }
                    r9 = r9 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r6 = *(u32*)((u8*)r9 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r5 = *(u16*)((u8*)r9 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r7 = *(u32*)((u8*)r9 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r9 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r3), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r3), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r7 = r7 | tmp;
                    /* psq_stu f9, 0x8(r3), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r3), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r9 = r9 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r8 = r8 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r7 = *(u32*)((u8*)r9 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r9 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r6 = *(u32*)((u8*)r9 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r5 = *(u16*)((u8*)r9 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r3), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r7 = r7 | tmp;
                /* psq_stu f4, 0x8(r3), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r3), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r3), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r9 = r9 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r8 = r8 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r7 = *(u32*)((u8*)r9 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r9 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r6 = *(u32*)((u8*)r9 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r5 = *(u16*)((u8*)r9 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r3), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r3), 0, qr0 */;
            /* psq_stu f10, 0x8(r3), 0, qr0 */;
            r7 = r7 | tmp;
            /* psq_stu f4, 0x8(r3), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r9 = lbl_8047B560;
        r3 = r30;
        tmp = lbl_8047B580;
        r5 = r4 << 2;
        r7 = tmp << 2;
        r8 = r5;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r7 = r8 + r7;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r6 = r9 + r8;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r5 = r9 + r7;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r3 = r3 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r8 = r8 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r7 = r7 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r6 = r9 + r8;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r5 = r9 + r7;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r30 + 0x11C);
        r7 = lbl_8047B5A0;
        r10 = r4 + 0x8;
        r3 = 0x8;
        ctr_fn = (void(*)(void))r3;
    L_801EA090:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r3 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r3 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801EA090;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r3 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r3 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r3 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r30;
        tmp = lbl_8047B580;
        r10 = r10 << 2;
        r6 = tmp << 2;
        r7 = r10;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r30 + 0x120);
        r7 = lbl_8047B5A0;
        r3 = 0x8;
        ctr_fn = (void(*)(void))r3;
    L_801EA49C:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r3 = *(u16*)((u8*)r8 + 0x2);
        r6 = r6 | tmp;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r3 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801EA49C;
                        break;
                    }
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x10;
                    /* ps_merge00 f2, f7, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_sub f1, f28, f29 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_msub f12, f7, f27, f13 */;
                    r3 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r3 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r3 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r30;
        tmp = lbl_8047B580;
        r7 = tmp << 3;
        r3 = r4 << 2;
        r6 = tmp << 2;
        /* ps_add f9, f7, f6 */;
        r7 = r7 + r3;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r8 = *(u32*)((u8*)r30 + 0x124);
        r7 = lbl_8047B5A0;
        r9 = r4 + 0x8;
        r3 = 0x8;
        ctr_fn = (void(*)(void))r3;
    L_801EA8B0:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r3 = *(u16*)((u8*)r8 + 0x2);
        r6 = r6 | tmp;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r3 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r10), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801EA8B0;
                        break;
                    }
                    /* ps_msub f13, f7, f28, f7 */;
                    r8 = r8 + 0x10;
                    /* ps_merge00 f2, f7, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_sub f1, f28, f29 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_msub f12, f7, f27, f13 */;
                    r3 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r10), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r10), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r10), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r10), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r3 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r10), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r10), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r10), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r10), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r3 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r10), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r10), 0, qr0 */;
            /* psq_stu f10, 0x8(r10), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r10), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r10 = r30;
        tmp = lbl_8047B580;
        r7 = tmp << 3;
        r9 = r9 << 2;
        r6 = tmp << 2;
        /* ps_add f9, f7, f6 */;
        r7 = r7 + r9;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r10 = r10 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        tmp = *(u32*)((u8*)r30 + 0x110);
        r3 = lbl_8047B5B0;
        r4 = (u32)r4 >> 1;
        lbl_8047B560 = tmp;
        lbl_8047B580 = r31;
        tmp = *(u8*)((u8*)r3 + 0x686);
        tmp = tmp << 8;
        tmp = r3 + tmp;
        lbl_8047B5A0 = tmp;
        r8 = *(u32*)((u8*)r30 + 0x128);
        r7 = lbl_8047B5A0;
        r3 = 0x8;
        ctr_fn = (void(*)(void))r3;
    L_801EACE4:
        r6 = *(u32*)((u8*)r8 + 0xC);
        tmp = *(u32*)((u8*)r8 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r8 + 0x4);
        r3 = *(u16*)((u8*)r8 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r3 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r9), 0, qr0 */;
                        r8 = r8 + 0x10;
                        if (--ctr != 0) goto L_801EACE4;
                        break;
                    }
                    r8 = r8 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r8 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r3 = *(u16*)((u8*)r8 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r8 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r8 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r9), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r9), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r9), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r8 = r8 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r8 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r8 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r8 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r3 = *(u16*)((u8*)r8 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r9), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r9), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r9), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r9), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r8 = r8 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r8 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r8 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r8 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r3 = *(u16*)((u8*)r8 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r9), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r9), 0, qr0 */;
            /* psq_stu f10, 0x8(r9), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r9), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r8 = lbl_8047B560;
        r9 = r30;
        tmp = lbl_8047B580;
        r3 = r4 << 2;
        r6 = tmp << 2;
        r7 = r3;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r6 = r7 + r6;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r5 = r8 + r7;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r8 + r6;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r9 = r9 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r7 = r7 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r6 = r6 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r5 = r8 + r7;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r8 + r6;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        tmp = *(u32*)((u8*)r30 + 0x114);
        r3 = lbl_8047B5B0;
        lbl_8047B560 = tmp;
        tmp = *(u8*)((u8*)r3 + 0x68C);
        tmp = tmp << 8;
        tmp = r3 + tmp;
        lbl_8047B5A0 = tmp;
        r9 = *(u32*)((u8*)r30 + 0x12C);
        r7 = lbl_8047B5A0;
        r3 = 0x8;
        ctr_fn = (void(*)(void))r3;
    L_801EB10C:
        r6 = *(u32*)((u8*)r9 + 0xC);
        tmp = *(u32*)((u8*)r9 + 0x8);
        /* ps_mul f7, f7, f6 */;
        r5 = *(u32*)((u8*)r9 + 0x4);
        r3 = *(u16*)((u8*)r9 + 0x2);
        /* or. r6, r6, tmp */;
    while (1) {
            if ((s32)r6 == 0) {
                /* ps_merge00 f4, f7, f7 */;
                if ((s32)r5 == 0) {
                    if ((s32)r3 == 0) {
                        r7 = r7 + 0x20;
                        /* psq_stu f4, 0x20(r8), 0, qr0 */;
                        r9 = r9 + 0x10;
                        if (--ctr != 0) goto L_801EB10C;
                        break;
                    }
                    r9 = r9 + 0x10;
                    /* ps_msub f13, f7, f28, f7 */;
                    r7 = r7 + 0x20;
                    /* ps_merge00 f2, f7, f7 */;
                    r5 = *(u32*)((u8*)r9 + 0x4);
                    /* ps_sub f1, f28, f29 */;
                    /* ps_msub f12, f7, f27, f13 */;
                    r3 = *(u16*)((u8*)r9 + 0x2);
                    /* ps_merge11 f10, f7, f13 */;
                    /* ps_nmsub f11, f7, f1, f12 */;
                    /* ps_add f8, f2, f10 */;
                    /* ps_merge11 f9, f12, f11 */;
                    /* ps_sub f10, f2, f10 */;
                    r6 = *(u32*)((u8*)r9 + 0xC);
                    /* ps_add f3, f2, f9 */;
                    tmp = *(u32*)((u8*)r9 + 0x8);
                    /* ps_sub f9, f2, f9 */;
                    /* psq_stu f8, 0x8(r8), 0, qr0 */;
                    /* ps_merge10 f9, f9, f9 */;
                    /* psq_stu f3, 0x8(r8), 0, qr0 */;
                    /* ps_merge10 f10, f10, f10 */;
                    r6 = r6 | tmp;
                    /* psq_stu f9, 0x8(r8), 0, qr0 */;
                    /* ps_mul f7, f7, f6 */;
                    /* psq_stu f10, 0x8(r8), 0, qr0 */;
                    if (--ctr != 0) continue;
                    break;
                }
                r9 = r9 + 0x10;
                /* ps_mul f1, f1, f2 */;
                r7 = r7 + 0x20;
                /* ps_sub f12, f7, f1 */;
                /* ps_add f13, f7, f1 */;
                r6 = *(u32*)((u8*)r9 + 0xC);
                /* ps_madd f11, f1, f27, f12 */;
                /* ps_nmsub f10, f1, f27, f13 */;
                /* ps_mul f3, f12, f28 */;
                /* ps_merge00 f11, f13, f11 */;
                tmp = *(u32*)((u8*)r9 + 0x8);
                /* ps_nmsub f9, f1, f30, f3 */;
                /* ps_merge00 f10, f10, f12 */;
                r5 = *(u32*)((u8*)r9 + 0x4);
                /* ps_sub f9, f9, f13 */;
                /* ps_nmsub f8, f7, f29, f3 */;
                r3 = *(u16*)((u8*)r9 + 0x2);
                /* ps_merge11 f13, f13, f9 */;
                /* ps_msub f3, f12, f27, f9 */;
                /* ps_add f2, f11, f13 */;
                /* ps_sub f8, f8, f3 */;
                /* ps_merge11 f12, f3, f8 */;
                /* ps_sub f11, f11, f13 */;
                /* psq_stu f2, 0x8(r8), 0, qr0 */;
                /* ps_add f4, f10, f12 */;
                /* ps_sub f1, f10, f12 */;
                r6 = r6 | tmp;
                /* psq_stu f4, 0x8(r8), 0, qr0 */;
                /* ps_merge10 f1, f1, f1 */;
                /* ps_merge10 f11, f11, f11 */;
                /* psq_stu f1, 0x8(r8), 0, qr0 */;
                /* ps_mul f7, f7, f6 */;
                /* psq_stu f11, 0x8(r8), 0, qr0 */;
                if (--ctr != 0) continue;
                break;
            }
            /* ps_mul f2, f2, f10 */;
            /* ps_merge01 f4, f7, f2 */;
            /* ps_merge01 f1, f2, f7 */;
            r9 = r9 + 0x10;
            /* ps_madd f11, f13, f9, f4 */;
            /* ps_nmsub f10, f13, f9, f4 */;
            /* ps_madd f9, f12, f8, f1 */;
            /* ps_nmsub f8, f12, f8, f1 */;
            r7 = r7 + 0x20;
            /* ps_add f4, f11, f9 */;
            /* ps_sub f12, f11, f9 */;
            /* ps_msub f13, f8, f27, f9 */;
            r6 = *(u32*)((u8*)r9 + 0xC);
            /* ps_sub f3, f8, f10 */;
            /* ps_add f1, f10, f13 */;
            /* ps_sub f13, f10, f13 */;
            /* ps_mul f3, f3, f28 */;
            tmp = *(u32*)((u8*)r9 + 0x8);
            /* ps_merge00 f1, f4, f1 */;
            /* ps_nmsub f9, f10, f30, f3 */;
            /* ps_msub f11, f8, f29, f3 */;
            r5 = *(u32*)((u8*)r9 + 0x4);
            /* ps_sub f9, f9, f4 */;
            /* ps_merge00 f13, f13, f12 */;
            r3 = *(u16*)((u8*)r9 + 0x2);
            /* ps_madd f10, f12, f27, f9 */;
            /* ps_merge11 f8, f4, f9 */;
            /* ps_sub f11, f11, f10 */;
            /* ps_add f12, f1, f8 */;
            /* ps_merge11 f11, f10, f11 */;
            /* ps_sub f4, f1, f8 */;
            /* ps_mul f7, f7, f6 */;
            /* ps_add f10, f13, f11 */;
            /* ps_sub f9, f13, f11 */;
            /* ps_merge10 f10, f10, f10 */;
            /* psq_stu f12, 0x8(r8), 0, qr0 */;
            /* ps_merge10 f4, f4, f4 */;
            /* psq_stu f9, 0x8(r8), 0, qr0 */;
            /* psq_stu f10, 0x8(r8), 0, qr0 */;
            r6 = r6 | tmp;
            /* psq_stu f4, 0x8(r8), 0, qr0 */;
            if (--ctr != 0) continue;
        break;
    }

        r7 = lbl_8047B560;
        r8 = r30;
        tmp = lbl_8047B580;
        r3 = r4 << 2;
        r5 = tmp << 2;
        r6 = r3;
        /* ps_add f9, f7, f6 */;
        /* ps_sub f3, f7, f6 */;
        r5 = r6 + r5;
        /* ps_add f9, f9, f31 */;
        tmp = 0x3;
        /* ps_add f8, f5, f4 */;
        r4 = r7 + r6;
        /* ps_sub f2, f5, f4 */;
        /* ps_add f4, f9, f8 */;
        r3 = r7 + r5;
        /* ps_add f3, f3, f31 */;
        ctr_fn = (void(*)(void))tmp;
        do {
            /* ps_msub f2, f2, f27, f8 */;
            /* ps_sub f12, f9, f8 */;
            /* ps_add f1, f3, f2 */;
            /* ps_sub f13, f3, f2 */;
            /* ps_add f3, f9, f10 */;
            /* ps_sub f9, f9, f10 */;
            r8 = r8 + 0x8;
            /* ps_add f2, f11, f8 */;
            /* ps_sub f11, f11, f8 */;
            /* ps_add f8, f2, f3 */;
            /* ps_sub f10, f2, f3 */;
            /* ps_add f3, f9, f11 */;
            /* ps_add f2, f4, f8 */;
            /* ps_mul f3, f3, f28 */;
            /* ps_sub f0, f4, f8 */;
            /* ps_madd f9, f9, f30, f3 */;
            /* ps_sub f9, f9, f8 */;
            r6 = r6 + 0x2;
            /* ps_msub f11, f11, f29, f3 */;
            /* ps_add f2, f1, f9 */;
            /* ps_msub f10, f10, f27, f9 */;
            /* ps_sub f1, f1, f9 */;
            /* ps_add f3, f13, f10 */;
            /* ps_add f11, f11, f10 */;
            r5 = r5 + 0x2;
            /* ps_sub f2, f12, f11 */;
            /* ps_add f3, f12, f11 */;
            /* ps_sub f2, f13, f10 */;
            /* ps_add f9, f7, f6 */;
            /* ps_sub f3, f7, f6 */;
            /* ps_add f9, f9, f31 */;
            /* ps_add f8, f5, f4 */;
            /* ps_sub f2, f5, f4 */;
            r4 = r7 + r6;
            /* ps_add f4, f9, f8 */;
            /* ps_add f3, f3, f31 */;
            r3 = r7 + r5;
        } while (--ctr != 0);
        /* ps_msub f2, f2, f27, f8 */;
        /* ps_sub f12, f9, f8 */;
        /* ps_add f1, f3, f2 */;
        /* ps_sub f13, f3, f2 */;
        /* ps_add f3, f9, f10 */;
        /* ps_sub f9, f9, f10 */;
        /* ps_add f2, f11, f8 */;
        /* ps_sub f11, f11, f8 */;
        /* ps_add f8, f2, f3 */;
        /* ps_sub f10, f2, f3 */;
        /* ps_add f3, f9, f11 */;
        /* ps_add f2, f4, f8 */;
        /* ps_mul f3, f3, f28 */;
        /* ps_sub f0, f4, f8 */;
        /* ps_madd f9, f9, f30, f3 */;
        /* ps_sub f9, f9, f8 */;
        /* ps_msub f11, f11, f29, f3 */;
        /* ps_add f2, f1, f9 */;
        /* ps_msub f10, f10, f27, f9 */;
        /* ps_sub f1, f1, f9 */;
        /* ps_add f3, f13, f10 */;
        /* ps_add f11, f11, f10 */;
        /* ps_sub f2, f12, f11 */;
        /* ps_add f3, f12, f11 */;
        /* ps_sub f2, f13, f10 */;
        r5 = lbl_8047B5B0;
        tmp = *(u8*)((u8*)r5 + 0x6A9);
        if (tmp != 0) {
            r4 = *(u16*)((u8*)r5 + 0x6AC);
            *(u16*)((u8*)r5 + 0x6AC) = tmp;
            r4 = lbl_8047B5B0;
            tmp = *(u16*)((u8*)r4 + 0x6AC);
            if (tmp == 0) {
                tmp = *(u16*)((u8*)r4 + 0x6AA);
                *(u16*)((u8*)r4 + 0x6AC) = tmp;
                r4 = lbl_8047B5B0;
                r3 = *(u32*)((u8*)r4 + 0x6A4);
                tmp = r3 + 0x6;
                /* clrrwi r3, tmp, 3 */;
                tmp = r3 + 0x1;
                *(u32*)((u8*)r4 + 0x6A4) = tmp;
                r3 = lbl_8047B5B0;
                tmp = *(u32*)((u8*)r3 + 0x6A4);
                if (tmp > 0x20) {
                    tmp = 0x21;
                    *(u32*)((u8*)r3 + 0x6A4) = tmp;
                }
                r3 = lbl_8047B5B0;
                tmp = 0x0;
                *(u16*)((u8*)r3 + 0x684) = tmp;
                r3 = lbl_8047B5B0;
                *(u16*)((u8*)r3 + 0x68A) = tmp;
                r3 = lbl_8047B5B0;
                *(u16*)((u8*)r3 + 0x690) = tmp;
        }
        }
        r29 = r29 + 0x1;


    }
    r29 = (u32)r28 >> 4;
    r3 = *(u32*)((u8*)r3 + 0x6B0);
    r4 = *(u32*)((u8*)r30 + 0x10C);
    /* extlwi r5, r28, 24, 4 */;
    fn_8009B55C();
    r3 = lbl_8047B5B0;
    r5 = r29 << 6;
    r4 = *(u32*)((u8*)r30 + 0x110);
    r3 = *(u32*)((u8*)r3 + 0x6B4);
    fn_8009B55C();
    r3 = lbl_8047B5B0;
    r5 = r29 << 6;
    r4 = *(u32*)((u8*)r30 + 0x114);
    r3 = *(u32*)((u8*)r3 + 0x6B8);
    fn_8009B55C();
    r5 = lbl_8047B5B0;
    tmp = r29 << 8;
    r4 = r29 << 6;
    r3 = *(u32*)((u8*)r5 + 0x6B0);
    tmp = r3 + tmp;
    *(u32*)((u8*)r5 + 0x6B0) = tmp;
    r3 = lbl_8047B5B0;
    tmp = *(u32*)((u8*)r3 + 0x6B4);
    tmp = tmp + r4;
    *(u32*)((u8*)r3 + 0x6B4) = tmp;
    r3 = lbl_8047B5B0;
    tmp = *(u32*)((u8*)r3 + 0x6B8);
    tmp = tmp + r4;
    *(u32*)((u8*)r3 + 0x6B8) = tmp;
    f31 = *(f64*)((u8*)(u32)sp + 0x38);
    f30 = *(f64*)((u8*)(u32)sp + 0x30);
    f29 = *(f64*)((u8*)(u32)sp + 0x28);
    f28 = *(f64*)((u8*)(u32)sp + 0x20);
    f27 = *(f64*)((u8*)(u32)sp + 0x18);
    return;
}
#endif

/* 0x801EB644 | size: 0x67C | large */
#if 0
asm void fn_801EB644(void) {
#include "src/game/battle/battle_logic_fn_801EB644.inc"
}
#else
void fn_801EB644(void) {
    extern u8 lbl_80279AE8[];
    extern u32 lbl_8047B4A0;
    extern u32 lbl_8047B500;
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* dcbz tmp, r4 */;
    r12 = lbl_8047B4A0;
    r11 = *(u32*)((u8*)r3 + 0x6A4);
    r9 = r12 + 0x20;
    r10 = *(u32*)((u8*)r3 + 0x6A0);
    r5 = r11 + 0x4;
    /* rlwnm r8, r10, r5, 27, 31 */;
    if ((s32)r11 <= 0x1c) {
        r5 = *(u8*)(r12 + r8);
        r9 = *(u8*)(r9 + r8);
        if ((s32)r5 != 0xff) {
            r11 = r11 + r9;
            *(u32*)((u8*)r3 + 0x6A4) = r11;
            goto L_801EB8DC;
        }
        r6 = r12 + 0x44;
        r11 = r11 + 0x5;
        tmp = 0x14;
        r5 = 0x5;
        r6 = r6 + 0x14;
        do {
            r8 = r8 << 1;
            if ((s32)r11 != 0x21) {
                /* rlwnm r9, r10, r11, 31, 31 */;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r8 = r8 | r9;
                r11 = r11 + 0x1;

            } else {
            r9 = *(u32*)((u8*)r3 + 0x69C);
            r11 = 0x1;
            r10 = *(u32*)((u8*)r9 + 0x4);
            tmp = *(u32*)((u8*)r6 + 0x4);
            *(u32*)((u8*)r3 + 0x69C) = r9;
            r8 = (r8 & ~0x00000001) | (((r10 << 1) | (r10 >> 31)) & 0x00000001);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            while (1) {
                r11 = r11 + 0x1;
                r5 = r5 + 0x1;
                if ((s32)r8 <= (s32)tmp) break;
                r8 = r8 << 1;
                /* rlwnm r9, r10, r11, 31, 31 */;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r8 = r8 | r9;


            }
            break;
            }
            r5 = r5 + 0x1;
        } while ((s32)r8 > (s32)tmp);

        *(u32*)((u8*)r3 + 0x6A4) = r11;
        tmp = r5 << 2;
        r5 = r12 + tmp;
        r6 = *(u32*)((u8*)r12 + 0x40);
        tmp = *(u32*)((u8*)r5 + 0x8C);
        tmp = tmp + r6;
        r5 = *(u8*)(r8 + tmp);
        goto L_801EB8DC;
    }
    r8 = *(u32*)((u8*)r3 + 0x69C);
    if ((s32)r11 == 0x21) goto L_801EB7F4;
    /* rlwnm r5, r10, r5, 27, 31 */;
    if ((s32)r11 != 0x20) {
        r8 = *(u8*)(r12 + r5);
        r9 = *(u8*)(r9 + r5);
        r5 = r11 + r9;
        if ((s32)r8 != 0xff) {
            *(u32*)((u8*)r3 + 0x6A4) = r5;
        }
        if ((s32)r5 > 0x21) goto L_801EB858;
        r5 = r8;
        goto L_801EB8DC;
    }
    r10 = *(u32*)((u8*)r8 + 0x4);
    *(u32*)((u8*)r3 + 0x69C) = r8;
    r5 = (r5 & ~0x0000000F) | (((r10 << 4) | (r10 >> 28)) & 0x0000000F);
    r8 = *(u8*)(r12 + r5);
    r9 = *(u8*)(r9 + r5);
    *(u32*)((u8*)r3 + 0x6A0) = r10;
    *(u32*)((u8*)r3 + 0x6A4) = r9;
    if ((s32)r8 != 0xff) {
        r5 = r8;
        goto L_801EB8DC;
    }
    r6 = r12 + 0x44;
    r11 = 0x14;
    r6 = r6 + 0x14;
    r8 = r5 << 27;
    r11 = 0x5;
    r8 = (r8 & ~0x7FFFFFFF) | (((r10 << 31) | (r10 >> 1)) & 0x7FFFFFFF);
    do {
        r10 = 0x1f - r11;
        tmp = *(u32*)((u8*)r6 + 0x4);
        r5 = (u32)r8 >> r10;
        r11 = r11 + 0x1;
    } while ((s32)r5 > (s32)tmp);
    *(u32*)((u8*)r3 + 0x6A4) = r11;
L_801EB7D8:
    tmp = r11 << 2;
    r7 = *(u32*)((u8*)r12 + 0x40);
    r6 = r12 + tmp;
    tmp = *(u32*)((u8*)r6 + 0x8C);
    tmp = tmp + r7;
    r5 = *(u8*)(r5 + tmp);
    goto L_801EB8DC;
L_801EB7F4:
    r10 = *(u32*)((u8*)r8 + 0x4);
    r5 = (u32)r10 >> 27;
    *(u32*)((u8*)r3 + 0x69C) = r8;
    r11 = *(u8*)(r12 + r5);
    r9 = *(u8*)(r9 + r5);
    *(u32*)((u8*)r3 + 0x6A0) = r10;
    r9 = r9 + 0x1;
    if ((s32)r11 != 0xff) {
        *(u32*)((u8*)r3 + 0x6A4) = r9;
        r5 = r11;
        goto L_801EB8DC;
    }
    r11 = 0x5;
    r6 = 0x14;
    do {
        r8 = 0x1f - r11;
        r11 = r11 + 0x1;
        r6 = r6 + 0x4;
        r5 = (u32)r10 >> r8;
        r7 = r12 + r6;
        tmp = *(u32*)((u8*)r7 + 0x44);
    } while ((s32)r5 > (s32)tmp);
    tmp = r11 + 0x1;
    *(u32*)((u8*)r3 + 0x6A4) = tmp;
    goto L_801EB7D8;
L_801EB858:
    tmp = 0x21 - r11;
    r5 = -0x1;
    r7 = r5 << tmp;
    r5 = r10 & ~r7;
    r7 = r12 + 0x44;
    r8 = *(u32*)((u8*)r3 + 0x69C);
    r6 = 0x21 - r11;
    r11 = r6 + 0x1;
    r6 = r6 << 2;
    r10 = *(u32*)((u8*)r8 + 0x4);
    r7 = r7 + r6;
    *(u32*)((u8*)r3 + 0x69C) = r8;
    r5 = r5 << 1;
    *(u32*)((u8*)r3 + 0x6A0) = r10;
    r5 = (r5 & ~0x00000001) | (((r10 << 1) | (r10 >> 31)) & 0x00000001);
    r6 = *(u32*)((u8*)r7 + 0x4);
    r8 = 0x2;
    while (1) {
        /* rlwnm r9, r10, r8, 31, 31 */;
        if ((s32)r5 <= (s32)r6) break;
        r5 = r5 << 1;
        r11 = r11 + 0x1;
        r6 = *(u32*)((u8*)r7 + 0x4);
        r5 = r5 + r9;
        r8 = r8 + 0x1;


    }
    *(u32*)((u8*)r3 + 0x6A4) = r8;
    tmp = r11 << 2;
    r6 = r12 + tmp;
    r7 = *(u32*)((u8*)r12 + 0x40);
    tmp = *(u32*)((u8*)r6 + 0x8C);
    tmp = tmp + r7;
    r5 = *(u8*)(r5 + tmp);
L_801EB8DC:
    tmp = 0x20;
    /* dcbz r4, tmp */;
    r7 = 0x0;
    tmp = 0x40;
    /* dcbz r4, tmp */;
    if ((s32)r5 != 0) {
        r7 = *(u32*)((u8*)r3 + 0x6A4);
        r8 = 0x21 - r7;
        r6 = *(u32*)((u8*)r3 + 0x6A0);
        /* subfc. r9, r8, r5 */;
        if ((s32)r5 <= 0) {
            tmp = r7 + r5;
            r7 = r6 << r10;
            *(u32*)((u8*)r3 + 0x6A4) = tmp;
            tmp = 0x20 - r5;
            r7 = (u32)r7 >> tmp;
        } else {

            r7 = *(u32*)((u8*)r3 + 0x69C);
            tmp = r6 << r10;
            r6 = *(u32*)((u8*)r7 + 0x4);
            r9 = r9 + 0x1;
            *(u32*)((u8*)r3 + 0x6A0) = r6;
            r6 = (u32)r6 >> r8;
            *(u32*)((u8*)r3 + 0x69C) = r7;
            tmp = r6 + tmp;
            *(u32*)((u8*)r3 + 0x6A4) = r9;
            r9 = 0x20 - r5;
            r7 = (u32)tmp >> r9;
        }
        tmp = (s16)r7;
        r6 = __cntlzw(tmp);
        tmp = 0x20 - r5;
        if ((s32)r6 > (s32)tmp) {
            tmp = -0x1;
            tmp = tmp << r5;
            r7 = tmp + r7;
            r7 = r7 + 0x1;
    }
    }
    tmp = 0x60;
    /* dcbz r4, tmp */;
    tmp = *(s16*)((u8*)r3 + 0x684);
    tmp = tmp + r7;
    *(u16*)((u8*)r3 + 0x684) = tmp;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    r8 = lbl_8047B500;
    r6 = *(u32*)((u8*)r3 + 0x6A4);
    r7 = r8 + 0x20;
    tmp = *(u32*)((u8*)r3 + 0x6A0);
    r5 = 0x1;
    r9 = (u32)lbl_80279AE8;
    r10 = (u32)lbl_80279AE8;
    while (1) {
    do {
        if ((s32)r5 >= 0x40) break;
        r31 = r6 + 0x4;
        /* rlwnm r12, tmp, r31, 27, 31 */;
        if ((s32)r6 <= 0x1c) {
            r30 = *(u8*)(r8 + r12);
            r31 = *(u8*)(r7 + r12);
            if ((s32)r30 != 0xff) {
                r6 = r6 + r31;
                break;
            }
            r6 = r6 + 0x5;
            r9 = r8 + 0x44;
            r11 = 0x14;
            r31 = 0x5;
            r9 = r9 + 0x14;
            do {
                r12 = r12 << 1;
                if ((s32)r6 != 0x21) {
                    /* rlwnm r30, tmp, r6, 31, 31 */;
                    r11 = *(u32*)((u8*)r9 + 0x4);
                    r12 = r12 | r30;
                    r6 = r6 + 0x1;

                } else {
                r30 = *(u32*)((u8*)r3 + 0x69C);
                r6 = 0x1;
                tmp = *(u32*)((u8*)r30 + 0x4);
                r11 = *(u32*)((u8*)r9 + 0x4);
                *(u32*)((u8*)r3 + 0x69C) = r30;
                r12 = (r12 & ~0x00000001) | (((tmp << 1) | (tmp >> 31)) & 0x00000001);
                while (1) {
                    r6 = r6 + 0x1;
                    r31 = r31 + 0x1;
                    if ((s32)r12 <= (s32)r11) break;
                    r12 = r12 << 1;
                    /* rlwnm r30, tmp, r6, 31, 31 */;
                    r11 = *(u32*)((u8*)r9 + 0x4);
                    r12 = r12 | r30;


                }
                break;
                }
                r31 = r31 + 0x1;
            } while ((s32)r12 > (s32)r11);

            r9 = r31 << 2;
            r11 = *(u32*)((u8*)r8 + 0x40);
            r9 = r8 + r9;
            r9 = *(u32*)((u8*)r9 + 0x8C);
            r9 = r9 + r11;
            r30 = *(u8*)(r12 + r9);
            break;
        }
        r12 = *(u32*)((u8*)r3 + 0x69C);
        if ((s32)r6 != 0x21) {
            /* rlwnm r31, tmp, r31, 27, 31 */;
            if ((s32)r6 != 0x20) {
                r30 = *(u8*)(r8 + r31);
                r29 = *(u8*)(r7 + r31);
                r31 = r6 + r29;
                if ((s32)r30 != 0xff && (s32)r31 <= 0x21) {

                    r6 = r31;
                    break;
                }
                tmp = *(u32*)((u8*)r12 + 0x4);
                r31 = (u32)tmp >> 27;
                *(u32*)((u8*)r3 + 0x69C) = r12;
                r30 = *(u8*)(r8 + r31);
                r12 = *(u8*)(r7 + r31);
                r6 = r12 + 0x1;
                if ((s32)r30 != 0xff) {
                    break;
                }
                r30 = 0x5;
                r6 = 0x14;
                do {
                    r12 = 0x1f - r30;
                    r30 = r30 + 0x1;
                    r6 = r6 + 0x4;
                    r31 = (u32)tmp >> r12;
                    r12 = r8 + r6;
                    r9 = *(u32*)((u8*)r12 + 0x44);
                } while ((s32)r31 > (s32)r9);
                r11 = *(u32*)((u8*)r8 + 0x40);
                r6 = r30 + 0x1;
                r9 = *(u32*)((u8*)r12 + 0x8C);
                r9 = r9 + r11;
                r30 = *(u8*)(r31 + r9);
                break;
                }
            tmp = *(u32*)((u8*)r12 + 0x4);
            *(u32*)((u8*)r3 + 0x69C) = r12;
            r31 = (r31 & ~0x0000000F) | (((tmp << 4) | (tmp >> 28)) & 0x0000000F);
            r30 = *(u8*)(r8 + r31);
            r6 = *(u8*)(r7 + r31);
            if ((s32)r30 != 0xff) {
                break;
            }
            r9 = r8 + 0x44;
            r6 = 0x14;
            r9 = r9 + 0x14;
            r12 = r31 << 27;
            r6 = 0x5;
            r12 = (r12 & ~0x7FFFFFFF) | (((tmp << 31) | (tmp >> 1)) & 0x7FFFFFFF);
            do {
                r30 = 0x1f - r6;
                r11 = *(u32*)((u8*)r9 + 0x4);
                r31 = (u32)r12 >> r30;
                r6 = r6 + 0x1;
            } while ((s32)r31 > (s32)r11);
            r9 = r6 << 2;
            r11 = *(u32*)((u8*)r8 + 0x40);
            r9 = r8 + r9;
            r9 = *(u32*)((u8*)r9 + 0x8C);
            r9 = r9 + r11;
            r30 = *(u8*)(r31 + r9);
            break;
                }
        r9 = 0x21 - r6;
        r11 = -0x1;
        r9 = r11 << r9;
        r31 = tmp & ~r9;
        r9 = r8 + 0x44;
        r12 = *(u32*)((u8*)r3 + 0x69C);
        r11 = 0x21 - r6;
        r30 = r11 + 0x1;
        r11 = r11 << 2;
        tmp = *(u32*)((u8*)r12 + 0x4);
        r9 = r9 + r11;
        *(u32*)((u8*)r3 + 0x69C) = r12;
        r31 = r31 << 1;
        r31 = (r31 & ~0x00000001) | (((tmp << 1) | (tmp >> 31)) & 0x00000001);
        r11 = *(u32*)((u8*)r9 + 0x4);
        r6 = 0x2;
        while (1) {
            /* rlwnm r29, tmp, r6, 31, 31 */;
            if ((s32)r31 <= (s32)r11) break;
            r31 = r31 << 1;
            r30 = r30 + 0x1;
            r11 = *(u32*)((u8*)r9 + 0x4);
            r31 = r31 + r29;
            r6 = r6 + 0x1;


        }
        r9 = r30 << 2;
        r11 = *(u32*)((u8*)r8 + 0x40);
        r9 = r8 + r9;
        r9 = *(u32*)((u8*)r9 + 0x8C);
        r9 = r9 + r11;
        r30 = *(u8*)(r31 + r9);
    } while (0);
        r29 = r30 & 0xf;
        r30 = (s32)r30 >> 4;
        if ((s32)r31 != (s32)r11) {
            r5 = r5 + r30;
            r31 = 0x21 - r6;
            /* subfc. r12, r31, r29 */;
            if ((s32)r31 <= (s32)r11) {
                r6 = r6 + r29;
                r11 = tmp << r9;
                r9 = 0x20 - r29;
                r30 = (u32)r11 >> r9;
            } else {

                r11 = *(u32*)((u8*)r3 + 0x69C);
                r9 = tmp << r9;
                tmp = *(u32*)((u8*)r11 + 0x4);
                r6 = r12 + 0x1;
                *(u32*)((u8*)r3 + 0x69C) = r11;
                r11 = (u32)tmp >> r31;
                r9 = r11 + r9;
                r12 = 0x20 - r29;
                r30 = (u32)r9 >> r12;
            }
            r11 = __cntlzw(r30);
            r9 = 0x20 - r29;
            if ((s32)r11 > (s32)r9) {
                r9 = -0x1;
                r9 = r9 << r29;
                r30 = r9 + r30;
                r30 = r30 + 0x1;
            }
            r9 = r10 + r5;
            r9 = *(u8*)((u8*)r9 + 0x0);
            r11 = (s16)r30;
            r9 = r9 << 1;
            *(u16*)(r4 + r9) = r11;

        } else {
        if ((s32)r30 != 0xf) break;
        r5 = r5 + 0xf;
        }
        r5 = r5 + 0x1;

    }

    *(u32*)((u8*)r3 + 0x6A4) = r6;
    *(u32*)((u8*)r3 + 0x6A0) = tmp;
    return;
}
#endif

/* 0x801EBCC0 | size: 0x6A8 | large */
#if 0
asm void fn_801EBCC0(void) {
#include "src/game/battle/battle_logic_fn_801EBCC0.inc"
}
#else
void fn_801EBCC0(void) {
    extern u8 lbl_80279AE8[];
    extern u32 lbl_8047B4C0;
    extern u32 lbl_8047B520;
    u8 sp[0x18];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* dcbz tmp, r4 */;
    r12 = lbl_8047B4C0;
    r11 = *(u32*)((u8*)r3 + 0x6A4);
    r9 = r12 + 0x20;
    r10 = *(u32*)((u8*)r3 + 0x6A0);
    r5 = r11 + 0x4;
    /* rlwnm r8, r10, r5, 27, 31 */;
    if ((s32)r11 <= 0x1c) {
        r5 = *(u8*)(r12 + r8);
        r9 = *(u8*)(r9 + r8);
        if ((s32)r5 != 0xff) {
            r11 = r11 + r9;
            *(u32*)((u8*)r3 + 0x6A4) = r11;
            goto L_801EBF54;
        }
        r6 = r12 + 0x44;
        r11 = r11 + 0x5;
        tmp = 0x14;
        r5 = 0x5;
        r6 = r6 + 0x14;
        do {
            r8 = r8 << 1;
            if ((s32)r11 != 0x21) {
                /* rlwnm r9, r10, r11, 31, 31 */;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r8 = r8 | r9;
                r11 = r11 + 0x1;
                continue;
            }
            r9 = *(u32*)((u8*)r3 + 0x69C);
            r11 = 0x1;
            r10 = *(u32*)((u8*)r9 + 0x4);
            tmp = *(u32*)((u8*)r6 + 0x4);
            *(u32*)((u8*)r3 + 0x69C) = r9;
            r8 = (r8 & ~0x00000001) | (((r10 << 1) | (r10 >> 31)) & 0x00000001);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            while (1) {
                r11 = r11 + 0x1;
                r5 = r5 + 0x1;
                if ((s32)r8 <= (s32)tmp) break;
                r8 = r8 << 1;
                /* rlwnm r9, r10, r11, 31, 31 */;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r8 = r8 | r9;

            }
            break;


        } while ((s32)r8 > (s32)tmp);

        *(u32*)((u8*)r3 + 0x6A4) = r11;
        tmp = r5 << 2;
        r5 = r12 + tmp;
        r6 = *(u32*)((u8*)r12 + 0x40);
        tmp = *(u32*)((u8*)r5 + 0x8C);
        tmp = tmp + r6;
        r5 = *(u8*)(r8 + tmp);
        goto L_801EBF54;
    }
    r8 = *(u32*)((u8*)r3 + 0x69C);
    if ((s32)r11 != 0x21) {
        /* rlwnm r5, r10, r5, 27, 31 */;
        if ((s32)r11 != 0x20) {
            r8 = *(u8*)(r12 + r5);
            r9 = *(u8*)(r9 + r5);
            r5 = r11 + r9;
            if ((s32)r8 != 0xff) {
                *(u32*)((u8*)r3 + 0x6A4) = r5;
            }
            if ((s32)r5 <= 0x21) {
                r5 = r8;
                goto L_801EBF54;
            }
            r10 = *(u32*)((u8*)r8 + 0x4);
            *(u32*)((u8*)r3 + 0x69C) = r8;
            r5 = (r5 & ~0x0000000F) | (((r10 << 4) | (r10 >> 28)) & 0x0000000F);
            r8 = *(u8*)(r12 + r5);
            r9 = *(u8*)(r9 + r5);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            *(u32*)((u8*)r3 + 0x6A4) = r9;
            if ((s32)r8 != 0xff) {
                r5 = r8;
                goto L_801EBF54;
            }
            r6 = r12 + 0x44;
            r11 = 0x14;
            r6 = r6 + 0x14;
            r8 = r5 << 27;
            r11 = 0x5;
            r8 = (r8 & ~0x7FFFFFFF) | (((r10 << 31) | (r10 >> 1)) & 0x7FFFFFFF);
            do {
                r10 = 0x1f - r11;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r5 = (u32)r8 >> r10;
                r11 = r11 + 0x1;
            } while ((s32)r5 > (s32)tmp);
            *(u32*)((u8*)r3 + 0x6A4) = r11;
        while (1) {
                tmp = r11 << 2;
                r7 = *(u32*)((u8*)r12 + 0x40);
                r6 = r12 + tmp;
                tmp = *(u32*)((u8*)r6 + 0x8C);
                tmp = tmp + r7;
                r5 = *(u8*)(r5 + tmp);
                goto L_801EBF54;
        }
            r10 = *(u32*)((u8*)r8 + 0x4);
            r5 = (u32)r10 >> 27;
            *(u32*)((u8*)r3 + 0x69C) = r8;
            r11 = *(u8*)(r12 + r5);
            r9 = *(u8*)(r9 + r5);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            r9 = r9 + 0x1;
            if ((s32)r11 != 0xff) {
                *(u32*)((u8*)r3 + 0x6A4) = r9;
                r5 = r11;
                goto L_801EBF54;
            }
            r11 = 0x5;
            r6 = 0x14;
            do {
                r8 = 0x1f - r11;
                r11 = r11 + 0x1;
                r6 = r6 + 0x4;
                r5 = (u32)r10 >> r8;
                r7 = r12 + r6;
                tmp = *(u32*)((u8*)r7 + 0x44);
            } while ((s32)r5 > (s32)tmp);
            tmp = r11 + 0x1;
            *(u32*)((u8*)r3 + 0x6A4) = tmp;
    }
            }
    tmp = 0x21 - r11;
    r5 = -0x1;
    r7 = r5 << tmp;
    r5 = r10 & ~r7;
    r7 = r12 + 0x44;
    r8 = *(u32*)((u8*)r3 + 0x69C);
    r6 = 0x21 - r11;
    r11 = r6 + 0x1;
    r6 = r6 << 2;
    r10 = *(u32*)((u8*)r8 + 0x4);
    r7 = r7 + r6;
    *(u32*)((u8*)r3 + 0x69C) = r8;
    r5 = r5 << 1;
    *(u32*)((u8*)r3 + 0x6A0) = r10;
    r5 = (r5 & ~0x00000001) | (((r10 << 1) | (r10 >> 31)) & 0x00000001);
    r6 = *(u32*)((u8*)r7 + 0x4);
    r8 = 0x2;
    while (1) {
        /* rlwnm r9, r10, r8, 31, 31 */;
        if ((s32)r5 <= (s32)r6) break;
        r5 = r5 << 1;
        r11 = r11 + 0x1;
        r6 = *(u32*)((u8*)r7 + 0x4);
        r5 = r5 + r9;
        r8 = r8 + 0x1;

    }
    *(u32*)((u8*)r3 + 0x6A4) = r8;
    tmp = r11 << 2;
    r6 = r12 + tmp;
    r7 = *(u32*)((u8*)r12 + 0x40);
    tmp = *(u32*)((u8*)r6 + 0x8C);
    tmp = tmp + r7;
    r5 = *(u8*)(r5 + tmp);
L_801EBF54:
    tmp = 0x20;
    /* dcbz r4, tmp */;
    r7 = 0x0;
    tmp = 0x40;
    /* dcbz r4, tmp */;
    if ((s32)r5 != 0) {
        r9 = *(u32*)((u8*)r3 + 0x6A4);
        r10 = 0x21 - r9;
        r7 = *(u32*)((u8*)r3 + 0x6A0);
        /* subfc. r11, r10, r5 */;
        if ((s32)r5 <= 0) {
            tmp = r9 + r5;
            r9 = r7 << r12;
            *(u32*)((u8*)r3 + 0x6A4) = tmp;
            tmp = 0x20 - r5;
            r7 = (u32)r9 >> tmp;
        } else {

            r9 = *(u32*)((u8*)r3 + 0x69C);
            tmp = r7 << r12;
            r7 = *(u32*)((u8*)r9 + 0x4);
            r11 = r11 + 0x1;
            *(u32*)((u8*)r3 + 0x6A0) = r7;
            r7 = (u32)r7 >> r10;
            *(u32*)((u8*)r3 + 0x69C) = r9;
            tmp = r7 + tmp;
            *(u32*)((u8*)r3 + 0x6A4) = r11;
            r11 = 0x20 - r5;
            r7 = (u32)tmp >> r11;
        }
        tmp = (s16)r7;
        r6 = __cntlzw(tmp);
        tmp = 0x20 - r5;
        if ((s32)r6 > (s32)tmp) {
            tmp = -0x1;
            tmp = tmp << r5;
            r7 = tmp + r7;
            r7 = r7 + 0x1;
    }
    }
    tmp = 0x60;
    /* dcbz r4, tmp */;
    tmp = *(s16*)((u8*)r3 + 0x68A);
    r5 = (u32)lbl_80279AE8;
    r8 = (u32)lbl_80279AE8;
    tmp = tmp + r7;
    *(u16*)((u8*)r3 + 0x68A) = tmp;
    r6 = 0x1;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    while (1) {
    do {
        if ((s32)r6 >= 0x40) break;
        r30 = lbl_8047B520;
        r31 = *(u32*)((u8*)r3 + 0x6A4);
        r11 = r30 + 0x20;
        r12 = *(u32*)((u8*)r3 + 0x6A0);
        r5 = r31 + 0x4;
        /* rlwnm r10, r12, r5, 27, 31 */;
        if ((s32)r31 <= 0x1c) {
            r5 = *(u8*)(r30 + r10);
            r11 = *(u8*)(r11 + r10);
            if ((s32)r5 != 0xff) {
                r31 = r31 + r11;
                *(u32*)((u8*)r3 + 0x6A4) = r31;
                break;
            }
            r7 = r30 + 0x44;
            r31 = r31 + 0x5;
            tmp = 0x14;
            r5 = 0x5;
            r7 = r7 + 0x14;
            do {
                r10 = r10 << 1;
                if ((s32)r31 != 0x21) {
                    /* rlwnm r11, r12, r31, 31, 31 */;
                    tmp = *(u32*)((u8*)r7 + 0x4);
                    r10 = r10 | r11;
                    r31 = r31 + 0x1;
                    continue;
                }
                r11 = *(u32*)((u8*)r3 + 0x69C);
                r31 = 0x1;
                r12 = *(u32*)((u8*)r11 + 0x4);
                tmp = *(u32*)((u8*)r7 + 0x4);
                *(u32*)((u8*)r3 + 0x69C) = r11;
                r10 = (r10 & ~0x00000001) | (((r12 << 1) | (r12 >> 31)) & 0x00000001);
                *(u32*)((u8*)r3 + 0x6A0) = r12;
                while (1) {
                    r31 = r31 + 0x1;
                    r5 = r5 + 0x1;
                    if ((s32)r10 <= (s32)tmp) break;
                    r10 = r10 << 1;
                    /* rlwnm r11, r12, r31, 31, 31 */;
                    tmp = *(u32*)((u8*)r7 + 0x4);
                    r10 = r10 | r11;

                }
                break;


            } while ((s32)r10 > (s32)tmp);

            *(u32*)((u8*)r3 + 0x6A4) = r31;
            tmp = r5 << 2;
            r5 = r30 + tmp;
            r7 = *(u32*)((u8*)r30 + 0x40);
            tmp = *(u32*)((u8*)r5 + 0x8C);
            tmp = tmp + r7;
            r5 = *(u8*)(r10 + tmp);
            break;
        }
        r10 = *(u32*)((u8*)r3 + 0x69C);
        if ((s32)r31 != 0x21) {
            /* rlwnm r5, r12, r5, 27, 31 */;
            if ((s32)r31 != 0x20) {
                r10 = *(u8*)(r30 + r5);
                r11 = *(u8*)(r11 + r5);
                r5 = r31 + r11;
                if ((s32)r10 != 0xff) {
                    *(u32*)((u8*)r3 + 0x6A4) = r5;
                }
                if ((s32)r5 <= 0x21) {
                    r5 = r10;
                    break;
                }
                r12 = *(u32*)((u8*)r10 + 0x4);
                *(u32*)((u8*)r3 + 0x69C) = r10;
                r5 = (r5 & ~0x0000000F) | (((r12 << 4) | (r12 >> 28)) & 0x0000000F);
                r10 = *(u8*)(r30 + r5);
                r11 = *(u8*)(r11 + r5);
                *(u32*)((u8*)r3 + 0x6A0) = r12;
                *(u32*)((u8*)r3 + 0x6A4) = r11;
                if ((s32)r10 != 0xff) {
                    r5 = r10;
                    break;
                }
                r7 = r30 + 0x44;
                r31 = 0x14;
                r7 = r7 + 0x14;
                r10 = r5 << 27;
                r31 = 0x5;
                r10 = (r10 & ~0x7FFFFFFF) | (((r12 << 31) | (r12 >> 1)) & 0x7FFFFFFF);
                do {
                    r12 = 0x1f - r31;
                    tmp = *(u32*)((u8*)r7 + 0x4);
                    r5 = (u32)r10 >> r12;
                    r31 = r31 + 0x1;
                } while ((s32)r5 > (s32)tmp);
                *(u32*)((u8*)r3 + 0x6A4) = r31;
            while (1) {
                    tmp = r31 << 2;
                    r9 = *(u32*)((u8*)r30 + 0x40);
                    r7 = r30 + tmp;
                    tmp = *(u32*)((u8*)r7 + 0x8C);
                    tmp = tmp + r9;
                    r5 = *(u8*)(r5 + tmp);
                    break;
            }
                r12 = *(u32*)((u8*)r10 + 0x4);
                r5 = (u32)r12 >> 27;
                *(u32*)((u8*)r3 + 0x69C) = r10;
                r31 = *(u8*)(r30 + r5);
                r11 = *(u8*)(r11 + r5);
                *(u32*)((u8*)r3 + 0x6A0) = r12;
                r11 = r11 + 0x1;
                if ((s32)r31 != 0xff) {
                    *(u32*)((u8*)r3 + 0x6A4) = r11;
                    r5 = r31;
                    break;
                }
                r31 = 0x5;
                r7 = 0x14;
                do {
                    r10 = 0x1f - r31;
                    r31 = r31 + 0x1;
                    r7 = r7 + 0x4;
                    r5 = (u32)r12 >> r10;
                    r9 = r30 + r7;
                    tmp = *(u32*)((u8*)r9 + 0x44);
                } while ((s32)r5 > (s32)tmp);
                tmp = r31 + 0x1;
                *(u32*)((u8*)r3 + 0x6A4) = tmp;
        }
                }
        tmp = 0x21 - r31;
        r5 = -0x1;
        r9 = r5 << tmp;
        r5 = r12 & ~r9;
        r9 = r30 + 0x44;
        r10 = *(u32*)((u8*)r3 + 0x69C);
        r7 = 0x21 - r31;
        r31 = r7 + 0x1;
        r7 = r7 << 2;
        r12 = *(u32*)((u8*)r10 + 0x4);
        r9 = r9 + r7;
        *(u32*)((u8*)r3 + 0x69C) = r10;
        r5 = r5 << 1;
        *(u32*)((u8*)r3 + 0x6A0) = r12;
        r5 = (r5 & ~0x00000001) | (((r12 << 1) | (r12 >> 31)) & 0x00000001);
        r7 = *(u32*)((u8*)r9 + 0x4);
        r10 = 0x2;
        while (1) {
            /* rlwnm r11, r12, r10, 31, 31 */;
            if ((s32)r5 <= (s32)r7) break;
            r5 = r5 << 1;
            r31 = r31 + 0x1;
            r7 = *(u32*)((u8*)r9 + 0x4);
            r5 = r5 + r11;
            r10 = r10 + 0x1;

        }
        *(u32*)((u8*)r3 + 0x6A4) = r10;
        tmp = r31 << 2;
        r7 = r30 + tmp;
        r9 = *(u32*)((u8*)r30 + 0x40);
        tmp = *(u32*)((u8*)r7 + 0x8C);
        tmp = tmp + r9;
        r5 = *(u8*)(r5 + tmp);
    } while (0);
        r30 = r5 & 0xF;
        r7 = (s32)r5 >> 4;
        if ((s32)r5 != (s32)r7) {
            r6 = r6 + r7;
            r9 = *(u32*)((u8*)r3 + 0x6A4);
            r10 = 0x21 - r9;
            r7 = *(u32*)((u8*)r3 + 0x6A0);
            /* subf. r11, r10, r30 */;
            if ((s32)r5 <= (s32)r7) {
                tmp = r9 + r30;
                r9 = r7 << r12;
                *(u32*)((u8*)r3 + 0x6A4) = tmp;
                tmp = 0x20 - r30;
                r7 = (u32)r9 >> tmp;
            } else {

                r9 = *(u32*)((u8*)r3 + 0x69C);
                tmp = r7 << r12;
                r7 = *(u32*)((u8*)r9 + 0x4);
                r11 = r11 + 0x1;
                *(u32*)((u8*)r3 + 0x6A0) = r7;
                r7 = (u32)r7 >> r10;
                *(u32*)((u8*)r3 + 0x69C) = r9;
                tmp = r7 + tmp;
                *(u32*)((u8*)r3 + 0x6A4) = r11;
                r11 = 0x20 - r30;
                r7 = (u32)tmp >> r11;
            }
            r5 = __cntlzw(r7);
            tmp = 0x20 - r30;
            if ((s32)r5 > (s32)tmp) {
                tmp = -0x1;
                tmp = tmp << r30;
                r7 = tmp + r7;
                r7 = r7 + 0x1;
            }
            r5 = r8 + r6;
            tmp = *(u8*)((u8*)r5 + 0x0);
            r5 = (s16)r7;
            tmp = tmp << 1;
            *(u16*)(r4 + tmp) = r5;
        } else {

            if ((s32)r7 != 0xf) return;
            r6 = r6 + 0xf;
        }
        r6 = r6 + 0x1;

    }

    return;
}
#endif

/* 0x801EC368 | size: 0x6A8 | large */
#if 0
asm void fn_801EC368(void) {
#include "src/game/battle/battle_logic_fn_801EC368.inc"
}
#else
void fn_801EC368(void) {
    extern u8 lbl_80279AE8[];
    extern u32 lbl_8047B4E0;
    extern u32 lbl_8047B540;
    u8 sp[0x18];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    /* dcbz tmp, r4 */;
    r12 = lbl_8047B4E0;
    r11 = *(u32*)((u8*)r3 + 0x6A4);
    r9 = r12 + 0x20;
    r10 = *(u32*)((u8*)r3 + 0x6A0);
    r5 = r11 + 0x4;
    /* rlwnm r8, r10, r5, 27, 31 */;
    if ((s32)r11 <= 0x1c) {
        r5 = *(u8*)(r12 + r8);
        r9 = *(u8*)(r9 + r8);
        if ((s32)r5 != 0xff) {
            r11 = r11 + r9;
            *(u32*)((u8*)r3 + 0x6A4) = r11;
            goto L_801EC5FC;
        }
        r6 = r12 + 0x44;
        r11 = r11 + 0x5;
        tmp = 0x14;
        r5 = 0x5;
        r6 = r6 + 0x14;
        do {
            r8 = r8 << 1;
            if ((s32)r11 != 0x21) {
                /* rlwnm r9, r10, r11, 31, 31 */;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r8 = r8 | r9;
                r11 = r11 + 0x1;
                continue;
            }
            r9 = *(u32*)((u8*)r3 + 0x69C);
            r11 = 0x1;
            r10 = *(u32*)((u8*)r9 + 0x4);
            tmp = *(u32*)((u8*)r6 + 0x4);
            *(u32*)((u8*)r3 + 0x69C) = r9;
            r8 = (r8 & ~0x00000001) | (((r10 << 1) | (r10 >> 31)) & 0x00000001);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            while (1) {
                r11 = r11 + 0x1;
                r5 = r5 + 0x1;
                if ((s32)r8 <= (s32)tmp) break;
                r8 = r8 << 1;
                /* rlwnm r9, r10, r11, 31, 31 */;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r8 = r8 | r9;

            }
            break;


        } while ((s32)r8 > (s32)tmp);

        *(u32*)((u8*)r3 + 0x6A4) = r11;
        tmp = r5 << 2;
        r5 = r12 + tmp;
        r6 = *(u32*)((u8*)r12 + 0x40);
        tmp = *(u32*)((u8*)r5 + 0x8C);
        tmp = tmp + r6;
        r5 = *(u8*)(r8 + tmp);
        goto L_801EC5FC;
    }
    r8 = *(u32*)((u8*)r3 + 0x69C);
    if ((s32)r11 != 0x21) {
        /* rlwnm r5, r10, r5, 27, 31 */;
        if ((s32)r11 != 0x20) {
            r8 = *(u8*)(r12 + r5);
            r9 = *(u8*)(r9 + r5);
            r5 = r11 + r9;
            if ((s32)r8 != 0xff) {
                *(u32*)((u8*)r3 + 0x6A4) = r5;
            }
            if ((s32)r5 <= 0x21) {
                r5 = r8;
                goto L_801EC5FC;
            }
            r10 = *(u32*)((u8*)r8 + 0x4);
            *(u32*)((u8*)r3 + 0x69C) = r8;
            r5 = (r5 & ~0x0000000F) | (((r10 << 4) | (r10 >> 28)) & 0x0000000F);
            r8 = *(u8*)(r12 + r5);
            r9 = *(u8*)(r9 + r5);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            *(u32*)((u8*)r3 + 0x6A4) = r9;
            if ((s32)r8 != 0xff) {
                r5 = r8;
                goto L_801EC5FC;
            }
            r6 = r12 + 0x44;
            r11 = 0x14;
            r6 = r6 + 0x14;
            r8 = r5 << 27;
            r11 = 0x5;
            r8 = (r8 & ~0x7FFFFFFF) | (((r10 << 31) | (r10 >> 1)) & 0x7FFFFFFF);
            do {
                r10 = 0x1f - r11;
                tmp = *(u32*)((u8*)r6 + 0x4);
                r5 = (u32)r8 >> r10;
                r11 = r11 + 0x1;
            } while ((s32)r5 > (s32)tmp);
            *(u32*)((u8*)r3 + 0x6A4) = r11;
        while (1) {
                tmp = r11 << 2;
                r7 = *(u32*)((u8*)r12 + 0x40);
                r6 = r12 + tmp;
                tmp = *(u32*)((u8*)r6 + 0x8C);
                tmp = tmp + r7;
                r5 = *(u8*)(r5 + tmp);
                goto L_801EC5FC;
        }
            r10 = *(u32*)((u8*)r8 + 0x4);
            r5 = (u32)r10 >> 27;
            *(u32*)((u8*)r3 + 0x69C) = r8;
            r11 = *(u8*)(r12 + r5);
            r9 = *(u8*)(r9 + r5);
            *(u32*)((u8*)r3 + 0x6A0) = r10;
            r9 = r9 + 0x1;
            if ((s32)r11 != 0xff) {
                *(u32*)((u8*)r3 + 0x6A4) = r9;
                r5 = r11;
                goto L_801EC5FC;
            }
            r11 = 0x5;
            r6 = 0x14;
            do {
                r8 = 0x1f - r11;
                r11 = r11 + 0x1;
                r6 = r6 + 0x4;
                r5 = (u32)r10 >> r8;
                r7 = r12 + r6;
                tmp = *(u32*)((u8*)r7 + 0x44);
            } while ((s32)r5 > (s32)tmp);
            tmp = r11 + 0x1;
            *(u32*)((u8*)r3 + 0x6A4) = tmp;
    }
            }
    tmp = 0x21 - r11;
    r5 = -0x1;
    r7 = r5 << tmp;
    r5 = r10 & ~r7;
    r7 = r12 + 0x44;
    r8 = *(u32*)((u8*)r3 + 0x69C);
    r6 = 0x21 - r11;
    r11 = r6 + 0x1;
    r6 = r6 << 2;
    r10 = *(u32*)((u8*)r8 + 0x4);
    r7 = r7 + r6;
    *(u32*)((u8*)r3 + 0x69C) = r8;
    r5 = r5 << 1;
    *(u32*)((u8*)r3 + 0x6A0) = r10;
    r5 = (r5 & ~0x00000001) | (((r10 << 1) | (r10 >> 31)) & 0x00000001);
    r6 = *(u32*)((u8*)r7 + 0x4);
    r8 = 0x2;
    while (1) {
        /* rlwnm r9, r10, r8, 31, 31 */;
        if ((s32)r5 <= (s32)r6) break;
        r5 = r5 << 1;
        r11 = r11 + 0x1;
        r6 = *(u32*)((u8*)r7 + 0x4);
        r5 = r5 + r9;
        r8 = r8 + 0x1;

    }
    *(u32*)((u8*)r3 + 0x6A4) = r8;
    tmp = r11 << 2;
    r6 = r12 + tmp;
    r7 = *(u32*)((u8*)r12 + 0x40);
    tmp = *(u32*)((u8*)r6 + 0x8C);
    tmp = tmp + r7;
    r5 = *(u8*)(r5 + tmp);
L_801EC5FC:
    tmp = 0x20;
    /* dcbz r4, tmp */;
    r7 = 0x0;
    tmp = 0x40;
    /* dcbz r4, tmp */;
    if ((s32)r5 != 0) {
        r9 = *(u32*)((u8*)r3 + 0x6A4);
        r10 = 0x21 - r9;
        r7 = *(u32*)((u8*)r3 + 0x6A0);
        /* subf. r11, r10, r5 */;
        if ((s32)r5 <= 0) {
            tmp = r9 + r5;
            r9 = r7 << r12;
            *(u32*)((u8*)r3 + 0x6A4) = tmp;
            tmp = 0x20 - r5;
            r7 = (u32)r9 >> tmp;
        } else {

            r9 = *(u32*)((u8*)r3 + 0x69C);
            tmp = r7 << r12;
            r7 = *(u32*)((u8*)r9 + 0x4);
            r11 = r11 + 0x1;
            *(u32*)((u8*)r3 + 0x6A0) = r7;
            r7 = (u32)r7 >> r10;
            *(u32*)((u8*)r3 + 0x69C) = r9;
            tmp = r7 + tmp;
            *(u32*)((u8*)r3 + 0x6A4) = r11;
            r11 = 0x20 - r5;
            r7 = (u32)tmp >> r11;
        }
        tmp = (s16)r7;
        r6 = __cntlzw(tmp);
        tmp = 0x20 - r5;
        if ((s32)r6 > (s32)tmp) {
            tmp = -0x1;
            tmp = tmp << r5;
            r7 = tmp + r7;
            r7 = r7 + 0x1;
    }
    }
    tmp = 0x60;
    /* dcbz r4, tmp */;
    tmp = *(s16*)((u8*)r3 + 0x690);
    r5 = (u32)lbl_80279AE8;
    r8 = (u32)lbl_80279AE8;
    tmp = tmp + r7;
    *(u16*)((u8*)r3 + 0x690) = tmp;
    r6 = 0x1;
    *(u16*)((u8*)r4 + 0x0) = tmp;
    while (1) {
    do {
        if ((s32)r6 >= 0x40) break;
        r30 = lbl_8047B540;
        r31 = *(u32*)((u8*)r3 + 0x6A4);
        r11 = r30 + 0x20;
        r12 = *(u32*)((u8*)r3 + 0x6A0);
        r5 = r31 + 0x4;
        /* rlwnm r10, r12, r5, 27, 31 */;
        if ((s32)r31 <= 0x1c) {
            r5 = *(u8*)(r30 + r10);
            r11 = *(u8*)(r11 + r10);
            if ((s32)r5 != 0xff) {
                r31 = r31 + r11;
                *(u32*)((u8*)r3 + 0x6A4) = r31;
                break;
            }
            r7 = r30 + 0x44;
            r31 = r31 + 0x5;
            tmp = 0x14;
            r5 = 0x5;
            r7 = r7 + 0x14;
            do {
                r10 = r10 << 1;
                if ((s32)r31 != 0x21) {
                    /* rlwnm r11, r12, r31, 31, 31 */;
                    tmp = *(u32*)((u8*)r7 + 0x4);
                    r10 = r10 | r11;
                    r31 = r31 + 0x1;
                    continue;
                }
                r11 = *(u32*)((u8*)r3 + 0x69C);
                r31 = 0x1;
                r12 = *(u32*)((u8*)r11 + 0x4);
                tmp = *(u32*)((u8*)r7 + 0x4);
                *(u32*)((u8*)r3 + 0x69C) = r11;
                r10 = (r10 & ~0x00000001) | (((r12 << 1) | (r12 >> 31)) & 0x00000001);
                *(u32*)((u8*)r3 + 0x6A0) = r12;
                while (1) {
                    r31 = r31 + 0x1;
                    r5 = r5 + 0x1;
                    if ((s32)r10 <= (s32)tmp) break;
                    r10 = r10 << 1;
                    /* rlwnm r11, r12, r31, 31, 31 */;
                    tmp = *(u32*)((u8*)r7 + 0x4);
                    r10 = r10 | r11;

                }
                break;


            } while ((s32)r10 > (s32)tmp);

            *(u32*)((u8*)r3 + 0x6A4) = r31;
            tmp = r5 << 2;
            r5 = r30 + tmp;
            r7 = *(u32*)((u8*)r30 + 0x40);
            tmp = *(u32*)((u8*)r5 + 0x8C);
            tmp = tmp + r7;
            r5 = *(u8*)(r10 + tmp);
            break;
        }
        r10 = *(u32*)((u8*)r3 + 0x69C);
        if ((s32)r31 != 0x21) {
            /* rlwnm r5, r12, r5, 27, 31 */;
            if ((s32)r31 != 0x20) {
                r10 = *(u8*)(r30 + r5);
                r11 = *(u8*)(r11 + r5);
                r5 = r31 + r11;
                if ((s32)r10 != 0xff) {
                    *(u32*)((u8*)r3 + 0x6A4) = r5;
                }
                if ((s32)r5 <= 0x21) {
                    r5 = r10;
                    break;
                }
                r12 = *(u32*)((u8*)r10 + 0x4);
                *(u32*)((u8*)r3 + 0x69C) = r10;
                r5 = (r5 & ~0x0000000F) | (((r12 << 4) | (r12 >> 28)) & 0x0000000F);
                r10 = *(u8*)(r30 + r5);
                r11 = *(u8*)(r11 + r5);
                *(u32*)((u8*)r3 + 0x6A0) = r12;
                *(u32*)((u8*)r3 + 0x6A4) = r11;
                if ((s32)r10 != 0xff) {
                    r5 = r10;
                    break;
                }
                r7 = r30 + 0x44;
                r31 = 0x14;
                r7 = r7 + 0x14;
                r10 = r5 << 27;
                r31 = 0x5;
                r10 = (r10 & ~0x7FFFFFFF) | (((r12 << 31) | (r12 >> 1)) & 0x7FFFFFFF);
                do {
                    r12 = 0x1f - r31;
                    tmp = *(u32*)((u8*)r7 + 0x4);
                    r5 = (u32)r10 >> r12;
                    r31 = r31 + 0x1;
                } while ((s32)r5 > (s32)tmp);
                *(u32*)((u8*)r3 + 0x6A4) = r31;
            while (1) {
                    tmp = r31 << 2;
                    r9 = *(u32*)((u8*)r30 + 0x40);
                    r7 = r30 + tmp;
                    tmp = *(u32*)((u8*)r7 + 0x8C);
                    tmp = tmp + r9;
                    r5 = *(u8*)(r5 + tmp);
                    break;
            }
                r12 = *(u32*)((u8*)r10 + 0x4);
                r5 = (u32)r12 >> 27;
                *(u32*)((u8*)r3 + 0x69C) = r10;
                r31 = *(u8*)(r30 + r5);
                r11 = *(u8*)(r11 + r5);
                *(u32*)((u8*)r3 + 0x6A0) = r12;
                r11 = r11 + 0x1;
                if ((s32)r31 != 0xff) {
                    *(u32*)((u8*)r3 + 0x6A4) = r11;
                    r5 = r31;
                    break;
                }
                r31 = 0x5;
                r7 = 0x14;
                do {
                    r10 = 0x1f - r31;
                    r31 = r31 + 0x1;
                    r7 = r7 + 0x4;
                    r5 = (u32)r12 >> r10;
                    r9 = r30 + r7;
                    tmp = *(u32*)((u8*)r9 + 0x44);
                } while ((s32)r5 > (s32)tmp);
                tmp = r31 + 0x1;
                *(u32*)((u8*)r3 + 0x6A4) = tmp;
        }
                }
        tmp = 0x21 - r31;
        r5 = -0x1;
        r9 = r5 << tmp;
        r5 = r12 & ~r9;
        r9 = r30 + 0x44;
        r10 = *(u32*)((u8*)r3 + 0x69C);
        r7 = 0x21 - r31;
        r31 = r7 + 0x1;
        r7 = r7 << 2;
        r12 = *(u32*)((u8*)r10 + 0x4);
        r9 = r9 + r7;
        *(u32*)((u8*)r3 + 0x69C) = r10;
        r5 = r5 << 1;
        *(u32*)((u8*)r3 + 0x6A0) = r12;
        r5 = (r5 & ~0x00000001) | (((r12 << 1) | (r12 >> 31)) & 0x00000001);
        r7 = *(u32*)((u8*)r9 + 0x4);
        r10 = 0x2;
        while (1) {
            /* rlwnm r11, r12, r10, 31, 31 */;
            if ((s32)r5 <= (s32)r7) break;
            r5 = r5 << 1;
            r31 = r31 + 0x1;
            r7 = *(u32*)((u8*)r9 + 0x4);
            r5 = r5 + r11;
            r10 = r10 + 0x1;

        }
        *(u32*)((u8*)r3 + 0x6A4) = r10;
        tmp = r31 << 2;
        r7 = r30 + tmp;
        r9 = *(u32*)((u8*)r30 + 0x40);
        tmp = *(u32*)((u8*)r7 + 0x8C);
        tmp = tmp + r9;
        r5 = *(u8*)(r5 + tmp);
    } while (0);
        r30 = r5 & 0xF;
        r7 = (s32)r5 >> 4;
        if ((s32)r5 != (s32)r7) {
            r6 = r6 + r7;
            r9 = *(u32*)((u8*)r3 + 0x6A4);
            r10 = 0x21 - r9;
            r7 = *(u32*)((u8*)r3 + 0x6A0);
            /* subf. r11, r10, r30 */;
            if ((s32)r5 <= (s32)r7) {
                tmp = r9 + r30;
                r9 = r7 << r12;
                *(u32*)((u8*)r3 + 0x6A4) = tmp;
                tmp = 0x20 - r30;
                r7 = (u32)r9 >> tmp;
            } else {

                r9 = *(u32*)((u8*)r3 + 0x69C);
                tmp = r7 << r12;
                r7 = *(u32*)((u8*)r9 + 0x4);
                r11 = r11 + 0x1;
                *(u32*)((u8*)r3 + 0x6A0) = r7;
                r7 = (u32)r7 >> r10;
                *(u32*)((u8*)r3 + 0x69C) = r9;
                tmp = r7 + tmp;
                *(u32*)((u8*)r3 + 0x6A4) = r11;
                r11 = 0x20 - r30;
                r7 = (u32)tmp >> r11;
            }
            r5 = __cntlzw(r7);
            tmp = 0x20 - r30;
            if ((s32)r5 > (s32)tmp) {
                tmp = -0x1;
                tmp = tmp << r30;
                r7 = tmp + r7;
                r7 = r7 + 0x1;
            }
            r5 = r8 + r6;
            tmp = *(u8*)((u8*)r5 + 0x0);
            r5 = (s16)r7;
            tmp = tmp << 1;
            *(u16*)(r4 + tmp) = r5;
        } else {

            if ((s32)r7 != 0xf) return;
            r6 = r6 + 0xf;
        }
        r6 = r6 + 0x1;

    }

    return;
}
#endif

/* 0x801ECA10 | size: 0xA0 | medium */
#if 0
asm void fn_801ECA10(void) {
#include "src/game/battle/battle_logic_fn_801ECA10.inc"
}
#else
void fn_801ECA10(void) {
    extern u8 lbl_8046D500[];
    extern u32 lbl_80478D08;
    extern u32 lbl_8047B5B4;
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r31 = 0;

    r4 = (u32)lbl_8046D500;
    r31 = (u32)lbl_8046D500;
    r3 = lbl_80478D08;
    OSRegisterVersion((const char *)r3);
    r4 = 0xE0000000;
    *(u32*)((u8*)r31 + 0x100) = r4;
    r4 = r4 + 0x2000;
    *(u32*)((u8*)r31 + 0x104) = r4;
    r4 = r4 + 0x800;
    *(u32*)((u8*)r31 + 0x108) = r4;
    r4 = 0xE0000000;
    *(u32*)((u8*)r31 + 0x10C) = r4;
    r4 = r4 + 0x2800;
    *(u32*)((u8*)r31 + 0x110) = r4;
    r4 = r4 + 0xa00;
    *(u32*)((u8*)r31 + 0x114) = r4;
    r3 = 0x4;
    r3 = r3 | (0x4 << 16);
    /* mtspr GQR2, r3 */;
    r3 = 0x5;
    r3 = r3 | (0x5 << 16);
    /* mtspr GQR3, r3 */;
    r3 = 0x6;
    r3 = r3 | (0x6 << 16);
    /* mtspr GQR4, r3 */;
    r3 = 0x7;
    r3 = r3 | (0x7 << 16);
    /* mtspr GQR5, r3 */;
    tmp = 0x1;
    lbl_8047B5B4 = tmp;
    r3 = 0x1;
    return;
}
#endif

/* 0x801ECAB0 | size: 0x464 | large */
#if 0
asm void fn_801ECAB0(void) {
#include "src/game/battle/battle_logic_fn_801ECAB0.inc"
}
#else
void fn_801ECAB0(void) {
    extern void fn_800C4C50();
    extern void fn_800C4C98();
    extern void fn_801ECF14();
    extern void fn_801ECFA4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r20 = 0;
    u32 r21 = 0;
    u32 r22 = 0;
    u32 r23 = 0;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = r4 + 0x0;
    if (r3 == 0) { r3 = 0x0; return; }
    if (r31 == 0) {

        r3 = 0x0;
        return;
    }
    tmp = *(u32*)((u8*)r31 + 0x0);
    r4 = r31 + 0x50;
    r27 = r31 + tmp;
    r27 = r27 + 0x50;
    if ((s32)r5 == 1) {
        tmp = *(u32*)((u8*)r31 + 0x4);
        r30 = r3 + 0x0;
        r26 = 0x1;
        tmp = tmp << 1;
        r29 = r3 + tmp;
    } else {

        r30 = r3 + 0x0;
        r29 = r3 + 0x2;
        r26 = 0x2;
    }
    tmp = *(u32*)((u8*)r31 + 0x0);
    if (tmp == 0) {
        r3 = (u32)sp + 0x14;
        fn_801ECFA4();
        r25 = 0x10000;
        r21 = *(s16*)((u8*)r31 + 0x48);
        r23 = 0x80000000;
        r20 = *(s16*)((u8*)r31 + 0x4A);
        r27 = r26 << 1;
        r22 = 0x0;
        while (1) {
        do {
            tmp = *(u32*)((u8*)r31 + 0x4);
            if (r22 >= tmp) break;
            r3 = (u32)sp + 0x14;
            fn_801ECF14();
            r5 = *(u8*)((u8*)(u32)sp + 0x1C);
            r6 = (s16)r20;
            tmp = *(u8*)((u8*)(u32)sp + 0x1D);
            r4 = (s16)r21;
            r5 = r5 << 2;
            r5 = r31 + r5;
            r7 = *(s16*)((u8*)r5 + 0xA);
            tmp = r3 << tmp;
            r3 = *(s16*)((u8*)r5 + 0x8);
            r5 = tmp << 11;
            r6 = r7 * r6;
            r4 = r3 * r4;
            r3 = (s32)r6 >> 31;
            tmp = (s32)r4 >> 31;
            r4 = r6 + r4;
            r3 = r3 + tmp; /* +carry */;
            tmp = (s32)r5 >> 31;
            r4 = r4 + r5;
            r3 = r3 + tmp; /* +carry */;
            r5 = 0x5;
            fn_800C4C50();
            tmp = r4 & r26;
            tmp = tmp & 0xFFFF;
            r6 = 0x0;
            r7 = r3 & r6;
            if (tmp > 0x8000) {
                r4 = r4 + r25;
                r3 = r3 + r6; /* +carry */;
                break;
            }
            if (tmp != 0x8000) break;
            tmp = r4 & r25;
            r5 = tmp ^ r6;
            tmp = r7 ^ r6;
            /* or. tmp, r5, tmp */;
            if (tmp == 0x8000) break;
            r4 = r4 + r25;
            r3 = r3 + r6; /* +carry */;
        } while (0);
            tmp = 0x0;
            tmp = r24 - r4;
            r5 = r6 - r5; /* -borrow */;
            r5 = r6 - r6; /* -borrow */;
            /* neg. r5, r5 */;
            if (tmp != 0x8000) {
                r3 = 0x0;
            }
            tmp = -0x1;
            tmp = r4 - r23;
            r5 = r6 - r5; /* -borrow */;
            r5 = r6 - r6; /* -borrow */;
            /* neg. r5, r5 */;
            if (tmp != 0x8000) {
                r4 = 0x80000000;
                r3 = -0x1;
            }
            r5 = 0x10;
            fn_800C4C98();
            *(u16*)((u8*)r29 + 0x0) = r4;
            r20 = r21;
            r21 = r4 + 0x0;
            *(u16*)((u8*)r30 + 0x0) = r4;
            r29 = r29 + r27;
            r30 = r30 + r27;
            r22 = r22 + 0x1;


        }
        r3 = *(u32*)((u8*)r31 + 0x4);
        return;
    }
    r3 = (u32)sp + 0x14;
    fn_801ECFA4();
    r24 = 0x10000;
    r22 = *(s16*)((u8*)r31 + 0x48);
    r25 = 0x80000000;
    r20 = *(s16*)((u8*)r31 + 0x4A);
    r28 = r26 << 1;
    r21 = 0x0;
    while (1) {
    do {
        tmp = *(u32*)((u8*)r31 + 0x4);
        if (r21 >= tmp) break;
        r3 = (u32)sp + 0x14;
        fn_801ECF14();
        r5 = *(u8*)((u8*)(u32)sp + 0x1C);
        r6 = (s16)r20;
        tmp = *(u8*)((u8*)(u32)sp + 0x1D);
        r4 = (s16)r22;
        r5 = r5 << 2;
        r5 = r31 + r5;
        r7 = *(s16*)((u8*)r5 + 0xA);
        tmp = r3 << tmp;
        r3 = *(s16*)((u8*)r5 + 0x8);
        r5 = tmp << 11;
        r6 = r7 * r6;
        r4 = r3 * r4;
        r3 = (s32)r6 >> 31;
        tmp = (s32)r4 >> 31;
        r4 = r6 + r4;
        r3 = r3 + tmp; /* +carry */;
        tmp = (s32)r5 >> 31;
        r4 = r4 + r5;
        r3 = r3 + tmp; /* +carry */;
        r5 = 0x5;
        fn_800C4C50();
        tmp = r4 & r23;
        tmp = tmp & 0xFFFF;
        r6 = 0x0;
        r7 = r3 & r6;
        if (tmp > 0x8000) {
            r4 = r4 + r24;
            r3 = r3 + r6; /* +carry */;
            break;
        }
        if (tmp != 0x8000) break;
        tmp = r4 & r24;
        r5 = tmp ^ r6;
        tmp = r7 ^ r6;
        /* or. tmp, r5, tmp */;
        if (tmp == 0x8000) break;
        r4 = r4 + r24;
        r3 = r3 + r6; /* +carry */;
    } while (0);
        r7 = 0x0;
        tmp = r26 - r4;
        r5 = r6 - r5; /* -borrow */;
        r5 = r6 - r6; /* -borrow */;
        /* neg. r5, r5 */;
        if (tmp != 0x8000) {
            r3 = r7 + 0x0;
        }
        r7 = -0x1;
        tmp = r4 - r25;
        r5 = r6 - r5; /* -borrow */;
        r5 = r6 - r6; /* -borrow */;
        /* neg. r5, r5 */;
        if (tmp != 0x8000) {
            r4 = 0x80000000;
            r3 = r7 + 0x0;
        }
        r5 = 0x10;
        fn_800C4C98();
        *(u16*)((u8*)r29 + 0x0) = r4;
        r20 = r22 + 0x0;
        r22 = r4 + 0x0;
        r29 = r29 + r28;
        r21 = r21 + 0x1;


    }
    r3 = (u32)sp + 0x14;
    r4 = r27 + 0x0;
    fn_801ECFA4();
    r26 = 0x10000;
    r23 = *(s16*)((u8*)r31 + 0x4C);
    r27 = 0x80000000;
    r21 = *(s16*)((u8*)r31 + 0x4E);
    r22 = 0x0;
    while (1) {
    do {
        tmp = *(u32*)((u8*)r31 + 0x4);
        if (r22 >= tmp) break;
        r3 = (u32)sp + 0x14;
        fn_801ECF14();
        r5 = *(u8*)((u8*)(u32)sp + 0x1C);
        r6 = (s16)r21;
        tmp = *(u8*)((u8*)(u32)sp + 0x1D);
        r4 = (s16)r23;
        r5 = r5 << 2;
        r5 = r31 + r5;
        r7 = *(s16*)((u8*)r5 + 0x2A);
        tmp = r3 << tmp;
        r3 = *(s16*)((u8*)r5 + 0x28);
        r5 = tmp << 11;
        r6 = r7 * r6;
        r4 = r3 * r4;
        r3 = (s32)r6 >> 31;
        tmp = (s32)r4 >> 31;
        r4 = r6 + r4;
        r3 = r3 + tmp; /* +carry */;
        tmp = (s32)r5 >> 31;
        r4 = r4 + r5;
        r3 = r3 + tmp; /* +carry */;
        r5 = 0x5;
        fn_800C4C50();
        tmp = r4 & r25;
        tmp = tmp & 0xFFFF;
        r6 = 0x0;
        r7 = r3 & r6;
        if (tmp > 0x8000) {
            r4 = r4 + r26;
            r3 = r3 + r6; /* +carry */;
            break;
        }
        if (tmp != 0x8000) break;
        tmp = r4 & r26;
        r5 = tmp ^ r6;
        tmp = r7 ^ r6;
        /* or. tmp, r5, tmp */;
        if (tmp == 0x8000) break;
        r4 = r4 + r26;
        r3 = r3 + r6; /* +carry */;
    } while (0);
        r7 = 0x0;
        tmp = r29 - r4;
        r5 = r6 - r5; /* -borrow */;
        r5 = r6 - r6; /* -borrow */;
        /* neg. r5, r5 */;
        if (tmp != 0x8000) {
            r3 = r7 + 0x0;
        }
        r7 = -0x1;
        tmp = r4 - r27;
        r5 = r6 - r5; /* -borrow */;
        r5 = r6 - r6; /* -borrow */;
        /* neg. r5, r5 */;
        if (tmp != 0x8000) {
            r4 = 0x80000000;
            r3 = r7 + 0x0;
        }
        r5 = 0x10;
        fn_800C4C98();
        *(u16*)((u8*)r30 + 0x0) = r4;
        r21 = r23 + 0x0;
        r23 = r4 + 0x0;
        r30 = r30 + r28;
        r22 = r22 + 0x1;


    }

    r3 = *(u32*)((u8*)r31 + 0x4);

    return;
}
#endif

/* 0x801ECF14 | size: 0x90 | medium */
#if 0
asm void fn_801ECF14(void) {
#include "src/game/battle/battle_logic_fn_801ECF14.inc"
}
#else
void fn_801ECF14(void) {
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    tmp = *(u32*)((u8*)r3 + 0x4);
    tmp = tmp & 0xF;
    if ((s32)tmp == 0) {
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r4 + 0x0);
        /* extrwi tmp, tmp, 3, 25 */;
        *(u8*)((u8*)r3 + 0x8) = tmp;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r4 + 0x0);
        tmp = tmp & 0xF;
        *(u8*)((u8*)r3 + 0x9) = tmp;
        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = r4 + 0x1;
        *(u32*)((u8*)r3 + 0x0) = tmp;
        r4 = *(u32*)((u8*)r3 + 0x4);
        tmp = r4 + 0x2;
        *(u32*)((u8*)r3 + 0x4) = tmp;
    }
    tmp = *(u32*)((u8*)r3 + 0x4);
    tmp = tmp & 0x1;
    if ((s32)tmp != 0) {
        r5 = *(u32*)((u8*)r3 + 0x0);
        r4 = *(u8*)((u8*)r5 + 0x0);
        tmp = r5 + 0x1;
        *(u32*)((u8*)r3 + 0x0) = tmp;
        tmp = r4 << 28;
        r5 = (s32)tmp >> 28;
    } else {

        r4 = *(u32*)((u8*)r3 + 0x0);
        tmp = *(u8*)((u8*)r4 + 0x0);
        /* extlwi tmp, tmp, 4, 24 */;
        r5 = (s32)tmp >> 28;
    }
    r4 = *(u32*)((u8*)r3 + 0x4);
    tmp = r4 + 0x1;
    *(u32*)((u8*)r3 + 0x4) = tmp;
    r3 = r5;
    return;
}
#endif

/* 0x801ECFA4 | size: 0x3C | small */
#if 0
asm void fn_801ECFA4(void) {
#include "src/game/battle/battle_logic_fn_801ECFA4.inc"
}
#else
void fn_801ECFA4(u8 *obj, u8 *src) {
    u8 *p;
    *(u8**)(obj + 0x0) = src;
    *(u32*)(obj + 0x4) = 2;
    p = *(u8**)(obj + 0x0);
    *(u8*)(obj + 0x8) = (*p >> 4) & 7;
    p = *(u8**)(obj + 0x0);
    *(u8*)(obj + 0x9) = *p & 0xf;
    p = *(u8**)(obj + 0x0);
    *(u8**)(obj + 0x0) = p + 1;
}
#endif

/* 0x801ECFE0 | size: 0xEC | medium */
#if 0
asm void fn_801ECFE0(void) {
#include "src/game/battle/battle_logic_fn_801ECFE0.inc"
}
#else
struct CopyBlk138 {
    u32 words[78];
};

#pragma push
#pragma optimize_for_size on
int fn_801ECFE0(void *arg0, void *arg1) {
    extern void *fn_80129280(int, int);
    extern u8 fn_8011ED68(void *);
    extern void fn_8011EE28(void *);
    extern void fn_80124A60(void *);
    u8 *src;
    void *obj;
    void *tmp;

    obj = arg0;
    if (arg0 == 0) {
        obj = fn_80129280(0, 0xB);
    }
    if (arg1 == 0) {
        return 0;
    }
    tmp = obj;
    if (obj == 0) {
        tmp = fn_80129280(0, 0xB);
    }
    src = (u8 *)tmp + 8;
    if (src == 0) {
        return 0;
    }
    *(struct CopyBlk138 *)arg1 = *(struct CopyBlk138 *)src;
    if (fn_8011ED68(src)) {
        fn_8011EE28(src);
    }
    if (obj == 0) {
        obj = fn_80129280(0, 0xB);
    }
    if (obj != 0) {
        *(u8 *)obj = 0;
    }
    fn_80124A60(src);
    return 1;
}
#pragma pop
#endif

/* 0x801ED0CC | size: 0x14C | medium */
#if 0
asm void fn_801ED0CC(void) {
#include "src/game/battle/battle_logic_fn_801ED0CC.inc"
}
#else
#pragma push
#pragma optimize_for_size on
int fn_801ED0CC(void *arg0, void *arg1) {
    extern void *fn_80129280(int, int);
    extern u8 fn_8011F4A8(void *);
    extern u8 fn_80121ADC(void *, int);
    extern void fn_80121B4C(void *, int);
    extern u8 fn_8011ED68(void *);
    extern void *fn_8011EE28(void *);
    extern void fn_80124A60(void *);
    void *obj;
    u8 *dst;
    u8 flag;
    void *tmp;

    obj = arg0;
    if (arg0 == 0) {
        obj = fn_80129280(0, 0xB);
    }
    if (arg1 == 0) {
        return 0;
    }
    flag = fn_8011F4A8(arg1);
    tmp = obj;
    if (obj == 0) {
        tmp = fn_80129280(0, 0xB);
    }
    if (tmp != 0) {
        *(u8 *)((u8 *)tmp + 1) = flag;
    }
    tmp = obj;
    if (obj == 0) {
        tmp = fn_80129280(0, 0xB);
    }
    dst = (u8 *)tmp + 8;
    if (dst == 0) {
        return 0;
    }
    if (fn_80121ADC(arg1, 0x3E)) {
        fn_80121B4C(arg1, 0x3E);
    }
    *(struct CopyBlk138 *)dst = *(struct CopyBlk138 *)arg1;
    if (fn_8011ED68(arg1)) {
        *(u32 *)((u8 *)obj + 4) = (u32)fn_8011EE28(arg1);
    } else {
        *(u32 *)((u8 *)obj + 4) = 0;
    }
    if (obj == 0) {
        obj = fn_80129280(0, 0xB);
    }
    if (obj != 0) {
        *(u8 *)obj = 1;
    }
    fn_80124A60(arg1);
    return 1;
}
#pragma pop
#endif

/* 0x801ED218 | size: 0x34
 * Get the type byte from a pokemon data structure.
 * If ptr is NULL, calls fn_80129280 to get a default.
 */
u32 fn_801ED218(u8* data) {
    if (data == NULL) {
        data = (u8*)fn_80129280(0, 0xB);
    }
    return *(u8*)(data + 0x0);
}

/* 0x801ED24C | size: 0x48 | small */
#if 0
asm void fn_801ED24C(void) {
#include "src/game/battle/battle_logic_fn_801ED24C.inc"
}
#else
s32 fn_801ED24C(u8 *data) {
    extern u8 *fn_80129280(void *, u32);
    if (data == 0) {
        data = fn_80129280(0, 0xb);
    }
    if (*(u8*)data != 0) {
        return *(u32*)(data + 0x4);
    }
    return -1;
}
#endif

/* 0x801ED294 | size: 0x48 | small */
#if 0
asm void fn_801ED294(void) {
#include "src/game/battle/battle_logic_fn_801ED294.inc"
}
#else
s32 fn_801ED294(u8 *data) {
    extern u8 *fn_80129280(void *, u32);
    if (data == 0) {
        data = fn_80129280(0, 0xb);
    }
    if (*(u8*)data != 0) {
        return *(u8*)(data + 0x1);
    }
    return 0xff;
}
#endif

/* 0x801ED2DC | size: 0x34
 * Get a pointer to the name field (offset 0x8) of a pokemon data structure.
 * If ptr is NULL, calls fn_80129280 to get a default.
 */
u8* fn_801ED2DC(u8* data) {
    if (data == NULL) {
        data = (u8*)fn_80129280(0, 0xB);
    }
    return data + 0x8;
}

/* 0x801ED310 | size: 0x78 | small */
#if 0
asm void fn_801ED310(void) {
#include "src/game/battle/battle_logic_fn_801ED310.inc"
}
#else
void fn_801ED310(u8* data) {
    extern u8* fn_80129280(void*, u32);
    u8* saved;

    saved = data;
    if (saved == 0) {
        saved = fn_80129280(0, 0xB);
    }
    if (saved != 0) {
        data = saved;
        if (saved == 0) {
            data = fn_80129280(0, 0xB);
        }
        if (data != 0) {
            *(u16*)(data + 2) = 0;
        }
        saved[1] = 0;
        saved[0] = 0;
    }
}
#endif

/* 0x801ED388 | size: 0x30 | small */
#if 0
asm void fn_801ED388(void) {
#include "src/game/battle/battle_logic_fn_801ED388.inc"
}
#else
void fn_801ED388(void) {
    extern u32 lbl_8047B5B8;
    extern u32 heroMoveAddStepCallback(void *, u32);
    extern void fn_801ED3B8(void);
    lbl_8047B5B8 = heroMoveAddStepCallback(fn_801ED3B8, 0);
}
#endif

/* 0x801ED3B8 | size: 0x288 | large */
#if 0
asm void fn_801ED3B8(void) {
#include "src/game/battle/battle_logic_fn_801ED3B8.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801ED3B8(void) {
    extern void *fn_80129280(int, int);
    extern void *fn_8012AC08(void *, u16);
    extern u8 fn_80123FBC(void);
    extern u16 fn_8011EE40(void *);
    extern u8 fn_801EEC74(u16);
    extern void *fn_8011EE28(void *);
    extern u16 fn_801EE470(u16);
    extern void fn_8011F910(void *, int, int);
    extern void fn_801EE4DC(u16, u16);
    extern u32 fn_801906A0(int);
    extern void fn_8019075C(int, u16);
    extern u8 fn_8011F4A8(void *);
    extern u32 fn_8011F4C0(void *);
    extern void fn_80125424(void *, u32);
    extern void fn_8012361C(void *, u8, int, void *);
    u16 seed;
    int i;
    void *mgr;
    void *entity;
    u16 handle;
    u8 *obj;
    u16 handle2;
    void *p;
    u16 t;
    u32 v;
    u8 sp8;

    mgr = fn_80129280(0, 2);
    if (mgr != 0) {
        for (i = 0; (u16)i < 6; i++) {
            entity = fn_8012AC08(mgr, i);
            if (!fn_80123FBC()) {
                continue;
            }
            if (entity == 0) {
                continue;
            }
            handle = fn_8011EE40(entity);
            if (handle != 0) {
                if (fn_801EEC74(handle)) {
                    handle = 0;
                }
            }
            if (handle == 0) {
                continue;
            }
            if (fn_8011EE28(entity) == 0) {
                continue;
            }
            if (fn_801EE470(handle) >= 0x100) {
                fn_8011F910(entity, 0, 1);
                fn_801EE4DC(handle, 0);
            } else {
                t = fn_801EE470(handle);
                fn_801EE4DC(handle, t + 1);
            }
        }
    }
    if (fn_801906A0(0xD0) != 0) {
        seed = fn_801906A0(0xD1);
        if (seed < 0x2710) {
            seed = seed + 1;
        }
        fn_8019075C(0xD1, seed & 0xFFFF);
    }
    if (*(u8 *)fn_80129280(0, 0xB) == 0) {
        return;
    }
    obj = (u8 *)fn_80129280(0, 0xB) + 8;
    handle2 = 0;
    if (obj != 0) {
        handle2 = fn_8011EE40(obj);
        if (handle2 != 0) {
            if (fn_801EEC74(handle2)) {
                handle2 = 0;
            }
        }
        if (handle2 != 0) {
            if (fn_801EEC74(handle2)) {
                handle2 = 0;
            }
        }
    }
    if (handle2 != 0) {
        if (fn_8011EE28(obj) != 0) {
            if (fn_801EE470(handle2) >= 0x100) {
                fn_8011F910(obj, 0, 3);
                fn_801EE4DC(handle2, 0);
            } else {
                t = fn_801EE470(handle2);
                fn_801EE4DC(handle2, t + 1);
            }
        }
    } else {
        if (fn_8011F4A8(obj) < 0x64) {
            v = fn_8011F4C0(obj);
            if (v < (u32)-1) {
                v = v + 1;
            }
            fn_80125424(obj, v);
            sp8 = 0;
            fn_8012361C(obj, fn_8011F4A8(obj), 1, &sp8);
        }
    }
    p = fn_80129280(0, 0xB);
    if (p != 0) {
        *(u16 *)((u8 *)p + 2) = seed;
    }
}
#pragma pop
#endif

/* 0x801ED648 | size: 0x2C | small */
#if 0
asm void fn_801ED648(void) {
#include "src/game/battle/battle_logic_fn_801ED648.inc"
}
#else
void fn_801ED648(u32 arg) {
    extern u8 lbl_80375230[];
    extern void fn_800E01D0(u8 *fmt, u32 arg);
    fn_800E01D0(lbl_80375230, arg);
}
#endif

/* 0x801ED674 | size: 0xC | tiny */
#if 0
asm void fn_801ED674(void) {
#include "src/game/battle/battle_logic_fn_801ED674.inc"
}
#else
void fn_801ED674(void) {
    extern u8 lbl_8047B5C0;
    lbl_8047B5C0 = 0;
}
#endif

/* 0x801ED680 | size: 0xC0 | medium */
#if 0
asm void fn_801ED680(void) {
#include "src/game/battle/battle_logic_fn_801ED680.inc"
}
#else
void fn_801ED680(void *arg0) {
    extern u32 lbl_8046D630[];
    extern u8 lbl_8047B5C0;
    extern void *lbl_8047B5C4;
    extern u8 *lbl_8047B5C8;
    extern void *fn_800EFD3C(void *);
    s32 i;
    u8 *entry;
    u8 *records;
    void *res;

    lbl_8047B5C4 = arg0;
    if (*(u32 *)arg0 == 0x7B1EE3F0) {
        records = (u8 *)arg0 + 0x1C;
        lbl_8047B5C8 = records;
        i = 0;
        entry = records + *(u16 *)((u8 *)arg0 + 6) * 0xC;
        while (i < *(u16 *)((u8 *)lbl_8047B5C4 + 4)) {
            *(u32 *)(entry + 4) += (u32)arg0;
            res = fn_800EFD3C(*(void **)(entry + 4));
            if (res == 0) {
                break;
            }
            lbl_8046D630[*(u16 *)entry] = (u32)res;
            entry += 8;
            i++;
        }
    }
    lbl_8047B5C0 = 1;
}
#endif

/* 0x801ED740 | size: 0x40 | small */
#if 0
asm void fn_801ED740(void) {
#include "src/game/battle/battle_logic_fn_801ED740.inc"
}
#else
void fn_801ED740(void) {
    extern u8 lbl_8047B5C0;
    extern u8 lbl_8047B5C1;
    extern void fn_800FE834(u32, u32, u32, void *);
    extern void fn_801ED780(void);

    lbl_8047B5C0 = 0;
    lbl_8047B5C1 = 0;
    fn_800FE834(1, 0xf0, 0xa, fn_801ED780);
}
#endif

/* 0x801ED780 | size: 0x8B4 | massive */
#if 0
asm void fn_801ED780(void) {
#include "src/game/battle/battle_logic_fn_801ED780.inc"
}
#else
void fn_801ED780(void) {
    extern u8 lbl_80314958[];
    extern u8 lbl_80314C78[];
    extern u8 lbl_80375230[];
    extern u8 lbl_8046D630[];
    extern u8 lbl_8047B5C0;
    extern u32 lbl_8047B5C4;
    extern u32 lbl_8047B5C8;
    extern f32 lbl_8047E4D0;
    extern f32 lbl_8047E4D4;
    extern f32 lbl_8047E4D8;
    extern f32 lbl_8047E4DC;
    extern f32 lbl_8047E4E0;
    extern f32 lbl_8047E4E4;
    extern f32 lbl_8047E4E8;
    extern f32 lbl_8047E4EC;
    extern f32 lbl_8047E4F0;
    extern f64 lbl_8047E4F8;
    extern f64 lbl_8047E500;
    extern void GScameraGetPosition();
    extern void GScameraGetActiveCamera();
    extern void fn_800D2F34();
    extern void fn_800D59B8();
    extern void fn_800D5C18();
    extern void fn_800D6680();
    extern void fn_800D6728();
    extern void fn_800D67BC();
    extern void fn_800D6A00();
    extern void fn_800D7820();
    extern void fn_800D85D4();
    extern void fn_800D888C();
    extern void fn_800D88DC();
    extern void fn_800D9B58();
    extern void fn_800D9ED8();
    extern void fn_800DA028();
    extern void fn_800DA1E8();
    extern void fn_800DA2BC();
    extern void fn_800DA4C4();
    extern void fn_800E008C();
    extern void fn_800E00AC();
    extern void fn_800E013C();
    extern void fn_800EF4F4();
    extern void fn_800EF4FC();
    extern void fn_80111C24();
    u8 sp[0xC0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f5 = 0.0f;
    f32 f27 = 0.0f;
    f32 f28 = 0.0f;
    f32 f29 = 0.0f;
    f32 f30 = 0.0f;
    f32 f31 = 0.0f;

    tmp = lbl_8047B5C0;
    if (tmp == 0) return;
    tmp = *(u8*)&lbl_8047B5C1;
    if (tmp == 0) {
        return;
    }
    GScameraGetActiveCamera();
    if (r3 == 0) return;
    r4 = (u32)sp + 0x8;
    GScameraGetPosition();
    r3 = (u32)lbl_80375230;
    r4 = (u32)sp + 0x8;
    r3 = (u32)lbl_80375230;
    fn_80111C24();
    if ((s32)r3 == 1) return;
    r3 = (u32)lbl_80375230;
    r4 = (u32)sp + 0x2c;
    r3 = (u32)lbl_80375230;
    fn_800D2F34();
    /* mr. r27, r3 */;
    if ((s32)r3 == 1) return;
    f3 = lbl_8047E4D0;
    r3 = (u32)sp + 0x20;
    f0 = *(f32*)(sp + 0x2C);
    f2 = lbl_8047E4D4;
    f1 = *(f32*)(sp + 0x30);
    f3 = f3 - f0;
    f0 = lbl_8047E4D8;
    f1 = f2 - f1;
    *(f32*)(sp + 0x20) = f3;
    *(f32*)(sp + 0x24) = f1;
    *(f32*)(sp + 0x28) = f0;
    fn_800E008C();
    f31 = f1;
    f0 = lbl_8047E4D8;
    if (f31 > f0) {
    } else {

        f1 = -f31;
    }
    f0 = lbl_8047E4DC;
    if (f1 < f0) return;
    r3 = lbl_8047B5C4;
    tmp = 0x43300000;
    *(u32*)(sp + 0x38) = tmp;
    tmp = *(u32*)((u8*)r3 + 0x8);
    f1 = lbl_8047E4F8;
    *(u32*)(sp + 0x3C) = tmp;
    f0 = f0 - f1;
    if (f31 > f0) {
        return;
    }
    r3 = 0x3;
    fn_800D88DC();
    r3 = 0x4;
    fn_800D888C();
    f1 = lbl_8047E4D8;
    f3 = lbl_8047E4E0;
    f2 = f1;
    f4 = lbl_8047E4E4;
    fn_800D9B58();
    r3 = 0x1;
    r4 = 0x1;
    r5 = 0x1;
    fn_800DA4C4();
    r3 = 0x1;
    r4 = 0x1;
    r5 = 0x0;
    fn_800DA2BC();
    r3 = 0x0;
    r4 = 0x1;
    r5 = 0x1;
    fn_800DA1E8();
    r3 = 0x0;
    fn_800DA028();
    r3 = 0x1;
    fn_800D9ED8();
    if ((s32)r27 == 2) {
    do {
        f1 = f31;
        r3 = (u32)sp + 0x20;
        r4 = r3;
        fn_800E00AC();
        r5 = lbl_8047B5C4;
        r3 = 0x43300000;
        r4 = *(u32*)((u8*)r5 + 0xC);
        f4 = lbl_8047E4F8;
        f0 = f0 - f4;
        if (f31 < f0) {
            f1 = lbl_8047E4E8;
            break;
        }
        tmp = *(u32*)((u8*)r5 + 0x8);
        tmp = tmp - r4;
        f0 = lbl_8047E4D8;
        *(u32*)(sp + 0x3C) = tmp;
        f1 = f1 - f4;
        f2 = f31 - f1;
        *(u32*)(sp + 0x4C) = tmp;
        f3 = f3 - f4;
        f2 = f3 - f2;
        f1 = f1 - f4;
        f1 = f2 / f1;
        if (f1 < f0) {
            f1 = f0;
            break;
        }
        f0 = lbl_8047E4E8;
        if (f1 <= f0) break;
        f1 = f0;
    } while (0);
    do {
        f0 = lbl_8047E4EC;
        r3 = 0x4;
        f0 = f0 * f1;
        f0 = (f64)(s32)f0;
        fn_800D6A00();
        r3 = (u32)lbl_80314C78;
        r3 = (u32)lbl_80314C78;
        fn_800D7820();
        r4 = lbl_8047B5C4;
        r3 = (u32)lbl_8046D630;
        r30 = lbl_8047B5C8;
        r31 = (u32)lbl_8046D630;
        r27 = *(u16*)((u8*)r4 + 0x6);
        while ((s32)r27 != 0) {

            r3 = *(u16*)((u8*)r30 + 0x2);
            tmp = *(u8*)((u8*)r30 + 0x0);
            r3 = r3 << 2;
            r29 = *(u32*)(r31 + r3);
            if (tmp == 1) {
                r3 = r29;
                fn_800EF4FC();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x48) = tmp;
                f3 = lbl_8047E500;
                r3 = r29;
                f1 = *(f32*)((u8*)r30 + 0x8);
                f0 = *(f32*)(sp + 0x2C);
                f2 = f2 - f3;
                f30 = -(f1 * f2 - f0);
                fn_800EF4F4();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x40) = tmp;
                f3 = lbl_8047E500;
                r3 = r29;
                f1 = *(f32*)((u8*)r30 + 0x8);
                f0 = *(f32*)(sp + 0x30);
                f2 = f2 - f3;
                f29 = -(f1 * f2 - f0);
                fn_800EF4FC();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x38) = tmp;
                f3 = lbl_8047E500;
                r3 = r29;
                f1 = *(f32*)((u8*)r30 + 0x8);
                f0 = *(f32*)(sp + 0x2C);
                f2 = f2 - f3;
                f28 = f1 * f2 + f0;
                fn_800EF4F4();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x50) = tmp;
                f3 = lbl_8047E500;
                f1 = *(f32*)((u8*)r30 + 0x8);
                f0 = *(f32*)(sp + 0x30);
                f2 = f2 - f3;
                f27 = f1 * f2 + f0;
            } else {

                f1 = lbl_8047E4F0;
                r3 = (u32)sp + 0x14;
                f0 = *(f32*)((u8*)r30 + 0x4);
                r4 = (u32)sp + 0x20;
                f0 = f1 * f0;
                f1 = f0 * f31;
                fn_800E013C();
                r3 = r29;
                fn_800EF4FC();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x50) = tmp;
                f3 = lbl_8047E500;
                r3 = r29;
                f1 = *(f32*)(sp + 0x14);
                f0 = lbl_8047E4D0;
                f3 = f2 - f3;
                f2 = *(f32*)((u8*)r30 + 0x8);
                f0 = f1 + f0;
                f30 = -(f2 * f3 - f0);
                fn_800EF4F4();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x48) = tmp;
                f3 = lbl_8047E500;
                r3 = r29;
                f1 = *(f32*)(sp + 0x18);
                f0 = lbl_8047E4D4;
                f3 = f2 - f3;
                f2 = *(f32*)((u8*)r30 + 0x8);
                f0 = f1 + f0;
                f29 = -(f2 * f3 - f0);
                fn_800EF4FC();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x40) = tmp;
                f3 = lbl_8047E500;
                r3 = r29;
                f1 = *(f32*)(sp + 0x14);
                f0 = lbl_8047E4D0;
                f3 = f2 - f3;
                f2 = *(f32*)((u8*)r30 + 0x8);
                f0 = f1 + f0;
                f28 = f2 * f3 + f0;
                fn_800EF4F4();
                /* extrwi r3, r3, 15, 16 */;
                tmp = 0x43300000;
                *(u32*)(sp + 0x38) = tmp;
                f3 = lbl_8047E500;
                f1 = *(f32*)(sp + 0x18);
                f0 = lbl_8047E4D4;
                f3 = f2 - f3;
                f2 = *(f32*)((u8*)r30 + 0x8);
                f0 = f1 + f0;
                f27 = f2 * f3 + f0;
            }
            r4 = r29;
            r3 = 0x0;
            fn_800D85D4();
            r3 = 0x4;
            fn_800D67BC();
            f1 = f30;
            f3 = lbl_8047E4D8;
            f2 = f29;
            fn_800D6680();
            r4 = r28 & 0xFF;
            r3 = 0x0;
            r5 = r4;
            r6 = r4;
            fn_800D5C18();
            f1 = lbl_8047E4D8;
            r3 = 0x0;
            f2 = f1;
            fn_800D59B8();
            f1 = f28;
            f3 = lbl_8047E4D8;
            f2 = f29;
            fn_800D6680();
            r4 = r28 & 0xFF;
            r3 = 0x0;
            r5 = r4;
            r6 = r4;
            fn_800D5C18();
            f1 = lbl_8047E4E8;
            r3 = 0x0;
            f2 = lbl_8047E4D8;
            fn_800D59B8();
            f1 = f30;
            f3 = lbl_8047E4D8;
            f2 = f27;
            fn_800D6680();
            r4 = r28 & 0xFF;
            r3 = 0x0;
            r5 = r4;
            r6 = r4;
            fn_800D5C18();
            f1 = lbl_8047E4D8;
            r3 = 0x0;
            f2 = lbl_8047E4E8;
            fn_800D59B8();
            f1 = f28;
            f3 = lbl_8047E4D8;
            f2 = f27;
            fn_800D6680();
            r4 = r28 & 0xFF;
            r3 = 0x0;
            r5 = r4;
            r6 = r4;
            fn_800D5C18();
            f1 = lbl_8047E4E8;
            r3 = 0x0;
            f2 = f1;
            fn_800D59B8();
            fn_800D6728();
            r30 = r30 + 0xc;

        }
        r3 = lbl_8047B5C4;
        tmp = 0x43300000;
        *(u32*)(sp + 0x50) = tmp;
        r5 = *(u32*)((u8*)r3 + 0xC);
        f5 = lbl_8047E4F8;
        f0 = f0 - f5;
        if (f31 >= f0) return;
        r4 = *(u32*)((u8*)r3 + 0x10);
        *(u32*)(sp + 0x50) = tmp;
        f0 = f0 - f5;
        if (f31 < f0) {
            f1 = *(f32*)((u8*)r3 + 0x14);
            break;
        }
        r4 = r5 - r4;
        f4 = *(f32*)((u8*)r3 + 0x14);
        *(u32*)(sp + 0x48) = tmp;
        f0 = lbl_8047E4D8;
        f1 = f1 - f5;
        *(u32*)(sp + 0x50) = tmp;
        f2 = f31 - f1;
        f3 = f3 - f5;
        *(u32*)(sp + 0x40) = tmp;
        f2 = f3 - f2;
        f1 = f1 - f5;
        f1 = f2 / f1;
        f1 = f4 * f1;
        if (f1 < f0) {
            f1 = f0;
            break;
        }
        f0 = lbl_8047E4E8;
        if (f1 <= f0) break;
        f1 = f0;
    } while (0);
        f0 = lbl_8047E4EC;
        r3 = 0x2;
        f0 = f0 * f1;
        f0 = (f64)(s32)f0;
        fn_800D888C();
        r3 = 0x4;
        fn_800D6A00();
        r3 = (u32)lbl_80314958;
        r3 = (u32)lbl_80314958;
        fn_800D7820();
        r3 = 0x4;
        fn_800D67BC();
        f1 = lbl_8047E4D8;
        f2 = f1;
        f3 = f1;
        fn_800D6680();
        r4 = r27 & 0xFF;
        r3 = 0x0;
        r5 = r4;
        r6 = r4;
        fn_800D5C18();
        f2 = lbl_8047E4D8;
        f1 = lbl_8047E4E0;
        f3 = f2;
        fn_800D6680();
        r4 = r27 & 0xFF;
        r3 = 0x0;
        r5 = r4;
        r6 = r4;
        fn_800D5C18();
        f1 = lbl_8047E4D8;
        f2 = lbl_8047E4E4;
        f3 = f1;
        fn_800D6680();
        r4 = r27 & 0xFF;
        r3 = 0x0;
        r5 = r4;
        r6 = r4;
        fn_800D5C18();
        f1 = lbl_8047E4E0;
        f2 = lbl_8047E4E4;
        f3 = lbl_8047E4D8;
        fn_800D6680();
        r4 = r27 & 0xFF;
        r3 = 0x0;
        r5 = r4;
        r6 = r4;
        fn_800D5C18();
        fn_800D6728();
        return;
    }
    r3 = 0x2;
    r4 = 0x1;
    r5 = 0x1;
    fn_800DA4C4();
    r3 = lbl_8047B5C4;
    tmp = 0x43300000;
    *(u32*)(sp + 0x50) = tmp;
    r5 = *(u32*)((u8*)r3 + 0xC);
    f5 = lbl_8047E4F8;
    f0 = f0 - f5;
    if (f31 >= f0) return;
    r4 = *(u32*)((u8*)r3 + 0x10);
    *(u32*)(sp + 0x50) = tmp;
    f0 = f0 - f5;
    if (f31 < f0) {
        f1 = *(f32*)((u8*)r3 + 0x18);
        goto L_801EDF2C;
    }
    r4 = r5 - r4;
    f4 = *(f32*)((u8*)r3 + 0x18);
    *(u32*)(sp + 0x48) = tmp;
    f0 = lbl_8047E4D8;
    f1 = f1 - f5;
    *(u32*)(sp + 0x50) = tmp;
    f2 = f31 - f1;
    f3 = f3 - f5;
    *(u32*)(sp + 0x40) = tmp;
    f2 = f3 - f2;
    f1 = f1 - f5;
    f1 = f2 / f1;
    f1 = f4 * f1;
    if (f1 < f0) {
        f1 = f0;
        goto L_801EDF2C;
    }
    f0 = lbl_8047E4E8;
    if (f1 > f0) {
        f1 = f0;
    }
L_801EDF2C:
    f0 = lbl_8047E4EC;
    r3 = 0x2;
    f0 = f0 * f1;
    f0 = (f64)(s32)f0;
    fn_800D888C();
    r3 = 0x4;
    fn_800D6A00();
    r3 = (u32)lbl_80314958;
    r3 = (u32)lbl_80314958;
    fn_800D7820();
    r3 = 0x4;
    fn_800D67BC();
    f1 = lbl_8047E4D8;
    f2 = f1;
    f3 = f1;
    fn_800D6680();
    r4 = r27 & 0xFF;
    r3 = 0x0;
    r5 = r4;
    r6 = r4;
    fn_800D5C18();
    f2 = lbl_8047E4D8;
    f1 = lbl_8047E4E0;
    f3 = f2;
    fn_800D6680();
    r4 = r27 & 0xFF;
    r3 = 0x0;
    r5 = r4;
    r6 = r4;
    fn_800D5C18();
    f1 = lbl_8047E4D8;
    f2 = lbl_8047E4E4;
    f3 = f1;
    fn_800D6680();
    r4 = r27 & 0xFF;
    r3 = 0x0;
    r5 = r4;
    r6 = r4;
    fn_800D5C18();
    f1 = lbl_8047E4E0;
    f2 = lbl_8047E4E4;
    f3 = lbl_8047E4D8;
    fn_800D6680();
    r4 = r27 & 0xFF;
    r3 = 0x0;
    r5 = r4;
    r6 = r4;
    fn_800D5C18();
    fn_800D6728();

    return;
}
#endif

/* 0x801EE07C | size: 0x2C | small */
#if 0
asm void fn_801EE07C(void) {
#include "src/game/battle/battle_logic_fn_801EE07C.inc"
}
#else
u8 *fn_801EE07C(u32 arg) {
    extern u32 *lbl_80478F78;
    extern u8 *lbl_80478F7C;
    u32 i = arg & 0xFFFF;

    if (i >= *lbl_80478F78) {
        return lbl_80478F7C;
    }
    return lbl_80478F7C + i * 8;
}
#endif

/* 0x801EE0A8 | size: 0x14 | tiny */
#if 0
asm void fn_801EE0A8(void) {
#include "src/game/battle/battle_logic_fn_801EE0A8.inc"
}
#else
#pragma peephole off
u16 fn_801EE0A8(u32 idx) {
    extern u16 *lbl_80478F74;
    u8 i;

    i = idx;
    return lbl_80478F74[i];
}
#pragma peephole on
#endif

/* 0x801EE0BC | size: 0x50 | small */
#if 0
asm void fn_801EE0BC(void) {
#include "src/game/battle/battle_logic_fn_801EE0BC.inc"
}
#else
s32 fn_801EE0BC(u16 arg) {
    extern u8 *fn_801EF1E4(u32);
    u8 *p;

    p = fn_801EF1E4(0);
    if (p != 0) {
        return *(s16*)(p + (u32)arg * 0x18 + 0x4a4);
    }
    return -1;
}
#endif

/* 0x801EE10C | size: 0x68 | small */
#if 0
asm void fn_801EE10C(void) {
#include "src/game/battle/battle_logic_fn_801EE10C.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EE10C(u16 arg, u8 val) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        *(u8*)(p + v * 0x18 + 0x49c) = val;
    }
}
#pragma pop
#endif

/* 0x801EE174 | size: 0x6C | small */
#if 0
asm void fn_801EE174(void) {
#include "src/game/battle/battle_logic_fn_801EE174.inc"
}
#else
u8 fn_801EE174(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        return *(u8*)(p + v * 0x18 + 0x49c);
    }
    return 0;
}
#endif

/* 0x801EE1E0 | size: 0x68 | small */
#if 0
asm void fn_801EE1E0(void) {
#include "src/game/battle/battle_logic_fn_801EE1E0.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EE1E0(u16 arg, u16 val) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        *(u16*)(p + v * 0x18 + 0x4a0) = val;
    }
}
#pragma pop
#endif

/* 0x801EE248 | size: 0x6C | small */
#if 0
asm void fn_801EE248(void) {
#include "src/game/battle/battle_logic_fn_801EE248.inc"
}
#else
u16 fn_801EE248(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        return *(u16*)(p + v * 0x18 + 0x4a0);
    }
    return 0;
}
#endif

/* 0x801EE2B4 | size: 0x74 | small */
#if 0
asm void fn_801EE2B4(void) {
#include "src/game/battle/battle_logic_fn_801EE2B4.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EE2B4(u16 arg, u32 val) {
    extern u8 *lbl_80478F6C;
    extern void fn_800F9E70(void *, u32);
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        fn_800F9E70(p + v * 0x18 + 0x490, val);
    }
}
#pragma pop
#endif

/* 0x801EE328 | size: 0x70 | small */
#if 0
asm void fn_801EE328(void) {
#include "src/game/battle/battle_logic_fn_801EE328.inc"
}
#else
u8 *fn_801EE328(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        return p + v * 0x18 + 0x490;
    }
    return 0;
}
#endif

/* 0x801EE398 | size: 0xA8 | medium */
#if 0
asm void fn_801EE398(void) {
#include "src/game/battle/battle_logic_fn_801EE398.inc"
}
#else
#pragma push
#pragma optimize_for_size on
s32 fn_801EE398(void) {
    extern u16 lbl_80375240[];
    extern u8 *lbl_80478F6C;
    extern s32 fn_801902E0(u16);
    u16 i;
    for (i = 0; i < 0x30; i++) {
        u16 idx = lbl_80375240[i];
        u8 *entry = lbl_80478F6C + idx * 0x38;
        s32 r;
        if (idx == 0 || idx > 0x60) {
            entry = 0;
        }
        if (entry != 0) {
            u16 v = *(u16*)(entry + 0x14);
            if (v != 0) {
                r = fn_801902E0(v);
            } else {
                r = 0;
            }
        } else {
            r = 0;
        }
        if ((r & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}
#pragma pop
#endif

/* 0x801EE440 | size: 0x28
 * Look up a value in the type effectiveness table.
 * Returns 0 if index > 0x30, otherwise returns the u16 at that offset.
 */
u32 fn_801EE440(u32 index) {
    extern u8 lbl_80375240[];

    if ((index & 0xFFFF) > 0x30) {
        return 0;
    }
    return ((u16*)lbl_80375240)[(u16)index];
}

/* 0x801EE470 | size: 0x6C | small */
#if 0
asm void fn_801EE470(void) {
#include "src/game/battle/battle_logic_fn_801EE470.inc"
}
#else
u16 fn_801EE470(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        return *(u16*)(p + v * 0xc + 0x6);
    }
    return 0;
}
#endif

/* 0x801EE4DC | size: 0x68 | small */
#if 0
asm void fn_801EE4DC(void) {
#include "src/game/battle/battle_logic_fn_801EE4DC.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EE4DC(u16 arg, u16 val) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        *(u16*)(p + v * 0xc + 0x6) = val;
    }
}
#pragma pop
#endif

/* 0x801EE544 | size: 0xD0 | medium */
#if 0
asm void fn_801EE544(void) {
#include "src/game/battle/battle_logic_fn_801EE544.inc"
}
#else
void fn_801EE544(void) {
    extern u32 lbl_80478F64;
    extern u32 lbl_80478F6C;
    extern void fn_801906A0();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r3 = r3 & 0xFFFF;
    tmp = r3 * 0x38;
    r29 = r4;
    r31 = 0x0;
    r5 = lbl_80478F6C;
    r4 = r5 + tmp;
    if ((s32)tmp == 0 || r3 > 0x60) {
        r4 = 0x0;
    }
    if (r4 == 0) { r3 = 0x0; return; }
    tmp = *(u16*)((u8*)r4 + 0x1E);
    r3 = lbl_80478F64;
    tmp = tmp << 3;
    r30 = r3 + tmp;
    while (1) {
        tmp = *(u32*)((u8*)r30 + 0x4);
        if (tmp == 0) { r3 = r31; return; }
        r3 = *(u16*)((u8*)r30 + 0x2);
        if (r3 == 0) {
            r31 = tmp;
            if (r29 != 0) {
                tmp = *(u8*)((u8*)r30 + 0x1);
                *(u8*)((u8*)r29 + 0x0) = tmp;
            }
        } else {
            fn_801906A0();
            tmp = *(u8*)((u8*)r30 + 0x0);
            r3 = r3 & 0xFF;
            if (r3 == tmp) {
                r31 = *(u32*)((u8*)r30 + 0x4);
                if (r29 != 0) {
                    tmp = *(u8*)((u8*)r30 + 0x1);
                    *(u8*)((u8*)r29 + 0x0) = tmp;
                }
            }
        }
        r30 = r30 + 0x8;
    }
}
#endif

/* 0x801EE614 | size: 0x68 | small */
#if 0
asm void fn_801EE614(void) {
#include "src/game/battle/battle_logic_fn_801EE614.inc"
}
#else
s32 fn_801EE614(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern s32 fn_801906A0(u16);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0x1c);
        if (v != 0) {
            return (s8)fn_801906A0(v);
        }
        return 0;
    }
    return 0;
}
#endif

/* 0x801EE67C | size: 0xD4 | medium */
#if 0
asm void fn_801EE67C(void) {
#include "src/game/battle/battle_logic_fn_801EE67C.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EE67C(void) {
    extern u32 lbl_80478F6C;
    extern void fn_8019075C();
    extern void fn_801EF1E4();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r30 = r3 & 0xFFFF;
    r27 = r3;
    r28 = r4;
    r29 = r30 * 0x38;
    tmp = lbl_80478F6C;
    r31 = tmp + r29;
    if ((s32)tmp == 0 || r30 > 0x60) {
        r31 = 0x0;
    }
    if (r31 == 0) return;
    r3 = 0x0;
    fn_801EF1E4();
    if (r3 != 0) {
        r4 = r30 * 0x18;
        r5 = r4 + 0x4a4;
        tmp = *(s16*)(r3 + r5);
        if ((s32)tmp < 0) {
            r4 = *(u32*)((u8*)r3 + 0x0);
            tmp = r4 + 0x1;
            *(u32*)((u8*)r3 + 0x0) = tmp;
            *(u16*)(r3 + r5) = r4;
    }
    }
    r3 = lbl_80478F6C;
    tmp = r27 & 0xFFFF;
    r3 = r3 + r29;
    if ((s32)tmp == 0 || tmp > 0x60) {
        r3 = 0x0;
    }
    if (r3 != 0) {
        r3 = *(u16*)((u8*)r3 + 0xC);
        if (r3 != 0) {
            r4 = 0x1;
            fn_8019075C();
    }
    }
    r3 = *(u16*)((u8*)r31 + 0x1C);
    if (r3 == 0) return;
    r4 = r28 & 0xFFFF;
    fn_8019075C();

    return;
}
#pragma pop
#endif

/* 0x801EE750 | size: 0x6C | small */
#if 0
asm void fn_801EE750(void) {
#include "src/game/battle/battle_logic_fn_801EE750.inc"
}
#else
u32 fn_801EE750(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        return *(u32*)(p + v * 0xc + 0x8);
    }
    return 0;
}
#endif

/* 0x801EE7BC | size: 0x68 | small */
#if 0
asm void fn_801EE7BC(void) {
#include "src/game/battle/battle_logic_fn_801EE7BC.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EE7BC(u16 arg, u32 val) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        *(u32*)(p + v * 0xc + 0x8) = val;
    }
}
#pragma pop
#endif

/* 0x801EE824 | size: 0x70 | small */
#if 0
asm void fn_801EE824(void) {
#include "src/game/battle/battle_logic_fn_801EE824.inc"
}
#else
void fn_801EE824(void) {
    extern u32 lbl_80478F6C;
    extern void fn_801906A0();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;

    r3 = r3 & 0xFFFF;
    tmp = r3 * 0x38;
    r5 = lbl_80478F6C;
    r5 = r5 + tmp;
    if ((s32)tmp == 0 || r3 > 0x60) {
        r5 = 0x0;
    }
    if (r5 != 0) {
        r3 = r5 + tmp;
        r3 = *(u16*)((u8*)r3 + 0x24);
        if (r3 != 0) {
            fn_801906A0();
            r3 = (s8)r3;
            return;
        }
        r3 = 0x0;
        return;
    }
    r3 = 0x0;

    return;
}
#endif

/* 0x801EE894 | size: 0x60 | small */
#if 0
asm void fn_801EE894(void) {
#include "src/game/battle/battle_logic_fn_801EE894.inc"
}
#else
void fn_801EE894(void) {
    extern u32 lbl_80478F6C;
    extern void fn_8019075C();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;

    r3 = r3 & 0xFFFF;
    tmp = r3 * 0x38;
    r6 = lbl_80478F6C;
    r6 = r6 + tmp;
    if ((s32)tmp == 0 || r3 > 0x60) {
        r6 = 0x0;
    }
    if (r6 != 0) {
        r3 = r6 + tmp;
        r3 = *(u16*)((u8*)r3 + 0x24);
        if (r3 != 0) {
            r4 = (s8)r5;
            fn_8019075C();
    }
    }
    return;
}
#endif

/* 0x801EE8F4 | size: 0x64 | small */
#if 0
asm void fn_801EE8F4(void) {
#include "src/game/battle/battle_logic_fn_801EE8F4.inc"
}
#else
s32 fn_801EE8F4(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern s32 fn_801902E0(u16);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0x16);
        if (v != 0) {
            return fn_801902E0(v);
        }
        return 0;
    }
    return 0;
}
#endif

/* 0x801EEAD0 | size: 0x64 | small */
#if 0
asm void fn_801EEAD0(void) {
#include "src/game/battle/battle_logic_fn_801EEAD0.inc"
}
#else
s32 fn_801EEAD0(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern s32 fn_801902E0(u16);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0x12);
        if (v != 0) {
            return fn_801902E0(v);
        }
        return 0;
    }
    return 0;
}
#endif

/* 0x801EED30 | size: 0x58 | small */
#if 0
asm void fn_801EED30(void) {
#include "src/game/battle/battle_logic_fn_801EED30.inc"
}
#else
void fn_801EED30(u16 arg, u8 val) {
    extern u8 *lbl_80478F6C;
    extern void fn_8019075C(u16, s32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xe);
        if (v != 0) {
            fn_8019075C(v, val);
        }
    }
}
#endif

/* 0x801EED88 | size: 0x64 | small */
#if 0
asm void fn_801EED88(void) {
#include "src/game/battle/battle_logic_fn_801EED88.inc"
}
#else
s32 fn_801EED88(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern s32 fn_801902E0(u16);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xc);
        if (v != 0) {
            return fn_801902E0(v);
        }
        return 0;
    }
    return 0;
}
#endif

/* 0x801EEDEC | size: 0x58 | small */
#if 0
asm void fn_801EEDEC(void) {
#include "src/game/battle/battle_logic_fn_801EEDEC.inc"
}
#else
void fn_801EEDEC(u16 arg, u8 val) {
    extern u8 *lbl_80478F6C;
    extern void fn_8019075C(u16, s32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xc);
        if (v != 0) {
            fn_8019075C(v, val);
        }
    }
}
#endif

/* 0x801EEE44 | size: 0x28 | small */
#if 0
asm void fn_801EEE44(void) {
#include "src/game/battle/battle_logic_fn_801EEE44.inc"
}
#else
u8 fn_801EEE44(u16 arg) {
    extern u8 *lbl_80478F6C;
    u8 *entry = lbl_80478F6C + arg * 0x38;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    return *entry;
}
#endif

/* 0x801EEE6C | size: 0x4C | small */
#if 0
asm void fn_801EEE6C(void) {
#include "src/game/battle/battle_logic_fn_801EEE6C.inc"
}
#else
#pragma push
#pragma optimize_for_size on
void fn_801EEE6C(u32 arg, u32 val) {
    extern u8 *fn_801EF1E4(u32);
    u8 *base;

    base = fn_801EF1E4(0);
    if (base != 0) {
        *(u16*)(base + ((arg & 0xFFFF) * 0x18) + 0x4A6) = val;
    }
}
#pragma pop
#endif

/* 0x801EEEB8 | size: 0x50 | small */
#if 0
asm void fn_801EEEB8(void) {
#include "src/game/battle/battle_logic_fn_801EEEB8.inc"
}
#else
u16 fn_801EEEB8(u32 arg) {
    extern u8 *fn_801EF1E4(u32);
    u8 *base;

    base = fn_801EF1E4(0);
    if (base != 0) {
        return *(u16*)(base + ((arg & 0xFFFF) * 0x18) + 0x4A6);
    }
    return 0;
}
#endif

/* 0x801EEF08 | size: 0x38 | small */
#if 0
asm void fn_801EEF08(void) {
#include "src/game/battle/battle_logic_fn_801EEF08.inc"
}
#else
u16 fn_801EEF08(u16 arg) {
    extern u8 *lbl_80478F6C;
    u8 *entry = lbl_80478F6C + arg * 0x38;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        return *(u16*)(entry + 0x8);
    }
    return 0;
}
#endif

/* 0x801EEF40 | size: 0x6C | small */
#if 0
asm void fn_801EEF40(void) {
#include "src/game/battle/battle_logic_fn_801EEF40.inc"
}
#else
u32 fn_801EEF40(u16 arg) {
    extern u8 *lbl_80478F6C;
    extern u8 *fn_801EF1E4(u32);
    u8 *entry = lbl_80478F6C + arg * 0x38;
    u16 v;
    u8 *p;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        v = *(u16*)(entry + 0xa);
        p = fn_801EF1E4(0);
        return *(u32*)(p + v * 0xc + 0xc);
    }
    return 0;
}
#endif

/* 0x801EEFAC | size: 0x48 | small */
#if 0
asm void fn_801EEFAC(void) {
#include "src/game/battle/battle_logic_fn_801EEFAC.inc"
}
#else
u16 fn_801EEFAC(u16 arg, s32 sel) {
    extern u8 *lbl_80478F6C;
    u8 *entry = lbl_80478F6C + arg * 0x38;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        if (sel == 0) {
            return *(u16*)(entry + 0x4);
        }
        return *(u16*)(entry + 0x6);
    }
    return 0;
}
#endif

/* 0x801EEFF4 | size: 0x38 | small */
#if 0
asm void fn_801EEFF4(void) {
#include "src/game/battle/battle_logic_fn_801EEFF4.inc"
}
#else
u16 fn_801EEFF4(u16 arg) {
    extern u8 *lbl_80478F6C;
    u8 *entry = lbl_80478F6C + arg * 0x38;
    if (arg == 0 || arg > 0x60) {
        entry = 0;
    }
    if (entry != 0) {
        return *(u16*)(entry + 0x2);
    }
    return 0;
}
#endif
