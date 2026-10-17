/**
 * @file gs_range_80265EC4.c
 * @brief gs-engine code, 0x80265EC4 - 0x8026635C (6 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). The range name stays honest until internal
 * TU structure is proven.
 */
#include "dolphin/types.h"

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

extern void* pokemonGetStatus();
extern u32 savedataGetStatus();
extern int fadeCheck();
extern int fadeSet();
extern void _threadSwitch();
extern void msgctrlSetValue();
extern void winMsgOpenField();
extern void winMsgCloseField();
extern void fn_801DB088();
extern void* fn_801DE190(u16 index, void* model, u8 variant);
extern void fn_801DDD28();
extern void fn_801DA4E8();
extern void fn_801DA9E8();
extern u8 fn_801DA94C();
extern void floorSetFadeScript();

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80265EC4_PREFIX)
/* Address: 0x80265EC4 | Size: 0x50 */
void exribbonSetNo(int no, u8 value)
{
    u8* data = (u8*)savedataGetStatus(0, 0x10);

    if (data[no] == 0 && value != 0) {
        data[no] = value;
    }
}

/* Address: 0x80265F14 | Size: 0x38 */
u8 exribbonGetNo(int r3)
{
    int iVar1;

    iVar1 = savedataGetStatus(0, 0x10);
    return *(u8 *)(iVar1 + r3);
}

/* Address: 0x80265F4C | Size: 0x48 */
void exribbonInit(u8* data)
{
    int i;

    if (data == NULL) {
        data = (u8*)savedataGetStatus(0, 0x10);
    }

    for (i = 0; i < 11; i++) {
        *data++ = 0;
    }
}
#endif

#if !defined(GS_RANGE_80265EC4_SPLIT) || defined(GS_RANGE_80265F94_SUFFIX)
/* Address: 0x80265F94 | Size: 0x2BC | Ghidra import
 * d2present gift scene: effect on the gift Pokemon, fade in, its cry,
 * message 0x44BA then a mode-dependent message (0x44BC/0x44BB/0x44B9),
 * fade out; run by fn_80266250 after d2presentOpen stores the mode. */
void d2presentPlayGiftScene(int r3)

