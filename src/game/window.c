/**
 * @file window.c
 * @brief window -- window allocation, cursor/sprite management, and the
 *        WINDOW_WORK object table (Colosseum "menu" UI subsystem). Split
 *        from the gs_model.c splitter bucket (address range 0x80103FE4 -
 *        0x801058CC, 25 fns). Corresponds to XD's window.cpp
 *        (0x80114D30-0x80116A90); 10 anchor symbols (windowGetAllocPtr,
 *        windowSetParam, windowSearchID, ...) strictly monotonic against
 *        the XD address run, TU head matches (windowGetAllocPtr).
 *        windowInit is called by menuInit (menu.c).
 */
#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */
extern u32   GSmsgGetRect(void* message);

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
extern void* menuDataBiosGetPtr(void* key);
extern void* menuItemBiosGetPtr(s16 idx); /* node by index */
extern void* menuSeBiosGetPtr(s32);
extern u16   fn_8005D798(void*, s32);
extern void* menuSpriteBiosGetPtr(s32);
extern int   fn_80166A28(u16);
extern u32   GSthreadGetCurrentThread(void);    /* poll/yield -- 0 if pending */
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
extern u16 lbl_8047E718;
extern u32 lbl_8047CDE8;
extern u32 lbl_8047CDF0;
extern u32 lbl_8047CDF4;
extern f32 lbl_8047CDF8;
extern f32 lbl_8047CDFC;
extern f32 lbl_8047CE00;
extern f64 lbl_8047CE08;
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
extern u8  lbl_80404AB0[];
extern u8  lbl_80271E10[];  /* format string */
extern u8  lbl_80271E40[];
extern u8  lbl_80271E64[];
extern u8  lbl_80271E4C[];  /* format string */
extern u8  lbl_80271EE8[];  /* format string */
extern u8  lbl_80271F18[];  /* format string */
extern u8  lbl_8035B060[];  /* module name string */
extern u8  lbl_8035B070[];
extern u8  lbl_8035B3F0[];  /* module name string */
extern u8  lbl_80314E08[];

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
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5BA0(s32 index, u32 color);
extern void fn_800D5648(f32 value);
extern void fn_800FE38C(s16 x, s16 y, s16 width, s16 height);
extern void fn_800FE35C(void);
extern f64 fmod(f64 value, f64 modulus);
extern void fn_800FE6AC(s16* x, s16* y);
extern void winSpriteDraw(void* context, void* sprite);
extern u8* winSpriteAdd(void* head);

/* Forward declarations for functions defined later in this TU */
extern u8    menuOffScreenCheckEnable(u8 param);
extern s32   windowClose(void* ptr, u32 flags);
extern void* windowSearchID(s32 param);
extern s32   _menuCBOffScreen__FP9GStextureUlPv(void);
extern void  winSpriteSetDisp(void* node, u32 enable);
extern s32   windowGetValue(s32 param);
extern s32   windowCheckCursor(s32 id, u8 wait);
extern void  windowDrawSprite2(void* x, void* y, s16 width, s16 height,
                               s32 color, s32 context, s32 spriteId, s32 flags);
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
extern s32   menuGetCursorItemID(s32 windowId);
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
extern void windowDrawSprite(void* p, void* a, void* b, u32 key, u32 data);
extern void windowDrawSprite2(void* x, void* y, s16 width, s16 height,
                              s32 color, s32 context, s32 spriteId, s32 flags);
extern u8* windowGetCursorToItem(u8* arg);
extern s32 windowGetValue(s32 param);
extern s32 fn_801044D0(s32 param, u16* val);
extern u32 windowGetCursor(s32 param);
extern s32 windowCheckCursor(s32 id, u8 wait);
extern u32 windowGetActiveID(void);
extern void* windowSearchItemID(void* head, s32 key);
extern void* windowSearchID(s32 param);
extern void windowCloseMain(void* obj);
extern s32 windowClose(void* ptr, u32 flags);
extern s32 _windowCreateItemSprite__FP14tagWINDOW_WORK(u8* window);
extern void windowCreateCursorSprite(u8* window);
extern void windowOpen(void);
extern void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(u8* item, s16* width, s16* height);
extern void windowInit(u16 count);
extern void* windowGetPortKeyInfo(u8 ports);
extern void* windowGetKeyInfo(void);
extern s32 fn_80105634(u8* window, u8* sprite);
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

