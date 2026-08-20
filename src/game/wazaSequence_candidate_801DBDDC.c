#define wazaSequenceFree wazaSequenceFree_disabled
#define PR409_WAZA_SEQUENCE_SPLIT
#define PR409_WAZA_SEQUENCE_BDDC_BFB0
#include "src/game/wazaSequence.c"
#undef wazaSequenceFree

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
    u8 pad_04[0x10];
    u32 resourceGroup;
    u32 resourceA;
    u32 resourceB;
    WazaSequenceNodeLocal* firstNode;
    u16 nodeHandle;
    u16 handle;
    u16 animationMode;
    u8 pad_30[2];
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
    extern void fn_80131268(u32);
    extern void GSlogWrite(const char*, ...);
    extern const char lbl_8027995C[];
    WazaSequenceLocal* sequence;
    WazaSequenceNodeLocal* node;
    WazaSequenceOwner* owner;

    sequence = obj;
    if (sequence == NULL) {
        return;
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
                    node->field_8C = 0;
                    node->resource = NULL;
                } else {
                    if (node->resource != NULL) {
                        fn_801193BC(node->resource);
                        fn_800F9210(node->resourceGroup, node->resourceA);
                    }
                    node->field_8C = 0;
                    node->resource = NULL;
                }
                break;
            case 4:
                fn_80131268(node->resourceGroup);
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

    if (sequence->nodeHandle != 0) {
        fn_800E24B0(sequence->nodeHandle);
        fn_800E209C(sequence->nodeHandle);
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

    owner = sequence->owner;
    if (sequence->previous != NULL) {
        sequence->previous->next = sequence->next;
    }
    if (sequence->next != NULL) {
        sequence->next->previous = sequence->previous;
    } else {
        owner->sequenceList = (WazaSequence*) sequence->previous;
    }

    if (sequence->handle != 0) {
        fn_800E24B0(sequence->handle);
        fn_800E209C(sequence->handle);
    }
}
