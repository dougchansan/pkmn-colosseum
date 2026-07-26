/**
 * @file menu.c
 * @brief menu -- core menu-item cursor/open/close/select engine (Colosseum
 *        "menu" UI subsystem). Split from the gs_model.c splitter bucket
 *        (address range 0x80102004 - 0x80103E68, 31 fns, 28 with real C
 *        bodies). Corresponds to XD's menu.cpp (0x8010E8C0-0x80110A04);
 *        15 anchor symbols (menuOpen, menuSetDisp, menuGetCursor, ...)
 *        are strictly monotonic against the XD address run. Tail fn
 *        menuInit calls the init routines of the neighboring TUs
 *        (cursorBiosInit, windowInit, menuOffScreenInit).
 */
#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern void  fn_8010C224(u32 count);
extern u32   fn_800D7894(void);
extern void  fn_800D7868(u32, u32, u32, u32, u32, u32, u32, u32);
extern void* GStextureLoad(void* data);
extern void  GSresRegisterResource(void* texture, u32, u32 resourceId, u32);

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

typedef struct MenuSystemState {
    u8 pad_00[0x94];
    s32 lastError;
} MenuSystemState;

#define GS_MODEL_STATE ((GSModelStateHeader*)lbl_80404ACC)

/* Resource index table */
extern u32 lbl_80478B20;   /* max resource index (sda21) */
extern u8  lbl_80315690[]; /* resource table, 8-byte entries */

/* Additional externs used by various functions */
extern void  GSmodelFree(u32);     /* GSmem release/unref */
extern void  fn_800D2738(void);
void* menuDataBiosGetPtr();    /* linked list head */
extern void* menuItemBiosGetPtr(s16 idx); /* node by index */
extern void* menuSeBiosGetPtr(s32);
extern u16   fn_8005D798(void*, s32);
extern void* menuSpriteBiosGetPtr(s32);
extern int   fn_80166A28(u16);
u8 fn_800F7EF8(s32 pad_id);
u32 fn_800F7A08(s32 pad_id, s32 axis);
u32 fn_800F7A7C(s32 pad_id, s32 axis);
u32 fn_800F7BC4(s32 pad_id);
f64 atan2(f64 y, f64 x);
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
extern u32 lbl_8047AD00;
extern u8  lbl_8047AD04[4];
extern u8  lbl_8047AD08[4];
extern u8  lbl_803156E0[];
extern u8  lbl_803254E0[];
extern u8  lbl_803357E0[];
extern volatile f32 lbl_8047CDC0;  /* sdata2: float constant */
extern f32 lbl_8047CDC4;  /* sdata2: float constant */
typedef f32 MenuAngle;
typedef f64 MenuSignedConversionBias;

/* Shared menu constants owned by game/gs_model_sdata2_8047CD98. */
extern const MenuAngle lbl_8047CDC8;
extern const MenuAngle lbl_8047CDCC;
extern const MenuAngle lbl_8047CDD0;
extern const MenuAngle lbl_8047CDD4;
extern const MenuSignedConversionBias lbl_8047CDD8;
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
typedef struct GcPadIdTable {
    s32 values[4];
} GcPadIdTable;
extern const GcPadIdTable lbl_80271E00;  /* GameCube pad ID table */
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
void* windowSearchID();
extern s32   _menuCBOffScreen__FP9GStextureUlPv(void);
extern void  winSpriteSetDisp(void* node, u32 enable);
s32   windowGetValue(s32 param);
void  windowCheckCursor(void* p, u32 flags);
extern void  windowDrawSprite2(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10);
extern u8    menuOffScreenFadeSync(u8 param);
extern void  menuOffScreenFadeSet(f32 f1, f32 f2);
extern u8    menuOffScreenCreate(u32 param);
extern void  menuOffScreenRelease(void);
extern u32   windowGetActiveID(void);
extern void* windowGetKeyInfo(void);
extern void* menuSeqBiosGetPtr(u32 idx);
extern void* windowSearchItemID(void* head, s32 key);
s32   menuOpenCustom(void* menu_id, u32 parent_id, s32* cursor_out, s32 close_flags, void* check_cursor, s32 open_param, ...);
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
s32 menuOpenCustom(void* menu_id, u32 parent_id, s32* cursor_out, s32 close_flags, void* check_cursor, s32 open_param, ...);
extern void menuSetPosition(void* p, s16 a, s16 b);
extern void menuButtonNormal(void* p);
extern void menuPlaySe(void* p, void* q);
extern u8 _menuGetAgbKeyInfo__FlPUs(s32, u16*);
extern void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK();
extern void menuGetKeyInfo(void* output, s32 port);
extern u8 menuGetEnablePort(void);
extern u8 menuSetEnablePort(u8 val);
extern void menuInit(u16 windowCount);
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
s32 windowGetValue(s32 param);
extern s32 fn_801044D0(s32 param, u16* val);
extern void windowGetCursor(void);
void windowCheckCursor(void* p, u32 flags);
extern u32 windowGetActiveID(void);
extern void* windowSearchItemID(void* head, s32 key);
void* windowSearchID();
extern void windowCloseMain(void* obj);
extern void windowClose(void* ptr, u32 flags);
extern void _windowCreateItemSprite__FP14tagWINDOW_WORK(void);
extern void windowCreateCursorSprite();
typedef struct MenuVaList {
    u8 gpr;
    u8 fpr;
    u16 reserved;
    u32* overflow_arg_area;
    u32* reg_save_area;
} MenuVaList;

