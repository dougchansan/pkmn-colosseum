/**
 * @file win_msg.c
 * @brief winMsg -- message-window open/close/check family for battle,
 *        field, and level-up status dialogs (Colosseum "menu" UI
 *        subsystem). Split from the gs_model.c splitter bucket (address
 *        range 0x801058CC - 0x80106F98, 20 fns). Corresponds to XD's
 *        winMsg.cpp (0x80116BD4-0x80117614); anchors winMsgCtrl/
 *        winMsgButton are monotonic. The winMsgOpen / winMsgClose /
 *        winMsgCheck farm calls the menu open/close/isCheck trio
 *        (menu.c).
 */
#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern void  fn_800FE6D0(s16 x, s16 y);
extern void  spriteSetEnv(void);
extern void  fn_8001EC08(s32, s32, s16, s16, u8, s32);
extern void  fn_800FBE7C(void* message, u32 state, u8 flag);
extern void  fn_800FBD88(void* message);
extern u32   GSmsgGetRect(void* message);
extern void  GSmsgExec(void* message, u8 a, u8 b);
extern s32   GSmsgIsCheck(void* message);

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
extern s32   menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
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
extern s32 menuCloseCustom(void* p, u32 mode, u32 wait);
extern s32 menuIsCheck(s32 param);
extern void menuOpen(void* p, void* q);
extern s32 menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
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
extern s32 winMsgDraw(u8* window, u8* item);
extern s32 winMsgCtrl(u8* window);
extern void winMsgButton(void* p);
extern void winMsgCloseLevelUpStatus(u32 wait);
extern s32 winMsgOpenLevelUpFiledStatus(u32 message, u32 wait);
extern s32 winMsgOpenLevelUpStatus(u32 message, u32 wait);
extern void winMsgCloseError(void);
extern s32 winMsgOpenError(u32 message, u32 mode, u8 wait);
extern void winMsgCloseFight(u32 wait);
extern u8 winMsgCloseCheckFight(void);
extern s32 winMsgOpenFightNoWait(u32 message, u32 wait, u8 pause);
extern s32 winMsgOpenFight(u32 message, u32 wait, u8 pause);
extern s32 winMsgCheckField(void);
extern void winMsgCloseField(u32 wait);
extern s32 winMsgOpenFieldWithSE(u32 message, u32 wait, u8 pause, u8 sound);
extern s32 winMsgOpenField(u32 message, u32 wait, u8 pause);
extern s32 winMsgCheck(void);
extern void winMsgClose(u32 wait);
extern s32 winMsgOpenWithSE(s8 type, u32 message, u32 wait, u8 pause, u8 sound);
extern s32 winMsgOpen(s8 type, u32 message, u32 wait, u8 pause);
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

extern s8 lbl_80478B30;

static inline s32 winMsgGetMenuId(s8 status)
{
    switch (status) {
    case 0: return 0x40;
    case 1: return 0x0F;
    case 2: return 0x10;
    case 3: return 0x40;
    case 4: return 0x50;
    case 5: return 0x51;
    case 6: return 0x10C;
    case 7: return 0xE6;
    case 8: return 0xE8;
    case 9: return 0x107;
    default: return 0;
    }
}

static inline s32 winMsgResolveMenuId(s8 status)
{
    s32 id = 0;

    switch (status) {
    case 0: id = 0x40; break;
    case 1: id = 0x0F; break;
    case 2: id = 0x10; break;
    case 3: id = 0x40; break;
    case 4: id = 0x50; break;
    case 5: id = 0x51; break;
    case 6: id = 0x10C; break;
    case 7: id = 0xE6; break;
    case 8: id = 0xE8; break;
    case 9: id = 0x107; break;
    }
    return id;
}

static inline void winMsgCloseCurrent(u8 wait)
{
    s32 id;

    if (lbl_8047AD10 != 0) {
        return;
    }
    id = winMsgGetMenuId(lbl_80478B30);
    if ((u8)menuIsCheck(id)) {
        menuCloseCustom((void*)id, 2, wait);
    }
}

