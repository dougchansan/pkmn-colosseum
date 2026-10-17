/**
 * @file fight_range_exact_802316FC.c
 * @brief Exact island 0x802316FC - 0x80232110 (fn_802316FC, fightSeqTurnEndAll,
 *        _fightFindFightOutPokemonWithSequenceSub, fn_80232024), moved out of fight_range_80211A00.c in
 *        address order.
 */
#include "dolphin/types.h"

extern u8 lbl_80478D78[8];
extern void* lbl_8047B62C;
extern void wsRunScript(void*, void*, u8);
s32 _fightFindFightOutPokemonWithSequenceSub(void* param1, s32 param2, void** param3);
extern u32 fn_80232024();
extern u8 lbl_80378968[];
extern u8 lbl_80378A4D[];
extern u8 lbl_80378A5F[];
extern u8 lbl_80378A7C[];
extern u8 lbl_80378A8E[];
extern u8 lbl_80378B30[];
extern u8 lbl_80378B5B[];
extern u8 lbl_80379249[];
extern u8 lbl_80379F58[];

/* Field-8 clear + event trigger when the move isn't 0x2B. */
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
u8 fn_802316FC(void* ctx) {
    extern u8 fn_802062FC();
    extern void fn_80202810();
    extern u8 fn_801F4C14();
    extern u16  fn_80207BF4(void*);
    extern u8   fightOutPokemonIsPokemonJoutaiDataId();
    extern u8   fn_801FECD4();
    extern void fn_801FE7EC();
    u16 moveId = fn_80207BF4(ctx);

    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }
    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(ctx, 8) == 1 && moveId != 0x2b) {
        fn_80202810(ctx, 8);
        fn_80202810(ctx, 0x17);
        lbl_80478D78[5] = 1;
        fn_801F4C14(0, 0, 0x36, 0, (u32)ctx);
        wsRunScript(lbl_8047B62C, (void*)lbl_80379249, 0);
        if ((u8)fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x7c, 0, 0);
        }
    }
    return 1;
}
#undef fn_80207BF4
#undef fn_802062FC
#undef fn_80202810
#undef fn_801F4C14
#undef fn_801FECD4
#undef fn_801FE7EC

