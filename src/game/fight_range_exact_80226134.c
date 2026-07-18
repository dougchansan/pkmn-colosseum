/**
 * @file fight_range_exact_80226134.c
 * @brief Exact pure-C fight-sequence island, 0x80226134 - 0x802274F0.
 */
#include "dolphin/types.h"

#pragma use_lmw_stmw on

extern u8* lbl_8047B610;
extern u8 lbl_8047B614;
extern u8 lbl_80478D78[1];
extern u32 lbl_8047B618;
extern void* lbl_8047B62C;
extern u8 lbl_80379F58[];
extern u8 lbl_80379A22[];
extern void fn_80211B94(void*, void*, u8);
extern void fn_802274F0(u32, char, char, char);
extern void fn_802271E0(char, char);

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_802624CC fightMenuOpenMsg
void fn_80226134(void) {
    extern int fn_801F54A4();
    extern u8 fn_802624CC();
    u32* arr = *(u32**)(lbl_8047B610 + 1);
    u32 val;

    if (arr != NULL) {
        val = arr[lbl_80478D78[5]];
    } else {
        val = fn_801F54A4(0, 0, 0x50, 0);
    }
    if ((u8)fn_802624CC(val) == 1) {
        lbl_80478D78[7] = 1;
    }
    lbl_8047B610 = lbl_8047B610 + 5;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_802624CC fightMenuOpenMsg
void fn_802261B0(void) {
    extern int fn_801F54A4();
    extern u8 fn_802624CC();
    u32* arr = *(u32**)(lbl_8047B610 + 1);
    u32 val;

    if (arr != NULL) {
        val = arr[lbl_80478D78[5]];
    } else {
        val = fn_801F54A4(0, 0, 0x50, 0);
    }
    if ((u8)fn_802624CC(val) == 1) {
        lbl_80478D78[7] = 1;
    }
    lbl_8047B610 = lbl_8047B610 + 5;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F000C fightMainWaitFrame
#define fn_8026246C fightMenuCloseMsg
void fn_8022622C(void) {
    if (lbl_80478D78[7] != 0) {
        fn_8026246C(fn_801F000C(*(u16*)(lbl_8047B610 + 1)));
        lbl_80478D78[7] = 0;
    }
    lbl_8047B610 += 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_802624CC fightMenuOpenMsg
void fn_80226284(void) {
    extern u8 fn_802624CC();
    if (fn_802624CC(*(u32*)(lbl_8047B610 + 1)) == 1) {
        lbl_80478D78[7] = 1;
    }
    lbl_8047B610 += 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_802624CC fightMenuOpenMsg
void fn_802262D0(void) {
    extern u8 fn_802624CC();
    if (fn_802624CC(*(u32*)(lbl_8047B610 + 1)) == 1) {
        lbl_80478D78[7] = 1;
    }
    lbl_8047B610 += 5;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma dont_inline on
#pragma optimization_level 4
#define fn_8012640C pokemonGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_80209618 fightWazaIsMix
#define fn_8020990C fightWazaIsJoutaiDataId
#define fn_80209960 fightWazaInitJoutaiDataId
#define fn_80262334 fightMenuWazaKoukaMsg
void fn_8022631C(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_802037DC();
    extern u8 fn_80209618();
    extern u8 fn_8020990C();
    extern void fn_80209960();
    extern void fn_80211B94();
    extern u8 fn_80262334();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern void* lbl_8047B62C;
    extern int lbl_80279D18[];
    extern u8 lbl_80379BE7[];
    extern u8 lbl_803791C7[];
    extern u8 lbl_803791BE[];
  u32 uVar1;
  u32 uVar2;
  u32 iVar5;
  u32 uVar3;
  u8 cVar4;

  uVar2 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar2 = fn_801F025C(0x12,0);
  fn_802037DC();
  uVar3 = fightOutPokemonGetSoubiItemDataId(uVar2);
  lbl_80478D78[7] = 0;
  iVar5 = 0;
  cVar4 = fn_8020990C(uVar1,0x40);
  if (cVar4 == 1) {
    cVar4 = fn_8020990C(uVar1,0x43);
    if (cVar4 == 0) goto _body;
  }
  cVar4 = fn_8020990C(uVar1,0x40);
  if (cVar4 != 1) goto _else;
  if (lbl_80478D78[6] < 3) goto _else;
_body:
  iVar5 = lbl_80279D18[lbl_80478D78[6]];
  goto _join;
_else:
  {
    cVar4 = fn_80209618(uVar1);
    if (cVar4 == 0) {
      cVar4 = fn_8020990C(uVar1,0x41);
      if (cVar4 == 1) {
        iVar5 = 0x7637;
      }
      else {
        cVar4 = fn_8020990C(uVar1,0x42);
        if (cVar4 == 1) {
          iVar5 = 0x7636;
        }
        else {
          cVar4 = fn_8020990C(uVar1,0x43);
          if (cVar4 == 1) {
            iVar5 = 0x7543;
          }
          else {
            cVar4 = fn_8020990C(uVar1,0x44);
            if (cVar4 == 1) {
              iVar5 = 0x7632;
            }
            else {
              cVar4 = fn_8020990C(uVar1,0x45);
              if (cVar4 == 1) {
                iVar5 = 0x7647;
              }
              else {
                cVar4 = fn_8020990C(uVar1,0x46);
                if (cVar4 == 1) {
                  iVar5 = 0x75c9;
                }
                else {
                  cVar4 = fn_8020990C(uVar1,0x47);
                  if (cVar4 == 1) {
                    fn_80209960(uVar1,0x46);
                    fn_80209960(uVar1,0x47);
                    fn_801F4C14(0,0,0x56,0,uVar3 & 0xffff);
                    fn_801F4C14(0,0,0x49,0,uVar2);
                    fn_80211B94(lbl_8047B62C,(u32)lbl_80379BE7,0);
                    return;
                  }
                }
              }
            }
          }
        }
      }
    }
    else {
      cVar4 = fn_8020990C(uVar1,0x43);
      if (cVar4 == 1) {
        iVar5 = 0x7543;
      }
      else {
        cVar4 = fn_8020990C(uVar1,0x44);
        if (cVar4 == 1) {
          fn_80209960(uVar1,0x44);
          fn_80209960(uVar1,0x41);
          fn_80209960(uVar1,0x42);
          fn_80211B94(lbl_8047B62C,(u32)lbl_803791C7,0);
          return;
        }
        cVar4 = fn_8020990C(uVar1,0x46);
        if (cVar4 == 1) {
          fn_80209960(uVar1,0x46);
          fn_80209960(uVar1,0x47);
          fn_80211B94(lbl_8047B62C,(u32)lbl_803791BE,0);
          return;
        }
        cVar4 = fn_8020990C(uVar1,0x47);
        if (cVar4 == 1) {
          fn_80209960(uVar1,0x46);
          fn_80209960(uVar1,0x47);
          fn_801F4C14(0,0,0x56,0,uVar3 & 0xffff);
          fn_801F4C14(0,0,0x49,0,uVar2);
          fn_80211B94(lbl_8047B62C,(u32)lbl_80379BE7,0);
          return;
        }
        cVar4 = fn_8020990C(uVar1,0x45);
        if (cVar4 == 1) {
          iVar5 = 0x7647;
        }
      }
    }
  }
_join:
  if (iVar5 != 0) {
    fn_801F4C14(0,0,0x53,0,iVar5);
    cVar4 = fn_80262334(iVar5,uVar2,uVar3);
    if (cVar4 == 1) {
  lbl_80478D78[7] = 1;
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void fn_802266EC(void) {
    extern void* fn_801F025C();
    extern void* fn_8012640C();
    fn_8012640C(fn_801F025C(0x11, 0), 0, 0xD9, 0);
    lbl_8047B610++;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_802096E8 fightWazaIsHit
#define fn_801F4C14 fightFloorSetStatus
#define fn_802624CC fightMenuOpenMsg
void fn_80226730(void) {
    extern void* fn_801F025C();
    extern u8 fn_802096E8();
    extern u8 fn_801F4C14();
    extern u8 fn_802624CC();
    extern u8 fn_8011BEB4();
    extern void* fn_8012640C();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx, 0, 0xD9, 0);

    if ((u8)fn_8011BEB4(resolved, 0, 0x2B, 0) == 2) {
        if ((u8)fn_802096E8(resolved) == 1) {
            fn_801F4C14(0, 0, 0x52, 0, 0x7631);
            if ((u8)fn_802624CC(0x7631) == 1) {
                lbl_80478D78[7] = 1;
            }
        }
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802656AC fightMenuGetFightOutPokemonPtrToStatusMenuId
#define fn_80102620 menuIsCheck
#define fn_80011C78 menuFightStatusWaitAnimeHP
#define fn_802038A4 fightOutPokemonIsHinsi
#define fn_80207C24 fightOutPokemonSetWazaEffectDownFlag
#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
void fn_802267E8(void) {
    extern int fn_802656AC();
    extern s32 fn_80102620();
    extern u32 fn_80011C78();
    extern u8 fn_802038A4();
    extern void fn_80207C24();
    extern s32 fn_801FEF74();
    extern void fn_80202810();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    u32 val;
    u32 ctx;
    u16 count;

    count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    val = fn_802656AC(ctx, count, 1);

    if ((u8)fn_80102620(val) == 1) {
        fn_80011C78(val, 1);
    }
    if ((u8)fn_802038A4(ctx) == 1) {
        fn_80207C24(ctx, 1);
    }
    if ((u8)fn_802026E4(ctx, 0x14) == 1 && fn_801FEF74(ctx) <= 0) {
        do {
            if ((u8)fn_801DA5C4(6) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
        fn_80202810(ctx, 0x14);
        fn_80211B94(lbl_8047B62C, (void*)&lbl_80379A22, 0);
        fn_8020248C(ctx, 0x14, 0);
        fn_80201764(ctx, 0x14, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma dont_inline on
#pragma optimization_level 4
#pragma opt_lifetimes off
void fn_80226914(void)
{
    extern void menuFightStatusStartAnimHP();
    extern u32 menuIsCheck(u32);
    extern int fn_8010C4A0();
    extern void wazaSetStatus();
    extern s32 wazaGetStatus();
    extern void fightMainWaitFrame();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern void fightOutPokemonAddFightOutPokemonEnemyDamage();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u32 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetJoutaiMigawariHp();
    extern u32 fightOutPokemonGetJoutaiMigawariHp();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightWazaIsHit();
    extern void fightMenuCloseMsg();
    extern void fightMenuOpenMsg();
    extern void fn_80265598();
    extern u32 fightMenuGetFightOutPokemonPtrToStatusMenuId();
    extern u32 lbl_8047B618;
    u32 initialEffectDamage;
    u32 attacker;
    u32 moveId;
    u32 ally;
    u16 category;
    u32 move;
    s32 damage;
    s32 hp;
    u32 target;
    s32 finalHp;
    u32 effectDamage;
    u16 floorId;
    s16 displayHp;
    u32 pokemon;
    s32 maxHp;
    s32 substituteHp;
    u32 relativeRaw;
    s16 previousDamage;
    u8 mode;

    floorId = fightFloorGetStatus(0, 0, 0x14, 0);
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    moveId = fightOutPokemonGetUseWazaDataId((void*)attacker);
    move = pokemonGetStatus(attacker, 0, 0xd9, 0);
    damage = wazaGetStatus(move, 0, 0x2d, 0);
    initialEffectDamage = 0;
    initialEffectDamage += wazaGetStatus(move, 0, 0x2e, 0);
    category = wazaGetStatus(move, 0, 0x30, 0);
    ally = fightTargetGetPtrAsNowFightType(0x12, 0);

    if ((u8)fightWazaIsHit(move) == 1) {
        target = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
        pokemon = fightOutPokemonGetPokemonPtr(target);
        hp = pokemonGetStatus(pokemon, 0, 0x83, 0);
        maxHp = pokemonGetStatus(pokemon, 0, 0x87, 0);
        substituteHp = fightOutPokemonGetJoutaiMigawariHp(target);

        if ((substituteHp != 0) && ((lbl_8047B618 & 0x100) == 0)) {
            if (substituteHp >= damage) {
                pokemon = damage;
                substituteHp -= damage;
            } else {
                pokemon = substituteHp;
                substituteHp = 0;
            }
            if (pokemonGetStatus(target, 0, 0x11b, 0) == 0) {
                pokemonSetStatus((void*)target, 0, 0x11b, 0, pokemon);
            }
            wazaSetStatus(move, 0, 0x2e, 0, pokemon);
            fightOutPokemonSetJoutaiMigawariHp(target, substituteHp);
            fightMenuOpenMsg(0x75b1);
            fightMainWaitFrame(0x40);
            fightMenuCloseMsg();
        } else {
            lbl_8047B618 &= 0xfffffeff;
            if (damage < 0) {
                effectDamage = initialEffectDamage;
                finalHp = hp - damage;
                if (finalHp > maxHp) {
                    finalHp = maxHp;
                }
            } else {
                if (hp > damage) {
                    effectDamage = damage;
                    finalHp = hp - damage;
                } else {
                    effectDamage = hp;
                    finalHp = 0;
                }

                if ((lbl_8047B618 & 0x20) != 0) {
                    lbl_8047B618 &= 0xffffffdf;
                } else {
                    previousDamage = (s16)pokemonGetStatus(target, 0, 0xf5, 0);
                    previousDamage += damage;
                    pokemonSetStatus((void*)target, 0, 0xf5, 0, previousDamage);
                    if (lbl_8047B610[1] == 0x12) {
                        relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floorId);
                        pokemonSetStatus((void*)target, 0, 0xf6, 0,
                                         relativeRaw & 0xffff);
                    } else {
                        relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ally, floorId);
                        pokemonSetStatus((void*)target, 0, 0xf6, 0,
                                         relativeRaw & 0xffff);
                    }
                    fightOutPokemonAddFightOutPokemonEnemyDamage(target, attacker,
                                                                  effectDamage & 0xffff);
                }

                if ((pokemonGetStatus(target, 0, 0x11b, 0) == 0) &&
                    ((lbl_8047B618 & 0x100000) == 0)) {
                    pokemonSetStatus((void*)target, 0, 0x11b, 0, effectDamage);
                }

                mode = fn_8010C4A0(category);
                switch (mode) {
                case 1:
                    if (((lbl_8047B618 & 0x100000) == 0) &&
                        ((moveId & 0xffff) != 0xdc)) {
                        pokemonSetStatus((void*)target, 0, 0x102, 0, effectDamage);
                        pokemonSetStatus((void*)target, 0, 0x11c, 0, effectDamage);
                        if (lbl_8047B610[1] == 0x12) {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floorId);
                            pokemonSetStatus((void*)target, 0, 0x103, 0,
                                             damage = relativeRaw & 0xffff);
                            pokemonSetStatus((void*)target, 0, 0x11d, 0, damage);
                        } else {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ally, floorId);
                            pokemonSetStatus((void*)target, 0, 0x103, 0,
                                             damage = relativeRaw & 0xffff);
                            pokemonSetStatus((void*)target, 0, 0x11d, 0, damage);
                        }
                    }
                    break;
                case 2:
                    if ((lbl_8047B618 & 0x100000) == 0) {
                        pokemonSetStatus((void*)target, 0, 0x104, 0, effectDamage);
                        pokemonSetStatus((void*)target, 0, 0x11e, 0, effectDamage);
                        if (lbl_8047B610[1] == 0x12) {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floorId);
                            pokemonSetStatus((void*)target, 0, 0x105, 0,
                                             damage = relativeRaw & 0xffff);
                            pokemonSetStatus((void*)target, 0, 0x11f, 0, damage);
                        } else {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ally, floorId);
                            pokemonSetStatus((void*)target, 0, 0x105, 0,
                                             damage = relativeRaw & 0xffff);
                            pokemonSetStatus((void*)target, 0, 0x11f, 0, damage);
                        }
                    }
                    break;
                }
            }

            lbl_8047B618 &= 0xffefffff;
            wazaSetStatus(move, 0, 0x2e, 0, effectDamage);
            pokemonSetStatus((void*)pokemon, 0, 0x83, 0, finalHp);
            if ((u8)fightOutPokemonIsUseHensinBuff(target) == 1) {
                fightOutPokemonSetHensinPokemonStatusId(target, 0x83, 0, 0);
            }
            maxHp = fightMenuGetFightOutPokemonPtrToStatusMenuId(target, floorId, 1);
            displayHp = finalHp;
            if ((u8)menuIsCheck(maxHp) == 0) {
                fn_80265598(target, floorId, 1);
            }
            menuFightStatusStartAnimHP(maxHp, displayHp);
        }
    } else {
        pokemon = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
        if (pokemonGetStatus(pokemon, 0, 0x11b, 0) == 0) {
            pokemonSetStatus((void*)pokemon, 0, 0x11b, 0, 0xffff);
        }
    }
    lbl_8047B610 += 2;
}

#pragma opt_lifetimes reset
#pragma opt_propagation off
#pragma opt_lifetimes off
#pragma optimization_level 2
#pragma use_lmw_stmw on
#pragma optimize_for_size on
void fn_80226F0C(void) {
    extern u32 lbl_8047B618;
    extern void _threadSwitch();
    extern u8 fn_801DA5C4();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    struct {
        u32 index;
        u32 attacker;
        u32 move;
    } vars;

    vars.attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    vars.move = fightOutPokemonGetUseWazaDataId(vars.attacker);
    pokemonGetStatus(vars.attacker, 0, 0xd9, 0);
    fightTargetGetPtrAsNowFightType(0x12, 0);
    if ((lbl_8047B618 & 0x80) == 0 || (u16)vars.move == 0x90 ||
        (u16)vars.move == 0xa4) {
        vars.index = lbl_8047B610[1];
        do {
            if (fn_801DA5C4(vars.index) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
    if ((u8)vars.index == 2 || (u8)vars.index == 6) {
        fightMenuAllFightOutPokemonCloseStatusMenu(0);
    }
    lbl_8047B610 += 2;
    return;
}

#pragma optimization_level reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma use_lmw_stmw on
#pragma optimize_for_size on
void fn_80226FD4(void)

{
    extern void fn_801DA9E8();
    extern u8 fn_801DDD28();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern void fightWazaDoEffectFunc();
    extern u8 fightWazaIsHit();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void fn_80211B94();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fn_80265598();
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
    extern u8 lbl_80378964[];
    u32 move;
    u32 attacker;
    u32 status;
    u32 target;
    u32 effectAttacker;
    u16 floorMove;

    fightFloorGetStatus(0, 0, 0x14, 0);
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    status = pokemonGetStatus(attacker, 0, 0xd9, 0);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fightWazaIsHit(status) == 1) {
        if ((lbl_8047B618 & 0x80) != 0 && (u16)move != 0x90 &&
            (u16)move != 0xa4) {
            fn_80211B94(lbl_8047B62C, lbl_80378964, 0);
        } else {
            fightWazaDoEffectFunc(move, attacker, target,
                                  lbl_80379F58[0x160a1]);
        }
        lbl_80379F58[0x16002]++;
        lbl_8047B610++;
        lbl_80379F58[0x160a1]++;
    } else {
        fightMenuAllFightOutPokemonCloseStatusMenu(0);
        floorMove = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
        effectAttacker = pokemonGetStatus(target, 0, 0xee, 0);
        if (effectAttacker != 0 && fn_801DDD28(effectAttacker, 0x57, 4, 0) != 0) {
            fn_801DA9E8(effectAttacker, 0x57, 4);
            fn_80265598(target, floorMove, 1);
        }
        lbl_8047B610++;
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_DAMAGE_LOSS_ONLY(void) { fn_802271E0(0, 1); lbl_8047B610 = lbl_8047B610 + 1; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_DAMAGE_LOSS(void) { fn_802271E0(1, 1); lbl_8047B610 = lbl_8047B610 + 1; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma dont_inline on
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_80203FE4 fightOutPokemonGetSoubiItemSoubiDataId
#define fn_80203EDC figthOutPokemonGetSoubiItemBuff
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_801254B4 pokemonSetStatus
#define fn_8020981C fightWazaCheckWriteJoutaiDataId
#define fn_802097C8 fightWazaWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
void fn_802271E0(char r3, char r4)
{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern int fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_802026E4();
    extern int fn_80203EDC();
    extern u16 fn_80203FE4();
    extern u16 fn_802040E8();
    extern u32 fn_80205B8C();
    extern void fn_802097C8();
    extern u8 fn_8020981C();
  u32 uVar4;
  u32 uVar2;
  u32 uVar7;
  u32 uVar1;
  int iVar3;
  u16 uVar9;
  u16 sVar10;
  int iVar5;
  u32 uVar6;
  u32 uVar0;
  u16 uVar8;
  u8 cVar11;

  uVar0 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184((void*)uVar0);
  uVar1 = (int)fn_8012640C(uVar0,0,0xd9,0);
  iVar3 = fn_8011BEB4(uVar1,0,0x2d,0);
  uVar4 = fn_801F025C(0x12,0);
  uVar9 = fn_802040E8();
  sVar10 = fn_80203FE4(uVar4);
  iVar5 = fn_80203EDC(uVar4);
  uVar6 = fn_80205B8C(uVar4);
  uVar7 = (int)fn_8012640C(uVar6,0,0x83,0) & 0xffff;
  if (r4 == 1) {
    uVar8 = fn_800E0C54();
    uVar8 = (100 - uVar8 % 16) & 0xffff;
    if (iVar3 != 0) {
      iVar3 *= uVar8;
      iVar3 /= 100;
      if (iVar3 == 0) {
        iVar3 = 1;
      }
      fn_8011BBD8(uVar1,0,0x2d,0,iVar3);
    }
  }
  if ((sVar10 == 0x27) && (uVar8 = fn_800E0C54(), (int)(uVar8 % 100) < iVar5)) {
    fn_801254B4((void*)uVar4,0,0x11a,0,1);
  }
  cVar11 = fn_802026E4(uVar4,0x14);
  if (cVar11 == 0) {
    sVar10 = fn_8011BEB4(0,uVar2,9,0);
    if ((((sVar10 != 0x65) || (r3 != 1)) &&
        (cVar11 = fn_802026E4(uVar4,0x2c), cVar11 == 0)) &&
       (iVar5 = (int)fn_8012640C(uVar4,0,0x11a,0), iVar5 == 0)) {
      return;
    }
    uVar2 = fn_80205B8C(uVar4);
    iVar5 = (int)fn_8012640C(uVar2,0,0x83,0);
    if (iVar5 <= iVar3) {
      fn_8011BBD8(uVar1,0,0x2d,0,(uVar7 & 0xffff) - 1);
      cVar11 = fn_802026E4(uVar4,0x2c);
      if (cVar11 != 0) {
        cVar11 = fn_8020981C(uVar1,0x46);
        if (cVar11 == 2) {
          fn_802097C8(uVar1,0x46,0);
        }
        return;
      }
      iVar3 = (int)fn_8012640C(uVar4,0,0x11a,0);
      if (iVar3 != 0) {
        cVar11 = fn_8020981C(uVar1,0x47);
        if (cVar11 == 2) {
          fn_802097C8(uVar1,0x47,0);
        }
        fn_801F4C14(0,0,0x56,0,uVar9);
      }
    }
    return;
  }
  return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80227490(void) { fn_802274F0(0, 1, 1, 0); }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_TYPE_CHECK(void) { fn_802274F0(1, 1, 1, 0); }
