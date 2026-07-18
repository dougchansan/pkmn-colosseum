/**
 * @file gbaCommunication_exact_80090720.c
 * @brief Matching gbaCommunication range, 0x80090720 - 0x80091DA4
 *        (8 functions).
 */
#include "dolphin/types.h"
#include "game/gs_material.h"

#define GBA_DATA_OFFSET 0x20
#define GBA_STATE_PORT 0x4338
#define GBA_STATE_TIMEOUT 0x433C
#define GBA_STATE_PHASE 0x4340
#define GBA_THREAD_PRIORITY 8

extern u32 lbl_8047A690;
extern u32 lbl_8047A694;
extern f32 lbl_8047C1D0; /* 0.833333313f -- PAL-adjusted 1-unit wait */
extern f32 lbl_8047C1D4; /* 0.0f */
extern f32 lbl_8047C1D8; /* 1.0f */
extern f32 lbl_8047C1DC; /* 83.3333282f -- PAL-adjusted 100-unit wait */
extern f32 lbl_8047C1E0; /* {41.6666641f, 0.0f} -- PAL-adjusted 50-unit wait */

/* Additional data labels referenced by the ported gba_comm_ext.c /
 * late_game.c bodies below (GBA link-cable state machine + battle-status
 * window helpers living in this same address range per the current
 * object map). */
extern u8 lbl_803FB328[];
extern u8 lbl_803FB338[];
extern u8 lbl_803FB380[];
extern u8 lbl_8047C1E8;
extern u8 lbl_8026F5A8[];
extern u8 lbl_8026F5C0[];
extern u8 lbl_80314F98[];
extern u16 lbl_802EED28[];

/* Common callees needed by the ported bodies below that are not already
 * declared with a full prototype at the point of use. */
extern void _threadSwitch(void);
extern void* windowSearchID(u32 id);
extern void fn_8009F7B4(void *p);
extern void fn_8009F890(void *p);
extern void fn_800A257C(void *p, u32 b);
extern void fn_800716E8(u32 port, u32 val);
extern void fn_8009FABC(void *p);
extern void fn_800A1E54(void *p, u32 v);
extern void fn_800716C8(u32 port, void *a, void *b);
extern u32 fn_800E202C(void *p);
extern void __assert(const u8 *file, u32 line, const u8 *msg);
extern void fn_800E24B0(u32 status);
extern void fn_800E209C(u32 status);
extern u32 fn_800A13F8(void);
extern void OSYieldThread(void);
extern void fn_800FF730(u32 id);
extern void floorSetFadeScript(u32 a, u32 b);
extern u32 GSresGetResource(u32 ctx, u32 id);

/* Storage used by the battle-status window renderer below. */
extern u8 lbl_8047C200;
extern u8 lbl_8047C204;
extern f32 lbl_8047C208;
extern u8 lbl_8047C20C;
extern u8 lbl_8047C210;
extern u8 lbl_8047C214;
extern u8 lbl_8047C218;
extern u8 lbl_8047C21C;
extern u8 lbl_8047C220;
extern u8 lbl_8047C228;
extern f32 lbl_8047C230;
extern f32 lbl_8047C234;
extern f32 lbl_8047C238;
extern void fn_801040F0();
extern void winSpriteSetDisp();
extern void fn_8001E58C();
extern void fn_800FA280();
extern void fn_800FA444();
extern void fn_800FB680();
extern void fn_800FB8C8();
extern void fn_800FBB34();

extern u16* windowGetKeyInfo(void);
extern void* pokemonDataBiosGetPtr(u32 id);
extern u8 pokemonBiosGetCatchBallId(void* pokemon);
extern u16 pokemonGetSoubiItemDataId(void* pokemon);
extern u32 pokemonDataBiosGetName(void* bios);
extern u32 GSmsgGetGSchar(u32 id);
extern u32 GSmsgGetRect(u32 id);
extern void msgctrlSetValue(u32 id, u32 value);
extern void windowDrawSprite(s32 x, s32 y, void* win, u16 sprite, u32 data);
extern void windowDrawSprite2(s32 x, s32 y, s16 w, s16 h, s32 color, s32 data, s32 sprite, s32 arg7);
extern void* menuModelRender(void* data);
extern void fn_800D88DC(u32 arg);
extern void fn_800D888C(u32 arg);
extern void fn_800D6A00(u32 arg);
extern void fn_800D7820(void* arg);
extern void fn_800D85D4(u32 arg0, void* arg1);
extern void fn_800D67BC(u32 arg);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D5CB8(u32 arg0, u32 r, u32 g, u32 b, u32 a);
extern void fn_800D59B8(u32 arg0, f32 s, f32 t);
extern void fn_800D6728(void);
extern u32 fn_8001D624(void* pokemon, u32 arg);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);

