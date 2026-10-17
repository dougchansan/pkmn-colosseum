/**
 * @file fight_menu.c
 * @brief game/pxdvs/app/fight/fightMenu.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x80261B68-0x802658C8, 33 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightMenu.cpp
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void battleCameraSelect();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern void fadeSet(f32 duration, u32 mode);
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(u32 trainer, u32 side, u32 canCancel);

/* FIGHT_MENU_ONLY_8026316C (OpenMenu) and FIGHT_MENU_ONLY_80263DE4 (SelectIrekae +
 * SubItem) build one linked .text range each from the candidate block. */
#if defined(FIGHT_MENU_ONLY_8026316C) || defined(FIGHT_MENU_ONLY_80263DE4)
#define FIGHT_MENU_GCHERO_SPLIT
#endif
#if defined(FIGHT_MENU_CANDIDATE_80261B68) && !defined(FIGHT_MENU_ONLY_80264D58)

/* Inline copies of fightMenuCloseInfoMenu (0x80261954) and
 * fightMenuOpenInfoMenu (0x80261AD0), which live in fight_gsfloor.c;
 * retail expands both inside the GC hero menus below. */
/* RULE-EXCEPTION(user-approved): inline copy of the real fightMenuCloseInfoMenu (fight_gsfloor.c) — see docs/RULE_EXCEPTIONS.md */
static inline void fightMenuCloseInfoMenuInline(u32 wait)
{
    extern void fightFloorLoopValidFightTrainer();
    extern u32 fightFloorLoopValidFightOutPokemon();
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightFloorIsUseFightTimerAll(u32);
    extern void menuFightCloseCountDown(void);
    extern void menuFightCloseTotalTimer(void);
    extern u8 menuFightCloseCheckCountDown(void);
    extern u8 menuFightCloseCheckTotalTimer(void);
    extern void _threadSwitch(void);
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv();
    u8 done;

    fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv, 0, 0);
    fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv, 0, 0);
    if (fightFloorIsUseFightTimerCommand(0) == 1) {
        menuFightCloseCountDown();
    }
    if (fightFloorIsUseFightTimerAll(0) == 1) {
        menuFightCloseTotalTimer();
    }
    if ((u8)wait == 1) {
        fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv, 0, 0);
        do {
            done = 1;
            fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv, &done, 0);
            if (done == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
        fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv, 0, 0);
        do {
            if ((u8)fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv, 0, 0) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
        if (fightFloorIsUseFightTimerCommand(0) == 1) {
            do {
                if (menuFightCloseCheckCountDown() == 0) {
                    break;
                }
                _threadSwitch();
            } while (1);
        }
        if (fightFloorIsUseFightTimerAll(0) == 1) {
            do {
                if (menuFightCloseCheckTotalTimer() == 0) {
                    break;
                }
                _threadSwitch();
            } while (1);
        }
    }
}

/* RULE-EXCEPTION(user-approved): inline copy of the real fightMenuOpenInfoMenu (fight_gsfloor.c) — see docs/RULE_EXCEPTIONS.md */
static inline void fightMenuOpenInfoMenuInline(s8 timerMode)
{
    extern void fightFloorLoopValidFightTrainer();
    extern u32 fightFloorLoopValidFightOutPokemon();
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightFloorIsUseFightTimerAll(u32);
    extern void menuFightOpenCountDown(void);
    extern void menuFightOpenTotalTimer(void);
    extern u32 _fightMenuAllFightTrainerOpenStatusMenuSub__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonOpenStatusMenuSub__FPvUsPv();
    u8 openStatus;

    fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerOpenStatusMenuSub__FPvUsPv, 0, 0);
    openStatus = 1;
    fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonOpenStatusMenuSub__FPvUsPv, &openStatus, 0);
    if (timerMode < 0) {
        if (fightFloorIsUseFightTimerCommand(0) == 1) {
            menuFightOpenCountDown();
        }
    }
    if (fightFloorIsUseFightTimerAll(0) == 1) {
        menuFightOpenTotalTimer();
    }
}

static inline u32 fightMenuGcHeroStatusMenuId(u32 trainer, u32 slot, u32 kind)
{
    extern u32 fightTargetGetPtr(u32, u32, u32);
    extern u32 fightSideGetStatus(u32, u32, u32, u32);
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightTargetDataBiosGetPtr(void);
    extern s32 fightTargetDataBiosGetBuff(void);
    u32 target;
    u16 side;
    s32 buff;
    u32 result;

    target = fightTargetGetPtr(2, trainer, slot);
    if (target == 0) {
        return 0;
    }
    side = fightSideGetStatus(target, 0, 5, 0);
    if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(trainer, slot) == 0) {
        return 0;
    }
    fightTargetDataBiosGetPtr();
    buff = fightTargetDataBiosGetBuff();
    if (buff < 0) {
        return 0;
    }
    result = fightSideGetStatus(0, side, kind, buff & 0xffff);
    return result;
}

/* RULE-EXCEPTION(user-approved): second copy of fightMenuGcHeroStatusMenuId that returns the status directly, used only by SubItem (the stored-result form gives OpenMenu/SelectIrekae their registers) — see docs/RULE_EXCEPTIONS.md */
static inline u32 fightMenuGcHeroStatusMenuIdDirect(u32 trainer, u32 slot, u32 kind)
{
    extern u32 fightTargetGetPtr(u32, u32, u32);
    extern u32 fightSideGetStatus(u32, u32, u32, u32);
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightTargetDataBiosGetPtr(void);
    extern s32 fightTargetDataBiosGetBuff(void);
    u32 target;
    u16 side;
    s32 buff;

    target = fightTargetGetPtr(2, trainer, slot);
    if (target == 0) {
        return 0;
    }
    side = fightSideGetStatus(target, 0, 5, 0);
    if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(trainer, slot) == 0) {
        return 0;
    }
    fightTargetDataBiosGetPtr();
    buff = fightTargetDataBiosGetBuff();
    if (buff < 0) {
        return 0;
    }
    return fightSideGetStatus(0, side, kind, buff & 0xffff);
}

static inline u32 fightMenuGcHeroMsgMenuId(u32 trainer, u32 slot)
{
    u32 msg = 0x100;

    switch (fightMenuGcHeroStatusMenuId(trainer, slot, 2)) {
    case 0xf1:
        msg = 0x100;
        break;
    case 0xf2:
        msg = 0x101;
        break;
    case 0xf3:
        msg = 0x102;
        break;
    case 0xf4:
        msg = 0x103;
        break;
    }
    return msg;
}