/* 0x80103FE4 | 0x18 */
void* windowGetAllocPtr(void* ptr) {
    if (ptr) {
        return *(void**)((u8*)ptr + 0xB0);
    }
    return (void*)0;
}

/* 0x80103FFC | 0xA4 */
#pragma push
#pragma peephole off
void* windowAllocMemory(void* p, s32 size) {
    s32 r31 = size;
    void* r30 = p;
    if (r30 == (void*)0) { return (void*)0; }
    if (*(u16*)((u8*)r30 + 0xac) != 0) {
        fn_800E24B0(*(u16*)((u8*)r30 + 0xac));
        fn_800E209C(*(u16*)((u8*)r30 + 0xac));
        *(u32*)((u8*)r30 + 0xb0) = 0;
    }
    if (r31 <= 0) { return (void*)0; }
    {
        u16 h = _toolentryAlloc__FUl((u32)r31);
        *(u16*)((u8*)r30 + 0xac) = h;
        if (*(u16*)((u8*)r30 + 0xac) != 0) {
            void* ptr = fn_800E27B0(*(u16*)((u8*)r30 + 0xac));
            *(void**)((u8*)r30 + 0xb0) = ptr;
        } else {
            return (void*)0;
        }
    }
    return *(void**)((u8*)r30 + 0xb0);
}
#pragma pop

/* 0x801040A0 | 0x18 */
void* windowGetFreeWork(void* ptr) {
    if (ptr) {
        return (void*)((u8*)ptr + 0x9C);
    }
    return (void*)0;
}

/* 0x801040B8 | 0x18 */
void windowSetParam(void* ptr, u32 idx, u32 val) {
    if (ptr == (void*)0) { return; }
    ((u32*)((u8*)ptr + 0x60))[idx] = val;
}

/* 0x801040D0 | 0x20 */
u32 windowGetParam(void* ptr, u32 idx) {
    if (ptr) {
        return ((u32*)((u8*)ptr + 0x60))[idx];
    }
    return 0;
}

/* 0x801040F0 | 0x70 */
/* menuSpriteBiosGetPtr already declared above */
void windowDrawSprite(void* p, void* a, void* b, u32 key, u32 data) {
    if ((u16)key != 0) {
        u8* sprite = menuSpriteBiosGetPtr((u16)key);

        windowDrawSprite2(p, a, *(s16*)(sprite + 0x0C),
                          *(s16*)(sprite + 0x0E), -1, (s32)b,
                          key, data);
    }
}

/* 0x80104160 | 0x1B8 */
void windowDrawSprite2(void* x, void* y, s16 width, s16 height, s32 color,
                       s32 contextValue, s32 spriteValue, s32 flags) {
    u8 sprite[0x78];
    u8 localContext[0xB4];
    void* context = (void*)contextValue;
    u8* info = menuSpriteBiosGetPtr((u16)spriteValue);
    s16 unusedX;
    s16 unusedY;
    u32 type;

    memset(sprite, 0, sizeof(sprite));
    sprite[4] = 7;
    *(u32*)(sprite + 0x64) = color;
    *(f32*)(sprite + 0x68) = 1.0f;
    *(f32*)(sprite + 0x6C) = 1.0f;
    type = (info[0] >> 4) & 3;
    if (type == 1) {
        sprite[5] |= 1;
        *(u32*)(sprite + 0x58) = *(u32*)(info + 0x10);
    } else if (type == 2) {
        sprite[5] |= 2;
        *(u32*)(sprite + 0x08) = *(u32*)(info + 0x10);
    }
    *(s16*)(sprite + 0x50) = (s16)((s32)x + (s8)info[5]);
    *(s16*)(sprite + 0x52) = (s16)((s32)y + (s8)info[6]);
    *(s16*)(sprite + 0x54) = width;
    *(s16*)(sprite + 0x56) = height;
    *(s16*)(sprite + 0x5C) = *(s16*)(info + 0x08);
    *(s16*)(sprite + 0x5E) = *(s16*)(info + 0x0A);
    *(s16*)(sprite + 0x60) = *(s16*)(info + 0x0C);
    *(s16*)(sprite + 0x62) = *(s16*)(info + 0x0E);
    sprite[0x67] = (u8)((info[7] * sprite[0x67]) / 255);
    if ((flags & 1) != 0) *(s16*)(sprite + 0x54) = -(s16)width;
    if ((flags & 2) != 0) *(s16*)(sprite + 0x56) = -(s16)height;
    if (context == NULL) {
        context = localContext;
        memset(context, 0, sizeof(localContext));
        fn_800FE6AC(&unusedX, &unusedY);
        *(s32*)(localContext + 0x88) = -1;
    }
    winSpriteDraw(context, sprite);
}

