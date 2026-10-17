/**
 * @file fight_range_exact_80224740.c
 * @brief Exact island 0x80224740 - 0x802247D0 (wsCmd18), moved out of
 *        fight_range_80211A00.c.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_80478D78[8];
extern u32 lbl_8047B618;
extern void* lbl_8047B62C;
extern void wsRunScript(void*, void*, u8);

#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (optimize_for_size on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma optimize_for_size on
void wsCmd18(void)

{
    extern u32 fn_801F025C();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId(u32, u32);
    extern void fn_80202810();
    extern u8 lbl_8047B625;
    extern u16 lbl_80279EF4[];
  u32 uVar2;
  u32 sVar1;
  u16 loadedStatus;
  u8 cVar3;

  uVar2 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  loadedStatus = lbl_80279EF4[lbl_80478D78[3]];
  sVar1 = loadedStatus;
  if ((loadedStatus != 0) && (cVar3 = fightOutPokemonIsPokemonJoutaiDataId(uVar2, sVar1), cVar3 == 1)) {
    fn_80202810(uVar2,sVar1);
  }
  lbl_80478D78[3] = 0;
  lbl_8047B625 = 0;
  lbl_8047B610 += 2;
  return;
}
#pragma optimize_for_size reset
#undef fn_801F025C
#undef fn_80202810
