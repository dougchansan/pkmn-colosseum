/**
 * @file menuFightStatus.c
 * @brief menuFightStatus -- XD translation unit: fight-menu HP/EXP bar
 *        animation state (wait/start-anim pairs for EXP and HP gauges,
 *        active-slot bookkeeping).
 *
 * Address range: 0x80011B4C - 0x80011EA4 (6 functions)
 *
 * Split from game/gs_npc_interact.c (a CodeCandidate bucket spanning two
 * XD-era translation units) into its true source file. See
 * game/menuFight.c for the sibling TU (0x8000DAA8-0x80011B4C). The split
 * point 0x80011B4C is high-confidence: menuFightStatusWaitAnimeEXP/menuFightStatusWaitAnimeEXP
 * and menuFightStatusWaitAnimeHP/menuFightStatusWaitAnimeHP are instruction-identical except
 * for the polled halfword offset, mirroring XD's WaitAnimeEXP/WaitAnimeHP
 * twin pair that opens menuFightStatus.cpp.
 *
 * The following range starts at 0x80011EA4 and is owned by
 * game/gs_range_80011EA4.c.
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

/* renamed symbols referenced by asm incs (symbolmap port) */
extern u32 _menuFightIsUse__FP16MENU_WAZA_STATUSUs();
extern void* menuSubCalcColor(void*, void*);

/* NPC/People system */
extern void  winSpriteSetDisp(void* npc, s32 direction);  /* Set NPC facing */
extern void* menuItemBiosGetPtr(s16 npcId);                  /* Lookup NPC data by ID */

/* Text/dialog system */
extern void  windowDrawSprite();
extern void  winSeqSetMenu(void* ctx, s32 state);       /* Set dialog state */
extern void  fn_801081F8(void* ctx, s32 msgId, s32 flags); /* Display message */

/* Rendering */
extern u32   GSmsgGetRect();                           /* Get model dimensions */
extern void  GSmsgPrintColor();

/* Map/warp */
extern u8    fightFloorCheckFightActionFightOutPokemonIrekaeSelect(s32 p1, void* warpId, void* outDest);
extern void* fightOutPokemonGetNicknamePtr(void* mapData);              /* Get map name string */
extern void  winMsgOpen(s32 slot, s32 msgId, s32 p3, s32 p4);
extern void  winMsgClose(s32 slot);                   /* Close message box */

/* Input/frame */
extern u8    fn_801F18DC(s32 controller);             /* Check input ready */
extern u8    fightFloorIsUseFightTimerCommand(s32 controller);             /* Check button pressed */
extern u8    fightTimerCommandIsOver(void);                       /* Check A button */
extern u16   std103_battleGetResult(void);                       /* Get input state */
extern void  _threadSwitch(void);                       /* Frame advance */
extern u32   GSinputButtonsChanged(s32 slot);                   /* Get render flags */
extern u32   fn_800F7BC4(s32 slot);                   /* Get VSync flags */

/* Battle bridge */
extern void  msgctrlSetValue();                           /* Set battle parameter */
extern void  fightFloorSetStatus();                           /* Configure map object */
extern u32   windowGetParam();                           /* Get participant data */

/* =========================================================================
 * SDA globals
 * ========================================================================= */

/* NPC interaction state variables are scattered across the SDA region.
 * The exact mapping is determined by the lbz/sth instructions that
 * reference r0+offset or r13+offset addressing modes. */