#if !defined(FIGHT_MENU_ONLY_80261B68)
#if !defined(FIGHT_MENU_ONLY_80263DE4)
/* Address: 0x8026316C | Size: 0xA5C */
u32 fightMenuFightTrainerGcHeroOpenMenu(u32 trainer, u32 side, u32 canCancel)
{
    extern void fightTypeDataBiosGetPtr(u32);
    extern u8 fightTypeDataBiosGetFightoutPokemonNum(void);
    extern u8 fn_801F18DC(u32);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u32 fightTrainerGetValidFightOutPokemonPtr(u32, u16);
    extern u8 fightOutPokemonCheckFightActionSelect(u32, u32);
    extern u16 std103_battleGetResult(void);
    extern void fightTrainerAllInitFightActionBuff(u32);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern void fightOutPokemonInitFightActionBuff(u32);
    extern void fightTrainerTimeOutSelectFightAction(u32, u32, u32);
    extern void menuFightStatusSetActive(u32, u32);
    extern void fightFloorSetMenuFightAction(u32, u32, u8*);
    extern s32 fn_80011A1C(u8*, s32*, u32);
    extern void menuFightCloseTop(u32);
    extern u32 _fightMenuFightTrainerGcHeroOpenMenuSubMain__FP13FIGHT_TRAINERP15FightOutPokemonUsl(u32, u32, u32, s32);
    extern u8 fightTrainerIsUsedItem(u32);
    extern void pokemonSetStatus(u32, u32, u32, u32, u32);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    s32 cursor;
    u8 action[0x18];
    u32 num;
    u16 i;
    u16 last;
    u32 pokemon;
    s32 result;

    fightTypeDataBiosGetPtr(side);
    num = fightTypeDataBiosGetFightoutPokemonNum();
    if (fn_801F18DC(0) != 0) {
        menuOpenCustom(fightMenuGcHeroMsgMenuId(trainer, side), 0, 0, 0, 0, 0);
    }
    cursor = 0;
    i = 0;
    last = 0;
    for (; i < num; i++) {
    select_pokemon:
        pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, i);
        if (pokemon == 0) {
            last = i;
            continue;
        }
        if (fightOutPokemonCheckFightActionSelect(pokemon, 1) == 0) {
            last = i;
            continue;
        }
        if (std103_battleGetResult() == 1) {
        cancel:
            fightTrainerAllInitFightActionBuff(trainer);
            break;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1) {
        timeout:
            fightOutPokemonInitFightActionBuff(pokemon);
            fightTrainerTimeOutSelectFightAction(trainer, pokemon, side);
            last = i;
            continue;
        }
        if (fn_801F18DC(0) != 1) {
            menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 1);
        }
        cursor = 0;
    select_action:
        fightOutPokemonInitFightActionBuff(pokemon);
        fightFloorSetMenuFightAction(0, pokemon, action);
        action[0x17] = fn_801F18DC(0);
        result = fn_80011A1C(action, &cursor, 1);
        if (std103_battleGetResult() == 1) {
            if (fn_801F18DC(0) != 1) {
                menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
            }
            menuFightCloseTop(1);
            goto cancel;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1 && result < 0) {
            if (fn_801F18DC(0) != 1) {
                menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
            }
            menuFightCloseTop(1);
            goto timeout;
        }
        if (result >= 0) {
            menuFightCloseTop(1);
        }
        result = _fightMenuFightTrainerGcHeroOpenMenuSubMain__FP13FIGHT_TRAINERP15FightOutPokemonUsl(
            trainer, pokemon, side, result);
        if (std103_battleGetResult() == 1) {
            if (fn_801F18DC(0) != 1) {
                menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
            }
            menuFightCloseTop(1);
            goto cancel;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1 && (u8)result != 0) {
            if (fn_801F18DC(0) != 1) {
                menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
            }
            menuFightCloseTop(1);
            goto timeout;
        }
        if ((u8)result == 1) {
            if (fightTrainerIsUsedItem(trainer) == 1) {
                goto select_action;
            }
            if (i != 0) {
                i = last;
                if (fn_801F18DC(0) != 1) {
                    menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
                }
                menuFightCloseTop(1);
                goto select_pokemon;
            }
            if ((u8)canCancel != 1 || i != 0) {
                goto select_action;
            }
            if (fn_801F18DC(0) != 1) {
                menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
            }
            menuFightCloseTop(1);
            for (i = 0; i < num; i++) {
                pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, i);
                if (pokemon != 0) {
                    pokemonSetStatus(pokemon, 0, 0x120, 0, 0);
                }
            }
            return 0;
        }
        if ((u8)result == 2) {
            goto select_action;
        }
        if (fn_801F18DC(0) != 1) {
            menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(pokemon, side, 3), 0);
        }
        last = i;
    }
    if (fn_801F18DC(0) != 0) {
        if (menuIsCheck(fightMenuGcHeroMsgMenuId(trainer, side)) != 0) {
            menuCloseCustom(fightMenuGcHeroMsgMenuId(trainer, side), 0, 1);
        }
    }
    for (i = 0; i < num; i++) {
        pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, i);
        if (pokemon != 0) {
            pokemonSetStatus(pokemon, 0, 0x120, 0, 0);
        }
    }
    return 1;
}

#endif /* !FIGHT_MENU_ONLY_80263DE4 */
#if !defined(FIGHT_MENU_GCHERO_SPLIT)
/* Address: 0x80263BC8 | Size: 0x21C | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubMain__FP13FIGHT_TRAINERP15FightOutPokemonUsl(u32 r3, u32 r4, u32 r5, s32 mode)
{
    extern u8 fightOutPokemonCheckFightActionWazaSelect(u32, u32);
    extern u8 fightFloorGetStatus(u32, u32, u32, u32);
    extern void msgctrlSetValue(u32, u32);
    extern void winMsgOpenFight(u32, u32, u32);
    extern void fightMainWaitFrame(u32);
    extern void winMsgCloseFight(u32);
    extern u8 _fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32, u32, u32);
    extern u8 _fightMenuFightTrainerGcHeroOpenMenuSubItem__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32, u32, u32);
    extern s32 fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(u32, u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, void *, s32);
    extern char lbl_80375D30[];
    u8 flag;
    u32 result;

    result = 0;
    switch (mode) {
    case 0:
        flag = fightOutPokemonCheckFightActionWazaSelect(r4, 1);
        if (flag == 1) {
            msgctrlSetValue(0x11, r4);
            winMsgOpenFight(0x75fc, 1, 1);
            fightMainWaitFrame(0x40);
            winMsgCloseFight(0);
        }
        if (flag == 0) {
            if (_fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs(r3, r4, r5) == 0) {
                result = 2;
            }
        }
        break;
    case 1:
        if (fightFloorGetStatus(0, 0, 0x20, 0) == 0) {
            winMsgOpenFight(0x75f5, 1, 1);
            fightMainWaitFrame(0x40);
            winMsgCloseFight(0);
            result = 2;
        }
        else if (_fightMenuFightTrainerGcHeroOpenMenuSubItem__FP13FIGHT_TRAINERP15FightOutPokemonUs(r3, r4, r5) == 0) {
            result = 2;
        }
        break;
    case 2:
        r5 = fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(r3, r4, r5, 1, 1);
        if ((s16)r5 < 0) {
            flag = 0;
        }
        else {
            fightOutPokemonCreateFightAction(r4, 0, 9, 0, lbl_80375D30, (s16)(r5 & 0xFFFFFFFFFFFFFFFFu));
            flag = 1;
        }
        if (flag == 0) {
            result = 2;
        }
        break;
    case 3:
        if (fightFloorGetStatus(0, 0, 0x22, 0) == 1) {
            fightOutPokemonCreateFightAction(r4, 0, 8, 0, lbl_80375D30, 0);
        }
        else if (fightFloorGetStatus(0, 0, 0x21, 0) == 1) {
            fightOutPokemonCreateFightAction(r4, 0, 0xa, 0, lbl_80375D30, 0);
        }
        else {
            result = 2;
        }
        break;
    default:
        result = 1;
        break;
    }
    return result;
}

#endif /* !FIGHT_MENU_GCHERO_SPLIT */
#if !defined(FIGHT_MENU_ONLY_8026316C)
/* Address: 0x80263DE4 | Size: 0x6A4 */
s32 fightMenuFightTrainerGcHeroSelectIrekaeFightPokemon(u32 ctx, u32 actor, u32 param, u32 retry, u32 reopen)
{
    extern u16 std103_battleGetResult(void);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern u8 fn_801F18DC(u32);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    extern s32 menuFightOpenPokemon(u32, u32, u32, u32, u8);
    extern void fightWaitFrames(u32);
    extern void menuFightStatusSetActive(u32, u32);
    extern s32 fightTrainerTimeOutSelectIrekaeFightPokemon(u32, u32, u32);
    extern u32 fightTrainerGetStatus(u32, u32, u32, u16);
    extern u8 fightPokemonCheckValid(u32);
    extern s16 pokemonGetStatus(u32, u32, u32, u32);
    u32 item;
    s32 choice;

    for (;;) {
        if (std103_battleGetResult() == 1) {
        cancel:
            return -2;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1) {
            goto timeout;
        }
        if (fn_801F18DC(0) != 0 && (u8)reopen == 0) {
            menuOpenCustom(fightMenuGcHeroMsgMenuId(ctx, param), 0, 0, 0, 0, 0);
        }
        if (fn_801F18DC(0) == 0) {
            fightMenuCloseInfoMenuInline(1);
        }
        choice = menuFightOpenPokemon(ctx, actor, param, retry, fn_801F18DC(0));
        fightWaitFrames(0xa);
        if (fn_801F18DC(0) == 0 && (u8)reopen == 1) {
            fightMenuOpenInfoMenuInline(-1);
            if (actor != 0 && fn_801F18DC(0) != 1) {
                menuFightStatusSetActive(fightMenuGcHeroStatusMenuId(actor, param, 3), 1);
            }
        }
        if (fn_801F18DC(0) != 0 && (u8)reopen == 0) {
            if (menuIsCheck(fightMenuGcHeroMsgMenuId(ctx, param)) != 0) {
                menuCloseCustom(fightMenuGcHeroMsgMenuId(ctx, param), 0, 1);
            }
        }
        if (std103_battleGetResult() == 1) {
            goto cancel;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1 && choice < 0) {
        timeout:
            if ((u8)reopen == 0) {
                return fightTrainerTimeOutSelectIrekaeFightPokemon(ctx, actor, param);
            }
            goto failed;
        }
        if (choice < 0) {
        failed:
            if ((u8)retry == 0) {
                continue;
            }
            return -1;
        }
        item = fightTrainerGetStatus(ctx, 0, 0x45, choice);
        if (fightPokemonCheckValid(item) == 0) {
            continue;
        }
        if ((u8)pokemonGetStatus(item, 0, 0xd2, 0) == 1) {
            continue;
        }
        return pokemonGetStatus(item, 0, 0xce, 0);
    }
}
#endif /* !FIGHT_MENU_ONLY_8026316C */


#endif
#if !defined(FIGHT_MENU_GCHERO_SPLIT)