static inline u32 fn_80090720_getHandle2(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    return GSresGetResource(ctx, 0x0CE61004);
}

/* Retail marks every function in this physical island active. */
#pragma force_active on

/* 0x80090720 | size: 0x2C4 */
void fn_80090720(u32 ctx) {
    #pragma peephole off
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern void GSmodelSetAnimIndex(u32 handle, u32 val);
    extern void GSmodelGetFrameCount(u32 handle, f32 *out, u32 flag);
    extern void GSmodelSetAnimFrame(u32 handle, f32 val);
    extern void GSmodelSetAnimType(u32 handle, u32 val);
    extern void GSmodelStartAnimation(u32 handle);

    u32 waitFrames;
    u32 elapsed;
    GSmaterialEntry *material;
    f32 frame;
    u32 new_var;
    u32 handle2;
    u32 iconHandle;
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    frame = lbl_8047C1D4;
    handle2 = fn_80090720_getHandle2(ctx);
    GSmodelSetAnimIndex(handle2, 1);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 1);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    fn_801CB834(0x0CE61000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    fn_801CB7C4(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    iconHandle = fn_801CBA0C(0x06BC0400);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF21800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x0CE91000);
    new_var = iconHandle;
    fn_801845E4(ctx, new_var, ctx, finalResult, 0);
    fn_801CB834(new_var, 1, 0, 0);
    fn_801CB834(0x0CE61004, 0, 0, 0);
    scriptWaitSyncMotion(iconHandle, 1);
    fn_801CB834(iconHandle, 2, 0, 0);
    scriptWaitSyncMotion(new_var, 1);
    fn_801CB834(new_var, 3, 0, 0);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x800909E4 | size: 0x350 */
void fn_800909E4(u32 ctx) {
    #pragma peephole off
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void GSmodelSetAnimIndex(u32 handle, u32 val);
    extern void GSmodelGetFrameCount(u32 handle, f32 *out, u32 flag);
    extern void GSmodelSetAnimFrame(u32 handle, f32 val);
    extern void GSmodelSetAnimType(u32 handle, u32 val);
    extern void GSmodelStartAnimation(u32 handle);

    u32 waitFrames;
    u32 elapsed;
    f32 frame;
    u32 handle2;
    u32 iconHandle;
    u32 iconHandle2;
    u32 iconResult;
    u32 finalResult;
    u32 finalResult2;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    frame = lbl_8047C1D4;
    handle2 = fn_80090720_getHandle2(ctx);
    GSmodelSetAnimIndex(handle2, 1);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 1);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    fn_801CB834(0x0CE61000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    fn_801CB7C4(0x0CE61000);
    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x111B1400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);
    fn_801CB834(0x0CE61000, 3, 0, 0);

    waitFrames = 0x32;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1E0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    iconHandle = fn_801CBA0C(0x06BD0400);
    iconHandle2 = fn_801CBA0C(0x06BA0400);

    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);
    iconResult = GSresGetResource(ctx, iconHandle2);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF11800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x0CE81001);
    finalResult2 = fn_801CBA0C(0x0CE81000);
    fn_801845E4(ctx, iconHandle, ctx, finalResult, 0);
    fn_801845E4(ctx, iconHandle2, ctx, finalResult2, 0);
    fn_801CB834(iconHandle, 2, 0, 1);
    fn_801CB834(iconHandle2, 2, 0, 1);
    cameraWaitSyncAnime(1);
    fn_800FF58C(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x80090D34 | size: 0x2D8 */
void fn_80090D34(u32 ctx) {
    #pragma peephole off
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void fn_80118874(void *texture, u32 flag);
    extern void GSmodelSetAnimIndex(u32 handle, u32 val);
    extern void GSmodelGetFrameCount(u32 handle, f32 *out, u32 flag);
    extern void GSmodelSetAnimFrame(u32 handle, f32 val);
    extern void GSmodelSetAnimType(u32 handle, u32 val);
    extern void GSmodelStartAnimation(u32 handle);

    u32 waitFrames;
    u32 elapsed;
    GSmaterialEntry *material;
    f32 frame;
    u32 handle2;
    u32 iconHandle;
    u32 iconHandle2;
    u32 iconResult;
    u32 finalResult;
    u32 finalResult2;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    frame = lbl_8047C1D4;
    handle2 = fn_80090720_getHandle2(ctx);
    GSmodelSetAnimIndex(handle2, 1);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 1);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    fn_801CB834(0x0CE61000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    fn_801CB7C4(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    iconHandle = fn_801CBA0C(0x06BD0400);
    iconHandle2 = fn_801CBA0C(0x06BA0400);

    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    iconResult = GSresGetResource(ctx, iconHandle2);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF01800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x0CE71001);
    finalResult2 = fn_801CBA0C(0x0CE71000);
    fn_801845E4(ctx, iconHandle, ctx, finalResult, 0);
    fn_801845E4(ctx, iconHandle2, ctx, finalResult2, 0);
    fn_801CB834(iconHandle, 2, 0, 1);
    fn_801CB834(iconHandle2, 2, 0, 1);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x89);
    floorSetFadeScript(0, 0);
}
#pragma push
#define SET_SCENE_SHADOW(model_)                                              \
    configured = GSresGetResource(ctx, (model_));                            \
    GSmodelSetShadowFlags(configured, 2);                                    \
    GSmodelSetShadowLight(configured, lbl_8047A690);                         \
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694)

