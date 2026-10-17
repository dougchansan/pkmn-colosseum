/**
 * @file field_camera_exact_80117AE4.c
 * @brief Field camera render-to-texture model, 0x80117AE4 - 0x80117E58.
 *
 * The last three functions of the field_camera unit (0x8011711C -
 * 0x80117E58), carved at their function boundaries: selecting the floor's
 * render-to-texture entry (texture, model, animation), releasing it, and
 * drawing the model into the texture. Text-only: the state is .sbss/.sdata
 * words owned by the auto-generated data units and stays extern.
 *
 * Built with the unit's flags, which include -opt nopeephole (see
 * configure.py): retail keeps unfolded copies such as
 * "mr r0,r3; li r3,2; mr r31,r0" after GScameraGetActiveCamera and the
 * "bne L; b end" double branch of fn_80117D14's guard, which the peephole
 * pass removes.
 *
 * fn_80117AE4 returns without a value when the requested entry is already
 * selected; retail does the same (that path branches straight to the
 * epilogue with no instruction setting r3).
 */
#include "dolphin/types.h"

typedef struct FieldCameraTexEntry {
    /* 0x00 */ u16 width;
    /* 0x02 */ u16 height;
    /* 0x04 */ u32 id;
    /* 0x08 */ u32 textureRes;
    /* 0x0C */ u32 modelRes;
    /* 0x10 */ u32 animIndex;
    /* 0x14 */ u32 cameraRes;
} FieldCameraTexEntry;

extern u32 std093_getCurrentGroupID(void);
extern void* GSresGetResource(u32 group, u32 id);
extern void GSmodelResetTextureChange(void* model);
extern void GStextureFree(void* texture);
extern void GSmodelFree(void* model);
extern void* GStextureCreate(u16 width, u16 height, u32 format, u32 arg3,
                             u32 arg4);
extern void* floorOpenModel(u32 group, u32 id);
extern void GSmodelSetVisibility(void* model, u32 visible);
extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
extern void GSmodelSetAnimIndex(void* model, u32 index);
extern void GSmodelStartAnimation(void* model);
extern void GSmodelSetTextureChange(void* model, void* texture);

extern void fn_800EC134(void* model);
extern void* GScameraGetActiveCamera(void);
extern void fn_800D4604(s32 mode);
extern void fn_800D377C(s32 mode);
extern void GSgfxRenderLayer(void* texture, s32 arg1);
extern void fn_800D9B24(u16* x, u16* y, u16* width, u16* height);
extern void fn_800D9AF0(u16* x, u16* y, u16* width, u16* height);
extern void fn_800D258C(void* camera);
extern void gfxSetScissor(u16 x, u16 y, u16 width, u16 height);
extern void gfxSetViewport(u16 x, u16 y, u16 width, u16 height);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern void GSmodelDrawModel(void* model, u32 flags);
extern void GSgfxRenderEnd(void);

extern u32 lbl_80478B40;                  /* selected entry id */
extern FieldCameraTexEntry* lbl_8047AD80; /* entry table */
extern u32 lbl_8047AD84;                  /* entry count */
extern FieldCameraTexEntry* lbl_8047AD88; /* selected entry */
extern void* lbl_8047AD8C;                /* render texture */
extern void* lbl_8047AD90;                /* model */
extern void* lbl_8047AD94;                /* camera */

