/**
 * @file gs_render_util_candidate_800D27FC.c
 * @brief GScamera pool: create from a descriptor, create with defaults, and
 *        pool initialisation.
 *
 * Address range: 0x800D27FC - 0x800D2B44.
 */

#include "dolphin/types.h"
#include "game/gs_render_util.h"
#include "hsd/hsd_cobj.h"
#include "hsd/hsd_wobj.h"

extern void GSvecCopy(void* dst, const void* src);
extern void HSD_ForeachAnim(void* obj, u32 type, u32 mask, void (*cb)(void*), ...);
extern u32 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u32 handle);
extern void _GScameraAnimCallback(void* aobj);

extern u16 lbl_8047AA68;             /* camera pool GSmem handle */
extern GSRenderCamera* lbl_8047AA6C; /* camera pool */
extern u32 lbl_8047AA70;             /* camera pool size */
extern f32 lbl_8047AA78;             /* animation end frame scratch */
extern const f32 lbl_8047C990;             /* 1.0f */
extern const f32 lbl_8047C994;
extern const f32 lbl_8047C998;             /* 0.0f */
extern const f32 lbl_8047C9C0;
extern const f32 lbl_8047C9D4;
extern const f32 lbl_8047C9D8;

static inline GSRenderCamera* cameraFindFree(void)
{
    GSRenderCamera* camera = lbl_8047AA6C;
    u32 i;

    for (i = 0; i < lbl_8047AA70; i++, camera++) {
        if (camera->active == 0) {
            return camera;
        }
    }
    return NULL;
}

/*
 * Same body as GScameraSetAnimIndex (0x800D1984), which the target expands
 * here and in GScameraRestoreState; the unsigned index/count compare against the
 * constant 0 survives in the expansion.
 */
static inline void cameraSetAnimIndex(GSRenderCamera* camera, u32 animIndex)
{
    if (camera->hasAnimation != 0) {
        HSD_CObjRemoveAnim(camera->cobj);
        if (animIndex <= camera->animCount) {
            camera->animIndex = animIndex;
            HSD_CObjAddAnim(camera->cobj, camera->desc->animations[camera->animIndex]);
            HSD_CObjReqAnim(camera->cobj, lbl_8047C998);
            lbl_8047AA78 = lbl_8047C998;
            HSD_ForeachAnim(camera->cobj, 2, 0xFFFF, _GScameraAnimCallback, 0);
            camera->animEndFrame = lbl_8047AA78;
        }
    }
}

GSRenderCamera* GScameraCreateFromDesc(GSRenderCameraDesc* desc)
{
    GSRenderCamera* camera = cameraFindFree();

    if (camera == NULL) {
        return NULL;
    }

    camera->desc = desc;
    camera->cobj = HSD_CObjLoadDesc(camera->desc->cobjDesc);
    camera->active = 1;
    camera->isAnimating = 0;
    if (camera->desc->animations != NULL) {
        camera->hasAnimation = 1;
        camera->animRate = lbl_8047C990;
        camera->animMode = 1;
        camera->animEnded = 0;
        camera->animCount = 0;
        while (camera->desc->animations[camera->animCount] != NULL) {
            camera->animCount++;
        }
        cameraSetAnimIndex(camera, 0);
    } else {
        camera->hasAnimation = 0;
    }

    camera->active = 1;
    camera->useLookAt = 0;
    GSvecCopy(&camera->eye, &((HSD_CObj*)camera->cobj)->eyepos->pos);
    GSvecCopy(&camera->interest, &((HSD_CObj*)camera->cobj)->interest->pos);
    return camera;
}

GSRenderCamera* GScameraCreate(void)
{
    GSRenderCamera* camera = cameraFindFree();

    if (camera == NULL) {
        return NULL;
    }

    camera->eyeDesc.className = NULL;
    camera->eyeDesc.pos.x = lbl_8047C998;
    camera->eyeDesc.pos.y = lbl_8047C998;
    camera->eyeDesc.pos.z = lbl_8047C998;
    camera->eyeDesc.robjDesc = NULL;
    camera->interestDesc.className = NULL;
    camera->interestDesc.pos.x = lbl_8047C998;
    camera->interestDesc.pos.y = lbl_8047C998;
    camera->interestDesc.pos.z = lbl_8047C990;
    camera->interestDesc.robjDesc = NULL;
    camera->cobjDesc.className = NULL;
    camera->cobjDesc.flags = 0;
    camera->cobjDesc.projectionType = 1;
    camera->cobjDesc.viewportLeft = 0;
    camera->cobjDesc.viewportRight = 640;
    camera->cobjDesc.viewportTop = 0;
    camera->cobjDesc.viewportBottom = 480;
    camera->cobjDesc.scissorLeft = 0;
    camera->cobjDesc.scissorRight = 640;
    camera->cobjDesc.scissorTop = 0;
    camera->cobjDesc.scissorBottom = 480;
    camera->cobjDesc.eyeDesc = &camera->eyeDesc;
    camera->cobjDesc.interestDesc = &camera->interestDesc;
    camera->cobjDesc.roll = lbl_8047C998;
    camera->cobjDesc.upVector = NULL;
    camera->cobjDesc.nearZ = lbl_8047C994;
    camera->cobjDesc.farZ = lbl_8047C9C0;
    camera->cobjDesc.fov = lbl_8047C9D4;
    camera->cobjDesc.aspect = lbl_8047C9D8;
    camera->cobj = HSD_CObjLoadDesc((HSD_CObjDesc*)&camera->cobjDesc);
    camera->active = 1;
    camera->useLookAt = 0;
    camera->isAnimating = 0;
    camera->hasAnimation = 0;
    camera->desc = NULL;
    return camera;
}

void fn_800D2AD4(u32 count)
{
    u32 i;

    lbl_8047AA70 = count;
    lbl_8047AA68 = _toolentryAlloc__FUl(count * sizeof(GSRenderCamera));
    if (lbl_8047AA68 != 0) {
        lbl_8047AA6C = GSmemLock(lbl_8047AA68);
        for (i = 0; i < lbl_8047AA70; i++) {
            lbl_8047AA6C[i].active = 0;
        }
    }
}