/* 0x80104318 | 0x8C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u8* windowGetCursorToItem(u8* arg) {
#pragma optimization_level 4
#pragma peephole off
    void* node;
    s32 idx;
    { extern void* menuDataBiosGetPtr(void*); node = menuDataBiosGetPtr(*(void**)(arg + 0x4)); }
    node = menuItemBiosGetPtr(*(s16*)((u8*)node + 0x4));
    idx = 0;
    while (1) {
        if (((u32)*(volatile u8*)node >> 7) & 1) {
            if ((s8)*(s8*)(arg + 0x95) == idx) {
                return (u8*)node;
            }
            idx = idx + 1;
        }
        if (((u32)*(volatile u8*)node >> 6) & 1) {
            break;
        }
        node = menuItemBiosGetPtr(*(s16*)((u8*)node + 0x18));
    }
    return (u8*)0;
}
#pragma pop

/* shared model-table lookup, inlined by the find-and-act helpers below */
static inline void* mdl_find(s32 param) {
    void* r;
    if (param <= 0) { return (void*)0; }
    r = *(void**)((u8*)lbl_80404ACC + 0xc);
    while (r != (void*)0) {
        if (*(s32*)((u8*)r + 0x4) == param) { return r; }
        r = *(void**)((u8*)r + 0x10);
    }
    return (void*)0;
}

/* 0x801043A4 | 0x12C */
s32 windowGetValue(s32 param) {
    u8* window = mdl_find(param);
    u8* menuData;
    u32 type;
    s32 result;

    if (window == NULL) {
        return -1;
    }
    if (window[0x99] != 0) {
        return -1;
    }
    menuData = menuDataBiosGetPtr(*(void**)(window + 0x04));
    if (menuData == NULL) {
        result = (s8)window[0x94] + (s8)window[0x95];
    } else {
        type = (menuData[0] >> 6) & 3;
        switch (type) {
        case 0:
            result = 0;
            break;
        case 1:
            result = (s8)window[0x94] + (s8)window[0x95];
            break;
        case 2:
            result = *(s32*)(window + 0x80);
            break;
        case 3:
            result = menuGetCursorItemID(param);
            break;
        }
    }
    return result;
}

/* 0x801044D0 | 0x60 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_801044D0(s32 param, u16* val) {
#pragma optimization_level 2
    void* node = mdl_find(param);
    if (node != (void*)0) {
        *(u16*)((u8*)node + 0x94) = *val;
        return 1;
    }
    return 0;
}
#pragma pop

/* 0x80104530 | 0x78 */
u32 windowGetCursor(s32 param) {
    u8* window = mdl_find(param);
    u16 cursor;

    if (window != NULL) {
        cursor = *(u16*)(window + 0x94);
    } else {
        cursor = lbl_8047E718;
    }
    return (u32)cursor << 16;
}

/* 0x801045A8 | 0x110 */
s32 windowCheckCursor(s32 id, u8 wait) {
    u8* window;

    do {
        window = mdl_find(id);
        if (window == NULL) {
            GSlogWrite((const char*)lbl_80271E40, (const char*)lbl_8035B070, id);
            return 0;
        }
        if (window[0x98] != 0 || window[0x99] != 0) {
            return 0;
        }
        if (wait == 0) {
            break;
        }
        if (GSthreadGetCurrentThread() == 0) {
            GSlogWrite((const char*)lbl_80271E64, (const char*)lbl_8035B070, id);
            break;
        }
        _threadSwitch();
    } while (1);
    return 1;
}

/* 0x801046B8 | 0x10 */
u32 windowGetActiveID(void) {
    return *(u32*)(lbl_80404ACC + 0x4);
}