/* Address: 0x80261B68 | Size: 0x84 | Ghidra import */
void fightMenuAllFightTrainerCloseStatusMenu(u32 wait)
{
    extern void fightFloorLoopValidFightTrainer();
    extern void _threadSwitch(void);
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv();
    u32 r30;
    u8 done;

    fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv, 0, 0);
    if ((u8)wait == 1) {
        r30 = 1;
        do {
            done = r30;
            fightFloorLoopValidFightTrainer(0, _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv, &done, 0);
            if (done == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
}

/*
 * The status-menu ID of a trainer / fight-out Pokemon: its side's
 * status-menu table entry for the target's buffer slot (kind 2 for
 * trainers, 3 for Pokemon), or 0 when the target, its host-side relative
 * ID or its buffer is missing. Retail expands these in every status-menu
 * helper of the unit (the three trainer subs and fightMenuFightTrainer-
 * RenewStatusMenu (twice) for kind 2; the three Pokemon subs, fightMenuStatusMenuClose
 * and fn_80265598 for kind 3), each copy the same call sequence with the
 * same constants. Pokemon XD keeps both as out-of-line functions with this
 * body: fightMenuGetFightTrainerPtrToStatusMenuId (0x8023943C) and
 * fightMenuGetFightOutPokemonPtrToStatusMenuId (0x8023926C, 0xA8 bytes;
 * TeamOrre/xd-decomp symbols.txt, trevor403/xd-asm b1087f1). Colosseum's
 * own out-of-line copy of the Pokemon one is at 0x802656AC
 * (FIGHT_MENU_EXACT_8026503C section below). Admitted as a repeated
 * expansion (docs/CAMPAIGN_OPERATIONS.md, "Reconstructed inline helpers").
 *
 * Still a candidate (lane U5, 2026-09-28): with the helpers the six subs
 * below are exact only when the callback's void* argument is first taken
 * into a typed local (FIGHT_TRAINER* trainer = work;). That local changes
 * no instruction, only the colouring: retail colours the object pointer
 * before the other arguments (trainer r30, done r29, slot r28 in the
 * CloseCheck), which MWCC does only for a separate value; passing `work`
 * straight to the helper colours the arguments in reverse (r28/r29/r30).
 * Tried without it: u32/void* / typed-pointer helper parameters, casts at
 * the call, early-return vs single-exit helper bodies, and an intermediate
 * inline (XD's fightMenuFightTrainerCloseStatusMenu shape). Retail is C++,
 * where the void* -> FIGHT_TRAINER* conversion needs a cast; the typed
 * local may model that, but it is a pure copy of a parameter whose only
 * effect is registers: rejected under the temporaries rule, and not a
 * "named computed value" (2026-09-28 rule), so these subs are not linked.
 */
typedef struct FIGHT_TRAINER FIGHT_TRAINER;
typedef struct FightOutPokemon FightOutPokemon;

static inline u32 fightMenuGetFightTrainerPtrToStatusMenuId(FIGHT_TRAINER* trainer, u32 slot)
{
    u32 target;
    u16 side;
    u32 buff;

    target = fightTargetGetPtr(2, trainer, slot);
    if (target == 0) {
        return 0;
    }
    side = fightSideGetStatus(target, 0, 5, 0);
    if ((u16)fightTargetGetTragetPtrToRelativeHostSideFightTargetId(trainer, slot) == 0) {
        return 0;
    }
    fightTargetDataBiosGetPtr();
    buff = fightTargetDataBiosGetBuff();
    if ((int)buff < 0) {
        return 0;
    }
    return fightSideGetStatus(0, side, 2, buff & 0xffff);
}

static inline u32 fightMenuGetFightOutPokemonPtrToStatusMenuId(FightOutPokemon* pokemon, u32 slot)
{
    u32 target;
    u16 side;
    u32 buff;

    target = fightTargetGetPtr(2, pokemon, slot);
    if (target == 0) {
        return 0;
    }
    side = fightSideGetStatus(target, 0, 5, 0);
    if ((u16)fightTargetGetTragetPtrToRelativeHostSideFightTargetId(pokemon, slot) == 0) {
        return 0;
    }
    fightTargetDataBiosGetPtr();
    buff = fightTargetDataBiosGetBuff();
    if ((int)buff < 0) {
        return 0;
    }
    return fightSideGetStatus(0, side, 3, buff & 0xffff);
}

/* The ball-status record fightTrainerToMenuBallStatus fills in and the
 * Pokemon status record fightOutPokemonToMenuPokemonStatus fills in; the
 * status menus take a copy of it. */
typedef struct MenuBallStatus {
    u16 word[3];
} MenuBallStatus;

typedef struct MenuPokemonStatus {
    /* 0x00 */ u32 word[10];
    /* 0x28 */ u8 unk28;
    /* 0x29 */ u8 active;
    /* 0x2A */ u8 pad2A[6];
} MenuPokemonStatus;

/* Address: 0x80261BEC | Size: 0xD0 */
u32 _fightMenuAllFightTrainerCloseStatusMenuSubCloseCheck__FPvUsPv(void* work, u32 slot, u8* done)
{
    FIGHT_TRAINER* trainer = work;
    if ((u8)menuIsCheck(fightMenuGetFightTrainerPtrToStatusMenuId(trainer, slot)) == 1 && done != NULL) {
        *done = 0;
    }
    return 1;
}

/* Address: 0x80261CBC | Size: 0xD0 */
u32 _fightMenuAllFightTrainerCloseStatusMenuSub__FPvUsPv(void* work, u32 slot, void* userData)
{
    FIGHT_TRAINER* trainer = work;
    u32 menuId;

    menuId = fightMenuGetFightTrainerPtrToStatusMenuId(trainer, slot);
    if ((u8)menuIsCheck(menuId) != 0) {
        menuCloseCustom(menuId, 0, 0);
    }
    return 1;
}

/* Address: 0x80261D8C | Size: 0xF0 */
u32 _fightMenuAllFightTrainerOpenStatusMenuSub__FPvUsPv(void* work, u32 slot)
{
    FIGHT_TRAINER* trainer = work;
    extern int fightTrainerToMenuBallStatus();
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    u32 menuId;
    MenuBallStatus copy;
    MenuBallStatus status;

    menuId = fightMenuGetFightTrainerPtrToStatusMenuId(trainer, slot);
    fightTrainerToMenuBallStatus(trainer, &status);
    copy = status;
    menuOpenCustom(menuId, 0, 0, 0, 0, 1, &copy);
    return 1;
}

/* Address: 0x80261E7C | Size: 0x7C | Ghidra import */
void fightMenuAllFightOutPokemonCloseStatusMenu(u32 wait)
{
    extern u32 fightFloorLoopValidFightOutPokemon();
    extern void _threadSwitch(void);
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv();
    extern u32 _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv();
    fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv, 0, 0);
    if ((u8)wait == 1) {
        do {
            if ((u8)fightFloorLoopValidFightOutPokemon(0, _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv, 0, 0) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
}

/* Address: 0x80261EF8 | Size: 0xBC */
int _fightMenuAllFightOutPokemonCloseStatusMenuSubCloseCheck__FPvUsPv(void* work, u32 slot)
{
    FightOutPokemon* pokemon = work;
    if ((u8)menuIsCheck(fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon, slot)) == 1) {
        return 0;
    }
    return 1;
}

/* Address: 0x80261FB4 | Size: 0xD0 */
u32 _fightMenuAllFightOutPokemonCloseStatusMenuSub__FPvUsPv(void* work, u32 slot)
{
    FightOutPokemon* pokemon = work;
    u32 menuId;

    menuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon, slot);
    if ((u8)menuIsCheck(menuId) != 0) {
        menuCloseCustom(menuId, 0, 0);
    }
    return 1;
}

/* Address: 0x80262084 | Size: 0x140 */
u32 _fightMenuAllFightOutPokemonOpenStatusMenuSub__FPvUsPv(void* work, u32 slot, u8* active)
{
    FightOutPokemon* pokemon = work;
    extern int fightOutPokemonToMenuPokemonStatus();
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    u8 isActive;
    u32 menuId;
    MenuPokemonStatus copy;
    MenuPokemonStatus status;

    if (active == NULL) {
        isActive = 1;
    } else {
        isActive = *active;
    }
    if ((u8)fightOutPokemonCheckFightOut(pokemon) == 0) {
        return 1;
    }
    menuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(pokemon, slot);
    fightOutPokemonToMenuPokemonStatus(pokemon, &status);
    if (isActive == 0) {
        status.active = 0;
    }
    copy = status;
    menuOpenCustom(menuId, -1, 0, 0, 0, 1, &copy);
    return 1;
}

/* Address: 0x802621C4 | Size: 0x30 | Ghidra import */
void fightMenuOpenLevelUpStatusMenu(u8 *dst, u8 value)
{
    extern void winMsgOpenLevelUpStatus(u8 *, u32);

    if (dst != NULL) {
        *dst = value;
        winMsgOpenLevelUpStatus(dst, 1);
    }
}

/* Address: 0x802621F4 | Size: 0x7C | Ghidra import */
void fightMenuSubMenuLvupStatus(s16 *current, s16 *previous, s16 *out)
{
    if (current == NULL) {
        return;
    }
    if (previous == NULL) {
        return;
    }
    if (out == NULL) {
        return;
    }
    out[1] = current[1] - previous[1];
    out[2] = current[2] - previous[2];
    out[3] = current[3] - previous[3];
    out[5] = current[5] - previous[5];
    out[6] = current[6] - previous[6];
    out[4] = current[4] - previous[4];
}

/* Address: 0x80262270 | Size: 0x74 | Ghidra import */
/* RULE-EXCEPTION(user-approved): extern-named stand-in for the 0.5f in
   colosseum_battle_sdata2 (0x8047E6C8) — see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047E6C8;

s32 fightMenuWazaWasure(u32 r3, u32 r4)
{
    extern s32 menuPokemonStatusOpenWazaForget(u32, u32);
    s32 result;

    fadeSet(lbl_8047E6C8, 3);
    fadeCheck(1);
    result = menuPokemonStatusOpenWazaForget(r3, r4);
    if (result == 4) {
        result = -1;
    }
    fadeSet(lbl_8047E6C8, 2);
    fadeCheck(1);
    return result;
}

/* Address: 0x802622E4 | Size: 0x24 | Ghidra import */
void fightMenuCloseLevelUpStatusMenu(void)
{
    extern void winMsgCloseLevelUpStatus(u32);

    winMsgCloseLevelUpStatus(1);
}

/* Address: 0x80262308 | Size: 0x2C | Ghidra import */
u32 fightMenuYesNo(void)
{
    extern s8 std011_dialogDisplayYesNoMenu(void);

    return (__cntlzw((s8)std011_dialogDisplayYesNoMenu()) >> 5) & 0xff;
}

/* Address: 0x80262334 | Size: 0x80 | Ghidra import */
u32 fightMenuWazaKoukaMsg(u32 msgId, u32 unused, u32 itemId)
{
    extern u32 itemGetStatus(u32, u32, u32, u32);
    extern u32 GSmsgGetGSchar(u32);
    u32 itemName;

    msgctrlSetValue(0x10);
    itemName = GSmsgGetGSchar(itemGetStatus(0, itemId, 1, 0));
    msgctrlSetValue(0x29, itemName);
    if (msgId != 0) {
        winMsgOpenFight(msgId, 1, 1);
        return 1;
    }
    return 0;
}

/* Address: 0x802623B4 | Size: 0xB8 | Ghidra import */
u32 fightMenuWazaOutMsg(u32 msgId, u32 pokemon)
{
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 GSmsgGetGSchar(u32);
    u32 name;

    msgctrlSetValue(0xf, msgId);
    name = GSmsgGetGSchar(wazaGetStatus(0, pokemon, 0xa, 0));
    msgctrlSetValue(0xd, name);
    msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, pokemon, 1, 0)));
    msgctrlSetValue(0xe, GSmsgGetGSchar(wazaGetStatus(0, pokemon, 0xb, 0)));
    winMsgOpenFight(0x768d, 1, 1);
    return 1;
}

