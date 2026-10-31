/**
 * @file gs_light_candidate_800DC878.c
 * @brief GSlightPopState (0x800DC878 - 0x800DCA10): restore a light's
 *        position, interest, animation index/frame/rate and flags from a
 *        GSlightPushState snapshot.
 *
 * Function-boundary carve of gs_light.c, text only, GC/1.3 -O4,p.
 *
 * RULE-EXCEPTION(user-approved): extern named stand-ins for the TU's own pool
 * literals and a static inline copy of the TU's global GSlightSetAnimIndex -
 * see docs/RULE_EXCEPTIONS.md. The 0.0f frame and the 50 Hz rate factor are
 * lbl_8047CA78 / lbl_8047CA88 (sdata2_8047CA70.c / sdata2_8047CA88.c), which
 * the linked gs_light carves also read by name. Retail expands
 * GSlightSetAnimIndex (0x800DCB78, linked in gs_light_exact_800DCA10.c) here,
 * so the carve carries a static inline copy of its body.
 */
#include "dolphin/types.h"

extern void HSD_LObjSetPosition();
extern void HSD_LObjSetInterest();
extern void HSD_LObjRemoveAnimAll(void*);
extern void HSD_LObjAddAnimAll(void*, void*);
extern void HSD_LObjReqAnimAll(void*, f32);
extern void HSD_ForeachAnim(void*, u32, u32, void*, u32, ...);
extern s32 GSgfxGetVsyncRate(void);
extern void HSD_AObjSetRate(void);
extern void lightGetFrameCount__FP9_HSD_AObj(u8*);

/* RULE-EXCEPTION(user-approved): pool stand-ins - see docs/RULE_EXCEPTIONS.md */
extern f32 lbl_8047CA78;
extern f32 lbl_8047AAF4;
extern f32 lbl_8047CA88;

/* RULE-EXCEPTION(user-approved): static inline copy of GSlightSetAnimIndex - see docs/RULE_EXCEPTIONS.md */
static inline void GSlightSetAnimIndexInline(u8* obj, u32 frame)
{
    u32 data;
    u32 frames;

    if (!obj[2]) {
        return;
    }
    HSD_LObjRemoveAnimAll(*(void**)(obj + 0xc));
    if (frame > *(u32*)(obj + 0x58)) {
        return;
    }
    *(u32*)(obj + 0x60) = frame;
    data = *(u32*)(obj + 0x8);
    frames = *(u32*)(data + 0x4);
    HSD_LObjAddAnimAll(*(void**)(obj + 0xc),
                       *(void**)(frames + *(u32*)(obj + 0x60) * 4));
    HSD_LObjReqAnimAll(*(void**)(obj + 0xc), lbl_8047CA78);
    lbl_8047AAF4 = lbl_8047CA78;
    HSD_ForeachAnim(*(void**)(obj + 0xc), 7, 0xffff,
                    (void*)lightGetFrameCount__FP9_HSD_AObj, 0);
    *(f32*)(obj + 0x6c) = lbl_8047AAF4;
}

void GSlightPopState(u8* obj, u8* snapshot) {
    f32 frame;
    f32 speed;

    snapshot[0] = obj[1];
    HSD_LObjSetPosition(*(void**)(obj + 0xc), snapshot + 4);
    HSD_LObjSetInterest(*(void**)(obj + 0xc), snapshot + 0x10);

    GSlightSetAnimIndexInline(obj, *(u32*)(snapshot + 0x1c));
    frame = *(f32*)(snapshot + 0x20);
    if (obj[2]) {
        *(f32*)(obj + 0x68) = frame;
    }
    speed = *(f32*)(snapshot + 0x24);
    if (obj[2]) {
        if (GSgfxGetVsyncRate() == 0x32) {
            speed *= lbl_8047CA88;
        }
        *(f32*)(obj + 0x64) = speed;
        HSD_ForeachAnim(*(void**)(obj + 0xc), 7, 0xffff, (void*)HSD_AObjSetRate,
                        1, *(f32*)(obj + 0x64));
    }

    *(u32*)(obj + 0x5c) = *(u32*)(snapshot + 0x28);
    obj[0x70] = snapshot[2];
    obj[0x71] = snapshot[3];
    if (obj[3] != 0 && obj[2] != 0) {
        obj[3] = 1;
        obj[0x70] = 0;
        obj[0x71] = 1;
    }
}
