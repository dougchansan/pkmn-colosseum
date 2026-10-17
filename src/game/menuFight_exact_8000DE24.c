/**
 * @file menuFight_exact_8000DE24.c
 * @brief menuFightButtonSecretKousan, 0x8000DE24 - 0x8000DEC4.
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

/* menuFightButtonSecretKousan - 0x8000DE24 | size: 0xa0 */
extern void menuButtonNormal(void);
void menuFightButtonSecretKousan(u8* ptr) {
    extern void menuButtonNormal(u8* a);
    extern u8 fn_801F18DC(s32 a);
    extern u8 fightFloorIsUseFightTimerCommand(s32 a);
    extern u8 fightTimerCommandIsOver(void);
    extern u16 std103_battleGetResult(void);
    u8 flag;
    menuButtonNormal(ptr);
    if (!(u8)fn_801F18DC(0)) goto _zero;
    if ((u8)fightFloorIsUseFightTimerCommand(0) == 1 && (u8)fightTimerCommandIsOver() == 1) { flag = 1; goto _check; }
    if ((u16)std103_battleGetResult() == 1) { flag = 1; goto _check; }
    _zero:
    flag = 0;
    _check:
    if (flag) {
        ptr[0x98] = 1;
        ptr[0x99] = 1;
    }
}
