/**
 * @file window_residual_body.c
 * @brief Residual window implementation shared by non-root window owners.
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
extern int   soundStartSE(u16);
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
extern void gfxVtxPos(f32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void gfxEnd(void);
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
extern u32   GSgetFrameStep(void);
extern u8    lbl_80404B68[];  /* scratch table for fn_80107F38, fn_801081F8 */
extern u8    lbl_80404B8C[];  /* scratch table for winSeqSetMenu */
extern u8    lbl_8047AD10;     /* resource request gate byte (sda21) - authoritative decl, use as-is */

typedef struct WindowVaList {
    u8 gpr;
    u8 fpr;
    u16 reserved;
    u32* overflow_arg_area;
    u32* reg_save_area;
} WindowVaList;

typedef WindowVaList WindowVaListArray[1];

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
extern s32 windowCheckCursor(s32 id, u8 wait);
extern u32 windowGetActiveID(void);
extern void windowCloseMain(void* obj);
extern s32 windowClose(void* ptr, u32 flags);
extern s32 _windowCreateItemSprite__FP14tagWINDOW_WORK(u8* window);
extern void windowCreateCursorSprite(u8* window);
extern void* windowOpen(s32* cursor_out, void* menu_id, u32 parent_id,
                        s32 close_flags, s32 open_param,
                        WindowVaList* args);
extern void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(u8* item, s16* width, s16* height);
extern void windowInit(u16 count);
extern void* windowGetPortKeyInfo(u8 ports);
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


#if defined(WINDOW_RANGE_80104530)
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

/* 0x80104530 | 0x78 */
typedef struct WindowCursorValue {
    u16 cursor;
} WindowCursorValue;

/* Returns the 2-byte cursor record by value; MWCC passes it back in r3,
 * left-justified, which is why callers see the cursor in the high half. */
WindowCursorValue windowGetCursor(s32 param) {
    WindowCursorValue result;
    /* RULE-EXCEPTION(user-approved): the .sbss2 default cursor is read as the 2-byte struct through a cast of the u16 extern - see docs/RULE_EXCEPTIONS.md */
    WindowCursorValue none = *(WindowCursorValue*)&lbl_8047E718;
    u8* window = mdl_find(param);

    if (window != NULL) {
        result = *(WindowCursorValue*)(window + 0x94);
    } else {
        result = none;
    }
    return result;
}

/* windowCheckCursor (0x801045A8 - 0x801046B8) lives in
 * window_exact_801045A8.c. */
#endif

/* windowCloseMain (0x8010474C) lives in window_exact_8010474C.c and
 * windowClose (0x80104828) in window_candidate_80104828.c. */

/* _windowCreateItemSprite (0x80104A94 - 0x80104CA0) lives in
 * window_r50_80104A94_o2.c. */

/* windowCreateCursorSprite, windowOpen and _winCalcWindowSize
 * (0x80104CA0 - 0x80105410) live in window_r50_80104CA0_suffix.c. */


#if defined(WINDOW_RANGE_80105634)
/* 0x80105634 | 0x298
 *
 * The two pool colours are read before the alpha and the sprite id are
 * computed: that is the order retail colours the scratch registers in
 * (colours in r8/r6, the id in r3, the alpha's operands in r6/r5). With the
 * alpha first, every register in the entry block shifts by one.
 *
 * Instruction-exact. The unit stays a candidate because the relocations that
 * remain point at data this carve cannot own: the window TU's shared .sdata2
 * literal pool (0x8047CDE0-0x8047CE20, which also holds the int-to-float bias
 * doubles MWCC emits for the conversions here) and the switch's jump table
 * (jumptable_8035B088). They belong to the whole window TU
 * (0x80103FE4-0x801058CC), which is carved across some twenty files.
 */
s32 fn_80105634(u8* window, u8* sprite) {
    u32 colors[2];
    f32 alpha;
    s16 id;

    colors[0] = lbl_8047CDF0;
    colors[1] = lbl_8047CDF4;
    alpha = (f32)((window[0x8B] * sprite[0x67]) / 65025);
    id = *(s16*)(sprite + 0x06);
    switch (id) {
    case 0x68:
    case 0x75:
    case 0x82:
        ((u8*)colors)[3] = (u8)(((u32)((u8*)colors)[3] * alpha));
        ((u8*)colors)[7] = (u8)(((u32)((u8*)colors)[7] * alpha));
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetPrimitive(6);
        gfxSetVtxDesc((s32)lbl_80314E08);
        gfxBegin(4);
        fn_800D61E4(0, 0);
        fn_800D5BA0(0, colors[0]);
        fn_800D61E4(*(s16*)(sprite + 0x54), 0);
        fn_800D5BA0(0, colors[0]);
        fn_800D61E4(*(s16*)(sprite + 0x54), *(s16*)(sprite + 0x56));
        fn_800D5BA0(0, colors[1]);
        fn_800D61E4(0, *(s16*)(sprite + 0x56));
        fn_800D5BA0(0, colors[1]);
        gfxEnd();
        fn_800FE38C(0, 0, *(s16*)(sprite + 0x54),
                    *(s16*)(sprite + 0x56));
        break;
    case 0x6B:
    case 0x78:
    case 0x83: {
        s16 y;

        gfxSetFlags(1);
        gfxClearFlags(6);
        fn_800D5648(lbl_8047CDF8);
        gfxSetPrimitive(1);
        gfxSetVtxDesc((s32)lbl_80314E08);
        ((u8*)colors)[0] = 0xFF;
        ((u8*)colors)[1] = 0xFF;
        ((u8*)colors)[2] = 0xFF;
        ((u8*)colors)[3] = (u8)(lbl_8047CDFC * alpha);
        for (y = 0; y < *(s16*)(sprite + 0x56); y += 4) {
            gfxBegin(2);
            fn_800D61E4(0, y);
            fn_800D5BA0(0, colors[0]);
            fn_800D61E4(*(s16*)(sprite + 0x54), y);
            fn_800D5BA0(0, colors[0]);
            gfxEnd();
        }
        fn_800FE35C();
        break;
    }
    case 0x69:
    case 0x6A:
    case 0x76:
    case 0x77:
    {
        /* The period is read into its own local, as retail does: its load is
         * scheduled ahead of the addition, not with the call's arguments. The
         * field is written and read back rather than kept in a register, so
         * the rotation is rounded to f32 before fmod sees it. */
        f64 period = lbl_8047CE08;

        *(f32*)(sprite + 0x70) += lbl_8047CE00;
        *(f32*)(sprite + 0x70) =
            (f32)fmod(*(f32*)(sprite + 0x70), period);
    }
        break;
    }
    return 0;
}
#endif