/* Address: 0x8026246C | Size: 0x24 | Ghidra import */
void fightMenuCloseMsg(void)
{
    winMsgCloseFight(0);
}

/* Address: 0x80262490 | Size: 0x3C | Ghidra import */
u32 fightMenuOpenTrainerMsg(u32 msgId)
{
    if (msgId != 0) {
        winMsgOpenFightNoWait(msgId, 1, 1);
        return 1;
    }
    return 0;
}

/* Address: 0x802624CC | Size: 0x3C | Ghidra import */
u32 fightMenuOpenMsg(u32 msgId)
{
    if (msgId != 0) {
        winMsgOpenFight(msgId, 1, 1);
        return 1;
    }
    return 0;
}

/* Address: 0x80262508 | Size: 0x82C */
u32 fightMenuFightTrainerAgbHeroOpenMenu(u32 trainer, u32 side)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u16);
    extern u8 fn_801F18DC(u32);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern void fightTypeDataBiosGetPtr(u32);
    extern u8 fightTypeDataBiosGetFightoutPokemonNum(void);
    extern u32 fightTrainerGetValidFightOutPokemonPtr(u32, u32);
    extern u8 fightOutPokemonCheckFightActionSelect(u32, u32);
    extern void fightOutPokemonInitFightActionBuff(u32);
    extern u16 std103_battleGetResult(void);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern u32 menuFightOpenGBAMain(u32, u32, u16, u32);
    extern s32 fn_80089F70(u32);
    extern void fightOutPokemonCreateFightAction(u32, u32, u32, u32, char*, s32);
    extern u8 fightOutPokemonCheckFightActionWazaSelect(u32, u32);
    extern u32 fn_80089F68(u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightSeqSelectTarget(u32, u16, u32, void*, u32, u32, s32);
    extern u32 _fightMenuFightTrainerAgbHeroSelectDefensePokemon__FP15FightOutPokemonUsUs();
    extern u32 fn_80089F60(u32);
    extern u32 fightFloorGetValidFightSidePtr(u32, u16);
    extern u32 fightSideGetValidFightTrainerPtr(u32, u16);
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightOutPokemonCreateFightActionAttackWaza(u32, u32, u32, u32, char*, u16, u32, s8, u32);
    extern u32 fn_80089F58(u32);
    extern void fightTrainerTimeOutSelectFightAction(u32, u32, u32);
    extern void fightTrainerAllInitFightActionBuff(u32);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    extern char lbl_80375D30[];
    extern char lbl_80375CA8[];
    extern u32* lbl_80478DF8;
    u32 pokemon;
    u32 target;
    u32 i;
    u32 num;
    u16 pokemonNo;
    u16 targetIndex;
    u16 waza;
    u32 last;
    u16 sideNo;
    u16 trainerNo;
    u16 count;
    u16 slot;
    s8 wazaSlot;
    s32 kind;
    u32 fightTrainer;
    u16 trainerNum;
    u32 menu;
    s16 index;
    u32 fightSide;
    u8 found;
    u32 battle;
    u32 defense;

    battle = fightTrainerGetStatus(trainer, 0, 0x4b, 0);
    if (fn_801F18DC(0) != 0) {
        menuOpenCustom(fightMenuGcHeroMsgMenuId(trainer, side), 0, 0, 0, 0, 0);
    }
    trainerNum = fightFloorGetStatus(0, 0, 0x16, 0);
    fightTypeDataBiosGetPtr(side);
    num = fightTypeDataBiosGetFightoutPokemonNum();
    last = 0;
    for (i = 0; (u16)i < num; i++) {
    select_pokemon:
        pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, i);
        if (pokemon == 0) {
            last = i;
            continue;
        }
        if (fightOutPokemonCheckFightActionSelect(pokemon, 1) == 0) {
            last = i;
            continue;
        }
        pokemonNo = i;
    select_action:
        fightOutPokemonInitFightActionBuff(pokemon);
        if (std103_battleGetResult() == 1) {
            goto cancel;
        }
        if (fightFloorIsUseFightTimerCommand(0) == 1 && fightTimerCommandIsOver() == 1) {
            goto timeout;
        }
        menu = menuFightOpenGBAMain(battle, trainer, pokemonNo, side);
        kind = fn_80089F70(menu);
        if (std103_battleGetResult() == 1) {
            goto cancel;
        }
        if (kind == 3) {
            fightOutPokemonCreateFightAction(pokemon, 0, 8, 0, lbl_80375D30, 0);
        } else if (kind == 1) {
            if (fightOutPokemonCheckFightActionWazaSelect(pokemon, 1) != 0) {
                last = i;
                continue;
            }
            wazaSlot = fn_80089F68(menu);
            if (wazaSlot < 0) {
                goto select_action;
            }
            waza = pokemonGetStatus(fightOutPokemonGetPokemonPtr(pokemon), 0, 0x7f, wazaSlot);
            if (waza == 0 || waza >= *lbl_80478DF8 || waza == 0x165) {
                goto select_action;
            }
            target = 0;
            defense = fightSeqSelectTarget(pokemon, waza, side,
                _fightMenuFightTrainerAgbHeroSelectDefensePokemon__FP15FightOutPokemonUsUs, 1, 0, -1);
            if (defense != 0) {
                target = defense;
            } else {
                targetIndex = fn_80089F60(menu);
                count = 0;
                found = 0;
                for (sideNo = 0; sideNo < 2; sideNo++) {
                    fightSide = fightFloorGetValidFightSidePtr(0, sideNo);
                    if (fightSide == 0) {
                        continue;
                    }
                    for (trainerNo = 0; trainerNo < trainerNum; trainerNo++) {
                        fightTrainer = fightSideGetValidFightTrainerPtr(fightSide, trainerNo);
                        if (fightTrainer == 0) {
                            continue;
                        }
                        for (slot = 0; slot < num; slot++) {
                            target = fightTrainerGetStatus(fightTrainer, 0, 0x46, slot);
                            if (count == targetIndex) {
                                found = 1;
                                break;
                            }
                            count++;
                        }
                        if (found == 1) {
                            break;
                        }
                    }
                    if (found == 1) {
                        break;
                    }
                }
            }
            if (target == 0) {
                goto select_action;
            }
            fightOutPokemonCreateFightActionAttackWaza(pokemon, 0, 0x13, 0, lbl_80375CA8, waza,
                fightTargetGetTragetPtrToRelativeHostSideFightTargetId(target, side), wazaSlot, 0);
        } else if (kind == 2) {
            index = pokemonGetStatus(fightTrainerGetStatus(trainer, 0, 0x45, fn_80089F58(menu)), 0, 0xce, 0);
            if (index < 0) {
                goto select_action;
            }
            fightOutPokemonCreateFightAction(pokemon, 0, 9, 0, lbl_80375D30, index);
        } else if (kind == 4) {
        timeout:
            fightTrainerTimeOutSelectFightAction(trainer, pokemon, side);
        } else if (kind == 5) {
        cancel:
            fightTrainerAllInitFightActionBuff(trainer);
            if (fn_801F18DC(0) != 0) {
                if (menuIsCheck(fightMenuGcHeroMsgMenuId(trainer, side)) != 0) {
                    menuCloseCustom(fightMenuGcHeroMsgMenuId(trainer, side), 0, 1);
                }
            }
            return 0;
        } else if (kind != 0) {
            goto timeout;
        } else {
            if ((u16)i != 0) {
                i = last;
            }
            goto select_pokemon;
        }
        last = i;
    }
    if (fn_801F18DC(0) != 0) {
        if (menuIsCheck(fightMenuGcHeroMsgMenuId(trainer, side)) != 0) {
            menuCloseCustom(fightMenuGcHeroMsgMenuId(trainer, side), 0, 1);
        }
    }
    return 1;
}

