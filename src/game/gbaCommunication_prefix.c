/**
 * @file gbaCommunication_prefix.c
 * @brief gbaCommunication carve, 0x8008C7B0 - 0x80090720 (16 functions,
 *        linked).
 *
 * The functions are defined in address order. Every one of them needs the
 * peephole pass off, so the unit is built with -opt nopeephole (configure.py)
 * instead of per-function pragmas. The float literals are the gbaCommunication
 * pool at 0x8047C1D0, which this carve shares with
 * gbaCommunication_exact_80090720.c; it stays in sdata2_8047C1A0.c.
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
/* RULE-EXCEPTION(user-approved): extern named stand-in for the TU's pool
 * literal - see docs/RULE_EXCEPTIONS.md. The gbaCommunication pool
 * (0x8047C1D0-0x8047C1E8) is shared by this carve and
 * gbaCommunication_exact_80090720.c and stays in sdata2_8047C1A0.c; const
 * gives the literal's load schedule (a plain 0.0f would emit a local
 * .sdata2 the carve does not own). */
extern const f32 lbl_8047C1D4; /* 0.0f */
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
extern void std098_mapWarpToMenuRoom(u32 id);
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
extern void gfxSetFlags(u32 arg);
extern void gfxClearFlags(u32 arg);
extern void gfxSetPrimitive(u32 arg);
extern void gfxSetVtxDesc(void* arg);
extern void gfxLoadTexture(u32 arg0, void* arg1);
extern void gfxBegin(u32 arg);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D5CB8(u32 arg0, u32 r, u32 g, u32 b, u32 a);
extern void gfxVtxTexCoord(u32 arg0, f32 s, f32 t);
extern void gfxEnd(void);
extern u32 fn_8001D624(void* pokemon, u32 arg);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);

/* 0x8008C7B0 */
void fn_8008C7B0(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    extern void std078_flagSetFalse(u32 id);
    extern void floorRequestChange(u32 id);
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
    u32 handle2;
    u32 iconHandle;
    u32 iconHandle2;
    u32 b2;
    u32 tmpA;
    u32 tmpB;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    iconHandle2 = std112_modelGetResourceID(0x0B720400);

    b2 = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(b2, 2);
    GSmodelSetShadowLight(b2, lbl_8047A690);
    GSmodelSetShadowSurface(b2, 1, &lbl_8047A694);

    b2 = GSresGetResource(ctx, iconHandle2);
    GSmodelSetShadowFlags(b2, 2);
    GSmodelSetShadowLight(b2, lbl_8047A690);
    GSmodelSetShadowSurface(b2, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0D041800, 0, 0);
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

    tmpA = std112_modelGetResourceID(0x0D0D1000);
    tmpB = std112_modelGetResourceID(0x0D0D1001);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, tmpA, 0);
    std070_characterAttachToCharacter(ctx, iconHandle2, ctx, tmpB, 0);
    std115_mapObjectPlayAnimation(iconHandle, 9, 0, 1);
    std115_mapObjectPlayAnimation(iconHandle2, 6, 0, 1);

    cameraWaitSyncAnime(1);
    std078_flagSetFalse(0x8D0);
    floorRequestChange(1);
    floorSetFadeScript(0, 0);
}

