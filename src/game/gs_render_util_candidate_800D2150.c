/**
 * GS camera scissor, 0x800D2150 - 0x800D21C8.
 *
 * The rectangle arrives as u16 (gfxSetScissor passes its u16 scissor corners)
 * and HSD_CObjSetScissorx4 takes u16 edges, so the clamped values are passed
 * through without re-extension.
 */
#include "dolphin/types.h"
#include "game/gs_render_util.h"
#include "hsd/hsd_cobj.h"

void GScameraSetScissor(GSRenderCamera* camera, u16 x0, u16 y0, u16 x1, u16 y1)
{
    if (x0 > 0x27E) {
        x0 = 0x27E;
    }
    if (y0 > 0x1DE) {
        y0 = 0x1DE;
    }
    if (x1 > 0x27F) {
        x1 = 0x27F;
    }
    if (y1 > 0x1DF) {
        y1 = 0x1DF;
    }
    HSD_CObjSetScissorx4((HSD_CObj*) camera->cobj, x0, x1 + 1, y0, y1 + 1);
}
