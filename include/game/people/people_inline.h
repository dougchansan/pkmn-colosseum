/**
 * @file people_inline.h
 * @brief The people TU's lookup/turn/range/motion routines, as expanded inline.
 *
 * Each routine below is a real function of the people TU that retail also
 * exports on its own, and whose body MWCC expands at earlier call sites:
 *
 *   peopleFindSelf       fn_8018D998      (groupId, index) -> self pointer
 *   peopleFindBySelf     peopleSearchID   self pointer -> entry
 *   peopleStartTurn      fn_8018805C      start turning toward a yaw
 *   peopleCalcRange      fn_801887D8      proximity ratio of a displacement
 *   peopleSetMotion      fn_8018B76C      (re)start a body/texture motion
 *   peopleSetMotionIndex peopleSetMotionSlot      play a person's motion slot
 *
 * Standalone units that carve functions out of the TU use these static
 * inline copies so the object emits no extra symbols.
 */
#ifndef GAME_PEOPLE_PEOPLE_INLINE_H
#define GAME_PEOPLE_PEOPLE_INLINE_H

#include "game/people/people.h"

extern void GSlogWrite(const char* fmt, ...);
extern f32 vecMag(void* vector);
extern u8 GSmodelHasAnimationEnded(void* model);
extern u8 GSmodelIsAnimating(void* model);
extern void GSmodelGetAnimIndex(void* model, s32* current, s32* secondary);
extern void GSmodelSetAnimIndex(void* model, s32 index);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelSetAnimType(void* model, s32 type);
extern void GSmodelSetTexAnimIndex(void* model, s32 index);
extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
extern void GSmodelSetTexAnimRate(void* model, f32 rate);
extern void GSmodelStartAnimation(void* model);
extern void fn_8018F4C8(void* info, u8 motion, s32* outAnim, u8* outLoop);
extern void* peopleInfoBiosGetPtr(void* scriptObj);
extern void peopleGetRotation(PeopleEntry* entry, void* rotation);

/* "Warining: people[%d,%d] group is different!!\n" */
extern const char lbl_80273FD8[];

/*
 * Look a person up by (groupId, index); if no person of that group has the
 * index, fall back to any group and warn.
 */
static inline PeopleEntry* peopleFindSelf(u32 groupId, u32 index)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        return entry->selfPtr;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite(lbl_80273FD8, groupId, index);
        return entry->selfPtr;
    }
    return NULL;
}

static inline PeopleEntry* peopleFindBySelf(PeopleEntry* found)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        return entry;
    }
    return NULL;
}

/* Start turning toward a yaw expressed in the model's current revolution. */
static inline void peopleStartTurn(u32 groupId, u32 index, f32 yaw, f32 speed)
{
    PeopleEntry* entry;
    f32 rotation[3];

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleGetRotation(entry, rotation);
        yaw += 6.2831855f * (s32)(rotation[1] / 6.2831855f);
        entry->pad22 = 1;
        entry->field_40 = yaw;
        entry->field_44 = speed;
    }
}

/*
 * Proximity ratio of a displacement against a person's near/far distances
 * (field_34/field_38): 0..1 inside near, 1..2 between near and far, 2 beyond.
 */
static inline f32 peopleCalcRange(u32 groupId, u32 index, void* delta)
{
    PeopleEntry* entry;
    f32 result;
    f32 t;

    result = 0.0f;
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0.0f;
    }
    t = vecMag(delta);
    if (entry->field_38 <= t) {
        result = 2.0f;
    } else if (entry->field_34 <= t) {
        if (entry->field_38 != 0.0f) {
            result = 1.0f + (t - entry->field_34) / (entry->field_38 - entry->field_34);
        }
    } else if (entry->field_34 != 0.0f) {
        result = t / entry->field_34;
    }
    return result;
}

/*
 * Start a person's body/texture animation unless it already plays that
 * motion unblended, then set the loop mode.
 */
static inline u8 peopleSetMotion(u32 groupId, u32 index, s32 animIndex,
                                 s32 frame, u8 loop)
{
    PeopleEntry* entry;
    void* model;
    s32 current;
    s32 secondary;
    u8 restart = 0;

    if (animIndex < 0) {
        return 0;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return 0;
    }
    if (GSmodelHasAnimationEnded(model)) {
        restart = 1;
    } else if (!GSmodelIsAnimating(model)) {
        restart = 1;
    } else {
        GSmodelGetAnimIndex(model, &current, &secondary);
        if (current != animIndex || secondary != -1) {
            restart = 1;
        }
    }
    if (restart) {
        entry->walkTargetNode = animIndex;
        entry->walkAnimRate = 0.0f;
        GSmodelSetAnimIndex(model, animIndex);
        GSmodelSetAnimFrame(model, frame);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelSetTexAnimIndex(model, animIndex);
        GSmodelSetTexAnimFrame(model, frame);
        GSmodelSetTexAnimRate(model, 0.5f);
        if (loop) {
            GSmodelSetAnimType(model, 1);
        } else {
            GSmodelSetAnimType(model, 0);
        }
        GSmodelStartAnimation(model);
    }
    if (loop) {
        GSmodelSetAnimType(model, 1);
    } else {
        GSmodelSetAnimType(model, 0);
    }
    return restart;
}

/* Select a person's motion slot and play the animation its info maps it to. */
static inline void peopleSetMotionIndex(PeopleEntry* entry, s32 motionIndex)
{
    PeopleInfoBiosEntry* info;
    s32 animIndex;
    u8 loop;

    entry->motionIndex = motionIndex;
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    fn_8018F4C8(info, (u8)entry->motionIndex, &animIndex, &loop);
    if (animIndex == -1) {
        return;
    }
    peopleSetMotion(entry->groupId, entry->index, animIndex, 0, loop);
}

#endif /* GAME_PEOPLE_PEOPLE_INLINE_H */
