#include "dolphin/types.h"

extern void HSD_LObjGetPosition(void* lobj, void* position);
extern void HSD_LObjGetInterest(void* lobj, void* interest);
extern void HSD_LObjRemoveAnimAll(void* lobj);
extern void HSD_LObjAddAnimAll(void* lobj, void* animation);
extern void HSD_LObjReqAnimAll(void* lobj, f32 frame);
extern void HSD_ForeachAnim(void* object, u32 type, u32 mask, void* function,
                            u32 argumentType, ...);
extern void HSD_AObjSetRate(void);
extern s32 GSgfxGetVsyncRate(void);
extern void lightGetFrameCount__FP9_HSD_AObj(u8* aobj);

extern f32 lbl_8047CA78;
extern f32 lbl_8047AAF4;
extern f32 lbl_8047CA88;

void GSlightPushState(u8* src, u8* dst)
{
    dst[0] = src[1];
    dst[1] = src[3];
    HSD_LObjGetPosition(*(void**)(src + 0xc), dst + 4);
    HSD_LObjGetInterest(*(void**)(src + 0xc), dst + 0x10);
    *(u32*)(dst + 0x1c) = *(u32*)(src + 0x60);
    *(f32*)(dst + 0x20) = *(f32*)(src + 0x68);
    *(f32*)(dst + 0x24) = *(f32*)(src + 0x64);
    *(u32*)(dst + 0x28) = *(u32*)(src + 0x5c);
    dst[2] = src[0x70];
    dst[3] = src[0x71];
}

u8 GSlightHasAnimationEnded(u8* object)
{
    return object[0x70];
}

void GSlightStopAnimation(u8* object)
{
    object[0x3] = 0;
}

void GSlightStartAnimation(u8* object)
{
    if (!object[0x2]) {
        return;
    }
    object[0x3] = 1;
    object[0x70] = 0;
    object[0x71] = 1;
}

void GSlightSetAnimType(u8* object, u32 type)
{
    *(u32*)(object + 0x5c) = type;
}

void GSlightSetAnimFrame(u8* object, f32 frame)
{
    if (object[0x2]) {
        *(f32*)(object + 0x68) = frame;
    }
}

void GSlightSetAnimRate(u8* object, f32 rate)
{
    if (!object[0x2]) {
        return;
    }
    if (GSgfxGetVsyncRate() == 0x32) {
        rate *= lbl_8047CA88;
    }
    *(f32*)(object + 0x64) = rate;
    HSD_ForeachAnim(*(void**)(object + 0xc), 7, 0xFFFF,
                    (void*)HSD_AObjSetRate, 1, *(f32*)(object + 0x64));
}

void GSlightSetAnimIndex(u8* object, u32 frame)
{
    u32 data;
    u32 currentFrame;
    u32 frames;

    if (!object[0x2]) {
        return;
    }
    HSD_LObjRemoveAnimAll(*(void**)(object + 0xc));
    if (frame > *(u32*)(object + 0x58)) {
        return;
    }
    *(u32*)(object + 0x60) = frame;
    data = *(u32*)(object + 0x8);
    currentFrame = *(u32*)(object + 0x60);
    frames = *(u32*)(data + 0x4);
    HSD_LObjAddAnimAll(*(void**)(object + 0xc),
                       *(void**)(frames + currentFrame * 4));
    HSD_LObjReqAnimAll(*(void**)(object + 0xc), lbl_8047CA78);
    lbl_8047AAF4 = lbl_8047CA78;
    HSD_ForeachAnim(*(void**)(object + 0xc), 7, 0xFFFF,
                    (void*)lightGetFrameCount__FP9_HSD_AObj, 0);
    *(f32*)(object + 0x6c) = lbl_8047AAF4;
}

u8 GSlightCanAnimate(u8* object)
{
    return object[0x2];
}

void GSlightSetActive(u8* object, u8 active)
{
    object[0x1] = active;
}
