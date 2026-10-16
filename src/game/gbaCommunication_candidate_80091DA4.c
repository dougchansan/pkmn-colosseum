/**
 * @file gbaCommunication_candidate_80091DA4.c
 * @brief gbaCommunication carve, 0x80091DA4 - 0x80092FC8 (9 functions).
 *
 * Standalone carve: GBA_COMMUNICATION_DECLS_ONLY takes only the shared
 * declarations from gbaCommunication.c, and the functions are defined here
 * in address order. Like gbaCommunication_prefix.c, the unit is built with
 * -opt nopeephole (configure.py) instead of per-function pragmas.
 */
#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

/* 0x80091DA4 | size: 0x1A4 */
void fn_80091DA4(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    extern u32 std112_modelGetResourceID(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 std093_getCurrentGroupID(void);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std116_mapObjectStopAnimation(0x0CE61000);
    std115_mapObjectPlayAnimation(0x0CE61004, 1, 0, 0);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x111B1400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    std115_mapObjectPlayAnimation(0x0CE61000, 3, 0, 0);
    waitFrames = 0x32;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1E0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    cameraPlayAnime(ctx, 0x0CEF1800, 0, 0);
    cameraWaitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        cameraWaitFrames = (u32)lbl_8047C1D0;
        if (cameraWaitFrames < 1) {
            cameraWaitFrames = 1;
        }
    }
    for (cameraElapsed = 0; cameraElapsed < cameraWaitFrames; ) {
        _threadSwitch();
        cameraElapsed += GSgetFrameStep();
    }

    cameraWaitSyncAnime(1);
    floorRequestChange(0x82);
    floorSetFadeScript(0, 0);
}

/* The receiver setup below is expanded twice here and five times in
 * fn_80092140 (same calls, same argument sources). */
static inline void gbaCommunicationSetShadowReceiver(u32 ctx, u32 model)
{
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    u32 target;

    target = GSresGetResource(ctx, model);
    GSmodelSetShadowFlags(target, 2);
    GSmodelSetShadowLight(target, lbl_8047A690);
    GSmodelSetShadowSurface(target, 1, &lbl_8047A694);
}

