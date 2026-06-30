/**
 * @file move_data.c
 * @brief Move data accessors via common_rel.
 *
 * Provides accessor functions for reading move data from the common_rel
 * data tables. Each move is 0x38 (56) bytes, stored at common_rel
 * offset 0x11E048.
 *
 * Move IDs follow Gen III indexing (1 = Pound, 2 = Karate Chop, etc.).
 * Shadow moves start at index 0x0164 (356). Move ID 0 is the "null move"
 * (Struggle is handled separately in the battle engine).
 *
 * Address context:
 *   Move data accessors are small functions in the battle logic region
 *   (0x801C53BC - 0x801D7230). The disassembly shows:
 *
 *     mulli r0, r3, 0x38        ; moveID * 56
 *     add   r3, rMoveTableBase, r0
 *     lbz   r3, <offset>(r3)   ; read field
 *     blr
 *
 *   The move table base is loaded from the resolved common_rel index:
 *     lwz   r4, sIndexPointers + COMMON_INDEX_MOVES*4(r13)
 *
 *   The battle damage calculation references multiple move fields:
 *     - basePower (0x17) for damage formula
 *     - type (0x02) for STAB and effectiveness
 *     - makesContact (0x06) for Rough Skin, etc.
 *     - priority (0x00) for turn order
 *     - accuracy (0x04) for hit check
 */

#include "game/data/common_rel.h"

/* ===================================================================
 * Core move data accessor
 *
 * CommonRel_GetMoveData is declared in common_rel.h and defined
 * in common_rel.c. The functions below provide field-level access.
 * =================================================================== */

/* ===================================================================
 * Primary battle-relevant accessors
 * =================================================================== */

/**
 * Get the base power of a move.
 * Power of 0 indicates a status move or variable-power move.
 */
u8 MoveData_GetBasePower(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->basePower;
}

/**
 * Get the type of a move.
 * Returns a type index (0-17, see battle.h TYPE_* constants).
 */
u8 MoveData_GetType(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->type;
}

/**
 * Get the accuracy of a move (0-100).
 * Accuracy of 0 means the move always hits (e.g., Swift).
 */
u8 MoveData_GetAccuracy(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->accuracy;
}

/**
 * Get the base PP (Power Points) of a move.
 */
u8 MoveData_GetBasePP(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->basePP;
}

/**
 * Get the priority of a move.
 * Normal priority is 0. Higher values move first.
 * Stored as unsigned; 255 means -1 priority.
 */
u8 MoveData_GetPriority(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->priority;
}

/**
 * Get the effect ID of a move.
 * This indexes into the Gen III effect table for secondary effects
 * (stat changes, status infliction, flinch, etc.).
 */
u8 MoveData_GetEffect(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->effect;
}

/**
 * Get the secondary effect accuracy (chance to trigger, 0-100).
 */
u8 MoveData_GetEffectAccuracy(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->effectAccuracy;
}

/* ===================================================================
 * Flag accessors
 * =================================================================== */

/**
 * Check if a move makes physical contact.
 * Contact moves trigger abilities like Rough Skin and Static.
 */
BOOL MoveData_MakesContact(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->makesContact != 0) ? TRUE : FALSE;
}

/**
 * Check if a move is blocked by Protect/Detect.
 */
BOOL MoveData_BlockedByProtect(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->blockedByProtect != 0) ? TRUE : FALSE;
}

/**
 * Check if a move can be reflected by Magic Coat.
 */
BOOL MoveData_CanReflect(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->magicCoatReflects != 0) ? TRUE : FALSE;
}

/**
 * Check if a move can be stolen by Snatch.
 */
BOOL MoveData_CanSnatch(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->snatchSteals != 0) ? TRUE : FALSE;
}

/**
 * Check if a move can be copied by Mirror Move.
 */
BOOL MoveData_CanMirrorMove(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->mirrorMoveCopies != 0) ? TRUE : FALSE;
}

/**
 * Check if a move can cause flinch with King's Rock.
 */
BOOL MoveData_KingsRockFlinch(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->kingsRockFlinch != 0) ? TRUE : FALSE;
}

/**
 * Check if a move is sound-based (blocked by Soundproof).
 */
BOOL MoveData_IsSoundBased(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->soundBased != 0) ? TRUE : FALSE;
}

/**
 * Check if a move is an HM move (or has the shadow flag in XD).
 * In Colosseum, this flag is 1 for HM moves.
 */
BOOL MoveData_IsHM(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return FALSE;
    }
    return (move->hmFlag != 0) ? TRUE : FALSE;
}

/* ===================================================================
 * Target and category helpers
 * =================================================================== */

/**
 * Get the target selection flags for a move.
 * Determines whether the move targets one opponent, both, self, etc.
 */
u8 MoveData_GetTargets(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->targets;
}

/**
 * Get the 16-bit effect ID for a move.
 * This is the extended effect identifier used in some battle calculations.
 */
u16 MoveData_GetEffectID(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->effectID;
}

/* ===================================================================
 * String / display accessors
 * =================================================================== */

u16 MoveData_GetNameStringID(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->nameStringID;
}

u16 MoveData_GetDescriptionID(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->descriptionID;
}

u16 MoveData_GetAnimationID(u16 moveID) {
    CommonMoveData* move;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 0;
    }
    return move->animationID;
}

/**
 * Determine the move category (physical/special/status) based on type.
 * In Gen III, the physical/special split is determined by type:
 *   Physical: Normal, Fighting, Flying, Poison, Ground, Rock, Bug, Ghost, Steel
 *   Special:  Fire, Water, Grass, Electric, Psychic, Ice, Dragon, Dark
 * Status moves have basePower == 0.
 *
 * @param moveID  Move index
 * @return        0 = physical, 1 = special, 2 = status
 */
u8 MoveData_GetCategory(u16 moveID) {
    CommonMoveData* move;
    u8 type;

    move = CommonRel_GetMoveData(moveID);
    if (move == NULL) {
        return 2; /* default to status */
    }

    /* Status moves have no base power */
    if (move->basePower == 0) {
        return 2;
    }

    /* Gen III physical/special split by type */
    type = move->type;
    switch (type) {
        case  0: /* Normal   */
        case  1: /* Fighting */
        case  2: /* Flying   */
        case  3: /* Poison   */
        case  4: /* Ground   */
        case  5: /* Rock     */
        case  6: /* Bug      */
        case  7: /* Ghost    */
        case  8: /* Steel    */
            return 0; /* physical */

        case  9: /* Fire     */
        case 10: /* Water    */
        case 11: /* Grass    */
        case 12: /* Electric */
        case 13: /* Psychic  */
        case 14: /* Ice      */
        case 15: /* Dragon   */
        case 16: /* Dark     */
            return 1; /* special */

        case 17: /* Shadow   */
            return 0; /* Shadow Rush is physical in Colosseum */

        default:
            return 0;
    }
}
