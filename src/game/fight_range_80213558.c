/**
 * @file fight_range_80213558.c
 * @brief Fight turn-end joutai countdown carve, 0x80213558 - 0x802136A4
 *        (_fightSeqTurnEndSubFightOutPokemon), linked.
 *
 * Standalone copy of the shared candidate body, built at -O4,s
 * (configure.py) in place of the shared file's optimize_for_size pragma.
 */
#include "dolphin/types.h"

u32 _fightSeqTurnEndSubFightOutPokemon(u32 r3, u32 r4, char* r5)

{
    extern int fightOutPokemonGetJoutaiMigawariHp();
    extern void fn_80201FDC();
    extern u32 fn_80202108();
    extern u32 fn_80202234();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightOutPokemonInitOneTurn();
    extern u32 pokemonGetStatus();
    extern u32 pokemonSetStatus();
  u16 status;
  s32 current;
  s32 count;
  s32 next;
  int iVar1;

  if (*r5 == 1) {
    fightOutPokemonWriteJoutaiDataId(r3,0x2b);
    fightOutPokemonWriteJoutaiDataId(r3,0x2c);
  }
  else {
    fightOutPokemonInitOneTurn();
    fightOutPokemonWriteJoutaiDataId(r3,0x32);
    fightOutPokemonWriteJoutaiDataId(r3,0x37);
    fightOutPokemonWriteJoutaiDataId(r3,0x33);
    status = (u16)pokemonGetStatus(r3,0,0xed,0);
    if (status != 0) {
      pokemonSetStatus(r3,0,0xed,0,(u16)(status - 1));
    }
    if (fightOutPokemonIsPokemonJoutaiDataId(r3,0x12) == 1) {
      current = fn_80202108(r3,0x12);
      count = fn_80202234(r3,0x12);
      current = (s8)current;
      count = (s8)count;
      next = (s8)(current + 1);
      if (next >= count) {
        fightOutPokemonWriteJoutaiDataId(r3,0x12);
      }
      else {
        fn_80201FDC(r3,0x12,next);
      }
    }
  }
  if ((fightOutPokemonIsPokemonJoutaiDataId(r3,0x14) == 1) &&
      (iVar1 = fightOutPokemonGetJoutaiMigawariHp(r3), iVar1 <= 0)) {
    fightOutPokemonWriteJoutaiDataId(r3,0x14);
  }
  return 1;
}