/* 0x801046C8 | 0x3C */
#pragma push
#pragma scheduling off
void* windowSearchItemID(void* head, s32 key) {
    if (head == (void*)0) { return (void*)0; }
    {
        void* r3 = *(void**)((u8*)head + 0x1c);
        s32 r4 = (u16)key;
        while (r3 != (void*)0) {
            s16 r0 = *(s16*)((u8*)r3 + 0x6);
            if ((s32)r0 == r4) { return r3; }
            r3 = *(void**)r3;
        }
        return (void*)0;
    }
}
#pragma pop

/* 0x80104704 | 0x48 */
void* windowSearchID(s32 param) {
    if (param <= 0) { return (void*)0; }
    {
        void* r4 = *(void**)((u8*)lbl_80404ACC + 0xc);
        while (r4 != (void*)0) {
            s32 r0 = *(s32*)((u8*)r4 + 0x4);
            if (r0 == param) { return r4; }
            r4 = *(void**)((u8*)r4 + 0x10);
        }
        return (void*)0;
    }
}

/* 0x8010474C | 0xDC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void windowCloseMain(void* obj) {
#pragma optimization_level 4
#pragma peephole off
    void* h;
    void* nx;
    { extern void* menuDataBiosGetPtr(void*); h = menuDataBiosGetPtr(*(void**)((u8*)obj + 0x4)); }
    if (*(u32*)((u8*)h + 0x14) != 0) {
        *(u8*)((u8*)obj + 0x1) = 5;
        (*(void (*)(void*))*(u32*)((u8*)h + 0x14))(obj);
    }
    if (obj != (void*)0) {
        if (*(void**)((u8*)obj + 0x14) == (void*)0) {
            *(void**)((u8*)lbl_80404ACC + 0xc) = *(void**)((u8*)obj + 0x10);
        } else {
            *(void**)((u8*)*(void**)((u8*)obj + 0x14) + 0x10) = *(void**)((u8*)obj + 0x10);
        }
        nx = *(void**)((u8*)obj + 0x10);
        if (nx != (void*)0) {
            *(void**)((u8*)nx + 0x14) = *(void**)((u8*)obj + 0x14);
        }
        { extern void winSpriteRelease(void* head); winSpriteRelease((u8*)obj + 0x1c); }
        { extern void winSpriteRelease(void* head); winSpriteRelease((u8*)obj + 0x20); }
        if (*(u16*)((u8*)obj + 0xac) != 0) {
            fn_800E24B0(*(u16*)((u8*)obj + 0xac));
            fn_800E209C(*(u16*)((u8*)obj + 0xac));
            *(u32*)((u8*)obj + 0xb0) = 0;
            *(u16*)((u8*)obj + 0xac) = 0;
        }
        *(u8*)((u8*)obj + 0x0) = 0;
        *(u32*)((u8*)obj + 0x4) = 0;
    }
}
#pragma pop

/* 0x80104828 | 0x26C */
s32 windowClose(void* ptr, u32 flags) {
    u8* window = ptr;
    u8* current;
    u8* active;
    u8 found;

    if ((flags & 2) == 0) {
        for (current = *(u8**)(lbl_80404ACC + 0x0C);
             current != NULL; current = *(u8**)(current + 0x10)) {
            u8* link = current;

            found = 0;
            if (current != window) {
                do {
                    link = *(u8**)(link + 0x0C);
                    if (link == window) {
                        found = 1;
                        break;
                    }
                } while (link != NULL);
            }
            if (found && (flags & 4) != 0) {
                u8* data = menuDataBiosGetPtr(*(void**)(current + 0x04));
                if ((data[0] & 8) == 0) {
                    current[0x0A] = 1;
                }
            }
        }
    }
    if ((flags & 1) == 0 && window != NULL) {
        if ((flags & 4) != 0) {
            u8* data = menuDataBiosGetPtr(*(void**)(window + 0x04));
            if ((data[0] & 8) == 0) {
                window[0x0A] = 1;
            }
        }
        if ((flags & 2) != 0) {
            for (current = *(u8**)(lbl_80404ACC + 0x0C);
                 current != NULL; current = *(u8**)(current + 0x10)) {
                if (*(u8**)(current + 0x0C) == window) {
                    *(u8**)(current + 0x0C) = *(u8**)(window + 0x0C);
                }
            }
        }
    }

    active = windowSearchID(*(s32*)(lbl_80404ACC + 0x04));
    found = 0;
    for (window = active; window != NULL; window = *(u8**)(window + 0x0C)) {
        if (window[0x0A] == 0 && window[0x18] != 0) {
            for (current = *(u8**)(lbl_80404ACC + 0x0C);
                 current != NULL; current = *(u8**)(current + 0x10)) {
                u8* link;

                if (current[0x0A] != 0 || current[0x18] != 0) continue;
                link = current;
                if (current != *(u8**)(window + 0x0C)) {
                    do {
                        link = *(u8**)(link + 0x0C);
                        if (link == *(u8**)(window + 0x0C)) {
                            *(s32*)(lbl_80404ACC + 0x04) =
                                *(s32*)(current + 0x04);
                            found = 1;
                            break;
                        }
                    } while (link != NULL);
                }
            }
            if (found) break;
        } else if (window[0x0A] == 0) {
            *(s32*)(lbl_80404ACC + 0x04) = *(s32*)(window + 0x04);
            break;
        }
    }
    if (window == NULL) {
        *(s32*)(lbl_80404ACC + 0x04) = 0;
    }
    return 0;
}

