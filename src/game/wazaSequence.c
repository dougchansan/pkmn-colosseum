/**
 * @file wazaSequence.c
 * @brief wazaSequence: waza sequence core -- start/stop/load/update and entry
 * linking.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"

#if !defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_B988_BB10)

/**
 * wazaSequenceUpdate - Waza rendering setup.
 * Address: 0x801DB988 | Size: 0x188
 */
s8 wazaSequenceUpdate(void* sequence) {
    /* TODO: Waza rendering setup (0x188 bytes) */
    return 0;
}

#endif

#if !defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_BB10_BDDC)

/**
 * wazaSequenceApplyStop - Waza rendering update.
 * Address: 0x801DBB10 | Size: 0x120
 */
void wazaSequenceApplyStop(void* obj) {
    WazaSequence* effect;
    WazaSequenceOwner* owner;
    WazaSequenceNode* node;

    effect = obj;
    if (effect != NULL) {
        owner = effect->owner;
        if (effect->active != 0) {
            if (effect->cameraActive != 0) {
                fn_801D3034(owner);
            }
            if (owner->animationActive == 0) {
                fn_801DEF0C(owner, 1, 0);
            }
            fn_800E3CC8(owner->model, 0);
            if ((effect->flags & 0x08000000) != 0) {
                GSmodelLinkToGSparticleBank(owner->model, owner->particleBank);
            }
            if ((effect->flags & 0x04000000) != 0) {
                battleGridResetModelVisibilityFlags();
            }
            node = effect->firstNode;
            while (node != NULL) {
                wazaSequenceEntryStop(node, 1);
                node = node->next;
            }
            node = effect->firstNode;
            while (node != NULL) {
                if (node->kind == 3 && node->state == 0 && node->resource != NULL) {
                    fn_80118874(node->resource, 1);
                }
                node = node->next;
            }
            owner->currentSequence = NULL;
            effect->active = 0;
            effect->stopping = 0;
        }
    }
}

/**
 * fn_801DBC30 - Waza rendering cleanup.
 * Address: 0x801DBC30 | Size: 0x9C
 */
void fn_801DBC30(void* obj) {
    WazaSequence* sequence;
    WazaSequenceOwner* owner;
    s32 kind;

    sequence = obj;
    if (sequence != NULL) {
        owner = sequence->owner;
        if (sequence->active != 0 && owner->currentSequence == sequence) {
            kind = sequence->kind;
            if (kind >= 0xB || kind < 9) {
                fn_801DEF0C(owner, 1, 0);
            }
            if (sequence->cameraActive != 0) {
                fn_801D3034(owner);
            }
            fn_800E3CC8(owner->model, 0);
            owner->currentSequence = NULL;
        }
    }
}

/**
 * wazaSequenceStart - Waza blend effect setup.
 * Address: 0x801DBCCC | Size: 0x110
 */
void wazaSequenceStart(void* sequence) {
    WazaSequence* obj;
    WazaSequenceOwner* owner;
    WazaSequenceNode* node;
    WazaSequence* current;
    s32 bit;
    struct GSmodel* model;
    u32 flags;

    obj = sequence;
    if (obj->active == 0) {
        owner = obj->owner;
        flags = obj->flags;
        current = owner->currentSequence;
        flags = (flags >> 1) & 1;
        node = obj->firstNode;
        model = owner->model;
        bit = flags;
        if (current != NULL) {
            wazaSequenceApplyStop(current);
        }
        if (owner->animationActive != 0 && obj->animationMode == 2) {
            wazaSequenceSysResetAnimationExcept(owner);
        }
        fn_801DD100(owner, obj);
        if ((obj->flags & 0x08000000) != 0) {
            GSmodelLinkToGSparticleBank(model, NULL);
        }
        wazaSequencePokemonMotionStart(owner, bit);
        owner->currentSequence = obj;
        obj->active = 1;
        if ((obj->flags & 0x04000000) != 0) {
            battleGridHideModelsExcept(owner);
        }
        if (obj->cameraActive != 0) {
            battleCameraStartWaza(owner, obj);
        }
        while (node != NULL) {
            node->runtimeState = 0;
            node = node->next;
        }
        obj->state = 0;
        obj->stopping = 0;
        wazaSequenceUpdate(obj);
    }
}

#endif

#if !defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_BDDC_BFB0)

/**
 * wazaSequenceFree - Waza blend effect update.
 * Address: 0x801DBDDC | Size: 0x1D4
 */
void wazaSequenceFree(void* obj) {
    /* TODO: Blend effect update (0x1D4 bytes) */
}

#endif

#if !defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_BFB0_C014)

/**
 * fn_801DBFB0 - Waza blend effect get state.
 * Address: 0x801DBFB0 | Size: 0x64
 */
WazaSequence* fn_801DBFB0(void) {
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* fn_800E27B0(u16 handle);

    u16 handle;
    WazaSequence* obj;

    handle = _toolentryAlloc__FUl(0x40);
    if (handle != 0) {
        obj = fn_800E27B0(handle);
        memset(obj, 0, 0x40);
        obj->handle = handle;
        return obj;
    }
    return NULL;
}

#endif

#if !defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8)

/**
 * wazaSequenceLoadData - Waza screen distortion effect.
 * Address: 0x801DC014 | Size: 0x2FC
 */
BOOL wazaSequenceLoadData(void* sequence, void* resource) {
    /* TODO: Screen distortion effect (0x2FC bytes) */
    return FALSE;
}

/**
 * wazaSequenceEntryLink - Waza screen distortion update.
 * Address: 0x801DC310 | Size: 0x15C
 */
void wazaSequenceEntryLink(void) {
    /* TODO: Screen distortion update (0x15C bytes) */
}

/**
 * fn_801DC46C - Waza screen overlay effect.
 * Address: 0x801DC46C | Size: 0x184
 */
void fn_801DC46C(s32 overlayType, u32 color) {
    /* TODO: Screen overlay effect (0x184 bytes) */
}

/**
 * fn_801DC5F0 - Waza screen overlay update.
 * Address: 0x801DC5F0 | Size: 0x22C
 */
void fn_801DC5F0(void) {
    /* TODO: Screen overlay update (0x22C bytes) */
}

/**
 * _wazaSequenceEffectEntryLoad - Waza screen effect composite.
 * Address: 0x801DC81C | Size: 0x284
 */
void _wazaSequenceEffectEntryLoad(void) {
    /* TODO: Screen effect composite (0x284 bytes) */
}

/**
 * _wazaSequenceParticleEntryLoad - Waza screen effect finalize.
 * Address: 0x801DCAA0 | Size: 0x128
 */
void _wazaSequenceParticleEntryLoad(void) {
    /* TODO: Screen effect finalize (0x128 bytes) */
}

/**
 * _wazaSequenceModelEntryLoad - Waza field effect handler.
 * Address: 0x801DCBC8 | Size: 0x1E0
 */
void _wazaSequenceModelEntryLoad(s32 fieldEffect) {
    /* TODO: Field effect handler (0x1E0 bytes)
     * Handles field-wide effects like weather, terrain changes.
     */
}

#endif
