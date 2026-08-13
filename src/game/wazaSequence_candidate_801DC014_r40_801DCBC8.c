/* Score instrumentation only; not evidence of a retail TU boundary. */
#define _wazaSequenceModelEntryLoad _wazaSequenceModelEntryLoad_stub
#include "src/game/wazaSequence_candidate_801DC014.c"
#undef _wazaSequenceModelEntryLoad

void* _wazaSequenceModelEntryLoad(void* sequence, void* entryPtr,
                                  void* dataPtr)
{
    extern void fn_8010147C(void* resource, u32 size, u32 group, u32 handle);
    extern void fn_801013A0(void* resource, u32 group, u32 arg2, u32 handle);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void GSmodelGetFrameCount(void* model, f32* a, f32* b);
    WazaSequenceNode* entry = entryPtr;
    u8* data = dataPtr;
    u32 firstHandle = wazaSequenceSysGetResID();
    u32 secondHandle = wazaSequenceSysGetResID();
    u32 size;
    f32 frameA;
    f32 frameB;
    u32 timing;

    entry->resourceId = 0x4E20;
    entry->field_80 = 0;
    entry->runtimeFlags = 0;
    entry->model = NULL;

    size = (*(u32*)(data + 0x1C) + 0x1F) & ~0x1F;
    fn_8010147C(data + 0x43, *(u32*)(data + 0x1C), 0x4E20, firstHandle);
    if (GSresGetResource(0x4E20, firstHandle) != NULL) {
        entry->field_80 = firstHandle;
        fn_801013A0(GSresGetResource(0x4E20, firstHandle), 0x4E20, 0,
                    secondHandle);
        if (GSresGetResource(0x4E20, secondHandle) != NULL) {
            entry->runtimeFlags = secondHandle;
            entry->model = GSresGetResource(0x4E20, secondHandle);
        }
    }

    data += size;
    entry->animationMode = (*(u32*)(data + 4) == 1) ? 1 : 0;
    entry->textureAnimationMode = (*(u32*)(data + 0x0C) == 1) ? 1 : 0;
    entry->textureAnimation = *(u32*)(data + 0x08);
    entry->restoreTransform = *(u32*)(data + 0x10);
    *(u32*)((u8*)entry + 0x98) = *(u32*)(data + 0x14);
    *(u32*)((u8*)entry + 0x9C) = *(u32*)(data + 0x18);

    timing = entry->timing[entry->timingIndex];
    if (timing == 0) {
        frameA = 0.0f;
        frameB = 0.0f;
        if (entry->model != NULL) {
            GSmodelGetFrameCount(entry->model, &frameA, &frameB);
        }
        timing = (u32)((frameA >= frameB) ? frameA : frameB);
        entry->timing[entry->timingIndex] = timing;
    }
    return data;
}
