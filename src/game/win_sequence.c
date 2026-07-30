/**
 * @file win_sequence.c
 * @brief winSeq/winSet -- per-window animation sequence assignment and
 *        the shared _winSeqMoveSub per-frame mover (Colosseum "menu" UI
 *        subsystem). Split from the gs_model.c splitter bucket (address
 *        range 0x80106F98 - 0x80108580, 11 fns, 10 with real C bodies;
 *        _winSeqMoveSub at 0x801074D4 remains asm-only). Corresponds to
 *        XD's winSeq TU (0x8010C3E4-0x8010D1A0); anchors winSeqCheckMove/
 *        winSeqIsCheck/winSetSequence strictly monotonic with exact size
 *        matches.
 */
#include "dolphin/types.h"

typedef struct WinSeqCommand {
    u8 flags;
    u8 type;
    s16 duration;
    s32 value0;
    s32 value1;
} WinSeqCommand;

typedef struct WinSeqState {
    WinSeqCommand* commands;
    s16 commandIndex;
    s16 delay;
    u8 positionMode;
    u8 colorMode;
    u8 scaleMode;
    u8 loopActive;
    s16 startX;
    s16 startY;
    s16 endX;
    s16 endY;
    s16 positionFrame;
    s16 positionDuration;
    u8 startColor[4];
    u8 endColor[4];
    s16 colorFrame;
    s16 colorDuration;
    f32 startScaleX;
    f32 startScaleY;
    f32 endScaleX;
    f32 endScaleY;
    s16 scaleFrame;
    s16 scaleDuration;
    s16 loopCount;
    u8 enabled;
    u8 pad_3B;
} WinSeqState;

typedef struct WinSeqTarget {
    s16 x;
    s16 y;
    u8 color[4];
    s16 baseX;
    s16 baseY;
    f32 scaleX;
    f32 scaleY;
    u32 image;
    s16 imageX;
    s16 imageY;
    s16 imageW;
    s16 imageH;
    u8 flags;
} WinSeqTarget;

typedef struct WinSeqSpriteData {
    u8 pad_00[8];
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    u32 image;
} WinSeqSpriteData;

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
extern f32 lbl_8047CE20;  /* sdata2: 100.0f */
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
extern void fn_80106F98(s32 windowId);
extern void _winSeqMoveSub(void* targetPtr, void* statePtr);
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

/* 0x80106F98 | 0x15C */
void fn_80106F98(s32 windowId) {
    u8* window = windowSearchID(windowId);
    u8* node;

    if (window == NULL) return;
    for (node = *(u8**)(window + 0x20); node != NULL; node = *(u8**)node) {
        u8* scratch = lbl_80404B68;
        u32 i;
        u8* item;

        *(u32*)(node + 0x0C) = 0;
        *(u16*)(node + 0x10) = 0;
        *(s16*)(scratch + 0x00) = *(s16*)(node + 0x50);
        *(s16*)(scratch + 0x02) = *(s16*)(node + 0x52);
        *(u32*)(scratch + 0x04) = *(u32*)(node + 0x64);
        *(f32*)(scratch + 0x0C) = *(f32*)(node + 0x68);
        *(f32*)(scratch + 0x10) = *(f32*)(node + 0x6C);
        scratch[0x20] = node[0x04];
        *(u32*)(scratch + 0x14) = *(u32*)(node + 0x58);
        *(s16*)(scratch + 0x18) = *(s16*)(node + 0x5C);
        *(s16*)(scratch + 0x1A) = *(s16*)(node + 0x5E);
        *(s16*)(scratch + 0x1C) = *(s16*)(node + 0x60);
        *(s16*)(scratch + 0x1E) = *(s16*)(node + 0x62);
        item = menuItemBiosGetPtr(*(s16*)(node + 0x06));
        *(s16*)(scratch + 0x08) = *(s16*)(item + 0x02);
        item = menuItemBiosGetPtr(*(s16*)(node + 0x06));
        *(s16*)(scratch + 0x0A) = *(s16*)(item + 0x04);
        for (i = 0; i < fn_800D3088(); i++) {
            _winSeqMoveSub(scratch, node + 0x0C);
        }
        *(s16*)(node + 0x50) = *(s16*)(scratch + 0x00);
        *(s16*)(node + 0x52) = *(s16*)(scratch + 0x02);
        *(u32*)(node + 0x64) = *(u32*)(scratch + 0x04);
        *(f32*)(node + 0x68) = *(f32*)(scratch + 0x0C);
        *(f32*)(node + 0x6C) = *(f32*)(scratch + 0x10);
        node[0x04] = scratch[0x20];
        *(u32*)(node + 0x58) = *(u32*)(scratch + 0x14);
        *(s16*)(node + 0x5C) = *(s16*)(scratch + 0x18);
        *(s16*)(node + 0x5E) = *(s16*)(scratch + 0x1A);
        *(s16*)(node + 0x60) = *(s16*)(scratch + 0x1C);
        *(s16*)(node + 0x62) = *(s16*)(scratch + 0x1E);
    }
}

