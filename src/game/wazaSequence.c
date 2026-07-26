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
    WazaSequence* obj = sequence;
    WazaSequenceNode* node = obj->firstNode;
    WazaSequenceOwner* owner;
    s32 elapsed;
    s32 pending = 0;
    s32 running = 0;

    elapsed = fn_800D3088();
    obj->state += elapsed;
    owner = obj->owner;

    if (fn_801DA74C(owner, obj->handle, obj->animationMode, 2) >
        (s32)obj->state) {
        pending = 1;
    }

    if (owner->motionBusy == 0 && owner->sequenceEnabled != 0 &&
        fn_801DA74C(owner, obj->handle, obj->animationMode, 1) <
            (s32)obj->state) {
        return -1;
    }

    while (node != NULL) {
        switch (node->runtimeState) {
        case 0:
            if (node->startTime <= (s32)obj->state) {
                if (wazaSequenceEntryStart(node) != 0) {
                    pending++;
                }
            } else {
                node->currentTime = node->startTime;
                running++;
            }
            break;
        case 1:
            if (wazaSequenceEntryUpdate(node, elapsed) == 0) {
                wazaSequenceEntryStop(node, FALSE);
            } else {
                pending++;
            }
            break;
        }
        node = node->next;
    }

    if (pending + running == 0) {
        return 0;
    }
    return obj->stopping == 0;
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
    u8* sequence = obj;
    u8* previous;
    u8* next;
    u16 handle;

    if (sequence == NULL) {
        return;
    }

    previous = *(u8**)(sequence + 0x34);
    next = *(u8**)(sequence + 0x38);
    if (previous != NULL) {
        *(u8**)(previous + 0x38) = next;
    }
    if (next != NULL) {
        *(u8**)(next + 0x34) = previous;
    } else {
        *(u8**)(*(u8**)(sequence + 0x3C) + 0x68) = previous;
    }

    handle = *(u16*)(sequence + 0x2A);
    if (handle != 0) {
        extern void fn_800E24B0(u16 handle);
        extern void fn_800E209C(u16 handle);
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }
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
u8 wazaSequenceLoadData(void* sequence, void* resource) {
    /* TODO: Screen distortion effect (0x2FC bytes) */
    return FALSE;
}

/**
 * wazaSequenceEntryLink - Waza screen distortion update.
 * Address: 0x801DC310 | Size: 0x15C
 */
void wazaSequenceEntryLink(void* sequencePtr, void* entryPtr) {
    u8* sequence = sequencePtr;
    u8* entry = entryPtr;
    u8* current = *(u8**)(sequence + 0x24);
    u8* previous = current;
    s32 key = *(s32*)(entry + 8);

    if (key != 0) {
        while (current != NULL) {
            if (*(s32*)current == key) {
                break;
            }
            current = *(u8**)(current + 0xA8);
        }
    }

    if (current != NULL) {
        *(s32*)(entry + 0x70) = *(s32*)(current + 0x70);
        *(s32*)(entry + 0x70) +=
            *(s32*)(current + 0x2C + *(s32*)(entry + 0x10) * 4);
    } else {
        u8* owner = *(u8**)(sequence + 0x3C);
        u8* table = *(u8**)(owner + 0x2C);
        s32 offset = *(s32*)(sequence + 0x0C) * 0xD4 + 0x0C;
        offset += *(s32*)(entry + 0x10) * 4;
        *(s32*)(entry + 0x70) = *(s32*)(table + offset);
    }

    *(s32*)(entry + 0x70) -=
        *(s32*)(entry + 0x2C + *(s32*)(entry + 0x0C) * 4);
    *(s32*)(entry + 0x74) = *(s32*)(entry + 0x70);

    if (previous == NULL) {
        *(u8**)(sequence + 0x24) = entry;
        *(void**)(entry + 0xA8) = NULL;
        *(void**)(entry + 0xAC) = NULL;
    } else if (*(s32*)(previous + 0x70) > *(s32*)(entry + 0x70) ||
               (*(s32*)(entry + 4) == 6 &&
                *(s32*)(previous + 0x70) == *(s32*)(entry + 0x70))) {
        *(u8**)(entry + 0xA8) = previous;
        *(void**)(entry + 0xAC) = NULL;
        *(u8**)(previous + 0xAC) = entry;
        *(u8**)(sequence + 0x24) = entry;
    } else {
        u8* following = *(u8**)(previous + 0xA8);
        while (following != NULL &&
               *(s32*)(following + 0x70) <= *(s32*)(entry + 0x70)) {
            if (*(s32*)(entry + 4) == 6 &&
                *(s32*)(previous + 0x70) == *(s32*)(entry + 0x70)) {
                break;
            }
            previous = following;
            following = *(u8**)(following + 0xA8);
        }
        *(u8**)(entry + 0xA8) = following;
        if (following != NULL) {
            *(u8**)(following + 0xAC) = entry;
        }
        *(u8**)(previous + 0xA8) = entry;
        *(u8**)(entry + 0xAC) = previous;
    }
    *(u8**)(entry + 0xB0) = sequence;
}