/* 0x80091F48 | size: 0x1F8 */
void fn_80091F48(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    extern u32 std112_modelGetResourceID(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 std093_getCurrentGroupID(void);

    u32 elapsed;
    u32 waitFrames;
    u32 animB;
    u32 modelB;
    u32 animA;
    u32 modelA;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DD1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DD1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    modelA = std112_modelGetResourceID(0x06BB0400);
    modelB = std112_modelGetResourceID(0x0D240400);

    gbaCommunicationSetShadowReceiver(ctx, modelA);

    gbaCommunicationSetShadowReceiver(ctx, modelB);

    cameraPlayAnime(ctx, 0x0B891800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    animA = std112_modelGetResourceID(0x0B861000);
    animB = std112_modelGetResourceID(0x0B861001);

    std070_characterAttachToCharacter(ctx, modelA, ctx, animA, 0);
    std070_characterAttachToCharacter(ctx, modelB, ctx, animB, 0);

    std115_mapObjectPlayAnimation(modelB, 8, 0, 1);
    std115_mapObjectPlayAnimation(modelA, 8, 0x32, 0);
    scriptWaitSyncMotion(modelA, 0);
    std115_mapObjectPlayAnimation(modelA, 9, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x80092140 | size: 0x358 */
void fn_80092140(u32 ctx) {
    extern void GSmodelSetShadowTextureSize(u32, u32);
    extern u32 std112_modelGetResourceID(u32);
    extern u32 GSmodelSetShadowFlags(u32, u32);
    extern void GSmodelSetShadowLight(u32, u32);
    extern void GSmodelSetShadowSurface(u32, u32, u32*);
    extern void cameraPlayAnime(u32, u32, u32, u32);
    extern s32 fn_800D37CC(void);
    extern u32 GSgetFrameStep(void);
    extern void std070_characterAttachToCharacter(u32, u32, u32, u32, u32);
    extern void std115_mapObjectPlayAnimation(u32, u32, u32, u32);
    extern void cameraWaitSyncAnime(s32);
    extern void floorRequestChange(u32);
    u32 configured, elapsed, waitFrames;
    u32 animation0, animation1, animation2, animation3, animation4;
    u32 model0, model1, model2, model3, model4;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DD1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DD1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    model0 = std112_modelGetResourceID(0x0D240400);
    model1 = std112_modelGetResourceID(0x0D240400);
    model2 = std112_modelGetResourceID(0x0D240400);
    model3 = std112_modelGetResourceID(0x0D240400);
    model4 = std112_modelGetResourceID(0x0D240400);

    configured = GSresGetResource(ctx, model0);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model1);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model2);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model3);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model4);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0B881800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames;) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    animation0 = std112_modelGetResourceID(0x0B851004);
    animation1 = std112_modelGetResourceID(0x0B851003);
    animation2 = std112_modelGetResourceID(0x0B851001);
    animation3 = std112_modelGetResourceID(0x0B851002);
    animation4 = std112_modelGetResourceID(0x0B851003);
    std070_characterAttachToCharacter(ctx, model0, ctx, animation0, 0);
    std070_characterAttachToCharacter(ctx, model1, ctx, animation1, 0);
    std070_characterAttachToCharacter(ctx, model2, ctx, animation2, 0);
    std070_characterAttachToCharacter(ctx, model3, ctx, animation3, 0);
    std070_characterAttachToCharacter(ctx, model4, ctx, animation4, 0);
    std115_mapObjectPlayAnimation(model0, 6, 0, 1);
    std115_mapObjectPlayAnimation(model1, 6, 0, 1);
    std115_mapObjectPlayAnimation(model2, 8, 0, 1);
    std115_mapObjectPlayAnimation(model3, 8, 0, 1);
    std115_mapObjectPlayAnimation(model4, 7, 0, 1);
    cameraWaitSyncAnime(1);
    floorRequestChange(0x83);
    floorSetFadeScript(0, 0);
}

/* 0x80092498 | size: 0x1CC */
void fn_80092498(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    extern u32 std112_modelGetResourceID(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 std093_getCurrentGroupID(void);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    GSmaterialEntry *material;

    lbl_8047A690 = GSresGetResource(ctx, 0x0B631602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0B631002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0B631000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0B631000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0B631000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0B631000),
                               GSresGetResource(ctx, 0x112B1400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0B631000), 4);

    cameraPlayAnime(ctx, 0x0B831800, 0, 0);
    cameraWaitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        cameraWaitFrames = (u32)lbl_8047C1D0;
        if (cameraWaitFrames < 1) {
            cameraWaitFrames = 1;
        }
    }
    for (cameraElapsed = 0; cameraElapsed < cameraWaitFrames; ) {
        _threadSwitch();
        cameraElapsed += GSgetFrameStep();
    }

    std115_mapObjectPlayAnimation(0x0B631000, 0, 0, 0);
    cameraWaitSyncAnime(1);
    floorRequestChange(0x83);
    floorSetFadeScript(0, 0);
}