static inline s32 winMsgOpenLevelUp(u32 message, u32 wait, s32 id, s8 status)
{
    if (id == 0) {
        return 0;
    }
    if (lbl_80478B30 != status && lbl_8047AD10 == 0) {
        s32 old_id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(old_id)) {
            menuCloseCustom((void*)old_id, 2, wait);
        }
    }
    lbl_80478B30 = status;
    return menuOpenCustom((void*)id, windowGetActiveID(), 0, 0,
                          (void*)wait, 3, message, 1, 0);
}

/* 0x801058CC | 0x170 */
s32 winMsgDraw(u8* window, u8* item) {
    u8* work = windowGetFreeWork(window);
    s16 x;
    s16 y;
    s16 left;
    s16 top;

    if (*(s32*)(window + 0x04) == 0x50) {
        x = *(s16*)(work + 0x0C);
        y = *(s16*)(work + 0x0E);
    } else {
        x = *(s16*)(item + 0x54);
        y = *(s16*)(item + 0x56);
    }

    left = *(s16*)(window + 0x84) + *(s16*)(item + 0x50);
    top = *(s16*)(window + 0x86) + *(s16*)(item + 0x52)
        + (*(s16*)(item + 0x56) - 4 - y);
    fn_800FE6D0(left, top);
    spriteSetEnv();

    switch (*(s32*)(window + 0x04)) {
    case 0x40:
    case 0x50:
    case 0x51:
        fn_8001EC08(0, 2, x, y, window[0x8B], 1);
        break;
    case 0x10C:
        fn_8001EC08(0, 2, x, y, window[0x8B], 0);
        break;
    }

    switch ((s8)window[1]) {
    case 2:
        if (*(s32*)(window + 0x04) != 0x51) {
            fn_800FBE7C(*(void**)work, *(u32*)(work + 0x08),
                        (window[0] & 4) == 0);
            *(u32*)(work + 0x08) = 0;
        }
        break;
    case 0:
    case 3:
    case 5:
        break;
    }
    return 0;
}

/* 0x80105A3C | 0x1F4 */
#pragma peephole off
s32 winMsgCtrl(u8* window) {
    u8* work = windowGetFreeWork(window);
    u8 ready = 0;
    u32 rect;

    switch ((s8)window[1]) {
    case 0:
        if (*(void**)work != NULL) {
            fn_800FBD88(*(void**)work);
        }
        *(u32*)work = windowGetParam(window, 0);
        work[4] = (u8)windowGetParam(window, 1);
        work[5] = (u8)windowGetParam(window, 2);

        if ((s8)window[2] == 0) {
            ready = 1;
        }
        if ((u8)winSeqIsCheck(*(s32*)(window + 0x04), 0x2A) == 1) {
            ready = 1;
        }
        if (window[0x0A] != 0) {
            ready = 1;
        }
        if (ready != 0) {
            winSeqSetMenu(*(s32*)(window + 0x04),
                          *(s32*)(window + 0x04) == 0x40 ? 0x26 : 0x22);
            window[2] = 1;
        }
        if (*(s32*)(window + 0x04) == 0x50) {
            rect = GSmsgGetRect(*(void**)work);
            *(s16*)(work + 0x0C) = (s16)(rect >> 16);
            *(s16*)(work + 0x0E) = (s16)rect;
        }
        break;
    case 2:
        if ((s8)window[2] == 0) {
            GSmsgExec(*(void**)work, work[4], work[5]);
            window[2] = 1;
        }
        if (GSmsgIsCheck(*(void**)work) != 0) {
            window[0x98] = 0;
        } else {
            window[0x98] = 1;
        }
        break;
    case 3:
        if ((s8)window[2] == 0) {
            winSeqSetMenu(*(s32*)(window + 0x04), 0x2A);
            window[2] = 1;
        }
        break;
    case 5:
        fn_800FBD88(*(void**)work);
        *(void**)work = NULL;
        break;
    }
    return 0;
}
#pragma peephole reset

/* 0x80105C30 | 0x38 */
void winMsgButton(void* p) {
    void* r31 = windowGetFreeWork(p);
    void* r3 = windowGetKeyInfo();
    *(u32*)((u8*)r31 + 0x8) = *(u16*)((u8*)r3 + 0x4);
}