typedef MenuVaList MenuVaListArray[1];

extern void* windowOpen(s32* cursor_out, void* menu_id, u32 parent_id,
                        s32 close_flags, s32 open_param,
                        MenuVaListArray args);
extern void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(void);
extern void windowInit(u16 count);
extern void* windowGetPortKeyInfo(u8 ports);
extern void* windowGetKeyInfo(void);
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
extern void fn_80106F98();
extern s32 winSeqCheckMove(s32 param);
extern s32 fn_80107170(s32 r3, s32 r31);
extern void winSeqMoveMenu();
extern void menuCursorNormal(void* p);
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

/* 0x80102004 | 0x10 */
s32 menuGetLastError(void) {
    return ((MenuSystemState*)lbl_80404ACC)->lastError;
}

/* 0x80102014 | 0x24 */
#pragma push
#pragma scheduling off
void menuGetOffScreenFlag(void) {
    menuOffScreenCheckEnable(0);
}
#pragma pop

/* 0x80102038 | 0x34 */
#pragma push
#pragma scheduling off
void menuReleaseOffScreen(f32 f1) {
    f32 f2;
    f2 = f1;
    menuOffScreenFadeSet(lbl_8047CDC0, f2);
    menuOffScreenFadeSync(1);
    menuOffScreenRelease();
}
#pragma pop

/* 0x8010206C | 0x54 */
void menuCreateOffScreen(f32 param) {
    f32 f31 = param;
    menuOffScreenCreate(1);
    menuOffScreenSetDisp(1);
    menuOffScreenSetPriority(0);
    menuOffScreenFadeSet(lbl_8047CDC4, f31);
}

/* 0x801020C0 | 0x78 */
#pragma push
#pragma peephole off
s32 menuGetSelectItemNum(void) {
    s32 r31 = 0;
    void* r3 = menuDataBiosGetPtr();
    if (r3 == (void*)0) { return 0; }
    {
        s32 r0 = (s16)*(s16*)((u8*)r3 + 0x4);
        do {
            void* node = menuItemBiosGetPtr((s16)r0);
            if (((u32)*(volatile u8*)node >> 7) & 1) {
                r31++;
            }
            if (((u32)*(volatile u8*)node >> 6) & 1) {
                goto _ret_r31;
            }
            r0 = (s16)*(s16*)((u8*)node + 0x18);
        } while (1);
    }
_ret_r31:
    return r31;
}
#pragma pop

/* 0x80102138 | 0xC0 */
#pragma push
#pragma peephole off
s32 menuGetCursorFromItemID(void* unused, u32 param) {
    u32 r29 = param;
    void* r3 = menuDataBiosGetPtr();
    if (r3 == (void*)0) { return -3; }
    {
        s32 r31 = (s16)*(s16*)((u8*)r3 + 0x4);
        s32 r30 = 0;
    loop:
        {
            void* node = menuItemBiosGetPtr((s16)r31);
            if ((u32)(s16)r31 == (u32)r29) {
                if (((u32)*(volatile u8*)node >> 7) & 1) {
                    return r30;
                }
                return -1;
            }
            if (((u32)*(volatile u8*)node >> 7) & 1) {
                r30++;
            }
            if (((u32)*(volatile u8*)node >> 6) & 1) {
                goto _ret_m2;
            }
            r31 = (s16)*(s16*)((u8*)node + 0x18);
            goto loop;
        }
    _ret_m2:
        return -2;
    }
}
#pragma pop

/* 0x801021F8 | 0x5C */
void fn_801021F8(void* p, u32 val) {
    u32 r30 = val;
    void* r3 = windowSearchID((s32)p);
    if (r3 == (void*)0) { return; }
    {
        void* r31 = *(void**)((u8*)r3 + 0x20);
        while (r31 != (void*)0) {
            winSpriteSetDisp(r31, r30);
            r31 = *(void**)r31;
        }
    }
}

