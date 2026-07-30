/**
 * @file menu_model.c
 * @brief menuModel -- GSmodel-backed menu 3D model motion/render/free
 *        wrapper (Colosseum "menu" UI subsystem; the only segment of
 *        this bucket that actually touches GSmodel). Split from the
 *        gs_model.c splitter bucket (address range 0x80109894 -
 *        0x80109C88, 4 fns). Corresponds to XD's menuModel.cpp
 *        (0x801121E8-0x80114D30, bounded by the __sinit_menuModel_cpp
 *        marker); menuModelSetMotion is an exact size match. This is a
 *        truncated TU head -- the remainder of menuModel.cpp's
 *        functions live past this bucket's end, in
 *        gs_range_80109C88.c (0x80109C88-0x8010C364).
 */
#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */

/* GSmem */
extern u16   _toolentryAlloc__FUl(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void* fn_800E24B0(u16 handle);                   /* GSmemLock */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */

/* GSgfx state */
extern u8 lbl_8047AA80[];  /* GSgfx state pointer (via sda21) */

/* Matrix math */
extern void  PSMTXIdentity(void);                         /* MTXIdentity */
extern void  PSMTXCopy(void* mtxA, void* mtxB);      /* MTXConcat */
extern void  PSMTXMultVec(void* mtx, void* vec, void* out); /* MTXMultVec3 */

/* Model resource table (BSS) */
extern u8 lbl_80402518[];  /* model resource table -- 0x2400 bytes */

/* Global model system state block */
extern u8 lbl_80404ACC[];

typedef struct GSModelStateHeader {
    u16 count;          /* 0x00 */
    u16 entryHandle;    /* 0x02 */
    u32 unk_04;         /* 0x04 */
    void* entries;      /* 0x08 */
} GSModelStateHeader;

#define GS_MODEL_STATE ((GSModelStateHeader*)lbl_80404ACC)

/* Resource index table */
extern u32 lbl_80478B20;   /* max resource index (sda21) */
extern u8  lbl_80315690[]; /* resource table, 8-byte entries */

/* Additional externs used by various functions */
extern void  GSmodelFree(u32);     /* GSmem release/unref */
extern void  fn_800D2738(void);
extern void* menuDataBiosGetPtr(void);    /* linked list head */
extern void* menuItemBiosGetPtr(s16 idx); /* node by index */
extern void* menuSeBiosGetPtr(s32);
extern u16   fn_8005D798(void*, s32);
extern void* menuSpriteBiosGetPtr(s32);
extern int   fn_80166A28(u16);
extern s32   GSthreadGetCurrentThread(void);    /* poll/yield -- 0 if pending */
extern void  _threadSwitch(void);    /* yield */
extern u32   fn_800BE31C(void);    /* rand or tick */
extern u32   fn_800B8FD8(void*);   /* register fn, returns handle */
extern void  fn_800BD91C(s32, s32);
extern void  fn_800B8C58(s32);
extern void  GSgfxBeginBackFBCapture(u32, void*, void*);
extern u32   GStextureCreate(s32, s32, s32, s32, s32);
extern u32   GSmodelCanAnimate(u32);
extern void  GSmodelSetAnimIndex(u32, u32);
extern void  GSmodelSetAnimRate(u32, f32);
extern void  GSmodelStartAnimation(u32);
extern void  fn_801DB100(u32);
extern u32   OSGetTick(void);

extern u32 lbl_8047ACF0;
extern u32 lbl_8047ACF4;  /* function pointer for callback */
extern u32 lbl_8047ACF8;  /* saved tick */
extern u32 lbl_8047ACEC;  /* tick counter */
extern u32 lbl_8047ACE8;  /* tick base */
extern u8 lbl_80478B28;   /* max slot byte */
extern u8 lbl_8047AD20;
extern u8 lbl_8047AD21;
extern u8 lbl_8047AD22;
extern u8 lbl_8047AD23;
extern u8 lbl_8047AD24;
extern f32 lbl_8047AD2C;
extern f32 lbl_8047AD30;
extern f32 lbl_8047AD34;
extern f32 lbl_8047AD38;
extern f32 lbl_8047AD3C;
extern u32 lbl_8047AD28;
extern u16 lbl_8047AD18;  /* GSmem handle */
extern u8* lbl_8047AD1C;  /* object pool pointer */
extern f32 lbl_8047CDC0;  /* sdata2: float constant */
extern f32 lbl_8047CDC4;  /* sdata2: float constant */
extern u16 lbl_8047CDE0;  /* sdata2: */
extern u16 lbl_8047CDE4;  /* sdata2: */
extern f32 lbl_8047CD80;  /* sdata2: float constant */
extern f32 lbl_8047CD84;  /* sdata2: float constant */
extern f32 lbl_8047CD88;  /* sdata2: float constant */
extern f32 lbl_8047CD8C;  /* sdata2: float constant */
extern f32 lbl_8047CD90;  /* sdata2: float constant */
extern f32 lbl_8047CD94;  /* sdata2: float constant */
extern f64 lbl_8047CD98;  /* sdata2: double constant */
extern f32 lbl_8047CE3C;  /* sdata2: float constant */
extern f32 lbl_8047CE50;  /* sdata2: float constant */
extern f32 lbl_8047CE5C;  /* sdata2: float constant */
extern f32 lbl_8047CE70;  /* sdata2: float constant */
extern u8  lbl_80404A98[];  /* table for display */
extern u8  lbl_80271E10[];  /* format string */
extern u8  lbl_80271E4C[];  /* format string */
extern u8  lbl_80271EE8[];  /* format string */
extern u8  lbl_80271F18[];  /* format string */
extern u8  lbl_8035B060[];  /* module name string */
extern u8  lbl_8035B3F0[];  /* module name string */

/* Additional external functions (not already declared above) */
extern void fn_800BF74C(void);
extern void fn_800D9ED8(s32);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800D9B58(f32, f32, f32, f32);
extern void fn_800DA4C4(s32, s32, s32);
extern void fn_800DA2BC(s32, s32, s32);
extern void fn_800DA1E8(s32, s32, s32);
extern void fn_800DA028(s32);
extern void fn_800D6A00(s32);
extern void fn_800D7820(s32);
extern void fn_800D67BC(s32);
extern void fn_800D6680(f32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D6728(void);
extern u32 fn_801DAC3C(u32 sequence);
extern void fn_800EC134(u32 model);
extern void GSmodelSetPEdescr(u32 model, void* descriptor);
extern void GSmodelResetPEdescr(u32 model);
extern void GSmodelDrawModel(u32 model, u32 flags);
extern void GSlightSetActive(u32 light, u32 active);
extern u32 GScameraGetActiveCamera(void);
extern void fn_800D4604(u32 layer);
extern void fn_800D377C(u32 state);
extern void fn_800D9B24(u16* left, u16* top, u16* right, u16* bottom);
extern void fn_800D9AF0(u16* left, u16* top, u16* right, u16* bottom);
extern void fn_800D258C(u32 camera);
extern u32 GStextureGetXsize(u32 texture);
extern u32 GStextureGetYsize(u32 texture);
extern void fn_800D9D68(u16 left, u16 top, u16 right, u16 bottom);
extern void fn_800D9C24(u16 left, u16 top, u16 right, u16 bottom);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern void fn_800D3410(u32 texture, u32 slot);
extern void fn_801DA4E8(u32 sequence, u32 active);
extern void fn_801DA448(u32 sequence, u32 active);
extern void fn_801DB088(void);
extern void fn_801DAAAC(u32 sequence);
extern void fn_800D3190(void);
extern u8 lbl_8035B468[];

/* Forward declarations for functions defined later in this TU */
extern u8    menuOffScreenCheckEnable(u8 param);
extern void  windowClose(void* ptr, u32 flags);
extern void* windowSearchID(s32 param);
extern s32   _menuCBOffScreen__FP9GStextureUlPv(void);
extern void  winSpriteSetDisp(void* node, u32 enable);
extern void  windowGetValue(s32 param);
extern void  windowCheckCursor(void* p, u8 flags);
extern void  windowDrawSprite2(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10);
extern u8    menuOffScreenFadeSync(u8 param);
extern void  menuOffScreenFadeSet(f32 f1, f32 f2);
extern u8    menuOffScreenCreate(u32 param);
extern void  menuOffScreenRelease(void);
extern u32   windowGetActiveID(void);
extern void* windowGetKeyInfo(void);
extern void* menuSeqBiosGetPtr(u32 idx);
extern void* windowSearchItemID(void* head, s32 key);
extern void  menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
extern u8    menuOffScreenSetPriority(u8 val);
extern u8    menuOffScreenSetDisp(u8 val);
extern u32   fn_800D3088(void);
extern u8    lbl_80404B68[];  /* scratch table for fn_80107F38, fn_801081F8 */
extern u8    lbl_80404B8C[];  /* scratch table for winSeqSetMenu */
extern u8    lbl_8047AD10;     /* resource request gate byte (sda21) - authoritative decl, use as-is */

/* Cross-segment prototypes for menu/window-engine functions defined in
 * sibling segment files split from the gs_model.c splitter bucket. */
extern void fn_801019F8(void);
extern void* kaisuuBiosGetMax(u32 index);
extern void* kaisuuBiosGetMin(u32 index);
extern u32 kaisuuGetKaisuu(u32 param);
extern void fn_80101B34(u32 param);
extern void fn_80101B88(u32 val);
extern void fn_80101B90(void);
extern void fn_80101D5C(void);
extern void fn_80101D8C(void);
extern void fn_80101FB8(u8 param);
extern void menuGetOffScreenFlag(void);
extern void menuReleaseOffScreen(f32 f1);
extern void menuCreateOffScreen(f32 param);
extern s32 menuGetSelectItemNum(void);
extern s32 menuGetCursorFromItemID(void* unused, u32 param);
extern void fn_801021F8(void* p, u32 val);
extern void menuSetDisp(void* p, u32 enable);
extern void* menuGetCursorItemID(void* p, u32 target);
extern s32 menuSetCursor(void* p, u32 val);
extern s32 menuGetCursor(void* p);
extern s32 menuCloseSync(void* p, u8 flag);
extern void menuCloseFloor(void);
extern void fn_801024E8(void);
extern void menuClose(s32 p);
extern s32 menuCloseCustom(void* p, u32 mode, u8 wait);
extern s32 menuIsCheck(s32 param);
extern void menuOpen(void* p, void* q);
extern void menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
extern void menuSetPosition(void* p, s16 a, s16 b);
extern void menuButtonNormal(void* p);
extern void menuPlaySe(void* p, void* q);
extern void fn_801034DC(void);
extern void _menuGetGcKeyInfo__FlPUs(void);
extern void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(void);
extern void menuGetKeyInfo(void);
extern u8 menuGetEnablePort(void);
extern u8 menuSetEnablePort(u8 val);
extern void menuInit(void);
extern u32 cursorBiosGetPos(u16 idx);
extern u32 cursorBiosSetPos(u16 idx, u16* out);
extern void cursorBiosInit(void);
extern void fn_80103F74(void* head, u16 key, u32 data);
extern void* windowGetAllocPtr(void* ptr);
extern void* windowAllocMemory(void* p, s32 size);
extern void* windowGetFreeWork(void* ptr);
extern void windowSetParam(void* ptr, u32 idx, u32 val);
extern u32 windowGetParam(void* ptr, u32 idx);
extern void windowDrawSprite(void* p, void* a, void* b, u16 key, u32 data);
extern void windowDrawSprite2(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10);
extern u8* windowGetCursorToItem(u8* arg);
extern void windowGetValue(s32 param);
extern s32 fn_801044D0(s32 param, u16* val);
extern void windowGetCursor(void);
extern void windowCheckCursor(void* p, u8 flags);
extern u32 windowGetActiveID(void);
extern void windowCloseMain(void* obj);
extern void windowClose(void* ptr, u32 flags);
extern void _windowCreateItemSprite__FP14tagWINDOW_WORK(void);
extern void windowCreateCursorSprite(void);
extern void windowOpen(void);
extern void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(void);
extern void windowInit(u16 count);
extern void windowGetPortKeyInfo(void);
extern void fn_80105634(void);
extern void winMsgDraw(void);
extern void winMsgCtrl(void);
extern void winMsgButton(void* p);
extern void winMsgCloseLevelUpStatus(void);
extern void winMsgOpenLevelUpFiledStatus(void);
extern void winMsgOpenLevelUpStatus(void);
extern void winMsgCloseError(void);
extern void winMsgOpenError(void);
extern void winMsgCloseFight(void);
extern void winMsgCloseCheckFight(void);
extern void winMsgOpenFightNoWait(void);
extern void winMsgOpenFight(void);
extern void winMsgCheckField(void);
extern void winMsgCloseField(void);
extern void winMsgOpenFieldWithSE(void);
extern void winMsgOpenField(void);
extern void winMsgCheck(void);
extern void winMsgClose(void);
extern void winMsgOpenWithSE(void);
extern void winMsgOpen(void);
extern void fn_80106F98(void);
extern s32 winSeqCheckMove(s32 param);
extern s32 fn_80107170(s32 r3, s32 r31);
extern void winSeqMoveMenu(void);
extern s32 fn_80107E78(void* r3, s32 r4, u16 r30);
extern s32 winSeqIsCheck(s32 r3, u16 r30);
extern void fn_80107F38(s32 param, u32 key);
extern void winSeqSetMenu(s32 param, u32 key);
extern void fn_801081F8(void* r3_arg, u16 r4, u16 r5);
extern void winSetSequence(void* out, u32 idx);
extern s32 winSpriteGetDisp(void* ptr);
extern void winSpriteSetDisp(void* node, u32 enable);
extern void winSpriteRelease(void* head);
extern void* fn_80109290(void* root);
extern void winSpriteInit(void);
extern void fn_801093C8(void);
extern u8 menuOffScreenFadeSync(u8 param);
extern void menuOffScreenFadeSet(f32 f1, f32 f2);
extern u8 menuOffScreenSetPriority(u8 val);
extern u8 menuOffScreenSetDisp(u8 val);
extern u8 menuOffScreenIsDoing(void);
extern u32 menuOffScreenGetPtr(void);
extern u8 menuOffScreenCheckEnable(u8 param);
extern void menuOffScreenRelease(void);
extern u8 menuOffScreenCreate(u32 param);
extern void menuOffScreenInit(void);
extern s32 _menuCBOffScreen__FP9GStextureUlPv(void);
extern s32 menuModelSetMotion(void* p, u32 val);
extern u32 menuModelRender(u8* work);
extern s32 menuModelCheck(void* obj, u8 wait);
extern s32 menuModelFree(void* p);

/* 0x80109894 | 0xA0 */
#pragma push
#pragma peephole off
s32 menuModelSetMotion(void* p, u32 val) {
    void* r31 = p;
    if (r31 == (void*)0) { return 0; }
    if (*(u8*)((u8*)r31 + 0x1) != 0) {
        if (*(u8*)((u8*)r31 + 0x4) == 0) {
            *(u32*)((u8*)r31 + 0xc) = val;
        }
    } else {
        if (*(u8*)((u8*)r31 + 0x14) == 0) {
            *(u32*)((u8*)r31 + 0x1c) = val;
            {
                u32 r3 = *(u32*)((u8*)r31 + 0x24);
                if ((u8)GSmodelCanAnimate(r3) != 0) {
                    GSmodelSetAnimIndex(*(u32*)((u8*)r31 + 0x24), *(u32*)((u8*)r31 + 0x1c));
                    GSmodelSetAnimRate(*(u32*)((u8*)r31 + 0x24), lbl_8047CE70);
                    GSmodelStartAnimation(*(u32*)((u8*)r31 + 0x24));
                }
            }
        }
    }
    return 1;
}
#pragma pop

/* 0x80109934 | 0x25C */
#pragma peephole off
u32 menuModelRender(u8* work) {
    u32 model;
    u32 renderModel;
    u32 previousCamera;
    u16 viewLeft;
    u16 viewTop;
    u16 viewRight;
    u16 viewBottom;
    u16 scissorLeft;
    u16 scissorTop;
    u16 scissorRight;
    u16 scissorBottom;
    u16 width;
    u16 height;
    s32 i;

    if (work == NULL) {
        return 0;
    }
    if (*(u32*)(work + 0x34) == 0) {
        return 0;
    }
    if (work[0] != 2) {
        return 0;
    }
    model = *(u32*)(work + 0x24);
    renderModel = work[0x14] != 0 ? fn_801DAC3C(model) : model;
    if (renderModel == 0) return 0;

    fn_800EC134(renderModel);
    GSmodelSetPEdescr(renderModel, lbl_8035B468);
    for (i = 0; i < 3; i++) {
        u32 light = *(u32*)(work + 0x3C + i * 4);
        if (light != 0) GSlightSetActive(light, 1);
    }
    previousCamera = GScameraGetActiveCamera();
    fn_800D4604(2);
    fn_800D377C(2);
    fn_800D9B24(&viewLeft, &viewTop, &viewRight, &viewBottom);
    fn_800D9AF0(&scissorLeft, &scissorTop, &scissorRight, &scissorBottom);
    fn_800D258C(*(u32*)(work + 0x38));
    fn_800D258C(*(u32*)(work + 0x38));
    width = (u16)(GStextureGetXsize(*(u32*)(work + 0x34)) - 1);
    height = (u16)(GStextureGetYsize(*(u32*)(work + 0x34)) - 1);
    fn_800D9D68(0, 0, width, height);
    fn_800D9C24(0, 0, width, height);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    fn_800D3410(*(u32*)(work + 0x34), 0);
    if (work[0x14] != 0) {
        fn_801DA4E8(model, 1);
        fn_801DA448(model, 1);
        fn_801DB088();
        fn_801DAAAC(model);
        fn_801DA4E8(model, 0);
        fn_801DA448(model, 0);
    } else {
        GSmodelDrawModel(model, 0x3010);
    }
    fn_800D3190();
    fn_800D377C(1);
    fn_800D258C(previousCamera);
    fn_800D9D68(viewLeft, viewTop, viewRight, viewBottom);
    fn_800D9C24(scissorLeft, scissorTop, scissorRight, scissorBottom);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
    fn_800D4604(1);
    for (i = 0; i < 3; i++) {
        u32 light = *(u32*)(work + 0x3C + i * 4);
        if (light != 0) GSlightSetActive(light, 0);
    }
    GSmodelResetPEdescr(renderModel);
    return *(u32*)(work + 0x34);
}
#pragma peephole reset

/* 0x80109B90 | 0x6C */
#pragma push
#pragma peephole off
s32 menuModelCheck(void* obj, u8 wait) {
    void* r30 = obj;
    if (r30 == (void*)0) { return 0; }
    {
        u8 r31 = (u8)wait;
        goto check;
    check:
        if (*(u8*)r30 != 1) { return 0; }
        if (r31 == 0) { goto done; }
        _threadSwitch();
        goto check;
    done:
        return 1;
    }
}
#pragma pop

/* 0x80109BFC | 0x8C */
#pragma push
#pragma peephole off
s32 menuModelFree(void* p) {
    void* r31 = p;
    if (r31 == (void*)0) { return 0; }
    *(u8*)((u8*)r31 + 0x1) = 0;
    if (*(u8*)((u8*)r31 + 0x14) != 0) {
        /* has some kind */
        if (*(void**)((u8*)r31 + 0x24) != (void*)0) {
            fn_801DB100(*(u32*)((u8*)r31 + 0x24));
            *(u32*)((u8*)r31 + 0x24) = 0;
        }
    } else {
        if (*(void**)((u8*)r31 + 0x24) != (void*)0) {
            GSmodelFree(*(u32*)((u8*)r31 + 0x24));
            *(u32*)((u8*)r31 + 0x24) = 0;
        }
    }
    *(u8*)((u8*)r31 + 0x0) = 0;
    return 1;
}
#pragma pop