/* 0x8008CACC */
void fn_8008CACC(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x11211400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    std115_mapObjectPlayAnimation(0x0CE61000, 2, 0, 0);
    waitFrames = 100;
    if (fn_800D37CC() == 0x32) {
        waitFrames = (u32)lbl_8047C1DC;
        if (waitFrames < 1) {
            waitFrames = 1;
        }
    }
    for (elapsed = 0; elapsed < waitFrames; ) {
        _threadSwitch();
        elapsed += GSgetFrameStep();
    }

    iconHandle = std112_modelGetResourceID(0x06BC0400);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0D021800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0D0C1000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    std115_mapObjectPlayAnimation(iconHandle, 4, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008CDD8 */
void fn_8008CDD8(u32 ctx) {
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0D011800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0D0B1000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    std115_mapObjectPlayAnimation(iconHandle, 0xB, 0, 0);
    scriptWaitSyncMotion(iconHandle, 1);
    std115_mapObjectPlayAnimation(iconHandle, 0xC, 0, 0);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008D0A0 */
void fn_8008D0A0(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0D001800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0D0A1000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    std115_mapObjectPlayAnimation(iconHandle, 9, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008D348 */
void fn_8008D348(u32 ctx) {
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32* param);
    extern void GSmodelGetFrameCount(u32 handle, f32* out, u32 arg);
    extern void GSmodelSetAnimFrame(u32 handle, f32 frame);
    extern void GSmodelSetAnimType(u32 handle, u32 type);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 bank);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 mode);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void fn_80118874(u32 ptr, u32 val);
    extern u32 std112_modelGetResourceID(u32 id);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void cameraWaitSyncAnime(s32 sync);

    u32 elapsed1;
    u32 waitFrames1;
    u32 elapsed2;
    u32 waitFrames2;
    u32 model;
    u32 base;
    f32 frame;
    u32 h06af0400;
    u32 h06bc0400;
    u32 h06be0400a;
    u32 h06be0400b;
    u32 h06be0400c;
    u32 h06be0400d;
    u32 h06be0400e;
    u32 h0d091000;
    u32 h0d091001;
    u32 h0d091006;
    u32 h0d091002;
    u32 h0d091003;
    u32 h0d091004;
    u32 h0d091005;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

    waitFrames1 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames1 = (u32)lbl_8047C1D0;
        if (waitFrames1 < 1) {
            waitFrames1 = 1;
        }
    }
    for (elapsed1 = 0; elapsed1 < waitFrames1; ) {
        _threadSwitch();
        elapsed1 += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0CE61000);

    base = GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(*(u32*)(base + 0x144), 1);
    *(u32*)(base + 0x144) = 0;

    frame = lbl_8047C1D4;
    model = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(model, 0);
    GSmodelGetFrameCount(model, &frame, 0);
    frame -= lbl_8047C1D8;
    GSmodelSetAnimIndex(model, 0);
    GSmodelSetAnimFrame(model, frame);
    GSmodelSetAnimType(model, 0);
    GSmodelStartAnimation(model);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x11201400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    h06af0400 = std112_modelGetResourceID(0x06AF0400);
    h06bc0400 = std112_modelGetResourceID(0x06BC0400);
    h06be0400a = std112_modelGetResourceID(0x06BE0400);
    h06be0400b = std112_modelGetResourceID(0x06BE0400);
    h06be0400c = std112_modelGetResourceID(0x06BE0400);
    h06be0400d = std112_modelGetResourceID(0x06BE0400);
    h06be0400e = std112_modelGetResourceID(0x06BE0400);

    model = GSresGetResource(ctx, h06af0400);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06bc0400);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400a);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400b);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400c);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400d);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400e);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, h06af0400), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, h06af0400), 4);

    cameraPlayAnime(ctx, 0x0CFF1800, 0, 0);

    waitFrames2 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames2 = (u32)lbl_8047C1D0;
        if (waitFrames2 < 1) {
            waitFrames2 = 1;
        }
    }
    for (elapsed2 = 0; elapsed2 < waitFrames2; ) {
        _threadSwitch();
        elapsed2 += GSgetFrameStep();
    }

    h0d091000 = std112_modelGetResourceID(0x0D091000);
    h0d091001 = std112_modelGetResourceID(0x0D091001);
    h0d091006 = std112_modelGetResourceID(0x0D091006);
    h0d091002 = std112_modelGetResourceID(0x0D091002);
    h0d091003 = std112_modelGetResourceID(0x0D091003);
    h0d091004 = std112_modelGetResourceID(0x0D091004);
    h0d091005 = std112_modelGetResourceID(0x0D091005);

    std070_characterAttachToCharacter(ctx, h06af0400, ctx, h0d091000, 0);
    std070_characterAttachToCharacter(ctx, h06bc0400, ctx, h0d091001, 0);
    std070_characterAttachToCharacter(ctx, h06be0400a, ctx, h0d091006, 0);
    std070_characterAttachToCharacter(ctx, h06be0400b, ctx, h0d091002, 0);
    std070_characterAttachToCharacter(ctx, h06be0400c, ctx, h0d091003, 0);
    std070_characterAttachToCharacter(ctx, h06be0400d, ctx, h0d091004, 0);
    std070_characterAttachToCharacter(ctx, h06be0400e, ctx, h0d091005, 0);

    std115_mapObjectPlayAnimation(0x0CE61000, 1, 0, 0);
    std115_mapObjectPlayAnimation(h06af0400, 9, 0, 0);
    std115_mapObjectPlayAnimation(h06bc0400, 7, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400a, 0xA, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400b, 0xA, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400c, 0xA, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400d, 0xA, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400e, 0xA, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008D938 */
void fn_8008D938(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 model, u32 bank);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 model, u32 mode);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 motion, u32 arg2, u32 arg3);
    extern void scriptWaitSyncMotion(u32 id, u32 sync);
    extern s32 fn_800D37CC(void);
    extern u32 GSgetFrameStep(void);
    extern u32 std112_modelGetResourceID(u32 id);
    extern u32 GSmodelSetShadowFlags(u32 model, u32 flags);
    extern void GSmodelSetShadowLight(u32 model, u32 light);
    extern void GSmodelSetShadowSurface(u32 model, u32 count, u32 *surface);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 arg2, u32 arg3);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 model, u32 ctx2, u32 motion, u32 flags);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 arg0, u32 arg1);
    extern void fn_80118874(void *texture, u32 flag);
    extern void GSmodelSetAnimIndex(u32 model, u32 index);
    extern void GSmodelGetFrameCount(u32 model, f32 *frame, u32 flags);
    extern void GSmodelSetAnimFrame(u32 model, f32 frame);
    extern void GSmodelSetAnimType(u32 model, u32 type);
    extern void GSmodelStartAnimation(u32 model);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    u32 elapsedLater;
    u32 waitFramesLater;
    u32 elapsedFinal;
    u32 waitFramesFinal;
    GSmaterialEntry *material;
    f32 frame;
    u32 model0;
    u32 model1;
    u32 model2;
    u32 model3;
    u32 model4;
    u32 model5;
    u32 model6;
    u32 model7;
    u32 model8;
    u32 motion0;
    u32 motion8;
    u32 motion1;
    u32 motion2;
    u32 motion3;
    u32 motion4;
    u32 motion5;
    u32 motion6;
    u32 motion7;
    u32 resource;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);
    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    resource = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(resource, 0);
    GSmodelGetFrameCount(resource, &frame, 0);
    frame -= lbl_8047C1D8;
    GSmodelSetAnimIndex(resource, 0);
    GSmodelSetAnimFrame(resource, frame);
    GSmodelSetAnimType(resource, 0);
    GSmodelStartAnimation(resource);

    resource = GSresGetResource(ctx, 0x111B1400);
    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), resource);
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);
    std115_mapObjectPlayAnimation(0x0CE61000, 3, 0, 0);

    waitFramesLater = 0x32;
    if (fn_800D37CC() == 0x32) {
        waitFramesLater = (u32)lbl_8047C1E0;
        if (waitFramesLater < 1) {
            waitFramesLater = 1;
        }
    }
    for (elapsedLater = 0; elapsedLater < waitFramesLater; ) {
        _threadSwitch();
        elapsedLater += GSgetFrameStep();
    }

    resource = GSresGetResource(ctx, 0x111F1400);
    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), resource);
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    model0 = std112_modelGetResourceID(0x06BC0400);
    model1 = std112_modelGetResourceID(0x06BE0400);
    model2 = std112_modelGetResourceID(0x0D290400);
    model3 = std112_modelGetResourceID(0x06BE0400);
    model4 = std112_modelGetResourceID(0x0D240400);
    model5 = std112_modelGetResourceID(0x0D290400);
    model6 = std112_modelGetResourceID(0x0D240400);
    model7 = std112_modelGetResourceID(0x06BE0400);
    model8 = std112_modelGetResourceID(0x0D240400);

