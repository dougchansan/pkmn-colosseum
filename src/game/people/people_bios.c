/**
 * @file people_bios.c
 * @brief The people core unit (.text 0x8018F470-0x8018FE30): the people
 *        array, entry accessors, info-bios getters and the floor
 *        save/restore hooks.
 *
 * A separate unit from people.c: it owns .sdata2 0x8047D8A8-0x8047D8B8
 * (0.0f, the degree factor and 1.0f, all of which people.c's pool also
 * holds), .sbss 0x8047B1F8-0x8047B208 and fn_8018F4C8's switch table at
 * .data 0x8036C540, and people.c calls even its eight-byte getters.
 *
 * Most of its functions are linked from their own carved units; this file
 * holds the save/restore hooks the candidate unit people_candidate_8018F730
 * scores (research source carried over from the old combined people.c).
 */
#include "dolphin/types.h"
#include "game/people/people.h"

extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void* GSmodelGetPositionPtr(void*);
extern void* GSmodelGetRotationPtr(void*);
extern void GSmodelPushState(void* model, void* state);
extern void GSmodelPopState(void* model, void* state);
extern void GSmodelSetVisibility(void*, u8);
extern void GScolsys2HumanEnable(s32, u8);
extern void GSvecCopy(void*, void*);
extern void GSmodelSetPosition(void*, void*);
extern void GSmodelSetRotation(void*, void*);
extern int peopleOpenSub(void* entry, u32 groupId, u32 indexId, s32 objectId);
extern void peopleSetMotionSlot(void* entry, u32 motionIndex);
extern void fn_801848D0(void* model, s32 group, s32 id, s32 part);
extern void std067_characterStopLookAt(u32 groupId, u32 index);
extern void std181_characterTurnHeadTowardsCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex);
extern void std066_characterLookAtPosition(u32 groupId, u32 index, u32 x, u32 y, u32 z);
extern void fn_801812C4(void* entry);

extern s32 lbl_8047B1F8;          /* maximum people count */
extern PeopleEntry* lbl_8047B200; /* people array (heap-allocated) */
extern const f32 lbl_8047D8B0;    /* default moveSpeed (1.0f) */

#define gPeopleMaxCount lbl_8047B1F8
#define gPeopleArray    lbl_8047B200

u32 peopleBiosGetPushDataSize(void) {
    PeopleEntry* current;
    PeopleEntry* entry = lbl_8047B200;
    s32 count = lbl_8047B1F8;
    u32 total;
    s32 i;

    total = 0;
    for (i = 0; i < count; i++) {
        if (i < 0 || count <= i) {
            current = NULL;
        } else {
            current = entry;
        }
        if (current->active != 0) {
            total += PEOPLE_SPAWN_DATA_SIZE;
        }
        entry = (PeopleEntry*)((u8*)entry + PEOPLE_ENTRY_SIZE);
    }

    return total;
}
void peopleBiosPushData(u8* dst, u32 size) {
    u32 offset;
    u8* end;
    u8* current;
    void* model;
    s32 i;

    i = 0;
    current = dst;
    offset = 0;
    end = dst + size;
    while (i < gPeopleMaxCount) {
        PeopleEntry* entry;

        if (i < 0 || gPeopleMaxCount <= i) {
            entry = NULL;
        } else {
            entry = (PeopleEntry*)((u8*)gPeopleArray + offset);
        }
        if (entry->active) {
            model = entry->modelHandle;
            memcpy(current, (u8*)entry + 0x20, 0xBC);
            memcpy(current + 0xBC, GSmodelGetPositionPtr(model), 0xC);
            memcpy(current + 0xC8, GSmodelGetRotationPtr(model), 0xC);
            memcpy(current + 0xD4, (u8*)model + 0x120, 0xC);
            memcpy(current + 0xE0, (u8*)model + 0x12C, 0xC);
            GSmodelPushState(model, current + 0xEC);
            current += PEOPLE_SPAWN_DATA_SIZE;
            if (current > end) {
                break;
            }
        }
        offset += PEOPLE_ENTRY_SIZE;
        i++;
    }
}
/* Take the first free slot of the people array and reset it. */
/* RULE-EXCEPTION(user-approved): single-use inline helper — see docs/RULE_EXCEPTIONS.md */
static inline PeopleEntry* peopleBiosAllocEntry(void) {
    s32 count = gPeopleMaxCount;
    PeopleEntry* current = gPeopleArray;
    PeopleEntry* entry;
    s32 i;

    for (i = 0; i < count; i++) {
        if (i < 0 || count <= i) {
            entry = NULL;
        } else {
            entry = current;
        }
        if (entry->active == 0) {
            memset(entry, 0, PEOPLE_ENTRY_SIZE);
            entry->active = 1;
            entry->selfPtr = entry;
            entry->shadowId = -1;
            entry->moveSpeed = lbl_8047D8B0;
            return entry;
        }
        current = (PeopleEntry*)((u8*)current + PEOPLE_ENTRY_SIZE);
    }
    return NULL;
}

