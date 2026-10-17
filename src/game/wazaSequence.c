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
s32 wazaSequenceUpdate(void* sequence) {
    /* RULE-EXCEPTION(title-path): named shared log strings from this TU's
     * original pool; see docs/RULE_EXCEPTIONS.md. */
    extern const char lbl_802798F0[];
    extern const char lbl_80279928[];
    WazaSequence* obj = sequence;
    WazaSequenceNode* node = obj->firstNode;
    WazaSequenceOwner* owner;
    s32 elapsed = msgGetCharsPerFrame();
    s32 running = 0;
    s32 pending = 0;

    obj->state += elapsed;
    owner = obj->owner;

    if (fn_801DA74C(owner, obj->moveIndex, obj->animationMode, 2) >
        (s32)obj->state) {
        pending = 1;
    }

    if (owner->motionBusy == 0 && owner->sequenceEnabled != 0 &&
        fn_801DA74C(owner, obj->moveIndex, obj->animationMode, 1) <
            (s32)obj->state) {
        return -1;
    }

    while (node != NULL) {
        switch (node->runtimeState) {
        case 2:
        default:
            break;
        case 0:
            if (node->startTime <= (s32)obj->state) {
                if (wazaSequenceEntryStart(node) != 0) {
                    pending++;
                } else {
                    GSlogWrite(lbl_802798F0);
                }
            } else {
                node->currentTime = node->startTime;
                running++;
            }
            break;
        case 1:
            if (wazaSequenceEntryUpdate(node, elapsed) == 0) {
                if (wazaSequenceEntryStop(node, FALSE) == 0) {
                    GSlogWrite(lbl_80279928);
                }
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
    return (s8)obj->stopping == 0;
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

/* WAZA_SEQUENCE_C014_ONLY (set by wazaSequence_r52_801DC014_prefix.c) builds
 * only 0x801DC014-0x801DCAA0: wazaSequenceLoadData, wazaSequenceEntryLink,
 * fn_801DC46C, fn_801DC5F0 and _wazaSequenceEffectEntryLoad. */
#if (!defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8)) && \
    !defined(WAZA_SEQUENCE_DCAA0_ONLY)

/**
 * wazaSequenceLoadData - Waza screen distortion effect.
 * Address: 0x801DC014 | Size: 0x2FC
 */
/* RULE-EXCEPTION(user-approved): single-use inline helpers; their return temps add the two coalesced neighbours that give seq retail's r31 - see docs/RULE_EXCEPTIONS.md */
static inline u8* wazaSequenceLoadModel(u8* seq, u8* entry, u8* parsed) {
    return _wazaSequenceModelEntryLoad(seq, entry, parsed);
}

static inline u8* wazaSequenceLoadParticle(u8* seq, u8* entry, u8* parsed) {
    return _wazaSequenceParticleEntryLoad(seq, entry, parsed);
}

u8 wazaSequenceLoadData(void* sequence, void* resource) {
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* fn_800E27B0(u16);
    extern void fn_800E24B0(u16);
    extern void fn_800E209C(u16);
    extern u32 fn_800D37CC(void);
    extern void GSlogWrite(const char*, ...);
    extern const char lbl_8027997C[];
    extern f32 lbl_8047E3A0;
    u8 parser[0xB4];
    u8* header;
    u8* data;
    s32 earliest;
    s32 adjustment;
    u8* current;
    u8* parsed;
    s32 count;
    s32 i;
    u16 handle;
    u8* entry;
    s32 value;
    u8* seq = sequence;

    if (seq == NULL) {
        return FALSE;
    }
    if (*(void**)(seq + 0x3C) == NULL) {
        return FALSE;
    }

    header = fn_801DC46C(parser, resource);
    count = *(s32*)(header + 4) - 1;
    data = fn_801DC5F0(seq, header);
    if (count != 0) {
        handle = _toolentryAlloc__FUl(count * 0xB4);
        if (handle != 0) {
            *(void**)(seq + 0x24) = NULL;
            entry = fn_800E27B0(handle);
            *(s32*)(seq + 4) = count;
            *(s32*)(seq + 0) = 0;
            *(u16*)(seq + 0x28) = handle;

            for (i = 0; i < count; i++, entry += 0xB4) {
                parsed = fn_801DC46C(entry, data);
                switch (*(u32*)(entry + 4)) {
                case 1:
                    *(s32*)(entry + 0x78) = *(s32*)(parsed + 0);
                    data = parsed + 0x0C;
                    switch (*(s32*)(entry + 0x78)) {
                    case 0:
                        *(s32*)(entry + 0x7C) =
                            (s32)(((f32)*(s32*)(parsed + 4) *
                                   (f32)(s32)fn_800D37CC()) /
                                  lbl_8047E3A0);
                        break;
                    case 1:
                        *(s32*)(entry + 0x7C) = *(s32*)(parsed + 4);
                        break;
                    case 2:
                        *(f32*)(entry + 0x7C) = *(f32*)(parsed + 4);
                        break;
                    case 3:
                        data += *(s32*)(parsed + 4) * 8;
                        *(s32*)(entry + 0x78) = 0;
                        *(s32*)(entry + 0x7C) = 0;
                        break;
                    }
                    break;
                case 2:
                    data = wazaSequenceLoadModel(seq, entry, parsed);
                    break;
                case 3:
                    data = wazaSequenceLoadParticle(seq, entry, parsed);
                    break;
                case 4:
                    data = _wazaSequenceEffectEntryLoad(entry, parsed);
                    break;
                case 5:
                    switch (*(s32*)(parsed + 4)) {
                    case 1:
                    case 2:
                        *(s32*)(entry + 0x7C) = 0;
                        adjustment = -4;
                        break;
                    case 3:
                    default:
                        *(s32*)(entry + 0x7C) = *(s32*)(parsed + 8);
                        adjustment = 0;
                        break;
                    }
                    *(s32*)(entry + 0x78) = *(s32*)(parsed + 0);
                    /* RULE-EXCEPTION(user-approved): codegen-only integer cast keeps retail's add operand order - see docs/RULE_EXCEPTIONS.md */
                    data = (u8*)(adjustment + (s32)parsed);
                    data += 0x0C;
                    break;
                case 6:
                    *(s32*)(entry + 0x78) = *(s32*)(parsed + 0);
                    data = parsed + 8;
                    break;
                case 0:
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
    u8* previous;
    s32 key;
    u8* current;

    current = *(u8**)(sequence + 0x24);
    previous = current;
    key = *(s32*)(entry + 8);

    if (key != 0) {
        while (current != NULL) {
            if (*(s32*)current == key) {
                break;
            }
            current = *(u8**)(current + 0xA8);
        }
        if (current != NULL) {
            *(s32*)(entry + 0x70) = *(s32*)(current + 0x70);
            *(s32*)(entry + 0x70) +=
                ((s32*)(current + 0x2C))[*(s32*)(entry + 0x10)];
            /* RULE-EXCEPTION(user-approved): goto reproduces retail's key == 0 branch straight to the table lookup - see docs/RULE_EXCEPTIONS.md */
            goto linked;
        }
    }
    *(s32*)(entry + 0x70) = ((s32*)(*(u8**)(*(u8**)(sequence + 0x3C) + 0x2C) +
        *(s32*)(sequence + 0x0C) * 0xD4 + 0x0C))[*(s32*)(entry + 0x10)];
linked:

    *(s32*)(entry + 0x70) -= ((s32*)(entry + 0x2C))[*(s32*)(entry + 0x0C)];
    *(s32*)(entry + 0x74) = *(s32*)(entry + 0x70);

    if (previous != NULL) {
        if (*(s32*)(previous + 0x70) > *(s32*)(entry + 0x70) ||
            (*(s32*)(entry + 4) == 6 &&
             *(s32*)(previous + 0x70) == *(s32*)(entry + 0x70))) {
            *(u8**)(entry + 0xA8) = previous;
            *(void**)(entry + 0xAC) = NULL;
            *(u8**)(previous + 0xAC) = entry;
            *(u8**)(sequence + 0x24) = entry;
        } else {
            while (*(u8**)(previous + 0xA8) != NULL) {
                if (*(s32*)(*(u8**)(previous + 0xA8) + 0x70) > *(s32*)(entry + 0x70)) {
                    break;
                }
                if (*(s32*)(entry + 4) == 6 &&
                    *(s32*)(previous + 0x70) == *(s32*)(entry + 0x70)) {
                    break;
                }
                previous = *(u8**)(previous + 0xA8);
            }
            *(u8**)(entry + 0xA8) = *(u8**)(previous + 0xA8);
            if (*(u8**)(entry + 0xA8) != NULL) {
                *(u8**)(*(u8**)(entry + 0xA8) + 0xAC) = entry;
            }
            *(u8**)(previous + 0xA8) = entry;
            *(u8**)(entry + 0xAC) = previous;
        }
    } else {
        *(u8**)(sequence + 0x24) = entry;
        *(void**)(entry + 0xA8) = NULL;
        *(void**)(entry + 0xAC) = NULL;
    }
    *(u8**)(entry + 0xB0) = sequence;
}

/**
 * fn_801DC46C - Waza screen overlay effect.
 * Address: 0x801DC46C | Size: 0x184
 */
void* fn_801DC46C(u8* entry, void* dataPtr) {
    extern s32 fn_800D37CC(void);
    extern const f32 lbl_8047E3A0;
    s32 adjustment = 0;
    /* RULE-EXCEPTION(user-approved): the switch and field reads go through a typed copy of dataPtr while the result is computed on dataPtr as an integer, as retail's registers do - see docs/RULE_EXCEPTIONS.md */
    u8* data = dataPtr;
    s32 i;

    switch (*(s32*)(data + 0x68)) {
    case 1:
        *(s32*)(entry + 0x18) = 0;
        adjustment = -4;
        break;
    case 2:
        *(s32*)(entry + 0x18) = *(s32*)(data + 0x6C);
        adjustment = 0;
        break;
    }

    *(s32*)(entry + 0x00) = *(s32*)(data + 0x00);
    *(s32*)(entry + 0x04) = *(s32*)(data + 0x04);
    *(s32*)(entry + 0x08) = *(s32*)(data + 0x10);
    *(s32*)(entry + 0x0C) = *(s32*)(data + 0x14);
    *(s32*)(entry + 0x10) = *(s32*)(data + 0x18);
    *(s32*)(entry + 0x14) = *(s32*)(data + 0x1C);
    *(s32*)(entry + 0x1C) = *(s32*)(data + 0x60);
    *(s32*)(entry + 0x24) = *(s32*)(data + 0x64);
    *(s32*)(entry + 0x20) = *(s32*)(data + 0x08);
    *(s32*)(entry + 0x28) = *(s32*)(data + 0x0C);
    memset(entry + 0x2C, 0, 0x40);
    for (i = 0; i < 16; i++) {
        *(s32*)(entry + i * 4 + 0x2C) =
            (s32)((f32)*(s32*)(data + i * 4 + 0x20) * (f32)fn_800D37CC() / lbl_8047E3A0);
    }

    *(void**)(entry + 0xA8) = NULL;
    *(void**)(entry + 0xAC) = NULL;
    *(s32*)(entry + 0x74) = 0;
    *(s32*)(entry + 0x70) = 0;
    *(s32*)(entry + 0x6C) = 0;
    dataPtr = (void*)(adjustment + (s32)dataPtr);
    dataPtr = (u8*)dataPtr + 0x70;
    return dataPtr;
}


#endif

#if (!defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8) || \
     defined(PR409_WAZA_SEQUENCE_DC5F0)) && !defined(WAZA_SEQUENCE_DCAA0_ONLY)
/**
 * fn_801DC5F0 - Waza screen overlay update.
 * Address: 0x801DC5F0 | Size: 0x22C
 */
void* fn_801DC5F0(void* sequencePtr, u8* next) {
    extern void loadArchivePtr(void* resource, u32 size, u32 group, u32 handle);
    extern void loadCamera(void* archive, u32 resourceArg, u32 callbackArg);
    extern void* GSresGetResource(u32 group, u32 resource);
    u8* sequence = sequencePtr;
    s32* header = (s32*)next;
    u32 firstHandle = wazaSequenceSysGetResID();
    u32 secondHandle = wazaSequenceSysGetResID();
    u8* owner = *(u8**)(sequence + 0x3C);
    s32 size;

    *(s32*)(sequence + 0x0C) = header[0];
    switch (*(s32*)(sequence + 0x0C)) {
    case 0x0B:
        if (*(u16*)(owner + 0x14) < *(s32*)(sequence + 0x0C)) {
            *(s32*)(sequence + 0x0C) = 0;
        }
        break;
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
        if (*(u16*)(owner + 0x14) < *(s32*)(sequence + 0x0C)) {
            *(s32*)(sequence + 0x0C) = 1;
        }
        break;
    }

    *(u32*)(sequence + 0x08) = header[2];
    *(u8*)(sequence + 0x16) = 1;
    *(u8*)(sequence + 0x17) = 2;

    switch (header[4]) {
    case 1:
        size = 0;
        next += 0x10;
        break;
    case 2:
        size = 0;
        next += 0x14;
        break;
    case 5:
        *(u8*)(sequence + 0x17) = header[3];
        /* fallthrough */
    default:
        next = (u8*)((((u32)next + 0x37) & ~0x1F));
        size = header[5];
        break;
    }

    if (header[4] <= 3) {
        *(u32*)(sequence + 0x08) |= 0x78;
    }
    if (header[4] <= 5) {
        if (*(u32*)(sequence + 0x08) & 0x00008000) {
            *(u32*)(sequence + 0x08) = *(u32*)(sequence + 0x08) ^ 0x00008000;
        }
        if (*(u32*)(sequence + 0x08) & 0x00010000) {
            *(u32*)(sequence + 0x08) = *(u32*)(sequence + 0x08) ^ 0x00010000;
        }
        if (*(u32*)(sequence + 0x08) & 0x00020000) {
            *(u32*)(sequence + 0x08) = *(u32*)(sequence + 0x08) ^ 0x00020000;
        }
        if (*(u32*)(sequence + 0x08) & 0x00040000) {
            *(u32*)(sequence + 0x08) = *(u32*)(sequence + 0x08) ^ 0x00040000;
        }
        if (*(u32*)(sequence + 0x08) & 0x00080000) {
            *(u32*)(sequence + 0x08) = *(u32*)(sequence + 0x08) ^ 0x00080000;
        }
        if (*(u32*)(sequence + 0x08) & 0x00100000) {
            *(u32*)(sequence + 0x08) = *(u32*)(sequence + 0x08) ^ 0x00100000;
        }
    }

    if (size != 0) {
        *(u32*)(sequence + 0x18) = 0x4E20;
        loadArchivePtr(next, size, 0x4E20, firstHandle);
        if (GSresGetResource(0x4E20, firstHandle) != NULL) {
            *(u32*)(sequence + 0x1C) = firstHandle;
            loadCamera(GSresGetResource(0x4E20, firstHandle), 0x4E20,
                        secondHandle);
            if (GSresGetResource(0x4E20, secondHandle) != NULL) {
                *(u32*)(sequence + 0x20) = secondHandle;
            }
        }
        next += (header[5] + 0x1F) & ~0x1F;
    } else {
        *(u32*)(sequence + 0x18) = 0;
        *(u32*)(sequence + 0x20) = 0;
        *(u32*)(sequence + 0x1C) = 0;
    }

    return next;
}
#endif

#if (!defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8)) && \
    !defined(WAZA_SEQUENCE_DCAA0_ONLY)

/**
 * _wazaSequenceEffectEntryLoad - Waza screen effect composite.
 * Address: 0x801DC81C | Size: 0x284
 */
typedef struct WazaEffectLoadList {
    u8 pad_00[0x08];
    u32 size;
    u8 pad_0C[0x04];
    struct WazaEffectLoadList* next;
} WazaEffectLoadList;


void* _wazaSequenceEffectEntryLoad(void* entryPtr, void* dataPtr)
{
    extern s32 fn_800D37CC(void);
    extern u8* fn_801364A8(u8* work, u8* desc);
    extern u32 fn_8013AABC(void* callbacks);
    extern u32 electronStartEffect(void* callbacks);
    extern u32 fn_8013A42C(void* callbacks);
    extern u32 fn_801380D4(void* callbacks);
    extern u32 fn_8013796C(void* callbacks);
    extern u32 leaffxStartEffect(void* callbacks);
    extern u32 fn_8013C5A0(void* callbacks);
    extern u32 fn_8013B490(void* callbacks);
    extern u32 fn_8013D6B8(void* callbacks);
    extern u32 fn_8013DC18(void* callbacks);
    extern u32 fn_8013E4D4(void* callbacks);
    extern u32 patchiruTextureStart(void* callbacks);
    extern u32 fn_8013F000(void* callbacks);
    extern const f32 lbl_8047E3A0;
    u8 parsed[0xD8];
    u32 type;
    u32 handle = 0;
    WazaSequenceNode* entry = entryPtr;
    const WazaEffectLoadList* list;
    const WazaEffectLoadList* node;

    entry->resourceId = 0;
    entry->runtimeFlags =
        (s32)(((f32)(s32)fn_800D37CC() * (f32)(s32)*(u32*)((u8*)dataPtr + 4)) /
              lbl_8047E3A0);
    entry->resource = *(void**)dataPtr;
    entry->field_80 = 0;
    entry->animationMode = 0;

    dataPtr = fn_801364A8(parsed, dataPtr);
    type = *(u32*)parsed;
    switch (type) {
    case 0:
        list = *(const WazaEffectLoadList**)(parsed + 0x58);
        entry->resourceId = fn_8013AABC(parsed + 8);
        while (list != NULL) {
            handle += list->size;
            list = list->next;
        }
        break;
    case 1:
        entry->field_80 = *(u32*)(parsed + 0x58);
        entry->resourceId = electronStartEffect(parsed + 8);
        handle = *(u16*)(parsed + 0x52);
        break;
    case 2:
        entry->resourceId = fn_8013A42C(parsed + 8);
        handle = *(u16*)(parsed + 0x12);
        break;
    case 3:
        entry->field_80 = *(u16*)(parsed + 0x16);
        entry->resourceId = fn_801380D4(parsed + 8);
        handle = *(u16*)(parsed + 0x1A);
        break;
    case 4:
        entry->field_80 = *(u16*)(parsed + 0x30);
        entry->animationMode = *(u16*)(parsed + 0x32);
        entry->resourceId = fn_8013796C(parsed + 8);
        handle = *(u16*)(parsed + 0x28);
        break;
    case 5:
        entry->field_80 = *(s16*)(parsed + 0x52);
        entry->resourceId = leaffxStartEffect(parsed + 8);
        handle = *(u16*)(parsed + 0x46);
        break;
    case 6:
        entry->resourceId = fn_8013C5A0(parsed + 8);
        handle = *(u16*)(parsed + 0xAE);
        break;
    case 7:
        entry->resourceId = fn_8013B490(parsed + 8);
        handle = *(u16*)(parsed + 0xD6);
        break;
    case 8:
        entry->resourceId = fn_8013D6B8(parsed + 8);
        handle = 0;
        node = *(const WazaEffectLoadList**)(parsed + 0x14);
        while (node != NULL) {
            handle += node->size;
            node = node->next;
        }
        break;
    case 9:
        entry->resourceId = fn_8013DC18(parsed + 8);
        handle = 0;
        node = *(const WazaEffectLoadList**)(parsed + 0x1C);
        while (node != NULL) {
            handle += node->size;
            node = node->next;
        }
        break;
    case 10:
        entry->resourceId = fn_8013E4D4(parsed + 8);
        handle = *(u16*)(parsed + 0x3A);
        break;
    case 11:
        entry->resourceId = patchiruTextureStart(parsed + 8);
        /* RULE-EXCEPTION(user-approved): redundant reset retail performs here - see docs/RULE_EXCEPTIONS.md */
        handle = 0;
        break;
    case 12:
        entry->resourceId = fn_8013F000(parsed + 8);
        handle = *(u16*)(parsed + 0xBA);
        break;
    }
    if (entry->timing[entry->timingIndex - 1] == 0) {
        entry->timing[entry->timingIndex - 1] = handle;
    }
    return dataPtr;
}

#endif

/* WAZA_SEQUENCE_DCAA0_ONLY (set by
 * wazaSequence_candidate_801DC014_r40_801DCAA0_gc125n.c) builds this function
 * alone; that unit compiles with -opt nopropagation. */
#if (!defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8)) && \
    !defined(WAZA_SEQUENCE_C014_ONLY)

/**
 * _wazaSequenceParticleEntryLoad - Waza screen effect finalize.
 * Address: 0x801DCAA0 | Size: 0x128
 */
void* _wazaSequenceParticleEntryLoad(void* sequence, u8* entry,
                                     u8* data) {
    extern void loadParticle(void*, u32, u32, s32);
    extern void* GSresGetResource(s32, s32);
    extern void* gsParticleBankLoad(void*);
    u8* header = data;
    u8* current;
    void* resource;
    s32 skip;
    s32 key;

    switch (*(s32*)(header + 0x0C)) {
    case 3:
        skip = 4;
        break;
    case 2:
    case 4:
    default:
        skip = 0;
        break;
    }
    data = (u8*)(skip + (s32)data);
    data += 0x10;

    key = *(s32*)(entry + 0x18);
    if (key != 0) {
        for (current = *(u8**)((u8*)sequence + 0x24); current != NULL;
             current = *(u8**)(current + 0xA8)) {
            if (*(s32*)current == key) {
                break;
            }
        }
        *(void**)(entry + 0x88) = *(void**)(current + 0x88);
        *(s32*)(entry + 0x78) = 0;
        *(s32*)(entry + 0x7C) = 0;
    } else {
        *(s32*)(entry + 0x78) = 0x4E20;
        *(s32*)(entry + 0x7C) = wazaSequenceSysGetResID();
        loadParticle(data, *(u32*)(header + 8), 0x4E20, *(s32*)(entry + 0x7C));
        data += (*(u32*)(header + 8) + 0x1F) & ~0x1F;
        resource = GSresGetResource(0x4E20, *(s32*)(entry + 0x7C));
        if (resource != NULL) {
            *(void**)(entry + 0x88) = gsParticleBankLoad(resource);
        } else {
            *(void**)(entry + 0x88) = NULL;
        }
    }

    *(s32*)(entry + 0x80) = *(s32*)(header + 0);
    *(s32*)(entry + 0x84) = *(s32*)(header + 4);
    *(s32*)(entry + 0x8C) = 0;
    return data;
}

#endif

#if (!defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8) || \
     defined(PR409_WAZA_SEQUENCE_DCBC8)) && !defined(WAZA_SEQUENCE_C014_ONLY) && \
    !defined(WAZA_SEQUENCE_DCAA0_ONLY)
/**
 * _wazaSequenceModelEntryLoad - Waza field effect handler.
 * Address: 0x801DCBC8 | Size: 0x1E0
 */
void* _wazaSequenceModelEntryLoad(void* sequence, u8* entry, u8* data) {
    extern void loadArchivePtr(void* resource, u32 size, u32 group, u32 handle);
    extern void loadModel(void* model, u32 group, u32 flags, u32 handle);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void GSmodelGetFrameCount(void* model, f32* start, f32* end);
    extern const f32 lbl_8047E3B0;
    u8* header = data;
    u32 modelHandle = wazaSequenceSysGetResID();
    u32 motionHandle = wazaSequenceSysGetResID();
    f32 start;
    f32 end;

    *(s32*)(entry + 0x78) = 0x4E20;
    data = (u8*)(((u32)data + 0x43) & ~0x1F);
    *(u32*)(entry + 0x80) = 0;
    *(u32*)(entry + 0x7C) = 0;
    *(void**)(entry + 0xA4) = NULL;
    loadArchivePtr(data, *(u32*)(header + 0x1C), 0x4E20, modelHandle);
    if (GSresGetResource(0x4E20, modelHandle) != NULL) {
        *(u32*)(entry + 0x80) = modelHandle;
        loadModel(GSresGetResource(0x4E20, modelHandle), 0x4E20, 0, motionHandle);
        if (GSresGetResource(0x4E20, motionHandle) != NULL) {
            *(u32*)(entry + 0x7C) = motionHandle;
            *(void**)(entry + 0xA4) = GSresGetResource(0x4E20, motionHandle);
        }
    }

    data += (*(u32*)(header + 0x1C) + 0x1F) & ~0x1F;
    *(s32*)(entry + 0x88) = *(s32*)(header + 0x00);
    switch (*(s32*)(header + 0x04)) {
    case 0:
        *(s32*)(entry + 0x84) = 0;
        break;
    case 1:
        *(s32*)(entry + 0x84) = 1;
        break;
    }
    *(s32*)(entry + 0x90) = *(s32*)(header + 0x08);
    switch (*(s32*)(header + 0x0C)) {
    case 0:
        *(s32*)(entry + 0x8C) = 0;
        break;
    case 1:
        *(s32*)(entry + 0x8C) = 1;
        break;
    }
    *(s32*)(entry + 0x94) = *(s32*)(header + 0x10);
    *(s32*)(entry + 0x98) = *(s32*)(header + 0x14);
    *(s32*)(entry + 0x9C) = *(s32*)(header + 0x18);

    if (((s32*)(entry + 0x28))[*(s32*)(entry + 0x14)] == 0) {
        start = lbl_8047E3B0;
        end = lbl_8047E3B0;
        if (*(void**)(entry + 0xA4) != NULL) {
            GSmodelGetFrameCount(*(void**)(entry + 0xA4), &start, &end);
        }
        ((s32*)(entry + 0x28))[*(s32*)(entry + 0x14)] = (s32)(start >= end ? start : end);
    }
    return data;
}
#endif

#if (!defined(PR409_WAZA_SEQUENCE_SPLIT) || defined(PR409_WAZA_SEQUENCE_C014_CDA8)) && \
    !defined(WAZA_SEQUENCE_DCAA0_ONLY)

#endif