/* Address: 0x80262D34 | Size: 0x8 | Pattern: return_constant */
u32 _fightMenuFightTrainerAgbHeroSelectDefensePokemon__FP15FightOutPokemonUsUs(void) { return 0; }

/* Address: 0x80262D3C | Size: 0x430 | Ghidra import */
s32 fightMenuFightTrainerAgbHeroSelectIrekaeFightPokemon(u32 ctx, u32 param1, u32 param2, u32 target)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern void fightTypeDataBiosGetPtr(u32);
    extern u32 fightTypeDataBiosGetFightoutPokemonNum(void);
    extern u16 std103_battleGetResult(void);
    extern u8 fightFloorIsUseFightTimerCommand(u32);
    extern u8 fightTimerCommandIsOver(void);
    extern u8 fn_801F18DC(u32);
    extern u32 fightTargetGetPtr(u32, u32, u32);
    extern u32 fightSideGetStatus(u32, u32, u32, u32);
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void fightTargetDataBiosGetPtr(void);
    extern s32 fightTargetDataBiosGetBuff(void);
    extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
    extern u32 menuFightOpenGBAIrekae(u32, u32, u16, u32);
    extern s32 fn_80089F70(u32);
    extern u16 fn_80089F58(u32);
    extern s16 pokemonGetStatus(u32, u32, u32, u32);
    extern s32 fightTrainerTimeOutSelectIrekaeFightPokemon(u32, s32, u32);
    extern u8 menuIsCheck(u32);
    extern void menuCloseCustom(u32, u32, u32);
    u16 side;
    u32 msg;
    u32 battle;
    u32 count;
    u32 found;
    u32 status;
    u32 i;
    s32 index;
    s32 kind;

    battle = fightTrainerGetStatus(ctx, 0, 0x4b, 0);
    fightTypeDataBiosGetPtr(param1);
    count = fightTypeDataBiosGetFightoutPokemonNum() & 0xff;
    i = 0;
    while ((u32)(u16)i < count) {
        found = fightTrainerGetStatus(ctx, 0, 0x46, i);
        if (found == target) {
            break;
        }
        i++;
    }
    if ((i & 0xffff) >= count) {
        return fightTrainerAiSelectIrekaeDasuFightPokemon((void *)ctx, param1, param2, target);
    }

    if ((u16)std103_battleGetResult() == 1) {
        goto set_cancel;
    }
    if ((fightFloorIsUseFightTimerCommand(0) == 1) && (fightTimerCommandIsOver() == 1)) {
        goto set_select;
    }
    {
        if (fn_801F18DC(0) != 0) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param1);
            if (found == 0) {
                status = 0;
            }
            else {
                side = (u16)fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param1) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    if (index < 0) {
                        status = 0;
                    }
                    else {
                        status = fightSideGetStatus(0, side, 2, index & 0xffff);
                    }
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            menuOpenCustom(msg, 0, 0, 0, 0, 0);
        }
        /* RULE-EXCEPTION(user-approved): msg reused for the GBA menu handle, which gives retail's register — see docs/RULE_EXCEPTIONS.md */
        msg = menuFightOpenGBAIrekae(battle, ctx, i, param1);
        kind = fn_80089F70(msg);
        if ((u16)std103_battleGetResult() == 1) {
            goto set_cancel;
        }
        if (kind == 2) {
            found = fightTrainerGetStatus(ctx, 0, 0x45, fn_80089F58(msg));
            target = pokemonGetStatus(found, 0, 0xce, 0);
            goto after_select;
        }
	        if (kind == 4) {
	set_select:
	            target = fightTrainerTimeOutSelectIrekaeFightPokemon(ctx, target, param1);
	            goto after_select;
	        }
	        if (kind != 5) {
	            goto set_select;
	        }
	    }
set_cancel:
    target = -2;
after_select:

    if (fn_801F18DC(0) != 0) {
        msg = 0x100;
        found = fightTargetGetPtr(2, ctx, param1);
        if (found == 0) {
            status = 0;
        }
        else {
            side = (u16)fightSideGetStatus(found, 0, 5, 0);
            if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param1) == 0) {
                status = 0;
            }
            else {
                fightTargetDataBiosGetPtr();
                index = fightTargetDataBiosGetBuff();
                if (index < 0) {
                    status = 0;
                }
                else {
                    status = fightSideGetStatus(0, side, 2, index & 0xffff);
                }
            }
        }
        switch (status) {
        case 0xf1:
            msg = 0x100;
            break;
        case 0xf2:
            msg = 0x101;
            break;
        case 0xf3:
            msg = 0x102;
            break;
        case 0xf4:
            msg = 0x103;
            break;
        }
        if (menuIsCheck(msg) != 0) {
            msg = 0x100;
            found = fightTargetGetPtr(2, ctx, param1);
            if (found == 0) {
                status = 0;
            }
            else {
                side = (u16)fightSideGetStatus(found, 0, 5, 0);
                if (fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ctx, param1) == 0) {
                    status = 0;
                }
                else {
                    fightTargetDataBiosGetPtr();
                    index = fightTargetDataBiosGetBuff();
                    if (index < 0) {
                        status = 0;
                    }
                    else {
                        status = fightSideGetStatus(0, side, 2, index & 0xffff);
                    }
                }
            }
            switch (status) {
            case 0xf1:
                msg = 0x100;
                break;
            case 0xf2:
                msg = 0x101;
                break;
            case 0xf3:
                msg = 0x102;
                break;
            case 0xf4:
                msg = 0x103;
                break;
            }
            menuCloseCustom(msg, 0, 1);
        }
    }
    return target;
}