/* 0x8009100C | size: 0x558 */
void fn_8009100C(u32 ctx) {
    #pragma peephole off
    extern void GSmodelSetShadowTextureSize(u32, u32);
    extern u32 fn_801CBA0C(u32);
    extern u32 GSmodelSetShadowFlags(u32, u32);
    extern void GSmodelSetShadowLight(u32, u32);
    extern void GSmodelSetShadowSurface(u32, u32, u32*);
    extern void cameraPlayAnime(u32, u32, u32, u32);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void fn_801845E4(u32, u32, u32, u32, u32);
    extern void fn_801CB834(u32, u32, u32, u32);
    extern void cameraWaitSyncAnime(s32);
    extern void fn_800FF58C(u32);
    u32 animation0, animation1, animation2, animation3, animation4;
    u32 animation5, animation6, animation7, animation8;
    u32 model0, model1, model2, model3, model4;
    u32 model5, model6, model7, model8;
    u32 configured, elapsed, waitFrames;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DB1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DB1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    model0 = fn_801CBA0C(0x06BC0400);
    model1 = fn_801CBA0C(0x06BE0400);
    model2 = fn_801CBA0C(0x0D240400);
    model3 = fn_801CBA0C(0x0D290400);
    model4 = fn_801CBA0C(0x06BE0400);
    model5 = fn_801CBA0C(0x0D240400);
    model6 = fn_801CBA0C(0x0D290400);
    model7 = fn_801CBA0C(0x06BE0400);
    model8 = fn_801CBA0C(0x06BE0400);
    SET_SCENE_SHADOW(model0);
    SET_SCENE_SHADOW(model1);
    SET_SCENE_SHADOW(model2);
    SET_SCENE_SHADOW(model3);
    SET_SCENE_SHADOW(model4);
    SET_SCENE_SHADOW(model5);
    SET_SCENE_SHADOW(model6);
    SET_SCENE_SHADOW(model7);
    SET_SCENE_SHADOW(model8);

    cameraPlayAnime(ctx, 0x0C391800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames;) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    animation0 = fn_801CBA0C(0x0C381000);
    animation1 = fn_801CBA0C(0x0C381008);
    animation2 = fn_801CBA0C(0x0C381001);
    animation3 = fn_801CBA0C(0x0C381002);
    animation4 = fn_801CBA0C(0x0C381003);
    animation5 = fn_801CBA0C(0x0C381004);
    animation6 = fn_801CBA0C(0x0C381005);
    animation7 = fn_801CBA0C(0x0C381006);
    animation8 = fn_801CBA0C(0x0C381007);
    fn_801845E4(ctx, model0, ctx, animation0, 0);
    fn_801845E4(ctx, model1, ctx, animation1, 0);
    fn_801845E4(ctx, model2, ctx, animation2, 0);
    fn_801845E4(ctx, model3, ctx, animation3, 0);
    fn_801845E4(ctx, model4, ctx, animation4, 0);
    fn_801845E4(ctx, model5, ctx, animation5, 0);
    fn_801845E4(ctx, model6, ctx, animation6, 0);
    fn_801845E4(ctx, model7, ctx, animation7, 0);
    fn_801845E4(ctx, model8, ctx, animation8, 0);
    fn_801CB834(model0, 3, 0, 1);
    fn_801CB834(model1, 5, 0, 1);
    fn_801CB834(model2, 14, 0, 1);
    fn_801CB834(model3, 15, 0, 1);
    fn_801CB834(model4, 4, 0, 1);
    fn_801CB834(model5, 4, 0, 1);
    fn_801CB834(model6, 14, 0, 1);
    fn_801CB834(model7, 14, 0, 1);
    fn_801CB834(model8, 14, 0, 1);
    cameraWaitSyncAnime(1);
    fn_800FF58C(0x89);
    floorSetFadeScript(0, 0);
}