/* 0x80102254 | 0x64 */
#pragma push
#pragma peephole off
void menuSetDisp(void* p, u32 enable) {
    u32 r31 = enable;
    void* r3 = windowSearchID((s32)p);
    if (r3 == (void*)0) { return; }
    if ((u8)r31 != 0) {
        s8 r0 = (s8)(*(u8*)r3 | 2);
        *(s8*)r3 = r0;
    } else {
        s8 r0 = (s8)(*(u8*)r3 & ~2);
        *(s8*)r3 = r0;
    }
}
#pragma pop

/* 0x801022B8 | 0xE0 */
#pragma push
#pragma peephole off
void* menuGetCursorItemID(void* p, u32 target) {
    void* r29 = p;
    void* r3 = windowSearchID((s32)p);
    s32 r31;
    if (r3 != (void*)0) {
        s8 r4 = (s8)*(u8*)((u8*)r3 + 0x95);
        s8 r0 = (s8)*(u8*)((u8*)r3 + 0x94);
        r31 = (s32)r4 + (s32)r0;
    } else {
        r31 = -1;
    }
    if (r31 == -1) { return (void*)0; }
    { extern void* menuDataBiosGetPtr(void*); r3 = menuDataBiosGetPtr(r29); }
    if (r3 == (void*)0) { return (void*)0; }
    {
        s32 r30 = (s16)*(s16*)((u8*)r3 + 0x4);
        s32 r29b = 0;
        void* result = (void*)0;
        do {
            void* node = menuItemBiosGetPtr((s16)r30);
            if (((u32)*(volatile u8*)node >> 7) & 1) {
                if (r29b == r31) {
                    return (void*)(s32)(s16)r30;
                }
                r29b++;
            }
            if (((u32)*(volatile u8*)node >> 6) & 1) {
                goto _ret0_loop;
            }
            r30 = (s16)*(s16*)((u8*)node + 0x18);
        } while (1);
    _ret0_loop:
        return (void*)0;
        (void)result;
    }
    (void)r29;
}
#pragma pop

/* 0x80102398 | 0x4C */
#pragma push
#pragma peephole off
s32 menuSetCursor(void* p, u32 val) {
    u32 r31 = val;
    void* r3 = windowSearchID((s32)p);
    if (r3 == (void*)0) goto ret_m1;
    *(s8*)((u8*)r3 + 0x95) = (s8)r31;
    goto ret0;
ret_m1:
    return -1;
ret0:
    return 0;
}
#pragma pop

/* 0x801023E4 | 0x44 */
s32 menuGetCursor(void* p) {
    void* r3 = windowSearchID((s32)p);
    unsigned char pad;
    if (r3 == (void*)0) {
        pad = 1;
        goto ret_m1;
    }
    {
        s8 r4 = (s8)*(u8*)((u8*)r3 + 0x95);
        s8 r0 = (s8)*(u8*)((u8*)r3 + 0x94);
        return (s32)r4 + (s32)r0;
    }
ret_m1:
    return -pad;
}

/* 0x80102428 | 0x98 */
#pragma push
#pragma peephole off
s32 menuCloseSync(void* p, u8 flag) {
    void* r31 = p;
    if ((u8)flag != 0) {
        goto loop;
    loop: {
            void* r3 = windowSearchID((s32)r31);
            if (r3 != (void*)0) goto step2;
            return 0;
        step2:
            if ((u32)GSthreadGetCurrentThread() != 0) goto do_yield;
            GSlogWrite((const char*)lbl_80271E10, (const char*)lbl_8035B060, r31);
            goto ret0;
        do_yield:
            _threadSwitch();
            goto loop;
        }
    } else {
        void* r3 = windowSearchID((s32)r31);
        s32 r0 = (s32)r3;
        s32 neg = -r0;
        return (u32)(neg | r0) >> 31;
    }
ret0:
    return 0;
}
#pragma pop

/* 0x801024C0 | 0x28 */
#pragma push
#pragma scheduling off
void menuCloseFloor(void) {
    windowClose(0, 4);
}
#pragma pop

/* 0x801024E8 | 0x28 */
#pragma push
#pragma scheduling off
void fn_801024E8(void) {
    windowClose(0, 4);
}
#pragma pop

/* 0x80102510 | 0x58 */
#pragma push
#pragma peephole off
void menuClose(s32 p) {
    s32 r31 = p;
    if (r31 == 0) {
        r31 = (s32)windowGetActiveID();
    }
    {
        void* r3 = windowSearchID(r31);
        if (r3 != (void*)0) {
            windowClose(r3, 0);
            windowSearchID(r31);
        }
    }
}
#pragma pop

