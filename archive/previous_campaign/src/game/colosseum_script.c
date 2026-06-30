/**
 * @file colosseum_script.c
 * @brief Story script interpreter and Colosseum tournament logic.
 *
 * =========================================================================
 * SUBSYSTEM ANALYSIS
 * =========================================================================
 *
 * Address range: 0x80212000 - 0x80240000
 * Total functions: ~320
 * Total code size: ~180KB
 *
 * This is the largest subsystem in the uncovered gap. It contains the
 * story script interpreter, the Colosseum tournament flow controller,
 * team management/validation, and extensive logic for the game's unique
 * stadium-based battle progression.
 *
 * THE SCRIPT INTERPRETER
 * ----------------------
 *
 * fn_802249B8 (StoryScriptExecute) is the crown jewel at 6012 bytes --
 * the single largest function in the entire uncovered range. It implements
 * a bytecode interpreter for the game's story/event scripting system.
 *
 * The interpreter reads commands from a data stream pointed to by
 * lbl_80478D78 (SDA21-relative) and dispatches them through
 * jumptable_8039A220 (52 entries). The command byte at offset +3 in the
 * stream is read with `lbz r0, 0x3(r31)`, then range-checked and used
 * as the jumptable index after subtracting 7 (commands 0x07-0x3B).
 *
 * Interpreter setup (first ~200 instructions):
 *   1. PokemonGet(0, 0, 0x14, 0) -> party count (stored in r24)
 *   2. fn_801F453C(0, 1) -> player party reference (stored in r19)
 *   3. PokemonSlotLookupDefault(0x11, 0) -> player active slot (r30)
 *   4. fn_80205184() -> some context (stored in r28)
 *   5. PokemonSlotLookupDefault(0x12, 0) -> enemy active slot (r18)
 *   6. Load flags from lbl_80478D78+3
 *   7. If bit 6 is set: player=r30, enemy=r18 (normal perspective)
 *      If bit 6 is clear: player=r18, enemy=r30 (swapped perspective)
 *   8. Set up active Pokemon via PokemonSet(0, 0, 0x47, 0, playerSlot)
 *      and PokemonSet(0, 0, 0x4B, 0, enemySlot)
 *
 * Script program counter:
 *   lbl_8047B610 is the script PC. It's incremented by +1 for most
 *   commands and by +5 for commands that skip the next instruction
 *   (like successful conditional branches). The pattern:
 *     lwz r3, lbl_8047B610@sda21(r0)
 *     addi r0, r3, 1  (or 5)
 *     stw r0, lbl_8047B610@sda21(r0)
 *   appears hundreds of times throughout this code.
 *
 * Data tables referenced from the interpreter:
 *   jumptable_8039A220 : Command dispatch table (52 entries)
 *   lbl_80399F58       : Command parameter table (indexed by command byte)
 *   lbl_80279EF4       : Half-word constant table (indexed by status values)
 *
 * COLOSSEUM TOURNAMENT FUNCTIONS
 * ------------------------------
 *
 *   fn_80221104 (ColosseumMatchSetup, 0x100C bytes):
 *     Sets up a complete Colosseum match. Resolves opponents from the
 *     bracket data, validates teams, initializes battle parameters.
 *     Uses TrainerDataGet extensively to configure both sides.
 *
 *   fn_8021FAD4 (TeamValidation, 0xCA4 bytes):
 *     Validates a team against Colosseum rules. Checks for banned
 *     species, duplicate items, level caps, and other restrictions.
 *     Called before each round to verify team legality.
 *
 *   fn_8022BE2C (ColosseumRoundExecute, 0x1258 bytes):
 *     Manages a full round: pre-battle setup, the battle itself,
 *     and post-battle processing (experience, rewards, healing).
 *
 *   fn_8022A6C8 (ColosseumBracketAdvance, 0xBD4 bytes):
 *     Advances the tournament bracket after a round completes.
 *     Handles win/loss paths and bracket reshuffling.
 *
 *   fn_8022F2F8 (RewardProcessing, 0xB28 bytes):
 *     Post-battle reward processing: prize money calculation,
 *     item rewards, experience distribution.
 *
 * TEAM MANAGEMENT (0x80226000-0x80240000)
 * ----------------------------------------
 *
 *   fn_80230568 (TeamRegistration, 0x1194 bytes):
 *     Full team registration flow for a Colosseum. Validates team,
 *     saves team composition, sets entry flags.
 *
 *   fn_80232110 (TeamCompositionCheck, 0xC18 bytes):
 *     Detailed team composition checking (species clause, item clause).
 *
 *   fn_802331F4 (OpponentTeamGeneration, 0xBBC bytes):
 *     Generates an opponent team for a Colosseum round. Selects from
 *     predefined trainer rosters based on difficulty level.
 *
 *   fn_80234A0C (TeamPokemonSetup, 0xC50 bytes):
 *     Initializes each Pokemon on a team for battle: calculates stats,
 *     applies level adjustments, sets up movesets.
 *
 * BATTLE FLOW HELPERS
 * -------------------
 *
 *   fn_80239984 (PreBattleSetup) - 491 calls
 *     One of the most-called functions. Sets up the pre-battle state:
 *     resolves the opponent, configures battle rules, starts the
 *     transition sequence.
 *
 *   fn_80239EE8 (BattleSequenceStart) - 491 calls
 *     Launches a battle sequence. Takes a full set of parameters:
 *     battle ID, trainer slot, Pokemon pointer, flags, and a sequence
 *     type ID (0xF1-0xF4).
 *
 *     The function at 0x80249000 region shows the pattern clearly:
 *       fn_80205B8C(ctx)          -> get Pokemon ptr
 *       fn_80239984(ctx, slot, seqId) -> pre-battle setup
 *       fn_80236BFC(slot, slot2, flag) -> check trainer Pokemon flag
 *       fn_80239EE8(battleId, slot, ptr, 0,0, r8, 0, 0xF1) -> start
 *
 *     Sequence type IDs:
 *       0xF1 : Initial battle setup
 *       0xF2 : Mid-battle (round 2 of multi-battle?)
 *       0xF3 : Battle continuation
 *       0xF4 : Final round
 *
 *   fn_802395C8 (BattleSequenceCheck) - 98 calls
 *   fn_8023793C (BattleResultCheck) - 98 calls
 *
 * MASSIVE FUNCTIONS SUMMARY (by size):
 *   fn_802249B8 : 6012 bytes (StoryScriptExecute)
 *   fn_8022BE2C : 4696 bytes (ColosseumRoundExecute)
 *   fn_8024E690 : 4644 bytes (unknown, in reward/post-battle region)
 *   fn_80245FC4 : 4228 bytes (unknown, in team setup region)
 *   fn_80221104 : 4108 bytes (ColosseumMatchSetup)
 *   fn_8023A740 : 3416 bytes (unknown, near BattleSequenceStart)
 *   fn_8023B498 : 3792 bytes (unknown, near BattleSequenceStart)
 *   fn_80230568 : 4500 bytes (TeamRegistration)
 *   fn_80232110 : 3096 bytes (TeamCompositionCheck)
 *   fn_802331F4 : 3004 bytes (OpponentTeamGeneration)
 *   fn_80234A0C : 3152 bytes (TeamPokemonSetup)
 *   fn_8021FAD4 : 3236 bytes (TeamValidation)
 *   fn_8022A6C8 : 2964 bytes (ColosseumBracketAdvance)
 *   fn_8022F2F8 : 2856 bytes (RewardProcessing)
 *   fn_80240BD0 : 2704 bytes (unknown)
 *   fn_801FFEC8 : 2920 bytes (unknown, in trainer/party region)
 *
 * =========================================================================
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* fn_8012640C();
extern u32   fn_801254B4();
extern u32   itemGetStatus();

/* fn_801F453C: Get player party reference */
extern void* fn_801F453C();

/* fn_80205184: Get current context (unknown purpose) */
extern u32 fn_80205184();

/* fn_802040E8: Another context query */
extern u32 fn_802040E8();

/* fn_80207BF4: Yet another context query */
extern u32 fn_80207BF4();

/* fn_801EF8F4: Battle system entry point (in battle_main.c range) */
extern int fn_801EF8F4();

/* SDA variables */
extern u32 lbl_8047B610;
extern u8 lbl_8047B614;
extern u8 lbl_8047B626;

/* Forward declarations for converted functions */

/* =========================================================================
 * fn_802249B8 - StoryScriptExecute
 *
 * THE MAIN STORY SCRIPT INTERPRETER.
 *
 * At 6012 bytes, this is the largest function in the uncovered range and
 * arguably one of the most important functions in the entire game. It
 * processes the bytecode-like command stream that drives all story events,
 * Colosseum round progressions, and scripted interactions.
 *
 * Decompilation outline (from disassembly analysis):
 *
 * void StoryScriptExecute(u32 param1, u32 param2) {
 *     u16 partyCount;
 *     void* playerParty;
 *     void* playerSlot;
 *     void* enemySlot;
 *     u32 contextA, contextB;
 *     u8* scriptPtr;
 *     u32 activePlayer, activeEnemy;
 *     u32 flags;
 *
 *     // Setup
 *     partyCount = PokemonGet(NULL, 0, 0x14, 0);
 *     playerParty = fn_801F453C(0, 1);
 *     playerSlot = PokemonSlotLookupDefault(0x11, 0);
 *     contextA = fn_80205184();
 *     enemySlot = PokemonSlotLookupDefault(0x12, 0);
 *
 *     scriptPtr = (u8*)lbl_80478D78;  // SDA21
 *     flags = scriptPtr[3];
 *
 *     if (flags & 0x40) {
 *         // Normal perspective
 *         activePlayer = playerSlot;
 *         PokemonSet(NULL, 0, 0x47, 0, playerSlot);
 *         PokemonSet(NULL, 0, 0x4B, 0, enemySlot);
 *         scriptPtr[3] &= ~0x40;
 *     } else {
 *         // Swapped perspective
 *         activePlayer = enemySlot;
 *         PokemonSet(NULL, 0, 0x47, 0, enemySlot);
 *         PokemonSet(NULL, 0, 0x4B, 0, playerSlot);
 *     }
 *
 *     // Resolve initial Pokemon
 *     PokemonGet(activePlayer, partyCount, ...);
 *     contextB = fn_80207BF4(activePlayer);
 *     ...fn_802040E8(activePlayer)...
 *
 *     // Main command loop
 *     while (1) {
 *         u8 cmd = scriptPtr[3];
 *         if (cmd < 7 || cmd > 0x3B) {
 *             // Invalid command or end
 *             break;
 *         }
 *
 *         // Dispatch through jumptable_8039A220
 *         switch (cmd - 7) {
 *             case 0x00: // cmd 0x07
 *                 // ... handler ...
 *                 lbl_8047B610++;
 *                 break;
 *             case 0x02: // cmd 0x09 - Battle command
 *                 if (CheckEventFlag(context, 0x09)) {
 *                     SetEventState(context, 0x09, 0);
 *                 }
 *                 // ... battle setup ...
 *                 break;
 *             // ... 50 more cases ...
 *         }
 *     }
 * }
 *
 * [Full decompilation requires analysis of all 52 command handlers
 *  within the jumptable_8039A220 dispatch.]
 * ========================================================================= */
/* Address: 0x802249B8 | Size: 0xbe8 | Ghidra import */
void fn_802249B8(void)

{
    char r3;
    char r4;

    extern s8 fn_80077B3C();
    extern s8 fn_80077B60();
    extern void fn_801F0134();
    extern u32 fn_801F025C();
    extern short fn_801F33E8();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern s8 fn_801F6E98();
    extern int fn_801F7258();
    extern int fn_801F986C();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_80200B10();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern s8 fn_80202ADC();
    extern s8 fn_80203CCC();
    extern s8 fn_802062FC();
    extern s8 fn_80206608();
    extern s8 fn_80207AE0();
    extern void fn_802097C8();
    extern s8 fn_8020981C();
    extern void fn_80211B94();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7D;
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B628;
    extern u32 lbl_8047B62C;
  u32 bVar1;
  u32 bVar2;
  u32 uVar3;
  u16 uVar9;
  s8 cVar13;
  u32 uVar4;
  u32 uVar5;
  short sVar10;
  u32 uVar6;
  s8 cVar14;
  short sVar11;
  u16 uVar12;
  int iVar7;
  int iVar8;

  u16 uVar15;
  
  bVar1 = 0;
  uVar9 = fn_801F54A4(0,0,0x14,0);
  cVar13 = (int)fn_801F453C(0,1);
  uVar4 = fn_801F025C(0x11,0);
  fn_80205184();
  uVar5 = fn_801F025C(0x12,0);
  bVar2 = (lbl_80478D7B & 0x40) == 0;
  if (bVar2) {
    fn_801F4C14(0,0,0x47,0,uVar5);
    fn_801F4C14(0,0,0x4b,0,uVar4);
    uVar6 = uVar4;
  }
  else {
    fn_801F4C14(0,0,0x47,0,uVar4);
    fn_801F4C14(0,0,0x4b,0,uVar5);
    lbl_80478D7B = lbl_80478D7B & 0xbf;
    uVar6 = uVar5;
    uVar5 = uVar4;
  }
  fn_801F0134(uVar6,uVar9);
  sVar10 = fn_80207BF4(uVar5);
  fn_802040E8(uVar5);
  uVar4 = fn_801F025C(2,uVar5);
  uVar6 = (int)fn_8012640C(uVar5,0,0xd9,0);
  fn_8012640C(uVar5,0,0xf8,0);
  if ((((sVar10 == 0x13) && ((lbl_8047B618 & 0x2000) == 0)) && (r3 == 0))
     && (lbl_80478D7B < 10)) {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    return;
  }
  cVar14 = fn_801F6E98(uVar4,0x4b);
  if (((cVar14 == 1) && ((lbl_8047B618 & 0x2000) == 0)) &&
     ((r3 == 0 && (lbl_80478D7B < 8)))) {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    return;
  }
  cVar14 = fn_802062FC(uVar5);
  if (((cVar14 == 0) && ((char)lbl_80478D7B != '\v')) &&
     ((char)lbl_80478D7B != 31)) {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    return;
  }
  cVar14 = fn_802026E4(uVar5,0x14);
  if ((cVar14 == 1) && (bVar2)) {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    return;
  }
  uVar3 = (u32)lbl_80478D7B;
  if (6 < uVar3) {
    if ((*(short *)(uVar3 * 2 + -0x7fd8610c) != 0) &&
       (cVar13 = fn_802026E4(uVar5), cVar13 == 1)) {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
      return;
    }
    uVar3 = lbl_80478D7B - 7;
    if (0x34 < uVar3) {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
      return;
    }

    ((int (*)(void))**(void ***)(uVar3 * 4 + -0x7fc65de0))();
    return;
  }
  uVar12 = *(u16 *)(uVar3 * 2 + -0x7fd8610c);
  if (uVar12 == 6) {
    if ((sVar10 == 0x29) && ((r3 == 1 || (r4 == -0x80)))) {
      if ((lbl_8047B618 & 0x2000) == 0) {
        lbl_80478D7D = 0;
      }
      else {
        lbl_8047B618 = lbl_8047B618 & 0xffffdfff;
        lbl_80478D7D = 1;
      }
      fn_80211B94(lbl_8047B62C,0x80379808,0);
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
      return;
    }
    cVar13 = fn_80207AE0(uVar5,10);
    if (((cVar13 == 1) && ((lbl_8047B618 & 0x2000) != 0)) &&
       ((r3 == 1 || (r4 == -0x80)))) {
      lbl_80478D7D = 2;
      fn_80211B94(lbl_8047B62C,0x80379808,0);
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
      return;
    }
    cVar13 = fn_80207AE0(uVar5,10);
    if (((cVar13 == 0) && (sVar10 != 0x29)) && (cVar13 = fn_80203CCC(uVar5), cVar13 == 1))
    {
      bVar1 = 1;
    }
    goto LAB_00222404;
  }
  if (uVar12 < 6) {
    if (uVar12 == 4) {
      if ((sVar10 == 0x11) && ((r3 == 1 || (r4 == -0x80)))) {
        if ((lbl_8047B618 & 0x2000) == 0) {
          lbl_80478D7D = 0;
        }
        else {
          lbl_8047B618 = lbl_8047B618 & 0xffffdfff;
          lbl_80478D7D = 1;
        }
        fn_80211B94(lbl_8047B62C,0x80379836,0);
        *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
        return;
      }
      cVar13 = fn_80207AE0(uVar5,3);
      if (((cVar13 == 1) || (cVar13 = fn_80207AE0(uVar5,8), cVar13 == 1)) &&
         (((lbl_8047B618 & 0x2000) != 0 &&
          ((r3 == 1 || (r4 == -0x80)))))) {
        lbl_80478D7D = 2;
        fn_80211B94(lbl_8047B62C,0x80379836,0);
        *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
        return;
      }
      cVar13 = fn_80203CCC(uVar5);
      if (cVar13 == 1) {
        cVar13 = fn_80207AE0(uVar5,3);
        if ((cVar13 == 0) && (cVar13 = fn_80207AE0(uVar5,8), cVar13 == 0)) {
          if (sVar10 != 0x11) {
            bVar1 = 1;
          }
        }
        else {
          cVar13 = fn_8020981C(uVar6,0x43);
          if (cVar13 == 2) {
            fn_802097C8(uVar6,0x43,0);
          }
        }
      }
    }
    else if (uVar12 < 4) {
      if (2 < uVar12) {
        if ((sVar10 == 0x11) && ((r3 == 1 || (r4 == -0x80)))) {
          if ((lbl_8047B618 & 0x2000) == 0) {
            lbl_80478D7D = 0;
          }
          else {
            lbl_8047B618 = lbl_8047B618 & 0xffffdfff;
            lbl_80478D7D = 1;
          }
          fn_80211B94(lbl_8047B62C,0x80379836,0);
          *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
          return;
        }
        cVar13 = fn_80207AE0(uVar5,3);
        if ((((cVar13 == 1) || (cVar13 = fn_80207AE0(uVar5,8), cVar13 == 1)) &&
            ((lbl_8047B618 & 0x2000) != 0)) &&
           ((r3 == 1 || (r4 == -0x80)))) {
          lbl_80478D7D = 2;
          fn_80211B94(lbl_8047B62C,0x80379836,0);
          *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
          return;
        }
        cVar13 = fn_80207AE0(uVar5,3);
        if (((cVar13 == 0) && (cVar13 = fn_80207AE0(uVar5,8), cVar13 == 0)) &&
           ((cVar13 = fn_80203CCC(uVar5), cVar13 == 1 && (sVar10 != 0x11)))) {
          bVar1 = 1;
        }
      }
    }
    else {
      if ((sVar10 == 7) && ((r3 == 1 || (r4 == -0x80)))) {
        if ((lbl_8047B618 & 0x2000) == 0) {
          lbl_80478D7D = 0;
        }
        else {
          lbl_8047B618 = lbl_8047B618 & 0xffffdfff;
          lbl_80478D7D = 1;
        }
        fn_80211B94(lbl_8047B62C,0x8037981f,0);
        *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
        return;
      }
      if ((sVar10 != 7) && (cVar13 = fn_80203CCC(uVar5), cVar13 == 1)) {
        bVar1 = 1;
      }
    }
    goto LAB_00222404;
  }
  if (uVar12 == 8) {
    bVar2 = 1;
    if ((sVar10 != 0x2b) && (sVar11 = fn_801F33E8(0,0xb), sVar11 != 0)) {
      bVar2 = 0;
    }
    cVar13 = fn_80203CCC(uVar5);
    if (((cVar13 != 1) ||
        (cVar13 = fn_802025B8(uVar5,*(u16 *)
                                      ((u32)lbl_80478D7B * 2 + -0x7fd8610c)),
        cVar13 != 2)) || ((!bVar2 || ((sVar10 == 0x48 || (sVar10 == 0xf))))))
    goto LAB_00222404;
    cVar13 = fn_801F54A4(0,0,0x34,0);
    cVar14 = fn_80077B60();
    fn_80077B3C();
    if ((cVar13 == 1) && (cVar14 != 1)) {
      sVar10 = 0;
      uVar4 = fn_801F025C(2,uVar5);
      uVar12 = fn_801F54A4(0,0,0x16,0);
      fn_801F54A4(0,0,0x17,0);
      for (uVar15 = 0; uVar15 < uVar12; uVar15 = uVar15 + 1) {
        iVar7 = fn_801F7258(uVar4,uVar15);
        if (iVar7 != 0) {
          for (uVar3 = 0; (uVar3 & 0xffff) < 6; uVar3 = uVar3 + 1) {
            iVar8 = fn_801F986C(iVar7,uVar3);
            if (((iVar8 != 0) && (cVar13 = fn_80206608(), cVar13 != 0)) &&
               (cVar13 = fn_80202ADC(iVar8,8), cVar13 == 1)) {
              sVar10 = sVar10 + 1;
            }
          }
        }
      }
      if (sVar10 != 0) {
          bVar2 = 1;
        }
        else {
      }
      bVar2 = 0;
    }
    if (bVar2 == 0) {
      fn_80200B10(uVar5);
      bVar1 = 1;
    }
    goto LAB_00222404;
  }
  if (((7 < uVar12) || (cVar14 = fn_80207AE0(uVar5,0xf), cVar14 != 0)) ||
     ((cVar14 = fn_80203CCC(uVar5), cVar14 != 1 || ((cVar13 == 1 || (sVar10 == 0x28))))))
  goto LAB_00222404;
  cVar13 = fn_801F54A4(0,0,0x34,0);
  fn_80077B60();
  cVar14 = fn_80077B3C();
  if ((cVar13 == 1) && (cVar14 != 1)) {
    sVar10 = 0;
    uVar4 = fn_801F025C(2,uVar5);
    uVar12 = fn_801F54A4(0,0,0x16,0);
    fn_801F54A4(0,0,0x17,0);
    for (uVar15 = 0; uVar15 < uVar12; uVar15 = uVar15 + 1) {
      iVar7 = fn_801F7258(uVar4,uVar15);
      if (iVar7 != 0) {
        for (uVar3 = 0; (uVar3 & 0xffff) < 6; uVar3 = uVar3 + 1) {
          iVar8 = fn_801F986C(iVar7,uVar3);
          if (((iVar8 != 0) && (cVar13 = fn_80206608(), cVar13 != 0)) &&
             (cVar13 = fn_80202ADC(iVar8,7), cVar13 == 1)) {
            sVar10 = sVar10 + 1;
          }
        }
      }
    }
    if (sVar10 != 0) {
        bVar2 = 1;
      }
      else {
    }
    bVar2 = 0;
  }
  if (bVar2 == 0) {
    fn_80200B10(uVar5);
    bVar1 = 1;
  }
LAB_00222404:
  if (bVar1) {
    cVar13 = fn_802025B8(uVar5,*(u16 *)
                                 ((u32)lbl_80478D7B * 2 + -0x7fd8610c));
    if (cVar13 == 2) {
      fn_8020248C(uVar5,*(u16 *)((u32)lbl_80478D7B * 2 + -0x7fd8610c),0);
    }
    cVar13 = fn_801FECD4(uVar5);
    if (cVar13 == 1) {
      fn_801FE7EC(uVar5,0x7c,0,0);
    }
    if ((lbl_8047B618 & 0x2000) == 0) {
      lbl_80478D7D = 0;
    }
    else {
      lbl_8047B618 = lbl_8047B618 & 0xffffdfff;
      lbl_80478D7D = 1;
    }
    uVar3 = (u32)lbl_80478D7B;
    if (((uVar3 == 2) || ((uVar3 - 5 & 0xff) < 2)) || (uVar3 == 3)) {
      lbl_8047B628 = lbl_80478D7B;
      lbl_8047B618 = lbl_8047B618 | 0x4000;
    }
    fn_80211B94(lbl_8047B62C,*(u32 *)(uVar3 * 4 + -0x7fc660a8),0);
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  }
  else {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  }
  return;
}
/* =========================================================================
 * fn_80221104 - ColosseumMatchSetup
 *
 * 4108 bytes. Initializes a Colosseum match by:
 *   1. Querying the bracket data for the current round's opponent
 *   2. Loading the opponent's trainer data via TrainerDataGet
 *   3. Setting up battle rules (level caps, species bans)
 *   4. Configuring the battle sequence
 *
 * Heavily references lbl_80279E7C (constant 0x7693) and uses
 * repeated calls to TrainerDataGet with field 0x43 (Pokemon pointer).
 * ========================================================================= */
/* Address: 0x80221104 | Size: 0xfcc | Ghidra import */
void fn_80221104(u8 r3, u32 r4)

{
    extern u8 lbl_80379F58[];
    extern u8 lbl_80378964[];
    extern void _threadSwitch();
    extern void fn_801C3430();
    extern void battleGridReplacePokemon();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern u8 fn_801DDD28();
    extern u32 fn_801F0204();
    extern void fn_801F0234();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u32 fn_80201C58();
    extern u32 fn_80207C6C();
    extern void fn_802085C4();
    extern void fn_80208750();
    extern void fn_8020F108();
    extern void fn_80211B94();
    extern u32 fn_80222110();
    extern void fn_80265598();
    extern u8 lbl_80478D78;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  u32 uVar1;
  u16 uVar6;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u8 cVar9;
  u16 uVar7;
  u16 sVar8;
  u8 bVar10;

  u32 uVar11;
  
  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F025C(r3,0);
  uVar3 = fn_801F025C(0x11,0);
  uVar4 = fn_801F025C(0x12,0);
  cVar9 = lbl_80379F58[0x160a4] - ((lbl_80379F58[0x1601e] & 0xf) - 1);
  if (((r4 == 0x11) || (r4 - 1U < 2)) || (r4 == 0x1a)) {
    if (r4 == 1) {
      if (0x80480820 == 0x7aa8) {
        uVar4 = fn_80222110(cVar9);
      }
      else if (*((&lbl_80478D78)+3) == 0) {
        uVar4 = fn_80222110(cVar9);
      }
      else {
        uVar1 = *((&lbl_80478D78)+3) - 0xf;
        if (uVar1 < 0xe) {

          ((int (*)(void))**(void ***)(uVar1 * 4 + -0x7fc65e38))();
          return;
        }
        uVar4 = fn_80222110(cVar9);
      }
      uVar6 = fn_801F54A4(0,0,0x14,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
      if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,uVar4,4,0), cVar9 != 0)) {
        fn_801DA9E8(uVar5,uVar4,4);
        fn_80265598(uVar2,uVar6,1);
      }
    }
    if (r4 == 0x1a) {
      uVar6 = fn_801F54A4(0,0,0x14,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
      if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x2d,4,0), cVar9 != 0)) {
        fn_801DA9E8(uVar5,0x2d,4);
        fn_80265598(uVar2,uVar6,1);
      }
    }
    if (r4 == 0x11) {
      fn_80208750(uVar2,0x121,1,0);
      fn_80208750(uVar3,0x121,2,0);
      fn_801F0234(0x11);
      uVar4 = fn_801F0204();
      fn_802085C4(uVar2,0x121,1,1,uVar4);
      fn_801F0234(0x12);
      uVar4 = fn_801F0204();
      fn_802085C4(uVar3,0x121,2,0,uVar4);
    }
    if (r4 != 2) {
      return;
    }
    fn_8020F108(0xa4,uVar2,uVar2,0,0);
    return;
  }
  if ((lbl_8047B618 & 0x80) != 0) {
    fn_80211B94(lbl_8047B62C,(u32)lbl_80378964,0);
    return;
  }
  if (((r4 - 10U < 4) || (r4 - 0x20U < 3)) || (r4 == 0x1e)) {
    if (r4 == 10) {
      fn_80208750(uVar2,0xf0,1,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xf0,1,0,uVar3);
    }
    if (r4 == 0xb) {
      fn_80208750(uVar2,0xf1,1,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xf1,1,0,uVar3);
    }
    if (r4 == 0xc) {
      fn_80208750(uVar2,0xc9,2,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xc9,2,0,uVar3);
    }
    if (r4 == 0xd) {
      fn_80208750(uVar2,0x102,2,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0x102,2,0,uVar3);
    }
    if (r4 == 0x20) {
      fn_80208750(uVar2,0xf0,1,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xf0,1,0,uVar3);
    }
    if (r4 == 0x22) {
      fn_80208750(uVar2,0xc9,1,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xc9,1,0,uVar3);
    }
    if (r4 == 0x21) {
      fn_80208750(uVar2,0xf1,1,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xf1,1,0,uVar3);
    }
    if (r4 != 0x1e) {
      return;
    }
    cVar9 = (int)fn_801F453C(0,0);
    if (cVar9 == 2) {
      fn_801F54A4(0,0,0x14,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
      if (uVar5 == 0) {
        return;
      }
      cVar9 = fn_801DDD28(uVar5,0x80,4,0);
      if (cVar9 == 0) {
        return;
      }
      fn_801DA9E8(uVar5,0x80,4);
      return;
    }
    cVar9 = (int)fn_801F453C(0,0);
    if (cVar9 == 1) {
      fn_801F54A4(0,0,0x14,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
      if (uVar5 == 0) {
        return;
      }
      cVar9 = fn_801DDD28(uVar5,0x7f,4,0);
      if (cVar9 == 0) {
        return;
      }
      fn_801DA9E8(uVar5,0x7f,4);
      return;
    }
    cVar9 = (int)fn_801F453C(0,0);
    if (cVar9 == 3) {
      fn_801F54A4(0,0,0x14,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
      if (uVar5 == 0) {
        return;
      }
      cVar9 = fn_801DDD28(uVar5,0x7e,4,0);
      if (cVar9 == 0) {
        return;
      }
      fn_801DA9E8(uVar5,0x7e,4);
      return;
    }
    cVar9 = (int)fn_801F453C(0,0);
    if (cVar9 != 4) {
      fn_801F54A4(0,0,0x14,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
      if (uVar5 == 0) {
        return;
      }
      cVar9 = fn_801DDD28(uVar5,0x9d,4,0);
      if (cVar9 == 0) {
        return;
      }
      fn_801DA9E8(uVar5,0x9d,4);
      return;
    }
    fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if (uVar5 == 0) {
      return;
    }
    cVar9 = fn_801DDD28(uVar5,0x7d,4,0);
    if (cVar9 == 0) {
      return;
    }
    fn_801DA9E8(uVar5,0x7d,4);
    return;
  }
  if (r4 == 0x17) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x57,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x57,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x1f) {
    fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x9d,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x9d,4);
    }
  }
  if (r4 == 0x18) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x58,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x58,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x23) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0xa3,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0xa3,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x24) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0xd8,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0xd8,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x19) {
    fn_80208750(uVar2,1,2,0);
    fn_801F0234(0x12);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar2,1,2,0,uVar3);
  }
  if (r4 == 0xe) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x38,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x38,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x16) {
    fn_80208750(uVar2,0x111,3,0);
    fn_801F0234(0x11);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar2,0x111,3,0,uVar3);
  }
  if (r4 == 0x12) {
    fn_80208750(uVar2,0xf8,2,0);
    fn_801F0234(0x12);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar2,0xf8,2,0,uVar3);
  }
  if (r4 == 0x13) {
    fn_80208750(uVar4,0x161,2,0);
    fn_801F0234(0x12);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar4,0x161,2,0,uVar3);
  }
  if (r4 == 0x15) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x3b,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x3b,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 7) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x39,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x39,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x1b) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x32,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x32,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 0x1c) {
    fn_80208750(uVar2,0x36,3,0);
    fn_801F0234(0x11);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar2,0x36,3,0,uVar3);
  }
  if (r4 == 0x1d) {
    fn_80208750(uVar2,0x74,3,0);
    fn_801F0234(0x11);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar2,0x74,3,0,uVar3);
  }
  if (r4 == 9) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
    if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x86,4,0), cVar9 != 0)) {
      fn_801DA9E8(uVar5,0x86,4);
      fn_80265598(uVar2,uVar7,1);
    }
  }
  if (r4 == 6) {
    uVar3 = fn_80201C58(uVar2,0xe);
    fn_80208750(uVar2,uVar3,2,0);
    fn_801F0234(0x12);
    uVar4 = fn_801F0204();
    fn_802085C4(uVar2,uVar3,2,0,uVar4);
  }
  if (r4 == 0x10) {
    sVar8 = fn_80205184(uVar2);
    if (sVar8 == 0x157) {
      fn_80208750(uVar2,0x157,3,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0x157,3,0,uVar3);
    }
    sVar8 = fn_80205184(uVar2);
    if (sVar8 == 0xa8) {
      fn_80208750(uVar2,0xa8,3,0);
      fn_801F0234(0x11);
      uVar3 = fn_801F0204();
      fn_802085C4(uVar2,0xa8,3,0,uVar3);
    }
  }
  if (r4 != 0) goto LAB_0021f0bc;
  bVar10 = (int)fn_801F453C(0,1);
  uVar3 = (int)fn_8012640C(uVar2,0,0xee,0);
  uVar4 = fn_80207C6C(uVar2,bVar10);
  if (bVar10 == 3) {
LAB_0021ef70:
    uVar11 = 0x83;
  }
  else if (bVar10 < 3) {
    if (bVar10 == 1) {
      uVar11 = 0x84;
    }
    else {
      if (bVar10 == 0) goto LAB_0021ef70;
      uVar11 = 0x85;
    }
  }
  else {
    if (4 < bVar10) goto LAB_0021ef70;
    uVar11 = 0x82;
  }
  fn_801DDD28(uVar3,0x81,4,0);
  fn_801DDD28(uVar4,uVar11,4,0);
  fn_80265598(uVar2,uVar6,1);
  fn_801F54A4(0,0,0x14,0);
  uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
  if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,0x81,4,0), cVar9 != 0)) {
    fn_801DA9E8(uVar5,0x81,4);
    while (cVar9 = fn_801DA94C(uVar5,0x81,4), cVar9 != 0) {
      _threadSwitch();
    }
    fn_801DA8C4(uVar5,0x81,4);
  }
  battleGridReplacePokemon(uVar3,uVar4);
  fn_801C3430();
  fn_801254B4(uVar2,0,0xee,0,uVar4);
  fn_801F54A4(0,0,0x14,0);
  uVar5 = (int)fn_8012640C(uVar2,0,0xee,0);
  if ((uVar5 != 0) && (cVar9 = fn_801DDD28(uVar5,uVar11,4,0), cVar9 != 0)) {
    fn_801DA9E8(uVar5,uVar11,4);
  }
  fn_801DB100(uVar3);
LAB_0021f0bc:
  if (r4 == 0x14) {
    fn_80208750(uVar2,0x75,3,0);
    fn_801F0234(0x11);
    uVar3 = fn_801F0204();
    fn_802085C4(uVar2,0x75,3,0,uVar3);
  }
  return;
}
/* =========================================================================
 * fn_8021FAD4 - TeamValidation
 *
 * 3236 bytes. Validates a team against Colosseum entry rules.
 * References lbl_80279FE0 (constants 0x000F, 0x0016) which may be
 * minimum/maximum level boundaries.
 * ========================================================================= */
/* TODO: Decompile fn_8021FAD4 (3236 bytes) */

/* =========================================================================
 * fn_8022BE2C - ColosseumRoundExecute
 *
 * 4696 bytes. Second largest function. Manages a complete battle round:
 *   1. Pre-battle: team validation, opponent setup
 *   2. Battle: calls into the battle engine
 *   3. Post-battle: reward processing, bracket advancement
 * ========================================================================= */
/* Address: 0x8022BE2C | Size: 0x1f8 | Ghidra import */


u32 fn_8022BE2C(u32 r3)

{
    extern u32 DAT_80279ff4;
    extern void fn_801F4C14();
    extern void fn_80203EDC();
    extern u32 fn_80203FE4();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern u8 lbl_8047E608;
  u32 *puVar1;
  u32 *puVar2;
  u32 uVar3;

  s8 cVar6;
  u16 uVar5;
  u32 uVar4;
  u32 *puVar7;
  u32 *puVar8;
  int iVar9;
  u8 local_54 [4];
  u32 local_50 [7];
  
  local_54[0] = lbl_8047E608;
  iVar9 = 2;
  puVar1 = (u32 *)&DAT_80279ff4;
  puVar2 = (u32 *)local_54;
  do {
    puVar8 = puVar2;
    puVar7 = puVar1;
    uVar3 = puVar7[2];
    puVar8[1] = puVar7[1];
    puVar8[2] = uVar3;
    iVar9 = iVar9 + -1;
    puVar1 = puVar7 + 2;
    puVar2 = puVar8 + 2;
  } while (iVar9 != 0);
  puVar8[3] = puVar7[3];
  cVar6 = fn_802062FC(r3);
  if (cVar6 != 0) {
    uVar5 = fn_802040E8(r3);
    uVar4 = fn_80203FE4(r3);
    fn_80203EDC(r3);
    fn_8012640C(r3,0,0xd9,0);
    uVar3 = fn_80205B8C(r3);
    fn_8012640C(uVar3,0,0x83,0);
    fn_8012640C(uVar3,0,0x87,0);
    fn_801F4C14(0,0,0x56,0,uVar5);
    if ((uVar4 & 0xffff) < 0x2c) {

      uVar3 = ((int (*)(void))**(void ***)((uVar4 & 0xffff) * 4 + -0x7fc65c38))();
      return uVar3;
    }
  }
  return 0;
}
/* =========================================================================
 * fn_80230568 - TeamRegistration
 *
 * 4500 bytes. Full team registration flow. Called when the player
 * enters a Colosseum and registers their team.
 * ========================================================================= */
/* Address: 0x80230568 | Size: 0x1194 | Ghidra import */

u32 fn_80230568(u32 r3,u32 r4)

{
    extern u32 DAT_8038fffc;
    extern u32 DAT_8038fffd;
    extern u32 _DAT_80399f74;
    extern void fn_80077B3C();
    extern s8 fn_80077B60();
    extern u32 fn_800FA280();
    extern void fn_8011BBD8(u32, u32, u32, u32, s16);
    extern void fn_8011BEB4();
    extern s8 fn_80123B5C();
    extern void fn_80132A38();
    extern void fn_801DA7AC();
    extern short fn_801EF634();
    extern int fn_801F00D0();
    extern int fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F37B0();
    extern void fn_801F4C14();
    extern s8 fn_801F54A4();
    extern int fn_801F7258();
    extern int fn_801F986C();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern s8 fightOutPokemonCheckNoAttackFlag();
    extern void fn_80200B10();
    extern s8 fn_80201704();
    extern u32 fn_80201890();
    extern u32 fn_80201C58();
    extern u32 fn_80201D84();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern short fn_80202360();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_80202ADC();
    extern u32 fn_80203B5C();
    extern s8 fn_80203CCC();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern s8 fn_80206608();
    extern void fn_80211B94();
    extern void fn_802249B8();
    extern u32 fn_8022BE2C();
    extern void fn_8022DF08();
    extern u32 fn_8022EDEC();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7D;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  u32 bVar1;
  short sVar9;
  s8 cVar14;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  int iVar5;
  u16 uVar10;
  short sVar11;
  u32 uVar6;
  u32 uVar7;
  s8 cVar15;
  short sVar12;
  u16 uVar13;
  int iVar8;

  u16 uVar16;
  
  sVar9 = fn_801EF634();
  if (sVar9 != 0) {
    return 0;
  }
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  fn_801F4C14(0,0,0x36,0,r3);
  uVar2 = fn_80205B8C(r3);
  uVar3 = (int)fn_8012640C(r3,0,0xd9,0);
  sVar9 = fn_80207BF4(r3);
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x25);
  if ((cVar14 == 1) && (cVar14 = fn_80201704(r3), cVar14 == 0)) {
    uVar4 = fn_80203B5C(r3,0x10);
    fn_8011BBD8(uVar3,0,0x2d,0,-(uVar4 & 0xffff));
    fn_80211B94(lbl_8047B62C,0x803790ab,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  fn_8022DF08(r3);
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  fn_8022BE2C(r3,0);
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  fn_8022BE2C(r3,1);
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x1c);
  if ((((cVar14 == 1) && (uVar4 = fn_80201D84(r3,0x1c), (uVar4 & 0xffff) != 0)) &&
      (iVar5 = fn_801F00D0(uVar4,r4), iVar5 != 0)) &&
     (cVar14 = fn_802062FC(), cVar14 == 1)) {
    fn_801F4C14(0,0,0x43,0,iVar5);
    uVar10 = fn_80203B5C(r3,8);
    fn_8011BBD8(uVar3,0,0x2d,0,uVar10);
    DAT_8038fffc = fn_801F0134(iVar5,r4);
    DAT_8038fffd = fn_801F0134(r3,r4);
    fn_80211B94(lbl_8047B62C,0x80378b72,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,3);
  if (cVar14 == 1) {
    uVar10 = fn_80203B5C(r3,8);
    fn_8011BBD8(uVar3,0,0x2d,0,uVar10);
    fn_80211B94(lbl_8047B62C,0x8037925f,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,4);
  if (cVar14 == 1) {
    sVar11 = fn_80202360(r3,4);
    uVar4 = fn_80203B5C(r3,0x10);
    fn_8011BBD8(uVar3,0,0x2d,0,(uVar4 & 0xffff) * (int)sVar11);
    fn_8020248C(r3,4,0);
    cVar14 = fn_801FECD4(r3);
    if (cVar14 == 1) {
      fn_801FE7EC(r3,0x7c,0,0);
    }
    fn_80211B94(lbl_8047B62C,0x8037925f,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,6);
  if (cVar14 == 1) {
    uVar10 = fn_80203B5C(r3,8);
    fn_8011BBD8(uVar3,0,0x2d,0,uVar10);
    fn_80211B94(lbl_8047B62C,0x80379287,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x17);
  if (cVar14 == 1) {
    cVar14 = fn_802026E4(r3,8);
    if (cVar14 == 1) {
      uVar10 = fn_80203B5C(r3,4);
      fn_8011BBD8(uVar3,0,0x2d,0,uVar10);
      fn_80211B94(lbl_8047B62C,0x80379402,0);
    }
    else {
      fn_80202810(r3,0x17);
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x18);
  if (cVar14 == 1) {
    uVar10 = fn_80203B5C(r3,4);
    fn_8011BBD8(uVar3,0,0x2d,0,uVar10);
    fn_80211B94(lbl_8047B62C,0x8037941f,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0xe);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0xe);
    uVar6 = fn_80201C58(r3,0xe);
    fn_8011BEB4(0,uVar6,1,0);
    uVar7 = fn_800FA280();
    fn_80132A38(0xd,uVar7);
    cVar15 = fn_80202234(r3,0xe);
    if (cVar14 < cVar15) {
      DAT_8038fffd = (u8)(uVar6 >> 8);
      DAT_8038fffc = (u8)uVar6;
      uVar10 = fn_80203B5C(r3,0x10);
      fn_8011BBD8(uVar3,0,0x2d,0,uVar10);
      fn_80201FDC(r3,0xe,(int)(char)(cVar14 + 1));
      uVar3 = 0x803793a5;
    }
    else {
      fn_80202810(r3,0xe);
      uVar3 = 0x803793c3;
    }
    fn_80211B94(lbl_8047B62C,uVar3,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0xb);
  if (cVar14 == 1) {
    fn_801F37B0(0,0x802316fc,0,0);
    fn_801F4C14(0,0,0x36,0,r3);
    cVar14 = fn_80202108(r3,0xb);
    cVar15 = fn_80202234(r3,0xb);
    if ((char)(cVar14 + 1) < cVar15) {
      fn_80201FDC(r3,0xb);
    }
    else {
      fn_80202810(r3,0xb);
    }
    cVar14 = fightOutPokemonCheckNoAttackFlag(r3);
    if ((cVar14 == 1) || (cVar14 = fn_802026E4(r3,0xb), cVar14 != 1)) {
      fn_80200B10(r3);
      lbl_80478D7D = 1;
    }
    else {
      fn_8020248C(r3,0x22,0);
      lbl_80478D7D = 0;
    }
    fn_80211B94(lbl_8047B62C,0x8037930a,0);
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0xd);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0xd);
    cVar15 = fn_80202234(r3,0xd);
    if ((char)(cVar14 + 1) < cVar15) {
      fn_80201FDC(r3,0xd);
    }
    else {
      fn_80202810(r3,0xd);
    }
    cVar14 = fightOutPokemonCheckNoAttackFlag(r3);
    if (cVar14 == 1) {
      fn_80200B10(r3);
    }
    else {
      cVar14 = fn_802026E4(r3,0xd);
      if ((cVar14 == 0) && (cVar14 = fn_802026E4(r3,0x22), cVar14 == 1)) {
        fn_80202810(r3,0x22);
        cVar14 = fn_802026E4(r3,9);
        uVar3 = _DAT_80399f74;
        if (cVar14 == 0) {
          uVar6 = *(u32 *)(lbl_8047B610);
          lbl_80478D7B = 0x47;
          _DAT_80399f74 = 0x80379bfe;
          fn_802249B8(1,0);
          *(u32 *)(lbl_8047B610) = uVar6;
          _DAT_80399f74 = uVar3;
          cVar14 = fn_802026E4(r3,9);
          if (cVar14 == 1) {
            fn_80211B94(lbl_8047B62C,0x8037931e,0);
          }
        }
      }
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x29);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0x29);
    uVar3 = fn_80201C58(r3,0x29);
    fn_80201FDC(r3,0x29,(int)(char)(cVar14 + 1));
    cVar15 = fn_80123B5C(uVar2,uVar3);
    if (cVar15 < 0) {
      fn_80202810(r3,0x29);
    }
    else {
      cVar15 = fn_80202234(r3,0x29);
      if (cVar15 <= (char)(cVar14 + 1)) {
        fn_80202810(r3,0x29);
        fn_80211B94(lbl_8047B62C,0x80378d2c,0);
      }
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x2a);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0x2a);
    sVar11 = fn_80201C58(r3,0x2a);
    uVar10 = fn_80201890(r3,0x2a);
    sVar12 = (int)fn_8012640C(uVar2,0,0x7f,uVar10);
    fn_80201FDC(r3,0x2a,(int)(char)(cVar14 + 1));
    if (sVar12 == sVar11) {
      cVar15 = fn_80202234(r3,0x2a);
      if ((cVar15 <= (char)(cVar14 + 1)) ||
         (cVar14 = (int)fn_8012640C(uVar2,0,0x80,uVar10), cVar14 == 0)) {
        fn_80202810(r3,0x2a);
        fn_80211B94(lbl_8047B62C,0x80378d40,0);
      }
    }
    else {
      fn_80202810(r3,0x2a);
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x1d);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0x1d);
    cVar15 = fn_80202234(r3,0x1d);
    if ((char)(cVar14 + 1) < cVar15) {
      fn_80201FDC(r3,0x1d);
    }
    else {
      fn_80202810(r3,0x1d);
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x24);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0x24);
    cVar15 = fn_80202234(r3,0x24);
    if ((char)(cVar14 + 1) < cVar15) {
      fn_80201FDC(r3,0x24);
    }
    else {
      fn_80202810(r3,0x24);
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x30);
  if (cVar14 == 1) {
    cVar14 = fn_80202108(r3,0x30);
    cVar15 = fn_80202234(r3,0x30);
    if ((char)(cVar14 + 1) < cVar15) {
      fn_80201FDC(r3,0x30);
    }
    else {
      fn_80202810(r3,0x30);
    }
  }
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar14 = fn_802026E4(r3,0x26);
  if (cVar14 != 1) goto LAB_0022e680;
  cVar14 = fn_80202108(r3,0x26);
  cVar15 = fn_80202234(r3,0x26);
  if ((char)(cVar14 + 1) < cVar15) {
    fn_80201FDC(r3,0x26);
    goto LAB_0022e680;
  }
  fn_80202810(r3,0x26);
  cVar14 = fn_80203CCC(r3);
  if ((((cVar14 != 1) || (sVar9 == 0x48)) || (sVar9 == 0xf)) ||
     (uVar4 = fn_801F37B0(0,0x8021c638,r3,1), iVar5 = -(uVar4 & 0xff) + 1,
     (iVar5 - ((u32)(iVar5 == 0) + -(uVar4 & 0xff)) & 0xff) != 0)) goto LAB_0022e680;
  cVar14 = fn_801F54A4(0,0,0x34,0);
  cVar15 = fn_80077B60();
  fn_80077B3C();
  if ((cVar14 == 1) && (cVar15 != 1)) {
    sVar9 = 0;
    uVar2 = fn_801F025C(2,r3);
    uVar13 = fn_801F54A4(0,0,0x16,0);
    fn_801F54A4(0,0,0x17,0);
    for (uVar16 = 0; uVar16 < uVar13; uVar16 = uVar16 + 1) {
      iVar5 = fn_801F7258(uVar2,uVar16);
      if (iVar5 != 0) {
        for (uVar4 = 0; (uVar4 & 0xffff) < 6; uVar4 = uVar4 + 1) {
          iVar8 = fn_801F986C(iVar5,uVar4);
          if (((iVar8 != 0) && (cVar14 = fn_80206608(), cVar14 != 0)) &&
             (cVar14 = fn_80202ADC(iVar8,8), cVar14 == 1)) {
            sVar9 = sVar9 + 1;
          }
        }
      }
    }
    if (sVar9 == 0) goto LAB_0022e5dc;
    bVar1 = 1;
  }
  else {
LAB_0022e5dc:
    bVar1 = 0;
  }
  if ((!bVar1) && (cVar14 = fn_802025B8(r3,8), cVar14 == 2)) {
    fn_8020248C(r3,8,0);
    fn_80200B10(r3);
    cVar14 = fn_801FECD4(r3);
    if (cVar14 == 1) {
      fn_801FE7EC(r3,0x7c,0,0);
    }
    fn_801F4C14(0,0,0x47,0,r3);
    fn_80211B94(lbl_8047B62C,0x80379464,0);
  }
LAB_0022e680:
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  fn_801DA7AC();
  cVar14 = fn_802062FC(r3);
  if (cVar14 != 0) {
    lbl_8047B618 = lbl_8047B618 | 0x1000020;
    fn_8022EDEC(r3,1);
    lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
    fn_801DA7AC();
  }
  return 1;
}
/* =========================================================================
 * fn_80239984 - PreBattleSetup
 *
 * 491 call sites. One of the most important battle flow functions.
 * Sets up the pre-battle state for a trainer encounter.
 *
 * @param context     Battle/scene context
 * @param trainerSlot Trainer slot to fight
 * @param sequenceId  Sequence type (0xF1-0xF4)
 * @return            Battle setup handle
 * ========================================================================= */
/* Address: 0x80239984 | Size: 0x74 | Ghidra import */
int fn_80239984(void)

{
    int r3;
    u32 r4;
    u32 r5;

    extern int fn_801FB1C0();
  int iVar1;
  
  iVar1 = fn_801FB1C0(0,r5,0x3e,0);
  if (iVar1 < 1) {
    if ((iVar1 < 0) && (r3 = r3 + iVar1, r3 < -200)) {
      r3 = -200;
    }
  }
  else {
    r3 = r3 + iVar1;
    if (200 < r3) {
      r3 = 200;
    }
  }
  return r3;
}
/* =========================================================================
 * fn_80239EE8 - BattleSequenceStart
 *
 * 491 call sites. Launches a battle with full configuration.
 *
 * Parameters visible from calling patterns:
 *   r3 = battle ID (e.g., 0xEC64 = large constant)
 *   r4 = trainer slot
 *   r5 = Pokemon pointer (from fn_80205B8C)
 *   r6 = 0 (flags)
 *   r7 = 0 (flags)
 *   r8 = context (from caller)
 *   r9 = 0 (reserved)
 *   r10 = sequence ID (0xF1, 0xF2, 0xF3, 0xF4)
 *
 * The constant 0xEC64 (decimal 60516) appears to be a standard battle
 * configuration ID. It's loaded via:
 *   lis r6, 0x1
 *   subi r3, r6, 0x139c  -> 0x10000 - 0x139C = 0xEC64
 * ========================================================================= */
/* Address: 0x80239EE8 | Size: 0x230 | Ghidra import */
u32 fn_80239EE8(u32 r3, u32 r4, u32 r5, u32 r6, u32 r7, u32 r8, u32 r9, u32 r10)

{
    extern u8 fn_80008164();
    extern u32 fn_800FA280();
    extern void fn_80103BA8();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern u32 fn_801FB1C0();
    extern void fn_8026246C();
    extern u8 fn_802624CC();
  u32 uVar3;
  u32 iVar1;
  u32 iVar2;
  u8 cVar5;
  u16 local_58 [20];

  iVar1 = fn_801FB1C0(0,r10,0x40,0);
  iVar2 = fn_801FB1C0(0,r10,0x41,0);
  uVar3 = fn_801FB1C0(0,r10,0x3e,0);
  if (iVar1 != 0) {
    fn_80132A38(0xd,fn_800FA280(iVar1));
  }
  if (iVar2 != 0) {
    fn_80132A38(0xe,fn_800FA280(iVar2));
  }
  if (r4 != 0) {
    fn_80132A38(0x13,fn_801F8100(r4));
  }
  if (r5 != 0) {
    fn_80132A38(0x14,(int)fn_8012640C(r5,0,0x77,0));
  }
  if (r6 != 0) {
    fn_80132A38(0x23,fn_801F8100(r6));
  }
  if (r7 != 0) {
    fn_80132A38(0x15,(int)fn_8012640C(r7,0,0x77,0));
  }
  if ((r8 & 0xffff) != 0) {
    fn_8011BEB4(0,r8,1,0);
    fn_80132A38(0x28,fn_800FA280());
  }
  if ((r9 & 0xffff) != 0) {
    itemGetStatus(0,r9,1,0);
    fn_80132A38(0x29,fn_800FA280());
  }
  if ((r10 & 0xffff) != 0) {
    fn_801FB1C0(0,r10,0x3f,0);
    fn_80132A38(0x41,fn_800FA280());
  }
  fn_80132A38(0x2f,uVar3);
  cVar5 = fn_80008164();
  if (cVar5 == 1) {
    fn_80103BA8(local_58,1);
    if ((local_58[0] & 0x800) != 0) {
      return 0;
    }
    cVar5 = fn_802624CC(r3);
    if (cVar5 == 1) {
      fn_8026246C();
      return 0;
    }
  }
  return 0;
}
/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 1 functions matched
 * =================================================================== */

/* Address: 0x8023C368 | Size: 0x8 | Pattern: return_constant */
u32 fn_8023C368(void) { return 0; }

/* ===================================================================
 * EXPANDED FUNCTION COVERAGE
 * 243 additional functions for 0x80220000-0x80240000
 * =================================================================== */

/* -------------------------------------------------------------------
 * Colosseum Setup (0x80220000-0x80222000)
 * 5 functions
 * ------------------------------------------------------------------- */

/* Address: 0x80220778 | Size: 0x5c | Ghidra import */
u32 fn_80220778(u32 r3)

{
    extern int fn_801FEF74();
    extern u8 fn_802026E4();
    extern void fn_80202810();
  u8 cVar2;
  int iVar1;

  cVar2 = fn_802026E4(r3,0x14);
  if ((cVar2 == 1) && (iVar1 = fn_801FEF74(r3), iVar1 <= 0)) {
    fn_80202810(r3,0x14);
  }
  return 1;
}
/* Address: 0x802207D4 | Size: 0x94 | Ghidra import */
u32 fn_802207D4(u32 r3)

{
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_8020147C();
  u16 sVar1;
  u8 cVar2;

  sVar1 = (int)fn_8012640C(r3,0,0xfb,0);
  if (sVar1 != 0) {
    fn_8020147C(r3,sVar1,1,0);
    fn_801254B4(r3,0,0xfb,0,0);
    cVar2 = fn_801FECD4(r3);
    if (cVar2 == 1) {
      fn_801FE7EC(r3,0x82,0,0);
    }
  }
  return 1;
}
/* Address: 0x80220868 | Size: 0x2b4 | Ghidra import */
void fn_80220868(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_801F6E98();
    extern void fn_80220B8C();
    extern u8 lbl_8047B64A;
  u8 bVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  u32 uVar5;
  short sVar7;
  u32 uVar6;
  s8 cVar8;
  s8 cVar9;
  u8 bVar10;

  int iVar11;
  int iVar12;
  u32 uVar13;
  
  uVar13 = 0;
  iVar12 = 0;
  iVar11 = 0;
  uVar4 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  uVar5 = fn_801F025C(2,uVar4);
  sVar7 = fn_80207BF4(uVar4);
  bVar1 = *(u8 *)(*(int *)(lbl_8047B610) + 3);
  uVar6 = (u32)*(u8 *)(*(int *)(lbl_8047B610) + 2);
  if ((bVar1 & 1) == 0) {
    iVar3 = 0xe;
    if ((bVar1 & 2) != 0) {
      iVar3 = 0x26;
    }
    for (; uVar6 != 0; uVar6 = (int)uVar6 >> 1) {
      if ((uVar6 & 1) != 0) {
        uVar2 = uVar13 & 0xff;
        if (uVar2 < 8) {

          ((int (*)(void))**(void ***)(uVar2 * 4 + -0x7fc65f10))();
          return;
        }
        bVar10 = (int)fn_8012640C(uVar4,0,0,0);
        if (bVar10 < 0xc) {
          iVar12 = iVar3 + uVar2;
          iVar11 = iVar11 + 1;
        }
      }
      uVar13 = uVar13 + 1;
    }
    if (1 < iVar11) {
      if ((bVar1 & 2) == 0) {
        iVar12 = 0x37;
      }
      else {
        iVar12 = 0x38;
      }
    }
  }
  else {
    iVar3 = 0x15;
    if ((bVar1 & 2) != 0) {
      iVar3 = 0x2d;
    }
    for (; uVar6 != 0; uVar6 = (int)uVar6 >> 1) {
      if ((uVar6 & 1) != 0) {
        uVar2 = uVar13 & 0xff;
        if (uVar2 < 8) {

          ((int (*)(void))**(void ***)(uVar2 * 4 + -0x7fc65ef0))();
          return;
        }
        cVar8 = (int)fn_8012640C(uVar4,0,0,0);
        if ((bVar1 & 8) == 0) {
          cVar9 = fn_801F6E98(uVar5,0x4c);
          if ((((cVar9 == 0) && (sVar7 != 0x1d)) && (sVar7 != 0x49)) && (cVar8 != 0)) {
            iVar12 = iVar3 + uVar2;
            iVar11 = iVar11 + 1;
          }
        }
        else if (cVar8 != 0) {
          iVar12 = iVar3 + uVar2;
          iVar11 = iVar11 + 1;
        }
      }
      uVar13 = uVar13 + 1;
    }
    if (1 < iVar11) {
      if ((bVar1 & 2) == 0) {
        iVar12 = 0x39;
      }
      else {
        iVar12 = 0x3a;
      }
    }
  }
  if (((bVar1 & 4) == 0) || (1 < iVar11)) {
    if ((iVar11 == 0) || ((char)lbl_8047B64A != 0)) {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 4;
    }
    else {
      fn_80220B8C(iVar12);
      if (((bVar1 & 4) != 0) && (1 < iVar11)) {
        lbl_8047B64A = 1;
      }
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 4;
    }
  }
  else {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 4;
  }
  return;
}
/* Address: 0x80220B8C | Size: 0x164 | Ghidra import */
void fn_80220B8C(void)

{
    u32 r3;

    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_80265598();
  u32 uVar1;
  u32 uVar3;
  int iVar2;
  s8 cVar4;

  uVar1 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  r3 = r3 & 0xffff;
  if ((r3 == 0x37) || (r3 == 0x38)) {
    uVar3 = fn_801F54A4(0,0,0x14,0);
    iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
    if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x2c,4,0), cVar4 != 0)) {
      fn_801DA9E8(iVar2,0x2c,4);
      fn_80265598(uVar1,uVar3,1);
    }
  }
  else if ((r3 == 0x39) || (r3 == 0x3a)) {
    uVar3 = fn_801F54A4(0,0,0x14,0);
    iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
    if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x3c,4,0), cVar4 != 0)) {
      fn_801DA9E8(iVar2,0x3c,4);
      fn_80265598(uVar1,uVar3,1);
    }
  }
  else if (r3 - 0xf < 0x26) {

    ((int (*)(void))**(void ***)((r3 - 0xf) * 4 + -0x7fc65ed0))();
    return;
  }
  return;
}
/* Address: 0x8022106C | Size: 0x98 */
#pragma optimization_level 0
void fn_8022106C(void* ctx, u32 param1, u32 param2) {
    extern u8 lbl_80379F58[];
    u32 tmp = 0;
    u32 r6 = 0;
    u32 r7 = 0;

ctx = (void*)(u32)lbl_80379F58;
    r7 = 0x0;
ctx = (void*)(u32)lbl_80379F58;
ctx = (void*)((void*)((u32)(u32)ctx + (0x1 << 16)));
ctx = (void*)((void*)*(u8*)((u8*)(u32)ctx + 0x601E));
tmp = (u32)ctx & 0x000000F0;
    switch ((s32)tmp) {
        case 0x10: r7 = 0xf; break;
        case 0x20: r7 = 0x27; break;
        case 0x90: r7 = 0x16; break;
        case 0xa0: r7 = 0x2e; break;
        default: break;
    }
r6 = (u32)ctx & 0xF;
ctx = (void*)*(u32*)&lbl_8047B610;
    param2 = (u32)lbl_80379F58;
    param1 = 0x0;
tmp = (u32)ctx + 0x1;
ctx = (void*)(u32)lbl_80379F58;
    *(u32*)&lbl_8047B610 = tmp;
ctx = (void*)((void*)((u32)(u32)ctx + (0x1 << 16)));
    tmp = r7 + r6;
    *(u8*)((u8*)ctx + 0x60A4) = tmp;
    *(u8*)((u8*)ctx + 0x60A5) = param1;
    return;
}
#pragma optimization_level 4

/* -------------------------------------------------------------------
 * Script Interpreter Helpers (0x80222000-0x80226000)
 * 53 functions
 * ------------------------------------------------------------------- */

/* Address: 0x80222110 | Size: 0xdc | Ghidra import */
#pragma push
#pragma peephole off
u32 fn_80222110(void)

{
    u32 r3;
    char r4;
    char r5;

  u32 uVar1;
  
  uVar1 = r3 & 0xff;
  if ((uVar1 == 0xf) || (uVar1 == 0x27)) {
    r5 = 1;
  }
  else if ((uVar1 == 0x16) || (uVar1 == 0x2e)) {
    r5 = 0;
  }
  if ((r4 == 1) || (r4 == 4)) {
    if (r5 == 1) {
      r3 = 0x26;
    }
    else {
      r3 = 0x27;
    }
  }
  if (((r4 == 2) || (r4 == 5)) || (r4 == '\a')) {
    if (r5 == 1) {
      r3 = 0x28;
    }
    else {
      r3 = 0x29;
    }
  }
  if (r4 == 3) {
    if (r5 == 1) {
      r3 = 0x2a;
    }
    else {
      r3 = 0x2b;
    }
  }
  if (r4 != 6) {
    return r3;
  }
  if (r5 == 1) {
    return 0x2c;
  }
  return 0x3c;
}
#pragma pop
/* Address: 0x802221EC | Size: 0x108 | Ghidra import */
void fn_802221EC(u32 r3, u32 r4, u8 r5, u8 r6)

{
    extern void _threadSwitch();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_801F54A4();
    extern void fn_8026532C();
    extern void fn_80265598();
  int iVar1;
  u16 uVar2;
  u8 cVar3;

  uVar2 = fn_801F54A4(0,0,0x14,0);
  iVar1 = (int)fn_8012640C(r4,0,0xee,0);
  if ((iVar1 != 0) && (cVar3 = fn_801DDD28(iVar1,r3,4,0), cVar3 != 0)) {
    fn_801DA9E8(iVar1,r3,4);
    if (r6 == 1) {
      fn_80265598(r4,uVar2,1);
    }
    if (r5 == 1) {
      while (1) {
        cVar3 = fn_801DA94C(iVar1,r3 & 0xffff,4);
        if (cVar3 == 0) break;
        _threadSwitch();
      }
      fn_801DA8C4(iVar1,r3 & 0xffff,4);
      if (r6 == 1) {
        fn_8026532C(r4,uVar2,0);
      }
    }
  }
  return;
}
/* Address: 0x802222F4 | Size: 0x7c | Ghidra import */
void fn_802222F4(void)

{
    extern void fn_801F025C();
    extern void fn_80221104();
  u8 uVar1;
  int iVar2;
  u16 uVar3;

  fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  iVar2 = *(int *)(lbl_8047B610);
  if (*(u8 **)(iVar2 + 2) == (void *)0) {
    uVar1 = 0;
  }
  else {
    uVar1 = **(u8 **)(iVar2 + 2);
  }
  if (*(u16 **)(iVar2 + 6) == (void *)0) {
    uVar3 = 0;
  }
  else {
    uVar3 = **(u16 **)(iVar2 + 6);
  }
  fn_80221104(*(u8 *)(iVar2 + 1),uVar1,uVar3);
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
  return;
}
/* Address: 0x80222370 | Size: 0x64 | Ghidra import */
void fn_80222370(void)

{
    extern void fn_801F025C();
    extern void fn_80221104();
  u16 uVar1;
  int iVar2;

  fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  iVar2 = *(int *)(lbl_8047B610);
  if (*(u16 **)(iVar2 + 3) == (void *)0) {
    uVar1 = 0;
  }
  else {
    uVar1 = **(u16 **)(iVar2 + 3);
  }
  fn_80221104(*(u8 *)(iVar2 + 1),*(u8 *)(iVar2 + 2),uVar1);
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
  return;
}
/* Address: 0x802223D4 | Size: 0xC | Pattern: simple_setter */
void fn_802223D4(void) { lbl_8047B614 = 2; }

/* Address: 0x802223E0 | Size: 0x58 | Ghidra import */
void fn_802223E0(void)

{
    extern u32 fn_801F349C();
  u32 iVar1;

  iVar1 = fn_801F349C(0,*(u8 *)(lbl_8047B610 + 1),0,0,0);
  if (iVar1 != 0) {
    iVar1 = *(u32 *)(lbl_8047B610 + 2);
  }
  else {
    iVar1 = lbl_8047B610 + 6;
  }
  lbl_8047B610 = iVar1;
  return;
}
/* Address: 0x80222438 | Size: 0x5c | Ghidra import */
void fn_80222438(void)
{
    extern u32 fn_801F025C();
    extern u8 fn_80207AE0();
  int iVar1;
  u32 uVar2;
  u8 cVar3;

  uVar2 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  cVar3 = fn_80207AE0(uVar2,*(u8 *)(lbl_8047B610 + 2));
  if (cVar3 == 1) {
    iVar1 = *(int *)(lbl_8047B610 + 3);
  }
  else {
    iVar1 = lbl_8047B610 + 7;
  }
  lbl_8047B610 = iVar1;
  return;
}
/* Address: 0x80222494 | Size: 0x3c | Ghidra import */
void fn_80222494(void)

{
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;

  fn_80211B94(lbl_8047B62C,
               *(u32 *)(lbl_8047B610 + 1),0);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x802224D0 | Size: 0xC | Pattern: simple_setter */
void fn_802224D0(void) { lbl_8047B614 = 2; }

/* Address: 0x802224DC | Size: 0xC | Pattern: simple_setter */
void fn_802224DC(void) { lbl_8047B614 = 1; }

/* Address: 0x802224E8 | Size: 0xC | Pattern: simple_setter */
void fn_802224E8(void) { lbl_8047B614 = 1; }

/* Address: 0x802224F4 | Size: 0xC | Pattern: simple_setter */
void fn_802224F4(void) { lbl_8047B614 = 2; }

/* Address: 0x80222500 | Size: 0x10 | Pattern: sda_getter */
void fn_80222500(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* Address: 0x80222510 | Size: 0x10 | Pattern: sda_getter */
void fn_80222510(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x80222520 | Size: 0x34 | Ghidra import */
void fn_80222520(void)

{
    extern void fn_801F000C();

  fn_801F000C(*(u16 *)(lbl_8047B610 + 1));
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* Address: 0x80222554 | Size: 0x30 | Pattern: simple_wrapper */
void fn_80222554(void)
{
  u32 t = lbl_8047B610;
  u32 *ptr = *(u32 **)(t+1);
  u32 val = *(u32 *)(t+5);
  *ptr &= ~val;
  lbl_8047B610 = lbl_8047B610 + 9;
}

/* Address: 0x80222584 | Size: 0x2C | Pattern: simple_wrapper */
void fn_80222584(void)
{
  u32 t = lbl_8047B610;
  u16 *ptr = *(u16 **)(t+1);
  u16 val = *(u16 *)(t+5);
  *ptr = *ptr & (val ^ 0xffff);
  lbl_8047B610 = lbl_8047B610 + 7;
}

/* Address: 0x802225B0 | Size: 0x2C | Pattern: simple_wrapper */
void fn_802225B0(void)
{
  u32 t = lbl_8047B610;
  u8 *ptr = *(u8 **)(t+1);
  u8 val = *(u8 *)(t+5);
  *ptr = *ptr & (val ^ 0xff);
  lbl_8047B610 = lbl_8047B610 + 6;
}

/* Address: 0x802225DC | Size: 0x28 | Pattern: simple_wrapper */
void fn_802225DC(void)
{
  u32 t = lbl_8047B610;
  u32 *ptr = *(u32 **)(t+1);
  u32 val = *(u32 *)(t+5);
  *ptr |= val;
  lbl_8047B610 = lbl_8047B610 + 9;
}

/* Address: 0x80222604 | Size: 0x28 | Pattern: simple_wrapper */
void fn_80222604(void)
{
  u32 t = lbl_8047B610;
  u16 *ptr = *(u16 **)(t+1);
  u16 val = *(u16 *)(t+5);
  *ptr |= val;
  lbl_8047B610 = lbl_8047B610 + 7;
}

/* Address: 0x8022262C | Size: 0x28 | Pattern: simple_wrapper */
void fn_8022262C(void)
{
  u32 t = lbl_8047B610;
  u8 *ptr = *(u8 **)(t+1);
  u8 val = *(u8 *)(t+5);
  *ptr |= val;
  lbl_8047B610 = lbl_8047B610 + 6;
}

/* Address: 0x80222654 | Size: 0x50 | Pattern: field_accessor */
#pragma optimization_level 0
void fn_80222654(void* ctx, u32 slot, u32 param) {
#pragma optimization_level 4
    u8 *dst;
    u8 *src;
    s32 i = 0;
    u8 *tbl;
    s32 n;
    u8 *base = (u8*)lbl_8047B610;

    n = *(u8 *)(base + 0xd);
    dst = *(u8 **)(base + 0x1);
    tbl = *(u8 **)(base + 0x5);
    src = *(u8 **)(base + 0x9);

    for (; i < n; i++) {
        *dst = tbl[i + *src];
        dst++;
    }
    lbl_8047B610 = lbl_8047B610 + 0xe;
}
#pragma optimization_level 4

/* Address: 0x802226A4 | Size: 0x48 | Pattern: field_accessor */
#pragma optimization_level 0
void fn_802226A4(void* ctx, u32 slot, u32 param) {
    u32 tmp = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

ctx = (void*)*(u32*)&lbl_8047B610;
    slot = *(u32*)((u8*)ctx + 0x1);
    param = *(u32*)((u8*)ctx + 0x5);
    tmp = *(u8*)((u8*)ctx + 0x9);
ctx = (void*)(u32)slot;
    slot = param;
    ctr_fn = (void(*)(void))tmp;
    if ((s32)tmp > 0) {
        do {
            tmp = *(u8*)((u8*)slot + 0x0);
            slot = slot + 0x1;
            *(u8*)((u8*)ctx + 0x0) = tmp;
        ctx = (void*)((void*)((u32)(u32)ctx + 0x1));
        } while (--ctr != 0);
    }
ctx = (void*)*(u32*)&lbl_8047B610;
tmp = (u32)ctx + 0xa;
    *(u32*)&lbl_8047B610 = tmp;
    return;
}
#pragma optimization_level 4

/* Address: 0x802226EC | Size: 0x28 | Pattern: simple_wrapper */
void fn_802226EC(void)
{
  u32 t = lbl_8047B610;
  u8 *ptr = *(u8 **)(t+1);
  u8 val = *(u8 *)(t+5);
  *ptr -= val;
  lbl_8047B610 = lbl_8047B610 + 6;
}

/* Address: 0x80222714 | Size: 0x28 | Pattern: simple_wrapper */
void fn_80222714(void)
{
  u32 t = lbl_8047B610;
  u8 *ptr = *(u8 **)(t+1);
  u8 val = *(u8 *)(t+5);
  *ptr += val;
  lbl_8047B610 = lbl_8047B610 + 6;
}

/* Address: 0x8022273C | Size: 0x20 | Pattern: null_check_getter */
void fn_8022273C(void)
{
  u32 t = lbl_8047B610;
  u8 val = *(u8 *)(t+5);
  u8 *ptr = *(u8 **)(t+1);
  *ptr = val;
  lbl_8047B610 = lbl_8047B610 + 6;
}

/* Address: 0x8022275C | Size: 0x78 | Pattern: field_accessor */
#pragma optimization_level 0
void fn_8022275C(void* ctx, u32 slot, u32 param) {
    u32 tmp = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;

    slot = *(u32*)&lbl_8047B610;
    r9 = 0x0;
    r8 = 0x0;
ctx = (void*)*(u8*)((u8*)slot + 0x9);
    param = *(u32*)((u8*)slot + 0x1);
    r6 = *(u32*)((u8*)slot + 0x5);
    r7 = *(u32*)((u8*)slot + 0xA);
    while (1) {
        tmp = r8 & 0xFF;
        if (tmp >= (u32)ctx) break;
        slot = *(u8*)((u8*)param + 0x0);
        tmp = *(u8*)((u8*)r6 + 0x0);
        if (slot == tmp) {
            slot = r9 & 0xFF;
            tmp = slot + 0x1;
            r9 = tmp & 0xFF;
        }
        param = param + 0x1;
        r6 = r6 + 0x1;
        r8 = r8 + 0x1;

    }
    tmp = r9 & 0xFF;
if (tmp != (u32)ctx) {
        tmp = r7;

} else {
    ctx = (void*)*(u32*)&lbl_8047B610;
    tmp = (u32)ctx + 0xe;
}
    *(u32*)&lbl_8047B610 = tmp;
    return;
}
#pragma optimization_level 4

/* Address: 0x802227D4 | Size: 0x70 | Pattern: field_accessor */
#pragma optimization_level 0
void fn_802227D4(void* ctx, u32 slot, u32 param) {
    u32 tmp = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;

ctx = (void*)*(u32*)&lbl_8047B610;
    r9 = 0x0;
    param = *(u8*)((u8*)ctx + 0x9);
    r6 = *(u32*)((u8*)ctx + 0x1);
    r7 = *(u32*)((u8*)ctx + 0x5);
    tmp = param & 0xFF;
    r8 = *(u32*)((u8*)ctx + 0xA);
    while (1) {
        ctx = (void*)(r9 & 0xFF);
        if ((u32)ctx >= tmp) break;
        slot = *(u8*)((u8*)r6 + 0x0);
    ctx = (void*)*(u8*)((u8*)r7 + 0x0);
    if (slot != (u32)ctx) {
        ctx = (void*)*(u32*)&lbl_8047B610;
        tmp = (u32)ctx + 0xe;
            *(u32*)&lbl_8047B610 = tmp;
            break;
    }
        r6 = r6 + 0x1;
        r7 = r7 + 0x1;
        r9 = r9 + 0x1;

    }

ctx = (void*)(r9 & 0xFF);
    tmp = param & 0xFF;
    if ((u32)ctx != (u32)tmp) return;
    *(u32*)&lbl_8047B610 = r8;
    return;
}
#pragma optimization_level 4

/* Address: 0x80222844 | Size: 0xC8 (200 bytes) */
#pragma optimization_level 0
void fn_80222844(void* ctx, u32 param1, u32 param2, u32 param3) {
#pragma optimization_level 4
    int sel;
    u32 *base;
    u32 *pa;
    u32 vb;
    u32 nxt;

    base = *(u32**)&lbl_8047B610;
    sel = *(u8*)((u8*)base + 0x1);
    pa = *(u32**)((u8*)base + 0x2);
    vb = *(u32*)((u8*)base + 0x6);
    nxt = *(u32*)((u8*)base + 0xA);
    *(u32*)&lbl_8047B610 = (u32)base + 0xe;
    switch (sel) {
    case 0:
        if (*pa != vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 1:
        if (*pa == vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 2:
        if (*pa <= vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 3:
        if (*pa >= vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 4:
        if ((*pa & vb) == 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 5:
        if ((*pa & vb) != 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    }
    return;
}
#pragma optimization_level 4

/* Address: 0x8022290C | Size: 0xE0 (224 bytes) */
#pragma optimization_level 0
void fn_8022290C(void* ctx, u32 param1, u32 param2, u32 param3) {
    param2 = lbl_8047B610;
    ctx = (void*)*(u8*)((u8*)param2 + 0x1);
    param3 = *(u32*)((u8*)param2 + 0x2);
    param1 = *(u16*)((u8*)param2 + 0x6);
    lbl_8047B610 = param2 + 0xc;
    param2 = *(u32*)((u8*)param2 + 0x8);
    if ((s32)ctx != 3) {
        if ((s32)ctx < 3) {
            if ((s32)ctx != 1) {
                if ((s32)ctx < 1) {
                    if ((s32)ctx < 0) {
                        return;
                    }
                    if ((s32)ctx != 5) {
                        if ((s32)ctx >= (s32)0x5) return;
                        goto L_802229BC;
                        }
                ctx = (void*)*(u16*)((u8*)param3 + 0x0);
                    if (!((u32)ctx & (param1 & 0xFFFF))) return;
                    lbl_8047B610 = param2;
                    return;
                    }
            ctx = (void*)*(u16*)((u8*)param3 + 0x0);
                if ((u32)ctx == (u32)(param1 & 0xFFFF)) return;
                lbl_8047B610 = param2;
                return;
                    }
        ctx = (void*)*(u16*)((u8*)param3 + 0x0);
            if ((u32)ctx <= (u32)(param1 & 0xFFFF)) return;
            lbl_8047B610 = param2;
            return;
        }
    ctx = (void*)*(u16*)((u8*)param3 + 0x0);
        if ((u32)ctx >= (u32)(param1 & 0xFFFF)) return;
        lbl_8047B610 = param2;
        return;
    L_802229BC:
    ctx = (void*)*(u16*)((u8*)param3 + 0x0);
        if ((u32)ctx & (param1 & 0xFFFF)) return;
        lbl_8047B610 = param2;
        return;
                    }
ctx = (void*)*(u16*)((u8*)param3 + 0x0);
    if (!((u32)ctx & (param1 & 0xFFFF))) return;
    lbl_8047B610 = param2;
    return;
}
#pragma optimization_level 4

/* Address: 0x802229EC | Size: 0xE0 (224 bytes) */
#pragma optimization_level 0
void fn_802229EC(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 tmp = 0;

    param2 = *(u32*)&lbl_8047B610;
ctx = (void*)*(u8*)((u8*)param2 + 0x1);
    tmp = param2 + 0xb;
    param3 = *(u32*)((u8*)param2 + 0x2);
    param1 = *(u8*)((u8*)param2 + 0x6);
    param2 = *(u32*)((u8*)param2 + 0x7);
    *(u32*)&lbl_8047B610 = tmp;
    if ((s32)ctx != 3) {
        if ((s32)ctx < 3) {
            if ((s32)ctx != 1) {
                if ((s32)ctx < 1) {
                    if ((s32)ctx < 0) {
                        return;
                    }
                    if ((s32)ctx != 5) {
                        if ((s32)ctx >= (s32)0x5) return;
                        goto L_80222A9C;
                        }
                ctx = (void*)*(u8*)((u8*)param3 + 0x0);
                    tmp = param1 & 0xFF;
                    if ((u32)ctx != (u32)tmp) return;
                    *(u32*)&lbl_8047B610 = param2;
                    return;
                    }
            ctx = (void*)*(u8*)((u8*)param3 + 0x0);
                tmp = param1 & 0xFF;
                if ((u32)ctx == (u32)tmp) return;
                *(u32*)&lbl_8047B610 = param2;
                return;
                    }
        ctx = (void*)*(u8*)((u8*)param3 + 0x0);
            tmp = param1 & 0xFF;
            if ((u32)ctx <= (u32)tmp) return;
            *(u32*)&lbl_8047B610 = param2;
            return;
        }
    ctx = (void*)*(u8*)((u8*)param3 + 0x0);
        tmp = param1 & 0xFF;
        if ((u32)ctx >= (u32)tmp) return;
        *(u32*)&lbl_8047B610 = param2;
        return;
    L_80222A9C:
    ctx = (void*)*(u8*)((u8*)param3 + 0x0);
        tmp = param1 & 0xFF;
        /* and. tmp, ctx, tmp */;
        if ((u32)ctx == (u32)tmp) return;
        *(u32*)&lbl_8047B610 = param2;
        return;
                    }
ctx = (void*)*(u8*)((u8*)param3 + 0x0);
    tmp = param1 & 0xFF;
    /* and. tmp, ctx, tmp */;
    if ((u32)ctx != (u32)tmp) return;
    *(u32*)&lbl_8047B610 = param2;
    return;
}
#pragma optimization_level 4

/* Address: 0x80222ACC | Size: 0x10 | Pattern: sda_getter */
u32 fn_80222ACC(void) { u32 r = lbl_8047B610; lbl_8047B610 = *(u32 *)(r + 1); return r; }

/* Address: 0x80222ADC | Size: 0xa0 | Ghidra import */
void fn_80222ADC(void)

{
    extern void fn_8011BBD8(u32, u32, u32, u32, s16);
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
  u32 uVar2;
  u16 uVar3;
  int sVar4;

  uVar2 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar3 = fn_8011BEB4(uVar2,0,0x31,0);
  sVar4 = (uVar3 & 0xff) - 1;
  if ((short)sVar4 < 0) {
    sVar4 = 0;
  }
  if ((short)sVar4 == 0) {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  fn_8011BBD8(uVar2,0,0x31,0,(s16)sVar4);
  return;
}
/* Address: 0x80222B7C | Size: 0x5c | Ghidra import */
void fn_80222B7C(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
  u32 uVar1;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  fn_8011BBD8(uVar1,0,0x31,0,*(u8 *)(lbl_8047B610 + 1));
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x80222BD8 | Size: 0x6c | Ghidra import */
void fn_80222BD8(void)

{
    extern u32 fn_801F025C();
    extern void fn_80209F18();
    extern u8 lbl_80478D7B;
    extern u8 lbl_80478D7E;
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 uVar2;

  uVar2 = fn_801F025C(0x11,0);
  fn_8012640C(uVar2,0,0xd9,0);
  fn_80209F18();
  uVar1 = lbl_8047B618;
  lbl_80478D7B = 0;
  lbl_8047B618 = uVar1 & 0xffffffbf;
  lbl_80478D7E = 0;
  lbl_8047B618 = uVar1 & 0xffffbfbf;
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x80222C44 | Size: 0x2ac | Ghidra import */
void fn_80222C44(void)

{
    extern u32 fn_80077AF4();
    extern short fn_8011BEB4();
    extern void fn_801C2D5C();
    extern void fn_801C2D68();
    extern void fn_801C3430();
    extern short fn_801EF634();
    extern void fn_801EF95C();
    extern u32 fn_801F025C();
    extern s8 fn_801F1700();
    extern void fn_801F2B5C();
    extern s8 fn_801F32B0();
    extern void fn_801F37B0();
    extern void fn_801F3984();
    extern u32 fn_801F54A4();
    extern int fn_801F7174();
    extern void fn_80222EF0();
    extern void fn_802230BC();
    extern void fn_80261954();
    extern void fn_80261AD0();
    extern void fn_802658C8();
    extern void fn_802659A4();
    extern void fn_802659F8();
    extern void fn_80265A6C();
  u8 bVar1;
  u32 uVar3;
  u32 uVar4;
  u32 uVar2;
  u16 uVar8;
  u16 uVar9;
  u32 uVar5;
  u32 iVar6;
  u16 sVar10;
  u8 cVar11;
  u32 cVar12;
  u32 iVar7;

  uVar2 = *(u32 *)(lbl_8047B610 + 1) & 0xff;
  uVar3 = fn_801F025C(0x11,0);
  uVar4 = fn_80205184();
  uVar8 = fn_801F54A4(0,0,0x16,0);
  uVar9 = fn_801F54A4(0,0,0x17,0);
  uVar5 = fn_801F025C(5,0);
  iVar6 = fn_801F7174(uVar5,uVar8,uVar9);
  if (iVar6 == 0) {
    fn_801F3984(0,2);
  }
  uVar5 = fn_801F025C(4,0);
  iVar6 = fn_801F7174(uVar5,uVar8,uVar9);
  if (iVar6 == 0) {
    fn_801F3984(0,3);
  }
  sVar10 = fn_801EF634();
  if (sVar10 != 0) {
    uVar5 = fn_801F54A4(0,0,0x34,0) & 0xff;
    cVar12 = fn_80077AF4();
    sVar10 = fn_8011BEB4(0,uVar4,9,0);
    if (((uVar5 == 1) && ((u8)cVar12 == 1)) && (sVar10 == 7)) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if ((bVar1 == 1) && (sVar10 = fn_801EF634(), sVar10 == 7)) {
      fn_801F3984(0,0);
      iVar6 = fn_801F025C(2,uVar3);
      iVar7 = fn_801F025C(4,0);
      if (iVar7 == iVar6) {
        fn_801F3984(0,3);
      }
      else {
        fn_801F3984(0,2);
      }
    }
  }
  else {
    if (uVar2 == 1) {
      cVar11 = fn_801F32B0(0);
      if (cVar11 != 0) {
        fn_801EF95C();
        sVar10 = fn_801EF634();
        if (sVar10 != 0) {
          lbl_8047B610 = lbl_8047B610 + 5;
          return;
        }
        cVar11 = fn_801F1700(0);
        if (cVar11 == 1) {
          fn_80265A6C();
          fn_802659F8();
        }
        fn_801C3430();
        iVar6 = fn_801EF8F4(0);
        fn_80261AD0(0xffffffff);
        fn_801F2B5C(0,(u32)fn_802230BC,0,1);
        fn_80261954(0);
        if ((u8)iVar6 == 1) {
          fn_801C2D68();
        }
        else {
          fn_801C2D5C();
        }
        cVar11 = fn_801F1700(0);
        if (cVar11 == 1) {
          fn_802659A4();
          fn_802658C8();
        }
        sVar10 = fn_801EF634();
        if (sVar10 != 0) {
          lbl_8047B610 = lbl_8047B610 + 5;
          return;
        }
      }
      fn_801F37B0(0,(u32)fn_80222EF0,0,0);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80222EF0 | Size: 0x1cc | Ghidra import */
#pragma push
#pragma peephole off
u32 fn_80222EF0(u32 r3)

{
    extern void fn_80124A60();
    extern void fn_801C3430();
    extern void fn_801F4220();
    extern void fn_801F4354();
    extern void fn_801F4C14();
    extern u8 fn_802062FC();
    extern void fn_80206C94();
    extern void fn_802078F0();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
    extern u8 lbl_8037879E[];
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar6;
  u8 cVar7;
  u8 cVar8;
  int iVar5;

  fn_801F4354(0,r3);
  fn_801F4220(0,r3);
  uVar1 = (int)fn_8012640C(r3,0,0xd5,0);
  uVar2 = (int)fn_8012640C(r3,0,0xd6,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xcb,0);
  uVar4 = (int)fn_8012640C(uVar1,0,0xcc,0);
  cVar6 = (int)fn_8012640C(r3,0,0x120,0);
  cVar7 = (int)fn_8012640C(uVar2,0,0xd2,0);
  cVar8 = fn_802062FC(r3);
  if (cVar8 == 0) {
    iVar5 = (int)fn_8012640C(r3,0,0x121,0);
    if (iVar5 < 0) {
      if (cVar6 == 1) {
        fn_80206C94(r3);
        fn_801C3430();
      }
      if (cVar7 == 1) {
        fn_80124A60(uVar3);
        fn_80124A60(uVar4);
        fn_802078F0(uVar1);
        fn_802078F0(uVar2);
        fn_80206C94(r3);
        fn_801C3430();
      }
    }
    else {
      fn_801F4C14(0,0,0x45,0,r3);
      fn_801F4C14(0,0,0x46,0,r3);
      if (cVar7 == 1) {
        fn_80124A60(uVar3);
        fn_80124A60(uVar4);
        fn_802078F0(uVar1);
        fn_802078F0(uVar2);
      }
      fn_80211B94(lbl_8047B62C,(u32)lbl_8037879E,0);
    }
  }
  return 1;
}
#pragma pop
/* Address: 0x802230BC | Size: 0x238 | Ghidra import */
u32 fn_802230BC(u32 r3, u32 r4)

{
    extern u16 fn_801EF634();
    extern void fn_801F150C();
    extern u32 fn_801F54A4();
    extern u32 fn_801F85B0();
    extern int fn_801F8A18();
    extern void fn_801F8F24();
    extern int fn_801F9600();
    extern u8 fn_801FA634();
    extern int fn_801FB1C0();
    extern u8 fn_802062FC();
    extern u8 fn_80206608();
    extern u8 fn_80206780();
    extern void fn_802068C8();
  u16 sVar6;
  u8 cVar8;
  u32 uVar1;
  int iVar2;
  int iVar3;
  u16 uVar7;
  int iVar4;
  u32 uVar5;
  int iVar9;
  u32 uVar10;
  u16 local_28 [4];
  
  sVar6 = fn_801EF634();
  if ((sVar6 == 0) && (cVar8 = fn_801FA634(r3), cVar8 != 0)) {
    uVar1 = fn_801F54A4(0,0,0x18,0);
    for (uVar10 = 0; (uVar10 & 0xffff) < (uVar1 & 0xffff); uVar10 = uVar10 + 1) {
      iVar2 = fn_801FB1C0(r3,0,0x46,uVar10);
      if (((iVar2 != 0) && (cVar8 = fn_802062FC(), cVar8 != 1)) &&
         (cVar8 = (int)fn_8012640C(iVar2,0,0x119,0), cVar8 != 1)) {
        local_28[0] = 0;
        iVar3 = fn_801F8A18(r3,local_28);
        if (iVar3 == 0) {
          return 1;
        }
        iVar9 = -1;
        cVar8 = fn_80206780(iVar2);
        iVar3 = 0;
        if (cVar8 == 0) {
          uVar7 = fn_801F85B0(r3,iVar2);
          iVar4 = fn_801FB1C0(r3,0,0x45,uVar7);
          cVar8 = fn_80206608();
          if (cVar8 == 1) {
            sVar6 = (int)fn_8012640C(iVar4,0,0xce,0);
            iVar9 = (int)sVar6;
            iVar3 = iVar4;
          }
        }
        if (iVar3 == 0) {
          iVar9 = fn_801F9600(r3,r4,0,iVar2);
          if ((short)iVar9 < 0) {
            if ((short)iVar9 != -2) {
              return 1;
            }
            fn_801F150C(0);
            return 1;
          }
          fn_801F8F24(r3,iVar9);
        }
        cVar8 = fn_80206780(iVar2);
        if (cVar8 == 0) {
          uVar7 = fn_801F85B0(r3,iVar2);
          uVar5 = fn_801FB1C0(r3,0,0x45,uVar7);
          fn_802068C8(iVar2,uVar5,0);
          fn_801254B4(iVar2,0,0x120,0,1);
        }
        fn_801254B4(iVar2,0,0x121,0,(int)(short)iVar9);
        fn_801254B4(iVar2,0,0x119,0,1);
      }
    }
  }
  return 1;
}
/* Address: 0x802232F4 | Size: 0x730 | Ghidra import */
void fn_802232F4(void)

{
    extern void fn_80011B4C();
    extern void fn_80011BC4();
    extern u32 fn_800FA280();
    extern int fn_8011F77C();
    extern s8 fn_8011FC74();
    extern u32 fn_8012A5B0();
    extern void fn_80132A38();
    extern void fn_801C3430();
    extern void fn_801F000C();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern int fn_801F7258();
    extern s8 fn_801F7404();
    extern u32 fightSideGetStatus();
    extern int fn_801F8E34();
    extern s8 fn_801F9034();
    extern s8 fn_801F90C4();
    extern int fn_801F9930();
    extern u32 fn_801FB1C0();
    extern s8 fn_801FB8F8();
    extern void fn_801FE468();
    extern void fn_801FE5D4();
    extern void fn_801FE710();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_802032E4();
    extern void fn_8020341C();
    extern u32 fn_8020355C();
    extern void fn_802035BC();
    extern int fn_80203620();
    extern void fn_8020367C();
    extern u32 fn_80203848();
    extern u32 fn_80203D3C();
    extern u32 fn_80203E0C();
    extern int fn_80203E7C();
    extern short fn_802041EC();
    extern s8 fn_80204A10();
    extern s8 fn_80205904();
    extern u32 fn_80205BE8();
    extern s8 fn_80206608();
    extern void fn_80211B94();
    extern void fn_8026246C();
    extern s8 fn_802624CC();
    extern void fn_8026532C();
    extern void fn_802653FC();
    extern void fn_80265598();
    extern u32 fn_802656AC();
    extern u32 lbl_8047B62C;
    extern u32 lbl_8047B64C;
  u32 bVar1;
  u16 uVar15;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  s8 cVar17;
  u32 uVar6;
  u32 uVar7;
  int iVar8;
  u32 uVar9;
  u32 uVar10;
  u32 uVar11;
  u8 bVar18;
  short sVar16;
  u32 uVar12;
  s8 cVar19;
  u32 uVar13;
  u8 uVar20;
  int iVar14;

  int iVar21;
  u32 uVar22;
  int iVar23;
  u32 uVar24;
  u32 uVar25;
  u32 uVar26;
  
  uVar15 = fn_801F54A4(0,0,0x14,0);
  bVar1 = 0;
  uVar2 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  uVar3 = fn_80203D3C();
  uVar4 = (int)fn_8012640C(0,uVar3,0x10,0);
  uVar5 = fn_80203E0C(uVar2);
  cVar17 = fn_80204A10(uVar2);
  if (cVar17 != 1) {
    cVar17 = fn_801F54A4(0,0,0x24,0);
    if (cVar17 != 0) {
      uVar6 = fn_801F025C(3,uVar2);
      cVar17 = fn_801F7404();
      if (cVar17 != 0) {
        iVar21 = 0;
        iVar23 = 0;
        uVar7 = fn_801F54A4(0,0,0x16,0);
        for (uVar24 = 0; (int)uVar24 < (int)(uVar7 & 0xffff); uVar24 = uVar24 + 1) {
          iVar14 = fn_801F7258(uVar6,uVar24 & 0xffff);
          if (iVar14 != 0) {
            cVar17 = fn_801F9034();
            if (cVar17 != 0) {
              uVar9 = fn_801FB1C0(iVar14,0,0x44,0);
              uVar25 = 0;
              do {
                uVar10 = fn_8012A5B0(uVar9,3,uVar25 & 0xffff);
                iVar8 = fn_801F9930(iVar14,uVar10);
                if ((iVar8 != 0) && (cVar17 = fn_80206608(), cVar17 != 0)) {
                  cVar17 = fn_80205904(uVar2,iVar8);
                  if (cVar17 == 1) {
                    iVar21 = iVar21 + 1;
                  }
                  sVar16 = fn_802041EC(iVar8);
                  if (sVar16 == 0x19) {
                    iVar23 = iVar23 + 1;
                  }
                }
                uVar25 = uVar25 + 1;
              } while ((int)uVar25 < 6);
            }
          }
        }
        if ((iVar21 != 0) || (iVar23 != 0)) {
          uVar4 = ((uVar4 & 0xffff) * (uVar5 & 0xff)) / 7 & 0xffff;
          if (iVar23 == 0) {
            uVar5 = (int)uVar4 / iVar21 & 0xffff;
            if (uVar5 == 0) {
              uVar5 = 1;
            }
            uVar4 = 0;
          }
          else {
            uVar4 = uVar4 >> 1;
            uVar5 = (int)uVar4 / iVar21 & 0xffff;
            if (uVar5 == 0) {
              uVar5 = 1;
            }
            uVar4 = (int)uVar4 / iVar23 & 0xffff;
            if (uVar4 == 0) {
              uVar4 = 1;
            }
          }
          for (uVar24 = 0; (int)uVar24 < (int)(uVar7 & 0xffff); uVar24 = uVar24 + 1) {
            uVar9 = fightSideGetStatus(uVar6,0,7,uVar24 & 0xffff);
            cVar17 = fn_801F9034();
            if (cVar17 != 0) {
              uVar10 = fn_801FB1C0(uVar9,0,0x44,0);
              uVar25 = 0;
              do {
                uVar11 = fn_8012A5B0(uVar10,3,uVar25 & 0xffff);
                iVar21 = fn_801F9930(uVar9,uVar11);
                if (((iVar21 != 0) && (cVar17 = fn_80206608(), cVar17 != 0)) &&
                   (bVar18 = fn_80203E7C(iVar21), bVar18 < 100)) {
                  cVar17 = fn_80205904(uVar2,iVar21);
                  uVar22 = 0;
                  if (cVar17 == 1) {
                    uVar22 = uVar5;
                  }
                  sVar16 = fn_802041EC(iVar21);
                  if (sVar16 == 0x19) {
                    uVar22 = uVar22 + uVar4;
                  }
                  if (uVar22 != 0) {
                    sVar16 = fn_802041EC(iVar21);
                    if (sVar16 == 0x28) {
                      uVar22 = (uVar22 * 0x96) / 100;
                    }
                    cVar17 = fn_801F54A4(0,0,0x2d,0);
                    if (cVar17 == 1) {
                      uVar22 = (uVar22 * 0x96) / 100;
                    }
                    cVar17 = fn_801F90C4(uVar9,iVar21);
                    if (cVar17 == 0) {
                      uVar11 = 0x7534;
                      uVar22 = (uVar22 * 0x96) / 100;
                    }
                    else {
                      uVar11 = 0x7533;
                    }
                    uVar12 = fn_80205BE8(iVar21);
                    cVar17 = fn_8011FC74();
                    if (cVar17 == 1) {
                      bVar18 = fn_8011F77C(uVar12);
                      if (2 < bVar18) {
                        iVar23 = (int)fn_8012640C(uVar12,0,0xc6,0);
                        fn_801254B4(uVar12,0,0xc6,0,iVar23 + uVar22);
                        iVar21 = fn_801F8E34(uVar9,iVar21);
                        if ((iVar21 != 0) && (cVar17 = fn_801FECD4(), cVar17 == 1)) {
                          fn_801FE7EC(iVar21,0xc6,0,0);
                        }
                      }
                    }
                    else {
                      if (bVar1 == 0) {
                        fn_801C3430();
                        fn_801EF8F4(1);
                        bVar1 = 1;
                      }
                      iVar23 = fn_801F8E34(uVar9,iVar21);
                      if (iVar23 != 0) {
                        fn_80265598(iVar23,uVar15,1);
                      }
                      uVar12 = fn_80203848(iVar21);
                      fn_80132A38(0xd,uVar12);
                      uVar11 = fn_800FA280(uVar11);
                      fn_80132A38(0xe,uVar11);
                      fn_80132A38(0x2f,uVar22);
                      cVar17 = fn_802624CC(0x7532);
                      fn_8026246C();
                      fn_8020341C(iVar21,0,uVar3);
                      while (((uVar22 != 0 && (cVar19 = fn_80206608(iVar21), cVar19 != 0)) &&
                             (bVar18 = fn_80203E7C(iVar21), bVar18 < 100))) {
                        fn_801FE468(iVar21,0x80478278);
                        lbl_8047B64C = iVar21;
                        iVar23 = fn_80203620(iVar21);
                        uVar13 = fn_8020355C(iVar21,bVar18 + 1);
                        uVar26 = iVar23 + uVar22;
                        if (uVar26 < uVar13) {
                          uVar22 = 0;
                          fn_802035BC(iVar21,uVar26);
                          iVar23 = fn_801F8E34(uVar9,iVar21);
                          if (iVar23 != 0) {
                            uVar11 = fn_802656AC(iVar23,uVar15,0);
                            iVar14 = fn_8020355C(iVar21,bVar18);
                            fn_80011BC4(uVar11,uVar26 - iVar14);
                            fn_801F000C(0x40);
                            fn_80011B4C(uVar11,1);
                            fn_802653FC(iVar23,uVar15,1);
                          }
                        }
                        else {
                          uVar22 = uVar26 - uVar13;
                          fn_8020367C(iVar21,uVar13);
                          uVar11 = fn_80203848(iVar21);
                          fn_80132A38(0xd,uVar11);
                          uVar20 = fn_80203E7C(iVar21);
                          fn_80132A38(0x2f,uVar20);
                          cVar17 = fn_801F54A4(0,0,0x27,0);
                          if (cVar17 == 1) {
                            cVar17 = fn_801FB8F8(uVar9);
                            if (cVar17 == 1) {
                              fn_802032E4(iVar21,0);
                            }
                          }
                          fn_801254B4(iVar21,0,0xd0,0,1);
                          iVar23 = fn_801F8E34(uVar9,iVar21);
                          if (iVar23 != 0) {
                            cVar17 = fn_801FECD4();
                            if (cVar17 == 1) {
                              fn_801FE5D4(iVar23);
                            }
                            cVar17 = fn_801FECD4(iVar23);
                            if (cVar17 == 1) {
                              fn_801FE710(iVar23,0xd0,0);
                            }
                          }
                          iVar23 = fn_801F8E34(uVar9,iVar21);
                          if (iVar23 != 0) {
                            uVar11 = fn_802656AC(iVar23,uVar15,0);
                            iVar14 = fn_8020355C(iVar21,bVar18);
                            fn_80011BC4(uVar11,uVar13 - iVar14);
                            fn_80011B4C(uVar11,1);
                            fn_802653FC(iVar23,uVar15,1);
                          }
                          fn_80211B94(lbl_8047B62C,0x80378724,0);
                          cVar17 = 0;
                        }
                        lbl_8047B64C = 0;
                      }
                      if (cVar17 == 1) {
                        fn_8026246C();
                      }
                      iVar21 = fn_801F8E34(uVar9,iVar21);
                      if (iVar21 != 0) {
                        fn_8026532C(iVar21,uVar15,0);
                      }
                    }
                  }
                }
                uVar25 = uVar25 + 1;
              } while ((int)uVar25 < 6);
            }
          }
        }
      }
    }
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 2;
  return;
}
/* Address: 0x80223A24 | Size: 0x64 | Ghidra import */
void fn_80223A24(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_80207AE0();
  u32 uVar1;
  s8 cVar2;

  int iVar3;
  
  uVar1 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  iVar3 = *(int *)(*(int *)(lbl_8047B610) + 3);
  cVar2 = fn_80207AE0(uVar1,*(u8 *)(*(int *)(lbl_8047B610) + 2));
  if (cVar2 != 1) {
    iVar3 = *(int *)(lbl_8047B610) + 7;
  }
  *(int *)(lbl_8047B610) = iVar3;
  return;
}
/* Address: 0x80223A88 | Size: 0x6c | Ghidra import */
void fn_80223A88(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
  u32 uVar1;
  u8 cVar2;

  int iVar3;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  iVar3 = *(int *)(lbl_8047B610 + 7);
  cVar2 = fn_802026E4(uVar1,*(u32 *)(lbl_8047B610 + 2) & 0xffff);
  if (*(u8 *)(lbl_8047B610 + 6) != cVar2) {
    iVar3 = lbl_8047B610 + 0xb;
  }
  lbl_8047B610 = iVar3;
  return;
}
/* Address: 0x80223AF4 | Size: 0x148 | Ghidra import */

void fn_80223AF4(void)

{
    extern u32 fn_801F025C();
  u8 bVar1;
  u8 bVar2;
  u32 bVar3;
  u32 uVar4;
  int iVar5;
  u32 uVar6;
  u8 bVar7;

  bVar3 = 0;
  uVar6 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  uVar4 = (u32)*(u8 *)(*(int *)(lbl_8047B610) + 3);
  bVar1 = *(u8 *)(*(int *)(lbl_8047B610) + 2);
  if (7 < uVar4) {
    bVar7 = (int)fn_8012640C(uVar6,0,0,0);
    bVar2 = *(u8 *)(*(int *)(lbl_8047B610) + 4);
    if (bVar1 == 3) {
      if (bVar7 < bVar2) {
        bVar3 = 1;
      }
    }
    else if (bVar1 < 3) {
      if (bVar1 == 1) {
        if (bVar7 != bVar2) {
          bVar3 = 1;
        }
      }
      else if (bVar1 == 0) {
        if (bVar7 == bVar2) {
          bVar3 = 1;
        }
      }
      else if (bVar2 < bVar7) {
        bVar3 = 1;
      }
    }
    else if (bVar1 == 5) {
      if ((bVar7 & bVar2) == 0) {
        bVar3 = 1;
      }
    }
    else if ((bVar1 < 5) && ((bVar7 & bVar2) != 0)) {
      bVar3 = 1;
    }
    if (bVar3) {
      iVar5 = *(int *)(*(int *)(lbl_8047B610) + 5);
    }
    else {
      iVar5 = *(int *)(lbl_8047B610) + 9;
    }
    *(int *)(lbl_8047B610) = iVar5;
    return;
  }

  ((int (*)(void))**(void ***)(uVar4 * 4 + -0x7fc65e00))();
  return;
}
/* Address: 0x80223C74 | Size: 0x74 | Ghidra import */
void fn_80223C74(void)
{
    extern u32 fn_801F025C();
    extern u8 fn_801F6E98();
  u32 uVar1;
  u8 cVar2;

  int iVar3;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar1 = fn_801F025C(2,uVar1);
  iVar3 = *(int *)(lbl_8047B610 + 4);
  cVar2 = fn_801F6E98(uVar1,*(u16 *)(lbl_8047B610 + 2));
  if (cVar2 != 1) {
    iVar3 = lbl_8047B610 + 8;
  }
  lbl_8047B610 = iVar3;
  return;
}
/* Address: 0x80223CE8 | Size: 0x7c | Ghidra import */
void fn_80223CE8(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
  u32 uVar1;
  u16 uVar2;
  int iVar3;

  u32 uVar4;
  
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar2 = fn_80207BF4();
  iVar3 = lbl_8047B610;
  uVar4 = *(u32 *)(iVar3 + 3);
  if (uVar2 == *(u8 *)(iVar3 + 2)) {
    fn_801F4C14(0,0,0x48,0,uVar1);
    lbl_8047B610 = uVar4;
  }
  else {
    lbl_8047B610 = iVar3 + 7;
  }
  return;
}
/* Address: 0x80223D64 | Size: 0xdc | Ghidra import */
void fn_80223D64(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_802026E4();
    extern s8 fn_80203C5C();
    extern s8 fn_80203CCC();
    extern s8 fn_802062FC();
  u32 uVar1;
  u32 uVar2;
  s8 cVar3;

  u32 uVar4;
  
  uVar2 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  uVar4 = *(u32 *)(*(int *)(lbl_8047B610) + 6);
  uVar1 = *(u32 *)(*(int *)(lbl_8047B610) + 2) & 0xffff;
  if (uVar1 == 1) {
    cVar3 = fn_80203CCC();
    if (cVar3 == 0) {
      *(u32 *)(lbl_8047B610) = uVar4;
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
    }
  }
  else if (uVar1 == 2) {
    cVar3 = fn_80203C5C();
    if (cVar3 == 1) {
      *(u32 *)(lbl_8047B610) = uVar4;
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
    }
  }
  else {
    cVar3 = fn_802026E4();
    if ((cVar3 == 1) && (cVar3 = fn_802062FC(uVar2), cVar3 == 1)) {
      *(u32 *)(lbl_8047B610) = uVar4;
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
    }
  }
  return;
}
/* Address: 0x80223E40 | Size: 0xdc | Ghidra import */
void fn_80223E40(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_802026E4();
    extern s8 fn_80203C5C();
    extern s8 fn_80203CCC();
    extern s8 fn_802062FC();
  u32 uVar1;
  u32 uVar2;
  s8 cVar3;

  u32 uVar4;
  
  uVar2 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  uVar4 = *(u32 *)(*(int *)(lbl_8047B610) + 6);
  uVar1 = *(u32 *)(*(int *)(lbl_8047B610) + 2) & 0xffff;
  if (uVar1 == 1) {
    cVar3 = fn_80203CCC();
    if (cVar3 == 0) {
      *(u32 *)(lbl_8047B610) = uVar4;
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
    }
  }
  else if (uVar1 == 2) {
    cVar3 = fn_80203C5C();
    if (cVar3 == 1) {
      *(u32 *)(lbl_8047B610) = uVar4;
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
    }
  }
  else {
    cVar3 = fn_802026E4();
    if ((cVar3 == 1) && (cVar3 = fn_802062FC(uVar2), cVar3 == 1)) {
      *(u32 *)(lbl_8047B610) = uVar4;
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
    }
  }
  return;
}
/* Address: 0x80223F1C | Size: 0x144 | Ghidra import */
void fn_80223F1C(void)

{
    extern void fn_801252E0();
    extern u32 fn_801F025C();
    extern void fn_801F37B0();
    extern void fn_801F4354();
    extern u32 fn_801F54A4();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_80202998();
    extern void fn_80205B8C();
    extern void fn_8020912C();
    extern void fn_8026532C();
    extern void fn_80232FE4();
  u32 uVar1;
  u16 uVar2;
  u8 cVar3;

  uVar2 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  fn_801F4354(0,uVar1);
  fn_8020912C(uVar1,2);
  fn_8026532C(uVar1,uVar2,0);
  fn_8020912C(uVar1,3);
  fn_801F37B0(0,(u32)fn_80232FE4,uVar1,0);
  fn_80205B8C(uVar1);
  fn_801252E0();
  fn_80202998(uVar1,0);
  fn_80202810(uVar1,0x17);
  cVar3 = fn_801FECD4(uVar1);
  if (cVar3 == 1) {
    fn_801FE7EC(uVar1,0x7c,0,0);
  }
  cVar3 = fn_802026E4(uVar1,0x3e);
  if (cVar3 == 1) {
    fn_80202810(uVar1,0x3e);
    cVar3 = fn_801FECD4(uVar1);
    if (cVar3 == 1) {
      fn_801FE7EC(uVar1,200,0,0);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x80224060 | Size: 0xf8 | Ghidra import */
void fn_80224060(void)

{
    extern void _threadSwitch();
    extern s8 fn_801DA5C4();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern int fn_801FEF74();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_802086B0();
    extern void fn_8020912C();
    extern void fn_8020F108();
    extern void fn_80265598();
  u32 uVar1;
  u16 uVar3;
  u8 cVar4;
  int iVar2;

  uVar3 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  cVar4 = fn_802026E4(uVar1,0x14);
  if ((cVar4 == 1) && (iVar2 = fn_801FEF74(uVar1), 0 < iVar2)) {
    fn_80202810(uVar1,0x14);
    fn_802086B0(uVar1);
    fn_8020F108(0xa4,uVar1,uVar1,0,0);
    for (;;) {
      cVar4 = fn_801DA5C4(6);
      if (cVar4 == 1) break;
      _threadSwitch();
    }
  }
  fn_802086B0(uVar1);
  fn_8020912C(uVar1,0);
  fn_8020912C(uVar1,1);
  fn_80265598(uVar1,uVar3,1);
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x80224158 | Size: 0x5e8 | Ghidra import */
void fn_80224158(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern int fn_8011BEB4();
    extern int fn_80123B5C();
    extern void fn_80132A38();
    extern int fn_801F025C();
    extern u32 fn_801F1D5C();
    extern void fn_801F4C14();
    extern s8 fn_801F54A4();
    extern void fn_801F75F8();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern u32 fn_80201890();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern s8 fn_80202B88();
    extern void fn_802032E4();
    extern s8 fn_802038A4();
    extern u32 fn_80203E0C();
    extern s8 fn_80204A10();
    extern u32 fn_80205224();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_80209FAC();
    extern void fn_8020A2B8();
    extern void fn_80211B94();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  int iVar1;
  u32 uVar2;
  u32 uVar3;
  s8 cVar13;
  u32 uVar4;
  u32 uVar5;
  u16 uVar10;
  u8 bVar14;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  short sVar11;
  u8 bVar15;
  u16 uVar12;

  u16 uVar17;
  int iVar16;
  int local_f8 [8];
  u8 auStack_d8 [176];
  
  fn_80261B68(0);
  fn_80261E7C(0);
  fn_8026246C();
  iVar1 = *(int *)(lbl_8047B610);
  if (*(char *)(iVar1 + 2) == 0) {
    cVar13 = *(char *)(iVar1 + 1);
    if (cVar13 == 17) {
      iVar1 = fn_801F025C(0x11,0);
      fn_8012640C(iVar1,0,0xd5,0);
      uVar3 = fn_801F025C(0x12,0);
      uVar2 = 0x803786f4;
    }
    else if (cVar13 == 18) {
      iVar1 = fn_801F025C(0x12,0);
      fn_8012640C(iVar1,0,0xd5,0);
      uVar3 = fn_801F025C(0x11,0);
      uVar2 = 0x80378703;
    }
    else if (cVar13 == 21) {
      iVar1 = fn_801F025C(0x15,0);
      fn_8012640C(iVar1,0,0xd5,0);
      uVar3 = fn_801F025C(0x11,0);
      uVar2 = 0x80378712;
    }
    else {
      iVar1 = 0;
      uVar3 = 0;
      uVar2 = 0x80378703;
    }
    cVar13 = fn_802038A4(iVar1);
    if (cVar13 == 1) {
      fn_801254B4(iVar1,0,0x120,0,1);
      cVar13 = fn_80204A10(iVar1);
      if (cVar13 == 1) {
        lbl_8047B618 = lbl_8047B618 | 0x400000;
        cVar13 = fn_801F54A4(0,0,0x27,0);
        if ((cVar13 == 1) && (iVar1 != 0)) {
          uVar4 = (int)fn_8012640C(iVar1,0,0xd5,0);
          uVar5 = fn_80203E0C(iVar1);
          uVar10 = fn_801F1D5C(0,0,2,iVar1,local_f8);
          bVar15 = 0;
          for (uVar17 = 0; uVar17 < uVar10; uVar17 = uVar17 + 1) {
            if ((local_f8[uVar17] != 0) && (bVar14 = fn_80203E0C(), bVar15 < bVar14)) {
              bVar15 = fn_80203E0C(local_f8[uVar17]);
            }
          }
          if ((uVar5 & 0xff) < (u32)bVar15) {
            if ((int)((u32)bVar15 - (uVar5 & 0xff)) < 0x1e) {
              fn_802032E4(uVar4,6);
            }
            else {
              fn_802032E4(uVar4,8);
            }
          }
        }
      }
      if ((lbl_8047B618 & 0x1000000) == 0) {
        uVar4 = fn_801F025C(0x12,0);
        cVar13 = fn_802025B8(uVar4,0x28);
        if (cVar13 == 1) {
          uVar6 = fn_801F025C(0x11,0);
          uVar7 = fn_80205B8C();
          uVar8 = (int)fn_8012640C(uVar6,0,0xd9,0);
          uVar9 = fn_80205224(uVar6);
          sVar11 = fn_80205184(uVar6);
          cVar13 = fn_80202B88(uVar6,uVar4);
          if ((((cVar13 == 0) && (cVar13 = fn_802062FC(uVar6), cVar13 == 1)) &&
              (sVar11 != 0xa5)) && (sVar11 != 0x164)) {
            bVar15 = fn_80123B5C(uVar7,uVar9);
            if ((char)bVar15 < 0) {
              bVar15 = fn_8011BEB4(uVar8,0,0x26,0);
            }
            iVar16 = (int)(char)bVar15;
            fn_801254B4(uVar7,0,0x80,iVar16,0);
            cVar13 = fn_802026E4(uVar6,0x10);
            if ((cVar13 == 0) && (cVar13 = fn_802026E4(uVar6,0x31), cVar13 == 1)) {
              uVar5 = fn_80201890(uVar6,0x31);
              if (((uVar5 & 1 << (u32)bVar15) == 0) &&
                 (cVar13 = fn_801FECD4(uVar6), cVar13 == 1)) {
                fn_801FE7EC(uVar6,0x80,(u32)bVar15,0);
              }
            }
            cVar13 = fn_801FECD4(uVar6);
            if (cVar13 == 1) {
              fn_801FE7EC(uVar6,0x80,iVar16,0);
            }
            uVar12 = (int)fn_8012640C(uVar7,0,0x7f,iVar16);
            fn_8011BEB4(0,uVar12,1,0);
            uVar4 = fn_800FA280();
            fn_80132A38(0xd,uVar4);
            fn_80211B94(lbl_8047B62C,0x80379167,0);
          }
        }
      }
      if ((lbl_8047B618 & 0x40) != 0) {
        uVar4 = fn_801F025C(0x11,0);
        cVar13 = fn_802062FC();
        if (cVar13 == 1) {
          uVar4 = (int)fn_8012640C(uVar4,0,0xd9,0);
          fn_8020A2B8(auStack_d8,uVar4);
          uVar3 = fn_80205B8C(uVar3);
          uVar12 = (int)fn_8012640C(uVar3,0,0x83,0);
          fn_8011BBD8(uVar4,0,0x2d,0,uVar12);
          fn_80209FAC(uVar4);
          fn_80211B94(lbl_8047B62C,0x80378d54,0);
          fn_8020A2B8(uVar4,auStack_d8);
        }
        lbl_8047B618 = lbl_8047B618 & 0xffffffbf;
      }
      uVar3 = fn_801F54A4(0,0,0x4b,0);
      fn_801F4C14(0,0,0x4b,0,iVar1);
      fn_80211B94(lbl_8047B62C,uVar2,0);
      fn_801F4C14(0,0,0x4b,0,uVar3);
    }
  }
  else {
    uVar2 = fn_801F025C(*(u8 *)(iVar1 + 1),0);
    uVar3 = fn_801F025C(2,uVar2);
    cVar13 = fn_802038A4(uVar2);
    if (cVar13 == 1) {
      fn_801F75F8(uVar3,0,8,0,0);
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 3);
      return;
    }
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
  return;
}
/* Address: 0x80224740 | Size: 0x90 | Ghidra import */
void fn_80224740(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u8 lbl_80478D7B;
    extern u8 lbl_8047B625;
  short sVar1;
  u32 uVar2;
  s8 cVar3;

  uVar2 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  sVar1 = *(short *)((u32)lbl_80478D7B * 2 + -0x7fd8610c);
  if ((sVar1 != 0) && (cVar3 = fn_802026E4(), cVar3 == 1)) {
    fn_80202810(uVar2,sVar1);
  }
  lbl_80478D7B = 0;
  lbl_8047B625 = 0;
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 2;
  return;
}
/* Address: 0x802247D0 | Size: 0x28 | Ghidra import */
void fn_802247D0(void)

{
    extern void fn_802249B8();
  fn_802249B8(0,0);
  return;
}
/* Address: 0x802247F8 | Size: 0x28 | Ghidra import */
void fn_802247F8(void)

{
    extern void fn_802249B8();
  fn_802249B8(1,0);
  return;
}
/* Address: 0x80224820 | Size: 0x198 | Ghidra import */
void fn_80224820(void)

{
    extern u8 fn_8000817C();
    extern u32 fn_800E0C54();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u8 fn_802096E8();
    extern void fn_802249B8();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern u8 lbl_80478D78;
    extern u8 lbl_8047B625;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u16 sVar6;
  u32 uVar4;
  u8 cVar7;
  u32 uVar5;
  u8 *pbVar8;
  u32 tmp9;

  fn_80261B68(0);
  fn_80261E7C(0);
  fn_8026246C();
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80205184(uVar1);
  sVar6 = fn_80207BF4(uVar1);
  if (sVar6 == 0x20) {
    tmp9 = fn_8011BEB4(0,uVar3,0xc,0);
    tmp9 = (tmp9 & 0xff) << 1;
  }
  else {
    tmp9 = fn_8011BEB4(0,uVar3,0xc,0);
    tmp9 = tmp9 & 0xff;
  }
  uVar4 = tmp9;
  cVar7 = fn_8000817C();
  if (cVar7 == 1) {
    uVar4 = 99;
  }
  if ((((&lbl_80478D78)[3] & 0x80) != 0) &&
     (cVar7 = fn_802096E8(uVar2), cVar7 == 1)) {
    (&lbl_80478D78)[3] = (&lbl_80478D78)[3] & 0x7f;
    fn_802249B8(0,0x80);
  }
  else {
    uVar5 = fn_800E0C54();
    if ((((int)(uVar5 & 0xffff) % 100 <= (u16)uVar4) && ((&lbl_80478D78)[3] != 0)) &&
       (cVar7 = fn_802096E8(uVar2), cVar7 == 1))
    {
      if ((u16)uVar4 >= 100) {
        fn_802249B8(0,0x80);
      }
      else {
        fn_802249B8(0,0);
      }
    }
    else {
      lbl_8047B610 = lbl_8047B610 + 1;
    }
  }
  (&lbl_80478D78)[3] = 0;
  lbl_8047B625 = 0;
  return;
}
/* -------------------------------------------------------------------
 * Team Management & Validation (0x80226000-0x80230000)
 * 59 functions
 * ------------------------------------------------------------------- */

/* Address: 0x80226134 | Size: 0x7c | Ghidra import */
void fn_80226134(void)

{
    extern u32 fn_801F54A4();
    extern u8 fn_802624CC();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;
  u32 iVar3;

  iVar3 = *(u32 *)(lbl_8047B610 + 1);
  if (iVar3 != 0) {
    uVar1 = *(u32 *)(iVar3 + (u32)(&lbl_80478D78)[5] * 4);
  }
  else {
    uVar1 = fn_801F54A4(0,0,0x50,0);
  }
  cVar2 = fn_802624CC(uVar1);
  if (cVar2 == 1) {
    (&lbl_80478D78)[7] = 1;
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x802261B0 | Size: 0x7c | Ghidra import */
void fn_802261B0(void)

{
    extern u32 fn_801F54A4();
    extern u8 fn_802624CC();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;
  u32 iVar3;

  iVar3 = *(u32 *)(lbl_8047B610 + 1);
  if (iVar3 != 0) {
    uVar1 = *(u32 *)(iVar3 + (u32)(&lbl_80478D78)[5] * 4);
  }
  else {
    uVar1 = fn_801F54A4(0,0,0x50,0);
  }
  cVar2 = fn_802624CC(uVar1);
  if (cVar2 == 1) {
    (&lbl_80478D78)[7] = 1;
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x8022622C | Size: 0x58 | Ghidra import */
void fn_8022622C(void)

{
    extern void fn_801F000C();
    extern void fn_8026246C();
    extern u8 lbl_80478D78;

  if (*((&lbl_80478D78)+7) != 0) {
    fn_801F000C(*(u16 *)(lbl_8047B610 + 1));
    fn_8026246C();
    *((&lbl_80478D78)+7) = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* Address: 0x80226284 | Size: 0x4c | Ghidra import */
void fn_80226284(void)
{
    extern u8 fn_802624CC();
    extern u8 lbl_80478D78;
  u8 cVar1;

  cVar1 = fn_802624CC(*(u32 *)(lbl_8047B610 + 1));
  if (cVar1 == 1) {
    *((&lbl_80478D78)+7) = 1;
  }
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* Address: 0x802262D0 | Size: 0x4c | Ghidra import */
void fn_802262D0(void)
{
    extern u8 fn_802624CC();
    extern u8 lbl_80478D78;
  u8 cVar1;

  cVar1 = fn_802624CC(*(u32 *)(lbl_8047B610 + 1));
  if (cVar1 == 1) {
    *((&lbl_80478D78)+7) = 1;
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x8022631C | Size: 0x3d0 | Ghidra import */
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
    extern u8 lbl_80478D78;
    extern u32 lbl_8047B62C;
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
  uVar3 = fn_802040E8(uVar2);
  (&lbl_80478D78)[0x7] = 0;
  iVar5 = 0;
  cVar4 = fn_8020990C(uVar1,0x40);
  if (cVar4 == 1) {
    cVar4 = fn_8020990C(uVar1,0x43);
    if (cVar4 == 0) goto _body;
  }
  cVar4 = fn_8020990C(uVar1,0x40);
  if (cVar4 != 1) goto _else;
  if ((&lbl_80478D78)[0x6] < 3) goto _else;
_body:
  iVar5 = lbl_80279D18[(&lbl_80478D78)[0x6]];
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
      (&lbl_80478D78)[0x7] = 1;
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802266EC | Size: 0x44 | Ghidra import */
void fn_802266EC(void)

{
    extern u32 fn_801F025C();
  u32 uVar1;

  uVar1 = fn_801F025C(0x11,0);
  fn_8012640C(uVar1,0,0xd9,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80226730 | Size: 0xb8 | Ghidra import */
void fn_80226730(void)

{
    extern u8 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_802096E8();
    extern u8 fn_802624CC();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  cVar2 = fn_8011BEB4(uVar1,0,0x2b,0);
  if ((cVar2 == 2) && (cVar2 = fn_802096E8(uVar1), cVar2 == 1)) {
    fn_801F4C14(0,0,0x52,0,0x7631);
    cVar2 = fn_802624CC(0x7631);
    if (cVar2 == 1) {
      *((&lbl_80478D78)+7) = 1;
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802267E8 | Size: 0x12c | Ghidra import */
void fn_802267E8(void)

{
    extern void fn_80011C78();
    extern void _threadSwitch();
    extern u8 fn_80102620();
    extern u8 fn_801DA5C4();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern int fn_801FEF74();
    extern void fn_80201764();
    extern void fn_8020248C();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802038A4();
    extern void fn_80207C24();
    extern void fn_80211B94();
    extern u32 fn_802656AC();
    extern u32 lbl_8047B62C;
    extern u8 lbl_80379A22[];
  u32 uVar2;
  u32 uVar1;
  u16 uVar4;
  u8 cVar5;
  int iVar3;

  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar2 = fn_802656AC(uVar1,uVar4,1);
  cVar5 = fn_80102620();
  if (cVar5 == 1) {
    fn_80011C78(uVar2,1);
  }
  cVar5 = fn_802038A4(uVar1);
  if (cVar5 == 1) {
    fn_80207C24(uVar1,1);
  }
  cVar5 = fn_802026E4(uVar1,0x14);
  if ((cVar5 == 1) && (iVar3 = fn_801FEF74(uVar1), iVar3 <= 0)) {
    for (;;) {
      cVar5 = fn_801DA5C4(6);
      if (cVar5 == 1) break;
      _threadSwitch();
    }
    fn_80202810(uVar1,0x14);
    fn_80211B94(lbl_8047B62C,(u32)lbl_80379A22,0);
    fn_8020248C(uVar1,0x14,0);
    fn_80201764(uVar1,0x14,0);
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x80226914 | Size: 0x5f8 | Ghidra import */
void fn_80226914(void)

{
    extern void fn_80011CF0();
    extern s8 fn_80102620();
    extern int fn_8010C4A0();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern void fn_801F000C();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_801FE55C();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_801FED3C();
    extern u32 fn_801FEF74();
    extern u32 fn_80205B8C();
    extern s8 fn_802096E8();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_80265598();
    extern u32 fn_802656AC();
    extern u32 lbl_8047B618;
  u16 uVar12;
  u32 uVar1;
  short sVar13;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar14;
  u32 uVar5;
  s8 cVar17;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  int iVar9;
  u32 uVar10;
  int iVar11;
  short sVar15;
  u16 uVar16;
  u8 bVar18;

  uVar12 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  sVar13 = fn_80205184();
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_8011BEB4(uVar2,0,0x2d,0);
  uVar4 = fn_8011BEB4(uVar2,0,0x2e,0);
  uVar14 = fn_8011BEB4(uVar2,0,0x30,0);
  uVar5 = fn_801F025C(0x12,0);
  cVar17 = fn_802096E8(uVar2);
  if (cVar17 == 1) {
    uVar6 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
    uVar7 = fn_80205B8C();
    uVar8 = (int)fn_8012640C(uVar7,0,0x83,0);
    iVar9 = (int)fn_8012640C(uVar7,0,0x87,0);
    uVar10 = fn_801FEF74(uVar6);
    if ((uVar10 == 0) || ((lbl_8047B618 & 0x100) != 0)) {
      lbl_8047B618 = lbl_8047B618 & 0xfffffeff;
      if ((int)uVar3 < 0) {
        iVar11 = uVar8 - uVar3;
        uVar8 = uVar4;
        if (iVar9 < iVar11) {
          iVar11 = iVar9;
        }
      }
      else {
        if ((int)uVar3 < (int)uVar8) {
          iVar11 = uVar8 - uVar3;
          uVar8 = uVar3;
        }
        else {
          iVar11 = 0;
        }
        if ((lbl_8047B618 & 0x20) == 0) {
          sVar15 = (int)fn_8012640C(uVar6,0,0xf5,0);
          fn_801254B4(uVar6,0,0xf5,0,(int)(short)(sVar15 + (short)uVar3));
          if (*(char *)(*(int *)(lbl_8047B610) + 1) == 18) {
            uVar16 = fn_801F0134(uVar1,uVar12);
            fn_801254B4(uVar6,0,0xf6,0,uVar16);
          }
          else {
            uVar16 = fn_801F0134(uVar5,uVar12);
            fn_801254B4(uVar6,0,0xf6,0,uVar16);
          }
          fn_801FE55C(uVar6,uVar1,uVar8 & 0xffff);
        }
        else {
          lbl_8047B618 = lbl_8047B618 & 0xffffffdf;
        }
        iVar9 = (int)fn_8012640C(uVar6,0,0x11b,0);
        if ((iVar9 == 0) && ((lbl_8047B618 & 0x100000) == 0)) {
          fn_801254B4(uVar6,0,0x11b,0,uVar8);
        }
        bVar18 = fn_8010C4A0(uVar14);
        if (bVar18 == 2) {
          if ((lbl_8047B618 & 0x100000) == 0) {
            fn_801254B4(uVar6,0,0x104,0,uVar8);
            fn_801254B4(uVar6,0,0x11e,0,uVar8);
            if (*(char *)(*(int *)(lbl_8047B610) + 1) == 18) {
              uVar14 = fn_801F0134(uVar1,uVar12);
              fn_801254B4(uVar6,0,0x105,0,uVar14);
              fn_801254B4(uVar6,0,0x11f,0,uVar14);
            }
            else {
              uVar14 = fn_801F0134(uVar5,uVar12);
              fn_801254B4(uVar6,0,0x105,0,uVar14);
              fn_801254B4(uVar6,0,0x11f,0,uVar14);
            }
          }
        }
        else if ((((bVar18 < 2) && (bVar18 != 0)) &&
                 ((lbl_8047B618 & 0x100000) == 0)) && (sVar13 != 0xdc)) {
          fn_801254B4(uVar6,0,0x102,0,uVar8);
          fn_801254B4(uVar6,0,0x11c,0,uVar8);
          if (*(char *)(*(int *)(lbl_8047B610) + 1) == 18) {
            uVar14 = fn_801F0134(uVar1,uVar12);
            fn_801254B4(uVar6,0,0x103,0,uVar14);
            fn_801254B4(uVar6,0,0x11d,0,uVar14);
          }
          else {
            uVar14 = fn_801F0134(uVar5,uVar12);
            fn_801254B4(uVar6,0,0x103,0,uVar14);
            fn_801254B4(uVar6,0,0x11d,0,uVar14);
          }
        }
      }
      lbl_8047B618 = lbl_8047B618 & 0xffefffff;
      fn_8011BBD8(uVar2,0,0x2e,0,uVar8);
      fn_801254B4(uVar7,0,0x83,0,iVar11);
      cVar17 = fn_801FECD4(uVar6);
      if (cVar17 == 1) {
        fn_801FE7EC(uVar6,0x83,0,0);
      }
      uVar1 = fn_802656AC(uVar6,uVar12,1);
      cVar17 = fn_80102620();
      if (cVar17 == 0) {
        fn_80265598(uVar6,uVar12,1);
      }
      fn_80011CF0(uVar1,(int)(short)iVar11);
    }
    else {
      if ((int)uVar10 < (int)uVar3) {
        iVar9 = 0;
      }
      else {
        iVar9 = uVar10 - uVar3;
        uVar10 = uVar3;
      }
      iVar11 = (int)fn_8012640C(uVar6,0,0x11b,0);
      if (iVar11 == 0) {
        fn_801254B4(uVar6,0,0x11b,0,uVar10);
      }
      fn_8011BBD8(uVar2,0,0x2e,0,uVar10);
      fn_801FED3C(uVar6,iVar9);
      fn_802624CC(0x75b1);
      fn_801F000C(0x40);
      fn_8026246C();
    }
  }
  else {
    uVar1 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
    iVar9 = (int)fn_8012640C(uVar1,0,0x11b,0);
    if (iVar9 == 0) {
      fn_801254B4(uVar1,0,0x11b,0,0xffff);
    }
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 2;
  return;
}
/* Address: 0x80226F0C | Size: 0xC8 (200 bytes) */
#pragma optimization_level 0
void fn_80226F0C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_8047B618;
    extern void _threadSwitch();
    extern void fn_801DA5C4();
    extern void fn_801F025C();
    extern void fn_80261E7C();
    u32 tmp = 0;
    u32 var_r29 = 0;
    u32 _ctx = 0;

    _ctx = 0x11;
    param1 = 0x0;
    fn_801F025C();
    _ctx = _ctx;
    ((void(*)(void))fn_80205184)();
    _ctx = _ctx;
    _ctx = _ctx;
    param1 = 0x0;
    param2 = 0xd9;
    param3 = 0x0;
    ((void(*)(void))fn_8012640C)();
    _ctx = 0x12;
    param1 = 0x0;
    fn_801F025C();
    tmp = lbl_8047B618;
    tmp = tmp & 0x00000080;
    if ((s32)tmp != 0) {
        tmp = _ctx & 0xFFFF;
        if (tmp != 0x90) {
            if (tmp == 0xa4) {
        }
        }
        _ctx = *(u32*)&lbl_8047B610;
        var_r29 = *(u8*)((u8*)_ctx + 0x1);
        do {
            _ctx = var_r29;
            fn_801DA5C4();
            tmp = _ctx & 0xFF;
            if (tmp == 1) break;
            _threadSwitch();
        } while (1);
            }
    tmp = var_r29 & 0xFF;

    if (tmp == 2 && tmp != 6) {

        _ctx = 0x0;
        fn_80261E7C();
    }
    _ctx = *(u32*)&lbl_8047B610;
    tmp = _ctx + 0x2;
    *(u32*)&lbl_8047B610 = tmp;
    return;
}
#pragma optimization_level 4

/* Address: 0x80226FD4 | Size: 0x1a4 | Ghidra import */
void fn_80226FD4(void)

{
    extern u32 DAT_8038ff5a;
    extern u32 DAT_8038fff9;
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_8020955C();
    extern s8 fn_802096E8();
    extern void fn_80211B94();
    extern void fn_80261E7C();
    extern void fn_80265598();
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  s8 cVar7;
  u16 uVar6;
  int iVar5;

  fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184();
  uVar3 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar4 = fn_801F025C(0x12,0);
  cVar7 = fn_802096E8(uVar3);
  if (cVar7 == 1) {
    if ((((lbl_8047B618 & 0x80) == 0) || ((uVar2 & 0xffff) == 0x90)) ||
       ((uVar2 & 0xffff) == 0xa4)) {
      fn_8020955C(uVar2,uVar1,uVar4,DAT_8038fff9);
    }
    else {
      fn_80211B94(lbl_8047B62C,0x80378964,0);
    }
    DAT_8038ff5a = DAT_8038ff5a + 1;
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    DAT_8038fff9 = DAT_8038fff9 + 1;
  }
  else {
    fn_80261E7C(0);
    uVar6 = fn_801F54A4(0,0,0x14,0);
    iVar5 = (int)fn_8012640C(uVar4,0,0xee,0);
    if ((iVar5 != 0) && (cVar7 = fn_801DDD28(iVar5,0x57,4,0), cVar7 != 0)) {
      fn_801DA9E8(iVar5,0x57,4);
      fn_80265598(uVar4,uVar6,1);
    }
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  }
  return;
}
/* Address: 0x80227178 | Size: 0x34 | Ghidra import */
void fn_80227178(void)

{
    extern void fn_802271E0();

  fn_802271E0(0,1);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802271AC | Size: 0x34 | Ghidra import */
void fn_802271AC(void)

{
    extern void fn_802271E0();

  fn_802271E0(1,1);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802271E0 | Size: 0x2b0 | Ghidra import */
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
    extern u32 fn_80205B8C();
    extern void fn_802097C8();
    extern u8 fn_8020981C();
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  u16 uVar9;
  u16 sVar10;
  int iVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 cVar11;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184();
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  iVar3 = fn_8011BEB4(uVar1,0,0x2d,0);
  uVar4 = fn_801F025C(0x12,0);
  uVar9 = fn_802040E8();
  sVar10 = fn_80203FE4(uVar4);
  iVar5 = fn_80203EDC(uVar4);
  uVar6 = fn_80205B8C(uVar4);
  uVar7 = (int)fn_8012640C(uVar6,0,0x83,0);
  if (r4 == 1) {
    uVar8 = fn_800E0C54();
    if (iVar3 != 0) {
      iVar3 = (int)(iVar3 * (100 - (uVar8 & 0xf) & 0xffff)) / 100;
      if (iVar3 == 0) {
        iVar3 = 1;
      }
      fn_8011BBD8(uVar1,0,0x2d,0,iVar3);
    }
  }
  if ((sVar10 == 0x27) && (uVar8 = fn_800E0C54(), (int)((uVar8 & 0xffff) % 100) < iVar5)) {
    fn_801254B4(uVar4,0,0x11a,0,1);
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
      if (cVar11 == 0) {
        iVar3 = (int)fn_8012640C(uVar4,0,0x11a,0);
        if (iVar3 != 0) {
          cVar11 = fn_8020981C(uVar1,0x47);
          if (cVar11 == 2) {
            fn_802097C8(uVar1,0x47,0);
          }
          fn_801F4C14(0,0,0x56,0,uVar9);
        }
      }
      else {
        cVar11 = fn_8020981C(uVar1,0x46);
        if (cVar11 == 2) {
          fn_802097C8(uVar1,0x46,0);
        }
      }
    }
    return;
  }
  return;
}
/* Address: 0x80227490 | Size: 0x30 | Ghidra import */
void fn_80227490(void)

{
    extern void fn_802274F0();
  fn_802274F0(0,1,1,0);
  return;
}
/* Address: 0x802274C0 | Size: 0x30 | Ghidra import */
void fn_802274C0(void)

{
    extern void fn_802274F0();
  fn_802274F0(1,1,1,0);
  return;
}
/* Address: 0x802274F0 | Size: 0x508 | Ghidra import */
void fn_802274F0(u32 r3, char r4, char r5, char r6)
{
    extern u32 fn_8010C74C();
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_802026E4();
    extern u32 fn_80207B8C();
    extern u8 fn_80209870();
    extern u8 fn_8020990C();
    extern void fn_802279F8();
    extern u8 lbl_80478D7E;
    extern u32 lbl_8047B618;
  u32 *puVar1;
  u32 *puVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar10;
  u32 uVar11;
  u32 uVar5;
  short sVar12;
  int iVar6;
  u16 uVar13;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  short sVar14;
  u8 cVar15;
  u8 cVar16;
  u32 *puVar17;
  u32 *puVar18;

  int iVar19;
  u32 uStack_fc;
  u32 local_f8 [45];
  
  uVar4 = fn_801F025C(0x11,0);
  uVar10 = fn_80207B8C(uVar4,0);
  uVar11 = fn_80207B8C(uVar4,1);
  fn_80207BF4(uVar4);
  uVar5 = fn_80205184(uVar4);
  sVar12 = fn_8011BEB4(0,uVar5,7,0);
  iVar6 = (int)fn_8012640C(uVar4,0,0xd9,0);
  uVar13 = fn_8011BEB4(iVar6,0,0x30,0);
  uVar7 = fn_801F025C(0x12,0);
  uVar8 = fn_80207B8C(uVar7,0);
  uVar9 = fn_80207B8C(uVar7,1);
  sVar14 = fn_80207BF4(uVar7);
  if (iVar6 != 0) {
    iVar19 = 0x15;
    puVar1 = (u32 *)(iVar6 + -4);
    puVar2 = &uStack_fc;
    do {
      puVar18 = puVar2;
      puVar17 = puVar1;
      uVar3 = puVar17[2];
      puVar18[1] = puVar17[1];
      puVar18[2] = uVar3;
      iVar19 = iVar19 + -1;
      puVar1 = puVar17 + 2;
      puVar2 = puVar18 + 2;
    } while (iVar19 != 0);
    puVar18[3] = puVar17[3];
  }
  if ((((uVar5 & 0xffff) != 0xa5) && ((uVar5 & 0xffff) != 0x164)) &&
     ((r6 != 1 || (sVar12 != 0)))) {
    if (((r3 & 0xff) == 1) && ((uVar10 == uVar13 || (uVar11 == uVar13)))) {
      iVar19 = fn_8011BEB4(iVar6,0,0x2d,0);
      fn_8011BBD8(iVar6,0,0x2d,0,(iVar19 * 0xf) / 10);
    }
    if ((sVar14 == 0x1a) && (uVar13 == 4)) {
      if (r5 == 1) {
        fn_801F4C14(0,0,0x3b,0,0x43);
        fn_801F4C14(0,0,0x3b,0,0x40);
        fn_801254B4(uVar7,0,0xf3,0,0);
        fn_801254B4(uVar7,0,0xf4,0,9);
      }
      lbl_80478D7E = 4;
    }
    else {
      cVar15 = fn_802026E4(uVar7,0x19);
      if ((cVar15 != 1) ||
         ((((uVar8 & 0xffff) != 7 && ((uVar9 & 0xffff) != 7)) || (1 < uVar13)))) {
        uVar3 = fn_8010C74C(uVar13,uVar8);
        fn_802279F8(iVar6,uVar3,uVar5,r3);
        if ((uVar8 & 0xffff) != (uVar9 & 0xffff)) {
          uVar3 = fn_8010C74C(uVar13,uVar9);
          fn_802279F8(iVar6,uVar3,uVar5,r3);
        }
      }
    }
    if (sVar14 == 0x19) {
      sVar14 = fn_8011BEB4(0,uVar5,9,0);
      if ((sVar14 == 0x97) && (cVar15 = (int)fn_801F453C(0,0), cVar15 == 1)) {
        iVar19 = 2;
      }
      else if ((((sVar14 == 0x91) || ((sVar14 == 0x27 || (sVar14 == 0x4b)))) || (sVar14 == 0x97)) ||
              ((sVar14 == 0x9b || (sVar14 == 0x1a)))) {
        if ((lbl_8047B618 & 0x8000000) == 0) {
          iVar19 = 2;
        }
        else {
          iVar19 = 1;
        }
      }
      else {
        iVar19 = 2;
      }
      if ((iVar19 == 2) &&
         (((cVar15 = fn_8020990C(iVar6,0x41), cVar15 == 0 ||
           (cVar15 = fn_80209870(iVar6), cVar15 == 1)) && (sVar12 != 0)))) {
        if (r5 == 1) {
          fn_801F4C14(0,0,0x3b,0,0x40);
          fn_801254B4(uVar7,0,0xf3,0,0);
          fn_801254B4(uVar7,0,0xf4,0,9);
        }
        lbl_80478D7E = 3;
      }
    }
    cVar15 = fn_8020990C(iVar6,0x43);
    if (cVar15 == 1) {
      fn_801254B4(uVar4,0,0x108,0,1);
    }
  }
  if ((r3 & 0xff) == 0) {
    cVar15 = fn_8020990C(iVar6,0x40);
    cVar16 = fn_8020990C(iVar6,0x43);
    if (iVar6 != 0) {
      iVar19 = 0x15;
      puVar1 = &uStack_fc;
      puVar2 = (u32 *)(iVar6 + -4);
      do {
        puVar18 = puVar2;
        puVar17 = puVar1;
        uVar4 = puVar17[2];
        puVar18[1] = puVar17[1];
        puVar18[2] = uVar4;
        iVar19 = iVar19 + -1;
        puVar1 = puVar17 + 2;
        puVar2 = puVar18 + 2;
      } while (iVar19 != 0);
      puVar18[3] = puVar17[3];
    }
    if (cVar15 == 1) {
      fn_801F4C14(0,0,0x3b,0,0x40);
    }
    if (cVar16 == 1) {
      fn_801F4C14(0,0,0x3b,0,0x43);
    }
  }
  if (r4 == 1) {
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  return;
}
/* Address: 0x802279F8 | Size: 0x244 | Ghidra import */
void fn_802279F8(u32 r3, u32 r4, u32 r5, char r6)

{
    extern void fn_8011BBD8();
    extern int fn_8011BEB4();
    extern s8 fn_802096E8();
    extern void fn_802097C8();
    extern s8 fn_8020981C();
    extern s8 fn_8020990C();
    extern void fn_80209960();
  int iVar1;
  u16 sVar2;
  u8 cVar3;
  u32 uVar4;

  iVar1 = fn_8011BEB4(r3,0,0x2d,0);
  sVar2 = fn_8011BEB4(0,r5,7,0);
  cVar3 = fn_8020990C(r3,0x43);
  if (((cVar3 != 1) || (r6 != 0)) && (uVar4 = r4 & 0xffff, uVar4 != 0x3f)) {
    if (uVar4 == 0x43) {
      uVar4 = 0;
    }
    else if (uVar4 == 0x42) {
      uVar4 = 5;
    }
    else if (uVar4 == 0x41) {
      uVar4 = 0x14;
    }
    else {
      return;
    }
    if (r6 == 1) {
      iVar1 = (int)(iVar1 * (u8)uVar4) / 10;
      if ((iVar1 == 0) && ((u8)uVar4 != 0)) {
        iVar1 = 1;
      }
      fn_8011BBD8(r3,0,0x2d,0,iVar1);
    }
    switch ((u8)uVar4) {
    case 0:
      cVar3 = fn_8020981C(r3,r4);
      if (cVar3 == 2) {
        fn_802097C8(r3,r4,0);
      }
      fn_80209960(r3,0x42);
      fn_80209960(r3,0x41);
      break;
    case 5:
      if ((sVar2 != 0) && (cVar3 = fn_802096E8(r3), cVar3 == 1)) {
        cVar3 = fn_8020990C(r3,0x41);
        if (cVar3 == 1) {
          fn_80209960(r3,0x41);
        }
        else {
          cVar3 = fn_8020981C(r3,r4);
          if (cVar3 == 2) {
            fn_802097C8(r3,r4,0);
          }
        }
      }
      break;
    case 0x14:
      if ((sVar2 != 0) && (cVar3 = fn_802096E8(r3), cVar3 == 1)) {
        cVar3 = fn_8020990C(r3,0x42);
        if (cVar3 == 1) {
          fn_80209960(r3,0x42);
        }
        else {
          cVar3 = fn_8020981C(r3,r4);
          if (cVar3 == 2) {
            fn_802097C8(r3,r4,0);
          }
        }
      }
      break;
    }
  }
  return;
}
/* Address: 0x80227C40 | Size: 0x178 | Ghidra import */
void fn_80227C40(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern int fn_80232110();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u16 uVar9;
  u16 sVar10;
  int iVar6;
  u32 uVar7;
  u32 uVar8;
  u8 cVar11;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(0x12,0);
  uVar3 = fn_801F025C(2,uVar2);
  uVar4 = fn_80205184(uVar1);
  uVar5 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar9 = fn_8011BEB4(uVar5,0,0x2f,0);
  sVar10 = fn_8011BEB4(uVar5,0,0x30,0);
  iVar6 = fn_80232110(uVar1,uVar2,uVar3,uVar4,uVar9,sVar10);
  uVar7 = fn_8011BEB4(uVar5,0,0x2b,0);
  uVar8 = fn_8011BEB4(uVar5,0,0x2c,0);
  iVar6 = iVar6 * (uVar7 & 0xff) * (uVar8 & 0xff);
  cVar11 = fn_802026E4(uVar1,0x24);
  if ((cVar11 == 1) && (sVar10 == 0xd)) {
    iVar6 = iVar6 * 2;
  }
  cVar11 = fn_802026E4(uVar1,0x32);
  if (cVar11 == 1) {
    iVar6 = (iVar6 * 0xf) / 10;
  }
  fn_8011BBD8(uVar5,0,0x2d,0,iVar6);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80227DB8 | Size: 0x2d4 | Ghidra import */
void fn_80227DB8(void)

{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u8 fn_801F54A4();
    extern u8 fn_802026E4();
    extern short fn_80203D3C();
    extern u32 fn_80203FE4();
    extern u32 fn_8020A068();
    extern void fn_8020A080();
    extern u32 lbl_80478D60;
  u32 uVar1;
  u32 uVar10;
  u32 uVar2;
  u32 uVar3;
  short sVar11;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  u8 cVar12;
  u32 uVar13;
  u32 uVar14;

  int iVar15;
  int iVar16;
  
  uVar1 = fn_801F025C(0x11,0);
  fn_80207BF4();
  uVar10 = fn_80203FE4(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80205184(uVar1);
  sVar11 = fn_80203D3C(uVar1);
  uVar4 = fn_801F025C(0x12,0);
  iVar15 = 0;
  if (((uVar10 & 0xffff) == 0x3f) && (sVar11 == 0x71)) {
    iVar15 = 1;
  }
  iVar16 = 0;
  if (((uVar10 & 0xffff) == 0x42) && (sVar11 == 0x53)) {
    iVar16 = 1;
  }
  uVar5 = fn_802026E4(uVar1,0xf);
  uVar5 = __cntlzw(1 - (uVar5 & 0xff));
  uVar6 = fn_8011BEB4(0,uVar3,9,0);
  uVar6 = __cntlzw(0x2b - (uVar6 & 0xffff));
  uVar7 = fn_8011BEB4(0,uVar3,9,0);
  uVar7 = __cntlzw(0x4b - (uVar7 & 0xffff));
  uVar8 = fn_8011BEB4(0,uVar3,9,0);
  uVar8 = __cntlzw(200 - (uVar8 & 0xffff));
  uVar9 = fn_8011BEB4(0,uVar3,9,0);
  uVar13 = __cntlzw(0xd1 - (uVar9 & 0xffff));
  uVar14 = (int)lbl_80478D60 - 1;
  uVar9 = __cntlzw(0x29 - (u32)uVar10);
  uVar5 = (uVar6 >> 5) +
          (uVar5 >> 4 & 0xffffffe) + (uVar7 >> 5) + (uVar8 >> 5) + (uVar13 >> 5) + (uVar9 >> 5) +
          iVar15 * 2 + iVar16 * 2 & 0xffff;
  if (uVar14 < uVar5) {
    uVar5 = uVar14 & 0xffff;
  }
  sVar11 = fn_80207BF4(uVar4);
  if ((sVar11 != 4) && (sVar11 = fn_80207BF4(uVar4), sVar11 != 0x4b)) {
    cVar12 = fn_801F54A4(0,0,0x29,0);
    if (cVar12 == 1) {
      fn_8020A080(uVar5);
      uVar5 = fn_8020A068();
      uVar6 = fn_800E0C54();
      if (((uVar6 & 0xffff) == ((uVar6 & 0xffff) / (uVar5 & 0xff)) * (uVar5 & 0xff)) ||
         (((cVar12 = fn_802026E4(uVar1,0x3e), cVar12 == 1 && ((uVar3 & 0xffff) == 0x164)) &&
          (uVar3 = fn_800E0C54(), (uVar3 & 0xffff) % 100 < 0x5a)))) {
        fn_8011BBD8(uVar2,0,0x2b,0,2);
      }
      else {
        fn_8011BBD8(uVar2,0,0x2b,0,1);
      }
      goto LAB_0022506c;
    }
  }
  fn_8011BBD8(uVar2,0,0x2b,0,1);
LAB_0022506c:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8022808C | Size: 0x24c | Ghidra import */
void fn_8022808C(void)

{
    extern int fn_8011BEB4();
    extern int fn_801F025C();
    extern s8 fn_801F3624();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    extern u32 fn_80205B8C();
    extern u32 lbl_8047B618;
  int iVar1;
  u32 uVar2;
  u32 uVar3;
  int iVar4;
  short sVar8;
  u32 uVar5;
  s8 bVar9;
  u8 bVar10;
  u8 bVar11;
  int iVar6;
  s8 cVar12;
  u32 uVar7;

  u8 bVar13;
  
  bVar13 = 1;
  iVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(iVar1,0,0xd9,0);
  uVar3 = fn_80205B8C(iVar1);
  iVar4 = fn_801F025C(0x12,0);
  sVar8 = fn_80207BF4();
  if ((lbl_8047B618 & 0xa00) != 0) goto LAB_002252ac;
  uVar5 = fn_80205184(iVar1);
  bVar9 = fn_8011BEB4(0,uVar5,5,0);
  bVar10 = fn_8011BEB4(uVar2,0,0x26,0);
  bVar11 = (int)fn_8012640C(uVar3,0,0x80,(int)(char)bVar10);
  iVar6 = (int)fn_8012640C(iVar1,0,0x118,0);
  if (iVar6 == 0) {
    if (bVar9 == 6) {
      cVar12 = fn_801F3624(0,0x2e,0,iVar1);
      bVar13 = cVar12 + 1;
    }
    else {
      if (bVar9 < 6) {
        if (bVar9 == 4) {
LAB_002251b0:
          cVar12 = fn_801F3624(0,0x2e,2,iVar1);
          bVar13 = cVar12 + 1;
          goto LAB_002251e8;
        }
      }
      else if (bVar9 < 8) goto LAB_002251b0;
      if ((iVar1 != iVar4) && (sVar8 == 0x2e)) {
        bVar13 = 2;
      }
    }
  }
LAB_002251e8:
  if (bVar11 != 0) {
    fn_801254B4(iVar1,0,0x111,0,1);
    fn_801254B4(uVar3,0,0x80,(int)(char)bVar10,bVar11 - bVar13 & -(bVar13 < bVar11));
    cVar12 = fn_802026E4(iVar1,0x10);
    if ((cVar12 == 0) && (cVar12 = fn_802026E4(iVar1,0x31), cVar12 == 1)) {
      uVar7 = fn_80201890(iVar1,0x31);
      if (((uVar7 & 1 << bVar10) == 0) && (cVar12 = fn_801FECD4(iVar1), cVar12 == 1)) {
        fn_801FE7EC(iVar1,0x80,(u32)bVar10,0);
      }
    }
  }
LAB_002252ac:
  lbl_8047B618 = lbl_8047B618 & 0xfffff7ff;
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802282D8 | Size: 0x1d8 | Ghidra import */
void fn_802282D8(void)

{
    extern void _threadSwitch();
    extern void fn_801DA8C4();
    extern s8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern u32 fn_801F54A4();
    extern void fn_801FB1C0();
    extern void fn_802037DC();
    extern void fn_80211B94();
    extern s8 fn_802623B4();
    extern void fn_8026246C();
    extern void fn_8026532C();
    extern void fn_80265598();
    extern u8 lbl_80478D7F;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  u16 uVar6;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  int iVar4;
  u16 uVar7;
  int iVar5;
  s8 cVar8;
  s8 cVar9;

  uVar6 = fn_801F54A4(0,0,0x14,0);
  if ((lbl_8047B618 & 0x600) == 0) {
    uVar1 = fn_801F025C(0x11,0);
    uVar2 = fn_801F4354(0,uVar1);
    fn_802037DC(uVar1);
    uVar3 = fn_80205184(uVar1);
    iVar4 = (int)fn_8012640C(uVar1,0,0xee,0);
    fn_801FB1C0(uVar2,0,0x4c,0);
    uVar7 = fn_801F54A4(0,0,0x14,0);
    iVar5 = (int)fn_8012640C(uVar1,0,0xee,0);
    if ((iVar5 != 0) && (cVar8 = fn_801DDD28(iVar5,0x9e,4,0), cVar8 != 0)) {
      fn_801DA9E8(iVar5,0x9e,4);
      fn_80265598(uVar1,uVar7,1);
    }
    fn_80265598(uVar1,uVar6,1);
    cVar8 = fn_802623B4(uVar1,uVar3);
    if (iVar4 == 0) {
      fn_80211B94(lbl_8047B62C,0x80378964,0);
    }
    else {
      while (1) {
        cVar9 = fn_801DA94C(iVar4,0x9e,4);
        if (cVar9 == 0) break;
        _threadSwitch();
      }
      fn_801DA8C4(iVar4,0x9e,4);
    }
    if (cVar8 == 1) {
      fn_8026246C();
    }
    fn_8026532C(uVar1,uVar6,0);
    lbl_8047B618 = lbl_8047B618 | 0x400;
  }
  lbl_80478D7F = 0;
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x802284B0 | Size: 0x8fc | Ghidra import */
void fn_802284B0(void)

{
    extern u32 fn_800E0C54();
    extern s8 fn_8010C4A0();
    extern s8 fn_8011BEB4();
    extern short fn_801F0134();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern short fn_802010C8();
    extern short fn_80201D84();
    extern s8 fn_802026E4();
    extern int fn_80203EDC();
    extern short fn_80203FE4();
    extern u32 fn_80205134();
    extern s8 fn_802096E8();
    extern void fn_802097C8();
    extern s8 fn_8020981C();
    extern u32 fn_8020A010();
    extern u32 fn_8020A028();
    extern void fn_8020A040();
    extern s8 fn_8020E4CC();
    extern void fn_802274F0();
    extern int fn_80228DAC();
    extern int fn_80229934();
    extern int fn_8022DCB8();
    extern u8 lbl_80478D7E;
    extern u32 lbl_8047B618;
  u32 bVar1;
  u32 bVar2;
  u16 uVar16;
  u32 uVar3;
  short sVar17;
  u32 uVar4;
  short sVar18;
  u32 uVar5;
  short sVar19;
  short sVar20;
  int iVar6;
  s8 cVar22;
  s8 cVar23;
  int iVar7;
  u32 uVar8;
  s8 cVar24;
  u32 uVar9;
  u32 uVar10;
  s8 cVar25;
  u32 uVar11;
  int iVar12;
  s8 cVar26;
  u32 uVar13;
  u32 uVar14;
  u32 uVar15;
  u16 uVar21;

  u32 uVar27;
  
  uVar16 = fn_801F54A4(0,0,0x14,0);
  uVar21 = *(u16 *)(*(int *)(lbl_8047B610) + 5);
  uVar27 = (u32)uVar21;
  uVar3 = fn_801F025C(0x11,0);
  sVar17 = fn_801F0134(uVar3,uVar16);
  uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
  sVar18 = fn_80207BF4(uVar3);
  uVar5 = fn_801F025C(0x12,0);
  sVar19 = fn_80207BF4();
  sVar20 = fn_80203FE4(uVar5);
  iVar6 = fn_80203EDC(uVar5);
  cVar22 = (int)fn_801F453C(0,1);
  if ((uVar21 == 0xffff) || (uVar21 == 0xfffe)) {
    if ((uVar21 == 0xffff) &&
       ((cVar22 = fn_802026E4(uVar5,0x1d), cVar22 == 1 &&
        (sVar18 = fn_80201D84(uVar5,0x1d), sVar17 == sVar18)))) {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
      return;
    }
    sVar17 = fn_802010C8(uVar5);
    if (sVar17 == 0) {
      fn_801F025C(0x11,0);
      uVar3 = fn_80205184();
      uVar4 = fn_801F025C(0x12,0);
      bVar1 = 0;
      cVar22 = fn_8011BEB4(0,uVar3,0xe,0);
      cVar23 = fn_802026E4(uVar4,0x2b);
      if ((cVar23 == 1) && (cVar22 == 1)) {
        bVar2 = 1;
      }
      else {
        bVar2 = 0;
      }
      if (bVar2) {
        fn_801F4C14(0,0,0x3b,0,0x40);
        uVar3 = fn_801F025C(0x11,0);
        uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
        uVar5 = fn_801F025C(0x12,0);
        cVar22 = fn_802096E8(uVar4);
        if (cVar22 == 0) {
          fn_801254B4(uVar5,0,0xf3,0,0);
          fn_801254B4(uVar5,0,0xf4,0,9);
          *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
        }
        else {
          uVar4 = fn_801F025C(0x11,0);
          iVar6 = fn_801F025C(2,uVar4);
          uVar4 = fn_801F025C(0x12,0);
          iVar7 = fn_801F025C(2,uVar4);
          cVar22 = fn_802026E4(uVar4,0x15);
          if (((cVar22 == 1) && (iVar6 != iVar7)) &&
             ((lbl_8047B618 & 0x1000000) == 0)) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
          cVar22 = fn_8022DCB8(uVar3,uVar5,0);
          if (cVar22 == 0) {
            *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
          }
        }
        bVar1 = 1;
        lbl_80478D7E = 1;
      }
      if (bVar1 == 0) {
        *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
      }
    }
    else {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
  }
  else {
    if (uVar21 == 0) {
      uVar27 = fn_80205184(uVar3);
      uVar8 = fn_80205134(uVar3);
    }
    else {
      uVar8 = fn_8011BEB4(0,uVar27,3,0);
      uVar8 = uVar8 & 0xffff;
    }
    cVar23 = fn_8011BEB4(0,uVar27,5,0);
    cVar24 = fn_80229934(uVar27,uVar3,uVar5);
    if (cVar24 == 1) {
      fn_801F4C14(0,0,0x3b,0,0x45);
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
    else {
      fn_801F025C(0x11,0);
      uVar9 = fn_80205184();
      uVar10 = fn_801F025C(0x12,0);
      bVar1 = 0;
      cVar24 = fn_8011BEB4(0,uVar9,0xe,0);
      cVar25 = fn_802026E4(uVar10,0x2b);
      if ((cVar25 == 1) && (cVar24 == 1)) {
        bVar2 = 1;
      }
      else {
        bVar2 = 0;
      }
      if (bVar2) {
        fn_801F4C14(0,0,0x3b,0,0x40);
        uVar9 = fn_801F025C(0x11,0);
        uVar10 = (int)fn_8012640C(uVar9,0,0xd9,0);
        uVar11 = fn_801F025C(0x12,0);
        cVar24 = fn_802096E8(uVar10);
        if (cVar24 == 0) {
          fn_801254B4(uVar11,0,0xf3,0,0);
          fn_801254B4(uVar11,0,0xf4,0,9);
          *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
        }
        else {
          uVar10 = fn_801F025C(0x11,0);
          iVar7 = fn_801F025C(2,uVar10);
          uVar10 = fn_801F025C(0x12,0);
          iVar12 = fn_801F025C(2,uVar10);
          cVar24 = fn_802026E4(uVar10,0x15);
          if (((cVar24 == 1) && (iVar7 != iVar12)) &&
             ((lbl_8047B618 & 0x1000000) == 0)) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
          cVar24 = fn_8022DCB8(uVar9,uVar11,uVar27);
          if (cVar24 == 0) {
            *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
          }
        }
        bVar1 = 1;
        lbl_80478D7E = 1;
      }
      if ((!bVar1) && (cVar24 = fn_80228DAC(uVar3,uVar5,uVar27), cVar24 == 0)) {
        cVar25 = (int)fn_8012640C(uVar3,0,0xeb,0);
        cVar24 = (int)fn_8012640C(uVar5,0,0xec,0);
        cVar26 = fn_802026E4(uVar5,0x19);
        cVar24 = (cVar25 - cVar24) + 6;
        if (cVar26 == 1) {
          cVar24 = cVar25;
        }
        cVar24 = fn_8020E4CC((int)cVar24);
        uVar13 = fn_8011BEB4(0,uVar27,6,0);
        uVar13 = uVar13 & 0xff;
        if ((cVar22 == 1) && (sVar17 = fn_8011BEB4(0,uVar27,9,0), sVar17 == 0x98)) {
          uVar13 = 0x32;
        }
        fn_8020A040((int)cVar24);
        uVar14 = fn_8020A028();
        fn_8020A040((int)cVar24);
        uVar15 = fn_8020A010();
        uVar13 = (uVar13 * (uVar14 & 0xff)) / (uVar15 & 0xff);
        if (sVar18 == 0xe) {
          uVar13 = (uVar13 * 0x82) / 100 & 0xffff;
        }
        if ((cVar22 == 3) && (sVar19 == 8)) {
          uVar13 = (uVar13 * 0x50) / 100 & 0xffff;
        }
        if ((sVar18 == 0x37) && (cVar22 = fn_8010C4A0(uVar8), cVar22 == 1)) {
          uVar13 = (uVar13 * 0x50) / 100 & 0xffff;
        }
        if (sVar20 == 0x16) {
          uVar13 = (int)(uVar13 * (100 - iVar6)) / 100 & 0xffff;
        }
        uVar8 = fn_800E0C54();
        if (uVar13 < (uVar8 & 0xffff) % 100 + 1) {
          cVar22 = fn_8020981C(uVar4,0x40);
          if (cVar22 == 2) {
            fn_802097C8(uVar4,0x40,0);
          }
          uVar21 = fn_801F54A4(0,0,0x19,0);
          if ((uVar21 < 2) || ((cVar23 != 4 && (cVar23 != 6)))) {
            lbl_80478D7E = 0;
          }
          else {
            lbl_80478D7E = 2;
          }
          cVar22 = (char)lbl_80478D7E;
          fn_802274F0(0,0,0,1);
          if ((lbl_80478D7E == 3) && ((cVar22 == 2 || (cVar22 == 0)))
             ) {
            lbl_80478D7E = cVar22;
          }
        }
        uVar3 = fn_801F025C(0x11,0);
        uVar4 = (int)fn_8012640C(uVar3,0,0xd9,0);
        uVar5 = fn_801F025C(0x12,0);
        cVar22 = fn_802096E8(uVar4);
        if (cVar22 == 0) {
          fn_801254B4(uVar5,0,0xf3,0,0);
          fn_801254B4(uVar5,0,0xf4,0,9);
          *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
        }
        else {
          uVar4 = fn_801F025C(0x11,0);
          iVar6 = fn_801F025C(2,uVar4);
          uVar4 = fn_801F025C(0x12,0);
          iVar7 = fn_801F025C(2,uVar4);
          cVar22 = fn_802026E4(uVar4,0x15);
          if (((cVar22 == 1) && (iVar6 != iVar7)) &&
             ((lbl_8047B618 & 0x1000000) == 0)) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
          cVar22 = fn_8022DCB8(uVar3,uVar5,uVar27);
          if (cVar22 == 0) {
            *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 7;
          }
        }
      }
    }
  }
  return;
}
/* Address: 0x80228DAC | Size: 0x8d0 | Ghidra import */
u32 fn_80228DAC(u32 r3, u32 r4, u32 r5)

{
    extern u16 fn_8011BEB4();
    extern u16 fn_801F0134();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u16 fn_80201D84();
    extern u8 fn_802026E4();
    extern u8 fn_802096E8();
    extern int fn_8022DCB8();
    extern u32 lbl_8047B618;
  u8 bVar1;
  u16 uVar8;
  u16 sVar9;
  u8 cVar11;
  u16 sVar10;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u32 uVar7;

  uVar8 = fn_801F54A4(0,0,0x14,0);
  fn_8011BEB4(0,r5,9,0);
  fn_801F453C(0,1);
  sVar9 = fn_801F0134(r3,uVar8);
  cVar11 = fn_802026E4(r4,0x1d);
  if ((cVar11 == 1) && (sVar10 = fn_80201D84(r4,0x1d), sVar10 == sVar9)) {
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
      fn_801254B4(uVar4,0,0xf3,0,0);
      fn_801254B4(uVar4,0,0xf4,0,9);
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar3 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar3);
      uVar3 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar3);
      cVar11 = fn_802026E4(uVar3,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  if (((lbl_8047B618 & 0x10000) == 0) &&
     (cVar11 = fn_802026E4(r4,0x1f), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
      fn_801254B4(uVar4,0,0xf3,0,0);
      fn_801254B4(uVar4,0,0xf4,0,9);
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar3 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar3);
      uVar3 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar3);
      cVar11 = fn_802026E4(uVar3,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  uVar7 = lbl_8047B618;
  lbl_8047B618 = uVar7 & 0xfffeffff;
  if (((uVar7 & 0x20000) == 0) && (cVar11 = fn_802026E4(r4,0x20), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
      fn_801254B4(uVar4,0,0xf3,0,0);
      fn_801254B4(uVar4,0,0xf4,0,9);
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar3 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar3);
      uVar3 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar3);
      cVar11 = fn_802026E4(uVar3,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  uVar7 = lbl_8047B618;
  lbl_8047B618 = uVar7 & 0xfffdffff;
  if (((uVar7 & 0x40000) == 0) && (cVar11 = fn_802026E4(r4,0x21), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
      fn_801254B4(uVar4,0,0xf3,0,0);
      fn_801254B4(uVar4,0,0xf4,0,9);
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar3 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar3);
      uVar3 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar3);
      cVar11 = fn_802026E4(uVar3,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 & 0xfffbffff;
  sVar9 = fn_8011BEB4(0,r5,9,0);
  cVar11 = (int)fn_801F453C(0,1);
  if ((cVar11 == 2) && (sVar9 == 0x98)) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if (bVar1 == 1) {
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
      fn_801254B4(uVar4,0,0xf3,0,0);
      fn_801254B4(uVar4,0,0xf4,0,9);
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar3 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar3);
      uVar3 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar3);
      cVar11 = fn_802026E4(uVar3,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
      if (cVar11 == 0) {
        lbl_8047B610 = lbl_8047B610 + 7;
      }
    }
    uVar2 = 1;
  }
  else {
    sVar9 = fn_8011BEB4(0,r5,9,0);
    if ((sVar9 == 0x11) || (sVar9 == 0x4e)) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1 == 1) {
      uVar2 = fn_801F025C(0x11,0);
      uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
      uVar4 = fn_801F025C(0x12,0);
      cVar11 = fn_802096E8(uVar3);
      if (cVar11 == 0) {
        fn_801254B4(uVar4,0,0xf3,0,0);
        fn_801254B4(uVar4,0,0xf4,0,9);
        lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
      }
      else {
        uVar3 = fn_801F025C(0x11,0);
        uVar5 = fn_801F025C(2,uVar3);
        uVar3 = fn_801F025C(0x12,0);
        uVar6 = fn_801F025C(2,uVar3);
        cVar11 = fn_802026E4(uVar3,0x15);
        if (((cVar11 == 1) && (uVar5 != uVar6)) &&
           ((lbl_8047B618 & 0x1000000) == 0)) {
          lbl_8047B618 = lbl_8047B618 | 0x40;
        }
        cVar11 = fn_8022DCB8(uVar2,uVar4,r5);
        if (cVar11 == 0) {
          lbl_8047B610 = lbl_8047B610 + 7;
        }
      }
      uVar2 = 1;
    }
    else {
      uVar2 = 0;
    }
  }
  return uVar2;
}
/* Address: 0x8022967C | Size: 0x88 | Ghidra import */
u32 fn_8022967C(u32 r3)

{
    extern u8 fn_80077AF4();
    extern u16 fn_8011BEB4();
    extern u8 fn_801F54A4();
  u32 cVar3;
  u8 cVar4;
  u32 sVar2;

  cVar3 = fn_801F54A4(0,0,0x34,0);
  cVar4 = fn_80077AF4();
  sVar2 = fn_8011BEB4(0,r3,9,0);
  if (((cVar3 == 1) && (cVar4 == 1)) && (sVar2 == 7)) {
    return 1;
  }
  return 0;
}
/* Address: 0x80229704 | Size: 0x230 | Ghidra import */
u32 fn_80229704(u32 r3, u32 r4)

{
    extern u8 fn_80077B3C();
    extern u8 fn_80077B60();
    extern u32 fn_801F025C();
    extern u8 fn_801F54A4();
    extern int fn_801F7258();
    extern int fn_801F986C();
    extern u8 fn_80202ADC();
    extern u8 fn_80206608();
  u8 cVar5;
  u8 cVar6;
  u8 cVar7;
  u32 uVar1;
  u16 uVar4;
  int iVar2;
  int iVar3;
  short sVar8;
  u16 uVar9;
  u32 uVar10;
  
  cVar5 = fn_801F54A4(0,0,0x34,0);
  cVar6 = fn_80077B60();
  cVar7 = fn_80077B3C();
  if (cVar5 == 1) {
    if ((r3 & 0xffff) == 8) {
      if (cVar6 != 1) {
        sVar8 = 0;
        uVar1 = fn_801F025C(2,r4);
        uVar4 = fn_801F54A4(0,0,0x16,0);
        fn_801F54A4(0,0,0x17,0);
        for (uVar9 = 0; uVar9 < (uVar4 & 0xffff); uVar9 = uVar9 + 1) {
          iVar2 = fn_801F7258(uVar1,uVar9);
          if (iVar2 != 0) {
            for (uVar10 = 0; (uVar10 & 0xffff) < 6; uVar10 = uVar10 + 1) {
              iVar3 = fn_801F986C(iVar2,uVar10);
              if (((iVar3 != 0) && (cVar5 = fn_80206608(), cVar5 != 0)) &&
                 (cVar5 = fn_80202ADC(iVar3,r3), cVar5 == 1)) {
                sVar8 = sVar8 + 1;
              }
            }
          }
        }
        if (sVar8 != 0) {
          return 1;
        }
      }
    }
    else if (((r3 & 0xffff) == 7) && (cVar7 != 1)) {
      sVar8 = 0;
      uVar1 = fn_801F025C(2,r4);
      uVar4 = fn_801F54A4(0,0,0x16,0);
      fn_801F54A4(0,0,0x17,0);
      for (uVar9 = 0; uVar9 < (uVar4 & 0xffff); uVar9 = uVar9 + 1) {
        iVar2 = fn_801F7258(uVar1,uVar9);
        if (iVar2 != 0) {
          for (uVar10 = 0; (uVar10 & 0xffff) < 6; uVar10 = uVar10 + 1) {
            iVar3 = fn_801F986C(iVar2,uVar10);
            if (((iVar3 != 0) && (cVar5 = fn_80206608(), cVar5 != 0)) &&
               (cVar5 = fn_80202ADC(iVar3,7), cVar5 == 1)) {
              sVar8 = sVar8 + 1;
            }
          }
        }
      }
      if (sVar8 != 0) {
        return 1;
      }
    }
  }
  return 0;
}
/* Address: 0x80229934 | Size: 0x23c | Ghidra import */
u32 fn_80229934(u32 r3, u32 r4, u32 r5)

{
    extern u8 fn_80077AAC();
    extern u8 fn_80077AD0();
    extern u8 fn_80077B18();
    extern void fn_80077B3C();
    extern u8 fn_80077B60();
    extern u16 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u8 fn_801F54A4();
    extern u32 fn_801F6FD4();
    extern int fn_801F7258();
    extern int fn_801F986C();
    extern u8 fn_80202ADC();
    extern u8 fn_80206608();
  u8 cVar8;
  u8 cVar9;
  u8 cVar10;
  u8 cVar11;
  u8 cVar12;
  u32 uVar1;
  u16 uVar4;
  u16 uVar5;
  u16 uVar6;
  u16 sVar7;
  int iVar2;
  int iVar3;
  u32 uVar13;
  u16 uVar14;
  
  cVar8 = fn_801F54A4(0,0,0x34,0);
  cVar9 = fn_80077B18();
  cVar10 = fn_80077AD0();
  cVar11 = fn_80077AAC();
  cVar12 = fn_80077B60();
  fn_80077B3C();
  if (cVar8 == 1) {
    uVar13 = r3 & 0xffff;
    if (uVar13 == 0x11d) {
      if (cVar9 != 1) {
        return 1;
      }
    }
    else if ((uVar13 == 0xc3) || (uVar13 == 0xc2)) {
      uVar1 = fn_801F025C(2,r4);
      uVar4 = fn_801F54A4(0,0,0x16,0);
      uVar5 = fn_801F54A4(0,0,0x17,0);
      uVar6 = fn_801F6FD4(uVar1,uVar4,uVar5);
      if ((uVar6 < 2) && (cVar10 != 1)) {
        return 1;
      }
    }
    else if ((uVar13 == 0x52) || (uVar13 == 0x31)) {
      if (cVar11 != 1) {
        return 1;
      }
    }
    else {
      sVar7 = fn_8011BEB4(0,r3,9,0);
      if (sVar7 == 1) {
        sVar7 = 0;
        uVar1 = fn_801F025C(2,r5);
        uVar6 = fn_801F54A4(0,0,0x16,0);
        fn_801F54A4(0,0,0x17,0);
        for (uVar14 = 0; uVar14 < uVar6; uVar14 = uVar14 + 1) {
          iVar2 = fn_801F7258(uVar1,uVar14);
          if (iVar2 != 0) {
            for (uVar13 = 0; (uVar13 & 0xffff) < 6; uVar13 = uVar13 + 1) {
              iVar3 = fn_801F986C(iVar2,uVar13);
              if (((iVar3 != 0) && (cVar8 = fn_80206608(), cVar8 != 0)) &&
                 (cVar8 = fn_80202ADC(iVar3,8), cVar8 == 1)) {
                sVar7 = sVar7 + 1;
              }
            }
          }
        }
        if ((cVar12 != 1) && (sVar7 != 0)) {
          return 1;
        }
      }
    }
  }
  return 0;
}
/* Address: 0x80229B70 | Size: 0x68 | Ghidra import */
u32 fn_80229B70(u32 r3)

{
    extern u16 fn_8011BEB4();
    extern u8 fn_801F453C();
  u32 sVar2;
  u8 cVar3;
  u32 uVar1;

  sVar2 = fn_8011BEB4(0,r3,9,0);
  cVar3 = fn_801F453C(0,1);
  if ((cVar3 == 2) && (sVar2 == 0x98)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* fn_80229BD8 | Size: 0x50 | Check if item type is 0x11 or 0x4E */
BOOL fn_80229BD8(u32 itemId) {
    extern u16 fn_8011BEB4(u32 context, u32 item, u32 field, u32 flags);
    u16 type = fn_8011BEB4(0, itemId, 9, 0);
    if (type == 0x11 || type == 0x4E) {
        return TRUE;
    }
    return FALSE;
}

/* Address: 0x80229C28 | Size: 0x68 | Ghidra import */
u32 fn_80229C28(u32 r3, u32 r4)

{
    extern u8 fn_8011BEB4();
    extern u8 fn_802026E4();
  u32 cVar2;
  u8 cVar3;
  u32 uVar1;

  cVar2 = (u8)fn_8011BEB4(0,r4,0xe,0);
  cVar3 = fn_802026E4(r3,0x2b);
  if ((cVar3 == 1) && (cVar2 == 1)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x80229C90 | Size: 0x1c4 | Ghidra import */
void fn_80229C90(void)

{
    extern u8 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_802026E4();
    extern u8 fn_802096E8();
    extern u8 fn_8022DCB8();
    extern u8 lbl_80478D78;
    extern u32 lbl_8047B618;
  u32 uVar1;
  u8 cVar6;
  u8 cVar7;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;

  fn_801F025C(0x11,0);
  uVar1 = fn_80205184();
  cVar6 = fn_8011BEB4(0,uVar1,0xe,0);
  uVar1 = fn_801F025C(0x12,0);
  cVar7 = fn_802026E4(uVar1,0x2b);
  if ((cVar7 == 1) && (cVar6 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar1 = fn_801F025C(0x11,0);
    uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
    uVar3 = fn_801F025C(0x12,0);
    cVar6 = fn_802096E8(uVar2);
    if (cVar6 == 0) {
      fn_801254B4(uVar3,0,0xf3,0,0);
      fn_801254B4(uVar3,0,0xf4,0,9);
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar2 = fn_801F025C(0x11,0);
      uVar4 = fn_801F025C(2,uVar2);
      uVar2 = fn_801F025C(0x12,0);
      uVar5 = fn_801F025C(2,uVar2);
      cVar6 = fn_802026E4(uVar2,0x15);
      if (cVar6 == 1) {
        if (uVar4 != uVar5) {
          if ((lbl_8047B618 & 0x1000000) == 0) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
        }
      }
      cVar6 = fn_8022DCB8(uVar1,uVar3,0);
      if (cVar6 == 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
      }
    }
    *((&lbl_80478D78)+6) = 1;
  }
  else {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x80229E54 | Size: 0x6b0 | Ghidra import */
void fn_80229E54(void)

{
    extern void fn_8011BBD8();
    extern s8 fn_8011BEB4();
    extern int fn_80123B5C();
    extern short fn_801EF634();
    extern int fn_801F025C();
    extern void fn_801F37B0();
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_80200B10();
    extern u32 fn_80201890();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern s8 fn_80207AE0();
    extern void fn_802097C8();
    extern s8 fn_8020981C();
    extern void fn_8020A2B8();
    extern void fn_80211B94();
    extern int fn_8022A6C8();
    extern int fn_8022F2F8();
    extern u8 lbl_80478D7E;
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  u32 bVar1;
  int iVar2;
  u32 uVar3;
  int iVar4;
  short sVar10;
  s8 cVar12;
  u32 uVar5;
  int iVar6;
  u32 uVar7;
  s8 cVar13;
  s8 cVar14;
  u32 uVar8;
  u8 bVar15;
  int iVar9;
  u16 uVar11;

  int local_28;
  int local_24;
  
  iVar2 = fn_801F025C(0x11,0);
  uVar3 = fn_80205B8C();
  iVar4 = fn_801F025C(0x12,0);
  sVar10 = fn_801EF634();
  if (sVar10 == 0) {
    cVar12 = fn_802062FC(iVar2);
    if ((cVar12 == 0) && ((lbl_8047B618 & 0x200) == 0)) {
      lbl_8047B618 = lbl_8047B618 | 0x80000;
      *(u32 *)(lbl_8047B610) = 0x80375fbf;
    }
    else {
      cVar12 = fn_8022F2F8();
      if (cVar12 == 0) {
        fn_80207BF4(iVar2);
        uVar5 = fn_80205184(iVar2);
        sVar10 = fn_80207BF4(iVar4);
        bVar1 = 0;
        if (((sVar10 == 0x2b) && (cVar12 = fn_802062FC(iVar4), cVar12 != 0)) &&
           (cVar12 = fn_8011BEB4(0,uVar5,0x17,0), cVar12 == 1)) {
          cVar12 = fn_802026E4(iVar2,0x22);
          if (cVar12 == 1) {
            lbl_8047B618 = lbl_8047B618 | 0x800;
          }
          bVar1 = 1;
          *(u32 *)(lbl_8047B610) = 0x8037989e;
        }
        if (bVar1 == 0) {
          iVar6 = (int)fn_8012640C(iVar2,0,0xd9,0);
          uVar7 = fn_80205184(iVar2);
          cVar12 = fn_8011BEB4(0,uVar7,0xe,0);
          cVar13 = fn_8011BEB4(0,uVar7,0xf,0);
          cVar14 = fn_8011BEB4(iVar6,0,0x26,0);
          if (((cVar14 < 0) || ((uVar7 & 0xffff) == 0xa5)) || ((uVar7 & 0xffff) == 0x164)) {
            cVar14 = 0;
          }
          else {
            cVar14 = (int)fn_8012640C(uVar3,0,0x80);
          }
          if (((cVar14 == 0) && ((uVar7 & 0xffff) != 0xa5)) &&
             (((uVar7 & 0xffff) != 0x164 &&
              ((cVar14 = fn_802026E4(iVar2,0x22), cVar14 == 0 &&
               ((lbl_8047B618 & 0x800200) == 0)))))) {
            cVar12 = fn_8020981C(iVar6,0x40);
            if (cVar12 == 2) {
              fn_802097C8(iVar6,0x40,0);
            }
            *(u32 *)(lbl_8047B610) = 0x80379021;
            lbl_8047B618 = lbl_8047B618 | 0x80000;
          }
          else {
            uVar8 = lbl_8047B618;
            lbl_8047B618 = uVar8 & 0xff7fffff;
            if (((uVar8 & 0x2000000) == 0) &&
               ((cVar14 = fn_802026E4(iVar2,0x22), cVar14 == 0 &&
                (cVar14 = fn_8022A6C8(iVar2), cVar14 != 0)))) {
              if (cVar14 == 2) {
                lbl_8047B618 = lbl_8047B618 | 0x2000000;
                return;
              }
              cVar12 = fn_8020981C(iVar6,0x40);
              if (cVar12 != 2) {
                return;
              }
              fn_802097C8(iVar6,0x40,0);
              return;
            }
            lbl_8047B618 = lbl_8047B618 | 0x2000000;
            cVar14 = fn_802026E4(iVar4,0x37);
            if ((cVar14 == 1) && (cVar13 == 1)) {
              if (((iVar2 != 0) && (iVar4 != 0)) && (sVar10 = fn_80207BF4(iVar2), sVar10 == 0x2e))
              {
                uVar3 = fn_80205B8C(iVar4);
                bVar15 = fn_80123B5C(uVar3,0x115);
                if (-1 < (char)bVar15) {
                  cVar13 = (int)fn_8012640C(uVar3,0,0x80,(int)(char)bVar15);
                  cVar12 = 0;
                  if (cVar13 != 0) {
                    cVar12 = cVar13 + -1;
                  }
                  fn_801254B4(uVar3,0,0x80,(int)(char)bVar15,cVar12);
                  cVar12 = fn_802026E4(iVar4,0x10);
                  if (((cVar12 == 0) && (cVar12 = fn_802026E4(iVar4,0x31), cVar12 == 1)) &&
                     ((uVar8 = fn_80201890(iVar4,0x31), (uVar8 & 1 << (u32)bVar15) == 0 &&
                      (cVar12 = fn_801FECD4(iVar4), cVar12 == 1)))) {
                    fn_801FE7EC(iVar4,0x80,(u32)bVar15,0);
                  }
                }
              }
              fn_80202810(iVar4,0x37);
              iVar2 = (int)fn_8012640C(iVar4,0,0xd9,0);
              fn_80211B94(lbl_8047B62C,0x8037917b,0);
              if ((iVar6 != 0) && (iVar2 != 0)) {
                fn_8020A2B8(iVar2,iVar6);
                fn_8011BBD8(iVar2,0,0x27,0,uVar7 & 0xffff);
              }
            }
            else {
              local_24 = 0;
              local_28 = iVar2;
              fn_801F37B0(0,0x8022a504,&local_28,1);
              iVar6 = local_24;
              if (local_24 != 0) {
                iVar9 = (int)fn_8012640C(iVar2,0,0xd9,0);
                iVar6 = (int)fn_8012640C(iVar6,0,0xd9,0);
                if ((iVar6 != 0) && (iVar9 != 0)) {
                  uVar11 = fn_8011BEB4(iVar6,0,0x28,0);
                  fn_8020A2B8(iVar6,iVar9);
                  fn_8011BBD8(iVar6,0,0x27,0,uVar11);
                }
              }
              iVar6 = (int)fn_8012640C(iVar4,0,0x114,0);
              if (iVar6 == 1) {
                fn_801254B4(iVar4,0,0x114,0,0);
                fn_80211B94(lbl_8047B62C,0x803796f3,0);
              }
              cVar13 = fn_802026E4(iVar4,0x2b);
              if ((cVar13 == 1) && (cVar12 == 1)) {
                if (((uVar7 & 0xffff) == 0xae) && (cVar12 = fn_80207AE0(iVar2,7), cVar12 == 0))
                {
                  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
                  return;
                }
                sVar10 = fn_8011BEB4(0,uVar7,9,0);
                if (((((sVar10 == 0x91) || (sVar10 == 0x27)) || (sVar10 == 0x4b)) ||
                    ((sVar10 == 0x97 || (sVar10 == 0x9b)))) || (sVar10 == 0x1a)) {
                  bVar1 = 1;
                }
                else {
                  bVar1 = 0;
                }
                if ((!bVar1) || (cVar12 = fn_802026E4(iVar2,0x22), cVar12 == 1)) {
                  fn_80200B10(iVar2);
                  fn_801F4C14(0,0,0x3b,0,0x40);
                  fn_801254B4(iVar4,0,0xf3,0,0);
                  fn_801254B4(iVar4,0,0xf4,0,9);
                  lbl_80478D7E = 1;
                  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
                  return;
                }
              }
              *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
            }
          }
        }
      }
    }
  }
  else {
    lbl_8047B614 = 2;
  }
  return;
}
/* Address: 0x8022A504 | Size: 0x1c4 | Ghidra import */
u32 fn_8022A504(void)

{
    int r3;
    u32 r4;
    int *r5;

    extern s8 fn_8011BEB4();
    extern int fn_80123B5C();
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern u32 fn_80201890();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
  u32 uVar1;
  s8 cVar4;
  s8 cVar5;
  short sVar3;
  u8 bVar6;
  u32 uVar2;

  int iVar7;
  
  iVar7 = *r5;
  uVar1 = fn_80205184(iVar7);
  cVar4 = fn_8011BEB4(0,uVar1,0x10,0);
  cVar5 = fn_802062FC(r3);
  if (((cVar5 != 0) && (cVar5 = fn_802026E4(r3,0x33), cVar5 == 1)) &&
     (cVar4 == 1)) {
    if (((iVar7 != 0) && (r3 != 0)) && (sVar3 = fn_80207BF4(iVar7), sVar3 == 0x2e)) {
      uVar1 = fn_80205B8C(r3);
      bVar6 = fn_80123B5C(uVar1,0x121);
      if (-1 < (char)bVar6) {
        cVar5 = (int)fn_8012640C(uVar1,0,0x80,(int)(char)bVar6);
        cVar4 = 0;
        if (cVar5 != 0) {
          cVar4 = cVar5 + -1;
        }
        fn_801254B4(uVar1,0,0x80,(int)(char)bVar6,cVar4);
        cVar4 = fn_802026E4(r3,0x10);
        if (((cVar4 == 0) && (cVar4 = fn_802026E4(r3,0x31), cVar4 == 1)) &&
           ((uVar2 = fn_80201890(r3,0x31), (uVar2 & 1 << (u32)bVar6) == 0 &&
            (cVar4 = fn_801FECD4(r3), cVar4 == 1)))) {
          fn_801FE7EC(r3,0x80,(u32)bVar6,0);
        }
      }
    }
    fn_80202810(r3,0x33);
    fn_801F4C14(0,0,0x4b,0,r3);
    fn_80211B94(lbl_8047B62C,0x8037919d,0);
    r5[1] = r3;
  }
  return 1;
}
/* Address: 0x8022A6C8 | Size: 0xbd4 | Ghidra import */
u32 fn_8022A6C8(void)

{
    u32 r3;

    extern u32 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern s8 fn_8011BEB4();
    extern u32 fn_8011CC54();
    extern u32 fn_8011CC6C();
    extern u32 fn_8011CC84();
    extern u32 fn_8011CC9C();
    extern u32 fn_8011CCB4();
    extern u32 fn_8011CCCC();
    extern u32 fn_8011CCE4();
    extern void fn_8011CE18();
    extern s8 fn_8011FC74();
    extern s8 fn_80123CD4();
    extern void fn_80132A38();
    extern s8 fn_80142984();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern int fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801F8100();
    extern int fn_801F8A18();
    extern void fn_801FB1C0();
    extern void fn_801FBA24();
    extern s8 fn_801FFEC8();
    extern s8 fn_802026E4();
    extern void fn_80203E0C();
    extern void fn_80203FE4();
    extern u32 fn_80205B8C();
    extern void fn_80208404();
    extern void fn_80208554();
    extern void fn_802085C4();
    extern void fn_802086E8();
    extern void fn_80208750();
    extern void fn_80208ED0();
    extern void fn_802099AC();
    extern int fn_8022B2CC();
    extern int fn_8022BE2C();
    extern int fn_80232110();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern u16 lbl_8047B60C;
    extern u32 lbl_8047B618;
  u32 bVar1;
  u16 uVar11;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  int iVar5;
  u32 uVar6;
  short sVar12;
  u32 uVar7;
  u32 uVar8;
  u8 uVar14;
  s8 cVar15;
  u32 uVar9;
  short sVar13;
  u32 uVar10;
  u8 bVar16;

  u32 uVar17;
  u16 local_48 [2];
  short local_44 [12];
  
  uVar11 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_80205B8C(r3);
  uVar3 = fn_801F025C(2,r3);
  uVar4 = (int)fn_8012640C(r3,0,0xd9,0);
  fn_80203E0C(r3);
  iVar5 = fn_801F4354(0,r3);
  if (iVar5 == 0) {
    return 0;
  }
  uVar6 = fn_801F025C(9,iVar5);
  fn_801FB1C0(iVar5,0,0x44,0);
  sVar12 = fn_80205184(r3);
  uVar7 = fn_802040E8(r3);
  fn_80203FE4(r3);
  uVar8 = fn_80205B8C(r3);
  uVar14 = (int)fn_8012640C(uVar8,0,0xbf,0);
  cVar15 = fn_8011BEB4(uVar4,0,0x32,0);
  if ((cVar15 != 1) && (cVar15 = fn_801F54A4(0,0,0x31,0), cVar15 != 0)) {
    fn_80205B8C(r3);
    cVar15 = fn_8011FC74();
    if ((cVar15 != 0) &&
       ((cVar15 = fn_802026E4(r3,0x3e), cVar15 != 0 && (sVar12 != 0x164)))) {
      uVar8 = fn_801F8100(iVar5);
      fn_80132A38(0x13,uVar8);
      uVar9 = fn_800E0C54();
      uVar17 = (uVar9 & 0xffff) % 100;
      fn_8011CE18(uVar14);
      uVar9 = fn_8011CCE4();
      if (uVar17 < (uVar9 & 0xff)) {
        for (bVar16 = 0; bVar16 < 4; bVar16 = bVar16 + 1) {
          local_44[bVar16] = -1;
        }
        bVar16 = 0;
        for (sVar12 = 0; sVar12 < 4; sVar12 = sVar12 + 1) {
          cVar15 = fn_80123CD4(uVar2,sVar12);
          if ((((cVar15 != 0) && (cVar15 = fn_801FFEC8(r3,sVar12,0,0), cVar15 == 0)) &&
              (sVar13 = (int)fn_8012640C(uVar2,0,0x7f,sVar12), sVar13 != 0)) &&
             ((sVar13 != 0x165 && (sVar13 != 0x163)))) {
            local_44[bVar16] = sVar12;
            bVar16 = bVar16 + 1;
          }
        }
        if (bVar16 != 0) {
          uVar7 = fn_800E0C54();
          sVar12 = *(short *)((int)local_44 +
                             (((uVar7 & 0xffff) - ((uVar7 & 0xffff) / (u32)bVar16) * (u32)bVar16)
                              * 2 & 0x1fe));
          if (((-1 < sVar12) && (sVar13 = (int)fn_8012640C(uVar2,0,0x7f,sVar12), sVar13 != 0)) &&
             ((sVar13 != 0x165 && (sVar13 != 0x163)))) {
            lbl_8047B60C = sVar13;
            uVar7 = lbl_8047B618 & 0xfffffbff;
            lbl_8047B618 = uVar7;
            lbl_8047B618 = uVar7 | 0x200000;
            uVar2 = fn_8022B2CC(r3,sVar13,uVar11,0x8022b29c,1,1, (void*)0xffffffff);
            uVar3 = fn_801F0134(uVar2,uVar11);
            fn_802099AC(uVar4,(int)(char)sVar12,sVar13,uVar3,0);
            fn_801F4C14(0,0,0x43,0,uVar2);
            fn_8011BEB4(0,sVar13,1,0);
            uVar2 = fn_800FA280();
            fn_80132A38(0x28,uVar2);
            fn_80208404(r3,0,1,0);
            fn_80208404(r3,0,2,0);
            fn_80208404(r3,0,1,1);
            fn_80208404(r3,0,1,2);
            fn_80208404(r3,0,2,1);
            fn_802624CC(0x770c);
            fn_80208404(r3,0,2,2);
            fn_8026246C();
            fn_80208404(r3,0,1,3);
            fn_80208404(r3,0,2,3);
            *(u32 *)(lbl_8047B610) = 0x803799ed;
            lbl_8047B618 = lbl_8047B618 | 0x400;
            return 2;
          }
        }
      }
      else {
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CCCC();
        uVar9 = (uVar9 & 0xff) + (uVar10 & 0xff);
        if (uVar17 < uVar9) {
          fn_80208404(r3,0,1,0);
          fn_80208750(r3,1,1,0);
          fn_801FBA24(uVar6,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_802085C4(r3,1,1,0, (void*)0xffffffff);
          fn_802624CC(0x770d);
          fn_80208554(r3,1,1,6);
          fn_8026246C();
          fn_801FBA24(uVar6,1);
          fn_801FBA24(uVar6,2);
          fn_80208404(r3,0,1,3);
          fn_802086E8(r3,1,1);
          fn_801FBA24(uVar6,3);
          *(u32 *)(lbl_8047B610) = 0x803799f4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CCB4();
        uVar9 = uVar9 + (uVar10 & 0xff);
        if (uVar17 < uVar9) {
          fn_80208404(r3,0,1,0);
          fn_80208750(r3,1,1,0);
          fn_801FBA24(iVar5,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_802085C4(r3,1,1,0, (void*)0xffffffff);
          fn_802624CC(0x770e);
          fn_80208554(r3,1,1,6);
          fn_8026246C();
          fn_801FBA24(iVar5,1);
          fn_801FBA24(iVar5,2);
          fn_80208404(r3,0,1,3);
          fn_802086E8(r3,1,1);
          fn_801FBA24(iVar5,3);
          *(u32 *)(lbl_8047B610) = 0x803799f4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC9C();
        uVar9 = uVar9 + (uVar10 & 0xff);
        if (uVar17 < uVar9) {
          fn_80208404(r3,0,1,0);
          fn_80208404(r3,0,2,0);
          fn_801F4C14(0,0,0x43,0,r3);
          uVar2 = fn_80232110(r3,r3,uVar3,1,0x28, (void*)0xffffffff);
          fn_8011BBD8(uVar4,0,0x2d,0,uVar2);
          fn_801254B4(r3,0,0x107,0,1);
          lbl_8047B618 = lbl_8047B618 | 0x80000;
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_80208404(r3,0,2,1);
          fn_802624CC(0x770f);
          fn_80208404(r3,0,2,2);
          fn_8026246C();
          fn_80208404(r3,0,1,3);
          fn_80208404(r3,0,2,3);
          *(u32 *)(lbl_8047B610) = 0x803799ef;
          return 2;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC84();
        uVar9 = uVar9 + (uVar10 & 0xff);
        if (uVar9 <= uVar17) {
          fn_8011CE18(uVar14);
          uVar10 = fn_8011CC6C();
          uVar9 = uVar9 + (uVar10 & 0xff);
          if (uVar17 < uVar9) {
            fn_80208404(r3,0,1,0);
            fn_80208750(r3,0x85,3,0);
            fn_80208404(r3,0,1,1);
            fn_80208404(r3,0,1,2);
            cVar15 = fn_80142984(uVar7);
            if (cVar15 == 0) {
              fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
              fn_802624CC(0x7712);
              fn_80208554(r3,0x85,3,6);
              fn_8026246C();
            }
            else {
              uVar9 = uVar7 & 0xffff;
              if ((((uVar9 == 0x2c) || ((uVar7 - 0x85 & 0xffff) < 0xf)) ||
                  ((uVar7 - 0xa8 & 0xffff) < 7)) || ((uVar9 == 0xb4 || (uVar9 == 0xb9)))) {
                bVar1 = 1;
              }
              else {
                bVar1 = 0;
              }
              if ((bVar1) && (cVar15 = fn_8022BE2C(r3,2), cVar15 != 0)) {
                fn_80208404(r3,0,1,3);
                fn_802086E8(r3,0x85,3);
                *(u32 *)(lbl_8047B610) = 0x803799f4;
                return 1;
              }
              fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
              fn_802624CC(0x7711);
              fn_80208554(r3,0x85,3,6);
              fn_8026246C();
            }
            fn_80208404(r3,0,1,3);
            fn_802086E8(r3,0x85,3);
            *(u32 *)(lbl_8047B610) = 0x803799f4;
            return 1;
          }
          fn_8011CE18(uVar14);
          uVar7 = fn_8011CC54();
          if (uVar9 + (uVar7 & 0xff) <= uVar17) {
            return 0;
          }
          fn_80208404(r3,0,1,0);
          fn_80208404(r3,0,0,0);
          fn_80208ED0(r3,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          local_48[0] = 0;
          iVar5 = fn_801F8A18(iVar5,local_48);
          if (iVar5 == 0) {
            fn_80208404(r3,0,0,1);
            fn_802624CC(0x7714);
            fn_80208404(r3,0,0,2);
            fn_8026246C();
            fn_80208404(r3,0,1,3);
            fn_80208404(r3,0,0,3);
            fn_80208ED0(r3,3);
            *(u32 *)(lbl_8047B610) = 0x803799f4;
            return 1;
          }
          fn_80208ED0(r3,1);
          fn_802624CC(0x7713);
          fn_80208ED0(r3,2);
          fn_8026246C();
          fn_80208404(r3,0,1,3);
          fn_80208404(r3,0,0,3);
          fn_80208ED0(r3,3);
          fn_80208ED0(r3,4);
          *(u32 *)(lbl_8047B610) = 0x803799fe;
          return 1;
        }
      }
      fn_80208404(r3,0,1,0);
      fn_80208750(r3,0x85,3,0);
      fn_80208404(r3,0,1,1);
      fn_80208404(r3,0,1,2);
      fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
      fn_802624CC(0x7710);
      fn_80208554(r3,0x85,3,6);
      fn_8026246C();
      fn_80208404(r3,0,1,3);
      fn_802086E8(r3,0x85,3);
      *(u32 *)(lbl_8047B610) = 0x803799f4;
      return 1;
    }
  }
  return 0;
}
/* Address: 0x8022B29C | Size: 0x30 | Ghidra import */
void fn_8022B29C(u32 r3)

{
    extern void fn_801F2598();
  fn_801F2598(0,1,3,r3);
  return;
}
/* Address: 0x8022B2CC | Size: 0xfc | Ghidra import */
u32 fn_8022B2CC(void)

{
    u32 r3;
    u32 r4;

    extern int fn_8011BEB4();
    extern void fn_801F2654();
    extern void fn_801F3624();
    extern s8 fn_80207AE0();
  u32 uVar1;
  s8 cVar3;
  u32 uVar2;
  u8 in_r9;
  
  if ((char)in_r9 < 0) {
    in_r9 = fn_8011BEB4(0,r4,5,0);
  }
  uVar1 = (u32)in_r9;
  if (((r4 & 0xffff) == 0xae) && (cVar3 = fn_80207AE0(r3,7), cVar3 == 0)) {
    uVar1 = 5;
  }
  fn_8011BEB4(0,r4,3,0);
  fn_801F2654(0,0,r3,1);
  fn_801F3624(0,0x1f,2,r3);
  if (7 < uVar1) {
    return 0;
  }

  uVar2 = ((int (*)(void))**(void ***)(uVar1 * 4 + -0x7fc65d0c))();
  return uVar2;
}
/* Address: 0x8022B5C8 | Size: 0x138 | Ghidra import */


u32 fn_8022B5C8(u32 r3)

{
    extern void fn_801F4C14();
    extern void fn_80203EDC();
    extern u32 fn_80203FE4();
    extern void fn_80205B8C();
    extern s8 fn_802062FC();
  u16 uVar3;
  u32 uVar1;
  s8 cVar4;
  u32 uVar2;
  
  fn_80205B8C();
  uVar3 = fn_802040E8(r3);
  uVar1 = fn_80203FE4(r3);
  fn_80203EDC(r3);
  cVar4 = fn_802062FC(r3);
  if (cVar4 != 0) {
    fn_801F4C14(0,0,0x56,0,uVar3);
    if ((uVar1 & 0xffff) < 0x1d) {

      uVar2 = ((int (*)(void))**(void ***)((uVar1 & 0xffff) * 4 + -0x7fc65cec))();
      return uVar2;
    }
  }
  return 1;
}
/* Address: 0x8022BB84 | Size: 0x2a8 | Ghidra import */
u32 fn_8022BB84(void)

{
    int r3;
    int r4;

    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern int fn_8011BEB4();
    extern void fn_801F4C14();
    extern s8 fn_80201704();
    extern int fn_80203EDC();
    extern u32 fn_80203FE4();
    extern s8 fn_802062FC();
    extern s8 fn_802096E8();
    extern void fn_80211B94();
    extern void fn_802249B8();
    extern u8 lbl_80478D7B;
    extern u32 lbl_8047B62C;
  u32 uVar1;
  u16 uVar6;
  u16 uVar7;
  int iVar2;
  int iVar3;
  u32 uVar4;
  s8 cVar10;
  s8 cVar11;
  short sVar8;
  short sVar9;
  u32 uVar5;

  u32 uVar12;
  
  uVar1 = (int)fn_8012640C(r3,0,0xd9,0);
  uVar6 = fn_802040E8(r3);
  uVar7 = fn_80203FE4(r3);
  iVar2 = fn_80203EDC(r3);
  fn_802040E8(r4);
  fn_80203FE4(r4);
  fn_80203EDC(r4);
  uVar12 = 0;
  iVar3 = fn_8011BEB4(uVar1,0,0x2d,0);
  if (iVar3 == 0) {
    uVar12 = 0;
  }
  else {
    uVar4 = fn_80205184(r3);
    cVar10 = fn_802096E8(uVar1);
    cVar11 = fn_8011BEB4(0,uVar4,0x12,0);
    sVar8 = (int)fn_8012640C(r4,0,0x11c,0);
    sVar9 = (int)fn_8012640C(r4,0,0x11e,0);
    iVar3 = (int)fn_8012640C(r4,0,0x11b,0);
    if (uVar7 == 0x3e) {
      if ((((cVar10 == 1) && (iVar3 != 0)) && (iVar3 != 0xffff)) &&
         (((r3 != r4 && (cVar10 = fn_80201704(r3), cVar10 == 0)) &&
          (cVar10 = fn_802062FC(r3), cVar10 == 1)))) {
        fn_801F4C14(0,0,0x56,0,uVar6);
        fn_801F4C14(0,0,0x49,0,r3);
        fn_801F4C14(0,0,0x4b,0,r3);
        iVar2 = -(iVar3 / iVar2);
        if (iVar2 == 0) {
          iVar2 = -1;
        }
        fn_8011BBD8(uVar1,0,0x2d,0,iVar2);
        fn_801254B4(r4,0,0x11b,0,0);
        fn_80211B94(lbl_8047B62C,0x80379b61,0);
        uVar12 = 1;
      }
    }
    else if (((uVar7 < 0x3e) && (uVar7 == 0x1e)) &&
            ((cVar10 == 1 &&
             ((((sVar8 != 0 || (sVar9 != 0)) && (cVar11 == 1)) &&
              ((uVar5 = fn_800E0C54(), (int)((uVar5 & 0xffff) % 100) < iVar2 &&
               (cVar10 = fn_802062FC(r4), cVar10 == 1)))))))) {
      lbl_80478D7B = 8;
      uVar1 = *(u32 *)(lbl_8047B610);
      fn_802249B8(0,0);
      *(u32 *)(lbl_8047B610) = uVar1;
    }
  }
  return uVar12;
}
/* Address: 0x8022D084 | Size: 0x188 | Ghidra import */
int fn_8022D084(u32 r3)

{
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    extern void fn_801FAA58();
    extern void fn_80203EDC();
    extern u32 fn_80203FE4();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
    extern u16 lbl_80279FD0[];
    extern u8 lbl_80379B06[];
  u32 uVar3;
  u32 uVar4;
  int iVar7;
  u32 uVar1;
  u8 cVar5;
  int iVar2;

  int bVar6;

  iVar7 = 0;
  uVar3 = fn_802040E8();
  uVar4 = fn_80203FE4(r3);
  fn_80203EDC(r3);
  uVar1 = fn_801F4354(0,r3);
  cVar5 = fn_802062FC(r3);
  if (cVar5 == 0) {
    return 0;
  }
  fn_801F4C14(0,0,0x56,0,(u16)uVar3);
  switch ((int)(u16)uVar4) {
  case 0x20:
    fn_801FAA58(uVar1,0,0x48,0,2);
    break;
  case 0x17:
    for (bVar6 = 0; (u8)bVar6 < 7; bVar6 = bVar6 + 1) {
      iVar2 = (int)fn_8012640C(r3,0,lbl_80279FD0[(u8)bVar6],0);
      if (iVar2 < 6) {
        fn_801254B4(r3,0,lbl_80279FD0[(u8)bVar6],0,6);
        iVar7 = 5;
      }
    }
    if ((u8)iVar7 != 0) {
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_801F4C14(0,0,0x36,0,r3);
      fn_801F4C14(0,0,0x49,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_80379B06,0);
    }
    break;
  }
  return iVar7;
}
/* Address: 0x8022D20C | Size: 0xc0 | Ghidra import */
void fn_8022D20C(u32 param)

{
    extern void fn_801F4C14();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 lbl_80478D78;
    extern u8 lbl_80379945[];
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B628;
    extern u32 lbl_8047B62C;
  u8 bVar1;
  u32 sVar2;
  u8 cVar3;

  sVar2 = fn_80207BF4(param);
  cVar3 = fn_802062FC(param);
  if (cVar3 != 0) {
    if (((u16)sVar2 == 0x1c) && ((lbl_8047B618 & 0x4000) != 0)) {
      lbl_8047B618 = lbl_8047B618 & 0xffffbfff;
      bVar1 = lbl_8047B628 & 0x3f;
      lbl_8047B628 = bVar1;
      if (bVar1 == 6) {
        lbl_8047B628 = 2;
      }
      *((&lbl_80478D78)+3) = lbl_8047B628;
      fn_801F4C14(0,0,0x4b,0,param);
      lbl_8047B618 = lbl_8047B618 | 0x2000;
      fn_80211B94(lbl_8047B62C,lbl_80379945,0);
    }
  }
  return;
}
/* Address: 0x8022D2CC | Size: 0xc8 | Ghidra import */
void fn_8022D2CC(u32 r3, u32 r4)

{
    extern u32 fn_80207BF4();
    extern void fn_801F4C14();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 lbl_80478D78;
    extern u8 lbl_80379945[];
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B628;
    extern u32 lbl_8047B62C;
  u8 bVar1;
  u32 sVar2;
  u8 cVar3;

  sVar2 = fn_80207BF4(r4);
  cVar3 = fn_802062FC(r4);
  if (cVar3 != 0) {
    if (((u16)sVar2 == 0x1c) && ((lbl_8047B618 & 0x4000) != 0)) {
      lbl_8047B618 = lbl_8047B618 & 0xffffbfff;
      bVar1 = lbl_8047B628 & 0x3f;
      lbl_8047B628 = bVar1;
      if (bVar1 == 6) {
        lbl_8047B628 = 2;
      }
      *((&lbl_80478D78)+3) = (char)lbl_8047B628 + '@';
      fn_801F4C14(0,0,0x4b,0,r4);
      lbl_8047B618 = lbl_8047B618 | 0x2000;
      fn_80211B94(lbl_8047B62C,lbl_80379945,0);
    }
  }
  return;
}
/* Address: 0x8022D394 | Size: 0x328 | Ghidra import */
u32 fn_8022D394(void)

{
    u32 r3;

    extern u32 fn_800FA280();
    extern void fn_80119F50();
    extern void fn_801252E0();
    extern void fn_80132A38();
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_80202998();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
  s8 cVar4;
  u16 uVar3;
  u32 uVar1;
  u32 uVar2;

  u32 uVar5;
  
  do {
    cVar4 = fn_802062FC(r3);
    if (cVar4 == 0) {
      return 1;
    }
    uVar3 = fn_80207BF4(r3);
    uVar1 = fn_80205B8C(r3);
    uVar5 = 0;
    if (uVar3 == 0x11) {
      cVar4 = fn_802026E4(r3,3);
      if ((cVar4 == 1) || (cVar4 = fn_802026E4(r3,4), cVar4 == 1)) {
        fn_80119F50(3);
        uVar2 = fn_800FA280();
        fn_80132A38(0xd,uVar2);
        uVar5 = 1;
      }
    }
    else if (uVar3 < 0x11) {
      if (uVar3 == 0xc) {
        cVar4 = fn_802026E4(r3,10);
        if (cVar4 == 1) {
          fn_80119F50(10);
          uVar2 = fn_800FA280();
          fn_80132A38(0xd,uVar2);
          uVar5 = 3;
        }
      }
      else if (uVar3 < 0xc) {
        if ((uVar3 == 7) && (cVar4 = fn_802026E4(r3,5), cVar4 == 1)) {
          fn_80119F50(5);
          uVar2 = fn_800FA280();
          fn_80132A38(0xd,uVar2);
          uVar5 = 1;
        }
      }
      else if (uVar3 == 0xf) {
LAB_0022a508:
        cVar4 = fn_802026E4(r3,8);
        if (cVar4 == 1) {
          fn_80202810(r3,0x17);
          fn_80119F50(8);
          uVar2 = fn_800FA280();
          fn_80132A38(0xd,uVar2);
          uVar5 = 1;
        }
      }
    }
    else if (uVar3 == 0x29) {
      cVar4 = fn_802026E4(r3,6);
      if (cVar4 == 1) {
        fn_80119F50(6);
        uVar2 = fn_800FA280();
        fn_80132A38(0xd,uVar2);
        uVar5 = 1;
      }
    }
    else if (uVar3 < 0x29) {
      if (uVar3 == 0x14) {
        cVar4 = fn_802026E4(r3,9);
        if (cVar4 == 1) {
          fn_80119F50(9);
          uVar2 = fn_800FA280();
          fn_80132A38(0xd,uVar2);
          uVar5 = 2;
        }
      }
      else if (((0x13 < uVar3) && (0x27 < uVar3)) &&
              (cVar4 = fn_802026E4(r3,7), cVar4 == 1)) {
        fn_80119F50(7);
        uVar2 = fn_800FA280();
        fn_80132A38(0xd,uVar2);
        uVar5 = 1;
      }
    }
    else if (uVar3 == 0x48) goto LAB_0022a508;
    if (uVar5 == 0) {
      return 1;
    }
    if (uVar5 == 2) {
      fn_80202810(r3,9);
    }
    else if (uVar5 < 2) {
      if (uVar5 != 0) {
        fn_801252E0(uVar1);
        fn_80202998(r3,0);
      }
    }
    else if (uVar5 < 4) {
      fn_80202810(r3,10);
    }
    cVar4 = fn_801FECD4(r3);
    if (cVar4 == 1) {
      fn_801FE7EC(r3,0x7c,0,0);
    }
    fn_801F4C14(0,0,0x4b,0,r3);
    fn_80211B94(lbl_8047B62C,0x8037994a,0);
  } while (1);
}
/* Address: 0x8022D6BC | Size: 0x174 | Ghidra import */
u32 fn_8022D6BC(void)

{
    u32 r3;
    u32 r4;

    extern void fn_8011BEB4();
    extern void fn_801F0134();
    extern u32 fn_801F54A4();
    extern void fn_802016A4();
    extern void fn_80205134();
    extern void fn_802096E8();
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  
  fn_8012640C(r4,0,0x11c,0);
  fn_8012640C(r4,0,0x11e,0);
  fn_80207BF4(r3);
  uVar1 = fn_80207BF4(r4);
  uVar2 = fn_80205184(r3);
  fn_8011BEB4(0,uVar2,7,0);
  fn_80205134(r3);
  fn_8011BEB4(0,uVar2,0xd,0);
  fn_8012640C(r3,0,0xd9,0);
  fn_802096E8();
  fn_8012640C(r3,0,0x107,0);
  fn_802016A4(r3);
  fn_802016A4(r4);
  uVar3 = fn_801F54A4(0,0,0x14,0);
  fn_801F0134(r4,uVar3);
  uVar1 = (uVar1 & 0xffff) - 9;
  if (uVar1 < 0x30) {

    uVar2 = ((int (*)(void))**(void ***)(uVar1 * 4 + -0x7fc65b88))();
    return uVar2;
  }
  return 0;
}
/* Address: 0x8022DCB8 | Size: 0x250 | Ghidra import */
int fn_8022DCB8(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern s8 fn_80201704();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern u32 fn_80203B5C();
    extern short fn_80205134();
    extern s8 fn_802062FC();
    extern u8 lbl_80478D7D;
  u32 uVar1;
  u16 uVar4;
  short sVar5;
  short sVar6;
  s8 cVar7;
  u32 uVar2;
  s8 cVar8;
  u32 uVar3;

  int iVar9;
  
  uVar4 = fn_80207BF4(r4);
  sVar5 = fn_8011BEB4(0,r5,7,0);
  sVar6 = fn_80205134(r3);
  cVar7 = (int)fn_8012640C(r3,0,0x111,0);
  iVar9 = 0;
  uVar2 = (int)fn_8012640C(r3,0,0xd9,0);
  cVar8 = fn_802062FC(r4);
  if (cVar8 == 0) {
    iVar9 = 0;
  }
  else if (((r5 & 0xffff) != 0) && ((r5 & 0xffff) != 0x165)) {
    if (uVar4 == 0xb) {
      if ((sVar6 == 0xb) && (sVar5 != 0)) {
        uVar1 = 0x80379714;
        if (cVar7 != 0) {
          uVar1 = 0x80379715;
        }
        *(u32 *)(lbl_8047B610) = uVar1;
        iVar9 = 1;
      }
    }
    else if (uVar4 < 0xb) {
      if (((9 < uVar4) && (sVar6 == 0xd)) && (sVar5 != 0)) {
        uVar1 = 0x80379714;
        if (cVar7 != 0) {
          uVar1 = 0x80379715;
        }
        *(u32 *)(lbl_8047B610) = uVar1;
        iVar9 = 1;
      }
    }
    else if (((uVar4 == 0x12) && (sVar6 == 10)) && (cVar8 = fn_802026E4(r4,7), cVar8 == 0))
    {
      cVar8 = fn_802025B8(r4,0x3a);
      if (cVar8 == 2) {
        fn_8020248C(r4,0x3a,0);
        lbl_80478D7D = 0;
      }
      else {
        lbl_80478D7D = 1;
      }
      uVar1 = 0x80379783;
      if (cVar7 != 0) {
        uVar1 = 0x80379784;
      }
      *(u32 *)(lbl_8047B610) = uVar1;
      iVar9 = 2;
    }
    if (iVar9 == 1) {
      cVar8 = fn_80201704(r4);
      if (cVar8 == 1) {
        if (cVar7 == 0) {
          *(u32 *)(lbl_8047B610) = 0x80379752;
        }
        else {
          *(u32 *)(lbl_8047B610) = 0x80379753;
        }
      }
      else {
        uVar3 = fn_80203B5C(r4,4);
        fn_8011BBD8(uVar2,0,0x2d,0,-(uVar3 & 0xffff));
      }
    }
  }
  return iVar9;
}
/* Address: 0x8022DF08 | Size: 0x2bc | Ghidra import */
void fn_8022DF08(void)

{
    int r3;

    extern u32 DAT_8038fffc;
    extern u32 DAT_8038fffd;
    extern u32 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern void fn_80120DD0();
    extern void fn_801252E0();
    extern void fn_80132A38();
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern s8 fn_80201704();
    extern void fn_80202810();
    extern void fn_80202998();
    extern u32 fn_80203B5C();
    extern s8 fn_80203CCC();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
  u32 uVar1;
  s8 cVar6;
  s8 cVar7;
  u16 uVar4;
  u32 uVar2;
  u32 uVar3;
  short sVar5;

  uVar1 = fn_80205B8C();
  cVar6 = (int)fn_801F453C(0,1);
  if ((r3 != 0) && (cVar7 = fn_802062FC(r3), cVar7 != 0)) {
    uVar4 = fn_80207BF4(r3);
    fn_801F4C14(0,0,0x36,0,r3);
    uVar2 = (int)fn_8012640C(r3,0,0xd9,0);
    if (uVar4 == 0x36) {
      uVar3 = (int)fn_8012640C(r3,0,0xf9,0);
      uVar3 = __cntlzw(uVar3 & 0xff);
      fn_801254B4(r3,0,0xf9,0,uVar3 >> 5 & 0xff);
    }
    else if (uVar4 < 0x36) {
      if (uVar4 == 0x2c) {
        if ((cVar6 == 2) && (cVar6 = fn_80201704(r3), cVar6 == 0)) {
          uVar3 = fn_80203B5C(r3,0x10);
          fn_8011BBD8(uVar2,0,0x2d,0,-(uVar3 & 0xffff));
          fn_80211B94(lbl_8047B62C,0x8037967e,0);
        }
      }
      else if (((uVar4 < 0x2c) && (uVar4 == 3)) &&
              ((cVar6 = (int)fn_8012640C(r3,0,0xea,0), cVar6 < '\f' &&
               (sVar5 = (int)fn_8012640C(r3,0,0xed,0), sVar5 != 2)))) {
        fn_801F4C14(0,0,0x4b,0,r3);
        fn_801254B4(r3,0,0xea,0,(int)(char)(cVar6 + 1));
        DAT_8038fffc = 0x11;
        DAT_8038fffd = 0;
        fn_80211B94(lbl_8047B62C,0x803796b5,0);
      }
    }
    else if (((uVar4 == 0x3d) && (cVar6 = fn_80203CCC(r3), cVar6 == 0)) &&
            (uVar3 = fn_800E0C54(), (uVar3 & 0xffff) == ((uVar3 & 0xffff) / 3) * 3)) {
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80120DD0(uVar1);
      uVar2 = fn_800FA280();
      fn_80132A38(0xd,uVar2);
      fn_801252E0(uVar1);
      fn_80202810(r3,0x17);
      fn_80202998(r3,0);
      cVar6 = fn_801FECD4(r3);
      if (cVar6 == 1) {
        fn_801FE7EC(r3,0x7c,0,0);
      }
      fn_80211B94(lbl_8047B62C,0x8037969f,0);
    }
  }
  return;
}
/* Address: 0x8022E1C4 | Size: 0x34 | Ghidra import */
void fn_8022E1C4(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8022E1F8();
  fn_801F37B0(0,fn_8022E1F8,0,0);
  return;
}
/* Address: 0x8022E1F8 | Size: 0x11c | Ghidra import */
u32 fn_8022E1F8(int r3)

{
    extern u32 fn_800FA280();
    extern void fn_8011CB54();
    extern void fn_8011CB6C();
    extern void fn_80132A38();
    extern u32 fn_801F2598();
    extern void fn_801F4C14();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_802037DC();
    extern s8 fn_802062FC();
    extern void fn_80207BC0();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
    extern u8 lbl_80379667[];
  u32 local;
  u32 sVar3;
  u8 cVar4;
  u32 uVar1;
  u32 uVar2;

  local = r3;
  sVar3 = fn_80207BF4();
  cVar4 = fn_802062FC(local);
  if (cVar4 == 0) {
    return 1;
  }
  if (((sVar3 & 0xffff) == 0x24) && (cVar4 = fn_802026E4(local,0x3c), cVar4 == 1)) {
    uVar1 = fn_801F2598(0,1,2,local);
    cVar4 = fn_802062FC();
    if (cVar4 == 1) {
      fn_80202810(local,0x3c);
      uVar2 = fn_80207BF4(uVar1);
      fn_80207BC0(local,uVar2);
      fn_801F4C14(0,0,0x4b,0,local);
      uVar1 = fn_802037DC(uVar1);
      fn_80132A38(0xd,uVar1);
      fn_8011CB6C(uVar2);
      fn_8011CB54();
      uVar1 = fn_800FA280();
      fn_80132A38(0xe,uVar1);
      fn_80211B94(lbl_8047B62C,(u32)lbl_80379667,0);
    }
  }
  return 1;
}
/* Address: 0x8022E314 | Size: 0x38 | Ghidra import */
void fn_8022E314(u8 r3)

{
    extern void fn_801F37B0();
    extern void fn_8022E34C();
  u8 local_8 [8];

  local_8[0] = r3;
  fn_801F37B0(0,(u32)fn_8022E34C,local_8,0);
  return;
}
/* Address: 0x8022E34C | Size: 0xc4 | Ghidra import */
u32 fn_8022E34C(void)

{
    u32 r3;
    u32 r4;
    char *r5;

    extern void fn_801F4C14();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
  short sVar1;
  s8 cVar2;

  u32 uVar3;
  
  sVar1 = fn_80207BF4();
  cVar2 = fn_802062FC(r3);
  if (cVar2 != 0) {
    uVar3 = 0x803795fe;
    if (*r5 == 1) {
      uVar3 = 0x803795f5;
    }
    if ((sVar1 == 0x16) && (cVar2 = fn_802026E4(r3,0x3b), cVar2 == 1)) {
      fn_80202810(r3,0x3b);
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,uVar3,0);
    }
  }
  return 1;
}
/* Address: 0x8022E410 | Size: 0x2e0 | Ghidra import */
void fn_8022E410(int r3)

{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F37B0();
    extern void fn_801F4C14();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 fn_8022EC40();
    extern u32 lbl_8047B62C;
    extern u8 lbl_80379F58[];
    extern u8 lbl_803795BB[];
    extern u8 lbl_8037959E[];
    extern u8 lbl_803795D8[];
    extern u8 lbl_803796D7[];
    extern void fn_8022EB9C();
  u8 cVar3;
  int uVar2;

  u8 local_18 [12];

  if ((r3 != 0) && (cVar3 = fn_802062FC(r3), cVar3 != 0)) {
    uVar2 = (int)(u16)fn_80207BF4(r3);
    switch (uVar2) {
    case 0x2:
      cVar3 = fn_801F2988(0,0x50);
      if (cVar3 != 2) {
        return;
      }
      fn_801F2934(0,0x50,0);
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_8037959E,0);
      return;
    case 0x2d:
      cVar3 = fn_801F2988(0,0x51);
      if (cVar3 == 2) {
        fn_801F2934(0,0x51,0);
        fn_801F4C14(0,0,0x4b,0,r3);
        fn_80211B94(lbl_8047B62C,(u32)lbl_803795BB,0);
      }
      break;
    case 0x46:
      cVar3 = fn_801F2988(0,0x4f);
      if (cVar3 != 2) {
        return;
      }
      fn_801F2934(0,0x4f,0);
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_803795D8,0);
      return;
    case 0x16:
      if ((int)fn_8012640C(r3,0,0x116,0) != 0) {
        return;
      }
      cVar3 = fn_802025B8(r3,0x3b);
      if (cVar3 == 2) {
        fn_8020248C(r3,0x3b,0);
      }
      fn_801254B4(r3,0,0x116,0,1);
      return;
    case 0x24:
      if ((int)fn_8012640C(r3,0,0x117,0) != 0) {
        return;
      }
      cVar3 = fn_802025B8(r3,0x3c);
      if (cVar3 == 2) {
        fn_8020248C(r3,0x3c,0);
      }
      fn_801254B4(r3,0,0x117,0,1);
      return;
    case 0x3b:
      cVar3 = fn_8022EC40(r3);
      if (cVar3 == 0) {
        return;
      }
      lbl_80379F58[0x1609b] = cVar3 - 1;
      fn_801F4C14(0,0,0x4b,0,r3);
      fn_80211B94(lbl_8047B62C,(u32)lbl_803796D7,0);
      return;
    case 0xd:
    case 0x4d:
      local_18[0] = 0;
      fn_801F37B0(0,(u32)fn_8022EB9C,local_18,0);
      break;
    default:
      return;
    }
  }
  return;
}
/* Address: 0x8022E6F0 | Size: 0x4ac | Ghidra import */
void fn_8022E6F0(u32 r3, u8 r4)

{
    extern void _threadSwitch();
    extern void fn_8011F910();
    extern u8 fn_8011FC74();
    extern int fn_801906A0();
    extern void fn_801C3430();
    extern void battleGridReplaceTrainer();
    extern void fn_801DA4E8();
    extern void fn_801DA8C4();
    extern u8 fn_801DA94C();
    extern void fn_801DA9E8();
    extern void fn_801DB100();
    extern void fn_801DDD28();
    extern u32 fn_801DE418();
    extern void fn_801EF7C4();
    extern void fn_801F000C();
    extern u32 fn_801F2A7C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u8 fn_801F7B70();
    extern int fn_801FB1C0();
    extern void fn_801FBA24();
    extern void fn_801FE710();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u8 fn_80204A10();
    extern u32 fn_80205B8C();
    extern u8 fn_802062FC();
    extern void fn_80208404();
    extern void fn_80208554();
    extern void fn_802085C4();
    extern void fn_802086E8();
    extern void fn_80208750();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_802653FC();
  u8 cVar8;
  u32 uVar1;
  u16 uVar6;
  u8 cVar9;

  fn_801F54A4(0,0,0x14,0);
  if (r3 != 0) {
    cVar8 = fn_802062FC(r3);
    if (cVar8 != 0) {
      fn_80205B8C(r3);
      cVar8 = fn_8011FC74();
      if (cVar8 != 0) {
        u32 uVar4;
        u32 uVar5;
        uVar1 = (int)fn_8012640C(r3,0,0xd6,0);
        cVar8 = fn_80204A10(r3);
        if (cVar8 == 1) {
          cVar8 = (int)fn_8012640C(uVar1,0,0xd1,0);
          if (cVar8 == 0) {
            u32 uVar2;
            uVar2 = fn_80205B8C(r3);
            fn_8011F910(uVar2,0,0);
            cVar8 = fn_801FECD4(r3);
            if (cVar8 == 1) {
              fn_801FE7EC(r3,0xc5,0,0);
            }
            uVar6 = fn_801F54A4(0,0,0x14,0);
            fn_802653FC(r3,uVar6,1);
            fn_801254B4(uVar1,0,0xd1,0,1);
            cVar8 = fn_801FECD4(r3);
            if (cVar8 == 1) {
              fn_801FE710(r3,0xd1,0);
            }
          }
        }
        else if (r4 == 0) {
          u32 uVar3;
          u32 uVar2;
          uVar2 = fn_801F2A7C(0);
          uVar3 = fn_801FB1C0(uVar2,0,0x4c,0);
          cVar8 = fn_801F7B70(uVar2);
          cVar9 = (int)fn_8012640C(uVar1,0,0xd1,0);
          if (((cVar9 == 0) && (cVar9 = fn_801F54A4(0,0,0x2b,0), cVar9 == 1)) &&
             (uVar3 != 0)) {
            u16 sVar7;
            u32 uVar10;
            sVar7 = fn_801F54A4(0,0,0xd,0);
            if (sVar7 != 0x11) {
              uVar10 = 0;
            }
            else {
              uVar10 = 3;
              fn_80208750(r3,1,1,0);
              fn_801FBA24(uVar2,0);
            }
            uVar4 = fn_801F54A4(0,0,0x36,0);
            fn_801F4C14(0,0,0x36,0,r3);
            uVar5 = fn_801DE418(0x32);
            fn_801DDD28(uVar5,0xa0,4,0);
            fn_80208404(r3,0,uVar10,0);
            battleGridReplaceTrainer(uVar3,uVar5);
            fn_801C3430();
            fn_801DA9E8(uVar5,0xa0,4);
            fn_801EF7C4(0);
            fn_801DA4E8(uVar5,1);
            while (1) {
              cVar9 = fn_801DA94C(uVar5,0xa0,4);
              if (cVar9 == 0) break;
              _threadSwitch();
            }
            battleGridReplaceTrainer(uVar5,uVar3);
            fn_801C3430();
            fn_801EF7C4(1);
            fn_801DA4E8(uVar5,0);
            fn_80208404(r3,0,uVar10,1);
            if (sVar7 == 0x11) {
              fn_802624CC(0x7729);
              fn_8026246C();
              fn_80208404(r3,0,uVar10,4);
              fn_802085C4(r3,1,1,0, (void*)0xffffffff);
              fn_80208554(r3,1,1,6);
              fn_801FBA24(uVar2,1);
              fn_801FBA24(uVar2,2);
              fn_80208404(r3,0,uVar10,1);
              fn_802624CC(0x772a);
              if (cVar8 == 0) {
                fn_8026246C();
                fn_801F000C(0x40);
                fn_802624CC(0x772b);
              }
              fn_8026246C();
              fn_80208404(r3,0,uVar10,4);
            }
            else {
              uVar3 = fn_801906A0(0x9a0);
              if (uVar3 == 0) {
                fn_802624CC(0x7717);
              }
              else {
                fn_802624CC(0x770a);
              }
              fn_80208404(r3,0,uVar10,2);
              fn_8026246C();
            }
            fn_801254B4(uVar1,0,0xd1,0,1);
            cVar8 = fn_801FECD4(r3);
            if (cVar8 == 1) {
              fn_801FE710(r3,0xd1,0);
            }
            fn_801F4C14(0,0,0x36,0,uVar4);
            fn_80208404(r3,0,uVar10,3);
            if (sVar7 == 0x11) {
              fn_802086E8(r3,1,1);
              fn_801FBA24(uVar2,3);
            }
            fn_801DA8C4(uVar5,0xa0,4);
            fn_801DB100(uVar5);
          }
        }
      }
    }
  }
  return;
}
/* Address: 0x8022EB9C | Size: 0xa4 | Ghidra import */
u32 fn_8022EB9C(void)

{
    u32 r3;
    u32 r4;
    u8 *r5;

    extern u32 DAT_8038fff3;
    extern void fn_801F4C14();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern int fn_8022EC40();
    extern u32 lbl_8047B62C;
  s8 cVar1;

  cVar1 = fn_802062FC();
  if ((cVar1 != 0) && (cVar1 = fn_8022EC40(r3), cVar1 != 0)) {
    DAT_8038fff3 = cVar1 + -1;
    fn_801F4C14(0,0,0x4b,0,r3);
    fn_80211B94(lbl_8047B62C,0x803796d7,0);
    if (r5 != (void *)0) {
      *r5 = 1;
    }
  }
  return 1;
}
/* Address: 0x8022EC40 | Size: 0x1ac | Ghidra import */
u32 fn_8022EC40(u32 r3)

{
    extern int fn_80203D3C();
    extern s8 fn_802062FC();
    extern s8 fn_80207AE0();
    extern void fn_80207B5C();
  u8 cVar4;
  u32 uVar1;
  u32 sVar2;
  u32 sVar3;
  s8 cVar5;
  u32 uVar6;

  if ((u8)fn_802062FC() == 0) {
    return 0;
  }
  sVar2 = fn_80203D3C(r3);
  sVar3 = fn_80207BF4(r3);
  if (((u16)sVar2 != 0x181) || ((u16)sVar3 != 0x3b)) {
    return 0;
  }
  cVar4 = (u8)fn_801F453C(0,1);
  uVar1 = 0;
  if (((cVar4 == 0) || (cVar4 == 3)) && (cVar5 = fn_80207AE0(r3,0), (u8)cVar5 == 0)
     ) {
    for (uVar6 = 0; (uVar6 & 0xff) < 2; uVar6 = uVar6 + 1) {
      fn_80207B5C(r3,uVar6,0);
    }
    uVar1 = 1;
  }
  if ((cVar4 == 1) && (cVar5 = fn_80207AE0(r3,10), (u8)cVar5 == 0)) {
    for (uVar6 = 0; (uVar6 & 0xff) < 2; uVar6 = uVar6 + 1) {
      fn_80207B5C(r3,uVar6,10);
    }
    uVar1 = 2;
  }
  if ((cVar4 == 2) && (cVar5 = fn_80207AE0(r3,0xb), (u8)cVar5 == 0)) {
    for (uVar6 = 0; (uVar6 & 0xff) < 2; uVar6 = uVar6 + 1) {
      fn_80207B5C(r3,uVar6,0xb);
    }
    uVar1 = 3;
  }
  if ((cVar4 == 4) && (cVar4 = fn_80207AE0(r3,0xf), cVar4 == 0)) {
    for (uVar6 = 0; (uVar6 & 0xff) < 2; uVar6 = uVar6 + 1) {
      fn_80207B5C(r3,uVar6,0xf);
    }
    uVar1 = 4;
  }
  return uVar1;
}
/* Address: 0x8022EDEC | Size: 0x50c | Ghidra import */
u32 fn_8022EDEC(void)

{
    int r3;
    char r4;

    extern u32 fn_800E0C54();
    extern u32 fn_8011F6D8();
    extern s8 fn_8011FC74();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_80204A10();
    extern void fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_802080A8();
    extern void fn_80208404();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_8026532C();
    extern void fn_802653FC();
    extern void fn_80265598();
  u16 uVar4;
  u32 uVar1;
  s8 cVar6;
  u32 uVar2;
  short sVar5;
  u32 uVar3;
  
  uVar4 = fn_801F54A4(0,0,0x14,0);
  if (r3 == 0) {
    uVar1 = 0;
  }
  else {
    cVar6 = fn_802062FC(r3);
    if (cVar6 == 0) {
      uVar1 = 0;
    }
    else {
      cVar6 = fn_80204A10(r3);
      if (cVar6 == 0) {
        uVar1 = 0;
      }
      else {
        cVar6 = fn_801F54A4(0,0,0x31,0);
        if (cVar6 == 0) {
          uVar1 = 0;
        }
        else {
          fn_80205B8C(r3);
          cVar6 = fn_8011FC74();
          if (cVar6 == 0) {
            uVar1 = 0;
          }
          else {
            fn_8012640C(r3,0,0xee,0);
            if (r4 == 0) {
              cVar6 = fn_802026E4(r3,0x3e);
              if (cVar6 == 1) {
                return 0;
              }
              cVar6 = fn_802026E4(r3,8);
              if (cVar6 == 1) {
                return 0;
              }
              fn_80205B8C(r3);
              uVar2 = fn_8011F6D8();
              sVar5 = fn_80205184(r3);
              uVar3 = fn_800E0C54();
              if (((uVar2 & 0xff) <= (uVar3 & 0xffff) % 100) || (sVar5 != 0x164)) {
                return 0;
              }
              fn_80208404(r3,1,1,0);
              uVar1 = fn_801F54A4(0,0,0x36,0);
              fn_801F4C14(0,0,0x36,0,r3);
              cVar6 = fn_802025B8(r3,0x3e);
              if (cVar6 == 2) {
                fn_8020248C(r3,0x3e,0);
              }
              cVar6 = fn_801FECD4(r3);
              if (cVar6 == 1) {
                fn_801FE7EC(r3,200,0,0);
              }
              fn_80208404(r3,1,1,1);
              fn_802653FC(r3,uVar4,1);
              fn_802624CC(0x771b);
              fn_80208404(r3,1,1,2);
              fn_8026246C();
              fn_8026532C(r3,uVar4,0);
              fn_801F4C14(0,0,0x36,0,uVar1);
              fn_80208404(r3,1,1,3);
            }
            else if (r4 == 1) {
              cVar6 = fn_802026E4(r3,0x3e);
              if (cVar6 == 0) {
                return 0;
              }
              fn_80208404(r3,0,2,0);
              uVar1 = fn_801F54A4(0,0,0x36,0);
              fn_801F4C14(0,0,0x36,0,r3);
              fn_80208404(r3,0,2,1);
              fn_80265598(r3,uVar4,1);
              fn_802624CC(0x771c);
              fn_80208404(r3,0,2,2);
              fn_8026246C();
              fn_8026532C(r3,uVar4,0);
              fn_801F4C14(0,0,0x36,0,uVar1);
              fn_80208404(r3,0,2,3);
            }
            else if (r4 == 2) {
              cVar6 = fn_802026E4(r3,0x3e);
              if (cVar6 == 0) {
                return 0;
              }
              cVar6 = fn_800E0C54();
              if (cVar6 != 0) {
                return 0;
              }
              fn_802026E4(r3,8);
              fn_802080A8(r3,1,1,0,0);
              uVar1 = fn_801F54A4(0,0,0x36,0);
              fn_801F4C14(0,0,0x36,0,r3);
              fn_80202810(r3,0x3e);
              fn_80202810(r3,8);
              fn_80202810(r3,0x17);
              cVar6 = fn_801FECD4(r3);
              if (cVar6 == 1) {
                fn_801FE7EC(r3,200,0,0);
              }
              cVar6 = fn_801FECD4(r3);
              if (cVar6 == 1) {
                fn_801FE7EC(r3,0x7c,0,0);
              }
              fn_802080A8(r3,1,1,0x771e,1);
              fn_802080A8(r3,1,1,0,2);
              fn_8026246C();
              fn_8026532C(r3,uVar4,0);
              fn_801F4C14(0,0,0x36,0,uVar1);
              fn_802080A8(r3,1,1,0,3);
            }
            uVar1 = 1;
          }
        }
      }
    }
  }
  return uVar1;
}
/* Address: 0x8022F2F8 | Size: 0x13c | Ghidra import */


u32 fn_8022F2F8(void)

{
    extern void fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F54A4();
    extern u32 lbl_8047B644;
  u32 uVar1;

  uVar1 = fn_801F025C(0x11,0);
  fn_8012640C(uVar1,0,0xd9,0);
  fn_801F025C(2,uVar1);
  fn_80207BF4(uVar1);
  uVar1 = fn_80205184(uVar1);
  fn_8011BEB4(0,uVar1,7,0);
  fn_8011BEB4(0,uVar1,9,0);
  fn_801F54A4(0,0,0x14,0);
  do {
    if (lbl_8047B644 < 0x10) {

      uVar1 = ((int (*)(void))**(void ***)(lbl_8047B644 * 4 + -0x7fc65ac8))();
      return uVar1;
    }
  } while ((int)lbl_8047B644 != 0xf);
  return 0;
}
/* Address: 0x8022FE20 | Size: 0x60 | Ghidra import */
u32 fn_8022FE20(u32 r3)

{
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u16 fn_80205224();
  u8 cVar2;
  u16 sVar1;

  cVar2 = fn_802026E4(r3,0x13);
  if ((cVar2 == 1) && (sVar1 = fn_80205224(r3), sVar1 != 99)) {
    fn_80202810(r3,0x13);
  }
  return 1;
}
/* Address: 0x8022FE80 | Size: 0x110 (272 bytes) */
#pragma optimization_level 0
void fn_8022FE80(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A00C[];
    extern u8 lbl_80378798[];
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B62C;
    extern void fn_801F37B0();
    extern void fn_8022E1F8();
    extern void fn_8022E34C();
    extern void fn_8022EB9C();
    extern void fn_80230088();
    extern void fn_8023011C();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r12 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;
    void (*ctr_fn)(void) = 0;

    param1 = (u32)fn_8023011C;
ctx = (void*)(u32)0x0;
    param1 = (u32)fn_8023011C;
    param2 = 0x0;
    param3 = 0x0;
    fn_801F37B0();
    var_r28 = lbl_8047B62C;
    param1 = (u32)lbl_80378798;
    var_r29 = lbl_8047B614;
    tmp = 0x0;
ctx = (void*)(u32)lbl_8027A00C;
    var_r30 = *(u32*)&lbl_8047B610;
    param1 = (u32)lbl_80378798;
    lbl_8047B614 = tmp;
    var_r31 = (u32)lbl_8027A00C;
    *(u32*)&lbl_8047B610 = param1;
    lbl_8047B62C = var_r28;
    do {
    ctx = (void*)*(u32*)&lbl_8047B610;
        tmp = *(u8*)((u8*)ctx + 0x0);
        tmp = tmp << 2;
        r12 = *(u32*)(var_r31 + tmp);
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
        tmp = lbl_8047B614;

    } while (tmp != 1 && tmp != 2);

    tmp = 0x1;
ctx = (void*)(u32)fn_8022E34C;
    lbl_8047B62C = var_r28;
    param1 = (u32)fn_8022E34C;
    param2 = (u32)sp + 0x9;
ctx = (void*)(u32)0x0;
    lbl_8047B614 = var_r29;
    param3 = 0x0;
    *(u32*)&lbl_8047B610 = var_r30;
    *(u8*)(sp + 0x9) = tmp;
    fn_801F37B0();
    param1 = (u32)fn_8022E1F8;
ctx = (void*)(u32)0x0;
    param1 = (u32)fn_8022E1F8;
    param2 = 0x0;
    param3 = 0x0;
    fn_801F37B0();
    param1 = (u32)fn_80230088;
ctx = (void*)(u32)0x0;
    param1 = (u32)fn_80230088;
    param2 = 0x0;
    param3 = 0x0;
    fn_801F37B0();
    tmp = 0x0;
ctx = (void*)(u32)fn_8022EB9C;
    *(u8*)(sp + 0x8) = tmp;
    param1 = (u32)fn_8022EB9C;
    param2 = (u32)sp + 0x8;
ctx = (void*)(u32)0x0;
    param3 = 0x0;
    fn_801F37B0();
    return;
}
#pragma optimization_level 4

/* Address: 0x8022FF90 | Size: 0xF8 (248 bytes) */
#pragma optimization_level 0
void fn_8022FF90(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A00C[];
    extern u8 lbl_80378798[];
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B62C;
    extern void fn_801F37B0();
    extern void fn_8022E1F8();
    extern void fn_8022E34C();
    extern void fn_8022EB9C();
    extern void fn_80230088();
    u8 sp[0x20];
    u32 tmp = 0;
    u32 r12 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;
    void (*ctr_fn)(void) = 0;

    param1 = (u32)lbl_80378798;
ctx = (void*)(u32)lbl_8027A00C;
    tmp = 0x0;
    param1 = (u32)lbl_80378798;
    var_r31 = (u32)lbl_8027A00C;
    var_r28 = lbl_8047B62C;
    var_r29 = lbl_8047B614;
    var_r30 = *(u32*)&lbl_8047B610;
    lbl_8047B614 = tmp;
    *(u32*)&lbl_8047B610 = param1;
    lbl_8047B62C = var_r28;
    do {
    ctx = (void*)*(u32*)&lbl_8047B610;
        tmp = *(u8*)((u8*)ctx + 0x0);
        tmp = tmp << 2;
        r12 = *(u32*)(var_r31 + tmp);
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
        tmp = lbl_8047B614;

    } while (tmp != 1 && tmp != 2);

    tmp = 0x1;
ctx = (void*)(u32)fn_8022E34C;
    lbl_8047B62C = var_r28;
    param1 = (u32)fn_8022E34C;
    param2 = (u32)sp + 0x9;
ctx = (void*)(u32)0x0;
    lbl_8047B614 = var_r29;
    param3 = 0x0;
    *(u32*)&lbl_8047B610 = var_r30;
    *(u8*)(sp + 0x9) = tmp;
    fn_801F37B0();
    param1 = (u32)fn_8022E1F8;
ctx = (void*)(u32)0x0;
    param1 = (u32)fn_8022E1F8;
    param2 = 0x0;
    param3 = 0x0;
    fn_801F37B0();
    param1 = (u32)fn_80230088;
ctx = (void*)(u32)0x0;
    param1 = (u32)fn_80230088;
    param2 = 0x0;
    param3 = 0x0;
    fn_801F37B0();
    tmp = 0x0;
ctx = (void*)(u32)fn_8022EB9C;
    *(u8*)(sp + 0x8) = tmp;
    param1 = (u32)fn_8022EB9C;
    param2 = (u32)sp + 0x8;
ctx = (void*)(u32)0x0;
    param3 = 0x0;
    fn_801F37B0();
    return;
}
#pragma optimization_level 4

/* -------------------------------------------------------------------
 * Tournament Flow (0x80230000-0x80236000)
 * 27 functions
 * ------------------------------------------------------------------- */

/* Address: 0x80230088 | Size: 0x94 | Ghidra import */
u32 fn_80230088(int r3)

{
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern s8 fn_802062FC();
    extern u32 fn_8022BE2C();
  u32 local;
  u32 uVar1;
  u8 cVar2;

  local = r3;
  cVar2 = fn_802062FC();
  if (cVar2 == 0) {
    return 1;
  }
  uVar1 = fn_801F54A4(0,0,0x36,0);
  fn_801F4C14(0,0,0x36,0,local);
  fn_8022BE2C(local,1);
  fn_801F4C14(0,0,0x36,0,uVar1);
  return 1;
}
/* Address: 0x8023011C | Size: 0x8c | Ghidra import */
u32 fn_8023011C(void* r3)

{
    extern void fn_801F4C14();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
    extern u8 lbl_80378721[];
  u8 cVar1;

  cVar1 = (int)fn_8012640C(r3,0,0x120,0);
  if (cVar1 == 1) {
    fn_801F4C14(0,0,0x46,0,r3);
    fn_801F4C14(0,0,0x42,0,r3);
    fn_80211B94(lbl_8047B62C,lbl_80378721,0);
  }
  return 1;
}
/* Address: 0x802301A8 | Size: 0x170 | Ghidra import */
u32 fn_802301A8(void)

{
    u32 r3;

    extern void fn_8011BBD8();
    extern void fn_80132A38();
    extern void fn_801F4C14();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern void fn_8022FE80();
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  s8 cVar4;
  s8 cVar5;
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;

  cVar4 = fn_802062FC();
  if (cVar4 != 0) {
    lbl_8047B618 = lbl_8047B618 | 0x1000020;
    cVar4 = fn_802026E4(r3,0x1e);
    if (cVar4 == 1) {
      cVar4 = fn_80202234(r3,0x1e);
      cVar5 = fn_80202108(r3,0x1e);
      fn_801F4C14(0,0,0x36,0,r3);
      uVar1 = (int)fn_8012640C(r3,0,0xd9,0);
      fn_80132A38(0x2f,(int)cVar4 - (int)cVar5);
      if ((int)cVar5 < (int)cVar4) {
        fn_80201FDC(r3,0x1e,(int)(char)(cVar5 + 1));
        uVar1 = 0x80378e46;
      }
      else {
        uVar2 = fn_80205B8C(r3);
        uVar3 = (int)fn_8012640C(uVar2,0,0x83,0);
        fn_8011BBD8(uVar1,0,0x2d,0,uVar3);
        fn_80202810(r3,0x1e);
        uVar1 = 0x80378e1e;
      }
      fn_80211B94(lbl_8047B62C,uVar1,0);
      fn_8022FE80();
    }
    lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  }
  return 1;
}
/* Address: 0x80230318 | Size: 0x250 | Ghidra import */
u32 fn_80230318(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F00D0();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_80201890();
    extern u32 fn_80201C58();
    extern u32 fn_80201D84();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_802062FC();
    extern void fn_802099AC();
    extern void fn_8020A2B8();
    extern void fn_80211B94();
    extern void fn_8022FE80();
    extern u8 lbl_80478D7D;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  s8 cVar5;
  s8 cVar6;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;

  u8 auStack_c8 [180];
  
  cVar5 = fn_802062FC();
  if (cVar5 != 0) {
    lbl_8047B618 = lbl_8047B618 | 0x1000020;
    cVar5 = fn_802026E4(r3,0x34);
    if (cVar5 == 1) {
      cVar5 = fn_80202234(r3,0x34);
      cVar6 = fn_80202108(r3,0x34);
      if (cVar6 < cVar5) {
        fn_80201FDC(r3,0x34,(int)(char)(cVar6 + 1));
      }
      else {
        uVar1 = fn_80201890(r3,0x34);
        uVar2 = fn_80201C58(r3,0x34);
        uVar3 = fn_80201D84(r3,0x34);
        uVar3 = fn_801F00D0(uVar3,r4);
        cVar5 = fn_802062FC();
        if (cVar5 == 0) {
          uVar3 = fn_801F025C(0xe,uVar3);
        }
        if ((uVar2 & 0xffff) == 0xf8) {
          lbl_80478D7D = 0;
        }
        else {
          lbl_80478D7D = 1;
        }
        fn_8011BEB4(0,uVar2,1,0);
        uVar4 = fn_800FA280();
        fn_80132A38(0xd,uVar4);
        fn_801F4C14(0,0,0x42,0,r3);
        uVar4 = fn_801F0134(r3,r4);
        fn_801F4C14(0,0,0x36,0,uVar3);
        uVar3 = (int)fn_8012640C(uVar3,0,0xd9,0);
        fn_8020A2B8(auStack_c8,uVar3);
        fn_802099AC(uVar3,0xffffffff,uVar2,uVar4,0);
        fn_8011BBD8(uVar3,0,0x2d,0,uVar1);
        fn_801254B4(r3,0,0x11b,0,0xffff);
        fn_80202810(r3,0x34);
        fn_80211B94(lbl_8047B62C,0x80378f39,0);
        fn_8020A2B8(uVar3,auStack_c8);
        fn_8022FE80();
      }
    }
    lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  }
  return 1;
}
/* Address: 0x802316FC | Size: 0xe8 | Ghidra import */
u32 fn_802316FC(u32 r3)

{
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 lbl_80478D78;
    extern u32 lbl_8047B62C;
    extern u8 lbl_80379249[];
    extern int fn_80207BF4();
  u32 self;
  u32 sVar1;
  u8 cVar2;

  self = r3;
  sVar1 = fn_80207BF4();
  if ((cVar2 = fn_802062FC(self)) == 0) {
    return 1;
  }
  if ((cVar2 = fn_802026E4(self,8)) != 1) goto _end;
  if ((u16)sVar1 == 0x2b) goto _end;
  fn_80202810(self,8);
  fn_80202810(self,0x17);
  (&lbl_80478D78)[5] = 1;
  fn_801F4C14(0,0,0x36,0,self);
  fn_80211B94(lbl_8047B62C,(u32)lbl_80379249,0);
  cVar2 = fn_801FECD4(self);
  if (cVar2 == 1) {
    fn_801FE7EC(self,0x7c,0,0);
  }
_end:
  return 1;
}
/* Address: 0x802317E4 | Size: 0x7e4 | Ghidra import */
void fn_802317E4(void)

{
    extern u32 DAT_8038fffc;
    extern u32 fn_800FA280();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern void fn_801DA7AC();
    extern short fn_801EF634();
    extern u32 fn_801F025C();
    extern void fn_801F2838();
    extern s8 fn_801F288C();
    extern s8 fn_801F28E0();
    extern s8 fn_801F29DC();
    extern void fn_801F2A30();
    extern void fn_801F37B0();
    extern void fn_801F3B24();
    extern void fn_801F4718();
    extern int fn_801F47B4();
    extern void fn_801F4C14();
    extern void fn_801F6CA0();
    extern s8 fn_801F6CF4();
    extern s8 fn_801F6D48();
    extern s8 fn_801F6E98();
    extern void fn_801F6EEC();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 lbl_80478D7D;
    extern u32 lbl_8047B62C;
  u32 uVar1;
  short sVar3;
  s8 cVar4;
  int iVar2;
  s8 cVar5;
  s8 cVar6;

  u8 bVar7;
  u32 local_28 [5];
  
  sVar3 = fn_801EF634();
  if (sVar3 == 0) {
    cVar4 = (int)fn_801F453C(0,0);
    fn_801F4718(0);
    fn_801F3B24(0,0);
    local_28[0] = 0;
    fn_801F37B0(0,0x80231fc8,local_28,0);
    bVar7 = 0;
    while (1) {
      if (1 < bVar7) break;
      iVar2 = fn_801F47B4(0,bVar7);
      if (iVar2 != 0) {
        uVar1 = fn_801F025C(0xc,iVar2);
        cVar5 = fn_802062FC();
        if (cVar5 == 0) {
          uVar1 = fn_801F025C(0xd,iVar2);
        }
        fn_801F4C14(0,0,0x36,0,uVar1);
        cVar5 = fn_801F6E98(iVar2,0x48);
        if (cVar5 == 1) {
          cVar5 = fn_801F6D48(iVar2,0x48);
          cVar6 = fn_801F6CF4(iVar2,0x48);
          if ((char)(cVar6 + 1) < cVar5) {
            fn_801F6CA0(iVar2,0x48);
          }
          else {
            fn_801F6EEC(iVar2,0x48);
            fn_8011BEB4(0,0x73,1,0);
            uVar1 = fn_800FA280();
            fn_80132A38(0xd,uVar1);
            fn_80211B94(lbl_8047B62C,0x80378b30,0);
          }
        }
      }
      bVar7 = bVar7 + 1;
    }
    fn_801DA7AC();
    bVar7 = 0;
    while (1) {
      if (1 < bVar7) break;
      iVar2 = fn_801F47B4(0,bVar7);
      if (iVar2 != 0) {
        uVar1 = fn_801F025C(0xc,iVar2);
        cVar5 = fn_802062FC();
        if (cVar5 == 0) {
          uVar1 = fn_801F025C(0xd,iVar2);
        }
        fn_801F4C14(0,0,0x36,0,uVar1);
        cVar5 = fn_801F6E98(iVar2,0x49);
        if (cVar5 == 1) {
          cVar5 = fn_801F6D48(iVar2,0x49);
          cVar6 = fn_801F6CF4(iVar2,0x49);
          if ((char)(cVar6 + 1) < cVar5) {
            fn_801F6CA0(iVar2,0x49);
          }
          else {
            fn_801F6EEC(iVar2,0x49);
            fn_8011BEB4(0,0x71,1,0);
            uVar1 = fn_800FA280();
            fn_80132A38(0xd,uVar1);
            lbl_80478D7D = bVar7;
            fn_80211B94(lbl_8047B62C,0x80378b30,0);
          }
        }
      }
      bVar7 = bVar7 + 1;
    }
    fn_801DA7AC();
    bVar7 = 0;
    while (1) {
      if (1 < bVar7) break;
      iVar2 = fn_801F47B4(0,bVar7);
      if (iVar2 != 0) {
        uVar1 = fn_801F025C(0xc,iVar2);
        cVar5 = fn_802062FC();
        if (cVar5 == 0) {
          uVar1 = fn_801F025C(0xd,iVar2);
        }
        fn_801F4C14(0,0,0x36,0,uVar1);
        cVar5 = fn_801F6E98(iVar2,0x4c);
        if (cVar5 == 1) {
          cVar5 = fn_801F6D48(iVar2,0x4c);
          cVar6 = fn_801F6CF4(iVar2,0x4c);
          if ((char)(cVar6 + 1) < cVar5) {
            fn_801F6CA0(iVar2,0x4c);
          }
          else {
            fn_801F6EEC(iVar2,0x4c);
            fn_8011BEB4(0,0x36,1,0);
            uVar1 = fn_800FA280();
            fn_80132A38(0xd,uVar1);
            fn_80211B94(lbl_8047B62C,0x80378b30,0);
          }
        }
      }
      bVar7 = bVar7 + 1;
    }
    fn_801DA7AC();
    bVar7 = 0;
    while (1) {
      if (1 < bVar7) break;
      iVar2 = fn_801F47B4(0,bVar7);
      if (iVar2 != 0) {
        uVar1 = fn_801F025C(0xc,iVar2);
        cVar5 = fn_802062FC();
        if (cVar5 == 0) {
          uVar1 = fn_801F025C(0xd,iVar2);
        }
        fn_801F4C14(0,0,0x36,0,uVar1);
        cVar5 = fn_801F6E98(iVar2,0x4b);
        if (cVar5 == 1) {
          cVar5 = fn_801F6D48(iVar2,0x4b);
          cVar6 = fn_801F6CF4(iVar2,0x4b);
          if ((char)(cVar6 + 1) < cVar5) {
            fn_801F6CA0(iVar2,0x4b);
          }
          else {
            fn_801F6EEC(iVar2,0x4b);
            fn_80211B94(lbl_8047B62C,0x80378b5b,0);
          }
        }
      }
      bVar7 = bVar7 + 1;
    }
    fn_801DA7AC();
    fn_801F37B0(0,0x80232024,0,1);
    fn_801DA7AC();
    if (cVar4 == 2) {
      fn_801F4C14(0,0,0x36,0,local_28[0]);
      cVar5 = fn_801F29DC(0,0x50);
      if (cVar5 == 0) {
        cVar5 = fn_801F28E0(0,0x54);
        cVar6 = fn_801F288C(0,0x54);
        if ((char)(cVar6 + 1) < cVar5) {
          fn_801F2838(0,0x54);
          lbl_80478D7D = 0;
        }
        else {
          fn_801F2A30(0,0x54);
          lbl_80478D7D = 2;
        }
      }
      else {
        lbl_80478D7D = 0;
      }
      fn_80211B94(lbl_8047B62C,0x80378a5f,0);
    }
    fn_801DA7AC();
    sVar3 = fn_801EF634();
    if (sVar3 == 0) {
      if (cVar4 == 3) {
        fn_801F4C14(0,0,0x36,0,local_28[0]);
        cVar5 = fn_801F29DC(0,0x51);
        if (cVar5 == 1) {
          uVar1 = 0x80378968;
        }
        else {
          cVar5 = fn_801F28E0(0,0x55);
          cVar6 = fn_801F288C(0,0x55);
          if ((char)(cVar6 + 1) < cVar5) {
            fn_801F2838(0,0x55);
            uVar1 = 0x80378968;
          }
          else {
            fn_801F2A30(0,0x55);
            uVar1 = 0x80378a4d;
          }
        }
        lbl_80478D7D = 0;
        DAT_8038fffc = 0xc;
        fn_80211B94(lbl_8047B62C,uVar1,0);
      }
      fn_801DA7AC();
      sVar3 = fn_801EF634();
      if (sVar3 == 0) {
        if (cVar4 == 1) {
          fn_801F4C14(0,0,0x36,0,local_28[0]);
          cVar5 = fn_801F29DC(0,0x4f);
          if (cVar5 == 1) {
            uVar1 = 0x80378a7c;
          }
          else {
            cVar5 = fn_801F28E0(0,0x53);
            cVar6 = fn_801F288C(0,0x53);
            if ((char)(cVar6 + 1) < cVar5) {
              fn_801F2838(0,0x53);
              uVar1 = 0x80378a7c;
            }
            else {
              fn_801F2A30(0,0x53);
              uVar1 = 0x80378a8e;
            }
          }
          fn_80211B94(lbl_8047B62C,uVar1,0);
        }
        fn_801DA7AC();
        sVar3 = fn_801EF634();
        if (sVar3 == 0) {
          if (cVar4 == 4) {
            fn_801F4C14(0,0,0x36,0,local_28[0]);
            cVar4 = fn_801F28E0(0,0x52);
            cVar5 = fn_801F288C(0,0x52);
            if ((char)(cVar5 + 1) < cVar4) {
              fn_801F2838(0,0x52);
              uVar1 = 0x80378968;
            }
            else {
              fn_801F2A30(0,0x52);
              uVar1 = 0x80378a4d;
            }
            lbl_80478D7D = 1;
            DAT_8038fffc = 0xd;
            fn_80211B94(lbl_8047B62C,uVar1,0);
          }
          fn_801DA7AC();
          fn_801EF634();
        }
      }
    }
  }
  return;
}
/* Address: 0x80231FC8 | Size: 0x5c | Ghidra import */
u32 fn_80231FC8(u32 r3, u32 r4, u32 *r5)

{
  if (fn_8012640C(r3,0,0xee,0) != 0) {
    if (r5 != (void *)0) {
      *r5 = r3;
    }
    return 0;
  }
  return 1;
}
/* Address: 0x80232024 | Size: 0xec | Ghidra import */
u32 fn_80232024(void)

{
    u32 r3;

    extern void fn_801F4C14();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
  s8 cVar1;
  s8 cVar2;

  cVar1 = fn_802062FC();
  if ((cVar1 != 0) && (cVar1 = fn_802026E4(r3,0x35), cVar1 == 1)) {
    cVar1 = fn_80202234(r3,0x35);
    cVar2 = fn_80202108(r3,0x35);
    if ((char)(cVar2 + 1) < cVar1) {
      fn_80201FDC(r3,0x35);
    }
    else {
      fn_801F4C14(0,0,0x36,0,r3);
      fn_801F4C14(0,0,0x43,0,r3);
      fn_80211B94(lbl_8047B62C,0x80379052,0);
      fn_80202810(r3,0x35);
    }
  }
  return 1;
}
/* Address: 0x80232110 | Size: 0xc18 | Ghidra import */
int fn_80232110(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;

    extern int fn_8010C4A0();
    extern s8 fn_8011BEB4();
    extern s8 fn_8012A5B0();
    extern s8 fn_801363A8();
    extern u32 fn_801F2654();
    extern short fn_801F33E8();
    extern short fn_801F3624();
    extern u32 fn_801F4220();
    extern s8 fn_801F54A4();
    extern s8 fn_801F6E98();
    extern s8 fn_802026E4();
    extern s8 fn_80203BDC();
    extern s8 fn_80203CCC();
    extern short fn_80203D3C();
    extern u32 fn_80203E0C();
    extern int fn_80203EDC();
    extern u32 fn_80203FE4();
    extern u32 fn_80205B8C();
    extern u32 fn_8020E4E8();
  s8 cVar16;
  s8 cVar17;
  s8 cVar18;
  s8 cVar19;
  u32 uVar1;
  s8 cVar20;
  short sVar9;
  u16 uVar10;
  int iVar2;
  short sVar11;
  u32 uVar3;
  u32 uVar4;
  u8 bVar21;
  u8 bVar22;
  u32 uVar5;
  short sVar12;
  short sVar13;
  short sVar14;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar23;
  u8 bVar24;
  u16 uVar15;
  s8 cVar25;
  u8 bVar26;
  int iVar27;
  u32 uVar28;
  
  cVar16 = (int)fn_801F453C(0,1);
  cVar17 = fn_801F54A4(0,0,0x1d,0);
  cVar18 = fn_801F54A4(0,0,0x2c,0);
  if ((r7 & 0xffff) == 0) {
    r7 = fn_8011BEB4(0,r6,7,0);
    r7 = r7 & 0xffff;
  }
  if ((int)r8 < 0) {
    r8 = fn_8011BEB4(0,r6,3,0);
  }
  r8 = r8 & 0xffff;
  cVar19 = fn_8011BEB4(0,r6,5,0);
  uVar1 = (int)fn_8012640C(r3,0,0xd9,0);
  cVar20 = fn_8011BEB4(uVar1,0,0x2b,0);
  sVar9 = fn_80207BF4(r3);
  uVar10 = fn_80203FE4(r3);
  iVar2 = fn_80203EDC(r3);
  sVar11 = fn_80203D3C(r3);
  uVar1 = fn_80205B8C(r3);
  uVar3 = (int)fn_8012640C(uVar1,0,0x88,0);
  uVar28 = uVar3 & 0xffff;
  uVar4 = (int)fn_8012640C(uVar1,0,0x8a,0);
  uVar4 = uVar4 & 0xffff;
  uVar1 = fn_801F4220(0,r3);
  bVar21 = (int)fn_8012640C(r3,0,0xe6,0);
  bVar22 = (int)fn_8012640C(r3,0,0xe8,0);
  uVar5 = fn_80203E0C(r3);
  sVar12 = fn_80207BF4(r4);
  sVar13 = fn_80203FE4(r4);
  fn_80203EDC(r4);
  sVar14 = fn_80203D3C(r4);
  uVar6 = fn_80205B8C(r4);
  uVar7 = (int)fn_8012640C(uVar6,0,0x89,0);
  uVar7 = uVar7 & 0xffff;
  uVar8 = (int)fn_8012640C(uVar6,0,0x8b,0);
  uVar8 = uVar8 & 0xffff;
  uVar6 = fn_801F4220(0,r4);
  bVar23 = (int)fn_8012640C(r4,0,0xe7,0);
  bVar24 = (int)fn_8012640C(r4,0,0xe9,0);
  uVar15 = fn_801F2654(0,1,r4,0);
  if ((sVar9 == 0x25) || (sVar9 == 0x4a)) {
    uVar28 = (uVar3 & 0x7fff) << 1;
  }
  if ((cVar17 == 1) && (cVar25 = fn_8012A5B0(uVar1,0xf,0), cVar25 == 1)) {
    uVar28 = (uVar28 * 0x6e) / 100 & 0xffff;
  }
  if ((cVar17 == 1) && (cVar25 = fn_8012A5B0(uVar6,0x13,0), cVar25 == 1)) {
    uVar7 = (uVar7 * 0x6e) / 100 & 0xffff;
  }
  if ((cVar17 == 1) && (cVar25 = fn_8012A5B0(uVar1,0x15,0), cVar25 == 1)) {
    uVar4 = (uVar4 * 0x6e) / 100 & 0xffff;
  }
  if ((cVar17 == 1) && (cVar17 = fn_8012A5B0(uVar6,0x15,0), cVar17 == 1)) {
    uVar8 = (uVar8 * 0x6e) / 100 & 0xffff;
  }
  uVar3 = 0;
  iVar27 = 0;
  do {
    if ((uVar10 == *(u8 *)(iVar27 + -0x7fd86130)) && (r8 == *(u8 *)(iVar27 + -0x7fd8612f)))
    {
      bVar26 = fn_8010C4A0(r8);
      if (bVar26 == 2) {
        uVar4 = (int)(uVar4 * (iVar2 + 100)) / 100 & 0xffff;
      }
      else if ((bVar26 < 2) && (bVar26 != 0)) {
        uVar28 = (int)(uVar28 * (iVar2 + 100)) / 100 & 0xffff;
      }
    }
    uVar3 = uVar3 + 1;
    iVar27 = iVar27 + 2;
  } while (uVar3 < 0x11);
  if (uVar10 == 0x1d) {
    uVar28 = (uVar28 * 0x96) / 100 & 0xffff;
  }
  if (((cVar18 == 1) && (uVar10 == 0x22)) && ((sVar11 == 0x198 || (sVar11 == 0x197)))) {
    uVar4 = (uVar4 * 0x96) / 100 & 0xffff;
  }
  if (((cVar18 == 1) && (sVar13 == 0x22)) && ((sVar14 == 0x198 || (sVar14 == 0x197)))) {
    uVar8 = (uVar8 * 0x96) / 100 & 0xffff;
  }
  if ((uVar10 == 0x23) && (sVar11 == 0x175)) {
    uVar4 = (uVar4 & 0x7fff) << 1;
  }
  if ((sVar13 == 0x24) && (sVar14 == 0x175)) {
    uVar8 = (uVar8 & 0x7fff) << 1;
  }
  if ((uVar10 == 0x2d) && (sVar11 == 0x19)) {
    uVar4 = (uVar4 & 0x7fff) << 1;
  }
  if ((sVar13 == 0x40) && (sVar14 == 0x84)) {
    uVar7 = (uVar7 & 0x7fff) << 1;
  }
  if ((uVar10 == 0x41) && ((sVar11 == 0x68 || (sVar11 == 0x69)))) {
    uVar28 = (uVar28 & 0x7fff) << 1;
  }
  if ((sVar12 == 0x2f) && ((r8 == 10 || (r8 == 0xf)))) {
    uVar4 = (int)uVar4 >> 1;
  }
  if (sVar9 == 0x37) {
    uVar28 = (uVar28 * 0x96) / 100 & 0xffff;
  }
  if ((sVar9 == 0x39) && (sVar11 = fn_801F3624(0,0x3a,0,0), sVar11 != 0)) {
    uVar4 = (uVar4 * 0x96) / 100 & 0xffff;
  }
  if ((sVar9 == 0x3a) && (sVar11 = fn_801F3624(0,0x39,0,0), sVar11 != 0)) {
    uVar4 = (uVar4 * 0x96) / 100 & 0xffff;
  }
  if ((sVar9 == 0x3e) && (cVar17 = fn_80203CCC(r3), cVar17 == 0)) {
    uVar28 = (uVar28 * 0x96) / 100 & 0xffff;
  }
  if ((sVar12 == 0x3f) && (cVar17 = fn_80203CCC(r4), cVar17 == 0)) {
    uVar7 = (uVar7 * 0x96) / 100 & 0xffff;
  }
  if ((r8 == 0xd) && (sVar11 = fn_801F33E8(0,0x38), sVar11 != 0)) {
    r7 = (int)(r7 & 0xffff) >> 1;
  }
  if ((r8 == 10) && (sVar11 = fn_801F33E8(0,0x39), sVar11 != 0)) {
    r7 = (int)(r7 & 0xffff) >> 1;
  }
  if (((r8 == 0xc) && (sVar9 == 0x41)) && (cVar17 = fn_80203BDC(r3,3), cVar17 != 0)) {
    r7 = ((r7 & 0xffff) * 0x96) / 100 & 0xffff;
  }
  if (((r8 == 10) && (sVar9 == 0x42)) && (cVar17 = fn_80203BDC(r3,3), cVar17 != 0)) {
    r7 = ((r7 & 0xffff) * 0x96) / 100 & 0xffff;
  }
  if (((r8 == 0xb) && (sVar9 == 0x43)) && (cVar17 = fn_80203BDC(r3,3), cVar17 != 0)) {
    r7 = ((r7 & 0xffff) * 0x96) / 100 & 0xffff;
  }
  if (((r8 == 6) && (sVar9 == 0x44)) && (cVar17 = fn_80203BDC(r3,3), cVar17 != 0)) {
    r7 = ((r7 & 0xffff) * 0x96) / 100 & 0xffff;
  }
  sVar11 = fn_8011BEB4(0,r6,9,0);
  if (sVar11 == 7) {
    uVar7 = uVar7 >> 1;
  }
  bVar26 = fn_8010C4A0(r8);
  if (bVar26 == 2) {
    if (cVar20 == 2) {
      if (6 < bVar22) {
        uVar4 = fn_8020E4E8(bVar22,uVar4);
      }
    }
    else {
      uVar4 = fn_8020E4E8(bVar22,uVar4);
    }
    if (cVar20 == 2) {
      if (bVar24 < 6) {
        uVar8 = fn_8020E4E8(bVar24,uVar8);
      }
    }
    else {
      uVar8 = fn_8020E4E8(bVar24,uVar8);
    }
    uVar7 = ((int)(uVar4 * (r7 & 0xffff) * (((uVar5 & 0xff) << 1) / 5 + 2)) / (int)uVar8) /
            0x32;
    cVar17 = fn_801F6E98(r5,0x49);
    if ((cVar17 == 1) && (cVar20 == 1)) {
      uVar10 = fn_801F54A4(0,0,0x19,0);
      if ((uVar10 < 2) || (uVar15 < 2)) {
        uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
      }
      else {
        uVar7 = (int)uVar7 / 3 << 1;
      }
    }
    uVar10 = fn_801F54A4(0,0,0x19,0);
    if (((1 < uVar10) && (cVar19 == 4)) && (1 < uVar15)) {
      uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
    }
    if (cVar16 == 2) {
      if (r8 == 0xb) {
        uVar7 = (int)(uVar7 * 0xf) / 10;
      }
      else if ((r8 < 0xb) && (9 < r8)) {
        uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
      }
    }
    cVar17 = fn_801363A8(cVar16);
    if ((cVar17 == 0) && ((r6 & 0xffff) == 0x4c)) {
      uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
    }
    if (cVar16 == 1) {
      if (r8 == 0xb) {
        uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
      }
      else if ((r8 < 0xb) && (9 < r8)) {
        uVar7 = (int)(uVar7 * 0xf) / 10;
      }
    }
    cVar16 = fn_802026E4(r3,0x3a);
    if ((cVar16 == 1) && (r8 == 10)) {
      uVar7 = (int)(uVar7 * 0xf) / 10;
    }
  }
  else if ((bVar26 < 2) && (bVar26 != 0)) {
    if (cVar20 == 2) {
      if (6 < bVar21) {
        uVar28 = fn_8020E4E8(bVar21,uVar28);
      }
    }
    else {
      uVar28 = fn_8020E4E8(bVar21,uVar28);
    }
    if (cVar20 == 2) {
      if (bVar23 < 6) {
        uVar7 = fn_8020E4E8(bVar23,uVar7);
      }
    }
    else {
      uVar7 = fn_8020E4E8(bVar23,uVar7);
    }
    uVar7 = ((int)(uVar28 * (r7 & 0xffff) * (((uVar5 & 0xff) << 1) / 5 + 2)) / (int)uVar7) /
            0x32;
    cVar16 = fn_802026E4(r3,6);
    if ((cVar16 == 1) && (sVar9 != 0x3e)) {
      uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
    }
    cVar16 = fn_801F6E98(r5,0x48);
    if ((cVar16 == 1) && (cVar20 == 1)) {
      uVar10 = fn_801F54A4(0,0,0x19,0);
      if ((uVar10 < 2) || (uVar15 < 2)) {
        uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
      }
      else {
        uVar7 = (int)uVar7 / 3 << 1;
      }
    }
    uVar10 = fn_801F54A4(0,0,0x19,0);
    if (((1 < uVar10) && (cVar19 == 4)) && (1 < uVar15)) {
      uVar7 = ((int)uVar7 >> 1) + (u32)((int)uVar7 < 0 && (uVar7 & 1) != 0);
    }
    if (uVar7 == 0) {
      uVar7 = 1;
    }
  }
  else {
    uVar7 = 0;
  }
  return uVar7 + 2;
}
/* Address: 0x80232D28 | Size: 0x2bc | Ghidra import */
u32 fn_80232D28(void)

{
    int r3;
    u32 r4;
    int *r5;

    extern int fn_801F00D0();
    extern void fn_80201340();
    extern u32 fn_80201D84();
    extern void fn_80201FDC();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_80202B88();
    extern void fn_80203198();
    extern s8 fn_802062FC();
  u32 bVar1;
  u32 uVar2;
  s8 cVar4;
  int iVar3;
  int iVar5;
  
  uVar2 = r5[1];
  iVar5 = *r5;
  cVar4 = fn_802062FC();
  if ((cVar4 != 0) && (r3 != iVar5)) {
    if ((uVar2 & 0xffff) == 0x7f) {
      cVar4 = fn_80202B88(iVar5,r3);
      if (cVar4 == 0) {
        cVar4 = fn_802026E4(r3,0x1d);
        if (((cVar4 == 1) && (uVar2 = fn_80201D84(r3,0x1d), (uVar2 & 0xffff) != 0)) &&
           ((iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0 && (iVar3 == iVar5)))) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
        if (bVar1) {
          fn_80201FDC(r3,0x1d,0);
        }
      }
    }
    else {
      cVar4 = fn_802026E4(r3,0x16);
      if ((((cVar4 == 1) && (uVar2 = fn_80201D84(r3,0x16), (uVar2 & 0xffff) != 0)) &&
          (iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0)) && (iVar3 == iVar5)) {
        bVar1 = 1;
      }
      else {
        bVar1 = 0;
      }
      if (bVar1) {
        fn_80202810(r3,0x16);
      }
      cVar4 = fn_802026E4(r3,0x1d);
      if (((cVar4 == 1) && (uVar2 = fn_80201D84(r3,0x1d), (uVar2 & 0xffff) != 0)) &&
         ((iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0 && (iVar3 == iVar5)))) {
        bVar1 = 1;
      }
      else {
        bVar1 = 0;
      }
      if (bVar1) {
        fn_80202810(r3,0x1d);
      }
    }
    cVar4 = fn_802026E4(r3,10);
    if ((((cVar4 == 1) && (uVar2 = fn_80201D84(r3,10), (uVar2 & 0xffff) != 0)) &&
        (iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0)) && (iVar3 == iVar5)) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      fn_80202810(r3,10);
    }
    cVar4 = fn_802026E4(r3,0xe);
    if (((cVar4 == 1) && (uVar2 = fn_80201D84(r3,0xe), (uVar2 & 0xffff) != 0)) &&
       ((iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0 && (iVar3 == iVar5)))) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      fn_80202810(r3,0xe);
    }
    cVar4 = fn_80202B88(r3,iVar5);
    if (cVar4 == 0) {
      fn_80201340(r3,iVar5,0);
      fn_80203198(r3,iVar5);
    }
  }
  return 1;
}
/* Address: 0x80232FE4 | Size: 0x1c0 | Ghidra import */
u32 fn_80232FE4(void)

{
    int r3;
    u32 r4;
    int r5;

    extern int fn_801F00D0();
    extern void fn_80201340();
    extern u32 fn_80201D84();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_80202B88();
    extern void fn_80203198();
    extern s8 fn_802062FC();
  u32 bVar1;
  s8 cVar4;
  u32 uVar2;
  int iVar3;
  
  cVar4 = fn_802062FC();
  if ((cVar4 != 0) && (r3 != r5)) {
    cVar4 = fn_802026E4(r3,0x16);
    if ((cVar4 == 1) &&
       (((uVar2 = fn_80201D84(r3,0x16), (uVar2 & 0xffff) != 0 &&
         (iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0)) && (iVar3 == r5)))) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      fn_80202810(r3,0x16);
    }
    cVar4 = fn_802026E4(r3,10);
    if (((cVar4 == 1) && (uVar2 = fn_80201D84(r3,10), (uVar2 & 0xffff) != 0)) &&
       ((iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0 && (iVar3 == r5)))) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      fn_80202810(r3,10);
    }
    cVar4 = fn_802026E4(r3,0xe);
    if (((cVar4 == 1) && (uVar2 = fn_80201D84(r3,0xe), (uVar2 & 0xffff) != 0)) &&
       ((iVar3 = fn_801F00D0(uVar2,r4), iVar3 != 0 && (iVar3 == r5)))) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1) {
      fn_80202810(r3,0xe);
    }
    cVar4 = fn_80202B88(r3,r5);
    if (cVar4 == 0) {
      fn_80201340(r3,r5,0);
      fn_80203198(r3,r5);
    }
  }
  return 1;
}
/* Address: 0x802331A4 | Size: 0x50 | Ghidra import */
void fn_802331A4(u32 r3, u32 r4)

{
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
  u8 cVar1;

  cVar1 = fn_801FECD4();
  if (cVar1 == 1) {
    fn_801FE7EC(r3,r4,0,0);
  }
  return;
}
/* Address: 0x802331F4 | Size: 0xbbc | Ghidra import */
void fn_802331F4(u32 r3, u32 r4, int r5, u32 r6, u16 r7)
{
    extern u32 fn_800E0C54();
    extern int fn_8010C54C();
    extern u32 fn_801F4804();
    extern u32 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
    extern void fn_80206AEC();
    extern int fn_80233DB0();
  u8 cVar1;
  u32 uVar2;
  u32 *puVar3;
  u32 *puVar4;
  u32 uVar5;
  u16 uVar10;
  int iVar6;
  int iVar7;
  u8 cVar17;
  u16 uVar11;
  u32 uVar8;
  u16 uVar12;
  u16 uVar13;
  u16 uVar14;
  u16 uVar15;
  short sVar16;
  int iVar9;
  u32 *puVar18;
  u32 *puVar19;
  u32 uVar20;
  u8 bVar21;
  u32 uVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  int iVar26;
  char local_318 [8];
  u32 auStack_310 [5];
  u32 uStack_2fc;
  u32 local_2f8 [84];
  u32 uStack_1a8;
  u32 local_1a4 [85];
  u32 local_50;
  u32 local_4c;
  
  uVar10 = fn_801FB1C0(r3,0,0x43,0);
  uVar10 = fn_801FB1C0(0,uVar10,2,0);
  iVar6 = fn_801FB1C0(r3,0,0x45,0);
  if ((iVar6 != 0) && (iVar7 = fn_801FB1C0(r3,0,0x45,1), iVar7 != 0)) {
    iVar26 = 0x2a;
    puVar3 = (u32 *)(iVar6 + -4);
    puVar4 = &uStack_1a8;
    do {
      puVar19 = puVar4;
      puVar18 = puVar3;
      uVar5 = puVar18[2];
      puVar19[1] = puVar18[1];
      puVar19[2] = uVar5;
      iVar26 = iVar26 + -1;
      puVar3 = puVar18 + 2;
      puVar4 = puVar19 + 2;
    } while (iVar26 != 0);
    puVar19[3] = puVar18[3];
    iVar26 = 0x2a;
    puVar3 = (u32 *)(iVar7 + -4);
    puVar4 = &uStack_2fc;
    do {
      puVar19 = puVar4;
      puVar18 = puVar3;
      uVar5 = puVar18[2];
      puVar19[1] = puVar18[1];
      puVar19[2] = uVar5;
      iVar26 = iVar26 + -1;
      puVar3 = puVar18 + 2;
      puVar4 = puVar19 + 2;
    } while (iVar26 != 0);
    puVar19[3] = puVar18[3];
    for (uVar20 = 0; (uVar20 & 0xffff) < (r6 & 0xffff); uVar20 = uVar20 + 1) {
      *(u32 *)(r5 + (uVar20 & 0xffff) * 4) = 0;
    }
    uVar20 = 0;
    for (uVar11 = 0; uVar11 < 6; uVar11 = uVar11 + 1) {
      local_318[uVar11] = 0;
    }
    cVar17 = fn_801FB1C0(0,uVar10,0x22,0);
    uVar11 = 1;
    if (cVar17 != 1) {
      uVar11 = r7;
    }
    cVar17 = fn_801FB1C0(0,uVar10,0x1f,0);
    if (cVar17 == 1) {
      while (((uVar20 & 0xffff) < (r6 & 0xffff) &&
             (uVar11 = fn_80233DB0(r3,r4,r5,r6,0,auStack_310), uVar11 != 0))) {
        uVar8 = fn_800E0C54();
        iVar26 = (uVar20 & 0xffff) * 4;
        *(u32 *)(r5 + iVar26) =
             *(u32 *)
              ((int)auStack_310 +
              (((uVar8 & 0xffff) - ((uVar8 & 0xffff) / (u32)uVar11) * (u32)uVar11) * 4 & 0x3fffc))
        ;
        uVar5 = fn_801F4804(0);
        fn_80206AEC(iVar6,*(u32 *)(r5 + iVar26),uVar5);
        fn_801F54A4(0,0,0x14,0);
        uVar12 = fn_801FB1C0(r3,0,0x43,0);
        uVar12 = fn_801FB1C0(0,uVar12,2,0);
        uVar5 = fn_80205BE8(iVar6);
        uVar13 = (int)fn_8012640C(uVar5,0,0xc9,0);
        cVar17 = fn_801FB1C0(0,uVar12,0x23,0);
        if (cVar17 == 1) {
          cVar17 = fn_801FB1C0(0,uVar13,0x1c,0);
        }
        else {
          cVar17 = 0;
        }
        local_318[uVar20 & 0xffff] = cVar17;
        uVar20 = uVar20 + 1;
      }
    }
    else {
      uVar15 = 0;
      while ((uVar15 < uVar11 &&
             (uVar14 = fn_80233DB0(r3,r4,r5,r6,1,auStack_310), uVar14 != 0))) {
        uVar22 = fn_800E0C54();
        local_318[uVar20 & 0xffff] = 2;
        uVar8 = uVar20 & 0xffff;
        uVar20 = uVar20 + 1;
        uVar15 = uVar15 + 1;
        *(u32 *)(r5 + uVar8 * 4) =
             *(u32 *)
              ((int)auStack_310 +
              (((uVar22 & 0xffff) - ((uVar22 & 0xffff) / (u32)uVar14) * (u32)uVar14) * 4 & 0x3fffc
              ));
      }
      uVar15 = 0;
      while ((uVar15 < uVar11 &&
             (uVar14 = fn_80233DB0(r3,r4,r5,r6,2,auStack_310), uVar14 != 0))) {
        uVar22 = fn_800E0C54();
        local_318[uVar20 & 0xffff] = 3;
        uVar8 = uVar20 & 0xffff;
        uVar20 = uVar20 + 1;
        uVar15 = uVar15 + 1;
        *(u32 *)(r5 + uVar8 * 4) =
             *(u32 *)
              ((int)auStack_310 +
              (((uVar22 & 0xffff) - ((uVar22 & 0xffff) / (u32)uVar14) * (u32)uVar14) * 4 & 0x3fffc
              ));
      }
      while (((uVar20 & 0xffff) < (r6 & 0xffff) &&
             (uVar11 = fn_80233DB0(r3,r4,r5,r6,3,auStack_310), uVar11 != 0))) {
        uVar8 = fn_800E0C54();
        iVar26 = (uVar20 & 0xffff) * 4;
        *(u32 *)(r5 + iVar26) =
             *(u32 *)
              ((int)auStack_310 +
              (((uVar8 & 0xffff) - ((uVar8 & 0xffff) / (u32)uVar11) * (u32)uVar11) * 4 & 0x3fffc))
        ;
        uVar5 = fn_801F4804(0);
        fn_80206AEC(iVar6,*(u32 *)(r5 + iVar26),uVar5);
        fn_801F54A4(0,0,0x14,0);
        uVar12 = fn_801FB1C0(r3,0,0x43,0);
        uVar12 = fn_801FB1C0(0,uVar12,2,0);
        uVar5 = fn_80205BE8(iVar6);
        uVar13 = (int)fn_8012640C(uVar5,0,0xc9,0);
        cVar17 = fn_801FB1C0(0,uVar12,0x23,0);
        if (cVar17 == 1) {
          cVar17 = fn_801FB1C0(0,uVar13,0x1c,0);
        }
        else {
          cVar17 = 0;
        }
        local_318[uVar20 & 0xffff] = cVar17;
        uVar20 = uVar20 + 1;
      }
      while (((uVar20 & 0xffff) < (r6 & 0xffff) &&
             (uVar11 = fn_80233DB0(r3,r4,r5,r6,0,auStack_310), uVar11 != 0))) {
        uVar22 = fn_800E0C54();
        iVar26 = (uVar20 & 0xffff) * 4;
        uVar8 = uVar20 & 0xffff;
        *(u32 *)(r5 + iVar26) =
             *(u32 *)
              ((int)auStack_310 +
              (((uVar22 & 0xffff) - ((uVar22 & 0xffff) / (u32)uVar11) * (u32)uVar11) * 4 & 0x3fffc
              ));
        uVar5 = fn_801F4804(0);
        fn_80206AEC(iVar6,*(u32 *)(r5 + iVar26),uVar5);
        fn_801F54A4(0,0,0x14,0);
        uVar12 = fn_801FB1C0(r3,0,0x43,0);
        uVar12 = fn_801FB1C0(0,uVar12,2,0);
        uVar5 = fn_80205BE8(iVar6);
        uVar13 = (int)fn_8012640C(uVar5,0,0xc9,0);
        cVar17 = fn_801FB1C0(0,uVar12,0x23,0);
        if (cVar17 == 1) {
          cVar17 = fn_801FB1C0(0,uVar13,0x1c,0);
        }
        else {
          cVar17 = 0;
        }
        local_318[uVar8] = cVar17;
        if (local_318[uVar8] == 2) {
          local_318[uVar8] = 3;
        }
        uVar20 = uVar20 + 1;
      }
    }
    cVar17 = fn_801FB1C0(0,uVar10,0x1f,0);
    if (cVar17 != 1) {
      cVar17 = fn_801FB1C0(0,uVar10,0x21,0);
      if (cVar17 == 1) {
        for (uVar20 = 0; (uVar20 & 0xffff) < (r6 & 0xffff); uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          if (*(int *)(r5 + iVar26) != 0) {
            for (uVar8 = uVar20 + 1 & 0xffff; (uVar8 & 0xffff) < (r6 & 0xffff);
                uVar8 = uVar8 + 1) {
              iVar23 = (uVar8 & 0xffff) * 4;
              if (*(int *)(r5 + iVar23) != 0) {
                uVar11 = (int)fn_8012640C(*(u32 *)(r5 + iVar26),0,0xc9,0);
                uVar15 = (int)fn_8012640C(*(u32 *)(r5 + iVar23),0,0xc9,0);
                if (uVar15 < uVar11) {
                  uVar5 = *(u32 *)(r5 + iVar26);
                  cVar17 = local_318[uVar20 & 0xffff];
                  cVar1 = local_318[uVar8 & 0xffff];
                  *(u32 *)(r5 + iVar26) = *(u32 *)(r5 + iVar23);
                  local_318[uVar20 & 0xffff] = cVar1;
                  *(u32 *)(r5 + iVar23) = uVar5;
                  local_318[uVar8 & 0xffff] = cVar17;
                }
              }
            }
          }
        }
      }
      else {
        for (uVar20 = 0; (uVar20 & 0xffff) < (r6 & 0xffff); uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          if (*(int *)(r5 + iVar26) != 0) {
            for (uVar8 = uVar20 + 1 & 0xffff; (uVar8 & 0xffff) < (r6 & 0xffff);
                uVar8 = uVar8 + 1) {
              iVar23 = (uVar8 & 0xffff) * 4;
              iVar25 = *(int *)(r5 + iVar23);
              if (((iVar25 != 0) && (cVar17 = local_318[uVar20 & 0xffff], cVar17 != 2)) &&
                 (local_318[uVar8 & 0xffff] == 2)) {
                uVar5 = *(u32 *)(r5 + iVar26);
                local_318[uVar20 & 0xffff] = 2;
                *(int *)(r5 + iVar26) = iVar25;
                *(u32 *)(r5 + iVar23) = uVar5;
                local_318[uVar8 & 0xffff] = cVar17;
              }
            }
          }
        }
        for (uVar20 = 0; (uVar20 & 0xffff) < (r6 & 0xffff); uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          uVar8 = uVar20 & 0xffff;
          if ((*(int *)(r5 + iVar26) != 0) && (local_318[uVar8] != 2)) {
            for (uVar22 = uVar20 + 1 & 0xffff; (uVar22 & 0xffff) < (r6 & 0xffff);
                uVar22 = uVar22 + 1) {
              iVar23 = (uVar22 & 0xffff) * 4;
              uVar2 = uVar22 & 0xffff;
              iVar25 = *(int *)(r5 + iVar23);
              if (((iVar25 != 0) && (local_318[uVar8] != 0)) && (local_318[uVar8] != 1)) {
                if ((local_318[uVar2] == 0) || (local_318[uVar2] == 1)) {
                  uVar5 = *(u32 *)(r5 + iVar26);
                  cVar17 = local_318[uVar8];
                  cVar1 = local_318[uVar2];
                  *(int *)(r5 + iVar26) = iVar25;
                  local_318[uVar8] = cVar1;
                  *(u32 *)(r5 + iVar23) = uVar5;
                  local_318[uVar2] = cVar17;
                }
              }
            }
          }
        }
        local_4c = r6 & 0xffff;
        for (uVar20 = 0; (uVar20 & 0xffff) < local_4c; uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          uVar8 = uVar20 & 0xffff;
          if ((*(int *)(r5 + iVar26) != 0) &&
             ((local_318[uVar8] == 1 || (local_318[uVar8] == 0)))) {
            local_50 = r6 & 0xffff;
            for (uVar22 = uVar20 + 1 & 0xffff; (uVar22 & 0xffff) < local_50; uVar22 = uVar22 + 1) {
              iVar23 = (uVar22 & 0xffff) * 4;
              uVar2 = uVar22 & 0xffff;
              if (*(int *)(r5 + iVar23) != 0) {
                if ((local_318[uVar2] == 1) || (local_318[uVar2] == 0)) {
                  iVar25 = 0;
                  for (bVar21 = 0; bVar21 < 2; bVar21 = bVar21 + 1) {
                    uVar10 = fn_801F54A4(0,0,0x14,0);
                    uVar12 = fn_801FB1C0(r3,0,0x43,0);
                    uVar12 = fn_801FB1C0(0,uVar12,2,0);
                    uVar5 = fn_80205BE8(iVar6);
                    uVar13 = (int)fn_8012640C(uVar5,0,0x6e,0);
                    cVar17 = fn_801FB1C0(0,uVar12,0x2a,0);
                    if (cVar17 == 1) {
                      cVar17 = fn_801F8424(r3,iVar6,uVar10);
                      if (cVar17 == 0) {
                        sVar16 = (int)fn_8012640C(0,uVar13,0x16,bVar21);
                      }
                      else {
                        sVar16 = (int)fn_8012640C(0,uVar13,0x16,bVar21);
                      }
                    }
                    else {
                      sVar16 = 9;
                    }
                    if (sVar16 != 9) {
                      iVar24 = fn_8010C54C(sVar16,0);
                      iVar25 = iVar25 + iVar24;
                    }
                  }
                  iVar24 = 0;
                  for (bVar21 = 0; bVar21 < 2; bVar21 = bVar21 + 1) {
                    uVar10 = fn_801F54A4(0,0,0x14,0);
                    uVar12 = fn_801FB1C0(r3,0,0x43,0);
                    uVar12 = fn_801FB1C0(0,uVar12,2,0);
                    uVar5 = fn_80205BE8(iVar7);
                    uVar13 = (int)fn_8012640C(uVar5,0,0x6e,0);
                    cVar17 = fn_801FB1C0(0,uVar12,0x2a,0);
                    if (cVar17 == 1) {
                      cVar17 = fn_801F8424(r3,iVar7,uVar10);
                      if (cVar17 == 0) {
                        sVar16 = (int)fn_8012640C(0,uVar13,0x16,bVar21);
                      }
                      else {
                        sVar16 = (int)fn_8012640C(0,uVar13,0x16,bVar21);
                      }
                    }
                    else {
                      sVar16 = 9;
                    }
                    if (sVar16 != 9) {
                      iVar9 = fn_8010C54C(sVar16,0);
                      iVar24 = iVar24 + iVar9;
                    }
                  }
                  if (iVar25 < iVar24) {
                    uVar5 = *(u32 *)(r5 + iVar26);
                    cVar17 = local_318[uVar8];
                    cVar1 = local_318[uVar2];
                    *(u32 *)(r5 + iVar26) = *(u32 *)(r5 + iVar23);
                    local_318[uVar8] = cVar1;
                    *(u32 *)(r5 + iVar23) = uVar5;
                    local_318[uVar2] = cVar17;
                  }
                }
              }
            }
          }
        }
      }
    }
    iVar26 = 0x2a;
    puVar3 = &uStack_1a8;
    puVar4 = (u32 *)(iVar6 + -4);
    do {
      puVar19 = puVar4;
      puVar18 = puVar3;
      uVar5 = puVar18[2];
      puVar19[1] = puVar18[1];
      puVar19[2] = uVar5;
      iVar26 = iVar26 + -1;
      puVar3 = puVar18 + 2;
      puVar4 = puVar19 + 2;
    } while (iVar26 != 0);
    puVar19[3] = puVar18[3];
    iVar6 = 0x2a;
    puVar3 = &uStack_2fc;
    puVar4 = (u32 *)(iVar7 + -4);
    do {
      puVar19 = puVar4;
      puVar18 = puVar3;
      uVar5 = puVar18[2];
      puVar19[1] = puVar18[1];
      puVar19[2] = uVar5;
      iVar6 = iVar6 + -1;
      puVar3 = puVar18 + 2;
      puVar4 = puVar19 + 2;
    } while (iVar6 != 0);
    puVar19[3] = puVar18[3];
  }
  return;
}
/* Address: 0x80233DB0 | Size: 0x51c | Ghidra import */
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
/* Address: 0x802342CC | Size: 0x740 | Ghidra import */
void fn_802342CC(int r3, u32 r4)
{
    extern u8 fn_80008164();
    extern u32 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_80103BA8();
    extern void fn_80132A38();
    extern u8 fn_801F2020();
    extern void fn_801F3BB4();
    extern u8 fn_801F54A4();
    extern u32 fn_801F8100();
    extern int fn_801F8518();
    extern int fn_801F981C();
    extern u32 fn_801FB1C0();
    extern u8 fn_801FF1BC();
    extern u8 fn_80205C24();
    extern void fn_80207760();
    extern u32 fn_8020E1A4();
    extern void fn_8020E204();
    extern void fn_80234A0C();
    extern void fn_8023A308();
    extern void fn_8024E534();
    extern void fn_8024E578();
    extern void fn_802502EC();
    extern void fn_8026246C();
    extern u8 fn_802624CC();
  u32 bVar1;
  u32 uVar2;
  u16 uVar6;
  u32 uVar3;
  u32 uVar4;
  u8 cVar7;
  int iVar5;
  u16 uVar8;
  u32 uVar9;
  u16 uVar10;
  u16 local_108 [14];
  u16 local_ec [14];
  u16 local_d0 [14];
  u16 local_b4 [14];
  u16 local_98 [14];
  u16 local_7c [14];
  u16 local_60 [14];
  int local_44 [10];
  
  fn_8020E204(r4);
  uVar2 = fn_8020E1A4();
  uVar2 = uVar2 & 0xff;
  fn_80234A0C(r3);
  uVar9 = 0;
  while (1) {
    if (uVar2 <= (uVar9 & 0xffff)) break;
    iVar5 = fn_801F981C(r3,uVar9);
    if ((iVar5 != 0) && (cVar7 = fn_80205C24(iVar5,1), cVar7 != 0)) {
      fn_80207760(iVar5);
    }
    uVar9 = uVar9 + 1;
  }
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  uVar6 = fn_801FB1C0(0,uVar6,2,0);
  uVar9 = fn_801FB1C0(0,uVar6,0x26,0);
  uVar9 = uVar9 & 0xff;
  uVar3 = fn_800E0C54();
  if ((uVar3 & 0xffff) % 100 < uVar9) {
    uVar4 = fn_800FA280(0xec04);
    fn_80132A38(0xd,uVar4);
    if (r3 != 0) {
      uVar4 = fn_801F8100(r3);
      fn_80132A38(0x13,uVar4);
    }
    fn_80132A38(0x2f,uVar9);
    cVar7 = fn_80008164();
    if (((cVar7 == 1) && (fn_80103BA8(local_7c,1), (local_7c[0] & 0x800) == 0)) &&
       (cVar7 = fn_802624CC(0xec67), cVar7 == 1)) {
      fn_8026246C();
    }
    bVar1 = 1;
  }
  else {
    uVar4 = fn_800FA280(0xec04);
    fn_80132A38(0xd,uVar4);
    if (r3 != 0) {
      uVar4 = fn_801F8100(r3);
      fn_80132A38(0x13,uVar4);
    }
    fn_80132A38(0x2f,uVar9);
    cVar7 = fn_80008164();
    if (((cVar7 == 1) && (fn_80103BA8(local_98,1), (local_98[0] & 0x800) == 0)) &&
       (cVar7 = fn_802624CC(0xec68), cVar7 == 1)) {
      fn_8026246C();
    }
    bVar1 = 0;
  }
  if (bVar1) {
    uVar9 = 0;
    while (1) {
      if (uVar2 <= (uVar9 & 0xffff)) break;
      iVar5 = fn_801F8518(r3,uVar9);
      if ((iVar5 != 0) && (cVar7 = fn_801F2020(0,iVar5,0), cVar7 == 0)) {
        fn_8024E578(r3,iVar5,r4);
      }
      uVar9 = uVar9 + 1;
    }
  }
  cVar7 = fn_801F54A4(0,0,0x20,0);
  if (cVar7 == 1) {
    uVar6 = fn_801FB1C0(r3,0,0x43,0);
    uVar6 = fn_801FB1C0(0,uVar6,2,0);
    uVar9 = fn_801FB1C0(0,uVar6,0x27,0);
    uVar9 = uVar9 & 0xff;
    uVar3 = fn_800E0C54();
    if ((uVar3 & 0xffff) % 100 < uVar9) {
      uVar4 = fn_800FA280(0xec46);
      fn_80132A38(0xd,uVar4);
      if (r3 != 0) {
        uVar4 = fn_801F8100(r3);
        fn_80132A38(0x13,uVar4);
      }
      fn_80132A38(0x2f,uVar9);
      cVar7 = fn_80008164();
      if (((cVar7 == 1) && (fn_80103BA8(local_b4,1), (local_b4[0] & 0x800) == 0)) &&
         (cVar7 = fn_802624CC(0xec67), cVar7 == 1)) {
        fn_8026246C();
      }
      bVar1 = 1;
    }
    else {
      uVar4 = fn_800FA280(0xec46);
      fn_80132A38(0xd,uVar4);
      if (r3 != 0) {
        uVar4 = fn_801F8100(r3);
        fn_80132A38(0x13,uVar4);
      }
      fn_80132A38(0x2f,uVar9);
      cVar7 = fn_80008164();
      if (((cVar7 == 1) && (fn_80103BA8(local_d0,1), (local_d0[0] & 0x800) == 0)) &&
         (cVar7 = fn_802624CC(0xec68), cVar7 == 1)) {
        fn_8026246C();
      }
      bVar1 = 0;
    }
    if (bVar1) {
      uVar9 = 0;
      while (1) {
        if (uVar2 <= (uVar9 & 0xffff)) break;
        iVar5 = fn_801F8518(r3,uVar9);
        if (iVar5 != 0) {
          fn_802502EC(r3,iVar5,r4);
        }
        uVar9 = uVar9 + 1;
      }
    }
  }
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  uVar6 = fn_801FB1C0(0,uVar6,2,0);
  uVar9 = fn_801FB1C0(0,uVar6,0x25,0);
  uVar9 = uVar9 & 0xff;
  uVar3 = fn_800E0C54();
  if ((uVar3 & 0xffff) % 100 < uVar9) {
    uVar4 = fn_800FA280(0xec47);
    fn_80132A38(0xd,uVar4);
    if (r3 != 0) {
      uVar4 = fn_801F8100(r3);
      fn_80132A38(0x13,uVar4);
    }
    fn_80132A38(0x2f,uVar9);
    cVar7 = fn_80008164();
    if (((cVar7 == 1) && (fn_80103BA8(local_ec,1), (local_ec[0] & 0x800) == 0)) &&
       (cVar7 = fn_802624CC(0xec67), cVar7 == 1)) {
      fn_8026246C();
    }
    bVar1 = 1;
  }
  else {
    uVar4 = fn_800FA280(0xec47);
    fn_80132A38(0xd,uVar4);
    if (r3 != 0) {
      uVar4 = fn_801F8100(r3);
      fn_80132A38(0x13,uVar4);
    }
    fn_80132A38(0x2f,uVar9);
    cVar7 = fn_80008164();
    if (((cVar7 == 1) && (fn_80103BA8(local_108,1), (local_108[0] & 0x800) == 0)) &&
       (cVar7 = fn_802624CC(0xec68), cVar7 == 1)) {
      fn_8026246C();
    }
    bVar1 = 0;
  }
  if (bVar1) {
    uVar9 = 0;
    while (1) {
      if (uVar2 <= (uVar9 & 0xffff)) break;
      iVar5 = fn_801F8518(r3,uVar9);
      if (iVar5 != 0) {
        fn_8024E534(r3,iVar5,r4);
      }
      uVar9 = uVar9 + 1;
    }
  }
  uVar4 = fn_800FA280(0xec2c);
  fn_80132A38(0xd,uVar4);
  if (r3 != 0) {
    uVar4 = fn_801F8100(r3);
    fn_80132A38(0x13,uVar4);
  }
  fn_80132A38(0x2f,100);
  cVar7 = fn_80008164();
  if (((cVar7 == 1) && (fn_80103BA8(local_60,1), (local_60[0] & 0x800) == 0)) &&
     (cVar7 = fn_802624CC(0xec67), cVar7 == 1)) {
    fn_8026246C();
  }
  for (uVar8 = 0; uVar8 < 8; uVar8 = uVar8 + 1) {
    local_44[uVar8] = 0;
  }
  uVar8 = 0;
  uVar9 = 0;
  while (1) {
    if (uVar2 <= (uVar9 & 0xffff)) break;
    iVar5 = fn_801F8518(r3,uVar9);
    if ((iVar5 != 0) && (cVar7 = fn_801FF1BC(iVar5,1), cVar7 == 0)) {
      uVar3 = (u32)uVar8;
      uVar8 = uVar8 + 1;
      local_44[uVar3] = iVar5;
    }
    uVar9 = uVar9 + 1;
  }
  if (uVar8 != 0) {
    fn_801F3BB4(0,local_44,8,0);
    for (uVar10 = 0; uVar10 < uVar8; uVar10 = uVar10 + 1) {
      if (local_44[uVar10] != 0) {
        fn_8023A308(r3,local_44[uVar10],r4);
      }
    }
  }
  return;
}
/* Address: 0x80234A0C | Size: 0xC50 (3152 bytes) */
#pragma optimization_level 0
void fn_80234A0C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8011BAC0();
    extern void fn_8011BEB4();
    extern void fn_80120B00();
    extern void fn_801F1C18();
    extern void fn_801F37B0();
    extern void fn_801F54A4();
    extern void fn_801F849C();
    extern void fn_801FAA58();
    extern void fn_801FB1C0();
    extern void fn_80203A6C();
    extern void fn_80203DAC();
    extern void fn_80203E7C();
    extern void fn_80205B2C();
    extern void fn_80205B8C();
    extern void fn_80205BE8();
    extern void fn_8023C370();
    extern void fn_80236268();
    u8 sp[0x120];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 var_r14 = 0;
    u32 _ctx = 0;
    u32 var_r16 = 0;
    u32 var_r17 = 0;
    u32 var_r18 = 0;
    u32 var_r19 = 0;
    u32 var_r20 = 0;
    u32 var_r21 = 0;
    u32 var_r22 = 0;
    u32 var_r23 = 0;
    u32 var_r24 = 0;
    u32 var_r25 = 0;
    u32 var_r26 = 0;
    u32 var_r27 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;

    param1 = 0x0;
    param2 = 0x43;
    param3 = 0x0;
    _ctx = _ctx;
    fn_801FB1C0();
    param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    param2 = 0x2;
    param3 = 0x0;
    fn_801FB1C0();
    _ctx = _ctx;
    param1 = 0x1;
    fn_801F849C();
    param1 = (u32)sp + 0x88;
    _ctx = (u32)sp + 0x34;
    param3 = 0x0;
    param2 = 0x0;
    while (1) {
        tmp = param3 & 0xFFFF;
        if (tmp >= 8) break;
        param3 = param3 + 0x1;
        *(u32*)(param1 + tmp) = param2;
        *(u32*)(_ctx + tmp) = param2;

    }
    param1 = _ctx;
    param2 = (u32)sp + 0xa8;
    _ctx = 0x0;
    param3 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    param1 = _ctx;
    param2 = (u32)sp + 0x68;
    _ctx = 0x0;
    param3 = 0x1;
    r7 = 0x1;
    fn_801F1C18();
    param2 = 0x10000;
    param1 = 0xFFFF0000;
    tmp = tmp & 0xFFFF;
    var_r24 = param1 + 0x1;
    *(u16*)(sp + 0xC8) = var_r19;
    var_r16 = var_r24;
    var_r17 = 0xff;
    *(u32*)(sp + 0xD4) = tmp;
    var_r25 = 0x0;
    while (1) {
        _ctx = var_r25 & 0xFFFF;
        if (_ctx >= tmp) break;
        _ctx = (u32)sp + 0xa8;
        var_r23 = *(u32*)(_ctx + var_r27);
        if (var_r23 != 0 || var_r23 < 0 || (s32)var_r24 >= (s32)tmp) {
            _ctx = var_r23;
            fn_80205B2C();
            tmp = (s16)_ctx;

            _ctx = var_r23;
            param1 = 0x0;
            param2 = 0xd6;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r14 = _ctx;
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r14;
            fn_80203DAC();
            var_r14 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            var_r30 = 0x0;
            _ctx = (u32)sp + 0x54;
            param1 = var_r30;
            while (1) {
                tmp = param1 & 0xFFFF;
                if (tmp >= 0xa) break;
                /* addic. tmp, (u32)sp, 0x54 */;
                if (var_r23 != 0) {
                    *(u16*)(_ctx + tmp) = var_r30;
                }
                param1 = param1 + 0x1;

            }
            var_r20 = 0x0;
            while (1) {
                tmp = var_r20 & 0xFFFF;
                if (tmp >= 4) break;
                _ctx = var_r23;
                param1 = 0x0;
                param2 = 0xd6;
                param3 = 0x0;
                ((void(*)(void))fn_8012640C)();
                var_r18 = _ctx;
                _ctx = 0x0;
                param1 = 0x0;
                param2 = 0x14;
                param3 = 0x0;
                fn_801F54A4();
                _ctx = _ctx;
                param1 = 0x0;
                param2 = 0x43;
                param3 = 0x0;
                fn_801FB1C0();
                param1 = _ctx & 0xFFFF;
                _ctx = 0x0;
                param2 = 0x2;
                param3 = 0x0;
                fn_801FB1C0();
                _ctx = var_r18;
                fn_80205BE8();
                _ctx = var_r18;
                fn_80205BE8();
                param3 = var_r20 & 0xFF;
                param1 = 0x0;
                param2 = 0x7f;
                ((void(*)(void))fn_8012640C)();
                param1 = _ctx & 0xFFFF;
                if (tmp != 0xa) {
                    if (param1 == 0x165) {
                        if (param1 != 0x163) {
                        }
                        /* addic. tmp, (u32)sp, 0x54 */;
                        if (param1 != 0x163) {
                            _ctx = (u32)sp + 0x54;
                            *(u16*)(_ctx + tmp) = param1;
                        }
                        var_r30 = var_r30 + 0x1;
                    }
                        }
                var_r20 = var_r20 + 0x1;

            }
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r23;
            fn_80203A6C();
            _ctx = _ctx & 0xFFFF;
            tmp = var_r19 & 0xFFFF;
            if (tmp > _ctx) {
                var_r19 = _ctx;
            }
            _ctx = var_r23;
            param1 = 0x0;
            param2 = 0xd6;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80203E7C();
            param1 = var_r17 & 0xFF;
            tmp = _ctx & 0xFF;
            if (param1 > tmp) {
                var_r17 = _ctx;
            }
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = var_r14;
            _ctx = 0x0;
            param2 = 0x5;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx & 0xFFFF;
            param1 = var_r14;
            _ctx = 0x0;
            param2 = 0x7;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = *(u16*)(sp + 0xC8);
            var_r18 = var_r18 + param1;
            tmp = var_r18 & 0xFFFF;
            if (_ctx > tmp) {
                *(u16*)(sp + 0xC8) = var_r18;
            }
            _ctx = var_r23;
            param1 = 0x0;
            param2 = 0xd6;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r14 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r14;
            fn_80205BE8();
            param1 = 0x0;
            param2 = 0x6e;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x3;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r14 = (u32)sp + 0x34;
            param3 = _ctx & 0xFFFF;
            tmp = *(u32*)(var_r14 + var_r27);
            _ctx = var_r23;
            param1 = 0x0;
            param2 = 0xd6;
            tmp = tmp + param3;
            param3 = 0x0;
            *(u32*)(var_r14 + var_r27) = tmp;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80205BE8();
            param1 = 0x0;
            param2 = 0x6e;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x4;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            tmp = *(u32*)(var_r14 + var_r27);
            param2 = _ctx & 0xFFFF;
            _ctx = var_r23;
            param1 = 0x0;
            tmp = tmp + param2;
            param2 = 0xd6;
            *(u32*)(var_r14 + var_r27) = tmp;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80205BE8();
            param1 = 0x0;
            param2 = 0x6e;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x5;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            tmp = *(u32*)(var_r14 + var_r27);
            param2 = _ctx & 0xFFFF;
            _ctx = var_r23;
            param1 = 0x0;
            tmp = tmp + param2;
            param2 = 0xd6;
            *(u32*)(var_r14 + var_r27) = tmp;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80205BE8();
            param1 = 0x0;
            param2 = 0x6e;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x6;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            tmp = *(u32*)(var_r14 + var_r27);
            param2 = _ctx & 0xFFFF;
            _ctx = var_r23;
            param1 = 0x0;
            tmp = tmp + param2;
            param2 = 0xd6;
            *(u32*)(var_r14 + var_r27) = tmp;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80205BE8();
            param1 = 0x0;
            param2 = 0x6e;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x7;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            tmp = *(u32*)(var_r14 + var_r27);
            param2 = _ctx & 0xFFFF;
            _ctx = var_r23;
            param1 = 0x0;
            tmp = tmp + param2;
            param2 = 0xd6;
            *(u32*)(var_r14 + var_r27) = tmp;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = 0x0;
            param1 = 0x0;
            param2 = 0x14;
            param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80205BE8();
            param1 = 0x0;
            param2 = 0x6e;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x8;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            tmp = *(u32*)(var_r14 + var_r27);
            param2 = _ctx & 0xFFFF;
            _ctx = var_r23;
            param1 = 0x0;
            tmp = tmp + param2;
            param2 = 0xd6;
            *(u32*)(var_r14 + var_r27) = tmp;
            param3 = 0x0;
            ((void(*)(void))fn_8012640C)();
            var_r18 = _ctx;
            _ctx = _ctx;
            param1 = 0x0;
            param2 = 0x43;
            param3 = 0x0;
            fn_801FB1C0();
            param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            param2 = 0x2;
            param3 = 0x0;
            fn_801FB1C0();
            _ctx = var_r18;
            fn_80203E7C();
            param1 = *(u32*)(var_r14 + var_r27);
            tmp = _ctx & 0xFF;
            tmp = param1 * tmp;
            *(u32*)(var_r14 + var_r27) = tmp;
            tmp = *(u32*)(var_r14 + var_r27);
            if ((s32)var_r16 < (s32)tmp) {
                var_r16 = tmp;
            }
            var_r26 = (u32)sp + 0x88;
            var_r21 = 0x0;
            var_r14 = tmp & 0xFFFF;
            while (1) {
                tmp = var_r21 & 0xFFFF;
                if (tmp >= var_r14) break;
                _ctx = (u32)sp + 0x68;
                var_r22 = *(u32*)(_ctx + tmp);
                if (var_r22 != 0) {
                    var_r31 = var_r30 & 0xFFFF;
                    var_r20 = 0x0;
                    while (1) {
                        tmp = var_r20 & 0xFFFF;
                        if (tmp >= var_r31) break;
                        _ctx = (u32)sp + 0x54;
                        var_r18 = *(u16*)(_ctx + tmp);
                        do {
                        if (var_r18 == 0 || var_r18 == 0x165) break;

                        _ctx = _ctx;
                        param1 = 0x0;
                        param2 = 0x43;
                        param3 = 0x0;
                        fn_801FB1C0();
                        param1 = _ctx & 0xFFFF;
                        _ctx = 0x0;
                        param2 = 0x2;
                        param3 = 0x0;
                        fn_801FB1C0();
                        var_r29 = _ctx & 0xFFFF;
                        _ctx = 0x0;
                        param1 = 0x0;
                        ((void(*)(void))fn_801F453C)();
                        tmp = 0x0;
                        param2 = 0xd;
                        param1 = (u32)fn_80236268;
                        var_r28 = _ctx;
                        param2 = (u32)sp + 0x20;
                        *(u32*)(sp + 0x24) = tmp;
                        param1 = (u32)fn_80236268;
                        _ctx = 0x0;
                        param3 = 0x0;
                        *(u32*)(sp + 0x28) = tmp;
                        *(u32*)(sp + 0x2C) = tmp;
                        fn_801F37B0();
                        if (tmp != 0) {
                            var_r28 = 0x0;

                        } else {
                            tmp = 0x0;
                            param1 = 0x4d;
                            _ctx = (u32)fn_80236268;
                            param1 = (u32)fn_80236268;
                            param2 = (u32)sp + 0xc;
                            *(u32*)(sp + 0x10) = tmp;
                            _ctx = 0x0;
                            param3 = 0x0;
                            *(u32*)(sp + 0x14) = tmp;
                            *(u32*)(sp + 0x18) = tmp;
                            fn_801F37B0();
                            if (tmp != 0) {
                                var_r28 = 0x0;
                            }
                        }
                        param1 = var_r29;
                        _ctx = 0x0;
                        param2 = 0x2a;
                        param3 = 0x0;
                        fn_801FB1C0();
                        tmp = _ctx & 0xFF;
                        if (tmp == 1) {
                            if (var_r18 != 0xa5 && var_r18 != 0x164 && var_r18 != 0xf8) {

                                if (var_r18 == 0x161) {
                                }
                                tmp = 0x9;
                                goto L_8023536C;
                                }
                            if (var_r18 == 0xed) {
                                _ctx = var_r23;
                                fn_80205B8C();
                                param2 = (u32)sp + 0x8;
                                param1 = 0x0;
                                fn_80120B00();
                                tmp = *(u16*)(sp + 0x8);

                            } else if (var_r18 == 0x137) {
                                tmp = var_r28 & 0xFF;
                                if (tmp == 2) {
                                    tmp = 0xb;
                                    *(u16*)(sp + 0x8) = tmp;

                                } else if (tmp == 3) {
                                    tmp = 0x5;
                                    *(u16*)(sp + 0x8) = tmp;

                                } else if (tmp == 1) {
                                    tmp = 0xa;
                                    *(u16*)(sp + 0x8) = tmp;

                                } else if (tmp == 4) {
                                    tmp = 0xf;
                                    *(u16*)(sp + 0x8) = tmp;

                                } else {
                                    tmp = 0x0;
                                    *(u16*)(sp + 0x8) = tmp;
                                }
                                tmp = *(u16*)(sp + 0x8);

                            }
                            param1 = var_r18;
                            _ctx = 0x0;
                            param2 = 0x3;
                            param3 = 0x0;
                            fn_8011BEB4();
                            tmp = _ctx & 0xFFFF;

                        } else {
                            tmp = 0x9;
                        }
                    L_8023536C:
                        tmp = tmp & 0xFFFF;
                        if (tmp == 9) break;

                        _ctx = _ctx;
                        param1 = 0x0;
                        param2 = 0x43;
                        param3 = 0x0;
                        fn_801FB1C0();
                        param1 = _ctx & 0xFFFF;
                        _ctx = 0x0;
                        param2 = 0x2;
                        param3 = 0x0;
                        fn_801FB1C0();
                        _ctx = var_r18;
                        param1 = 0x1;
                        fn_8011BAC0();
                        tmp = _ctx & 0xFF;
                        if (tmp == 9) break;

                        _ctx = _ctx;
                        param1 = var_r23;
                        param2 = var_r18;
                        param3 = var_r22;
                        r7 = 0x0;
                        fn_8023C370();
                        tmp = *(u32*)(var_r26 + var_r27);
                        tmp = tmp + _ctx;
                        *(u32*)(var_r26 + var_r27) = tmp;
                        } while (0);

                        var_r20 = var_r20 + 0x1;

                    }
                }
                var_r21 = var_r21 + 0x1;

            }
            tmp = *(u32*)(var_r26 + var_r27);

            var_r24 = tmp;
        }
        var_r25 = var_r25 + 0x1;

    }
    var_r19 = var_r19 & 0xFFFF;
    var_r18 = (u32)sp + 0xa8;
    var_r21 = 0x0;
    var_r22 = tmp & 0xFFFF;
    while (1) {
        tmp = var_r21 & 0xFFFF;
        if (tmp >= var_r22) break;
        var_r26 = *(u32*)(var_r18 + var_r20);
        if (var_r26 != 0) {
            _ctx = var_r26;
            fn_80205B2C();
            var_r14 = _ctx;
            tmp = (s16)var_r14;
            if (var_r26 >= 0) {
                _ctx = var_r26;
                param1 = 0x0;
                param2 = 0xd6;
                param3 = 0x0;
                ((void(*)(void))fn_8012640C)();
                var_r23 = _ctx;
                _ctx = _ctx;
                param1 = 0x0;
                param2 = 0x43;
                param3 = 0x0;
                fn_801FB1C0();
                param1 = _ctx & 0xFFFF;
                _ctx = 0x0;
                param2 = 0x2;
                param3 = 0x0;
                fn_801FB1C0();
                _ctx = var_r23;
                fn_80203DAC();
                var_r25 = _ctx;
                _ctx = _ctx;
                param1 = 0x0;
                param2 = 0x43;
                param3 = 0x0;
                fn_801FB1C0();
                param1 = _ctx & 0xFFFF;
                _ctx = 0x0;
                param2 = 0x2;
                param3 = 0x0;
                fn_801FB1C0();
                _ctx = var_r26;
                fn_80203A6C();
                if ((s32)var_r19 >= (s32)_ctx) {
                    _ctx = _ctx;
                    param1 = var_r14 & 0xFFFF;
                    param2 = 0x52;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801FAA58();
                }
                _ctx = var_r26;
                param1 = 0x0;
                param2 = 0xd6;
                param3 = 0x0;
                ((void(*)(void))fn_8012640C)();
                var_r23 = _ctx;
                _ctx = _ctx;
                param1 = 0x0;
                param2 = 0x43;
                param3 = 0x0;
                fn_801FB1C0();
                param1 = _ctx & 0xFFFF;
                _ctx = 0x0;
                param2 = 0x2;
                param3 = 0x0;
                fn_801FB1C0();
                _ctx = var_r23;
                fn_80203E7C();
                param1 = var_r17 & 0xFF;
                tmp = _ctx & 0xFF;
                if (param1 >= tmp) {
                    _ctx = _ctx;
                    param1 = var_r14 & 0xFFFF;
                    param2 = 0x53;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801FAA58();
                }
                _ctx = _ctx;
                param1 = 0x0;
                param2 = 0x43;
                param3 = 0x0;
                fn_801FB1C0();
                param1 = _ctx & 0xFFFF;
                _ctx = 0x0;
                param2 = 0x2;
                param3 = 0x0;
                fn_801FB1C0();
                param1 = var_r25;
                _ctx = 0x0;
                param2 = 0x5;
                param3 = 0x0;
                ((void(*)(void))fn_8012640C)();
                var_r23 = _ctx & 0xFFFF;
                param1 = var_r25;
                _ctx = 0x0;
                param2 = 0x7;
                param3 = 0x0;
                ((void(*)(void))fn_8012640C)();
                param1 = _ctx & 0xFFFF;
                _ctx = *(u16*)(sp + 0xC8);
                var_r23 = var_r23 + param1;
                tmp = var_r23 & 0xFFFF;
                if (_ctx >= tmp) {
                    _ctx = _ctx;
                    param1 = var_r14 & 0xFFFF;
                    param2 = 0x54;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801FAA58();
                }
                _ctx = (u32)sp + 0x88;
                tmp = *(u32*)(_ctx + var_r20);
                if ((s32)var_r24 <= (s32)tmp) {
                    _ctx = _ctx;
                    param1 = var_r14 & 0xFFFF;
                    param2 = 0x55;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801FAA58();
                }
                _ctx = (u32)sp + 0x34;
                tmp = *(u32*)(_ctx + var_r20);
                if ((s32)var_r16 <= (s32)tmp) {
                    _ctx = _ctx;
                    param1 = var_r14 & 0xFFFF;
                    param2 = 0x56;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801FAA58();
        }
        }
        }
        var_r21 = var_r21 + 0x1;

    }
    return;
}
#pragma optimization_level 4

/* Address: 0x8023565C | Size: 0xb8 | Ghidra import */
u32 fn_8023565C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_8020156C();
  u16 uVar1;
  u16 uVar2;
  u32 uVar3;
  u16 uVar4;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar3 = fn_8020156C(r4);
  uVar4 = fn_801FB1C0(0,uVar1,0x2f,0);
  return (u8)((s32)(u8)uVar4 >= (s32)(uVar3 & 0xffff));
}
/* Address: 0x80235714 | Size: 0xb8 | Ghidra import */
u8 fn_80235714(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_8020156C();
  u16 uVar1;
  u16 uVar2;
  u32 uVar3;
  u16 uVar4;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar3 = fn_8020156C(r4);
  uVar4 = fn_801FB1C0(0,uVar1,0x2e,0);
  return (s32)(u8)uVar4 <= (s32)(u16)uVar3;
}
/* Address: 0x802357CC | Size: 0xe0 | Ghidra import */
u8 fn_802357CC(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_802026E4();
  u16 uVar1;
  u8 bVar2;
  u32 cVar3;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  bVar2 = (int)fn_8012640C(r4,0,0xec,0);
  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  if ((u8)fn_801FB1C0(0,uVar1,0x24,0) == 1) {
    cVar3 = fn_802026E4(r4,0x19);
  }
  else {
    cVar3 = 0;
  }
  if (((u8)cVar3 == 1) && (6 < bVar2)) {
    bVar2 = 6;
  }
  return bVar2;
}
/* Address: 0x802358AC | Size: 0x64 | Ghidra import */
u8 fn_802358AC(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = (int)fn_8012640C(r4,0,0xeb,0);
  return uVar2;
}
/* Address: 0x80235910 | Size: 0x64 | Ghidra import */
u8 fn_80235910(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = (int)fn_8012640C(r4,0,0xea,0);
  return uVar2;
}
/* Address: 0x80235974 | Size: 0x64 | Ghidra import */
u8 fn_80235974(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = (int)fn_8012640C(r4,0,0xe9,0);
  return uVar2;
}
/* Address: 0x802359D8 | Size: 0x64 | Ghidra import */
u8 fn_802359D8(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = (int)fn_8012640C(r4,0,0xe8,0);
  return uVar2;
}
/* Address: 0x80235A3C | Size: 0x64 | Ghidra import */
u8 fn_80235A3C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = (int)fn_8012640C(r4,0,0xe7,0);
  return uVar2;
}
/* Address: 0x80235AA0 | Size: 0x64 | Ghidra import */
u8 fn_80235AA0(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = (int)fn_8012640C(r4,0,0xe6,0);
  return uVar2;
}
/* Address: 0x80235B04 | Size: 0xe0 | Ghidra import */
u32 fn_80235B04(u32 r3, u32 r4, u8 r5)

{
    extern void fn_801F37B0();
    extern u8 fn_80236268[];
  u32 uVar1;
  u32 local_24[5];
  u32 local_38[5];

  uVar1 = (int)fn_801F453C(r4,0);
  if (r5 == 1) {
    local_24[0] = 0xd;
    local_24[1] = 0;
    local_24[2] = 0;
    local_24[3] = 0;
    local_24[4] = r3;
    fn_801F37B0(r4,fn_80236268,local_24,0);
    if (local_24[1] != 0) {
      return 0;
    }
    local_38[0] = 0x4d;
    local_38[1] = 0;
    local_38[2] = 0;
    local_38[3] = 0;
    local_38[4] = r3;
    fn_801F37B0(r4,fn_80236268,local_38,0);
    if (local_38[1] != 0) {
      return 0;
    }
  }
  return uVar1;
}
/* Address: 0x80235BE4 | Size: 0x684 | Ghidra import */
u32 fn_80235BE4(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    int *r6;

    extern void fn_801F37B0();
    extern u32 fn_801F54A4();
    extern s8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern s8 fn_802026E4();
    extern s8 fn_80206780();
    extern short fn_80207B8C();
  u32 bVar1;
  u32 bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  s8 cVar10;
  u16 uVar7;
  u16 uVar8;
  short sVar9;
  u32 uVar6;
  u32 local_68;
  int local_64;
  u32 local_60;
  u32 local_5c;
  u32 local_58;
  u32 local_54;
  int local_50;
  u32 local_4c;
  u32 local_48;
  u32 local_44;
  u32 local_40;
  int local_3c;
  u32 local_38;
  u32 local_34;
  u32 local_30;
  
  bVar1 = 0;
  cVar10 = fn_80206780(r5);
  if (cVar10 == 0) {
    return 0;
  }
  local_40 = 0x17;
  local_3c = 0;
  local_38 = 2;
  local_34 = r5;
  local_30 = r3;
  fn_801F37B0(r4,0x80236268,&local_40,0);
  iVar5 = local_3c;
  local_54 = 0x47;
  local_50 = 0;
  local_4c = 2;
  local_48 = r5;
  local_44 = r3;
  fn_801F37B0(r4,0x80236268,&local_54,0);
  iVar4 = local_50;
  local_68 = 0x2a;
  local_64 = 0;
  local_60 = 0;
  local_5c = r5;
  local_58 = r3;
  fn_801F37B0(r4,0x80236268,&local_68,0);
  iVar3 = local_64;
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  uVar7 = fn_801F54A4(0,0,0x14,0);
  uVar8 = fn_801FB1C0(r3,0,0x43,0);
  uVar8 = fn_801FB1C0(0,uVar8,2,0);
  cVar10 = fn_801FB1C0(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fn_801F8424(r3,r5,uVar7);
    if (cVar10 == 0) {
      sVar9 = fn_80207B8C(r5,0);
    }
    else {
      sVar9 = fn_80207B8C(r5,0);
    }
  }
  else {
    sVar9 = 9;
  }
  if (sVar9 == 2) {
LAB_00232e4c:
    bVar2 = 1;
  }
  else {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar8 = fn_801FB1C0(r3,0,0x43,0);
    uVar8 = fn_801FB1C0(0,uVar8,2,0);
    cVar10 = fn_801FB1C0(0,uVar8,0x2a,0);
    if (cVar10 == 1) {
      cVar10 = fn_801F8424(r3,r5,uVar7);
      if (cVar10 == 0) {
        sVar9 = fn_80207B8C(r5,1);
      }
      else {
        sVar9 = fn_80207B8C(r5,1);
      }
    }
    else {
      sVar9 = 9;
    }
    if (sVar9 == 2) goto LAB_00232e4c;
    bVar2 = 0;
  }
  if (bVar2) {
LAB_00232f10:
    bVar1 = 1;
  }
  else {
    uVar7 = fn_801FB1C0(r3,0,0x43,0);
    fn_801FB1C0(0,uVar7,2,0);
    fn_801F54A4(0,0,0x14,0);
    uVar7 = fn_801FB1C0(r3,0,0x43,0);
    uVar7 = fn_801FB1C0(0,uVar7,2,0);
    cVar10 = fn_801FB1C0(0,uVar7,0x2b,0);
    if (cVar10 == 1) {
      uVar6 = fn_80207BF4(r5);
    }
    else {
      uVar6 = 0;
    }
    uVar6 = __cntlzw(0x1a - (uVar6 & 0xffff));
    if ((uVar6 >> 5 & 0xff) == 1) goto LAB_00232f10;
  }
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  uVar7 = fn_801F54A4(0,0,0x14,0);
  uVar8 = fn_801FB1C0(r3,0,0x43,0);
  uVar8 = fn_801FB1C0(0,uVar8,2,0);
  cVar10 = fn_801FB1C0(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fn_801F8424(r3,r5,uVar7);
    if (cVar10 == 0) {
      sVar9 = fn_80207B8C(r5,0);
    }
    else {
      sVar9 = fn_80207B8C(r5,0);
    }
  }
  else {
    sVar9 = 9;
  }
  if (sVar9 != 8) {
    uVar7 = fn_801F54A4(0,0,0x14,0);
    uVar8 = fn_801FB1C0(r3,0,0x43,0);
    uVar8 = fn_801FB1C0(0,uVar8,2,0);
    cVar10 = fn_801FB1C0(0,uVar8,0x2a,0);
    if (cVar10 == 1) {
      cVar10 = fn_801F8424(r3,r5,uVar7);
      if (cVar10 == 0) {
        sVar9 = fn_80207B8C(r5,1);
      }
      else {
        sVar9 = fn_80207B8C(r5,1);
      }
    }
    else {
      sVar9 = 9;
    }
    if (sVar9 != 8) {
      bVar2 = 0;
      goto LAB_00233098;
    }
  }
  bVar2 = 1;
LAB_00233098:
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  uVar7 = fn_801FB1C0(0,uVar7,2,0);
  cVar10 = fn_801FB1C0(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(r5,0x16);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 != 1) {
    uVar7 = fn_801FB1C0(r3,0,0x43,0);
    uVar7 = fn_801FB1C0(0,uVar7,2,0);
    cVar10 = fn_801FB1C0(0,uVar7,0x24,0);
    if (cVar10 == 1) {
      cVar10 = fn_802026E4(r5,0xe);
    }
    else {
      cVar10 = 0;
    }
    if (cVar10 != 1) {
      uVar7 = fn_801FB1C0(r3,0,0x43,0);
      uVar7 = fn_801FB1C0(0,uVar7,2,0);
      cVar10 = fn_801FB1C0(0,uVar7,0x24,0);
      if (cVar10 == 1) {
        cVar10 = fn_802026E4(r5,0x25);
      }
      else {
        cVar10 = 0;
      }
      if (cVar10 != 1) {
        if (iVar5 != 0) {
          if (r6 != (void *)0) {
            *r6 = iVar5;
          }
          return 2;
        }
        if ((iVar4 != 0) && (!bVar1)) {
          if (r6 != (void *)0) {
            *r6 = iVar4;
          }
          return 2;
        }
        if ((iVar3 != 0) && (bVar2)) {
          if (r6 != (void *)0) {
            *r6 = iVar3;
          }
          return 2;
        }
        return 0;
      }
    }
  }
  return 1;
}
/* -------------------------------------------------------------------
 * Battle Flow Helpers (0x80236000-0x8023A000)
 * 64 functions
 * ------------------------------------------------------------------- */

/* Address: 0x80236268 | Size: 0x1f0 | Ghidra import */
u32 fn_80236268(void)

{
    u32 r3;
    u32 r4;
    u32 *r5;

    extern int fn_801F025C();
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern s8 fn_802062FC();
  u32 bVar1;
  u32 uVar2;
  s8 cVar6;
  u32 uVar3;
  int iVar4;
  u16 uVar5;
  int iVar7;
  u32 uVar8;
  u32 uVar9;
  
  uVar9 = r5[3];
  uVar8 = r5[4];
  cVar6 = fn_802062FC();
  if (cVar6 == 0) {
    uVar3 = 1;
  }
  else {
    if (uVar9 == 0) {
      iVar7 = 0;
    }
    else if (r5[2] == 1) {
      iVar7 = fn_801F025C(2,uVar9);
    }
    else if (r5[2] == 2) {
      iVar7 = fn_801F025C(3,uVar9);
    }
    else {
      iVar7 = 0;
    }
    iVar4 = fn_801F025C(2,r3);
    uVar2 = r5[2];
    if (((uVar2 == 1) || (uVar2 == 2)) && (iVar7 == 0)) {
      uVar3 = 1;
    }
    else {
      if (uVar2 == 0) {
        if ((uVar9 != 0) && (uVar9 == r3)) {
          return 1;
        }
      }
      else {
        if ((uVar2 != 1) && (uVar2 != 2)) {
          return 1;
        }
        if (iVar7 != iVar4) {
          return 1;
        }
      }
      uVar9 = *r5;
      uVar5 = fn_801FB1C0(uVar8,0,0x43,0);
      fn_801FB1C0(0,uVar5,2,0);
      if ((uVar9 & 0xffff) == 0) {
        bVar1 = 0;
      }
      else {
        fn_801F54A4(0,0,0x14,0);
        uVar5 = fn_801FB1C0(uVar8,0,0x43,0);
        uVar5 = fn_801FB1C0(0,uVar5,2,0);
        cVar6 = fn_801FB1C0(0,uVar5,0x2b,0);
        if (cVar6 == 1) {
          uVar8 = fn_80207BF4(r3);
        }
        else {
          uVar8 = 0;
        }
        if ((uVar9 & 0xffff) == (uVar8 & 0xffff)) {
          bVar1 = 1;
        }
        else {
          bVar1 = 0;
        }
      }
      if (bVar1) {
        r5[1] = r3;
        uVar3 = 0;
      }
      else {
        uVar3 = 1;
      }
    }
  }
  return uVar3;
}
/* Address: 0x80236458 | Size: 0x64 | Ghidra import */
u16 fn_80236458(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar1 = (int)fn_8012640C(r4,0,0xef,0);
  return uVar1;
}
/* Address: 0x802364BC | Size: 0x64 | Ghidra import */
u16 fn_802364BC(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar1 = (int)fn_8012640C(r4,0,0xf0,0);
  return uVar1;
}
/* Address: 0x80236520 | Size: 0x64 | Ghidra import */
u16 fn_80236520(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar1 = (int)fn_8012640C(r4,0,0xf1,0);
  return uVar1;
}
/* Address: 0x80236584 | Size: 0x248 (584 bytes) */
#pragma optimization_level 0
void fn_80236584(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_801F54A4();
    extern void fn_801FB1C0();
    extern void fn_801FFEC8();
    extern void fn_80205BE8();
    u8 sp[0x60];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 var_r23 = 0;
    u32 var_r24 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;
    u32 _param3 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;

    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    var_r29 = r7;
    _ctx = 0x0;
    _param1 = 0x0;
    _param2 = 0x14;
    _param3 = 0x0;
    fn_801F54A4();
    _ctx = _ctx;
    _param1 = 0x0;
    _param2 = 0x43;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2;
    _param3 = 0x0;
    fn_801FB1C0();
    var_r31 = 0x0;
    _param2 = (u32)sp + 0x1c;
    _param3 = var_r31;
    _ctx = (u32)sp + 0x8;
    _param1 = -0x1;
    while (1) {
        tmp = _param3 & 0xFFFF;
        if (tmp >= 0xa) break;
        /* addic. tmp, (u32)sp, 0x1c */;
        if ((s32)tmp != 0) {
            *(u16*)(_param2 + tmp) = var_r31;
        }
        /* addic. tmp, (u32)sp, 0x8 */;
        if ((s32)tmp != 0) {
            *(u16*)(_ctx + tmp) = _param1;
        }
        _param3 = _param3 + 0x1;

    }
    var_r30 = 0x0;
    while (1) {
    do {
        tmp = var_r30 & 0xFFFF;
        if (tmp >= 4) break;
        _ctx = _param1;
        _param1 = 0x0;
        _param2 = 0xd6;
        _param3 = 0x0;
        ((void(*)(void))fn_8012640C)();
        var_r24 = _ctx;
        _ctx = 0x0;
        _param1 = 0x0;
        _param2 = 0x14;
        _param3 = 0x0;
        fn_801F54A4();
        _ctx = _ctx;
        _param1 = 0x0;
        _param2 = 0x43;
        _param3 = 0x0;
        fn_801FB1C0();
        _param1 = _ctx & 0xFFFF;
        _ctx = 0x0;
        _param2 = 0x2;
        _param3 = 0x0;
        fn_801FB1C0();
        _ctx = var_r24;
        fn_80205BE8();
        _ctx = var_r24;
        fn_80205BE8();
        var_r23 = var_r30 & 0xFF;
        _param1 = 0x0;
        _param2 = 0x7f;
        _param3 = var_r23;
        ((void(*)(void))fn_8012640C)();
        var_r24 = _ctx & 0xFFFF;
        if (tmp == 0xa) break;
        if (var_r24 == 0x165) {
            if (var_r24 == 0x163) break;
        }
        tmp = var_r29 & 0xFF;
        if (tmp == 1) {
            _ctx = 0x0;
            _param1 = 0x0;
            _param2 = 0x14;
            _param3 = 0x0;
            fn_801F54A4();
            _ctx = _ctx;
            _param1 = 0x0;
            _param2 = 0x43;
            _param3 = 0x0;
            fn_801FB1C0();
            _param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            _param2 = 0x2;
            _param3 = 0x0;
            fn_801FB1C0();
            _ctx = _param1;
            _param1 = var_r23;
            _param2 = 0x0;
            _param3 = 0x0;
            fn_801FFEC8();
            tmp = _ctx & 0xFF;
            if (tmp != 1) break;
        }
        /* addic. tmp, (u32)sp, 0x1c */;
        if (tmp != 1) {
            _ctx = (u32)sp + 0x1c;
            *(u16*)(_ctx + tmp) = var_r24;
        }
        /* addic. tmp, (u32)sp, 0x8 */;
        if (tmp != 1) {
            _ctx = (u32)sp + 0x8;
            *(u16*)(_ctx + tmp) = var_r30;
        }
        var_r31 = var_r31 + 0x1;
    } while (0);
        var_r30 = var_r30 + 0x1;

    }
    _param2 = _param2 & 0xFFFF;
    _param1 = (u32)sp + 0x1c;
    tmp = var_r31 & 0xFFFF;
    r7 = 0x0;
    while (1) {
        _ctx = r7 & 0xFFFF;
        if (_ctx >= tmp) break;
        _ctx = *(u16*)(_param1 + _param3);
        if (_param2 == _ctx) {
            if (_param3 != 0) {
                _ctx = (u32)sp + 0x8;
                tmp = *(s16*)(_ctx + _param3);
                *(u16*)((u8*)_param3 + 0x0) = tmp;
            }
            _ctx = 0x1;
            return;
        }
        r7 = r7 + 0x1;

    }
    _ctx = 0x0;

    return;
}
#pragma optimization_level 4

/* Address: 0x802367CC | Size: 0x1ec | Ghidra import */
u32 fn_802367CC(u32 r3, u32 r4, int r5, int r6, char r7)
{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u8 fn_801FFEC8();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u32 uVar1;
  short sVar3;
  u8 cVar4;
  u16 uVar5;
  u32 uVar6;
  
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar6 = 0;
  for (uVar5 = 0; uVar5 < 10; uVar5 = uVar5 + 1) {
    if (r5 != 0) {
      *(u16 *)(r5 + (u32)uVar5 * 2) = 0;
    }
    if (r6 != 0) {
      *(u16 *)(r6 + (u32)uVar5 * 2) = 0xffff;
    }
  }
  uVar5 = 0;
  do {
    if (3 < uVar5) {
      return uVar6;
    }
    uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
    fn_801F54A4(0,0,0x14,0);
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    fn_801FB1C0(0,uVar2,2,0);
    fn_80205BE8(uVar1);
    uVar1 = fn_80205BE8(uVar1);
    sVar3 = (int)fn_8012640C(uVar1,0,0x7f,uVar5 & 0xff);
    if (sVar3 != 0) {
      if (r7 == 1) {
        fn_801F54A4(0,0,0x14,0);
        uVar2 = fn_801FB1C0(r3,0,0x43,0);
        fn_801FB1C0(0,uVar2,2,0);
        cVar4 = fn_801FFEC8(r4,uVar5 & 0xff,0,0);
        if (cVar4 != 0) goto LAB_00233990;
      }
      if (r5 != 0) {
        *(short *)(r5 + (uVar6 & 0xffff) * 2) = sVar3;
      }
      if (r6 != 0) {
        *(u16 *)(r6 + (uVar6 & 0xffff) * 2) = uVar5;
      }
      uVar6 = uVar6 + 1;
    }
LAB_00233990:
    uVar5 = uVar5 + 1;
  } while (1);
}
/* Address: 0x802369B8 | Size: 0x1e0 | Ghidra import */
u32 fn_802369B8(u32 r3, u32 r4, int r5, int r6, char r7)
{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u32 uVar1;
  short sVar3;
  s8 cVar4;
  u16 uVar5;
  u32 uVar6;
  
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar6 = 0;
  for (uVar5 = 0; uVar5 < 10; uVar5 = uVar5 + 1) {
    if (r5 != 0) {
      *(u16 *)(r5 + (u32)uVar5 * 2) = 0;
    }
    if (r6 != 0) {
      *(u16 *)(r6 + (u32)uVar5 * 2) = 0xffff;
    }
  }
  uVar5 = 0;
  do {
    if (3 < uVar5) {
      return uVar6;
    }
    fn_801F54A4(0,0,0x14,0);
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    fn_801FB1C0(0,uVar2,2,0);
    fn_80205BE8(r4);
    uVar1 = fn_80205BE8(r4);
    sVar3 = (int)fn_8012640C(uVar1,0,0x7f,uVar5 & 0xff);
    if (sVar3 != 0) {
      if (r7 == 1) {
        fn_801F54A4(0,0,0x14,0);
        uVar2 = fn_801FB1C0(r3,0,0x43,0);
        fn_801FB1C0(0,uVar2,2,0);
        fn_80205BE8(r4);
        uVar1 = fn_80205BE8(r4);
        cVar4 = (int)fn_8012640C(uVar1,0,0x80,uVar5 & 0xff);
        if (cVar4 == 0) goto LAB_00233b70;
      }
      if (r5 != 0) {
        *(short *)(r5 + (uVar6 & 0xffff) * 2) = sVar3;
      }
      if (r6 != 0) {
        *(u16 *)(r6 + (uVar6 & 0xffff) * 2) = uVar5;
      }
      uVar6 = uVar6 + 1;
    }
LAB_00233b70:
    uVar5 = uVar5 + 1;
  } while (1);
}
/* Address: 0x80236B98 | Size: 0x64 | Ghidra import */
u16 fn_80236B98(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar1 = (int)fn_8012640C(r4,0,0xfa,0);
  return uVar1;
}
/* Address: 0x80236BFC | Size: 0x84 | Ghidra import */
u32 fn_80236BFC(u32 param1, u32 param2, u32 param3)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_802026E4();
  u16 uVar2;
  u8 cVar3;
  u32 uVar1;

  uVar2 = fn_801FB1C0(param1,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  cVar3 = fn_801FB1C0(0,uVar2,0x24,0);
  if (cVar3 == 1) {
    uVar1 = fn_802026E4(param2,param3);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x80236C80 | Size: 0xe0 | Ghidra import */
u8 fn_80236C80(void)

{
    u32 r3;
    u32 r4;

    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;
  u16 uVar3;
  s8 cVar4;
  u8 uVar5;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar3 = (int)fn_8012640C(uVar1,0,0xc9,0);
  cVar4 = fn_801FB1C0(0,uVar2,0x33,0);
  if (cVar4 == 1) {
    uVar5 = fn_801FB1C0(0,uVar3,0x1d,0);
  }
  else {
    uVar5 = 1;
  }
  return uVar5;
}
/* Address: 0x80236D60 | Size: 0x13c | Ghidra import */
int fn_80236D60(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar4;
  u32 uVar2;
  u32 uVar3;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar4 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar4,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar4 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,uVar4,8,0);
  uVar1 = (int)fn_8012640C(r5,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar4 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar4,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar4 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar3 = (int)fn_8012640C(0,uVar4,8,0);
  return (uVar2 & 0xffff) - (uVar3 & 0xffff);
}
/* Address: 0x80236E9C | Size: 0xb0 | Ghidra import */
u16 fn_80236E9C(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,uVar2,8,0);
  return uVar2;
}
/* Address: 0x80236F4C | Size: 0xb0 | Ghidra import */
u16 fn_80236F4C(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,uVar2,7,0);
  return uVar2;
}
/* Address: 0x80236FFC | Size: 0xb0 | Ghidra import */
u16 fn_80236FFC(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,uVar2,6,0);
  return uVar2;
}
/* Address: 0x802370AC | Size: 0xb0 | Ghidra import */
u16 fn_802370AC(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,uVar2,5,0);
  return uVar2;
}
/* Address: 0x8023715C | Size: 0xb0 | Ghidra import */
u16 fn_8023715C(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x6e,0);
  uVar2 = (int)fn_8012640C(0,uVar2,4,0);
  return uVar2;
}
/* Address: 0x8023720C | Size: 0x7c | Ghidra import */
u32 fn_8023720C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80203C5C();
  u16 uVar2;
  u8 cVar3;
  u32 uVar1;

  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  cVar3 = fn_801FB1C0(0,uVar2,0x24,0);
  if (cVar3 == 1) {
    uVar1 = fn_80203C5C(r4);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x80237288 | Size: 0x88 | Ghidra import */
u32 fn_80237288(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern short fn_802010C8();
  u16 uVar1;
  u8 cVar3;
  u16 sVar2;
  u32 bVar4;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  cVar3 = fn_801FB1C0(0,uVar1,0x24,0);
  if (cVar3 == 1) {
    sVar2 = fn_802010C8(r4);
    bVar4 = sVar2 != 0;
  }
  else {
    bVar4 = 0;
  }
  return bVar4;
}
/* Address: 0x80237310 | Size: 0xa0 | Ghidra import */
u32 fn_80237310(u32 r3, u32 r4)

{
    extern u32 fn_80122DDC();
    extern u32 fn_801FB1C0();
    extern void fn_80205BE8();
  u32 uVar1;
  u16 uVar2;
  u8 cVar3;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  cVar3 = fn_801FB1C0(0,uVar2,0x24,0);
  if (cVar3 == 1) {
    fn_80205BE8(uVar1);
    uVar1 = fn_80122DDC();
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}
/* Address: 0x802373B0 | Size: 0x18c | Ghidra import */

u32 fn_802373B0(double r3,u32 r4,u32 r5,char r6)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
    extern f32 lbl_8047E618;
    extern f32 lbl_8047E61C;
    extern f64 lbl_8047E620;

  u32 uVar1;
  u16 uVar5;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;

  u64 in_f31;
  u8 auStack_8 [8];

  uVar1 = (int)fn_8012640C(r5,0,0xd6,0);
  uVar5 = fn_801FB1C0(r4,0,0x43,0);
  fn_801FB1C0(0,uVar5,2,0);
  uVar5 = fn_801FB1C0(r4,0,0x43,0);
  fn_801FB1C0(0,uVar5,2,0);
  uVar2 = fn_80205BE8(uVar1);
  uVar3 = (int)fn_8012640C(uVar2,0,0x83,0);
  uVar3 = uVar3 & 0xffff;
  uVar5 = fn_801FB1C0(r4,0,0x43,0);
  fn_801FB1C0(0,uVar5,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar4 = (int)fn_8012640C(uVar1,0,0x87,0);
  if ((double)lbl_8047E61C == r3) {
    r3 = (double)lbl_8047E618;
  }
  uVar4 = (u32)((double)(float)((double)((u64)(0x43300000) << 32 | (u32)(uVar4 & 0xffff)) -
                                lbl_8047E620) * r3);
  if (r6 == 0) {
    if (uVar3 == uVar4) {
      uVar1 = 1;
      goto LAB_00234520;
    }
  }
  else if (r6 < 1) {
    if ((r6 < 0) && ((int)uVar3 <= (int)uVar4)) {
      uVar1 = 1;
      goto LAB_00234520;
    }
  }
  else if ((int)uVar4 <= (int)uVar3) {
    uVar1 = 1;
    goto LAB_00234520;
  }
  uVar1 = 0;
LAB_00234520:

  return uVar1;
}
/* Address: 0x8023753C | Size: 0x128 | Ghidra import */
u32 fn_8023753C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
    extern u32 fn_8012640C();
  u16 uVar3;
  u32 uVar1;
  u16 uVar6;
  u16 uVar7;
  u32 uVar2;
  u16 uVar4;
  u8 uVar5;

  uVar1 = fn_8012640C(r4,0,0xd6,0);
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  uVar6 = fn_801FB1C0(0,uVar6,2,0);
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  uVar2 = fn_80205BE8(uVar1);
  uVar3 = fn_8012640C(uVar2,0,0x83,0);
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar4 = fn_8012640C(uVar1,0,0x87,0);
  uVar5 = fn_801FB1C0(0,uVar6,0x2c,0);
  return (int)((int)uVar3 * 100) / (int)uVar4 <= (int)uVar5;
}
/* Address: 0x80237664 | Size: 0x88 | Ghidra import */
u16 fn_80237664(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x87,0);
  return uVar2;
}
/* Address: 0x802376EC | Size: 0x88 | Ghidra import */
u16 fn_802376EC(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x83,0);
  return uVar2;
}
/* Address: 0x80237774 | Size: 0x74 | Ghidra import */
void fn_80237774(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern void fn_80203E7C();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  fn_80203E7C(uVar1);
  return;
}
/* Address: 0x802377E8 | Size: 0x74 | Ghidra import */
void fn_802377E8(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern void fn_80203DAC();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  fn_80203DAC(uVar1);
  return;
}
/* Address: 0x8023785C | Size: 0xe0 | Ghidra import */
u8 fn_8023785C(void)

{
    u32 r3;
    u32 r4;

    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;
  u16 uVar3;
  s8 cVar4;
  u8 uVar5;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar3 = (int)fn_8012640C(uVar1,0,0xc9,0);
  cVar4 = fn_801FB1C0(0,uVar2,0x23,0);
  if (cVar4 == 1) {
    uVar5 = fn_801FB1C0(0,uVar3,0x1c,0);
  }
  else {
    uVar5 = 0;
  }
  return uVar5;
}
/* Address: 0x8023793C | Size: 0x37c | Ghidra import */
u32 fn_8023793C(u32 r3, u32 r4, u16 r5, short r6)
{
    extern u32 fn_8010C650();
    extern u32 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern u8 fn_802026E4();
    extern u32 fn_80207B8C();
  u32 uVar1;
  u16 uVar2;
  u8 cVar5;
  u16 uVar3;
  u16 uVar4;
  u32 uVar6;
  u32 uVar7;
  u16 local_28 [6];
  
  if ((r5 & 0xffff) == 9) {
    uVar1 = 0x3f;
  }
  else {
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    fn_801FB1C0(0,uVar2,2,0);
    fn_801F54A4(0,0,0x14,0);
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    uVar2 = fn_801FB1C0(0,uVar2,2,0);
    cVar5 = fn_801FB1C0(0,uVar2,0x2b,0);
    if (cVar5 == 1) {
      uVar1 = fn_80207BF4(r4);
    }
    else {
      uVar1 = 0;
    }
    uVar1 = __cntlzw(0x1a - (uVar1 & 0xffff));
    if (((uVar1 >> 5 & 0xff) == 1) && ((r5 & 0xffff) == 4)) {
      uVar1 = 0x43;
    }
    else {
      uVar7 = 0;
      for (uVar1 = 0; (uVar1 & 0xff) < 2; uVar1 = uVar1 + 1) {
        uVar2 = fn_801F54A4(0,0,0x14,0);
        uVar3 = fn_801FB1C0(r3,0,0x43,0);
        uVar3 = fn_801FB1C0(0,uVar3,2,0);
        cVar5 = fn_801FB1C0(0,uVar3,0x2a,0);
        if (cVar5 == 1) {
          cVar5 = fn_801F8424(r3,r4,uVar2);
          if (cVar5 == 0) {
            uVar4 = fn_80207B8C(r4,uVar1);
          }
          else {
            uVar4 = fn_80207B8C(r4,uVar1);
          }
        }
        else {
          uVar4 = 9;
        }
        if (uVar4 != 9) {
          local_28[uVar7 & 0xffff] = uVar4;
          uVar7 = uVar7 + 1;
        }
      }
      if ((uVar7 & 0xffff) == 0) {
        uVar1 = 0x3f;
      }
      else {
        uVar1 = fn_8010C650(r5,local_28,uVar7);
        if (((r5 & 0xffff) == 0) || ((r5 & 0xffff) == 1)) {
          uVar2 = fn_801FB1C0(r3,0,0x43,0);
          uVar2 = fn_801FB1C0(0,uVar2,2,0);
          cVar5 = fn_801FB1C0(0,uVar2,0x24,0);
          if (cVar5 == 1) {
            cVar5 = fn_802026E4(r4,0x19);
          }
          else {
            cVar5 = 0;
          }
          if (cVar5 == 1) {
            for (uVar6 = 0; (uVar6 & 0xffff) < (uVar7 & 0xffff); uVar6 = uVar6 + 1) {
              if ((local_28[uVar6 & 0xffff] != 9) && (local_28[uVar6 & 0xffff] == 7)) {
                uVar1 = 0x3f;
              }
            }
          }
        }
        uVar2 = fn_801FB1C0(r3,0,0x43,0);
        fn_801FB1C0(0,uVar2,2,0);
        fn_801F54A4(0,0,0x14,0);
        uVar2 = fn_801FB1C0(r3,0,0x43,0);
        uVar2 = fn_801FB1C0(0,uVar2,2,0);
        cVar5 = fn_801FB1C0(0,uVar2,0x2b,0);
        if (cVar5 == 1) {
          uVar7 = fn_80207BF4(r4);
        }
        else {
          uVar7 = 0;
        }
        uVar7 = __cntlzw(0x19 - (uVar7 & 0xffff));
        if ((((uVar7 >> 5 & 0xff) == 1) && ((uVar1 & 0xffff) != 0x41)) && (0 < r6)) {
          uVar1 = 0x43;
        }
      }
    }
  }
  return uVar1;
}
/* Address: 0x80237CB8 | Size: 0x104 | Ghidra import */
u32 fn_80237CB8(u32 r3, u32 r4, int r5)

{
    extern u16 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u16 fn_801FB1C0();
    extern u16 fn_80207B8C();
  u32 uVar1;
  u32 uVar2;
  u8 cVar5;
  u16 sVar4;
  u32 uVar7;
  u32 uVar6;

  uVar7 = 0;
  for (uVar6 = 0; (uVar6 & 0xff) < 2; uVar6 = uVar6 + 1) {
    uVar2 = fn_801F54A4(0,0,0x14,0);
    cVar5 = fn_801FB1C0(0,fn_801FB1C0(0,fn_801FB1C0(r3,0,0x43,0),2,0),0x2a,0);
    if (cVar5 == 1) {
      cVar5 = fn_801F8424(r3,r4,uVar2);
      if (cVar5 == 0) {
        sVar4 = fn_80207B8C(r4,uVar6);
      }
      else {
        sVar4 = fn_80207B8C(r4,uVar6);
      }
    }
    else {
      sVar4 = 9;
    }
    if (sVar4 != 9) {
      uVar1 = uVar7 & 0xffff;
      uVar7 = uVar7 + 1;
      *(short *)(r5 + uVar1 * 2) = sVar4;
    }
  }
  return uVar7;
}
/* Address: 0x80237DBC | Size: 0x1b8 | Ghidra import */
u32 fn_80237DBC(u32 r3, u32 r4, u16 r5)

{
    extern u32 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern short fn_80207B8C();
  u16 uVar1;
  u16 uVar2;
  u8 cVar4;
  short sVar3;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar1 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  cVar4 = fn_801FB1C0(0,uVar2,0x2a,0);
  if (cVar4 == 1) {
    cVar4 = fn_801F8424(r3,r4,uVar1);
    if (cVar4 == 0) {
      sVar3 = fn_80207B8C(r4,0);
    }
    else {
      sVar3 = fn_80207B8C(r4,0);
    }
  }
  else {
    sVar3 = 9;
  }
  if (r5 != (u16)sVar3) {
    uVar1 = fn_801F54A4(0,0,0x14,0);
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    uVar2 = fn_801FB1C0(0,uVar2,2,0);
    cVar4 = fn_801FB1C0(0,uVar2,0x2a,0);
    if (cVar4 == 1) {
      cVar4 = fn_801F8424(r3,r4,uVar1);
      if (cVar4 == 0) {
        sVar3 = fn_80207B8C(r4,1);
      }
      else {
        sVar3 = fn_80207B8C(r4,1);
      }
    }
    else {
      sVar3 = 9;
    }
    if (r5 != (u16)sVar3) {
      goto _ret0;
    }
  }
  return 1;
_ret0:
  return 0;
}
/* Address: 0x80237F74 | Size: 0xec | Ghidra import */
u32 fn_80237F74(u32 r3, u32 r4, u16 r5)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u16 fn_80207BF4();
  u16 uVar2;
  u32 uVar1;
  u8 cVar4;
  u16 sVar3;

  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  if (r5 == 0) {
    uVar1 = 0;
  }
  else {
    fn_801F54A4(0,0,0x14,0);
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    uVar2 = fn_801FB1C0(0,uVar2,2,0);
    cVar4 = fn_801FB1C0(0,uVar2,0x2b,0);
    if (cVar4 == 1) {
      sVar3 = fn_80207BF4(r4);
    }
    else {
      sVar3 = 0;
    }
    if (r5 == sVar3) {
      uVar1 = 1;
    }
    else {
      uVar1 = 0;
    }
  }
  return uVar1;
}
/* Address: 0x80238060 | Size: 0x164 | Ghidra import */
u32 fn_80238060(void)

{
    u32 r3;
    u32 r4;
    u8 r5;

    extern u32 fn_80123E70();
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar6;
  u16 uVar7;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  
  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  uVar6 = fn_801FB1C0(0,uVar6,2,0);
  fn_801F54A4(0,0,0x14,0);
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  fn_80205BE8(uVar1);
  uVar2 = fn_80205BE8(uVar1);
  uVar3 = (int)fn_8012640C(uVar2,0,0x80,r5);
  fn_801F54A4(0,0,0x14,0);
  uVar7 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar7,2,0);
  fn_80205BE8(uVar1);
  uVar1 = fn_80205BE8(uVar1);
  uVar4 = fn_80123E70(uVar1,r5);
  uVar5 = fn_801FB1C0(0,uVar6,0x37,0);
  return ((uVar3 & 0xff) * 100) / (uVar4 & 0xff) <= (uVar5 & 0xff);
}
/* Address: 0x802381C4 | Size: 0xac | Ghidra import */
u8 fn_802381C4(u32 r3, u32 r4, u8 r5)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;
  u8 uVar3;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  fn_80205BE8(uVar1);
  uVar1 = fn_80205BE8(uVar1);
  uVar3 = (int)fn_8012640C(uVar1,0,0x80,r5);
  return uVar3;
}
/* Address: 0x80238270 | Size: 0xac | Ghidra import */
u16 fn_80238270(u32 r3, u32 r4, u8 r5)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  fn_80205BE8(uVar1);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x7f,r5);
  return uVar2;
}
/* Address: 0x8023831C | Size: 0x88 | Ghidra import */
void fn_8023831C(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void fn_802041EC();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  fn_802041EC(uVar1);
  return;
}
/* Address: 0x802383A4 | Size: 0x88 | Ghidra import */
void fn_802383A4(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void fn_802042E0();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  fn_802042E0(uVar1);
  return;
}
/* Address: 0x8023842C | Size: 0x88 | Ghidra import */
u16 fn_8023842C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar2;

  uVar1 = (int)fn_8012640C(r4,0,0xd6,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(uVar1);
  uVar2 = (int)fn_8012640C(uVar1,0,0x99,0);
  return uVar2;
}
/* Address: 0x802384B4 | Size: 0x84 | Ghidra import */
u32 fn_802384B4(u32 r3, u32 r4, u32 r5)

{
    extern u32 fn_801FB1C0();
    extern u32 fn_80202ADC();
  u16 uVar2;
  u8 cVar3;
  u32 uVar1;

  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  cVar3 = fn_801FB1C0(0,uVar2,0x24,0);
  if (cVar3 == 1) {
    uVar1 = fn_80202ADC(r4,r5);
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x80238538 | Size: 0xc8 | Ghidra import */
u8 fn_80238538(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u32 uVar1;
  u16 uVar3;
  u8 cVar4;
  u8 uVar5;

  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(r4);
  uVar3 = (int)fn_8012640C(uVar1,0,0xc9,0);
  cVar4 = fn_801FB1C0(0,uVar2,0x33,0);
  if (cVar4 == 1) {
    uVar5 = fn_801FB1C0(0,uVar3,0x1d,0);
  }
  else {
    uVar5 = 1;
  }
  return uVar5;
}
/* Address: 0x80238600 | Size: 0xc8 | Ghidra import */
u8 fn_80238600(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u32 uVar1;
  u16 uVar3;
  u8 cVar4;
  u8 uVar5;

  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(r4);
  uVar3 = (int)fn_8012640C(uVar1,0,0xc9,0);
  cVar4 = fn_801FB1C0(0,uVar2,0x23,0);
  if (cVar4 == 1) {
    uVar5 = fn_801FB1C0(0,uVar3,0x1c,0);
  }
  else {
    uVar5 = 0;
  }
  return uVar5;
}
/* Address: 0x802386C8 | Size: 0x80 | Ghidra import */
void fn_802386C8(u32 r3, u32 r4)

{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u32 uVar1;

  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar1 = fn_80205BE8(r4);
  fn_8012640C(uVar1,0,0x8c,0);
  return;
}
/* Address: 0x80238748 | Size: 0x80 | Ghidra import */
u32 fn_80238748(u32 r3, u32 r4)

{
    extern u32 fn_80122DDC();
    extern u32 fn_801FB1C0();
    extern void fn_80205BE8();
  u16 uVar2;
  u8 cVar3;
  u32 uVar1;

  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar2 = fn_801FB1C0(0,uVar2,2,0);
  cVar3 = fn_801FB1C0(0,uVar2,0x24,0);
  if (cVar3 == 1) {
    fn_80205BE8(r4);
    uVar1 = fn_80122DDC();
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}
/* Address: 0x802387C8 | Size: 0x54 | Ghidra import */
void fn_802387C8(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern void fn_80203A6C();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  fn_80203A6C(r4);
  return;
}
/* Address: 0x8023881C | Size: 0x110 | Ghidra import */
u32 fn_8023881C(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar5;
  u16 uVar6;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  
  uVar5 = fn_801FB1C0(r3,0,0x43,0);
  uVar5 = fn_801FB1C0(0,uVar5,2,0);
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar6,2,0);
  uVar1 = fn_80205BE8(r4);
  uVar2 = (int)fn_8012640C(uVar1,0,0x83,0);
  uVar6 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar6,2,0);
  uVar1 = fn_80205BE8(r4);
  uVar3 = (int)fn_8012640C(uVar1,0,0x87,0);
  uVar4 = fn_801FB1C0(0,uVar5,0x2c,0);
  return ((uVar2 & 0xffff) * 100) / (uVar3 & 0xffff) <= (uVar4 & 0xff);
}
/* Address: 0x8023892C | Size: 0x54 | Ghidra import */
void fn_8023892C(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern void fn_80203E7C();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  fn_80203E7C(r4);
  return;
}
/* Address: 0x80238980 | Size: 0x54 | Ghidra import */
void fn_80238980(u32 r3, u32 r4)

{
    extern u32 fn_801FB1C0();
    extern void fn_80203DAC();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  fn_80203DAC(r4);
  return;
}
/* Address: 0x802389D4 | Size: 0x138 | Ghidra import */
int fn_802389D4(u32 r3, u32 r4)
{
    extern int fn_8010C54C();
    extern u32 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar5;
  u16 uVar4;
  u16 uVar3;
  u32 uVar1;
  u8 cVar7;
  u16 sVar6;
  int iVar2;
  u8 bVar9;
  int iVar8;

  iVar8 = 0;
  for (bVar9 = 0; bVar9 < 2; bVar9 = bVar9 + 1) {
    uVar3 = fn_801F54A4(0,0,0x14,0);
    uVar4 = fn_801FB1C0(r3,0,0x43,0);
    uVar4 = fn_801FB1C0(0,uVar4,2,0);
    uVar1 = fn_80205BE8(r4);
    uVar5 = (int)fn_8012640C(uVar1,0,0x6e,0);
    cVar7 = fn_801FB1C0(0,uVar4,0x2a,0);
    if (cVar7 == 1) {
      cVar7 = fn_801F8424(r3,r4,uVar3);
      if (cVar7 == 0) {
        sVar6 = (int)fn_8012640C(0,uVar5,0x16,bVar9);
      }
      else {
        sVar6 = (int)fn_8012640C(0,uVar5,0x16,bVar9);
      }
    }
    else {
      sVar6 = 9;
    }
    if (sVar6 != 9) {
      iVar2 = fn_8010C54C(sVar6,0);
      iVar8 = iVar8 + iVar2;
    }
  }
  return iVar8;
}
/* Address: 0x80238B0C | Size: 0x324 | Ghidra import */
u32 fn_80238B0C(u32 r3, u32 r4, u16 r5, short r6)
{
    extern u32 fn_8010C650();
    extern u32 fn_801248C4();
    extern u32 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u16 uVar4;
  u32 uVar2;
  u8 cVar8;
  u16 uVar5;
  u16 uVar6;
  u16 uVar7;
  u32 uVar3;
  u8 bVar9;
  u16 local_38 [10];
  
  if ((r5 & 0xffff) == 9) {
    uVar1 = 0x3f;
  }
  else {
    uVar4 = fn_801FB1C0(r3,0,0x43,0);
    fn_801FB1C0(0,uVar4,2,0);
    fn_801F54A4(0,0,0x14,0);
    uVar4 = fn_801FB1C0(r3,0,0x43,0);
    uVar4 = fn_801FB1C0(0,uVar4,2,0);
    uVar2 = fn_80205BE8(r4);
    cVar8 = fn_801FB1C0(0,uVar4,0x2b,0);
    if (cVar8 == 1) {
      uVar1 = fn_801248C4(uVar2);
    }
    else {
      uVar1 = 0;
    }
    uVar1 = __cntlzw(0x1a - (uVar1 & 0xffff));
    if (((uVar1 >> 5 & 0xff) == 1) && ((r5 & 0xffff) == 4)) {
      uVar1 = 0x43;
    }
    else {
      uVar1 = 0;
      for (bVar9 = 0; bVar9 < 2; bVar9 = bVar9 + 1) {
        uVar4 = fn_801F54A4(0,0,0x14,0);
        uVar5 = fn_801FB1C0(r3,0,0x43,0);
        uVar5 = fn_801FB1C0(0,uVar5,2,0);
        uVar2 = fn_80205BE8(r4);
        uVar6 = (int)fn_8012640C(uVar2,0,0x6e,0);
        cVar8 = fn_801FB1C0(0,uVar5,0x2a,0);
        if (cVar8 == 1) {
          cVar8 = fn_801F8424(r3,r4,uVar4);
          if (cVar8 == 0) {
            uVar7 = (int)fn_8012640C(0,uVar6,0x16,bVar9);
          }
          else {
            uVar7 = (int)fn_8012640C(0,uVar6,0x16,bVar9);
          }
        }
        else {
          uVar7 = 9;
        }
        if (uVar7 != 9) {
          local_38[uVar1 & 0xffff] = uVar7;
          uVar1 = uVar1 + 1;
        }
      }
      if ((uVar1 & 0xffff) == 0) {
        uVar1 = 0x3f;
      }
      else {
        uVar1 = fn_8010C650(r5,local_38,uVar1);
        uVar4 = fn_801FB1C0(r3,0,0x43,0);
        fn_801FB1C0(0,uVar4,2,0);
        fn_801F54A4(0,0,0x14,0);
        uVar4 = fn_801FB1C0(r3,0,0x43,0);
        uVar4 = fn_801FB1C0(0,uVar4,2,0);
        uVar2 = fn_80205BE8(r4);
        cVar8 = fn_801FB1C0(0,uVar4,0x2b,0);
        if (cVar8 == 1) {
          uVar3 = fn_801248C4(uVar2);
        }
        else {
          uVar3 = 0;
        }
        uVar3 = __cntlzw(0x19 - (uVar3 & 0xffff));
        if ((((uVar3 >> 5 & 0xff) == 1) && ((uVar1 & 0xffff) != 0x41)) && (0 < r6)) {
          uVar1 = 0x43;
        }
      }
    }
  }
  return uVar1;
}
/* Address: 0x80238E30 | Size: 0x228 | Ghidra import */
u32 fn_80238E30(void)

{
    u32 r3;
    u32 r4;
    short r5;

    extern u32 fn_801F54A4();
    extern s8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u16 uVar3;
  u32 uVar1;
  u16 uVar4;
  s8 cVar6;
  short sVar5;
  
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  uVar2 = fn_801F54A4(0,0,0x14,0);
  uVar3 = fn_801FB1C0(r3,0,0x43,0);
  uVar3 = fn_801FB1C0(0,uVar3,2,0);
  uVar1 = fn_80205BE8(r4);
  uVar4 = (int)fn_8012640C(uVar1,0,0x6e,0);
  cVar6 = fn_801FB1C0(0,uVar3,0x2a,0);
  if (cVar6 == 1) {
    cVar6 = fn_801F8424(r3,r4,uVar2);
    if (cVar6 == 0) {
      sVar5 = (int)fn_8012640C(0,uVar4,0x16,0);
    }
    else {
      sVar5 = (int)fn_8012640C(0,uVar4,0x16,0);
    }
  }
  else {
    sVar5 = 9;
  }
  if (r5 != sVar5) {
    uVar2 = fn_801F54A4(0,0,0x14,0);
    uVar3 = fn_801FB1C0(r3,0,0x43,0);
    uVar3 = fn_801FB1C0(0,uVar3,2,0);
    uVar1 = fn_80205BE8(r4);
    uVar4 = (int)fn_8012640C(uVar1,0,0x6e,0);
    cVar6 = fn_801FB1C0(0,uVar3,0x2a,0);
    if (cVar6 == 1) {
      cVar6 = fn_801F8424(r3,r4,uVar2);
      if (cVar6 == 0) {
        sVar5 = (int)fn_8012640C(0,uVar4,0x16,1);
      }
      else {
        sVar5 = (int)fn_8012640C(0,uVar4,0x16,1);
      }
    }
    else {
      sVar5 = 9;
    }
    if (r5 != sVar5) {
      return 0;
    }
  }
  return 1;
}
/* Address: 0x80239058 | Size: 0xfc | Ghidra import */
u32 fn_80239058(u32 r3, u32 r4, u16 r5)

{
    extern u16 fn_801248C4();
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar2;
  u32 uVar1;
  u8 cVar4;
  u16 sVar3;

  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  if (r5 == 0) {
    uVar1 = 0;
  }
  else {
    fn_801F54A4(0,0,0x14,0);
    uVar2 = fn_801FB1C0(r3,0,0x43,0);
    uVar2 = fn_801FB1C0(0,uVar2,2,0);
    uVar1 = fn_80205BE8(r4);
    cVar4 = fn_801FB1C0(0,uVar2,0x2b,0);
    if (cVar4 == 1) {
      sVar3 = fn_801248C4(uVar1);
    }
    else {
      sVar3 = 0;
    }
    if (r5 == sVar3) {
      uVar1 = 1;
    }
    else {
      uVar1 = 0;
    }
  }
  return uVar1;
}
/* Address: 0x80239154 | Size: 0x8c | Ghidra import */
u8 fn_80239154(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 cVar2;
  u8 uVar3;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  cVar2 = fn_801FB1C0(0,uVar1,0x36,0);
  if (cVar2 == 1) {
    uVar3 = fn_8011BEB4(0,r4,0x1b,0);
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}
/* Address: 0x802391E0 | Size: 0x64 | Ghidra import */
u8 fn_802391E0(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_8011BEB4(0,r4,2,0);
  return uVar2;
}
/* Address: 0x80239244 | Size: 0x64 | Ghidra import */
u8 fn_80239244(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_8011BEB4(0,r4,5,0);
  return uVar2;
}
/* Address: 0x802392A8 | Size: 0xf8 | Ghidra import */
char fn_802392A8()

{
    u32 r3;
    u32 r4;

    extern int fn_8011BEB4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80229B70();
    extern s8 fn_80229BD8();
  u16 uVar1;
  u16 uVar2;
  s8 cVar3;
  u8 bVar4;
  u8 bVar5;
  
  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar2,2,0);
  cVar3 = fn_80229B70(r4);
  if (cVar3 == 1) {
    bVar4 = 100;
  }
  else {
    cVar3 = fn_80229BD8(r4);
    if (cVar3 == 1) {
      bVar4 = 100;
    }
    else {
      bVar4 = fn_8011BEB4(0,r4,6,0);
    }
  }
  bVar5 = fn_801FB1C0(0,uVar1,0x35,0);
  return -((bVar4 < bVar5) + -1);
}
/* Address: 0x802393A0 | Size: 0x9c | Ghidra import */
u8 fn_802393A0(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801FB1C0();
    extern u8 fn_80229B70();
    extern u8 fn_80229BD8();
  u16 uVar1;
  u8 cVar2;
  u8 uVar3;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  cVar2 = fn_80229B70(r4);
  if (cVar2 == 1) {
    uVar3 = 100;
  }
  else {
    cVar2 = fn_80229BD8(r4);
    if (cVar2 == 1) {
      uVar3 = 100;
    }
    else {
      uVar3 = fn_8011BEB4(0,r4,6,0);
    }
  }
  return uVar3;
}
/* Address: 0x8023943C | Size: 0x5c | Ghidra import */
void fn_8023943C(u32 r3, u32 r4, u32 r5)

{
    extern void fn_8011BAC0();
    extern u32 fn_801FB1C0();
  u16 uVar1;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  fn_8011BAC0(r4,r5);
  return;
}
/* Address: 0x80239498 | Size: 0x68 | Ghidra import */
u8 fn_80239498(u32 r3, u32 r4, u8 r5)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_8011BEB4(0,r4,0x1a,r5);
  return uVar2;
}
/* Address: 0x80239500 | Size: 0x64 | Ghidra import */
int fn_80239500(u32 r3, u32 r4)

{
    extern short fn_8011BEB4();
    extern u32 fn_801FB1C0();
  u16 uVar1;
  short sVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  sVar2 = fn_8011BEB4(0,r4,7,0);
  return (int)sVar2;
}
/* Address: 0x80239564 | Size: 0x64 | Ghidra import */
u8 fn_80239564(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801FB1C0();
  u16 uVar1;
  u8 uVar2;

  uVar1 = fn_801FB1C0(r3,0,0x43,0);
  fn_801FB1C0(0,uVar1,2,0);
  uVar2 = fn_8011BEB4(0,r4,0xc,0);
  return uVar2;
}
/* Address: 0x802395C8 | Size: 0x1F0 (496 bytes) */
#pragma optimization_level 0
void fn_802395C8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8011BEB4();
    extern void fn_80120B00();
    extern void fn_801F37B0();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236268();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 _ctx = 0;
    u32 var_r28 = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;

    param3 = 0x0;
    _param1 = _param1;
    _param2 = _param2;
    _ctx = _ctx;
    _param1 = 0x0;
    _param2 = 0x43;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2;
    param3 = 0x0;
    fn_801FB1C0();
    var_r28 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param1 = 0x0;
    ((void(*)(void))fn_801F453C)();
    tmp = 0x0;
    _param2 = 0xd;
    _param1 = (u32)fn_80236268;
    _ctx = _ctx;
    _param2 = (u32)sp + 0x20;
    *(u32*)(sp + 0x24) = tmp;
    _param1 = (u32)fn_80236268;
    _ctx = 0x0;
    param3 = 0x0;
    *(u32*)(sp + 0x28) = tmp;
    *(u32*)(sp + 0x2C) = tmp;
    fn_801F37B0();
    if (tmp != 0) {
        _ctx = 0x0;

    } else {
        tmp = 0x0;
        _param1 = 0x4d;
        _ctx = (u32)fn_80236268;
        _param1 = (u32)fn_80236268;
        _param2 = (u32)sp + 0xc;
        *(u32*)(sp + 0x10) = tmp;
        _ctx = 0x0;
        param3 = 0x0;
        *(u32*)(sp + 0x14) = tmp;
        *(u32*)(sp + 0x18) = tmp;
        fn_801F37B0();
        if (tmp != 0) {
            _ctx = 0x0;
        }
    }
    _param1 = var_r28;
    _ctx = 0x0;
    _param2 = 0x2a;
    param3 = 0x0;
    fn_801FB1C0();
    tmp = _ctx & 0xFF;
    if (tmp != 1) { _ctx = 0x9; return; }
    tmp = _param1 & 0xFFFF;
    if (tmp == 0xa5) { _ctx = 0x9; return; }
    if (tmp == 0x164) { _ctx = 0x9; return; }
    if (tmp == 0xf8) { _ctx = 0x9; return; }
    if (tmp == 0x161) {

        _ctx = 0x9;
        return;
    }
    if (tmp == 0xed) {
        _ctx = _param2;
        fn_80205B8C();
        _param2 = (u32)sp + 0x8;
        _param1 = 0x0;
        fn_80120B00();
        _ctx = *(u16*)(sp + 0x8);
        return;
    }
    if (tmp == 0x137) {
        tmp = _ctx & 0xFF;
        if (tmp == 2) {
            tmp = 0xb;
            *(u16*)(sp + 0x8) = tmp;
            _ctx = *(u16*)(sp + 0x8);
            return;
        }
        if (tmp == 3) {
            tmp = 0x5;
            *(u16*)(sp + 0x8) = tmp;
            _ctx = *(u16*)(sp + 0x8);
            return;
        }
        if (tmp == 1) {
            tmp = 0xa;
            *(u16*)(sp + 0x8) = tmp;
            _ctx = *(u16*)(sp + 0x8);
            return;
        }
        if (tmp == 4) {
            tmp = 0xf;
            *(u16*)(sp + 0x8) = tmp;
            _ctx = *(u16*)(sp + 0x8);
            return;
        }
        tmp = 0x0;
        *(u16*)(sp + 0x8) = tmp;

        _ctx = *(u16*)(sp + 0x8);
        return;
    }
    _param1 = _param1;
    _ctx = 0x0;
    _param2 = 0x3;
    param3 = 0x0;
    fn_8011BEB4();
    _ctx = _ctx & 0xFFFF;
    return;

    _ctx = 0x9;

    return;
}
#pragma optimization_level 4

/* Address: 0x802397B8 | Size: 0x12c | Ghidra import */
u32 fn_802397B8(void)

{
    int r3;
    u16 r4;
    char r5;

    extern u32 fn_800E0C54();
  u32 uVar1;
  int iVar2;
  u16 uVar3;
  int iVar4;
  u16 uVar5;
  u32 local_48 [17];
  
  iVar4 = -200;
  uVar5 = 0;
  for (uVar3 = 0; uVar3 < 0x10; uVar3 = uVar3 + 1) {
    local_48[uVar3] = 0xffffffff;
  }
  for (uVar3 = 0; uVar3 < r4; uVar3 = uVar3 + 1) {
    iVar2 = *(int *)(r3 + (u32)uVar3 * 4);
    if (iVar4 < iVar2) {
      iVar4 = iVar2;
    }
  }
  uVar3 = 0;
  while (1) {
    if (r4 <= uVar3) break;
    if (iVar4 <= *(int *)(r3 + uVar3 * 4)) {
      uVar1 = (u32)uVar5;
      uVar5 = uVar5 + 1;
      local_48[uVar1] = (u32)uVar3;
    }
    if (0xf < uVar5) break;
    uVar3 = uVar3 + 1;
  }
  if (uVar5 == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    if (uVar5 == 1) {
      uVar1 = 0;
    }
    else if (r5 == 1) {
      uVar1 = fn_800E0C54();
      uVar1 = (uVar1 & 0xffff) - ((uVar1 & 0xffff) / (u32)uVar5) * (u32)uVar5 & 0xffff;
    }
    else {
      uVar1 = 0;
    }
    uVar1 = local_48[uVar1];
  }
  return uVar1;
}
/* Address: 0x802398E4 | Size: 0x9c | Ghidra import */
int fn_802398E4(void)

{
    int r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern int fn_801FB1C0();
  int iVar1;
  
  iVar1 = fn_801FB1C0(0,r6,0x3e,0);
  iVar1 = (int)(short)((((short)(((r4 & 0xff) * 100) / 0xff) + -0x32) * iVar1) / 0x32);
  if (iVar1 < 1) {
    if ((iVar1 < 0) && (r3 = r3 + iVar1, r3 < -200)) {
      r3 = -200;
    }
  }
  else {
    r3 = r3 + iVar1;
    if (200 < r3) {
      r3 = 200;
    }
  }
  return r3;
}
/* Address: 0x802399FC | Size: 0x44 | Ghidra import */
int fn_802399FC(void)

{
    int r3;
    int r4;

  if (0 < r4) {
    if (200 < r3 + r4) {
      return 200;
    }
    return r3 + r4;
  }
  if (-1 < r4) {
    return r3;
  }
  if (r3 + r4 < -200) {
    return -200;
  }
  return r3 + r4;
}
/* Address: 0x80239A40 | Size: 0x28c | Ghidra import */
u32
fn_80239A40(void)

{
    u32 r3;
    int r4;
    int r5;
    int r6;
    int r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u8 param_9;

    extern s8 fn_80008164();
    extern u32 fn_800FA280();
    extern void fn_80103BA8();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern int fn_801FB1C0();
    extern void fn_8026246C();
    extern s8 fn_802624CC();
  int iVar1;
  int iVar2;
  int iVar3;
  u32 uVar4;
  s8 cVar5;
  int iVar6;
  u16 local_58 [22];
  
  iVar1 = fn_801FB1C0(0,r10,0x40,0);
  iVar2 = fn_801FB1C0(0,r10,0x41,0);
  iVar3 = fn_801FB1C0(0,r10,0x3e,0);
  iVar3 = (int)(short)((((short)(((u32)param_9 * 100) / 0xff) + -0x32) * iVar3) / 0x32);
  if (iVar3 < 1) {
    iVar6 = 0;
    if ((iVar3 < 0) && (iVar6 = iVar3, iVar3 < -200)) {
      iVar6 = -200;
    }
  }
  else {
    iVar6 = iVar3;
    if (200 < iVar3) {
      iVar6 = 200;
    }
  }
  if (iVar1 != 0) {
    uVar4 = fn_800FA280(iVar1);
    fn_80132A38(0xd,uVar4);
  }
  if (iVar2 != 0) {
    uVar4 = fn_800FA280(iVar2);
    fn_80132A38(0xe,uVar4);
  }
  if (r4 != 0) {
    uVar4 = fn_801F8100(r4);
    fn_80132A38(0x13,uVar4);
  }
  if (r5 != 0) {
    uVar4 = (int)fn_8012640C(r5,0,0x77,0);
    fn_80132A38(0x14,uVar4);
  }
  if (r6 != 0) {
    uVar4 = fn_801F8100(r6);
    fn_80132A38(0x23,uVar4);
  }
  if (r7 != 0) {
    uVar4 = (int)fn_8012640C(r7,0,0x77,0);
    fn_80132A38(0x15,uVar4);
  }
  if ((r8 & 0xffff) != 0) {
    fn_8011BEB4(0,r8,1,0);
    uVar4 = fn_800FA280();
    fn_80132A38(0x28,uVar4);
  }
  if ((r9 & 0xffff) != 0) {
    itemGetStatus(0,r9,1,0);
    uVar4 = fn_800FA280();
    fn_80132A38(0x29,uVar4);
  }
  if ((r10 & 0xffff) != 0) {
    fn_801FB1C0(0,r10,0x3f,0);
    uVar4 = fn_800FA280();
    fn_80132A38(0x41,uVar4);
  }
  fn_80132A38(0x2f,iVar6);
  cVar5 = fn_80008164();
  if (((cVar5 == 1) && (fn_80103BA8(local_58,1), (local_58[0] & 0x800) == 0)) &&
     (cVar5 = fn_802624CC(r3), cVar5 == 1)) {
    fn_8026246C();
  }
  return 0;
}
/* Address: 0x80239CCC | Size: 0x21c | Ghidra import */
u32
fn_80239CCC(void)

{
    u32 r3;
    int r4;
    int r5;
    int r6;
    int r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 param_9;

    extern s8 fn_80008164();
    extern u32 fn_800FA280();
    extern void fn_80103BA8();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern int fn_801FB1C0();
    extern void fn_8026246C();
    extern s8 fn_802624CC();
  int iVar1;
  int iVar2;
  u32 uVar3;
  s8 cVar4;
  u16 local_58 [22];
  
  iVar1 = fn_801FB1C0(0,r10,0x40,0);
  iVar2 = fn_801FB1C0(0,r10,0x41,0);
  if (iVar1 != 0) {
    uVar3 = fn_800FA280(iVar1);
    fn_80132A38(0xd,uVar3);
  }
  if (iVar2 != 0) {
    uVar3 = fn_800FA280(iVar2);
    fn_80132A38(0xe,uVar3);
  }
  if (r4 != 0) {
    uVar3 = fn_801F8100(r4);
    fn_80132A38(0x13,uVar3);
  }
  if (r5 != 0) {
    uVar3 = (int)fn_8012640C(r5,0,0x77,0);
    fn_80132A38(0x14,uVar3);
  }
  if (r6 != 0) {
    uVar3 = fn_801F8100(r6);
    fn_80132A38(0x23,uVar3);
  }
  if (r7 != 0) {
    uVar3 = (int)fn_8012640C(r7,0,0x77,0);
    fn_80132A38(0x15,uVar3);
  }
  if ((r8 & 0xffff) != 0) {
    fn_8011BEB4(0,r8,1,0);
    uVar3 = fn_800FA280();
    fn_80132A38(0x28,uVar3);
  }
  if ((r9 & 0xffff) != 0) {
    itemGetStatus(0,r9,1,0);
    uVar3 = fn_800FA280();
    fn_80132A38(0x29,uVar3);
  }
  if ((r10 & 0xffff) != 0) {
    fn_801FB1C0(0,r10,0x3f,0);
    uVar3 = fn_800FA280();
    fn_80132A38(0x41,uVar3);
  }
  fn_80132A38(0x2f,param_9);
  cVar4 = fn_80008164();
  if (((cVar4 == 1) && (fn_80103BA8(local_58,1), (local_58[0] & 0x800) == 0)) &&
     (cVar4 = fn_802624CC(r3), cVar4 == 1)) {
    fn_8026246C();
  }
  return 0;
}
/* -------------------------------------------------------------------
 * Battle Sequence & Pre-Battle (0x8023A000-0x80240000)
 * 35 functions
 * ------------------------------------------------------------------- */

/* Address: 0x8023A118 | Size: 0x1f0 | Ghidra import */
u32
fn_8023A118(void)

{
    u32 r3;
    int r4;
    int r5;
    int r6;
    int r7;
    int r8;
    int r9;
    u32 r10;
    short param_9;
    short param_10;
    u32 param_11;

    extern s8 fn_80008164();
    extern u32 fn_800FA280();
    extern void fn_80103BA8();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern void fn_801FB1C0();
    extern void fn_8026246C();
    extern s8 fn_802624CC();
  u32 uVar1;
  s8 cVar2;
  u16 local_48 [16];
  
  if (r4 != 0) {
    uVar1 = fn_800FA280(r4);
    fn_80132A38(0xd,uVar1);
  }
  if (r5 != 0) {
    uVar1 = fn_800FA280(r5);
    fn_80132A38(0xe,uVar1);
  }
  if (r6 != 0) {
    uVar1 = fn_801F8100(r6);
    fn_80132A38(0x13,uVar1);
  }
  if (r7 != 0) {
    uVar1 = (int)fn_8012640C(r7,0,0x77,0);
    fn_80132A38(0x14,uVar1);
  }
  if (r8 != 0) {
    uVar1 = fn_801F8100(r8);
    fn_80132A38(0x23,uVar1);
  }
  if (r9 != 0) {
    uVar1 = (int)fn_8012640C(r9,0,0x77,0);
    fn_80132A38(0x15,uVar1);
  }
  if ((r10 & 0xffff) != 0) {
    fn_8011BEB4(0,r10,1,0);
    uVar1 = fn_800FA280();
    fn_80132A38(0x28,uVar1);
  }
  if (param_9 != 0) {
    itemGetStatus(0,param_9,1,0);
    uVar1 = fn_800FA280();
    fn_80132A38(0x29,uVar1);
  }
  if (param_10 != 0) {
    fn_801FB1C0(0,param_10,0x3f,0);
    uVar1 = fn_800FA280();
    fn_80132A38(0x41,uVar1);
  }
  fn_80132A38(0x2f,param_11);
  cVar2 = fn_80008164();
  if (((cVar2 == 1) && (fn_80103BA8(local_48,1), (local_48[0] & 0x800) == 0)) &&
     (cVar2 = fn_802624CC(r3), cVar2 == 1)) {
    fn_8026246C();
  }
  return 0;
}
/* Address: 0x8023A308 | Size: 0x438 | Ghidra import */
u32 fn_8023A308(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern u32 fn_800E0C54();
    extern u32 fn_801F0134();
    extern u32 fn_801F1C18();
    extern u32 fn_801F4354();
    extern u32 fn_801FB1C0();
    extern void fn_80204F6C();
    extern u32 fn_80205B8C();
    extern u32 fn_8022B2CC();
    extern u32 fn_802367CC();
    extern u16 fn_80238270();
    extern u32 fn_8023A118();
    extern int fn_8023A740();
    extern int fn_8023B498();
  short sVar1;
  short sVar2;
  u16 uVar12;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  s8 cVar13;
  u32 uVar6;
  int iVar7;
  u32 uVar8;
  u32 uVar9;
  u32 uVar10;
  int iVar11;
  u16 local_68 [2];
  int local_64;
  int aiStack_60 [8];
  short asStack_40 [10];
  short asStack_2c [10];
  
  uVar12 = fn_801FB1C0(r3,0,0x43,0);
  uVar12 = fn_801FB1C0(0,uVar12,2,0);
  uVar3 = fn_802367CC(r3,r4,asStack_2c,asStack_40,1);
  if ((uVar3 & 0xffff) == 0) {
    uVar4 = 0;
  }
  else {
    uVar5 = fn_801F1C18(0,r3,aiStack_60,0,1);
    if ((uVar5 & 0xffff) == 0) {
      uVar4 = 0;
    }
    else {
      cVar13 = fn_801FB1C0(0,uVar12,0x30,0);
      if (cVar13 == 1) {
        uVar4 = fn_80205B8C(r4);
        fn_8023A118(0xec6b,0xec2c,0xec2d,r3,uVar4,0,0,0,0,0,0);
        uVar6 = fn_800E0C54();
        iVar7 = (uVar6 & 0xffff) - ((uVar6 & 0xffff) / (uVar5 & 0xffff)) * (uVar5 & 0xffff);
      }
      else {
        local_64 = 0;
        local_68[0] = 0xffff;
        iVar7 = fn_8023B498(r3,r4,uVar3,asStack_2c,uVar5,aiStack_60,&local_64,local_68);
      }
      iVar11 = local_64;
      if (iVar7 < 0) {
        if ((local_64 != 0) && (-1 < (short)local_68[0])) {
          uVar4 = fn_801F0134(local_64,r5);
          uVar3 = fn_80238270(r3,r4,local_68[0] & 0xff);
          if ((uVar3 & 0xffff) != 0) {
            fn_80204F6C(r4,0,0x13,0,0x80375ca8,uVar3 & 0xffff,uVar4,(int)(char)local_68[0],0);
            uVar4 = fn_80205B8C(iVar11);
            uVar8 = fn_801F4354(0,iVar11);
            uVar9 = fn_80205B8C(r4);
            uVar10 = fn_801F4354(0,r4);
            fn_8023A118(0xec6c,0xec2c,0xec2d,uVar10,uVar9,uVar8,uVar4,0,0,0x228,0);
            uVar4 = fn_80205B8C(r4);
            uVar8 = fn_801F4354(0,r4);
            fn_8023A118(0xec64,0xec2c,0xec32,uVar8,uVar4,0,0,uVar3,0,0x228,0);
            return 1;
          }
          return 0;
        }
        uVar4 = 0;
      }
      else {
        iVar7 = aiStack_60[iVar7];
        if (iVar7 == 0) {
          uVar4 = 0;
        }
        else {
          cVar13 = fn_801FB1C0(0,uVar12,0x31,0);
          if (cVar13 == 1) {
            uVar4 = fn_80205B8C(r4);
            fn_8023A118(0xec6b,0xec2c,0xec32,r3,uVar4,0,0,0,0,0,0);
            uVar5 = fn_800E0C54();
            iVar11 = (uVar5 & 0xffff) - ((uVar5 & 0xffff) / (uVar3 & 0xffff)) * (uVar3 & 0xffff);
          }
          else {
            iVar11 = fn_8023A740(r3,r4,uVar3,asStack_2c,asStack_40,iVar7,r5);
          }
          if (iVar11 < 0) {
            uVar4 = 0;
          }
          else {
            sVar2 = asStack_2c[iVar11];
            if (sVar2 == 0) {
              uVar4 = 0;
            }
            else {
              sVar1 = asStack_40[iVar11];
              if (sVar1 < 0) {
                uVar4 = 0;
              }
              else {
                iVar11 = fn_8022B2CC(r4,sVar2,r5,0x8023c368,1,0, (void*)0xffffffff);
                if (iVar11 != 0) {
                  iVar7 = iVar11;
                }
                if (iVar7 == 0) {
                  uVar4 = 0;
                }
                else {
                  uVar4 = fn_801F0134(iVar7,r5);
                  fn_80204F6C(r4,0,0x13,0,0x80375ca8,sVar2,uVar4,(int)(char)sVar1,0);
                  uVar4 = 1;
                }
              }
            }
          }
        }
      }
    }
  }
  return uVar4;
}
/* Address: 0x8023A740 | Size: 0xD58 (3416 bytes) */
#pragma optimization_level 0
void fn_8023A740(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8000815C();
    extern void fn_800E0C54();
    extern void fn_8011BEB4();
    extern void fn_801F025C();
    extern void fn_801F1C18();
    extern void fn_801F2654();
    extern void fn_801F4354();
    extern void fn_801F54A4();
    extern void fn_801F6F38();
    extern void fn_801FB1C0();
    extern void fn_80205B8C();
    extern void fn_80236C80();
    extern void fn_8023753C();
    extern void fn_802376EC();
    extern void fn_8023793C();
    extern void fn_80238060();
    extern void fn_80239154();
    extern void fn_80239244();
    extern void fn_802392A8();
    extern void fn_802393A0();
    extern void fn_8023943C();
    extern void fn_80239498();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern void fn_802397B8();
    extern void fn_802398E4();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239A40();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C530();
    extern void fn_80243C5C();
    extern void fn_8024E52C();
    extern void fn_80253948();
    u8 sp[0x140];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r12 = 0;
    u32 var_r14 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;
    u32 _param3 = 0;
    u32 var_r19 = 0;
    u32 var_r20 = 0;
    u32 var_r21 = 0;
    u32 var_r22 = 0;
    u32 var_r23 = 0;
    u32 var_r24 = 0;
    u32 var_r25 = 0;
    u32 var_r26 = 0;
    u32 var_r27 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;
    void (*ctr_fn)(void) = 0;

    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    var_r19 = r8;
    _param1 = _ctx;
    _ctx = 0x3;
    fn_801F025C();
    tmp = _ctx;
    _ctx = 0x0;
    var_r14 = tmp;
    _param1 = 0x0;
    _param2 = 0x16;
    _param3 = 0x0;
    fn_801F54A4();
    var_r21 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param1 = 0x0;
    _param2 = 0x17;
    _param3 = 0x0;
    fn_801F54A4();
    var_r22 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param1 = 0x0;
    _param2 = 0x18;
    _param3 = 0x0;
    fn_801F54A4();
    var_r24 = _ctx & 0xFFFF;
    _ctx = _ctx;
    _param1 = 0x0;
    _param2 = 0x43;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2;
    _param3 = 0x0;
    fn_801FB1C0();
    var_r25 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param1 = var_r25;
    _param2 = 0x38;
    _param3 = 0x0;
    fn_801FB1C0();
    tmp = _ctx & 0xFF;
    _param1 = _param1;
    *(u8*)(sp + 0xDC) = tmp;
    _ctx = 0x0;
    fn_801F4354();
    tmp = _ctx;
    _ctx = _ctx;
    var_r20 = tmp;
    _param1 = _param1;
    fn_80236C80();
    var_r23 = _ctx;
    _ctx = var_r14;
    _param1 = var_r21;
    _param2 = var_r22;
    _param3 = var_r24;
    fn_801F6F38();
    _param3 = (u32)sp + 0xb4;
    _param2 = (u32)sp + 0x8c;
    _param1 = (u32)sp + 0x64;
    _ctx = (u32)sp + 0x3c;
    r8 = 0x0;
    r7 = 0x0;
    while (1) {
        tmp = r8 & 0xFFFF;
        if (tmp >= 0xa) break;
        r8 = r8 + 0x1;
        *(u32*)(_param3 + tmp) = r7;
        *(u32*)(_param2 + tmp) = r7;
        *(u32*)(_param1 + tmp) = r7;
        *(u32*)(_ctx + tmp) = r7;

    }
    _ctx = _ctx;
    _param1 = var_r19;
    fn_802376EC();
    _param1 = 0xFFFF0000;
    var_r24 = _ctx & 0xFFFF;
    var_r22 = _param1 + 0x1;
    var_r14 = _param2 & 0xFFFF;
    var_r30 = var_r22;
    var_r21 = 0x0;
    var_r28 = 0x0;
    while (1) {
        tmp = var_r28 & 0xFFFF;
        if (tmp >= var_r14) break;
        var_r26 = var_r28 & 0xFFFF;
        var_r29 = *(u16*)(_param3 + tmp);
        if (var_r29 == 0 || var_r29 == 0x165 || tmp == 1 || (s32)tmp > 0 || (s32)var_r21 >= (s32)tmp) {

            _ctx = _ctx;
            _param1 = 0x0;
            _param2 = 0x43;
            _param3 = 0x0;
            fn_801FB1C0();
            _param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            _param2 = 0x2;
            _param3 = 0x0;
            fn_801FB1C0();
            _param1 = _ctx & 0xFFFF;
            _ctx = 0x0;
            _param2 = 0x2d;
            _param3 = 0x0;
            fn_801FB1C0();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
                _param1 = var_r29;
                _ctx = 0x0;
                _param2 = 0x1e;
                _param3 = 0x0;
                fn_8011BEB4();
                /* mr. var_r27, _ctx */;
                if (tmp == 1) {
                    _ctx = (u32)fn_80253948;
                    var_r27 = (u32)fn_80253948;
                }

            } else {
                _ctx = 0x0;
                goto L_8023A98C;
            }
            _param1 = _ctx;
            _param2 = (u32)sp + 0x1c;
            _ctx = 0x0;
            _param3 = 0x0;
            r7 = 0x1;
            fn_801F1C18();
            _param1 = var_r29;
            _ctx = 0x0;
            _param2 = 0x5;
            _param3 = 0x0;
            fn_8011BEB4();
            r12 = var_r27;
            _ctx = _ctx;
            _param1 = _param1;
            _param2 = var_r29;
            _param3 = var_r19;
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        L_8023A98C:
            var_r26 = var_r26 << 2;
            _param2 = (u32)sp + 0x8c;
            *(u32*)(_param2 + var_r26) = _ctx;
            var_r31 = (u32)sp + 0x64;
            _ctx = _ctx;
            _param1 = var_r29;
            var_r27 = *(u32*)(_param2 + var_r26);
            _param2 = 0x1;
            tmp = var_r24 - var_r27;
            *(u32*)(var_r31 + var_r26) = tmp;
            fn_8023943C();
            tmp = _ctx & 0xFF;

            if ((s32)var_r22 < (s32)var_r27) {
                var_r22 = var_r27;
            }
            tmp = *(u32*)(var_r31 + var_r26);

            if ((s32)var_r30 < (s32)tmp) {
                var_r30 = tmp;
            }
            _ctx = _ctx;
            _param1 = var_r29;
            fn_802393A0();
            tmp = _ctx & 0xFF;
            _ctx = (u32)sp + 0x3c;
            *(u32*)(_ctx + var_r26) = tmp;
            tmp = *(u32*)(_ctx + var_r26);

            var_r21 = tmp;
        }
        var_r28 = var_r28 + 0x1;

    }
    tmp = *(u8*)(sp + 0xDC);
    var_r29 = 0x0;
    tmp = var_r23 & 0xFF;
    *(u16*)(sp + 0xE4) = tmp;
    tmp = _ctx + 0x1;
    *(u32*)(sp + 0xE0) = tmp;
    tmp = _param2 & 0xFFFF;
    *(u32*)(sp + 0xEC) = tmp;
    while (1) {
        _ctx = var_r29 & 0xFFFF;
        if (_ctx >= tmp) break;
        var_r26 = var_r29 & 0xFFFF;
        var_r28 = *(u16*)(_param3 + tmp);
        do {
        if (var_r28 == 0) break;

        var_r24 = *(s16*)(_ctx + tmp);
        if ((s32)var_r24 < 0) break;

        _ctx = _ctx;
        _param1 = var_r28;
        _param2 = _param1;
        fn_802395C8();
        tmp = _ctx;
        _ctx = _ctx;
        var_r14 = tmp;
        _param1 = var_r28;
        fn_80239244();
        _param1 = var_r28;
        _ctx = 0x0;
        _param2 = 0x1c;
        _param3 = 0x0;
        fn_8011BEB4();
        var_r23 = _ctx;
        _ctx = _ctx;
        _param1 = 0x0;
        _param2 = 0x43;
        _param3 = 0x0;
        fn_801FB1C0();
        _param1 = _ctx & 0xFFFF;
        _ctx = 0x0;
        _param2 = 0x2;
        _param3 = 0x0;
        fn_801FB1C0();
        _param1 = _ctx & 0xFFFF;
        _ctx = 0x0;
        _param2 = 0x32;
        _param3 = 0x0;
        fn_801FB1C0();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _param1 = var_r28;
            _ctx = 0x0;
            _param2 = 0x1c;
            _param3 = 0x0;
            fn_8011BEB4();
            if (_ctx == 0) {
                _ctx = (u32)fn_8024E52C;
                _ctx = (u32)fn_8024E52C;
            }

        } else {
        _ctx = 0x0;
        goto L_8023AB40;
        }
        r12 = _ctx;
        _ctx = _ctx;
        _param1 = _param1;
        _param2 = var_r28;
        _param3 = var_r19;
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
    L_8023AB40:
        var_r30 = var_r26 << 2;
        var_r31 = (u32)sp + 0xb4;
        *(u32*)(var_r31 + var_r30) = _ctx;
        _ctx = _param1;
        fn_80205B8C();
        tmp = 0x0;
        _param2 = 0x10000;
        *(u32*)(sp + 0x8) = tmp;
        tmp = 0x227;
        r7 = _ctx;
        _param3 = var_r20;
        *(u32*)(sp + 0xC) = tmp;
        r10 = var_r28;
        tmp = *(u32*)(var_r31 + var_r30);
        r8 = 0x0;
        r9 = 0x0;
        *(u32*)(sp + 0x10) = tmp;
        fn_8023A118();
        _ctx = _ctx;
        _param1 = _param1;
        _param2 = var_r28;
        _param3 = var_r19;
        fn_8023C530();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = (u32)fn_80243C5C;
            tmp = (u32)fn_80243C5C;
            if (var_r23 == tmp) {
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = _ctx;
                _param2 = 0x3e;
                fn_80239984();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = var_r20;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x3e;
                fn_80239EE8();
                goto L_8023AC98;
            }
            _ctx = *(u32*)(var_r31 + var_r30);
            _param1 = _ctx;
            _param2 = 0x3c;
            fn_80239984();
            *(u32*)(var_r31 + var_r30) = _ctx;
            _ctx = _param1;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = var_r20;
            r8 = var_r28;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x3c;
            fn_80239EE8();
            goto L_8023AC98;
        }
        _ctx = *(u32*)(var_r31 + var_r30);
        _param1 = _ctx;
        _param2 = 0x3d;
        fn_80239984();
        *(u32*)(var_r31 + var_r30) = _ctx;
        _ctx = _param1;
        fn_80205B8C();
        _param3 = 0x10000;
        _param2 = _ctx;
        _param1 = var_r20;
        r8 = var_r28;
        _param3 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x3d;
        fn_80239EE8();
    L_8023AC98:
        tmp = var_r14 & 0xFFFF;
        if (tmp != 9) {
            _ctx = _ctx;
            _param1 = var_r28;
            _param2 = 0x1;
            fn_8023943C();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
                _ctx = _ctx;
                _param1 = var_r28;
                fn_80239500();
                _param3 = _ctx;
                _ctx = _ctx;
                _param1 = var_r19;
                _param2 = var_r14;
                fn_8023793C();
                tmp = _ctx & 0xFFFF;
                if (tmp == 0x42) {
                    _ctx = *(u32*)(var_r31 + var_r30);
                    _param1 = _ctx;
                    _param2 = 0x3f;
                    fn_80239984();
                    *(u32*)(var_r31 + var_r30) = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    _param3 = 0x10000;
                    _param2 = _ctx;
                    _param1 = var_r20;
                    r8 = var_r28;
                    _param3 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x3f;
                    fn_80239EE8();
                }
                _ctx = _ctx;
                _param1 = var_r28;
                fn_80239500();
                _param3 = _ctx;
                _ctx = _ctx;
                _param1 = var_r19;
                _param2 = var_r14;
                fn_8023793C();
                tmp = _ctx & 0xFFFF;
                if (tmp == 0x43) {
                    _ctx = *(u32*)(var_r31 + var_r30);
                    _param1 = _ctx;
                    _param2 = 0x40;
                    fn_80239984();
                    *(u32*)(var_r31 + var_r30) = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    _param3 = 0x10000;
                    _param2 = _ctx;
                    _param1 = var_r20;
                    r8 = var_r28;
                    _param3 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x40;
                    fn_80239EE8();
        }
        }
        }
        var_r23 = 0x0;
        while (1) {
            tmp = var_r23 & 0xFFFF;
            if (tmp >= 3) break;
            _ctx = _ctx;
            _param1 = var_r28;
            _param2 = var_r23 & 0xFF;
            fn_80239498();
            _param3 = _ctx & 0xFF;
            if (tmp != 0x43) {
                _param1 = *(u16*)(sp + 0xE4);
                _ctx = 0x0;
                _param2 = 0xc;
                fn_801FB1C0();
                var_r26 = _ctx & 0xFF;
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = var_r26;
                _param2 = _ctx;
                _param3 = 0x41;
                fn_802398E4();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = _ctx;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x41;
                fn_80239A40();
            }
            var_r23 = var_r23 + 0x1;

        }
        tmp = var_r14 & 0xFFFF;
        if (tmp != 9) {
            var_r23 = 0x0;
            while (1) {
                tmp = var_r23 & 0xFFFF;
                if (tmp >= 2) break;
                _param1 = var_r25;
                _param3 = var_r23;
                _ctx = 0x0;
                _param2 = 0x39;
                fn_801FB1C0();
                _ctx = _ctx & 0xFFFF;
                if (_ctx != 9) {
                    tmp = var_r14 & 0xFFFF;
                    if (tmp == _ctx) {
                        _param1 = var_r25;
                        _param3 = var_r23;
                        _ctx = 0x0;
                        _param2 = 0x3a;
                        fn_801FB1C0();
                        var_r26 = _ctx & 0xFF;
                        _ctx = *(u32*)(var_r31 + var_r30);
                        _param1 = var_r26;
                        _param2 = _ctx;
                        _param3 = 0x42;
                        fn_802398E4();
                        *(u32*)(var_r31 + var_r30) = _ctx;
                        _ctx = _param1;
                        fn_80205B8C();
                        _param3 = 0x10000;
                        _param2 = _ctx;
                        _param1 = _ctx;
                        r8 = var_r28;
                        _param3 = 0x0;
                        r7 = 0x0;
                        r9 = 0x0;
                        r10 = 0x42;
                        fn_80239A40();
                }
                }
                var_r23 = var_r23 + 0x1;

            }
        }
        var_r27 = 0x0;
        while (1) {
            tmp = var_r27 & 0xFFFF;
            if (tmp >= 3) break;
            _ctx = _ctx;
            _param1 = var_r28;
            _param2 = var_r27 & 0xFF;
            fn_80239498();
            tmp = _ctx & 0xFF;
            var_r14 = _ctx;
            if (tmp != 2) {
                var_r23 = 0x0;
                while (1) {
                    tmp = var_r23 & 0xFFFF;
                    if (tmp >= 2) break;
                    _param1 = var_r25;
                    _param3 = var_r23;
                    _ctx = 0x0;
                    _param2 = 0x3b;
                    fn_801FB1C0();
                    _ctx = _ctx & 0xFF;
                    if (tmp != 2) {
                        tmp = var_r14 & 0xFF;
                        if (tmp == _ctx) {
                            _param1 = var_r25;
                            _param3 = var_r23;
                            _ctx = 0x0;
                            _param2 = 0x3c;
                            fn_801FB1C0();
                            var_r26 = _ctx & 0xFF;
                            _ctx = *(u32*)(var_r31 + var_r30);
                            _param1 = var_r26;
                            _param2 = _ctx;
                            _param3 = 0x43;
                            fn_802398E4();
                            *(u32*)(var_r31 + var_r30) = _ctx;
                            _ctx = _param1;
                            fn_80205B8C();
                            _param3 = 0x10000;
                            _param2 = _ctx;
                            _param1 = _ctx;
                            r8 = var_r28;
                            _param3 = 0x0;
                            r7 = 0x0;
                            r9 = 0x0;
                            r10 = 0x43;
                            fn_80239A40();
                    }
                    }
                    var_r23 = var_r23 + 0x1;

                }
            }
            var_r27 = var_r27 + 0x1;

        }
        _ctx = _ctx;
        _param1 = var_r28;
        _param2 = 0x1;
        fn_8023943C();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = (u32)sp + 0x8c;
            tmp = *(u32*)(_ctx + var_r30);
            if ((s32)var_r22 <= (s32)tmp) {
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = _ctx;
                _param2 = 0x44;
                fn_80239984();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = var_r20;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x44;
                fn_80239EE8();
            }
            _ctx = (u32)sp + 0x64;
            var_r14 = *(u32*)(_ctx + var_r30);
            if ((s32)var_r14 < 0) {
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = _ctx;
                _param2 = 0x45;
                fn_80239984();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = var_r20;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x45;
                fn_80239EE8();
            }
            if ((s32)var_r14 <= 0) {
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = _ctx;
                _param2 = 0x46;
                fn_80239984();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = var_r20;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x46;
                fn_80239EE8();
            }
            if ((s32)var_r14 <= 0) {
                _ctx = (u32)sp + 0x3c;
                tmp = *(u32*)(_ctx + var_r30);
                if ((s32)var_r21 <= (s32)tmp) {
                    _ctx = *(u32*)(var_r31 + var_r30);
                    _param1 = _ctx;
                    _param2 = 0x47;
                    fn_80239984();
                    *(u32*)(var_r31 + var_r30) = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    _param3 = 0x10000;
                    _param2 = _ctx;
                    _param1 = var_r20;
                    r8 = var_r28;
                    _param3 = 0x0;
                    r7 = 0x0;
                    r9 = 0x0;
                    r10 = 0x47;
                    fn_80239EE8();
        }
        }
        }
        _ctx = _ctx;
        _param1 = var_r28;
        _param2 = 0x2;
        fn_8023943C();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = _ctx;
            _param1 = _param1;
            fn_8023753C();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = _ctx;
                _param2 = 0x48;
                fn_80239984();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = var_r20;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x48;
                fn_80239EE8();
        }
        }
        _ctx = _ctx;
        _param1 = var_r28;
        fn_802392A8();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = *(u32*)(var_r31 + var_r30);
            _param1 = _ctx;
            _param2 = 0x49;
            fn_80239984();
            *(u32*)(var_r31 + var_r30) = _ctx;
            _ctx = _param1;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = var_r20;
            r8 = var_r28;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x49;
            fn_80239EE8();
        }
        tmp = tmp & 0xFF;
        if (tmp == 4) {
            _param2 = var_r19;
            _ctx = 0x0;
            _param1 = 0x1;
            _param3 = 0x1;
            fn_801F2654();
            tmp = _ctx & 0xFFFF;
            if (tmp >= 2) {
                _ctx = *(u32*)(var_r31 + var_r30);
                _param1 = _ctx;
                _param2 = 0x4a;
                fn_80239984();
                *(u32*)(var_r31 + var_r30) = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = var_r20;
                r8 = var_r28;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x4a;
                fn_80239EE8();
        }
        }
        _ctx = _ctx;
        _param1 = _param1;
        _param2 = var_r24 & 0xFF;
        fn_80238060();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = *(u32*)(var_r31 + var_r30);
            _param1 = _ctx;
            _param2 = 0x4b;
            fn_80239984();
            *(u32*)(var_r31 + var_r30) = _ctx;
            _ctx = _param1;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = var_r20;
            r8 = var_r28;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x4b;
            fn_80239EE8();
        }
        _ctx = _ctx;
        _param1 = var_r28;
        fn_80239154();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = *(u32*)(var_r31 + var_r30);
            _param1 = _ctx;
            _param2 = 0x4c;
            fn_80239984();
            *(u32*)(var_r31 + var_r30) = _ctx;
            _ctx = _param1;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = var_r20;
            r8 = var_r28;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x4c;
            fn_80239EE8();
        }
        fn_8000815C();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            fn_800E0C54();
            _param2 = _ctx & 0xFFFF;
            _ctx = *(u32*)(var_r31 + var_r30);
            _param1 = (s32)_param2 / (s32)tmp;
            tmp = _param1 * tmp;
            _param1 = _param2 - tmp;
            tmp = *(u8*)(sp + 0xDC);
            var_r14 = _param1 - tmp;
            _param1 = var_r14;
            fn_802399FC();
            *(u32*)(var_r31 + var_r30) = _ctx;
            _ctx = _param1;
            fn_80205B8C();
            tmp = 0x0;
            _param2 = 0x10000;
            *(u32*)(sp + 0x8) = tmp;
            tmp = 0x225;
            r7 = _ctx;
            _param3 = var_r20;
            *(u32*)(sp + 0xC) = tmp;
            r10 = var_r28;
            r8 = 0x0;
            r9 = 0x0;
            fn_8023A118();
        }
        _ctx = _param1;
        fn_80205B8C();
        tmp = 0x0;
        _param2 = 0x10000;
        *(u32*)(sp + 0x8) = tmp;
        tmp = 0x226;
        r7 = _ctx;
        _param3 = var_r20;
        *(u32*)(sp + 0xC) = tmp;
        r10 = var_r28;
        tmp = *(u32*)(var_r31 + var_r30);
        r8 = 0x0;
        r9 = 0x0;
        *(u32*)(sp + 0x10) = tmp;
        fn_8023A118();
        } while (0);

        var_r29 = var_r29 + 0x1;

    }
    _param1 = _param2;
    _ctx = (u32)sp + 0xb4;
    _param2 = 0x1;
    fn_802397B8();
    /* mr. var_r14, _ctx */;
    if (_ctx < tmp) {
        _ctx = (u32)-0x1;
    } else {

        _ctx = _param1;
        fn_80205B8C();
        tmp = 0x0;
        r7 = var_r14 << 2;
        *(u32*)(sp + 0x8) = tmp;
        tmp = 0x228;
        _param1 = (u32)sp + 0xb4;
        _param2 = 0x10000;
        *(u32*)(sp + 0xC) = tmp;
        tmp = var_r14 << 1;
        _param3 = var_r20;
        r9 = 0x0;
        r8 = *(u32*)(_param1 + r7);
        r7 = _ctx;
        r8 = 0x0;
        r10 = *(u16*)(_param3 + tmp);
        fn_8023A118();
        _ctx = var_r14;
    }
    return;
}
#pragma optimization_level 4

/* Address: 0x8023B498 | Size: 0xED0 (3792 bytes) */
#pragma optimization_level 0
void fn_8023B498(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 lbl_8027A408[];
    extern u32 lbl_8047E628;
    extern u32 lbl_8047E62C;
    extern void fn_8000815C();
    extern void fn_800E0C54();
    extern void fn_8011BEB4();
    extern void fn_80120B00();
    extern void fn_801F1990();
    extern void fn_801F1C18();
    extern void fn_801F4354();
    extern void fn_801FB1C0();
    extern void fn_80205B2C();
    extern void fn_80205B8C();
    extern void fn_802062FC();
    extern void fn_80235714();
    extern void fn_80236584();
    extern void fn_80236BFC();
    extern void fn_80237310();
    extern void fn_802376EC();
    extern void fn_8023785C();
    extern void fn_80237F74();
    extern void fn_8023831C();
    extern void fn_8023943C();
    extern void fn_802397B8();
    extern void fn_80239984();
    extern void fn_802399FC();
    extern void fn_80239EE8();
    extern void fn_8023A118();
    extern void fn_8023C530();
    extern u8 jumptable_8039A578[];
    extern void fn_80253948();
    u8 sp[0x170];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r12 = 0;
    u32 var_r14 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 var_r17 = 0;
    u32 var_r18 = 0;
    u32 var_r19 = 0;
    u32 var_r20 = 0;
    u32 var_r21 = 0;
    u32 var_r22 = 0;
    u32 var_r23 = 0;
    u32 var_r24 = 0;
    u32 var_r25 = 0;
    u32 var_r26 = 0;
    u32 var_r27 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    _param1 = _param1;
    _ctx = _ctx;
    var_r17 = r7;
    *(u16*)(sp + 0x18) = param2;
    var_r18 = r8;
    var_r21 = r9;
    var_r20 = r10;
    _param1 = 0x0;
    param2 = 0x43;
    param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    param2 = 0x2;
    param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    param2 = 0x38;
    param3 = 0x0;
    fn_801FB1C0();
    param2 = (u32)lbl_8027A408;
    tmp = _ctx & 0xFF;
    param2 = (u32)lbl_8027A408;
    _param1 = 0x2;
    *(u8*)(sp + 0x10C) = tmp;
    r9 = (u32)sp + 0x7c;
    ctr_fn = (void(*)(void))_param1;
    do {
        _ctx = *(u32*)((u8*)r8 + 0x4);
        tmp = *(u32*)((u8*)r8 + 0x8);
        *(u32*)((u8*)r9 + 0x4) = _ctx;
        r9 += 8; *(u32*)r9 = tmp;
    } while (--ctr != 0);
    tmp = *(u32*)((u8*)r8 + 0x4);
    _param1 = _ctx;
    param2 = (u32)sp + 0x98;
    _ctx = 0x0;
    *(u32*)((u8*)r9 + 0x4) = tmp;
    param3 = 0x1;
    r7 = 0x1;
    tmp = *(u16*)((u8*)r8 + 0x8);
    *(u16*)((u8*)r9 + 0x8) = tmp;
    r8 = lbl_8047E628;
    tmp = lbl_8047E62C;
    *(u32*)(sp + 0x2C) = tmp;
    fn_801F1C18();
    var_r14 = _ctx;
    _param1 = _param1;
    _ctx = 0x0;
    fn_801F4354();
    var_r24 = (u32)sp + 0x98;
    var_r19 = _ctx;
    var_r22 = var_r14 & 0xFFFF;
    var_r27 = 0x0;
    while (1) {
        tmp = var_r27 & 0xFFFF;
        if (tmp >= var_r22) break;
        var_r14 = *(u32*)(var_r24 + tmp);
        do {
        if (var_r14 == 0) break;

        _ctx = var_r14;
        fn_802062FC();
        tmp = _ctx & 0xFF;
        if (var_r14 == 0 || _param1 == var_r14) break;

        _ctx = _ctx;
        _param1 = var_r14;
        param2 = 0x12;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        do {
        if (tmp != 1) break;

        _ctx = _ctx;
        _param1 = var_r14;
        param2 = 0x3a;
        fn_80236BFC();
        tmp = _ctx & 0xFF;
        if (tmp != 1) break;

        var_r23 = (u32)sp + 0x80;
        var_r26 = 0x0;
        var_r25 = 0x0;
        while (1) {
        do {
            tmp = var_r25 & 0xFFFF;
            if (tmp >= 0xb) break;
            var_r28 = *(u16*)(var_r23 + tmp);
            if (var_r28 == 0xed) {
                _ctx = _param1;
                fn_80205B8C();
                _param1 = (u32)sp + 0x22;
                param2 = (u32)sp + 0x20;
                fn_80120B00();
                tmp = *(u16*)(sp + 0x20);
                if (tmp != 0xa) break;
            }
            _ctx = _ctx;
            _param1 = _param1;
            param2 = var_r28;
            param3 = (u32)sp + 0x24;
            r7 = 0x1;
            fn_80236584();
            tmp = _ctx & 0xFF;
            if (tmp == 0xa) break;
            _param1 = *(s16*)((u8*)(u32)sp + 0x24);
            tmp = (s16)_param1;
            if (tmp < 0xa) break;
            tmp = (s16)var_r26;
            _ctx = (u32)sp + 0xb8;
            tmp = tmp << 1;
            var_r26 = var_r26 + 0x1;
            *(u16*)(_ctx + tmp) = _param1;
        } while (0);
            var_r25 = var_r25 + 0x1;

        }
        tmp = (s16)var_r26;
        if (tmp <= 0xb) break;

        fn_800E0C54();
        param2 = _ctx & 0xFFFF;
        _param1 = (s16)var_r26;
        tmp = (s32)param2 / (s32)_param1;
        _ctx = (u32)sp + 0xb8;
        tmp = tmp * _param1;
        tmp = param2 - tmp;
        tmp = tmp << 1;
        tmp = *(s16*)(_ctx + tmp);
        *(u16*)(sp + 0x24) = tmp;
        if (var_r21 != 0) {
            *(u32*)((u8*)var_r21 + 0x0) = var_r14;
        }
        if (var_r20 != 0) {
            tmp = *(s16*)((u8*)(u32)sp + 0x24);
            *(u16*)((u8*)var_r20 + 0x0) = tmp;
        }
        _ctx = (u32)-0x1;
        return;
        } while (0);

        _ctx = _ctx;
        _param1 = var_r14;
        param2 = 0x3e;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp != 1) {
            _ctx = _ctx;
            _param1 = var_r14;
            param2 = 0x3f;
            fn_80237F74();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
            }
            _ctx = _ctx;
            _param1 = var_r14;
            fn_80237310();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
                var_r25 = 0x0;
                var_r23 = (u32)sp + 0x28;
                var_r26 = var_r25;
                var_r28 = (u32)sp + 0xb8;
                while (1) {
                    tmp = var_r26 & 0xFFFF;
                    if (tmp >= 4) break;
                    _ctx = _ctx;
                    param2 = *(u16*)(var_r23 + tmp);
                    _param1 = _param1;
                    param3 = (u32)sp + 0x24;
                    r7 = 0x1;
                    fn_80236584();
                    tmp = _ctx & 0xFF;
                    if (tmp != 1) {
                        _ctx = *(s16*)((u8*)(u32)sp + 0x24);
                        tmp = (s16)_ctx;
                        if (tmp >= 1) {
                            tmp = (s16)var_r25;
                            var_r25 = var_r25 + 0x1;
                            tmp = tmp << 1;
                            *(u16*)(var_r28 + tmp) = _ctx;
                    }
                    }
                    var_r26 = var_r26 + 0x1;

                }
                tmp = (s16)var_r25;
                if (tmp > 4) {
                    fn_800E0C54();
                    param2 = _ctx & 0xFFFF;
                    _param1 = (s16)var_r25;
                    tmp = (s32)param2 / (s32)_param1;
                    _ctx = (u32)sp + 0xb8;
                    tmp = tmp * _param1;
                    tmp = param2 - tmp;
                    tmp = tmp << 1;
                    tmp = *(s16*)(_ctx + tmp);
                    *(u16*)(sp + 0x24) = tmp;
                    if (var_r21 != 0) {
                        *(u32*)((u8*)var_r21 + 0x0) = var_r14;
                    }
                    if (var_r20 != 0) {
                        tmp = *(s16*)((u8*)(u32)sp + 0x24);
                        *(u16*)((u8*)var_r20 + 0x0) = tmp;
                    }
                    _ctx = (u32)-0x1;
                    return;
            }
            }
            }
        _ctx = _ctx;
        _param1 = var_r14;
        var_r23 = 0x0;
        param2 = 0x36;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            var_r23 = 0x1;
        }
        _ctx = _ctx;
        _param1 = var_r14;
        fn_8023785C();
        tmp = _ctx & 0xFF;
        if (tmp == 2 && tmp == 1) {
            _ctx = _ctx;
            _param1 = var_r14;
            param2 = 0x25;
            fn_80237F74();
            tmp = _ctx & 0xFF;
            if (tmp != 1) {
                _ctx = _ctx;
                _param1 = var_r14;
                param2 = 0x4a;
                fn_80237F74();
                tmp = _ctx & 0xFF;

            }
            var_r23 = 0x1;
        }
        _ctx = _ctx;
        _param1 = var_r14;
        fn_8023785C();
        tmp = _ctx & 0xFF;
        if (tmp == 3 && tmp == 1) {
            _ctx = _ctx;
            _param1 = _param1;
            param2 = 0x25;
            fn_80237F74();
            tmp = _ctx & 0xFF;
            if (tmp != 1) {
                _ctx = _ctx;
                _param1 = _param1;
                param2 = 0x4a;
                fn_80237F74();
                tmp = _ctx & 0xFF;

            }
            var_r23 = 0x1;
        }
        tmp = var_r23 & 0xFF;
        if (tmp == 1) {
            _ctx = _ctx;
            _param1 = _param1;
            param3 = (u32)sp + 0x24;
            param2 = 0x11d;
            r7 = 0x1;
            fn_80236584();
            tmp = _ctx & 0xFF;
            if (tmp != 1) {
                tmp = *(s16*)((u8*)(u32)sp + 0x24);
                if ((s32)tmp >= 0) {
                    if (var_r21 != 0) {
                        *(u32*)((u8*)var_r21 + 0x0) = var_r14;
                    }
                    if (var_r20 != 0) {
                        tmp = *(s16*)((u8*)(u32)sp + 0x24);
                        *(u16*)((u8*)var_r20 + 0x0) = tmp;
                    }
                    _ctx = (u32)-0x1;
                    return;
        }
        }
        }
        _ctx = _ctx;
        _param1 = var_r14;
        var_r23 = 0x0;
        fn_80235714();
        tmp = _ctx & 0xFF;
        if (var_r20 == 0 && tmp == 8 || tmp == 9) {
            _ctx = _ctx;
            _param1 = var_r14;
            param2 = 0x14;
            fn_80237F74();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
                var_r23 = 0x1;
            }
            _ctx = _ctx;
            _param1 = var_r14;
            fn_8023831C();
            tmp = _ctx & 0xFFFF;

            var_r23 = 0x1;
        }
        tmp = var_r23 & 0xFF;
        if (tmp == 1) {
            _ctx = _ctx;
            _param1 = _param1;
            param3 = (u32)sp + 0x24;
            param2 = 0xcf;
            r7 = 0x1;
            fn_80236584();
            tmp = _ctx & 0xFF;
            if (tmp != 1) {
                tmp = *(s16*)((u8*)(u32)sp + 0x24);
                if ((s32)tmp >= 0) {
                    if (var_r21 != 0) {
                        *(u32*)((u8*)var_r21 + 0x0) = var_r14;
                    }
                    if (var_r20 != 0) {
                        tmp = *(s16*)((u8*)(u32)sp + 0x24);
                        *(u16*)((u8*)var_r20 + 0x0) = tmp;
                    }
                    _ctx = (u32)-0x1;
                    return;
        }
        }
        }
        _ctx = _ctx;
        _param1 = var_r14;
        fn_80235714();
        tmp = _ctx & 0xFF;
        if (tmp != 1) break;

        _ctx = _ctx;
        _param1 = _param1;
        fn_80235714();
        tmp = _ctx & 0xFF;
        if (tmp != 1) break;

        _ctx = _ctx;
        _param1 = _param1;
        param3 = (u32)sp + 0x24;
        param2 = 0xf4;
        r7 = 0x1;
        fn_80236584();
        tmp = _ctx & 0xFF;
        if (tmp == 1) break;

        tmp = *(s16*)((u8*)(u32)sp + 0x24);
        if ((s32)tmp < 0) break;

        if (var_r21 != 0) {
            *(u32*)((u8*)var_r21 + 0x0) = var_r14;
        }
        if (var_r20 != 0) {
            tmp = *(s16*)((u8*)(u32)sp + 0x24);
            *(u16*)((u8*)var_r20 + 0x0) = tmp;
        }
        _ctx = (u32)-0x1;
        return;
        } while (0);

        var_r27 = var_r27 + 0x1;

    }
    param3 = (u32)sp + 0xec;
    param2 = (u32)sp + 0xcc;
    _param1 = (u32)sp + 0x38;
    _ctx = (u32)sp + 0x30;
    r9 = 0x0;
    r7 = 0x0;
    while (1) {
        tmp = r9 & 0xFFFF;
        if (tmp >= 8) break;
        r8 = r9 & 0xFFFF;
        *(u32*)(param3 + tmp) = r7;
        r9 = r9 + 0x1;
        *(u32*)(param2 + tmp) = r7;
        *(u8*)(_param1 + r8) = r7;
        *(u8*)(_ctx + r8) = r7;

    }
    tmp = var_r17 & 0xFFFF;
    var_r22 = 0x0;
    *(u32*)(sp + 0x118) = tmp;
    var_r24 = 0x0;
    while (1) {
        _ctx = var_r24 & 0xFFFF;
        if (_ctx >= tmp) break;
        var_r27 = var_r24 & 0xFFFF;
        var_r21 = *(u32*)(var_r18 + var_r26);
        if (var_r21 != 0 || var_r21 < 0 || (s32)var_r22 >= (s32)tmp) {
            _ctx = var_r21;
            fn_80205B2C();
            tmp = (s16)_ctx;

            _ctx = _ctx;
            _param1 = var_r21;
            fn_802376EC();
            tmp = _ctx & 0xFFFF;
            var_r23 = 0x0;
            *(u32*)(sp + 0x110) = tmp;
            var_r25 = 0x0;
            tmp = *(u16*)(sp + 0x18);
            *(u32*)(sp + 0x114) = tmp;
            while (1) {
                _ctx = var_r25 & 0xFFFF;
                if (_ctx >= tmp) break;
                var_r20 = *(u16*)(_ctx + tmp);
                if (var_r20 == 0 || var_r20 == 0x165 || tmp == 1 || (s32)tmp >= (s32)_ctx || tmp == 1) {
                do {

                    _ctx = _ctx;
                    _param1 = _param1;
                    param2 = var_r20;
                    param3 = var_r21;
                    fn_8023C530();
                    var_r31 = _ctx;
                    tmp = _ctx & 0xFF;
                    if (tmp == 1) {
                        tmp = 0x1;
                        _ctx = (u32)sp + 0x30;
                        *(u8*)(_ctx + var_r27) = tmp;
                    }
                    _ctx = _ctx;
                    _param1 = var_r20;
                    param2 = 0x1;
                    fn_8023943C();
                    tmp = _ctx & 0xFF;

                    _ctx = _ctx;
                    _param1 = 0x0;
                    param2 = 0x43;
                    param3 = 0x0;
                    fn_801FB1C0();
                    _param1 = _ctx & 0xFFFF;
                    _ctx = 0x0;
                    param2 = 0x2;
                    param3 = 0x0;
                    fn_801FB1C0();
                    _param1 = _ctx & 0xFFFF;
                    _ctx = 0x0;
                    param2 = 0x2d;
                    param3 = 0x0;
                    fn_801FB1C0();
                    tmp = _ctx & 0xFF;
                    if (tmp == 1) {
                        _param1 = var_r20;
                        _ctx = 0x0;
                        param2 = 0x1e;
                        param3 = 0x0;
                        fn_8011BEB4();
                        /* mr. var_r28, _ctx */;
                        if (tmp == 1) {
                            _ctx = (u32)fn_80253948;
                            var_r28 = (u32)fn_80253948;
                        }

                    } else {
                    var_r30 = 0x0;
                    break;
                    }
                    _param1 = _ctx;
                    param2 = (u32)sp + 0x60;
                    _ctx = 0x0;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801F1C18();
                    var_r14 = _ctx;
                    _param1 = var_r20;
                    _ctx = 0x0;
                    param2 = 0x5;
                    param3 = 0x0;
                    fn_8011BEB4();
                    tmp = _ctx & 0xFF;
                    var_r30 = 0x0;
                    if (tmp > 7) break;
                    _ctx = (u32)jumptable_8039A578;
                    tmp = tmp << 2;
                    _ctx = (u32)jumptable_8039A578;
                    tmp = *(u32*)(_ctx + tmp);
                    ctr_fn = (void(*)(void))tmp;
                    r12 = var_r28;
                    _ctx = _ctx;
                    _param1 = _param1;
                    param2 = var_r20;
                    param3 = var_r21;
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                    var_r30 = _ctx;
                    break;
                    var_r14 = var_r14 & 0xFFFF;
                    var_r29 = 0x0;
                    while (1) {
                        tmp = var_r29 & 0xFFFF;
                        if (tmp >= var_r14) break;
                        r12 = var_r28;
                        _ctx = _ctx;
                        _param1 = _param1;
                        param2 = var_r20;
                        param3 = var_r21;
                        ctr_fn = (void(*)(void))r12;
                        ctr_fn();
                        var_r30 = var_r30 + _ctx;
                        var_r29 = var_r29 + 0x1;

                    }
                    break;
                    r12 = var_r28;
                    _ctx = _ctx;
                    _param1 = _param1;
                    param2 = var_r20;
                    param3 = var_r21;
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                    var_r30 = _ctx;
                } while (0);
                    if ((s32)var_r23 < (s32)var_r30) {
                        var_r23 = var_r30;
                    }
                    _ctx = _ctx;
                    _param1 = 0x0;
                    param2 = 0x43;
                    param3 = 0x0;
                    fn_801FB1C0();
                    _param1 = _ctx & 0xFFFF;
                    _ctx = 0x0;
                    param2 = 0x2;
                    param3 = 0x0;
                    fn_801FB1C0();
                    _param1 = _ctx & 0xFFFF;
                    _ctx = 0x0;
                    param2 = 0x2d;
                    param3 = 0x0;
                    fn_801FB1C0();
                    tmp = _ctx & 0xFF;
                    if (tmp == 1) {
                        _param1 = var_r20;
                        _ctx = 0x0;
                        param2 = 0x1e;
                        param3 = 0x0;
                        fn_8011BEB4();
                        /* mr. var_r14, _ctx */;
                        if (tmp == 1) {
                            _ctx = (u32)fn_80253948;
                            var_r14 = (u32)fn_80253948;
                        }

                    } else {
                    _ctx = 0x0;
                    goto L_8023BD6C;
                    }
                    _param1 = _ctx;
                    param2 = (u32)sp + 0x40;
                    _ctx = 0x0;
                    param3 = 0x0;
                    r7 = 0x1;
                    fn_801F1C18();
                    _param1 = var_r20;
                    _ctx = 0x0;
                    param2 = 0x5;
                    param3 = 0x0;
                    fn_8011BEB4();
                    r12 = var_r14;
                    _ctx = _ctx;
                    _param1 = _param1;
                    param2 = var_r20;
                    param3 = var_r21;
                    ctr_fn = (void(*)(void))r12;
                    ctr_fn();
                L_8023BD6C:

                    tmp = var_r31 & 0xFF;

                    tmp = 0x1;
                    _ctx = (u32)sp + 0x38;
                    *(u8*)(_ctx + var_r27) = tmp;
                }
                var_r25 = var_r25 + 0x1;

            }
            _ctx = (u32)sp + 0xcc;
            *(u32*)(_ctx + var_r26) = var_r23;
            tmp = *(u32*)(_ctx + var_r26);

            var_r22 = tmp;
        }
        var_r24 = var_r24 + 0x1;

    }
    tmp = *(u8*)(sp + 0x10C);
    var_r23 = var_r17 & 0xFFFF;
    var_r14 = 0x0;
    var_r26 = _ctx + 0x1;
    while (1) {
        tmp = var_r14 & 0xFFFF;
        if (tmp >= var_r23) break;
        var_r29 = var_r14 & 0xFFFF;
        var_r20 = *(u32*)(var_r18 + var_r27);
        if (var_r20 != 0) {
            _ctx = var_r20;
            fn_80205B2C();
            var_r25 = _ctx;
            tmp = (s16)var_r25;
            if (var_r20 >= 0) {
                _param1 = var_r20;
                _ctx = 0x0;
                fn_801F4354();
                tmp = _ctx;
                _ctx = var_r20;
                var_r21 = tmp;
                fn_80205B8C();
                var_r24 = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                tmp = 0x0;
                param2 = 0x10000;
                *(u32*)(sp + 0x8) = tmp;
                tmp = 0x227;
                r7 = _ctx;
                var_r28 = (u32)sp + 0xec;
                *(u32*)(sp + 0xC) = tmp;
                param3 = var_r19;
                r8 = var_r21;
                r9 = var_r24;
                tmp = *(u32*)(var_r28 + var_r27);
                *(u32*)(sp + 0x10) = tmp;
                r10 = 0x0;
                fn_8023A118();
                var_r24 = var_r25 & 0xFFFF;
                _ctx = _ctx;
                _param1 = var_r24;
                param2 = 0x52;
                param3 = 0x0;
                fn_801FB1C0();
                tmp = _ctx & 0xFF;
                if (tmp == 1) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x34;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r25 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r25;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x34;
                    fn_80239EE8();
                }
                _ctx = _ctx;
                _param1 = var_r24;
                param2 = 0x53;
                param3 = 0x0;
                fn_801FB1C0();
                tmp = _ctx & 0xFF;
                if (tmp == 1) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x35;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r25 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r25;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x35;
                    fn_80239EE8();
                }
                _ctx = _ctx;
                _param1 = var_r24;
                param2 = 0x55;
                param3 = 0x0;
                fn_801FB1C0();
                tmp = _ctx & 0xFF;
                if (tmp == 1) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x36;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r25 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r25;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x36;
                    fn_80239EE8();
                }
                _ctx = (u32)sp + 0xcc;
                tmp = *(u32*)(_ctx + var_r27);
                if ((s32)var_r22 <= (s32)tmp) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x37;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r25 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r25;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x37;
                    fn_80239EE8();
                }
                _param1 = _ctx;
                r8 = var_r20;
                _ctx = 0x0;
                param2 = 0x1;
                param3 = 0x1;
                r7 = 0x0;
                fn_801F1990();
                tmp = _ctx & 0xFF;
                if (tmp == 1) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x38;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r25 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r25;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x38;
                    fn_80239EE8();
                }
                _ctx = _ctx;
                _param1 = var_r24;
                param2 = 0x56;
                param3 = 0x0;
                fn_801FB1C0();
                if ((s32)_ctx == 1) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x39;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r24 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r24;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x39;
                    fn_80239EE8();
                }
                _ctx = (u32)sp + 0x30;
                tmp = *(u8*)(_ctx + var_r29);
                if (tmp == 0) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x3a;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r24 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r24;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x3a;
                    fn_80239EE8();
                }
                _ctx = (u32)sp + 0x38;
                tmp = *(u8*)(_ctx + var_r29);
                if (tmp == 1) {
                    _ctx = *(u32*)(var_r28 + var_r27);
                    _param1 = _ctx;
                    param2 = 0x3b;
                    fn_80239984();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r24 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    r8 = 0x10000;
                    param2 = _ctx;
                    _param1 = var_r19;
                    param3 = var_r21;
                    r7 = var_r24;
                    r8 = 0x0;
                    r9 = 0x0;
                    r10 = 0x3b;
                    fn_80239EE8();
                }
                fn_8000815C();
                tmp = _ctx & 0xFF;
                if (tmp == 1) {
                    fn_800E0C54();
                    _param1 = _ctx & 0xFFFF;
                    _ctx = *(u32*)(var_r28 + var_r27);
                    tmp = (s32)_param1 / (s32)var_r26;
                    tmp = tmp * var_r26;
                    _param1 = _param1 - tmp;
                    tmp = *(u8*)(sp + 0x10C);
                    var_r25 = _param1 - tmp;
                    _param1 = var_r25;
                    fn_802399FC();
                    *(u32*)(var_r28 + var_r27) = _ctx;
                    _ctx = var_r20;
                    fn_80205B8C();
                    var_r24 = _ctx;
                    _ctx = _param1;
                    fn_80205B8C();
                    tmp = 0x0;
                    param2 = 0x10000;
                    *(u32*)(sp + 0x8) = tmp;
                    tmp = 0x225;
                    r7 = _ctx;
                    param3 = var_r19;
                    *(u32*)(sp + 0xC) = tmp;
                    r8 = var_r21;
                    r9 = var_r24;
                    r10 = 0x0;
                    fn_8023A118();
                }
                _ctx = var_r20;
                fn_80205B8C();
                var_r20 = _ctx;
                _ctx = _param1;
                fn_80205B8C();
                tmp = 0x0;
                param2 = 0x10000;
                *(u32*)(sp + 0x8) = tmp;
                tmp = 0x226;
                r7 = _ctx;
                param3 = var_r19;
                *(u32*)(sp + 0xC) = tmp;
                r8 = var_r21;
                r9 = var_r20;
                tmp = *(u32*)(var_r28 + var_r27);
                r10 = 0x0;
                *(u32*)(sp + 0x10) = tmp;
                fn_8023A118();
        }
        }
        var_r14 = var_r14 + 0x1;

    }
    _param1 = var_r17;
    _ctx = (u32)sp + 0xec;
    param2 = 0x1;
    fn_802397B8();
    /* mr. var_r20, _ctx */;
    if (tmp < var_r23) {
        _ctx = (u32)-0x1;
        return;
    }
    _ctx = var_r20 << 2;
    _ctx = 0x0;
    _param1 = *(u32*)(var_r18 + _ctx);
    fn_801F4354();
    var_r17 = _ctx;
    _ctx = *(u32*)(var_r18 + _ctx);
    fn_80205B8C();
    var_r14 = _ctx;
    _ctx = _param1;
    fn_80205B8C();
    tmp = 0x0;
    _param1 = (u32)sp + 0xec;
    *(u32*)(sp + 0x8) = tmp;
    tmp = 0x228;
    param2 = 0x10000;
    r7 = _ctx;
    *(u32*)(sp + 0xC) = tmp;
    param3 = var_r19;
    r8 = var_r17;
    r9 = var_r14;
    tmp = *(u32*)(_param1 + _ctx);
    *(u32*)(sp + 0x10) = tmp;
    r10 = 0x0;
    fn_8023A118();
    _ctx = var_r20;

    return;
}
#pragma optimization_level 4

/* Address: 0x8023C370 | Size: 0x1C0 (448 bytes) */
#pragma optimization_level 0
void fn_8023C370(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8011BEB4();
    extern void fn_801F1C18();
    extern void fn_801FB1C0();
    extern u8 jumptable_8039A598[];
    extern void fn_80253948();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 r12 = 0;
    u32 var_r24 = 0;
    u32 var_r25 = 0;
    u32 var_r26 = 0;
    u32 var_r27 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;
    u32 _param3 = 0;
    void (*ctr_fn)(void) = 0;

    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    _ctx = _ctx;
    var_r24 = r7;
    _param1 = 0x0;
    _param2 = 0x43;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2d;
    _param3 = 0x0;
    fn_801FB1C0();
    tmp = _ctx & 0xFF;
    if (tmp != 1) { _ctx = 0x0; return; }
    _param1 = _param2;
    _ctx = 0x0;
    _param2 = 0x1e;
    _param3 = 0x0;
    fn_8011BEB4();
    /* mr. var_r26, _ctx */;
    do {
    if (tmp != 1) break;

    _ctx = (u32)fn_80253948;
    tmp = (u32)fn_80253948;
    var_r26 = tmp;
    break;

    _ctx = 0x0;
    return;
    } while (0);

    _param1 = _ctx;
    _param2 = (u32)sp + 0x8;
    _ctx = 0x0;
    _param3 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    var_r27 = _ctx;
    _param1 = _param2;
    _ctx = 0x0;
    _param2 = 0x5;
    _param3 = 0x0;
    fn_8011BEB4();
    tmp = var_r24 & 0xFF;
    _param1 = _ctx & 0xFF;
    var_r24 = 0x0;
    if (tmp == 1) {
        if (_param1 > 7) { _ctx = var_r24; return; }
        _ctx = (u32)jumptable_8039A598;
        tmp = _param1 << 2;
        _ctx = (u32)jumptable_8039A598;
        tmp = *(u32*)(_ctx + tmp);
        ctr_fn = (void(*)(void))tmp;
        r12 = var_r26;
        _ctx = _ctx;
        _param1 = _param1;
        _param2 = _param2;
        _param3 = _param3;
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
        var_r24 = _ctx;
        _ctx = var_r24;
        return;
        var_r27 = var_r27 & 0xFFFF;
        var_r25 = 0x0;
        while (1) {
            tmp = var_r25 & 0xFFFF;
            if (tmp >= var_r27) break;
            r12 = var_r26;
            _ctx = _ctx;
            _param1 = _param1;
            _param2 = _param2;
            _param3 = _param3;
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
            var_r24 = var_r24 + _ctx;
            var_r25 = var_r25 + 0x1;

        }
        _ctx = var_r24;
        return;
        r12 = var_r26;
        _ctx = _ctx;
        _param1 = _param1;
        _param2 = _param2;
        _param3 = _param3;
        ctr_fn = (void(*)(void))r12;
        ctr_fn();
        var_r24 = _ctx;
        _ctx = var_r24;
        return;
    }
    r12 = var_r26;
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    var_r24 = _ctx;

    _ctx = var_r24;

    return;
}
#pragma optimization_level 4

/* Address: 0x8023C530 | Size: 0x56C (1388 bytes) */
#pragma optimization_level 0
void fn_8023C530(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8011BEB4();
    extern void fn_801F1C18();
    extern void fn_801FB1C0();
    extern void fn_802026E4();
    extern void fn_802062FC();
    extern void fn_80237F74();
    extern void fn_80239500();
    extern void fn_802395C8();
    extern u8 jumptable_8039A5B8[];
    extern void fn_8025C25C();
    u8 sp[0x50];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 r12 = 0;
    u32 var_r22 = 0;
    u32 var_r23 = 0;
    u32 var_r24 = 0;
    u32 var_r25 = 0;
    u32 var_r26 = 0;
    u32 var_r27 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;
    u32 _param3 = 0;
    void (*ctr_fn)(void) = 0;

    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    _ctx = _ctx;
    _param1 = 0x0;
    _param2 = 0x43;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x34;
    _param3 = 0x0;
    fn_801FB1C0();
    tmp = _ctx & 0xFF;
    if (tmp != 1) { _ctx = 0x1; return; }
    _param1 = _param2;
    _ctx = 0x0;
    _param2 = 0x1d;
    _param3 = 0x0;
    fn_8011BEB4();
    /* mr. var_r25, _ctx */;
    do {
    if (tmp != 1) break;

    _ctx = (u32)fn_8025C25C;
    tmp = (u32)fn_8025C25C;
    var_r25 = tmp;
    break;

    _ctx = 0x1;
    return;
    } while (0);

    _param1 = _ctx;
    _param2 = (u32)sp + 0x8;
    _ctx = 0x0;
    _param3 = 0x0;
    r7 = 0x1;
    fn_801F1C18();
    var_r22 = _ctx;
    _param1 = _param2;
    _ctx = 0x0;
    _param2 = 0x5;
    _param3 = 0x0;
    fn_8011BEB4();
    tmp = _ctx & 0xFF;
    _ctx = 0x0;
    if (tmp > 7) return;
    _param1 = (u32)jumptable_8039A5B8;
    tmp = tmp << 2;
    _param1 = (u32)jumptable_8039A5B8;
    tmp = *(u32*)(_param1 + tmp);
    ctr_fn = (void(*)(void))tmp;
    _ctx = _ctx;
    _param1 = _param2;
    fn_80239500();
    var_r23 = _ctx;
    _ctx = _ctx;
    _param1 = _param2;
    _param2 = _param1;
    fn_802395C8();
    var_r22 = _ctx;
    _ctx = _param3;
    var_r27 = 0x1;
    fn_802062FC();
    tmp = _ctx & 0xFF;
    if (tmp == 7) {
        var_r27 = 0x0;

    } else {
    tmp = _param2 & 0xFFFF;
    if (tmp != 7 && tmp != 0x165) {

        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0xa;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            tmp = var_r22 & 0xFFFF;
            if (tmp == 0xd) {
                tmp = (s16)var_r23;
                if (tmp != 0xd) {
                    var_r27 = 0x0;
        }
        }
        }
        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0xb;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            tmp = var_r22 & 0xFFFF;
            if (tmp == 0xb) {
                tmp = (s16)var_r23;
                if (tmp != 0xb) {
                    var_r27 = 0x0;
        }
        }
        }
        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0x12;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            tmp = var_r22 & 0xFFFF;
            if (tmp == 0xa) {
                _ctx = _param3;
                _param1 = 0x7;
                fn_802026E4();
                tmp = _ctx & 0xFF;
                if (tmp == 0xa) {
                    var_r27 = 0x0;
        }
        }
        }
        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0x2b;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _param1 = _param2;
            _ctx = 0x0;
            _param2 = 0x17;
            _param3 = 0x0;
            fn_8011BEB4();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
                var_r27 = 0x0;
    }
    }
    }
    }
do {
    tmp = var_r27 & 0xFF;
    if (tmp == 1) {
        _ctx = 0x0;
        return;
    }
    r12 = var_r25;
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();
    return;
    var_r23 = var_r22 & 0xFFFF;
    var_r24 = 0x0;
    while (1) {
    do {
        tmp = var_r24 & 0xFFFF;
        if (tmp >= var_r23) break;
        _ctx = _ctx;
        _param1 = _param2;
        fn_80239500();
        var_r22 = _ctx;
        _ctx = _ctx;
        _param1 = _param2;
        _param2 = _param1;
        fn_802395C8();
        var_r27 = _ctx;
        _ctx = _param3;
        var_r26 = 0x1;
        fn_802062FC();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            var_r26 = 0x0;
            break;
        }
        tmp = _param2 & 0xFFFF;
        if (tmp == 1 || tmp == 0x165) break;

        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0xa;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            tmp = var_r27 & 0xFFFF;
            if (tmp == 0xd) {
                tmp = (s16)var_r22;
                if (tmp != 0xd) {
                    var_r26 = 0x0;
        }
        }
        }
        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0xb;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            tmp = var_r27 & 0xFFFF;
            if (tmp == 0xb) {
                tmp = (s16)var_r22;
                if (tmp != 0xb) {
                    var_r26 = 0x0;
        }
        }
        }
        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0x12;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            tmp = var_r27 & 0xFFFF;
            if (tmp == 0xa) {
                _ctx = _param3;
                _param1 = 0x7;
                fn_802026E4();
                tmp = _ctx & 0xFF;
                if (tmp == 0xa) {
                    var_r26 = 0x0;
        }
        }
        }
        _ctx = _ctx;
        _param1 = _param3;
        _param2 = 0x2b;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp != 1) break;
        _param1 = _param2;
        _ctx = 0x0;
        _param2 = 0x17;
        _param3 = 0x0;
        fn_8011BEB4();
        tmp = _ctx & 0xFF;
        if (tmp != 1) break;
        var_r26 = 0x0;
    } while (0);
        tmp = var_r26 & 0xFF;
        if (tmp == 1) {
            _ctx = 0x0;
        } else {

            r12 = var_r25;
            _param3 = (u32)sp + 0x8;
            _ctx = _ctx;
            _param1 = _param1;
            _param2 = _param2;
            _param3 = *(u32*)(_param3 + tmp);
            ctr_fn = (void(*)(void))r12;
            ctr_fn();
        }
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            return;
        }
        var_r24 = var_r24 + 0x1;

    }
    return;
    _ctx = _ctx;
    _param1 = _param2;
    fn_80239500();
    var_r27 = _ctx;
    _ctx = _ctx;
    _param1 = _param2;
    _param2 = _param1;
    fn_802395C8();
    var_r24 = _ctx;
    _ctx = _param3;
    var_r26 = 0x1;
    fn_802062FC();
    tmp = _ctx & 0xFF;
    if (tmp == var_r23) {
        var_r26 = 0x0;
        break;
    }
    tmp = _param2 & 0xFFFF;
    if (tmp == var_r23 || tmp == 0x165) break;

    _ctx = _ctx;
    _param1 = _param3;
    _param2 = 0xa;
    fn_80237F74();
    tmp = _ctx & 0xFF;
    if (tmp == 1) {
        tmp = var_r24 & 0xFFFF;
        if (tmp == 0xd) {
            tmp = (s16)var_r27;
            if (tmp != 0xd) {
                var_r26 = 0x0;
    }
    }
    }
    _ctx = _ctx;
    _param1 = _param3;
    _param2 = 0xb;
    fn_80237F74();
    tmp = _ctx & 0xFF;
    if (tmp == 1) {
        tmp = var_r24 & 0xFFFF;
        if (tmp == 0xb) {
            tmp = (s16)var_r27;
            if (tmp != 0xb) {
                var_r26 = 0x0;
    }
    }
    }
    _ctx = _ctx;
    _param1 = _param3;
    _param2 = 0x12;
    fn_80237F74();
    tmp = _ctx & 0xFF;
    if (tmp == 1) {
        tmp = var_r24 & 0xFFFF;
        if (tmp == 0xa) {
            _ctx = _param3;
            _param1 = 0x7;
            fn_802026E4();
            tmp = _ctx & 0xFF;
            if (tmp == 0xa) {
                var_r26 = 0x0;
    }
    }
    }
    _ctx = _ctx;
    _param1 = _param3;
    _param2 = 0x2b;
    fn_80237F74();
    tmp = _ctx & 0xFF;
    if (tmp != 1) break;
    _param1 = _param2;
    _ctx = 0x0;
    _param2 = 0x17;
    _param3 = 0x0;
    fn_8011BEB4();
    tmp = _ctx & 0xFF;
    if (tmp != 1) break;
    var_r26 = 0x0;
} while (0);
    tmp = var_r26 & 0xFF;
    if (tmp == 1) {
        _ctx = 0x0;
        return;
    }
    r12 = var_r25;
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();

    return;
}
#pragma optimization_level 4

/* Address: 0x8023CA9C | Size: 0xC4 (196 bytes) */
#pragma optimization_level 0
void fn_8023CA9C(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void fn_8011BEB4();
    extern void fn_801FB1C0();
    extern void fn_8024E52C();
    u32 tmp = 0;
    u32 r12 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;
    u32 _param3 = 0;
    void (*ctr_fn)(void) = 0;

    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    _ctx = _ctx;
    _param1 = 0x0;
    _param2 = 0x43;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x2;
    _param3 = 0x0;
    fn_801FB1C0();
    _param1 = _ctx & 0xFFFF;
    _ctx = 0x0;
    _param2 = 0x32;
    _param3 = 0x0;
    fn_801FB1C0();
    tmp = _ctx & 0xFF;
    if (tmp != 1) { _ctx = 0x0; return; }
    _param1 = _param2;
    _ctx = 0x0;
    _param2 = 0x1c;
    _param3 = 0x0;
    fn_8011BEB4();
    do {
    if (_ctx != 0) break;

    _ctx = (u32)fn_8024E52C;
    _ctx = (u32)fn_8024E52C;
    break;

    _ctx = 0x0;
    return;
    } while (0);

    r12 = _ctx;
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    ctr_fn = (void(*)(void))r12;
    ctr_fn();

    return;
}
#pragma optimization_level 4

/* Address: 0x8023CB60 | Size: 0x140 | Ghidra import */
u32 fn_8023CB60(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern s8 fn_801F1990();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u16 fn_802376EC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
    extern int fn_8023C370();
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  u32 uVar5;
  s8 cVar6;
  
  uVar1 = fn_802376EC();
  uVar2 = fn_8023C370(r3,r4,r5,r6,1);
  iVar3 = fn_801FB1C0(0,0x223,0x3e,0);
  iVar3 = ((int)((((int)uVar2 >> 1) + (u32)((int)uVar2 < 0 && (uVar2 & 1) != 0)) * 100) /
          (int)(uVar1 & 0xffff)) / iVar3;
  uVar4 = fn_802399FC(0,iVar3);
  uVar5 = fn_80205B8C(r4);
  fn_80239CCC(0xec64,r3,uVar5,0,0,r5,0,0x223,iVar3);
  cVar6 = fn_801F1990(0,r3,1,1,0x10e,r4);
  if (cVar6 == 1) {
    uVar4 = fn_80239984(uVar4,r3,0x224);
    uVar5 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar5,0,0,r5,0,0x224);
  }
  return uVar4;
}
/* Address: 0x8023CCA0 | Size: 0x12c | Ghidra import */
u32 fn_8023CCA0(u32 r3, u32 r4, u32 r5, u32 r6)

{
    extern u32 fn_801F025C();
    extern u8 fn_801F1990();
    extern u8 fn_801F6E98();
    extern u32 fn_80205B8C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u32 uVar3;
  u8 cVar2;
  u32 uVar1;

  uVar3 = 0;
  uVar1 = fn_801F025C(2,r6);
  cVar2 = fn_801F6E98(uVar1,0x49);
  if ((cVar2 == 1) || (cVar2 = fn_801F6E98(uVar1,0x48), cVar2 == 1)) {
    uVar3 = fn_80239984(0,r3,0x221);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x221);
  }
  cVar2 = fn_801F1990(0,r3,1,1,0x10e,r4);
  if (cVar2 == 1) {
    uVar3 = fn_80239984(uVar3,r3,0x222);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x222);
  }
  return uVar3;
}
/* Address: 0x8023CDCC | Size: 0x94 | Ghidra import */
u32 fn_8023CDCC(u32 arg0, u32 arg1, u32 arg2, u32 arg3)

{
    extern u32 fn_80205B8C();
    extern u16 fn_802377E8();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  if (fn_802377E8(arg0, arg3) == 0xd5) {
    uVar3 = fn_80239984(0, arg0, 0x220);
    uVar1 = fn_80205B8C(arg1);
    fn_80239EE8(0xec64, arg0, uVar1, 0, 0, arg2, 0, 0x220);
  }
  return uVar3;
}
/* Address: 0x8023CE60 | Size: 0x17c | Ghidra import */
#pragma optimization_level 2
u32 fn_8023CE60(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80205B8C();
    extern u16 fn_80236520();
    extern u16 fn_80236FFC();
    extern u16 fn_8023715C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern int fn_8023C530();
  u32 uVar6;
  u16 uVar2;
  u16 uVar3;
  u16 sVar4;
  u8 cVar5;
  u32 uVar1;

  uVar6 = 0;
  uVar2 = fn_80236FFC(r3,r6);
  uVar3 = fn_8023715C(r3,r6);
  sVar4 = fn_80236520(r3,r6);
  cVar5 = fn_8023C530(r3,r4,r5,r6);
  if (cVar5 == 1) {
    uVar6 = fn_80239984(0,r3,0x21d);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21d);
  }
  if (uVar3 < uVar2) {
    uVar6 = fn_80239984(uVar6,r3,0x21e);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21e);
  }
  if (sVar4 == 0x118) {
    uVar6 = fn_80239984(uVar6,r3,0x21f);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21f);
  }
  return uVar6;
}
#pragma optimization_level 4
/* Address: 0x8023CFDC | Size: 0x17c | Ghidra import */
u32 fn_8023CFDC(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_80205B8C();
    extern u16 fn_80236520();
    extern u16 fn_80236FFC();
    extern u16 fn_8023715C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern int fn_8023C530();
  u16 uVar2;
  u16 uVar3;
  u16 sVar4;
  u8 cVar5;
  u32 uVar1;
  u32 uVar6;
  
  uVar6 = 0;
  uVar2 = fn_80236FFC(r3,r6);
  uVar3 = fn_8023715C(r3,r6);
  sVar4 = fn_80236520(r3,r6);
  cVar5 = fn_8023C530(r3,r4,r5,r6);
  if (cVar5 == 1) {
    uVar6 = fn_80239984(0,r3,0x21a);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21a);
  }
  if (uVar2 < uVar3) {
    uVar6 = fn_80239984(uVar6,r3,0x21b);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21b);
  }
  if (sVar4 == 0x118) {
    uVar6 = fn_80239984(uVar6,r3,0x21c);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x21c);
  }
  return uVar6;
}
/* Address: 0x8023D158 | Size: 0x1cc | Ghidra import */
u32 fn_8023D158(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern u32 fn_801F1C18();
    extern u32 fn_80205B8C();
    extern u16 fn_80236520();
    extern u16 fn_80236FFC();
    extern u16 fn_8023715C();
    extern int fn_802377E8();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u16 uVar2;
  u16 uVar3;
  short sVar4;
  u16 uVar5;
  short sVar6;
  u32 uVar1;
  u16 uVar7;
  u32 uVar8;
  u32 local_48 [8];
  
  uVar8 = 0;
  uVar2 = fn_80236FFC(r3,r6);
  uVar3 = fn_8023715C(r3,r6);
  sVar4 = fn_80236520(r3,r6);
  uVar5 = fn_801F1C18(0,r3,local_48,0,1);
  uVar7 = 0;
  do {
    if (uVar5 <= uVar7) {
LAB_0023a260:
      if (uVar2 < uVar3) {
        uVar8 = fn_80239984(uVar8,r3,0x218);
        uVar1 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x218);
      }
      if (sVar4 == 0x11f) {
        uVar8 = fn_80239984(uVar8,r3,0x219);
        uVar1 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x219);
      }
      return uVar8;
    }
    sVar6 = fn_802377E8(r3,local_48[uVar7]);
    if ((((sVar6 == 0xca) || (sVar6 == 0x168)) || (sVar6 == 0x12f)) || (sVar6 == 0xd5)) {
      uVar8 = fn_80239984(0,r3,0x217);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x217);
      goto LAB_0023a260;
    }
    uVar7 = uVar7 + 1;
  } while (1);
}
/* Address: 0x8023D324 | Size: 0x15c | Ghidra import */
u32 fn_8023D324(void)

{
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;

    extern u32 fn_801F1C18();
    extern u32 fn_80205B8C();
    extern u16 fn_80236520();
    extern int fn_802377E8();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u16 uVar2;
  short sVar3;
  short sVar4;
  u32 uVar1;
  u16 uVar5;
  u32 uVar6;
  u32 local_48 [10];
  
  uVar6 = 0;
  uVar2 = fn_801F1C18(0,r3,local_48,0,1);
  sVar3 = fn_80236520(r3,r6);
  uVar5 = 0;
  do {
    if (uVar2 <= uVar5) {
LAB_0023a414:
      if (sVar3 == 0x11f) {
        uVar6 = fn_80239984(uVar6,r3,0x216);
        uVar1 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x216);
      }
      return uVar6;
    }
    sVar4 = fn_802377E8(r3,local_48[uVar5]);
    if ((((sVar4 == 0xca) || (sVar4 == 0x168)) || (sVar4 == 0x12f)) || (sVar4 == 0xd5)) {
      uVar6 = fn_80239984(0,r3,0x215);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x215);
      goto LAB_0023a414;
    }
    uVar5 = uVar5 + 1;
  } while (1);
}
/* Address: 0x8023D480 | Size: 0x90 | Ghidra import */
u32 fn_8023D480(u32 r3, u32 r4, u32 r5)

{
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u8 cVar2;
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  cVar2 = fn_80235714();
  if (cVar2 == 1) {
    uVar3 = fn_80239984(0,r3,0x214);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x214);
  }
  return uVar3;
}
/* Address: 0x8023D510 | Size: 0x23c | Ghidra import */
u32 fn_8023D510(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u32 fn_802367CC();
    extern u32 fn_802373B0();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern f32 lbl_8047E630;
  u32 bVar1;

  s8 cVar5;
  u32 uVar2;
  u16 uVar3;
  u16 uVar4;
  u16 uVar6;
  u16 uVar7;
  u32 uVar8;
  int local_58 [8];
  short local_38 [14];
  
  uVar8 = 0;
  cVar5 = fn_80235714();
  if (cVar5 == 0) {
    uVar8 = fn_80239984(0,r3,0x210);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x210);
  }
  uVar3 = fn_801F1C18(0,r3,local_58,1,1);
  uVar7 = 0;
  do {
    if (uVar3 <= uVar7) {
      bVar1 = 0;
LAB_0023a61c:
      if (bVar1) {
        uVar8 = fn_80239984(uVar8,r3,0x211);
        uVar2 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x211);
      }
      cVar5 = fn_80235714(r3,r4);
      if (cVar5 == 1) {
        uVar8 = fn_80239984(uVar8,r3,0x212);
        uVar2 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x212);
      }
      cVar5 = fn_802373B0((double)lbl_8047E630,r3,r4, (void*)0xffffffff);
      if (cVar5 == 1) {
        uVar8 = fn_80239984(uVar8,r3,0x213);
        uVar2 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x213);
      }
      return uVar8;
    }
    if (r4 != local_58[uVar7]) {
      uVar4 = fn_802367CC(r3,local_58[uVar7],local_38,0,1);
      if (uVar4 != 0) {
        for (uVar6 = 0; uVar6 < uVar4; uVar6 = uVar6 + 1) {
          if (local_38[uVar6] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023a61c;
          }
        }
      }
    }
    uVar7 = uVar7 + 1;
  } while (1);
}
/* Address: 0x8023D74C | Size: 0x200 | Ghidra import */
u32 fn_8023D74C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235A3C();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u16 uVar4;
  u32 uVar1;
  s8 cVar5;
  u32 uVar2;
  int iVar3;
  u16 uVar6;
  u32 uVar7;
  int iVar8;
  short local_30 [12];
  
  uVar4 = fn_802367CC(r3,r4,local_30,0,1);
  uVar1 = fn_80235A3C(r3,r4);
  uVar7 = 0;
  cVar5 = fn_80235714(r3,r4);
  if (cVar5 == 0) {
    uVar7 = fn_80239984(0,r3,0x20c);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20c);
  }
  cVar5 = fn_80235714(r3,r4);
  if (cVar5 == 1) {
    uVar7 = fn_80239984(uVar7,r3,0x20e);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20e);
  }
  iVar8 = (uVar1 & 0xff) - 6;
  if (iVar8 < 0) {
    iVar8 = 0;
  }
  iVar3 = fn_801FB1C0(0,0x20f,0x3e,0);
  uVar7 = fn_802399FC(uVar7,iVar8 * iVar3);
  uVar2 = fn_80205B8C(r4);
  fn_80239CCC(0xec64,r3,uVar2,0,0,r5,0,0x20f,iVar8 * iVar3);
  uVar6 = 0;
  while (1) {
    if (uVar4 <= uVar6) {
      return uVar7;
    }
    if ((local_30[uVar6] == 0xcd) || (local_30[uVar6] == 0x12d)) break;
    uVar6 = uVar6 + 1;
  }
  uVar7 = fn_80239984(uVar7,r3,0x20d);
  uVar2 = fn_80205B8C(r4);
  fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20d);
  return uVar7;
}
/* Address: 0x8023D94C | Size: 0x24c | Ghidra import */
u32 fn_8023D94C(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235974();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235974();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x208);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x208);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023aa68:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x209);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x209);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x20a);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x20a);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x20b,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x20b,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023aa68;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023DB98 | Size: 0x24c | Ghidra import */
u32 fn_8023DB98(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235910();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235910();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x204);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x204);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023acb4:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x205);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x205);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x206);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x206);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x207,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x207,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023acb4;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023DDE4 | Size: 0x24c | Ghidra import */
u32 fn_8023DDE4(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235AA0();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235AA0();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x200);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x200);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023af00:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x201);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x201);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x202);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x202);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x203,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x203,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023af00;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023E030 | Size: 0x24c | Ghidra import */
u32 fn_8023E030(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235974();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235974();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x1fc);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1fc);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023b14c:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1fd);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1fd);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1fe);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1fe);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x1ff,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x1ff,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023b14c;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023E27C | Size: 0x24c | Ghidra import */
u32 fn_8023E27C(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_802357CC();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_802357CC();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x1f8);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f8);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023b398:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1f9);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f9);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1fa);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1fa);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x1fb,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x1fb,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023b398;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023E4C8 | Size: 0x24c | Ghidra import */
u32 fn_8023E4C8(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235974();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235974();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,500);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,500);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023b5e4:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1f5);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f5);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1f6);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f6);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x1f7,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x1f7,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023b5e4;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023E714 | Size: 0x24c | Ghidra import */
u32 fn_8023E714(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_802359D8();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_802359D8();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x1f0);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f0);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023b830:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1f1);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f1);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1f2);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1f2);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,499,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,499,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023b830;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023E960 | Size: 0x24c | Ghidra import */
u32 fn_8023E960(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235910();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235910();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x1ec);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1ec);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023ba7c:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1ed);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1ed);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1ee);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1ee);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x1ef,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x1ef,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023ba7c;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023EBAC | Size: 0x24c | Ghidra import */
u32 fn_8023EBAC(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235A3C();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235A3C();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x1e8);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1e8);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023bcc8:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1e9);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1e9);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1ea);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1ea);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x1eb,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x1eb,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023bcc8;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023EDF8 | Size: 0x24c | Ghidra import */
u32 fn_8023EDF8(void)

{
    u32 r3;
    int r4;
    u32 r5;

    extern u32 fn_801F1C18();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u8 fn_80235AA0();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 bVar1;
  u32 uVar2;
  s8 cVar7;
  u32 uVar3;
  u16 uVar5;
  u16 uVar6;
  int iVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar10;
  u32 uVar11;
  int local_60 [8];
  short local_40 [16];
  
  uVar2 = fn_80235AA0();
  uVar11 = 0;
  cVar7 = fn_80235714(r3,r4);
  if (cVar7 == 0) {
    uVar11 = fn_80239984(0,r3,0x1e4);
    uVar3 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1e4);
  }
  uVar5 = fn_801F1C18(0,r3,local_60,1,1);
  uVar9 = 0;
  do {
    if (uVar5 <= uVar9) {
      bVar1 = 0;
LAB_0023bf14:
      if (bVar1) {
        uVar11 = fn_80239984(uVar11,r3,0x1e5);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1e5);
      }
      cVar7 = fn_80235714(r3,r4);
      if (cVar7 == 1) {
        uVar11 = fn_80239984(uVar11,r3,0x1e6);
        uVar3 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar3,0,0,r5,0,0x1e6);
      }
      iVar10 = (uVar2 & 0xff) - 6;
      if (iVar10 < 0) {
        iVar10 = 0;
      }
      iVar4 = fn_801FB1C0(0,0x1e7,0x3e,0);
      uVar11 = fn_802399FC(uVar11,iVar10 * iVar4);
      uVar3 = fn_80205B8C(r4);
      fn_80239CCC(0xec64,r3,uVar3,0,0,r5,0,0x1e7,iVar10 * iVar4);
      return uVar11;
    }
    if (r4 != local_60[uVar9]) {
      uVar6 = fn_802367CC(r3,local_60[uVar9],local_40,0,1);
      if (uVar6 != 0) {
        for (uVar8 = 0; uVar8 < uVar6; uVar8 = uVar8 + 1) {
          if (local_40[uVar8] == 0x10a) {
            bVar1 = 1;
            goto LAB_0023bf14;
          }
        }
      }
    }
    uVar9 = uVar9 + 1;
  } while (1);
}
/* Address: 0x8023F044 | Size: 0x100 | Ghidra import */
u32 fn_8023F044(u32 r3, u32 r4, u32 r5, u32 r6)

{
    extern u32 fn_80205B8C();
    extern u32 fn_80237F74();
    extern u16 fn_8023831C();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u16 sVar2;
  u32 uVar1;
  u8 cVar3;
  u32 uVar4;

  uVar4 = 0;
  sVar2 = fn_8023831C();
  if ((sVar2 == 0x1d) || (sVar2 == 0x18)) {
    uVar4 = fn_80239984(0,r3,0x1e2);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1e2);
  }
  cVar3 = fn_80237F74(r3,r6,0x3c);
  if (cVar3 == 1) {
    uVar4 = fn_80239984(uVar4,r3,0x1e3);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1e3);
  }
  return uVar4;
}
/* Address: 0x8023F144 | Size: 0x134 | Ghidra import */
u32 fn_8023F144(u32 arg0, u32 arg1, u32 arg2, u32 arg3)

{
    extern u32 fn_80205B8C();
    extern u8 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u8 cVar2;
  u32 uVar1;
  u32 uVar3;

  uVar3 = 0;
  cVar2 = fn_80237F74(arg0,arg1,0x36);
  if ((cVar2 == 1) || (cVar2 = fn_80237F74(arg0,arg1,0x10), cVar2 == 1)) {
    uVar3 = fn_80239984(0,arg0,0x1e0);
    uVar1 = fn_80205B8C(arg1);
    fn_80239EE8(0xec64,arg0,uVar1,0,0,arg2,0,0x1e0);
  }
  cVar2 = fn_80237F74(arg0,arg3,0x36);
  if ((cVar2 == 1) || (cVar2 = fn_80237F74(arg0,arg3,0x10), cVar2 == 1)) {
    uVar3 = fn_80239984(uVar3,arg0,0x1e1);
    uVar1 = fn_80205B8C(arg1);
    fn_80239EE8(0xec64,arg0,uVar1,0,0,arg2,0,0x1e1);
  }
  return uVar3;
}
/* Address: 0x8023F278 | Size: 0x648 (1608 bytes) */
#pragma optimization_level 0
void fn_8023F278(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 lbl_80478DF8;
    extern void fn_8011BEB4();
    extern void fn_801F1990();
    extern void fn_801F1C18();
    extern void fn_80205B8C();
    extern void fn_80236520();
    extern void fn_802367CC();
    extern void fn_80237DBC();
    extern void fn_80237F74();
    extern void fn_8023943C();
    extern void fn_80239984();
    extern void fn_80239EE8();
    extern void fn_8024B474();
    extern void fn_8024BFC0();
    extern void fn_8024E52C();
    u8 sp[0x90];
    u32 tmp = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 _param3 = 0;
    u32 _ctx = 0;
    u32 _param1 = 0;
    u32 _param2 = 0;
    u32 var_r18 = 0;
    u32 var_r19 = 0;
    u32 var_r20 = 0;
    u32 var_r21 = 0;
    u32 var_r22 = 0;
    u32 var_r23 = 0;
    u32 var_r24 = 0;
    u32 var_r25 = 0;
    u32 var_r27 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;

    r7 = 0x1;
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = _param2;
    _param3 = _param3;
    _param1 = _ctx;
    _param2 = (u32)sp + 0x1c;
    var_r25 = 0x0;
    var_r21 = 0x0;
    var_r20 = 0x0;
    var_r19 = 0x0;
    var_r18 = 0x0;
    _ctx = 0x0;
    _param3 = 0x1;
    fn_801F1C18();
    _ctx = _ctx;
    _ctx = _ctx;
    _param1 = _param1;
    fn_80236520();
    tmp = _ctx;
    _ctx = _ctx;
    var_r22 = tmp;
    _param1 = _param3;
    fn_80236520();
    _param3 = (u32)sp + 0x1c;
    var_r31 = _ctx & 0xFFFF;
    var_r24 = 0x0;
    while (1) {
        tmp = var_r24 & 0xFFFF;
        if (tmp >= var_r31) break;
        _param1 = *(u32*)(_param3 + tmp);
        do {
        if (_param1 == _param1) break;

        _ctx = _ctx;
        _param2 = (u32)sp + 0x8;
        _param3 = 0x0;
        r7 = 0x1;
        fn_802367CC();
        var_r27 = _ctx & 0xFFFF;
        if (_param1 == _param1) break;

        _param1 = (u32)fn_8024B474;
        _ctx = (u32)fn_8024BFC0;
        _param2 = (u32)fn_8024E52C;
        var_r23 = 0x0;
        var_r29 = (u32)fn_8024B474;
        var_r30 = (u32)fn_8024BFC0;
        var_r28 = (u32)fn_8024E52C;
        while (1) {
            tmp = var_r23 & 0xFFFF;
            if (tmp >= var_r27) break;
            _ctx = (u32)sp + 0x8;
            _param1 = *(u16*)(_ctx + tmp);
            _ctx = 0x0;
            _param2 = 0x1c;
            _param3 = 0x0;
            fn_8011BEB4();
            if (_ctx == 0) {
                _ctx = var_r28;
            }
            if (_ctx == var_r29) {
                var_r21 = 0x1;
            }
            if (_ctx == var_r30) {
                var_r20 = 0x1;
            }
            var_r23 = var_r23 + 0x1;

        }
        } while (0);

        var_r24 = var_r24 + 0x1;

    }
    _ctx = (u32)fn_8024B474;
    var_r23 = 0x0;
    _param3 = (u32)fn_8024B474;
    while (1) {
        _ctx = lbl_80478DF8;
        _param1 = var_r23 & 0xFFFF;
        tmp = *(u32*)((u8*)_ctx + 0x0);
        if (_param1 >= tmp) break;
        _param1 = var_r23;
        _ctx = 0x0;
        _param2 = 0x1c;
        _param3 = 0x0;
        fn_8011BEB4();
        /* mr. var_r24, _ctx */;
        if (tmp == var_r31) {
            _ctx = (u32)fn_8024E52C;
            var_r24 = (u32)fn_8024E52C;
        }
        if (var_r24 != _param3) {
            _ctx = (u32)fn_8024BFC0;
            tmp = (u32)fn_8024BFC0;
            if (var_r24 == tmp && tmp == 1 && var_r24 == tmp) {
            }
            _param1 = _ctx;
            r7 = var_r23;
            _ctx = 0x0;
            _param2 = 0x1;
            _param3 = 0x1;
            r8 = 0x0;
            fn_801F1990();
            tmp = _ctx & 0xFF;

            _ctx = (u32)fn_8024B474;
            tmp = (u32)fn_8024B474;
            if (var_r24 == tmp) {
                var_r19 = 0x1;
            }
            _ctx = (u32)fn_8024BFC0;
            tmp = (u32)fn_8024BFC0;

            var_r18 = 0x1;
            }
        var_r23 = var_r23 + 0x1;

    }
    tmp = var_r21 & 0xFF;
    if (tmp != 1) {
        tmp = var_r20 & 0xFF;
        if (tmp == 1) {
        }
        _param1 = _ctx;
        _ctx = 0x0;
        _param2 = 0x1d7;
        fn_80239984();
        tmp = _ctx;
        _ctx = _param1;
        var_r25 = tmp;
        fn_80205B8C();
        _param3 = 0x10000;
        _param2 = _ctx;
        _param1 = _ctx;
        r8 = _param2;
        _param3 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1d7;
        fn_80239EE8();
        }
    tmp = var_r19 & 0xFF;
    if (tmp != 1) {
        tmp = var_r18 & 0xFF;
        if (tmp == 1) {
        }
        _ctx = var_r25;
        _param1 = _ctx;
        _param2 = 0x1d8;
        fn_80239984();
        tmp = _ctx;
        _ctx = _param1;
        var_r25 = tmp;
        fn_80205B8C();
        _param3 = 0x10000;
        _param2 = _ctx;
        _param1 = _ctx;
        r8 = _param2;
        _param3 = 0x0;
        r7 = 0x0;
        r9 = 0x0;
        r10 = 0x1d8;
        fn_80239EE8();
        }
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = 0x3;
    fn_80237F74();
    tmp = _ctx & 0xFF;
    if (tmp == 1) {
        _ctx = _ctx;
        _param1 = var_r22;
        _param2 = 0x4;
        fn_8023943C();
        tmp = _ctx & 0xFF;
        if (tmp == 1) {
            _ctx = var_r25;
            _param1 = _ctx;
            _param2 = 0x1d9;
            fn_80239984();
            tmp = _ctx;
            _ctx = _param1;
            var_r25 = tmp;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = _ctx;
            r8 = _param2;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x1d9;
            fn_80239EE8();
    }
    }
    tmp = var_r22 & 0xFFFF;

    do {
    if (tmp != 0xb6 && tmp != 0xc5) break;

    _ctx = _param1;
    _param1 = 0x0;
    _param2 = 0xfc;
    _param3 = 0x0;
    ((void(*)(void))fn_8012640C)();
    if ((s32)_ctx == 0) break;

    _ctx = var_r25;
    _param1 = _ctx;
    _param2 = 0x1da;
    fn_80239984();
    tmp = _ctx;
    _ctx = _param1;
    var_r25 = tmp;
    fn_80205B8C();
    _param3 = 0x10000;
    _param2 = _ctx;
    _param1 = _ctx;
    r8 = _param2;
    _param3 = 0x0;
    r7 = 0x0;
    r9 = 0x0;
    r10 = 0x1da;
    fn_80239EE8();
    } while (0);

    _ctx = _ctx;
    _param1 = _param1;
    _param2 = 0x7;
    fn_80237DBC();
    tmp = _ctx & 0xFF;
    if (tmp == 1) {
        tmp = var_r20 & 0xFF;
        if (tmp == 1) {
            _ctx = var_r25;
            _param1 = _ctx;
            _param2 = 0x1db;
            fn_80239984();
            tmp = _ctx;
            _ctx = _param1;
            var_r25 = tmp;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = _ctx;
            r8 = _param2;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x1db;
            fn_80239EE8();
        }
        tmp = var_r18 & 0xFF;
        if (tmp == 1) {
            _ctx = var_r25;
            _param1 = _ctx;
            _param2 = 0x1dc;
            fn_80239984();
            tmp = _ctx;
            _ctx = _param1;
            var_r25 = tmp;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = _ctx;
            r8 = _param2;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x1dc;
            fn_80239EE8();
    }
    }
    _ctx = _ctx;
    _param1 = _param1;
    _param2 = 0x2;
    fn_80237DBC();
    tmp = _ctx & 0xFF;
    if (tmp != 1) {
        _ctx = _ctx;
        _param1 = _param1;
        _param2 = 0x1a;
        fn_80237F74();
        tmp = _ctx & 0xFF;
        if (tmp != 1) {
            _ctx = _ctx;
            _param1 = _param1;
            _param2 = 0x19;
            fn_80237F74();
            tmp = _ctx & 0xFF;
            if (tmp == 1) {
        }
        }
        tmp = var_r21 & 0xFF;
        if (tmp == 1) {
            _ctx = var_r25;
            _param1 = _ctx;
            _param2 = 0x1dd;
            fn_80239984();
            tmp = _ctx;
            _ctx = _param1;
            var_r25 = tmp;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = _ctx;
            r8 = _param2;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x1dd;
            fn_80239EE8();
        }
        tmp = var_r19 & 0xFF;
        if (tmp == 1) {
            _ctx = var_r25;
            _param1 = _ctx;
            _param2 = 0x1de;
            fn_80239984();
            tmp = _ctx;
            _ctx = _param1;
            var_r25 = tmp;
            fn_80205B8C();
            _param3 = 0x10000;
            _param2 = _ctx;
            _param1 = _ctx;
            r8 = _param2;
            _param3 = 0x0;
            r7 = 0x0;
            r9 = 0x0;
            r10 = 0x1de;
            fn_80239EE8();
        }
            }
    _param3 = (u32)sp + 0x1c;
    var_r19 = _ctx & 0xFFFF;
    var_r18 = 0x0;
    while (1) {
        tmp = var_r18 & 0xFFFF;
        if (tmp >= var_r19) break;
        _param1 = *(u32*)(_param3 + tmp);
        do {
        if (_param1 == _param1) break;

        _ctx = _ctx;
        _param2 = (u32)sp + 0x8;
        _param3 = 0x0;
        r7 = 0x1;
        fn_802367CC();
        _param1 = _ctx & 0xFFFF;
        var_r20 = _ctx;
        if (_param1 == _param1) break;

        _ctx = (u32)sp + 0x8;
        var_r21 = 0x0;
        while (1) {
            tmp = var_r21 & 0xFFFF;
            if (tmp >= _param1) break;
            tmp = *(u16*)(_ctx + tmp);
            if (tmp == 0x10a) {
                _ctx = var_r25;
                _param1 = _ctx;
                _param2 = 0x1df;
                fn_80239984();
                tmp = _ctx;
                _ctx = _param1;
                var_r25 = tmp;
                fn_80205B8C();
                _param3 = 0x10000;
                _param2 = _ctx;
                _param1 = _ctx;
                r8 = _param2;
                _param3 = 0x0;
                r7 = 0x0;
                r9 = 0x0;
                r10 = 0x1df;
                fn_80239EE8();
                break;
            }
            var_r21 = var_r21 + 0x1;

        }

        _ctx = var_r21 & 0xFFFF;
        tmp = var_r20 & 0xFFFF;
        if (_ctx < tmp) { _ctx = var_r25; return; }
        } while (0);

        var_r18 = var_r18 + 0x1;

    }

    _ctx = var_r25;
    return;
}
#pragma optimization_level 4

/* Address: 0x8023F8C0 | Size: 0x15c | Ghidra import */
u32 fn_8023F8C0(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern short fn_80202360();
    extern u32 fn_80205B8C();
    extern u32 fn_80236BFC();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  s8 cVar3;
  short sVar2;
  u32 uVar1;
  u32 uVar4;
  
  uVar4 = 0;
  cVar3 = fn_80236BFC(r3,r4,0x2d);
  if (cVar3 == 1) {
    sVar2 = fn_80202360(r4,0x2d);
  }
  else {
    sVar2 = 0;
  }
  if (sVar2 == 0) {
    uVar4 = fn_80239984(0,r3,0x1d4);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d4);
  }
  else if (sVar2 == 1) {
    uVar4 = fn_80239984(0,r3,0x1d5);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d5);
  }
  else if (sVar2 == 2) {
    uVar4 = fn_80239984(0,r3,0x1d6);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d6);
  }
  return uVar4;
}
/* Address: 0x8023FA1C | Size: 0x140 | Ghidra import */
u32 fn_8023FA1C(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern u32 fn_80205B8C();
    extern u32 fn_802367CC();
    extern u32 fn_80236BFC();
    extern int fn_802395C8();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u16 uVar2;
  short sVar3;
  s8 cVar4;
  u32 uVar1;
  u16 uVar5;
  u32 uVar6;
  short local_38 [14];
  
  uVar6 = 0;
  uVar2 = fn_802367CC(r3,r4,local_38,0,1);
  uVar5 = 0;
  do {
    if (uVar2 <= uVar5) {
LAB_0023cae0:
      cVar4 = fn_80236BFC(r3,r4,0x24);
      if (cVar4 == 1) {
        uVar6 = fn_80239984(uVar6,r3,0x1d3);
        uVar1 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d3);
      }
      return uVar6;
    }
    if ((local_38[uVar5] != 0x10c) &&
       (sVar3 = fn_802395C8(r3,local_38[uVar5],r4), sVar3 == 0xd)) {
      uVar6 = fn_80239984(0,r3,0x1d2);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d2);
      goto LAB_0023cae0;
    }
    uVar5 = uVar5 + 1;
  } while (1);
}
/* Address: 0x8023FB5C | Size: 0x1e8 | Ghidra import */
u32 fn_8023FB5C(u32 r3, u32 r4, u32 r5)

{
    extern int fn_80202108();
    extern s8 fn_80202234();
    extern u32 fn_80205B8C();
    extern u32 fn_80236BFC();
    extern u32 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u8 cVar3;
  u32 uVar1;
  u32 uVar2;
  s8 cVar4;
  s8 cVar6;
  int iVar7;
  int iVar8;
  u32 uVar5;

  uVar5 = 0;
  cVar3 = fn_80236BFC(r3,r4,8);
  if (cVar3 == 1) {
    cVar3 = fn_80236BFC(r3,r4,8);
    if (cVar3 == 0) {
      cVar6 = -1;
    }
    else {
      uVar2 = fn_80237F74(r3,r4,0x30);
      iVar7 = ((u32)__cntlzw(1 - (uVar2 & 0xff)) >> 5) + 1;
      iVar8 = fn_80202108(r4,8);
      iVar7 = iVar8 + iVar7;
      if ((s8)iVar7 >= (cVar4 = fn_80202234(r4,8))) {
        cVar6 = 1;
      }
      else {
        cVar6 = 0;
      }
    }
    if (cVar6 == 0) {
      uVar5 = fn_80239984(0,r3,0x1d0);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d0);
    }
    cVar3 = fn_80236BFC(r3,r4,8);
    if (cVar3 == 0) {
      cVar6 = -1;
    }
    else {
      uVar2 = fn_80237F74(r3,r4,0x30);
      iVar7 = ((u32)__cntlzw(1 - (uVar2 & 0xff)) >> 5) + 1;
      iVar8 = fn_80202108(r4,8);
      iVar7 = iVar8 + iVar7;
      if ((s8)iVar7 >= (cVar4 = fn_80202234(r4,8))) {
        cVar6 = 1;
      }
      else {
        cVar6 = 0;
      }
    }
    if (cVar6 == 1) {
      uVar5 = fn_80239984(uVar5,r3,0x1d1);
      uVar1 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1d1);
    }
  }
  return uVar5;
}
/* Address: 0x8023FD44 | Size: 0x98 | Ghidra import */
u32 fn_8023FD44(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern u32 fn_80205B8C();
    extern u32 fn_802373B0();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern f32 lbl_8047E634;

  s8 cVar2;
  u32 uVar1;
  u32 uVar3;
  
  uVar3 = 0;
  cVar2 = fn_802373B0((double)lbl_8047E634,r3,r4,1);
  if (cVar2 == 1) {
    uVar3 = fn_80239984(0,r3,0x1cf);
    uVar1 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar1,0,0,r5,0,0x1cf);
  }
  return uVar3;
}
/* Address: 0x8023FDDC | Size: 0x2fc | Ghidra import */
u32 fn_8023FDDC(u32 r3, int r4, u32 r5, u32 r6)
{
    extern u32 fn_801F1C18();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern u32 fn_80205B8C();
    extern u32 fn_802367CC();
    extern u32 fn_80236BFC();
    extern u32 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u16 uVar3;
  u16 uVar4;
  s8 cVar5;
  u32 uVar1;
  s8 cVar6;
  u32 uVar2;
  int iVar7;
  u16 uVar8;
  u32 uVar9;
  u16 uVar10;
  short local_68 [10];
  int local_54 [10];
  
  uVar9 = 0;
  uVar3 = fn_801F1C18(0,r3,local_54,1,1);
  for (uVar10 = 0; uVar10 < uVar3; uVar10 = uVar10 + 1) {
    iVar7 = local_54[uVar10];
    if (r4 != iVar7) {
      cVar5 = fn_80236BFC(r3,iVar7,8);
      if (cVar5 == 0) {
        cVar5 = -1;
      }
      else {
        uVar1 = fn_80237F74(r3,iVar7,0x30);
        uVar1 = __cntlzw(1 - (uVar1 & 0xff));
        cVar5 = fn_80202108(iVar7,8);
        cVar6 = fn_80202234(iVar7,8);
        if ((char)(cVar5 + (char)(uVar1 >> 5) + 1) < cVar6) {
          cVar5 = 0;
        }
        else {
          cVar5 = 1;
        }
      }
      if (cVar5 == 0) {
        uVar9 = fn_80239984(0,r3,0x1cc);
        uVar2 = fn_80205B8C(r4);
        fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1cc);
        break;
      }
    }
  }
  uVar10 = 0;
  while (1) {
    if (uVar3 <= uVar10) break;
    if (r4 != local_54[uVar10]) {
      uVar4 = fn_802367CC(r3,local_54[uVar10],local_68,0,1);
      if (uVar4 != 0) {
        for (uVar8 = 0; uVar8 < uVar4; uVar8 = uVar8 + 1) {
          if (local_68[uVar8] == 0x9c) {
            uVar9 = fn_80239984(uVar9,r3,0x1cd);
            uVar2 = fn_80205B8C(r4);
            fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1cd);
            break;
          }
        }
        if (uVar8 < uVar4) break;
      }
    }
    uVar10 = uVar10 + 1;
  }
  cVar5 = fn_80236BFC(r3,r6,8);
  if (cVar5 == 0) {
    cVar5 = -1;
  }
  else {
    uVar1 = fn_80237F74(r3,r6,0x30);
    uVar1 = __cntlzw(1 - (uVar1 & 0xff));
    cVar5 = fn_80202108(r6,8);
    cVar6 = fn_80202234(r6,8);
    if ((char)(cVar5 + (char)(uVar1 >> 5) + 1) < cVar6) {
      cVar5 = 0;
    }
    else {
      cVar5 = 1;
    }
  }
  if (cVar5 == 0) {
    uVar9 = fn_80239984(uVar9,r3,0x1ce);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1ce);
  }
  return uVar9;
}
/* #######################################################################
 * COVERAGE STUBS: Colosseum script system (0x80212000 - 0x80220000)
 * 212 functions remaining for full coverage of the target range
 * within colosseum_script.c TU.
 *
 * Key functions in this range:
 *   fn_80212D6C - Colosseum round transition handler
 *   fn_80213xxx - Event state management helpers
 *   fn_80214xxx - Scene transition helpers
 *   fn_80215xxx-0x80220000 - Colosseum match setup, team validation
 * ####################################################################### */

/* Address: 0x802126C4 | Size: 0x17c | Ghidra import */
int fn_802126C4(void)

{
    short r3;

    extern s8 fn_801437E0();
    extern s8 fn_80143878();
    extern s8 fn_801438A0();
    extern s8 fn_801438C8();
    extern s8 fn_801438F0();
    extern s8 fn_80143918();
    extern s8 fn_80143940();
    extern s8 fn_80143990();
    extern s8 fn_801439B8();
    extern s8 fn_801439D4();
    extern s8 fn_801439F0();
    extern s8 fn_80143A0C();
    extern s8 fn_80143A28();
    extern s8 fn_80143A44();
    extern int fn_80143A94();
    extern void fn_80143DFC();
    extern void fn_801440A0();
  int iVar1;
  s8 cVar2;
  
  fn_801440A0();
  fn_80143DFC();
  iVar1 = fn_80143A94();
  if (iVar1 == 0) {
    iVar1 = 7;
  }
  else if (r3 == 0x13) {
    iVar1 = 1;
  }
  else {
    cVar2 = fn_801437E0();
    if (cVar2 == 0) {
      cVar2 = fn_80143940(iVar1);
      if (((((cVar2 == 1) || (cVar2 = fn_80143918(iVar1), cVar2 == 1)) ||
           (cVar2 = fn_801438F0(iVar1), cVar2 == 1)) ||
          ((cVar2 = fn_801438C8(iVar1), cVar2 == 1 ||
           (cVar2 = fn_801438A0(iVar1), cVar2 == 1)))) ||
         (cVar2 = fn_80143878(iVar1), cVar2 == 1)) {
        iVar1 = 3;
      }
      else {
        cVar2 = fn_80143A44(iVar1);
        if (cVar2 == 1) {
          iVar1 = 4;
        }
        else {
          cVar2 = fn_80143A28(iVar1);
          if (((cVar2 == 0) && (cVar2 = fn_80143A0C(iVar1), cVar2 == 0)) &&
             ((cVar2 = fn_801439F0(iVar1), cVar2 == 0 &&
              ((cVar2 = fn_801439D4(iVar1), cVar2 == 0 &&
               (cVar2 = fn_801439B8(iVar1), cVar2 == 0)))))) {
            cVar2 = fn_80143990(iVar1);
            return 7 - (u32)(cVar2 == 1);
          }
          iVar1 = 5;
        }
      }
    }
    else {
      iVar1 = 2;
    }
  }
  return iVar1;
}
/* Address: 0x80212840 | Size: 0x90 | Ghidra import */
void fn_80212840(u32 r3)

{
    extern u8 fn_802026E4();
    extern void fn_80202810();
  u8 cVar1;

  cVar1 = fn_802026E4(r3,0x2e);
  if (cVar1 == 1) {
    fn_80202810(r3,0x2e);
  }
  cVar1 = fn_802026E4(r3,0x15);
  if (cVar1 == 1) {
    fn_80202810(r3,0x15);
  }
  cVar1 = fn_802026E4(r3,0x28);
  if (cVar1 == 1) {
    fn_80202810(r3,0x28);
  }
  return;
}
/* Address: 0x802128D0 | Size: 0x498 | Ghidra import */
void fn_802128D0(void)

{
    u32 r3;
    u32 r4;

    extern u32 fn_800FA280();
    extern short fn_8011BEB4();
    extern void fn_80132A38();
    extern void fn_801DA7AC();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern s8 fn_801F1170();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_80201890();
    extern u32 fn_80201C58();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_80204F6C();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern int fn_8022B2CC();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern u8 lbl_80478D7E;
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B644;
  u32 bVar1;
  u16 uVar10;
  short sVar11;
  u32 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  s8 cVar14;
  u32 uVar6;
  u16 uVar12;
  u32 uVar7;
  u32 uVar8;
  short sVar13;
  int iVar9;

  uVar10 = fn_801F54A4(0,0,0x14,0);
  sVar11 = fn_8011BEB4(0,r4,9,0);
  uVar2 = fn_801F025C(0x11,0);
  iVar3 = fn_801F025C(2,uVar2);
  iVar4 = fn_801F025C(0x12,0);
  iVar5 = fn_801F025C(2,iVar4);
  cVar14 = fn_802062FC(uVar2);
  if (cVar14 == 0) {
    return;
  }
  fn_8012640C(uVar2,0,0xfe,0);
  cVar14 = fn_801F1170();
  if (cVar14 == 0) {
    return;
  }
  fn_801F4C14(0,0,0x51,0,0);
  fn_801F4C14(0,0,0x52,0,0);
  fn_801F4C14(0,0,0x53,0,0);
  lbl_80478D7E = 0;
  lbl_8047B644 = 0;
  cVar14 = fn_802026E4(uVar2,0x2a);
  if ((cVar14 == 1) && ((r4 & 0xffff) != 0xa5)) {
    uVar6 = fn_80201C58(uVar2,0x2a);
    uVar12 = fn_80201890(uVar2,0x2a);
    uVar7 = fn_80205B8C(uVar2);
    r4 = (int)fn_8012640C(uVar7,0,0x7f,uVar12);
    r4 = r4 & 0xffff;
    if ((uVar6 & 0xffff) != r4) {
      fn_80202810(uVar2,0x2a);
    }
    uVar7 = fn_8022B2CC(uVar2,r4,uVar10,0,1,1, (void*)0xffffffff);
    uVar8 = fn_801F0134(uVar7,uVar10);
    fn_80204F6C(uVar2,0,0x13,0,0x80375ca8,r4,uVar8,(int)(char)uVar12,1);
    fn_801F4C14(0,0,0x42,0,uVar7);
    iVar4 = fn_801F025C(0x12,0);
    iVar5 = fn_801F025C(2,iVar4);
  }
  if (((r4 & 0xffff) == 0xa5) || ((r4 & 0xffff) == 0x164)) {
    lbl_8047B618 = lbl_8047B618 | 0x800;
  }
  sVar13 = fn_8011BEB4(0,r4,9,0);
  if ((((sVar13 == 0x91) || (sVar13 == 0x27)) || (sVar13 == 0x4b)) ||
     (((sVar13 == 0x97 || (sVar13 == 0x9b)) || (sVar13 == 0x1a)))) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if ((bVar1) && (cVar14 = fn_802026E4(uVar2,0x22), cVar14 == 0)) {
    if (((sVar11 == 0x97) && (cVar14 = (int)fn_801F453C(0,1), cVar14 == 1)) &&
       (iVar9 = fn_8022B2CC(uVar2,r4,uVar10,0,0,1, (void*)0xffffffff), iVar9 != 0)) {
      fn_801F4C14(0,0,0x43,0,iVar9);
      iVar5 = fn_801F025C(2,iVar9);
      iVar4 = iVar9;
    }
  }
  else {
    iVar9 = fn_8022B2CC(uVar2,r4,uVar10,0,0,1, (void*)0xffffffff);
    if (iVar9 != 0) {
      fn_801F4C14(0,0,0x43,0,iVar9);
      iVar5 = fn_801F025C(2,iVar9);
      iVar4 = iVar9;
    }
  }
  cVar14 = fn_802062FC(iVar4);
  if (cVar14 != 0) goto LAB_0020fce0;
  if (iVar3 == iVar5) {
    iVar4 = fn_801F025C(0xf,uVar2);
    cVar14 = fn_802062FC();
    if (cVar14 == 0) goto LAB_0020fc94;
  }
  else {
LAB_0020fc94:
    iVar4 = fn_801F025C(0xe,iVar4);
    if (iVar4 == 0) {
      return;
    }
    cVar14 = fn_802062FC();
    if (cVar14 == 0) {
      return;
    }
  }
  fn_801F4C14(0,0,0x43,0,iVar4);
  fn_801F025C(2,iVar4);
LAB_0020fce0:
  uVar6 = fn_8011BEB4(0,r4,9,0);
  lbl_8047B614 = 0;
  fn_8011BEB4(0,r4,1,0);
  uVar2 = fn_800FA280();
  fn_80132A38(0x28,uVar2);
  fn_80211B94(r3,*(u32 *)((uVar6 & 0xffff) * 4 + -0x7fc86401),1);
  fn_801DA7AC();
  fn_80261B68(0);
  fn_80261E7C(0);
  fn_8026246C();
  return;
}
/* Address: 0x80212D6C | Size: 0x3ec | Ghidra import */
void fn_80212D6C(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011F634();
    extern void fn_8011F910();
    extern u8 fn_8011FC74();
    extern void fn_80132A38();
    extern void fn_801DA7AC();
    extern void fn_801F000C();
    extern u32 fn_801F4354();
    extern u32 fn_801F54A4();
    extern u32 fn_801F8100();
    extern void fn_801FB974();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_80204A10();
    extern u32 fn_80205B8C();
    extern void fn_802080A8();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_8026532C();
    extern void fn_80265598();
  u16 uVar6;
  u32 uVar1;
  u32 uVar2;
  u8 cVar7;
  u32 uVar3;
  int uVar4;
  u32 uVar5;
  int iVar8;
  int iVar9;
  u32 uVar10;
  
  iVar9 = 0;
  iVar8 = 0;
  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F54A4(0,0,0x36,0);
  uVar2 = fn_801F4354(0,uVar1);
  cVar7 = fn_802026E4(uVar1,0x2e);
  if (cVar7 == 1) {
    fn_80202810(uVar1,0x2e);
  }
  cVar7 = fn_802026E4(uVar1,0x15);
  if (cVar7 == 1) {
    fn_80202810(uVar1,0x15);
  }
  cVar7 = fn_802026E4(uVar1,0x28);
  if (cVar7 == 1) {
    fn_80202810(uVar1,0x28);
  }
  fn_80205B8C(uVar1);
  cVar7 = fn_8011FC74();
  if (cVar7 == 1) {
    fn_80205B8C(uVar1);
    uVar3 = fn_8011F634();
    cVar7 = fn_802026E4(uVar1,0x3e);
    if (cVar7 == 1) {
      uVar4 = fn_800E0C54();
      if ((uVar4 & 0xffff) % 100 < (uVar3 & 0xff)) {
        uVar10 = 0x7707;
        iVar8 = 1;
        cVar7 = fn_802026E4(uVar1,8);
        if (cVar7 == 1) {
          iVar9 = 1;
        }
      }
      else {
        uVar10 = 0x7708;
        cVar7 = fn_802026E4(uVar1,8);
        if ((cVar7 == 1) &&
           (uVar4 = fn_800E0C54(), (uVar4 & 0xffff) % 100 < (uVar3 >> 1 & 0x7f))) {
          iVar9 = 1;
        }
      }
    }
    else {
      cVar7 = fn_802026E4(uVar1,8);
      if (cVar7 == 1) {
        uVar10 = 0x7706;
        iVar9 = 1;
      }
      else {
        uVar10 = 0x771d;
      }
    }
  }
  else {
    cVar7 = fn_802026E4(uVar1,8);
    if (cVar7 == 1) {
      uVar10 = 0x7706;
      iVar9 = 1;
    }
    else {
      uVar10 = 0x7709;
    }
  }
  fn_801FB974(uVar2,0);
  fn_802080A8(uVar1,iVar9,iVar8,0,0);
  uVar5 = fn_801F8100(uVar2);
  fn_80132A38(0x13,uVar5);
  fn_801EF8F4(1);
  fn_802624CC(0x7704);
  fn_801F000C(0x40);
  fn_8026246C();
  fn_801FB974(uVar2,1);
  fn_80265598(uVar1,uVar6,1);
  fn_802624CC(0x7705);
  fn_801FB974(uVar2,2);
  fn_8026246C();
  if (iVar9 == 1) {
    fn_80202810(uVar1,8);
    fn_80202810(uVar1,0x17);
    cVar7 = fn_801FECD4(uVar1);
    if (cVar7 == 1) {
      fn_801FE7EC(uVar1,0x7c,0,0);
    }
  }
  if (iVar8 == 1) {
    fn_80202810(uVar1,0x3e);
    cVar7 = fn_801FECD4(uVar1);
    if (cVar7 == 1) {
      fn_801FE7EC(uVar1,200,0,0);
    }
    cVar7 = fn_80204A10(uVar1);
    if (cVar7 == 1) {
      uVar5 = fn_80205B8C(uVar1);
      fn_8011F910(uVar5,0,2);
      cVar7 = fn_801FECD4(uVar1);
      if (cVar7 == 1) {
        fn_801FE7EC(uVar1,0xc5,0,0);
      }
    }
  }
  fn_802080A8(uVar1,iVar9,iVar8,uVar10,1);
  fn_802080A8(uVar1,iVar9,iVar8,0,2);
  fn_8026246C();
  fn_8026532C(uVar1,uVar6,0);
  fn_801FB974(uVar2,3);
  fn_802080A8(uVar1,iVar9,iVar8,0,3);
  fn_801DA7AC();
  fn_80261B68(0);
  fn_80261E7C(0);
  fn_8026246C();
  return;
}
/* Address: 0x80213158 | Size: 0x118 | Ghidra import */
void fn_80213158(u32 r3)

{
    extern void fn_80132A38();
    extern void fn_801DA7AC();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_802037DC();
    extern void fn_80211B94();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern u8 lbl_8047B614;
    extern u8 lbl_8037889D[];
  u32 uVar1;
  u8 cVar3;

  uVar1 = fn_801F54A4(0,0,0x45,0);
  fn_801F4C14(0,0,0x36,0,uVar1);
  fn_80132A38(0xd,fn_802037DC(uVar1));
  lbl_8047B614 = 0;
  fn_801EF8F4(1);
  cVar3 = fn_802026E4(uVar1,0x2e);
  if (cVar3 == 1) {
    fn_80202810(uVar1,0x2e);
  }
  cVar3 = fn_802026E4(uVar1,0x15);
  if (cVar3 == 1) {
    fn_80202810(uVar1,0x15);
  }
  cVar3 = fn_802026E4(uVar1,0x28);
  if (cVar3 == 1) {
    fn_80202810(uVar1,0x28);
  }
  fn_80211B94(r3,(u32)lbl_8037889D,1);
  fn_801DA7AC();
  fn_80261B68(0);
  fn_80261E7C(0);
  fn_8026246C();
  return;
}
/* Address: 0x80213270 | Size: 0x264 | Ghidra import */
void fn_80213270(void)

{
    extern u32 DAT_8038ff5a;
    extern u32 DAT_8038fff9;
    extern u32 fn_800E0C54();
    extern void fn_801DA7AC();
    extern short fn_801EF634();
    extern void fn_801F27D4();
    extern void fn_801F37B0();
    extern int fn_801F47B4();
    extern void fn_801F4C14();
    extern short fn_801F54A4();
    extern void fn_801F6EEC();
    extern void fn_8022FE80();
    extern void fn_802317E4();
    extern void fn_80261B68();
    extern void fn_80261E7C();
    extern void fn_8026246C();
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B618;
  int iVar1;
  short sVar2;
  u16 uVar3;
  u16 uVar4;
  u8 bVar5;

  u32 uVar6;
  u8 local_18;
  u8 local_17 [19];
  
  fn_80261B68(0);
  fn_80261E7C(0);
  fn_8026246C();
  for (bVar5 = 0; bVar5 < 8; bVar5 = bVar5 + 1) {
    *(u8 *)(lbl_80478D78 + (u32)bVar5) = 0;
  }
  fn_801F37B0(0,0x802134d4,0,0);
  fn_801F27D4(0);
  local_17[0] = 1;
  fn_801F37B0(0,0x80213558,local_17,0);
  for (uVar6 = 0; (uVar6 & 0xffff) < 2; uVar6 = uVar6 + 1) {
    iVar1 = fn_801F47B4(0,uVar6);
    if (iVar1 != 0) {
      fn_801F6EEC(iVar1,0x4d);
    }
  }
  fn_802317E4();
  fn_801F37B0(0,0x80230568,0,1);
  fn_8022FE80();
  fn_801F37B0(0,0x80230318,0,0);
  fn_801DA7AC();
  fn_801F37B0(0,0x802301a8,0,1);
  fn_801DA7AC();
  local_18 = 0;
  fn_801F37B0(0,0x80213558,&local_18,0);
  for (uVar6 = 0; (uVar6 & 0xffff) < 2; uVar6 = uVar6 + 1) {
    iVar1 = fn_801F47B4(0,uVar6);
    if (iVar1 != 0) {
      fn_801F6EEC(iVar1,0x4d);
    }
  }
  uVar6 = lbl_8047B618;
  lbl_8047B618 = uVar6 & 0xfffffdff;
  bVar5 = 0;
  lbl_8047B618 = uVar6 & 0xfff7fdff;
  lbl_8047B618 = uVar6 & 0xffb7fdff;
  lbl_8047B618 = uVar6 & 0xffa7fdff;
  DAT_8038ff5a = 0;
  DAT_8038fff9 = 0;
  while (1) {
    if (4 < bVar5) break;
    uVar6 = (u32)bVar5;
    bVar5 = bVar5 + 1;
    *(u8 *)(lbl_80478D78 + uVar6) = 0;
  }
  sVar2 = fn_801EF634();
  if (sVar2 == 0) {
    sVar2 = fn_801F54A4(0,0,0xc,0);
    uVar3 = sVar2 + 1;
    if (0xff < uVar3) {
      uVar3 = 0xff;
    }
    fn_801F4C14(0,0,0xc,0,uVar3);
    uVar4 = fn_800E0C54();
    fn_801F4C14(0,0,0x5b,0,uVar4);
    fn_801DA7AC();
    fn_80261B68(0);
    fn_80261E7C(0);
    fn_8026246C();
  }
  return;
}
/* Address: 0x802134D4 | Size: 0x84 | Ghidra import */
u32 fn_802134D4(u32 r3)
{
    extern void fn_80200B10();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802062FC();
  u8 cVar1;
  
  cVar1 = fn_802062FC();
  if (cVar1 == 0) {
    return 1;
  }
  fn_80202810(r3,0x11);
  cVar1 = fn_802026E4(r3,8);
  if ((cVar1 == 1) && (cVar1 = fn_802026E4(r3,0x22), cVar1 == 1)) {
    fn_80200B10(r3);
  }
  return 1;
}
/* Address: 0x80213558 | Size: 0x14c | Ghidra import */
u32 fn_80213558(void)

{
    u32 r3;
    u32 r4;
    char *r5;

    extern int fn_801FEF74();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_802075A4();
  short sVar2;
  s8 cVar3;
  s8 cVar4;
  int iVar1;
  
  if (*r5 == 1) {
    fn_80202810(r3,0x2b);
    fn_80202810(r3,0x2c);
  }
  else {
    fn_802075A4();
    fn_80202810(r3,0x32);
    fn_80202810(r3,0x37);
    fn_80202810(r3,0x33);
    sVar2 = (int)fn_8012640C(r3,0,0xed,0);
    if (sVar2 != 0) {
      fn_801254B4(r3,0,0xed,0,sVar2 + -1);
    }
    cVar4 = fn_802026E4(r3,0x12);
    if (cVar4 == 1) {
      cVar4 = fn_80202108(r3,0x12);
      cVar3 = fn_80202234(r3,0x12);
      if ((char)(cVar4 + 1) < cVar3) {
        fn_80201FDC(r3,0x12);
      }
      else {
        fn_80202810(r3,0x12);
      }
    }
  }
  cVar4 = fn_802026E4(r3,0x14);
  if ((cVar4 == 1) && (iVar1 = fn_801FEF74(r3), iVar1 < 1)) {
    fn_80202810(r3,0x14);
  }
  return 1;
}
/* Address: 0x802136A4 | Size: 0x24 | Ghidra import */
u32 fn_802136A4(void)

{
    extern void fn_80207448();
  fn_80207448();
  return 1;
}
/* Address: 0x802136C8 | Size: 0x88 | Ghidra import */
void fn_802136C8(void)

{
    extern void statusSetStatus();
  int iVar1;
  u32 *puVar2;
  u8 *puVar3;
  u8 *puVar4;
  u32 *puVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar9;
  u16 uVar10;

  iVar1 = lbl_8047B610;
  puVar2 = *(u32 **)(iVar1 + 2);
  bVar9 = *(u8 *)(iVar1 + 1);
  uVar10 = *(u16 *)(iVar1 + 10);
  puVar3 = *(u8 **)(iVar1 + 6);
  puVar4 = *(u8 **)(iVar1 + 0xc);
  puVar5 = *(u32 **)(iVar1 + 0x10);
  if (puVar2 == (void *)0) {
    uVar6 = 0;
  }
  else {
    uVar6 = *puVar2;
  }
  if (puVar3 == (void *)0) {
    uVar7 = 0;
  }
  else {
    uVar7 = *puVar3;
  }
  if (puVar4 == (void *)0) {
    uVar8 = 0;
  }
  else {
    uVar8 = *puVar4;
  }
  statusSetStatus(bVar9,uVar6,uVar7,uVar10,uVar8,*puVar5);
  lbl_8047B610 = lbl_8047B610 + 0x14;
  return;
}
/* Address: 0x80213750 | Size: 0x88 | Ghidra import */
void fn_80213750(void)

{
    extern void statusSetStatus();
  int iVar1;
  u32 *pVar5;
  u8 uVar1b;
  u16 uVar6;
  u8 *pVar7;
  u8 *pVar8;
  u16 *pVar9;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;

  iVar1 = lbl_8047B610;
  pVar5 = *(u32 **)(iVar1 + 2);
  uVar1b = *(u8 *)(iVar1 + 1);
  uVar6 = *(u16 *)(iVar1 + 0xa);
  pVar7 = *(u8 **)(iVar1 + 6);
  pVar8 = *(u8 **)(iVar1 + 0xc);
  pVar9 = *(u16 **)(iVar1 + 0x10);
  if (pVar5 == (void *)0) {
    uVar2 = 0;
  }
  else {
    uVar2 = *pVar5;
  }
  if (pVar7 == (void *)0) {
    uVar3 = 0;
  }
  else {
    uVar3 = *pVar7;
  }
  if (pVar8 == (void *)0) {
    uVar4 = 0;
  }
  else {
    uVar4 = *pVar8;
  }
  statusSetStatus(uVar1b,uVar2,uVar3,uVar6,uVar4,*pVar9);
  lbl_8047B610 = lbl_8047B610 + 0x14;
  return;
}
/* Address: 0x802137D8 | Size: 0x88 | Ghidra import */
void fn_802137D8(void)

{
    extern void statusSetStatus();
  int iVar1;
  u32 *puVar2;
  u8 *puVar3;
  u8 *puVar4;
  u8 *puVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar9;
  u16 uVar10;

  iVar1 = lbl_8047B610;
  puVar2 = *(u32 **)(iVar1 + 2);
  bVar9 = *(u8 *)(iVar1 + 1);
  uVar10 = *(u16 *)(iVar1 + 10);
  puVar3 = *(u8 **)(iVar1 + 6);
  puVar4 = *(u8 **)(iVar1 + 0xc);
  puVar5 = *(u8 **)(iVar1 + 0x10);
  if (puVar2 == (void *)0) {
    uVar6 = 0;
  }
  else {
    uVar6 = *puVar2;
  }
  if (puVar3 == (void *)0) {
    uVar7 = 0;
  }
  else {
    uVar7 = *puVar3;
  }
  if (puVar4 == (void *)0) {
    uVar8 = 0;
  }
  else {
    uVar8 = *puVar4;
  }
  statusSetStatus(bVar9,uVar6,uVar7,uVar10,uVar8,*puVar5);
  lbl_8047B610 = lbl_8047B610 + 0x14;
  return;
}
/* Address: 0x80213860 | Size: 0x90 | Ghidra import */
void fn_80213860(void)

{
    extern u32 statusGetStatus();
  int iVar1;
  u32 *puVar2;
  u8 *puVar3;
  u8 *puVar4;
  u32 *puVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar9;
  u16 uVar10;

  iVar1 = lbl_8047B610;
  puVar2 = *(u32 **)(iVar1 + 2);
  bVar9 = *(u8 *)(iVar1 + 1);
  uVar10 = *(u16 *)(iVar1 + 10);
  puVar3 = *(u8 **)(iVar1 + 6);
  puVar4 = *(u8 **)(iVar1 + 0xc);
  puVar5 = *(u32 **)(iVar1 + 0x10);
  if (puVar2 == (void *)0) {
    uVar6 = 0;
  }
  else {
    uVar6 = *puVar2;
  }
  if (puVar3 == (void *)0) {
    uVar7 = 0;
  }
  else {
    uVar7 = *puVar3;
  }
  if (puVar4 == (void *)0) {
    uVar8 = 0;
  }
  else {
    uVar8 = *puVar4;
  }
  *puVar5 = statusGetStatus(bVar9,uVar6,uVar7,uVar10,uVar8);
  lbl_8047B610 = lbl_8047B610 + 0x14;
  return;
}
/* Address: 0x802138F0 | Size: 0x90 | Ghidra import */
void fn_802138F0(void)

{
    extern u32 statusGetStatus();
  int iVar1;
  u32 *puVar2;
  u8 *puVar3;
  u8 *puVar4;
  u16 *puVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar9;
  u16 uVar10;

  iVar1 = lbl_8047B610;
  puVar2 = *(u32 **)(iVar1 + 2);
  bVar9 = *(u8 *)(iVar1 + 1);
  uVar10 = *(u16 *)(iVar1 + 10);
  puVar3 = *(u8 **)(iVar1 + 6);
  puVar4 = *(u8 **)(iVar1 + 0xc);
  puVar5 = *(u16 **)(iVar1 + 0x10);
  if (puVar2 == (void *)0) {
    uVar6 = 0;
  }
  else {
    uVar6 = *puVar2;
  }
  if (puVar3 == (void *)0) {
    uVar7 = 0;
  }
  else {
    uVar7 = *puVar3;
  }
  if (puVar4 == (void *)0) {
    uVar8 = 0;
  }
  else {
    uVar8 = *puVar4;
  }
  *puVar5 = statusGetStatus(bVar9,uVar6,uVar7,uVar10,uVar8);
  lbl_8047B610 = lbl_8047B610 + 0x14;
  return;
}
/* Address: 0x80213980 | Size: 0x90 | Ghidra import */
void fn_80213980(void)

{
    extern u32 statusGetStatus();
  int iVar1;
  u32 *puVar2;
  u8 *puVar3;
  u8 *puVar4;
  u8 *puVar5;
  u32 uVar6;
  u32 uVar7;
  u32 uVar8;
  u8 bVar9;
  u16 uVar10;

  iVar1 = lbl_8047B610;
  puVar2 = *(u32 **)(iVar1 + 2);
  bVar9 = *(u8 *)(iVar1 + 1);
  uVar10 = *(u16 *)(iVar1 + 10);
  puVar3 = *(u8 **)(iVar1 + 6);
  puVar4 = *(u8 **)(iVar1 + 0xc);
  puVar5 = *(u8 **)(iVar1 + 0x10);
  if (puVar2 == (void *)0) {
    uVar6 = 0;
  }
  else {
    uVar6 = *puVar2;
  }
  if (puVar3 == (void *)0) {
    uVar7 = 0;
  }
  else {
    uVar7 = *puVar3;
  }
  if (puVar4 == (void *)0) {
    uVar8 = 0;
  }
  else {
    uVar8 = *puVar4;
  }
  *puVar5 = statusGetStatus(bVar9,uVar6,uVar7,uVar10,uVar8);
  lbl_8047B610 = lbl_8047B610 + 0x14;
  return;
}
/* 0x80213A10 | size: 0xC | tiny */
void fn_80213A10(void) { lbl_8047B614 = 1; }

/* 0x80213A1C | size: 0xC | tiny */
void fn_80213A1C(void) { lbl_8047B614 = 1; }

/* 0x80213A28 | size: 0x10 | tiny */
void fn_80213A28(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* 0x80213A38 | size: 0x10 | tiny */
void fn_80213A38(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* 0x80213A48 | size: 0x10 | tiny */
void fn_80213A48(void) { lbl_8047B610 = lbl_8047B610 + 5; }

/* 0x80213A58 | size: 0x10 | tiny */
void fn_80213A58(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* 0x80213A68 | size: 0x10 | tiny */
void fn_80213A68(void) { lbl_8047B610 = lbl_8047B610 + 5; }

/* Address: 0x80213A78 | Size: 0x41c | Ghidra import */
void fn_80213A78(void)

{
    extern void fn_8012190C();
    extern u8 fn_80121ADC();
    extern void fn_801232E0();
    extern void fn_80123368();
    extern u8 fn_80123FBC();
    extern void fn_801252E0();
    extern short fn_80129F20();
    extern int fn_8012A5B0();
    extern void fn_80232FE4();
    extern u32 fn_801F025C();
    extern void fn_801F37B0();
    extern u32 fn_801F4220();
    extern u32 fn_801F4354();
    extern u32 fn_801F4804();
    extern u8 fn_801F54A4();
    extern int fn_801F7F80();
    extern void fn_801FAA58();
    extern u8 fn_801FB8F8();
    extern void fn_801FE710();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_80202998();
    extern void fn_802032E4();
    extern u32 fn_80205B8C();
    extern u32 fn_80205BE8();
    extern void fn_80206AEC();
  u32 uVar1;
  u32 uVar2;
  u16 uVar9;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u16 uVar10;
  short sVar11;
  int iVar7;
  int iVar8;
  u8 cVar12;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xe5,0);
  uVar9 = itemGetStatus(uVar2,0,0x1e,0);
  uVar9 = itemGetStatus(0,uVar9,10,0);
  uVar2 = fn_801F4354(0,uVar1);
  uVar1 = fn_801F4220(0,uVar1);
  uVar3 = fn_801F025C(0x12,0);
  uVar4 = fn_801F4354(0,uVar3);
  uVar5 = fn_801F54A4(0,0,0x4a,0);
  fn_8012640C(uVar3,0,0xd5,0);
  uVar6 = fn_80205BE8();
  uVar10 = fn_801F54A4(0,0,0x17,0);
  fn_801254B4(uVar6,0,0xc9,0,0);
  sVar11 = fn_80129F20(uVar1,uVar6,uVar5,uVar9,1);
  if ((((sVar11 != -1) && (-1 < sVar11)) && (iVar7 = fn_801F7F80(uVar2,uVar10), iVar7 != 0)) &&
     ((iVar8 = fn_8012A5B0(uVar1,3,sVar11), iVar8 != 0 &&
      (cVar12 = fn_80123FBC(), cVar12 == 1)))) {
    cVar12 = fn_80121ADC(iVar8,4);
    if (cVar12 == 1) {
      fn_8012190C(iVar8,4,1);
    }
    uVar1 = fn_801F4804(0);
    fn_80206AEC(iVar7,iVar8,uVar1);
    fn_801254B4(iVar7,0,0xcf,0,1);
    cVar12 = fn_801F54A4(0,0,0x27,0);
    if (((cVar12 == 1) && (cVar12 = fn_801F54A4(0,0,0x2e,0), cVar12 == 1)) &&
       (cVar12 = fn_801FB8F8(uVar2), cVar12 == 1)) {
      fn_802032E4(iVar7,3);
    }
  }
  cVar12 = fn_801F54A4(0,0,0x1f,0);
  if ((cVar12 == 1) && (cVar12 = fn_801FB8F8(uVar2), cVar12 == 1)) {
    fn_80123368(uVar6,1);
    fn_801232E0(uVar6,1);
  }
  fn_801254B4(uVar6,0,0x83,0,0);
  uVar1 = fn_80205B8C(uVar3);
  fn_801254B4(uVar1,0,0x83,0,0);
  fn_801F37B0(0,(u32)fn_80232FE4,uVar3,0);
  fn_80205B8C(uVar3);
  fn_801252E0();
  fn_80202998(uVar3,0);
  fn_80202810(uVar3,0x17);
  cVar12 = fn_801FECD4(uVar3);
  if (cVar12 == 1) {
    fn_801FE7EC(uVar3,0x7c,0,0);
  }
  cVar12 = fn_802026E4(uVar3,0x3e);
  if (cVar12 == 1) {
    fn_80202810(uVar3,0x3e);
    cVar12 = fn_801FECD4(uVar3);
    if (cVar12 == 1) {
      fn_801FE7EC(uVar3,200,0,0);
    }
  }
  uVar1 = (int)fn_8012640C(uVar3,0,0xd6,0);
  fn_801254B4(uVar1,0,0xd2,0,1);
  cVar12 = fn_801FECD4(uVar3);
  if (cVar12 == 1) {
    fn_801FE710(uVar3,0xd2,0);
  }
  fn_801FAA58(uVar4,0,0x4a,0,1);
  if (sVar11 != -1) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    fn_801EF8F4(1);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x80213E94 | Size: 0x470 | Ghidra import */
void fn_80213E94(void)

{
    extern u32 fn_800C46B0();
    extern void fn_800CE77C();
    extern u32 fn_800E0C54();
    extern s8 fn_8011FC74();
    extern void fn_80132A38();
    extern void fn_80165668();
    extern void fn_801EEE44();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern short fn_801F54A4();
    extern void fn_801FBAD4();
    extern s8 fn_802026E4();
    extern u32 fn_802037DC();
    extern void fn_80203E0C();
    extern u32 fn_80205B8C();
    extern void fn_802087C0();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern u8 lbl_80478D7D;
    extern f64 lbl_8047E610;

  short sVar7;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar8;
  u16 uVar9;
  int iVar5;
  s8 cVar10;
  u32 uVar6;

  u32 uVar11;
  u32 uVar12;
  u8 auStack_58 [8];
  u32 local_50;
  u32 uStack_4c;
  u32 local_48;
  u32 uStack_44;
  
  sVar7 = fn_801F54A4(0,0,0xd,0);
  fn_801F54A4(0,0,0xf,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xe5,0);
  itemGetStatus(uVar2,0,0x20,0);
  uVar3 = itemGetStatus(uVar2,0,0x1e,0);
  uVar3 = uVar3 & 0xffff;
  uVar2 = fn_801F025C(0x12,0);
  uVar4 = fn_80205B8C();
  uVar8 = (int)fn_8012640C(uVar4,0,0xc3,0);
  uVar9 = (int)fn_8012640C(uVar4,0,0x6e,0);
  fn_80203E0C(uVar2);
  fn_8012640C(uVar4,0,0x83,0);
  iVar5 = (int)fn_8012640C(uVar4,0,0x87,0);
  uVar1 = fn_801F4354(0,uVar1);
  cVar10 = fn_8011FC74(uVar4);
  if (cVar10 == 1) {
    fn_801EEE44(uVar8);
  }
  else {
    fn_8012640C(0,uVar9,0x12,0);
  }
  if (uVar3 < 0xd) {

    ((int (*)(void))**(void ***)(uVar3 * 4 + -0x7fc6600c))();
    return;
  }
  if ((uVar3 == 1) || (sVar7 == 0x11)) {
    uVar12 = 4;
  }
  else {
    uVar11 = 0 / (iVar5 * 3);
    cVar10 = fn_802026E4(uVar2,8);
    if ((cVar10 == 1) || (cVar10 = fn_802026E4(uVar2,7), cVar10 == 1)) {
      uVar11 = uVar11 << 1;
    }
    cVar10 = fn_802026E4(uVar2,3);
    if ((((cVar10 == 1) || (cVar10 = fn_802026E4(uVar2,4), cVar10 == 1)) ||
        (cVar10 = fn_802026E4(uVar2,5), cVar10 == 1)) ||
       (cVar10 = fn_802026E4(uVar2,6), cVar10 == 1)) {
      uVar11 = (uVar11 * 0xf) / 10;
    }
    if (uVar11 < 0xff) {
      uStack_4c = 0xff0000 / uVar11;
      local_50 = 0x43300000;
      fn_800CE77C((double)((u64)(0x43300000) << 32 | (u32)(uStack_4c)) - lbl_8047E610);
      uStack_44 = fn_800C46B0();
      local_48 = 0x43300000;
      fn_800CE77C((double)((u64)(0x43300000) << 32 | (u32)(uStack_44)) - lbl_8047E610);
      uVar11 = fn_800C46B0();
      uVar12 = 0;
      while (((uVar12 & 0xff) < 4 && (uVar6 = fn_800E0C54(), (uVar6 & 0xffff) < 0xffff0 / uVar11)))
      {
        uVar12 = uVar12 + 1;
      }
    }
    else {
      uVar12 = 4;
    }
  }
  fn_801FBAD4(uVar1,uVar3,0);
  fn_802087C0(uVar2,uVar12,uVar3,0,auStack_58);
  fn_801FBAD4(uVar1,uVar3,1);
  fn_801FBAD4(uVar1,uVar3,3);
  fn_802087C0(uVar2,uVar12,uVar3,1,auStack_58);
  uVar4 = fn_802037DC(uVar2);
  fn_80132A38(0x16,uVar4);
  if ((uVar12 & 0xff) < 4) {
    fn_802087C0(uVar2,uVar12,uVar3,2,auStack_58);
    lbl_80478D7D = (char)uVar12;
    fn_802624CC(*(u32 *)((uVar12 & 0xff) * 4 + -0x7fd861e4));
    fn_802087C0(uVar2,uVar12,uVar3,3,auStack_58);
    fn_8026246C();
    *(u32 *)(lbl_8047B610) = 0x80375e5f;
  }
  else {
    fn_801FBAD4(uVar1,uVar3,4);
    fn_80132A38(0x5d,0);
    fn_802624CC(0x771f);
    fn_80165668(0x3f6,0,0xff);
    fn_801FBAD4(uVar1,uVar3,5);
    fn_8026246C();
    *(u32 *)(lbl_8047B610) = 0x80375e51;
  }
  fn_801FBAD4(uVar1,uVar3,2);
  fn_801FBAD4(uVar1,uVar3,6);
  fn_802087C0(uVar2,uVar12,uVar3,4,auStack_58);
  return;
}
/* Address: 0x80214450 | Size: 0x178 | Ghidra import */
void fn_80214450(void)

{
    extern u8 lbl_80379F58[];
    extern u32 fn_801F025C();
    extern u8 fn_801F6E98();
    extern void fn_801F6EEC();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = fn_801F025C(3,uVar1);
  cVar2 = fn_801F6E98(uVar1,0x49);
  if ((cVar2 == 1) || (cVar2 = fn_801F6E98(uVar1,0x48), cVar2 == 1)) {
    cVar2 = fn_801F6E98(uVar1,0x49);
    if ((cVar2 == 1) && (cVar2 = fn_801F6E98(uVar1,0x48), cVar2 == 1)) {
      fn_801F6EEC(uVar1,0x49);
      fn_801F6EEC(uVar1,0x48);
      lbl_80379F58[0x16002] = 3;
      lbl_80379F58[0x160a1] = 3;
    }
    else {
      cVar2 = fn_801F6E98(uVar1,0x49);
      if (cVar2 == 1) {
        fn_801F6EEC(uVar1,0x49);
        lbl_80379F58[0x16002] = 1;
        lbl_80379F58[0x160a1] = 1;
      }
      else {
        cVar2 = fn_801F6E98(uVar1,0x48);
        if (cVar2 == 1) {
          fn_801F6EEC(uVar1,0x48);
          lbl_80379F58[0x16002] = 2;
          lbl_80379F58[0x160a1] = 2;
        }
      }
    }
  }
  else {
    lbl_80379F58[0x16002] = 0;
    lbl_80379F58[0x160a1] = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802145C8 | Size: 0xec | Ghidra import */
void fn_802145C8(void)

{
    extern int fn_801F025C();
    extern void fn_801F4C14();
  u32 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;

  iVar2 = fn_801F025C(0x11,0);
  iVar3 = fn_801F025C(0x12,0);
  iVar4 = fn_801F025C(0x19,0);
  fn_801F4C14(0,0,0x47,0,iVar2);
  uVar1 = __cntlzw(iVar3 - iVar2);
  iVar3 = iVar4;
  if (uVar1 >> 5 == 0) {
    iVar3 = iVar2;
  }
  fn_801F4C14(0,0,0x4b,0,iVar2);
  fn_801F4C14(0,0,0x36,0,iVar3);
  fn_801F4C14(0,0,0x43,0,iVar4);
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* 0x802146B4 | size: 0x10 | tiny */
void fn_802146B4(void) { lbl_8047B610 = lbl_8047B610 + 5; }

/* Address: 0x802146C4 | Size: 0xd0 | Ghidra import */
void fn_802146C4(void)

{
    extern u32 fn_800FA280();
    extern void fn_8010C4D4();
    extern void fn_80132A38();
    extern u8 fn_80136428();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_80207AE0();
    extern void fn_80207B5C();
  u16 uVar2;
  u32 uVar1;
  u8 uVar3;
  u8 cVar4;

  u32 uVar5;

  uVar2 = fn_801F54A4(0,0,0xf,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar3 = fn_80136428(uVar2);
  cVar4 = fn_80207AE0(uVar1,uVar3);
  if (cVar4 == 0) {
    for (uVar5 = 0; (uVar5 & 0xff) < 2; uVar5 = uVar5 + 1) {
      fn_80207B5C(uVar1,uVar5,uVar3);
    }
    fn_8010C4D4(uVar3);
    uVar1 = fn_800FA280();
    fn_80132A38(0xd,uVar1);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x80214794 | Size: 0xd0 | Ghidra import */
void fn_80214794(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_8020147C();
  u32 uVar1;
  short sVar2;
  short sVar3;
  s8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  sVar2 = fn_802040E8();
  sVar3 = (int)fn_8012640C(uVar1,0,0xfa,0);
  if ((sVar3 == 0) || (sVar2 != 0)) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  else {
    fn_801F4C14(0,0,0x56,0,sVar3);
    fn_8020147C(uVar1,sVar3,1,1);
    cVar4 = fn_801FECD4(uVar1);
    if (cVar4 == 1) {
      fn_801FE7EC(uVar1,0x82,0,0);
    }
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
  }
  return;
}
/* Address: 0x80214864 | Size: 0x154 | Ghidra import */
void fn_80214864(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
  u8 cVar2;
  u32 uVar1;

  cVar2 = (int)fn_801F453C(0,1);
  uVar1 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  if (cVar2 == 2) {
    fn_8011BBD8(uVar1,0,0x30,0,0xb);
    fn_8011BBD8(uVar1,0,0x2c,0,2);
  }
  else if (cVar2 == 3) {
    fn_8011BBD8(uVar1,0,0x30,0,5);
    fn_8011BBD8(uVar1,0,0x2c,0,2);
  }
  else if (cVar2 == 1) {
    fn_8011BBD8(uVar1,0,0x30,0,10);
    fn_8011BBD8(uVar1,0,0x2c,0,2);
  }
  else if (cVar2 == 4) {
    fn_8011BBD8(uVar1,0,0x30,0,0xf);
    fn_8011BBD8(uVar1,0,0x2c,0,2);
  }
  else {
    fn_8011BBD8(uVar1,0,0x30,0,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802149B8 | Size: 0xfc | Ghidra import */
void fn_802149B8(void)

{
    extern u16 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 lbl_80478D78;
  u32 uVar3;
  u8 bVar1;
  int iVar2;
  u32 uVar4;
  u16 sVar5;
  u8 cVar6;

  uVar3 = fn_801F025C(0x11,0);
  fn_80207BF4();
  uVar4 = fn_80205184(uVar3);
  sVar5 = fn_8011BEB4(0,uVar4,9,0);
  bVar1 = 0;
  if (sVar5 == 0xc9) {
    cVar6 = fn_802025B8(uVar3,0x38);
    if (cVar6 == 2) {
      fn_8020248C(uVar3,0x38,0);
      (&lbl_80478D78)[5] = 0;
      bVar1 = 1;
    }
  }
  else {
    cVar6 = fn_802025B8(uVar3,0x39);
    if (cVar6 == 2) {
      fn_8020248C(uVar3,0x39,0);
      (&lbl_80478D78)[5] = 1;
      bVar1 = 1;
    }
  }
  if (bVar1) {
    iVar2 = lbl_8047B610 + 5;
  }
  else {
    iVar2 = *(int *)(lbl_8047B610 + 1);
  }
  lbl_8047B610 = iVar2;
  return;
}
/* Address: 0x80214AB4 | Size: 0x48 | Ghidra import */
void fn_80214AB4(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8022EB9C();

  u8 local_8 [8];

  local_8[0] = 0;
  fn_801F37B0(0,fn_8022EB9C,local_8,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80214AFC | Size: 0x5c | Ghidra import */
void fn_80214AFC(void)

{
    extern u8 lbl_80379F58[];
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x19,0);
  cVar2 = fn_802026E4(uVar1,0x14);
  if (cVar2 == 0) {
    lbl_80379F58[0x1609b] = lbl_80379F58[0x1609b] | 0x80;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* 0x80214B58 | size: 0x10 | tiny */
void fn_80214B58(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x80214B68 | Size: 0x4c | Ghidra import */
void fn_80214B68(void)

{
    extern u32 fn_80136468();
    extern u32 fn_801F54A4();
    extern u8 lbl_80478D78;
  u16 uVar1;
  u8 uVar2;

  u32 pc;
  uVar1 = fn_801F54A4(0,0,0xf,0);
  uVar2 = fn_80136468(uVar1);
  pc = *(volatile u32*)&lbl_8047B610;
  *((&lbl_80478D78)+3) = uVar2;
  lbl_8047B610 = pc + 1;
  return;
}
/* Address: 0x80214BB4 | Size: 0x50 | Ghidra import */
void fn_80214BB4(void)

{
    extern void fn_801F025C();
    extern s8 fn_802062FC();
  int iVar1;
  u8 cVar2;

  fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  cVar2 = fn_802062FC();
  if (cVar2 == 0) {
    iVar1 = *(int *)(lbl_8047B610 + 2);
  }
  else {
    iVar1 = lbl_8047B610 + 6;
  }
  lbl_8047B610 = iVar1;
  return;
}
/* Address: 0x80214C04 | Size: 0xac | Ghidra import */
void fn_80214C04(void)

{
    extern void fn_801252E0();
    extern u32 fn_801F025C();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_80202810();
    extern void fn_80202998();
    extern u32 fn_80205B8C();
    extern u16 fn_80207BF4();
  u32 uVar1;
  u32 uVar2;
  u16 sVar3;
  u8 cVar4;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar2 = fn_80205B8C();
  sVar3 = fn_80207BF4(uVar1);
  if (sVar3 == 0x1e) {
    fn_801252E0(uVar2);
    fn_80202998(uVar1,0);
    fn_80202810(uVar1,0x17);
    cVar4 = fn_801FECD4(uVar1);
    if (cVar4 == 1) {
      fn_801FE7EC(uVar1,0x7c,0,0);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x80214CB0 | Size: 0x4c | Ghidra import */
void fn_80214CB0(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F37B0();
    extern u32 fn_80214CFC();
  u32 uVar1;

  uVar1 = fn_801F025C(0x19,0);
  fn_801F37B0(0,fn_80214CFC,uVar1,0);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80214CFC | Size: 0xb4 | Ghidra import */
u32 fn_80214CFC(void)

{
    u32 r3;
    u32 r4;
    u32 r5;

    extern u32 fn_800FA280();
    extern void fn_8011CB54();
    extern void fn_8011CB6C();
    extern void fn_80132A38();
    extern void fn_801F4C14();
    extern s8 fn_80202B88();
    extern s8 fn_802062FC();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
  u32 uVar1;
  s8 cVar2;

  uVar1 = fn_80207BF4(r5);
  cVar2 = fn_802062FC(r3);
  if ((cVar2 != 0) && (cVar2 = fn_80202B88(r3,r5), cVar2 == 0)) {
    fn_801F4C14(0,0,0x42,0,r3);
    fn_8011CB6C(uVar1);
    fn_8011CB54();
    uVar1 = fn_800FA280();
    fn_80132A38(0xd,uVar1);
    fn_80211B94(lbl_8047B62C,0x8037960a,0);
  }
  return 1;
}
/* Address: 0x80214DB0 | Size: 0xa0 | Ghidra import */
void fn_80214DB0(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_801F221C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  fn_801254B4(uVar1,0,0x118,0,1);
  cVar2 = fn_801F221C(0);
  if (cVar2 == 1) {
_skip:
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    return;
  }
  cVar2 = fn_802025B8(uVar1,0x33);
  if (cVar2 != 2) goto _skip;
  fn_8020248C(uVar1,0x33,0);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80214E50 | Size: 0xc0 | Ghidra import */
void fn_80214E50(void)
{
    extern u32 fn_801F025C();
    extern u8 fn_801F221C();
    extern void fn_801F4C14();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  fn_801F4C14(0,0,0x43,0,uVar1);
  fn_801254B4(uVar1,0,0x118,0,1);
  cVar2 = fn_801F221C(0);
  if (cVar2 == 1) {
_skip:
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    return;
  }
  cVar2 = fn_802025B8(uVar1,0x37);
  if (cVar2 != 2) goto _skip;
  fn_8020248C(uVar1,0x37,0);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80214F10 | Size: 0xf8 | Ghidra import */
void fn_80214F10(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_80215008();
    extern int fn_8022B2CC();
    extern u16 lbl_8047B60C;
    extern u32 lbl_8047B618;
  u16 uVar1;
  u16 uVar6;
  u32 uVar2;
  u32 uVar3;
  int iVar4;
  u32 uVar5;

  u16 auStack_48 [30];
  
  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F025C(0x11,0);
  uVar3 = fn_801F4354(0,uVar2);
  iVar4 = fn_80215008(uVar3,auStack_48,0x18,uVar2);
  if (iVar4 == 0) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  else {
    uVar5 = fn_800E0C54();
    lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
    uVar1 = auStack_48[(uVar5 & 0xffff) - ((int)(uVar5 & 0xffff) / iVar4) * iVar4];
    lbl_8047B60C = uVar1;
    uVar2 = fn_8022B2CC(uVar2,uVar1,uVar6,0,1,1, (void*)0xffffffff);
    fn_801F4C14(0,0,0x43,0,uVar2);
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
  }
  return;
}
/* Address: 0x80215008 | Size: 0x1b8 | Ghidra import */
int fn_80215008(void)

{
    u32 r3;
    int r4;
    u32 r5;
    u32 r6;

    extern s8 fn_80123CD4();
    extern u32 fn_8012A5B0();
    extern int fn_801F9930();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
    extern s8 fn_80206608();
  short sVar1;
  u32 bVar2;
  u32 uVar3;
  int iVar4;
  u32 uVar5;
  int iVar6;
  s8 cVar8;
  short sVar7;
  int iVar9;
  u32 uVar10;
  int iVar11;
  u32 uVar12;
  
  uVar3 = fn_801FB1C0(r3,0,0x44,0);
  iVar4 = (int)fn_8012640C(r6,0,0xd5,0);
  iVar9 = 0;
  for (uVar12 = r5 & 0xffff; uVar12 != 0; uVar12 = uVar12 - 1) {
    *(u16 *)(r4 + iVar9) = 0;
    iVar9 = iVar9 + 2;
  }
  iVar11 = 0;
  iVar9 = 0;
  uVar12 = 0;
  do {
    uVar5 = fn_8012A5B0(uVar3,3,uVar12 & 0xffff);
    iVar6 = fn_801F9930(r3,uVar5);
    if (((iVar6 != 0) && (cVar8 = fn_80206608(), cVar8 != 0)) && (iVar4 != iVar6)) {
      uVar5 = fn_80205BE8(iVar6);
      uVar10 = 0;
      do {
        cVar8 = fn_80123CD4(uVar5,uVar10 & 0xffff);
        if (cVar8 != 0) {
          sVar7 = (int)fn_8012640C(uVar5,0,0x7f,uVar10 & 0xffff);
          if ((((sVar7 == 0) || (sVar7 == 0x165)) ||
              ((sVar7 == 0xd6 || ((sVar7 == 0x112 || (sVar7 == 0x77)))))) || (sVar7 == 0x76)) {
            bVar2 = 1;
          }
          else {
            bVar2 = 0;
          }
          if (bVar2 == 0) {
            for (iVar6 = 0;
                (sVar1 = *(short *)(iVar6 + -0x7fd86060), sVar1 != -1 && (sVar7 != sVar1));
                iVar6 = iVar6 + 2) {
            }
            if ((((sVar1 == -1) && (sVar7 != 0)) && (sVar7 != 0x165)) &&
               (iVar11 < (int)(r5 & 0xffff))) {
              *(short *)(r4 + iVar9) = sVar7;
              iVar11 = iVar11 + 1;
              iVar9 = iVar9 + 2;
            }
          }
        }
        uVar10 = uVar10 + 1;
      } while ((int)uVar10 < 4);
    }
    uVar12 = uVar12 + 1;
  } while ((int)uVar12 < 6);
  return iVar11;
}
/* Address: 0x802151C0 | Size: 0xe8 | Ghidra import */
void fn_802151C0(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_80203D3C();
  u16 uVar1;
  u16 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar5;
  u32 uVar6;

  uVar3 = fn_801F025C(0x11,0);
  uVar3 = (int)fn_8012640C(uVar3,0,0xd9,0);
  fn_801F025C(0x12,0);
  uVar4 = fn_80203D3C();
  uVar5 = (int)fn_8012640C(0,uVar4,0x5f,0);
  uVar6 = 0;
  while ((uVar1 = *(u16 *)((uVar6 & 0xffff) * 2 + -0x7fd86078), uVar1 != 0xffff &&
         (uVar1 <= uVar5))) {
    uVar6 = uVar6 + 2;
  }
  if (uVar1 == 0xffff) {
    uVar2 = 0x78;
  }
  else {
    uVar2 = *(u16 *)((uVar6 & 0xffff) * 2 + -0x7fd86076);
  }
  fn_8011BBD8(uVar3,0,0x2f,0,uVar2);
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x802152A8 | Size: 0x58 | Ghidra import */
u16 fn_802152A8(void)

{
    u16 r3;

  u16 uVar1;
  u32 uVar2;
  
  uVar2 = 0;
  while ((uVar1 = *(u16 *)((uVar2 & 0xffff) * 2 + -0x7fd86078), uVar1 != 0xffff &&
         (uVar1 <= r3))) {
    uVar2 = uVar2 + 2;
  }
  if (uVar1 != 0xffff) {
    return *(u16 *)((uVar2 & 0xffff) * 2 + -0x7fd86076);
  }
  return 0x78;
}
/* Address: 0x80215300 | Size: 0x74 | Ghidra import */
void fn_80215300(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0x28);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x28,0);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x80215374 | Size: 0x1b4 | Ghidra import */
void fn_80215374(void)

{
    extern int fn_80123B5C();
    extern int fn_801F025C();
    extern int fn_801F3624();
    extern void fn_801F37B0();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern u32 fn_80201890();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern u32 fn_80205B8C();
  int iVar1;
  s8 cVar4;
  u8 bVar5;
  u32 uVar2;
  u8 bVar6;
  u8 bVar7;
  u32 uVar3;

  int local_28;
  int local_24;
  
  iVar1 = fn_801F025C(0x11,0);
  cVar4 = fn_802025B8(iVar1,0x27);
  if (cVar4 == 2) {
    if (iVar1 != 0) {
      bVar5 = fn_801F3624(0,0x2e,2,iVar1);
      if (bVar5 != 0) {
        uVar2 = fn_80205B8C(iVar1);
        bVar6 = fn_80123B5C(uVar2,0x11e);
        if (-1 < (char)bVar6) {
          bVar7 = (int)fn_8012640C(uVar2,0,0x80,(int)(char)bVar6);
          fn_801254B4(uVar2,0,0x80,(int)(char)bVar6,bVar7 - bVar5 & -(bVar5 < bVar7));
          cVar4 = fn_802026E4(iVar1,0x10);
          if (cVar4 == 0) {
            cVar4 = fn_802026E4(iVar1,0x31);
            if (cVar4 == 1) {
              uVar3 = fn_80201890(iVar1,0x31);
              if (((uVar3 & 1 << (u32)bVar6) == 0) &&
                 (cVar4 = fn_801FECD4(iVar1), cVar4 == 1)) {
                fn_801FE7EC(iVar1,0x80,(u32)bVar6,0);
              }
            }
          }
        }
      }
    }
    local_24 = 0;
    local_28 = iVar1;
    fn_801F37B0(0,0x80215528,&local_28,0);
    if (local_24 == 0) {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
    else {
      fn_8020248C(iVar1,0x27,0);
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
    }
  }
  else {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  return;
}
/* Address: 0x80215528 | Size: 0xec | Ghidra import */
u32 fn_80215528(void)

{
    u32 r3;
    u32 r4;
    u32 *r5;

    extern s8 fn_80123B5C();
    extern s8 fn_80123CD4();
    extern s8 fn_80202B88();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
  s8 cVar3;
  u32 uVar1;
  s8 cVar4;
  u16 uVar2;
  u32 uVar5;
  
  uVar5 = *r5;
  cVar3 = fn_802062FC();
  if ((cVar3 != 0) && (cVar3 = fn_80202B88(r3,uVar5), cVar3 != 1)) {
    uVar5 = fn_80205B8C(uVar5);
    uVar1 = fn_80205B8C(r3);
    for (cVar3 = 0; cVar3 < 4; cVar3 = cVar3 + 1) {
      cVar4 = fn_80123CD4(uVar5,(int)cVar3);
      if (cVar4 != 0) {
        uVar2 = (int)fn_8012640C(uVar5,0,0x7f,(int)cVar3);
        cVar4 = fn_80123B5C(uVar1,uVar2);
        if (-1 < cVar4) {
          r5[1] = 1;
          return 0;
        }
      }
    }
  }
  return 1;
}
/* Address: 0x80215614 | Size: 0x10c | Ghidra import */
void fn_80215614(void)

{
    extern u32 fn_801F025C();
    extern void fn_80207BC0();
    extern u8 fn_802096E8();
    extern u8 fn_80229934();
  u32 uVar1;
  u32 uVar2;
  u32 uVar5;
  u32 uVar3;
  u32 uVar6;
  u32 uVar4;
  u8 cVar7;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80207BF4(uVar1);
  uVar4 = fn_80205184(uVar1);
  uVar5 = fn_801F025C(0x12,0);
  uVar6 = fn_80207BF4();
  if (((((uVar3 & 0xffff) == 0) && ((uVar6 & 0xffff) == 0)) || ((u16)uVar3 == 0x19)) ||
     ((((uVar6 & 0xffff) == 0x19 || (cVar7 = fn_802096E8(uVar2), cVar7 == 0)) ||
      (cVar7 = fn_80229934(uVar4,uVar1,uVar5), cVar7 == 1)))) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    return;
  }
  fn_80207BC0(uVar1,uVar6);
  fn_80207BC0(uVar5,uVar3);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80215720 | Size: 0xe8 | Ghidra import */
void fn_80215720(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  u32 uVar4;
  u16 uVar5;
  u16 uVar6;

  if ((lbl_8047B618 & 0x200) == 0) {
    uVar1 = fn_801F025C(0x11,0);
    uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
    fn_80205184(uVar1);
    uVar3 = fn_8011BEB4(uVar2,0,0x2f,0);
    uVar4 = fn_80205B8C(uVar1);
    uVar5 = (int)fn_8012640C(uVar4,0,0x83,0);
    uVar1 = fn_80205B8C(uVar1);
    uVar6 = (int)fn_8012640C(uVar1,0,0x87,0);
    uVar3 = (u16)((s32)(uVar3 * uVar5) / (s32)uVar6);
    if (uVar3 == 0) {
      uVar3 = 1;
    }
    fn_8011BBD8(uVar2,0,0x2f,0,uVar3);
  }
  *(u32*)&lbl_8047B610 = *(u32*)&lbl_8047B610 + 1;
  return;
}
/* Address: 0x80215808 | Size: 0xc8 | Ghidra import */
void fn_80215808(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar3;
  u32 uVar2;
  u32 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar1 = fn_80205B8C(uVar1);
  uVar3 = (u32)(int)fn_8012640C(uVar1,0,0x83,0) & 0xffff;
  fn_801F025C(0x12,0);
  uVar1 = fn_80205B8C();
  uVar4 = (u32)(int)fn_8012640C(uVar1,0,0x83,0) & 0xffff;
  if ((int)uVar4 > (int)uVar3) goto _do_if;
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
_do_if:
  fn_8011BBD8(uVar2,0,0x2d,0,uVar4 - uVar3);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x802158D0 | Size: 0x84 | Ghidra import */
void fn_802158D0(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_80203CCC();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x12,0);
  cVar2 = fn_802025B8(uVar1,0x26);
  if (cVar2 == 2) {
    cVar2 = fn_80203CCC(uVar1);
    if (cVar2 != 0) goto _then;
  }
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
_then:
  fn_8020248C(uVar1,0x26,0);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80215954 | Size: 0x124 | Ghidra import */
void fn_80215954(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
  u32 uVar1;
  u32 uVar2;
  u16 uVar5;
  short sVar4;
  short sVar6;
  u16 uVar7;
  u32 uVar8;
  u16 uVar3;

  uVar3 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (u32)fn_8012640C(uVar1,0,0xd9,0);
  sVar4 = (int)fn_8012640C(uVar1,0,0x102,0);
  uVar5 = (int)fn_8012640C(uVar1,0,0x103,0);
  sVar6 = (int)fn_8012640C(uVar1,0,0x104,0);
  uVar7 = (int)fn_8012640C(uVar1,0,0x105,0);
  uVar1 = fn_801F025C(0x12,0);
  uVar8 = fn_801F0134(uVar1,uVar3);
  if (((sVar4 != 0) && (uVar5 == (u16)uVar8)) ||
      ((sVar6 != 0) && (uVar7 == (u16)uVar8))) {
    fn_8011BBD8(uVar2,0,0x2c,0,2);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80215A78 | Size: 0x74 | Ghidra import */
void fn_80215A78(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0x25);
  if (cVar2 != 2) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    fn_8020248C(uVar1,0x25,0);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x80215AEC | Size: 0x184 | Ghidra import */

void fn_80215AEC(void)

{
    extern void fn_8011BBD8();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern s8 fn_80201704();
    extern void fn_80201764();
    extern u32 fn_80201890();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern u32 fn_80203B5C();
  u32 uVar1;
  u32 uVar2;
  s8 cVar5;
  u32 uVar3;
  u32 uVar4;

  cVar5 = *(char *)(*(int *)(lbl_8047B610) + 1);
  if (cVar5 == 1) {
    uVar1 = fn_801F025C(0x11,0);
    uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
    uVar2 = fn_801F025C(0x12,0);
    uVar3 = fn_80201890(uVar2,0x35);
    fn_80132A38(0xd,uVar3);
    uVar4 = fn_80203B5C(uVar2,2);
    fn_8011BBD8(uVar1,0,0x2d,0,-(uVar4 & 0xffff));
    cVar5 = fn_80201704(uVar2);
    if (cVar5 == 1) {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 2);
    }
    else {
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 6;
    }
  }
  else if (cVar5 == 0) {
    uVar1 = fn_801F025C(0x11,0);
    uVar2 = (int)fn_8012640C(uVar1,0,0xd5,0);
    uVar2 = (int)fn_8012640C(uVar2,0,0xcb,0);
    uVar2 = (int)fn_8012640C(uVar2,0,0x77,0);
    cVar5 = fn_802025B8(uVar1,0x35);
    if (cVar5 == 2) {
      fn_8020248C(uVar1,0x35,0);
      fn_80201764(uVar1,0x35,uVar2);
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 6;
    }
    else {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 2);
    }
  }
  return;
}
/* Address: 0x80215C70 | Size: 0x80 | Ghidra import */
void fn_80215C70(void)

{
    extern u32 fn_801F025C();
    extern void fn_80207BC0();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(0x11,0);
  fn_80207BF4();
  fn_801F025C(0x12,0);
  uVar2 = fn_80207BF4();
  if (((uVar2 & 0xffff) == 0) || ((uVar2 & 0xffff) == 0x19)) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  else {
    fn_80207BC0(uVar1,uVar2);
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
  }
  return;
}
/* Address: 0x80215CF0 | Size: 0x358 | Ghidra import */
void fn_80215CF0(void)

{
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern s8 fn_80142984();
    extern u32 fn_801F025C();
    extern s8 fn_801F54A4();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_8020147C();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern s8 fn_80204A10();
    extern u8 lbl_80478D7D;
  u32 bVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  short sVar7;
  u32 uVar5;
  s8 cVar8;
  int iVar6;

  uVar2 = fn_801F025C(0x11,0);
  uVar3 = fn_802040E8();
  uVar4 = fn_801F025C(0x12,0);
  sVar7 = fn_80207BF4();
  uVar5 = fn_802040E8(uVar4);
  cVar8 = fn_801F54A4(0,0,0x2f,0);
  if ((cVar8 == 1) && (cVar8 = fn_80204A10(uVar2), cVar8 == 0)) {
    bVar1 = 0;
  }
  else {
    cVar8 = fn_801F54A4(0,0,0x2f,0);
    if ((cVar8 == 1) && (cVar8 = fn_802026E4(uVar2,0x3d), cVar8 == 1)) {
      bVar1 = 0;
    }
    else {
      bVar1 = 1;
    }
  }
  if (bVar1 == 0) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    return;
  }
  iVar6 = fn_801F54A4(0,0,0x2f,0);
  if ((iVar6 == 0) ||
     ((cVar8 = fn_802026E4(uVar2,0x3d), cVar8 == 0 &&
      (cVar8 = fn_802026E4(uVar4,0x3d), cVar8 == 0)))) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if (bVar1) {
    if ((((((uVar3 & 0xffff) != 0) || ((uVar5 & 0xffff) != 0)) && ((uVar3 & 0xffff) != 0xaf)) &&
        ((uVar5 & 0xffff) != 0xaf)) &&
       ((((uVar3 & 0xffff) == 0 || (cVar8 = fn_80142984(uVar3), cVar8 != 0)) &&
        (((uVar5 & 0xffff) == 0 || (cVar8 = fn_80142984(uVar5), cVar8 != 0)))))) {
      if (sVar7 == 0x3c) {
        *(u32 *)(lbl_8047B610) = 0x803798d8;
        return;
      }
      if ((uVar3 & 0xffff) != 0) {
        fn_8020147C(uVar2,0,0,0);
      }
      if ((uVar5 & 0xffff) != 0) {
        fn_8020147C(uVar4,0,0,0);
      }
      if ((uVar3 & 0xffff) != 0) {
        fn_8020147C(uVar4,uVar3,1,0);
      }
      if ((uVar5 & 0xffff) != 0) {
        fn_801254B4(uVar2,0,0xfb,0);
      }
      cVar8 = fn_801FECD4(uVar2);
      if (cVar8 == 1) {
        fn_801FE7EC(uVar2,0x82,0,0);
      }
      cVar8 = fn_801FECD4(uVar4);
      if (cVar8 == 1) {
        fn_801FE7EC(uVar4,0x82,0,0);
      }
      fn_80202810(uVar2,0x36);
      fn_80202810(uVar4,0x36);
      itemGetStatus(0,uVar5,1,0);
      uVar2 = fn_800FA280();
      fn_80132A38(0xd,uVar2);
      itemGetStatus(0,uVar3,1,0);
      uVar2 = fn_800FA280();
      fn_80132A38(0xe,uVar2);
      if (((uVar3 & 0xffff) == 0) || ((uVar5 & 0xffff) == 0)) {
        if ((uVar5 & 0xffff) == 0) {
          lbl_80478D7D = 1;
        }
        else {
          lbl_80478D7D = 0;
        }
      }
      else {
        lbl_80478D7D = 2;
      }
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
      return;
    }
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    return;
  }
  *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  return;
}
/* Address: 0x80216048 | Size: 0xa4 | Ghidra import */
u32 fn_80216048(u32 r3)
{
    extern u8 fn_801F54A4();
    extern u8 fn_802026E4();
    extern u8 fn_80204A10();
  u8 cVar2;
  u32 uVar1;
  
  cVar2 = fn_801F54A4(0,0,0x2f,0);
  if ((cVar2 == 1) && (cVar2 = fn_80204A10(r3), cVar2 == 0)) {
    uVar1 = 0;
  }
  else {
    cVar2 = fn_801F54A4(0,0,0x2f,0);
    if ((cVar2 == 1) && (cVar2 = fn_802026E4(r3,0x3d), cVar2 == 1)) {
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  return uVar1;
}
/* Address: 0x802160EC | Size: 0x104 | Ghidra import */
void fn_802160EC(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern u8 fn_802062FC();
  u32 uVar1;
  int iVar2;
  u16 uVar3;
  u8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  iVar2 = fn_801F025C(0xe,uVar1);
  if ((((iVar2 != 0) && (uVar3 = fn_801F54A4(0,0,0x19,0), 2 <= uVar3)) &&
      (cVar4 = fn_802062FC(iVar2), cVar4 == 1)) &&
     (cVar4 = fn_802026E4(uVar1,0x32), cVar4 == 0) &&
     (cVar4 = fn_802026E4(iVar2,0x32), cVar4 == 0)) {
    fn_801F4C14(0,0,0x43,0,iVar2);
    cVar4 = fn_802025B8(iVar2,0x32);
    if (cVar4 == 2) {
      fn_8020248C(iVar2,0x32,0);
    }
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x802161F0 | Size: 0x74 | Ghidra import */
void fn_802161F0(void)
{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x12,0);
  cVar2 = fn_802025B8(uVar1,0x30);
  if (cVar2 == 2) goto eq2;
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  goto done;
eq2:
  fn_8020248C(uVar1,0x30,0);
  lbl_8047B610 = lbl_8047B610 + 5;
done:
  return;
}
/* Address: 0x80216264 | Size: 0x8c | Ghidra import */
void fn_80216264(void)

{
    extern u32 fn_801F025C();
  u32 uVar1;
  short sVar2;
  short sVar3;

  uVar1 = fn_801F025C(0x11,0);
  sVar2 = (int)fn_8012640C(uVar1,0,0x102,0);
  sVar3 = (int)fn_8012640C(uVar1,0,0x104,0);
  if ((sVar2 != 0) || (sVar3 != 0)) {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x802162F0 | Size: 0x74 | Ghidra import */
void fn_802162F0(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x12,0);
  cVar2 = fn_802025B8(uVar1,0x1b);
  if (cVar2 == 2) goto eq2;
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  goto done;
eq2:
  fn_8020248C(uVar1,0x1b,0);
  lbl_8047B610 = lbl_8047B610 + 5;
done:
  return;
}
/* Address: 0x80216364 | Size: 0xac | Ghidra import */
void fn_80216364(void)

{
    extern void fn_801252E0();
    extern u32 fn_801F025C();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_80202998();
    extern u8 fn_80203C5C();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u8 cVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  cVar3 = fn_80203C5C(uVar1);
  if (cVar3 == 1) {
    fn_801252E0(uVar2);
    fn_80202998(uVar1,0);
    lbl_8047B610 = lbl_8047B610 + 5;
    cVar3 = fn_801FECD4(uVar1);
    if (cVar3 == 1) {
      fn_801FE7EC(uVar1,0x7c,0,0);
    }
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x80216410 | Size: 0x140 | Ghidra import */
void fn_80216410(void)

{
    extern u32 fn_800FA280();
    extern u16 fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801363E8();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_80209C1C();
    extern void fn_80211B94();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
    extern u8 lbl_80377EA4[];
    extern u32 lbl_80379BFF[];
  u16 uVar5;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar6;
  u32 uVar7;

  uVar5 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar6 = fn_801F54A4(0,0,0xf,0);
  uVar3 = fn_801363E8(uVar6);
  uVar4 = fn_8011BEB4(0,uVar3,9,0);
  fn_80209C1C(uVar2,uVar3);
  fn_8011BEB4(0,uVar3,1,0);
  uVar2 = fn_800FA280();
  fn_80132A38(0x28,uVar2);
  uVar7 = fn_8022B2CC(uVar1,uVar3,uVar5,0,1,1, (void*)0xffffffff);
  fn_801F4C14(0,0,0x43,0,uVar7);
  lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
  fn_80211B94(lbl_8047B62C,(u32)lbl_80377EA4,0);
  lbl_8047B610 = lbl_80379BFF[(uVar4 & 0xffff)];
  return;
}
/* Address: 0x80216550 | Size: 0x64 | Ghidra import */
void fn_80216550(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0x24);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x24,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802165B4 | Size: 0x9c | Ghidra import */
void fn_802165B4(void)

{
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_801F6DF0();
    extern u8 fn_801F6E44();
  u32 uVar1;
  u32 uVar2;
  u8 cVar4;
  u16 uVar3;
  u32 uVar5;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(2,uVar1);
  cVar4 = fn_801F6E44(uVar2,0x4d);
  if (cVar4 == 2) {
    uVar3 = fn_801F54A4(0,0,0x14,0);
    uVar5 = fn_801F0134(uVar1,uVar3);
    fn_801F6DF0(uVar2,0x4d,uVar5);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80216650 | Size: 0x130 | Ghidra import */
void fn_80216650(void)

{
    extern void fn_80011E68();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u32 fn_80205B8C();
    extern u32 fn_802656AC();
    extern u8 lbl_80478D78;
  u16 uVar8;
  u32 uVar1;
  u32 uVar2;
  u32 uVar4;
  u8 cVar6;
  u8 cVar7;

  {
    u32 uVar3;
    {
      u16 uVar5;
      uVar5 = fn_801F54A4(0,0,0x14,0);
      uVar1 = fn_801F025C(0x11,0);
      uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
      uVar3 = fn_802656AC(uVar1,uVar5,1);
    }
    uVar4 = fn_801F025C(0x12,0);
    cVar6 = (int)fn_8012640C(uVar4,0,0xe6,0);
    cVar7 = (int)fn_8012640C(uVar4,0,0xe8,0);
    if (((cVar6 == 0) && (cVar7 == 0)) && (*((&lbl_80478D78)+6) != 1)) {
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar1 = fn_80205B8C(uVar1);
      uVar8 = (int)fn_8012640C(uVar1,0,0x83,0);
      fn_8011BBD8(uVar2,0,0x2d,0,uVar8);
      fn_80011E68(uVar3,0);
      lbl_8047B610 = lbl_8047B610 + 5;
    }
  }
  return;
}
/* Address: 0x80216780 | Size: 0x84 | Ghidra import */
void fn_80216780(void)

{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F4C14();
    extern u8 lbl_80478D78;
  u8 cVar1;

  cVar1 = fn_801F2988(0,0x52);
  if (cVar1 != 2) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    *((&lbl_80478D78)+5) = 2;
  }
  else {
    fn_801F2934(0,0x52,0);
    *((&lbl_80478D78)+5) = 5;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80216804 | Size: 0x70 | Ghidra import */
void fn_80216804(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  if (((lbl_8047B618 & 0x2000000) != 0) &&
     (cVar2 = fn_802025B8(uVar1,0x23), cVar2 == 2)) {
    fn_8020248C(uVar1,0x23,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80216874 | Size: 0xec | Ghidra import */
void fn_80216874(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern void fn_80202810();
  u32 uVar1;
  int uVar2;
  u8 cVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (u16)fn_80205184();
  switch (uVar2) {
  case 0x13:
  case 0x154:
    cVar3 = fn_802026E4(uVar1,0x1f);
    if (cVar3 == 1) {
      fn_80202810(uVar1,0x1f);
    }
    break;
  case 0x5b:
    cVar3 = fn_802026E4(uVar1,0x20);
    if (cVar3 == 1) {
      fn_80202810(uVar1,0x20);
    }
    break;
  case 0x123:
    cVar3 = fn_802026E4(uVar1,0x21);
    if (cVar3 == 1) {
      fn_80202810(uVar1,0x21);
    }
    break;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80216960 | Size: 0xf8 | Ghidra import */
void fn_80216960(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar1;
  u16 uVar2;
  u8 cVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184();
  switch (uVar2) {
  case 0x13:
  case 0x154:
    cVar3 = fn_802025B8(uVar1,0x1f);
    if (cVar3 == 2) {
      fn_8020248C(uVar1,0x1f,0);
    }
    break;
  case 0x5b:
    cVar3 = fn_802025B8(uVar1,0x20);
    if (cVar3 == 2) {
      fn_8020248C(uVar1,0x20,0);
    }
    break;
  case 0x123:
    cVar3 = fn_802025B8(uVar1,0x21);
    if (cVar3 == 2) {
      fn_8020248C(uVar1,0x21,0);
    }
    break;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80216A58 | Size: 0x2a0 | Ghidra import */
void fn_80216A58(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern s8 fn_80122DDC();
    extern s8 fn_801233F4();
    extern u32 fn_8012A5B0();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern int fn_801F9930();
    extern u32 fn_801FB1C0();
    extern s8 fn_802026E4();
    extern u32 fn_80203D3C();
    extern void fn_80203E0C();
    extern u32 fn_80205BE8();
    extern s8 fn_802062FC();
    extern s8 fn_80206A04();
    extern u8 lbl_80478D78[1];
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  s8 cVar12;
  s8 cVar13;
  u16 uVar11;
  u32 uVar7;
  int iVar8;
  u32 uVar9;
  int iVar10;

  u32 uVar14;
  
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80205184(uVar1);
  uVar4 = fn_801F4354(0,uVar1);
  uVar5 = fn_801FB1C0(uVar4,0,0x44,0);
  uVar6 = fn_801F025C(0x12,0);
  cVar12 = fn_802062FC();
  if (cVar12 == 0) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  else {
    cVar12 = *(char *)(lbl_80478D78);
    uVar14 = 0;
    while (1) {
      if (5 < *(u8 *)(lbl_80478D78)) break;
      uVar14 = fn_8012A5B0(uVar5,3);
      iVar8 = fn_801F9930(uVar4,uVar14);
      if ((iVar8 != 0) && (cVar13 = fn_80206A04(), cVar13 != 0)) {
        uVar14 = fn_80205BE8(iVar8);
        cVar13 = fn_801233F4();
        if ((cVar13 == 1) && (cVar13 = fn_80122DDC(uVar14), cVar13 == 1)) break;
      }
      *(char *)(lbl_80478D78) = *(char *)(lbl_80478D78) + 1;
    }
    if (*(u8 *)(lbl_80478D78) < 6) {
      uVar11 = (int)fn_8012640C(uVar14,0,0x6e,0);
      uVar7 = (int)fn_8012640C(uVar14,0,0x7a,0);
      uVar4 = fn_80203D3C(uVar6);
      fn_80203E0C(uVar6);
      uVar5 = (int)fn_8012640C(uVar14,0,0x77,0);
      fn_80132A38(0xd,uVar5);
      iVar8 = (int)fn_8012640C(0,uVar11,4,0);
      uVar9 = fn_8011BEB4(0,uVar3,7,0);
      iVar10 = (int)fn_8012640C(0,uVar4,5,0);
      if (iVar10 < 1) {
        iVar10 = 1;
      }
      iVar8 = ((int)(iVar8 * (uVar9 & 0xffff) * (((uVar7 & 0xff) << 1) / 5 + 2)) / iVar10) / 0x32 +
              2;
      cVar12 = fn_802026E4(uVar1,0x32);
      if (cVar12 == 1) {
        iVar8 = (iVar8 * 0xf) / 10;
      }
      fn_8011BBD8(uVar2,0,0x2d,0,iVar8);
      *(char *)(lbl_80478D78) = *(char *)(lbl_80478D78) + 1;
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 9;
    }
    else if (cVar12 == 0) {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 5);
    }
    else {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
  }
  return;
}
/* Address: 0x80216CF8 | Size: 0xa4 | Ghidra import */
int fn_80216CF8(u32 r3, u32 r4, u32 r5, u32 r6)

{
    extern u32 fn_8011BEB4();
  int iVar1;
  u32 uVar2;
  int iVar3;
  int div5;

  iVar1 = (int)fn_8012640C(0,r4,4,0);
  uVar2 = fn_8011BEB4(0,r3,7,0);
  div5 = (s32)((r5 & 0xffff) << 1) / 5;
  iVar1 = iVar1 * (uVar2 & 0xffff);
  iVar1 = iVar1 * (div5 + 2);
  iVar3 = (int)fn_8012640C(0,r6,5,0);
  if (iVar3 <= 0) {
    iVar3 = 1;
  }
  iVar1 = iVar1 / iVar3;
  return iVar1 / 0x32 + 2;
}
/* Address: 0x80216D9C | Size: 0x1b4 | Ghidra import */
void fn_80216D9C(void)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_80201764();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern int fn_80232110();
    extern u8 lbl_80478D78;
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
  uVar4 = fn_80205184(uVar1);
  uVar10 = fn_8011BEB4(uVar11,0,0x2f,0);
  uVar8 = fn_8011BEB4(uVar11,0,0x30,0);
  uVar3 = fn_801F025C(0x12,0);
  uVar5 = fn_801F025C(2,uVar3);
  cVar9 = fn_802025B8(uVar3,0x34);
  if (cVar9 != 2) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    return;
  }
  fn_8020248C(uVar3,0x34,uVar2);
  fn_80201B2C(uVar3,0x34,uVar4);
  iVar6 = fn_80232110(uVar1,uVar3,uVar5,uVar4,uVar10,uVar8);
  cVar9 = fn_802026E4(uVar1,0x32);
  if (cVar9 == 1) {
    iVar6 = (iVar6 * 0xf) / 10;
  }
  fn_80201764(uVar3,0x34,iVar6);
  if ((uVar4 & 0xffff) == 0x161) {
    *((&lbl_80478D78)+5) = 1;
  }
  else {
    *((&lbl_80478D78)+5) = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80216F50 | Size: 0xc8 | Ghidra import */
void fn_80216F50(void)

{
    extern int fn_801F025C();
    extern void fn_801F4C14();
    extern int fn_801F54A4();
    extern s8 fn_802062FC();
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  int iVar1;
  int iVar2;
  s8 cVar3;

  iVar1 = fn_801F025C(0x11,0);
  lbl_8047B649 = 8;
  lbl_8047B648 = 0;
  while ((iVar2 = 0, lbl_8047B648 < lbl_8047B649 &&
         (((iVar2 = fn_801F54A4(0,0,0x5d), iVar2 == 0 || (cVar3 = fn_802062FC(), cVar3 == 0))
          || (iVar1 == iVar2))))) {
    lbl_8047B648 = (char)lbl_8047B648 + 1;
  }
  if (iVar2 != 0) {
    fn_801F4C14(0,0,0x43,0,iVar2);
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x80217018 | Size: 0x9c | Ghidra import */
void fn_80217018(void)

{
    extern void fn_8011BBD8();
    extern void fn_80120B00();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u16 local_buf[2];

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar1 = fn_80205B8C(uVar1);
  fn_80120B00(uVar1,&local_buf[1],&local_buf[0]);
  fn_8011BBD8(uVar2,0,0x2f,0,local_buf[1]);
  fn_8011BBD8(uVar2,0,0x30,0,local_buf[0]);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802170B4 | Size: 0x108 | Ghidra import */
void fn_802170B4(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_80201704();
    extern u32 fn_80203B5C();
  u32 uVar1;
  u32 uVar2;
  u8 cVar4;
  u8 cVar5;
  u32 uVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  fn_801F4C14(0,0,0x43,0,uVar1);
  cVar4 = (u8)(int)fn_801F453C(0,1);
  cVar5 = fn_80201704(uVar1);
  if (cVar5 == 0) {
    if (cVar4 == 0) {
      uVar3 = fn_80203B5C(uVar1,2);
      uVar3 = uVar3 & 0xffff;
    }
    else if (cVar4 == 1) {
      uVar3 = fn_80203B5C(uVar1,1);
      uVar3 = ((int)(uVar3 & 0xffff) * 0x14) / 0x1e;
    }
    else {
      uVar3 = fn_80203B5C(uVar1,4);
      uVar3 = uVar3 & 0xffff;
    }
    fn_8011BBD8(uVar2,0,0x2d,0,-uVar3);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x802171BC | Size: 0x64 | Ghidra import */
void fn_802171BC(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0x1a);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x1a,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80217220 | Size: 0x1b4 | Ghidra import */
void fn_80217220(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern int fn_801F00D0();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern s8 fn_801F6E98();
    extern void fn_801F6EEC();
    extern u32 fn_80201C58();
    extern u32 fn_80201D84();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
    extern u8 lbl_80378EFD[];
    extern u8 lbl_80378F11[];
    extern u8 lbl_80378F25[];
  u16 uVar7;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar8;
  u32 uVar4;
  u32 iVar5;
  u32 uVar6;

  uVar7 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(2,uVar1);
  uVar3 = fn_801F025C(0x12,0);
  cVar8 = fn_802026E4(uVar1,0xe);
  if (((cVar8 == 1) && (uVar4 = fn_80201D84(uVar1,0xe), (uVar4 & 0xffff) != 0)) &&
     (iVar5 = fn_801F00D0(uVar4,uVar7), iVar5 != 0)) {
    fn_801F4C14(0,0,0x42,0,iVar5);
    uVar6 = fn_80201C58(uVar1,0xe);
    fn_8011BEB4(0,uVar6,1,0);
    uVar6 = fn_800FA280();
    fn_80132A38(0xd,uVar6);
    fn_80202810(uVar1,0xe);
    fn_80211B94(lbl_8047B62C,lbl_80378EFD,0);
    fn_801F4C14(0,0,0x42,0,uVar3);
  }
  cVar8 = fn_802026E4(uVar1,0x1c);
  if (cVar8 == 1) {
    fn_80202810(uVar1,0x1c);
    fn_80211B94(lbl_8047B62C,lbl_80378F11,0);
  }
  cVar8 = fn_801F6E98(uVar2,0x4a);
  if (cVar8 == 1) {
    fn_801F6EEC(uVar2,0x4a);
    fn_80211B94(lbl_8047B62C,lbl_80378F25,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802173D4 | Size: 0x60 | Ghidra import */
void fn_802173D4(void)

{
    extern u32 fn_801F025C();
    extern void fn_80201600();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(0x12,0);
  fn_80201600(uVar1,uVar2);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x80217434 | Size: 0xf0 | Ghidra import */
void fn_80217434(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_80203B5C();
    extern u32 fn_80205B8C();
  u32 uVar5;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u16 uVar4;
  s8 cVar6;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80205B8C(uVar1);
  uVar4 = (int)fn_8012640C(uVar3,0,0x83,0);
  uVar5 = fn_80203B5C(uVar1,2);
  cVar6 = (int)fn_8012640C(uVar1,0,0xe6,0);
  if ((cVar6 < '\f') && (uVar4 > (u16)uVar5)) {
    fn_801254B4(uVar1,0,0xe6,0,0xc);
    fn_8011BBD8(uVar2,0,0x2d,0,uVar5 & 0xffff);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x80217524 | Size: 0x84 | Ghidra import */
void fn_80217524(void)
{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F4C14();
    extern u8 lbl_80478D78;
  u8 cVar1;

  cVar1 = fn_801F2988(0,0x53);
  if (cVar1 == 2) goto eq2;
  fn_801F4C14(0,0,0x3b,0,0x40);
  *((&lbl_80478D78)+5) = 2;
  goto done;
eq2:
  fn_801F2934(0,0x53,0);
  *((&lbl_80478D78)+5) = 4;
done:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802175A8 | Size: 0x23c | Ghidra import */
void fn_802175A8(void)

{
    extern u32 DAT_8038ff5a;
    extern short fn_8011BEB4();
    extern short fn_801F0134();
    extern int fn_801F025C();
    extern short fn_801F0898();
    extern u8 fn_801F1170();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    extern u8 fn_80202B88();
    extern u8 fn_802062FC();
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  int iVar1;
  u16 uVar5;
  short sVar6;
  u8 cVar8;
  int iVar2;
  int iVar3;
  short sVar7;
  u32 uVar4;

  int iVar9;
  
  iVar1 = fn_801F025C(0x14,0);
  uVar5 = fn_801F54A4(0,0,0x14,0);
  sVar6 = fn_801F0134(iVar1,uVar5);
  cVar8 = fn_802062FC(iVar1);
  if (cVar8 == 0) {
    lbl_8047B648 = lbl_8047B649;
  }
  else {
    iVar9 = 0;
    while (iVar2 = 0, lbl_8047B648 < lbl_8047B649) {
      iVar2 = fn_801F54A4(0,0,0x5d);
      iVar9 = iVar2;
      if (((((iVar2 != 0) && (cVar8 = fn_802062FC(), cVar8 != 0)) && (iVar1 != iVar2)) &&
          ((cVar8 = fn_80202B88(iVar1,iVar2), cVar8 != 1 &&
           (iVar3 = (int)fn_8012640C(iVar2,0,0xfe,0), iVar3 != 0)))) &&
         ((cVar8 = fn_801F1170(), cVar8 != 0 && (sVar7 = fn_801F0898(iVar3), sVar7 == 0x13))))
      {
        uVar4 = (int)fn_8012640C(iVar2,0,0xd9,0);
        sVar7 = fn_80205184(iVar2);
        if ((((sVar7 == 0xe4) && (sVar7 = fn_8011BEB4(uVar4,0,0x29,0), sVar6 == sVar7)) &&
            (cVar8 = fn_802026E4(iVar2,8), cVar8 != 1)) &&
           ((cVar8 = fn_802026E4(iVar2,7), cVar8 != 1 &&
            (iVar3 = (int)fn_8012640C(iVar2,0,0xf9,0), iVar3 != 1)))) break;
      }
      lbl_8047B648 = (char)lbl_8047B648 + 1;
    }
    if (iVar2 != 0) {
      fn_801F4C14(0,0,0x42,0,iVar2);
      DAT_8038ff5a = 1;
      lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
      fn_801254B4(iVar9,0,0x112,0,1);
      lbl_8047B610 = lbl_8047B610 + 5;
      return;
    }
  }
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x802177E4 | Size: 0x110 | Ghidra import */
void fn_802177E4(void)

{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
  u8 uVar4;
  u32 uVar2;
  u32 uVar1;
  u8 uVar3;

  uVar2 = (int)fn_8012640C(fn_801F025C(0x11,0),0,0xd9,0);
  uVar3 = (u8)((s32)(fn_800E0C54() & 0xffff) % 100);
  if (uVar3 < 5) {
    uVar1 = 10;
    uVar4 = 4;
  }
  else if (uVar3 < 0xf) {
    uVar1 = 0x1e;
    uVar4 = 5;
  }
  else if (uVar3 < 0x23) {
    uVar1 = 0x32;
    uVar4 = 6;
  }
  else if (uVar3 < 0x41) {
    uVar1 = 0x46;
    uVar4 = 7;
  }
  else if (uVar3 < 0x55) {
    uVar1 = 0x5a;
    uVar4 = 8;
  }
  else if (uVar3 < 0x5f) {
    uVar1 = 0x6e;
    uVar4 = 9;
  }
  else {
    uVar1 = 0x96;
    uVar4 = 10;
  }
  fn_8011BBD8(uVar2,0,0x2f,0,uVar1 & 0xffff);
  fn_80132A38(0x2f,uVar4);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802178F4 | Size: 0xa8 | Ghidra import */
void fn_802178F4(void)
{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_801F6DF0();
    extern u8 fn_801F6E44();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = fn_801F025C(2,uVar1);
  cVar2 = fn_801F6E44(uVar1,0x4b);
  if (cVar2 == 2) goto eq2;
  fn_801F4C14(0,0,0x3b,0,0x40);
  *((&lbl_80478D78)+5) = 0;
  goto done;
eq2:
  fn_801F6DF0(uVar1,0x4b,0);
  *((&lbl_80478D78)+5) = 5;
done:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021799C | Size: 0x148 | Ghidra import */
void fn_8021799C(void)

{
    extern int fn_800E0C54();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u8 fn_80201704();
    extern u32 fn_80203B5C();
    extern void fn_80209960();
    extern u8 lbl_80375F98[];
    extern u8 lbl_80377AD9[];
    extern u8 lbl_80377AB8[];
  u32 uVar2;
  u32 uVar1;
  u8 bVar4;
  u32 uVar3;
  u8 cVar5;

  uVar1 = (int)fn_8012640C(fn_801F025C(0x11,0),0,0xd9,0);
  uVar2 = fn_801F025C(0x12,0);
  bVar4 = fn_800E0C54();
  if (bVar4 < 0x66) {
    fn_8011BBD8(uVar1,0,0x2f,0,0x28);
  }
  else if (bVar4 < 0xb2) {
    fn_8011BBD8(uVar1,0,0x2f,0,0x50);
  }
  else if (bVar4 < 0xcc) {
    fn_8011BBD8(uVar1,0,0x2f,0,0x78);
  }
  else {
    fn_8011BBD8(uVar1,0,0x2d,0,-(fn_80203B5C(uVar2,4) & 0xffff));
  }
  if (bVar4 < 0xcc) {
    lbl_8047B610 = (u32)lbl_80375F98;
  }
  else {
    cVar5 = fn_80201704(uVar2);
    if (cVar5 == 1) {
      lbl_8047B610 = (u32)lbl_80377AD9;
    }
    else {
      fn_80209960(uVar1,0x43);
      lbl_8047B610 = (u32)lbl_80377AB8;
    }
  }
  return;
}
/* Address: 0x80217AE4 | Size: 0xec | Ghidra import */
void fn_80217AE4(void)

{
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  short sVar5;
  u32 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  uVar3 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar1 = fn_80205184(uVar1);
  sVar5 = fn_8011BEB4(0,uVar1,9,0);
  uVar4 = (int)fn_8012640C(uVar2,0,0x99,0);
  if (sVar5 == 0x79) {
    uVar4 = ((uVar4 & 0xffff) * 10) / 0x19;
  }
  else {
    uVar4 = (int)((0xff - (uVar4 & 0xffff)) * 10) / 0x19;
  }
  fn_8011BBD8(uVar3,0,0x2f,0,uVar4 & 0xffff);
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x80217BD0 | Size: 0x1c | Ghidra import */
u32 fn_80217BD0(u32 r3)
{
  return (int)((0xff - (r3 & 0xffff)) * 10) / 0x19 & 0xffff;
}
/* Address: 0x80217BEC | Size: 0x18 | Ghidra import */
u32 fn_80217BEC(u32 r3)

{
  return (u16)((s32)((r3 & 0xffff) * 10) / 0x19);
}
/* Address: 0x80217C04 | Size: 0x130 | Ghidra import */
void fn_80217C04(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern short fn_80202360();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802096E8();
    extern u8 lbl_80375FDF[];
  int iVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar7;
  short sVar6;
  u32 uVar5;

  uVar2 = fn_801F025C(0x11,0);
  uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar4 = fn_80205184(uVar2);
  cVar7 = fn_802096E8(uVar3);
  if (cVar7 == 0) {
    cVar7 = fn_802026E4(uVar2,0x2e);
    if (cVar7 == 1) {
      fn_80202810(uVar2,0x2e);
    }
    lbl_8047B610 = (u32)lbl_80375FDF;
  }
  else {
    cVar7 = fn_802025B8(uVar2,0x2e);
    if (cVar7 == 2) {
      fn_8020248C(uVar2,0x2e,0);
    }
    sVar6 = fn_80202360(uVar2,0x2e);
    {
      u32 uVar7;
      uVar5 = fn_8011BEB4(0,uVar4,7,0) & 0xffff;
      uVar7 = uVar5;
      for (iVar1 = 1; iVar1 < sVar6; iVar1 = iVar1 + 1) {
        uVar7 = uVar7 << 1;
      }
      fn_8011BBD8(uVar3,0,0x2f,0,uVar7);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  return;
}
/* Address: 0x80217D34 | Size: 0xb4 | Ghidra import */
void fn_80217D34(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_802026E4();
  u8 bVar1;
  u32 uVar2;
  u32 uVar3;
  s8 cVar4;
  s8 cVar5;

  bVar1 = *(u8 *)(*(int *)(lbl_8047B610) + 1);
  uVar3 = fn_801F025C(0x12,0);
  uVar2 = (u32)bVar1;
  if (uVar2 < 8) {

    ((int (*)(void))**(void ***)(uVar2 * 4 + -0x7fc65fd8))();
    return;
  }
  cVar4 = (int)fn_8012640C(uVar3,0,0,0);
  cVar5 = fn_802026E4(uVar3,9);
  if ((cVar5 == 1) && ('\v' < cVar4)) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 2);
  }
  else {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 6;
  }
  return;
}
/* Address: 0x80217E20 | Size: 0x1f8 | Ghidra import */
void fn_80217E20(void)

{
    extern u32 fn_80119DD0();
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_80200B10();
    extern u32 fn_80202360();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802096E8();
    extern void fn_8020A2B8();
    extern u8 lbl_80375FDF[];
  u32 uVar3;
  u32 uVar1;
  u32 uVar2;
  u32 uVar4;
  u8 cVar8;
  u8 uVar5;
  u32 uVar6;
  u16 sVar7;
  u32 uVar9;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xf8,0);
  uVar4 = fn_80205184(uVar1);
  cVar8 = fn_802096E8(uVar2);
  if (cVar8 == 0) {
    fn_80200B10(uVar1);
    lbl_8047B610 = (u32)lbl_80375FDF;
  }
  else {
    cVar8 = fn_802025B8(uVar1,0x2f);
    if (cVar8 == 2) {
      fn_8020248C(uVar1,0x2f,0);
      cVar8 = fn_802026E4(uVar1,0x22);
      if (cVar8 == 0) {
        fn_8020248C(uVar1,0x22,0);
        fn_8020A2B8(uVar3,uVar2);
      }
    }
    uVar6 = 1;
    cVar8 = fn_802026E4(uVar1,0x2f);
    if (cVar8 == 1) {
      uVar5 = fn_80119DD0(0x2f);
      uVar6 = fn_80202360(uVar1,0x2f);
      if ((short)uVar6 == uVar5) {
        fn_80202810(uVar1,0x2f);
        fn_80202810(uVar1,0x22);
      }
    }
    sVar7 = fn_8011BEB4(0,uVar4,7,0);
    for (uVar9 = 1; (int)(uVar9 & 0xffff) < (int)(short)uVar6; uVar9 = uVar9 + 1) {
      sVar7 = sVar7 << 1;
    }
    fn_8011BBD8(uVar2,0,0x2f,0,sVar7);
    cVar8 = fn_802026E4(uVar1,0x1a);
    if (cVar8 == 1) {
      uVar9 = (u16)fn_8011BEB4(uVar2,0,0x2f,0);
      fn_8011BBD8(uVar2,0,0x2f,0,(uVar9 << 1) & 0xfffe);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  return;
}
/* Address: 0x80218018 | Size: 0x1c0 | Ghidra import */
void fn_80218018(void)

{
    extern int fn_80123B5C();
    extern int fn_801F025C();
    extern int fn_801F3624();
    extern void fn_801F37B0();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern u32 fn_80201890();
    extern s8 fn_802026E4();
    extern u32 fn_80205B8C();
    extern int fn_80229934();
  int iVar1;
  u32 uVar2;
  u32 uVar3;
  s8 cVar5;
  u8 bVar6;
  u8 bVar7;
  u8 bVar8;
  u32 uVar4;

  int local_28 [5];
  
  local_28[0] = 0;
  iVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184();
  uVar3 = fn_801F025C(0x12,0);
  cVar5 = fn_80229934(uVar2,iVar1,uVar3);
  if (cVar5 == 1) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  else {
    fn_801F37B0(0,0x802181d8,local_28,0);
    if (iVar1 != 0) {
      bVar6 = fn_801F3624(0,0x2e,0,iVar1);
      if (bVar6 != 0) {
        uVar2 = fn_80205B8C(iVar1);
        bVar7 = fn_80123B5C(uVar2,0xc3);
        if (-1 < (char)bVar7) {
          bVar8 = (int)fn_8012640C(uVar2,0,0x80,(int)(char)bVar7);
          fn_801254B4(uVar2,0,0x80,(int)(char)bVar7,bVar8 - bVar6 & -(bVar6 < bVar8));
          cVar5 = fn_802026E4(iVar1,0x10);
          if (cVar5 == 0) {
            cVar5 = fn_802026E4(iVar1,0x31);
            if (cVar5 == 1) {
              uVar4 = fn_80201890(iVar1,0x31);
              if (((uVar4 & 1 << (u32)bVar7) == 0) &&
                 (cVar5 = fn_801FECD4(iVar1), cVar5 == 1)) {
                fn_801FE7EC(iVar1,0x80,(u32)bVar7,0);
              }
            }
          }
        }
      }
    }
    if (local_28[0] == 0) {
      iVar1 = *(int *)(*(int *)(lbl_8047B610) + 1);
    }
    else {
      iVar1 = *(int *)(lbl_8047B610) + 5;
    }
    *(int *)(lbl_8047B610) = iVar1;
  }
  return;
}
/* Address: 0x802181D8 | Size: 0x98 | Ghidra import */
u32 fn_802181D8(void)

{
    u32 r3;
    u32 r4;
    int *r5;

    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802062FC();
  s8 cVar2;
  short sVar1;
  
  cVar2 = fn_802062FC();
  if (cVar2 != 0) {
    sVar1 = fn_80207BF4(r3);
    cVar2 = fn_802025B8(r3,0x1e);
    if ((cVar2 == 2) && (sVar1 != 0x2b)) {
      fn_8020248C(r3,0x1e,0);
      *r5 = *r5 + 1;
    }
  }
  return 1;
}
/* Address: 0x80218270 | Size: 0x64 | Ghidra import */
void fn_80218270(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x12,0);
  cVar2 = fn_802025B8(uVar1,0x19);
  if (cVar2 == 2) {
    fn_8020248C(uVar1,0x19,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x802182D4 | Size: 0xe8 | Ghidra import */
void fn_802182D4(void)

{
    extern u32 fn_80119DD0();
    extern u32 fn_801F025C();
    extern u32 fn_801F6D9C();
    extern void fn_801F6DF0();
    extern s8 fn_801F6E44();
    extern s8 fn_801F6E98();
  u32 uVar1;
  u32 uVar2;
  u8 cVar5;
  int uVar3;
  u8 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(3,uVar1);
  cVar5 = fn_801F6E44(uVar2,0x4a);
  if (cVar5 == 2) {
    cVar5 = fn_801F6E98(uVar2,0x4a);
    if (cVar5 == 1) {
      uVar3 = fn_801F6D9C(uVar2,0x4a);
    }
    else {
      uVar3 = 0;
    }
    uVar4 = fn_80119DD0(0x4a);
    if ((s16)uVar3 != uVar4) goto _drop;
  }
  fn_801254B4(uVar1,0,0x118,0,1);
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
_drop:
  fn_801F6DF0(uVar2,0x4a,0);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* Address: 0x802183BC | Size: 0xc0 | Ghidra import */
void fn_802183BC(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern u32 fn_80203B5C();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  s8 cVar5;
  u16 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_801F025C(0x12,0);
  cVar5 = fn_802025B8(uVar3,0x18);
  if (cVar5 == 2) {
    fn_8020248C(uVar3,0x18,0);
    uVar4 = fn_80203B5C(uVar1,2);
    fn_8011BBD8(uVar2,0,0x2d,0,uVar4);
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
  }
  else {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  return;
}
/* Address: 0x8021847C | Size: 0x3a8 | Ghidra import */
void fn_8021847C(void)

{
    extern u16 fn_801248C4();
    extern void fn_801252E0();
    extern u32 fn_8012A5B0();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_801F7258();
    extern int fn_801F9930();
    extern u32 fn_801FB1C0();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_80202810();
    extern void fn_80202998();
    extern void fn_80205B8C();
    extern u32 fn_80205BE8();
    extern u8 fn_80206780();
    extern u8 fn_80206A04();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u32 sVar7;
  u32 iVar2;
  u32 uVar3;
  u16 uVar4;
  u8 cVar8;
  u32 uVar5;
  int iVar6;

  u32 uVar9;
  u32 uVar10;

  uVar1 = fn_801F025C(0x11,0);
  sVar7 = fn_80205184();
  iVar2 = fn_801F025C(0xe,uVar1);
  uVar3 = fn_801F025C(2,uVar1);
  uVar4 = fn_801F54A4(0,0,0x16,0);
  if ((sVar7 & 0xffff) == 0xd7) {
    *((&lbl_80478D78)+5) = 0;
    sVar7 = fn_80207BF4(uVar1);
    if ((sVar7 & 0xffff) != 0x2b) {
      fn_80205B8C(uVar1);
      fn_801252E0();
      fn_80202998(uVar1,0);
      fn_80202810(uVar1,0x17);
      cVar8 = fn_801FECD4(uVar1);
      if (cVar8 == 1) {
        fn_801FE7EC(uVar1,0x7c,0,0);
      }
    }
    else {
      *((&lbl_80478D78)+5) = *((&lbl_80478D78)+5) | 1;
    }
    if ((iVar2 != 0) && (cVar8 = fn_80206780(iVar2), cVar8 == 1)) {
      fn_801F4C14(0,0,0x4b,0,iVar2);
      sVar7 = fn_80207BF4(iVar2);
      if ((sVar7 & 0xffff) != 0x2b) {
        fn_80205B8C(iVar2);
        fn_801252E0();
        fn_80202998(iVar2,0);
        fn_80202810(iVar2,0x17);
        cVar8 = fn_801FECD4(iVar2);
        if (cVar8 == 1) {
          fn_801FE7EC(iVar2,0x7c,0,0);
        }
      }
      else {
        *((&lbl_80478D78)+5) = *((&lbl_80478D78)+5) | 2;
      }
    }
    for (uVar9 = 0; uVar9 < uVar4; uVar9 = uVar9 + 1) {
      iVar2 = fn_801F7258(uVar3,uVar9 & 0xffff);
      if (iVar2 != 0) {
        uVar1 = fn_801FB1C0(iVar2,0,0x44,0);
        uVar10 = 0;
        do {
          uVar5 = fn_8012A5B0(uVar1,3,uVar10 & 0xffff);
          iVar6 = fn_801F9930(iVar2,uVar5);
          if ((iVar6 != 0) && (cVar8 = fn_80206A04(), cVar8 != 0)) {
            uVar5 = fn_80205BE8(iVar6);
            sVar7 = fn_801248C4();
            if ((sVar7 & 0xffff) != 0x2b) {
              fn_801252E0(uVar5);
            }
          }
          uVar10 = uVar10 + 1;
        } while (uVar10 < 6);
      }
    }
  }
  else {
    *((&lbl_80478D78)+5) = 4;
    fn_80205B8C(uVar1);
    fn_801252E0();
    fn_80202810(uVar1,0x17);
    cVar8 = fn_801FECD4(uVar1);
    if (cVar8 == 1) {
      fn_801FE7EC(uVar1,0x7c,0,0);
    }
    if ((iVar2 != 0) && (cVar8 = fn_80206780(iVar2), cVar8 == 1)) {
      fn_801F4C14(0,0,0x4b,0,iVar2);
      fn_80205B8C(iVar2);
      fn_801252E0();
      fn_80202998(iVar2,0);
      fn_80202810(iVar2,0x17);
      cVar8 = fn_801FECD4(iVar2);
      if (cVar8 == 1) {
        fn_801FE7EC(iVar2,0x7c,0,0);
      }
    }
    for (uVar9 = 0; uVar9 < uVar4; uVar9 = uVar9 + 1) {
      iVar2 = fn_801F7258(uVar3,uVar9 & 0xffff);
      if (iVar2 != 0) {
        uVar1 = fn_801FB1C0(iVar2,0,0x44,0);
        uVar10 = 0;
        do {
          uVar5 = fn_8012A5B0(uVar1,3,uVar10 & 0xffff);
          iVar6 = fn_801F9930(iVar2,uVar5);
          if ((iVar6 != 0) && (cVar8 = fn_80206A04(), cVar8 != 0)) {
            fn_80205BE8(iVar6);
            fn_801252E0();
          }
          uVar10 = uVar10 + 1;
        } while (uVar10 < 6);
      }
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80218824 | Size: 0x248 | Ghidra import */
void fn_80218824(void)

{
    extern int fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8011BEB4();
    extern u8 fn_80123CD4();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_80200B10();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u16 sVar5;
  u16 sVar6;
  u8 bVar7;
  u8 bVar8;
  u32 uVar3;
  u8 cVar9;
  u32 uVar4;

  u8 bVar10;
  
  fn_801F025C(0x11,0);
  uVar1 = fn_801F025C(0x12,0);
  uVar2 = fn_80205B8C();
  sVar5 = (int)fn_8012640C(uVar1,0,0xf0,0);
  if ((((sVar5 == 0) || (sVar5 == 0x165)) || (sVar5 == 0xffff)) || (sVar5 == 0x164)) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    bVar7 = 0;
    bVar10 = 0;
    while (1) {
      if (3 < bVar10) break;
      cVar9 = fn_80123CD4(uVar2,bVar10);
      if ((cVar9 != 0) && (sVar6 = (int)fn_8012640C(uVar2,0,0x7f,bVar10), sVar5 == sVar6)) {
        bVar7 = (int)fn_8012640C(uVar2,0,0x80,bVar10);
        break;
      }
      bVar10 = bVar10 + 1;
    }
    if (bVar10 < 4) {
      if (bVar7 < 2) {
        lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
      }
      else {
        bVar8 = fn_800E0C54();
        bVar8 = (bVar8 & 3) + 2;
        if (bVar7 < bVar8) {
          bVar8 = bVar7;
        }
        fn_8011BEB4(0,sVar5,1,0);
        uVar3 = fn_800FA280();
        fn_80132A38(0xd,uVar3);
        fn_80132A38(0x2f,bVar8);
        fn_801254B4(uVar2,0,0x80,bVar10,bVar7 - bVar8);
        cVar9 = fn_802026E4(uVar1,0x10);
        if (cVar9 == 0) {
          cVar9 = fn_802026E4(uVar1,0x31);
          if (cVar9 == 1) {
            uVar4 = fn_80201890(uVar1,0x31);
            if ((uVar4 & 1 << bVar10) == 0) {
              cVar9 = fn_801FECD4(uVar1);
              if (cVar9 == 1) {
                fn_801FE7EC(uVar1,0x80,(u32)bVar10,0);
              }
            }
          }
        }
        if ((u8)(bVar7 - bVar8) == 0) {
          fn_80200B10(uVar1);
        }
        lbl_8047B610 = lbl_8047B610 + 5;
      }
    }
    else {
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
  }
  return;
}
/* 0x80218A6C | size: 0x100 | medium */
#pragma optimization_level 0
void fn_80218A6C(void) {
    extern u8 lbl_80279F7C[];
    extern void fn_8011BBD8();
    extern void fn_801F025C();
    extern void fn_80205B8C();
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r3 = 0x11;
    r4 = 0x0;
    fn_801F025C();
    r4 = 0x0;
    var_r31 = r3;
    r5 = 0xd9;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    tmp = r3;
    r3 = var_r31;
    var_r31 = tmp;
    fn_80205B8C();
    r4 = 0x0;
    var_r30 = r3;
    r5 = 0x83;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    var_r29 = r3 & 0xFFFF;
    r3 = var_r30;
    r4 = 0x0;
    r5 = 0x87;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    tmp = var_r29 * 0x30;
    r3 = r3 & 0xFFFF;
    tmp = (s32)tmp / (s32)r3;
    r4 = tmp & 0xFF;
    if (((s32)tmp == 0) && (var_r29 != 0)) {

        r4 = 0x1;
    }
    r3 = (u32)lbl_80279F7C;
    tmp = 0x6;
    r3 = (u32)lbl_80279F7C;
    r4 = r4 & 0xFF;
    r5 = 0x0;
    ctr_fn = (void(*)(void))tmp;
    do {
        tmp = *(u8*)((u8*)r3 + 0x0);
        if (r4 > tmp) {
            r5 = r5 + 0x2;
            r3 = r3 + 0x2;
        }
    } while (--ctr != 0);
    r4 = (u32)lbl_80279F7C;
    r3 = var_r31;
    tmp = (u32)lbl_80279F7C;
    r6 = tmp + r5;
    r4 = 0x0;
    r7 = *(u8*)((u8*)r6 + 0x1);
    r5 = 0x2f;
    r6 = 0x0;
    fn_8011BBD8();
    r3 = *(u32*)&lbl_8047B610;
    tmp = r3 + 0x1;
    *(u32*)&lbl_8047B610 = tmp;
    return;
}
#pragma optimization_level 4

/* 0x80218B6C | size: 0x68 | small */
#pragma optimization_level 0
void fn_80218B6C(void) {
    extern u8 lbl_80279F7C[];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r5 = r3 & 0xFFFF;
    tmp = r4 & 0xFFFF;
    r3 = r5 * 0x30;
    tmp = (s32)r3 / (s32)tmp;
    r4 = tmp & 0xFF;
    if (((s32)tmp == 0) && (r5 != 0)) {

        r4 = 0x1;
    }
    r3 = (u32)lbl_80279F7C;
    tmp = 0x6;
    r3 = (u32)lbl_80279F7C;
    r4 = r4 & 0xFF;
    r5 = 0x0;
    ctr_fn = (void(*)(void))tmp;
    do {
        tmp = *(u8*)((u8*)r3 + 0x0);
        if (r4 > tmp) {
            r5 = r5 + 0x2;
            r3 = r3 + 0x2;
        }
    } while (--ctr != 0);
    r3 = (u32)lbl_80279F7C;
    tmp = (u32)lbl_80279F7C;
    r3 = tmp + r5;
    r3 = *(u8*)((u8*)r3 + 0x1);
    return;
}
#pragma optimization_level 4

/* Address: 0x80218BD4 | Size: 0xa0 | Ghidra import */
void fn_80218BD4(void)

{
    extern u32 fn_801F025C();
    extern s8 fn_802026E4();
    extern u32 lbl_8047B618;
  u32 uVar1;
  int iVar2;
  int iVar3;
  s8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  iVar2 = fn_801F025C(2,uVar1);
  uVar1 = fn_801F025C(0x12,0);
  iVar3 = fn_801F025C(2,uVar1);
  cVar4 = fn_802026E4(uVar1,0x15);
  if (cVar4 == 1) {
    if ((iVar2 != iVar3) && ((lbl_8047B618 & 0x1000000) == 0)) {
      lbl_8047B618 = lbl_8047B618 | 0x40;
    }
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x80218C74 | Size: 0xb0 | Ghidra import */
void fn_80218C74(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern int fn_80229934();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184();
  uVar3 = fn_801F025C(0x12,0);
  cVar4 = fn_80229934(uVar2,uVar1,uVar3);
  if (cVar4 == 1) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    cVar4 = fn_802025B8(uVar1,0x15);
    if (cVar4 == 2) {
      fn_8020248C(uVar1,0x15,0);
    }
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x80218D24 | Size: 0x2b8 | Ghidra import */
void fn_80218D24(void)

{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u8 fn_80123CD4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u8 fn_801FFEC8();
    extern u32 fn_80205B8C();
    extern int fn_8022B2CC();
    extern u16 lbl_8047B60C;
    extern u32 lbl_8047B618;
  u32 bVar1;
  u16 uVar6;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar8;
  u32 uVar5;
  short sVar7;
  u8 bVar9;

  short sVar10;
  short local_28 [6];
  
  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F025C(0x11,0);
  uVar3 = fn_80205B8C();
  uVar4 = (int)fn_8012640C(uVar2,0,0xd9,0);
  for (bVar9 = 0; bVar9 < 4; bVar9 = bVar9 + 1) {
    local_28[bVar9] = -1;
  }
  bVar9 = 0;
  for (sVar10 = 0; sVar10 < 4; sVar10 = sVar10 + 1) {
    cVar8 = fn_80123CD4(uVar3,sVar10);
    if (cVar8 != 0) {
      cVar8 = fn_801FFEC8(uVar2,sVar10,0,0);
      if ((cVar8 == 0) || (cVar8 == 6)) {
        sVar7 = (int)fn_8012640C(uVar3,0,0x7f,sVar10);
        if ((sVar7 != 0) && ((sVar7 != 0x165 && (sVar7 != 0x163)))) {
          if ((sVar7 == 0) ||
             ((((sVar7 == 0x165 || (sVar7 == 0xd6)) || (sVar7 == 0x112)) ||
              ((sVar7 == 0x77 || (sVar7 == 0x76)))))) {
            bVar1 = 1;
          }
          else {
            bVar1 = 0;
          }
          if (((!bVar1) && (sVar7 != 0x108)) && (sVar7 != 0xfd)) {
            sVar7 = fn_8011BEB4(0,sVar7,9,0);
            if (((((sVar7 == 0x91) || (sVar7 == 0x27)) || (sVar7 == 0x4b)) ||
                ((sVar7 == 0x97 || (sVar7 == 0x9b)))) || (sVar7 == 0x1a)) {
              bVar1 = 1;
            }
            else {
              bVar1 = 0;
            }
            if (bVar1 == 0) {
              local_28[bVar9] = sVar10;
              bVar9 = bVar9 + 1;
            }
          }
        }
      }
    }
  }
  if (bVar9 != 0) {
    uVar5 = fn_800E0C54();
    sVar10 = *(short *)((int)local_28 +
                       (((uVar5 & 0xffff) - ((uVar5 & 0xffff) / (u32)bVar9) * (u32)bVar9) * 2 &
                       0x1fe));
    if (-1 < sVar10) {
      sVar7 = (int)fn_8012640C(uVar3,0,0x7f,sVar10);
      if (((sVar7 != 0) && (sVar7 != 0x165)) && (sVar7 != 0x163)) {
        lbl_8047B60C = sVar7;
        fn_8011BBD8(uVar4,0,0x26,0,(int)sVar10);
        lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
        uVar2 = fn_8022B2CC(uVar2,lbl_8047B60C,uVar6,0,1,1, (void*)0xffffffff);
        fn_801F4C14(0,0,0x43,0,uVar2);
        lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
        return;
      }
    }
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
/* fn_80218FDC | Size: 0x40 | Check if ID is in special set */
BOOL fn_80218FDC(u16 id) {
    if (id == 0 || id == 0x165 || id == 0xD6 ||
        id == 0x112 || id == 0x77 || id == 0x76) {
        return TRUE;
    }
    return FALSE;
}

/* Address: 0x8021901C | Size: 0x70 | Ghidra import */
u32 fn_8021901C(u32 r3)

{
    extern u16 fn_8011BEB4();
  u16 sVar2;
  u32 uVar1;

  sVar2 = fn_8011BEB4(0,r3,9,0);
  if ((((sVar2 == 0x91) || (sVar2 == 0x27)) || (sVar2 == 0x4b)) ||
     (((sVar2 == 0x97 || (sVar2 == 0x9b)) || (sVar2 == 0x1a)))) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}
/* Address: 0x8021908C | Size: 0x1e4 | Ghidra import */
void fn_8021908C(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern u8 fn_8011BEB4();
    extern s8 fn_80123B5C();
    extern void fn_80123D58();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern s8 fn_801FECD4();
    extern s8 fn_802026E4();
    extern u32 fn_80205BE8();
  u32 bVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 sVar6;
  s8 cVar7;
  u32 uVar5;

  int iVar8;

  uVar2 = fn_801F025C(0x11,0);
  uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar4 = fn_801F025C(0x12,0);
  sVar6 = (int)fn_8012640C(uVar4,0,0xef,0);
  fn_8011BBD8(uVar3,0,0x27,0,0xffff);
  cVar7 = fn_802026E4(uVar2,0x10);
  if (cVar7 != 1) {
    if ((((sVar6 == 0x164) || ((u16)(sVar6 - 0xa5U) < 2)) || (sVar6 == 0xffff)) ||
       ((sVar6 == 0 || (sVar6 == 0x165)))) {
      bVar1 = 1;
    }
    else {
      bVar1 = 0;
    }
    if (bVar1 == 0) {
      uVar4 = (int)fn_8012640C(uVar2,0,0xd5,0);
      uVar5 = (int)fn_8012640C(uVar2,0,0xd6,0);
      uVar4 = fn_80205BE8(uVar4);
      uVar5 = fn_80205BE8(uVar5);
      cVar7 = fn_80123B5C(uVar4,sVar6);
      if (cVar7 < 0) {
        cVar7 = fn_8011BEB4(uVar3,0,0x26,0);
        iVar8 = (int)cVar7;
        if (-1 < iVar8) {
          fn_80123D58(uVar4,iVar8,sVar6);
          cVar7 = fn_801FECD4(uVar2);
          if (cVar7 == 1) {
            fn_80123D58(uVar5,iVar8,sVar6);
          }
          fn_8011BEB4(0,sVar6,1,0);
          uVar2 = fn_800FA280();
          fn_80132A38(0xd,uVar2);
          lbl_8047B610 = lbl_8047B610 + 5;
          return;
        }
      }
    }
  }
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x80219270 | Size: 0x44 | Ghidra import */
u32 fn_80219270(u32 r3)

{
  if ((((u16)r3 == 0x164) || (1 >= (u16)(r3 - 0xa5U))) || ((u16)r3 == 0xffff) ||
     (((u16)r3 == 0 || ((u16)r3 == 0x165)))) {
    return 1;
  }
  return 0;
}
/* Address: 0x802192B4 | Size: 0xa0 | Ghidra import */
void fn_802192B4(void)

{
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
  u32 uVar0;
  u16 uVar3;
  u32 uVar2;
  u32 uVar1;
  u8 cVar4;

  uVar0 = fn_801F025C(0x11,0);
  uVar3 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F0134(uVar0,uVar3);
  uVar2 = fn_801F025C(0x12,0);
  cVar4 = fn_802025B8(uVar2,0x1d);
  if (cVar4 == 2) {
    fn_8020248C(uVar2,0x1d,uVar1);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80219354 | Size: 0x24c | Ghidra import */
void fn_80219354(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8010C4D4();
    extern short fn_8011BEB4();
    extern void fn_80132A38();
    extern int fn_801F00D0();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern s8 fn_802026E4();
    extern u32 fn_802079DC();
    extern void fn_80207B5C();
  u32 bVar1;
  u16 uVar5;
  u32 uVar2;
  short sVar6;
  u16 uVar7;
  u16 uVar8;
  int iVar3;
  s8 cVar10;
  u16 uVar9;
  u32 uVar4;

  u32 auStack_58 [18];
  
  uVar5 = fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F025C(0x11,0);
  fn_8012640C(uVar2,0,0xd9,0);
  sVar6 = (int)fn_8012640C(uVar2,0,0xf3,0);
  fn_8012640C(uVar2,0,0x102,0);
  fn_8012640C(uVar2,0,0x104,0);
  uVar7 = (int)fn_8012640C(uVar2,0,0xf4,0);
  uVar8 = (int)fn_8012640C(uVar2,0,0xf2,0);
  iVar3 = fn_801F00D0(uVar8,uVar5);
  if ((((sVar6 == 0) || (sVar6 == 0x165)) || (sVar6 == -1)) || (sVar6 == 0x164)) {
    *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  }
  else {
    if (iVar3 != 0) {
      sVar6 = fn_8011BEB4(0,sVar6,9,0);
      if ((((sVar6 == 0x91) || (sVar6 == 0x27)) ||
          ((sVar6 == 0x4b || ((sVar6 == 0x97 || (sVar6 == 0x9b)))))) || (sVar6 == 0x1a)) {
        bVar1 = 1;
      }
      else {
        bVar1 = 0;
      }
      if ((bVar1) && (cVar10 = fn_802026E4(iVar3,0x22), cVar10 == 1)) {
        *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
        return;
      }
    }
    uVar9 = fn_802079DC(uVar2,uVar7,auStack_58);
    if (uVar9 == 0) {
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
    else {
      uVar4 = fn_800E0C54();
      uVar4 = *(u32 *)((int)auStack_58 +
                       (((uVar4 & 0xffff) - ((uVar4 & 0xffff) / (u32)uVar9) * (u32)uVar9) * 4 &
                       0x3fffc));
      if ((int)uVar4 < 0) {
        *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
      }
      else {
        uVar4 = uVar4 & 0xffff;
        for (uVar9 = 0; uVar9 < 2; uVar9 = uVar9 + 1) {
          fn_80207B5C(uVar2,uVar9 & 0xff,uVar4);
        }
        fn_8010C4D4(uVar4);
        uVar2 = fn_800FA280();
        fn_80132A38(0xd,uVar2);
        *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
      }
    }
  }
  return;
}
/* Address: 0x802195A0 | Size: 0x108 | Ghidra import */
void fn_802195A0(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern u32 fn_80205B8C();
    extern u32 lbl_8047B608;
  u32 uVar2;
  u16 uVar6;
  u16 uVar4;
  u32 uVar1;
  u32 uVar5;
  u32 uVar3;
  int iVar1;
  u8 cVar7;

  uVar1 = fn_801F025C(0x11,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar4 = (int)fn_8012640C(fn_80205B8C(uVar1),0,0x83,0);
  uVar5 = fn_801F025C(0x12,0);
  uVar2 = fn_80205B8C();
  uVar6 = (int)fn_8012640C(uVar2,0,0x83,0);
  cVar7 = fn_802026E4(uVar5,0x14);
  if (cVar7 == 0) {
    iVar1 = (uVar4 + uVar6) / 2;
    lbl_8047B608 = uVar6 - iVar1;
    fn_8011BBD8(uVar3,0,0x2d,0,uVar4 - iVar1);
    fn_801254B4(uVar5,0,0x11b,0,0xffff);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x802196A8 | Size: 0x15c | Ghidra import */
void fn_802196A8(void)

{
    extern s8 fn_80123B5C();
    extern u32 fn_801F025C();
    extern void fn_80201764();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u32 fn_80205B8C();
  u32 bVar1;
  u32 uVar2;
  u32 uVar3;
  u16 sVar4;
  s8 cVar6;
  u8 cVar7;
  u8 cVar8;
  u16 uVar5;

  uVar2 = fn_801F025C(0x12,0);
  uVar3 = fn_80205B8C();
  sVar4 = (int)fn_8012640C(uVar2,0,0xf0,0);
  cVar6 = fn_80123B5C(uVar3,sVar4);
  if (cVar6 < 0) {
    cVar7 = 0;
  }
  else {
    cVar7 = (int)fn_8012640C(uVar3,0,0x80,(int)cVar6);
  }
  if ((((sVar4 == 0xa5) || (sVar4 == 0xe3)) || (sVar4 == 0x77)) || (sVar4 == 0xffff)) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if (bVar1) {
    cVar7 = 0;
    cVar6 = -1;
  }
  cVar8 = fn_802025B8(uVar2,0x2a);
  if (((cVar8 == 2) && (-1 < cVar6)) && (cVar7 != 0)) {
    fn_8020248C(uVar2,0x2a,0);
    uVar5 = (int)fn_8012640C(uVar3,0,0x7f,(int)cVar6);
    fn_80201B2C(uVar2,0x2a,uVar5);
    fn_80201764(uVar2,0x2a,(int)cVar6);
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x80219804 | Size: 0x34 | Ghidra import */
u32 fn_80219804(u16 r3)

{
  if (r3 == 0xa5 || r3 == 0xe3 || r3 == 0x77 || r3 == 0xffff) {
    return 1;
  }
  return 0;
}
/* Address: 0x80219838 | Size: 0x12c | Ghidra import */
void fn_80219838(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BEB4();
    extern s8 fn_80123B5C();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  s8 cVar4;
  s8 cVar5;
  s8 cVar6;

  uVar1 = fn_801F025C(0x12,0);
  uVar2 = fn_80205B8C();
  uVar3 = (int)fn_8012640C(uVar1,0,0xf0,0);
  cVar4 = fn_80123B5C(uVar2,uVar3);
  if (cVar4 < 0) {
    cVar5 = 0;
  }
  else {
    cVar5 = (int)fn_8012640C(uVar2,0,0x80,(int)cVar4);
  }
  cVar6 = fn_802025B8(uVar1,0x29);
  if (((cVar6 == 2) && (-1 < cVar4)) && (cVar5 != 0)) {
    uVar3 = (int)fn_8012640C(uVar2,0,0x7f,(int)cVar4);
    fn_8011BEB4(0,uVar3,1,0);
    uVar2 = fn_800FA280();
    fn_80132A38(0xd,uVar2);
    fn_8020248C(uVar1,0x29,0);
    fn_80201B2C(uVar1,0x29,uVar3);
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
    return;
  }
  *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  return;
}
/* Address: 0x80219964 | Size: 0x1c8 | Ghidra import */
void fn_80219964(void)

{
    extern void fn_8011BBD8();
    extern int fn_801F00D0();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801F6C4C();
    extern u8 fn_801F6E98();
    extern u8 fn_80202B88();
    extern u8 fn_802062FC();
  u16 uVar6;
  u32 uVar1;
  u32 uVar2;
  u16 sVar7;
  short sVar8;
  int iVar3;
  u8 cVar9;
  u32 uVar4;
  int iVar5;
  int iVar10;

  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  sVar7 = (int)fn_8012640C(uVar1,0,0x105,0);
  sVar8 = (int)fn_8012640C(uVar1,0,0x104,0);
  if ((((sVar7 != 0) && (sVar8 != 0)) && (iVar3 = fn_801F00D0(sVar7,uVar6), iVar3 != 0)) &&
     ((cVar9 = fn_80202B88(uVar1,iVar3), cVar9 == 0 &&
      (cVar9 = fn_802062FC(iVar3), cVar9 == 1)))) {
    fn_8011BBD8(uVar2,0,0x2d,0,(int)sVar8 << 1);
    uVar1 = fn_801F025C(3,uVar1);
    uVar6 = fn_801F54A4(0,0,0x14,0);
    iVar10 = 0;
    cVar9 = fn_801F6E98(uVar1,0x4d);
    if (((cVar9 == 1) &&
        ((uVar4 = fn_801F6C4C(uVar1,0x4d), (uVar4 & 0xffff) != 0 &&
         (iVar5 = fn_801F00D0(uVar4,uVar6), iVar5 != 0)))) &&
       (cVar9 = fn_802062FC(), cVar9 == 1)) {
      iVar10 = iVar5;
    }
    if (iVar10 != 0) {
      iVar3 = iVar10;
    }
    fn_801F4C14(0,0,0x43,0,iVar3);
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
  fn_801254B4(uVar1,0,0x118,0,1);
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x80219B2C | Size: 0x1c8 | Ghidra import */
void fn_80219B2C(void)

{
    extern void fn_8011BBD8();
    extern int fn_801F00D0();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801F6C4C();
    extern u8 fn_801F6E98();
    extern u8 fn_80202B88();
    extern u8 fn_802062FC();
  u16 uVar6;
  u32 uVar1;
  u32 uVar2;
  u16 sVar7;
  short sVar8;
  int iVar3;
  u8 cVar9;
  u32 uVar4;
  int iVar5;
  int iVar10;

  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  sVar7 = (int)fn_8012640C(uVar1,0,0x103,0);
  sVar8 = (int)fn_8012640C(uVar1,0,0x102,0);
  if ((((sVar7 != 0) && (sVar8 != 0)) && (iVar3 = fn_801F00D0(sVar7,uVar6), iVar3 != 0)) &&
     ((cVar9 = fn_80202B88(uVar1,iVar3), cVar9 == 0 &&
      (cVar9 = fn_802062FC(iVar3), cVar9 == 1)))) {
    fn_8011BBD8(uVar2,0,0x2d,0,(int)sVar8 << 1);
    uVar1 = fn_801F025C(3,uVar1);
    uVar6 = fn_801F54A4(0,0,0x14,0);
    iVar10 = 0;
    cVar9 = fn_801F6E98(uVar1,0x4d);
    if (((cVar9 == 1) &&
        ((uVar4 = fn_801F6C4C(uVar1,0x4d), (uVar4 & 0xffff) != 0 &&
         (iVar5 = fn_801F00D0(uVar4,uVar6), iVar5 != 0)))) &&
       (cVar9 = fn_802062FC(), cVar9 == 1)) {
      iVar10 = iVar5;
    }
    if (iVar10 != 0) {
      iVar3 = iVar10;
    }
    fn_801F4C14(0,0,0x43,0,iVar3);
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
  fn_801254B4(uVar1,0,0x118,0,1);
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x80219CF4 | Size: 0xa4 | Ghidra import */
void fn_80219CF4(void)

{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_80203E0C();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80203E0C(uVar1) & 0xff;
  uVar4 = fn_800E0C54();
  fn_8011BBD8(uVar2,0,0x2d,0,(int)(uVar3 * (((int)(uVar4 & 0xffff) % 0xb) * 10 + 0x32)) / 100);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x80219D98 | Size: 0x78 | Ghidra import */
void fn_80219D98(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_80203E0C();
  u32 uVar1;
  u32 uVar2;
  u8 uVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80203E0C(uVar1);
  fn_8011BBD8(uVar2,0,0x2d,0,uVar3);
  lbl_8047B610++;
  return;
}
/* Address: 0x80219E10 | Size: 0x1d4 | Ghidra import */
void fn_80219E10(void)

{
    extern u32 fn_8000814C();
    extern u32 fn_800E0C54();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_80209C1C();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
  u16 uVar1;
  u32 bVar2;
  u32 uVar3;
  u16 uVar6;
  u32 uVar4;
  u32 uVar5;
  int iVar7;
  int iVar8;
  u32 uVar9;
  u32 uVar10;

  u32 uVar11;
  int iVar12;
  u16 local_338 [404];
  
  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar4 = fn_801F025C(0x11,0);
  uVar5 = (int)fn_8012640C(uVar4,0,0xd9,0);
  iVar7 = 0;
  iVar12 = 400;
  do {
    *(u16 *)((int)local_338 + iVar7) = 0;
    iVar7 = iVar7 + 2;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  uVar11 = 0;
  iVar7 = 0;
  uVar10 = 0;
  iVar12 = 0x163;
  do {
    if (uVar10 != 0) {
      uVar3 = uVar10 & 0xffff;
      if (uVar3 == 0) {
        bVar2 = 0;
      }
      else if (uVar3 == 0x165) {
        bVar2 = 0;
      }
      else if (uVar3 == 0x163) {
        bVar2 = 0;
      }
      else {
        for (iVar8 = 0;
            (uVar9 = (u32)*(u16 *)(iVar8 + -0x7fd86060), uVar9 != 0xffff && (uVar3 != uVar9));
            iVar8 = iVar8 + 2) {
        }
        if (uVar9 == 0xffff) {
          bVar2 = 1;
        }
        else {
          bVar2 = 0;
        }
      }
      if (bVar2) {
        uVar11 = uVar11 + 1;
        *(short *)((int)local_338 + iVar7) = (short)uVar10;
        iVar7 = iVar7 + 2;
      }
    }
    uVar10 = uVar10 + 1;
    iVar12 = iVar12 + -1;
  } while (iVar12 != 0);
  uVar10 = fn_800E0C54();
  uVar1 = local_338[(uVar10 & 0xffff) - ((uVar10 & 0xffff) / uVar11) * uVar11];
  uVar11 = fn_8000814C();
  uVar10 = (u32)uVar1;
  if ((uVar11 & 0xffff) != 0) {
    uVar10 = uVar11;
  }
  lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
  fn_80209C1C(uVar5,uVar10);
  uVar4 = fn_8022B2CC(uVar4,uVar10,uVar6,0,1,1, (void*)0xffffffff);
  fn_801F4C14(0,0,0x43,0,uVar4);
  uVar10 = fn_8011BEB4(0,uVar10,9,0);
  *(u32 *)(lbl_8047B610) = *(u32 *)((uVar10 & 0xffff) * 4 + -0x7fc86401);
  return;
}
/* Address: 0x80219FE4 | Size: 0x70 | Ghidra import */
u32 fn_80219FE4(u16 r3)

{
    extern u16 lbl_80279FA0[];
  u16 uVar1;
  int iVar2;

  if (r3 == 0) {
    return 0;
  }
  if (r3 == 0x165) {
    return 0;
  }
  if (r3 == 0x163) {
    return 0;
  }
  iVar2 = 0;
  while (1) {
    uVar1 = lbl_80279FA0[iVar2];
    if (uVar1 == 0xffff) {
      break;
    }
    if (r3 == uVar1) {
      break;
    }
    iVar2 = iVar2 + 1;
  }
  if (uVar1 != 0xffff) {
    return 0;
  }
  return 1;
}
/* Address: 0x8021A054 | Size: 0x26c | Ghidra import */
void fn_8021A054(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern s8 fn_8011BEB4();
    extern s8 fn_80123B5C();
    extern void fn_80123D58();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern s8 fn_801FEC10();
    extern s8 fn_801FECD4();
    extern void fn_80201764();
    extern u32 fn_80201890();
    extern void fn_8020248C();
    extern s8 fn_802026E4();
    extern u32 fn_80205B8C();
  short sVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  short sVar6;
  s8 cVar7;
  u8 bVar8;
  u32 uVar5;

  int iVar9;
  u32 bVar10;
  
  uVar2 = fn_801F025C(0x11,0);
  uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar4 = fn_801F025C(0x12,0);
  sVar6 = (int)fn_8012640C(uVar4,0,0xf0,0);
  fn_8011BBD8(uVar3,0,0x27,0,0xffff);
  cVar7 = fn_802026E4(uVar2,0x10);
  if (cVar7 != 1) {
    if (sVar6 == 0) {
      bVar10 = 1;
    }
    else if (sVar6 == 0x165) {
      bVar10 = 1;
    }
    else if (sVar6 == 0x163) {
      bVar10 = 1;
    }
    else {
      for (iVar9 = 0; (sVar1 = *(short *)(iVar9 + -0x7fd86060), sVar1 != -2 && (sVar6 != sVar1));
          iVar9 = iVar9 + 2) {
      }
      bVar10 = sVar1 != -2;
    }
    if ((((!bVar10) && (sVar6 != 0)) && (sVar6 != -1)) && ((sVar6 != 0x165 && (sVar6 != 0x163)))) {
      uVar4 = fn_80205B8C(uVar2);
      cVar7 = fn_80123B5C(uVar4,sVar6);
      if (cVar7 < 0) {
        cVar7 = fn_8011BEB4(uVar3,0,0x26,0);
        iVar9 = (int)cVar7;
        if ((-1 < iVar9) &&
           ((cVar7 = fn_801FECD4(uVar2), cVar7 != 0 ||
            (cVar7 = fn_801FEC10(uVar2), cVar7 != 0)))) {
          cVar7 = fn_802026E4(uVar2,0x31);
          if (cVar7 == 0) {
            fn_8020248C(uVar2,0x31,0);
          }
          uVar3 = fn_80205B8C(uVar2);
          fn_80123D58(uVar3,iVar9,sVar6);
          bVar8 = fn_8011BEB4(0,sVar6,2,0);
          if (5 < bVar8) {
            bVar8 = 5;
          }
          fn_801254B4(uVar3,0,0x80,iVar9,bVar8);
          fn_8011BEB4(0,sVar6,1,0);
          uVar3 = fn_800FA280();
          fn_80132A38(0xd,uVar3);
          uVar5 = fn_80201890(uVar2,0x31);
          fn_80201764(uVar2,0x31,uVar5 | 1 << iVar9);
          *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
          return;
        }
      }
    }
  }
  *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  return;
}
/* Address: 0x8021A2C0 | Size: 0x78 | Ghidra import */
u32 fn_8021A2C0(u16 r3)

{
    extern u16 lbl_80279FA0[];

  u16 uVar1;
  int iVar2;

  if (r3 == 0) {
    return 1;
  }
  if (r3 == 0x165) {
    return 1;
  }
  if (r3 == 0x163) {
    return 1;
  }
  iVar2 = 0;
  while ((uVar1 = lbl_80279FA0[iVar2]) != 0xfffe) {
    if (r3 == uVar1) {
      break;
    }
    iVar2 = iVar2 + 1;
  }
  return (u8)(uVar1 != 0xfffe);
}
/* Address: 0x8021A338 | Size: 0x140 | Ghidra import */
void fn_8021A338(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_80201764();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_80203B5C();
    extern u32 fn_80205B8C();
    extern u8 lbl_80478D7D;
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  u16 uVar4;
  s8 cVar5;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  uVar3 = (int)fn_8012640C(uVar2,0,0x83,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar4 = fn_80203B5C(uVar1,4);
  if ((uVar4 < uVar3) && (cVar5 = fn_802025B8(uVar1,0x14), cVar5 == 2)) {
    fn_8011BBD8(uVar2,0,0x2d,0,uVar4);
    fn_8020248C(uVar1,0x14,0);
    fn_80201764(uVar1,0x14,uVar4);
    cVar5 = fn_802026E4(uVar1,0xe);
    if (cVar5 == 1) {
      fn_80202810(uVar1,0xe);
    }
    lbl_80478D7D = 0;
    lbl_8047B618 = lbl_8047B618 | 0x100;
  }
  else {
    fn_8011BBD8(uVar2,0,0x2d,0,0);
    lbl_80478D7D = 1;
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  return;
}
/* Address: 0x8021A478 | Size: 0x254 | Ghidra import */
void fn_8021A478(void)

{
    extern void fn_8011BBD8();
    extern u8 fn_8011FC74();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_801FE91C();
    extern u8 fn_801FEC10();
    extern u8 fn_801FECD4();
    extern u16 fn_802010C8();
    extern void fn_80201764();
    extern void fn_80201B2C();
    extern void fn_80201EB0();
    extern void fn_80201FDC();
    extern void fn_8020248C();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_80203758();
    extern u32 fn_80205B8C();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u8 cVar9;
  u16 sVar8;
  u32 *puVar7;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_801F025C(0x12,0);
  uVar4 = fn_80205B8C();
  uVar4 = (int)fn_8012640C(uVar4,0,0x6f,0);
  uVar5 = fn_80205B8C(uVar3);
  uVar6 = (int)fn_8012640C(uVar5,0,0x75,0);
  fn_8011BBD8(uVar2,0,0x27,0,0xffff);
  cVar9 = fn_802026E4(uVar3,0x10);
  if ((cVar9 != 1) && (sVar8 = fn_802010C8(uVar3), sVar8 == 0)) {
    fn_80205B8C(uVar3);
    cVar9 = fn_8011FC74();
    if ((cVar9 != 1) &&
       ((cVar9 = fn_801FECD4(uVar1), cVar9 != 0 || (cVar9 = fn_801FEC10(uVar1), cVar9 != 0))
       )) {
      cVar9 = fn_802026E4(uVar1,0x10);
      if (cVar9 == 0) {
        fn_8020248C(uVar1,0x10,0);
      }
      fn_80201764(uVar1,0x10,uVar4);
      fn_80201EB0(uVar1,0x10,(int)(char)uVar6);
      fn_80201FDC(uVar1,0x10,(int)(char)(uVar6 >> 8));
      fn_80201B2C(uVar1,0x10,uVar6 >> 0x10);
      cVar9 = fn_802026E4(uVar1,0x29);
      if (cVar9 == 1) {
        fn_80202810(uVar1,0x29);
      }
      cVar9 = fn_802026E4(uVar1,0x31);
      if (cVar9 == 1) {
        fn_80202810(uVar1,0x31);
      }
      uVar2 = fn_80203758(uVar3);
      fn_80132A38(0xd,uVar2);
      *((&lbl_80478D78)+5) = 0;
      fn_801FE91C(uVar3,uVar1);
      puVar7 = (u32 *)fn_8012640C(uVar1,0,0x101,0);
      if (puVar7 != (void *)0) {
        *puVar7 = 0;
      }
      lbl_8047B610 = lbl_8047B610 + 1;
      return;
    }
  }
  fn_801F4C14(0,0,0x3b,0,0x45);
  *((&lbl_80478D78)+5) = 1;
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021A6CC | Size: 0x98 | Ghidra import */
void fn_8021A6CC(void)
{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0xf);
  if (cVar2 == 2) goto eq2;
  fn_801F4C14(0,0,0x3b,0,0x45);
  *((&lbl_80478D78)+5) = 1;
  goto done;
eq2:
  fn_8020248C(uVar1,0xf,0);
  *((&lbl_80478D78)+5) = 0;
done:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021A764 | Size: 0xa8 | Ghidra import */
void fn_8021A764(void)
{
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_801F6DF0();
    extern u8 fn_801F6E44();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = fn_801F025C(2,uVar1);
  cVar2 = fn_801F6E44(uVar1,0x4c);
  if (cVar2 == 2) goto eq2;
  fn_801F4C14(0,0,0x3b,0,0x45);
  *((&lbl_80478D78)+5) = 1;
  goto done;
eq2:
  fn_801F6DF0(uVar1,0x4c,0);
  *((&lbl_80478D78)+5) = 0;
done:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021A80C | Size: 0x6c | Ghidra import */
void fn_8021A80C(void)

{
    extern int fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_802653FC();
  u16 uVar2;
  u32 iVar1;

  uVar2 = fn_801F54A4(0,0,0x14,0);
  iVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  if (iVar1 != 0) {
    fn_802653FC(iVar1,uVar2,1);
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021A878 | Size: 0x10c | Ghidra import */
void fn_8021A878(void)

{
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802016A4();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 lbl_8037984D[];
  u32 uVar1;
  u8 cVar5;
  u16 uVar3;
  u32 uVar2;
  u32 uVar5;
  u8 cVar6;
  u16 uVar4;
  u8 cVar7;

  uVar1 = fn_801F025C(0x11,0);
  cVar5 = fn_802016A4();
  uVar3 = fn_801F54A4(0,0,0x14,0);
  uVar5 = fn_801F0134(uVar1,uVar3);
  uVar2 = fn_801F025C(0x12,0);
  cVar6 = fn_802016A4();
  uVar4 = fn_80207BF4(uVar2);
  if (uVar4 == 0xc) {
    lbl_8047B610 = (u32)lbl_8037984D;
  }
  else if ((((cVar5 == cVar6) || (cVar7 = fn_802025B8(uVar2,10), cVar7 != 2)) ||
           (cVar5 == 2)) || (cVar6 == 2)) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    fn_8020248C(uVar2,10,uVar5);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x8021A984 | Size: 0x194 | Ghidra import */
void fn_8021A984(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern u32 fn_80203B5C();
    extern u8 fn_802062FC();
    extern u8 fn_80207AE0();
  u32 uVar1;
  u32 uVar2;
  u32 sVar3;
  u32 uVar4;
  u8 cVar6;
  u8 cVar5;

  uVar4 = 0;
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  sVar3 = fn_80207BF4(uVar1);
  cVar5 = (int)fn_801F453C(0,1);
  cVar6 = fn_802062FC(uVar1);
  if (cVar6 == 0) {
    uVar4 = 0;
    goto LAB_00217ae0;
  }
  if (cVar5 == 3) {
    cVar6 = fn_80207AE0(uVar1,5);
    if (cVar6 == 0) {
      cVar6 = fn_80207AE0(uVar1,8);
      if ((cVar6 == 0) && (cVar6 = fn_80207AE0(uVar1,4), cVar6 == 0)) {
        if ((sVar3 & 0xffff) != 8) {
          cVar6 = fn_802026E4(uVar1,0x20);
          if ((cVar6 == 0) && (cVar6 = fn_802026E4(uVar1,0x21), cVar6 == 0)) {
            uVar4 = fn_80203B5C(uVar1,0x10) & 0xffff;
            goto LAB_00217a84;
          }
        }
      }
    }
    uVar4 = 0;
  }
LAB_00217a84:
  if (cVar5 == 4) {
    cVar5 = fn_80207AE0(uVar1,0xf);
    if (cVar5 == 0) {
      cVar5 = fn_802026E4(uVar1,0x20);
      if ((cVar5 == 0) && (cVar5 = fn_802026E4(uVar1,0x21), cVar5 == 0)) {
        uVar4 = fn_80203B5C(uVar1,0x10) & 0xffff;
        goto LAB_00217ae0;
      }
    }
    uVar4 = 0;
  }
LAB_00217ae0:
  fn_8011BBD8(uVar2,0,0x2d,0,uVar4);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021AB18 | Size: 0x84 | Ghidra import */
void fn_8021AB18(void)

{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F4C14();
    extern u8 lbl_80478D78;
  u8 cVar1;

  cVar1 = fn_801F2988(0,0x55);
  if (cVar1 != 2) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    *((&lbl_80478D78)+5) = 2;
  }
  else {
    fn_801F2934(0,0x55,0);
    *((&lbl_80478D78)+5) = 3;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021AB9C | Size: 0x80 | Ghidra import */
#pragma push
#pragma optimization_level 2
void fn_8021AB9C(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_80203ADC();
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar2 = fn_801F025C(0x12,0);
  uVar3 = fn_80203ADC(uVar2,2);
  fn_8011BBD8(uVar1,0,0x2d,0,uVar3);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#pragma pop
/* Address: 0x8021AC1C | Size: 0x390 | Ghidra import */
void fn_8021AC1C(void)

{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern short fn_801F0134();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern short fn_80201D84();
    extern s8 fn_802026E4();
    extern int fn_80203E0C();
    extern int fn_80203EDC();
    extern short fn_80203FE4();
    extern u32 fn_80205B8C();
    extern u8 lbl_80478D7D;
  u32 bVar1;
  u32 uVar2;
  u16 uVar10;
  short sVar11;
  u32 uVar3;
  u32 uVar4;
  u8 bVar14;
  short sVar12;
  int iVar5;
  u32 uVar6;
  u32 uVar7;
  short sVar13;
  u8 bVar15;
  u32 uVar8;
  s8 cVar16;
  u32 uVar9;

  uVar2 = fn_801F025C(0x11,0);
  uVar10 = fn_801F54A4(0,0,0x14,0);
  sVar11 = fn_801F0134(uVar2,uVar10);
  uVar3 = fn_80205184(uVar2);
  uVar4 = (int)fn_8012640C(uVar2,0,0xd9,0);
  bVar14 = fn_80203E0C(uVar2);
  uVar2 = fn_801F025C(0x12,0);
  uVar10 = fn_802040E8();
  sVar12 = fn_80203FE4(uVar2);
  iVar5 = fn_80203EDC(uVar2);
  uVar6 = fn_80205B8C(uVar2);
  uVar7 = (int)fn_8012640C(uVar6,0,0x83,0);
  uVar7 = uVar7 & 0xffff;
  sVar13 = fn_80207BF4(uVar2);
  bVar15 = fn_80203E0C(uVar2);
  fn_801F4C14(0,0,0x49,0,uVar2);
  if ((sVar12 == 0x27) && (uVar8 = fn_800E0C54(), (int)((uVar8 & 0xffff) % 100) < iVar5)) {
    fn_801254B4(uVar2,0,0x11a,0,1);
  }
  if (sVar13 != 5) {
    cVar16 = fn_802026E4(uVar2,0x1d);
    if ((cVar16 == 1) && (sVar12 = fn_80201D84(uVar2,0x1d), sVar11 == sVar12)) {
      if (bVar14 < bVar15) {
        bVar1 = 0;
      }
      else {
        bVar1 = 1;
      }
    }
    else {
      uVar8 = fn_8011BEB4(0,uVar3,6,0);
      uVar9 = fn_800E0C54();
      if (((uVar9 & 0xffff) % 100 + 1 < (((u32)bVar14 - (u32)bVar15) + (uVar8 & 0xff) & 0xffff))
         && (bVar15 <= bVar14)) {
        bVar1 = 1;
      }
      else {
        bVar1 = 0;
      }
    }
    if (bVar1) {
      cVar16 = fn_802026E4(uVar2,0x2c);
      if (cVar16 == 0) {
        iVar5 = (int)fn_8012640C(uVar2,0,0x11a,0);
        if (iVar5 == 0) {
          fn_8011BBD8(uVar4,0,0x2d,0,uVar7);
          fn_801F4C14(0,0,0x3b,0,0x44);
        }
        else {
          fn_8011BBD8(uVar4,0,0x2d,0,uVar7 - 1);
          fn_801F4C14(0,0,0x3b,0,0x47);
          fn_801F4C14(0,0,0x56,0,uVar10);
        }
      }
      else {
        fn_8011BBD8(uVar4,0,0x2d,0,uVar7 - 1);
        fn_801F4C14(0,0,0x3b,0,0x46);
      }
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
    }
    else {
      fn_801F4C14(0,0,0x3b,0,0x40);
      if (bVar14 < bVar15) {
        lbl_80478D7D = 1;
      }
      else {
        lbl_80478D7D = 0;
      }
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
    return;
  }
  fn_801F4C14(0,0,0x3b,0,0x40);
  *(u32 *)(lbl_8047B610) = 0x803797a0;
  return;
}
/* Address: 0x8021AFAC | Size: 0x104 | Ghidra import */
void fn_8021AFAC(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F2654();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_801F6DF0();
    extern u8 fn_801F6E44();
    extern u8 lbl_80478D78[6];
  u32 uVar1;
  u32 uVar3;
  u32 uVar2;
  u8 cVar5;
  u16 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(2,uVar1);
  uVar3 = fn_801F2654(0,1,uVar1,1);
  cVar5 = fn_801F6E44(uVar2,0x49);
  if (cVar5 != 2) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    lbl_80478D78[5] = 0;
  }
  else {
    fn_801F6DF0(uVar2,0x49,0);
    uVar4 = fn_801F54A4(0,0,0x19,0);
    if ((uVar4 >= 2) && ((u16)uVar3 >= 2)) {
      lbl_80478D78[5] = 4;
    }
    else {
      lbl_80478D78[5] = 3;
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021B0B0 | Size: 0xf4 | Ghidra import */
void fn_8021B0B0(void)

{
    extern void fn_801294C4();
    extern u32 fn_8012A5B0();
    extern void fn_80132A38();
    extern int fn_801F2A7C();
    extern u8 fn_801F54A4();
    extern int fn_801FB1C0();
    extern void fn_80211B94();
    extern u32 lbl_8047B62C;
    extern u8 lbl_8037939C[];
  u8 cVar5;
  int iVar1;
  int iVar2;
  u32 uVar3;
  u32 uVar4;

  cVar5 = fn_801F54A4(0,0,0x25,0);
  if (cVar5 == 1) {
    iVar1 = fn_801F2A7C(0);
    if (iVar1 != 0) {
      iVar2 = fn_801FB1C0(iVar1,0,0x49,0);
      if (iVar2 != 0) {
        uVar3 = fn_801FB1C0(iVar1,0,0x48,0);
        iVar2 = iVar2 * (uVar3 & 0xff);
        uVar4 = fn_801FB1C0(iVar1,0,0x44,0);
        if (uVar4 != 0) {
          fn_801294C4(uVar4,iVar2);
          fn_80132A38(0x2f,iVar2);
          fn_80132A38(0x13,fn_8012A5B0(uVar4,1,0));
          fn_80211B94(lbl_8047B62C,(u32)lbl_8037939C,0);
        }
      }
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021B1A4 | Size: 0x1c0 | Ghidra import */
void fn_8021B1A4(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8010C4D4();
    extern u32 fn_8011BEB4();
    extern u8 fn_80123CD4();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
    extern u8 fn_80207AE0();
    extern void fn_80207B5C();
  u32 uVar1;
  u32 uVar2;
  u8 cVar5;
  u16 sVar4;
  u32 uVar3;
  u8 bVar6;

  u32 uVar7;
  u8 bVar8;
  u32 local_28 [5];
  
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  for (bVar6 = 0; bVar6 < 4; bVar6 = bVar6 + 1) {
    local_28[bVar6] = 0xffffffff;
  }
  bVar8 = 0;
  for (bVar6 = 0; bVar6 < 4; bVar6 = bVar6 + 1) {
    cVar5 = fn_80123CD4(uVar2,bVar6);
    if (cVar5 == 1) {
      sVar4 = (int)fn_8012640C(uVar2,0,0x7f,bVar6);
      if ((sVar4 != 0xa5) && (sVar4 != 0x164)) {
        uVar3 = fn_8011BEB4(0,sVar4,3,0);
        uVar3 = uVar3 & 0xffff;
        if (uVar3 == 9) {
          cVar5 = fn_80207AE0(uVar1,7);
          if (cVar5 == 1) {
            uVar3 = 7;
          }
          else {
            uVar3 = 0;
          }
        }
        cVar5 = fn_80207AE0(uVar1,uVar3);
        if (cVar5 == 0) {
          uVar7 = (u32)bVar8;
          bVar8 = bVar8 + 1;
          local_28[uVar7] = uVar3;
        }
      }
    }
  }
  if (bVar8 != 0) {
    uVar3 = fn_800E0C54();
    uVar3 = *(u32 *)((int)local_28 +
                     (((uVar3 & 0xffff) - ((uVar3 & 0xffff) / (u32)bVar8) * (u32)bVar8) * 4 &
                     0x3fc));
    if (-1 < (int)uVar3) {
      uVar3 = uVar3 & 0xffff;
      for (uVar7 = 0; (uVar7 & 0xff) < 2; uVar7 = uVar7 + 1) {
        fn_80207B5C(uVar1,uVar7,uVar3);
      }
      fn_8010C4D4(uVar3);
      uVar1 = fn_800FA280();
      fn_80132A38(0xd,uVar1);
      lbl_8047B610 = lbl_8047B610 + 5;
      return;
    }
  }
  lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  return;
}
/* Address: 0x8021B364 | Size: 0x120 | Ghidra import */
u32 fn_8021B364(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u8 fn_80123CD4();
    extern u32 fn_80205B8C();
    extern u8 fn_80207AE0();
  u32 uVar6;
  u32 uVar1;
  u8 cVar4;
  u16 sVar3;
  u32 uVar2;
  int bVar5;

  uVar1 = fn_80205B8C();
  for (bVar5 = 0; (u8)bVar5 < 4; bVar5 = bVar5 + 1) {
    *(u32 *)(r4 + (u32)(u8)bVar5 * 4) = 0xffffffff;
  }
  uVar6 = 0;
  for (bVar5 = 0; (u8)bVar5 < 4; bVar5 = bVar5 + 1) {
    cVar4 = fn_80123CD4(uVar1,(u8)bVar5);
    if (cVar4 == 1) {
      sVar3 = (int)fn_8012640C(uVar1,0,0x7f,(u8)bVar5);
      if ((sVar3 != 0xa5) && (sVar3 != 0x164)) {
        uVar2 = fn_8011BEB4(0,sVar3,3,0);
        uVar2 = uVar2 & 0xffff;
        if (uVar2 == 9) {
          cVar4 = fn_80207AE0(r3,7);
          if (cVar4 == 1) {
            uVar2 = 7;
          }
          else {
            uVar2 = 0;
          }
        }
        cVar4 = fn_80207AE0(r3,uVar2);
        if (cVar4 == 0) {
          *(u32 *)(r4 + (uVar6 & 0xff) * 4) = uVar2 & 0xffff;
          uVar6 = uVar6 + 1;
        }
      }
    }
  }
  return uVar6;
}
/* Address: 0x8021B484 | Size: 0x18c | Ghidra import */
void fn_8021B484(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F87CC();
    extern u32 fn_80203E0C();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u16 uVar8;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  int iVar7;
  short sVar9;
  u32 bVar10;

  int aiStack_28 [6];
  
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(0x12,0);
  uVar3 = fn_801F4354(0,uVar2);
  uVar8 = fn_801F87CC(uVar3,aiStack_28);
  if (uVar8 != 0) {
    uVar4 = fn_80203E0C(uVar1);
    uVar5 = fn_80203E0C(uVar2);
    if (((uVar4 & 0xff) < (uVar5 & 0xff)) &&
       (uVar6 = fn_800E0C54(),
       ((int)(((uVar4 & 0xff) + (uVar5 & 0xff)) * (uVar6 & 0xff)) >> 8) + 1U <= (uVar5 >> 2 & 0x3f))
       ) {
      bVar10 = 0;
      *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
    }
    else {
      bVar10 = 1;
      *(u32 *)(lbl_8047B610) = 0x80378cce;
    }
    if (bVar10 == 0) {
      return;
    }
    uVar4 = fn_800E0C54();
    iVar7 = *(int *)((int)aiStack_28 +
                    (((uVar4 & 0xffff) - ((uVar4 & 0xffff) / (u32)uVar8) * (u32)uVar8) * 4 &
                    0x3fffc));
    if (iVar7 != 0) {
      sVar9 = (int)fn_8012640C(iVar7,0,0xce,0);
      if (-1 < sVar9) {
        fn_801F4C14(0,0,0x45,0,uVar2);
        fn_801254B4(uVar2,0,0x121,0,(int)sVar9);
        fn_801254B4(uVar2,0,0x119,0,1);
        return;
      }
    }
  }
  *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  return;
}
/* 0x8021B610 | size: 0x18 */
void fn_8021B610(void) {
    u32 pc = lbl_8047B610;
    lbl_8047B626 = 0;
    lbl_8047B610 = pc + 1;
}

/* Address: 0x8021B628 | Size: 0xe4 | Ghidra import */
void fn_8021B628(void)

{
    extern u32 fn_800E0C54();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
  u32 uVar1;
  u8 uVar2;
  u8 uVar3;
  u32 r7;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  r7 = *(u8 *)(lbl_8047B610 + 1);
  if (r7 != 0) {
    fn_8011BBD8(uVar1,0,0x31,0,r7);
  }
  else {
    uVar2 = (int)(u16)fn_800E0C54() % 4;
    if (uVar2 < 2) {
      uVar3 = uVar2 + 2;
    }
    else {
      uVar3 = (int)(u16)fn_800E0C54() % 4 + 2;
    }
    fn_8011BBD8(uVar1,0,0x31,0,uVar3);
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021B70C | Size: 0x54 | Ghidra import */
void fn_8021B70C(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802025B8();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0xd);
  if (cVar2 == 2) {
    *((&lbl_80478D78)+3) = 0x75;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021B760 | Size: 0xd0 | Ghidra import */
void fn_8021B760(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern void fn_8020A2B8();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xf8,0);
  cVar4 = fn_802025B8(uVar1,0xc);
  if (cVar4 == 2) {
    fn_8020248C(uVar1,0xc,0);
    fn_8020248C(uVar1,0x22,0);
  }
  fn_8020A2B8(uVar3,uVar2);
  fn_801254B4(uVar1,0,0xf5,0,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021B830 | Size: 0x40 | Ghidra import */
void fn_8021B830(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8021B870();

  fn_801F37B0(0,fn_8021B870,0,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021B870 | Size: 0x48 | Ghidra import */
u32 fn_8021B870(u32 r3)

{
    extern u8 fn_802062FC();
    extern void fn_802073C0();
  u32 uVar1;
  u8 cVar1;

  uVar1 = r3;
  cVar1 = fn_802062FC();
  if (cVar1 == 0) {
    return 0;
  }
  fn_802073C0(uVar1);
  return 1;
}
/* Address: 0x8021B8B8 | Size: 0x58 | Ghidra import */
void fn_8021B8B8(void)

{
    extern u8 lbl_80379F58[];
    extern int fn_8021B910();
  u8 uVar1;
  u8 cVar1;

  uVar1 = lbl_80379F58[0x1601e];
  cVar1 = fn_8021B910(uVar1 & 0xf0,uVar1 & 0xf,
                       *(u8 *)(lbl_8047B610 + 1),
                       *(u32 *)(lbl_8047B610 + 2));
  if (cVar1 == 0) {
    lbl_8047B610 = lbl_8047B610 + 6;
  }
  return;
}
/* Address: 0x8021B910 | Size: 0x6ec | Ghidra import */


u32 fn_8021B910(u8 r3,u32 r4,u32 r5,u32 r6)

{
    extern u32 fn_800FA280();
    extern s8 fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern s8 fn_801F6E98();
    extern s8 fn_802026E4();
    extern s8 fn_802096E8();
    extern void fn_80211B94();
    extern void fn_8022DCB8();
    extern u32 lbl_80478D7D;
    extern u8 lbl_80478D7E;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B62C;
  u32 bVar1;
  u32 bVar2;
  u32 bVar3;
  u32 bVar4;
  u32 uVar5;
  u32 uVar6;
  short sVar11;
  short sVar12;
  s8 cVar13;
  u32 uVar7;
  s8 cVar14;
  int iVar8;
  s8 cVar15;
  u32 uVar9;
  int iVar10;

  s8 cVar16;
  
  bVar2 = (r5 & 0x40) == 0;
  if (bVar2) {
    uVar5 = fn_801F025C(0x12,0);
  }
  else {
    uVar5 = fn_801F025C(0x11,0);
  }
  uVar6 = fn_801F025C(2,uVar5);
  sVar11 = fn_80207BF4(uVar5);
  sVar12 = fn_80205184(uVar5);
  bVar3 = (r5 & 0x80) == 0;
  if ((r4 & 0xff) < 8) {

    uVar5 = ((int (*)(void))**(void ***)((r4 & 0xff) * 4 + -0x7fc65fb8))();
    return uVar5;
  }
  cVar13 = (int)fn_8012640C(uVar5,0,0,0);
  uVar7 = fn_800FA280(*(u32 *)((r4 & 0xff) * 4 + -0x7fd86184));
  fn_80132A38(0xd,uVar7);
  if (r3 == 0x90) {
    cVar16 = -1;
    goto LAB_00218aa8;
  }
  if (r3 < 0x90) {
    if (r3 == 0x20) {
      cVar16 = 2;
      goto LAB_00218aa8;
    }
    if ((r3 < 0x20) && (r3 == 0x10)) {
      cVar16 = 1;
      goto LAB_00218aa8;
    }
  }
  else if (r3 == 0xa0) {
    cVar16 = -2;
    goto LAB_00218aa8;
  }
  cVar16 = 0;
LAB_00218aa8:
  if (cVar16 < 0) {
    cVar14 = fn_801F6E98(uVar6,0x4c);
    if (((cVar14 == 1) && (bVar3)) && (sVar12 != 0xae)) {
      if ((r5 & 0x1f) == 1) {
        iVar8 = (int)fn_8012640C(uVar5,0,0x113,0);
        if (iVar8 == 0) {
          fn_801F4C14(0,0,0x4b,0,uVar5);
          fn_801254B4(uVar5,0,0x113,0,1);
          fn_80211B94(lbl_8047B62C,0x80378ceb,0);
          *(u32 *)(lbl_8047B610) = r6;
        }
        else {
          *(u32 *)(lbl_8047B610) = r6;
        }
      }
      return 1;
    }
    if ((sVar12 != 0xae) && ((r5 & 0x20) == 0)) {
      fn_801F025C(0x11,0);
      uVar6 = fn_80205184();
      uVar7 = fn_801F025C(0x12,0);
      bVar1 = 0;
      cVar14 = fn_8011BEB4(0,uVar6,0xe,0);
      cVar15 = fn_802026E4(uVar7,0x2b);
      if ((cVar15 == 1) && (cVar14 == 1)) {
        bVar4 = 1;
      }
      else {
        bVar4 = 0;
      }
      if (bVar4) {
        fn_801F4C14(0,0,0x3b,0,0x40);
        uVar6 = fn_801F025C(0x11,0);
        uVar7 = (int)fn_8012640C(uVar6,0,0xd9,0);
        uVar9 = fn_801F025C(0x12,0);
        cVar14 = fn_802096E8(uVar7);
        if (cVar14 == 0) {
          fn_801254B4(uVar9,0,0xf3,0,0);
          fn_801254B4(uVar9,0,0xf4,0,9);
          *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
        }
        else {
          uVar7 = fn_801F025C(0x11,0);
          iVar8 = fn_801F025C(2,uVar7);
          uVar7 = fn_801F025C(0x12,0);
          iVar10 = fn_801F025C(2,uVar7);
          cVar14 = fn_802026E4(uVar7,0x15);
          if (((cVar14 == 1) && (iVar8 != iVar10)) &&
             ((lbl_8047B618 & 0x1000000) == 0)) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
          fn_8022DCB8(uVar6,uVar9,0);
        }
        bVar1 = 1;
        lbl_80478D7E = 1;
      }
      if (bVar1) {
        *(u32 *)(lbl_8047B610) = 0x80377b05;
        return 1;
      }
    }
    if ((((sVar11 == 0x1d) || (sVar11 == 0x49)) && (bVar3)) && (sVar12 != 0xae)) {
      if ((r5 & 0x1f) == 1) {
        iVar8 = (int)fn_8012640C(uVar5,0,0x113,0);
        if (iVar8 == 0) {
          fn_801F4C14(0,0,0x4b,0,uVar5);
          fn_801254B4(uVar5,0,0x113,0,1);
          fn_80211B94(lbl_8047B62C,0x803797f1,0);
          *(u32 *)(lbl_8047B610) = r6;
        }
        else {
          *(u32 *)(lbl_8047B610) = r6;
        }
      }
      return 1;
    }
    if ((sVar11 == 0x13) && ((r5 & 0x1f) == 0)) {
      return 1;
    }
    if (cVar16 < -1) {
      uVar6 = fn_800FA280(0x7628);
      fn_80132A38(0xe,uVar6);
    }
    else {
      uVar6 = fn_800FA280(0x76bd);
      fn_80132A38(0xe,uVar6);
    }
    uVar6 = fn_800FA280(0x7629);
    fn_80132A38(0x41,uVar6);
    if (cVar13 < 1) {
      lbl_80478D7D = 2;
    }
    else {
      lbl_80478D7D = bVar2;
    }
  }
  else {
    if (cVar16 < 2) {
      uVar6 = fn_800FA280(0x76bd);
      fn_80132A38(0xe,uVar6);
    }
    else {
      uVar6 = fn_800FA280(0x7626);
      fn_80132A38(0xe,uVar6);
    }
    uVar6 = fn_800FA280(0x7627);
    fn_80132A38(0x41,uVar6);
    if (cVar13 < '\f') {
      lbl_80478D7D = bVar2;
    }
    else {
      lbl_80478D7D = 2;
    }
  }
  cVar13 = cVar13 + cVar16;
  if (cVar13 < 0) {
    cVar13 = 0;
  }
  if ('\f' < cVar13) {
    cVar13 = '\f';
  }
  fn_801254B4(uVar5,0,0,0,(int)cVar13);
  if ((lbl_80478D7D == 2) && ((r5 & 1) != 0)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
  }
  if ((lbl_80478D7D == 2) && ((r5 & 1) == 0)) {
    uVar5 = 1;
  }
  else {
    uVar5 = 0;
  }
  return uVar5;
}
/* Address: 0x8021C034 | Size: 0x5c | Ghidra import */
u32 fn_8021C034(u32 r3)

{
  switch ((u8)r3) {
  case 0x10:
    return 1;
  case 0x20:
    return 2;
  case 0x90:
    return 0xffffffff;
  case 0xa0:
    return 0xfffffffe;
  }
  return 0;
}
/* Address: 0x8021C090 | Size: 0x2c | Ghidra import */
u32 fn_8021C090(void)

{
    u32 r3;

  u32 uVar1;
  
  if ((r3 & 0xff) < 8) {

    uVar1 = ((int (*)(void))**(void ***)((r3 & 0xff) * 4 + -0x7fc65f98))();
    return uVar1;
  }
  return 0;
}
/* Address: 0x8021C0F4 | Size: 0x9c | Ghidra import */
void fn_8021C0F4(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
  u32 uVar1;
  int uVar2;
  int iVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar1 = (int)fn_8012640C(uVar1,0,0xd9,0);
  fn_8011BEB4(uVar1,0,0x2d,0);
  uVar2 = fn_8011BEB4(uVar1,0,0x2e,0);
  iVar3 = -(uVar2 / 2);
  if (iVar3 == 0) {
    iVar3 = -1;
  }
  fn_8011BBD8(uVar1,0,0x2d,0,iVar3);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021C190 | Size: 0x178 | Ghidra import */
void fn_8021C190(void)

{
    extern u32 fn_80119DD0();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_80201704();
    extern short fn_80202360();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u32 fn_80203B5C();
    extern u8 lbl_80478D78;
    extern u8 lbl_80379F58[];
  short sVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar7;
  short sVar5;
  u16 uVar6;
  u32 uVar4;

  u32 uVar8;

  uVar8 = *(u32 *)(lbl_8047B610 + 1);
  uVar2 = fn_801F025C(0x11,0);
  uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
  cVar7 = fn_802026E4(uVar2,0x2d);
  if (cVar7 == 0) {
    sVar5 = 0;
  }
  else {
    sVar5 = fn_80202360(uVar2,0x2d);
  }
  if (sVar5 <= 0) {
    (&lbl_80478D78)[5] = 0;
    lbl_8047B610 = uVar8;
  }
  else {
    cVar7 = fn_80201704(uVar2);
    if (cVar7 == 1) {
      fn_80202810(uVar2,0x2d);
      fn_801F4C14(0,0,0x43,0,uVar2);
      (&lbl_80478D78)[5] = 1;
      lbl_8047B610 = uVar8;
    }
    else {
      uVar6 = fn_80119DD0(0x2d);
      sVar1 = (uVar6 & 0xff) - sVar5;
      if (sVar1 < 0) {
        sVar1 = 0;
      }
      fn_8011BBD8(uVar3,0,0x2d,0,-(fn_80203B5C(uVar2,1 << (int)sVar1 & 0xffff) & 0xffff));
      *(u8 *)(lbl_80379F58 + 0x16002) = sVar5;
      fn_80202810(uVar2,0x2d);
      fn_801F4C14(0,0,0x43,0,uVar2);
      lbl_8047B610 = lbl_8047B610 + 5;
    }
  }
  return;
}
/* Address: 0x8021C308 | Size: 0x188 | Ghidra import */
void fn_8021C308(void)

{
    extern u8 lbl_80379F58[];
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern short fn_80202360();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern int fn_80232110();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u32 uVar2;
  u8 cVar10;
  short sVar9;
  u32 uVar4;
  u32 uVar11;
  u32 uVar3;
  u16 uVar7;
  u16 uVar8;
  u32 uVar5;
  int iVar6;

  uVar11 = *(u32 *)(lbl_8047B610 + 1);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_80205184(uVar1);
  uVar7 = fn_8011BEB4(uVar2,0,0x2f,0);
  uVar8 = fn_8011BEB4(uVar2,0,0x30,0);
  cVar10 = fn_802026E4(uVar1,0x2d);
  if (cVar10 == 0) {
    sVar9 = 0;
  }
  else {
    sVar9 = fn_80202360(uVar1,0x2d);
  }
  uVar4 = fn_801F025C(0x12,0);
  uVar5 = fn_801F025C(2,uVar4);
  if (sVar9 <= 0) {
    lbl_8047B610 = uVar11;
  }
  else {
    if (*((&lbl_80478D78)+6) != 1) {
      iVar6 = fn_80232110(uVar1,uVar4,uVar5,uVar3,uVar7,uVar8);
      iVar6 = sVar9 * iVar6;
      lbl_80379F58[0x16002] = (u8)sVar9;
      cVar10 = fn_802026E4(uVar1,0x32);
      if (cVar10 == 1) {
        iVar6 = (iVar6 * 0xf) / 10;
      }
      fn_8011BBD8(uVar2,0,0x2d,0,iVar6);
    }
    fn_80202810(uVar1,0x2d);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x8021C490 | Size: 0xf8 | Ghidra import */
void fn_8021C490(void)

{
    extern u32 fn_80119DD0();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_80202360();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u8 cVar5;
  int uVar2;
  u8 uVar3;

  uVar1 = fn_801F025C(0x11,0);
  cVar5 = fn_802026E4(uVar1,0x2d);
  if (cVar5 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = fn_80202360(uVar1,0x2d);
  }
  uVar3 = fn_80119DD0(0x2d);
  if ((short)uVar2 == uVar3) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    (&lbl_80478D78)[5] = 1;
  }
  else {
    cVar5 = fn_802025B8(uVar1,0x2d);
    if (cVar5 == 2) {
      fn_8020248C(uVar1,0x2d,0);
    }
    uVar2 = fn_80202360(uVar1,0x2d);
    fn_80132A38(0x2f,(int)(short)uVar2);
    (&lbl_80478D78)[5] = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021C588 | Size: 0xb0 | Ghidra import */
void fn_8021C588(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F37B0();
    extern u8 fn_8021C638();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u32 sVar4;
  u8 uVar2;
  u32 uVar5;
  u8 bVar7;
  int iVar3;

  uVar1 = fn_801F025C(0x12,0);
  sVar4 = fn_80207BF4();
  uVar5 = *(u32 *)(lbl_8047B610 + 1);
  uVar2 = fn_801F37B0(0,fn_8021C638,uVar1,1);
  iVar3 = 1 - uVar2;
  bVar7 = (u8)(iVar3 != 0);
  if (bVar7 & 0xff) {
    lbl_8047B610 = uVar5;
  }
  else {
    if (((u16)sVar4 == 0xf) || ((u16)sVar4 == 0x48)) {
      (&lbl_80478D78)[5] = 2;
      lbl_8047B610 = uVar5;
    }
    else {
      lbl_8047B610 = lbl_8047B610 + 5;
    }
  }
  return;
}
/* Address: 0x8021C638 | Size: 0xbc | Ghidra import */
u32 fn_8021C638(void)

{
    int r3;
    u32 r4;
    int r5;

    extern void fn_801F4C14();
    extern s8 fn_802026E4();
    extern s8 fn_802062FC();
    extern u8 lbl_80478D7D;
  short sVar2;
  s8 cVar3;
  u32 uVar1;

  sVar2 = fn_80207BF4(r5);
  cVar3 = fn_802062FC(r3);
  if (cVar3 == 0) {
    uVar1 = 1;
  }
  else {
    cVar3 = fn_802026E4(r3,0xb);
    if ((cVar3 == 1) && (sVar2 != 0x2b)) {
      fn_801F4C14(0,0,0x4b,0,r3);
      if (r3 == r5) {
        lbl_80478D7D = 0;
      }
      else {
        lbl_80478D7D = 1;
      }
      uVar1 = 0;
    }
    else {
      uVar1 = 1;
    }
  }
  return uVar1;
}
/* 0x8021C6F4 | size: 0x10 | tiny */
void fn_8021C6F4(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x8021C704 | Size: 0x58 | Ghidra import */
void fn_8021C704(void)

{
    extern u32 fn_801F025C();
  u32 uVar2;
  u16 sVar3;

  uVar2 = fn_801F025C(0x11,0);
  sVar3 = (int)fn_8012640C(uVar2,0,0xed,0);
  if (sVar3 != 0) {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x8021C75C | Size: 0x1a4 | Ghidra import */
void fn_8021C75C(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern s8 fn_80201704();
    extern void fn_80201EB0();
    extern void fn_8020248C();
    extern s8 fn_802026E4();
    extern u32 fn_80203B5C();
    extern u8 lbl_80478D78;
  u32 uVar1;
  u32 uVar2;
  u8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  fn_801F025C(0x12,0);
  fn_801F4C14(0,0,0x43,0,uVar1);
  fn_8011BBD8(uVar2,0,0x2d,0,-(fn_80203B5C(uVar1,1) & 0xffff));
  cVar4 = fn_80201704(uVar1);
  if (cVar4 == 1) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    cVar4 = fn_802026E4(uVar1,3);
    if (cVar4 == 1) goto _true;
    cVar4 = fn_802026E4(uVar1,4);
    if (cVar4 == 1) goto _true;
    cVar4 = fn_802026E4(uVar1,5);
    if (cVar4 == 1) goto _true;
    cVar4 = fn_802026E4(uVar1,6);
    if (cVar4 == 1) goto _true;
    cVar4 = fn_802026E4(uVar1,7);
    if (cVar4 == 1) {
_true:
      (&lbl_80478D78)[5] = 1;
    }
    else {
      (&lbl_80478D78)[5] = 0;
    }
    fn_8020248C(uVar1,8,0);
    fn_80201EB0(uVar1,8,3);
    cVar4 = fn_801FECD4(uVar1);
    if (cVar4 == 1) {
      fn_801FE7EC(uVar1,0x7c,0,0);
    }
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  return;
}
/* Address: 0x8021C900 | Size: 0x100 | Ghidra import */

void fn_8021C900(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
  int bVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;

  uVar2 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar3 = fn_8011BEB4(uVar2,0,0x2d,0);
  fn_801F025C(0x12,0);
  uVar4 = fn_80205B8C();
  uVar5 = (int)fn_8012640C(uVar4,0,0x87,0);
  bVar1 = *(u8 *)(lbl_8047B610 + 1);
  switch (bVar1) {
  case 0:
    uVar3 = uVar3 * -1;
    break;
  case 1:
    uVar3 = (int)uVar3 / 2;
    if (uVar3 == 0) {
      uVar3 = 1;
    }
    uVar5 = (int)uVar5 / 2;
    if ((int)uVar5 < (int)uVar3) {
      uVar3 = uVar5;
    }
    break;
  case 2:
    uVar3 = uVar3 << 1;
    break;
  }
  fn_8011BBD8(uVar2,0,0x2d,0,uVar3);
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021CA00 | Size: 0x158 | Ghidra import */
void fn_8021CA00(void)

{
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern s8 fn_80207AE0();
    extern s8 fn_802096E8();
    extern u8 lbl_80478D7D;
  u32 uVar1;
  u32 uVar2;
  u16 uVar4;
  u32 uVar3;
  s8 cVar5;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F0134(uVar1,uVar4);
  uVar3 = fn_801F025C(0x12,0);
  cVar5 = fn_802096E8(uVar2);
  if ((cVar5 == 0) || (cVar5 = fn_802025B8(uVar3,0x1c), cVar5 != 2)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    lbl_80478D7D = 1;
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  }
  else {
    cVar5 = fn_80207AE0(uVar3,0xc);
    if (cVar5 == 1) {
      fn_801F4C14(0,0,0x3b,0,0x40);
      lbl_80478D7D = 2;
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    }
    else {
      fn_8020248C(uVar3,0x1c,uVar1);
      lbl_80478D7D = 0;
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
    }
  }
  return;
}
/* Address: 0x8021CB58 | Size: 0x104 | Ghidra import */
void fn_8021CB58(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F2654();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_801F6DF0();
    extern u8 fn_801F6E44();
    extern u8 lbl_80478D78[6];
  u32 uVar1;
  u32 uVar3;
  u32 uVar2;
  u8 cVar5;
  u16 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_801F025C(2,uVar1);
  uVar3 = fn_801F2654(0,1,uVar1,1);
  cVar5 = fn_801F6E44(uVar2,0x48);
  if (cVar5 != 2) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    lbl_80478D78[5] = 0;
  }
  else {
    fn_801F6DF0(uVar2,0x48,0);
    uVar4 = fn_801F54A4(0,0,0x19,0);
    if ((uVar4 >= 2) && ((u16)uVar3 >= 2)) {
      lbl_80478D78[5] = 2;
    }
    else {
      lbl_80478D78[5] = 1;
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021CC5C | Size: 0x84 | Ghidra import */
void fn_8021CC5C(void)
{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F4C14();
    extern u8 lbl_80478D78;
  u8 cVar1;

  cVar1 = fn_801F2988(0,0x54);
  if (cVar1 == 2) goto eq2;
  fn_801F4C14(0,0,0x3b,0,0x40);
  *((&lbl_80478D78)+5) = 2;
  goto done;
eq2:
  fn_801F2934(0,0x54,0);
  *((&lbl_80478D78)+5) = 0;
done:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021CCE0 | Size: 0x180 | Ghidra import */
void fn_8021CCE0(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_80201248();
    extern void fn_80209C1C();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern u32 lbl_80379BFF[];
  u32 uVar5;
  u16 uVar4;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  int bVar6;

  u16 auStack_18 [4];

  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar5 = (u32)fn_8012640C(uVar1,0,0xf7,0) & 0xffff;
  if (uVar5 == 0) goto LAB_else;
  if (uVar5 == 0x165) goto LAB_else;
  if (uVar5 == 0xffff) goto LAB_else;
LAB_main:
  lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
  fn_80209C1C(uVar2,uVar5);
  uVar3 = fn_8022B2CC(uVar1,uVar5,uVar4,0,1,1, (void*)0xffffffff);
  fn_801F4C14(0,0,0x43,0,uVar3);
  uVar3 = fn_8011BEB4(0,uVar5,9,0);
  lbl_8047B610 = lbl_80379BFF[(u16)uVar3];
  return;
LAB_else:
  bVar6 = fn_80201248(uVar1,auStack_18);
  if ((u8)bVar6 != 0) {
    uVar3 = (u16)fn_800E0C54();
    uVar5 = auStack_18[(u8)((int)uVar3 % (int)(u8)bVar6)];
    if (uVar5 == 0) goto LAB_217848;
    if (uVar5 != 0x165) goto LAB_main;
  }
LAB_217848:
  fn_801254B4(uVar1,0,0x118,0,1);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021CE60 | Size: 0xdc | Ghidra import */
void fn_8021CE60(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u8 fn_80201704();
    extern u32 fn_80203B5C();
  u32 uVar1;
  u32 uVar3;
  u32 uVar2;
  u32 uVar4;
  u8 cVar5;

  int iVar6;
  int iVar7;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = fn_801F025C(0x12,0);
  iVar6 = *(int *)(lbl_8047B610 + 1);
  if (*(char *)(lbl_8047B610 + 5) == 17) {
    uVar3 = uVar1;
    fn_801F4C14(0,0,0x43,0,uVar1);
  }
  fn_8011BBD8(uVar2,0,0x2d,0,-(fn_80203B5C(uVar3,2) & 0xffff));
  cVar5 = fn_80201704(uVar3);
  if (cVar5 == 1) {
    iVar7 = iVar6;
  } else {
    iVar7 = *(volatile u32 *)&lbl_8047B610 + 6;
  }
  lbl_8047B610 = iVar7;
  return;
}
/* Address: 0x8021CF3C | Size: 0xd4 | Ghidra import */
void fn_8021CF3C(void)

{
    extern int fn_801F025C();
    extern void fn_801F4C14();
    extern int fn_801F54A4();
    extern s8 fn_802062FC();
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  int iVar1;
  int iVar2;
  s8 cVar3;

  u32 uVar4;
  
  iVar1 = fn_801F025C(0x11,0);
  uVar4 = *(u32 *)(*(int *)(lbl_8047B610) + 1);
  lbl_8047B648 = (char)lbl_8047B648 + 1;
  while ((iVar2 = 0, lbl_8047B648 < lbl_8047B649 &&
         (((iVar2 = fn_801F54A4(0,0,0x5d), iVar2 == 0 || (cVar3 = fn_802062FC(), cVar3 == 0))
          || (iVar1 == iVar2))))) {
    lbl_8047B648 = (char)lbl_8047B648 + 1;
  }
  if (iVar2 == 0) {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
  }
  else {
    fn_801F4C14(0,0,0x43,0,iVar2);
    *(u32 *)(lbl_8047B610) = uVar4;
  }
  return;
}
/* Address: 0x8021D010 | Size: 0x80 | Ghidra import */
void fn_8021D010(void)

{
    extern u32 fn_801F025C();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u8 cVar3;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  fn_801254B4(uVar2,0,0x83,0,0);
  cVar3 = fn_801FECD4(uVar1);
  if (cVar3 == 1) {
    fn_801FE7EC(uVar1,0x83,0,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021D090 | Size: 0x194 | Ghidra import */
void fn_8021D090(void)

{
    extern void fn_80011E68();
    extern void fn_8011BBD8();
    extern int fn_801F025C();
    extern int fn_801F349C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_80205B8C();
    extern s8 fn_802062FC();
    extern u32 fn_802656AC();
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  u16 uVar6;
  int iVar1;
  u32 uVar2;
  u32 uVar3;
  int iVar4;
  u32 uVar5;
  s8 cVar7;

  uVar6 = fn_801F54A4(0,0,0x14,0);
  iVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(iVar1,0,0xd9,0);
  uVar3 = fn_802656AC(iVar1,uVar6,1);
  iVar4 = fn_801F349C(0,6,0,0,0);
  if (iVar4 == 0) {
    uVar5 = fn_80205B8C(iVar1);
    uVar6 = (int)fn_8012640C(uVar5,0,0x83,0);
    fn_8011BBD8(uVar2,0,0x2d,0,uVar6);
    fn_80011E68(uVar3,0);
    lbl_8047B649 = 8;
    lbl_8047B648 = 0;
    while ((iVar4 = 0, lbl_8047B648 < lbl_8047B649 &&
           (((iVar4 = fn_801F54A4(0,0,0x5d), iVar4 == 0 || (cVar7 = fn_802062FC(), cVar7 == 0))
            || (iVar1 == iVar4))))) {
      lbl_8047B648 = (char)lbl_8047B648 + 1;
    }
    if (iVar4 != 0) {
      fn_801F4C14(0,0,0x43,0,iVar4);
    }
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 1;
  }
  else {
    fn_801F4C14(0,0,0x42,0,iVar4);
    *(u32 *)(lbl_8047B610) = 0x803797bb;
  }
  return;
}
/* Address: 0x8021D224 | Size: 0x1e8 | Ghidra import */
void fn_8021D224(void)

{
    extern u32 fn_800E0C54();
    extern u16 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u8 fn_801F221C();
    extern void fn_801F4C14();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 lbl_80478D78;
    extern u16 lbl_8047E5F8;

  u8 flag;
  u32 uVar1;
  u32 uVar2;
  u16 sVar5;
  u32 uVar3;
  u16 sVar4;
  u16 uVar6;

  flag = 1;
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184();
  sVar4 = (u16)fn_8011BEB4(0,uVar2,9,0);
  sVar5 = (int)fn_8012640C(uVar1,0,0xf1,0);
  uVar3 = (u32)fn_8012640C(uVar1,0,0xfc,0) & 0xff;
  if (((sVar5 != 0xb6) && (sVar5 != 0xc5)) && (sVar5 != 0xcb)) {
    uVar3 = 0;
    fn_801254B4(uVar1,0,0xfc,0,0);
  }
  if ((u8)fn_801F221C(0) == 1) {
    flag = 0;
  }
  uVar6 = fn_800E0C54();
  if (((&lbl_8047E5F8)[uVar3 & 0xff] > uVar6) && (flag != 0)) {
    if (sVar4 == 0x6f) {
      if ((u8)fn_802025B8(uVar1,0x2b) == 2) {
        fn_8020248C(uVar1,0x2b,0);
      }
      (&lbl_80478D78)[5] = 0;
    }
    if (sVar4 == 0x74) {
      if ((u8)fn_802025B8(uVar1,0x2c) == 2) {
        fn_8020248C(uVar1,0x2c,0);
      }
      (&lbl_80478D78)[5] = 1;
    }
    uVar3 = uVar3 + 1;
    if (3 < (uVar3 & 0xff)) {
      uVar3 = 3;
    }
    fn_801254B4(uVar1,0,0xfc,0,uVar3 & 0xff);
  }
  else {
    fn_801254B4(uVar1,0,0xfc,0,0);
    (&lbl_80478D78)[5] = 2;
    fn_801F4C14(0,0,0x3b,0,0x40);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* 0x8021D40C | size: 0x27C | large */
#pragma optimization_level 0
void fn_8021D40C(void) {
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B64C;
    extern void fn_80123B5C();
    extern void fn_801F00D0();
    extern void fn_801F025C();
    extern void fn_801F4460();
    extern void fn_801F4C14();
    extern void fn_801F54A4();
    extern void fn_801F6C4C();
    extern void fn_801F6E98();
    extern void fn_801F8E34();
    extern void fn_80200B10();
    extern void fn_80201C58();
    extern void fn_802026E4();
    extern void fn_80202810();
    extern void fn_80205B8C();
    extern void fn_802062FC();
    extern void fn_8022B2CC();
    extern void fn_80265598();
    extern u8 jumptable_8039A088[];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 var_r28 = 0;
    u32 var_r29 = 0;
    u32 var_r30 = 0;
    u32 var_r31 = 0;
    void (*ctr_fn)(void) = 0;

    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fn_801F54A4();
    r5 = *(u32*)&lbl_8047B610;
    var_r28 = r3 & 0xFFFF;
    r4 = 0x0;
    r3 = *(u8*)((u8*)r5 + 0x1);
    fn_801F025C();
    var_r31 = r3;
    ((void(*)(void))fn_80205184)();
    r5 = *(u32*)&lbl_8047B610;
    r4 = r3;
    tmp = *(u8*)((u8*)r5 + 0x2);
do {
    if (tmp > 7) break;
    r3 = (u32)jumptable_8039A088;
    tmp = tmp << 2;
    r3 = (u32)jumptable_8039A088;
    tmp = *(u32*)(r3 + tmp);
    ctr_fn = (void(*)(void))tmp;
    r3 = var_r31;
    fn_80200B10();
    break;
    r3 = 0x11;
    r4 = 0x0;
    fn_801F025C();
    r3 = 0x12;
    r4 = 0x0;
    fn_801F025C();
    tmp = r3;
    r3 = 0x0;
    var_r28 = tmp;
    r4 = 0x0;
    r7 = var_r28;
    r5 = 0x36;
    r6 = 0x0;
    fn_801F4C14();
    r4 = var_r28;
    r3 = 0x3;
    fn_801F025C();
    var_r29 = r3;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fn_801F54A4();
    var_r30 = r3 & 0xFFFF;
    r3 = var_r29;
    var_r28 = 0x0;
    r4 = 0x4d;
    fn_801F6E98();
    tmp = r3 & 0xFF;
    if (tmp == 1) {
        r3 = var_r29;
        r4 = 0x4d;
        fn_801F6C4C();
        tmp = r3 & 0xFFFF;
        if (tmp != 1) {
            r4 = var_r30;
            fn_801F00D0();
            /* mr. var_r30, r3 */;
            if (tmp != 1) {
                fn_802062FC();
                tmp = r3 & 0xFF;
                if (tmp == 1) {
                    var_r28 = var_r30;
    }
    }
    }
    }
    if (var_r28 == 0) {
        var_r28 = var_r31;
    }
    r7 = var_r28;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x43;
    r6 = 0x0;
    fn_801F4C14();
    break;
    r3 = var_r31;
    r5 = var_r28;
    r6 = 0x0;
    r7 = 0x1;
    r8 = 0x1;
    r9 = -0x1;
    fn_8022B2CC();
    if (r3 == 0) break;
    r7 = r3;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x43;
    r6 = 0x0;
    fn_801F4C14();
    break;
    r3 = var_r31;
    r4 = 0x0;
    r5 = 0x120;
    r6 = 0x0;
    ((void(*)(void))fn_8012640C)();
    tmp = r3 & 0xFF;
    if (tmp == 1) {
        tmp = 0x1;
        *(u8*)lbl_80478D78 = tmp;
        break;
    }
    tmp = 0x0;
    *(u8*)lbl_80478D78 = tmp;
    break;
    var_r28 = lbl_8047B64C;
    if (var_r28 == 0) break;
    r4 = var_r28;
    r3 = 0x0;
    fn_801F4460();
    if (r3 == 0) break;
    r4 = var_r28;
    fn_801F8E34();
    /* mr. var_r29, r3 */;
    if (r3 == 0) break;
    r4 = 0x36;
    fn_802026E4();
    tmp = r3 & 0xFF;
    if (tmp != 1) break;
    r3 = var_r29;
    r4 = 0x36;
    fn_80201C58();
    tmp = r3;
    r3 = var_r29;
    var_r28 = tmp;
    fn_80205B8C();
    r4 = var_r28;
    fn_80123B5C();
    tmp = (s8)r3;
    if (tmp >= 1) break;
    r3 = var_r29;
    r4 = 0x36;
    fn_80202810();
    break;
    r3 = var_r31;
    r4 = var_r28;
    r5 = 0x1;
    fn_80265598();
} while (0);
    r3 = *(u32*)&lbl_8047B610;
    tmp = r3 + 0x3;
    *(u32*)&lbl_8047B610 = tmp;
    return;
}
#pragma optimization_level 4

/* Address: 0x8021D688 | Size: 0x338 | Ghidra import */
void fn_8021D688(void)

{
    extern void fn_8011BBD8();
    extern u8 fn_801437E0();
    extern u8 fn_80143878();
    extern u8 fn_801438A0();
    extern u8 fn_801438C8();
    extern u8 fn_801438F0();
    extern u8 fn_80143918();
    extern u8 fn_80143940();
    extern u8 fn_80143990();
    extern u8 fn_801439B8();
    extern u8 fn_801439D4();
    extern u8 fn_801439F0();
    extern u8 fn_80143A0C();
    extern u8 fn_80143A28();
    extern u8 fn_80143A44();
    extern int fn_80143A94();
    extern void fn_80143DFC();
    extern void fn_801440A0();
    extern short fn_80144574();
    extern void fn_801DA36C();
    extern u32 fn_801F025C();
    extern u32 fn_80205BE8();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar7;
  u32 uVar5;
  u16 sVar8;
  int iVar6;
  u8 cVar9;
  u32 uVar10;

  int iVar11;
  int local_128;
  short local_124 [130];
  
  iVar11 = 0;
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xd6,0);
  uVar4 = (int)fn_8012640C(uVar1,0,0xd5,0);
  uVar3 = fn_80205BE8(uVar3);
  uVar7 = (int)fn_8012640C(uVar3,0,0x83,0);
  uVar5 = (int)fn_8012640C(uVar1,0,0xe5,0);
  sVar8 = itemGetStatus(uVar5,0,0x1e,0);
  fn_801440A0(sVar8);
  fn_80143DFC();
  iVar6 = fn_80143A94();
  if (iVar6 == 0) {
    iVar6 = 7;
  }
  else if (sVar8 == 0x13) {
    iVar6 = 1;
  }
  else {
    cVar9 = fn_801437E0();
    if (cVar9 == 0) {
      cVar9 = fn_80143940(iVar6);
      if (((((cVar9 == 1) || (cVar9 = fn_80143918(iVar6), cVar9 == 1)) ||
           (cVar9 = fn_801438F0(iVar6), cVar9 == 1)) ||
          ((cVar9 = fn_801438C8(iVar6), cVar9 == 1 ||
           (cVar9 = fn_801438A0(iVar6), cVar9 == 1)))) ||
         (cVar9 = fn_80143878(iVar6), cVar9 == 1)) {
        iVar6 = 3;
      }
      else {
        cVar9 = fn_80143A44(iVar6);
        if (cVar9 == 1) {
          iVar6 = 4;
        }
        else {
          cVar9 = fn_80143A28(iVar6);
          if (cVar9 == 0) {
            cVar9 = fn_80143A0C(iVar6);
            if (cVar9 == 0) {
              cVar9 = fn_801439F0(iVar6);
              if (cVar9 == 0) {
                cVar9 = fn_801439D4(iVar6);
                if ((cVar9 == 0) && (cVar9 = fn_801439B8(iVar6), cVar9 == 0)) {
                  cVar9 = fn_80143990(iVar6);
                  if (cVar9 == 1) {
                    iVar6 = 6;
                  }
                  else {
                    iVar6 = 7;
                  }
                  goto LAB_0021a89c;
                }
              }
            }
          }
          iVar6 = 5;
        }
      }
    }
    else {
      iVar6 = 2;
    }
  }
LAB_0021a89c:
  uVar1 = (int)fn_8012640C(uVar1,0,0xee,0);
  sVar8 = fn_80144574(&local_128,0,uVar4,sVar8,0);
  if ((iVar6 == 1) || (iVar6 == 2)) {
    for (uVar10 = 0; (int)(uVar10 & 0xffff) < (int)sVar8; uVar10 = uVar10 + 1) {
      if ((&local_128)[(uVar10 & 0xffff) * 2] == 0x15) {
        iVar11 = (int)local_124[(uVar10 & 0xffff) * 4];
      }
    }
    fn_801254B4(uVar3,0,0x83,0,uVar7);
    fn_8011BBD8(uVar2,0,0x2d,0,-iVar11);
  }
  for (uVar10 = 0; (int)(uVar10 & 0xffff) < (int)sVar8; uVar10 = uVar10 + 1) {
    if ((&local_128)[(uVar10 & 0xffff) * 2] == 0xf) {
      fn_801DA36C(uVar1,2);
    }
    if ((&local_128)[(uVar10 & 0xffff) * 2] == 9) {
      fn_801DA36C(uVar1,1);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021D9C0 | Size: 0x1b8 | Ghidra import */
void fn_8021D9C0(void)

{
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern u32 fn_801F18DC();
    extern int fn_801F2350();
    extern u32 fn_801F4354();
    extern int fn_801F8000();
    extern u32 fn_801F8100();
    extern u32 fn_802037DC();
    extern u32 fn_80204A10();
    extern u8 lbl_80478D78;
  u32 uVar2;
  u32 uVar3;
  u32 uVar1;
  u32 uVar4;
  int iVar5;
  u32 uVar6;

  uVar2 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar3 = fn_801F4354(0,uVar2);
  uVar4 = fn_80204A10(uVar2);
  uVar1 = (u32)__cntlzw(1 - (uVar4 & 0xff)) >> 5;
  uVar4 = fn_801F18DC(0);
  uVar4 = (u32)__cntlzw(1 - (uVar4 & 0xff)) >> 5;
  uVar6 = fn_801F8000(uVar3);
  if ((uVar6 == 0) && ((uVar1 & 0xff) == 0)) {
    uVar4 = 1;
  }
  uVar6 = fn_802037DC(uVar2);
  fn_80132A38(0x16,uVar6);
  uVar6 = fn_802037DC(uVar2);
  fn_80132A38(0xd,uVar6);
  uVar6 = fn_801F8000(uVar3);
  fn_80132A38(0x22,uVar6);
  uVar6 = fn_801F8100(uVar3);
  fn_80132A38(0x23,uVar6);
  uVar3 = fn_801F8100(uVar3);
  fn_80132A38(0x25,uVar3);
  if ((uVar4 & 0xff) == 1) {
    (&lbl_80478D78)[5] = 5;
  }
  else if ((uVar1 & 0xff) == 1) {
    iVar5 = fn_801F2350(0,uVar2);
    if (iVar5 < 0) {
      (&lbl_80478D78)[5] = 1;
    }
    else if (iVar5 == 0) {
      (&lbl_80478D78)[5] = 0;
    }
    else if (iVar5 < 0x1e) {
      (&lbl_80478D78)[5] = 1;
    }
    else if (iVar5 < 0x46) {
      (&lbl_80478D78)[5] = 2;
    }
    else {
      (&lbl_80478D78)[5] = 3;
    }
  }
  else {
    (&lbl_80478D78)[5] = 4;
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021DB78 | Size: 0x1ac | Ghidra import */
void fn_8021DB78(void)

{
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern u32 fn_801F18DC();
    extern int fn_801F2434();
    extern u32 fn_801F4354();
    extern u32 fn_801F8000();
    extern u32 fn_801F8100();
    extern u32 fn_802037DC();
    extern u32 fn_80204A10();
    extern u8 lbl_80478D78;
  u32 uVar2;
  u32 uVar3;
  u32 uVar1;
  u32 uVar4;
  int iVar5;
  u32 uVar6;

  uVar2 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar3 = fn_801F4354(0,uVar2);
  uVar4 = fn_80204A10(uVar2);
  uVar1 = (u32)__cntlzw(1 - (uVar4 & 0xff)) >> 5;
  uVar4 = fn_801F18DC(0);
  uVar4 = (u32)__cntlzw(1 - (uVar4 & 0xff)) >> 5;
  uVar6 = fn_801F8000(uVar3);
  if ((uVar6 == 0) && ((uVar1 & 0xff) == 0)) {
    uVar4 = 1;
  }
  uVar6 = fn_802037DC(uVar2);
  fn_80132A38(0x16,uVar6);
  uVar6 = fn_802037DC(uVar2);
  fn_80132A38(0xd,uVar6);
  uVar6 = fn_801F8000(uVar3);
  fn_80132A38(0x22,uVar6);
  uVar6 = fn_801F8100(uVar3);
  fn_80132A38(0x23,uVar6);
  uVar3 = fn_801F8100(uVar3);
  fn_80132A38(0x25,uVar3);
  if ((uVar4 & 0xff) == 1) {
    (&lbl_80478D78)[5] = 5;
  }
  else if ((uVar1 & 0xff) == 1) {
    iVar5 = fn_801F2434(0,uVar2);
    if (iVar5 >= 0x46) goto _zero;
    if (iVar5 < 0) {
_zero:
      (&lbl_80478D78)[5] = 0;
    }
    else if (iVar5 >= 0x28) {
      (&lbl_80478D78)[5] = 1;
    }
    else if (iVar5 >= 0xa) {
      (&lbl_80478D78)[5] = 2;
    }
    else {
      (&lbl_80478D78)[5] = 3;
    }
  }
  else {
    (&lbl_80478D78)[5] = 4;
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* 0x8021DD24 | size: 0x10 | tiny */
void fn_8021DD24(void) { lbl_8047B610 = lbl_8047B610 + 5; }

/* 0x8021DD34 | size: 0x10 | tiny */
void fn_8021DD34(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x8021DD44 | Size: 0x74 | Ghidra import */
void fn_8021DD44(void)

{
    extern void fn_801DA36C();
    extern int fn_801F025C();
    extern void fn_80209484();
  u32 iVar1;

  iVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  if (iVar1 != 0) {
    fn_80209484(iVar1,1);
    iVar1 = (int)fn_8012640C(iVar1,0,0xee,0);
    if (iVar1 != 0) {
      fn_801DA36C(iVar1,3);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* 0x8021DDB8 | size: 0x10 | tiny */
void fn_8021DDB8(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* 0x8021DDC8 | size: 0x10 | tiny */
void fn_8021DDC8(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x8021DDD8 | Size: 0x64 | Ghidra import */
void fn_8021DDD8(void)

{
    extern void fn_801FE468();
    extern void fn_802621C4();
    extern void fn_802621F4();
    extern void fn_802622E4();
    extern u32 lbl_8047B64C;
    extern u8 lbl_80478278[];

  u8 auStack_28 [16];
  u8 auStack_18 [24];

  fn_801FE468(lbl_8047B64C,auStack_18);
  fn_802621F4(auStack_18,lbl_80478278,auStack_28);
  fn_802621C4(auStack_28,1);
  fn_802621C4(auStack_18,0);
  fn_802622E4();
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* 0x8021DE3C | size: 0x10 | tiny */
void fn_8021DE3C(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x8021DE4C | Size: 0x7c | Ghidra import */
void fn_8021DE4C(void)

{
    extern u32 fn_801F025C();
    extern void fn_801FE7EC();
    extern s8 fn_801FECD4();
    extern void fn_8020147C();
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  fn_8020147C(uVar1,0,0,1);
  cVar2 = fn_801FECD4(uVar1);
  if (cVar2 == 1) {
    fn_801FE7EC(uVar1,0x82,0,0);
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021DEC8 | Size: 0x34 | Ghidra import */
void fn_8021DEC8(void)

{
    extern void fn_802271E0();

  fn_802271E0(1,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021DEFC | Size: 0x40 | Ghidra import */
void fn_8021DEFC(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8021DF3C();

  fn_801F37B0(0,fn_8021DF3C,0,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021DF3C | Size: 0x34 | Ghidra import */
u32 fn_8021DF3C(void)

{
    u32 r3;

  fn_801254B4(r3,0,0x112,0,1);
  return 1;
}
/* 0x8021DF70 | size: 0x10 | tiny */
void fn_8021DF70(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x8021DF80 | Size: 0xcc | Ghidra import */
void fn_8021DF80(void)

{
    extern void fn_801DA9E8();
    extern u8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_80265598();
    extern u32 lbl_8047B618;
  u16 uVar3;
  int iVar2;
  u32 uVar1;
  u8 cVar4;
  u32 uVar5;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar5 = *(u32 *)(lbl_8047B610 + 3);
  if (((lbl_8047B618 & 0x80) == 0) && (uVar5 == 7)) {
    uVar3 = fn_801F54A4(0,0,0x14,0);
    iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
    if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x34,4,0), cVar4 != 0)) {
      fn_801DA9E8(iVar2,0x34,4);
      fn_80265598(uVar1,uVar3,1);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 7;
  return;
}
/* Address: 0x8021E04C | Size: 0x23c | Ghidra import */
void fn_8021E04C(void)

{
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_80265598();
    extern u32 lbl_8047B618;
  u16 uVar3;
  int iVar2;
  u8 cVar4;
  int iVar7;
  u16 uVar6;
  u32 iVar5;
  u32 uVar1;

  fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  iVar5 = *(u32 *)(lbl_8047B610 + 2);
  if ((lbl_8047B618 & 0x80) == 0) {
    if (iVar5 == 9) {
      uVar6 = fn_801F54A4(0,0,0x14,0);
      iVar7 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar7 != 0) && (cVar4 = fn_801DDD28(iVar7,0x34,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar7,0x34,4);
        fn_80265598(uVar1,uVar6,1);
      }
    }
    if (iVar5 == 0x18) {
      uVar3 = fn_801F54A4(0,0,0x14,0);
      iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x35,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x35,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    if (iVar5 == 0x17) {
      uVar3 = fn_801F54A4(0,0,0x14,0);
      iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x36,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x36,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    if (iVar5 == 10) {
      uVar3 = fn_801F54A4(0,0,0x14,0);
      iVar5 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar5 != 0) && (cVar4 = fn_801DDD28(iVar5,0x37,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar5,0x37,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
  }
  lbl_8047B610 = lbl_8047B610 + 6;
  return;
}
/* Address: 0x8021E288 | Size: 0x378 | Ghidra import */
void fn_8021E288(void)

{
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    extern void fn_80265598();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u8 cVar4;

  fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  if ((lbl_8047B618 & 0x80) == 0) {
    cVar4 = fn_802026E4(uVar1,8);
    if (cVar4 == 1) {
      u16 uVar3 = fn_801F54A4(0,0,0x14,0);
      int iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x2e,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x2e,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    cVar4 = fn_802026E4(uVar1,5);
    if (cVar4 == 1) {
      u16 uVar3 = fn_801F54A4(0,0,0x14,0);
      int iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x2f,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x2f,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    cVar4 = fn_802026E4(uVar1,7);
    if (cVar4 == 1) {
      u16 uVar3 = fn_801F54A4(0,0,0x14,0);
      int iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x30,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x30,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    cVar4 = fn_802026E4(uVar1,6);
    if (cVar4 == 1) {
      u16 uVar3 = fn_801F54A4(0,0,0x14,0);
      int iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x31,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x31,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    cVar4 = fn_802026E4(uVar1,3);
    if (cVar4 == 1) {
      u16 uVar3 = fn_801F54A4(0,0,0x14,0);
      int iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x32,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x32,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
    cVar4 = fn_802026E4(uVar1,4);
    if (cVar4 == 1) {
      u16 uVar3 = fn_801F54A4(0,0,0x14,0);
      int iVar2 = (int)fn_8012640C(uVar1,0,0xee,0);
      if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2,0x33,4,0), cVar4 != 0)) {
        fn_801DA9E8(iVar2,0x33,4);
        fn_80265598(uVar1,uVar3,1);
      }
    }
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021E600 | Size: 0xcc | Ghidra import */
void fn_8021E600(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern void fn_801F54A4();
    extern void fn_80209C1C();
    extern u16 lbl_8047B60C;
  u16 uVar1;
  u32 uVar2;
  u32 uVar3;

  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar2,0,0xd9,0);
  uVar1 = lbl_8047B60C;
  uVar3 = fn_8011BEB4(0,uVar1,9,0);
  if (*(char *)(*(int *)(lbl_8047B610) + 1) == 0) {
    fn_8011BBD8(uVar2,0,0x27,0,uVar1);
    fn_80209C1C(uVar2,uVar1);
  }
  else {
    fn_80209C1C(uVar2,uVar1);
  }
  *(u32 *)(lbl_8047B610) = *(u32 *)((uVar3 & 0xffff) * 4 + -0x7fc86401);
  return;
}
/* 0x8021E6CC | size: 0x10 | tiny */
void fn_8021E6CC(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* 0x8021E6DC | size: 0x10 | tiny */
void fn_8021E6DC(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* 0x8021E6EC | size: 0x10 | tiny */
void fn_8021E6EC(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* Address: 0x8021E6FC | Size: 0x48 | Ghidra import */
void fn_8021E6FC(void)

{
    extern void fn_801F22D8();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 uVar2;

  fn_801F22D8(0);
  uVar1 = lbl_8047B618;
  uVar2 = uVar1 | 0x1000;
  if ((uVar1 & 0x1000) != 0) {
    uVar2 = uVar1 & 0xffffefff;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  lbl_8047B618 = uVar2;
  return;
}
/* 0x8021E744 | size: 0x10 | tiny */
void fn_8021E744(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* Address: 0x8021E754 | Size: 0x2a0 | Ghidra import */
void fn_8021E754(void)

{
    extern void fn_801294C4();
    extern u32 fn_8012A5B0();
    extern void fn_80132A38();
    extern void fn_801CA8B0();
    extern u32 fn_801F025C();
    extern int fn_801F2A7C();
    extern u32 fn_801F54A4();
    extern int fn_801FB1C0();
    extern u32 fn_801FBD40();
    extern void fn_801FBD58();
    extern int fn_80203E7C();
    extern u8 fn_80206A04();
  u16 uVar8;
  u8 cVar10;
  int iVar1;
  int iVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar9;
  u32 uVar5;
  u32 uVar6;
  u8 bVar11;
  int iVar7;

  u32 uVar12;
  u8 bVar13;
  
  uVar8 = fn_801F54A4(0,0,0x18,0);
  cVar10 = fn_801F54A4(0,0,0x25,0);
  if (cVar10 == 0) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  else {
    iVar1 = fn_801F2A7C(0);
    if (iVar1 == 0) {
      lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
    }
    else {
      iVar2 = fn_801FB1C0(iVar1,0,0x44,0);
      if (iVar2 == 0) {
        lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
      }
      else {
        uVar3 = fn_801FB1C0(iVar1,0,0x48,0);
        uVar4 = fn_801F025C(9,iVar1);
        uVar9 = fn_801FB1C0(uVar4,0,0x43,0);
        uVar9 = fn_801FB1C0(0,uVar9,4,0);
        fn_801FBD58(uVar9);
        uVar5 = fn_801FBD40();
        bVar13 = 0;
        uVar12 = 0;
        do {
          uVar6 = fn_801FB1C0(uVar4,0,0x45,uVar12 & 0xffff);
          cVar10 = fn_80206A04();
          if ((cVar10 != 0) && (bVar11 = fn_80203E7C(uVar6), bVar13 < bVar11)) {
            bVar13 = bVar11;
          }
          uVar12 = uVar12 + 1;
        } while ((int)uVar12 < 6);
        uVar12 = 0;
        do {
          uVar4 = fn_801FB1C0(iVar1,0,0x45,uVar12 & 0xffff);
          cVar10 = fn_80206A04();
          if (((cVar10 != 0) && (iVar7 = (int)fn_8012640C(uVar4,0,0xcf,0), iVar7 == 1)) &&
             (bVar11 = fn_80203E7C(uVar4), bVar13 < bVar11)) {
            bVar13 = bVar11;
          }
          uVar12 = uVar12 + 1;
        } while ((int)uVar12 < 6);
        cVar10 = fn_801F54A4(0,0,0x26,0);
        if (cVar10 == 0) {
          if (uVar8 < 2) {
            iVar1 = 1;
          }
          else {
            iVar1 = 2;
          }
        }
        else {
          iVar1 = 4;
        }
        iVar1 = iVar1 * (uVar5 & 0xffff) * (u32)bVar13 * (uVar3 & 0xff);
        cVar10 = fn_801F54A4(0,0,0x26,0);
        if (cVar10 == 1) {
          fn_801CA8B0(iVar1);
          lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
        }
        else {
          fn_801294C4(iVar2,iVar1);
          fn_80132A38(0x2f,iVar1);
          fn_80132A38(0x4b,iVar1);
          uVar4 = fn_8012A5B0(iVar2,1,0);
          fn_80132A38(0x13,uVar4);
          lbl_8047B610 = lbl_8047B610 + 5;
        }
      }
    }
  }
  return;
}
/* Address: 0x8021E9F4 | Size: 0xa0 | Ghidra import */
void fn_8021E9F4(void)

{
    extern u32 fn_801F025C();
    extern void fn_80209380();
    extern u8 fn_802096E8();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 uVar2;
  u8 cVar3;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar2 = fn_801F025C(0x11,0);
  fn_8012640C(uVar2,0,0xd9,0);
  cVar3 = fn_802096E8();
  if (cVar3 == 1) {
    if ((lbl_8047B618 & 0x100) != 0) {
      lbl_8047B610 = lbl_8047B610 + 2;
      return;
    }
    if ((lbl_8047B618 & 0x80) != 0) {
      fn_80209380(uVar1);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021EA94 | Size: 0x54 | Ghidra import */
void fn_8021EA94(void)

{
    extern u8 fn_80262308();
    extern u8 lbl_8047B642;
  u8 cVar1;

  cVar1 = fn_80262308();
  if (cVar1 == 1) {
    lbl_8047B642 = (char)lbl_8047B642 + 1;
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
/* Address: 0x8021EAE8 | Size: 0x118 | Ghidra import */
void fn_8021EAE8(void)

{
    extern u32 fn_800FA280();
    extern s8 fn_8011BEB4();
    extern void fn_80123D58();
    extern void fn_80132A38();
    extern u32 fn_80205BE8();
    extern u32 fn_80262270();
    extern s8 fn_80262308();
    extern void fn_8026246C();
    extern s8 fn_802624CC();
    extern u16 lbl_8047B61C;
    extern u32 lbl_8047B64C;
  u32 uVar1;
  s8 cVar4;
  u32 uVar2;
  u16 uVar3;

  uVar1 = fn_80205BE8(lbl_8047B64C);
  cVar4 = fn_80262308();
  if (cVar4 == 1) {
    while (1) {
      fn_8026246C();
      uVar2 = fn_80262270(uVar1,lbl_8047B61C);
      if ((uVar2 == 0xffffffff) || (3 < (int)uVar2)) break;
      uVar3 = (int)fn_8012640C(uVar1,0,0x7f,uVar2 & 0xffff);
      cVar4 = fn_8011BEB4(0,uVar3,0x19,0);
      if (cVar4 != 1) {
        fn_80123D58(uVar1,uVar2 & 0xffff,lbl_8047B61C);
        fn_8011BEB4(0,uVar3,1,0);
        uVar1 = fn_800FA280();
        fn_80132A38(0xe,uVar1);
        fn_80132A38(0x5d,0x468);
        *(u32 *)(lbl_8047B610) = *(u32 *)(*(int *)(lbl_8047B610) + 1);
        return;
      }
      cVar4 = fn_802624CC(0x7635);
      if (cVar4 == 1) {
        fn_8026246C();
      }
    }
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 5;
  return;
}
/* Address: 0x8021EC00 | Size: 0xf8 | Ghidra import */
void fn_8021EC00(void)

{
    extern u32 fn_80123584();
    extern s8 fn_8012361C();
    extern u32 fn_801236F8();
    extern u32 fn_80203E7C();
    extern u32 fn_80205BE8();
    extern u16 lbl_8047B61C;
    extern u8 lbl_8047B642;
    extern u32 lbl_8047B64C;
  u32 uVar1;
  u32 uVar2;
  u8 uVar4;
  s8 cVar5;
  u16 uVar3;
  int iVar6;

  u32 uVar7;
  u32 uVar8;
  
  uVar1 = lbl_8047B64C;
  uVar2 = fn_80203E7C();
  uVar1 = fn_80205BE8(uVar1);
  iVar6 = *(int *)(lbl_8047B610);
  uVar8 = *(u32 *)(iVar6 + 1);
  uVar7 = *(u32 *)(iVar6 + 5);
  if (*(char *)(iVar6 + 9) != 0) {
    uVar4 = fn_80123584(uVar1,uVar2);
    lbl_8047B642 = uVar4;
  }
  while (1) {
    cVar5 = fn_8012361C(uVar1,uVar2,0,lbl_8047B642);
    if (cVar5 == -1) {
      uVar3 = fn_801236F8(uVar1,uVar2,lbl_8047B642);
      lbl_8047B61C = uVar3;
      *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 10;
      return;
    }
    if (cVar5 != -2) break;
    lbl_8047B642 = (char)lbl_8047B642 + 1;
  }
  if (cVar5 == -3) {
    *(u32 *)(lbl_8047B610) = uVar7;
    return;
  }
  lbl_8047B642 = (char)lbl_8047B642 + 1;
  uVar3 = (int)fn_8012640C(uVar1,0,0x7f);
  lbl_8047B61C = uVar3;
  *(u32 *)(lbl_8047B610) = uVar8;
  return;
}
/* Address: 0x8021ECF8 | Size: 0x78 | Ghidra import */
void fn_8021ECF8(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_80203848();
    extern u16 lbl_8047B61C;
    extern u32 lbl_8047B64C;
  u32 uVar2;
  u32 uVar1;
  u32 uVar3;

  uVar2 = lbl_8047B64C;
  fn_8011BEB4(0,lbl_8047B61C,1,0);
  uVar1 = fn_800FA280();
  uVar3 = fn_80203848(uVar2);
  fn_80132A38(0xd,uVar3);
  fn_80132A38(0xe,uVar1);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021ED70 | Size: 0xc8 | Ghidra import */
void fn_8021ED70(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802062FC();
    extern void fn_80208ED0();
    extern void fn_8026532C();
    extern void fn_80265598();
  u32 uVar1;
  u16 uVar2;
  u8 cVar3;

  uVar2 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  cVar3 = fn_802062FC();
  if (cVar3 == 1) {
    fn_80208ED0(uVar1,0);
    fn_80265598(uVar1,uVar2,1);
    fn_80208ED0(uVar1,1);
    fn_80208ED0(uVar1,2);
    fn_80208ED0(uVar1,3);
    fn_80208ED0(uVar1,4);
    fn_8026532C(uVar1,uVar2,0);
  }
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* 0x8021EE38 | size: 0x10 | tiny */
void fn_8021EE38(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Address: 0x8021EE48 | Size: 0x50 | Ghidra import */
void fn_8021EE48(void)

{
    extern void fn_80166A50();
    extern void fn_801F025C();
    extern u32 fn_802036D4();
  u16 uVar1;

  fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar1 = fn_802036D4();
  fn_80166A50(uVar1,0,0xff,0);
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021EE98 | Size: 0x3c | Ghidra import */
void fn_8021EE98(void)

{
    extern void fn_80165668();

  fn_80165668(*(u16 *)(lbl_8047B610 + 1),0,0xff);
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* Address: 0x8021EED4 | Size: 0x40 | Ghidra import */
void fn_8021EED4(void)

{
    extern void fn_80166A50();

  fn_80166A50(*(u16 *)(lbl_8047B610 + 1),0,0xff,0);
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* 0x8021EF14 | size: 0x10 | tiny */
void fn_8021EF14(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* Address: 0x8021EF24 | Size: 0x2a8 | Ghidra import */
void fn_8021EF24(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern void fn_801F2F3C();
    extern void fn_801F3074();
    extern void fn_801F3178();
    extern void fn_801F4C14();
    extern short fn_801F6D9C();
    extern s8 fn_801F6E98();
    extern void fn_801F75F8();
    extern int fightSideGetStatus();
    extern u32 fn_80203B5C();
    extern s8 fn_80207AE0();
    extern void fn_80209FAC();
    extern void fn_8020A2B8();
    extern void fn_80211B94();
    extern void fn_8022D084();
    extern void fn_8022E410();
    extern void fn_8022E6F0();
    extern u32 lbl_8047B62C;
    extern u8 lbl_80378DAF[];
    extern u8 lbl_80378D7C[];
    extern u8 lbl_80378DE2[];
  u32 uVar2;
  u32 uVar3;
  u16 uVar7;
  u32 uVar5;
  u32 uVar1;
  u32 sVar6;
  int iVar4;
  u8 cVar8;

  u8 auStack_c8 [184];

  uVar2 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar3 = fn_801F025C(2,uVar2);
  sVar6 = fn_80207BF4(uVar2);
  fn_801F3178(0);
  fn_801F3074(0);
  fn_801254B4(uVar2,0,0x119,0,0);
  fn_801254B4(uVar2,0,0x121,0, (void*)0xffffffff);
  iVar4 = fightSideGetStatus(uVar3,0,8,0);
  if ((((iVar4 == 0) && (cVar8 = fn_801F6E98(uVar3,0x4a), cVar8 == 1)) &&
      (cVar8 = fn_80207AE0(uVar2,2), cVar8 == 0)) && ((u16)sVar6 != 0x1a)) {
    fn_801F75F8(uVar3,0,8,0,1);
    uVar7 = fn_80203B5C(uVar2,(5 - (short)fn_801F6D9C(uVar3,0x4a)) * 2 & 0xfffe);
    uVar5 = (int)fn_8012640C(fn_801F025C(0x11,0),0,0xd9,0);
    fn_8020A2B8(auStack_c8,uVar5);
    fn_8011BBD8(uVar5,0,0x2d,0,uVar7);
    fn_80209FAC(uVar5);
    fn_801F4C14(0,0,0x4b,0,uVar2);
    cVar8 = *(char *)(lbl_8047B610 + 1);
    if (cVar8 == 18) {
      uVar1 = (u32)lbl_80378DAF;
    }
    else if ((cVar8 == 17) || (cVar8 == 20)) {
      uVar1 = (u32)lbl_80378D7C;
    }
    else {
      uVar1 = (u32)lbl_80378DE2;
    }
    fn_80211B94(lbl_8047B62C,uVar1,0);
    fn_8020A2B8(uVar5,auStack_c8);
    iVar4 = fightSideGetStatus(uVar3,0,8,0);
    if (iVar4 == 0) {
      lbl_8047B610 = lbl_8047B610 + 2;
      return;
    }
  }
  sVar6 = fn_80207BF4(uVar2);
  if ((u16)sVar6 == 0x36) {
    fn_801254B4(uVar2,0,0xf9,0,1);
  }
  fn_8022E6F0(uVar2,0);
  fn_8022E410(uVar2);
  fn_8022D084(uVar2);
  fn_8022E6F0(uVar2,1);
  fn_801F2F3C(0);
  fn_801F75F8(uVar3,0,8,0,0);
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021F1CC | Size: 0x80 | Ghidra import */
void fn_8021F1CC(void)

{
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern u32 fn_80203758();
    extern u32 fn_802037DC();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  switch (*(u8 *)(lbl_8047B610 + 2)) {
  case 0:
  case 1:
    break;
  case 2:
    fn_80132A38(0xd,fn_802037DC());
    uVar1 = fn_80203758(uVar1);
    fn_80132A38(0xe,uVar1);
    break;
  }
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* Address: 0x8021F24C | Size: 0x150 | Ghidra import */
void fn_8021F24C(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F150C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_801F8A18();
    extern short fn_801F9600();
  u16 uVar4;
  u32 uVar1;
  u32 uVar2;
  s8 cVar6;
  int iVar3;
  short sVar5;

  u32 uVar7;
  u16 local_18 [4];
  
  uVar4 = fn_801F54A4(0,0,0x14,0);
  uVar7 = *(u32 *)(*(int *)(lbl_8047B610) + 2);
  uVar1 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  uVar2 = fn_801F4354(0,uVar1);
  cVar6 = (int)fn_8012640C(uVar1,0,0x119,0);
  if (cVar6 == 1) {
    *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 6;
  }
  else {
    local_18[0] = 0;
    iVar3 = fn_801F8A18(uVar2,local_18);
    if (iVar3 == 0) {
      *(u32 *)(lbl_8047B610) = uVar7;
    }
    else {
      sVar5 = fn_801F9600(uVar2,uVar4,0,uVar1);
      if (sVar5 < 0) {
        if (sVar5 == -2) {
          fn_801F150C(0);
        }
        *(u32 *)(lbl_8047B610) = uVar7;
      }
      else {
        fn_801F4C14(0,0,0x45,0,uVar1);
        fn_801254B4(uVar1,0,0x121,0,(int)sVar5);
        fn_801254B4(uVar1,0,0x119,0,1);
        *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 6;
      }
    }
  }
  return;
}
/* Address: 0x8021F39C | Size: 0xbc | Ghidra import */
void fn_8021F39C(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_801F2020();
    extern u32 fn_801F4354();
    extern u32 fn_801F8A18();
  u32 uVar1;
  u32 uVar2;
  u8 cVar4;
  u32 iVar3;

  u16 local_18 [8];

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar2 = fn_801F4354(0,uVar1);
  if ((*(char *)(lbl_8047B610 + 6) == 1) &&
     (cVar4 = fn_801F2020(0,uVar1,0), cVar4 == 1)) {
    lbl_8047B610 = *(u32 *)(lbl_8047B610 + 2);
  }
  else {
    local_18[0] = 0;
    iVar3 = fn_801F8A18(uVar2,local_18);
    if (iVar3 == 0) {
      iVar3 = *(int *)(lbl_8047B610 + 2);
    }
    else {
      iVar3 = lbl_8047B610 + 7;
    }
    lbl_8047B610 = iVar3;
  }
  return;
}
/* Address: 0x8021F458 | Size: 0x20c | Ghidra import */
void fn_8021F458(void)

{
    extern void _threadSwitch();
    extern u8 fn_801DA5C4();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void fn_801FBC20();
    extern int fn_801FEF74();
    extern u8 fn_802026E4();
    extern u8 fn_80204A10();
    extern void fn_80205A7C();
    extern void fn_80205AD4();
    extern void fn_80206C94();
    extern void fn_802086B0();
    extern void fn_80208C18();
    extern void fn_8020F108();
    extern void fn_8026246C();
    extern void fn_8026532C();
    extern void fn_80265598();
  u32 uVar1;
  u32 uVar3;
  u32 uVar2;
  u16 uVar5;
  u8 cVar7;
  u8 cVar6;
  int iVar4;

  uVar5 = fn_801F54A4(0,0,0x14,0);
  cVar6 = *(u8 *)(lbl_8047B610 + 1);
  cVar7 = *(u8 *)(lbl_8047B610 + 2);
  uVar1 = fn_801F025C(cVar6,0);
  fn_8012640C(uVar1,0,0xd5,0);
  uVar2 = fn_801F4354(0,uVar1);
  uVar3 = fn_801FB1C0(uVar2,0,0x47,0);
  if (cVar7 != 2) {
    fn_801FBC20(uVar2,uVar1,2);
  }
  cVar6 = fn_80204A10(uVar1);
  if (cVar6 == 0) {
    fn_80265598(uVar1,uVar5,0);
  }
  else {
    fn_80265598(uVar1,uVar5,1);
  }
  if (cVar7 != 2) {
    fn_80208C18(uVar1,1);
    fn_80208C18(uVar1,2);
  }
  fn_80208C18(uVar1,3);
  fn_80208C18(uVar1,4);
  fn_8026532C(uVar1,uVar5,0);
  if (cVar7 != 2) {
    fn_801FBC20(uVar2,uVar1,3);
  }
  fn_80208C18(uVar1,5);
  if (((cVar7 == 1) && (cVar7 = fn_802026E4(uVar1,0x14), cVar7 == 1)) &&
     (iVar4 = fn_801FEF74(uVar1), 0 < iVar4)) {
    fn_8026246C();
    fn_8020F108(0xa4,uVar1,uVar1,0,0);
    while (1) {
      cVar7 = fn_801DA5C4(6);
      if (cVar7 == 1) break;
      _threadSwitch();
    }
    fn_802086B0(uVar1);
  }
  cVar7 = fn_801F54A4(0,0,0x1e,0);
  if ((cVar7 == 1) && (cVar7 = fn_80204A10(uVar1), cVar7 == 0)) {
    fn_80205AD4(uVar1,0);
    fn_80205A7C(uVar1,0);
  }
  fn_80206C94(uVar3);
  lbl_8047B610 = lbl_8047B610 + 3;
  return;
}
/* Address: 0x8021F664 | Size: 0x2c8 | Ghidra import */
void fn_8021F664(void)

{
    extern void fn_8010AE2C();
    extern u32 fn_80121C18();
    extern void fn_80132A38();
    extern void fn_801C3430();
    extern void fn_801C3E3C();
    extern u32 fn_801F025C();
    extern void fn_801F37B0();
    extern u32 fn_801F4354();
    extern u32 fn_801F8F24();
    extern u32 fn_801FB1C0();
    extern void fn_801FBC20();
    extern void fn_80201600();
    extern void fn_802019BC();
    extern s8 fn_802026E4();
    extern void fn_80202A2C();
    extern s8 fn_80202ADC();
    extern u32 fn_802037DC();
    extern void fn_80204970();
    extern void fn_80205BE8();
    extern void fn_802068C8();
    extern void fn_80208028();
    extern void fn_80208C18();
  s8 cVar1;
  u16 uVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  u32 uVar6;
  short sVar11;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  s8 cVar12;
  u32 uVar10;
  int *piVar13;
  int *piVar14;

  u8 bVar15;
  int iVar16;
  u32 local_48;
  int local_44;
  int local_40 [3];
  u16 auStack_34 [10];
  
  cVar1 = *(char *)(*(int *)(lbl_8047B610) + 2);
  uVar6 = fn_801F025C(*(u8 *)(*(int *)(lbl_8047B610) + 1),0);
  sVar11 = (int)fn_8012640C(uVar6,0,0x121,0);
  uVar7 = (int)fn_8012640C(uVar6,0,0xd5,0);
  uVar8 = fn_801F4354(0,uVar6);
  uVar9 = fn_801FB1C0(uVar8,0,0x47,0);
  cVar12 = fn_80202ADC(uVar7,4);
  if (cVar12 == 1) {
    fn_80202A2C(uVar7,4,1);
  }
  uVar10 = fn_801F8F24(uVar8,(int)sVar11);
  fn_80204970(uVar10,uVar7);
  fn_8010AE2C(uVar7,0,0);
  fn_80205BE8(uVar7);
  uVar10 = fn_80121C18();
  fn_802068C8(uVar6,uVar7,uVar10);
  fn_80208028(uVar6);
  iVar16 = 2;
  piVar3 = (int *)0x80279fdc;
  piVar4 = &local_44;
  do {
    piVar14 = piVar4;
    piVar13 = piVar3;
    iVar5 = piVar13[2];
    piVar14[1] = piVar13[1];
    piVar14[2] = iVar5;
    iVar16 = iVar16 + -1;
    piVar3 = piVar13 + 2;
    piVar4 = piVar14 + 2;
  } while (iVar16 != 0);
  iVar16 = 0;
  piVar14[3] = piVar13[3];
  *(u16 *)(piVar14 + 4) = *(u16 *)(piVar13 + 4);
  if (cVar1 == 1) {
    iVar16 = 0x7f;
  }
  if (iVar16 == 0x7f) {
    fn_80201600(uVar6,uVar9);
    for (bVar15 = 0; bVar15 < 0xb; bVar15 = bVar15 + 1) {
      uVar2 = auStack_34[bVar15 - 6];
      cVar12 = fn_802026E4(uVar9,uVar2);
      if (cVar12 == 1) {
        fn_802019BC(uVar6,uVar9,uVar2);
      }
    }
  }
  local_48 = uVar6;
  local_44 = iVar16;
  fn_801F37B0(0,0x80232d28,&local_48,0);
  cVar12 = fn_802026E4(uVar9,0x34);
  if (cVar12 == 1) {
    fn_802019BC(uVar6,uVar9,0x34);
  }
  cVar12 = fn_802026E4(uVar9,0x35);
  if (cVar12 == 1) {
    fn_802019BC(uVar6,uVar9,0x35);
  }
  uVar7 = fn_802037DC(uVar6);
  fn_80132A38(0xd,uVar7);
  iVar16 = fn_801FB1C0(uVar8,0,0x4c,0);
  if ((iVar16 != 0) && (iVar5 = (int)fn_8012640C(uVar6,0,0xee,0), iVar5 != 0)) {
    fn_801C3E3C(iVar16);
  }
  if (cVar1 != 2) {
    fn_801FBC20(uVar8,uVar6,0);
  }
  fn_80208C18(uVar6,0);
  fn_801C3430();
  if (cVar1 != 2) {
    fn_801FBC20(uVar8,uVar6,1);
  }
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 3;
  return;
}
/* Address: 0x8021F92C | Size: 0x6c | Ghidra import */
void fn_8021F92C(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern u32 fn_801FB1C0();
    extern void fn_801FCEC4();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar2 = fn_801F4354(0,uVar1);
  uVar2 = fn_801FB1C0(uVar2,0,0x47,0);
  fn_801FCEC4(uVar2,uVar1);
  lbl_8047B610 = lbl_8047B610 + 2;
  return;
}
/* Address: 0x8021F998 | Size: 0x13c | Ghidra import */
void fn_8021F998(void)

{
    extern void _threadSwitch();
    extern u8 fn_801DA5C4();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern int fn_801FEF74();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802062FC();
    extern void fn_802086B0();
    extern void fn_80208ED0();
    extern void fn_8020F108();
    extern void fn_8026532C();
    extern void fn_80265598();
  u32 uVar1;
  u16 uVar3;
  u8 cVar4;
  int iVar2;

  uVar3 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x14,0);
  cVar4 = fn_802062FC();
  if (cVar4 == 1) {
    cVar4 = fn_802026E4(uVar1,0x14);
    if ((cVar4 == 1) && (iVar2 = fn_801FEF74(uVar1), 0 < iVar2)) {
      fn_80202810(uVar1,0x14);
      fn_802086B0(uVar1);
      fn_8020F108(0xa4,uVar1,uVar1,0,0);
      for (;;) {
        cVar4 = fn_801DA5C4(6);
        if (cVar4 == 1) break;
        _threadSwitch();
      }
    }
    fn_802086B0(uVar1);
    fn_80208ED0(uVar1,0);
    fn_80265598(uVar1,uVar3,1);
    fn_80208ED0(uVar1,1);
    fn_80208ED0(uVar1,2);
    fn_80208ED0(uVar1,3);
    fn_80208ED0(uVar1,4);
    fn_8026532C(uVar1,uVar3,0);
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
/* Address: 0x8021FAD4 | Size: 0x240 | Ghidra import */
void fn_8021FAD4(void)

{
    extern void fn_8011BEB4();
    extern int fn_801F025C();
    extern void fn_801F54A4();
    extern void fn_80203FE4();
    extern u32 fn_80205224();
    extern void fn_802096E8();
    extern u8 lbl_8047B627;
  s8 cVar1;
  s8 cVar2;
  int iVar3;
  u32 uVar4;
  u32 uVar5;
  u16 uVar6;

  fn_801F54A4(0,0,0x14,0);
  cVar1 = *(char *)(*(int *)(lbl_8047B610) + 2);
  cVar2 = *(char *)(*(int *)(lbl_8047B610) + 1);
  iVar3 = fn_801F025C(0x11,0);
  if (iVar3 != 0) {
    fn_80203FE4();
    uVar4 = (int)fn_8012640C(iVar3,0,0xd9,0);
    fn_802096E8();
    uVar5 = fn_80205184(iVar3);
    uVar6 = fn_80205224(iVar3);
    fn_8011BEB4(uVar4,0,0x30,0,uVar6);
    fn_8011BEB4(0,uVar5,5,0);
    fn_8011BEB4(0,uVar5,9,0);
    fn_8011BEB4(0,uVar5,7,0);
  }
  iVar3 = fn_801F025C(0x12,0);
  if (iVar3 != 0) {
    fn_8012640C(iVar3,0,0x11c,0);
    fn_8012640C(iVar3,0,0x11e,0);
  }
  do {
    if (lbl_8047B627 < 0x12) {

      ((int (*)(void))**(void ***)((u32)lbl_8047B627 * 4 + -0x7fc65f58))();
      return;
    }
    if (cVar2 == 1) {
      lbl_8047B627 = 0x11;
    }
    if ((cVar2 == 2) && (cVar1 == (char)lbl_8047B627)) {
      lbl_8047B627 = 0x11;
    }
  } while ((char)lbl_8047B627 != 17);
  *(int *)(lbl_8047B610) = *(int *)(lbl_8047B610) + 3;
  return;
}