/* 0x80104A94 | 0x20C */
s32 _windowCreateItemSprite__FP14tagWINDOW_WORK(u8* window) {
    u8* menuData = menuDataBiosGetPtr(*(void**)(window + 0x04));
    s16 itemId = *(s16*)(menuData + 0x04);
    u8* item = menuItemBiosGetPtr(itemId);
    s16 width;
    s16 height;

    winSpriteRelease(window + 0x1C);
    _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(item, &width, &height);
    for (;;) {
        u8* sprite;
        u8* spriteData;
        u32 type;
        u16 sequence;

        item = menuItemBiosGetPtr(itemId);
        sprite = winSpriteAdd(window + 0x1C);
        if (sprite == NULL) {
            winSpriteRelease(window + 0x1C);
            return 1;
        }
        *(s16*)(sprite + 0x06) = itemId;
        *(s16*)(sprite + 0x50) = *(s16*)(item + 0x02);
        *(s16*)(sprite + 0x52) = *(s16*)(item + 0x04);
        *(s16*)(sprite + 0x54) = *(s16*)(item + 0x06);
        *(s16*)(sprite + 0x56) = *(s16*)(item + 0x08);
        sprite[0x67] = item[0x01];
        sprite[0x74] = (item[0] >> 4) & 3;

        if (*(s16*)(item + 0x0A) != 0) {
            spriteData = menuSpriteBiosGetPtr(*(s16*)(item + 0x0A));
            type = (spriteData[0] >> 4) & 3;
            if (type == 1) {
                sprite[5] |= 1;
                *(u32*)(sprite + 0x58) = *(u32*)(spriteData + 0x10);
            } else if (type == 2) {
                sprite[5] |= 2;
                *(u32*)(sprite + 0x08) = *(u32*)(spriteData + 0x10);
            }
            *(s16*)(sprite + 0x5C) = *(s16*)(spriteData + 0x08);
            *(s16*)(sprite + 0x5E) = *(s16*)(spriteData + 0x0A);
            *(s16*)(sprite + 0x60) = *(s16*)(spriteData + 0x0C);
            *(s16*)(sprite + 0x62) = *(s16*)(spriteData + 0x0E);
            sprite[0x67] = (u8)(((u32)sprite[0x67] * spriteData[7]) / 255);
            if ((spriteData[0] & 0x40) != 0) {
                *(s16*)(sprite + 0x54) = width;
                *(s16*)(sprite + 0x56) = height;
            }
            sequence = *(u16*)spriteData & 0x0FFF;
            if (sequence != 0) {
                winSetSequence(sprite + 0x0C, sequence);
            }
        }
        *(u32*)(sprite + 0x4C) = *(u32*)(item + 0x10);
        if (*(u32*)(item + 0x14) != 0) {
            sprite[5] |= 8;
            *(u32*)(sprite + 0x48) = *(u32*)(item + 0x14);
        }
        if ((item[0] & 0x40) != 0) break;
        itemId = *(s16*)(item + 0x18);
    }
    return 0;
}

