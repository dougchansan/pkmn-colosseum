/**
 * @file menuFight_exact_8000F310.c
 * @brief menuFightCtrlSecretPokemonTop and menuFightDrawSecretSelect, 0x8000F310 - 0x8000F400.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c). GC/1.3
 * -O4,p with the TU's unit-wide -opt nopeephole, no pragmas. Owns the
 * DrawSecretSelect jump table (jumptable_802E4C80, .data 0x802E4C80-0x802E4CA8).
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

/* menuFightCtrlSecretPokemonTop - 0x8000F310 | size: 0x4c */
u32 menuFightCtrlSecretPokemonTop(u32 arg) {
    windowGetParam((void*)arg, 0);
    windowGetParam((void*)arg, 1);
    windowGetParam((void*)arg, 2);
    return 0;
}

/* menuFightDrawSecretSelect - 0x8000F35C | size: 0xa4 */
extern void jumptable_802E4C80();
void menuFightDrawSecretSelect(u8* ctx, u8* npc) {
    s32 visible;
    s32 id;
    s32 idx;
    s32 state;

    visible = 1;
    state = *(s32*)(ctx + 4);
    if (state == 0xF7) {
        goto visibility_done;
    }
    if (state == 0xF8) {
        if ((u8)windowGetParam(ctx, 2) == 0) {
            visible = 0;
        }
    }
visibility_done:
    id = *(s16*)(npc + 6);
    idx = id - 0x11CE;
    switch (idx) {
    case 1:
    case 5:
    case 8:
        winSpriteSetDisp(npc, 1);
        break;
    case 0:
    case 6:
    case 9:
        winSpriteSetDisp(npc, visible);
        break;
    default:
        break;
    }
}