/* 0x80102568 | 0xB8 */
s32 menuCloseCustom(void* p, u32 mode, u8 wait) {
    void* r29 = p;
    void* r30 = (void*)mode;
    u8 r31 = (u8)wait;
    void* r3 = windowSearchID((s32)p);
    if (r3 == (void*)0) { return 1; }
    windowClose(r29, (u32)r30);
    if ((u8)r31 != 0) {
        while (1) {
            r3 = windowSearchID((s32)r29);
            if (r3 == (void*)0) { return 0; }
            if (GSthreadGetCurrentThread() != 0) {
                _threadSwitch();
                continue;
            }
            GSlogWrite((const char*)lbl_80271E10, (const char*)lbl_8035B060, r29);
            return 0;
        }
    }
    r3 = windowSearchID((s32)r29);
    return 0;
}

/* 0x80102620 | 0x2C */
s32 menuIsCheck(s32 param) {
    s32 r = (s32)windowSearchID(param);
    return (u32)((-r) | r) >> 31;
}

/* 0x8010264C | 0x58 */
#pragma push
#pragma peephole off
void menuOpen(void* p, void* q) {
    void* r30 = p;
    void* r31 = q;
    menuOpenCustom(r30, windowGetActiveID(), 0, 0, r31, 0);
}
#pragma pop

typedef struct MenuDataCursorInfo {
    u8 mode;
    u8 field_01;
    u8 field_02;
    u8 cursor_slot;
} MenuDataCursorInfo;

typedef union MenuWindowCursorField {
    u16 position;
    struct {
        s8 base;
        s8 offset;
    } cursor;
} MenuWindowCursorField;

typedef struct MenuWindowCursorInfo {
    u8 pad_00[0x94];
    MenuWindowCursorField field_94;
} MenuWindowCursorInfo;

/* 0x801026A4 | 0x1C4 */
#pragma push
#pragma peephole off
s32 menuOpenCustom(void* menu_id, u32 parent_id, s32* cursor_out,
                   s32 close_flags, void* check_cursor, s32 open_param, ...) {
    MenuVaListArray args;
    MenuWindowCursorInfo* window;
    s32 value = 0;

    __builtin_va_info(&args);
    window = windowOpen(cursor_out, menu_id, parent_id, close_flags,
                        open_param, args);

    if ((u8)check_cursor != 0) {
        MenuDataCursorInfo* menu_data;
        u16 cursor_position;

        windowCheckCursor(menu_id, (u32)check_cursor);
        value = windowGetValue((s32)menu_id);
        menu_data = (MenuDataCursorInfo*)menuDataBiosGetPtr(menu_id);
        if (menu_data->cursor_slot != 0) {
            cursor_position = window->field_94.position;
            cursorBiosSetPos(menu_data->cursor_slot, &cursor_position);
        }

        if (cursor_out != 0) {
            s32 cursor;
            MenuWindowCursorInfo* current =
                (MenuWindowCursorInfo*)windowSearchID((s32)menu_id);
            if (current != 0) {
                cursor = current->field_94.cursor.offset +
                         current->field_94.cursor.base;
            } else {
                cursor = -1;
            }
            *cursor_out = cursor;
        }
    }

    if (((u32)close_flags & 1) != 0) {
        MenuWindowCursorInfo* current =
            (MenuWindowCursorInfo*)windowSearchID((s32)menu_id);
        if (current != 0) {
            windowClose(current, 0);
            if ((u8)check_cursor != 0) {
                do {
                    void* found = windowSearchID(menu_id);
                    if (found == 0) {
                        break;
                    }
                    if ((u32)GSthreadGetCurrentThread() == 0) {
                        GSlogWrite((const char*)lbl_80271E10,
                                   (const char*)lbl_8035B060, menu_id);
                        break;
                    }
                    _threadSwitch();
                } while (1);
            } else {
                windowSearchID((s32)menu_id);
            }
        }
    }

    return value;
}
#pragma pop

/* 0x80102868 | 0x48 */
#pragma push
#pragma peephole off
void menuSetPosition(void* p, s16 a, s16 b) {
    s16 r30 = a;
    s16 r31 = b;
    void* r3 = windowSearchID((s32)p);
    if (r3 == (void*)0) { return; }
    *(s16*)((u8*)r3 + 0x84) = r30;
    *(s16*)((u8*)r3 + 0x86) = r31;
}
#pragma pop

#pragma push
#pragma peephole off
typedef struct MenuDaemonNode MenuDaemonNode;
typedef void (*MenuNodeCallback)(MenuDaemonNode* node);

typedef struct MenuDaemonData {
    u8 soundGroup;
    u8 actionType;
    u8 pad_02[0x0A];
    MenuNodeCallback cursorCallback;
    MenuNodeCallback buttonCallback;
    MenuNodeCallback tickCallback;
    MenuNodeCallback drawCallback;
} MenuDaemonData;

