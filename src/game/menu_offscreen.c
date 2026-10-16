/**
 * @file menu_offscreen.c
 * @brief menuOffScreen -- back-framebuffer capture / fade-transition
 *        offscreen texture (Colosseum "menu" UI subsystem). Split from
 *        the gs_model.c splitter bucket (address range 0x801093C8 -
 *        0x80109894, 12 fns). Corresponds to XD's menuOffScreen.cpp
 *        (0x801145EC-0x80114D30); 8 anchor symbols strictly monotonic,
 *        with _menuCBOffScreen (segment's and XD TU's last fn) an exact
 *        size match. menuOffScreenInit is called by menuInit (menu.c).
 *
 *        fn_801093C8 is XD's first function here, menuOffScreenDraw
 *        (TeamOrre/xd-decomp config/GXXE01/symbols.txt @4989794e,
 *        0x801145EC): it drives the same fade state (lbl_8047AD2C..AD3C).
 *        The TU owns the .sdata2 literal pool 0x8047CE48-0x8047CE70
 *        (255, 100, 0, 640, 480, 1, and the two int-to-float doubles), in
 *        first-use order; menuOffScreenFadeSet/Create/Init share its 0.0f
 *        and 1.0f.
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
extern int   soundStartSE(u16);
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
/* Capture-done flag: set by _menuCBOffScreen from the back-framebuffer
 * capture callback and polled by menuOffScreenCheckEnable, so it is
 * volatile (every read reloads it). */