/* 0x80104CA0 | 0x1E0 */
void windowCreateCursorSprite(u8* window) {
    u8* menuData = menuDataBiosGetPtr(*(void**)(window + 0x04));
    u8* item = menuItemBiosGetPtr(
        *(s16*)((u8*)menuDataBiosGetPtr(*(void**)(window + 0x04)) + 0x04));
    u8* selected = NULL;
    s32 index = 0;

    while (item != NULL) {
        if ((item[0] & 0x80) != 0) {
            if ((s8)window[0x95] == index) {
                selected = item;
                break;
            }
            index++;
        }
        if ((item[0] & 0x40) != 0) break;
        item = menuItemBiosGetPtr(*(s16*)(item + 0x18));
    }
    if (selected == NULL) return;

    winSpriteRelease(window + 0x20);
    if (*(s16*)(selected + 0x0C) == 0) return;
    for (item = menuSpriteBiosGetPtr(*(s16*)(selected + 0x0C));
         item != NULL;) {
        u8* sprite = winSpriteAdd(window + 0x20);
        u32 type;
        u16 sequence;

        if (sprite == NULL) {
            winSpriteRelease(window + 0x20);
            return;
        }
        type = (item[0] >> 4) & 3;
        if (type == 1) {
            sprite[5] |= 1;
            *(u32*)(sprite + 0x58) = *(u32*)(item + 0x10);
        } else if (type == 2) {
            sprite[5] |= 2;
            *(u32*)(sprite + 0x08) = *(u32*)(item + 0x10);
        }
        *(s16*)(sprite + 0x06) = *(s16*)(menuData + 0x04);
        *(s16*)(sprite + 0x50) = *(s16*)(selected + 0x02) + (s8)item[5];
        *(s16*)(sprite + 0x52) = *(s16*)(selected + 0x04) + (s8)item[6];
        *(s16*)(sprite + 0x54) = *(s16*)(item + 0x0C);
        *(s16*)(sprite + 0x56) = *(s16*)(item + 0x0E);
        *(s16*)(sprite + 0x5C) = *(s16*)(item + 0x08);
        *(s16*)(sprite + 0x5E) = *(s16*)(item + 0x0A);
        *(s16*)(sprite + 0x60) = *(s16*)(item + 0x0C);
        *(s16*)(sprite + 0x62) = *(s16*)(item + 0x0E);
        sprite[0x67] = item[7];
        sequence = *(u16*)item & 0x0FFF;
        if (sequence != 0) {
            winSetSequence(sprite + 0x0C, sequence);
        }
        if ((item[0] & 0x80) != 0) break;
        item = menuSpriteBiosGetPtr(*(s16*)(item + 0x14));
    }
}

/* 0x80104E80 | 0x474 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void windowOpen(void) {
    /* TODO: match -- 1140 bytes at 0x80104E80 */
}
#pragma pop

/* 0x801052F4 | 0x11C */
void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(u8* item, s16* width, s16* height) {
    s32 minX = 0x280;
    s32 minY = 0x1E0;
    s32 maxX = 0;
    s32 maxY = 0;

    *width = 0;
    *height = 0;
    for (;;) {
        u32 rect;
        s16 textWidth;
        s16 textHeight;
        s16 x = *(s16*)(item + 0x02);
        s16 y = *(s16*)(item + 0x04);

        menuSpriteBiosGetPtr(*(s16*)(item + 0x0A));
        rect = *(void**)(item + 0x10) != NULL
            ? GSmsgGetRect(*(void**)(item + 0x10)) : 0;
        textWidth = (s16)(rect >> 16);
        textHeight = (s16)rect;
        if (x < minX) minX = x;
        if (x + *(s16*)(item + 0x06) > maxX) maxX = x + *(s16*)(item + 0x06);
        if (x + textWidth > maxX) maxX = x + textWidth;
        if (y < minY) minY = y;
        if (y + *(s16*)(item + 0x08) > maxY) maxY = y + *(s16*)(item + 0x08);
        if (y + textHeight > maxY) maxY = y + textHeight;
        if ((item[0] & 0x40) != 0) break;
        item = menuItemBiosGetPtr(*(s16*)(item + 0x18));
    }
    *width = (s16)(maxX - minX);
    *height = (s16)(maxY - minY);
}