#if !defined(FIGHT_MENU_ONLY_80261B68)
/* Address: 0x80264ADC | Size: 0x27C | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubBallSelectTargetPokemon__FP15FightOutPokemonUsUs(u32 r3,u32 r4,u32 r5)

{
    extern int menuFightCloseTarget();
    extern int menuFightOpenTarget();
    extern int winMsgCloseFight();
    extern int winMsgOpenFight();
    extern int std082_flagGetValue();
    extern int fightMainWaitFrame();
  u16 uVar4;
  u32 uVar1;
  u8 cVar6;
  int iVar2;
  u16 sVar5;
  u32 uVar3;
  u32 found;
  u8 auStack_38 [0x24];
  u32 local_34;
  u32 local_2c;
  u32 local_24;
  u32 local_1c;
  u8 local_18;
  u8 local_17;
  
LAB_00261af4:
  uVar1 = fightTargetGetPtr(0xf,r3,r5);
    cVar6 = fightOutPokemonCheckFightOut();
    if (cVar6 == '\x01') {
    found = fightTargetGetPtr(2,uVar1,r5);
    if (found == 0) {
      uVar3 = 0;
    }
    else {
      uVar4 = fightSideGetStatus(found,0,5,0);
      sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1,r5);
      if (sVar5 == 0) {
        uVar3 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar3 = fightTargetDataBiosGetBuff();
        if ((int)uVar3 < 0) {
          uVar3 = 0;
        }
        else {
          uVar3 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
        }
      }
    }
  }
  else {
    uVar3 = 0;
  }
  *(u32 *)(auStack_38 + 4) = uVar3;
  {
    u32 uVar1b;
    u16 uVar4b;

    uVar1b = fightTargetGetPtr(0x10,r3,r5);
    cVar6 = fightOutPokemonCheckFightOut();
    if (cVar6 == '\x01') {
      found = fightTargetGetPtr(2,uVar1b,r5);
      if (found == 0) {
        uVar3 = 0;
      }
      else {
        uVar4b = fightSideGetStatus(found,0,5,0);
        sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1b,r5);
        if (sVar5 == 0) {
          uVar3 = 0;
        }
        else {
          fightTargetDataBiosGetPtr();
          uVar3 = fightTargetDataBiosGetBuff();
          if ((int)uVar3 < 0) {
            uVar3 = 0;
          }
          else {
            uVar3 = fightSideGetStatus(0,uVar4b,3,uVar3 & 0xffff);
          }
        }
      }
    }
    else {
      uVar3 = 0;
    }
  }
  *(u32 *)(auStack_38 + 0xc) = uVar3;
  *(u32 *)(auStack_38 + 0x14) = 0;
  *(u32 *)(auStack_38 + 0x1c) = 0;
  auStack_38[0x20] = 2;
  auStack_38[0x21] = fn_801F18DC(0);
  iVar2 = menuFightOpenTarget((int*)auStack_38,0,1);
  if (iVar2 < 0) {
    menuFightCloseTarget(1);
    return 0;
  }
  if (iVar2 != 0) goto LAB_00261cb0;
  uVar1 = fightTargetGetPtr(0xf,r3,r5);
  goto LAB_00261ccc;
LAB_00261cb0:
  if (iVar2 == 1) {
    uVar1 = fightTargetGetPtr(0x10,r3,r5);
LAB_00261ccc:
    cVar6 = fightOutPokemonCheckFightOut(uVar1);
    if (cVar6 != '\0') {
      fightOutPokemonGetPokemonPtr(uVar1);
      cVar6 = pokemonIsDarkPokemon();
      if (cVar6 == '\0') {
        uVar3 = std082_flagGetValue(0x99f);
        if (uVar3 == 0) {
          winMsgOpenFight(0x7716,1,1);
        }
        else {
          winMsgOpenFight(0x7702,1,1);
        }
        fightMainWaitFrame(0x40);
        winMsgCloseFight(0);
        goto LAB_00261af4;
      }
      menuFightCloseTarget(1);
      return uVar1;
    }
  }
  goto LAB_00261af4;
}

#endif
#endif /* !FIGHT_MENU_GCHERO_SPLIT */
#endif

#if defined(FIGHT_MENU_EXACT_8026503C)

/* Address: 0x8026503C | Size: 0x2F0 | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs(u32 r3,u32 r4,u32 r5)

{
    extern u8 lbl_8047B678;
  u16 uVar4;
  u32 uVar1;
  u8 cVar6;
  int iVar2;
  u16 sVar5;
  u32 uVar3;
  u32 found;

  u8 auStack_38 [0x24];
  u32 local_34;
  u32 local_2c;
  u32 local_24;
  u32 local_1c;
  u8 local_18;
  u8 local_17;
  
LAB_00262054:
  do {
    uVar1 = fightTargetGetPtr(0xf,r3,r5);
    cVar6 = fightOutPokemonCheckFightOut();
    if (cVar6 == '\x01') {
      found = fightTargetGetPtr(2,uVar1,r5);
      if (found == 0) {
        uVar3 = 0;
      }
      else {
        uVar4 = fightSideGetStatus(found,0,5,0);
        sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1,r5);
        if (sVar5 == 0) {
          uVar3 = 0;
        }
        else {
          fightTargetDataBiosGetPtr();
          uVar3 = fightTargetDataBiosGetBuff();
          if ((int)uVar3 < 0) {
            uVar3 = 0;
          }
          else {
            uVar3 = fightSideGetStatus(0,uVar4,3,uVar3 & 0xffff);
          }
        }
      }
    }
    else {
      uVar3 = 0;
    }
    *(u32 *)(auStack_38 + 4) = uVar3;
    {
      u32 uVar1b;
      u16 uVar4b;

      uVar1b = fightTargetGetPtr(0x10,r3,r5);
      cVar6 = fightOutPokemonCheckFightOut();
      if (cVar6 == '\x01') {
        found = fightTargetGetPtr(2,uVar1b,r5);
        if (found == 0) {
          uVar3 = 0;
        }
        else {
          uVar4b = fightSideGetStatus(found,0,5,0);
          sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1b,r5);
          if (sVar5 == 0) {
            uVar3 = 0;
          }
          else {
            fightTargetDataBiosGetPtr();
            uVar3 = fightTargetDataBiosGetBuff();
            if ((int)uVar3 < 0) {
              uVar3 = 0;
            }
            else {
              uVar3 = fightSideGetStatus(0,uVar4b,3,uVar3 & 0xffff);
            }
          }
        }
      }
      else {
        uVar3 = 0;
      }
    }
    *(u32 *)(auStack_38 + 0xc) = uVar3;
    {
      u32 uVar1c;
      u16 uVar4c;

      uVar1c = fightTargetGetPtr(0xe,r3,r5);
      cVar6 = fightOutPokemonCheckFightOut();
      if (cVar6 == '\x01') {
        found = fightTargetGetPtr(2,uVar1c,r5);
        if (found == 0) {
          uVar3 = 0;
        }
        else {
          uVar4c = fightSideGetStatus(found,0,5,0);
          sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar1c,r5);
          if (sVar5 == 0) {
            uVar3 = 0;
          }
          else {
            fightTargetDataBiosGetPtr();
            uVar3 = fightTargetDataBiosGetBuff();
            if ((int)uVar3 < 0) {
              uVar3 = 0;
            }
            else {
              uVar3 = fightSideGetStatus(0,uVar4c,3,uVar3 & 0xffff);
            }
          }
        }
      }
      else {
        uVar3 = 0;
      }
    }
    *(u32 *)(auStack_38 + 0x14) = uVar3;
    *(u32 *)(auStack_38 + 0x1c) = 0;
    auStack_38[0x20] = 3;
    auStack_38[0x21] = fn_801F18DC(0);
    iVar2 = menuFightOpenTarget((int*)auStack_38,0,1);
    lbl_8047B678 = 1;
    if (iVar2 < 0) {
      menuFightCloseTarget(1);
      return 0;
    }
    if (iVar2 == 0) {
      uVar1 = fightTargetGetPtr(0xf,r3,r5);
    }
    else if (iVar2 == 1) {
      uVar1 = fightTargetGetPtr(0x10,r3,r5);
    }
    else {
      if (iVar2 != 2) goto LAB_00262054;
      uVar1 = fightTargetGetPtr(0xe,r3,r5);
    }
    cVar6 = fightOutPokemonCheckFightOut(uVar1);
    if (cVar6 != '\0') {
      menuFightCloseTarget(1);
      return uVar1;
    }
  } while (1);
}

/* Address: 0x8026532C | Size: 0xD0 | Ghidra import */
void fightMenuStatusMenuClose(u32 r3, u32 r4, u32 r5)
{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u32 uVar6;

  iVar1 = fightTargetGetPtr(2, r3, r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1, 0, 5, 0);
    sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3, r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar2 = fightTargetDataBiosGetBuff();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0, uVar3, 3, uVar2 & 0xffff);
      }
    }
  }
  if ((u8)menuIsCheck(uVar6) != 0) {
    menuCloseCustom(uVar6, 0, r5);
  }
}