u8 std171_mapSetTextureChange(u32 id)
{
    u32 count;
    u8 found;

    if ((s32) lbl_80478B40 == (s32) id) {
        return;
    }

    if (lbl_8047AD88 != NULL) {
        GSmodelResetTextureChange(
            GSresGetResource(std093_getCurrentGroupID(), lbl_8047AD88->textureRes));
        if (lbl_8047AD8C != NULL) {
            GStextureFree(lbl_8047AD8C);
            lbl_8047AD8C = NULL;
        }
        if (lbl_8047AD90 != NULL) {
            GSmodelFree(lbl_8047AD90);
            lbl_8047AD90 = NULL;
        }
        lbl_8047AD94 = NULL;
        lbl_80478B40 = (u32) -1;
    }

    count = lbl_8047AD84;
    found = 0;
    lbl_8047AD88 = lbl_8047AD80;
    while (count != 0) {
        if (lbl_8047AD88->id == id) {
            found = 1;
            break;
        }
        lbl_8047AD88++;
        count--;
    }
    if (!found) {
        lbl_8047AD88 = NULL;
        return 0;
    }

    lbl_8047AD8C = GStextureCreate(lbl_8047AD88->width, lbl_8047AD88->height,
                                   0x44, 0, 0);
    if (lbl_8047AD8C == NULL) {
        lbl_8047AD88 = NULL;
        return 0;
    }
    lbl_8047AD90 = floorOpenModel(std093_getCurrentGroupID(), lbl_8047AD88->modelRes);
    GSmodelSetVisibility(lbl_8047AD90, 0);
    GSmodelLinkTexAnimToAnim(lbl_8047AD90, 1);
    GSmodelSetAnimIndex(lbl_8047AD90, lbl_8047AD88->animIndex);
    GSmodelStartAnimation(lbl_8047AD90);
    lbl_8047AD94 = GSresGetResource(std093_getCurrentGroupID(), lbl_8047AD88->cameraRes);
    GSmodelSetTextureChange(
        GSresGetResource(std093_getCurrentGroupID(), lbl_8047AD88->textureRes),
        lbl_8047AD8C);
    lbl_80478B40 = id;
    return 1;
}

void tvRelease(void)
{
    FieldCameraTexEntry* entry = lbl_8047AD88;

    if (entry != NULL) {
        GSmodelResetTextureChange(
            GSresGetResource(std093_getCurrentGroupID(), entry->textureRes));
        if (lbl_8047AD8C != NULL) {
            GStextureFree(lbl_8047AD8C);
            lbl_8047AD8C = NULL;
        }
        if (lbl_8047AD90 != NULL) {
            GSmodelFree(lbl_8047AD90);
            lbl_8047AD90 = NULL;
        }
        lbl_8047AD94 = NULL;
        lbl_8047AD88 = NULL;
        lbl_80478B40 = (u32) -1;
    }
    lbl_8047AD80 = NULL;
    lbl_8047AD84 = 0;
}

void tvRender(void)
{
    u16 viewportX;
    u16 viewportY;
    u16 viewportWidth;
    u16 viewportHeight;
    u16 scissorX;
    u16 scissorY;
    u16 scissorWidth;
    u16 scissorHeight;
    void* camera;

    if (lbl_8047AD88 == NULL || lbl_8047AD90 == NULL) {
        return;
    }
    fn_800EC134(lbl_8047AD90);
    camera = GScameraGetActiveCamera();
    fn_800D4604(2);
    fn_800D377C(1);
    GSgfxRenderLayer(lbl_8047AD8C, 0);
    fn_800D9B24(&viewportX, &viewportY, &viewportWidth, &viewportHeight);
    fn_800D9AF0(&scissorX, &scissorY, &scissorWidth, &scissorHeight);
    fn_800D258C(lbl_8047AD94);
    gfxSetScissor(0, 0, lbl_8047AD88->width - 1, lbl_8047AD88->height - 1);
    gfxSetViewport(0, 0, lbl_8047AD88->width - 1, lbl_8047AD88->height - 1);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    GSmodelDrawModel(lbl_8047AD90, 0x3010);
    GSgfxRenderEnd();
    fn_800D377C(1);
    fn_800D258C(camera);
    gfxSetScissor(viewportX, viewportY, viewportWidth, viewportHeight);
    gfxSetViewport(scissorX, scissorY, scissorWidth, scissorHeight);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    fn_800D4604(1);
}
