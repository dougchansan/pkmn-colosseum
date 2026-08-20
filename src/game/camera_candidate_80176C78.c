/** Candidate-only owner for 0x80176C78 - 0x80176F68. */
#include "src/game/camera.c"

void GScameraSetAnimIndex(void* camera, s32 index);
void GScameraSetAnimFrame(void* camera, f32 frame);
void fn_800D1858(void* camera, s32 loop);
void clear__5GSvecFv(void* vector);
extern const f32 lbl_8047D730;

void cameraPlayOffsetAnime(u32 groupId, u32 animationId, s32 frame, u8 loop) {
    CameraPadState* state;
    void* animation;
    void* camera;
    u8 previousMode;

    state = lbl_80478C40;
    if (state->animationGroup != 0 || state->animationId != 0) {
        animation = GSresGetResource(state->animationGroup, state->animationId);
        if (animation == NULL) {
            animation = fn_800F92D4(state->animationId);
        }
        ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
        ((CameraPadState*)lbl_80478C40)->animationId = 0;
        if (animation != NULL) {
            GScameraStopAnimation(animation);
        }
    }

    /* Already-in-mode short-circuits the store and reports the mode itself. */
    previousMode = ((CameraPadState*)lbl_80478C40)->mode;
    if (previousMode == 8) {
        previousMode = 8;
    } else {
        ((CameraPadState*)lbl_80478C40)->mode = 8;
    }
    ((CameraPadState*)lbl_80478C40)->animationGroup = groupId;
    ((CameraPadState*)lbl_80478C40)->animationId = animationId;
    ((CameraPadState*)lbl_80478C40)->flags[1] = previousMode;

    animation = cameraGetCurrentAnimation();
    if (animation == NULL) {
        return;
    }
    GScameraSetAnimIndex(animation, 0);
    if (loop != 0) {
        fn_800D1858(animation, 1);
    } else {
        fn_800D1858(animation, 0);
    }
    GScameraSetAnimRate(animation, lbl_8047D730);
    GScameraSetAnimFrame(animation, (f32)frame);
    GScameraStartAnimation(animation);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetPosition);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetRotation);
    set__5GSvecFfff(&((CameraPadState*)lbl_80478C40)->offsetScale,
                    lbl_8047D724, lbl_8047D724, lbl_8047D724);
    camera = GSresGetResource(0, 0);
    fn_800D258C(camera);
}

void cameraPlayAnime(u32 groupId, u32 animationId, s32 frame, u8 loop) {
    CameraPadState* state;
    void* animation;
    u8 previousMode;

    state = lbl_80478C40;
    if (state->animationGroup != 0 || state->animationId != 0) {
        animation = GSresGetResource(state->animationGroup, state->animationId);
        if (animation == NULL) {
            animation = fn_800F92D4(state->animationId);
        }
        ((CameraPadState*)lbl_80478C40)->animationGroup = 0;
        ((CameraPadState*)lbl_80478C40)->animationId = 0;
        if (animation != NULL) {
            GScameraStopAnimation(animation);
        }
    }

    /* Already-in-mode short-circuits the store and reports the mode itself. */
    previousMode = ((CameraPadState*)lbl_80478C40)->mode;
    if (previousMode == 4) {
        previousMode = 4;
    } else {
        ((CameraPadState*)lbl_80478C40)->mode = 4;
    }
    ((CameraPadState*)lbl_80478C40)->animationGroup = groupId;
    ((CameraPadState*)lbl_80478C40)->animationId = animationId;
    ((CameraPadState*)lbl_80478C40)->flags[1] = previousMode;

    state = lbl_80478C40;
    animation = GSresGetResource(state->animationGroup, state->animationId);
    if (animation == NULL) {
        animation = fn_800F92D4(state->animationId);
    }
    if (animation == NULL) {
        return;
    }
    GScameraSetAnimIndex(animation, 0);
    if (loop != 0) {
        fn_800D1858(animation, 1);
    } else {
        fn_800D1858(animation, 0);
    }
    GScameraSetAnimRate(animation, lbl_8047D730);
    GScameraSetAnimFrame(animation, (f32)frame);
    GScameraStartAnimation(animation);
    fn_800D258C(animation);
}