/* 0x80105410 | 0xA8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void windowInit(u16 count) {
#pragma optimization_level 4
#pragma peephole off
    extern u8 lbl_80271EC4[];
    extern void winSpriteInit(void);
    u32 size;
    u16 handle;
    void* ptr;

    memset(lbl_80404ACC, 0, 0x9c);
    size = (u16)count * 0xb4;
    handle = _toolentryAlloc__FUl(size);
    GS_MODEL_STATE->entryHandle = handle;
    if ((u16)handle == 0) {
        GSlogWrite((const char*)lbl_80271EC4);
    } else {
        ptr = fn_800E27B0((u16)handle);
        GS_MODEL_STATE->entries = ptr;
        GS_MODEL_STATE->count = count;
        memset(ptr, 0, size);
        winSpriteInit();
    }
}
#pragma pop

/* 0x801054B8 | 0x16C */
void* windowGetPortKeyInfo(u8 ports) {
    u8 masks[4];
    u16* output = (u16*)lbl_80404AB0;
    u8* state = lbl_80404ACC;
    u32 i;

    *(u32*)masks = lbl_8047CDE8;
    memset(output, 0, 0x1A);
    for (i = 0; i < 4; i++) {
        u16* keys = (u16*)(state + 0x2A);

        if ((ports & masks[i]) != 0) {
            output[0] |= keys[0];
            output[1] |= keys[1];
            output[2] |= keys[2];
            output[3] |= keys[3];
            output[4] |= keys[4];
        }
        state += 0x1A;
    }
    return output;
}

/* 0x80105624 | 0x10 */
void* windowGetKeyInfo(void) {
    return (void*)(lbl_80404ACC + 0x10);
}

/* 0x80105634 | 0x298 */
s32 fn_80105634(u8* window, u8* sprite) {
    u32 colors[2];
    f32 alpha = (f32)(((u32)window[0x8B] * sprite[0x67]) / 255);
    s16 id = *(s16*)(sprite + 0x06);

    colors[0] = lbl_8047CDF0;
    colors[1] = lbl_8047CDF4;
    switch (id) {
    case 0x68:
    case 0x75:
    case 0x82:
        ((u8*)colors)[3] = (u8)(((u32)((u8*)colors)[3] * alpha));
        ((u8*)colors)[7] = (u8)(((u32)((u8*)colors)[7] * alpha));
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D6A00(6);
        fn_800D7820((s32)lbl_80314E08);
        fn_800D67BC(4);
        fn_800D61E4(0, 0);
        fn_800D5BA0(0, colors[0]);
        fn_800D61E4(*(s16*)(sprite + 0x54), 0);
        fn_800D5BA0(0, colors[0]);
        fn_800D61E4(*(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56));
        fn_800D5BA0(0, colors[1]);
        fn_800D61E4(0, *(s16*)(sprite + 0x56));
        fn_800D5BA0(0, colors[1]);
        fn_800D6728();
        fn_800FE38C(0, 0, *(s16*)(sprite + 0x54),
                    *(s16*)(sprite + 0x56));
        break;
    case 0x6B:
    case 0x78:
    case 0x83: {
        s16 y;

        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D5648(lbl_8047CDF8);
        fn_800D6A00(1);
        fn_800D7820((s32)lbl_80314E08);
        colors[0] = 0xFFFFFF00 | (u8)(lbl_8047CDFC * alpha);
        for (y = 0; y < *(s16*)(sprite + 0x56); y += 4) {
            fn_800D67BC(2);
            fn_800D61E4(0, y);
            fn_800D5BA0(0, colors[0]);
            fn_800D61E4(*(s16*)(sprite + 0x54), y);
            fn_800D5BA0(0, colors[0]);
            fn_800D6728();
        }
        fn_800FE35C();
        break;
    }
    case 0x69:
    case 0x6A:
    case 0x76:
    case 0x77:
        *(f32*)(sprite + 0x70) =
            (f32)fmod(*(f32*)(sprite + 0x70) + lbl_8047CE00,
                      lbl_8047CE08);
        break;
    }
    return 0;
}