struct MenuDaemonNode {
    s8 flags;
    s8 phase;
    u8 phaseFrame;
    u8 pad_03;
    u32 id;
    u8 pad_08;
    s8 fadePriority;
    u8 transitionDone;
    u8 pad_0B[5];
    MenuDaemonNode* next;
    u8 pad_14[5];
    s8 cursorCount;
    u8 cursorMode;
    u8 cursorFlags;
    void* sprites;
    void* overlaySprites;
    u8 pad_24[0x60];
    s16 x;
    s16 y;
    u8 pad_88[0x0C];
    u16 cursor;
    u16 previousCursor;
    u8 close;
    u8 back;
};

typedef struct MenuDaemonSystem {
    u8 pad_00[0x0C];
    MenuDaemonNode* head;
    u8 pad_10[0x84];
    u32 cursorChanged;
    u32 activeId;
} MenuDaemonSystem;

typedef union MenuCursorPair {
    u16 packed;
    struct {
        s8 x;
        s8 y;
    } pos;
} MenuCursorPair;

void menuDaemon(void)
{
    MenuDaemonSystem* system;
    MenuDaemonNode* active;
    MenuDaemonNode* node;
    MenuDaemonData* data;
    void* sprite;
    u32 i;
    u16 keys;
    u8 doFade = 1;

    fn_801D2404();
    active = windowSearchID(windowGetActiveID());
    _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(lbl_80404ACC);
    fn_800D9E4C(0);
    fn_801093C8();
    fn_800D9ED8(1);
    system = (MenuDaemonSystem*)lbl_80404ACC;

    for (node = system->head; node != NULL; node = node->next) {
        data = menuDataBiosGetPtr(node->id);
        switch (node->phase) {
        case 0:
            node->phase = 1;
            node->phaseFrame = 0;
            break;
        case 1:
            if ((u8)winSeqCheckMove(node->id) == 0) {
                if ((u8)((data->actionType >> 2) & 7) == 0) {
                    node->close = 1;
                }
                node->phase = 2;
                node->phaseFrame = 0;
            }
            break;
        case 2:
            if (node->transitionDone != 0) {
                MenuDaemonData* soundData = menuDataBiosGetPtr(node->id);
                void* sound = menuSeBiosGetPtr(soundData->soundGroup & 7);
                if (sound != NULL) {
                    u16 se = fn_8005D798(sound, 5);
                    if (se != 0) {
                        fn_80166A28(se);
                    }
                }
                node->phase = 3;
                node->phaseFrame = 0;
            }
            break;
        case 3:
            if ((u8)winSeqCheckMove(node->id) == 0) {
                node->phase = 4;
                node->phaseFrame = 0;
            }
            break;
        case 4:
            break;
        }

        if (active != NULL && node == active) {
            if (node->close == 0) {
                switch (node->phase) {
                case 2:
                    switch ((data->actionType >> 2) & 7) {
                    case 0:
                        node->close = 1;
                        break;
                    case 1:
                        if (node != NULL) {
                            keys = *(u16*)((u8*)windowGetKeyInfo() + 4);
                            if (keys & 0x10) {
                                node->close = 1;
                            }
                            if (keys & 0x20) {
                                node->close = 1;
                                node->back = 1;
                            }
                        }
                        break;
                    case 3:
                        fn_800F7434(data->buttonCallback, 0);
                        break;
                    case 4:
                        data->buttonCallback(node);
                        break;
                    }
                    break;
                case 3:
                    break;
                }

                if ((node->cursorFlags & 8) != 0 && node->back != 0) {
                    node->back = 0;
                    node->close = 0;
                }

                if (node->close != 0) {
                    system->cursorChanged = 0;
                    system->activeId = node->id;
                } else if ((*(u16*)windowGetKeyInfo() & 0x8000) != 0) {
                    system->cursorChanged = 1;
                    system->activeId = node->id;
                    if ((node->cursorFlags & 0x10) == 0) {
                        node->close = 1;
                        node->back = 1;
                    }
                }

                if (node->back != 0) {
                    MenuDaemonData* soundData = menuDataBiosGetPtr(node->id);
                    void* sound = menuSeBiosGetPtr(soundData->soundGroup & 7);
                    if (sound != NULL) {
                        u16 se = fn_8005D798(sound, 3);
                        if (se != 0) {
                            fn_80166A28(se);
                        }
                    }
                } else if (node->close != 0) {
                    MenuDaemonData* soundData = menuDataBiosGetPtr(node->id);
                    void* sound = menuSeBiosGetPtr(soundData->soundGroup & 7);
                    if (sound != NULL) {
                        u16 se = fn_8005D798(sound, 2);
                        if (se != 0) {
                            fn_80166A28(se);
                        }
                    }
                }
                if (node->close != 0) {
                    fn_80106F98(node->id);
                }
            }

            if (node->close == 0) {
                MenuCursorPair oldPos;
                MenuCursorPair newPos;

                switch (node->phase) {
                case 2:
                node->previousCursor = node->cursor;
                if (node->cursorCount > 0) {
                    switch (node->cursorMode) {
                    case 1:
                    case 2:
                        menuCursorNormal(node);
                        break;
                    case 3:
                        fn_800F7434(data->cursorCallback, 0);
                        break;
                    case 4:
                        data->cursorCallback(node);
                        break;
                    }
                }
                oldPos.packed = node->previousCursor;
                newPos.packed = node->cursor;
                if (oldPos.pos.x != newPos.pos.x ||
                    oldPos.pos.y != newPos.pos.y) {
                    void* sound;
                    MenuDaemonData* soundData;
                    u16 se;

                    windowCreateCursorSprite(node);
                    soundData = menuDataBiosGetPtr(node->id);
                    sound = menuSeBiosGetPtr(soundData->soundGroup & 7);
                    if (sound != NULL) {
                        se = fn_8005D798(sound, 1);
                        if (se != 0) {
                            fn_80166A28(se);
                        }
                    }
                }
                    break;
                case 3:
                    break;
                }
            }
        }

        if ((node->flags & 4) != 0) {
            winSeqMoveMenu(node);
            if (data->tickCallback != NULL) {
                for (i = 0; i < fn_800D3088(); i++) {
                    data->tickCallback(node);
                }
            }
        }
    }

    for (node = system->head; node != NULL; node = node->next) {
        if (node->phase == 4) {
            windowCloseMain(node);
        }
    }

    for (node = system->head; node != NULL; node = node->next) {
        if (doFade != 0 && node->fadePriority >= 0x50) {
            doFade = 0;
            fadeDaemon();
            fn_800D9ED8(1);
        }
        if ((node->flags & 2) != 0) {
            fn_800FE6D0(node->x, node->y);
            fn_800FE35C();
            spriteSetEnv();
            for (sprite = node->sprites; sprite != NULL;
                 sprite = *(void**)sprite) {
                winSpriteDraw(node, sprite);
            }
            for (sprite = node->overlaySprites; sprite != NULL;
                 sprite = *(void**)sprite) {
                winSpriteDraw(node, sprite);
            }
            data = menuDataBiosGetPtr(node->id);
            if (data->drawCallback != NULL) {
                data->drawCallback(node);
            }
        }
    }

    fn_800FE6D0(0, 0);
    fn_800FE35C();
    fn_800DA4C4(1, 6, 7);
    if (doFade != 0) {
        fadeDaemon();
    }
    fn_800D9ED8(0);
    fn_800D9E4C(1);
}
#pragma pop