#define SET_MODEL_SHADOW(model)                                                    \
    resource = GSresGetResource(ctx, (model));                                     \
    GSmodelSetShadowFlags(resource, 2);                                            \
    GSmodelSetShadowLight(resource, lbl_8047A690);                                 \
    GSmodelSetShadowSurface(resource, 1, &lbl_8047A694)

    SET_MODEL_SHADOW(model0);
    SET_MODEL_SHADOW(model1);
    SET_MODEL_SHADOW(model2);
    SET_MODEL_SHADOW(model3);
    SET_MODEL_SHADOW(model4);
    SET_MODEL_SHADOW(model5);
    SET_MODEL_SHADOW(model6);
    SET_MODEL_SHADOW(model7);
    SET_MODEL_SHADOW(model8);

#undef SET_MODEL_SHADOW

    cameraPlayAnime(ctx, 0x0CFE1800, 0, 0);
    waitFramesFinal = 1;
    if (fn_800D37CC() == 0x32) {
        waitFramesFinal = (u32)lbl_8047C1D0;
        if (waitFramesFinal < 1) {
            waitFramesFinal = 1;
        }
    }
    for (elapsedFinal = 0; elapsedFinal < waitFramesFinal; ) {
        _threadSwitch();
        elapsedFinal += GSgetFrameStep();
    }

    motion0 = std112_modelGetResourceID(0x0D081000);
    motion8 = std112_modelGetResourceID(0x0D081008);
    motion1 = std112_modelGetResourceID(0x0D081001);
    motion2 = std112_modelGetResourceID(0x0D081002);
    motion3 = std112_modelGetResourceID(0x0D081003);
    motion4 = std112_modelGetResourceID(0x0D081004);
    motion5 = std112_modelGetResourceID(0x0D081005);
    motion6 = std112_modelGetResourceID(0x0D081006);
    motion7 = std112_modelGetResourceID(0x0D081007);

    std070_characterAttachToCharacter(ctx, model0, ctx, motion0, 0);
    std070_characterAttachToCharacter(ctx, model1, ctx, motion8, 0);
    std070_characterAttachToCharacter(ctx, model2, ctx, motion1, 0);
    std070_characterAttachToCharacter(ctx, model3, ctx, motion2, 0);
    std070_characterAttachToCharacter(ctx, model4, ctx, motion3, 0);
    std070_characterAttachToCharacter(ctx, model5, ctx, motion4, 0);
    std070_characterAttachToCharacter(ctx, model6, ctx, motion5, 0);
    std070_characterAttachToCharacter(ctx, model7, ctx, motion6, 0);
    std070_characterAttachToCharacter(ctx, model8, ctx, motion7, 0);

    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);
    std115_mapObjectPlayAnimation(model0, 6, 0, 0);
    std115_mapObjectPlayAnimation(model1, 9, 0, 0);
    std115_mapObjectPlayAnimation(model2, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model3, 9, 0, 0);
    std115_mapObjectPlayAnimation(model4, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model5, 9, 0, 0);
    std115_mapObjectPlayAnimation(model6, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model7, 9, 0, 0);
    std115_mapObjectPlayAnimation(model7, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model8, 0xB, 0, 0);

    scriptWaitSyncMotion(model0, 1);
    scriptWaitSyncMotion(model1, 1);
    scriptWaitSyncMotion(model2, 1);
    scriptWaitSyncMotion(model3, 1);
    scriptWaitSyncMotion(model4, 1);
    scriptWaitSyncMotion(model5, 1);
    scriptWaitSyncMotion(model6, 1);
    scriptWaitSyncMotion(model7, 1);
    scriptWaitSyncMotion(model7, 1);
    scriptWaitSyncMotion(model8, 1);

    std115_mapObjectPlayAnimation(model0, 7, 0, 0);
    std115_mapObjectPlayAnimation(model1, 0xA, 0, 0);
    std115_mapObjectPlayAnimation(model2, 0xC, 0, 0);
    std115_mapObjectPlayAnimation(model3, 0xA, 0, 0);
    std115_mapObjectPlayAnimation(model5, 0xA, 0, 0);
    std115_mapObjectPlayAnimation(model4, 0xC, 0, 0);
    std115_mapObjectPlayAnimation(model6, 0xC, 0, 0);
    std115_mapObjectPlayAnimation(model7, 0xA, 0, 0);
    std115_mapObjectPlayAnimation(model7, 0xC, 0, 0);
    std115_mapObjectPlayAnimation(model8, 0xC, 0, 0);

    scriptWaitSyncMotion(model0, 1);
    scriptWaitSyncMotion(model1, 1);
    scriptWaitSyncMotion(model2, 1);
    scriptWaitSyncMotion(model3, 1);
    scriptWaitSyncMotion(model4, 1);
    scriptWaitSyncMotion(model5, 1);
    scriptWaitSyncMotion(model6, 1);
    scriptWaitSyncMotion(model7, 1);
    scriptWaitSyncMotion(model7, 1);
    scriptWaitSyncMotion(model8, 1);

    std115_mapObjectPlayAnimation(model0, 6, 0, 0);
    std115_mapObjectPlayAnimation(model1, 9, 0, 0);
    std115_mapObjectPlayAnimation(model2, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model3, 9, 0, 0);
    std115_mapObjectPlayAnimation(model4, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model5, 9, 0, 0);
    std115_mapObjectPlayAnimation(model6, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model7, 9, 0, 0);
    std115_mapObjectPlayAnimation(model7, 0xB, 0, 0);
    std115_mapObjectPlayAnimation(model8, 0xB, 0, 0);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008E320 */