/* RULE-EXCEPTION(user-approved): opt_propagation off for fightSeqTurnEndAll only; the unit builds with -O4,s — see docs/RULE_EXCEPTIONS.md */
#pragma opt_propagation off
void fightSeqTurnEndAll(void) {
    extern u16 std103_battleGetResult();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fightSideIsJoutaiDataId();
    extern u8 fightFloorIsJoutaiDataId();
    u32 selected;
    u32 weather;
    u32 target;
    u32 side;
    s32 count;
    s32 next;
    u8 weatherCode;
    u8* flags;
    u8* msg;
    u8 i;

    if (std103_battleGetResult() != 0) {
        return;
    }

    weather = (u8)fightFloorGetNowTenkouDataId(0, 0);
    fightFloorCreateFightOutPokemonPtrAry(0);
    fightFloorSortFightOutPokemonPtrAry(0, 0);
    selected = 0;
    fightFloorLoopValidFightOutPokemon(0, _fightFindFightOutPokemonWithSequenceSub, &selected, 0);

    for (i = 0; i < 2; i++) {
        side = fightFloorGetValidFightSidePtr(0, i);
        if (side != 0) {
            target = fightTargetGetPtrAsNowFightType(0xc, side);
            if (fightOutPokemonCheckFightOut(target) == 0) {
                target = fightTargetGetPtrAsNowFightType(0xd, side);
            }
            fightFloorSetStatus(0, 0, 0x36, 0, target);
            if (fightSideIsJoutaiDataId(side, 0x48) == 1) {
                count = fightSideGetKaisuuJoutaiDataId(side, 0x48);
                next = (s8)((s8)fightSideGetNowKaisuuJoutaiDataId(side, 0x48) + 1);
                if (next < (s8)count) {
                    fightSideSetNowKaisuuJoutaiDataId(side, 0x48, next);
                } else {
                    fightSideInitJoutaiDataId(side, 0x48);
                    msgctrlSetValue(0xd, GSmsgGetGSchar(wazaGetStatus(0, 0x73, 1, 0)));
                    wsRunScript(lbl_8047B62C, lbl_80378B30, 0);
                }
            }
        }
    }

    wazaSequenceSysFreeAll();
    for (i = 0, flags = lbl_80478D78; i < 2; i++) {
        side = fightFloorGetValidFightSidePtr(0, i);
        if (side != 0) {
            target = fightTargetGetPtrAsNowFightType(0xc, side);
            if (fightOutPokemonCheckFightOut(target) == 0) {
                target = fightTargetGetPtrAsNowFightType(0xd, side);
            }
            fightFloorSetStatus(0, 0, 0x36, 0, target);
            if (fightSideIsJoutaiDataId(side, 0x49) == 1) {
                count = fightSideGetKaisuuJoutaiDataId(side, 0x49);
                next = (s8)((s8)fightSideGetNowKaisuuJoutaiDataId(side, 0x49) + 1);
                if (next < (s8)count) {
                    fightSideSetNowKaisuuJoutaiDataId(side, 0x49, next);
                } else {
                    fightSideInitJoutaiDataId(side, 0x49);
                    msgctrlSetValue(0xd, GSmsgGetGSchar(wazaGetStatus(0, 0x71, 1, 0)));
                    flags[5] = i;
                    wsRunScript(lbl_8047B62C, lbl_80378B30, 0);
                }
            }
        }
    }

    wazaSequenceSysFreeAll();
    for (i = 0; i < 2; i++) {
        side = fightFloorGetValidFightSidePtr(0, i);
        if (side != 0) {
            target = fightTargetGetPtrAsNowFightType(0xc, side);
            if (fightOutPokemonCheckFightOut(target) == 0) {
                target = fightTargetGetPtrAsNowFightType(0xd, side);
            }
            fightFloorSetStatus(0, 0, 0x36, 0, target);
            if (fightSideIsJoutaiDataId(side, 0x4c) == 1) {
                count = fightSideGetKaisuuJoutaiDataId(side, 0x4c);
                next = (s8)((s8)fightSideGetNowKaisuuJoutaiDataId(side, 0x4c) + 1);
                if (next < (s8)count) {
                    fightSideSetNowKaisuuJoutaiDataId(side, 0x4c, next);
                } else {
                    fightSideInitJoutaiDataId(side, 0x4c);
                    msgctrlSetValue(0xd, GSmsgGetGSchar(wazaGetStatus(0, 0x36, 1, 0)));
                    wsRunScript(lbl_8047B62C, lbl_80378B30, 0);
                }
            }
        }
    }

    wazaSequenceSysFreeAll();
    for (i = 0; i < 2; i++) {
        side = fightFloorGetValidFightSidePtr(0, i);
        if (side != 0) {
            target = fightTargetGetPtrAsNowFightType(0xc, side);
            if (fightOutPokemonCheckFightOut(target) == 0) {
                target = fightTargetGetPtrAsNowFightType(0xd, side);
            }
            fightFloorSetStatus(0, 0, 0x36, 0, target);
            if (fightSideIsJoutaiDataId(side, 0x4b) == 1) {
                count = fightSideGetKaisuuJoutaiDataId(side, 0x4b);
                next = (s8)((s8)fightSideGetNowKaisuuJoutaiDataId(side, 0x4b) + 1);
                if (next < (s8)count) {
                    fightSideSetNowKaisuuJoutaiDataId(side, 0x4b, next);
                } else {
                    fightSideInitJoutaiDataId(side, 0x4b);
                    wsRunScript(lbl_8047B62C, lbl_80378B5B, 0);
                }
            }
        }
    }

    wazaSequenceSysFreeAll();
    fightFloorLoopValidFightOutPokemon(0, fn_80232024, 0, 1);
    wazaSequenceSysFreeAll();

    if (weather == 2) {
        fightFloorSetStatus(0, 0, 0x36, 0, selected);
        if (fightFloorIsJoutaiDataId(0, 0x50) == 0) {
            count = fightFloorGetKaisuuJoutaiDataId(0, 0x54);
            next = (s8)((s8)fightFloorGetNowKaisuuJoutaiDataId(0, 0x54) + 1);
            if (next < (s8)count) {
                fightFloorSetNowKaisuuJoutaiDataId(0, 0x54, next);
                flags[5] = 0;
            } else {
                fightFloorInitJoutaiDataId(0, 0x54);
                flags[5] = 2;
            }
        } else {
            flags[5] = 0;
        }
        wsRunScript(lbl_8047B62C, lbl_80378A5F, 0);
    }

    wazaSequenceSysFreeAll();
    if (std103_battleGetResult() != 0) {
        return;
    }

    if (weather == 3) {
        fightFloorSetStatus(0, 0, 0x36, 0, selected);
        if (fightFloorIsJoutaiDataId(0, 0x51) == 1) {
            msg = lbl_80378968;
        } else {
            count = fightFloorGetKaisuuJoutaiDataId(0, 0x55);
            next = (s8)((s8)fightFloorGetNowKaisuuJoutaiDataId(0, 0x55) + 1);
            if (next < (s8)count) {
                fightFloorSetNowKaisuuJoutaiDataId(0, 0x55, next);
                msg = lbl_80378968;
            } else {
                fightFloorInitJoutaiDataId(0, 0x55);
                msg = lbl_80378A4D;
            }
        }
        weatherCode = 0xc;
        flags[5] = 0;
        lbl_80379F58[0x160A4] = weatherCode;
        wsRunScript(lbl_8047B62C, msg, 0);
    }

    wazaSequenceSysFreeAll();
    if (std103_battleGetResult() != 0) {
        return;
    }

    if (weather == 1) {
        fightFloorSetStatus(0, 0, 0x36, 0, selected);
        if (fightFloorIsJoutaiDataId(0, 0x4f) == 1) {
            msg = lbl_80378A7C;
        } else {
            count = fightFloorGetKaisuuJoutaiDataId(0, 0x53);
            next = (s8)((s8)fightFloorGetNowKaisuuJoutaiDataId(0, 0x53) + 1);
            if (next < (s8)count) {
                fightFloorSetNowKaisuuJoutaiDataId(0, 0x53, next);
                msg = lbl_80378A7C;
            } else {
                fightFloorInitJoutaiDataId(0, 0x53);
                msg = lbl_80378A8E;
            }
        }
        wsRunScript(lbl_8047B62C, msg, 0);
    }

    wazaSequenceSysFreeAll();
    if (std103_battleGetResult() != 0) {
        return;
    }

    if (weather == 4) {
        fightFloorSetStatus(0, 0, 0x36, 0, selected);
        count = fightFloorGetKaisuuJoutaiDataId(0, 0x52);
        next = (s8)((s8)fightFloorGetNowKaisuuJoutaiDataId(0, 0x52) + 1);
        if (next < (s8)count) {
            fightFloorSetNowKaisuuJoutaiDataId(0, 0x52, next);
            msg = lbl_80378968;
        } else {
            fightFloorInitJoutaiDataId(0, 0x52);
            msg = lbl_80378A4D;
        }
        weatherCode = 0xd;
        flags[5] = 1;
        lbl_80379F58[0x160A4] = weatherCode;
        wsRunScript(lbl_8047B62C, msg, 0);
    }

    wazaSequenceSysFreeAll();
    if (std103_battleGetResult() != 0) {
        return;
    }
}
#pragma opt_propagation reset