/* 0x80105C68 | 0xE0 */
#pragma peephole off
void winMsgCloseLevelUpStatus(u32 wait) {
    if (lbl_8047AD10 == 0) {
        s32 id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(id)) {
            menuCloseCustom((void*)id, 2, wait);
        }
        lbl_80478B30 = -1;
    }
}
#pragma peephole reset

/* 0x80105D48 | 0x134 */
#pragma peephole off
s32 winMsgOpenLevelUpFiledStatus(u32 message, u32 wait) {
    return winMsgOpenLevelUp(message, wait, 0x107, 9);
}
#pragma peephole reset

/* 0x80105E7C | 0x134 */
#pragma peephole off
s32 winMsgOpenLevelUpStatus(u32 message, u32 wait) {
    return winMsgOpenLevelUp(message, wait, 0x51, 5);
}
#pragma peephole reset

/* 0x80105FB0 | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void winMsgCloseError(void) {
#pragma peephole off
#pragma scheduling off
    if ((u8)menuIsCheck(0x10C)) {
        menuCloseCustom((void*)0x10C, 2, 0);
    }
    lbl_8047AD10 = 0;
}
#pragma pop

/* 0x80105FF8 | 0x88 */
s32 winMsgOpenError(u32 message, u32 mode, u8 wait) {
    s32 id = 0x10C;
    s32 result = 0;
    u8 flags = 0;

    if (id == 0) {
        return result;
    }
    lbl_8047AD10 = 1;
    if (wait != 0) {
        flags |= 1;
    }
    flags |= 2;
    result = menuOpenCustom((void*)id, -1, 0, 0, NULL, 3, message, flags, 0);
    return result;
}

/* 0x80106080 | 0xE0 */
#pragma peephole off
void winMsgCloseFight(u32 wait) {
    if (lbl_8047AD10 == 0) {
        s32 id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(id)) {
            menuCloseCustom((void*)id, 2, wait);
        }
        lbl_80478B30 = -1;
    }
}
#pragma peephole reset

/* 0x80106160 | 0xE4 */
u8 winMsgCloseCheckFight(void) {
    s32 id = winMsgResolveMenuId(lbl_80478B30);
    u8* window = windowSearchID(id);
    s8 result;

    if (window == NULL) {
        result = -1;
    } else if (window[0x98] != 0) {
        result = 0;
    } else if (window[0x99] != 0) {
        result = 0;
    } else {
        result = 1;
    }
    return result >= 0;
}

/* 0x80106244 | 0x150 */
s32 winMsgOpenFightNoWait(u32 message, u32 wait, u8 pause) {
    s32 id = 0x50;
    u8 flags = 0;

    if (id == 0) {
        return 0;
    }
    if (lbl_80478B30 != 4 && lbl_8047AD10 == 0) {
        s32 old_id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(old_id)) {
            menuCloseCustom((void*)old_id, 2, wait);
        }
    }
    lbl_80478B30 = 4;
    if (pause != 0) {
        flags |= 1;
    }
    flags |= 2;
    return menuOpenCustom((void*)id, windowGetActiveID(), 0, 0,
                          (void*)wait, 3, message, flags, 0);
}

/* 0x80106394 | 0x14C */
s32 winMsgOpenFight(u32 message, u32 wait, u8 pause) {
    s32 id = 0x50;
    u8 flags = 0;

    if (id == 0) {
        return 0;
    }
    if (lbl_80478B30 != 4 && lbl_8047AD10 == 0) {
        s32 old_id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(old_id)) {
            menuCloseCustom((void*)old_id, 2, wait);
        }
    }
    lbl_80478B30 = 4;
    if (pause != 0) {
        flags |= 1;
    }
    return menuOpenCustom((void*)id, windowGetActiveID(), 0, 0,
                          (void*)wait, 3, message, flags, 0);
}

/* 0x801064E0 | 0xD8 */
s32 winMsgCheckField(void) {
    s32 id = winMsgResolveMenuId(lbl_80478B30);
    u8* window = windowSearchID(id);
    s8 result;

    if (window == NULL) {
        result = -1;
    } else if (window[0x98] != 0) {
        result = 0;
    } else if (window[0x99] != 0) {
        result = 0;
    } else {
        result = 1;
    }
    return result;
}

