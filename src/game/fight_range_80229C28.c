/**
 * @file fight_range_80229C28.c
 * @brief Hit check, 0x80229C28 - 0x8022A504 (fightIsWazaBlockedByMamoru, wsCmd40,
 *        WS_HITCHECK).
 *
 * Standalone source at GC/1.3.2 -O4,s. WS_HITCHECK's register walls were two
 * inline blocks: the defender-ability block (Soundproof, ability 0x2b,
 * against a sound move), fightSeqCheckTokuseiBlock, and the move copy after
 * the field loop, fightSeqCopyWaza. Its checkValue read takes the move slot
 * as the fourth argument, and the miss path sets the flag before the PC.
 */
#include "dolphin/types.h"
extern u8* lbl_8047B610;
extern u8 lbl_80478D78[8];
#define fn_8011BEB4 wazaGetStatus
u8 fightIsWazaBlockedByMamoru(void* ctx, u32 param) {
    extern u32 fn_8011BEB4();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    u8 val = (u8)fn_8011BEB4(0, param, 0xe, 0);

    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(ctx, 0x2b) == 1 && val == 1) {
        return 1;
    }
    return 0;
}

void wsCmd40(void)
{
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern s32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 fightWazaIsHit();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u8 fightCheckTokuseiWazaKyuushuu();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 initialFlag;
  u8 cVar6;
  u8 cVar7;
  /* RULE-EXCEPTION(user-approved): one-member struct locals used only for
   * register colouring -- see docs/RULE_EXCEPTIONS.md. */
  struct {
    u32 value;
  } uVar2;
  struct {
    u32 value;
  } uVar3;
  u32 uVar4;
  u32 uVar5;

  uVar1 = fightOutPokemonGetUseWazaDataId(fightTargetGetPtrAsNowFightType(0x11,0));
  initialFlag = wazaGetStatus(0,uVar1,0xe,0) & 0xff;
  uVar1 = fightTargetGetPtrAsNowFightType(0x12,0);
  cVar7 = fightOutPokemonIsPokemonJoutaiDataId(uVar1,0x2b);
  if ((cVar7 == 1) && (initialFlag == 1)) {
    fightFloorSetStatus(0,0,0x3b,0,0x40);
    uVar1 = fightTargetGetPtrAsNowFightType(0x11,0);
    uVar2.value = (int)pokemonGetStatus(uVar1,0,0xd9,0);
    uVar3.value = fightTargetGetPtrAsNowFightType(0x12,0);
    cVar6 = fightWazaIsHit(uVar2.value);
    if (cVar6 == 0) {
      pokemonSetStatus((void*)uVar3.value,0,0xf3,0,0);
      pokemonSetStatus((void*)uVar3.value,0,0xf4,0,9);
      lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar2.value = fightTargetGetPtrAsNowFightType(0x11,0);
      uVar4 = fightTargetGetPtrAsNowFightType(2,uVar2.value);
      uVar2.value = fightTargetGetPtrAsNowFightType(0x12,0);
      uVar5 = fightTargetGetPtrAsNowFightType(2,uVar2.value);
      cVar6 = fightOutPokemonIsPokemonJoutaiDataId(uVar2.value,0x15);
      if (cVar6 == 1) {
        if (uVar4 != uVar5) {
          if ((lbl_8047B618 & 0x1000000) == 0) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
        }
      }
      cVar6 = fightCheckTokuseiWazaKyuushuu(uVar1,uVar3.value,0);
      if (cVar6 == 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
      }
    }
    lbl_80478D78[6] = 1;
  }
  else {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}

extern u32 fightOutPokemonGetTokuseiDataId();
extern u32 fightOutPokemonGetUseWazaDataId();
extern u8 fightOutPokemonCheckFightOut();
extern u32 wazaGetStatus();
extern u8 fightOutPokemonIsPokemonJoutaiDataId();
extern u32 lbl_8047B618;
extern u8 lbl_8037989E[];
/* RULE-EXCEPTION(user-approved): single-use inline helper, register-only
 * evidence -- see docs/RULE_EXCEPTIONS.md. */
static inline u8 fightSeqCheckTokuseiBlock(u32 attacker, u32 defender)
{
    u32 move;
    u16 ability;
    u8 blocked;

    fightOutPokemonGetTokuseiDataId(attacker);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    ability = fightOutPokemonGetTokuseiDataId(defender);
    blocked = 0;
    switch (ability) {
    case 0x2b:
        if (fightOutPokemonCheckFightOut(defender) != 0 && (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
            if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x22) == 1) {
                lbl_8047B618 |= 0x800;
            }
            blocked = 1;
            lbl_8047B610 = lbl_8037989E;
        }
        break;
    }
    return blocked;
}
extern s32 pokemonGetStatus();
extern void fightWazaBiosCopy();
extern void wazaSetStatus();
/* RULE-EXCEPTION(user-approved): single-use inline helper, register-only
 * evidence -- see docs/RULE_EXCEPTIONS.md. */