/* 0x80102ED4 | 0x64 */
#pragma push
#pragma peephole off
void menuButtonNormal(void* p) {
    void* r31;
    if ((r31 = p) == (void*)0) { return; }
    {
        void* base = windowGetKeyInfo();
        u16 r3 = *(u16*)((u8*)base + 0x4);
        s32 bits = (s32)r3;
        if (bits & 0x10) {
            *(u8*)((u8*)r31 + 0x98) = 1;
        }
        if (bits & 0x20) {
            *(u8*)((u8*)r31 + 0x98) = 1;
            *(u8*)((u8*)r31 + 0x99) = 1;
        }
    }
}
#pragma pop

/* 0x80102F38 | 0x54C */
typedef struct MenuCursorItem {
    u8 flags;
    u8 pad01;
    s16 x;
    s16 y;
    u8 pad06[0x12];
    s16 next;
} MenuCursorItem;

#define MENU_ITEM_SELECTABLE(item) ((((item)->flags >> 7) & 1) != 0)
#define MENU_ITEM_END(item) ((((item)->flags >> 6) & 1) != 0)

void menuCursorNormal(void* p) {
    u8* window;
    u8* data;
    MenuCursorItem* current;
    MenuCursorItem* item;
    u16 repeat;
    s32 isMode2;
    s32 up;
    s32 down;
    s32 left;
    s32 right;
    s32 xTol;
    s32 yTol;
    s32 best;
    s32 index;
    s32 delta;
    s32 cross;

    window = p;
    isMode2 = 0;
    if (window == 0) {
        return;
    }

    if (window[0x1A] == 2) {
        isMode2 = 1;
    }

    data = menuDataBiosGetPtr(*(u32*)(window + 4));
    current = (MenuCursorItem*)windowGetCursorToItem(window);
    if (current == 0) {
        window[0x95] = 0;
        return;
    }

    repeat = *(u16*)((u8*)windowGetKeyInfo() + 6);
    up = repeat & 1;
    down = repeat & 2;
    left = repeat & 4;
    right = repeat & 8;

    xTol = 0x10;
    yTol = 0x0C;
    for (;;) {
        item = (MenuCursorItem*)menuItemBiosGetPtr(*(s16*)(data + 4));
        index = 0;

        if (up != 0) {
            best = 0x1E0;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = current->y - item->y;
                    if (delta > 0) {
                        cross = item->x - current->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xTol && delta < best) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        } else if (down != 0) {
            best = 0x1E0;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = item->y - current->y;
                    if (delta > 0) {
                        cross = item->x - current->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xTol && delta < best) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        } else if (left != 0) {
            best = 0x280;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = current->x - item->x;
                    if (delta > 0) {
                        cross = item->y - current->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yTol && delta < best) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        } else if (right != 0) {
            best = 0x280;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = item->x - current->x;
                    if (delta > 0) {
                        cross = item->y - current->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yTol && delta < best) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        }

        if ((s8)window[0x97] != (s8)window[0x95]) {
            break;
        }

        xTol += 0x10;
        yTol += 0x0C;
        if (xTol >= 0x280) {
            xTol = 0x280;
        }
        if (yTol >= 0x1E0) {
            yTol = 0x1E0;
        }
        if (xTol == 0x280 && yTol == 0x1E0) {
            break;
        }
    }

    if ((u16)isMode2 == 0) {
        return;
    }

    xTol = 0x10;
    yTol = 0x0C;
    for (;;) {
        if ((s8)window[0x97] != (s8)window[0x95]) {
            return;
        }

        item = (MenuCursorItem*)menuItemBiosGetPtr(*(s16*)(data + 4));
        index = 0;

        if (up != 0) {
            best = 0;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = item->y - current->y;
                    if (delta > 0) {
                        cross = item->x - current->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xTol && best < delta) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        } else if (down != 0) {
            best = 0;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = current->y - item->y;
                    if (delta > 0) {
                        cross = item->x - current->x;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < xTol && best < delta) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        } else if (left != 0) {
            best = 0;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = item->x - current->x;
                    if (delta > 0) {
                        cross = item->y - current->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yTol && best < delta) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        } else if (right != 0) {
            best = 0;
            for (;;) {
                if (MENU_ITEM_SELECTABLE(item)) {
                    delta = current->x - item->x;
                    if (delta > 0) {
                        cross = item->y - current->y;
                        if (cross < 0) {
                            cross = -cross;
                        }
                        if (cross < yTol && best < delta) {
                            best = delta;
                            window[0x95] = index;
                        }
                    }
                    index++;
                }
                if (MENU_ITEM_END(item)) {
                    break;
                }
                item = (MenuCursorItem*)menuItemBiosGetPtr(item->next);
            }
        }

        xTol += 0x10;
        yTol += 0x0C;
        if (xTol >= 0x280) {
            xTol = 0x280;
        }
        if (yTol >= 0x1E0) {
            yTol = 0x1E0;
        }
        if (xTol == 0x280 && yTol == 0x1E0) {
            break;
        }
    }
}