/* 0x801070F4 | 0x7C */
s32 winSeqCheckMove(s32 param) {
    void* r3 = windowSearchID(param);
    if (r3 == (void*)0) { return 0; }
    if (*(void**)((u8*)r3 + 0x24) != (void*)0) { return 1; }
    {
        void* node = *(void**)((u8*)r3 + 0x1c);
        while (node != (void*)0) {
            if (*(void**)((u8*)node + 0xc) != (void*)0) {
                if ((u8)*(u8*)((u8*)node + 0x46) == 0) { return 1; }
            }
            node = *(void**)((u8*)node + 0x0);
        }
        return 0;
    }
}

/* 0x80107170 | 0x60 */
s32 fn_80107170(s32 r3, s32 r31) {
    void* r3r = windowSearchID(r3);
    void* result = windowSearchItemID(r3r, r31);
    if (result == (void*)0) { goto _ret0; }
    if (*(void**)((u8*)result + 0xc) == (void*)0) { goto _ret0; }
    if ((u8)*(u8*)((u8*)result + 0x46) != 0) { goto _ret0; }
    return 1;
_ret0:
    return 0;
}

/* 0x801071D0 | 0x304 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void winSeqMoveMenu(void) {
    /* TODO: match -- 772 bytes at 0x801071D0 */
}
#pragma pop

