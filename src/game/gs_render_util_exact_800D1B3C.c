/* GS camera view/inverse matrix accessors, 0x800D1B3C - 0x800D1EB8. */
#include "dolphin/types.h"
#include "game/gs_render_util.h"
#include "game/gs_camera_build_matrix.h"

extern void GSmtxInverse(void* dst, void* src);

/* XD: GScameraGetInvMatrixPtr. */
void* GScameraGetInvMatrixPtr(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;

    if (c->dirty != 0) {
        cameraBuildMatrix(c);
    }
    GSmtxInverse(c->projectionMtx, c->viewMtx);
    return c->projectionMtx;
}

/* XD: GScameraGetMatrixPtr. */
void* GScameraGetMatrixPtr(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;

    if (c->dirty != 0) {
        cameraBuildMatrix(c);
    }
    return c->viewMtx;
}
