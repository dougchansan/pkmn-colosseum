#include "game/battle/battle_waza_types.h"

typedef struct WazaSequenceNodeLocal {
    s32 linkKey;
    s32 kind;
    s32 linkedEntryKey;
    s32 sourceIndex;
    s32 targetIndex;
    s32 timingIndex;
    s32 state;
    u32 flags;
    s32 attachment;
    s32 partIndex;
    s32 positionType;
    s32 timing[0x10];
    u32 runtimeState;
    s32 startTime;
    s32 currentTime;
    u32 resourceGroup;
    u32 resourceA;
    u32 resourceB;
    u32 field_84;
    void* resource;
    u32 field_8C;
    s32 textureAnimation;
    s32 restoreTransform;
    u8 pad_98[8];
    s32 attached;
    void* model;
    struct WazaSequenceNodeLocal* next;
    struct WazaSequenceNodeLocal* previous;
    WazaSequence* sequence;
} WazaSequenceNodeLocal;

typedef struct WazaSequenceLocal {
    u32 state;
    u8 pad_04[0x14];
    u32 resourceGroup;
    u32 resourceA;
    u32 resourceB;
    WazaSequenceNodeLocal* firstNode;
    u16 nodeHandle;
    u16 handle;
    u16 moveIndex;
    u16 animationMode;
    u16 resourceId;
    u8 pad32[2];
    struct WazaSequenceLocal* previous;
    struct WazaSequenceLocal* next;
    WazaSequenceOwner* owner;
} WazaSequenceLocal;

void wazaSequenceFree(void* obj)
{
    extern void fn_800E24B0(u16 handle);
    extern void fn_800E209C(u16 handle);
    extern void fn_800F9210(u32, u32);
    extern void fn_801193BC(void*);
    extern void GSeffectFree(u32);
    extern void GSlogWrite(const char*, ...);
    /* RULE-EXCEPTION(title-path): shared log-pool stand-in;
     * see docs/RULE_EXCEPTIONS.md. */
    extern const char lbl_8027995C[];
    WazaSequenceLocal* sequence;
    WazaSequenceNodeLocal* node;
    WazaSequenceOwner* owner;
    WazaSequenceLocal* previous;
    WazaSequenceLocal* next;
    u16 handle;

    sequence = obj;
    if (sequence == NULL) {
        return;
    }

    /* RULE-EXCEPTION(title-path): retail retains this second null guard;
     * see docs/RULE_EXCEPTIONS.md. */
    if (sequence == NULL) {
        goto unlink;
    }
    node = sequence->firstNode;
    if (node != NULL) {
        while (node->next != NULL) {
            node = node->next;
        }
        while (node != NULL) {
            switch (node->kind) {
            case 2:
                if (node->resourceGroup != 0) {
                    if (node->resourceA != 0) {
                        fn_800F9210(node->resourceGroup, node->resourceA);
                    }
                    if (node->resourceB != 0) {
                        fn_800F9210(node->resourceGroup, node->resourceB);
                    }
                }
                break;
            case 3:
                if (node->state != 0) {
                    /* RULE-EXCEPTION(title-path): retain retail's stores before
                     * the common reset; see docs/RULE_EXCEPTIONS.md. */
                    node->field_8C = 0;
                    node->resource = NULL;
                } else {
                    if (node->resource != NULL) {
                        fn_801193BC(node->resource);
                        fn_800F9210(node->resourceGroup, node->resourceA);
                    }
                }
                node->field_8C = 0;
                node->resource = NULL;
                break;
            case 4:
                GSeffectFree(node->resourceGroup);
                break;
            case 0:
            case 1:
            case 5:
            case 6:
                break;
            default:
                GSlogWrite(lbl_8027995C);
                break;
            }
            node = node->previous;
        }
    }

    handle = sequence->nodeHandle;
    if (handle != 0) {
        fn_800E24B0(handle);
        fn_800E209C(handle);
    }

    if (sequence->resourceGroup != 0) {
        if (sequence->resourceB != 0) {
            fn_800F9210(sequence->resourceGroup, sequence->resourceB);
        }
        if (sequence->resourceA != 0) {
            fn_800F9210(sequence->resourceGroup, sequence->resourceA);
        }
    }

    wazaSequenceSysFreeWazaResource(sequence);

unlink:
    owner = sequence->owner;
    previous = sequence->previous;
    next = sequence->next;
    if (previous != NULL) {
        previous->next = next;
    }
    if (next != NULL) {
        next->previous = previous;
    } else {
        owner->sequenceList = (WazaSequence*)previous;
    }

    {
        u16 sequenceHandle = sequence->handle;
        if (sequenceHandle != 0) {
            fn_800E24B0(sequenceHandle);
            fn_800E209C(sequenceHandle);
        }
    }
}