/* Address: 0x802653FC | Size: 0x19C | Ghidra import */
void fightMenuFightOutPokemonRenewStatusMenu(u32 r3, u32 r4, u32 r5)
{
  typedef struct StatusMenuCopy {
    u32 word[12];
  } StatusMenuCopy;
  typedef struct StatusMenuOut {
    StatusMenuCopy copy;
    u32 pad[2];
  } StatusMenuOut;
  extern int fightOutPokemonToMenuPokemonStatus();
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 r28;
  u32 r29;
  u32 r30;
  u32 iVar3;
  u16 uVar5;
  u16 sVar6;
  u32 uVar4;
  u32 checkStatus;
  u32 uVar11;
  StatusMenuOut local_48;
  StatusMenuCopy local_78;

  r28 = r3;
  r29 = r4;
  r30 = r5;
  iVar3 = fightTargetGetPtr(2, r28, r29);
  if (iVar3 == 0) {
    checkStatus = 0;
  }
  else {
    uVar5 = fightSideGetStatus(iVar3, 0, 5, 0);
    sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r28, r29);
    if (sVar6 == 0) {
      checkStatus = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar4 = fightTargetDataBiosGetBuff();
      if ((int)uVar4 < 0) {
        checkStatus = 0;
      }
      else {
        checkStatus = fightSideGetStatus(0, uVar5, 3, uVar4 & 0xffff);
      }
    }
  }
  if ((u8)menuIsCheck(checkStatus) != 0) {
    iVar3 = fightTargetGetPtr(2, r28, r29);
    if (iVar3 == 0) {
      uVar11 = 0;
    }
    else {
      uVar5 = fightSideGetStatus(iVar3, 0, 5, 0);
      sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r28, r29);
      if (sVar6 == 0) {
        uVar11 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar4 = fightTargetDataBiosGetBuff();
        if ((int)uVar4 < 0) {
          uVar11 = 0;
        }
        else {
          uVar11 = fightSideGetStatus(0, uVar5, 3, uVar4 & 0xffff);
        }
      }
    }
    fightOutPokemonToMenuPokemonStatus(r28, &local_78);
    if ((u8)r30 == 0) {
      *(u8 *)((u8 *)&local_78 + 0x29) = 0;
    }
    local_48.copy = local_78;
    menuOpenCustom(uVar11, -1, 0, 0, 0, 1, &local_48.copy);
  }
}

/* Address: 0x80265598 | Size: 0x114 | Ghidra import */
void fn_80265598(u32 r3, u32 r4, u32 r5)
{
  typedef struct StatusMenuCopy {
    u32 word[12];
  } StatusMenuCopy;
  typedef struct StatusMenuOut {
    StatusMenuCopy copy;
    u32 pad[2];
  } StatusMenuOut;
  extern int fightOutPokemonToMenuPokemonStatus();
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 r28;
  u32 r29;
  u32 r30;
  u32 iVar3;
  u16 uVar5;
  u16 sVar6;
  u32 uVar4;
  u32 uVar11;
  StatusMenuOut local_48;
  StatusMenuCopy local_78;

  r28 = r3;
  r29 = r4;
  r30 = r5;
  iVar3 = fightTargetGetPtr(2, r28, r29);
  if (iVar3 == 0) {
    uVar11 = 0;
  }
  else {
    uVar5 = fightSideGetStatus(iVar3, 0, 5, 0);
    sVar6 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r28, r29);
    if (sVar6 == 0) {
      uVar11 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar4 = fightTargetDataBiosGetBuff();
      if ((int)uVar4 < 0) {
        uVar11 = 0;
      }
      else {
        uVar11 = fightSideGetStatus(0, uVar5, 3, uVar4 & 0xffff);
      }
    }
  }
  fightOutPokemonToMenuPokemonStatus(r28, &local_78);
  if ((u8)r30 == 0) {
    *(u8 *)((u8 *)&local_78 + 0x29) = 0;
  }
  local_48.copy = local_78;
  menuOpenCustom(uVar11, -1, 0, 0, 0, 1, &local_48.copy);
}

/* Address: 0x802656AC | Size: 0xA8 | Ghidra import */
u32 fightMenuGetFightOutPokemonPtrToStatusMenuId(u32 r3, u32 r4)
{
  u32 iVar1;
  u16 uVar3;
  u16 sVar4;
  u32 uVar2;
  u32 uVar6;

  iVar1 = fightTargetGetPtr(2, r3, r4);
  if (iVar1 == 0) {
    uVar6 = 0;
  }
  else {
    uVar3 = fightSideGetStatus(iVar1, 0, 5, 0);
    sVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3, r4);
    if (sVar4 == 0) {
      uVar6 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar2 = fightTargetDataBiosGetBuff();
      if ((int)uVar2 < 0) {
        uVar6 = 0;
      }
      else {
        uVar6 = fightSideGetStatus(0, uVar3, 3, uVar2 & 0xffff);
      }
    }
  }
  return uVar6;
}

/* Address: 0x80265754 | Size: 0x174 | Ghidra import */
void fightMenuFightTrainerRenewStatusMenu(u32 r3,u32 r4)

{
  typedef struct BattleStatusPair {
      u32 unk0;
      u16 unk4;
  } BattleStatusPair;
  extern void menuOpenCustom(u32, u32, u32, u32, u32, u32, ...);
  u32 iVar1;
  u32 uVar2;
  u8 cVar6;
  u16 uVar4;
  u16 sVar5;
  u32 uVar3;
  BattleStatusPair local_20;
  BattleStatusPair local_28;
  
  iVar1 = fightTargetGetPtr(2,r3,r4);
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    uVar4 = fightSideGetStatus(iVar1,0,5,0);
    sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
    if (sVar5 == 0) {
      uVar2 = 0;
    }
    else {
      fightTargetDataBiosGetPtr();
      uVar3 = fightTargetDataBiosGetBuff();
      if ((int)uVar3 < 0) {
        uVar2 = 0;
      }
      else {
        uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
      }
    }
  }
  cVar6 = menuIsCheck(uVar2);
  if (cVar6 != '\0') {
    iVar1 = fightTargetGetPtr(2,r3,r4);
    if (iVar1 == 0) {
      uVar2 = 0;
    }
    else {
      uVar4 = fightSideGetStatus(iVar1,0,5,0);
      sVar5 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
      if (sVar5 == 0) {
        uVar2 = 0;
      }
      else {
        fightTargetDataBiosGetPtr();
        uVar3 = fightTargetDataBiosGetBuff();
        if ((int)uVar3 < 0) {
          uVar2 = 0;
        }
        else {
          uVar2 = fightSideGetStatus(0,uVar4,2,uVar3 & 0xffff);
        }
      }
    }
    fightTrainerToMenuBallStatus(r3,&local_28);
    local_20.unk0 = local_28.unk0;
    local_20.unk4 = local_28.unk4;
    menuOpenCustom(uVar2,0,0,0,0,1,&local_20);
  }
  return;
}

#endif

#if defined(FIGHT_MENU_CANDIDATE_80261B68) && !defined(FIGHT_MENU_ONLY_80261B68) && !defined(FIGHT_MENU_ONLY_8026316C)
#if !defined(FIGHT_MENU_ONLY_80263DE4)