/* fn_8018FB60's body: show the model with the given animation. */
/* RULE-EXCEPTION(user-approved): inline copy of the real fn_8018FB60 — see docs/RULE_EXCEPTIONS.md */
static inline void peopleBiosSetAnim(PeopleEntry* entry, u8 animationId) {
    void* model = entry->modelHandle;

    if (model != NULL) {
        entry->animId = animationId;
        GSmodelSetVisibility(model, animationId);
    }
}

void peopleBiosPopData(u8* src, u32 size) {
    PeopleEntry* entry;
    u8* saved;
    u32 n;
    void* model;
    u32 count;
    PeopleEntry* restored[64];
    u32 k;
    u32 j;

    memset(gPeopleArray, 0, gPeopleMaxCount * PEOPLE_ENTRY_SIZE);
    count = size / PEOPLE_SPAWN_DATA_SIZE;
    if (size % PEOPLE_SPAWN_DATA_SIZE != 0) {
        return;
    }
    if (count > 64) {
        return;
    }

    saved = src;
    n = count;
    k = 0;
    while (n--) {
        entry = peopleBiosAllocEntry();
        peopleOpenSub(entry, *(u32*)(saved + 8), *(u32*)(saved + 0x0C),
                      *(u32*)(saved + 0x10));
        memcpy((u8*)entry + 0x20, saved, 0xBC);
        restored[k++] = entry;
        saved += PEOPLE_SPAWN_DATA_SIZE;
    }

    saved = src;
    k = 0;
    while (count--) {
        entry = restored[k++];
        model = entry->modelHandle;
        peopleBiosSetAnim(entry, entry->animId);
        {
            u8 shadowAnimId = entry->shadowAnimId;
            s32 shadowId = entry->shadowId;

            if (shadowId >= 0) {
                GScolsys2HumanEnable(shadowId, shadowAnimId);
            }
        }
        peopleSetMotionSlot(entry, *(u32*)((u8*)entry + 0x90));
        GSmodelPopState(model, saved + 0xEC);

        if (*(s32*)((u8*)entry + 0xC8) != -1 && *(s32*)((u8*)entry + 0xCC) != -1) {
            fn_801848D0(model, *(s32*)((u8*)entry + 0xC8), *(s32*)((u8*)entry + 0xCC),
                        *(s32*)((u8*)entry + 0xD0));
            GSvecCopy(GSmodelGetPositionPtr(model), saved + 0xBC);
            GSvecCopy(GSmodelGetRotationPtr(model), saved + 0xC8);
            GSmodelSetPosition(model, saved + 0xD4);
            GSmodelSetRotation(model, saved + 0xE0);
        }

        switch (((u8*)entry)[0x96]) {
        case 0:
            std067_characterStopLookAt(entry->groupId, entry->index);
            break;
        case 1:
            std181_characterTurnHeadTowardsCharacter(entry->groupId, entry->index, *(u32*)((u8*)entry + 0xC0),
                        *(u32*)((u8*)entry + 0xC4));
            break;
        case 2:
            std066_characterLookAtPosition(entry->groupId, entry->index, (s32)*(f32*)((u8*)entry + 0xA8),
                        (s32)*(f32*)((u8*)entry + 0xAC), (s32)*(f32*)((u8*)entry + 0xB0));
            break;
        }
        if (((u8*)entry)[0x96] != 0) {
            for (j = 0; j < 60; j++) {
                fn_801812C4(entry);
            }
        }
        saved += PEOPLE_SPAWN_DATA_SIZE;
    }
}