#undef SET_SCENE_SHADOW

#pragma pop
void fn_80091564(u32 ctx) {
    #pragma peephole off
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);

    u32 waitFrames;
    u32 elapsed;
    u32 iconHandle;
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DC1605);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DC1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB7C4(0x06DC1000);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x06DC1000), GSresGetResource(ctx, 0x11261400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x06DC1000), 4);

    fn_801CB834(0x06DC1000, 4, 0, 0);
    waitFrames = 100;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1DC;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    iconHandle = fn_801CBA0C(0x06BD0400);
    fn_801CB834(iconHandle, 7, 0, 0);

    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0C421800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x0C3D1000);
    fn_801845E4(ctx, iconHandle, ctx, finalResult, 0);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x81);
    floorSetFadeScript(0, 0);
}

/* 0x80091774 | size: 0x210 */
void fn_80091774(u32 ctx) {
    #pragma peephole off
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);

    u32 waitFrames;
    u32 elapsed;
    u32 iconHandle;
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DC1605);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DC1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB7C4(0x06DC1000);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x06DC1000), GSresGetResource(ctx, 0x11251400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x06DC1000), 4);

    fn_801CB834(0x06DC1000, 3, 0, 0);
    waitFrames = 100;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1DC;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    iconHandle = fn_801CBA0C(0x06BD0400);
    fn_801CB834(iconHandle, 6, 0, 0);

    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0C411800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x0C3C1000);
    fn_801845E4(ctx, iconHandle, ctx, finalResult, 0);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x82);
    floorSetFadeScript(0, 0);
}

/* 0x80091984 | size: 0x210 */
void fn_80091984(u32 ctx) {
    #pragma peephole off
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);

    u32 waitFrames;
    u32 elapsed;
    u32 iconHandle;
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DC1605);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DC1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB7C4(0x06DC1000);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x06DC1000), GSresGetResource(ctx, 0x11241400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x06DC1000), 4);

    fn_801CB834(0x06DC1000, 2, 0, 0);
    waitFrames = 100;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1DC;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    iconHandle = fn_801CBA0C(0x06BD0400);
    fn_801CB834(iconHandle, 5, 0, 1);

    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0C401800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x0C3B1000);
    fn_801845E4(ctx, iconHandle, ctx, finalResult, 0);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x82);
    floorSetFadeScript(0, 0);
}

/* 0x80091B94 | size: 0x210 */
void fn_80091B94(u32 ctx) {
    #pragma peephole off
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void fn_801CB7C4(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void fn_801CB834(u32 id, u32 slot, u32 x, u32 y);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 fn_800D3088(void);
    extern u32 fn_801CBA0C(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void fn_801845E4(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void fn_800FF58C(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);

    u32 waitFrames;
    u32 elapsed;
    u32 iconHandle;
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DC1605);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DC1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    fn_801CB7C4(0x06DC1000);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x06DC1000), GSresGetResource(ctx, 0x11221400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x06DC1000), 4);

    fn_801CB834(0x06DC1000, 0, 0, 0);
    waitFrames = 100;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1DC;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    iconHandle = fn_801CBA0C(0x06BD0400);
    fn_801CB834(iconHandle, 1, 0, 0);

    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0C3E1800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += fn_800D3088();
    }

    finalResult = fn_801CBA0C(0x10491000);
    fn_801845E4(ctx, iconHandle, ctx, finalResult, 0);

    cameraWaitSyncAnime(1);
    fn_800FF58C(0x82);
    floorSetFadeScript(0, 0);
}
