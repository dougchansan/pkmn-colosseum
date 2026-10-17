/* GS camera transform update, 0x800D258C - 0x800D2738. */
#include "dolphin/types.h"
#include "game/gs_render_util.h"

extern u32 lbl_8047AA74;
extern f32 lbl_80478ACC[];
/* RULE-EXCEPTION(title-path): volatile extern stands in for this TU's own
 * pooled zero literal so each absolute-value comparison reloads it; see
 * docs/RULE_EXCEPTIONS.md. */
extern volatile f32 lbl_8047C998;
extern double lbl_8047C9A0;

extern void fn_800E0628(void* dst, void* src);
extern void* HSD_CObjGetViewingMtxPtr(void* cobj);
extern void HSD_CObjGetEyePosition(void* cobj, void* data);
extern void HSD_CObjGetUpVector(void* cobj, void* data);
extern void HSD_CObjGetInterest(void* cobj, void* data);
extern void GSvecSub(void* dst, void* src1, void* src2);
extern void GSmtxLookAt(void* mtx, void* eye, void* up, void* interest);
extern void GSmtxMakeXRotation(void* mtx, f32 angle);
extern void GSmtxMakeYRotation(void* mtx, f32 angle);
extern void GSmtxMakeZRotation(void* mtx, f32 angle);
extern void fn_800E05C0(void* mtx, f32 x, f32 y, f32 z);
extern void GSmtxConcat(void* dst, void* lhs, void* rhs);

void fn_800D258C(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;
    lbl_8047AA74 = (u32)obj;
    if (c->isAnimating != 0) {
        fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
        HSD_CObjGetEyePosition(c->cobj, &c->eye);
        HSD_CObjGetUpVector(c->cobj, &c->upVector);
        HSD_CObjGetInterest(c->cobj, &c->interest);
    } else if (c->useLookAt == 1) {
        f32 tmp[3];
        GSvecSub(tmp, &c->eye, &c->interest);
        {
            f32 ax = tmp[0];
            ax = ax > lbl_8047C998 ? ax : -ax;
            if (ax < lbl_80478ACC[0]) {
                ax = tmp[1];
                ax = ax > lbl_8047C998 ? ax : -ax;
                if (ax < lbl_80478ACC[0]) {
                    ax = tmp[2];
                    ax = ax > lbl_8047C998 ? ax : -ax;
                    if (ax < lbl_80478ACC[0]) {
                        f32 v = c->interest.x;
                        c->interest.x = (f32)(v + lbl_8047C9A0);
                    }
                }
            }
        }
        GSmtxLookAt(c->viewMtx, &c->eye, &c->upVector, &c->interest);
    } else {
        f32 tmp2[3][4];
        f32 tmp1[3][4];
        f32 tmp3[3][4];
        GSmtxMakeXRotation(c->viewMtx, -c->rotation.x);
        GSmtxMakeYRotation(tmp1, -c->rotation.y);
        GSmtxMakeZRotation(tmp2, -c->rotation.z);
        fn_800E05C0(tmp3, -c->eye.x, -c->eye.y, -c->eye.z);
        GSmtxConcat(c->viewMtx, c->viewMtx, tmp1);
        GSmtxConcat(c->viewMtx, c->viewMtx, tmp2);
        GSmtxConcat(c->viewMtx, c->viewMtx, tmp3);
    }
    c->dirty = 0;
}