/* 0x80103484 | 0x58 */
#pragma push
#pragma peephole off
void menuPlaySe(void* p, void* q) {
    s32 r31 = (s32)q;
    void* r3 = menuDataBiosGetPtr();
    void* r3c = menuSeBiosGetPtr((s32)(*(u8*)((u8*)r3 + 0x0) & 0x7));
    if (r3c == (void*)0) { return; }
    {
        u32 v = (u32)(u16)fn_8005D798(r3c, r31);
        if (v == 0) { return; }
        fn_80166A28((u16)v);
    }
}
#pragma pop

/* 0x801034DC | 0x138 */
extern u8 fn_8008ABA0(s32);
extern s32 fn_8008AB8C(s32);

u8 _menuGetAgbKeyInfo__FlPUs(s32 port, u16* keys) {
    s32 device;
    u16 buttons;
    u16 result;

    device = port + 1;
    result = 0;
    if (fn_8008ABA0(device) == 0) {
        return 0;
    }

    buttons = (u16)fn_8008AB8C(device);
    if (buttons & 0x40) result |= 0x1;
    if (buttons & 0x80) result |= 0x2;
    if (buttons & 0x20) result |= 0x4;
    if (buttons & 0x10) result |= 0x8;
    if (buttons & 0x1) result |= 0x10;
    if (buttons & 0x2) result |= 0x20;
    if (buttons & 0x4) result |= 0x100;
    if (buttons & 0x200) result |= 0x200;
    if (buttons & 0x100) result |= 0x400;
    if (buttons & 0x8) result |= 0x800;

    *keys = result;
    return 1;
}