/**
 * fn_801DC46C - Waza screen overlay effect.
 * Address: 0x801DC46C | Size: 0x184
 */
void* fn_801DC46C(void* entryPtr, void* dataPtr) {
    u8* entry = entryPtr;
    u8* data = dataPtr;
    s32 adjustment = 0;

    switch (*(s32*)(data + 0x68)) {
    case 1:
        *(s32*)(entry + 0x18) = 0;
        adjustment = -4;
        break;
    case 2:
        *(s32*)(entry + 0x18) = 1;
        adjustment = -8;
        break;
    }
    *(void**)(entry + 0xA8) = NULL;
    *(void**)(entry + 0xAC) = NULL;
    *(s32*)(entry + 0x6C) = 0;
    *(s32*)(entry + 0x70) = 0;
    *(s32*)(entry + 0x74) = 0;
    return data + adjustment + 0x70;
}

/**
 * fn_801DC5F0 - Waza screen overlay update.
 * Address: 0x801DC5F0 | Size: 0x22C
 */
void* fn_801DC5F0(void* sequencePtr, void* dataPtr) {
    u8* sequence = sequencePtr;
    u8* data = dataPtr;

    *(s32*)(sequence + 0x0C) = *(s32*)data;
    *(s32*)(sequence + 0x18) = 0;
    *(s32*)(sequence + 0x1C) = 0;
    *(s32*)(sequence + 0x20) = 0;
    return data;
}

/**
 * _wazaSequenceEffectEntryLoad - Waza screen effect composite.
 * Address: 0x801DC81C | Size: 0x284
 */
void* _wazaSequenceEffectEntryLoad(void* entryPtr, void* dataPtr) {
    u8* entry = entryPtr;
    *(void**)(entry + 0x78) = NULL;
    return dataPtr;
}

/**
 * _wazaSequenceParticleEntryLoad - Waza screen effect finalize.
 * Address: 0x801DCAA0 | Size: 0x128
 */
void* _wazaSequenceParticleEntryLoad(void* sequence, void* entryPtr,
                                     void* dataPtr) {
    u8* entry = entryPtr;
    u8* data = dataPtr;
    s32 adjustment = (*(s32*)(data + 0x0C) == 3) ? 4 : 0;

    *(s32*)(entry + 0x80) = *(s32*)data;
    *(s32*)(entry + 0x84) = *(s32*)(data + 4);
    *(void**)(entry + 0x88) = NULL;
    *(void**)(entry + 0x8C) = NULL;
    return data + adjustment + 0x10;
}

/**
 * _wazaSequenceModelEntryLoad - Waza field effect handler.
 * Address: 0x801DCBC8 | Size: 0x1E0
 */
void* _wazaSequenceModelEntryLoad(void* sequence, void* entryPtr,
                                  void* dataPtr) {
    u8* entry = entryPtr;
    u8* data = dataPtr;
    u32 size = (*(u32*)data + 0x1F) & ~0x1F;

    *(u32*)(entry + 0x78) = 0x4E20;
    *(void**)(entry + 0x7C) = NULL;
    *(void**)(entry + 0x80) = NULL;
    *(void**)(entry + 0xA4) = NULL;
    return data + size;
}

#endif