/* 0x801074D4 | 0x9A4 */
void _winSeqMoveSub(void* targetPtr, void* statePtr) {
    extern f64 sqrt(f64 value);
    WinSeqTarget* target = targetPtr;
    WinSeqState* state = statePtr;
    WinSeqCommand* command;
    WinSeqSpriteData* sprite;
    f32 t;
    u8 stop;

    stop = 0;
    if (state->commands == NULL) {
        return;
    }

    if (state->positionMode != 0) {
        state->positionFrame++;
        switch (state->positionMode) {
        case 1:
            t = (f32)state->positionFrame / (f32)state->positionDuration;
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        case 2:
            t = ((f32)state->positionFrame * (f32)state->positionFrame /
                 (f32)state->positionDuration) /
                (f32)state->positionDuration;
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        case 3:
            {
                f64 durationRoot = sqrt((f64)state->positionDuration);
                t = (f32)(sqrt((f64)state->positionFrame) / durationRoot);
            }
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        }
        if (state->positionFrame >= state->positionDuration) {
            state->positionMode = 0;
        }
    }

    if (state->colorMode != 0) {
        state->colorFrame++;
        if (state->colorMode == 1) {
            t = (f32)state->colorFrame / (f32)state->colorDuration;
            target->color[0] = state->startColor[0] +
                               t * (state->endColor[0] - state->startColor[0]);
            target->color[1] = state->startColor[1] +
                               t * (state->endColor[1] - state->startColor[1]);
            target->color[2] = state->startColor[2] +
                               t * (state->endColor[2] - state->startColor[2]);
            target->color[3] = state->startColor[3] +
                               t * (state->endColor[3] - state->startColor[3]);
        }
        if (state->colorFrame >= state->colorDuration) {
            state->colorMode = 0;
        }
    }

    if (state->scaleMode != 0) {
        state->scaleFrame++;
        if (state->scaleMode == 1) {
            t = (f32)state->scaleFrame / (f32)state->scaleDuration;
            target->scaleX = state->startScaleX +
                             t * (state->endScaleX - state->startScaleX);
            target->scaleY = state->startScaleY +
                             t * (state->endScaleY - state->startScaleY);
        }
        if (state->scaleFrame >= state->scaleDuration) {
            state->scaleMode = 0;
        }
    }

next_command:
    if (state->delay > 0) {
        state->delay--;
        return;
    }

    command = &state->commands[state->commandIndex];
    switch (command->type) {
    case 0:
        state->commands = NULL;
        state->commandIndex = 0;
        stop = 1;
        break;
    case 1:
        state->delay = command->duration;
        break;
    case 2:
        if (command->value0 != 0) {
            target->flags = (s8)(target->flags | 2);
        } else {
            target->flags = (s8)(target->flags & ~2);
        }
        break;
    case 3:
    case 9:
        if (command->type == 9) {
            state->endX = target->baseX;
            state->endY = target->baseY;
        } else if ((command->flags & 0x40) == 0) {
            state->endX = (s16)command->value0;
            state->endY = (s16)command->value1;
        } else {
            state->endX = (s16)(target->x + command->value0);
            state->endY = (s16)(target->y + command->value1);
        }
        if ((command->flags & 2) != 0) {
            state->endX = target->x;
        }
        if ((command->flags & 4) != 0) {
            state->endY = target->y;
        }
        state->positionDuration = command->duration;
        if (state->positionDuration == 0) {
            target->x = state->endX;
            target->y = state->endY;
            state->positionMode = 0;
        } else {
            state->startX = target->x;
            state->startY = target->y;
            state->positionFrame = 0;
            switch ((command->flags >> 3) & 3) {
            case 0:
                state->positionMode = 1;
                break;
            case 1:
                state->positionMode = 2;
                break;
            case 2:
                state->positionMode = 3;
                break;
            case 3:
                state->positionMode = 1;
                break;
            }
        }
        break;
    case 4:
    case 5:
        break;
    case 6:
        state->startScaleX = target->scaleX;
        state->startScaleY = target->scaleY;
        state->endScaleX = (f32)command->value0 / lbl_8047CE20;
        state->endScaleY = (f32)command->value1 / lbl_8047CE20;
        state->scaleFrame = 0;
        state->scaleDuration = command->duration;
        state->scaleMode = 1;
        if (state->scaleDuration == 0) {
            target->scaleX = state->endScaleX;
            target->scaleY = state->endScaleY;
            state->scaleMode = 0;
        }
        break;
    case 7:
        state->startColor[0] = target->color[0];
        state->startColor[1] = target->color[1];
        state->startColor[2] = target->color[2];
        state->startColor[3] = target->color[3];
        state->endColor[0] = command->value0 >> 24;
        state->endColor[1] = command->value0 >> 16;
        state->endColor[2] = command->value0 >> 8;
        state->endColor[3] = command->value0;
        state->colorFrame = 0;
        state->colorDuration = command->duration;
        state->colorMode = 1;
        if (state->colorDuration == 0) {
            target->color[0] = state->endColor[0];
            target->color[1] = state->endColor[1];
            target->color[2] = state->endColor[2];
            target->color[3] = state->endColor[3];
            state->colorMode = 0;
        }
        break;
    case 8:
        sprite = menuSpriteBiosGetPtr(command->value0);
        target->image = sprite->image;
        target->imageX = sprite->x;
        target->imageY = sprite->y;
        target->imageW = sprite->width;
        target->imageH = sprite->height;
        break;
    case 10:
        if (state->loopActive == 0) {
            state->loopActive = 1;
            state->loopCount = command->duration;
        }
        if ((command->flags & 0x40) == 0) {
            state->loopCount--;
        }
        if (state->loopCount < 0) {
            state->loopCount = 0;
            state->loopActive = 0;
        } else {
            state->commandIndex = (s16)command->value0;
            goto next_command;
        }
        break;
    case 11:
        state->enabled = command->value0 != 0;
        break;
    }

    if (stop != 0) {
        return;
    }
    state->commandIndex++;
    goto next_command;
}


/* 0x80107E78 | 0x60 */
#pragma push
#pragma peephole off
s32 fn_80107E78(void* r3, s32 r4, u16 r30) {
    void* r31 = windowSearchItemID(r3, r4);
    if (r31 == (void*)0) { goto _ret0; }
    {
        void* r3b = menuSeqBiosGetPtr((u32)(u16)r30);
        if (*(void**)((u8*)r31 + 0xc) != r3b) { goto _ret0; }
        return 1;
    }
_ret0:
    return 0;
}
#pragma pop

/* 0x80107ED8 | 0x60 */
#pragma push
#pragma peephole off
s32 winSeqIsCheck(s32 r3, u16 r30) {
    void* r31 = windowSearchID(r3);
    if (r31 == (void*)0) { goto _ret0; }
    {
        void* r3b = menuSeqBiosGetPtr((u32)(u16)r30);
        if (*(void**)((u8*)r31 + 0x24) != r3b) { goto _ret0; }
        return 1;
    }
_ret0:
    return 0;
}
#pragma pop