/* 0x80092664 | size: 0x358 */
void fn_80092664(u32 ctx) {
    extern void GSmodelSetShadowTextureSize(u32, u32);
    extern u32 std112_modelGetResourceID(u32);
    extern u32 GSmodelSetShadowFlags(u32, u32);
    extern void GSmodelSetShadowLight(u32, u32);
    extern void GSmodelSetShadowSurface(u32, u32, u32*);
    extern void cameraPlayAnime(u32, u32, u32, u32);
    extern s32 fn_800D37CC(void);
    extern u32 GSgetFrameStep(void);
    extern void std070_characterAttachToCharacter(u32, u32, u32, u32, u32);
    extern void std115_mapObjectPlayAnimation(u32, u32, u32, u32);
    extern void cameraWaitSyncAnime(s32);
    extern void floorRequestChange(u32);
    u32 configured, elapsed, waitFrames;
    u32 animation0, animation1, animation2, animation3, animation4;
    u32 model0, model1, model2, model3, model4;

    lbl_8047A690 = GSresGetResource(ctx, 0x06DD1604);
    lbl_8047A694 = GSresGetResource(ctx, 0x06DD1001);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    model0 = std112_modelGetResourceID(0x0D240400);
    model1 = std112_modelGetResourceID(0x0D240400);
    model2 = std112_modelGetResourceID(0x0D240400);
    model3 = std112_modelGetResourceID(0x0D240400);
    model4 = std112_modelGetResourceID(0x0D240400);

    configured = GSresGetResource(ctx, model0);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model1);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model2);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model3);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);
    configured = GSresGetResource(ctx, model4);
    GSmodelSetShadowFlags(configured, 2);
    GSmodelSetShadowLight(configured, lbl_8047A690);
    GSmodelSetShadowSurface(configured, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0B871800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames;) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    animation0 = std112_modelGetResourceID(0x0B841004);
    animation1 = std112_modelGetResourceID(0x0B841000);
    animation2 = std112_modelGetResourceID(0x0B841001);
    animation3 = std112_modelGetResourceID(0x0B841002);
    animation4 = std112_modelGetResourceID(0x0B841003);
    std070_characterAttachToCharacter(ctx, model0, ctx, animation0, 0);
    std070_characterAttachToCharacter(ctx, model1, ctx, animation1, 0);
    std070_characterAttachToCharacter(ctx, model2, ctx, animation2, 0);
    std070_characterAttachToCharacter(ctx, model3, ctx, animation3, 0);
    std070_characterAttachToCharacter(ctx, model4, ctx, animation4, 0);
    std115_mapObjectPlayAnimation(model0, 1, 0, 1);
    std115_mapObjectPlayAnimation(model1, 1, 0, 1);
    std115_mapObjectPlayAnimation(model2, 1, 0, 1);
    std115_mapObjectPlayAnimation(model3, 2, 0, 1);
    std115_mapObjectPlayAnimation(model4, 2, 0, 1);
    cameraWaitSyncAnime(1);
    floorRequestChange(0x87);
    floorSetFadeScript(0, 0);
}

/* 0x800929BC | size: 0x170 */
void fn_800929BC(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    extern u32 std112_modelGetResourceID(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 std093_getCurrentGroupID(void);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    GSmaterialEntry *material;

    lbl_8047A690 = GSresGetResource(ctx, 0x0B631602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0B631002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0B631000, 0, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0B631000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0B631000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    cameraPlayAnime(std093_getCurrentGroupID(), 0x0B661800, 0, 0);
    cameraWaitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        cameraWaitFrames = (u32)lbl_8047C1D0;
        if (cameraWaitFrames < 1) {
            cameraWaitFrames = 1;
        }
    }
    for (cameraElapsed = 0; cameraElapsed < cameraWaitFrames; ) {
        _threadSwitch();
        cameraElapsed += GSgetFrameStep();
    }

    cameraWaitSyncAnime(1);
    floorRequestChange(0x83);
    floorSetFadeScript(0, 0);
}

/* 0x80092B2C | size: 0x164 */
void fn_80092B2C(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern s32 fn_800D37CC(void);
    extern void _threadSwitch(void);
    extern u32 GSgetFrameStep(void);
    extern u32 std112_modelGetResourceID(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32 *param);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);
    extern void fn_80118874(void *texture, u32 flag);
    extern u32 std093_getCurrentGroupID(void);

    u32 elapsed;
    u32 shadowTarget;
    u32 waitFrames;
    u32 model;
    u32 anim;

    lbl_8047A690 = GSresGetResource(ctx, 0x0B631602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0B631002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);

    model = std112_modelGetResourceID(0x0B721000);
    std115_mapObjectPlayAnimation(model, 6, 0, 1);

    shadowTarget = GSresGetResource(ctx, model);
    GSmodelSetShadowFlags(shadowTarget, 2);
    GSmodelSetShadowLight(shadowTarget, lbl_8047A690);
    GSmodelSetShadowSurface(shadowTarget, 1, &lbl_8047A694);

    cameraPlayAnime(std093_getCurrentGroupID(), 0x0B651800, 0, 0);
    waitFrames = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1D0;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    anim = std112_modelGetResourceID(0x0B731000);
    std070_characterAttachToCharacter(ctx, model, ctx, anim, 0);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x87);
    floorSetFadeScript(0, 0);
}