static inline void fightSeqCopyWaza(u32 attacker, u32 other)
{
    u32 mine;
    u32 theirs;
    u16 last;

    mine = pokemonGetStatus(attacker, 0, 0xd9, 0);
    theirs = pokemonGetStatus(other, 0, 0xd9, 0);
    if (theirs != 0 && mine != 0) {
        last = wazaGetStatus(theirs, 0, 0x28, 0);
        fightWazaBiosCopy(theirs, mine);
        wazaSetStatus(theirs, 0, 0x27, 0, last);
    }
}
void WS_HITCHECK(void)
{
    struct HitCheckLoopState {
      u32 attacker;
      u32 result;
    };
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern s8 pokemonSearchWazaDataId();
    extern u16 std103_battleGetResult();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightFloorSetStatus();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonInitJoutaiKeep();
    extern u32 fightOutPokemonGetPokemonJoutai08();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightWazaWriteJoutaiDataId();
    extern u8 fightWazaCheckWriteJoutaiDataId();
    extern void fightWazaBiosCopy();
    extern void wsRunScript();
    extern u32 _fightCheckYokodoriSub();
    extern u32 _wsHitCheckSub();
    extern u8 _wsHitCheckJoutaiSub();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_80375FBF[];
    extern u8 lbl_8037989E[];
    extern u8 lbl_80379021[];
    extern u8 lbl_8037917B[];
    extern u8 lbl_803796F3[];
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
  u32 bVar1;
  u32 iVar2;
  u32 iVar4;
  u32 uVar3;
  u16 floorState;
  u8 cVar12;
  u32 iVar6;
  u32 uVar7;
  u8 moveFlag;
  u8 cVar13;
  s8 cVar14;
  u32 uVar8;
  s8 bVar15;
  u32 iVar9;
  u16 attackerAbility;
  u16 moveEffect;
  u8 checkValue;
  u8 specialResult;
  s32 hitResult;
  u8 moveSlot;
  s32 status114;

  struct HitCheckLoopState loopState;

  iVar2 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar3 = fightOutPokemonGetPokemonPtr();
  iVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
  floorState = std103_battleGetResult();
  if (floorState != 0) {
    lbl_8047B614 = 2;
  }
  else {
    cVar12 = fightOutPokemonCheckFightOut(iVar2);
    if ((cVar12 == 0) && ((lbl_8047B618 & 0x200) == 0)) {
      lbl_8047B618 = lbl_8047B618 | 0x80000;
      lbl_8047B610 = lbl_80375FBF;
    }
    else {
      cVar12 = _wsHitCheckJoutaiSub();
      if (cVar12 == 0) {
        if (fightSeqCheckTokuseiBlock(iVar2, iVar4) == 0) {
          iVar6 = pokemonGetStatus(iVar2,0,0xd9,0);
          uVar7 = fightOutPokemonGetUseWazaDataId((void*)iVar2);
          moveFlag = wazaGetStatus(0,uVar7,0xe,0);
          cVar13 = wazaGetStatus(0,uVar7,0xf,0);
          cVar14 = wazaGetStatus(iVar6,0,0x26,0);
          if (((cVar14 < 0) || ((uVar7 & 0xffff) == 0xa5)) || ((uVar7 & 0xffff) == 0x164)) {
            checkValue = 0;
          }
          else {
            checkValue = pokemonGetStatus(uVar3,0,0x80,cVar14);
          }
          if (checkValue != 0 || (uVar7 & 0xffff) == 0xa5 ||
              (uVar7 & 0xffff) == 0x164) {
            goto normal_hit;
          }
          if (fightOutPokemonIsPokemonJoutaiDataId(iVar2,0x22) != 0 || (lbl_8047B618 & 0x800200) != 0) {
            goto normal_hit;
          }
          {
            specialResult = fightWazaCheckWriteJoutaiDataId(iVar6,0x40);
            if (specialResult == 2) {
              fightWazaWriteJoutaiDataId(iVar6,0x40,0);
            }
            lbl_8047B618 = lbl_8047B618 | 0x80000;
            lbl_8047B610 = lbl_80379021;
            return;
          }
        normal_hit:
          {
            uVar8 = lbl_8047B618;
            lbl_8047B618 = uVar8 & 0xff7fffff;
            if ((uVar8 & 0x2000000) == 0) {
              if (fightOutPokemonIsPokemonJoutaiDataId(iVar2,0x22) == 0) {
                hitResult = (u8)_wsHitCheckSub(iVar2);
                if (hitResult != 0) {
                  if (hitResult == 2) {
                    lbl_8047B618 = lbl_8047B618 | 0x2000000;
                    return;
                  }
                  specialResult = fightWazaCheckWriteJoutaiDataId(iVar6,0x40);
                  if (specialResult != 2) {
                    return;
                  }
                  fightWazaWriteJoutaiDataId(iVar6,0x40,0);
                  return;
                }
              }
            }
            lbl_8047B618 = lbl_8047B618 | 0x2000000;
            specialResult = fightOutPokemonIsPokemonJoutaiDataId(iVar4,0x37);
            if ((specialResult == 1) && (cVar13 == 1)) {
              if (((iVar2 != 0) && (iVar4 != 0)) &&
                  (attackerAbility = fightOutPokemonGetTokuseiDataId(iVar2),
                   attackerAbility == 0x2e))
              {
                uVar3 = fightOutPokemonGetPokemonPtr(iVar4);
                bVar15 = pokemonSearchWazaDataId(uVar3,0x115);
                if (bVar15 >= 0) {
                  s32 slot = bVar15;
                  checkValue = pokemonGetStatus(uVar3,0,0x80,slot);
                  if (checkValue != 0) {
                    checkValue = checkValue - 1;
                  }
                  pokemonSetStatus((void*)uVar3,0,0x80,slot,checkValue);
                  moveSlot = (u8)bVar15;
                  if (fightOutPokemonIsPokemonJoutaiDataId(iVar4,0x10) != 0) {
                    goto after_hensin_update;
                  }
                  if (fightOutPokemonIsPokemonJoutaiDataId(iVar4,0x31) != 1) {
                    goto after_hensin_update;
                  }
                  uVar8 = fightOutPokemonGetPokemonJoutai08(iVar4,0x31);
                  if ((uVar8 & 1 << moveSlot) != 0) {
                    goto after_hensin_update;
                  }
                  if (fightOutPokemonIsUseHensinBuff(iVar4) != 1) {
                    goto after_hensin_update;
                  }
                  fightOutPokemonSetHensinPokemonStatusId(iVar4,0x80,moveSlot,0);
                after_hensin_update:;
                }
              }
              fightOutPokemonWriteJoutaiDataId(iVar4,0x37);
              iVar9 = pokemonGetStatus(iVar4,0,0xd9,0);
              wsRunScript(lbl_8047B62C,lbl_8037917B,0);
              if ((iVar6 != 0) && (iVar9 != 0)) {
                fightWazaBiosCopy(iVar9,iVar6);
                wazaSetStatus(iVar9,0,0x27,0,uVar7 & 0xffff);
              }
            }
            else {
              loopState.attacker = iVar2;
              loopState.result = 0;
              fightFloorLoopValidFightOutPokemon(0,_fightCheckYokodoriSub,&loopState,1);
              if (loopState.result != 0) {
                fightSeqCopyWaza(iVar2, loopState.result);
              }
              status114 = pokemonGetStatus(iVar4,0,0x114,0);
              if (status114 == 1) {
                pokemonSetStatus((void*)iVar4,0,0x114,0,0);
                wsRunScript(lbl_8047B62C,lbl_803796F3,0);
              }
              specialResult = fightOutPokemonIsPokemonJoutaiDataId(iVar4,0x2b);
              if ((specialResult == 1) && (moveFlag == 1)) {
                if (((uVar7 & 0xffff) == 0xae) &&
                    (specialResult = fightOutPokemonIsZokuseiDataId(iVar2,7),
                     specialResult == 0))
                {
                  lbl_8047B610 += 1;
                  return;
                }
                moveEffect = wazaGetStatus(0,uVar7,9,0);
                if (((((moveEffect == 0x91) || (moveEffect == 0x27)) ||
                       (moveEffect == 0x4b)) ||
                      ((moveEffect == 0x97 || (moveEffect == 0x9b)))) ||
                    (moveEffect == 0x1a)) {
                  bVar1 = 1;
                }
                else {
                  bVar1 = 0;
                }
                if (((u8)bVar1 == 0) ||
                    (specialResult = fightOutPokemonIsPokemonJoutaiDataId(iVar2,0x22), specialResult == 1)) {
                  fightOutPokemonInitJoutaiKeep(iVar2);
                  fightFloorSetStatus(0,0,0x3b,0,0x40);
                  pokemonSetStatus((void*)iVar4,0,0xf3,0,0);
                  pokemonSetStatus((void*)iVar4,0,0xf4,0,9);
                  lbl_80478D78[6] = 1;
                  lbl_8047B610 += 1;
                  return;
                }
              }
              lbl_8047B610 += 1;
            }
          }
        }
      }
    }
  }
  return;
}
