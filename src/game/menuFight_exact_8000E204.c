/**
 * @file menuFight_exact_8000E204.c
 * @brief menuFightDrawTargetCursor and menuFightCtrlTarget, 0x8000E204 - 0x8000E290.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c). GC/1.3
 * -O4,p with the TU's unit-wide -opt nopeephole, no pragmas.
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
extern void menuClose();

/* menuFightDrawTargetCursor - 0x8000E204 | size: 0x88 */
extern void menuItemBiosGetSelectFlag(void);
extern void menuGetCursorItemID(void);
void menuFightDrawTargetCursor(u8* arg1, u8* arg2) {
    extern u32 menuItemBiosGetSelectFlag(s16 val);
    extern s32 menuGetCursorItemID(u32 val);
    extern void winSpriteSetDisp(u8* a, u32 b);
    if ((u8)menuItemBiosGetSelectFlag(*(s16*)(arg2 + 6)) != 0) {
        if (*(s16*)(arg2 + 6) == menuGetCursorItemID(*(u32*)(arg1 + 4))) {
            winSpriteSetDisp(arg2, 1);
        } else {
            winSpriteSetDisp(arg2, 0);
        }
    } else {
        winSpriteSetDisp(arg2, 0);
    }
}

/* menuFightCtrlTarget - 0x8000E28C | size: 0x4 */
void menuFightCtrlTarget(void) {}