/* 0x80092C90 | size: 0x1A8 */
s32 fn_80092C90(s32 channel, void* arg1, void* arg2)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern s32 fn_80089048(void* out, void* arg1, void* arg2);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        requestStarted = fn_80089048(requestWork + 0x4344, arg1, arg2);
        if (requestStarted != 0) {
            *(s32*)(requestWork + GBA_STATE_PHASE) = 12;
            *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x3000C;
        }
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}

/* 0x80092E38 | size: 0x190 */
s32 fn_80092E38(s32 channel, void* requestValue)
{
    extern u32 fn_800E2C04(u32 size, u32 align);
    extern void* fn_800E27B0(u32 handle);
    extern void fn_8009F77C(void* work);
    extern void fn_8009F9C8(void* callback);
    extern s32 fn_800937F4(void* arg);
    extern void fn_80093B04(u32 a, u32 b);
    extern void OSCreateThread(void* thread, void* entry, void* arg,
                               void* stack, u32 stackSize, s32 priority,
                               u16 attributes);
    extern void OSResumeThread(void* thread);
    extern void* memset(void* dst, int value, u32 size);

    u32 slot;
    u32 handle;
    u8* allocated;
    u8* work;
    s32 started;
    s32 requestStarted;
    u8* requestWork;

    if (channel < 0 || channel > 3) {
        started = 0;
    } else {
        slot = (u32)channel << 2;
        if (*(u8**)(lbl_803FB328 + slot) != NULL) {
            started = 1;
        } else {
            handle = fn_800E2C04(0x44A0, 0x20);
            if ((handle & 0xFFFF) == 0) {
                __assert(lbl_8026F5A8, 0x1DD, &lbl_8047C1E8);
            }
            allocated = fn_800E27B0(handle);
            memset(allocated, 0, 0x4490);
            *(u8**)(lbl_803FB328 + slot) = allocated;

            work = *(u8**)(lbl_803FB328 + slot);
            *(u32*)(work + GBA_STATE_PHASE) = 0;
            *(s32*)(work + GBA_STATE_PORT) = channel;
            fn_800716C8(channel, work + GBA_DATA_OFFSET, fn_80093B04);
            fn_8009F77C(work);
            fn_8009F9C8(work + 0x18);
            OSCreateThread(work + GBA_DATA_OFFSET, fn_800937F4, work,
                           work + GBA_STATE_PORT, 0x4000,
                           GBA_THREAD_PRIORITY, 0);
            OSResumeThread(work + GBA_DATA_OFFSET);
            started = 1;
        }
    }

    if (started == 0) {
        return 0;
    }

    requestWork = *(u8**)(lbl_803FB328 + ((u32)channel << 2));
    requestStarted = 0;
    fn_8009F7B4(requestWork);
    if (*(s32*)(requestWork + GBA_STATE_PHASE) == 0) {
        *(s32*)(requestWork + GBA_STATE_PHASE) = 11;
        *(u32*)(requestWork + GBA_STATE_TIMEOUT) = 0x3000B;
        requestStarted = 1;
        *(void**)(requestWork + 0x4344) = requestValue;
    }
    fn_8009F890(requestWork);
    fn_800A257C(requestWork + GBA_DATA_OFFSET, GBA_THREAD_PRIORITY);
    if (requestStarted != 0) {
        fn_8009FABC(requestWork + 0x18);
    }
    return requestStarted;
}