/* 0x801065B8 | 0xE0 */
#pragma peephole off
void winMsgCloseField(u32 wait) {
    if (lbl_8047AD10 == 0) {
        s32 id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(id)) {
            menuCloseCustom((void*)id, 2, wait);
        }
        lbl_80478B30 = -1;
    }
}
#pragma peephole reset

/* 0x80106698 | 0x150 */
s32 winMsgOpenFieldWithSE(u32 message, u32 wait, u8 pause, u8 sound) {
    u8 flags = 0;

    if (lbl_80478B30 != 3 && lbl_8047AD10 == 0) {
        s32 old_id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(old_id)) {
            menuCloseCustom((void*)old_id, 2, wait);
        }
    }
    lbl_80478B30 = 3;
    if (pause != 0) {
        flags |= 1;
    }
    return menuOpenCustom((void*)0x40, windowGetActiveID(), 0, 0,
                          (void*)wait, 3, message, flags, sound);
}

/* 0x801067E8 | 0x14C */
s32 winMsgOpenField(u32 message, u32 wait, u8 pause) {
    u8 flags = 0;

    if (lbl_80478B30 != 3 && lbl_8047AD10 == 0) {
        s32 old_id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(old_id)) {
            menuCloseCustom((void*)old_id, 2, wait);
        }
    }
    lbl_80478B30 = 3;
    if (pause != 0) {
        flags |= 1;
    }
    return menuOpenCustom((void*)0x40, windowGetActiveID(), 0, 0,
                          (void*)wait, 3, message, flags, 0);
}

/* 0x80106934 | 0xC8 */
s32 winMsgCheck(void) {
    u8* window = windowSearchID(winMsgResolveMenuId(lbl_80478B30));

    if (window == NULL) {
        return -1;
    }
    if (window[0x98] != 0) {
        return 0;
    }
    return window[0x99] == 0;
}

/* 0x801069FC | 0xE0 */
#pragma peephole off
void winMsgClose(u32 wait) {
    if (lbl_8047AD10 == 0) {
        s32 id = winMsgResolveMenuId(lbl_80478B30);
        if ((u8)menuIsCheck(id)) {
            menuCloseCustom((void*)id, 2, wait);
        }
        lbl_80478B30 = -1;
    }
}
#pragma peephole reset

/* 0x80106ADC | 0x260 */
s32 winMsgOpenWithSE(s8 type, u32 message, u32 wait, u8 pause, u8 sound) {
    s32 id = winMsgResolveMenuId(type);
    s32 parent;
    u8 flags = 0;
    u8 error = 0;

    if (id == 0) {
        return 0;
    }
    if (type == 6) {
        error = 1;
        parent = -1;
        lbl_8047AD10 = 1;
        wait = 0;
    } else {
        if (type != lbl_80478B30 && lbl_8047AD10 == 0) {
            s32 old_id = winMsgResolveMenuId(lbl_80478B30);
            if ((u8)menuIsCheck(old_id)) {
                menuCloseCustom((void*)old_id, 2, wait);
            }
        }
        lbl_80478B30 = type;
        parent = windowGetActiveID();
    }
    if (pause != 0) {
        flags |= 1;
    }
    if (error != 0) {
        flags |= 2;
    }
    return menuOpenCustom((void*)id, parent, 0, 0, (void*)wait, 3,
                          message, flags, sound);
}

/* 0x80106D3C | 0x25C */
s32 winMsgOpen(s8 type, u32 message, u32 wait, u8 pause) {
    s32 id = winMsgResolveMenuId(type);
    s32 parent;
    u8 flags = 0;
    u8 error = 0;

    if (id == 0) {
        return 0;
    }
    if (type == 6) {
        error = 1;
        parent = -1;
        lbl_8047AD10 = 1;
        wait = 0;
    } else {
        if (type != lbl_80478B30 && lbl_8047AD10 == 0) {
            s32 old_id = winMsgResolveMenuId(lbl_80478B30);
            if ((u8)menuIsCheck(old_id)) {
                menuCloseCustom((void*)old_id, 2, wait);
            }
        }
        lbl_80478B30 = type;
        parent = windowGetActiveID();
    }
    if (pause != 0) {
        flags |= 1;
    }
    if (error != 0) {
        flags |= 2;
    }
    return menuOpenCustom((void*)id, parent, 0, 0, (void*)wait, 3,
                          message, flags, 0);
}