/* 0x80107F38 | 0x194 */
#pragma push
#pragma optimization_level 2
#pragma push
#pragma optimization_level 1
void fn_80107F38(s32 param, u32 key) {
    u32 r28 = key;
    void* r3 = windowSearchID(param);
    if (r3 == (void*)0) { return; }
    {
        u8* r31 = lbl_80404B68;
        u32 r30 = (u32)(u16)r28;
        void* r29 = *(void**)((u8*)r3 + 0x20);
        while (r29 != (void*)0) {
            if (r30 == 0) {
                *(u32*)((u8*)r29 + 0xc) = 0;
                *(u16*)((u8*)r29 + 0x10) = 0;
            } else {
                memset((u8*)r29 + 0xc, 0, 0x3c);
                *(void**)((u8*)r29 + 0xc) = menuSeqBiosGetPtr(r30);
            }
            *(s16*)(r31 + 0x0) = *(s16*)((u8*)r29 + 0x50);
            *(s16*)(r31 + 0x2) = *(s16*)((u8*)r29 + 0x52);
            *(u32*)(r31 + 0x4) = *(u32*)((u8*)r29 + 0x64);
            *(f32*)(r31 + 0xc) = *(f32*)((u8*)r29 + 0x68);
            *(f32*)(r31 + 0x10) = *(f32*)((u8*)r29 + 0x6c);
            *(u8*)(r31 + 0x20) = *(u8*)((u8*)r29 + 0x4);
            *(u32*)(r31 + 0x14) = *(u32*)((u8*)r29 + 0x58);
            *(s16*)(r31 + 0x18) = *(s16*)((u8*)r29 + 0x5c);
            *(s16*)(r31 + 0x1a) = *(s16*)((u8*)r29 + 0x5e);
            *(s16*)(r31 + 0x1c) = *(s16*)((u8*)r29 + 0x60);
            *(s16*)(r31 + 0x1e) = *(s16*)((u8*)r29 + 0x62);
            *(s16*)(r31 + 0x8) = *(s16*)((u8*)menuItemBiosGetPtr(*(s16*)((u8*)r29 + 0x6)) + 0x2);
            *(s16*)(r31 + 0xa) = *(s16*)((u8*)menuItemBiosGetPtr(*(s16*)((u8*)r29 + 0x6)) + 0x4);
            r28 = 0;
            while (r28 < fn_800D3088()) {
                _winSeqMoveSub(r31, (u8*)r29 + 0xc);
                r28++;
            }
            *(s16*)((u8*)r29 + 0x50) = *(s16*)(r31 + 0x0);
            *(s16*)((u8*)r29 + 0x52) = *(s16*)(r31 + 0x2);
            *(u32*)((u8*)r29 + 0x64) = *(u32*)(r31 + 0x4);
            *(f32*)((u8*)r29 + 0x68) = *(f32*)(r31 + 0xc);
            *(f32*)((u8*)r29 + 0x6c) = *(f32*)(r31 + 0x10);
            *(u8*)((u8*)r29 + 0x4) = *(u8*)(r31 + 0x20);
            *(u32*)((u8*)r29 + 0x58) = *(u32*)(r31 + 0x14);
            *(s16*)((u8*)r29 + 0x5c) = *(s16*)(r31 + 0x18);
            *(s16*)((u8*)r29 + 0x5e) = *(s16*)(r31 + 0x1a);
            *(s16*)((u8*)r29 + 0x60) = *(s16*)(r31 + 0x1c);
            *(s16*)((u8*)r29 + 0x62) = *(s16*)(r31 + 0x1e);
            r29 = *(void**)r29;
        }
    }
}
#pragma pop

#pragma pop

