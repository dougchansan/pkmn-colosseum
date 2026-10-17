/**
 * @file menuFight_exact_800100C0.c
 * @brief menuFightCtrlBall and _menuFightIsUse, 0x800100C0 - 0x80010294.
 *
 * Function-boundary carve of the menuFight TU (see menuFight.c): the
 * ball window control and the move-usability check. GC/1.3
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
extern f64 atan2(f64, f64);

/* menuFightCtrlBall - 0x800100C0 | size: 0x68 */
u32 menuFightCtrlBall(u8* ptr) {
    extern void* windowGetFreeWork(u8* a);
    extern void* windowGetParam(u8* a, u32 b);
    void* dst = windowGetFreeWork(ptr);
    if ((s8)ptr[1] == 0) {
        memcpy(dst, windowGetParam(ptr, 0), 6);
    }
    return 0;
}

/* _menuFightIsUse__FP16MENU_WAZA_STATUSUs - 0x80010128 | size: 0x16c */
extern u32 fightOutPokemonCheckCanOutOkWazaBanme();
extern u32 fightOutPokemonGetSoubiItemDataId();
u32 _menuFightIsUse__FP16MENU_WAZA_STATUSUs(ctx, arg)
u8* ctx;
u16 arg;
{
    extern u32 fightOutPokemonGetPokemonPtr(void*);
    extern u32 fightOutPokemonCheckCanOutOkWazaBanme(void*, u16, s32, void*);
    extern u16 pokemonGetStatus(u32, s32, s32, u16);
    extern u32 wazaGetStatus();
    extern u32 GSmsgGetGSchar();
    extern s32 fightOutPokemonGetSoubiItemDataId(void*);
    u16 stackValue;
    u8* obj;
    u32 battle;
    u32 state;
    u32 result;
    u32 msg;
    u32 item;

    obj = *(u8**)(ctx + 0x40);
    battle = fightOutPokemonGetPokemonPtr(obj);
    state = fightOutPokemonCheckCanOutOkWazaBanme(obj, arg, 1, &stackValue);
    result = pokemonGetStatus(battle, 0, 0x7F, arg);
    if ((u8)state != 0) {
        msgctrlSetValue(0x11, (s32)obj);
        msg = wazaGetStatus(0, result, 1, 0);
        msgctrlSetValue(0x28, GSmsgGetGSchar(msg));
        item = fightOutPokemonGetSoubiItemDataId(obj);
        fightFloorSetStatus(0, 0, 0x56, 0, (u16)item);
    }
    if ((u8)state == 6) {
        msg = 0x7661;
    } else if ((u8)state == 5) {
        msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, stackValue, 1, 0)));
        msg = 0x76BB;
    } else if ((u8)state == 4) {
        msg = 0x7600;
    } else if ((u8)state == 3) {
        msg = 0x75FF;
    } else if ((u8)state == 2) {
        msg = 0x75FE;
    } else if ((u8)state == 1) {
        msg = 0x75FD;
    }
    if ((u8)state != 0) {
        return msg;
    }
    return 0;
}