/* 0x80103614 | 0x2E4 */
#pragma push
#pragma peephole off
u8 _menuGetGcKeyInfo__FlPUs(s32 port, u16* keys) {
    typedef union GcPadDoubleConversion {
        f64 value;
        struct {
            u32 high;
            u32 low;
        } words;
    } GcPadDoubleConversion;
    struct {
        GcPadIdTable pad_ids;
        GcPadDoubleConversion converted_y;
        GcPadDoubleConversion converted_x;
    } work;
    u16 result = 0;
    s32 pad_id;
    u32 stick_x;
    u32 stick_y;
    u32 buttons;
    f32 angle;
    f32 lower_bound;

    work.pad_ids = lbl_80271E00;
    pad_id = work.pad_ids.values[port];
    if (fn_800F7EF8(pad_id) == 0) {
        return 0;
    }

    stick_x = fn_800F7A08(pad_id, 0);
    stick_y = fn_800F7A7C(pad_id, 0);
    if (((s8)stick_y < 0 ? -(s8)stick_y : (s8)stick_y) > 0x20 ||
        ((s8)stick_x < 0 ? -(s8)stick_x : (s8)stick_x) > 0x20) {
        work.converted_y.words.low = (s32)(s8)stick_y ^ 0x80000000;
        work.converted_y.words.high = 0x43300000;
        work.converted_x.words.low = (s32)(s8)stick_x ^ 0x80000000;
        work.converted_x.words.high = 0x43300000;
        angle = atan2(work.converted_y.value - lbl_8047CDD8,
                      work.converted_x.value - lbl_8047CDD8);
        if ((angle > lbl_8047CDC0 ? angle : -angle) < lbl_8047CDC8) {
            result |= 2;
        } else if ((angle > lbl_8047CDC0 ? angle : -angle) > lbl_8047CDCC) {
            result |= 1;
        }
        lower_bound = lbl_8047CDD0;
        if (lower_bound < (angle > lbl_8047CDC0 ? angle : -angle) &&
            (angle > lbl_8047CDC0 ? angle : -angle) < lbl_8047CDD4) {
            if (angle < lbl_8047CDC0) {
                result |= 4;
            } else {
                result |= 8;
            }
        }
    }

    buttons = fn_800F7BC4(pad_id);
    if (buttons & 0x8) result |= 0x1;
    if (buttons & 0x4) result |= 0x2;
    if (buttons & 0x1) result |= 0x4;
    if (buttons & 0x2) result |= 0x8;
    if (buttons & 0x100) result |= 0x10;
    if (buttons & 0x200) result |= 0x20;
    if (buttons & 0x400) result |= 0x40;
    if (buttons & 0x800) result |= 0x80;
    if (buttons & 0x10) result |= 0x100;
    if (buttons & 0x40) result |= 0x200;
    if (buttons & 0x20) result |= 0x400;
    if (buttons & 0x1000) result |= 0x800;

    *keys = result;
    return 1;
}
#pragma pop

/* 0x801038F8 | 0x2B0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(void) {
    /* TODO: match -- 688 bytes at 0x801038F8 */
}
#pragma pop

/* 0x80103BA8 | 0x108 */
void menuGetKeyInfo(void* output, s32 port) {
    u8 info[0x1A];
    u8 mask;

    switch (port) {
    case 1: mask = 1; break;
    case 2: mask = 2; break;
    case 3: mask = 4; break;
    case 4: mask = 8; break;
    default: mask = 0; break;
    }
    if (mask != 0) {
        memcpy(info, windowGetPortKeyInfo(mask), sizeof(info));
    } else {
        memset(info, 0, sizeof(info));
    }
    memcpy(output, info, sizeof(info));
}

/* 0x80103CB0 | 0x10 */
u8 menuGetEnablePort(void) {
    return lbl_80404ACC[0x92];
}

/* 0x80103CC0 | 0x18 */
u8 menuSetEnablePort(u8 val) {
    u8 old = lbl_80404ACC[0x92];
    lbl_80404ACC[0x92] = val;
    return old;
}

/* 0x80103CD8 | 0x190 */
void menuInit(u16 windowCount) {
    void* texture;

    windowInit(windowCount);
    cursorBiosInit();
    menuOffScreenInit();
    fn_8010C224(0x18);
    lbl_80404ACC[0x92] = 1;
    memset(lbl_8047AD04, 0, sizeof(lbl_8047AD04));
    memset(lbl_8047AD08, 0, sizeof(lbl_8047AD08));

    if (lbl_8047AD00 == 0) {
        lbl_8047AD00 = fn_800D7894();
        fn_800D7868(lbl_8047AD00, 1, 0, 0, 3, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 4, 0, 6, 10, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 6, 0, 8, 4, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 7, 0, 8, 4, 0, 0, 0);
        fn_800D7868(lbl_8047AD00, 8, 0, 8, 4, 0, 0, 0);
    }

    texture = GStextureLoad(lbl_803156E0);
    GSresRegisterResource(texture, 0, 0x31A1200, 0);
    texture = GStextureLoad(lbl_803254E0);
    GSresRegisterResource(texture, 0, 0x6221200, 0);
    texture = GStextureLoad(lbl_803357E0);
    GSresRegisterResource(texture, 0, 0x6F71200, 0);
}
