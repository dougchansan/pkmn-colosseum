/**
 * @file gs_model_state.c
 * @brief GSmodel push/pop state (XD parse-TU tail 0x800FC9D0-0x800FCD04)
 *
 * Split from gs_range_800E202C.c (0x800E9B2C-0x800E9E34) — one XD source unit per
 * segment (Fable re-split, 2026-07-07). Functions asm-only until matched.
 */
#define GSMODEL_SUFFIX_ISOLATED
#include "src/game/gs_model_main_suffix_800E4AC0.c"

typedef struct GSmodelState {
    u32 flags;
    GSvec position;
    GSvec rotation;
    GSvec scale;
    u32 animIndex;
    u32 texAnimIndex;
    f32 animFrame;
    f32 animRate;
    f32 texAnimFrame;
    f32 texAnimRate;
    u32 animType;
    u32 texAnimType;
    GSvec attachedPosition;
    GSvec attachedRotation;
    GSvec attachedScale;
} GSmodelState;

extern void GSmodelSetPosition(GSmodel*, const GSvec*);
extern void GSmodelSetRotation(GSmodel*, const GSvec*);
extern void GSmodelSetScale(GSmodel*, const GSvec*);
extern void GSmodelSetAttachTransform(GSmodel*, const GSvec*, const GSvec*,
                                      const GSvec*);
extern void GSmodelSetAnimIndex(GSmodel*, u32);
extern void GSmodelSetAnimFrame(GSmodel*, f32);
extern void GSmodelSetAnimRate(GSmodel*, f32);
extern void GSmodelSetAnimType(GSmodel*, u32);
extern void GSmodelSetTexAnimIndex(GSmodel*, u32);
extern void GSmodelSetTexAnimFrame(GSmodel*, f32);
extern void GSmodelSetTexAnimRate(GSmodel*, f32);
extern void GSmodelSetTexAnimType(GSmodel*, u32);
extern void GSmodelStartAnim(GSmodel*);
extern void GSmodelStartTexAnim(GSmodel*);
extern void GSmodelUpdateAnim(GSmodel*);
extern GSvec* GSmodelGetPosition(GSmodel*);
extern GSvec* GSmodelGetRotation(GSmodel*);
extern GSvec* GSmodelGetScale(GSmodel*);
extern void fn_800E01D0(GSvec*, const GSvec*);

void GSmodelPopState(GSmodel* model, const GSmodelState* state)
{
    model->flags.raw =
        (model->flags.raw & 0xFFCDD49D) | state->flags;
    GSmodelSetPosition(model, &state->position);
    GSmodelSetRotation(model, &state->rotation);
    GSmodelSetScale(model, &state->scale);
    if (state->flags & GSMODEL_FLAG_ROOT_NULL_ADDED) {
        model->flags.raw ^= GSMODEL_FLAG_ROOT_NULL_ADDED;
        GSmodelSetAttachTransform(model, &state->attachedPosition,
                                  &state->attachedRotation,
                                  &state->attachedScale);
    }
    model->animIndex = -1;
    model->texAnimIndex = -1;
    GSmodelSetAnimIndex(model, state->animIndex);
    GSmodelSetAnimFrame(model, state->animFrame);
    GSmodelSetAnimRate(model, state->animRate);
    GSmodelSetAnimType(model, state->animType);
    GSmodelSetTexAnimIndex(model, state->texAnimIndex);
    GSmodelSetTexAnimFrame(model, state->texAnimFrame);
    GSmodelSetTexAnimRate(model, state->texAnimRate);
    GSmodelSetTexAnimType(model, state->texAnimType);
    if (state->flags & 0x20) {
        GSmodelStartAnim(model);
    }
    if (state->flags & 0x40) {
        GSmodelStartTexAnim(model);
    }
    GSmodelUpdateAnim(model);
}

void GSmodelPushState(GSmodel* model, GSmodelState* state)
{
    HSDJObj* attached;

    state->flags = model->flags.raw & 0x00322B62;
    fn_800E01D0(&state->position, GSmodelGetPosition(model));
    fn_800E01D0(&state->rotation, GSmodelGetRotation(model));
    fn_800E01D0(&state->scale, GSmodelGetScale(model));
    state->animIndex = model->animIndex;
    state->texAnimIndex = model->texAnimIndex;
    state->animFrame = model->animFrame;
    state->animRate = model->animRate;
    state->animType = model->animType;
    state->texAnimFrame = model->texAnimFrame;
    state->texAnimRate = model->texAnimRate;
    state->texAnimType = model->texAnimType;
    if (state->flags & GSMODEL_FLAG_ROOT_NULL_ADDED) {
        attached = model->renderJObj->child;
        state->attachedPosition = attached->translate;
        state->attachedRotation = attached->rotation;
        state->attachedScale = attached->scale;
    }
}
