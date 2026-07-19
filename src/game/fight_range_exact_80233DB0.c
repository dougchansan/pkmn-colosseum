/**
 * @file fight_range_exact_80233DB0.c
 * @brief Strict target-order fight island, 0x80233DB0 - 0x80234A0C.
 *
 * Bodies are mechanically extracted from the shared pure-C candidate.
 * Compiler state and active symbol aliases are re-established per body.
 */
#include "dolphin/types.h"

typedef struct FightSeqOpU8Operand {
    u8 opcode;
    u8 operand;
} FightSeqOpU8Operand;
typedef struct { u16 v[11]; } SpecialMoveList22;
typedef struct { u16 a, b; } U16Pair;

extern f32 lbl_8047E630;
extern u8 lbl_80378724[];
extern u8 lbl_80378B30[];
extern u8 lbl_80378B5B[];
extern u8 lbl_80378A5F[];
extern u8 lbl_80378968[];
extern u8 lbl_80378A4D[];
extern u8 lbl_80378A7C[];
extern u8 lbl_80378A8E[];
extern u32 fn_80232024();
extern void fn_80234A0C();
extern u32 fightOutPokemonGetPokemonPtr();
extern u16 fn_800E0C54(void);
extern u8* fn_801440A0(u16 idx);
extern u8* itemDataBiosGetPtr(u16 idx);
extern u8   fn_80143DFC(u8* p);
extern u8   itemDataBiosGetItemEffectParam(u8* p);
extern u8*  fn_80143A94(u8 idx);
extern u8*  itemParamGetPtr(u8 idx);
extern u8   fn_801437E0(u8* p);
extern u8   itemParamGetHPUp(u8* p);
extern u8   fn_80143940(u8* p);
extern u8   itemParamGetSleepFlag(u8* p);
extern u8   fn_80143918(u8* p);
extern u8   itemParamGetPoisonFlag(u8* p);
extern u8   fn_801438F0(u8* p);
extern u8   itemParamGetBurnFlag(u8* p);
extern u8   fn_801438C8(u8* p);
extern u8   itemParamGetFreezeFlag(u8* p);
extern u8   fn_801438A0(u8* p);
extern u8   itemParamGetParalyzeFlag(u8* p);
extern u8   fn_80143878(u8* p);
extern u8   itemParamGetConfuseFlag(u8* p);
extern u8   fn_80143A44(u8* p);
extern u8   itemParamGetCriticalFlag(u8* p);
extern u8   fn_80143A28(u8* p);
extern u8   itemParamGetAttackUp(u8* p);
extern u8   fn_80143A0C(u8* p);
extern u8   itemParamGetDefenceUp(u8* p);
extern u8   fn_801439F0(u8* p);
extern u8   itemParamGetQuickUp(u8* p);
extern u8   fn_801439D4(u8* p);
extern u8   itemParamGetHitUp(u8* p);
extern u8   fn_801439B8(u8* p);
extern u8   itemParamGetSpAttackUp(u8* p);
extern u8   fn_80143990(u8* p);
extern u8   itemParamGetGuardFlag(u8* p);
extern u8* lbl_8047B610;
extern u8  lbl_8047B614;
extern u8  lbl_8047B626;
extern void fn_80207448(void* p);
extern void fightOutPokemonInitOneSelfTurn(void* p);
extern void fn_802249B8();
extern void fn_802271E0(char, char);
extern void fn_802274F0(u32, char, char, char);
extern void* fn_801F025C();
extern u8 fn_802624CC();
extern u8 lbl_80478D78[1];
extern void fn_801F37B0();
extern u32 fn_8022E1F8();
extern s32 _fightSeqWsKuroikiriSub__FPvUsPv(void*, u16, void*);
extern u8  fn_80136468();
extern u8  fn_802025B8();
extern u32 fn_80214CFC();
extern int  fn_801F000C();
extern int  fightMainWaitFrame();
extern void fn_8026246C();
extern void fightMenuCloseMsg();
extern void fn_801F2598();
extern u8 lbl_80379F58[];
extern void fn_80201600();
extern void* lbl_8047B64C;
extern u8    lbl_80478278[0x10];
extern void  fn_801DA36C(void* obj, u32 val);
extern void  fn_80209484(void* ctx, u32 param);
extern u16   fn_8020147C(void* context, u16 moveId, u8 slot, u8 updateFlag);
extern void  fn_801FE468(void* context, u8* dest);
extern void  fightMenuSubMenuLvupStatus(void* a, void* b, void* c);
extern void  fightMenuOpenLevelUpStatusMenu(void* a, u32 flag);
extern void  fn_802622E4(void);
extern u32 fn_80211A78();
extern void fn_801DA7AC();
extern u8   lbl_8047B625;
extern void fn_8020248C();
extern void* lbl_8047B62C;
extern void  fn_80211B94(void* a, void* b, u8 c);
extern u8    fn_80207AE0(void* obj, u8 v);
extern void* fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(u32 a, u8 b, u32 c, u32 d, u32 e);
extern void  fn_801254B4(void* a, u32 b, u32 c, u32 d, u32 e);
extern u32  fn_80239984();
extern u32  fn_80205B8C(u32);
extern u8 fn_80239EE8();
extern u8 fn_80239CCC();
extern u16  fn_802377E8(void*, u32);
extern u16  fn_801F1990(u32, void*, u32, u32, u32, u32);
extern u16  fn_801F1C18(u32, void*, void*, u32, u32);
extern u8   fn_801F4C14(u32, u16, u32, u16, u32);
extern u32  fn_802096E8(void*);
extern u32  fn_80077AF4(void);
extern u8    fn_8021B910();
extern u8    lbl_80279F7C[12];
extern void  fn_802653FC(void* ptr, u16 a, s32 b);
extern u8    fn_80203E0C(void* ctx);
extern void  fn_8011BBD8();
extern u32   fn_80203ADC(void* ctx, u32 param);
extern u8    fn_801F2988(u32 param1, u32 param2);
extern void  fn_801F2934(u32 param1, u32 param2, u32 param3);
extern void* fn_801F0134();
extern u8    fn_801F6E44(u32 param1, u32 param2);
extern void  fn_801F6DF0(u32 param1, u32 param2, u32 param3);
extern u32   fn_80203B5C(void* ctx, u32 param);
extern u32   fn_80205184(void* ctx);
extern u8    fn_80229934(u32 param2, u32 param1, u32 param3);
extern u16  fn_80205224();
extern void fn_80202810();
extern u8   fn_802062FC();
extern u8   lbl_80378721[];
extern u8   lbl_80379249[];
extern u32 lbl_8047B618;
extern u8   fn_80203CCC();
extern u8   fn_801F221C();
extern void fn_80120B00();
extern void statusSetStatus();
extern u32  statusGetStatus();
extern void fn_801252E0();
extern void fn_80202998();
extern u8   fn_80204A10();
extern void* fn_802037DC(void* ctx);
extern void fn_801EF8F4();
extern void fightMenuAllFightTrainerCloseStatusMenu();
extern void fightMenuAllFightOutPokemonCloseStatusMenu();
extern u8   lbl_8037889D[0x23];
extern void fn_80132A38();
extern void fn_80165668(s32, s32, s32);
extern void fn_80166A50(s32, s32, s32, s32);
extern void fn_801F22D8(u32 obj);
extern u32  fn_80262308(void);
extern u8   lbl_8047B642;
extern u32  fn_802036D4();
extern u32  fn_801F4354();
extern void fn_801FCEC4();
extern u8   fn_801F2020();
extern u32  fn_801F8A18();
extern void _threadSwitch(void);
extern u32  fn_800FA280();
extern u32  fn_80203848();
extern u16  lbl_8047B61C;
extern s32  fn_801FEF74();
extern void fn_802086B0();
extern void fn_8020F108();
extern s8   fn_801DA5C4();
extern u8   lbl_80379A22[0x14];
extern u32  fn_8020912C();
extern int  fn_802656AC();
extern s32  fn_80102620();
extern u32  fn_80011C78();
extern u8   fn_802038A4();
extern void fn_80207C24();
extern void fn_80201764();
extern s32  fn_80232110();
extern u32 fn_8022E34C();
extern u8   fn_801F453C();
extern u8   lbl_8047B628;
extern u8   lbl_80379945[];
extern u16  fn_802040E8();
extern u16  fn_80203FE4();
extern void fn_80203EDC();
extern void fn_801FAA58();
extern u16  lbl_80279FD0[8];
extern u8   lbl_80379B06[];
extern u32 fn_80232FE4();
extern void fn_8020A2B8();
extern u8   fn_80119DD0();
extern u16  fightFloorGetValidFightOutPokemonCount(u32, u8, u32, u8);
extern u8   fn_802016A4();
extern u8   lbl_8037984D[27];
extern u8   lbl_80375FDF[9];
extern void heroAddPokedoru(u32, s32);
extern u8   lbl_8037939C[9];
extern u8  fn_801DDD28();
extern void fn_801DA9E8();
extern u16 lbl_8047B60C;
extern int fn_80215008();
extern u32 fn_8022B2CC();
extern void fn_801F6EEC();
extern void fightSideInitJoutaiDataId();
extern void fn_801FBC20();
extern void fn_80208C18();
extern void fn_80205AD4();
extern void fn_80205A7C();
extern void fn_80206C94();
extern SpecialMoveList22 lbl_8027A408;
extern U16Pair lbl_8047E628;
extern U16Pair lbl_8047E62C;
extern u8  fightTrainerIsAllyFightTargetPtr(u32 ctx, u32 poke, u16 floorVal);
extern u16 fightOutPokemonGetZokuseiDataId(u32 poke, u8 idx);
extern u16 fightOutPokemonGetTokuseiDataId(u32 poke);
extern u32 fn_8010C650(u16 waza, u16* types, u16 typeCount);
extern int fn_802026E4();
extern u8 fn_80235714();
extern u32 fn_802367CC();
extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
extern u32 fn_80239984();
extern s32 fightTrainerAiAddValue();
extern u32* lbl_80478DF8;
extern int wazaGetStatus();
extern int pokemonGetStatus();
extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
extern s32 fightTrainerAiWazaValueJisin();
extern s32 fightTrainerAiWazaValueJibaku();
extern s32 fightTrainerAiWazaValueNull();