/* Address: 0x80264D58 | Size: 0x2E4 | Ghidra import */
u32 _fightMenuFightTrainerGcHeroOpenMenuSubWaza__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32 trainer, u32 outPokemon, u32 side)
{
  extern void fightTypeDataBiosGetPtr(u32);
  extern u32 fightTypeDataBiosGetFightoutPokemonNum(void);
  extern void fightOutPokemonToMenuWazaStatus(u32, u8 *);
  extern u8 fn_801F18DC(u32);
  extern u32 fightOutPokemonGetPokemonPtr(u32);
  u32 pokemon;
  extern s32 pokemonGetStatus(u32, u32, u32, u16);
  extern s32 menuFightOpenWaza(u8 *, u32, u32);
  extern void menuFightCloseWaza(u32);
  extern u8 fightOutPokemonCheckCanOutOkWazaBanme(u32, u16, u32, u16 *);
  extern void msgctrlSetValue(u32, u32);
  extern u32 GSmsgGetGSchar(u32);
  extern u32 fightOutPokemonGetSoubiItemDataId(u32);
  unsigned int attacker;
  extern void fightFloorSetStatus(u32, u32, u32, u32, u16);
  extern void winMsgOpenFight(u32, u32, u32);
  extern void fightMainWaitFrame(u32);
  extern u32 wazaGetStatus(u32, u16, u32, u32);
  extern void winMsgCloseFight(u32);
  extern u32 fightSeqSelectTarget(u32, u16, u32, void *, u32, u32, s32);
  extern u32 _fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs(u32, u16, u32);
  extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
  extern void fightOutPokemonCreateFightActionAttackWaza(u32, u32, u32, u32, char *, u16, u32, s8, u32);
  extern char lbl_80375CA8[];
  extern u32 *lbl_80478DF8;
  extern u8 lbl_8047B678;
  u16 waza;
  u32 msgId;
  u32 result;
  u8 num;
  u16 wazaId;
  u32 target;
  u8 check;
  s32 select;
  u16 altWaza;
  u8 work[69];
  u32 status;
  u32 item;
  u32 carrier;
  /* RULE-EXCEPTION(user-approved): carrier copies of outPokemon/the target result and a 64-bit mask on waza are kept for register allocation - see docs/RULE_EXCEPTIONS.md */
  carrier = outPokemon;
  fightTypeDataBiosGetPtr(side);
  num = fightTypeDataBiosGetFightoutPokemonNum();
  fightOutPokemonToMenuWazaStatus(carrier, work);
  work[68] = fn_801F18DC(0);
  pokemon = fightOutPokemonGetPokemonPtr(carrier);
  for (;;)
  {
    status = pokemonGetStatus(carrier, 0, 0x101, 0);
    select = menuFightOpenWaza(work, status, 1);
    if (select < 0)
    {
      menuFightCloseWaza(1);
      return 0;
    }
    check = fightOutPokemonCheckCanOutOkWazaBanme(carrier, select, 1, &altWaza);
    wazaId = pokemonGetStatus(pokemon, 0, 0x7f, select);
    if (check != 0)
    {
      msgctrlSetValue(0x11, carrier);
      msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, wazaId, 1, 0)));
      item = fightOutPokemonGetSoubiItemDataId(carrier);
      fightFloorSetStatus(0, 0, 0x56, 0, item);
    }
    if (check == 6)
    {
      msgId = 0x7661;
    }
    else
      if (check == 5)
    {
      msgctrlSetValue(0x28, GSmsgGetGSchar(wazaGetStatus(0, altWaza, 1, 0)));
      msgId = 0x76bb;
    }
    else
      if (check == 4)
    {
      msgId = 0x7600;
    }
    else
      if (check == 3)
    {
      msgId = 0x75ff;
    }
    else
      if (check == 2)
    {
      msgId = 0x75fe;
    }
    else
      if (check == 1)
    {
      msgId = 0x75fd;
    }
    if (check != 0)
    {
      if (msgId != 0)
      {
        winMsgOpenFight(msgId, 1, 1);
      }
      fightMainWaitFrame(0x40);
      winMsgCloseFight(0);
      continue;
    }
    waza = pokemonGetStatus(pokemon, 0, 0x7f, select);
    if (((waza == 0) || (waza >= (*lbl_80478DF8))) || (waza == 0x165))
    {
      continue;
    }
    lbl_8047B678 = 0;
    attacker = carrier;
    result = fightSeqSelectTarget(attacker, waza, side, _fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs, 1, 0, -1);
    target = result;
    if (((fn_801F18DC(0) == 1) && (lbl_8047B678 == 0)) && (num >= 2))
    {
      if (_fightMenuFightTrainerGcHeroOpenMenuSubWazaSelectDefensePokemon__FP15FightOutPokemonUsUs(carrier, waza, side) == 0)
      {
        continue;
      }
    }
    if (target == 0)
    {
      continue;
    }
    target = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(target, side);
    menuFightCloseWaza(1);
    fightOutPokemonCreateFightActionAttackWaza(carrier, 0, 0x13, 0, lbl_80375CA8, waza & 0xFFFFFFFFFFFFFFFFu, target, select, 0);
    return 1;
  }

}
#endif /* !FIGHT_MENU_ONLY_80263DE4 */
#if !defined(FIGHT_MENU_ONLY_80264D58)

/* Address: 0x80264488 | Size: 0x654 | Ghidra import (PSQ removed) */


u32 _fightMenuFightTrainerGcHeroOpenMenuSubItem__FP13FIGHT_TRAINERP15FightOutPokemonUs(u32 trainer, u32 outPokemon, u32 side)
{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u16);
    extern u32 fightTrainerGetFightOutPokemonToTemotiBanme(u32, u32);
    extern u8 fightPokemonCheckFightOut(u32);
    /* RULE-EXCEPTION(user-approved): long copy of the selected item for its last two uses, chosen for register allocation (permuter farm) — see docs/RULE_EXCEPTIONS.md */
    long itemId;
    extern u8 fightOutPokemonIsPokemonJoutaiDataId(u32, u32);
    extern s16 fightTrainerGetFightPokemonToTemotiBanme(u32, u32);
    extern s32 pokemonGetStatus(u32, u32, u32, u32);
    extern s32 fn_800D37CC(void);
    extern const f32 lbl_8047E6CC; /* 2.0f */
    extern void menuCreateOffScreen(f32);
    extern void menuReleaseOffScreen(f32);
    extern u32 fn_80018F88(u32, s32*, u32);
    extern void fightWaitFrames(u32);
    extern u32 fn_80019064(void);
    extern u8 itemGetStatus(u32, u32, u32, u32);
    extern void fn_801DA36C(u32, u32);
    extern u8 fn_801F18DC(u32);
    extern void menuFightStatusSetActive(u32, u32);
    extern u8 fightTrainerIsSelectedItemBall(u32);
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u32);
    extern void wsRunEndOfTurnScript2(void);
    extern u8 fightOutPokemonCheckFightOut(u32);
    extern void pokemonSetStatus(u32, u32, u32, u32, u32);
    extern void fightOutPokemonCreateFightActionUseItem(u32, u32, u32, u32, char*, u16, u32, u8, u32);
    extern char lbl_80375D70[];
    u16 i;
    u32 index;
    u32 temoti;
    u32 pokemon;
    u32 effect;
    u32 item;
    u8 menuKind;
    u32 target;
    u32 useSlot;
    u8 wasOut[2];
    u8 hadFlag8[2];
    u8 hadFlag7[2];
    s32 slot;

    slot = 0;
    for (i = 0; i < 2; i++) {
        wasOut[i] = 0;
        hadFlag8[i] = 0;
        hadFlag7[i] = 0;
    }
    for (i = 0; i < 2; i++) {
        pokemon = fightTrainerGetStatus(trainer, 0, 0x46, i);
        if (pokemon != 0) {
            index = fightTrainerGetFightOutPokemonToTemotiBanme(trainer, pokemon);
            wasOut[i] = fightPokemonCheckFightOut(fightTrainerGetStatus(trainer, 0, 0x45, index));
            hadFlag8[i] = fightOutPokemonIsPokemonJoutaiDataId(pokemon, 8);
            hadFlag7[i] = fightOutPokemonIsPokemonJoutaiDataId(pokemon, 7);
        }
    }
    if (fightTrainerGetStatus(trainer, 0, 0x44, 0) == 0) {
        return 0;
    }
    temoti = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
    slot = fightTrainerGetFightPokemonToTemotiBanme(trainer, temoti);
    for (;;) {
        fightMenuCloseInfoMenuInline(1);
        menuCreateOffScreen(lbl_8047E6CC / (f32)fn_800D37CC());
        item = fn_80018F88(1, &slot, trainer);
        fightWaitFrames(10);
        menuKind = fn_80019064();
        if ((u16)item != 0 && itemGetStatus(0, item, 2, 0) == 2) {
            for (i = 0; i < 2; i++) {
                pokemon = fightTrainerGetStatus(trainer, 0, 0x46, i);
                if (pokemon != 0 && (effect = pokemonGetStatus(pokemon, 0, 0xee, 0)) != 0) {
                    if (hadFlag8[i] == 1 && fightOutPokemonIsPokemonJoutaiDataId(pokemon, 8) == 0) {
                        fn_801DA36C(effect, 1);
                    }
                    if (hadFlag7[i] == 1 && fightOutPokemonIsPokemonJoutaiDataId(pokemon, 7) == 0) {
                        fn_801DA36C(effect, 2);
                    }
                }
            }
        }
        itemId = item;
        fightMenuOpenInfoMenuInline(-1);
        if (outPokemon != 0 && fn_801F18DC(0) != 1) {
            menuFightStatusSetActive(fightMenuGcHeroStatusMenuIdDirect(outPokemon, side, 3), 1);
        }
        menuReleaseOffScreen(lbl_8047E6CC / (f32)fn_800D37CC());
        fightWaitFrames(10);
        if ((u16)item == 0) {
            return 0;
        }
        if (itemGetStatus(0, itemId, 2, 0) == 1) {
            if (fightTrainerIsSelectedItemBall(trainer) == 1) {
                continue;
            }
            target = _fightMenuFightTrainerGcHeroOpenMenuSubBallSelectTargetPokemon__FP15FightOutPokemonUsUs(
                outPokemon, item, side);
            if (target == 0) {
                continue;
            }
            target = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(target, side);
            useSlot = 0;
        } else {
            if (itemGetStatus(0, item, 2, 0) == 2) {
                wsRunEndOfTurnScript2();
                for (i = 0; i < 2; i++) {
                    pokemon = fightTrainerGetStatus(trainer, 0, 0x46, i);
                    if (pokemon != 0 && wasOut[i] == 0 && fightOutPokemonCheckFightOut(pokemon) == 1) {
                        pokemonSetStatus(pokemon, 0, 0x120, 0, 1);
                    }
                }
            }
            useSlot = 1;
            target = (u16)slot;
        }
        fightOutPokemonCreateFightActionUseItem(outPokemon, 0, 0x12, 0, lbl_80375D70, itemId, target, menuKind, useSlot);
        return 1;
    }
}

#endif /* !FIGHT_MENU_ONLY_80264D58 */
#endif