/* 0x801080CC | 0x12C */
#pragma push
#pragma optimization_level 2
#pragma peephole off
void winSeqSetMenu(s32 param, u32 key) {
    u32 r28 = key;
    void* r31 = windowSearchID(param);
    if (r31 == (void*)0) { goto _ret80CC; }
    if ((u32)(u16)r28 == 0) {
        *(u32*)((u8*)r31 + 0x24) = 0;
        *(u16*)((u8*)r31 + 0x28) = 0;
    } else {
        memset((u8*)r31 + 0x24, 0, 0x3c);
        *(void**)((u8*)r31 + 0x24) = menuSeqBiosGetPtr((u32)(u16)r28);
    }
    {
        u8* r29 = lbl_80404B8C;
        u8* r30;
        *(s16*)(r29 + 0x0) = *(s16*)((u8*)r31 + 0x84);
        *(s16*)(r29 + 0x2) = *(s16*)((u8*)r31 + 0x86);
        *(u32*)(r29 + 0x4) = *(u32*)((u8*)r31 + 0x88);
        *(u8*)(r29 + 0x20) = *(u8*)r31;
        { extern void* menuDataBiosGetPtr(void*);
          *(s16*)(r29 + 0x8) = *(s16*)((u8*)menuDataBiosGetPtr(*(void**)((u8*)r31 + 0x4)) + 0x6); }
        { extern void* menuDataBiosGetPtr(void*); void* t;
          t = menuDataBiosGetPtr(*(void**)((u8*)r31 + 0x4));
          r30 = lbl_80404B8C;
          r28 = 0;
          *(s16*)(r30 + 0xa) = *(s16*)((u8*)t + 0x8); }
        while (r28 < fn_800D3088()) {
            _winSeqMoveSub(r30, (u8*)r31 + 0x24);
            r28++;
        }
        *(s16*)((u8*)r31 + 0x84) = *(s16*)(r29 + 0x0);
        *(s16*)((u8*)r31 + 0x86) = *(s16*)(r29 + 0x2);
        *(u32*)((u8*)r31 + 0x88) = *(u32*)(r29 + 0x4);
        *(u8*)r31 = *(u8*)(r29 + 0x20);
    }
_ret80CC:;
}
#pragma pop

