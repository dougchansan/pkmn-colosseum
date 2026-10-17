/* GS camera: load the active camera's matrices, 0x800D2248 - 0x800D2584. */
#include "dolphin/types.h"
#include "game/gs_render_util.h"
#include "game/gs_camera_build_matrix.h"

extern u32 lbl_8047AA74; /* active camera */
extern char lbl_8047C9C4[] __attribute__((section(".sdata2"))); /* "cobj.h" */
extern char lbl_8047C9CC[] __attribute__((section(".sdata2"))); /* "cobj" */

extern void __assert(const char* file, u32 line, const char* msg);
extern void PSMTXCopy(void* src, void* dst);
extern void HSD_CObjGetPerspective(void* cobj, f32* fovy, f32* aspect);
extern f32 HSD_CObjGetNear(void* cobj);
extern f32 HSD_CObjGetFar(void* cobj);
extern void HSD_CObjGetOrtho(void* cobj, f32* top, f32* bottom, f32* left, f32* right);
extern void gfxMultMatrix(void* mtx);
extern void gfxLoadIdentity(void);
extern void gfxSetPerspective(f32 fovy, f32 aspect, f32 near, f32 far);
extern void gfxSetOrtho(f32 a, f32 b, f32 c, f32 d);

void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void) {
    GSRenderCamera* c = (GSRenderCamera*)lbl_8047AA74;
    void* cobj;
    /* One set of four projection values for both paths: retail gives them a
     * single set of stack slots (0x14/0x10/0xC/0x8), and because the ortho
     * path takes all four addresses the perspective path stores near/far to
     * those slots as well. */
    f32 p0, p1, p2, p3;

    if (c == NULL) {
        return;
    }
    if (c->isAnimating != 0) {
        cobj = c->cobj;
        if (cobj == NULL) {
            __assert(lbl_8047C9C4, 0x1ae, lbl_8047C9CC);
        }
        *(u32*)((u8*)cobj + 0x8) &= ~0x2u;
        HSD_CObjGetPerspective(c->cobj, &p0, &p1);
        p2 = HSD_CObjGetNear(c->cobj);
        p3 = HSD_CObjGetFar(c->cobj);
        gfxSetPerspective(p0, p1, p2, p3);
        fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
        gfxLoadIdentity();
        gfxMultMatrix(c->viewMtx);
        HSD_CObjGetEyePosition(c->cobj, &c->eye);
        HSD_CObjGetUpVector(c->cobj, &c->upVector);
        HSD_CObjGetInterest(c->cobj, &c->interest);
    } else if (*(u8*)((u8*)c->cobj + 0x50) == 1) {
        if (c->dirty != 0) {
            cameraBuildMatrix(c);
        }
        cobj = c->cobj;
        if (cobj == NULL) {
            __assert(lbl_8047C9C4, 0x1a2, lbl_8047C9CC);
        }
        *(u32*)((u8*)cobj + 0x8) |= 0x80000002u;
        PSMTXCopy(c->viewMtx, (u8*)cobj + 0x54);
        HSD_CObjGetPerspective(c->cobj, &p0, &p1);
        p2 = HSD_CObjGetNear(c->cobj);
        p3 = HSD_CObjGetFar(c->cobj);
        gfxSetPerspective(p0, p1, p2, p3);
        gfxLoadIdentity();
        gfxMultMatrix(c->viewMtx);
    } else {
        HSD_CObjGetOrtho(c->cobj, &p1, &p3, &p0, &p2);
        gfxSetOrtho(p0, p1, p2, p3);
        gfxLoadIdentity();
    }
}