{
    extern s8 winMsgCheck();
    extern u16 pokemonDataBiosGetVoice();
    extern void* pokemonDataBiosGetPtr();
    extern int GSsndGetStatus();
    extern int soundStartSE();
    extern u16 lbl_8047E6F8;
    extern f32 lbl_8047E6FC;
  u16 uVar1;
  u32 uVar2;

  u16 sVar6;
  u16 sVar7;
  u32 uVar3;
  s8 cVar9;
  u8 effectStatus;
  int iVar4;
  void* pokemonData;
  u16 uVar8;
  u32 uVar5;

  sVar6 = (u16)(int)pokemonGetStatus(0,0xfa,0x66,0);
  sVar7 = 0xFFFF;
  if ((u16)sVar6 != 0) {
    sVar7 = sVar6;
  }
  uVar3 = (u32)fn_801DE190(sVar7,0,0);
  fn_801DDD28(uVar3,lbl_8047E6F8,4,0);
  fn_801DA4E8(uVar3,1);
  uVar1 = lbl_8047E6F8;
  fn_801DA9E8(uVar3,uVar1,4);
  fadeSet((double)lbl_8047E6FC,2);
  while (cVar9 = fadeCheck(0), cVar9 != '\0') {
    fn_801DB088();
    effectStatus = fn_801DA94C(uVar3,uVar1,4);
    if (effectStatus == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  pokemonData = pokemonDataBiosGetPtr(0xfa);
  if (pokemonData == 0) {
    uVar8 = 0;
  }
  else {
    uVar8 = pokemonDataBiosGetVoice();
    soundStartSE(uVar8);
  }
  while (iVar4 = GSsndGetStatus(uVar8), iVar4 == 2) {
    fn_801DB088();
    effectStatus = fn_801DA94C(uVar3,uVar1,4);
    if (effectStatus == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  winMsgOpenField(0x44ba,0,0);
  while (cVar9 = winMsgCheck(), uVar2 = __cntlzw(1 - cVar9),
         (uVar2 >> 5 & 0xff) != 0) {
    fn_801DB088();
    effectStatus = fn_801DA94C(uVar3,uVar1,4);
    if (effectStatus == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  winMsgCloseField(1);
  switch (r3) {
  case 0:
    msgctrlSetValue(0x5d,0x3d2);
    uVar5 = 0x44bc;
    break;
  case 1:
    msgctrlSetValue(0x5d,0x3d2);
    uVar5 = 0x44bb;
    break;
  default:
    uVar5 = 0x44b9;
    break;
  }
  winMsgOpenField(uVar5,0,0);
  while (cVar9 = winMsgCheck(), uVar2 = __cntlzw(1 - cVar9),
         (uVar2 >> 5 & 0xff) != 0) {
    fn_801DB088();
    effectStatus = fn_801DA94C(uVar3,uVar1,4);
    if (effectStatus == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  winMsgCloseField(1);
  fadeSet((double)lbl_8047E6FC,3);
  while (cVar9 = fadeCheck(0), cVar9 != '\0') {
    fn_801DB088();
    effectStatus = fn_801DA94C(uVar3,uVar1,4);
    if (effectStatus == '\0') {
      fn_801DA9E8(uVar3,uVar1,4);
    }
    _threadSwitch();
  }
  return;
}

/* RULE-EXCEPTION(user-approved): redundant masks preserve retail's saved-volume register moves; see docs/RULE_EXCEPTIONS.md. */
/* Address: 0x80266250 | Size: 0xD0 | Ghidra import */
void fn_80266250(void)
{
    extern int fn_800FF660();
    extern u32 lbl_8047B680;
    extern int fn_801653BC();
    extern int std228_soundGetCurrentBackgroundMusic();
    extern u32 fn_801656D8(u32 id);
    extern u32 std104_soundSetBackgroundMusic(u32 id, u32 volume, u32 priority);
    extern void scriptSoundStop();
    extern u32 std105_soundPlayEffect(u32 id, u32 volume, u32 priority);
    extern void fn_801DADC0();
    extern void wazaSequenceSysRelease();
    extern void d2presentPlayGiftScene();
    u32 uVar5;
    u8 uVar4;
    u8 uVar3;
    u32 iVar2;
    u32 iVar1;

    uVar5 = lbl_8047B680;
    iVar1 = std228_soundGetCurrentBackgroundMusic();
    if (iVar1 != 0) {
        uVar3 = fn_801656D8(iVar1) & 0xFFFFFFFFFFFFFFFFu;
        std104_soundSetBackgroundMusic(1, 0x32, 0xff);
    } else {
        uVar3 = 0;
    }
    iVar2 = fn_801653BC();
    if (iVar2 != 0) {
        uVar4 = fn_801656D8(iVar2) & 0xFFFFFFFFFFFFFFFFu;
        scriptSoundStop(0x32);
    } else {
        uVar4 = 0;
    }
    fn_801DADC0(1);
    d2presentPlayGiftScene(uVar5);
    wazaSequenceSysRelease();
    if (iVar1 != 0) {
        std104_soundSetBackgroundMusic(iVar1, 0x32, uVar3);
    }
    if (iVar2 != 0) {
        std105_soundPlayEffect(iVar2, 0x32, uVar4);
    }
    fn_800FF660();
    floorSetFadeScript(0, 0x5960008);
}

/* Address: 0x80266320 | Size: 0x3C */
void d2presentOpen(int r3)
{
    extern void std098_mapWarpToMenuRoom(u32);
    extern u32 lbl_8047B680;

    lbl_8047B680 = r3;
    std098_mapWarpToMenuRoom(0x8f);
    floorSetFadeScript((0x596 << 16) | 0x9, 0);
    _threadSwitch();
}
#endif