/* 0x801081F8 | 0x320 */
#pragma peephole off
void fn_801081F8(void* r3_arg, u16 r4, u16 r5) {
    u16 r25 = r5;
    if (r3_arg == (void*)0) {
        s32 r29 = (s32)(u16)r4;
        u32 r28 = (u32)(u16)r25;
        s32 r26 = 0;
        u8* r30 = lbl_80404B68;
        s32 r31 = 0;
        while (r26 < 0x168) {
            u8* r27 = lbl_8047AD1C + r31;
            if ((s8)*(u8*)(r27 + 0x4) != 0 && *(s16*)(r27 + 0x6) == (s16)r29) {
                if ((u16)r25 == 0) {
                    *(u32*)(r27 + 0xc) = 0;
                    *(u16*)(r27 + 0x10) = 0;
                } else {
                    memset(r27 + 0xc, 0, 0x3c);
                    *(void**)(r27 + 0xc) = menuSeqBiosGetPtr(r28);
                }
                *(s16*)(r30 + 0x0) = *(s16*)(r27 + 0x50);
                *(s16*)(r30 + 0x2) = *(s16*)(r27 + 0x52);
                *(u32*)(r30 + 0x4) = *(u32*)(r27 + 0x64);
                *(f32*)(r30 + 0xc) = *(f32*)(r27 + 0x68);
                *(f32*)(r30 + 0x10) = *(f32*)(r27 + 0x6c);
                *(u8*)(r30 + 0x20) = *(u8*)(r27 + 0x4);
                *(u32*)(r30 + 0x14) = *(u32*)(r27 + 0x58);
                *(s16*)(r30 + 0x18) = *(s16*)(r27 + 0x5c);
                *(s16*)(r30 + 0x1a) = *(s16*)(r27 + 0x5e);
                *(s16*)(r30 + 0x1c) = *(s16*)(r27 + 0x60);
                *(s16*)(r30 + 0x1e) = *(s16*)(r27 + 0x62);
                {
                    void* t;
                    t = menuItemBiosGetPtr(*(s16*)(r27 + 0x6));
                    *(s16*)(r30 + 0x8) = *(s16*)((u8*)t + 0x2);
                    t = menuItemBiosGetPtr(*(s16*)(r27 + 0x6));
                    *(s16*)(r30 + 0xa) = *(s16*)((u8*)t + 0x4);
                }
                {
                    u8* r24 = lbl_80404B68;
                    s32 r23 = 0;
                    do {
                        _winSeqMoveSub(r24, r27 + 0xc);
                        r23++;
                    } while ((u32)r23 < fn_800D3088());
                }
                *(s16*)(r27 + 0x50) = *(s16*)(r30 + 0x0);
                *(s16*)(r27 + 0x52) = *(s16*)(r30 + 0x2);
                *(u32*)(r27 + 0x64) = *(u32*)(r30 + 0x4);
                *(f32*)(r27 + 0x68) = *(f32*)(r30 + 0xc);
                *(f32*)(r27 + 0x6c) = *(f32*)(r30 + 0x10);
                *(u8*)(r27 + 0x4) = *(u8*)(r30 + 0x20);
                *(u32*)(r27 + 0x58) = *(u32*)(r30 + 0x14);
                *(s16*)(r27 + 0x5c) = *(s16*)(r30 + 0x18);
                *(s16*)(r27 + 0x5e) = *(s16*)(r30 + 0x1a);
                *(s16*)(r27 + 0x60) = *(s16*)(r30 + 0x1c);
                *(s16*)(r27 + 0x62) = *(s16*)(r30 + 0x1e);
            }
            r31 += 0x78;
            r26++;
        }
    } else {
        void* r30 = windowSearchItemID(r3_arg, (s32)r4);
        if (r30 == (void*)0) { return; }
        if ((u32)(u16)r25 == 0) {
            *(u32*)((u8*)r30 + 0xc) = 0;
            *(u16*)((u8*)r30 + 0x10) = 0;
        } else {
            memset((u8*)r30 + 0xc, 0, 0x3c);
            *(void**)((u8*)r30 + 0xc) = menuSeqBiosGetPtr((u32)(u16)r25);
        }
        {
            u8* r31 = lbl_80404B68;
            *(s16*)(r31 + 0x0) = *(s16*)((u8*)r30 + 0x50);
            *(s16*)(r31 + 0x2) = *(s16*)((u8*)r30 + 0x52);
            *(u32*)(r31 + 0x4) = *(u32*)((u8*)r30 + 0x64);
            *(f32*)(r31 + 0xc) = *(f32*)((u8*)r30 + 0x68);
            *(f32*)(r31 + 0x10) = *(f32*)((u8*)r30 + 0x6c);
            *(u8*)(r31 + 0x20) = *(u8*)((u8*)r30 + 0x4);
            *(u32*)(r31 + 0x14) = *(u32*)((u8*)r30 + 0x58);
            *(s16*)(r31 + 0x18) = *(s16*)((u8*)r30 + 0x5c);
            *(s16*)(r31 + 0x1a) = *(s16*)((u8*)r30 + 0x5e);
            *(s16*)(r31 + 0x1c) = *(s16*)((u8*)r30 + 0x60);
            *(s16*)(r31 + 0x1e) = *(s16*)((u8*)r30 + 0x62);
            {
                void* t;
                t = menuItemBiosGetPtr(*(s16*)((u8*)r30 + 0x6));
                *(s16*)(r31 + 0x8) = *(s16*)((u8*)t + 0x2);
                t = menuItemBiosGetPtr(*(s16*)((u8*)r30 + 0x6));
                {
                    u8* r25b = lbl_80404B68;
                    s32 r23 = 0;
                    *(s16*)(r25b + 0xa) = *(s16*)((u8*)t + 0x4);
                    do {
                        _winSeqMoveSub(r25b, (u8*)r30 + 0xc);
                        r23++;
                    } while ((u32)r23 < fn_800D3088());
                }
            }
            *(s16*)((u8*)r30 + 0x50) = *(s16*)(r31 + 0x0);
            *(s16*)((u8*)r30 + 0x52) = *(s16*)(r31 + 0x2);
            *(u32*)((u8*)r30 + 0x64) = *(u32*)(r31 + 0x4);
            *(f32*)((u8*)r30 + 0x68) = *(f32*)(r31 + 0xc);
            *(f32*)((u8*)r30 + 0x6c) = *(f32*)(r31 + 0x10);
            *(u8*)((u8*)r30 + 0x4) = *(u8*)(r31 + 0x20);
            *(u32*)((u8*)r30 + 0x58) = *(u32*)(r31 + 0x14);
            *(s16*)((u8*)r30 + 0x5c) = *(s16*)(r31 + 0x18);
            *(s16*)((u8*)r30 + 0x5e) = *(s16*)(r31 + 0x1a);
            *(s16*)((u8*)r30 + 0x60) = *(s16*)(r31 + 0x1c);
            *(s16*)((u8*)r30 + 0x62) = *(s16*)(r31 + 0x1e);
        }
    }
}
#pragma peephole reset

/* 0x80108518 | 0x68 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void winSetSequence(void* out, u32 idx) {
#pragma optimization_level 4
#pragma peephole off
    if ((u16)idx == 0) {
        *(u32*)((u8*)out + 0x0) = 0;
        *(u16*)((u8*)out + 0x4) = 0;
    } else {
        memset(out, 0, 0x3c);
        *(void**)((u8*)out + 0x0) = menuSeqBiosGetPtr((u32)(u16)idx);
    }
}
#pragma pop
