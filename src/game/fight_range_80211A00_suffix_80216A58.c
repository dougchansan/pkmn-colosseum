/**
 * @file fight_range_80211A00_suffix_80216A58.c
 * @brief Beat Up damage and neighbours, 0x80216A58 - 0x80217018
 *        (wsCmdC4, fn_80216CF8, wsCmdC3, wsCmdC2).
 *
 * Standalone source at GC/1.3 -O4,s with no pragmas. wsCmdC4 walks the
 * party with heroGetStatus(party, 3, index): the index is the third argument,
 * which is why retail loads the loop test into r5. Its locals follow the
 * declaration order found with the colouring simulator; the damage division is
 * two statements (divide by the defense, then /50 + 2).
 */
#include "dolphin/types.h"
extern u8* lbl_8047B610;
extern u8 lbl_80478D78[1];
void wsCmdC4(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightTrainerGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 heroGetStatus();
    extern u32 fightTrainerCheckTemotiPokemonFightEntry();
    extern u8 fightPokemonCheckValid();
    extern u32 fightPokemonGetPokemonPtr();
    extern u8 pokemonCheckFightOut();
    extern u8 pokemonIsJoutaiNormal();
    extern u32 figthOutPokemonGetPokemonDataId();
    extern u8 figthOutPokemonGetLevel();
    extern void msgctrlSetValue();
    extern u32 wazaGetStatus();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void wazaSetStatus();
    s32 damage;
    u32 attacker;
    u32 power;
    u32 defender;
    u32 fightPokemon;
    u32 trainer;
    u32 pokemon;
    u32 party;
    u32 move;
    u8 initialIndex;
    u16 species;
    u32 moveData;
    s32 result;
    u32 defenderPokemon;
    u8 level;
    s32 defense;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    moveData = pokemonGetStatus(attacker, 0, 0xd9, 0);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, attacker);
    party = fightTrainerGetStatus(trainer, 0, 0x44, 0);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    if (fightOutPokemonCheckFightOut(defender) == 0) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        return;
    }

    initialIndex = lbl_80478D78[0];
    pokemon = 0;
    for (; lbl_80478D78[0] < 6; lbl_80478D78[0]++) {
        pokemon = heroGetStatus(party, 3, lbl_80478D78[0]);
        fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(trainer, pokemon);
        if (fightPokemon != 0 && fightPokemonCheckValid(fightPokemon) != 0) {
            pokemon = fightPokemonGetPokemonPtr(fightPokemon);
            if (pokemonCheckFightOut(pokemon) == 1 && pokemonIsJoutaiNormal(pokemon) == 1) {
                break;
            }
        }
    }
    if (lbl_80478D78[0] < 6) {
        species = (u16)pokemonGetStatus(pokemon, 0, 0x6e, 0);
        level = (u8)pokemonGetStatus(pokemon, 0, 0x7a, 0);
        defenderPokemon = figthOutPokemonGetPokemonDataId(defender);
        figthOutPokemonGetLevel(defender);
        msgctrlSetValue(0xd, pokemonGetStatus(pokemon, 0, 0x77, 0));
        damage = (s32)pokemonGetStatus(0, species, 4, 0);
        power = wazaGetStatus(0, move, 7, 0);
        damage *= power & 0xffff;
        damage *= (((s32)level << 1) / 5) + 2;
        defense = (s32)pokemonGetStatus(0, defenderPokemon, 5, 0);
        if (defense <= 0) {
            defense = 1;
        }
        damage /= defense;
        damage = damage / 50 + 2;
        result = damage;
        if (fightOutPokemonIsPokemonJoutaiDataId(attacker, 0x32) == 1) {
            result = damage * 15 / 10;
        }
        wazaSetStatus(moveData, 0, 0x2d, 0, result);
        lbl_80478D78[0]++;
        lbl_8047B610 += 9;
        return;
    }
    if (initialIndex != 0) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        return;
    }
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 5);
}

#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
u32 fn_80216CF8(u32 p1, u32 p2, u16 p3, u32 p4, u8 p5) {
    extern u32 fn_8012640C();
    extern s32 fn_8011BEB4();
    s32 acc = fn_8012640C(0, p2, 0x4, 0);
    s32 sub = (u16)fn_8011BEB4(0, p1, 0x7, 0);
    s32 scale = (s32)((p3 << 1) / 5) + 2;
    s32 divisor;

    acc = acc * sub;
    acc = acc * scale;
    divisor = fn_8012640C(0, p4, 0x5, 0);
    if (divisor <= 0) {
        divisor = 1;
    }
    acc = acc / divisor;
    return acc / 50 + 2;
}

#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
void wsCmdC3(void)
{
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fightOutPokemonSetPokemonJoutai08();
    extern void fightOutPokemonSetPokemonJoutai0C();
    extern void fightOutPokemonWritePokemonJoutaiDataId();
    extern u8 fn_802025B8();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern u32 fn_80205184();
    extern int fightSeqCalcDamage();
  u16 uVar7;
  u32 uVar1;
  u32 uVar11;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar10;
  u16 uVar8;
  int iVar6;
  u32 uVar5;
  u8 cVar9;

  uVar7 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F0134(uVar1,uVar7);
  uVar11 = (u32)fn_8012640C(uVar1,0,0xd9,0);
  uVar4 = fn_80205184((void*)uVar1);
  uVar10 = fn_8011BEB4(uVar11,0,0x2f,0);
  uVar8 = fn_8011BEB4(uVar11,0,0x30,0);
  uVar3 = fn_801F025C(0x12,0);
  uVar5 = fn_801F025C(2,uVar3);
  cVar9 = fn_802025B8(uVar3,0x34);
  if (cVar9 != 2) {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    return;
  }
  fightOutPokemonWritePokemonJoutaiDataId(uVar3,0x34,uVar2);
  fightOutPokemonSetPokemonJoutai0C(uVar3,0x34,uVar4);
  iVar6 = fightSeqCalcDamage(uVar1,uVar3,uVar5,uVar4,uVar10,uVar8);
  cVar9 = fightOutPokemonIsPokemonJoutaiDataId(uVar1,0x32);
  if (cVar9 == 1) {
    iVar6 = iVar6 * 15 / 10;
  }
  fightOutPokemonSetPokemonJoutai08(uVar3,0x34,iVar6);
  if ((uVar4 & 0xffff) == 0x161) {
    lbl_80478D78[5] = 1;
  }
  else {
    lbl_80478D78[5] = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}

void wsCmdC2(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  u32 context;
  u32 candidate;
  u32 selected;

  context = fightTargetGetPtrAsNowFightType(0x11,0);
  lbl_8047B649 = 8;
  lbl_8047B648 = 0;
  selected = 0;
  while (lbl_8047B648 < lbl_8047B649) {
    candidate = fightFloorGetStatus(0,0,0x5d,lbl_8047B648);
    if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 &&
        context != candidate) {
      selected = candidate;
      break;
    }
    lbl_8047B648++;
  }
  if (selected != 0) {
    fightFloorSetStatus(0,0,0x43,0,selected);
  }
  lbl_8047B610++;
  return;
}
