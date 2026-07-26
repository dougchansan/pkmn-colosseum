/**
 * @file wazaSequenceEntry.c
 * @brief wazaSequenceEntry: per-entry (particle/model/camera/sound) dispatchers
 * for a waza sequence.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"


/**
 * wazaSequenceEntryStop / wazaSequenceEntryStop - Stop a single waza entry.
 * Address: 0x801D7E58 | Size: 0x374
 * Proposed name from symbols: wazaSequenceEntryStop.
 */
u8 wazaSequenceEntryStop(void* entry, BOOL immediate) {
    WazaSequenceNode* node = entry;

    if (node->runtimeState == 2) {
        return TRUE;
    }
    if (!immediate && node->runtimeState != 1) {
        return FALSE;
    }

    /*
     * Kinds zero and six have no allocated object to tear down.  The other
     * cases continue with their resource-specific cleanup below in the
     * original routine.
     */
    if (node->kind == 0 || node->kind == 6) {
        node->runtimeState = 2;
        return TRUE;
    }
    return FALSE;
}

/**
 * wazaSequenceEntryUpdate / wazaSequenceEntryUpdate - Update a single waza entry.
 * Address: 0x801D81CC | Size: 0x328
 * Proposed name from symbols: wazaSequenceEntryUpdate.
 */
u8 wazaSequenceEntryUpdate(void* entry, s32 elapsed) {
    WazaSequenceNode* node = entry;

    node->currentTime += elapsed;
    if ((u32)node->kind > 6) {
        return FALSE;
    }

    /*
     * Entries without a live resource have already completed.  Model,
     * particle, sound, and camera entries perform their individual end tests
     * in the remainder of the target routine.
     */
    if (node->kind != 0 && node->resource == NULL) {
        return TRUE;
    }
    return FALSE;
}

/**
 * wazaSequenceEntryStart / wazaSequenceEntryStart - Start a single waza entry.
 * Address: 0x801D84F4 | Size: 0x2BC
 * Proposed name from symbols: wazaSequenceEntryStart.
 * Referenced by battle_logic.c.
 */
u8 wazaSequenceEntryStart(void* entry) {
    WazaSequenceNode* node = entry;
    WazaSequence* sequence = node->sequence;
    WazaSequenceOwner* owner = sequence->owner;
    u8 started = FALSE;

    if ((sequence->flags & 0x2000) == 0 &&
        (owner->flags & 1) == 0 &&
        node->kind != 5 && node->kind != 6 && node->kind != 1) {
        node->runtimeState = 2;
        node->currentTime = node->startTime;
        return TRUE;
    }

    switch (node->kind) {
    case 2:
        started = _wazaSequenceModelEntryStart(node);
        break;
    case 3:
        started = _wazaSequenceParticleEntryStart(node);
        break;
    case 4:
        started = _wazaSequenceEffectEntryStart(node);
        break;
    }

    if (started) {
        node->runtimeState = 1;
        node->currentTime = node->startTime;
        return TRUE;
    }
    node->runtimeState = 2;
    return FALSE;
}

/**
 * _wazaSequenceEffectEntryStart / wazaSequenceStartEntry - Initialize entry resources.
 * Address: 0x801D87B0 | Size: 0x388
 * Proposed name from symbols: wazaSequenceStartEntry.
 */
u8 _wazaSequenceEffectEntryStart(void* entry) {
    WazaSequenceNode* node = entry;

    if (node->resourceId == 0) {
        return FALSE;
    }

    /*
     * A nonzero resource id is resolved and configured by the remaining
     * effect-specific portion of the target routine.
     */
    return TRUE;
}

/**
 * _wazaSequenceParticleEntryStart / _wazaSequenceParticleEntryStart - Particle entry init.
 * Address: 0x801D8B38 | Size: 0x6B4
 * Proposed name from symbols: _wazaSequenceParticleEntryStart.
 * Large function that initializes a particle effect for a move animation.
 */
u8 _wazaSequenceParticleEntryStart(void* entry) {
    WazaSequenceNode* node = entry;

    /*
     * Particle entries cannot start until their resource was loaded.  The
     * remaining target code selects an attachment mode and places the emitter.
     */
    if (node->resource == NULL) {
        return FALSE;
    }
    return TRUE;
}

/**
 * _wazaSequenceModelEntryStart / _wazaSequenceModelEntryStart - Model entry init.
 * Address: 0x801D91EC | Size: 0x604
 * Proposed name from symbols: _wazaSequenceModelEntryStart.
 * Initializes a 3D model effect for a move animation.
 */
u8 _wazaSequenceModelEntryStart(void* entry) {
    WazaSequenceNode* node = entry;

    /* A model entry cannot start until its model resource was created. */
    if (node->model == NULL) {
        return FALSE;
    }
    return TRUE;
}

/**
 * fn_801D97F0 - Waza entry camera movement init.
 * Address: 0x801D97F0 | Size: 0x160
 */
void* fn_801D97F0(void* entry) {
    WazaSequenceNode* node = entry;
    WazaSequenceNode* linked;

    if ((node->flags & 1) != 0) {
        if (node->linkedEntryKey <= 0) {
            return NULL;
        }
        linked = fn_801DCDA8(node->sequence, node->linkedEntryKey);
        if (linked->kind == 2 && linked->model != NULL) {
            return GSmodelGetPart(linked->model, node->partIndex);
        }
        return NULL;
    }

    /*
     * Unlinked entries continue by resolving a part on the owning battler's
     * model in the remainder of the target routine.
     */
    return NULL;
}

/**
 * fn_801D9950 / wazaSequencePokemonMotionStart - Pokemon motion during move.
 * Address: 0x801D9950 | Size: 0x2CC
 * Proposed name from symbols: wazaSequencePokemonMotionStart.
 * Controls the Pokemon's physical movement during an attack animation
 * (e.g., lunging forward for Tackle, jumping for Bounce).
 */
void fn_801D9950(void* owner, f32* scale, s32 selector) {
    f32 value;

    switch (selector) {
    case -2:
        value = 0.5f;
        break;
    case -1:
        value = 0.75f;
        break;
    case 1:
        value = 1.33329999f;
        break;
    case 2:
        value = 2.0f;
        break;
    case 3:
        value = 3.25f;
        break;
    default:
        value = 1.0f;
        break;
    }
    set__5GSvecFfff(scale, value, value, value);
}

/**
 * wazaSequencePokemonMotionStart - Pokemon motion update.
 * Address: 0x801D9C1C | Size: 0x200
 */
u8 wazaSequencePokemonMotionStart(void* ownerPtr, BOOL enabled) {
    WazaSequenceOwner* owner = ownerPtr;

    if (owner->motionBusy != 0) {
        return FALSE;
    }
    if (owner->model == NULL) {
        return FALSE;
    }
    fn_800E3CC8(owner->model, enabled);
    if ((owner->flags & 8) != 0) {
        return TRUE;
    }
    return FALSE;
}