/* fn_80233DB0 */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
#define fn_801233F4 pokemonCheckFightOut
#define fn_80123FBC pokemonCheckValid
#define fn_8012640C pokemonGetStatus
#define fn_8012A5B0 heroGetStatus
#define fn_801F4804 fightFloorGetFightPokemonEntryCntInc
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80206AEC fightPokemonCreate
u32 fn_80233DB0(u32 r3, u32 r4, int r5, u16 r6, char r7, int r8)
{
    extern u8 fn_801233F4();
    extern u8 fn_80123FBC();
    extern int fn_8012A5B0();
    extern u32 fn_801F4804();
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
    extern void fn_80206AEC();
  struct cpy85 { u32 d[85]; };
  u32 uVar1;
  u32 *puVar2;
  u32 *puVar3;
  u32 uVar4;
  u16 uVar9;
  struct cpy85 *iVar7;
  u32 uVar15;
  u32 uVar6;
  int iVar16;
  struct cpy85 *iVar5;
  u8 cVar11;
  int iVar8;
  u16 uVar10;
  u32 uVar13;
  u32 *puVar12;
  u32 *puVar14;
  struct cpy85 uStack_198;
  struct cpy85 uStack_2ec;

  uVar9 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar9,2,0);
  for (uVar13 = 0; (u16)uVar13 < 6; uVar13 = uVar13 + 1) {
    *(u32 *)(r8 + (u32)(u16)uVar13 * 4) = 0;
  }
  iVar5 = (struct cpy85 *)fn_801FB1C0(r3,0,0x45,0);
  if (iVar5 == 0) {
    return 0;
  }
  else {
    iVar7 = (struct cpy85 *)fn_801FB1C0(r3,0,0x45,1);
    if (iVar7 == 0) {
      return 0;
    }
    else {
      uStack_198 = *iVar5;
      uStack_2ec = *iVar7;
      for (uVar13 = 0; (u16)uVar13 < r6; uVar13 = uVar13 + 1) {
      }
      uVar6 = 0;
      for (uVar15 = 0; (uVar15 & 0xffff) < 6; uVar15 = uVar15 + 1) {
        iVar16 = fn_8012A5B0(r4,3,uVar15);
        if (((iVar16 != 0) && (cVar11 = fn_80123FBC(), cVar11 != 0)) &&
           (cVar11 = fn_801233F4(iVar16), cVar11 != 0)) {
          uVar13 = 0;
          goto _wcond;
_wbody:
          iVar8 = *(int *)(r5 + (u32)(u16)uVar13 * 4);
          if ((u32)iVar8 == 0) goto _wincr;
          if ((u32)iVar8 == (u32)iVar16) goto _wafter;
_wincr:
          uVar13 = uVar13 + 1;
_wcond:
          if ((u16)uVar13 < r6) goto _wbody;
_wafter:
          if ((u16)uVar13 < r6) goto LAB_0023124c;
          {
            fn_80206AEC(iVar5,iVar16,fn_801F4804(0));
            if (r7 != 0) {
              if (r7 == 1) {
                fn_801F54A4(0,0,0x14,0);
                uVar10 = fn_801FB1C0(r3,0,0x43,0);
                uVar10 = fn_801FB1C0(0,uVar10,2,0);
                uVar4 = fn_80205BE8(iVar5);
                uVar9 = (int)fn_8012640C(uVar4,0,0xc9,0);
                cVar11 = fn_801FB1C0(0,uVar10,0x23,0);
                if (cVar11 == 1) {
                  cVar11 = fn_801FB1C0(0,uVar9,0x1c,0);
                }
                else {
                  cVar11 = 0;
                }
                if (cVar11 != 2) goto LAB_0023124c;
              }
              else if (r7 == 2) {
                fn_801F54A4(0,0,0x14,0);
                uVar9 = fn_801FB1C0(r3,0,0x43,0);
                uVar9 = fn_801FB1C0(0,uVar9,2,0);
                uVar4 = fn_80205BE8(iVar5);
                uVar10 = (int)fn_8012640C(uVar4,0,0xc9,0);
                cVar11 = fn_801FB1C0(0,uVar9,0x23,0);
                if (cVar11 == 1) {
                  cVar11 = fn_801FB1C0(0,uVar10,0x1c,0);
                }
                else {
                  cVar11 = 0;
                }
                if (cVar11 != 3) goto LAB_0023124c;
              }
              else if (r7 == 3) {
                fn_801F54A4(0,0,0x14,0);
                uVar9 = fn_801FB1C0(r3,0,0x43,0);
                uVar9 = fn_801FB1C0(0,uVar9,2,0);
                uVar4 = fn_80205BE8(iVar5);
                uVar10 = (int)fn_8012640C(uVar4,0,0xc9,0);
                cVar11 = fn_801FB1C0(0,uVar9,0x23,0);
                if (cVar11 == 1) {
                  cVar11 = fn_801FB1C0(0,uVar10,0x1c,0);
                }
                else {
                  cVar11 = 0;
                }
                if (cVar11 != 2) {
                  fn_801F54A4(0,0,0x14,0);
                  uVar9 = fn_801FB1C0(r3,0,0x43,0);
                  uVar9 = fn_801FB1C0(0,uVar9,2,0);
                  uVar4 = fn_80205BE8(iVar5);
                  uVar10 = (int)fn_8012640C(uVar4,0,0xc9,0);
                  cVar11 = fn_801FB1C0(0,uVar9,0x23,0);
                  if (cVar11 == 1) {
                    cVar11 = fn_801FB1C0(0,uVar10,0x1c,0);
                  }
                  else {
                    cVar11 = 0;
                  }
                  if (cVar11 != 3) goto LAB_00231240;
                }
                goto LAB_0023124c;
              }
            }
LAB_00231240:
            uVar1 = uVar6 & 0xffff;
            uVar6 = uVar6 + 1;
            *(int *)(r8 + uVar1 * 4) = iVar16;
          }
        }
LAB_0023124c: (void)0;
      }
      *iVar5 = uStack_198;
      *iVar7 = uStack_2ec;
    }
  }
  return uVar6;
}
#undef fn_80206AEC
#undef fn_80205BE8
#undef fn_801FB1C0
#undef fn_801F54A4
#undef fn_801F4804
#undef fn_8012A5B0
#undef fn_8012640C
#undef fn_80123FBC
#undef fn_801233F4

