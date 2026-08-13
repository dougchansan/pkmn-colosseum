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
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* fn_800E27B0(u16);
    extern void fn_800E24B0(u16);
    extern void fn_800E209C(u16);
    extern u32 fn_800D37CC(void);
    extern void GSlogWrite(const char*, ...);
    extern const char lbl_8027997C[];
    extern f32 lbl_8047E3A0;
    u8 parser[0xD8];
    u8* seq = sequence;
    u8* header;
    u8* data;
    u8* entry;
    u8* parsed;
    u8* current;
    s32 count;
    s32 i;
    s32 earliest;
    s32 value;
    u16 handle;
    u32 type;

    if (seq == NULL || *(void**)(seq + 0x3C) == NULL) {
        return FALSE;
    }

    header = fn_801DC46C(parser, resource);
    count = *(s32*)(header + 4) - 1;
    data = fn_801DC5F0(seq, header);
    if (count == 0) {
        return TRUE;
    }

    handle = _toolentryAlloc__FUl(count * 0xB4);
    if (handle == 0) {
        return TRUE;
    }
    *(void**)(seq + 0x24) = NULL;
    entry = fn_800E27B0(handle);
    *(s32*)(seq + 4) = count;
    *(s32*)(seq + 0) = 0;
    *(u16*)(seq + 0x28) = handle;

    for (i = 0; i < count; i++, entry += 0xB4) {
        parsed = fn_801DC46C(entry, data);
        type = *(u32*)(entry + 4);
        switch (type) {
        case 1:
            *(u32*)(entry + 0x78) = *(u32*)(parsed + 0);
            data = parsed + 0x0C;
            switch (*(u32*)(entry + 0x78)) {
            case 0:
                *(s32*)(entry + 0x7C) =
                    (s32)(((f32)*(s32*)(parsed + 4) *
                           (f32)(s32)fn_800D37CC()) /
                          lbl_8047E3A0);
                break;
            case 1:
                *(u32*)(entry + 0x7C) = *(u32*)(parsed + 4);
                break;
            case 2:
                *(f32*)(entry + 0x7C) = *(f32*)(parsed + 4);
                break;
            case 3:
                data += *(u32*)(parsed + 4) * 8;
                *(u32*)(entry + 0x78) = 0;
                *(u32*)(entry + 0x7C) = 0;
                break;
            }
            break;
        case 2:
            data = _wazaSequenceModelEntryLoad(seq, entry, parsed);
            break;
        case 3:
            data = _wazaSequenceParticleEntryLoad(seq, entry, parsed);
            break;
        case 4:
            data = _wazaSequenceEffectEntryLoad(entry, parsed);
            break;
        case 5:
            type = *(u32*)(parsed + 4);
            if (type == 1 || type == 2) {
                *(u32*)(entry + 0x7C) = 0;
                data = parsed + 8;
            } else {
                *(u32*)(entry + 0x7C) = *(u32*)(parsed + 8);
                data = parsed + 0x0C;
            }
            *(u32*)(entry + 0x78) = *(u32*)(parsed + 0);
            break;
        case 6:
            *(u32*)(entry + 0x78) = *(u32*)(parsed + 0);
            data = parsed + 8;
            break;
        default:
            GSlogWrite(lbl_8027997C);
            fn_800E24B0(handle);
            fn_800E209C(handle);
            return FALSE;
        }
        wazaSequenceEntryLink(seq, entry);
    }

    earliest = 0;
    current = *(u8**)(seq + 0x24);
    while (current != NULL) {
        value = *(s32*)(current + 0x70);
        if (value < earliest) {
            earliest = value;
        }
        current = *(u8**)(current + 0xA8);
    }
    if (earliest != 0) {
        value = -earliest;
        *(s32*)(seq + 0x10) = value;
        current = *(u8**)(seq + 0x24);
        while (current != NULL) {
            *(s32*)(current + 0x70) += value;
            *(s32*)(current + 0x74) += value;
            current = *(u8**)(current + 0xA8);
        }
    }
    return TRUE;
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
    extern void fn_8010147C(void* resource, u32 size, u32 group, u32 handle);
    extern void fn_801012E8(void* archive, u32 resourceArg, u32 callbackArg);
    extern void* GSresGetResource(u32 group, u32 resource);
    u8* sequence = sequencePtr;
    u8* data = dataPtr;
    u8* next = data;
    u8* owner = *(u8**)(sequence + 0x3C);
    u32 firstHandle = wazaSequenceSysGetResID();
    u32 secondHandle = wazaSequenceSysGetResID();
    u32 size;
    u32 flags;
    u32 mode;

    *(u32*)(sequence + 0x0C) = *(u32*)(next + 0x00);
    if (*(u32*)(sequence + 0x0C) == 0x0B) {
        if (*(u16*)(owner + 0x14) < *(u32*)(sequence + 0x0C)) {
            *(u32*)(sequence + 0x0C) = 0;
        }
    } else if (*(u32*)(sequence + 0x0C) >= 0x0C &&
               *(u32*)(sequence + 0x0C) < 0x10 &&
               *(u16*)(owner + 0x14) < *(u32*)(sequence + 0x0C)) {
        *(u32*)(sequence + 0x0C) = 1;
    }

    flags = *(u32*)(next + 0x08);
    *(u32*)(sequence + 0x08) = flags;
    *(u8*)(sequence + 0x16) = 1;
    *(u8*)(sequence + 0x17) = 2;

    mode = *(u32*)(next + 0x10);
    switch (mode) {
    case 1:
        size = 0;
        next += 0x10;
        break;
    case 2:
        size = 0;
        next += 0x14;
        break;
    case 5:
        *(u8*)(sequence + 0x17) = *(u32*)(next + 0x0C);
        /* fallthrough */
    default:
        next = (u8*)((((u32)next + 0x37) & ~0x1F));
        size = *(u32*)(data + 0x14);
        break;
    }

    if (mode <= 3) {
        *(u32*)(sequence + 0x08) |= 0x78;
    }
    if (mode <= 5) {
        flags = *(u32*)(sequence + 0x08);
        if (flags & 0x80000000) {
            flags ^= 0x80000000;
        }
        if (flags & 0x00010000) {
            flags ^= 0x00010000;
        }
        if (flags & 0x00020000) {
            flags ^= 0x00020000;
        }
        if (flags & 0x00040000) {
            flags ^= 0x00040000;
        }
        if (flags & 0x00080000) {
            flags ^= 0x00080000;
        }
        if (flags & 0x00100000) {
            flags ^= 0x00100000;
        }
        *(u32*)(sequence + 0x08) = flags;
    }

    if (size != 0) {
        *(u32*)(sequence + 0x18) = 0x4E20;
        fn_8010147C(next, size, 0x4E20, firstHandle);
        if (GSresGetResource(0x4E20, firstHandle) != NULL) {
            *(u32*)(sequence + 0x1C) = firstHandle;
            fn_801012E8(GSresGetResource(0x4E20, firstHandle), 0x4E20,
                        secondHandle);
            if (GSresGetResource(0x4E20, secondHandle) != NULL) {
                *(u32*)(sequence + 0x20) = secondHandle;
            }
        }
        next += (size + 0x1F) & ~0x1F;
    } else {
        *(u32*)(sequence + 0x18) = 0;
        *(u32*)(sequence + 0x20) = 0;
        *(u32*)(sequence + 0x1C) = 0;
    }

    return next;
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
    typedef struct WazaParticleData {
        s32 field_00;
        s32 field_04;
        u32 resourceSize;
        s32 format;
        u8 payload[];
    } WazaParticleData;
    extern void loadParticle(void*, u32, u32, s32);
    extern void* GSresGetResource(s32, s32);
    extern void* fn_801195AC(void*);
    WazaSequence* owner = sequence;
    WazaSequenceNode* entry = entryPtr;
    WazaParticleData* data = dataPtr;
    WazaSequenceNode* linked;
    u8* payload = data->payload + (data->format == 3 ? 4 : 0);

    if (entry->state != 0) {
        linked = owner->firstNode;
        while (linked != NULL && linked->linkKey != entry->state) {
            linked = linked->next;
        }
        entry->resource = linked->resource;
        entry->resourceId = 0;
        entry->runtimeFlags = 0;
    } else {
        entry->resourceId = 0x4E20;
        entry->runtimeFlags = wazaSequenceSysGetResID();
        loadParticle(payload, data->resourceSize, 0x4E20,
                     entry->runtimeFlags);
        payload += (data->resourceSize + 0x1F) & ~0x1F;
        entry->resource =
            GSresGetResource(0x4E20, entry->runtimeFlags);
        if (entry->resource != NULL) {
            entry->resource = fn_801195AC(entry->resource);
        }
    }

    entry->field_80 = data->field_00;
    entry->animationMode = data->field_04;
    entry->textureAnimationMode = 0;
    return payload;
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