/* 0x78 | menuFightStatusWaitAnimeEXP | generic */
extern u32 windowSearchID();
extern u8* windowGetFreeWork();
u32 menuFightStatusWaitAnimeEXP(u32 arg1, u8 arg2) {
    u32 r;
    while (1) {
        if (!(r = windowSearchID(arg1))) return 0;
        if (*(s16*)((u8*)windowGetFreeWork(r) + 0xc) == 0) return 0;
        if (arg2 != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

/* 0x80011BC4 | 0xB4 */
extern u8* windowGetAllocPtr();
extern void soundStartSE(u32 val);
#if 0
asm void menuFightStatusStartAnimEXP(void) {
#include "src/game/gs_npc_interact_fn_80011BC4.inc"
}
#else
void menuFightStatusStartAnimEXP(u32 arg1, u32 target) {
    u32 ptr;
    u32 state;
    u32 data;
    u32 diff;
    s16 score;
    ptr = windowSearchID(arg1);
    if (!ptr) { return; }
    state = (u32)windowGetAllocPtr(ptr);
    data = (u32)windowGetFreeWork(ptr);
    if (target > *(u32*)(state + 0x20)) {
        diff = target - *(u32*)(state + 0x20);
    } else {
        diff = *(u32*)(state + 0x20) - target;
    }
    score = diff * 100 / *(u32*)(state + 0x1c);
    *(s16*)(data + 0xc) = score;
    if (*(s16*)(data + 0xc) < 0xf) { *(s16*)(data + 0xc) = 0xf; }
    *(u32*)(data + 0x8) = *(u32*)(state + 0x20);
    *(u32*)(state + 0x20) = target;
    *(s16*)(data + 0xe) = 0;
    soundStartSE(0x4d0);
}
#endif

/* 0x78 | menuFightStatusWaitAnimeHP | generic */
u32 menuFightStatusWaitAnimeHP(u32 arg1, u8 arg2) {
    u32 r;
    while (1) {
        if (!(r = windowSearchID(arg1))) return 0;
        if (*(s16*)((u8*)windowGetFreeWork(r) + 0x2) == 0) return 0;
        if (arg2 != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

/* 0x80011CF0 | 0xAC */
#if 0
asm void menuFightStatusStartAnimHP(void) {
#include "src/game/gs_npc_interact_fn_80011CF0.inc"
}
#else
void menuFightStatusStartAnimHP(u32 arg1, s16 target) {
    u32 ptr;
    u32 state;
    u32 data;
    s32 diff;

    ptr = windowSearchID(arg1);
    if (ptr == 0) {
        return;
    }
    state = (u32)windowGetAllocPtr(ptr);
    data = (u32)windowGetFreeWork(ptr);
    diff = *(s16*)(state + 0x1a) - (s16)target;
    if (diff < 0) {
        diff = -diff;
    }
    *(s16*)(data + 2) = (s16)((diff * 100) / *(s16*)(state + 0x18));
    if (*(s16*)(data + 2) <= 0) {
        *(s16*)(data + 2) = 1;
    }
    *(s16*)(data + 0) = *(s16*)(state + 0x1a);
    *(s16*)(state + 0x1a) = target;
    *(s16*)(data + 4) = 0;
}
#endif

/* 0x80011D9C | 0xCC - item-kind resolver + dispatch (same switch as fn_800129A8) */
#if 0
asm void menuFightStatusSetActive(void) {
#include "src/game/gs_npc_interact_fn_80011D9C.inc"
}
#else
void menuFightStatusSetActive(s32 id, s32 do_extra) {
    u32 resolved;
    s32 kind;
    kind = 0;
    resolved = windowSearchID(id);
    if (resolved == 0) return;
    switch (id) {
    case 0x45: case 0x46: case 0x49:
        kind = 0x538;
        break;
    case 0x47: case 0x48: case 0x4a:
        kind = 0x540;
        break;
    }
    if (do_extra != 0) {
        fn_80103F74(resolved, kind, 1);
        fn_801081F8((void*)resolved, kind, 0x2d);
    } else {
        fn_80103F74(resolved, kind, 0);
    }
}
#endif

/* 0x80011E68 | 0x3C */
/* Set an NPC's facing direction. */
void menuFightStatusSetHP(u32 npcId, u16 direction) {
    extern void* windowGetAllocPtr(void* obj);
    extern void* windowSearchID(u32 npcId);
    void* npc;

    npc = windowSearchID(npcId);
    if (npc != NULL) {
        u8* obj = (u8*)windowGetAllocPtr(npc);
        *(u16*)(obj + 0x1A) = direction;
    }
}