/* fn_802342CC */
#pragma optimization_level reset
#pragma optimize_for_size on
#pragma scheduling reset
#pragma dont_inline reset
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma switch_tables reset
void fn_802342CC(u32 trainer, u32 fightType) {
    extern u8 fightTypeDataBiosGetFightoutPokemonNum();
    extern u8 fightOutPokemonCheckFightActionSelect();
    extern u8 fn_80008164();
    extern u8 fightMenuOpenMsg();
    extern u8 fightFloorCheckFightActionFightOutPokemonIrekaeSelect();
    extern u8 fightFloorGetStatus();
    extern u8 fightOutPokemonCheckFightActionWazaSelect();
    u32 noAction[8];
    u16 keyFinal[14];
    u16 keyOk1[14];
    u16 keyFail1[14];
    u16 keyOk2[14];
    u16 keyFail2[14];
    u16 keyOk3[14];
    u16 keyFail3[14];
    u32 fightTypeData;
    u32 pokemon;
    u32 i;
    u16 j;
    u32 count;
    u16 species;
    u8 selected;
    u16 selectedCount;

    fightTypeData = fightTypeDataBiosGetPtr(fightType);
    count = (u8)fightTypeDataBiosGetFightoutPokemonNum(fightTypeData);
    fn_80234A0C(trainer);

    for (i = 0; (u16)i < count; i++) {
        pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, i);
        if (pokemon != 0 && fightOutPokemonCheckFightActionSelect(pokemon, 1) != 0) {
            fightOutPokemonInitFightActionBuff(pokemon);
        }
    }

    species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    species = (u16)fightTrainerGetStatus(0, species, 2, 0);
    j = (u8)fightTrainerGetStatus(0, species, 0x26, 0);
    if ((s32)j > (fn_800E0C54() % 100)) {
        msgctrlSetValue(0xd, GSmsgGetGSchar(0xec04));
        if (trainer != 0) {
            msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
        }
        msgctrlSetValue(0x2f, j);
        if (fn_80008164() == 1) {
            menuGetKeyInfo(keyOk1, 1);
            if ((keyOk1[0] & 0x800) == 0 && fightMenuOpenMsg(0xec67) == 1) {
                fightMenuCloseMsg();
            }
        }
        selected = 1;
    } else {
        msgctrlSetValue(0xd, GSmsgGetGSchar(0xec04));
        if (trainer != 0) {
            msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
        }
        msgctrlSetValue(0x2f, j);
        if (fn_80008164() == 1) {
            menuGetKeyInfo(keyFail1, 1);
            if ((keyFail1[0] & 0x800) == 0 && fightMenuOpenMsg(0xec68) == 1) {
                fightMenuCloseMsg();
            }
        }
        selected = 0;
    }

    if (selected == 1) {
        for (i = 0; (u16)i < count; i++) {
            pokemon = fightTrainerGetNoActionFightOutPokemonPtr(trainer, i);
            if (pokemon != 0 && fightFloorCheckFightActionFightOutPokemonIrekaeSelect(0, pokemon, 0) == 0) {
                fightTrainerAiSelectFightActionIrekae(trainer, pokemon, fightType);
            }
        }
    }

    if (fightFloorGetStatus(0, 0, 0x20, 0) == 1) {
        species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
        species = (u16)fightTrainerGetStatus(0, species, 2, 0);
        j = (u8)fightTrainerGetStatus(0, species, 0x27, 0);
        if ((s32)j > (fn_800E0C54() % 100)) {
            msgctrlSetValue(0xd, GSmsgGetGSchar(0xec46));
            if (trainer != 0) {
                msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
            }
            msgctrlSetValue(0x2f, j);
            if (fn_80008164() == 1) {
                menuGetKeyInfo(keyOk2, 1);
                if ((keyOk2[0] & 0x800) == 0 && fightMenuOpenMsg(0xec67) == 1) {
                    fightMenuCloseMsg();
                }
            }
            selected = 1;
        } else {
            msgctrlSetValue(0xd, GSmsgGetGSchar(0xec46));
            if (trainer != 0) {
                msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
            }
            msgctrlSetValue(0x2f, j);
            if (fn_80008164() == 1) {
                menuGetKeyInfo(keyFail2, 1);
                if ((keyFail2[0] & 0x800) == 0 && fightMenuOpenMsg(0xec68) == 1) {
                    fightMenuCloseMsg();
                }
            }
            selected = 0;
        }

        if (selected == 1) {
            for (i = 0; (u16)i < count; i++) {
                pokemon = fightTrainerGetNoActionFightOutPokemonPtr(trainer, i);
                if (pokemon != 0) {
                    fightTrainerAiSelectFightActionItem(trainer, pokemon, fightType);
                }
            }
        }
    }

    species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    species = (u16)fightTrainerGetStatus(0, species, 2, 0);
    j = (u8)fightTrainerGetStatus(0, species, 0x25, 0);
    if ((s32)j > (fn_800E0C54() % 100)) {
        msgctrlSetValue(0xd, GSmsgGetGSchar(0xec47));
        if (trainer != 0) {
            msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
        }
        msgctrlSetValue(0x2f, j);
        if (fn_80008164() == 1) {
            menuGetKeyInfo(keyOk3, 1);
            if ((keyOk3[0] & 0x800) == 0 && fightMenuOpenMsg(0xec67) == 1) {
                fightMenuCloseMsg();
            }
        }
        selected = 1;
    } else {
        msgctrlSetValue(0xd, GSmsgGetGSchar(0xec47));
        if (trainer != 0) {
            msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
        }
        msgctrlSetValue(0x2f, j);
        if (fn_80008164() == 1) {
            menuGetKeyInfo(keyFail3, 1);
            if ((keyFail3[0] & 0x800) == 0 && fightMenuOpenMsg(0xec68) == 1) {
                fightMenuCloseMsg();
            }
        }
        selected = 0;
    }

    if (selected == 1) {
        for (i = 0; (u16)i < count; i++) {
            pokemon = fightTrainerGetNoActionFightOutPokemonPtr(trainer, i);
            if (pokemon != 0) {
                fn_8024E534(trainer, pokemon, fightType);
            }
        }
    }

    msgctrlSetValue(0xd, GSmsgGetGSchar(0xec2c));
    if (trainer != 0) {
        msgctrlSetValue(0x13, fightTrainerGetNamePtr(trainer));
    }
    msgctrlSetValue(0x2f, 100);
    if (fn_80008164() == 1) {
        menuGetKeyInfo(keyFinal, 1);
        if ((keyFinal[0] & 0x800) == 0 && fightMenuOpenMsg(0xec67) == 1) {
            fightMenuCloseMsg();
        }
    }

    for (j = 0; j < 8; j++) {
        noAction[j] = 0;
    }

    selectedCount = 0;
    for (i = 0; (u16)i < count; i++) {
        pokemon = fightTrainerGetNoActionFightOutPokemonPtr(trainer, i);
        if (pokemon != 0 && fightOutPokemonCheckFightActionWazaSelect(pokemon, 1) == 0) {
            noAction[selectedCount] = pokemon;
            selectedCount++;
        }
    }

    if (selectedCount != 0) {
        fightFloorSortFightOutPokemonPtrArySub(0, noAction, 8, 0);
        for (count = 0; (u16)count < selectedCount; count++) {
            if (noAction[(u16)count] != 0) {
                fn_8023A308(trainer, noAction[(u16)count], fightType);
            }
        }
    }
}
