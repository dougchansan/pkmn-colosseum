#include "game/battle/battle_waza_types.h"

void wazaSequenceApplyStop(void* obj)
{
    WazaSequence* sequence;
    WazaSequenceOwner* owner;
    WazaSequenceNode* node;

    sequence = obj;
    if (sequence != NULL) {
        owner = sequence->owner;
        if (sequence->active != 0) {
            if (sequence->cameraActive != 0) {
                fn_801D3034(owner);
            }
            if (owner->animationActive == 0) {
                wazaSequenceOwnerPlayIdle(owner, 1, 0);
            }
            fn_800E3CC8(owner->model, 0);
            if ((sequence->flags & 0x08000000) != 0) {
                GSmodelLinkToGSparticleBank(owner->model, owner->particleBank);
            }
            if ((sequence->flags & 0x04000000) != 0) {
                battleGridResetModelVisibilityFlags();
            }
            node = sequence->firstNode;
            while (node != NULL) {
                wazaSequenceEntryStop(node, 1);
                node = node->next;
            }
            node = sequence->firstNode;
            while (node != NULL) {
                if (node->kind == 3 && node->state == 0 && node->resource != NULL) {
                    gsParticleKill(node->resource, 1);
                }
                node = node->next;
            }
            owner->currentSequence = NULL;
            sequence->active = 0;
            sequence->stopping = 0;
        }
    }
}

void fn_801DBC30(void* obj)
{
    WazaSequence* sequence;
    WazaSequenceOwner* owner;
    s32 kind;

    sequence = obj;
    if (sequence != NULL) {
        owner = sequence->owner;
        if (sequence->active != 0 && owner->currentSequence == sequence) {
            kind = sequence->kind;
            if (kind >= 0xB || kind < 9) {
                wazaSequenceOwnerPlayIdle(owner, 1, 0);
            }
            if (sequence->cameraActive != 0) {
                fn_801D3034(owner);
            }
            fn_800E3CC8(owner->model, 0);
            owner->currentSequence = NULL;
        }
    }
}

void wazaSequenceStart(void* sequencePtr)
{
    WazaSequence* sequence;
    WazaSequenceOwner* owner;
    WazaSequenceNode* node;
    WazaSequence* current;
    s32 bit;
    struct GSmodel* model;
    u32 flags;

    sequence = sequencePtr;
    if (sequence->active == 0) {
        owner = sequence->owner;
        flags = sequence->flags;
        current = owner->currentSequence;
        flags = (flags >> 1) & 1;
        node = sequence->firstNode;
        model = owner->model;
        bit = flags;
        if (current != NULL) {
            wazaSequenceApplyStop(current);
        }
        if (owner->animationActive != 0 && sequence->animationMode == 2) {
            wazaSequenceSysResetAnimationExcept(owner);
        }
        wazaSequenceOwnerSetAnim(owner, sequence);
        if ((sequence->flags & 0x08000000) != 0) {
            GSmodelLinkToGSparticleBank(model, NULL);
        }
        wazaSequencePokemonMotionStart(owner, bit);
        owner->currentSequence = sequence;
        sequence->active = 1;
        if ((sequence->flags & 0x04000000) != 0) {
            battleGridHideModelsExcept(owner);
        }
        if (sequence->cameraActive != 0) {
            battleCameraStartWaza(owner, sequence);
        }
        while (node != NULL) {
            node->runtimeState = 0;
            node = node->next;
        }
        sequence->state = 0;
        sequence->stopping = 0;
        wazaSequenceUpdate(sequence);
    }
}