void fn_8008E320(u32 ctx) {
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32* param);
    extern void GSmodelGetFrameCount(u32 handle, f32* out, u32 arg);
    extern void GSmodelSetAnimFrame(u32 handle, f32 frame);
    extern void GSmodelSetAnimType(u32 handle, u32 type);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 bank);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 mode);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern u32 std112_modelGetResourceID(u32 id);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);

    u32 elapsed1;
    u32 waitFrames1;
    u32 elapsed2;
    u32 waitFrames2;
    u32 elapsed3;
    u32 waitFrames3;
    f32 frame;
    u32 handle2;
    u32 h06af0400;
    u32 h06bc0400;
    u32 h06be0400a;
    u32 h06be0400b;
    u32 h0d071000;
    u32 h0d071001;
    u32 h0d071003;
    u32 h0d071002;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

    waitFrames1 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames1 = (u32)lbl_8047C1D0;
        if (waitFrames1 < 1) {
            waitFrames1 = 1;
        }
    }
    for (elapsed1 = 0; elapsed1 < waitFrames1; ) {
        _threadSwitch();
        elapsed1 += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0CE61000);

    handle2 = GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(((GSmaterialEntry*)handle2)->texture, 1);
    ((GSmaterialEntry*)handle2)->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame -= lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x111B1400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    std115_mapObjectPlayAnimation(0x0CE61000, 3, 0, 0);

    waitFrames2 = 0x32;
    if (fn_800D37CC() == 0x32) {
        waitFrames2 = (u32)lbl_8047C1E0;
        if (waitFrames2 < 1) {
            waitFrames2 = 1;
        }
    }
    for (elapsed2 = 0; elapsed2 < waitFrames2; ) {
        _threadSwitch();
        elapsed2 += GSgetFrameStep();
    }

    h06af0400 = std112_modelGetResourceID(0x06AF0400);
    h06bc0400 = std112_modelGetResourceID(0x06BC0400);
    h06be0400a = std112_modelGetResourceID(0x06BE0400);
    h06be0400b = std112_modelGetResourceID(0x06BE0400);

    handle2 = GSresGetResource(ctx, h06af0400);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);
    handle2 = GSresGetResource(ctx, h06bc0400);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);
    handle2 = GSresGetResource(ctx, h06be0400a);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);
    handle2 = GSresGetResource(ctx, h06be0400b);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, h06af0400), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, h06af0400), 4);

    cameraPlayAnime(ctx, 0x0CFD1800, 0, 0);

    waitFrames3 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames3 = (u32)lbl_8047C1D0;
        if (waitFrames3 < 1) {
            waitFrames3 = 1;
        }
    }
    for (elapsed3 = 0; elapsed3 < waitFrames3; ) {
        _threadSwitch();
        elapsed3 += GSgetFrameStep();
    }

    h0d071000 = std112_modelGetResourceID(0x0D071000);
    h0d071001 = std112_modelGetResourceID(0x0D071001);
    h0d071003 = std112_modelGetResourceID(0x0D071003);
    h0d071002 = std112_modelGetResourceID(0x0D071002);

    std070_characterAttachToCharacter(ctx, h06af0400, ctx, h0d071000, 0);
    std070_characterAttachToCharacter(ctx, h06bc0400, ctx, h0d071001, 0);
    std070_characterAttachToCharacter(ctx, h06be0400a, ctx, h0d071003, 0);
    std070_characterAttachToCharacter(ctx, h06be0400b, ctx, h0d071002, 0);

    std115_mapObjectPlayAnimation(h06af0400, 0xA, 0, 0);
    std115_mapObjectPlayAnimation(h06bc0400, 5, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400a, 0xE, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400b, 0xE, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008E7D4 */
void fn_8008E7D4(u32 ctx) {
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32* param);
    extern void GSmodelGetFrameCount(u32 handle, f32* out, u32 arg);
    extern void GSmodelSetAnimFrame(u32 handle, f32 frame);
    extern void GSmodelSetAnimType(u32 handle, u32 type);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 bank);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 mode);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern u32 std112_modelGetResourceID(u32 id);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void cameraWaitSyncAnime(s32 sync);
    extern void scriptWaitSyncMotion(u32 id, u32 val);
    extern void floorRequestChange(u32 id);
    extern void floorSetFadeScript(u32 a, u32 b);

    u32 elapsed1;
    u32 waitFrames1;
    u32 elapsed2;
    u32 waitFrames2;
    u32 elapsed3;
    u32 waitFrames3;
    f32 frame;
    u32 handle2;
    u32 h06af0400;
    u32 h06bc0400;
    u32 h06be0400;
    u32 h0d061000;
    u32 h0d061001;
    u32 h0d061002;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

    waitFrames1 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames1 = (u32)lbl_8047C1D0;
        if (waitFrames1 < 1) {
            waitFrames1 = 1;
        }
    }
    for (elapsed1 = 0; elapsed1 < waitFrames1; ) {
        _threadSwitch();
        elapsed1 += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0CE61000);

    handle2 = GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(((GSmaterialEntry*)handle2)->texture, 1);
    ((GSmaterialEntry*)handle2)->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame -= lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, 0x0CE61000), GSresGetResource(ctx, 0x111B1400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, 0x0CE61000), 4);

    std115_mapObjectPlayAnimation(0x0CE61000, 3, 0, 0);

    waitFrames2 = 0x32;
    if (fn_800D37CC() == 0x32) {
        waitFrames2 = (u32)lbl_8047C1E0;
        if (waitFrames2 < 1) {
            waitFrames2 = 1;
        }
    }
    for (elapsed2 = 0; elapsed2 < waitFrames2; ) {
        _threadSwitch();
        elapsed2 += GSgetFrameStep();
    }

    h06af0400 = std112_modelGetResourceID(0x06AF0400);
    h06bc0400 = std112_modelGetResourceID(0x06BC0400);
    h06be0400 = std112_modelGetResourceID(0x06BE0400);

    handle2 = GSresGetResource(ctx, h06af0400);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);
    handle2 = GSresGetResource(ctx, h06bc0400);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);
    handle2 = GSresGetResource(ctx, h06be0400);
    GSmodelSetShadowFlags(handle2, 2);
    GSmodelSetShadowLight(handle2, lbl_8047A690);
    GSmodelSetShadowSurface(handle2, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, h06af0400), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, h06af0400), 4);

    cameraPlayAnime(ctx, 0x0CFC1800, 0, 0);

    waitFrames3 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames3 = (u32)lbl_8047C1D0;
        if (waitFrames3 < 1) {
            waitFrames3 = 1;
        }
    }
    for (elapsed3 = 0; elapsed3 < waitFrames3; ) {
        _threadSwitch();
        elapsed3 += GSgetFrameStep();
    }

    h0d061000 = std112_modelGetResourceID(0x0D061000);
    h0d061001 = std112_modelGetResourceID(0x0D061001);
    h0d061002 = std112_modelGetResourceID(0x0D061002);

    std070_characterAttachToCharacter(ctx, h06af0400, ctx, h0d061000, 2);
    std070_characterAttachToCharacter(ctx, h06bc0400, ctx, h0d061001, 0);
    std070_characterAttachToCharacter(ctx, h06be0400, ctx, h0d061002, 0);

    std115_mapObjectPlayAnimation(h06af0400, 7, 0, 0);
    scriptWaitSyncMotion(h06af0400, 1);
    std115_mapObjectPlayAnimation(h06af0400, 8, 0, 0);
    std115_mapObjectPlayAnimation(h06bc0400, 5, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400, 0xE, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008EC28 */
void fn_8008EC28(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0CFB1800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0D051000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    std115_mapObjectPlayAnimation(iconHandle, 7, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008EED0 */
void fn_8008EED0(u32 ctx) {
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    std115_mapObjectPlayAnimation(iconHandle, 4, 0, 0);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0CFA1800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0D031000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    scriptWaitSyncMotion(iconHandle, 1);
    std115_mapObjectPlayAnimation(iconHandle, 5, 0, 0);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008F190 */
void fn_8008F190(u32 ctx) {
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

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

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    std115_mapObjectPlayAnimation(iconHandle, 0, 0, 0);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0CF91800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0CF81000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    std115_mapObjectPlayAnimation(iconHandle, 1, 0, 0);
    scriptWaitSyncMotion(iconHandle, 1);
    std115_mapObjectPlayAnimation(iconHandle, 2, 0, 0);
    scriptWaitSyncMotion(iconHandle, 1);
    std115_mapObjectPlayAnimation(iconHandle, 3, 0, 0);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008F524 */
void fn_8008F524(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    extern void GSmodelSetAnimIndex(u32 handle, u32 val);
    extern void GSmodelGetFrameCount(u32 handle, f32 *out, u32 flag);
    extern void GSmodelSetAnimFrame(u32 handle, f32 val);
    extern void GSmodelSetAnimType(u32 handle, u32 val);
    extern void GSmodelStartAnimation(u32 handle);

    u32 el1;
    u32 wf1;
    u32 elapsed;
    u32 waitFrames;
    u32 sA;
    u32 sB;
    u32 sC;
    f32 frame;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    u32 handle2;
    u32 modelA;
    u32 modelB;
    u32 modelC;
    u32 animA;
    u32 animB;
    u32 animC;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

    wf1 = 1;
    if (fn_800D37CC() == 0x32) {
        wf1 = (u32)lbl_8047C1D0;
        if (wf1 < 1) {
            wf1 = 1;
        }
    }
    for (el1 = 0; el1 < wf1; ) {
        _threadSwitch();
        el1 += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0CE61000);

    handle2 = GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(((GSmaterialEntry *)handle2)->texture, 1);
    ((GSmaterialEntry *)handle2)->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

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

    modelA = std112_modelGetResourceID(0x06BC0400);
    modelB = std112_modelGetResourceID(0x06BE0400);
    modelC = std112_modelGetResourceID(0x06BE0400);

    sA = GSresGetResource(ctx, modelA);
    GSmodelSetShadowFlags(sA, 2);
    GSmodelSetShadowLight(sA, lbl_8047A690);
    GSmodelSetShadowSurface(sA, 1, &lbl_8047A694);

    sB = GSresGetResource(ctx, modelB);
    GSmodelSetShadowFlags(sB, 2);
    GSmodelSetShadowLight(sB, lbl_8047A690);
    GSmodelSetShadowSurface(sB, 1, &lbl_8047A694);

    sC = GSresGetResource(ctx, modelC);
    GSmodelSetShadowFlags(sC, 2);
    GSmodelSetShadowLight(sC, lbl_8047A690);
    GSmodelSetShadowSurface(sC, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF71800, 0, 0);
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

    animA = std112_modelGetResourceID(0x0CEE1000);
    animB = std112_modelGetResourceID(0x0CEE1002);
    animC = std112_modelGetResourceID(0x0CEE1001);

    std070_characterAttachToCharacter(ctx, modelA, ctx, animA, 0);
    std070_characterAttachToCharacter(ctx, modelB, ctx, animB, 0);
    std070_characterAttachToCharacter(ctx, modelC, ctx, animC, 0);

    std115_mapObjectPlayAnimation(modelA, 3, 0, 1);
    std115_mapObjectPlayAnimation(modelB, 4, 0, 1);
    std115_mapObjectPlayAnimation(modelC, 4, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008F91C */
void fn_8008F91C(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    extern void GSmodelSetAnimIndex(u32 handle, u32 val);
    extern void GSmodelGetFrameCount(u32 handle, f32 *out, u32 flag);
    extern void GSmodelSetAnimFrame(u32 handle, f32 val);
    extern void GSmodelSetAnimType(u32 handle, u32 val);
    extern void GSmodelStartAnimation(u32 handle);

    u32 elapsed;
    u32 waitFrames;
    u32 cameraElapsed;
    u32 cameraWaitFrames;
    GSmaterialEntry *material;
    f32 frame;
    u32 handle2;
    u32 modelA;
    u32 modelB;
    u32 animA;
    u32 animB;
    u32 shadowB;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    modelA = std112_modelGetResourceID(0x06BD0400);
    modelB = std112_modelGetResourceID(0x06BA0400);

    animA = GSresGetResource(ctx, modelA);
    GSmodelSetShadowFlags(animA, 2);
    GSmodelSetShadowLight(animA, lbl_8047A690);
    GSmodelSetShadowSurface(animA, 1, &lbl_8047A694);

    shadowB = GSresGetResource(ctx, modelB);
    GSmodelSetShadowFlags(shadowB, 2);
    GSmodelSetShadowLight(shadowB, lbl_8047A690);
    GSmodelSetShadowSurface(shadowB, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF61800, 0, 0);
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

    animA = std112_modelGetResourceID(0x0CED1000);
    animB = std112_modelGetResourceID(0x0CED1001);

    std070_characterAttachToCharacter(ctx, modelB, ctx, animA, 0);
    std070_characterAttachToCharacter(ctx, modelA, ctx, animB, 0);

    std115_mapObjectPlayAnimation(modelB, 2, 0, 1);
    std115_mapObjectPlayAnimation(modelA, 2, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008FBF4 */
void fn_8008FBF4(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void GSmodelLinkToGSparticleBank(u32 handle, u32 val);
    extern void GSmodelSetGSparticleLinkAttachMode(u32 handle, u32 val);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06AF0400);
    std115_mapObjectPlayAnimation(iconHandle, 0, 0, 1);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    GSmodelLinkToGSparticleBank(GSresGetResource(ctx, iconHandle), GSresGetResource(ctx, 0x11511400));
    GSmodelSetGSparticleLinkAttachMode(GSresGetResource(ctx, iconHandle), 4);

    cameraPlayAnime(ctx, 0x0CF51800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0CEC1000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x8008FE94 */
void fn_8008FE94(u32 ctx) {
    extern u32 GSresGetResource(u32 ctx, u32 id);
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
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
    u32 iconResult;
    u32 finalResult;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

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

    std116_mapObjectStopAnimation(0x0CE61000);

    material = (GSmaterialEntry *)GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(material->texture, 1);
    material->texture = NULL;

    frame = lbl_8047C1D4;
    handle2 = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelGetFrameCount(handle2, &frame, 0);
    frame = frame - lbl_8047C1D8;
    GSmodelSetAnimIndex(handle2, 0);
    GSmodelSetAnimFrame(handle2, frame);
    GSmodelSetAnimType(handle2, 0);
    GSmodelStartAnimation(handle2);

    iconHandle = std112_modelGetResourceID(0x06BD0400);
    iconResult = GSresGetResource(ctx, iconHandle);
    GSmodelSetShadowFlags(iconResult, 2);
    GSmodelSetShadowLight(iconResult, lbl_8047A690);
    GSmodelSetShadowSurface(iconResult, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF41800, 0, 0);
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

    finalResult = std112_modelGetResourceID(0x0CEB1000);
    std070_characterAttachToCharacter(ctx, iconHandle, ctx, finalResult, 0);
    std115_mapObjectPlayAnimation(iconHandle, 2, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}

/* 0x80090100 */
void fn_80090100(u32 ctx) {
    extern void GSmodelSetShadowTextureSize(u32 w, u32 h);
    extern void GSmodelSetShadowFlags(u32 handle, u32 val);
    extern void GSmodelSetShadowLight(u32 handle, u32 val);
    extern void GSmodelSetShadowSurface(u32 handle, u32 val, u32* param);
    extern void GSmodelGetFrameCount(u32 handle, f32* out, u32 arg);
    extern void GSmodelSetAnimFrame(u32 handle, f32 frame);
    extern void GSmodelSetAnimType(u32 handle, u32 type);
    extern void std116_mapObjectStopAnimation(u32 id);
    extern void std115_mapObjectPlayAnimation(u32 id, u32 slot, u32 x, u32 y);
    extern void fn_80118874(u32 ptr, u32 val);
    extern u32 std112_modelGetResourceID(u32 id);
    extern void std070_characterAttachToCharacter(u32 ctx, u32 modelHandle, u32 ctx2, u32 handle, u32 flags);
    extern void cameraPlayAnime(u32 ctx, u32 id, u32 a, u32 b);
    extern void cameraWaitSyncAnime(s32 sync);

    u32 elapsed1;
    u32 waitFrames1;
    u32 elapsed2;
    u32 waitFrames2;
    u32 elapsed3;
    u32 waitFrames3;
    u32 model;
    u32 base;
    f32 frame;
    u32 h06bc0400;
    u32 h0cea1000;
    u32 h0d290400a;
    u32 h06be0400a;
    u32 h0d240400a;
    u32 h06be0400b;
    u32 h0d240400b;
    u32 h0d290400b;
    u32 h0d240400c;
    u32 h0cea1006;
    u32 h0cea1007;
    u32 h0cea1001;
    u32 h0cea1002;
    u32 h0cea1003;
    u32 h0cea1004;
    u32 h0cea1005;

    lbl_8047A690 = GSresGetResource(ctx, 0x0CE61602);
    lbl_8047A694 = GSresGetResource(ctx, 0x0CE61002);
    GSmodelSetShadowTextureSize(0x280, 0x1E0);
    std115_mapObjectPlayAnimation(0x0CE61000, 0, 0, 0);

    waitFrames1 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames1 = (u32)lbl_8047C1D0;
        if (waitFrames1 < 1) {
            waitFrames1 = 1;
        }
    }
    for (elapsed1 = 0; elapsed1 < waitFrames1; ) {
        _threadSwitch();
        elapsed1 += GSgetFrameStep();
    }

    std116_mapObjectStopAnimation(0x0CE61000);

    waitFrames2 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames2 = (u32)lbl_8047C1D0;
        if (waitFrames2 < 1) {
            waitFrames2 = 1;
        }
    }
    for (elapsed2 = 0; elapsed2 < waitFrames2; ) {
        _threadSwitch();
        elapsed2 += GSgetFrameStep();
    }

    base = GSresGetResource(ctx, 0x0CE61000);
    fn_80118874(*(u32*)(base + 0x144), 1);
    *(u32*)(base + 0x144) = 0;

    frame = lbl_8047C1D4;
    model = GSresGetResource(ctx, 0x0CE61004);
    GSmodelSetAnimIndex(model, 0);
    GSmodelGetFrameCount(model, &frame, 0);
    frame -= lbl_8047C1D8;
    GSmodelSetAnimIndex(model, 0);
    GSmodelSetAnimFrame(model, frame);
    GSmodelSetAnimType(model, 0);
    GSmodelStartAnimation(model);

    h06bc0400 = std112_modelGetResourceID(0x06BC0400);
    h0cea1000 = std112_modelGetResourceID(0x0CEA1000);

    model = GSresGetResource(ctx, h06bc0400);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);

    h0d290400a = std112_modelGetResourceID(0x0D290400);
    h06be0400a = std112_modelGetResourceID(0x06BE0400);
    h0d240400a = std112_modelGetResourceID(0x0D240400);
    h06be0400b = std112_modelGetResourceID(0x06BE0400);
    h0d240400b = std112_modelGetResourceID(0x0D240400);
    h0d290400b = std112_modelGetResourceID(0x0D290400);
    h0d240400c = std112_modelGetResourceID(0x0D240400);

    model = GSresGetResource(ctx, h0d290400a);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400a);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h0d240400a);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h06be0400b);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h0d240400b);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h0d290400b);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);
    model = GSresGetResource(ctx, h0d240400c);
    GSmodelSetShadowFlags(model, 2);
    GSmodelSetShadowLight(model, lbl_8047A690);
    GSmodelSetShadowSurface(model, 1, &lbl_8047A694);

    cameraPlayAnime(ctx, 0x0CF31800, 0, 0);

    waitFrames3 = 1;
    if (fn_800D37CC() == 0x32) {
        waitFrames3 = (u32)lbl_8047C1D0;
        if (waitFrames3 < 1) {
            waitFrames3 = 1;
        }
    }
    for (elapsed3 = 0; elapsed3 < waitFrames3; ) {
        _threadSwitch();
        elapsed3 += GSgetFrameStep();
    }

    h0cea1006 = std112_modelGetResourceID(0x0CEA1006);
    h0cea1007 = std112_modelGetResourceID(0x0CEA1007);
    h0cea1001 = std112_modelGetResourceID(0x0CEA1001);
    h0cea1002 = std112_modelGetResourceID(0x0CEA1002);
    h0cea1003 = std112_modelGetResourceID(0x0CEA1003);
    h0cea1004 = std112_modelGetResourceID(0x0CEA1004);
    h0cea1005 = std112_modelGetResourceID(0x0CEA1005);

    std070_characterAttachToCharacter(ctx, h06bc0400, ctx, h0cea1000, 0);
    std070_characterAttachToCharacter(ctx, h0d290400a, ctx, h0cea1006, 0);
    std070_characterAttachToCharacter(ctx, h06be0400a, ctx, h0cea1007, 0);
    std070_characterAttachToCharacter(ctx, h0d240400a, ctx, h0cea1001, 0);
    std070_characterAttachToCharacter(ctx, h06be0400b, ctx, h0cea1002, 0);
    std070_characterAttachToCharacter(ctx, h0d240400b, ctx, h0cea1003, 0);
    std070_characterAttachToCharacter(ctx, h0d290400b, ctx, h0cea1004, 0);
    std070_characterAttachToCharacter(ctx, h0d240400c, ctx, h0cea1005, 0);

    std115_mapObjectPlayAnimation(h06bc0400, 3, 0, 1);
    std115_mapObjectPlayAnimation(h0d290400a, 4, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400a, 3, 0, 1);
    std115_mapObjectPlayAnimation(h0d240400a, 4, 0, 1);
    std115_mapObjectPlayAnimation(h06be0400b, 3, 0, 1);
    std115_mapObjectPlayAnimation(h0d240400b, 5, 0, 1);
    std115_mapObjectPlayAnimation(h0d290400b, 4, 0, 1);
    std115_mapObjectPlayAnimation(h0d240400c, 4, 0, 1);

    cameraWaitSyncAnime(1);
    floorRequestChange(0x89);
    floorSetFadeScript(0, 0);
}