extern volatile u8 lbl_8047AD20;
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
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void gfxSetOrtho(f32, f32, f32, f32);
extern void gfxSetBlendMode(s32, s32, s32);
extern void gfxSetUpdates(s32, s32, s32);
extern void gfxSetZMode(s32, s32, s32);
extern void gfxSetCullMode(s32);
extern void gfxSetPrimitive(s32);
extern void gfxSetVtxDesc(s32);
extern void gfxBegin(s32);
extern void gfxVtxPos(f32, f32, f32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void gfxEnd(void);

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
extern u32   GSgetFrameStep(void);
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
extern void menuModelRender(void);
extern s32 menuModelCheck(void* obj, u8 wait);
extern s32 menuModelFree(void* p);

extern u8 lbl_80314AE8[];
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void gfxEnd(void);
extern void gfxLoadTexture(s32, void*);
extern void gfxVtxTexCoord(s32, f32, f32);
extern s32 fn_800D37CC(void);
extern void fn_800D9F40(s32);
extern u32 GSgetFrameStep(void);

/* 0x801093C8 | 0x29C */
void fn_801093C8(void) {
    s32 alpha;
    f32 progress;

    if (lbl_8047AD24 != 0) {
        lbl_8047AD3C += (f32)GSgetFrameStep() / (f32)fn_800D37CC();
        if (lbl_8047AD3C >= lbl_8047AD38) {
            lbl_8047AD3C = lbl_8047AD38;
            lbl_8047AD24 = 0;
        }
    }
    if (lbl_8047AD22 == 0) return;
    if (lbl_8047AD20 == 0) return;

    progress = lbl_8047AD2C +
        (lbl_8047AD3C / lbl_8047AD38) * (lbl_8047AD34 - lbl_8047AD2C);
    lbl_8047AD30 = progress;
    alpha = (s32)(255.0f -
        (255.0f * progress) / 100.0f);

    if (lbl_8047AD23 != 0) {
        fn_800D9ED8(1);
    } else {
        fn_800D9ED8(0);
    }
    gfxSetFlags(3);
    gfxClearFlags(4);
    gfxSetOrtho(0.0f, 0.0f,
                640.0f, 480.0f);
    gfxSetBlendMode(0, 1, 1);
    gfxSetUpdates(1, 1, 1);
    gfxSetZMode(1, 2, 1);
    gfxSetCullMode(0);
    gfxSetPrimitive(4);
    gfxSetVtxDesc((s32)lbl_80314AE8);
    gfxLoadTexture(0, (void*)lbl_8047AD28);
    gfxBegin(4);

    gfxVtxPos(0.0f, 0.0f, 0.0f);
    fn_800D5CB8(0, alpha, alpha, alpha, 0xFF);
    gfxVtxTexCoord(0, 0.0f, 0.0f);
    gfxVtxPos(640.0f, 0.0f, 0.0f);
    fn_800D5CB8(0, alpha, alpha, alpha, 0xFF);
    gfxVtxTexCoord(0, 1.0f, 0.0f);
    gfxVtxPos(0.0f, 480.0f, 0.0f);
    fn_800D5CB8(0, alpha, alpha, alpha, 0xFF);
    gfxVtxTexCoord(0, 0.0f, 1.0f);
    gfxVtxPos(640.0f, 480.0f, 0.0f);
    fn_800D5CB8(0, alpha, alpha, alpha, 0xFF);
    gfxVtxTexCoord(0, 1.0f, 1.0f);
    gfxEnd();
    if (lbl_8047AD23 == 0) fn_800D9F40(0);
}

/* 0x80109664 | 0x48 */
u8 menuOffScreenFadeSync(u8 wait) {
    while (wait) {
        if (lbl_8047AD24 == 0) {
            break;
        }
        _threadSwitch();
    }
    return lbl_8047AD24;
}

/* 0x801096AC | 0x3C */
void menuOffScreenFadeSet(f32 target, f32 time) {
    lbl_8047AD24 = 1;
    lbl_8047AD2C = lbl_8047AD30;
    lbl_8047AD34 = target;
    lbl_8047AD30 = 0.0f;
    lbl_8047AD38 = time;
    lbl_8047AD3C = 0.0f;
    if (time == 0.0f) {
        lbl_8047AD3C = lbl_8047AD38 = 1.0f;
    }
}

/* 0x801096E8 | 0x10 */
u8 menuOffScreenSetPriority(u8 val) {
    u8 old = lbl_8047AD23;
    lbl_8047AD23 = val;
    return old;
}

/* 0x801096F8 | 0x10 */
u8 menuOffScreenSetDisp(u8 val) {
    u8 old = lbl_8047AD22;
    lbl_8047AD22 = val;
    return old;
}

/* 0x80109708 | 0x8 | sda_getter */
u8 menuOffScreenIsDoing(void) { return lbl_8047AD21; }

/* 0x80109710 | 0x8 | sda_getter */
u32 menuOffScreenGetPtr(void) { return lbl_8047AD28; }

/* 0x80109718 | 0x4C */
u8 menuOffScreenCheckEnable(u8 wait) {
    while (wait) {
        if (lbl_8047AD20 != 0) {
            break;
        }
        _threadSwitch();
    }
    return lbl_8047AD20;
}

/* 0x80109764 | 0x18 */
void menuOffScreenRelease(void) {
    lbl_8047AD24 = 0;
    lbl_8047AD21 = 0;
    lbl_8047AD20 = 0;
    lbl_8047AD22 = 0;
}

/* 0x8010977C | 0x94 */
u8 menuOffScreenCreate(u32 wait) {
    lbl_8047AD24 = 0;
    lbl_8047AD21 = 1;
    lbl_8047AD20 = 0;
    lbl_8047AD22 = 0;
    lbl_8047AD2C = 0.0f;
    lbl_8047AD30 = 0.0f;
    lbl_8047AD34 = 0.0f;
    lbl_8047AD38 = 0.0f;
    lbl_8047AD3C = 0.0f;
    GSgfxBeginBackFBCapture(lbl_8047AD28, _menuCBOffScreen__FP9GStextureUlPv, (void*)0);
    return menuOffScreenCheckEnable(wait);
}

/* 0x80109810 | 0x74 */
void menuOffScreenInit(void) {
    if (lbl_8047AD28 == 0) {
        lbl_8047AD28 = GStextureCreate(0, 0, 0x44, 0, 0);
    }
    lbl_8047AD21 = 0;
    lbl_8047AD20 = 0;
    lbl_8047AD22 = 0;
    lbl_8047AD23 = 0;
    lbl_8047AD24 = 0;
    lbl_8047AD38 = 0.0f;
    lbl_8047AD3C = 0.0f;
    lbl_8047AD2C = 0.0f;
    lbl_8047AD30 = 0.0f;
    lbl_8047AD34 = 0.0f;
}

/* 0x80109884 | 0x10 */
s32 _menuCBOffScreen__FP9GStextureUlPv(void) {
    lbl_8047AD20 = 1;
    return 0;
}