/* Report whether the 0xEE record is absent; stash the owner when present. */
#define fn_8012640C pokemonGetStatus
s32 _fightFindFightOutPokemonWithSequenceSub(void* param1, s32 param2, void** param3) {
    extern void* fn_8012640C();
    if (fn_8012640C(param1, 0, 0xee, 0) != 0) {
        if (param3 != NULL) {
            *param3 = param1;
        }
        return 0;
    }
    return 1;
}
#undef fn_8012640C

u32 fn_80232024(u32 r3)

{
    extern void fightFloorSetStatus();
    extern void fn_80201FDC();
    extern u32 fn_80202108();
    extern u32 fn_80202234();
    extern u8 fightOutPokemonIsPokemonJoutaiDataId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern void wsRunScript();
    extern void* lbl_8047B62C;
    extern u8 lbl_80379052[];
  u8 check;
  u32 count;
  u32 current;
  s32 signedCount;
  s32 signedCurrent;
  s32 incremented;

  check = fightOutPokemonCheckFightOut(r3);
  if (check == 0) {
    return 1;
  }
  check = fightOutPokemonIsPokemonJoutaiDataId(r3,0x35);
  if (check == 1) {
    count = fn_80202234(r3,0x35);
    current = fn_80202108(r3,0x35);
    signedCurrent = (s8)current;
    signedCount = (s8)count;
    incremented = (s8)(signedCurrent + 1);
    if (incremented < signedCount) {
      fn_80201FDC(r3,0x35,incremented);
    }
    else {
      fightFloorSetStatus(0,0,0x36,0,r3);
      fightFloorSetStatus(0,0,0x43,0,r3);
      wsRunScript(lbl_8047B62C,lbl_80379052,0);
      fightOutPokemonWriteJoutaiDataId(r3,0x35);
    }
  }
  return 1;
}
