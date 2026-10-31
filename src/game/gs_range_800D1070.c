/**
 * @file gs_range_800D1070.c
 * @brief GS camera per-frame update, 0x800D1070 - 0x800D13C4.
 *
 * Steps every active camera's animation, then rebuilds its view matrix with
 * the cameraBuildMatrix body (a fifth expansion; this one re-tests the
 * isAnimating byte the animation step already tested).
 */
#include "dolphin/types.h"
#include "game/gs_render_util.h"
#include "game/gs_camera_build_matrix.h"

extern u32 lbl_8047AA6C; /* camera array */
extern u32 lbl_8047AA70; /* camera count */
extern f32 lbl_8047C990;
extern f32 lbl_8047C994;
extern void GSvecCopy(void* dst, const void* src);
extern void HSD_CObjReqAnim(void* cobj, f32 frame);
extern void HSD_CObjAnim(void* cobj);

void cameraObjectAnimUpdate(u32 ticks)
{
    u32 offset;
    u32 i;
    GSRenderCamera* c;
    f32 step;
    f32 end;
    s32 mode;
    f32 last;

    for (offset = 0, i = 0; i < lbl_8047AA70; offset += sizeof(GSRenderCamera), i++) {
        c = (GSRenderCamera*)(lbl_8047AA6C + offset);
        if (c->active != 1) {
            continue;
        }
        if (c->isAnimating == 1) {
            GSvecCopy(&c->prevEye, &c->eye);
            HSD_CObjReqAnim(c->cobj, c->animFrame);
            HSD_CObjAnim(c->cobj);
            HSD_CObjGetEyePosition(c->cobj, &c->eye);
            HSD_CObjGetUpVector(c->cobj, &c->upVector);
            HSD_CObjGetInterest(c->cobj, &c->interest);

            step = c->animRate * (f32)ticks;
            end = c->animEndFrame - lbl_8047C990;
            mode = c->animMode;
            if (c->animDirection == -1) {
                c->animFrame -= step;
            } else if (c->animDirection == 1) {
                c->animFrame += step;
            }

            switch (mode) {
            case 0:
                if (c->animFrame >= (last = end - lbl_8047C994)) {
                    c->animEnded = 1;
                    c->animDirection = 0;
                    c->animFrame = last;
                }
                break;
            case 1:
                if (c->animFrame >= end) {
                    c->animFrame -= end;
                }
                break;
            case 2:
                if (c->animFrame >= end) {
                    c->animDirection = -1;
                } else if (c->animFrame <= lbl_8047C998) {
                    c->animDirection = 1;
                }
                break;
            }
            c->dirty = 1;
        }
        cameraBuildMatrix(c);
    }
}
