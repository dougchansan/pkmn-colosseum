/** Exact carve 0x80179E04 - 0x80179F4C (_cameraMakeStateData, _cameraRestoreStateData). */
#include "game/camera_types.h"
#include "game/gs_render_util.h"

typedef struct CameraSaveData {
    /* 0x000 */ CameraPadState state;
    /* 0x0FC */ GSRenderCameraSnapshot camera;
} CameraSaveData;

extern void* lbl_80478C40;

extern void* GSresGetResource(u32 group, u32 id);
extern void* GSresFindById(u32 id);
extern void GScameraRestoreState(void* camera, void* snapshot);
extern void GScameraSaveState(void* camera, void* snapshot);
extern void fn_800D258C(void* camera);
extern void* memcpy(void* dst, const void* src, u32 n);

/* Retail expands this same lookup in both state save/restore functions. */
static inline GSRenderCamera* cameraGetCurrentAnimation(void)
{
    CameraPadState* state = lbl_80478C40;
    GSRenderCamera* animation = GSresGetResource(
        state->animationGroup, state->animationId);

    if (animation == NULL) {
        animation = GSresFindById(state->animationId);
    }
    return animation;
}

void _cameraRestoreStateData(void* data)
{
    CameraSaveData* save = data;
    GSRenderCamera* camera;

    memcpy(lbl_80478C40, &save->state, sizeof(CameraPadState));
    camera = GSresGetResource(0, 0);
    if (((CameraPadState*)lbl_80478C40)->mode == 4 ||
        ((CameraPadState*)lbl_80478C40)->mode == 8) {
        camera = cameraGetCurrentAnimation();
    }
    GScameraRestoreState(camera, &save->camera);
    fn_800D258C(camera);
}

void _cameraMakeStateData(void* data)
{
    CameraSaveData* save = data;
    GSRenderCamera* camera = GSresGetResource(0, 0);

    if (((CameraPadState*)lbl_80478C40)->mode == 4 ||
        ((CameraPadState*)lbl_80478C40)->mode == 8) {
        camera = cameraGetCurrentAnimation();
    }
    memcpy(&save->state, lbl_80478C40, sizeof(CameraPadState));
    GScameraSaveState(camera, &save->camera);
}
