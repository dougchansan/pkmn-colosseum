/**
 * @file fight_range_80211A00.c
 * @brief Fight/battle-AI support layer, 0x80211A00 - 0x802405C0 (470 fns).
 *
 * Split out of the previously-unassigned auto block (audit 2026-07-01,
 * docs/fable5_audit_pass3_fight_engine.md). Contains fightSeqGetItemType,
 * fightTrainerAiGetValueAryMaxBanme, and the callee layer used by
 * colosseum_battle.c's WazaHit family. Range name kept honest until the
 * module's internal structure is proven.
 */
#include "dolphin/types.h"

typedef struct FightSeqOpU8Operand {
    u8 opcode;
    u8 operand;
} FightSeqOpU8Operand;

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

/* fn_800E0C54: RNG (see src/game/gs_render.c, gs_model.c) */
extern u16 fn_800E0C54(void);
/* fwd: fn_802221EC defined below, called earlier in fn_80230568 */
void fn_802221EC(u32, u32, u32, u32);
int fn_802373B0(u32, u32, int, f32);

/*
 * fightTrainerAiGetValueAryMaxBanme (0x802397B8)
 *
 * Argmax over a 16-slot AI value array: scans valueAry[0..count) for the
 * maximum value (starting from the AI's "unset" sentinel -200), collects
 * every index that ties for that maximum into a 16-slot local array, then
 * returns one of the tied indices -- the first if there's no tie or
 * useRandom is not set, otherwise an RNG-selected tie via fn_800E0C54().
 */
#pragma optimize_for_size on
s32 fightTrainerAiGetValueAryMaxBanme(s32* valueAry, u16 count, u8 useRandom) {
    s32 banmeAry[16];
    s32 max = -0xC8;
    u16 tieCount = 0;
    u16 i;
    u16 idx;

    for (i = 0; i < 16; i++) {
        banmeAry[i] = -1;
    }

    for (i = 0; i < count; i++) {
        if (max < valueAry[i]) {
            max = valueAry[i];
        }
    }

    for (i = 0; i < count; i++) {
        if (max <= valueAry[i]) {
            banmeAry[tieCount] = i;
            tieCount++;
        }
        if (tieCount >= 16) {
            break;
        }
    }

    if (tieCount == 0) {
        return -1;
    }

    if (tieCount == 1) {
        idx = 0;
    } else if (useRandom == 1) {
        idx = fn_800E0C54() % tieCount;
    } else {
        idx = 0;
    }

    return banmeAry[idx];
}
#pragma optimize_for_size reset

/* Item-data accessor chain (src/game/people/people_data.c hosts fn_80143A94;
 * fn_801440A0/fn_80143DFC are still asm-only there). */
#define fn_801437E0 itemParamGetHPUp
#define fn_80143878 itemParamGetConfuseFlag
#define fn_801438A0 itemParamGetParalyzeFlag
#define fn_801438C8 itemParamGetFreezeFlag
#define fn_801438F0 itemParamGetBurnFlag
#define fn_80143918 itemParamGetPoisonFlag
#define fn_80143940 itemParamGetSleepFlag
#define fn_80143990 itemParamGetGuardFlag
#define fn_801439B8 itemParamGetSpAttackUp
#define fn_801439D4 itemParamGetHitUp
#define fn_801439F0 itemParamGetQuickUp
#define fn_80143A0C itemParamGetDefenceUp
#define fn_80143A28 itemParamGetAttackUp
#define fn_80143A44 itemParamGetCriticalFlag
#define fn_80143A94 itemParamGetPtr
#define fn_80143DFC itemDataBiosGetItemEffectParam
#define fn_801440A0 itemDataBiosGetPtr
extern u8* fn_801440A0(u16 idx);
extern u8   fn_80143DFC(u8* p);
extern u8*  fn_80143A94(u8 idx);
extern u8   fn_801437E0(u8* p);
extern u8   fn_80143940(u8* p);
extern u8   fn_80143918(u8* p);
extern u8   fn_801438F0(u8* p);
extern u8   fn_801438C8(u8* p);
extern u8   fn_801438A0(u8* p);
extern u8   fn_80143878(u8* p);
extern u8   fn_80143A44(u8* p);
extern u8   fn_80143A28(u8* p);
extern u8   fn_80143A0C(u8* p);
extern u8   fn_801439F0(u8* p);
extern u8   fn_801439D4(u8* p);
extern u8   fn_801439B8(u8* p);
extern u8   fn_80143990(u8* p);

/*
 * fightSeqGetItemType (0x802126C4)
 *
 * Item-type classifier: resolves the item record for itemId through the
 * item-data accessor chain (fn_801440A0 -> fn_80143DFC -> fn_80143A94),
 * then runs it through 7 chained predicate/flag checks, returning a
 * type code in {1..7}.
 */
#pragma optimize_for_size on
s32 fightSeqGetItemType(u16 itemId) {
    u8* p;

    p = fn_80143A94(fn_80143DFC(fn_801440A0(itemId)));
    if (p == NULL) {
        return 7;
    }

    if (itemId == 0x13) {
        return 1;
    }

    if (fn_801437E0(p)) {
        return 2;
    }

    if (fn_80143940(p) == 1 || fn_80143918(p) == 1 || fn_801438F0(p) == 1 ||
        fn_801438C8(p) == 1 || fn_801438A0(p) == 1 || fn_80143878(p) == 1) {
        return 3;
    }

    if (fn_80143A44(p) == 1) {
        return 4;
    }

    if (fn_80143A28(p) || fn_80143A0C(p) || fn_801439F0(p) || fn_801439D4(p) ||
        fn_801439B8(p)) {
        return 5;
    }

    return (fn_80143990(p) == 1) ? 6 : 7;
}
#pragma optimize_for_size reset
#undef fn_801437E0
#undef fn_80143878
#undef fn_801438A0
#undef fn_801438C8
#undef fn_801438F0
#undef fn_80143918
#undef fn_80143940
#undef fn_80143990
#undef fn_801439B8
#undef fn_801439D4
#undef fn_801439F0
#undef fn_80143A0C
#undef fn_80143A28
#undef fn_80143A44
#undef fn_80143A94
#undef fn_80143DFC
#undef fn_801440A0

/*
 * Fight-sequence bytecode interpreter state (0x8047B610 range, .sbss).
 *
 * lbl_8047B610 is the script instruction pointer (PC) into the battle
 * sequencer's packed bytecode stream (see colosseum_battle.c /
 * colosseum_event.c); operands are read at PC-relative byte offsets and the
 * PC is advanced by the instruction length. lbl_8047B614 is a companion u8
 * status/mode byte. The advance helpers return the pre-advance PC.
 */
extern u8* lbl_8047B610;
extern u8  lbl_8047B614;
extern u8  lbl_8047B626;

/* PC advance helpers: return old PC, then PC += n. */
u8* fn_80213A28(void) { return lbl_8047B610++; }
u8* fn_80214B58(void) { return lbl_8047B610++; }
u8* fn_8021C6F4(void) { return lbl_8047B610++; }
u8* fn_80222510(void) { return lbl_8047B610++; }

u8* fn_80222500(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 2;
    return pc;
}

u8* fn_802146B4(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

u8* fn_8021DD24(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

/* Status-byte setters: lbl_8047B614 = const. */
void fn_80213A10(void) { lbl_8047B614 = 1; }
void fn_80213A1C(void) { lbl_8047B614 = 1; }
void fn_802224DC(void) { lbl_8047B614 = 1; }
void fn_802224E8(void) { lbl_8047B614 = 1; }
void fn_802223D4(void) { lbl_8047B614 = 2; }
void fn_802224D0(void) { lbl_8047B614 = 2; }
void fn_802224F4(void) { lbl_8047B614 = 2; }

/* Advance PC by 1 and clear the lbl_8047B626 status byte; return old PC. */
u8* fn_8021B610(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B626 = 0;
    lbl_8047B610 = pc + 1;
    return pc;
}

/* More PC advance helpers. */
u8* fn_80213A38(void) { return lbl_8047B610++; }
u8* fn_80213A58(void) { return lbl_8047B610++; }

u8* fn_80213A48(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

u8* fn_80213A68(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

/* Run fn_80207448 on the passed object, then report success. */
#define fn_80207448 fightOutPokemonInitOneSelfTurn
extern void fn_80207448(void* p);
s32 fn_802136A4(void* p) {
    fn_80207448(p);
    return 1;
}
#undef fn_80207448

/* True if wazaId is one of the Encore-ineligible moves (or the 0xFFFF none id). */
s32 fightSeqIsEncoreNgWazaDataId(u16 wazaId) {
    if (wazaId == 0xA5 || wazaId == 0xE3 || wazaId == 0x77 || wazaId == 0xFFFF) {
        return 1;
    }
    return 0;
}

/*
 * Add delta to an AI evaluation value, clamping the result to [-200, 200].
 * A zero delta leaves the value unchanged.
 *
 * Byte-exact codegen needs the target branch layout from scheduling 601,
 * O2's condition propagation, and CSE disabled so the in-range paths
 * recompute `value + delta` after their bounds checks.
 */
#pragma optimize_for_size on
#pragma scheduling 601
#pragma push
#pragma optimization_level 2
#pragma opt_common_subs off
s32 fightTrainerAiAddValue(s32 value, s32 delta) {
    if (delta > 0) {
        if (value + delta > 200) {
            return 200;
        }
        value += delta;
        return value;
    } else {
        if (delta >= 0) {
            return value;
        }
        if (value + delta < -200) {
            return -200;
        }
        value += delta;
        return value;
    }
}
#pragma pop
#pragma scheduling reset
#pragma optimize_for_size reset

/* Fixed-argument dispatchers into the sequence/command emitters. */
extern void fn_802249B8();
extern void fn_802271E0(char, char);
extern void fn_802274F0(u32, char, char, char);

void fn_802247D0(void) { fn_802249B8(0, 0); }
void fn_802247F8(void) { fn_802249B8(1, 0); }

void fn_80227178(void) {
    fn_802271E0(0, 1);
    lbl_8047B610++;
}
void fn_802271AC(void) {
    fn_802271E0(1, 1);
    lbl_8047B610++;
}

void fn_80227490(void) { fn_802274F0(0, 1, 1, 0); }
void fn_802274C0(void) { fn_802274F0(1, 1, 1, 0); }

/* Emit effect 0x11 then run command 0xD9 on its result; advance PC. */
extern void* fn_801F025C();
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void fn_802266EC(void) {
    extern void* fn_801F025C();
    extern void* fn_8012640C();
    fn_8012640C(fn_801F025C(0x11, 0), 0, 0xD9, 0);
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_8012640C

/*
 * If the operand resolves (fn_802624CC == 1), set the lbl_80478D78[7] flag;
 * then advance PC (by 3 / by 5 for the two operand widths).
 */
extern u8 fn_802624CC();
extern u8 lbl_80478D78[1];
#define fn_802624CC fightMenuOpenMsg
void fn_80226284(void) {
    extern u8 fn_802624CC();
    if (fn_802624CC(*(u32*)(lbl_8047B610 + 1)) == 1) {
        lbl_80478D78[7] = 1;
    }
    lbl_8047B610 += 3;
}
#undef fn_802624CC
#define fn_802624CC fightMenuOpenMsg
void fn_802262D0(void) {
    extern u8 fn_802624CC();
    if (fn_802624CC(*(u32*)(lbl_8047B610 + 1)) == 1) {
        lbl_80478D78[7] = 1;
    }
    lbl_8047B610 += 5;
}
#undef fn_802624CC

/*
 * Sequence-callback registration wrappers: fn_801F37B0(flag, handler, arg, 0).
 * Handlers are the fightSeqWs* effect routines (some C++-linkage symbols,
 * referenced here by their mangled names).
 */
extern void fn_801F37B0();
extern u32 fn_8022E1F8();
extern s32 _fightSeqWsKuroikiriSub__FPvUsPv(void*, u16, void*);

#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_8022E1C4(void) {
    extern void fn_801F37B0();
    fn_801F37B0(0, fn_8022E1F8, 0, 0);
}
#undef fn_801F37B0

void fn_8021B830(void) {
    fn_801F37B0(0, _fightSeqWsKuroikiriSub__FPvUsPv, 0, 0);
    lbl_8047B610++;
}

/* Move-id whitelists (return 1 for a listed id, else 0). */
s32 fn_80218FDC(u16 id) {
    if (id == 0 || id == 0x165 || id == 0xD6 || id == 0x112 || id == 0x77 ||
        id == 0x76) {
        return 1;
    }
    return 0;
}
s32 fn_80219270(u16 id) {
    if (id == 0x164 || (u16)(id - 0xA5) <= 1 || id == 0xFFFF || id == 0 ||
        id == 0x165) {
        return 1;
    }
    return 0;
}

/* Emit effect, run it through a converter, store result to lbl_80478D78[3]. */
extern u8  fn_80136468();
extern u8  fn_802025B8();
extern u32 fn_80214CFC();

/* TODO(0x80214B68): 88%. Logic correct; mwcc schedules the PC load after the
 * flag store (r3 reuse) whereas the target hoists it into r4 before the store. */
void fn_80214B68(void) {
    extern int fn_801F54A4();
    lbl_80478D78[3] = fn_80136468((u16)fn_801F54A4(0, 0, 0xF, 0));
    lbl_8047B610++;
}

/* fn_80214CB0 now defined in the wave2-w0 section below (100%). */

/* If effect 0x11's state resolves to 2, set lbl_80478D78[3] = 0x75. */
void fn_8021B70C(void) {
    if ((u8)fn_802025B8(fn_801F025C(0x11, 0), 0xD) == 2) {
        lbl_80478D78[3] = 0x75;
    }
    lbl_8047B610++;
}

/* Map a condition-change action parameter id to its signed value delta. */
s32 fightSeqCondChgActParaIdToValue(u8 paraId) {
    switch (paraId) {
    case 0x10:
        return 1;
    case 0x20:
        return 2;
    case 0x90:
        return -1;
    case 0xA0:
        return -2;
    default:
        return 0;
    }
}

/* If the lbl_80478D78[7] flag is set, run the pending command then clear it. */
#define fn_801F000C fightMainWaitFrame
#define fn_8026246C fightMenuCloseMsg
extern int  fn_801F000C();
extern void fn_8026246C();
void fn_8022622C(void) {
    if (lbl_80478D78[7] != 0) {
        fn_8026246C(fn_801F000C(*(u16*)(lbl_8047B610 + 1)));
        lbl_80478D78[7] = 0;
    }
    lbl_8047B610 += 3;
}
#undef fn_801F000C
#undef fn_8026246C

/* Fixed-arg dispatch forwarding the caller's argument as the 4th parameter. */
extern void fn_801F2598();
#define fn_801F2598 fightFloorGetFightOutPokemonPtrRandom
void fn_8022B29C(s32 arg) {
    extern void fn_801F2598();
    fn_801F2598(0, 1, 3, arg);
}
#undef fn_801F2598

/*
 * Map a condition-change action type to its Pokemon status id (jump table).
 * TODO(0x8023A700): 99.4% -- code is byte-identical; the only diff is the
 * compiler-generated jump-table label (@NNN vs jumptable_8039A068). Reaching
 * 100% needs a symbol/splits entry naming the table, deferred per campaign rules.
 */
s32 fightSeqCondChgActTypeToPokemonStatusId(u8 type) {
    switch (type) {
    case 0:
        return 0xE6;
    case 1:
        return 0xE7;
    case 2:
        return 0xEA;
    case 3:
        return 0xE8;
    case 4:
        return 0xE9;
    case 5:
        return 0xEB;
    case 6:
        return 0xEC;
    case 7:
    default:
        return 0;
    }
}

/* If effect 0x19 state check returns 0, set a bit in the lbl_80379F58 table. */
extern u8 lbl_80379F58[];
#define fn_801F025C fightTargetGetPtrAsNowFightType
void fn_80214AFC(void) {
    extern u32 fn_801F025C();
    extern u32 fn_802026E4(u32, u16);
    if ((u8)fn_802026E4((u32)fn_801F025C(0x19, 0), 0x14) == 0) {
        lbl_80379F58[0x1609B] |= 0x80;
    }
    lbl_8047B610++;
}
#undef fn_801F025C

/* Rate param's threat: 100 if it passes either check, else its stat-6 value.
 * (Placed before fn_80229BD8's definition: this caller was byte-verified
 * against u8-returning K&R decls of the two predicates.) */
/* Address: 0x802393A0 | Size: 0x9C */
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8011BEB4 wazaGetStatus
u8 fn_802393A0(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u8 fn_80229B70();
    extern u8 fn_80229BD8();
    extern u32 fn_8011BEB4();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if (fn_80229B70(param) == 1) {
        return 100;
    }
    if (fn_80229BD8(param) == 1) {
        return 100;
    }
    return (u8)fn_8011BEB4(0, param, 0x6, 0);
}
#undef fn_801FB1C0
#undef fn_8011BEB4

/* True if the resolved id is one of {0x11, 0x4E}. */
#define fn_8011BEB4 wazaGetStatus
s32 fn_80229BD8(s32 arg) {
    extern int fn_8011BEB4();
    u16 r = (u16)fn_8011BEB4(0, arg, 9, 0);
    if (r == 0x11 || r == 0x4E) {
        return 1;
    }
    return 0;
}
#undef fn_8011BEB4

/* Combine the results of effects 0x11 and 0x12. */
extern void fn_80201600();
void fn_802173D4(void) {
    void* a = fn_801F025C(0x11, 0);
    void* b = fn_801F025C(0x12, 0);
    fn_80201600(a, b);
    lbl_8047B610 += 5;
}

/* AI setup: seed two fn_801FB1C0 slots, then dispatch fn_8011BAC0(arg2, arg3). */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8011BAC0 wazaIsWazaTypeId
u8 fn_8023943C(int arg1, int arg2, int arg3) {
    extern int  fn_801FB1C0();
    extern u8 fn_8011BAC0();
    u16 r = (u16)fn_801FB1C0(arg1, 0, 0x43, 0);
    fn_801FB1C0(0, r, 2, 0);
    return fn_8011BAC0(arg2, arg3);
}
#undef fn_801FB1C0
#undef fn_8011BAC0
#pragma optimize_for_size reset

/* ===== 0x8021DDxx bytecode ops (integrated from batch-2 worker) ===== */
extern void* lbl_8047B64C;
extern u8    lbl_80478278[0x10];
extern void  fn_801DA36C(void* obj, u32 val);
extern void  fn_80209484(void* ctx, u32 param);
extern u16   fn_8020147C(void* context, u16 moveId, u8 slot, u8 updateFlag);
extern void  fn_801FE468(void* context, u8* dest);
extern void  fightMenuSubMenuLvupStatus(void* a, void* b, void* c);
extern void  fightMenuOpenLevelUpStatusMenu(void* a, u32 flag);
extern void  fn_802622E4(void);

#pragma optimize_for_size on

/* No-op bytecode ops: advance PC by 1. */
void fn_8021DD34(void) { lbl_8047B610++; }
void fn_8021DDB8(void) { lbl_8047B610++; }
void fn_8021DDC8(void) { lbl_8047B610++; }
void fn_8021DE3C(void) { lbl_8047B610++; }

/* Resolve trainer slot from PC[1]; mark item state and apply effect 3. PC += 2. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80209484 fightOutPokemonSetVisibility
#define fn_8012640C pokemonGetStatus
void fn_8021DD44(void) {
    extern void* fn_801F025C();
    extern void fn_80209484();
    extern void* fn_8012640C();
    void* obj = (void*)fn_801F025C(lbl_8047B610[1], 0);
    if (obj != NULL) {
        void* p;
        fn_80209484(obj, 1);
        p = fn_8012640C(obj, 0, 0xee, 0);
        if (p != NULL) {
            fn_801DA36C(p, 3);
        }
    }
    lbl_8047B610 += 2;
}
#undef fn_801F025C
#undef fn_80209484
#undef fn_8012640C

/* Build and show the level-up-status sub-menus from context + template. PC += 1. */
#define fn_801FE468 fightPokemonToMenuLvupStatus
#define fn_802622E4 fightMenuCloseLevelUpStatusMenu
void fn_8021DDD8(void) {
    extern void fn_801FE468();
    extern void fn_802622E4();
    u8 buf1[24];
    u8 buf2[16];

    fn_801FE468(lbl_8047B64C, buf1);
    fightMenuSubMenuLvupStatus(buf1, lbl_80478278, buf2);
    fightMenuOpenLevelUpStatusMenu(buf2, 1);
    fightMenuOpenLevelUpStatusMenu(buf1, 0);
    fn_802622E4();

    lbl_8047B610 += 1;
}
#undef fn_802622E4
#undef fn_801FE468

/* Resolve trainer slot from PC[1], refresh move slot 0; if story state 1, set
 * event 0x82. PC += 2. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8020147C fightOutPokemonDoItemSoubi
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_8021DE4C(void) {
    extern int fn_801F025C();
    extern int fn_8020147C();
    extern u8   fn_801FECD4(void* trainer);
    extern void fn_801FE7EC(void* trainer, u32 eventId, u32 param1, u32 param2);
    void* ctx = (void*)fn_801F025C(lbl_8047B610[1], 0);
    fn_8020147C(ctx, 0, 0, 1);
    if (fn_801FECD4(ctx) == 1) {
        fn_801FE7EC(ctx, 0x82, 0, 0);
    }
    lbl_8047B610 += 2;
}
#undef fn_801F025C
#undef fn_8020147C
#undef fn_801FECD4
#undef fn_801FE7EC

#pragma optimize_for_size reset

/* ===== AI-handler resolver + battle-message triggers (integrated from batch-7 worker) ===== */

/* Resolve an AI-handler function pointer for param2 and invoke it; falls back
 * to the stub fn_8024E52C when no handler is registered. */
#pragma optimize_for_size on
#pragma dont_inline on
#define fn_8011BEB4 wazaGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8024E52C fightTrainerAiWazaValueNull
s32 fn_8023CA9C(void* ctx, u32 param1, u32 param2, u32 param3) {
    typedef s32 (*Handler)();
    extern Handler fn_8011BEB4(u32, u32, u32, u32);
    extern u32 fn_801FB1C0(void*, u32, u32, u32);
    extern s32 fn_8024E52C();
    Handler fp;
    u16 v;
    u8 v2;

    v = (u16)fn_801FB1C0(ctx, 0, 0x43, 0);
    v = (u16)fn_801FB1C0(0, v, 2, 0);
    v2 = (u8)fn_801FB1C0(0, v, 0x32, 0);
    if (v2 != 1) {
        goto ret0;
    }
    fp = fn_8011BEB4(0, param2, 0x1c, 0);
    if (fp != NULL) {
        goto call;
    }
    fp = fn_8024E52C;
    goto call;
ret0:
    return 0;
call:
    return fp(ctx, param1, param2, param3);
}
#undef fn_8024E52C
#undef fn_801FB1C0
#undef fn_8011BEB4
#pragma dont_inline reset
#pragma optimize_for_size reset

/* Dispatch to fn_8023CA9C's handler, skipping when the resolved value ties
 * param2 or is 0. */
#pragma optimize_for_size on
u32 fn_802400D8(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_8025CB3C(void*, u32, u32);
    u32 result = fn_8025CB3C(ctx, param1, param2);
    if ((u16)result == (u16)param2) {
        goto ret0;
    }
    if ((u16)result != 0) {
        return fn_8023CA9C(ctx, param1, result, param3);
    }
ret0:
    return 0;
}
#pragma optimize_for_size reset

/* Battle-message trigger, seq 0x214. */
#pragma optimize_for_size on
s32 fn_8023D480(void* ctx, u32 param1, u32 param2) {
    extern u8 fn_80235714(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 handle = 0;

    if ((u8)fn_80235714(ctx, param1) == 1) {
        handle = fn_80239984(0, ctx, 0x214);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x214);
    }
    return handle;
}
#pragma optimize_for_size reset

/* Battle-message trigger, seq 0x220. */
#pragma optimize_for_size on
s32 fn_8023CDCC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_802377E8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    s32 handle = 0;

    if (fn_802377E8(ctx, param3) == 0xd5) {
        handle = fn_80239984(0, ctx, 0x220);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x220);
    }
    return handle;
}
#pragma optimize_for_size reset

/*
 * fn_80211A00 (0x80211A00): module init -- registers the fn_80211A78 and
 * fn_8022FE20 sequence handlers, clears the working flags, and resets state.
 */
extern u32 fn_80211A78();
u8 fn_8022FE20(void* ctx);
extern void fn_801DA7AC();
extern u8   lbl_8047B625;
/* TODO(0x80211A00): 76%. All instructions present and store order correct; the
 * target schedules the lbl_80379F58 base setup (lis/addi/addis) before the
 * first flag store, mwcc here computes it lazily (address-gen scheduling). */
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_80211A00(void) {
    fn_801F37B0(0, fn_80211A78, 0, 1);
    fn_801F37B0(0, fn_8022FE20, 0, 1);
    lbl_80478D78[3] = 0;
    lbl_8047B625 = 0;
    lbl_80478D78[4] = 0;
    lbl_80379F58[0x16002] = 0;
    lbl_80379F58[0x160A1] = 0;
    fn_801DA7AC();
}
#undef fn_801F37B0

/* Effect-check handlers: if effect 0x11 state == 2, apply action for id. */
extern void fn_8020248C();
void fn_80216550(void) {
    void* obj = fn_801F025C(0x11, 0);
    if ((u8)fn_802025B8(obj, 0x24) == 2) {
        fn_8020248C(obj, 0x24, 0);
    }
    lbl_8047B610++;
}
void fn_802171BC(void) {
    void* obj = fn_801F025C(0x11, 0);
    if ((u8)fn_802025B8(obj, 0x1A) == 2) {
        fn_8020248C(obj, 0x1A, 0);
    }
    lbl_8047B610++;
}

/* ===== 0x80222xxx bytecode-op cluster (integrated from batch-2 worker) ===== */
extern void* lbl_8047B62C;
extern void  fn_80211B94(void* a, void* b, u8 c);
extern u8    fn_80207AE0(void* obj, u8 v);
extern void* fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(u32 a, u8 b, u32 c, u32 d, u32 e);
extern void  fn_801254B4(void* a, u32 b, u32 c, u32 d, u32 e);

/* *dst |= imm (32/16/8-bit); advance PC past opcode+ptr+imm. */
void fn_802225DC(void) {
    u32* dst = *(u32**)(lbl_8047B610 + 1);
    *dst |= *(u32*)(lbl_8047B610 + 5);
    lbl_8047B610 += 9;
}
void fn_80222604(void) {
    u16* dst = *(u16**)(lbl_8047B610 + 1);
    *dst |= *(u16*)(lbl_8047B610 + 5);
    lbl_8047B610 += 7;
}
void fn_8022262C(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst |= *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}

/* *dst &= ~imm (16/8-bit). The explicit ^ mask defeats the andc peephole. */
void fn_80222584(void) {
    u16* dst = *(u16**)(lbl_8047B610 + 1);
    *dst &= *(u16*)(lbl_8047B610 + 5) ^ 0xFFFF;
    lbl_8047B610 += 7;
}
void fn_802225B0(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst &= *(u8*)(lbl_8047B610 + 5) ^ 0xFF;
    lbl_8047B610 += 6;
}

/* *dst -/+/= imm8. */
void fn_802226EC(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst -= *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}
void fn_80222714(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst += *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}
void fn_8022273C(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst = *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}

/* fn_801F000C(imm16); PC += 3. */
#define fn_801F000C fightMainWaitFrame
void fn_80222520(void) {
    extern void fn_801F000C();
    fn_801F000C(*(u16*)(lbl_8047B610 + 1));
    lbl_8047B610 += 3;
}
#undef fn_801F000C

/* fn_80211B94(ctx, ptr, 0); PC += 5. */
void fn_80222494(void) {
    fn_80211B94(lbl_8047B62C, *(void**)(lbl_8047B610 + 1), 0);
    lbl_8047B610 += 5;
}

/* Conditional jumps: test a resolved value, branch to PC-embedded target. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
void fn_80222438(void) {
    extern void* fn_801F025C();
    extern u8 fn_80207AE0();
    void* obj = (void*)fn_801F025C(lbl_8047B610[1], 0);
    if (fn_80207AE0(obj, lbl_8047B610[2]) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 3);
    } else {
        lbl_8047B610 += 7;
    }
}
#undef fn_80207AE0
#undef fn_801F025C
void fn_802223E0(void) {
    if (fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(0, lbl_8047B610[1], 0, 0, 0) != NULL) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 2);
    } else {
        lbl_8047B610 += 6;
    }
}

/* Classify (a,b) into an item/effect code; b selects a flag-resolved code pair. */
u8 fn_80222110(u8 a, u8 b) {
    u8 flag;
    u8 r = a;

    if (a == 0xf || a == 0x27) {
        flag = 1;
    } else if (a == 0x16 || a == 0x2e) {
        flag = 0;
    }

    if (b == 1 || b == 4) {
        if (flag == 1) {
            r = 0x26;
        } else {
            r = 0x27;
        }
    }
    if (b == 2 || b == 5 || b == 7) {
        if (flag == 1) {
            r = 0x28;
        } else {
            r = 0x29;
        }
    }
    if (b == 3) {
        if (flag == 1) {
            r = 0x2a;
        } else {
            r = 0x2b;
        }
    }
    if (b == 6) {
        if (flag == 1) {
            r = 0x2c;
        } else {
            r = 0x3c;
        }
    }
    return r;
}

/* Misc single-call bytecode ops. */
void fn_8021DEC8(void) {
    fn_802271E0(1, 0);
    lbl_8047B610 += 1;
}
#define fn_801254B4 pokemonSetStatus
s32 fn_8021DF3C(void* arg) {
    extern u32 fn_801254B4();
    fn_801254B4(arg, 0, 0x112, 0, 1);
    return 1;
}
#undef fn_801254B4
void fn_8021DEFC(void) {
    fn_801F37B0(0, fn_8021DF3C, 0, 0);
    lbl_8047B610 += 1;
}
void fn_8021DF70(void) { lbl_8047B610++; }

/* ===== battle-message-trigger family (integrated from batch-7 worker) =====
 * Each fires one or more sequence messages (fn_80239984 handle + fn_80239EE8
 * emit) gated on trainer/move state. handle accumulates across blocks. */
extern u32  fn_80239984();
extern u32  fn_80205B8C(u32);
extern u8 fn_80239EE8();
extern u8 fn_80239CCC();
extern u16  fn_802377E8(void*, u32);
extern u16  fn_801F1990(u32, void*, u32, u32, u32, u32);
extern u16  fn_801F1C18(u32, void*, void*, u32, u32);

#pragma optimize_for_size on
u32 fn_8023F044(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_8023831C(void*);
    extern u8  fn_80237F74(void*, u32, u32);
    u32 handle = 0;
    u16 v = fn_8023831C(ctx);

    if (v == 0x1d || v == 0x18) {
        handle = fn_80239984(0, ctx, 0x1e2);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e2);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x3c) == 1) {
        handle = fn_80239984(handle, ctx, 0x1e3);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e3);
    }
    return handle;
}

u32 fn_8023F144(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74(void*, u32, u32);
    u32 handle = 0;

    if ((u8)fn_80237F74(ctx, param1, 0x36) == 1 ||
        (u8)fn_80237F74(ctx, param1, 0x10) == 1) {
        handle = fn_80239984(0, ctx, 0x1e0);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e0);
    }
    if ((u8)fn_80237F74(ctx, param3, 0x36) == 1 ||
        (u8)fn_80237F74(ctx, param3, 0x10) == 1) {
        handle = fn_80239984(handle, ctx, 0x1e1);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e1);
    }
    return handle;
}

u32 fn_8023CCA0(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_801F6E98(void*, u32);
    u32 handle = 0;
    void* obj = fn_801F025C(2, param3);

    if (fn_801F6E98(obj, 0x49) == 1 || fn_801F6E98(obj, 0x48) == 1) {
        handle = fn_80239984(0, ctx, 0x221);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x221);
    }
    if ((u8)fn_801F1990(0, ctx, 1, 1, 0x10e, param1) == 1) {
        handle = fn_80239984(handle, ctx, 0x222);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x222);
    }
    return handle;
}

u32 fn_8023CE60(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_80236FFC(void*, u32);
    extern u16 fn_8023715C(void*, u32);
    extern u16 fn_80236520(void*, u32);
    extern u8  fn_8023C530(void*, u32, u32, u32);
    u32 handle = 0;
    u16 a = fn_80236FFC(ctx, param3);
    u16 b = fn_8023715C(ctx, param3);
    u16 c = fn_80236520(ctx, param3);

    if ((u8)fn_8023C530(ctx, param1, param2, param3) == 1) {
        handle = fn_80239984(0, ctx, 0x21d);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x21d);
    }
    if (b < a) {
        handle = fn_80239984(handle, ctx, 0x21e);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x21e);
    }
    if (c == 0x118) {
        handle = fn_80239984(handle, ctx, 0x21f);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x21f);
    }
    return handle;
}

u32 fn_8023CFDC(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_80236FFC(void*, u32);
    extern u16 fn_8023715C(void*, u32);
    extern u16 fn_80236520(void*, u32);
    extern u8  fn_8023C530(void*, u32, u32, u32);
    u32 handle = 0;
    u16 a = fn_80236FFC(ctx, param3);
    u16 b = fn_8023715C(ctx, param3);
    u16 c = fn_80236520(ctx, param3);

    if ((u8)fn_8023C530(ctx, param1, param2, param3) == 1) {
        handle = fn_80239984(0, ctx, 0x21a);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x21a);
    }
    if (b > a) {
        handle = fn_80239984(handle, ctx, 0x21b);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x21b);
    }
    if (c == 0x118) {
        handle = fn_80239984(handle, ctx, 0x21c);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x21c);
    }
    return handle;
}

u32 fn_8023F8C0(void* ctx, u32 param1, u32 param2) {
    extern s16 fn_80202360(u32, u32);
    extern u8  fn_80236BFC(void*, u32, u32);
    s16 val;
    u32 handle = 0;

    if ((u8)fn_80236BFC(ctx, param1, 0x2d) == 1) {
        val = fn_80202360(param1, 0x2d);
    } else {
        val = 0;
    }
    if (val == 0) {
        handle = fn_80239984(0, ctx, 0x1d4);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d4);
    } else if (val == 1) {
        handle = fn_80239984(0, ctx, 0x1d5);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d5);
    } else if (val == 2) {
        handle = fn_80239984(0, ctx, 0x1d6);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d6);
    }
    return handle;
}

u32 fn_80240454(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8  fn_801F6E98(void*, u32);
    extern s16 fn_801F6D9C(void*, u32);
    u32 handle = 0;
    void* obj = fn_801F025C(2, param3);
    s16 val;

    if (fn_801F6E98(obj, 0x4a) == 1) {
        val = fn_801F6D9C(obj, 0x4a);
    } else {
        val = 0;
    }
    if (val == 0) {
        handle = fn_80239984(0, ctx, 0x1c4);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c4);
    } else if (val == 1) {
        handle = fn_80239984(0, ctx, 0x1c5);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c5);
    } else if (val == 2) {
        handle = fn_80239984(0, ctx, 0x1c6);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c6);
    }
    return handle;
}

u32 fn_8023D324(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_80236520(void*, u32);
    u32 arr[10];
    u32 handle = 0;
    u16 count;
    u16 c;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 0, 1);
    c = fn_80236520(ctx, param3);
    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, arr[i]);
        if (v == 0xca || v == 0x168 || v == 0x12f || v == 0xd5) {
            handle = fn_80239984(0, ctx, 0x215);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x215);
            break;
        }
    }
    if (c == 0x11f) {
        handle = fn_80239984(handle, ctx, 0x216);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x216);
    }
    return handle;
}

u32 fn_8023FA1C(void* ctx, u32 param1, u32 param2) {
    extern u16 fn_802367CC(void*, u32, void*, u32, u32);
    extern u16 fn_802395C8(void*, u32, u32);
    extern u8  fn_80236BFC(void*, u32, u32);
    u16 buf[10];
    u32 handle = 0;
    u16 count;
    u16 i;

    count = fn_802367CC(ctx, param1, buf, 0, 1);
    for (i = 0; i < count; i++) {
        if (buf[i] == 0x10c) {
            continue;
        }
        if (fn_802395C8(ctx, buf[i], param1) == 0xd) {
            handle = fn_80239984(0, ctx, 0x1d2);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d2);
            break;
        }
    }
    if ((u8)fn_80236BFC(ctx, param1, 0x24) == 1) {
        handle = fn_80239984(handle, ctx, 0x1d3);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d3);
    }
    return handle;
}
#pragma optimize_for_size reset

/* ===== 0x80223-0x80229 conditional jumps / dispatchers (batch-3 worker) ===== */
extern u8   fn_801F4C14(u32, u16, u32, u16, u32);
extern u32  fn_802096E8(void*);
extern u32  fn_80077AF4(void);

/* Conditional bytecode jump: if fn_802026E4(ctx, op32) low byte == pc[6],
 * jump to the target at pc+7, else PC += 0xB. */
#pragma optimize_for_size on
void fn_80223A88(void) {
    extern u32 fn_802026E4(u32, u16);
    u8* pc = lbl_8047B610;
    void* ctx = fn_801F025C(pc[1], 0);
    u32 val;
    u8* target;
    u32 flag;

    pc = lbl_8047B610;
    val = *(u32*)(pc + 2);
    target = *(u8**)(pc + 7);
    flag = fn_802026E4((u32)ctx, val);
    if (lbl_8047B610[6] == (u8)flag) {
        lbl_8047B610 = target;
    } else {
        lbl_8047B610 = lbl_8047B610 + 0xb;
    }
}
#pragma optimize_for_size reset

/* Conditional jump w/ side effect: if fn_80207BF4(ctx) == pc[2], run
 * fn_801F4C14 then jump to pc+3, else PC += 7. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F4C14 fightFloorSetStatus
void fn_80223CE8(void) {
    extern void* fn_801F025C();
    extern u16 fn_80207BF4(void*);
    extern void fn_801F4C14();
    u8* pc = lbl_8047B610;
    void* ctx = fn_801F025C(pc[1], 0);
    u16 ret = fn_80207BF4(ctx);
    u8* target;

    pc = lbl_8047B610;
    target = *(u8**)(pc + 3);
    if (ret == pc[2]) {
        fn_801F4C14(0, 0, 0x48, 0, (u32)ctx);
        lbl_8047B610 = target;
    } else {
        lbl_8047B610 = lbl_8047B610 + 7;
    }
}
#undef fn_801F025C
#undef fn_80207BF4
#undef fn_801F4C14
#pragma optimize_for_size reset

/* Pointer-array lookup indexed by lbl_80478D78[5] (or a default), fed to
 * fn_802624CC; set lbl_80478D78[7] on success. PC += 5. */
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
#undef fn_802624CC
#undef fn_801F54A4
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
#undef fn_802624CC
#undef fn_801F54A4

/* Resolve a context, gate on two predicates, post message 0x7631. PC += 1. */
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
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_802096E8
#undef fn_801F4C14
#undef fn_802624CC

/* Tri-condition predicate. */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_8011BEB4 wazaGetStatus
u8 fn_8022967C(u32 param2) {
    extern u16 fn_8011BEB4();
    extern int fn_801F54A4();
    u8 a = (u8)fn_801F54A4(0, 0, 0x34, 0);
    u32 b = fn_80077AF4();
    u16 c = (u16)fn_8011BEB4(0, param2, 9, 0);

    if (a == 1 && (u8)b == 1 && c == 7) {
        return 1;
    }
    return 0;
}
#undef fn_8011BEB4
#undef fn_801F54A4
#pragma optimize_for_size reset

/* ===== 0x80217-0x8021D bytecode ops (integrated from batch-1 worker) ===== */
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

/* Resolve slot-0x12 ctx; if state 0x19 == 2, reset it. PC += 1. */
void fn_80218270(void) {
    u32 ctx = (u32)fn_801F025C(0x12, 0);
    if (fn_802025B8(ctx, 0x19) == 2) {
        fn_8020248C(ctx, 0x19, 0);
    }
    lbl_8047B610++;
}

/* Read a status byte from the big table, split nibbles, forward to fn_8021B910
 * with the instruction operands; PC += 6 only if not handled. */
void fn_8021B8B8(void) {
    u8 val = *(u8*)((u8*)lbl_80379F58 + (0x1 << 16) + 0x601e);
    if (fn_8021B910(val & 0xF0, val & 0x0F, lbl_8047B610[1],
                    *(u32*)(lbl_8047B610 + 2)) == 0) {
        lbl_8047B610 += 6;
    }
}

/* Slot-0x11 field 0xED: 0 -> absolute jump to operand, else skip (PC += 5). */
void fn_8021C704(void) {
    extern void* fn_8012640C();
    u32 ctx = (u32)fn_801F025C(0x11, 0);
    if ((u16)(u32)fn_8012640C(ctx, 0, 0xed, 0) != 0) {
        lbl_8047B610 += 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}

/* Gauge-segment lookup: pct = a*0x30/b (>=1 if a!=0), bucketed through the
 * 6-entry {threshold, value} table lbl_80279F7C. */
#pragma optimize_for_size on
u8 fn_80218B6C(u16 a, u16 b) {
    u8 key = (u8)((a * 0x30) / b);
    s32 offset;
    s32 i;

    if (key == 0 && a != 0) {
        key = 1;
    }
    offset = 0;
    for (i = 6; i > 0; i--) {
        if (key <= lbl_80279F7C[offset]) {
            break;
        }
        offset += 2;
    }
    return lbl_80279F7C[offset + 1];
}
#pragma optimize_for_size reset

/* Predicate: is field 0x9 of elem one of six "special" ids? */
#define fn_8011BEB4 wazaGetStatus
u8 fn_8021901C(u32 elem) {
    extern u16 fn_8011BEB4();
    u16 val = (u16)fn_8011BEB4(0, elem, 0x9, 0);
    if (val == 0x91 || val == 0x27 || val == 0x4b || val == 0x97 ||
        val == 0x9b || val == 0x1a) {
        return 1;
    } else {
        return 0;
    }
}
#undef fn_8011BEB4

/* Hand the party count to fn_802653FC on the resolved ctx. PC += 2. */
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802653FC fightMenuFightOutPokemonRenewStatusMenu
void fn_8021A80C(void) {
    extern int fn_801F54A4();
    extern int fn_801F025C();
    extern void fn_802653FC(void*, u16, s32);
    u16 count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    u8 op = lbl_8047B610[1];
    void* ctx = (void*)fn_801F025C(op, 0);
    if (ctx != 0) {
        fn_802653FC(ctx, count, 1);
    }
    lbl_8047B610 += 2;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_802653FC

/* Forward ctx's flag byte into fn_8011BBD8 field 0x2d. PC += 1. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80203E0C figthOutPokemonGetLevel
#define fn_8011BBD8 wazaSetStatus
void fn_80219D98(void) {
    extern int fn_801F025C();
    extern void* fn_8012640C();
    extern u8 fn_80203E0C(void*);
    void* ctx = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx, 0, 0xd9, 0);
    fn_8011BBD8(resolved, 0, 0x2d, 0, fn_80203E0C(ctx));
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80203E0C
#undef fn_8011BBD8

/* Forward fn_80203ADC(ctx2, 2) into fn_8011BBD8 field 0x2d. PC += 1. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80203ADC fightOutPokemonNowHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_8021AB9C(void) {
    extern u32 fn_801F025C();
    extern void* fn_8012640C();
    extern u32 fn_80203ADC();
    extern void fn_8011BBD8();
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx1, 0, 0xd9, 0);
    void* ctx2 = (void*)fn_801F025C(0x12, 0);
    u16 val = (u16)fn_80203ADC(ctx2, 2);
    fn_8011BBD8(resolved, 0, 0x2d, 0, val);
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80203ADC
#undef fn_8011BBD8

/* Set trainer-pokemon field 0x83, then clear event-state 0x83 if set. PC += 1. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_801254B4 pokemonSetStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_8021D010(void) {
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
    extern u32 fn_801254B4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    fn_801254B4((void*)fn_80205B8C((u32)ctx), 0, 0x83, 0, 0);
    if (fn_801FECD4(ctx) == 1) {
        fn_801FE7EC(ctx, 0x83, 0, 0);
    }
    lbl_8047B610++;
}
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_801254B4
#undef fn_80205B8C
#undef fn_801F025C

/* State 0xF == 2: clear + stash 0; else fire message 0x45 + stash 1. PC += 1. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
void fn_8021A6CC(void) {
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    if (fn_802025B8(ctx, 0xf) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x45);
        lbl_80478D78[5] = 1;
    } else {
        fn_8020248C(ctx, 0xf, 0);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610++;
}
#undef fn_801F4C14
#undef fn_801F025C

/* Global state 0x55: 2 -> run + stash 3; else message 0x40 + stash 2. PC += 1. */
void fn_8021AB18(void) {
    if (fn_801F2988(0, 0x55) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x55, 0);
        lbl_80478D78[5] = 3;
    }
    lbl_8047B610++;
}

/* Same shape as fn_8021AB18 for state 0x54, stash values swapped. PC += 1. */
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
void fn_8021CC5C(void) {
    extern u8 fn_801F2988(u32, u32);
    extern void fn_801F4C14();
    extern void fn_801F2934(u32, u32, u32);
    if (fn_801F2988(0, 0x54) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x54, 0);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610++;
}
#undef fn_801F2988
#undef fn_801F4C14
#undef fn_801F2934

/* Look up party slot `count` in ctx1; if ctx2 state 0x1d == 2, write it back. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
void fn_802192B4(void) {
    extern int fn_801F025C();
    extern int fn_801F54A4();
    extern void* fn_801F0134();
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    void* ctx2;
    u16 count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    void* val = fn_801F0134(ctx1, count);
    ctx2 = (void*)fn_801F025C(0x12, 0);
    if (fn_802025B8(ctx2, 0x1d) == 2) {
        fn_8020248C(ctx2, 0x1d, val);
    }
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_801F54A4
#undef fn_801F0134
#pragma optimize_for_size reset

/* Negate half of field 0x2e (>= -1), write to field 0x2d. PC += 1. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_8021C0F4(void) {
    extern u32 fn_801F025C();
    extern void* fn_8012640C();
    extern s32 fn_8011BEB4();
    extern void fn_8011BBD8();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx, 0, 0xd9, 0);
    s32 v2;
    s32 neg;

    fn_8011BEB4(resolved, 0, 0x2d, 0);
    v2 = fn_8011BEB4(resolved, 0, 0x2e, 0);
    neg = -(v2 / 2);
    if (neg == 0) {
        neg = -1;
    }
    fn_8011BBD8(resolved, 0, 0x2d, 0, neg);
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_8011BBD8
#pragma optimize_for_size reset

/* Slot-0x11 field 0x2 -> fn_801F6E44(id 0x4b): !=2 message 0x40 stash 0; else
 * run + stash 5. PC += 1. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
void fn_802178F4(void) {
    extern void* fn_801F025C();
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    u32 a = (u32)fn_801F025C(0x2, ctx1);
    if (fn_801F6E44(a, 0x4b) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 0;
    } else {
        fn_801F6DF0(a, 0x4b, 0);
        lbl_80478D78[5] = 5;
    }
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_801F6E44
#undef fn_801F4C14
#undef fn_801F6DF0

/* ctx2 state 0x18 == 2: clear + write fn_80203B5C(ctx,2) to field 0x2d, PC += 5;
 * else absolute jump to operand. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_802183BC(void) {
    extern void* fn_801F025C();
    extern u16 fn_80203B5C();
    extern void fn_8011BBD8();
    extern void* fn_8012640C();
    void* ctx = (void*)fn_801F025C(0x11, 0);
    u32 ctx2;
    u32 resolved = (u32)fn_8012640C(ctx, 0, 0xd9, 0);
    ctx2 = (u32)fn_801F025C(0x12, 0);
    if (fn_802025B8(ctx2, 0x18) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx2, 0x18, 0);
        fn_8011BBD8(resolved, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 2));
        lbl_8047B610 += 5;
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80203B5C
#undef fn_8011BBD8
#pragma optimize_for_size reset

/* fn_80229934(temp1, ctx1, ctx2) == 1 -> jump; else clear ctx1 state 0x15,
 * PC += 5. */
#pragma optimize_for_size on
void fn_80218C74(void) {
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    u32 temp1 = fn_80205184(ctx1);
    void* ctx2 = (void*)fn_801F025C(0x12, 0);
    if (fn_80229934(temp1, (u32)ctx1, (u32)ctx2) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        if (fn_802025B8(ctx1, 0x15) == 2) {
            fn_8020248C(ctx1, 0x15, 0);
        }
        lbl_8047B610 += 5;
    }
}
#pragma optimize_for_size reset

/* Scale ctx's flag byte by a randomized 50-149% factor, write field 0x2d. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80203E0C figthOutPokemonGetLevel
#define fn_8011BBD8 wazaSetStatus
void fn_80219CF4(void) {
    extern int fn_801F025C();
    extern void* fn_8012640C();
    extern u8 fn_80203E0C(void*);
    void* ctx = (void*)fn_801F025C(0x11, 0);
    void* resolved = fn_8012640C(ctx, 0, 0xd9, 0);
    u32 val1 = fn_80203E0C(ctx);
    u16 rng = fn_800E0C54();
    s32 result = (s32)(val1 * ((rng % 11) * 10 + 50)) / 100;
    fn_8011BBD8(resolved, 0, 0x2d, 0, result);
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80203E0C
#undef fn_8011BBD8
#pragma optimize_for_size reset

/* Slot-0x11 field 0x2 -> fn_801F6E44(id 0x4c): !=2 message 0x45 stash 1; else
 * run + stash 0. PC += 1. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
void fn_8021A764(void) {
    extern void* fn_801F025C();
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    void* ctx1 = (void*)fn_801F025C(0x11, 0);
    u32 a = (u32)fn_801F025C(0x2, ctx1);
    if (fn_801F6E44(a, 0x4c) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x45);
        lbl_80478D78[5] = 1;
    } else {
        fn_801F6DF0(a, 0x4c, 0);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610++;
}
#undef fn_801F025C
#undef fn_801F6E44
#undef fn_801F4C14
#undef fn_801F6DF0

/* AI value-add with a random modulo scale, seq 0x1cb. Local declaration order
 * (handle, scale, mod, rng) is load-bearing: it colors handle/mod into the
 * r27-r31 frame the target uses. */
#pragma optimize_for_size on
s32 fn_80240144(void* ctx, u32 param1, u32 param2) {
    extern s32 fn_801FB1C0(u32, u32, u32, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    s32 handle;
    s32 scale;
    s32 mod;
    u16 rng;

    scale = fn_801FB1C0(0, 0x1cb, 0x3e, 0);
    rng = fn_800E0C54();
    mod = rng % (scale + 1);
    handle = fightTrainerAiAddValue(0, mod);
    fn_80239CCC(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1cb, mod);
    return handle;
}
#pragma optimize_for_size reset

/* ===== 0x8022FE20-0x80235AA0 handlers (integrated from wave2-w5 worker) ===== */
extern u16  fn_80205224();
extern void fn_80202810();
extern u8   fn_802062FC();
extern u8   lbl_80378721[];
extern u8   lbl_80379249[];

/* "0x13-field" sequence handler: clear field 0x13 unless status is 0x63. */
#define fn_80205224 fightOutPokemonGetMotoWazaDataId
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
u8 fn_8022FE20(void* ctx) {
    extern u16 fn_80205224();
    extern void fn_80202810();
    extern u8 fn_802026E4();
    if ((u8)fn_802026E4(ctx, 0x13) == 1) {
        if ((u16)fn_80205224(ctx) != 0x63) {
            fn_80202810(ctx, 0x13);
        }
    }
    return 1;
}
#undef fn_80202810
#undef fn_80205224

/* Report whether the 0xEE record is absent; stash the owner when present. */
#pragma optimize_for_size on
#define fn_8012640C pokemonGetStatus
s32 fn_80231FC8(void* param1, s32 param2, void** param3) {
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
#pragma optimize_for_size reset

/* Clear the trainer event if set. */
#pragma optimize_for_size on
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_802331A4(void* param1, void* param2) {
    extern u8   fn_801FECD4();
    extern void fn_801FE7EC();
    if ((u8)fn_801FECD4(param1, param2) == 1) {
        fn_801FE7EC(param1, param2, 0, 0);
    }
}
#undef fn_801FECD4
#undef fn_801FE7EC
#pragma optimize_for_size reset

/*
 * fn_802358AC family: species-id -> slot resolve (fn_801FB1C0 pair), then
 * read field K of param2 (0xEB..0xE6, one per copy).
 */
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8012640C pokemonGetStatus
u8 fn_802358AC(void* param1, void* param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xeb, 0);
}
u8 fn_80235910(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xea, 0);
}
u8 fn_80235974(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe9, 0);
}
u8 fn_802359D8(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe8, 0);
}
u8 fn_80235A3C(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe7, 0);
}
u8 fn_80235AA0(u32 param1, u32 param2) {
    extern u32 fn_801FB1C0();
    extern void* fn_8012640C();
    u16 slot = (u16)fn_801FB1C0(param1, 0, 0x43, 0);
    fn_801FB1C0(0, slot, 2, 0);
    return (u8)(u32)fn_8012640C(param2, 0, 0xe6, 0);
}
#undef fn_801FB1C0
#undef fn_8012640C

/* If ctx field 0x120 == 1: write slots 0x46/0x42 and trigger lbl_80378721. */
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
u8 fn_8023011C(void* ctx) {
    extern void* fn_8012640C();
    extern void fn_801F4C14();
    if ((u8)(u32)fn_8012640C(ctx, 0, 0x120, 0) == 1) {
        fn_801F4C14(0, 0, 0x46, 0, (u32)ctx);
        fn_801F4C14(0, 0, 0x42, 0, (u32)ctx);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80378721, 0);
    }
    return 1;
}
#undef fn_801F4C14
#undef fn_8012640C

/* Field-8 clear + event trigger when the move isn't 0x2B. */
#pragma optimize_for_size on
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
    extern u8   fn_802026E4();
    extern u8   fn_801FECD4();
    extern void fn_801FE7EC();
    u16 moveId = fn_80207BF4(ctx);

    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }
    if ((u8)fn_802026E4(ctx, 8) == 1 && moveId != 0x2b) {
        fn_80202810(ctx, 8);
        fn_80202810(ctx, 0x17);
        lbl_80478D78[5] = 1;
        fn_801F4C14(0, 0, 0x36, 0, (u32)ctx);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80379249, 0);
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
#pragma optimize_for_size reset

/* ===== 0x80235BE4-0x8023C370 trainer-AI accessor layer (wave2-w6 worker) =====
 * Selector idiom: fn_801FB1C0(0, fn_801FB1C0(ctx,0,0x43,0)&0xFFFF, 2, 0)
 * switches to the species context, then chained lookups read the stat. ===== */

/* Address: 0x802387C8 | Size: 0x54 */
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80203A6C fightOutPokemonGetNowHpPercentage
#define fn_80203E7C figthPokemonGetLevel
#define fn_80203DAC figthPokemonGetPokemonDataId
u32 fn_802387C8(void* ctx, void* param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_80203A6C(void* ctx);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203A6C(param);
}

/* Address: 0x8023892C | Size: 0x54 */
u32 fn_8023892C(void* ctx, void* param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_80203E7C(u32 ctx);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203E7C((u32)param);
}

/* Address: 0x80238980 | Size: 0x54 */
u16 fn_80238980(void* ctx, void* param) {
    extern u32 fn_801FB1C0();
    extern u16 fn_80203DAC(void* ctx);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203DAC(param);
}
#undef fn_801FB1C0
#undef fn_80203A6C
#undef fn_80203E7C
#undef fn_80203DAC

/* Address: 0x80236458 | Size: 0x64 */
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
u16 fn_80236458(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xEF, 0);
}

/* Address: 0x802364BC | Size: 0x64 */
u16 fn_802364BC(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xF0, 0);
}

/* Address: 0x80236520 | Size: 0x64 */
u32 fn_80236520(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xF1, 0);
}

/* Address: 0x80236B98 | Size: 0x64 */
u16 fn_80236B98(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(param, 0, 0xFA, 0);
}

/* Address: 0x802391E0 | Size: 0x64 */
u8 fn_802391E0(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8011BEB4();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u8)fn_8011BEB4(0, param, 0x2, 0);
}

/* Address: 0x80239244 | Size: 0x64 */
u8 fn_80239244(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8011BEB4();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u8)fn_8011BEB4(0, param, 0x5, 0);
}

/* Address: 0x80239500 | Size: 0x64 */
s16 fn_80239500(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8011BEB4();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (s16)fn_8011BEB4(0, param, 0x7, 0);
}

/* Address: 0x80239564 | Size: 0x64 */
u8 fn_80239564(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8011BEB4();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u8)fn_8011BEB4(0, param, 0xc, 0);
}
#undef fn_801FB1C0
#undef fn_8012640C
#undef fn_8011BEB4

/* Address: 0x80237774 | Size: 0x74 */
#define fn_8012640C pokemonGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80203E7C figthPokemonGetLevel
#define fn_80203DAC figthPokemonGetPokemonDataId
u32 fn_80237774(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern u32 fn_80203E7C(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203E7C(resolved);
}

/* Address: 0x802377E8 | Size: 0x74 */
u16 fn_802377E8(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern u16 fn_80203DAC(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_80203DAC(resolved);
}
#undef fn_8012640C
#undef fn_801FB1C0
#undef fn_80203E7C
#undef fn_80203DAC

/* Address: 0x80239498 | Size: 0x68 */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8011BEB4 wazaGetStatus
u8 fn_80239498(void* ctx, u32 param, u8 flag) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8011BEB4();
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u8)fn_8011BEB4(0, param, 0x1A, flag);
}
#undef fn_801FB1C0
#undef fn_8011BEB4
#pragma optimize_for_size reset

/* Address: 0x8023720C | Size: 0x7C */
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
u32 fn_8023720C(void* ctx, void* param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_80203C5C(void* ctx);
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_80203C5C(param);
    }
    return 0;
}
#undef fn_801FB1C0
#undef fn_80203C5C

/* Address: 0x802386C8 | Size: 0x80 */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_8012640C pokemonGetStatus
u32 fn_802386C8(void* ctx, void* param) {
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_8012640C();
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_8012640C(fn_80205BE8(param), 0, 0x8C, 0);
}
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_80205BE8
#undef fn_8012640C
#pragma optimize_for_size reset

/* Address: 0x80238748 | Size: 0x80 */
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80122DDC pokemonIsJoutaiNormal
u32 fn_80238748(void* ctx, void* param) {
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_80122DDC(u8* ptr);
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_80122DDC(fn_80205BE8(param));
    }
    return 1;
}
#undef fn_801FB1C0
#undef fn_80205BE8
#undef fn_80122DDC

/* Address: 0x80236BFC | Size: 0x84 */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
u32 fn_80236BFC(void* ctx, void* param2, void* param3) {
    extern u32 fn_801FB1C0();
    extern u32 fn_802026E4();
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_802026E4(param2, param3);
    }
    return 0;
}
#undef fn_801FB1C0
#pragma optimize_for_size reset

/* Address: 0x802384B4 | Size: 0x84 */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80202ADC fightPokemonCheckWriteJoutaiDataId
u32 fn_802384B4(void* ctx, void* param2, void* param3) {
    extern u32 fn_801FB1C0();
    extern u32 fn_80202ADC(void* ctx, void* typeObj);
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_80202ADC(param2, param3);
    }
    return 0;
}
#undef fn_801FB1C0
#undef fn_80202ADC
#pragma optimize_for_size reset

#define fn_8012640C pokemonGetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80122DDC pokemonIsJoutaiNormal
#define fn_802041EC fightPokemonGetSoubiItemSoubiDataId
#define fn_802042E0 fightPokemonGetSoubiItemDataId
#define fn_8011BEB4 wazaGetStatus
/* Address: 0x80237664 | Size: 0x88 */
u16 fn_80237664(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x87, 0);
}

/* Address: 0x802376EC | Size: 0x88 */
u16 fn_802376EC(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x83, 0);
}

/* Address: 0x8023831C | Size: 0x88 */
#pragma optimize_for_size on
u32 fn_8023831C(ctx, param)
void* ctx;
u32 param;
{
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_802041EC(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_802041EC(resolved);
}
#pragma optimize_for_size reset

/* Address: 0x802383A4 | Size: 0x88 */
#pragma optimize_for_size on
u32 fn_802383A4(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_802042E0(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return fn_802042E0(resolved);
}
#pragma optimize_for_size reset

/* Address: 0x8023842C | Size: 0x88 */
u16 fn_8023842C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    return (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x99, 0);
}

/* Address: 0x80239154 | Size: 0x8C */
u8 fn_80239154(void* ctx, u32 param) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8011BEB4();
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x36, 0) == 1) {
        return (u8)fn_8011BEB4(0, param, 0x1B, 0);
    }
    return 0;
}

/* Address: 0x80237310 | Size: 0xA0 */
u32 fn_80237310(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_80122DDC(u8* ptr);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x24, 0) == 1) {
        return fn_80122DDC(fn_80205BE8(resolved));
    }
    return 1;
}

/* Address: 0x802381C4 | Size: 0xAC */
#pragma optimize_for_size on
u8 fn_802381C4(void* ctx, void* param2, u32 param3) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param2, 0, 0xD6, 0);
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    fn_80205BE8(resolved);
    return (u8)fn_8012640C(fn_80205BE8(resolved), 0, 0x80, (u8)param3);
}
#pragma optimize_for_size reset

/* Address: 0x80238270 | Size: 0xAC */
#pragma optimize_for_size on
u16 fn_80238270(void* ctx, void* param2, u32 param3) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param2, 0, 0xD6, 0);
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    fn_80205BE8(resolved);
    return (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x7F, (u8)param3);
}
#pragma optimize_for_size reset

/* Address: 0x80236E9C | Size: 0xB0 */
#pragma optimize_for_size on
u16 fn_80236E9C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x8, 0);
}
#pragma optimize_for_size reset

/* Address: 0x80236F4C | Size: 0xB0 */
#pragma optimize_for_size on
u16 fn_80236F4C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x7, 0);
}
#pragma optimize_for_size reset

/* Address: 0x80236FFC | Size: 0xB0 */
#pragma optimize_for_size on
u16 fn_80236FFC(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x6, 0);
}
#pragma optimize_for_size reset

/* Address: 0x802370AC | Size: 0xB0 */
#pragma optimize_for_size on
u16 fn_802370AC(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x5, 0);
}
#pragma optimize_for_size reset

/* Address: 0x8023715C | Size: 0xB0 */
#pragma optimize_for_size on
u16 fn_8023715C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val;
    fn_801F54A4(0, 0, 0x14, 0);
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0x6E, 0);
    return (u16)fn_8012640C(0, val, 0x4, 0);
}
#pragma optimize_for_size reset

/* Address: 0x80238538 | Size: 0xC8 */
#pragma optimize_for_size on
u8 fn_80238538(void* ctx, void* param2) {
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_8012640C();
    u16 val, val2;
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val2 = (u16)fn_8012640C(fn_80205BE8(param2), 0, 0xC9, 0);
    if ((u8)fn_801FB1C0(0, val, 0x33, 0) == 1) {
        return (u8)fn_801FB1C0(0, val2, 0x1D, 0);
    }
    return 1;
}
#pragma optimize_for_size reset

/* Address: 0x80238600 | Size: 0xC8 */
#pragma optimize_for_size on
u8 fn_80238600(void* ctx, void* param2) {
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_8012640C();
    u16 val, val2;
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val2 = (u16)fn_8012640C(fn_80205BE8(param2), 0, 0xC9, 0);
    if ((u8)fn_801FB1C0(0, val, 0x23, 0) == 1) {
        return (u8)fn_801FB1C0(0, val2, 0x1C, 0);
    }
    return 0;
}
#pragma optimize_for_size reset

/* Address: 0x80236C80 | Size: 0xE0 */
#pragma optimize_for_size on
u8 fn_80236C80(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val, val2;
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val2 = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0xC9, 0);
    if ((u8)fn_801FB1C0(0, val, 0x33, 0) == 1) {
        return (u8)fn_801FB1C0(0, val2, 0x1D, 0);
    }
    return 1;
}
#pragma optimize_for_size reset

/* Address: 0x8023785C | Size: 0xE0 */
#pragma optimize_for_size on
u8 fn_8023785C(void* ctx, u32 param) {
    extern void* fn_8012640C();
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    void* resolved = fn_8012640C(param, 0, 0xD6, 0);
    u16 val, val2;
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val2 = (u16)fn_8012640C(fn_80205BE8(resolved), 0, 0xC9, 0);
    if ((u8)fn_801FB1C0(0, val, 0x23, 0) == 1) {
        return (u8)fn_801FB1C0(0, val2, 0x1C, 0);
    }
    return 0;
}
#pragma optimize_for_size reset
#undef fn_8012640C
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_80205BE8
#undef fn_80122DDC
#undef fn_802041EC
#undef fn_802042E0
#undef fn_8011BEB4

/* Address: 0x80237F74 | Size: 0xEC */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
u8 fn_80237F74(void* ctx, u32 param2, u32 param3) {
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80207BF4(void* param);
    u16 val;
    u32 temp;
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u16)param3 == 0) {
        return 0;
    }
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u8)fn_801FB1C0(0, val, 0x2B, 0) == 1) {
        temp = fn_80207BF4((void*)param2);
    } else {
        temp = 0;
    }
    if ((u16)param3 == (u16)temp) {
        return 1;
    }
    return 0;
}
#undef fn_801FB1C0
#undef fn_801F54A4
#undef fn_80207BF4
#pragma optimize_for_size reset

/* Address: 0x80239058 | Size: 0xFC */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_801248C4 pokemonGetTokuseiDataId
u8 fn_80239058(void* ctx, void* param2, u32 param3) {
    extern u32 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern void* fn_80205BE8(void* ctx);
    extern u32 fn_801248C4(void* param);
    u16 val;
    u32 temp;
    void* resolved;
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    if ((u16)param3 == 0) {
        return 0;
    }
    fn_801F54A4(0, 0, 0x14, 0);
    val = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    resolved = fn_80205BE8(param2);
    if ((u8)fn_801FB1C0(0, val, 0x2B, 0) == 1) {
        temp = fn_801248C4(resolved);
    } else {
        temp = 0;
    }
    if ((u16)param3 == (u16)temp) {
        return 1;
    }
    return 0;
}
#undef fn_801FB1C0
#undef fn_801F54A4
#undef fn_80205BE8
#undef fn_801248C4
#pragma optimize_for_size reset

/* TEST: block-scope u32 fn_801F025C under file-scope void* decl */
extern u32 lbl_8047B618;
void fn_80216804(void) {
    extern u32 fn_801F025C();
    u32 ctx = fn_801F025C(0x11, 0);

    if ((lbl_8047B618 & 0x2000000) != 0) {
        if (fn_802025B8(ctx, 0x23) == 2) {
            fn_8020248C(ctx, 0x23, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

/* ===== 0x80213-0x80217 FightSeq opcode handlers (wave2-w0 worker, part 2) ===== */
extern u8   fn_80203CCC();
extern u8   fn_801F221C();
extern void fn_80120B00();

/*
 * fn_80215300 (0x80215300)
 *
 * FightSeq opcode handler: if event-state 0x28 on the ctx from slot
 * 0x11 reads 2, clears it back to 0 and advances the PC past this
 * instruction (+5); otherwise takes the script-embedded jump.
 */
void fn_80215300(void) {
    extern u32 fn_801F025C();
    u32 ctx = fn_801F025C(0x11, 0);

    if (fn_802025B8(ctx, 0x28) == 2) {
        fn_8020248C(ctx, 0x28, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}

/* fn_80215A78 (0x80215A78): same shape, slot 0x11 / event-state id 0x25. */
void fn_80215A78(void) {
    extern u32 fn_801F025C();
    u32 ctx = fn_801F025C(0x11, 0);

    if (fn_802025B8(ctx, 0x25) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x25, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

/* fn_802161F0 (0x802161F0): same FightSeq opcode-handler shape as
 * fn_80215300, slot 0x12 / event-state id 0x30. */
void fn_802161F0(void) {
    extern u32 fn_801F025C();
    u32 ctx = fn_801F025C(0x12, 0);

    if (fn_802025B8(ctx, 0x30) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x30, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

/* fn_802162F0 (0x802162F0): same shape, slot 0x12 / event-state id 0x1b. */
void fn_802162F0(void) {
    extern u32 fn_801F025C();
    u32 ctx = fn_801F025C(0x12, 0);

    if (fn_802025B8(ctx, 0x1b) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x1b, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

void fn_802158D0(void) {
    extern u32 fn_801F025C();
    u32 ctx = fn_801F025C(0x12, 0);

    if (fn_802025B8(ctx, 0x26) != 2) {
        goto deref;
    }
    if (fn_80203CCC(ctx) != 0) {
        goto plus5;
    }
deref:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
plus5:
    fn_8020248C(ctx, 0x26, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}

#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F6DF0 fightSideWriteJoutaiDataId
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80120B00 pokemonGetMezamerupower
#define fn_8011BBD8 wazaSetStatus
void fn_802165B4(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u32 fn_801F0134();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 tmp = fn_801F025C(2, ctx1);

    if (fn_801F6E44(tmp, 0x4d) == 2) {
        u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
        u32 val2 = fn_801F0134(ctx1, val);
        fn_801F6DF0(tmp, 0x4d, val2);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#pragma optimize_for_size reset

void fn_80217018(void) {
    extern u32 fn_80205B8C();
    extern void fn_80120B00();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    u32 ctx = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    u32 poke = fn_80205B8C(ctx);
    u16 outA, outB;

    fn_80120B00(poke, &outA, &outB);
    fn_8011BBD8(fieldD9, 0, 0x2f, 0, outA);
    fn_8011BBD8(fieldD9, 0, 0x30, 0, outB);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_801F6E44
#undef fn_801F54A4
#undef fn_801F0134
#undef fn_801F6DF0
#undef fn_8012640C
#undef fn_80205B8C
#undef fn_80120B00
#undef fn_8011BBD8

#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801254B4 pokemonSetStatus
#define fn_801F221C fightFloorIsLastActionFightOutPokemon
void fn_80214DB0(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801254B4();
    extern u8 fn_801F221C();
    u32 ctx = fn_801F025C(0x11, 0);
    fn_801254B4(ctx, 0, 0x118, 0, 1);

    if (fn_801F221C(0) != 1) {
        goto check2;
    }
deref:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
check2:
    if (fn_802025B8(ctx, 0x33) != 2) {
        goto deref;
    }
    fn_8020248C(ctx, 0x33, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F221C
#undef fn_801254B4
#undef fn_801F025C

#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_801F221C fightFloorIsLastActionFightOutPokemon
void fn_80214E50(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801254B4();
    extern void fn_801F4C14();
    extern u8 fn_801F221C();
    u32 ctx = fn_801F025C(0x11, 0);
    fn_801F4C14(0, 0, 0x43, 0, ctx);
    fn_801254B4(ctx, 0, 0x118, 0, 1);

    if (fn_801F221C(0) != 1) {
        goto check2;
    }
deref:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
check2:
    if (fn_802025B8(ctx, 0x37) != 2) {
        goto deref;
    }
    fn_8020248C(ctx, 0x37, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_801F4C14
#undef fn_801254B4
#undef fn_801F221C

#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
void fn_80215808(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 poke1 = fn_80205B8C(ctx1);
    s32 statA = (u16)fn_8012640C(poke1, 0, 0x83, 0);

    u32 ctx2 = fn_801F025C(0x12, 0);
    u32 poke2 = fn_80205B8C(ctx2);
    s32 statB = (u16)fn_8012640C(poke2, 0, 0x83, 0);

    if (statB <= statA) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, statB - statA);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205B8C
#undef fn_8011BBD8
#pragma optimize_for_size reset

void fn_80216874(void) {
    extern u32 fn_801F025C();
    extern u16 fn_80205184();
    extern u8 fn_802026E4();
    u32 ctx = fn_801F025C(0x11, 0);
    u16 moveId = fn_80205184(ctx);

    switch (moveId) {
    case 0x13:
    case 0x154:
        if (fn_802026E4(ctx, 0x1f) == 1) {
            fn_80202810(ctx, 0x1f);
        }
        break;
    case 0x5b:
        if (fn_802026E4(ctx, 0x20) == 1) {
            fn_80202810(ctx, 0x20);
        }
        break;
    case 0x123:
        if (fn_802026E4(ctx, 0x21) == 1) {
            fn_80202810(ctx, 0x21);
        }
        break;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

/*
 * fn_80216960 (0x80216960)
 *
 * Same move-id dispatch table as fn_80216874 (0x13/0x154 -> flag
 * 0x1f, 0x5b -> flag 0x20, 0x123 -> flag 0x21), but using the
 * GetEventState==2 / SetEventState(...,0) idiom instead of
 * CheckEventFlag/fn_80202810. PC always advances by 1.
 */
void fn_80216960(void) {
    extern u32 fn_801F025C();
    extern u16 fn_80205184();
    u32 ctx = fn_801F025C(0x11, 0);
    u16 moveId = fn_80205184(ctx);

    switch (moveId) {
    case 0x13:
    case 0x154:
        if (fn_802025B8(ctx, 0x1f) == 2) {
            fn_8020248C(ctx, 0x1f, 0);
        }
        break;
    case 0x5b:
        if (fn_802025B8(ctx, 0x20) == 2) {
            fn_8020248C(ctx, 0x20, 0);
        }
        break;
    case 0x123:
        if (fn_802025B8(ctx, 0x21) == 2) {
            fn_8020248C(ctx, 0x21, 0);
        }
        break;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

/* ===== 0x80213-0x80217 FightSeq status ops + dispatchers (wave2-w0 worker, part 1) ===== */
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

/* fn_80214AB4: bytecode op -- queue callback fn_8022EB9C via fn_801F37B0
 * with a 1-byte scratch buffer; PC += 1. */
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_80214AB4(void) {
    extern void fn_801F37B0();
    extern void fn_8022EB9C();
    u8 local = 0;
    fn_801F37B0(0, fn_8022EB9C, &local, 0);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F37B0

/* fn_80214BB4: bytecode op -- if fn_802062FC(ctx) for ctx from slot pc[1] is
 * nonzero, jump to the u32 target at pc+2; else PC += 6. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802062FC fightOutPokemonCheckFightOut
void fn_80214BB4(void) {
    extern u32 fn_801F025C();
    extern u8 fn_802062FC();
    u32 ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    if (fn_802062FC(ctx) == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 2);
    } else {
        lbl_8047B610 = lbl_8047B610 + 6;
    }
}
#undef fn_801F025C
#undef fn_802062FC

/* fn_80214CB0: bytecode op -- queue callback fn_80214CFC via fn_801F37B0
 * with the ctx resolved from slot 0x19; PC += 5. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_80214CB0(void) {
    extern u32 fn_801F025C();
    extern int fn_801F37B0();
    u32 ctx = fn_801F025C(0x19, 0);
    fn_801F37B0(0, fn_80214CFC, ctx, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_801F37B0

/* fn_802136C8/fn_80213750/fn_802137D8: bytecode op -- statusSetStatus with
 * operands unpacked from the script stream: id (u8 @1), then three
 * optional-pointer args (NULL -> 0, else read through), a plain u16, and a
 * final unconditional deref whose width (u32/u16/u8) is the only difference
 * between the three op variants; PC += 0x14. */
void fn_802136C8(void) {
    u8* p2 = *(u8**)(lbl_8047B610 + 2);
    u8 id = *(u8*)(lbl_8047B610 + 1);
    u16 valB = *(u16*)(lbl_8047B610 + 0xa);
    u8* p6 = *(u8**)(lbl_8047B610 + 6);
    u8* pc_ = *(u8**)(lbl_8047B610 + 0xc);
    u32* pE = *(u32**)(lbl_8047B610 + 0x10);
    u32 a2, a3, a5;
    if (p2 == 0) { a2 = 0; } else { a2 = *(u32*)p2; }
    if (p6 == 0) { a3 = 0; } else { a3 = *p6; }
    if (pc_ == 0) { a5 = 0; } else { a5 = *pc_; }
    statusSetStatus(id, a2, a3, valB, a5, *pE);
    lbl_8047B610 = lbl_8047B610 + 0x14;
}

void fn_80213750(void) {
    u8* p2 = *(u8**)(lbl_8047B610 + 2);
    u8 id = *(u8*)(lbl_8047B610 + 1);
    u16 valB = *(u16*)(lbl_8047B610 + 0xa);
    u8* p6 = *(u8**)(lbl_8047B610 + 6);
    u8* pc_ = *(u8**)(lbl_8047B610 + 0xc);
    u16* pE = *(u16**)(lbl_8047B610 + 0x10);
    u32 a2, a3, a5;
    if (p2 == 0) { a2 = 0; } else { a2 = *(u32*)p2; }
    if (p6 == 0) { a3 = 0; } else { a3 = *p6; }
    if (pc_ == 0) { a5 = 0; } else { a5 = *pc_; }
    statusSetStatus(id, a2, a3, valB, a5, *pE);
    lbl_8047B610 = lbl_8047B610 + 0x14;
}

void fn_802137D8(void) {
    u8* p2 = *(u8**)(lbl_8047B610 + 2);
    u8 id = *(u8*)(lbl_8047B610 + 1);
    u16 valB = *(u16*)(lbl_8047B610 + 0xa);
    u8* p6 = *(u8**)(lbl_8047B610 + 6);
    u8* pc_ = *(u8**)(lbl_8047B610 + 0xc);
    u8* pE = *(u8**)(lbl_8047B610 + 0x10);
    u32 a2, a3, a5;
    if (p2 == 0) { a2 = 0; } else { a2 = *(u32*)p2; }
    if (p6 == 0) { a3 = 0; } else { a3 = *p6; }
    if (pc_ == 0) { a5 = 0; } else { a5 = *pc_; }
    statusSetStatus(id, a2, a3, valB, a5, *pE);
    lbl_8047B610 = lbl_8047B610 + 0x14;
}

/* fn_80213860/fn_802138F0/fn_80213980: bytecode op -- statusGetStatus with
 * the same 3 optional-pointer/id/u16 operand unpacking as the SetStatus
 * trio above, storing the result through an output pointer at pc+0x10
 * whose width (u32/u16/u8) is the only difference between variants;
 * PC += 0x14. */
void fn_80213860(void) {
    u8* p2 = *(u8**)(lbl_8047B610 + 2);
    u8 id = *(u8*)(lbl_8047B610 + 1);
    u16 valB = *(u16*)(lbl_8047B610 + 0xa);
    u8* p6 = *(u8**)(lbl_8047B610 + 6);
    u8* p8 = *(u8**)(lbl_8047B610 + 0xc);
    u32* outPtr = *(u32**)(lbl_8047B610 + 0x10);
    u32 a2, a3, a5;
    if (p2 == 0) { a2 = 0; } else { a2 = *(u32*)p2; }
    if (p6 == 0) { a3 = 0; } else { a3 = *p6; }
    if (p8 == 0) { a5 = 0; } else { a5 = *p8; }
    *outPtr = statusGetStatus(id, a2, a3, valB, a5);
    lbl_8047B610 = lbl_8047B610 + 0x14;
}

void fn_802138F0(void) {
    u8* p2 = *(u8**)(lbl_8047B610 + 2);
    u8 id = *(u8*)(lbl_8047B610 + 1);
    u16 valB = *(u16*)(lbl_8047B610 + 0xa);
    u8* p6 = *(u8**)(lbl_8047B610 + 6);
    u8* p8 = *(u8**)(lbl_8047B610 + 0xc);
    u16* outPtr = *(u16**)(lbl_8047B610 + 0x10);
    u32 a2, a3, a5;
    if (p2 == 0) { a2 = 0; } else { a2 = *(u32*)p2; }
    if (p6 == 0) { a3 = 0; } else { a3 = *p6; }
    if (p8 == 0) { a5 = 0; } else { a5 = *p8; }
    *outPtr = (u16)statusGetStatus(id, a2, a3, valB, a5);
    lbl_8047B610 = lbl_8047B610 + 0x14;
}

void fn_80213980(void) {
    u8* p2 = *(u8**)(lbl_8047B610 + 2);
    u8 id = *(u8*)(lbl_8047B610 + 1);
    u16 valB = *(u16*)(lbl_8047B610 + 0xa);
    u8* p6 = *(u8**)(lbl_8047B610 + 6);
    u8* p8 = *(u8**)(lbl_8047B610 + 0xc);
    u8* outPtr = *(u8**)(lbl_8047B610 + 0x10);
    u32 a2, a3, a5;
    if (p2 == 0) { a2 = 0; } else { a2 = *(u32*)p2; }
    if (p6 == 0) { a3 = 0; } else { a3 = *p6; }
    if (p8 == 0) { a5 = 0; } else { a5 = *p8; }
    *outPtr = (u8)statusGetStatus(id, a2, a3, valB, a5);
    lbl_8047B610 = lbl_8047B610 + 0x14;
}

/* fn_80216264: bytecode op -- jump if BOTH field 0x102 and field 0x104 of
 * the slot-0x11 ctx are zero; else skip (PC += 5). */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void fn_80216264(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    u32 ctx = fn_801F025C(0x11, 0);
    s16 val1 = (s16)fn_8012640C(ctx, 0, 0x102, 0);
    s16 val2 = (s16)fn_8012640C(ctx, 0, 0x104, 0);
    if (val1 != 0 || val2 != 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_8012640C
#undef fn_801F025C
#pragma optimize_for_size reset

/* fn_80214C04: bytecode op -- if the ctx from slot pc[1] has state 0x1e,
 * run a fixed side-effect chain; PC += 2 unconditionally. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801252E0 pokemonInitJoutai
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_80214C04(void) {
    extern u32 fn_80205B8C();
    extern void fn_801252E0();
    extern void fn_80202998();
    extern void fn_80202810();
    extern u32 fn_801F025C();
    extern u32 fn_80207BF4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    u32 tmp = fn_80205B8C(ctx);
    if ((u16)fn_80207BF4(ctx) == 0x1e) {
        fn_801252E0(tmp);
        fn_80202998(ctx, 0);
        fn_80202810(ctx, 0x17);
        if (fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x7c, 0, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_80205B8C
#undef fn_80207BF4
#undef fn_801252E0
#undef fn_80202998
#undef fn_80202810
#undef fn_801FECD4
#undef fn_801FE7EC
#pragma optimize_for_size reset

/* fn_80216364: bytecode op -- if the slot-0x11 ctx has state 1 (per
 * fn_80203C5C), run a side-effect chain (PC += 5 mid-branch); else jump to
 * the u32 target at pc+1. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_801252E0 pokemonInitJoutai
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_80216364(void) {
    extern u32 fn_80205B8C();
    extern void fn_801252E0();
    extern void fn_80202998();
    extern u32 fn_801F025C();
    extern u8 fn_80203C5C();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx = fn_801F025C(0x11, 0);
    u32 tmp = fn_80205B8C(ctx);
    if (fn_80203C5C(ctx) == 1) {
        fn_801252E0(tmp);
        fn_80202998(ctx, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
        if (fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x7c, 0, 0);
        }
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_80205B8C
#undef fn_80203C5C
#undef fn_801252E0
#undef fn_80202998
#undef fn_801FECD4
#undef fn_801FE7EC
#pragma optimize_for_size reset

/* fn_80216048: helper -- returns 0 if elem fails either of two
 * fn_801F54A4(0x2f)-gated checks (fn_80204A10 / fn_802026E4(elem,0x3d)),
 * else 1. */
#define fn_801F54A4 fightFloorGetStatus
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
u8 fn_80216048(u32 elem) {
    extern u32 fn_801F54A4();
    extern u8 fn_80204A10();
    extern u8 fn_802026E4();
    if ((u8)fn_801F54A4(0, 0, 0x2f, 0) == 1) {
        if (fn_80204A10(elem) == 0) {
            return 0;
        }
    }
    if ((u8)fn_801F54A4(0, 0, 0x2f, 0) == 1) {
        if (fn_802026E4(elem, 0x3d) == 1) {
            return 0;
        }
    }
    return 1;
}
#undef fn_80204A10
#undef fn_801F54A4

/* fn_80216CF8: damage-ish formula -- acc = fn_8012640C(0,p2,4,0) *
 * (u16)fn_8011BEB4(0,p1,7,0) * (((u16)p3<<1)/5 + 2), all divided by
 * fn_8012640C(0,p4,5,0) (clamped to >=1) then by 50, +2. p5 is unused. */
#pragma optimize_for_size on
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
#undef fn_8011BEB4
#undef fn_8012640C
#pragma optimize_for_size reset

/* fn_802160EC: bytecode op -- cascaded gate (ctx2 from slot 0xe on ctx1
 * nonzero; fn_801F54A4(...,0x19,...)>=2; fn_802062FC(ctx2)==1; both ctx1
 * and ctx2 lack CheckEventFlag 0x32) then fn_801F4C14(...,0x43,...,ctx2)
 * plus a SetEventState(ctx2,0x32,0) if state==2; PC+=5. Else script jump. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F54A4 fightFloorGetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
void fn_802160EC(void) {
    extern u8 fn_802062FC();
    extern u8 fn_801F4C14();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2 = fn_801F025C(0xe, ctx1);

    if (ctx2 != 0 &&
        (u16)fn_801F54A4(0, 0, 0x19, 0) >= 2 &&
        fn_802062FC(ctx2) == 1 &&
        fn_802026E4(ctx1, 0x32) == 0 &&
        fn_802026E4(ctx2, 0x32) == 0) {
        fn_801F4C14(0, 0, 0x43, 0, ctx2);
        if (fn_802025B8(ctx2, 0x32) == 2) {
            fn_8020248C(ctx2, 0x32, 0);
        }
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_801F54A4
#undef fn_802062FC
#undef fn_801F4C14
#pragma optimize_for_size reset

/* fn_80217434: bytecode op -- if field 0xe6 (an s8 counter) is < 12 and
 * statA (field 0x83 of the slot-0x11 side's pokemon) exceeds
 * fn_80203B5C(ctx,2), bumps field 0xe6 to 12 via fn_801254B4 and applies
 * the delta via fn_8011BBD8(fieldD9,0,0x2d,0,val2); PC += 5. Else jump. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_801254B4 pokemonSetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80217434(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern u16 fn_80203B5C();
    extern u32 fn_801254B4();
    extern void fn_8011BBD8();
    u32 ctx = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    u16 statA = (u16)fn_8012640C(fn_80205B8C(ctx), 0, 0x83, 0);
    u16 val2 = fn_80203B5C(ctx, 2);
    s8 e6field = (s8)fn_8012640C(ctx, 0, 0xe6, 0);

    if (e6field < 0xc && statA > val2) {
        fn_801254B4(ctx, 0, 0xe6, 0, 0xc);
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, val2);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205B8C
#undef fn_80203B5C
#undef fn_801254B4
#undef fn_8011BBD8
#pragma optimize_for_size reset

/* fn_80213158: dispatcher -- fetches val=fn_801F54A4(...,0x45,...), queues
 * fn_801F4C14(...,0x36,...,val), resolves fn_802037DC(val) into
 * fn_80132A38(0xd,...), clears lbl_8047B614, fires fn_801EF8F4(1),
 * applies fn_80202810 for any of flags {0x2e,0x15,0x28} set on val, then
 * fires TriggerEvent(param1,lbl_8037889D,1), cleans up party UI. */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_80132A38 msgctrlSetValue
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_8026246C fightMenuCloseMsg
void fn_80213158(u32 param1) {
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void* fn_802037DC(void*);
    extern void fn_80132A38();
    extern void fn_80202810();
    extern void fn_8026246C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    extern void fn_80211B94();
    u32 val = fn_801F54A4(0, 0, 0x45, 0);

    fn_801F4C14(0, 0, 0x36, 0, val);
    fn_80132A38(0xd, fn_802037DC((void*)val));
    lbl_8047B614 = 0;
    fn_801EF8F4(1);

    if (fn_802026E4(val, 0x2e) == 1) {
        fn_80202810(val, 0x2e);
    }
    if (fn_802026E4(val, 0x15) == 1) {
        fn_80202810(val, 0x15);
    }
    if (fn_802026E4(val, 0x28) == 1) {
        fn_80202810(val, 0x28);
    }

    fn_80211B94(param1, lbl_8037889D, 1);
    fn_801DA7AC();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fn_8026246C();
}
#undef fn_801F54A4
#undef fn_801F4C14
#undef fn_802037DC
#undef fn_80132A38
#undef fn_80202810
#undef fn_8026246C
#pragma optimize_for_size reset

/* ===== 0x8021D688-0x80223A24 sound/anim/thread ops (wave2-w2 worker) ===== */
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

/* fn_8021E6CC/DC/EC/fn_8021E744: PC-advance-by-2, return pre-advance PC. */
u8* fn_8021E6CC(void) { u8* pc = lbl_8047B610; lbl_8047B610 = pc + 2; return pc; }
u8* fn_8021E6DC(void) { u8* pc = lbl_8047B610; lbl_8047B610 = pc + 2; return pc; }
u8* fn_8021E6EC(void) { u8* pc = lbl_8047B610; lbl_8047B610 = pc + 2; return pc; }
u8* fn_8021E744(void) { u8* pc = lbl_8047B610; lbl_8047B610 = pc + 2; return pc; }

/* PC-advance-by-1 (void). */
void fn_8021EE38(void) { lbl_8047B610 = lbl_8047B610 + 1; }

/* Sound-id opcode handlers: id read at pc+1 (u16), advance pc by 3. */
void fn_8021EE98(void) {
    fn_80165668(*(u16*)(lbl_8047B610 + 1), 0, 0xff);
    lbl_8047B610 = lbl_8047B610 + 3;
}

void fn_8021EED4(void) {
    fn_80166A50(*(u16*)(lbl_8047B610 + 1), 0, 0xff, 0);
    lbl_8047B610 = lbl_8047B610 + 3;
}

/* PC-advance-by-2 (void). */
void fn_8021EF14(void) { lbl_8047B610 = lbl_8047B610 + 2; }

/* Jump opcode: pc = *(u32*)(pc+1). */
void fn_80222ACC(void) { lbl_8047B610 = *(u8**)(lbl_8047B610 + 1); }

/* Toggle flag bit 0x1000 of lbl_8047B618; pc += 1. */
void fn_8021E6FC(void) {
    fn_801F22D8(0);

    if ((lbl_8047B618 & 0x1000) != 0) {
        lbl_8047B618 = lbl_8047B618 & ~0x1000;
    } else {
        lbl_8047B618 = lbl_8047B618 | 0x1000;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

/* If fn_80262308()==1, increment counter and pc+=5; else take script jump. */
#define fn_80262308 fightMenuYesNo
void fn_8021EA94(void) {
    extern u32 fn_80262308();
    if ((u8)fn_80262308() == 1) {
        lbl_8047B642++;
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_80262308

/* ctx = fn_801F025C(byte@pc+1, 0); play sound via fn_80166A50; pc += 2. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802036D4 fightOutPokemonGetVoiceSndId
void fn_8021EE48(void) {
    extern u32 fn_801F025C();
    extern int fn_802036D4();
    u32 ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    fn_80166A50((u16)fn_802036D4(ctx), 0, 0xff, 0);
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_802036D4

/* Trainer-message-related dispatcher chain; pc += 2. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801FB1C0 fightTrainerGetStatus
void fn_8021F92C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern u32 fn_801FB1C0();
    u32 ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    fn_801FCEC4(fn_801FB1C0(fn_801F4354(0, ctx), 0, 0x47, 0), ctx);
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_801F4354
#undef fn_801FB1C0

/* fieldD9 delta write via byte@pc+1; pc += 2. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80222B7C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xd9, 0);
    fn_8011BBD8(fieldD9, 0, 0x31, 0, *(u8*)(lbl_8047B610 + 1));
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BBD8

/* Cache lbl_8047B64C, cache lbl_8047B61C via fn_8011BEB4/fn_800FA280,
 * write via fn_80132A38(0xd/0xe); pc += 1. */
#define fn_8011BEB4 wazaGetStatus
#define fn_800FA280 GSmsgGetGSchar
#define fn_80203848 fightPokemonGetNicknamePtr
#define fn_80132A38 msgctrlSetValue
void fn_8021ECF8(void) {
    extern s32 fn_8011BEB4();
    extern u32 fn_800FA280();
    extern u32 fn_80203848();
    extern void fn_80132A38();
    u32 val = (u32)lbl_8047B64C;
    u32 t = fn_800FA280(fn_8011BEB4(0, lbl_8047B61C, 1, 0));
    u32 result = fn_80203848(val);
    fn_80132A38(0xd, result);
    fn_80132A38(0xe, t);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_8011BEB4
#undef fn_800FA280
#undef fn_80203848
#undef fn_80132A38

/* If field 0xfb is nonzero, apply move-related updates and possibly
 * SetTrainerEventState 0x82; always returns 1. */
#define fn_8012640C pokemonGetStatus
#define fn_8020147C fightOutPokemonDoItemSoubi
#define fn_801254B4 pokemonSetStatus
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
s32 fn_802207D4(u32 ctx) {
    extern u32 fn_8012640C();
    extern u16 fn_8020147C();
    extern u32 fn_801254B4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u16 field = (u16)fn_8012640C(ctx, 0, 0xfb, 0);
    if (field != 0) {
        fn_8020147C(ctx, field, 1, 0);
        fn_801254B4(ctx, 0, 0xfb, 0, 0);
        if ((u8)fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x82, 0, 0);
        }
    }
    return 1;
}
#undef fn_8012640C
#undef fn_8020147C
#undef fn_801254B4
#undef fn_801FECD4
#undef fn_801FE7EC

/* fieldD9 shadow-check; gated fn_80209380 call on flag bits; pc += 2. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_802096E8 fightWazaIsHit
#define fn_80209380 fightOutPokemonDamageEffect
void fn_8021E9F4(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u8 fn_802096E8();
    u32 ctxA = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    u32 ctx2 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx2, 0, 0xd9, 0);
    if (fn_802096E8(fieldD9) == 1) {
        if ((lbl_8047B618 & 0x100) != 0) {
            lbl_8047B610 = lbl_8047B610 + 2;
            return;
        } else if ((lbl_8047B618 & 0x80) != 0) {
            fn_80209380(ctxA);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_802096E8
#undef fn_80209380

/* Resolve tableId via fn_801F54A4, gated party-slot refresh sequence via
 * fn_802062FC/fn_80208ED0/fn_80265598/fn_8026532C; pc += 2. */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80208ED0 fightOutPokemonModosuEffect
void fn_8021ED70(void) {
    extern u8 fn_802062FC();
    extern void fn_80208ED0();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    u32 ctx;
    u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    if (fn_802062FC(ctx) == 1) {
        fn_80208ED0(ctx, 0);
        fn_80265598(ctx, val, 1);
        fn_80208ED0(ctx, 1);
        fn_80208ED0(ctx, 2);
        fn_80208ED0(ctx, 3);
        fn_80208ED0(ctx, 4);
        fn_8026532C(ctx, val, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_802062FC
#undef fn_80208ED0
#pragma optimize_for_size reset

/* Gated warp-link check (byte@pc+6==1 && fn_801F2020) then
 * fn_801F8A18 result decides pc+=7 vs script jump. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F2020 fightFloorCheckFightActionFightOutPokemonIrekaeSelect
void fn_8021F39C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern u8 fn_801F2020();
    u32 ctxA = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    u32 result = fn_801F4354(0, ctxA);
    u16 buf;

    if (*(u8*)(lbl_8047B610 + 6) == 1 && fn_801F2020(0, ctxA, 0) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 2);
        return;
    }

    buf = 0;
    if (fn_801F8A18(result, &buf) == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 2);
    } else {
        lbl_8047B610 = lbl_8047B610 + 7;
    }
}
#undef fn_801F2020
#undef fn_801F4354
#undef fn_801F025C
#pragma optimize_for_size reset

/* Party-slot animation refresh gated on shadow-purify check + threaded
 * wait loop; shared fn_80208ED0/fn_80265598/fn_8026532C tail. pc += 1. */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_802086B0 fightOutPokemonFreeAllSequenceWaza
#define fn_8020F108 fightWazaWzxTypeFuncMigawari
#define fn_80208ED0 fightOutPokemonModosuEffect
void fn_8021F998(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    u32 ctx;
    u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(0x14, 0);

    if ((u8)fn_802062FC(ctx) == 1) {
        if ((u8)fn_802026E4(ctx, 0x14) == 1 && fn_801FEF74(ctx) > 0) {
            fn_80202810(ctx, 0x14);
            fn_802086B0(ctx);
            fn_8020F108(0xa4, ctx, ctx, 0, 0);
            for (;;) {
                if ((u8)fn_801DA5C4(6) == 1) {
                    break;
                }
                _threadSwitch();
            }
        }
        fn_802086B0(ctx);

        fn_80208ED0(ctx, 0);
        fn_80265598(ctx, val, 1);
        fn_80208ED0(ctx, 1);
        fn_80208ED0(ctx, 2);
        fn_80208ED0(ctx, 3);
        fn_80208ED0(ctx, 4);
        fn_8026532C(ctx, val, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_802062FC
#undef fn_801FEF74
#undef fn_80202810
#undef fn_802086B0
#undef fn_8020F108
#undef fn_80208ED0
#pragma optimize_for_size reset

/* ===== 0x80223A24-0x8022FE20 handlers (wave2-w34 worker) ===== */
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

/*
 * fn_80223D64 (0x80223D64)
 *
 * Generic FightSeq conditional-jump handler: the target slot is read
 * from the script byte at PC+1 (not hardcoded), a 4-byte "kind" field
 * at PC+2 selects among fn_80203CCC/fn_80203C5C/fn_802026E4+
 * fn_802062FC, and the jump target is the pointer at PC+6. Instruction
 * length is 10 bytes.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80203CCC fightOutPokemonIsJoutaiNormal
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_802062FC fightOutPokemonCheckFightOut
void fn_80223D64(void) {
    extern u32 fn_801F025C();
    extern u8 fn_80203CCC();
    extern u8 fn_80203C5C();
    extern u8 fn_802026E4();
    extern u32 fn_802062FC();
    u8* target;
    u32 ctx;
    u16 kind;

    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    kind = *(u32*)(lbl_8047B610 + 2);
    target = *(u8**)(lbl_8047B610 + 6);

    if (kind == 1) {
        if ((u8)fn_80203CCC(ctx) == 0) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    } else if (kind == 2) {
        if ((u8)fn_80203C5C(ctx) == 1) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    } else {
        if ((u8)fn_802026E4(ctx, kind) == 1 && (u8)fn_802062FC(ctx) == 1) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    }
}
#undef fn_802062FC
#undef fn_80203C5C
#undef fn_80203CCC
#undef fn_801F025C
#pragma optimize_for_size reset

/* fn_80223E40 (0x80223E40): identical body to fn_80223D64; register
 * coloring differs (ctx/target swap r30<->r31), controlled via
 * declaration order. */
#pragma optimize_for_size on
void fn_80223E40(void) {
    extern u32 fn_801F025C();
    extern u8 fn_80203C5C();
    extern u8 fn_802026E4();
    extern u32 fn_802062FC();
    u32 ctx;
    u8* target;
    u16 kind;

    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    kind = *(u32*)(lbl_8047B610 + 2);
    target = *(u8**)(lbl_8047B610 + 6);

    if (kind == 1) {
        if ((u8)fn_80203CCC(ctx) == 0) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    } else if (kind == 2) {
        if ((u8)fn_80203C5C(ctx) == 1) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    } else {
        if ((u8)fn_802026E4(ctx, kind) == 1 && (u8)fn_802062FC(ctx) == 1) {
            lbl_8047B610 = target;
        } else {
            lbl_8047B610 = lbl_8047B610 + 10;
        }
    }
}
#pragma optimize_for_size reset

/*
 * fn_80224060 (0x80224060)
 *
 * Wait-for-input handler: opens a message/menu on the slot-derived ctx
 * (event flag 0x14 + fn_801FEF74 gate) then polls fn_801DA5C4(6) in a
 * do/while with _threadSwitch() until input resolves; always closes
 * out via fn_802086B0/fn_8020912C x2/fn_80265598(ctx,count,1) and
 * advances PC by 2.
 */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_802086B0 fightOutPokemonFreeAllSequenceWaza
#define fn_8020F108 fightWazaWzxTypeFuncMigawari
#define fn_8020912C fightOutPokemonKizetuEffect
void fn_80224060(void) {
    extern s32 fn_801FEF74();
    extern void fn_80202810();
    extern void fn_802086B0();
    extern void fn_8020F108();
    extern u32 fn_8020912C();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    u32 ctx;
    u16 count;

    count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);

    if ((u8)fn_802026E4(ctx, 0x14) == 1 && fn_801FEF74(ctx) > 0) {
        fn_80202810(ctx, 0x14);
        fn_802086B0(ctx);
        fn_8020F108(0xa4, ctx, ctx, 0, 0);
        do {
            if ((u8)fn_801DA5C4(6) == 1) {
                break;
            }
            _threadSwitch();
        } while (1);
    }
    fn_802086B0(ctx);
    fn_8020912C(ctx, 0);
    fn_8020912C(ctx, 1);
    fn_80265598(ctx, count, 1);
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_801FEF74
#undef fn_80202810
#undef fn_802086B0
#undef fn_8020F108
#undef fn_8020912C
#pragma optimize_for_size reset

/*
 * fn_80223F1C (0x80223F1C)
 *
 * Sets up a slot-derived ctx for FightSeq: fn_801F4354(0,ctx) attach,
 * status-flag toggles via fn_8020912C(ctx,2)/fn_8026532C(ctx,count,0)/
 * fn_8020912C(ctx,3), registers the fn_80232FE4 callback, forwards the
 * ctx's active Pokemon into fn_801252E0, then applies a couple of
 * CheckEventFlag-gated fn_801FE7EC field writes. PC always advances by
 * 2.
 */
#define fn_801252E0 pokemonInitJoutai
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8020912C fightOutPokemonKizetuEffect
#pragma optimize_for_size on
void fn_80223F1C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u8 fn_802026E4();
    extern u8 fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx;
    u16 count;

    count = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);

    fn_801F4354(0, ctx);
    fn_8020912C(ctx, 2);
    fn_8026532C(ctx, count, 0);
    fn_8020912C(ctx, 3);
    fn_801F37B0(0, fn_80232FE4, ctx, 0);
    fn_801252E0(fn_80205B8C(ctx));
    fn_80202998(ctx, 0);
    fn_80202810(ctx, 0x17);

    if ((u8)fn_801FECD4(ctx) == 1) {
        fn_801FE7EC(ctx, 0x7c, 0, 0);
    }
    if ((u8)fn_802026E4(ctx, 0x3e) == 1) {
        fn_80202810(ctx, 0x3e);
        if ((u8)fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0xc8, 0, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#pragma optimize_for_size reset
#undef fn_801252E0
#undef fn_801F025C
#undef fn_801F37B0
#undef fn_801F4354
#undef fn_801F54A4
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80202810
#undef fn_80202998
#undef fn_80205B8C
#undef fn_8020912C

/*
 * fn_802267E8 (0x802267E8)
 *
 * Registers a slot ctx via fn_802656AC(ctx,count,1); fires
 * fn_80102620/fn_80011C78 and fn_802038A4/fn_80207C24 side-effect
 * pairs; then if event flag 0x14 is set and fn_801FEF74 hasn't already
 * resolved, polls fn_801DA5C4(6) (do/while + _threadSwitch) before
 * closing out with fn_80202810/fn_80211B94/fn_8020248C/fn_80201764.
 * PC always advances by 2.
 */
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
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_802656AC
#undef fn_80102620
#undef fn_80011C78
#undef fn_802038A4
#undef fn_80207C24
#undef fn_801FEF74
#undef fn_80202810
#pragma optimize_for_size reset

/*
 * fn_80227C40 (0x80227C40)
 *
 * Move-power modifier: derives a base power via fn_80232110(ctx1, ctx2,
 * sub, moveId, val2f, val30) then scales it by the field-0xD9 object's
 * 0x2b/0x2c byte multipliers; doubles it when flag 0x24 is set and
 * val30==0xd, then applies a 1.5x (*15/10) scale when flag 0x32 is
 * set. Result stored to field 0x2d. PC always advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80227C40(void) {
    extern u32 fn_80205184();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_8011BEB4();
    extern u8 fn_802026E4();
    u32 ctx2;
    u32 ctx1;
    u32 sub;
    u32 moveId;
    u16 val2f;
    u16 val30;
    u32 fieldD9;
    s32 power;
    u8 mult1;
    u8 mult2;

    ctx1 = fn_801F025C(0x11, 0);
    ctx2 = fn_801F025C(0x12, 0);
    sub = fn_801F025C(2, ctx2);
    moveId = fn_80205184(ctx1);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    val2f = (u16)fn_8011BEB4(fieldD9, 0, 0x2f, 0);
    val30 = (u16)fn_8011BEB4(fieldD9, 0, 0x30, 0);
    power = fn_80232110(ctx1, ctx2, sub, moveId, val2f, val30);
    mult1 = (u8)fn_8011BEB4(fieldD9, 0, 0x2b, 0);

    power = power * mult1;
    mult2 = (u8)fn_8011BEB4(fieldD9, 0, 0x2c, 0);
    power = power * mult2;

    if ((u8)fn_802026E4(ctx1, 0x24) == 1 && val30 == 0xd) {
        power = power * 2;
    }
    if ((u8)fn_802026E4(ctx1, 0x32) == 1) {
        power = power * 15 / 10;
    }
    fn_8011BBD8(fieldD9, 0, 0x2d, 0, power);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_80205184
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_8011BBD8
#pragma optimize_for_size reset

/*
 * fn_8022E314 (0x8022E314)
 *
 * Registers fn_8022E34C as a callback via fn_801F37B0(0, fn_8022E34C,
 * &local, 0), where `local` is a stack copy of the caller-supplied
 * byte argument.
 */
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_8022E314(u8 param) {
    extern void fn_801F37B0();
    u8 local = param;
    fn_801F37B0(0, fn_8022E34C, &local, 0);
}
#undef fn_801F37B0

/*
 * fn_80229B70 (0x80229B70)
 *
 * Returns 1 iff fn_801F453C(0,1) reports mode 2 and the moveId's
 * field 9 lookup equals 0x98.
 */
#define fn_8011BEB4 wazaGetStatus
#define fn_801F453C fightFloorGetNowTenkouDataId
u8 fn_80229B70(u32 moveId) {
    extern u32 fn_8011BEB4();
    extern u8 fn_801F453C();
    u16 val = (u16)fn_8011BEB4(0, moveId, 9, 0);

    if ((u8)fn_801F453C(0, 1) == 2 && val == 0x98) {
        return 1;
    }
    return 0;
}
#undef fn_801F453C
#undef fn_8011BEB4

/*
 * fn_80229C28 (0x80229C28)
 *
 * Returns 1 iff flag 0x2b is set on ctx and param's field 0xe lookup
 * equals 1.
 */
#pragma optimize_for_size on
#define fn_8011BEB4 wazaGetStatus
u8 fn_80229C28(void* ctx, u32 param) {
    extern u32 fn_8011BEB4();
    extern u8 fn_802026E4();
    u8 val = (u8)fn_8011BEB4(0, param, 0xe, 0);

    if ((u8)fn_802026E4(ctx, 0x2b) == 1 && val == 1) {
        return 1;
    }
    return 0;
}
#undef fn_8011BEB4
#pragma optimize_for_size reset

/*
 * fn_8022D20C (0x8022D20C)
 *
 * Shadow "reset burst" gate: if ctx is shadow, its move-effect id is
 * 0x1c, and lbl_8047B618 bit 0x4000 is set, clear that bit, fold
 * lbl_8047B628 down to its low 6 bits (remapping 6->2), stash it as
 * lbl_80478D78[3], mark slot 0x4b (ctx) via fn_801F4C14, set
 * lbl_8047B618 bit 0x2000, and fire fn_80211B94 on lbl_80379945. No-op
 * otherwise.
 */
#pragma optimize_for_size on
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
void fn_8022D20C(void* ctx) {
    extern u16 fn_80207BF4();
    extern u32 fn_802062FC();
    u16 val = fn_80207BF4(ctx);
    u8 byte;

    if ((u8)fn_802062FC(ctx) != 0 && val == 0x1c && (lbl_8047B618 & 0x4000) != 0) {
        byte = lbl_8047B628;
        lbl_8047B618 = lbl_8047B618 & ~0x4000;
        byte = byte & 0x3f;
        lbl_8047B628 = byte;
        if (byte == 6) {
            lbl_8047B628 = 2;
        }
        lbl_80478D78[3] = lbl_8047B628;
        fn_801F4C14(0, 0, 0x4b, 0, (u32)ctx);
        lbl_8047B618 = lbl_8047B618 | 0x2000;
        fn_80211B94(lbl_8047B62C, (void*)&lbl_80379945, 0);
    }
}
#undef fn_80207BF4
#undef fn_802062FC
#undef fn_801F4C14
#pragma optimize_for_size reset

/*
 * fn_8022D2CC (0x8022D2CC)
 *
 * Same gate/body as fn_8022D20C (ctx is the SECOND param; the first is
 * unused), except the lbl_80478D78[3] stash adds 0x40 to the folded
 * lbl_8047B628 value.
 */
#pragma optimize_for_size on
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
void fn_8022D2CC(void* unused, void* ctx) {
    extern u16 fn_80207BF4();
    extern u32 fn_802062FC();
    u16 val = fn_80207BF4(ctx);
    u8 byte;

    if ((u8)fn_802062FC(ctx) != 0 && val == 0x1c && (lbl_8047B618 & 0x4000) != 0) {
        byte = lbl_8047B628;
        lbl_8047B618 = lbl_8047B618 & ~0x4000;
        byte = byte & 0x3f;
        lbl_8047B628 = byte;
        if (byte == 6) {
            lbl_8047B628 = 2;
        }
        lbl_80478D78[3] = lbl_8047B628 + 0x40;
        fn_801F4C14(0, 0, 0x4b, 0, (u32)ctx);
        lbl_8047B618 = lbl_8047B618 | 0x2000;
        fn_80211B94(lbl_8047B62C, (void*)&lbl_80379945, 0);
    }
}
#undef fn_80207BF4
#undef fn_802062FC
#undef fn_801F4C14
#pragma optimize_for_size reset

/*
 * fn_8022D084 (0x8022D084)
 *
 * Shadow reset gate: derives val1/val30 via fn_802040E8/fn_80203FE4,
 * runs fn_80203EDC(ctx) for side effect, and resolves tmp via
 * fn_801F4354(0,ctx). If ctx isn't shadow, returns 0. Otherwise marks
 * slot 0x56 (val1); if val30==0x20, applies fn_801FAA58(tmp,0,0x48,0,2);
 * if val30==0x17, walks the 7-entry lbl_80279FD0 message table
 * resetting any field below 6 back to 6 (fn_801254B4), and if any were
 * reset, marks slots 0x4b/0x36/0x49 and fires fn_80211B94 on
 * lbl_80379B06. Returns 5 if any field was reset, else 0.
 */
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_80203FE4 fightOutPokemonGetSoubiItemSoubiDataId
#define fn_80203EDC figthOutPokemonGetSoubiItemBuff
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
#define fn_801FAA58 fightTrainerSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801254B4 pokemonSetStatus
u8 fn_8022D084(void* ctx) {
    extern u16 fn_802040E8();
    extern u16 fn_80203FE4();
    extern void fn_80203EDC();
    extern void* fn_801F4354();
    extern u32 fn_802062FC();
    extern u8 fn_801F4C14();
    extern void fn_801FAA58();
    extern s32   fn_8012640C();
    extern void fn_801254B4();
    u16 val30;
    u16 val1;
    u8 result;
    void* tmp;
    u8 i;

    result = 0;
    val1 = fn_802040E8();
    val30 = fn_80203FE4(ctx);
    fn_80203EDC(ctx);
    tmp = (void*)fn_801F4354(0, ctx);

    if ((u8)fn_802062FC(ctx) == 0) {
        return 0;
    }
    fn_801F4C14(0, 0, 0x56, 0, val1);

    switch (val30) {
    case 0x20:
        fn_801FAA58(tmp, 0, 0x48, 0, 2);
        break;
    case 0x17:
        for (i = 0; i < 7; i++) {
            if (fn_8012640C(ctx, 0, lbl_80279FD0[i], 0) < 6) {
                fn_801254B4(ctx, 0, lbl_80279FD0[i], 0, 6);
                result = 5;
            }
        }
        if (result != 0) {
            fn_801F4C14(0, 0, 0x4b, 0, (u32)ctx);
            fn_801F4C14(0, 0, 0x36, 0, (u32)ctx);
            fn_801F4C14(0, 0, 0x49, 0, (u32)ctx);
            fn_80211B94(lbl_8047B62C, (void*)&lbl_80379B06, 0);
        }
        break;
    }
    return result;
}
#undef fn_802040E8
#undef fn_80203FE4
#undef fn_80203EDC
#undef fn_801F4354
#undef fn_802062FC
#undef fn_801F4C14
#undef fn_801FAA58
#undef fn_8012640C
#undef fn_801254B4

/* ===== 0x802175A8-0x8021D688 handlers (wave2-w1 worker) ===== */
extern void fn_8020A2B8();
extern u8   fn_80119DD0();
extern u16  fightFloorGetValidFightOutPokemonCount(u32, u8, u32, u8);
extern u8   fn_802016A4();
extern u8   lbl_8037984D[27];
extern u8   lbl_80375FDF[9];
extern void heroAddPokedoru(u32, s32);
extern u8   lbl_8037939C[9];

/*
 * fn_8021B628 (0x8021B628)
 *
 * Reads the script-embedded operand byte at PC+1: if nonzero, uses it
 * directly as field 0x31's value; otherwise rolls a random 2..5 (with
 * a re-roll on the low half of the range) via fn_800E0C54 % 4. PC
 * always advances by 2 (opcode + 1 operand byte).
 */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_8021B628(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8 flag = *(u8*)(lbl_8047B610 + 1);
    s32 roll;
    u8 val;

    if (flag != 0) {
        fn_8011BBD8(fieldD9, 0, 0x31, 0, flag);
    } else {
        roll = fn_800E0C54();
        val = roll % 4;
        if (val < 2) {
            val = val + 2;
        } else {
            roll = fn_800E0C54();
            val = roll % 4 + 2;
        }
        fn_8011BBD8(fieldD9, 0, 0x31, 0, val);
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_8011BBD8
#undef fn_8012640C
#undef fn_801F025C

/*
 * fn_8021B760 (0x8021B760)
 *
 * If state 0xc on slot-0x11 is 2, clears states 0xc/0x22; unconditionally
 * copies field 0xF8 into field 0xD9 via fn_8020A2B8 and marks slot 0xf5
 * changed. PC always advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8020A2B8 fightWazaBiosCopy
#define fn_801254B4 pokemonSetStatus
void fn_8021B760(void) {
    extern void fn_8020A2B8();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_801254B4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 fieldF8 = fn_8012640C(ctx1, 0, 0xF8, 0);

    if (fn_802025B8(ctx1, 0xc) == 2) {
        fn_8020248C(ctx1, 0xc, 0);
        fn_8020248C(ctx1, 0x22, 0);
    }
    fn_8020A2B8(fieldF8, fieldD9);
    fn_801254B4(ctx1, 0, 0xf5, 0, 0);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8020A2B8
#undef fn_801254B4
#pragma optimize_for_size reset

/*
 * fn_8021C490 (0x8021C490)
 *
 * If field 0x2d is set, compares its value against fn_80119DD0(0x2d);
 * on a match sends message slot 0x3b and marks lbl_80478D78[5]=1.
 * Otherwise clears state 0x2d if pending and copies field 0x2d into
 * field 0x2f, marking lbl_80478D78[5]=0. PC always advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_80132A38 msgctrlSetValue
void fn_8021C490(void) {
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern void fn_80132A38();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    u32 ctx1 = fn_801F025C(0x11, 0);
    s16 field2d;

    if (fn_802026E4(ctx1, 0x2d) == 0) {
        field2d = 0;
    } else {
        field2d = fn_80202360(ctx1, 0x2d);
    }
    if (field2d == fn_80119DD0(0x2d)) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 1;
    } else {
        if (fn_802025B8(ctx1, 0x2d) == 2) {
            fn_8020248C(ctx1, 0x2d, 0);
        }
        field2d = fn_80202360(ctx1, 0x2d);
        fn_80132A38(0x2f, field2d);
        lbl_80478D78[5] = 0;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_80132A38
#undef fn_801F4C14
#undef fn_801F025C
#pragma optimize_for_size reset

/*
 * fn_8021AFAC (0x8021AFAC)
 *
 * If fn_801F6E44(ctx2,0x49)==2, clears state 0x49 and, if both the
 * count of eligible switch-in candidates (fn_801F54A4) and the valid
 * fight-out count are >=2, marks lbl_80478D78[5]=4, else 3. Otherwise
 * sends message slot 0x3b and marks lbl_80478D78[5]=0. PC always
 * advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
#define fn_801F54A4 fightFloorGetStatus
void fn_8021AFAC(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2 = fn_801F025C(2, ctx1);
    u16 count = fightFloorGetValidFightOutPokemonCount(0, 1, ctx1, 1);

    if (fn_801F6E44(ctx2, 0x49) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 0;
    } else {
        u16 x;
        fn_801F6DF0(ctx2, 0x49, 0);
        x = fn_801F54A4(0, 0, 0x19, 0);
        if (x < 2) {
            goto setThree;
        }
        if (count < 2) {
            goto setThree;
        }
        lbl_80478D78[5] = 4;
        goto doneAfac;
    setThree:
        lbl_80478D78[5] = 3;
    doneAfac:;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#pragma optimize_for_size reset

/*
 * fn_8021CB58 (0x8021CB58)
 *
 * If fn_801F6E44(ctx2,0x48)==2, clears state 0x48 and, if both the
 * count of eligible switch-in candidates (fn_801F54A4) and the valid
 * fight-out count are >=2, marks lbl_80478D78[5]=2, else 1. Otherwise
 * sends message slot 0x3b and marks lbl_80478D78[5]=0. PC always
 * advances by 1.
 */
#pragma optimize_for_size on
void fn_8021CB58(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2 = fn_801F025C(2, ctx1);
    u16 count = fightFloorGetValidFightOutPokemonCount(0, 1, ctx1, 1);

    if (fn_801F6E44(ctx2, 0x48) != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 0;
    } else {
        u16 x;
        fn_801F6DF0(ctx2, 0x48, 0);
        x = fn_801F54A4(0, 0, 0x19, 0);
        if (x < 2) {
            goto setOne;
        }
        if (count < 2) {
            goto setOne;
        }
        lbl_80478D78[5] = 2;
        goto doneCb58;
    setOne:
        lbl_80478D78[5] = 1;
    doneCb58:;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_801F6E44
#undef fn_801F4C14
#undef fn_801F6DF0
#undef fn_801F54A4
#pragma optimize_for_size reset

/*
 * fn_802177E4 (0x802177E4)
 *
 * Rolls fn_800E0C54() % 100 and picks a (value, category) pair from a
 * fixed ascending-threshold table, writing value into field 0x2f of
 * the field-0xD9 object and recording category via fn_80132A38(0x2f,
 * category). PC always advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
#define fn_80132A38 msgctrlSetValue
void fn_802177E4(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern void fn_80132A38();
    u32 ctx = fn_801F025C(0x11, 0);
    u8 idx;
    u32 fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    u8 v = fn_800E0C54() % 100;
    u16 result;

    if (v < 5) {
        result = 10;
        idx = 4;
    } else if (v < 15) {
        result = 30;
        idx = 5;
    } else if (v < 35) {
        result = 50;
        idx = 6;
    } else if (v < 65) {
        result = 70;
        idx = 7;
    } else if (v < 85) {
        result = 90;
        idx = 8;
    } else if (v < 95) {
        result = 110;
        idx = 9;
    } else {
        result = 150;
        idx = 10;
    }
    fn_8011BBD8(fieldD9, 0, 0x2f, 0, result);
    fn_80132A38(0x2f, idx);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BBD8
#undef fn_80132A38
#pragma optimize_for_size reset

/*
 * fn_8021A878 (0x8021A878)
 *
 * Resolves a new switch-in poke for slot 0x11 and compares each side's
 * status (fn_802016A4). If slot-0x12's fn_80207BF4 reads 0xc, jumps to
 * a fixed label (lbl_8037984D). Otherwise, if the statuses match, or
 * state 0xa isn't 2, or slot-0x11's status is 2, takes the
 * script-embedded jump; else if slot-0x12's status isn't 2, applies
 * the switch-in via fn_8020248C and advances PC by 5; else takes the
 * script-embedded jump.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802016A4 fightOutPokemonGetSex
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
void fn_8021A878(void) {
    extern u8 fn_802016A4();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern u32 fn_801F0134();
    extern u16 fn_80207BF4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u8 statusA = fn_802016A4(ctx1);
    u16 cand = fn_801F54A4(0, 0, 0x14, 0);
    u32 ctx2;
    u32 newPoke = fn_801F0134(ctx1, cand);
    u8 statusB;
    u16 res;
    ctx2 = fn_801F025C(0x12, 0);
    statusB = fn_802016A4(ctx2);
    res = fn_80207BF4(ctx2);

    if (res == 0xc) {
        lbl_8047B610 = (u8*)&lbl_8037984D;
        return;
    }
    if (statusA == statusB) {
        goto takeJump;
    }
    if (fn_802025B8(ctx2, 0xa) != 2) {
        goto takeJump;
    }
    if (statusA == 2) {
        goto takeJump;
    }
    if (statusB != 2) {
        goto doCall;
    }
takeJump:
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
doCall:
    fn_8020248C(ctx2, 0xa, newPoke);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_802016A4
#undef fn_801F54A4
#undef fn_801F0134
#undef fn_80207BF4
#pragma optimize_for_size reset

/*
 * fn_80217C04 (0x80217C04)
 *
 * If field 0xD9's fn_802096E8 flag is clear, clears state 0x2e (if
 * pending) and jumps to a fixed label. Otherwise, if state 0x2e is 2,
 * clears it; then doubles fn_8011BEB4(0,poke,7,0) field2e-1 times and
 * writes the result to field 0x2f. PC always advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_802096E8 fightWazaIsHit
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80217C04(void) {
    extern void fn_80202810();
    extern void fn_8011BBD8();
    extern u32 fn_80205184();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    extern u8 fn_802096E8();
    extern s32 fn_8011BEB4();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 poke1 = fn_80205184(ctx1);
    u8 flag = fn_802096E8(fieldD9);
    s16 field2e;
    u16 n;
    u32 result;
    s32 i;

    if (flag == 0) {
        if (fn_802026E4(ctx1, 0x2e) == 1) {
            fn_80202810(ctx1, 0x2e);
        }
        lbl_8047B610 = (u8*)lbl_80375FDF;
        return;
    }
    if (fn_802025B8(ctx1, 0x2e) == 2) {
        fn_8020248C(ctx1, 0x2e, 0);
    }
    field2e = fn_80202360(ctx1, 0x2e);
    n = fn_8011BEB4(0, poke1, 7, 0);
    result = n;
    for (i = 1; i < field2e; i++) {
        result = result * 2;
    }
    fn_8011BBD8(fieldD9, 0, 0x2f, 0, result);
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205184
#undef fn_802096E8
#undef fn_80202810
#undef fn_8011BEB4
#undef fn_8011BBD8
#pragma optimize_for_size reset

/*
 * fn_8021B0B0 (0x8021B0B0)
 *
 * If fn_801F54A4(0,0,0x25,0)==1 and a hero handle, its field-0x49 and
 * field-0x48 values resolve, computes their product and, if the
 * field-0x44 value also resolves, awards Poke Coupons, records fields
 * 0x2f/0x13, and reports via fn_80211B94. PC always advances by 1.
 */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F2A7C fightFloorGetGcHeroFightTrainerPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_8012A5B0 heroGetStatus
void fn_8021B0B0(void) {
    extern void fn_80132A38();
    extern u8 fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern s32 fn_801F2A7C();
    extern s32 fn_8012A5B0();
    u32 handle;
    u32 c;
    u8 b;
    u32 a;

    if (fn_801F54A4(0, 0, 0x25, 0) != 1) {
        goto skipB0B0;
    }
    handle = fn_801F2A7C(0);
    if (handle == 0) {
        goto skipB0B0;
    }
    a = fn_801FB1C0(handle, 0, 0x49, 0);
    if (a == 0) {
        goto skipB0B0;
    }
    b = fn_801FB1C0(handle, 0, 0x48, 0);
    a = a * b;
    c = fn_801FB1C0(handle, 0, 0x44, 0);
    if (c == 0) {
        goto skipB0B0;
    }
    heroAddPokedoru(c, a);
    fn_80132A38(0x2f, a);
    fn_80132A38(0x13, fn_8012A5B0(c, 1, 0));
    fn_80211B94(lbl_8047B62C, lbl_8037939C, 0);
skipB0B0:
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F54A4
#undef fn_801F2A7C
#undef fn_801FB1C0
#undef fn_80132A38
#undef fn_8012A5B0
#pragma optimize_for_size reset

/* ===== reconciliation: fns from w0 worktree + new origin/master ===== */

/*
 * fn_802182D4 (0x802182D4)
 *
 * If field 0x4a of the slot-3-relative-to-slot-0x11 context reads 2,
 * compare its recorded value (fn_801F6D9C if set, else 0) against
 * fn_80119DD0(0x4a); a mismatch clears the field and advances the PC
 * by 5. Otherwise (including the initial field != 2 case) marks field
 * 0x118 of slot-0x11 changed and takes the script-embedded jump.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E44 fightSideCheckWriteJoutaiDataId
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6D9C fightSideGetCountAsJoutaiDataId
#define fn_801254B4 pokemonSetStatus
#define fn_801F6DF0 fightSideWriteJoutaiDataId
void fn_802182D4(void) {
    extern u8 fn_801F6E44(u32, u32);
    extern void fn_801F6DF0(u32, u32, u32);
    extern u32 fn_801F025C();
    extern u32 fn_801254B4();
    extern u8  fn_801F6E98();
    extern s16 fn_801F6D9C();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 tmp = fn_801F025C(3, ctx1);
    s16 val;

    if (fn_801F6E44(tmp, 0x4a) != 2) {
        goto matched;
    }
    if (fn_801F6E98(tmp, 0x4a) == 1) {
        val = fn_801F6D9C(tmp, 0x4a);
    } else {
        val = 0;
    }
    if (val != fn_80119DD0(0x4a)) {
        goto notmatched;
    }
matched:
    fn_801254B4(ctx1, 0, 0x118, 0, 1);
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    return;
notmatched:
    fn_801F6DF0(tmp, 0x4a, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_801F6E44
#undef fn_801F6E98
#undef fn_801F6D9C
#undef fn_801254B4
#undef fn_801F6DF0
#pragma optimize_for_size reset

/*
 * fn_80218BD4 (0x80218BD4)
 *
 * If the slot-0x12 side has flag 0x15 set, its slot-2-relative
 * sub-context differs from the slot-0x11 side's, and flag bit
 * 0x1000000 of lbl_8047B618 is clear, sets flag bit 0x40 of
 * lbl_8047B618. PC always advances by 1.
 */
/* TODO(0x80218BD4): 97.1% here and in origin/master's copy alike -- one
 * persistent-register hop on the second context resists all source forms. */
#pragma optimize_for_size on
inline u32 inline_fn_80218BD4() {
    return fightTargetGetPtrAsNowFightType(0x12, 0);
}

void fn_80218BD4(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fn_802026E4();
    u32 sub1;
    u32 ctx2;
    u32 sub2;

    sub1 = fightTargetGetPtrAsNowFightType(2, fightTargetGetPtrAsNowFightType(0x11, 0));
    ctx2 = inline_fn_80218BD4();
    sub2 = fightTargetGetPtrAsNowFightType(2, ctx2);

    if (fn_802026E4(ctx2, 0x15) == 1 && sub1 != sub2 && (lbl_8047B618 & 0x1000000) == 0) {
        sub1 = 0x40;
        lbl_8047B618 = lbl_8047B618 | sub1;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#pragma optimize_for_size reset

/*
 * fn_80222ADC (0x80222ADC)
 *
 * Decrements field 0x31 of the field-0xD9 object (clamped at 0): a
 * nonzero result after decrementing takes the script-embedded jump
 * and a zero result advances the PC by 5. Either way the decremented
 * value is written back to field 0x31.
 */
/* TODO(0x80222ADC): 99.6% here and in origin/master's copy alike -- single
 * subi/extsh register swap (r0/r4), the documented clamp peephole limit. */
#pragma optimize_for_size on
void fn_80222ADC(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern s32 wazaGetStatus();
    extern void wazaSetStatus();
    u32 ctx1 = fightTargetGetPtrAsNowFightType(0x11, 0);
    u32 fieldD9 = pokemonGetStatus(ctx1, 0, 0xD9, 0);
    s16 val = (u8)wazaGetStatus(fieldD9, 0, 0x31, 0);

    val--;

    if (val < 0) {
        val = 0;
    }
    if (val == 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
    wazaSetStatus(fieldD9, 0, 0x31, 0, val);
}
#pragma optimize_for_size reset

/* ===== wave3-d: cracked near-misses ===== */

/*
 * fn_80224820 (0x80224820)
 *
 * Move-effect accuracy/flinch-style roll: closes the status menus,
 * derives a base power (doubled if the move-effect id is 0x20, else a
 * plain field-0xc lookup), then unconditionally applies a
 * fn_8000817C-gated override to 0x63. If lbl_80478D78[3] bit 0x80 is
 * set and the mon is shadow (fn_802096E8), clears the bit and fires
 * fn_802249B8(0,0x80) without advancing the PC (fn_802249B8 owns PC on
 * that path). Otherwise rolls fn_800E0C54()%100 against power; on
 * success, if lbl_80478D78[3] is nonzero and the mon is shadow, fires
 * fn_802249B8(0, power>=100 ? 0x80 : 0) (again without a PC bump). All
 * other paths advance PC by 1 and clear lbl_80478D78[3]/lbl_8047B625.
 */
#pragma optimize_for_size on
#define fn_8026246C fightMenuCloseMsg
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_802096E8 fightWazaIsHit
void fn_80224820(void) {
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fn_8026246C();
    extern u32  fn_801F025C();
    extern u32  fn_8012640C();
    extern u32  fn_80205184();
    extern u16  fn_80207BF4();
    extern s32  fn_8011BEB4();
    extern u8   fn_8000817C(void);
    extern u8   fn_802096E8();
    extern void fn_802249B8();
    extern u16  fn_800E0C54(void);
    extern u8   lbl_8047B625;

    u32 ctx;
    u32 fieldD9;
    u32 moveId;
    u16 val;
    u16 power;

    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fn_8026246C();

    ctx = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    moveId = fn_80205184(ctx);
    val = fn_80207BF4(ctx);

    if (val == 0x20) {
        power = (u8)fn_8011BEB4(0, moveId, 0xc, 0) * 2;
    } else {
        power = (u8)fn_8011BEB4(0, moveId, 0xc, 0);
    }
    if ((u8)fn_8000817C() == 1) {
        power = 0x63;
    }

    if ((lbl_80478D78[3] & 0x80) != 0 && (u8)fn_802096E8(fieldD9) == 1) {
        lbl_80478D78[3] = lbl_80478D78[3] & 0x7f;
        fn_802249B8(0, 0x80);
        goto skipIncrement;
    }

    if ((u16)fn_800E0C54() % 100 <= power) {
        if (lbl_80478D78[3] != 0 && (u8)fn_802096E8(fieldD9) == 1) {
            if (power >= 100) {
                fn_802249B8(0, 0x80);
            } else {
                fn_802249B8(0, 0);
            }
            goto skipIncrement;
        }
    }

    lbl_8047B610 = lbl_8047B610 + 1;

skipIncrement:
    lbl_80478D78[3] = 0;
    lbl_8047B625 = 0;
}
#undef fn_8026246C
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205184
#undef fn_80207BF4
#undef fn_8011BEB4
#undef fn_802096E8
#pragma optimize_for_size reset

/*
 * fn_8022DCB8 (0x8022DCB8)
 *
 * Shadow move-warning dispatcher: gates on p2 being shadow and extra
 * being a "real" message id (not 0/0x165). Dispatches on p2's
 * move-effect id (val26): 0xb or 0xa require a matching field-0x111
 * "kind" (val27, 0xb vs 0xd respectively) and a nonzero field-7 lookup
 * on extra (val25) to pick between lbl_80379714/lbl_80379715 (by
 * val31) and set result=1; 0x12 requires kind 0xa and flag 7, applies
 * event-state 0x3a bookkeeping, picks lbl_80379783/lbl_80379784 (by
 * val31), and sets result=2. When result==1, fn_80201704(p2)==1
 * instead redirects the jump to lbl_80379752/lbl_80379753 (by val31)
 * and returns immediately; otherwise it records -fn_80203B5C(p2,4)
 * into field 0x2d of p1's field-0xD9 object. Returns 0/1/2.
 */
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_80205134 fightOutPokemonGetWazaZokuseiDataId
#define fn_8012640C pokemonGetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
u8 fn_8022DCB8(u32 p1, u32 p2, u32 extra) {
    extern u16 fn_80207BF4();
    extern s32 fn_8011BEB4();
    extern u16 fn_80205134();
    extern u32 fn_8012640C();
    extern u32 fn_802062FC();
    extern u8  fn_802026E4();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    extern u8  lbl_80379714[];
    extern u8  lbl_80379715[];
    extern u8  lbl_80379752[];
    extern u8  lbl_80379753[];
    extern u8  lbl_80379783[];
    extern u8  lbl_80379784[];
    extern u8  fn_80201704();
    extern void fn_8011BBD8();
    extern u16 fn_80203B5C();

    u16 val26 = fn_80207BF4(p2);
    u16 val25 = (u16)fn_8011BEB4(0, extra, 7, 0);
    u16 val27 = fn_80205134(p1);
    u8 val31 = (u8)fn_8012640C(p1, 0, 0x111, 0);
    u8 result = 0;
    u32 fieldD9 = fn_8012640C(p1, 0, 0xD9, 0);
    u16 val4;

    if ((u8)fn_802062FC(p2) == 0) {
        return 0;
    }
    if ((u16)extra != 0 && (u16)extra != 0x165) {
        switch (val26) {
        case 0xa:
            if (val27 == 0xd && val25 != 0) {
                lbl_8047B610 = (val31 != 0) ? (u8*)&lbl_80379715 : (u8*)&lbl_80379714;
                result = 1;
            }
            break;
        case 0xb:
            if (val27 == 0xb && val25 != 0) {
                lbl_8047B610 = (val31 != 0) ? (u8*)&lbl_80379715 : (u8*)&lbl_80379714;
                result = 1;
            }
            break;
        case 0x12:
            if (val27 == 0xa && (u8)fn_802026E4(p2, 7) == 0) {
                if ((u8)fn_802025B8(p2, 0x3a) == 2) {
                    fn_8020248C(p2, 0x3a, 0);
                    lbl_80478D78[5] = 0;
                } else {
                    lbl_80478D78[5] = 1;
                }
                lbl_8047B610 = (val31 != 0) ? (u8*)&lbl_80379784 : (u8*)&lbl_80379783;
                result = 2;
            }
            break;
        }

        if (result == 1) {
            if ((u8)fn_80201704(p2) == 1) {
                if (val31 != 0) {
                    lbl_8047B610 = (u8*)&lbl_80379753;
                } else {
                    lbl_8047B610 = (u8*)&lbl_80379752;
                }
                goto done;
            }
            fn_8011BBD8(fieldD9, 0, 0x2d, 0, -fn_80203B5C(p2, 4));
        }
    }
done:
    return result;
}
#undef fn_80207BF4
#undef fn_8011BEB4
#undef fn_80205134
#undef fn_8012640C
#undef fn_802062FC
#undef fn_80201704
#undef fn_80203B5C
#undef fn_8011BBD8

/*
 * fn_8022EC40 (0x8022EC40)
 *
 * Shadow-move-slot resolver: returns 0 unless ctx is shadow, its
 * field-0x181 lookup is 0x3b, and the move-effect id is 0x181 (both
 * gate the whole function). Then picks a "kind" via fn_801F453C(0,1)
 * and, for whichever of {0|3, 1, 2, 4} it matches, checks
 * fn_80207AE0(ctx, subId); if unset, fills 2 slots via
 * fn_80207B5C(ctx, 0/1, subId) and returns a fixed code (1/2/3/4).
 * Falls through with count 0 if none matched or the field was already
 * set.
 */
#pragma optimize_for_size on
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80203D3C figthOutPokemonGetPokemonDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_80207B5C fightOutPokemonSetZokuseiDataId
u8 fn_8022EC40(void* ctx) {
    extern u32 fn_802062FC();
    extern u16 fn_80203D3C();
    extern u16 fn_80207BF4();
    extern u8  fn_801F453C();
    extern u32 fn_80207AE0();
    extern u32 fn_80207B5C(void*, u8, u32);

    u16 x;
    u16 y;
    u8 kind;
    u8 count;

    if ((u8)fn_802062FC(ctx) == 0) {
        return 0;
    }
    x = fn_80203D3C(ctx);
    y = fn_80207BF4(ctx);
    if (x != 0x181 || y != 0x3b) {
        return 0;
    }

    kind = (u8)fn_801F453C(0, 1);
    count = 0;

    if (kind == 0 || kind == 3) {
        if ((u8)fn_80207AE0(ctx, 0) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0);
            }
            count = 1;
        }
    }
    if (kind == 1) {
        if ((u8)fn_80207AE0(ctx, 0xa) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0xa);
            }
            count = 2;
        }
    }
    if (kind == 2) {
        if ((u8)fn_80207AE0(ctx, 0xb) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0xb);
            }
            count = 3;
        }
    }
    if (kind == 4) {
        if ((u8)fn_80207AE0(ctx, 0xf) == 0) {
            for (count = 0; count < 2; count++) {
                fn_80207B5C(ctx, count, 0xf);
            }
            count = 4;
        }
    }
    return count;
}
#undef fn_802062FC
#undef fn_80203D3C
#undef fn_80207BF4
#undef fn_801F453C
#undef fn_80207AE0
#undef fn_80207B5C
#pragma optimize_for_size reset

/* ===== wave3-e ===== */

/*
 * fn_802357CC (0x802357CC)
 * val = ctx's field 0xec read via param1; if ctx's field 0x24 (through the
 * slot resolve) reads 1 and field 0x19 predicate on param1 also reads 1,
 * clamp val to max 6.
 */
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8012640C pokemonGetStatus
u8 fn_802357CC(u32 ctx, u32 param1) {
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    extern u8  fn_802026E4();
    u16 slot1;
    u16 slot2;
    u8 val;
    u8 x;

    slot1 = (u16)fn_801FB1C0(ctx, 0, 0x43, 0);
    fn_801FB1C0(0, slot1, 2, 0);
    val = (u8)fn_8012640C(param1, 0, 0xec, 0);

    slot2 = (u16)fn_801FB1C0(ctx, 0, 0x43, 0);
    slot2 = (u16)fn_801FB1C0(0, slot2, 2, 0);

    if ((u8)fn_801FB1C0(0, slot2, 0x24, 0) == 1) {
        x = fn_802026E4(param1, 0x19);
    } else {
        x = 0;
    }

    if (x == 1) {
        if (val > 6) {
            val = 6;
        }
    }
    return val;
}
#undef fn_801FB1C0
#undef fn_8012640C
#pragma optimize_for_size reset

/*
 * fn_80235B04 (0x80235B04)
 * ret = fn_801F453C(arg2, 0). If arg3==1: build a 5-word message
 * {0xd,0,0,0,ctx} and dispatch via fn_801F37B0(arg2, fn_80236268, &msg, 0);
 * if msg[1] != 0, return 0. Otherwise (nested, only reached when the first
 * check passed) build a second message {0x4d,0,0,0,ctx} the same way; if
 * ITS msg[1] != 0, return 0. Falls through to return ret.
 */
#pragma optimize_for_size on
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
u8 fn_80235B04(void* ctx, u32 arg2, u32 arg3) {
    extern u8 fn_801F453C();
    extern u32 fn_801F37B0();
    extern void fn_80236268();
    u8 ret = fn_801F453C(arg2, 0);
    if ((u8)arg3 == 1) {
        u32 msg1[5];
        msg1[0] = 0xd;
        msg1[1] = 0;
        msg1[2] = 0;
        msg1[3] = 0;
        msg1[4] = (u32)ctx;
        fn_801F37B0(arg2, fn_80236268, &msg1[0], 0);
        if (msg1[1] != 0) {
            return 0;
        }
        {
            u32 msg2[5];
            msg2[0] = 0x4d;
            msg2[1] = 0;
            msg2[2] = 0;
            msg2[3] = 0;
            msg2[4] = (u32)ctx;
            fn_801F37B0(arg2, fn_80236268, &msg2[0], 0);
            if (msg2[1] != 0) {
                return 0;
            }
        }
    }
    return ret;
}
#undef fn_801F37B0
#undef fn_801F453C
#pragma optimize_for_size reset

/* ===== wave3-g ===== */

/* fn_8023D158 (0x8023D158): seqs 0x217 / 0x218 / 0x219.
 * Verified 100.0% match. */
#pragma optimize_for_size on
u32 fn_8023D158(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_80236FFC(void*, u32);
    extern u16 fn_8023715C(void*, u32);
    extern u16 fn_80236520(void*, u32);
    extern u16 fn_801F1C18(u32, void*, void*, u32, u32);
    extern u16 fn_802377E8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[8];
    u32 handle = 0;
    u16 a = fn_80236FFC(ctx, param3);
    u16 b = fn_8023715C(ctx, param3);
    u16 c = fn_80236520(ctx, param3);
    u16 count;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 0, 1);
    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, arr[i]);
        if (v == 0xca || v == 0x168 || v == 0x12f || v == 0xd5) {
            handle = fn_80239984(0, ctx, 0x217);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x217);
            break;
        }
    }
    if (b > a) {
        handle = fn_80239984(handle, ctx, 0x218);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x218);
    }
    if (c == 0x11f) {
        handle = fn_80239984(handle, ctx, 0x219);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x219);
    }
    return handle;
}
#pragma optimize_for_size reset

/* ===== wave3-b ===== */

/*
 * fn_8021DF80 (0x8021DF80)
 *
 * FightSeq opcode handler with two operand bytes: a slot byte at PC+1
 * and a u32 mode operand at PC+3, expected to be 7. If flag bit 0x80 of
 * lbl_8047B618 is clear and the mode is 7, resolves a switch-in
 * candidate (fn_801F54A4) and the slot ctx's field 0xee; if that field
 * is set and fn_801DDD28 accepts it, applies fn_801DA9E8 and
 * fn_80265598. PC always advances by 7 (no script-embedded jump).
 */
extern u8  fn_801DDD28();
extern void fn_801DA9E8();
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F54A4 fightFloorGetStatus
#define fn_8012640C pokemonGetStatus
void fn_8021DF80(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_801F54A4();
    u8 slot = *(u8*)(lbl_8047B610 + 1);
    u16 cand;
    u32 fieldEE;
    u32 ctx = fn_801F025C(slot, 0);
    u32 operand = *(u32*)(lbl_8047B610 + 3);

    if ((lbl_8047B618 & 0x80) != 0) {
        goto skip;
    }
    if (operand != 7) {
        goto skip;
    }
    cand = fn_801F54A4(0, 0, 0x14, 0);
    fieldEE = fn_8012640C(ctx, 0, 0xee, 0);
    if (fieldEE == 0) {
        goto skip;
    }
    if (fn_801DDD28(fieldEE, 0x34, 4, 0) == 0) {
        goto skip;
    }
    fn_801DA9E8(fieldEE, 0x34, 4);
    fn_80265598(ctx, cand, 1);
skip:
    lbl_8047B610 = lbl_8047B610 + 7;
}
#undef fn_8012640C
#undef fn_801F54A4
#undef fn_801F025C
#pragma optimize_for_size reset

/* ===== wave3-a ===== */

/* fn_80215C70: bytecode op -- reads slot-0x11/0x12 contexts; if the
 * slot-0x12 fn_80207BF4() value is 0 or 0x19, takes the script-embedded
 * jump; else applies it via fn_80207BC0(ctx1, val), PC += 5. */
void fn_80215C70(void) {
    extern u32 fn_801F025C();
    extern u32 fn_80207BF4();
    extern u32 fn_80207BC0();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 val;
    u16 v;

    fn_80207BF4(ctx1);
    val = fn_80207BF4(fn_801F025C(0x12, 0));
    v = (u16)val;

    if (v != 0 && v != 0x19) {
        fn_80207BC0(ctx1, val);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}

/* fn_80216780: bytecode op -- if fn_801F2988(0,0x52) == 2, applies
 * fn_801F2934(0,0x52,0) and marks lbl_80478D78[5]=5; else stashes 0x40
 * into slot 0x3b (fn_801F4C14) and marks lbl_80478D78[5]=2. PC += 1. */
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
void fn_80216780(void) {
    extern u32 fn_801F2988();
    extern void fn_801F2934();
    extern void fn_801F4C14();
    u8 val = (u8)fn_801F2988(0, 0x52);

    if (val != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x52, 0);
        lbl_80478D78[5] = 5;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988

/* fn_80217524: bytecode op -- if fn_801F2988(0,0x53) == 2, applies
 * fn_801F2934(0,0x53,0) and marks lbl_80478D78[5]=4; else stashes 0x40
 * into slot 0x3b (fn_801F4C14) and marks lbl_80478D78[5]=2. PC += 1. */
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
void fn_80217524(void) {
    extern u32 fn_801F2988();
    extern void fn_801F2934();
    extern void fn_801F4C14();
    u8 val = (u8)fn_801F2988(0, 0x53);

    if (val != 2) {
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
        lbl_80478D78[5] = 2;
    } else {
        fn_801F2934(0, 0x53, 0);
        lbl_80478D78[5] = 4;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988

/* fn_802170B4: bytecode op -- stashes ctx1 into slot 0x43, selects a
 * stat via fn_801F453C(0,1): 0->field2, 1->(field1*20)/30, else->field4
 * (all fn_80203B5C(ctx1,n)); if fn_80201704(ctx1), takes the
 * script-embedded jump; else applies -val via fn_8011BBD8(fieldD9,0,
 * 0x2d,0,-val), PC += 5. */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_802170B4(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_801F4C14();
    extern u8 fn_801F453C();
    extern u16 fn_80203B5C();
    extern u8 fn_80201704();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8 sel;
    s32 val;

    fn_801F4C14(0, 0, 0x43, 0, ctx1);
    sel = (u8)fn_801F453C(0, 1);

    if (!fn_80201704(ctx1)) {
        if (sel == 0) {
            val = (u16)fn_80203B5C(ctx1, 2);
        } else if (sel == 1) {
            val = (fn_80203B5C(ctx1, 1) * 20) / 30;
        } else {
            val = (u16)fn_80203B5C(ctx1, 4);
        }
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(s32)val);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_801F4C14
#undef fn_801F453C
#undef fn_80201704
#undef fn_80203B5C
#undef fn_8011BBD8
#pragma optimize_for_size reset

/* fn_8021799C: bytecode op -- picks a damage-multiplier constant by
 * fn_800E0C54() range (<0x66->0x28,<0xb2->0x50,<0xcc->0x78,else->
 * -(u16)fn_80203B5C(ctx2,4)) via fn_8011BBD8; then dispatches PC by the
 * same range (<0xcc -> lbl_80375F98; else if fn_80201704(ctx2)==1 ->
 * lbl_80377AD9; else fn_80209960(fieldD9,0x43) then -> lbl_80377AB8). */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80209960 fightWazaInitJoutaiDataId
void fn_8021799C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern u16 fn_800E0C54(void);
    extern u16 fn_80203B5C();
    extern u8 fn_80201704();
    extern void fn_80209960();
    extern u8 lbl_80375F98[16];
    extern u8 lbl_80377AD9[0x2A];
    extern u8 lbl_80377AB8[0x21];
    u32 ctx2;
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8 val;
    ctx2 = fn_801F025C(0x12, 0);
    val = (u8)fn_800E0C54();

    if (val < 0x66) {
        fn_8011BBD8(fieldD9, 0, 0x2f, 0, 0x28);
    } else if (val < 0xb2) {
        fn_8011BBD8(fieldD9, 0, 0x2f, 0, 0x50);
    } else if (val < 0xcc) {
        fn_8011BBD8(fieldD9, 0, 0x2f, 0, 0x78);
    } else {
        u16 v4 = (u16)fn_80203B5C(ctx2, 4);
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(s32)v4);
    }

    if (val < 0xcc) {
        lbl_8047B610 = (u8*)lbl_80375F98;
    } else if (fn_80201704(ctx2) == 1) {
        lbl_8047B610 = (u8*)lbl_80377AD9;
    } else {
        fn_80209960(fieldD9, 0x43);
        lbl_8047B610 = (u8*)lbl_80377AB8;
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BBD8
#undef fn_80203B5C
#undef fn_80201704
#undef fn_80209960
#pragma optimize_for_size reset

/* fn_80214864: bytecode op -- picks a constant by fn_801F453C(0,1)
 * selector (2->0xb,3->5,1->0xa,4->0xf,else->0) via fn_8011BBD8(fieldD9,
 * 0,0x30,0,k); non-default selectors also apply fn_8011BBD8(fieldD9,0,
 * 0x2c,0,2). PC += 1. */
#pragma optimize_for_size on
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80214864(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern u8 fn_801F453C();
    u32 fieldD9;
    u8 sel = (u8)fn_801F453C(0, 1);
    u32 ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);

    if (sel == 2) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0xb);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else if (sel == 3) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 5);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else if (sel == 1) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0xa);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else if (sel == 4) {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0xf);
        fn_8011BBD8(fieldD9, 0, 0x2c, 0, 2);
    } else {
        fn_8011BBD8(fieldD9, 0, 0x30, 0, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F453C
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BBD8
#pragma optimize_for_size reset

/* ===== wave4-G ===== */

/* fn_8023FB5C (0x8023FB5C): rank-threshold pair, seqs 0x1d0 / 0x1d1. */
#pragma optimize_for_size on
u32 fn_8023FB5C(void* ctx, u32 param1, u32 param2) {
    extern u8  fn_80236BFC(void*, u32, u32);
    extern u8  fn_80237F74(void*, u32, u32);
    extern s32 fn_80202108(u32, u32);
    extern s32 fn_80202234(u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;
    s8 result;
    s32 bump;
    s32 x;
    s8 y;

    if ((u8)fn_80236BFC(ctx, param1, 8) != 1) {
        goto done;
    }

    if ((u8)fn_80236BFC(ctx, param1, 8) == 0) {
        result = -1;
    } else {
        bump = ((u8)fn_80237F74(ctx, param1, 0x30) == 1) ? 2 : 1;
        x = fn_80202108(param1, 8);
        x += bump;
        y = (s8)fn_80202234(param1, 8);
        if ((s8)x >= y) {
            result = 1;
        } else {
            result = 0;
        }
    }
    if (result == 0) {
        handle = fn_80239984(0, ctx, 0x1d0);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d0);
    }

    if ((u8)fn_80236BFC(ctx, param1, 8) == 0) {
        result = -1;
    } else {
        bump = ((u8)fn_80237F74(ctx, param1, 0x30) == 1) ? 2 : 1;
        x = fn_80202108(param1, 8);
        x += bump;
        y = (s8)fn_80202234(param1, 8);
        if ((s8)x >= y) {
            result = 1;
        } else {
            result = 0;
        }
    }
    if (result == 1) {
        handle = fn_80239984(handle, ctx, 0x1d1);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d1);
    }
done:
    return handle;
}
#pragma optimize_for_size reset

/* ===== wave4-A ===== */

/* fn_80214F10: bytecode op -- resolves id=(u16)fn_801F54A4(...,0x14,...),
 * ctx11 from slot 0x11, a trainer context via fn_801F4354(0,ctx11), and a
 * candidate-move buffer via fn_80215008(trainerCtx,buf,0x18,ctx11); if that
 * returns a nonzero divisor, picks buf[fn_800E0C54()%divisor], clears
 * lbl_8047B618 bit 0x400, stashes the pick in lbl_8047B60C, resolves it via
 * fn_8022B2CC(ctx11,val,id,0,1,1,-1) and queues fn_801F4C14(...,0x43,...,
 * result); PC += 5. Otherwise takes the script-embedded jump. */
extern u16 lbl_8047B60C;
extern int fn_80215008();
extern u32 fn_8022B2CC();

#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F4C14 fightFloorSetStatus
void fn_80214F10(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    u32 ctx11;
    u16 id;
    u32 trainerCtx;
    s32 divisor;
    u16 val;
    u32 result;
    u16 buf[0x1C];

    id = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx11 = fn_801F025C(0x11, 0);
    trainerCtx = fn_801F4354(0, ctx11);
    divisor = fn_80215008(trainerCtx, buf, 0x18, ctx11);
    if (divisor != 0) {
        val = buf[(u16)fn_800E0C54() % divisor];
        lbl_8047B618 = lbl_8047B618 & ~0x400;
        lbl_8047B60C = val;
        result = fn_8022B2CC(ctx11, val, id, 0, 1, 1, -1);
        fn_801F4C14(0, 0, 0x43, 0, result);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_801F4354
#undef fn_801F4C14
#pragma optimize_for_size reset

/* fn_80214450: bytecode op -- resolves ctx = fn_801F025C(3, fn_801F025C(
 * 0x11,0)), then checks fn_801F6E98(ctx,0x49)/0x48 in combination: if BOTH
 * flags are set, clears both (fn_801F6EEC) and marks lbl_80379F58[.]=3;
 * else if only 0x49, clears it and marks 1; else if only 0x48, clears it
 * and marks 2; else marks 0. PC += 1 always. */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6EEC fightSideInitJoutaiDataId
extern void fn_801F6EEC();

void fn_80214450(void) {
    extern u32 fn_801F025C();
    extern u8 fn_801F6E98();
    u32 ctxA = fn_801F025C(0x11, 0);
    u32 ctx = fn_801F025C(3, ctxA);

    if (fn_801F6E98(ctx, 0x49) == 1) {
        goto L_check49_recheck;
    }
    if (fn_801F6E98(ctx, 0x48) != 1) {
        goto L_zero;
    }

L_check49_recheck:
    if (fn_801F6E98(ctx, 0x49) != 1) {
        goto L_check48_only;
    }
    if (fn_801F6E98(ctx, 0x48) != 1) {
        goto L_check48_only;
    }
    fn_801F6EEC(ctx, 0x49);
    fn_801F6EEC(ctx, 0x48);
    lbl_80379F58[0x16002] = 3;
    lbl_80379F58[0x160a1] = 3;
    goto L_done;

L_check48_only:
    if (fn_801F6E98(ctx, 0x49) == 1) {
        fn_801F6EEC(ctx, 0x49);
        lbl_80379F58[0x16002] = 1;
        lbl_80379F58[0x160a1] = 1;
        goto L_done;
    }
    if (fn_801F6E98(ctx, 0x48) == 1) {
        fn_801F6EEC(ctx, 0x48);
        lbl_80379F58[0x16002] = 2;
        lbl_80379F58[0x160a1] = 2;
    }
    goto L_done;

L_zero:
    lbl_80379F58[0x16002] = 0;
    lbl_80379F58[0x160a1] = 0;

L_done:
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_801F6E98
#undef fn_801F6EEC

/* ===== codex-cracked: comma-expression load-order lever ===== */
extern void fn_801FBC20();
extern void fn_80208C18();
extern void fn_80205AD4();
extern void fn_80205A7C();
extern void fn_80206C94();
/* NOTE: 0x8020F108 = fightWazaWzxTypeFuncMigawari on newer origin/master
 * (codex-confirmed vs the target relocation there); this branch's symbols.txt
 * still uses fn_8020F108 -- reconcile at merge. */

/* Slot switch-in refresh op. The comma-expression temporary in the
 * fn_801F025C argument is load-bearing: it forces the slot byte (PC+1) to
 * load before the mode byte (PC+2) with no extra instructions. PC += 3. */
#define fn_8012640C pokemonGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FBC20 fightTrainerBallThrowEffect
#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_80205A7C fightOutPokemonSetOnDarkPokemonFlag
#define fn_80205AD4 fightOutPokemonSetOnZukanFlag
#define fn_80206C94 fightOutPokemonInit
#define fn_802086B0 fightOutPokemonFreeAllSequenceWaza
#define fn_80208C18 fightOutPokemonDasuEffect
#define fn_8020F108 fightWazaWzxTypeFuncMigawari
#define fn_8026246C fightMenuCloseMsg
void fn_8021F458(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_801FB1C0();
    extern u8  fn_802026E4();
    u32 ctx1;
    u32 trainerData;
    u32 result;
    u16 val;
    u8 mode;
    u8 slot;

    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    ctx1 = fn_801F025C((slot = *(u8*)(lbl_8047B610 + 1), mode = *(u8*)(lbl_8047B610 + 2), slot), 0);
    fn_8012640C(ctx1, 0, 0xd5, 0);
    result = fn_801F4354(0, ctx1);
    trainerData = fn_801FB1C0(result, 0, 0x47, 0);

    if (mode != 2) {
        fn_801FBC20(result, ctx1, 2);
    }

    if ((u8)fn_80204A10(ctx1) == 0) {
        fn_80265598(ctx1, val, 0);
    } else {
        fn_80265598(ctx1, val, 1);
    }

    if (mode != 2) {
        fn_80208C18(ctx1, 1);
        fn_80208C18(ctx1, 2);
    }
    fn_80208C18(ctx1, 3);
    fn_80208C18(ctx1, 4);

    fn_8026532C(ctx1, val, 0);

    if (mode != 2) {
        fn_801FBC20(result, ctx1, 3);
    }
    fn_80208C18(ctx1, 5);

    if (mode == 1 && (u8)fn_802026E4(ctx1, 0x14) == 1 && fn_801FEF74(ctx1) > 0) {
        fn_8026246C();
        fn_8020F108(0xa4, ctx1, ctx1, 0, 0);
        for (;;) {
            if ((u8)fn_801DA5C4(6) == 1) {
                break;
            }
            _threadSwitch();
        }
        fn_802086B0(ctx1);
    }

    if ((u8)fn_801F54A4(0, 0, 0x1e, 0) == 1 && (u8)fn_80204A10(ctx1) == 0) {
        fn_80205AD4(ctx1, 0);
        fn_80205A7C(ctx1, 0);
    }

    fn_80206C94(trainerData);
    lbl_8047B610 = lbl_8047B610 + 3;
}
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F4354
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_801FBC20
#undef fn_801FEF74
#undef fn_80204A10
#undef fn_80205A7C
#undef fn_80205AD4
#undef fn_80206C94
#undef fn_802086B0
#undef fn_80208C18
#undef fn_8020F108
#undef fn_8026246C

/* ===== wave4-F: fight-target checks + the battle-message emitters ===== */

/* fn_80237DBC (0x80237DBC): checks whether `target` matches the ally/foe
 * fight-target id computed twice (mode 0 then mode 1 via fn_80207B8C).
 * Verified 100.0% match.
 * Key lever: the final comparison compiles as a DIRECT (non-inverted)
 * branch -- source must be `if (cond) goto L; found: return 1; L: return 0;`
 * (goto-to-not_found), not the naive `if(!cond) return 0; found: return 1;`
 * (which the compiler treats identically to the inverted form and mismatches
 * branch polarity + block order on the last 4 instructions). */
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80207B8C fightOutPokemonGetZokuseiDataId
#pragma optimize_for_size on
u8 fn_80237DBC(u32 ctx, void* param2, u16 target) {
    extern u32 fn_801FB1C0();
    extern u32 fn_801F54A4();
    extern u8 fightTrainerIsAllyFightTargetPtr(u32 fieldA, void* floor, u16 idx);
    extern u16 fn_80207B8C(void* context, u8 field);
    u16 val;
    u16 temp;
    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    if ((u8)fn_801FB1C0(0, fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0) & 0xFFFF, 0x2A, 0) == 1) {
        if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
            temp = fn_80207B8C(param2, 0);
        } else {
            temp = fn_80207B8C(param2, 0);
        }
    } else {
        temp = 9;
    }
    if (target == temp) {
        goto found;
    }
    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    if ((u8)fn_801FB1C0(0, fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0) & 0xFFFF, 0x2A, 0) == 1) {
        if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
            temp = fn_80207B8C(param2, 1);
        } else {
            temp = fn_80207B8C(param2, 1);
        }
    } else {
        temp = 9;
    }
    if (target != temp) {
        goto not_found;
    }
found:
    return 1;
not_found:
    return 0;
}
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_80207B8C
#pragma optimize_for_size reset

/* fn_80238E30 (0x80238E30): sibling of fn_80237DBC -- same twice-computed
 * -target/tail-merge shape, but the per-slot id comes from
 * fn_8012640C(0, field6E, 0x16, idx) instead of fn_80207B8C(param2, idx).
 * Matched first try by reusing the fn_80237DBC tail pattern verbatim.
 * Verified 100.0% match. */
#define fn_8012640C pokemonGetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#pragma optimize_for_size on
u8 fn_80238E30(u32 ctx, void* param2, u16 target) {
    extern u32 fn_801FB1C0();
    extern u32 fn_801F54A4();
    extern u32 fn_8012640C();
    extern void* fn_80205BE8(void* ctx);
    extern u8 fightTrainerIsAllyFightTargetPtr(u32 fieldA, void* floor, u16 idx);
    u16 val;
    u16 flag;
    u16 field6E;
    u16 temp;

    fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    flag = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    field6E = (u16)fn_8012640C(fn_80205BE8(param2), 0, 0x6E, 0);
    if ((u8)fn_801FB1C0(0, flag, 0x2A, 0) == 1) {
        if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
            temp = (u16)fn_8012640C(0, field6E, 0x16, 0);
        } else {
            temp = (u16)fn_8012640C(0, field6E, 0x16, 0);
        }
    } else {
        temp = 9;
    }
    if (target == temp) {
        goto found;
    }
    val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    flag = (u16)fn_801FB1C0(0, fn_801FB1C0(ctx, 0, 0x43, 0) & 0xFFFF, 0x2, 0);
    field6E = (u16)fn_8012640C(fn_80205BE8(param2), 0, 0x6E, 0);
    if ((u8)fn_801FB1C0(0, flag, 0x2A, 0) == 1) {
        if (!fightTrainerIsAllyFightTargetPtr(ctx, param2, val)) {
            temp = (u16)fn_8012640C(0, field6E, 0x16, 1);
        } else {
            temp = (u16)fn_8012640C(0, field6E, 0x16, 1);
        }
    } else {
        temp = 9;
    }
    if (target != temp) {
        goto not_found;
    }
found:
    return 1;
not_found:
    return 0;
}
#undef fn_8012640C
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_80205BE8
#pragma optimize_for_size reset

/* fn_80239CCC (0x80239CCC): 9-arg battle-message emitter. Builds an event
 * with up to 9 optional/const fields via fn_80132A38(fieldId, value), then
 * dispatches. p8 packs two sub-ids decoded via fn_801FB1C0(0,p8,0x40/0x41,0)
 * AND is reused later for field 0x41 via fn_801FB1C0(0,p8,0x3f,0).
 * Key levers: (u8) casts on the two status-check calls (fn_80008164 /
 * fn_802624CC) -- without them the compiler emits a spurious clrlwi at the
 * callsite; a 0x20-byte stack buffer for fn_80103BA8's output (a bare u16
 * local undersizes the frame by 0x20 bytes and shifts the whole GPR save
 * area); and splitting the tail into an explicit `goto do_check` + duplicate
 * `return 0;` inside the fn_8026246C() branch to match the target's
 * non-tail-merged, non-inverted branch layout.
 * Verified 100.0% match. */
#define fn_800FA280 GSmsgGetGSchar
#define fn_80103BA8 menuGetKeyInfo
#define fn_8011BEB4 wazaGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_801F8100 fightTrainerGetNamePtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#pragma optimize_for_size on
u8 fn_80239CCC(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8, s32 p9) {
    extern u32 fn_801FB1C0();
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern u32 fn_8012640C();
    extern u32 fn_8011BEB4();
    extern u32 itemGetStatus();
    extern void fn_80103BA8();
    extern u32 fn_80008164();
    extern u32 fn_802624CC();
    extern void fn_8026246C();
    u32 v40;
    u32 v41;
    u8 buf[0x20];

    v40 = fn_801FB1C0(0, p8, 0x40, 0);
    v41 = fn_801FB1C0(0, p8, 0x41, 0);

    if (v40) fn_80132A38(0xD, fn_800FA280(v40));
    if (v41) fn_80132A38(0xE, fn_800FA280(v41));
    if (p2) fn_80132A38(0x13, fn_801F8100(p2));
    if (p3) fn_80132A38(0x14, fn_8012640C(p3, 0, 0x77, 0));
    if (p4) fn_80132A38(0x23, fn_801F8100(p4));
    if (p5) fn_80132A38(0x15, fn_8012640C(p5, 0, 0x77, 0));
    if ((u16)p6) fn_80132A38(0x28, fn_800FA280(fn_8011BEB4(0, p6, 1, 0)));
    if ((u16)p7) fn_80132A38(0x29, fn_800FA280(itemGetStatus(0, p7, 1, 0)));
    if ((u16)p8) fn_80132A38(0x41, fn_800FA280(fn_801FB1C0(0, p8, 0x3f, 0)));
    fn_80132A38(0x2F, p9);

    if ((u8)fn_80008164() == 1) {
        fn_80103BA8(buf, 1);
        if ((*(u16*)buf & 0x800) == 0) {
            goto do_check;
        }
        return 0;
do_check:
        if ((u8)fn_802624CC(ctx) == 1) {
            fn_8026246C();
            return 0;
        }
    }
    return 0;
}
#pragma optimize_for_size reset

/* fn_8023A118 (0x8023A118): 11-param battle-message emitter, sibling of
 * fn_80239CCC with the same field-id layout, but p2/p3 arrive pre-resolved
 * (no fn_801FB1C0 0x40/0x41 decode step) and 3 extra params ride the stack.
 * Key lever beyond the fn_80239CCC ones: p8's parameter type must be u32
 * (not u16) even though its null-check truncates to 16 bits -- declaring it
 * u16 lets the compiler fuse the truncate into the fn_8011BEB4 call argument
 * (one shared clrlwi), but the target keeps the raw untruncated value for
 * the call and a separate clrlwi only for the condition.
 * Verified 100.0% match. */
#pragma optimize_for_size on
u8 fn_8023A118(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8,
               u16 p9, u16 p10, u32 p11) {
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern u32 fn_8012640C();
    extern u32 fn_8011BEB4();
    extern u32 itemGetStatus();
    extern u32 fn_801FB1C0();
    extern void fn_80103BA8();
    extern u32 fn_80008164();
    extern u32 fn_802624CC();
    extern void fn_8026246C();
    u8 buf[0x20];

    if (p2) fn_80132A38(0xD, fn_800FA280(p2));
    if (p3) fn_80132A38(0xE, fn_800FA280(p3));
    if (p4) fn_80132A38(0x13, fn_801F8100(p4));
    if (p5) fn_80132A38(0x14, fn_8012640C(p5, 0, 0x77, 0));
    if (p6) fn_80132A38(0x23, fn_801F8100(p6));
    if (p7) fn_80132A38(0x15, fn_8012640C(p7, 0, 0x77, 0));
    if ((u16)p8) fn_80132A38(0x28, fn_800FA280(fn_8011BEB4(0, p8, 1, 0)));
    if (p9) fn_80132A38(0x29, fn_800FA280(itemGetStatus(0, p9, 1, 0)));
    if (p10) fn_80132A38(0x41, fn_800FA280(fn_801FB1C0(0, p10, 0x3f, 0)));
    fn_80132A38(0x2F, p11);

    if ((u8)fn_80008164() == 1) {
        fn_80103BA8(buf, 1);
        if ((*(u16*)buf & 0x800) == 0) {
            goto do_check;
        }
        return 0;
do_check:
        if ((u8)fn_802624CC(ctx) == 1) {
            fn_8026246C();
            return 0;
        }
    }
    return 0;
}
#pragma optimize_for_size reset

/* fn_80239EE8 (0x80239EE8): 8-arg emitter (no trailing free param): p8 packs
 * THREE sub-ids (0x40, 0x41, 0x3e) via fn_801FB1C0; the 0x3e one feeds field
 * 0x2F directly (no fn_800FA280 wrap), replacing the 9-arg sibling's p9.
 * Key lever beyond the fn_80239CCC ones: local declaration ORDER (v3e
 * declared before v40/v41) was required to match the target's r28-r31
 * coloring for ctx/p7/p8/v3e.
 * Verified 100.0% match. */
#pragma optimize_for_size on
u8 fn_80239EE8(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8) {
    extern u32 fn_801FB1C0();
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern u32 fn_8012640C();
    extern u32 fn_8011BEB4();
    extern u32 itemGetStatus();
    extern void fn_80103BA8();
    extern u32 fn_80008164();
    extern u32 fn_802624CC();
    extern void fn_8026246C();
    u32 v3e;
    u32 v40;
    u32 v41;
    u8 buf[0x20];

    v40 = fn_801FB1C0(0, p8, 0x40, 0);
    v41 = fn_801FB1C0(0, p8, 0x41, 0);
    v3e = fn_801FB1C0(0, p8, 0x3e, 0);

    if (v40) fn_80132A38(0xD, fn_800FA280(v40));
    if (v41) fn_80132A38(0xE, fn_800FA280(v41));
    if (p2) fn_80132A38(0x13, fn_801F8100(p2));
    if (p3) fn_80132A38(0x14, fn_8012640C(p3, 0, 0x77, 0));
    if (p4) fn_80132A38(0x23, fn_801F8100(p4));
    if (p5) fn_80132A38(0x15, fn_8012640C(p5, 0, 0x77, 0));
    if ((u16)p6) fn_80132A38(0x28, fn_800FA280(fn_8011BEB4(0, p6, 1, 0)));
    if ((u16)p7) fn_80132A38(0x29, fn_800FA280(itemGetStatus(0, p7, 1, 0)));
    if ((u16)p8) fn_80132A38(0x41, fn_800FA280(fn_801FB1C0(0, p8, 0x3f, 0)));
    fn_80132A38(0x2F, v3e);

    if ((u8)fn_80008164() == 1) {
        fn_80103BA8(buf, 1);
        if ((*(u16*)buf & 0x800) == 0) {
            goto do_check;
        }
        return 0;
do_check:
        if ((u8)fn_802624CC(ctx) == 1) {
            fn_8026246C();
            return 0;
        }
    }
    return 0;
}
#pragma optimize_for_size reset
#undef fn_800FA280
#undef fn_80103BA8
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_80132A38
#undef fn_801F8100
#undef fn_801FB1C0
#undef fn_8026246C
#undef fn_802624CC

/* ===== wave4-B (+stray wave4-C fn_8021E288) ===== */

/*
 * fn_8021C190 (0x8021C190)
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
void fn_8021C190(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    extern u8 fn_80201704();
    extern void fn_80202810();
    extern void fn_801F4C14();
    extern u8 fn_80119DD0();
    extern u16 fn_80203B5C();
    extern void fn_8011BBD8();

    s16 val;
    u32 ctx1;
    u32 fieldD9;
    u8* jumpTarget;
    s16 diff;

    jumpTarget = *(u8**)(lbl_8047B610 + 1);
    ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);

    if (fn_802026E4(ctx1, 0x2d) == 0) {
        val = 0;
    } else {
        val = fn_80202360(ctx1, 0x2d);
    }
    if (val <= 0) {
        lbl_80478D78[5] = 0;
        lbl_8047B610 = jumpTarget;
        return;
    }
    if (fn_80201704(ctx1) == 1) {
        fn_80202810(ctx1, 0x2d);
        fn_801F4C14(0, 0, 0x43, 0, ctx1);
        lbl_80478D78[5] = 1;
        lbl_8047B610 = jumpTarget;
        return;
    }
    diff = (s16)fn_80119DD0(0x2d) - val;
    if (diff < 0) {
        diff = 0;
    }
    fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(s32)(u16)fn_80203B5C(ctx1, (u16)(1 << diff)));
    lbl_80379F58[0x16002] = (u8)val;
    fn_80202810(ctx1, 0x2d);
    fn_801F4C14(0, 0, 0x43, 0, ctx1);

    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80201704
#undef fn_80202810
#undef fn_801F4C14
#undef fn_80203B5C
#undef fn_8011BBD8
#pragma optimize_for_size reset

/*
 * fn_8021C308 (0x8021C308)
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
void fn_8021C308(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205184();
    extern s32 fn_8011BEB4();
    extern u8 fn_802026E4();
    extern s16 fn_80202360();
    extern s32 fn_80232110();
    extern void fn_8011BBD8();
    extern void fn_80202810();

    u32 ctx2;
    u8* jumpTarget;
    u32 aux;
    u16 v1;
    u16 v2;
    u32 ctx1;
    u32 fieldD9;
    s16 val;
    u32 obj;

    jumpTarget = *(u8**)(lbl_8047B610 + 1);
    ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    aux = fn_80205184(ctx1);
    v1 = (u16)fn_8011BEB4(fieldD9, 0, 0x2f, 0);
    v2 = (u16)fn_8011BEB4(fieldD9, 0, 0x30, 0);

    if (fn_802026E4(ctx1, 0x2d) == 0) {
        val = 0;
    } else {
        val = fn_80202360(ctx1, 0x2d);
    }
    ctx2 = fn_801F025C(0x12, 0);
    obj = fn_801F025C(2, ctx2);
    if (val <= 0) {
        lbl_8047B610 = jumpTarget;
        return;
    }
    if (lbl_80478D78[6] != 1) {
        s32 result = fn_80232110(ctx1, ctx2, obj, aux, v1, v2);
        ctx2 = (s32)val * result;
        lbl_80379F58[0x16002] = (u8)val;
        if (fn_802026E4(ctx1, 0x32) == 1) {
            ctx2 = ((s32)ctx2 * 15) / 10;
        }
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, ctx2);
    }
    fn_80202810(ctx1, 0x2d);
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205184
#undef fn_8011BEB4
#undef fn_8011BBD8
#undef fn_80202810
#pragma optimize_for_size reset

/*
 * fn_8021E288 (0x8021E288)
 */
#define fn_8012640C pokemonGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F54A4 fightFloorGetStatus
#pragma optimize_for_size on
void fn_8021E288(void) {
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4(u32 a, u32 b, u32 c, u32 d);
    extern void* fn_8012640C(void* context, u32 slot, u16 tableId, u32 flags);
    extern u8 fn_802026E4(void* ctx, void* typeObj);
    extern u8 fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_80265598();

    void* ctx;
    u8* pc;

    fn_801F54A4(0, 0, 0x14, 0);
    pc = (u8*)lbl_8047B610;
    ctx = (void*)fn_801F025C(pc[1], 0);

    if ((lbl_8047B618 & 0x80) == 0) {
        if (fn_802026E4(ctx, (void*)0x8) == 1) {
            u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
            void* poke = fn_8012640C(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x2e, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x2e, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x5) == 1) {
            u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
            void* poke = fn_8012640C(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x2f, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x2f, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x7) == 1) {
            u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
            void* poke = fn_8012640C(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x30, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x30, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x6) == 1) {
            u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
            void* poke = fn_8012640C(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x31, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x31, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x3) == 1) {
            u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
            void* poke = fn_8012640C(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x32, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x32, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x4) == 1) {
            u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
            void* poke = fn_8012640C(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x33, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x33, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
    }

    lbl_8047B610 += 2;
}
#pragma optimize_for_size reset
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F54A4

/* ===== wave4-C (harvested after worker hit session limit) ===== */

#define fn_8011BEB4 wazaGetStatus
#define fn_801C2D5C battleCameraDoFull
#define fn_801C2D68 battleCameraDoSimple
#define fn_801C3430 battleGridUpdate
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F1700 fightFloorIsUseFightTimerCommand
#define fn_801F2B5C fightFloorLoopValidFightTrainer
#define fn_801F32B0 fightFloorIsIrekaeFightOutPokemon
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_801F3984 fightFloorSetFightResultId
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F7174 fightSideGetFightPokemonNokoriHp
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80261954 fightMenuCloseInfoMenu
#define fn_80261AD0 fightMenuOpenInfoMenu
#define fn_802658C8 fightTimerCommandTerminate
#define fn_802659A4 fightTimerCommandBlock
#define fn_802659F8 fightTimerCommandStart
#define fn_80265A6C fightTimerCommandInit
void fn_80222C44(void) {
    extern u32 fn_801F025C();
    extern u32 fn_80205184();
    extern u32 fn_801F54A4();
    extern u32 fn_801F7174(u32, u16, u16);
    extern u8  fn_801F3984();
    extern u16 fn_801EF634();
    extern u32 fn_80077AF4();
    extern u32 fn_8011BEB4();
    extern u8  fn_801F32B0();
    extern void fn_801EF95C();
    extern u8  fn_801F1700();
    extern void fn_80265A6C();
    extern void fn_802659F8();
    extern void fn_801C3430(void);
    extern u32 fn_801EF8F4();
    extern void fn_80261AD0();
    extern void fn_801F2B5C();
    extern void fn_80261954();
    extern void fn_801C2D68(void);
    extern void fn_801C2D5C(void);
    extern void fn_802659A4();
    extern void fn_802658C8();
    extern void fn_801F37B0();
    extern void fn_802230BC();
    extern void fn_80222EF0();

    u16 a17;
    u16 a16;
    u8 a34;
    u32 ctx1;
    u32 moveId;
    u8 opByte;
    u32 ctx2;
    u32 val77AF4;
    u16 t9;
    u8 flag;
    u32 ctx4;
    u32 ctx5;
    u32 h;

    opByte = (u8)*(u32*)(lbl_8047B610 + 1);
    ctx1 = fn_801F025C(0x11, 0);
    moveId = fn_80205184(ctx1);
    a16 = (u16)fn_801F54A4(0, 0, 0x16, 0);
    a17 = (u16)fn_801F54A4(0, 0, 0x17, 0);

    ctx2 = fn_801F025C(5, 0);
    if (fn_801F7174(ctx2, a16, a17) == 0) {
        fn_801F3984(0, 2);
    }

    ctx2 = fn_801F025C(4, 0);
    if (fn_801F7174(ctx2, a16, a17) == 0) {
        fn_801F3984(0, 3);
    }

    if ((u16)fn_801EF634() == 0) goto opByteCheck;

    a34 = (u8)fn_801F54A4(0, 0, 0x34, 0);
    val77AF4 = fn_80077AF4();
    t9 = (u16)fn_8011BEB4(0, moveId, 9, 0);

    if (a34 == 1 && (u8)val77AF4 == 1 && t9 == 7) {
        flag = 1;
    } else {
        flag = 0;
    }

    if (flag != 1) goto advance;
    if ((u16)fn_801EF634() != 7) goto advance;

    fn_801F3984(0, 0);
    ctx4 = fn_801F025C(2, ctx1);
    ctx5 = fn_801F025C(4, 0);
    if (ctx5 == ctx4) {
        fn_801F3984(0, 3);
    } else {
        fn_801F3984(0, 2);
    }
    goto advance;

opByteCheck:
    if (opByte != 1) goto advance;
    if ((u8)fn_801F32B0(0) == 0) goto msgAdvance;

    fn_801EF95C();
    if ((u16)fn_801EF634() != 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
        return;
    }

    if ((u8)fn_801F1700(0) == 1) {
        fn_80265A6C();
        fn_802659F8();
    }
    fn_801C3430();
    h = fn_801EF8F4(0);
    fn_80261AD0(-1);
    fn_801F2B5C(0, (u32)fn_802230BC, 0, 1);
    fn_80261954(0);
    if ((u8)h == 1) {
        fn_801C2D68();
    } else {
        fn_801C2D5C();
    }

    if ((u8)fn_801F1700(0) == 1) {
        fn_802659A4();
        fn_802658C8();
    }
    if ((u16)fn_801EF634() == 0) goto msgAdvance;
    lbl_8047B610 = lbl_8047B610 + 5;
    return;

msgAdvance:
    fn_801F37B0(0, (u32)fn_80222EF0, 0, 0);
advance:
    lbl_8047B610 = lbl_8047B610 + 5;
}

#undef fn_8011BEB4
#undef fn_801C2D5C
#undef fn_801C2D68
#undef fn_801C3430
#undef fn_801F025C
#undef fn_801F1700
#undef fn_801F2B5C
#undef fn_801F32B0
#undef fn_801F37B0
#undef fn_801F3984
#undef fn_801F54A4
#undef fn_801F7174
#undef fn_80205184
#undef fn_80261954
#undef fn_80261AD0
#undef fn_802658C8
#undef fn_802659A4
#undef fn_802659F8
#undef fn_80265A6C
#define fn_8012640C pokemonGetStatus
#define fn_8012A5B0 heroGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F2A7C fightFloorGetGcHeroFightTrainerPtr
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FBD40 fightTrainerKindDataBiosGetSyoukinBairitu
#define fn_801FBD58 fightTrainerKindDataBiosGetPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80203E7C figthPokemonGetLevel
#define fn_80206A04 fightPokemonCheckValid
void fn_8021E754(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F2A7C();
    extern u32 fn_801FB1C0(u32, u32, u32, u16);
    extern u32 fn_801F025C();
    extern u8* fn_801FBD58(u16 idx);
    extern u16 fn_801FBD40(u8* ptr);
    extern u8  fn_80206A04();
    extern u32 fn_80203E7C();
    extern u32 fn_8012640C();
    extern void scriptAddPremium();
    extern void heroAddPokedoru();
    extern void fn_80132A38();
    extern u32 fn_8012A5B0();
    u32 ctx1;
    u32 field44;
    u8 field48;
    u32 best;
    u16 a26;
    u32 v1;
    u32 v2;
    u32 amount;
    u32 ctx2;
    s32 i;
    u16 t43;
    u16 fieldVal;
    u32 t;
    u16 mult;

    a26 = (u16)fn_801F54A4(0, 0, 0x18, 0);
    if ((u8)fn_801F54A4(0, 0, 0x25, 0) == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    ctx1 = fn_801F2A7C(0);
    if (ctx1 == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    field44 = fn_801FB1C0(ctx1, 0, 0x44, 0);
    if (field44 == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    field48 = (u8)fn_801FB1C0(ctx1, 0, 0x48, 0);
    ctx2 = fn_801F025C(9, ctx1);
    t43 = (u16)fn_801FB1C0(ctx2, 0, 0x43, 0);
    fieldVal = fn_801FBD40(fn_801FBD58((u16)fn_801FB1C0(0, t43, 4, 0)));

    best = 0;
    for (i = 0; i < 6; i++) {
        v1 = fn_801FB1C0(ctx2, 0, 0x45, i);
        if (fn_80206A04(v1)) {
            t = fn_80203E7C(v1);
            if ((u8)best < (u8)t) best = t;
        }
    }

    for (i = 0; i < 6; i++) {
        v2 = fn_801FB1C0(ctx1, 0, 0x45, i);
        if (fn_80206A04(v2) && (s32)fn_8012640C(v2, 0, 0xcf, 0) == 1) {
            t = fn_80203E7C(v2);
            if ((u8)best < (u8)t) best = t;
        }
    }

    if ((u8)fn_801F54A4(0, 0, 0x26, 0) == 0) {
        if (a26 <= 1) {
            mult = 1;
        } else {
            mult = 2;
        }
    } else {
        mult = 4;
    }

    amount = mult * (fieldVal * ((best & 0xFF) * field48));

    if ((u8)fn_801F54A4(0, 0, 0x26, 0) == 1) {
        scriptAddPremium(amount);
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    heroAddPokedoru(field44, amount);
    fn_80132A38(0x2f, amount);
    fn_80132A38(0x4b, amount);
    fn_80132A38(0x13, fn_8012A5B0(field44, 1, 0));
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_8012640C
#undef fn_8012A5B0
#undef fn_80132A38
#undef fn_801F025C
#undef fn_801F2A7C
#undef fn_801F54A4
#undef fn_801FBD40
#undef fn_801FBD58
#undef fn_801FB1C0
#undef fn_80203E7C
#undef fn_80206A04

/* ===== fable deep-dive: the story-script interpreter (documented partial) ===== */

/*
 * fn_802249B8 (0x802249B8, 6012 bytes) - story-script interpreter,
 * fight_range_80211A00. mwcc 2.4.2b53 -O4,p, C89.
 *
 * STATUS: 96.92% objdiff match (NOT 100%). Instruction count identical to
 * target (1526/1526 after canonicalization); control flow, call sequence,
 * and all constants verified identical. Residual diffs are:
 *
 * 1. REGISTER-NUMBER ONLY (~65 instrs):
 *    - count<->slotB callee-saved swap: ours count=r18/slotB=r24,
 *      target count=r24/slotB=r18. Blocked: mwcc's allocator class for a
 *      u16 truncation-def web never reaches the r24+ pool in any of ~800
 *      declaration-order permutations tested; a u32 "& 0xFFFF" form reaches
 *      r24 but then slotB lodges at r23 (never below the params).
 *    - case-7 (pre-switch tbl==7) loop scratch permuted:
 *      ours cnt=r15,deck2=r16,limit=r17,j=r18,party=r19,i=r20 vs target
 *      r18,r20,r19,r16,r17,r15. (Case-8's identical loop DOES match after
 *      scratch decl reordering; splitting case-7 into its own vars fixes
 *      case-7 but regresses the objdiff score elsewhere - see notes.)
 * 2. LOCAL-TEMP/SCHEDULING SHAPE (~30 instrs, 12 sites): the
 *    "lbl_8047B618 &= ~0x2000; lbl_80478D78[5]=1" blocks compile to
 *    rlwinm-in-place+serialized stores in ours vs rlwinm-to-r0 + hoisted
 *    li's in target; fn_802025B8 table-read arg order (mr r3 vs slwi);
 *    fn_8021B910 arg clrlwi placement; case-8 deck2 def r0-routed;
 *    cmd31/cmd14 mr r7 placement. Believed downstream of (1): the local
 *    temp allocation depends on which callee-saved regs are free.
 * 3. JUMPTABLE RELOC NAME (2 instrs): lis/addi reference the compiler's
 *    anonymous @NNN jumptable vs target's named jumptable_8039A220 -
 *    known objdiff artifact, not a code difference.
 *
 * Everything else (1400+ instructions) matches, including the jumptable
 * dispatch (cases 7..0x3B), all 25 distinct case bodies, the pre-switch
 * (lbl_80279EF4 values 3-8), the party-scan loops, divw codegen
 * (optimize_for_size), and the GCSE of &lbl_80478D78 into r31.
 *
 * Established discoveries needed for this codegen (see agent notes):
 * - #pragma optimize_for_size on (divw by 3, srawi/addze for /4)
 * - lbl_80478D78[3] accessed directly (no pointer local): mwcc GCSE's the
 *   base into r31 and rematerializes for the [5] stores.
 * - inner party loop: s32 i with "(u16)i < 6" bound -> raw mr at the
 *   fn_801F986C call (u16 i would emit a spurious clrlwi; this is also
 *   what blocks the matched-sibling fn_80229704 at 96.8%).
 * - fn_8012640C must be declared s32 (cmpwi at the cmd-8 0x112 check).
 * - case-7 head condition is fn_80207AE0(ctx,0xF) == 0.
 * - cmd14: lvl += (u16)fn_800E0C54() % 3; lvl -= 1; as separate stmts
 *   (single expression makes mwcc reorder the two calls).
 * - PC (lbl_8047B610) typed u8* here per integrator convention; verified
 *   in-tree as u32 with (u32)& casts - identical codegen.
 *//* ==== fn_802249B8 work-in-progress (story-script interpreter) ==== */
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_802249B8(u32 param1, u32 param2) {
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_8012640C pokemonGetStatus
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80203CCC fightOutPokemonIsJoutaiNormal
#define fn_801F7258 fightSideGetValidFightTrainerPtr
#define fn_801F986C fightTrainerGetValidFightPokemonPtr
#define fn_80206608 fightPokemonCheckFightOut
#define fn_80202ADC fightPokemonCheckWriteJoutaiDataId
#define fn_80200B10 fightOutPokemonInitJoutaiKeep
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_8020981C fightWazaCheckWriteJoutaiDataId
#define fn_802097C8 fightWazaWriteJoutaiDataId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80203E0C figthOutPokemonGetLevel
#define fn_801FAA58 fightTrainerSetStatus
#define fn_8020A2B8 fightWazaBiosCopy
#define fn_801254B4 pokemonSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_8020147C fightOutPokemonDoItemSoubi
#define fn_801FE710 fightOutPokemonSetHensinFightPokemonStatusId
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
    extern u8* lbl_8047B610;
    extern u32 lbl_8047B618;
    extern u8  lbl_8047B628;
    extern u16 lbl_80279EF4[];
    extern u16 lbl_80279F6C[];
    extern u32 lbl_80399F58[];
    extern u8  lbl_80379F58[];
    extern u8  lbl_80379808[];
    extern u8  lbl_8037981F[];
    extern u8  lbl_80379836[];
    extern u8  lbl_80379868[];
    extern u8  lbl_803798D8[];
    extern u8  lbl_8037958A[];
    extern u8  lbl_80378E5A[];
    extern u8  lbl_80378EFB[];
    extern u8  lbl_8037943C[];
    extern u8  lbl_803790E7[];
    extern u8  lbl_8037913D[];
    extern u8  lbl_803791D0[];
    extern u8  lbl_8037631E[];
    extern u8  lbl_803763CE[];
    extern u32 fn_801F54A4();
    extern u32 fn_801F453C();
    extern u32 fn_801F025C();
    extern u32 fn_80205184();
    extern void fn_801F4C14();
    extern u32 fn_801F0134();
    extern u32 fn_80207BF4();
    extern u32 fn_802040E8();
    extern s32 fn_8012640C();
    extern u32 fn_801F6E98();
    extern u32 fn_802062FC();
    extern u32 fn_802026E4();
    extern u32 fightFloorCheckFightOutPokemonPtrAryJoutaiDataId();
    extern u32 fn_80203CCC();
    extern u32 fn_802025B8();
    extern u8  fn_80077B60(void);
    extern u8  fn_80077B3C(void);
    extern u32 fn_801F7258();
    extern u32 fn_801F986C();
    extern u32 fn_80206608();
    extern u32 fn_80202ADC();
    extern void fn_80200B10();
    extern void fn_80211B94();
    extern u32 fn_80207AE0();
    extern u32 fn_80204A10();
    extern u32 fn_8020981C();
    extern void fn_802097C8();
    extern void fn_8020248C();
    extern u32 fn_801FECD4();
    extern void fn_801FE7EC();
    extern void fn_801FE710();
    extern u32 fn_801F4354();
    extern u32 fn_801FB1C0();
    extern u32 fn_80203E0C();
    extern void fn_801FAA58();
    extern u32 fn_800E0C54(void);
    extern void fn_8020A2B8();
    extern u32 fn_801254B4();
    extern void fn_80201B2C();
    extern u32 fn_80203B5C();
    extern s32 fn_8011BEB4();
    extern void fn_8011BBD8();
    extern u32 fn_8021B910();
    extern u32 fn_80142984();
    extern void fn_8020147C();
    extern void fn_80202810();

    u16 count;
    u32 other;
    u32 slotA;
    u32 ctx;
    u32 pparty;
    u32 trainer;
    u32 monIdx;
    u32 deck;
    u8  flag27;
    u32 hF8;
    u8  flag26;
    u32 hD9;
    u32 waza;
    u32 lvl21;
    u32 slotB;
    u8  ok;
    u8  v;
    u8  fightable;
    s32 i;
    u16 j;
    u32 party;
    u16 cnt;
    u16 limit;
    u32 deck2;
    u32 mon;
    u8  flag34;
    u8  valA;
    u8  flag3C;
    s32 lvl;
    u32 p;
    u32 cur;
    u32 sum;
    u32 newv;
    u32 w1;
    u32 wazaB;
    u32 lvlB;
    u16 lv2;
    u16 tbl;
    u32 newFlags;
    u8 newState;

    flag27 = 0;
    flag26 = 0;
    count = fn_801F54A4(0, 0, 0x14, 0);
    pparty = fn_801F453C(0, 1);
    slotA = fn_801F025C(0x11, 0);
    trainer = fn_80205184();
    slotB = fn_801F025C(0x12, 0);

    if ((lbl_80478D78[3] & 0x40) != 0) {
        ctx = slotA;
        fn_801F4C14(0, 0, 0x47, 0, slotA);
        fn_801F4C14(0, 0, 0x4B, 0, slotB);
        lbl_80478D78[3] = lbl_80478D78[3] & 0xBF;
        flag26 = 0x40;
        other = slotB;
    } else {
        ctx = slotB;
        fn_801F4C14(0, 0, 0x47, 0, slotB);
        fn_801F4C14(0, 0, 0x4B, 0, slotA);
        other = slotA;
    }
    monIdx = fn_801F0134(other, count);
    waza = fn_80207BF4(ctx);
    lvl21 = fn_802040E8(ctx);
    deck = fn_801F025C(2, ctx);
    hD9 = fn_8012640C(ctx, 0, 0xD9, 0);
    hF8 = fn_8012640C(ctx, 0, 0xF8, 0);

    if ((u16)waza == 0x13 && (lbl_8047B618 & 0x2000) == 0 && (u8)param1 == 0 && lbl_80478D78[3] < 0xA) {
        lbl_8047B610 += 1;
        return;
    }
    if ((u8)fn_801F6E98(deck, 0x4B) == 1 && (lbl_8047B618 & 0x2000) == 0 && (u8)param1 == 0 && lbl_80478D78[3] < 8) {
        lbl_8047B610 += 1;
        return;
    }
    if ((u8)fn_802062FC(ctx) == 0 && lbl_80478D78[3] != 0xB && lbl_80478D78[3] != 0x1F) {
        lbl_8047B610 += 1;
        return;
    }
    if ((u8)fn_802026E4(ctx, 0x14) == 1 && flag26 != 0x40) {
        lbl_8047B610 += 1;
        return;
    }

    if (lbl_80478D78[3] < 7) {
        switch (lbl_80279EF4[lbl_80478D78[3]]) {
        case 8:
            fightable = 1;
            if ((u16)waza != 0x2B) {
                if ((u16)fightFloorCheckFightOutPokemonPtrAryJoutaiDataId(0, 0xB) != 0) {
                    fightable = 0;
                }
            }
            if ((u8)fn_80203CCC(ctx) == 1) {
                tbl = lbl_80279EF4[lbl_80478D78[3]];
                if ((u8)fn_802025B8(ctx, tbl) == 2 && fightable == 1 &&
                    (u16)waza != 0x48 && (u16)waza != 0xF) {
                flag34 = (u8)fn_801F54A4(0, 0, 0x34, 0);
                valA = fn_80077B60();
                fn_80077B3C();
                if (flag34 == 1 && valA != 1) {
                    cnt = 0;
                    deck2 = fn_801F025C(2, ctx);
                    limit = (u16)fn_801F54A4(0, 0, 0x16, 0);
                    fn_801F54A4(0, 0, 0x17, 0);
                    for (j = 0; j < limit; j++) {
                        party = fn_801F7258(deck2, j);
                        if (party == 0) {
                            continue;
                        }
                        for (i = 0; (u16)i < 6; i++) {
                            mon = fn_801F986C(party, i);
                            if (mon == 0) {
                                continue;
                            }
                            if ((u8)fn_80206608(mon) == 0) {
                                continue;
                            }
                            if ((u8)fn_80202ADC(mon, 8) == 1) {
                                cnt++;
                            }
                        }
                    }
                    if ((u16)cnt >= 1) {
                        ok = 1;
                        goto chk8;
                    }
                }
                ok = 0;
chk8:
                    if (ok != 1) {
                        fn_80200B10(ctx);
                        flag27 = 1;
                    }
                }
            }
            break;
        case 3:
            if ((u16)waza == 0x11 && ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                if ((lbl_8047B618 & 0x2000) != 0) {
                    newFlags = lbl_8047B618 & ~0x2000;
                    newState = 1;
                    lbl_8047B618 = newFlags;
                    lbl_80478D78[5] = newState;
                } else {
                    lbl_80478D78[5] = 0;
                }
                fn_80211B94(lbl_8047B62C, &lbl_80379836, 0);
                lbl_8047B610 += 1;
                return;
            }
            if (((u8)fn_80207AE0(ctx, 3) == 1 || (u8)fn_80207AE0(ctx, 8) == 1) && (lbl_8047B618 & 0x2000) != 0 &&
                ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                lbl_80478D78[5] = 2;
                fn_80211B94(lbl_8047B62C, &lbl_80379836, 0);
                lbl_8047B610 += 1;
                return;
            }
            if ((u8)fn_80207AE0(ctx, 3) == 0 && (u8)fn_80207AE0(ctx, 8) == 0 && (u8)fn_80203CCC(ctx) == 1 &&
                (u16)waza != 0x11) {
                flag27 = 1;
            }
            break;
        case 6:
            if ((u16)waza == 0x29 && ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                if ((lbl_8047B618 & 0x2000) != 0) {
                    newFlags = lbl_8047B618 & ~0x2000;
                    newState = 1;
                    lbl_8047B618 = newFlags;
                    lbl_80478D78[5] = newState;
                } else {
                    lbl_80478D78[5] = 0;
                }
                fn_80211B94(lbl_8047B62C, &lbl_80379808, 0);
                lbl_8047B610 += 1;
                return;
            }
            if ((u8)fn_80207AE0(ctx, 0xA) == 1 && (lbl_8047B618 & 0x2000) != 0 &&
                ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                lbl_80478D78[5] = 2;
                fn_80211B94(lbl_8047B62C, &lbl_80379808, 0);
                lbl_8047B610 += 1;
                return;
            }
            if ((u8)fn_80207AE0(ctx, 0xA) == 0 && (u16)waza != 0x29 && (u8)fn_80203CCC(ctx) == 1) {
                flag27 = 1;
            }
            break;
        case 7:
            if ((u8)fn_80207AE0(ctx, 0xF) == 0 && (u8)fn_80203CCC(ctx) == 1 && (u8)pparty != 1 &&
                (u16)waza != 0x28) {
                flag34 = (u8)fn_801F54A4(0, 0, 0x34, 0);
                fn_80077B60();
                flag3C = fn_80077B3C();
                if (flag34 == 1 && flag3C != 1) {
                    u16 j7;
                    u32 party7;
                    u16 cnt7;
                    u16 limit7;
                    s32 i7;
                    u32 mon7;

                    cnt7 = 0;
                    deck2 = fn_801F025C(2, ctx);
                    limit7 = (u16)fn_801F54A4(0, 0, 0x16, 0);
                    fn_801F54A4(0, 0, 0x17, 0);
                    for (j7 = 0; j7 < limit7; j7++) {
                        party7 = fn_801F7258(deck2, j7);
                        if (party7 == 0) {
                            continue;
                        }
                        for (i7 = 0; (u16)i7 < 6; i7++) {
                            mon7 = fn_801F986C(party7, i7);
                            if (mon7 == 0) {
                                continue;
                            }
                            if ((u8)fn_80206608(mon7) == 0) {
                                continue;
                            }
                            if ((u8)fn_80202ADC(mon7, 7) == 1) {
                                cnt7++;
                            }
                        }
                    }
                    if ((u16)cnt7 >= 1) {
                        ok = 1;
                        goto chk7;
                    }
                }
                ok = 0;
chk7:
                if (ok != 1) {
                    fn_80200B10(ctx);
                    flag27 = 1;
                }
            }
            break;
        case 5:
            if ((u16)waza == 7 && ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                if ((lbl_8047B618 & 0x2000) != 0) {
                    newFlags = lbl_8047B618 & ~0x2000;
                    newState = 1;
                    lbl_8047B618 = newFlags;
                    lbl_80478D78[5] = newState;
                } else {
                    lbl_80478D78[5] = 0;
                }
                fn_80211B94(lbl_8047B62C, &lbl_8037981F, 0);
                lbl_8047B610 += 1;
                return;
            }
            if ((u16)waza != 7 && (u8)fn_80203CCC(ctx) == 1) {
                flag27 = 1;
            }
            break;
        case 4:
            if ((u16)waza == 0x11 && ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                if ((lbl_8047B618 & 0x2000) != 0) {
                    newFlags = lbl_8047B618 & ~0x2000;
                    newState = 1;
                    lbl_8047B618 = newFlags;
                    lbl_80478D78[5] = newState;
                } else {
                    lbl_80478D78[5] = 0;
                }
                fn_80211B94(lbl_8047B62C, &lbl_80379836, 0);
                lbl_8047B610 += 1;
                return;
            }
            if (((u8)fn_80207AE0(ctx, 3) == 1 || (u8)fn_80207AE0(ctx, 8) == 1) && (lbl_8047B618 & 0x2000) != 0 &&
                ((u8)param1 == 1 || (u8)param2 == 0x80)) {
                lbl_80478D78[5] = 2;
                fn_80211B94(lbl_8047B62C, &lbl_80379836, 0);
                lbl_8047B610 += 1;
                return;
            }
            if ((u8)fn_80203CCC(ctx) == 1) {
                if ((u8)fn_80207AE0(ctx, 3) == 0 && (u8)fn_80207AE0(ctx, 8) == 0) {
                    if ((u16)waza != 0x11) {
                        flag27 = 1;
                    }
                } else {
                    if ((u8)fn_8020981C(hD9, 0x43) == 2) {
                        fn_802097C8(hD9, 0x43, 0);
                    }
                }
            }
            break;
        }
        if (flag27 != 0) {
            tbl = lbl_80279EF4[lbl_80478D78[3]];
            if ((u8)fn_802025B8(ctx, tbl) == 2) {
                tbl = lbl_80279EF4[lbl_80478D78[3]];
                fn_8020248C(ctx, tbl, 0);
            }
            if ((u8)fn_801FECD4(ctx) == 1) {
                fn_801FE7EC(ctx, 0x7C, 0, 0);
            }
            if ((lbl_8047B618 & 0x2000) != 0) {
                lbl_8047B618 = lbl_8047B618 & ~0x2000;
                lbl_80478D78[5] = 1;
            } else {
                lbl_80478D78[5] = 0;
            }
            v = lbl_80478D78[3];
            if (v == 2 || v == 5 || v == 6 || v == 3) {
                lbl_8047B628 = v;
                lbl_8047B618 = lbl_8047B618 | 0x4000;
            }
            fn_80211B94(lbl_8047B62C, lbl_80399F58[v], 0);
            lbl_8047B610 += 1;
            return;
        }
        lbl_8047B610 += 1;
        return;
    }

    tbl = lbl_80279EF4[lbl_80478D78[3]];
    if (tbl != 0 && (u8)fn_802026E4(ctx, tbl) == 1) {
        lbl_8047B610 += 1;
        return;
    }

    switch (lbl_80478D78[3]) {
    case 7:
        if ((u16)waza != 0x14 && (u8)fn_802026E4(ctx, 9) == 0) {
            if ((u8)fn_802025B8(ctx, 9) == 2) {
                fn_8020248C(ctx, 9, 0);
            }
            fn_80211B94(lbl_8047B62C, lbl_80399F58[lbl_80478D78[3]], 0);
            lbl_8047B610 += 1;
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 8:
        if ((u16)waza == 0x27 && ((u8)param1 == 1 || (u8)param2 == 0x80)) {
            lbl_8047B610 = (u8*)&lbl_80379868;
            return;
        }
        if ((u16)waza != 0x27 && fn_8012640C(ctx, 0, 0x112, 0) == 0) {
            tbl = lbl_80279EF4[lbl_80478D78[3]];
            if ((u8)fn_802025B8(ctx, tbl) == 2) {
                tbl = lbl_80279EF4[lbl_80478D78[3]];
                fn_8020248C(ctx, tbl, 0);
            }
        }
        lbl_8047B610 += 1;
        return;
    case 10:
        if ((u8)fn_802025B8(ctx, 0xB) == 2) {
            fn_8020248C(ctx, 0xB, 0);
            fn_8020248C(slotA, 0x22, 0);
            fn_8020A2B8(hF8, hD9);
            fn_80211B94(lbl_8047B62C, lbl_80399F58[lbl_80478D78[3]], 0);
            lbl_8047B610 += 1;
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 11:
        p = fn_801F4354(0, slotA);
        if (p != 0) {
            cur = fn_801FB1C0(p, 0, 0x49, 0);
            sum = cur + (u8)fn_80203E0C(slotA) * 5;
            newv = 0xFFFF;
            if (sum <= 0xFFFF) {
                newv = sum;
            }
            fn_801FAA58(p, 0, 0x49, 0, newv);
            fn_80211B94(lbl_8047B62C, lbl_80399F58[lbl_80478D78[3]], 0);
        }
        lbl_8047B610 += 1;
        return;
    case 9:
        if ((u8)fn_80203CCC(ctx) == 1) {
            lbl_80478D78[3] = (u16)fn_800E0C54() % 3 + 3;
            fn_802249B8(0, 0);
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 12:
        fn_8020248C(ctx, 0x22, 0);
        fn_8020A2B8(hF8, hD9);
        fn_801254B4(ctx, 0, 0x109, 0, 1);
        lbl_8047B610 += 1;
        return;
    case 13:
        if ((u8)fn_802025B8(ctx, 0xE) == 2) {
            fn_8020248C(ctx, 0xE, monIdx);
            fn_80201B2C(ctx, 0xE, trainer);
            lbl_80478D78[5] = 0;
            while (lbl_80478D78[5] < 5) {
                if (lbl_80279F6C[lbl_80478D78[5]] == (u16)trainer) {
                    break;
                }
                lbl_80478D78[5] += 1;
            }
            fn_80211B94(lbl_8047B62C, lbl_80399F58[lbl_80478D78[3]], 0);
            lbl_8047B610 += 1;
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 14:
        if ((u16)trainer == 0x164) {
            lvl = (u16)fn_80203B5C(ctx, 0x10);
            lvl += (u16)fn_800E0C54() % 3;
            lvl -= 1;
        } else {
            lvl = fn_8011BEB4(hD9, 0, 0x2E, 0) / 4;
        }
        if (lvl == 0) {
            lvl = 1;
        }
        fn_8011BBD8(hD9, 0, 0x2D, 0, lvl);
        fn_80211B94(lbl_8047B62C, lbl_80399F58[lbl_80478D78[3]], 0);
        lbl_8047B610 += 1;
        return;
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
        if ((u8)fn_8021B910(0x10, (u8)(lbl_80478D78[3] - 0xE), flag26, 0) != 0) {
            lbl_8047B610 += 1;
            return;
        }
        lbl_80379F58[0x160A4] = lbl_80478D78[3] & 0x3F;
        lbl_80379F58[0x160A5] = 0;
        fn_80211B94(lbl_8047B62C, &lbl_8037631E, 0);
        lbl_8047B610 += 1;
        return;
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
        if ((u8)fn_8021B910(0x90, (u8)(lbl_80478D78[3] - 0x15), flag26, 0) != 0) {
            lbl_8047B610 += 1;
            return;
        }
        lbl_80379F58[0x160A4] = lbl_80478D78[3] & 0x3F;
        lbl_80379F58[0x160A5] = 0;
        fn_80211B94(lbl_8047B62C, &lbl_803763CE, 0);
        lbl_8047B610 += 1;
        return;
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
        if ((u8)fn_8021B910(0x20, (u8)(lbl_80478D78[3] - 0x26), flag26, 0) != 0) {
            lbl_8047B610 += 1;
            return;
        }
        lbl_80379F58[0x160A4] = lbl_80478D78[3] & 0x3F;
        lbl_80379F58[0x160A5] = 0;
        fn_80211B94(lbl_8047B62C, &lbl_8037631E, 0);
        lbl_8047B610 += 1;
        return;
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
        if ((u8)fn_8021B910(0xA0, (u8)(lbl_80478D78[3] - 0x2D), flag26, 0) != 0) {
            lbl_8047B610 += 1;
            return;
        }
        lbl_80379F58[0x160A4] = lbl_80478D78[3] & 0x3F;
        lbl_80379F58[0x160A5] = 0;
        fn_80211B94(lbl_8047B62C, &lbl_803763CE, 0);
        lbl_8047B610 += 1;
        return;
    case 29:
        if ((u8)fn_802025B8(ctx, 0x12) == 2) {
            fn_8020248C(ctx, 0x12, 0);
            fn_8020A2B8(hF8, hD9);
        }
        lbl_8047B610 += 1;
        return;
    case 30:
        if ((u8)fn_802025B8(slotA, 0x13) == 2) {
            fn_8020248C(slotA, 0x13, 0);
        }
        lbl_8047B610 += 1;
        return;
    case 31:
        if ((u8)fn_801F54A4(0, 0, 0x2F, 0) == 1 && (u8)fn_80204A10(slotA) == 0) {
            ok = 0;
            goto chk31;
        }
        if ((u8)fn_801F54A4(0, 0, 0x2F, 0) == 1 && (u8)fn_802026E4(slotA, 0x3D) == 1) {
            ok = 0;
            goto chk31;
        }
        ok = 1;
chk31:
        if (ok == 0) {
            lbl_8047B610 += 1;
            return;
        }
        w1 = fn_802040E8(slotA);
        wazaB = fn_80207BF4(slotB);
        lvlB = fn_802040E8(slotB);
        if ((u16)wazaB == 0x3C && (u16)lvlB != 0) {
            lbl_8047B610 = (u8*)&lbl_803798D8;
            return;
        }
        if ((u16)w1 == 0 && (u16)lvlB != 0xAF && (u16)lvlB != 0 && (u8)fn_80142984(lvlB) == 1) {
            fn_8020147C(slotB, 0, 0, 0);
            lv2 = lvlB;
            fn_801254B4(slotA, 0, 0xFB, 0, lv2);
            if ((u8)fn_801FECD4(slotA) == 1) {
                fn_801FE7EC(slotA, 0x82, 0, 0);
            }
            if ((u8)fn_801FECD4(slotB) == 1) {
                fn_801FE7EC(slotB, 0x82, 0, 0);
            }
            fn_80202810(slotB, 0x36);
            fn_801F4C14(0, 0, 0x56, 0, lv2);
            fn_80211B94(lbl_8047B62C, &lbl_8037958A, 0);
            lbl_8047B610 += 1;
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 32:
        if ((u8)fn_802025B8(ctx, 0x16) == 2) {
            fn_8020248C(ctx, 0x16, fn_801F0134(slotA, count));
        }
        lbl_8047B610 += 1;
        return;
    case 33:
        if ((u8)fn_802025B8(ctx, 0x17) == 2) {
            fn_8020248C(ctx, 0x17, 0);
        }
        lbl_8047B610 += 1;
        return;
    case 34:
        fn_80211B94(lbl_8047B62C, &lbl_80378E5A, 0);
        lbl_8047B610 += 1;
        return;
    case 35:
        fn_80211B94(lbl_8047B62C, &lbl_80378EFB, 0);
        lbl_8047B610 += 1;
        return;
    case 36:
        if ((u8)fn_802026E4(slotB, 5) == 1) {
            fn_80202810(slotB, 5);
            if ((u8)fn_801FECD4(slotB) == 1) {
                fn_801FE7EC(slotB, 0x7C, 0, 0);
            }
            fn_80211B94(lbl_8047B62C, &lbl_8037943C, 0);
        }
        lbl_8047B610 += 1;
        return;
    case 37:
        fn_80211B94(lbl_8047B62C, &lbl_803790E7, 0);
        lbl_8047B610 += 1;
        return;
    case 38:
        lvl = fn_8011BEB4(hD9, 0, 0x2E, 0) / 3;
        if (lvl == 0) {
            lvl = 1;
        }
        fn_8011BBD8(hD9, 0, 0x2D, 0, lvl);
        fn_80211B94(lbl_8047B62C, lbl_80399F58[lbl_80478D78[3]], 0);
        lbl_8047B610 += 1;
        return;
    case 53:
        if ((u8)fn_802025B8(ctx, 0xD) == 2) {
            fn_8020248C(ctx, 0xD, 0);
            fn_8020248C(ctx, 0x22, 0);
            fn_8020A2B8(hF8, hD9);
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 54:
        if ((u16)waza == 0x3C && (u16)lvl21 != 0) {
            lbl_8047B610 = (u8*)&lbl_803798D8;
            return;
        }
        if ((u16)lvl21 != 0 && (u8)fn_802025B8(ctx, 0x3D) == 2) {
            fn_801F4C14(0, 0, 0x56, 0, (u16)lvl21);
            fn_8020248C(ctx, 0x3D, 0);
            if ((u8)fn_801FECD4(ctx) == 1) {
                fn_801FE710(ctx, 0xCD, 0);
            }
            fn_80202810(ctx, 0x36);
            fn_80211B94(lbl_8047B62C, &lbl_8037913D, 0);
            lbl_8047B610 += 1;
            return;
        }
        lbl_8047B610 += 1;
        return;
    case 59:
        fn_80211B94(lbl_8047B62C, &lbl_803791D0, 0);
        lbl_8047B610 += 1;
        return;
    }
    lbl_8047B610 += 1;
#undef fn_80202810
#undef fn_801FE710
#undef fn_8020147C
#undef fn_80204A10
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_80203B5C
#undef fn_801254B4
#undef fn_8020A2B8
#undef fn_801FAA58
#undef fn_80203E0C
#undef fn_801FB1C0
#undef fn_801F4354
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_802097C8
#undef fn_8020981C
#undef fn_80207AE0
#undef fn_80200B10
#undef fn_80202ADC
#undef fn_80206608
#undef fn_801F986C
#undef fn_801F7258
#undef fn_80203CCC
#undef fn_802062FC
#undef fn_801F6E98
#undef fn_8012640C
#undef fn_802040E8
#undef fn_80207BF4
#undef fn_801F0134
#undef fn_801F4C14
#undef fn_80205184
#undef fn_801F025C
#undef fn_801F453C
#undef fn_801F54A4
}
#pragma opt_propagation reset
#pragma optimize_for_size reset

/* ===== fable deep-dive #2 (documented partial) ===== */

/*
 * fn_8022BE2C (0x8022BE2C, 4696 bytes) -- fight_range_80211A00 unit.
 * VERIFIED 98.85264% (objdiff) in worktree pkmn-fight-c3 as of delivery.
 *
 * RESIDUAL DIFFS (all classified, no logic diffs):
 *  - register-only (~149 instrs): callee-saved coloring. Correct webs: ctx=r31,
 *    result=r30, msg=r27, cmd/t2=r20, c=r28, e=r23, f=r26, mode=r21, mvId/t3=r24.
 *    Wrong-number-only webs: t1-first-web wants r19 (ours r29), g wants r25
 *    (ours r29 -- shares with t1 because mwcc hoists the (u16)t1 arg mask above
 *    g's def, erasing the interference the target had), d wants r22 (ours r25),
 *    case-7 pk wants r19 (ours r20), per-case m/cnt/i/kind webs want r19 (ours
 *    r20/r21). Exhausted: decl-order steepest-ascent+hill-climb (~2000 builds,
 *    plateau), scheduling 601/602/603/off/750 sweep (all regress or neutral),
 *    variable merge/split, block-scoping. Root blocker: the t1/g share saves a
 *    register (our build stmw r20 vs target stmw r19); no source-level lever
 *    found that keeps t1 live past g's def under the 750 scheduler.
 *  - stmw/lmw (2 instrs): r19 vs r20 start reg -- consequence of the share above.
 *  - label-only relocs (10 instrs): anonymous local-initializer data (kinds init
 *    -> target lbl_8047E604/lbl_8047E608, avail init -> target lbl_80279FF8) and
 *    the three compiler-generated jumptables (jumptable_8039A3C8/3A8/388) get
 *    @NNN local names in a fresh compile. Known splitter artifact, not fixable
 *    from source (struct-copy-from-named-extern was tried: changes codegen).
 *  - instruction-shape (4 instrs): (a) fn_801FE7EC arg-order swap: target emits
 *    clrlwi r5 (mask) before mr r3,ctx; ours emits mr first (both operand sets
 *    identical); (b) "and. r0,r3,r0" vs "and. r0,r0,r3" operand order in the
 *    case-7 bit test; (c) one dead "b" in the tail switch(result) tree.
 *
 * INTEGRATION NOTES:
 *  - Wrap in #pragma optimize_for_size on/reset (included below) -- required for
 *    the compact mtctr/lwzu/stwu 20-byte initializer copy.
 *  - NOT block-declared here (per integrator file-scope decls): lbl_80478D78
 *    (u8[8] .sdata) and lbl_8047B62C (.sbss, 4 bytes). lbl_8047B62C is passed
 *    by VALUE as arg1 of K&R fn_80211B94 -- any 4-byte file-scope type works.
 *  - lbl_80279FD0 is the unit's .rodata u16[8] {0xE6..0xEC,0} status-op table
 *    (also used by matched fn_8022D084); block-declared extern here.
 *  - Local array initializers {1,2,3,4,5} (u8[5], .sdata2) and {-1 x5} (s32[5],
 *    .rodata) must stay as LOCAL initializers -- they emit the anonymous copy
 *    sources that correspond to lbl_8047E604/lbl_80279FF8.
 *  - fn_80203BDC/fn_80203B5C/fn_801FE7EC prototypes below are load-bearing
 *    (arg-order/CSE); "g & 0xFF" (not "(u8)g") in the fn_801FE7EC call is
 *    load-bearing (prevents a GCSE the target does not have).
 *  - Local declaration ORDER is load-bearing (register coloring).
 */
/* ==== fn_8022BE2C (battle condition-cure / item-effect dispatcher) ==== */
#pragma optimize_for_size on
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_80123CD4 pokemonWazaCheckValid
#define fn_80123E70 pokemonWazaGetMaxPP
#define fn_801252E0 pokemonInitJoutai
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_80143AF0 tasteDataGetNigateMsgDataId
#define fn_80143B08 tasteDataGetPtr
#define fn_801F4C14 fightFloorSetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80201400 fightOutPokemonGetTasteLike
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_80203BDC fightOutPokemonIsNokoriHpFollowing
#define fn_80203CCC fightOutPokemonIsJoutaiNormal
#define fn_80203EDC figthOutPokemonGetSoubiItemBuff
#define fn_80203FE4 fightOutPokemonGetSoubiItemSoubiDataId
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_802062FC fightOutPokemonCheckFightOut
u32 fn_8022BE2C(u32 ctx, u8 mode) {
    extern u16 lbl_80279FD0[8];
    extern u8  lbl_80379F58[];
    extern u8  lbl_80379A36[];
    extern u8  lbl_80379A54[];
    extern u8  lbl_80379A72[];
    extern u8  lbl_80379A90[];
    extern u8  lbl_80379AAE[];
    extern u8  lbl_80379ACC[];
    extern u8  lbl_80379AE8[];
    extern u8  lbl_80379B06[];
    extern u8  lbl_80379B22[];
    extern u8  lbl_80379B45[];
    extern u8  lbl_80379B5B[];
    extern u8  lbl_80379B82[];
    extern u8  lbl_80379BB4[];
    extern u8  lbl_80379BD1[];
    extern u32 fn_802062FC();
    extern u32 fn_802040E8();
    extern u32 fn_80203FE4();
    extern s32 fn_80203EDC();
    extern u32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern void fn_801F4C14();
    extern u32 fn_80203BDC(u32, u16);
    extern u8  fn_80201704();
    extern void fn_8011BBD8();
    extern u32 fn_80205BE8();
    extern u32 fn_80123CD4();
    extern u8  fn_80123E70();
    extern u32 fn_801254B4();
    extern u8  fn_802026E4();
    extern u32 fn_80201890();
    extern u8  fn_801FECD4();
    extern void fn_801FE7EC(u32, u32, u8, u32);
    extern s32 fn_8011BEB4();
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern void fn_80143B08();
    extern void fn_80143AF0();
    extern u16 fn_80203B5C(u32, u16);
    extern u32 fn_80201400();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    extern u16 fn_800E0C54(void);
    extern void fn_80202810();
    extern u8  fn_80203CCC();
    extern void fn_80119F50();
    extern u32 fn_801252E0();
    extern void fn_80202998();
    extern void fn_80211B94();
    u32 t2;
    s32 g;
    s32 c;
    u32 result = 0;
    u8* msg = 0;
    u8 mx;
    s32 sel;
    u8 selkind;
    s32 op;
    s32 sum;
    u16 f;
    u32 t4;
    u16 t3;
    u32 e;
    s32 amt;
    u16 pick;
    u8 npp;
    u8 kinds[5] = { 1, 2, 3, 4, 5 };
    u32 d;
    u8 selstatus;
    u32 t1;
    s32 jj;
    s32 avail[5] = { -1, -1, -1, -1, -1 };

    if ((u8)fn_802062FC(ctx) == 0) {
        return 0;
    }
    t1 = fn_802040E8(ctx);
    t2 = fn_80203FE4(ctx);
    c = fn_80203EDC(ctx);
    d = fn_8012640C(ctx, 0, 0xD9, 0);
    e = fn_80205B8C(ctx);
    f = fn_8012640C(e, 0, 0x83, 0);
    g = (u16)fn_8012640C(e, 0, 0x87, 0);
    {
        extern void fn_801F4C14(u32, u32, u32, u32, u16);
        fn_801F4C14(0, 0, 0x56, 0, (u16)t1);
    }

    switch ((u16)t2) {
    case 1:
        if (((u8)fn_80203BDC(ctx, 2) && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            amt = c;
            if (f + c > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            result = 4;
            msg = lbl_80379B22;
        }
        break;
    case 7:
        if (mode == 0 || mode == 2) {
            t1 = fn_80205BE8(fn_8012640C(ctx, 0, 0xD5, 0));
            for (g = 0; (u16)g < 4; g++) {
                if ((u8)fn_80123CD4(t1, g)) {
                    t3 = fn_8012640C(t1, 0, 0x7F, g);
                    t4 = fn_8012640C(t1, 0, 0x80, g) & 0xFF;
                    if (t4 == 0) {
                        break;
                    }
                }
            }
            if ((u16)g == 4) {
                break;
            }
            mx = fn_80123E70(t1, g);
            sum = t4 + c;
            npp = sum;
            if (sum > mx) {
                npp = mx;
            }
            fn_801254B4(t1, 0, 0x80, g, npp);
            if (fn_802026E4(ctx, 0x10) == 0 && fn_802026E4(ctx, 0x31) == 1 &&
                !(fn_80201890(ctx, 0x31) & (1 << (u8)g)) &&
                fn_801FECD4(ctx) == 1) {
                fn_801FE7EC(ctx, 0x80, g & 0xFF, 1);
            }
            fn_8011BEB4(0, t3, 1, 0);
            fn_80132A38(0xD, fn_800FA280());
            result = 3;
            msg = lbl_80379B45;
        }
        break;
    case 0x17:
        for (t1 = 0; (u16)t1 < 7; t1++) {
            if ((s32)fn_8012640C(ctx, 0, lbl_80279FD0[(u16)t1], 0) < 6) {
                fn_801254B4(ctx, 0, lbl_80279FD0[(u16)t1], 0, 6);
                result = 5;
            }
        }
        if ((u8)result == 0) {
            break;
        }
        msg = lbl_80379B06;
        break;
    case 0x2B:
        if ((fn_80201704(ctx) == 0 && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            amt = fn_80203B5C(ctx, 0x10);
            if (f + amt > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            result = 4;
            msg = lbl_80379B5B;
        }
        break;
    case 0xA:
        t1 = 0;
        if (((u8)fn_80203BDC(ctx, 2) && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            fn_80143B08(0);
            fn_80143AF0();
            fn_80132A38(0xD, fn_800FA280());
            amt = fn_80203B5C(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            if ((s8)fn_80201400(ctx, 0) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xB:
        t1 = 0;
        if (((u8)fn_80203BDC(ctx, 2) && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            fn_80143B08(1);
            fn_80143AF0();
            fn_80132A38(0xD, fn_800FA280());
            amt = fn_80203B5C(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            if ((s8)fn_80201400(ctx, 1) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xC:
        t1 = 0;
        if (((u8)fn_80203BDC(ctx, 2) && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            fn_80143B08(2);
            fn_80143AF0();
            fn_80132A38(0xD, fn_800FA280());
            amt = fn_80203B5C(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            if ((s8)fn_80201400(ctx, 2) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xD:
        t1 = 0;
        if (((u8)fn_80203BDC(ctx, 2) && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            fn_80143B08(3);
            fn_80143AF0();
            fn_80132A38(0xD, fn_800FA280());
            amt = fn_80203B5C(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            if ((s8)fn_80201400(ctx, 3) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xE:
        t1 = 0;
        if (((u8)fn_80203BDC(ctx, 2) && mode == 0) ||
            (fn_80201704(ctx) == 0 && mode == 2)) {
            fn_80143B08(4);
            fn_80143AF0();
            fn_80132A38(0xD, fn_800FA280());
            amt = fn_80203B5C(ctx, c);
            if (f + amt > g) {
                amt = g - f;
            }
            fn_8011BBD8(d, 0, 0x2D, 0, -amt);
            if ((s8)fn_80201400(ctx, 4) == -1) {
                t1 = (u32)lbl_80379B82;
            } else {
                t1 = (u32)lbl_80379B22;
            }
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 4;
        break;
    case 0xF:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 fn_8012640C(u32, u32, u32, u32);
                t1 = fn_8012640C(ctx, 0, 0xE6, 0);
            }
            if ((u8)fn_80203BDC(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x11;
                lbl_80379F58[0x160A4] = 0xF;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x10:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 fn_8012640C(u32, u32, u32, u32);
                t1 = fn_8012640C(ctx, 0, 0xE7, 0);
            }
            if ((u8)fn_80203BDC(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x12;
                lbl_80379F58[0x160A4] = 0x10;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x11:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 fn_8012640C(u32, u32, u32, u32);
                t1 = fn_8012640C(ctx, 0, 0xEA, 0);
            }
            if ((u8)fn_80203BDC(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x13;
                lbl_80379F58[0x160A4] = 0x11;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x12:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 fn_8012640C(u32, u32, u32, u32);
                t1 = fn_8012640C(ctx, 0, 0xE8, 0);
            }
            if ((u8)fn_80203BDC(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x14;
                lbl_80379F58[0x160A4] = 0x12;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x13:
        t2 = 0;
        if (mode == 0 || mode == 2) {
            {
                extern u8 fn_8012640C(u32, u32, u32, u32);
                t1 = fn_8012640C(ctx, 0, 0xE9, 0);
            }
            if ((u8)fn_80203BDC(ctx, c) && t1 < 12) {
                lbl_80379F58[0x1601E] = 0x15;
                lbl_80379F58[0x160A4] = 0x13;
                lbl_80379F58[0x160A5] = 0;
                t2 = (u32)lbl_80379BB4;
            }
        }
        msg = (u8*)t2;
        if (t2 == 0) {
            break;
        }
        result = 5;
        break;
    case 0x14:
        if (mode == 0 || mode == 2) {
            if ((u8)fn_80203BDC(ctx, c) == 0) {
                break;
            }
            if (fn_802025B8(ctx, 0xF) != 2) {
                break;
            }
            fn_8020248C(ctx, 0xF, 0);
            result = 2;
            msg = lbl_80379BD1;
        }
        break;
    case 0x15:
        if (mode == 0 || mode == 2) {
            if ((u8)fn_80203BDC(ctx, c) == 0) {
                break;
            }
            t3 = 0;
            for (jj = 0; (u16)jj < 5; jj++) {
                avail[(u16)jj] = -1;
            }
            for (t4 = 0; (u16)t4 < 5; t4++) {
                t1 = kinds[(u16)t4];
                switch (t1) {
                case 1: op = 0xE6; break;
                case 2: op = 0xE7; break;
                case 3: op = 0xEA; break;
                case 4: op = 0xE8; break;
                case 5: op = 0xE9; break;
                case 6: op = 0xEB; break;
                case 7: op = 0xEC; break;
                default: op = 0; break;
                }
                if ((u8)fn_8012640C(ctx, 0, op, 0) < 12) {
                    avail[t3] = t1;
                    t3++;
                }
            }
            if (t3 < 1) {
                break;
            }
            pick = fn_800E0C54() % t3;
            sel = avail[pick];
            if (sel == -1) {
                break;
            }
            selkind = sel;
            selstatus = selkind + 0x26;
            t2 = 0;
            if (mode == 0 || mode == 2) {
                switch (selkind) {
                case 1: op = 0xE6; break;
                case 2: op = 0xE7; break;
                case 3: op = 0xEA; break;
                case 4: op = 0xE8; break;
                case 5: op = 0xE9; break;
                case 6: op = 0xEB; break;
                case 7: op = 0xEC; break;
                default: op = 0; break;
                }
                {
                    extern u8 fn_8012640C(u32, u32, u32, u32);
                    t1 = fn_8012640C(ctx, 0, op, 0);
                }
                if ((u8)fn_80203BDC(ctx, c) && t1 < 12) {
                    lbl_80379F58[0x1601E] = selkind + 0x20;
                    lbl_80379F58[0x160A4] = selstatus;
                    lbl_80379F58[0x160A5] = 0;
                    t2 = (u32)lbl_80379BB4;
                }
            }
            msg = (u8*)t2;
            if (t2 == 0) {
                break;
            }
            result = 5;
        }
        break;
    case 2:
        t1 = 0;
        if (fn_802026E4(ctx, 5) == 1) {
            fn_80202810(ctx, 5);
            t1 = (u32)lbl_80379A36;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 4:
        t1 = 0;
        if (fn_802026E4(ctx, 3) == 1) {
            fn_80202810(ctx, 3);
            fn_80202810(ctx, 4);
            t1 = (u32)lbl_80379A54;
        }
        msg = (u8*)t1;
        if (t1 != 0) {
            result = 1;
            break;
        }
        t1 = 0;
        if (fn_802026E4(ctx, 4) == 1) {
            fn_80202810(ctx, 4);
            fn_80202810(ctx, 3);
            t1 = (u32)lbl_80379A54;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 5:
        t1 = 0;
        if (fn_802026E4(ctx, 6) == 1) {
            fn_80202810(ctx, 6);
            t1 = (u32)lbl_80379A72;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 6:
        t1 = 0;
        if (fn_802026E4(ctx, 7) == 1) {
            fn_80202810(ctx, 7);
            t1 = (u32)lbl_80379A90;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 3:
        t1 = 0;
        if (fn_802026E4(ctx, 8) == 1) {
            fn_80202810(ctx, 8);
            fn_80202810(ctx, 0x17);
            t1 = (u32)lbl_80379AAE;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 1;
        break;
    case 8:
        t1 = 0;
        if (fn_802026E4(ctx, 9) == 1) {
            fn_80202810(ctx, 9);
            t1 = (u32)lbl_80379ACC;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        result = 2;
        break;
    case 9:
        if ((u8)fn_80203CCC(ctx)) {
            if (fn_802026E4(ctx, 9) != 1) {
                break;
            }
        }
        t2 = 0;
        lbl_80478D78[5] = 0;
        if (fn_802026E4(ctx, 3) == 1 || fn_802026E4(ctx, 4) == 1) {
            fn_80119F50(3);
            fn_80132A38(0xD, fn_800FA280());
            t2 = 1;
        }
        if (fn_802026E4(ctx, 8) == 1) {
            fn_80202810(ctx, 0x17);
            fn_80119F50(8);
            fn_80132A38(0xD, fn_800FA280());
            t2++;
        }
        if (fn_802026E4(ctx, 5) == 1) {
            fn_80119F50(5);
            fn_80132A38(0xD, fn_800FA280());
            t2++;
        }
        if (fn_802026E4(ctx, 6) == 1) {
            fn_80119F50(6);
            fn_80132A38(0xD, fn_800FA280());
            t2++;
        }
        if (fn_802026E4(ctx, 7) == 1) {
            fn_80119F50(7);
            fn_80132A38(0xD, fn_800FA280());
            t2++;
        }
        if (fn_802026E4(ctx, 9) == 1) {
            fn_80119F50(9);
            fn_80132A38(0xD, fn_800FA280());
            t2++;
        }
        if ((u16)t2 >= 2) {
            lbl_80478D78[5] = 1;
        }
        fn_801252E0(e);
        fn_80202810(ctx, 9);
        fn_80202998(ctx, 0);
        result = 1;
        msg = lbl_80379AE8;
        break;
    case 0x1C:
        t1 = 0;
        if (fn_802026E4(ctx, 0xA) == 1) {
            fn_80202810(ctx, 0xA);
            t1 = (u32)lbl_80379AE8;
        }
        msg = (u8*)t1;
        if (t1 == 0) {
            break;
        }
        fn_80119F50(0xA);
        fn_80132A38(0xD, fn_800FA280());
        lbl_80478D78[5] = 0;
        result = 2;
        break;
    }

    if ((u8)result != 0) {
        fn_801F4C14(0, 0, 0x4B, 0, ctx);
        fn_801F4C14(0, 0, 0x36, 0, ctx);
        fn_801F4C14(0, 0, 0x49, 0, ctx);
        switch ((u8)result) {
        case 2:
            if ((u8)result == 2) {
                break;
            }
            break;
        case 1:
            if (fn_801FECD4(ctx) == 1) {
                fn_801FE7EC(ctx, 0x7C, 0, 0);
            }
            break;
        }
        if (msg != 0) {
            fn_80211B94(lbl_8047B62C, msg, 0);
        }
    }
    return result;
}
#pragma optimize_for_size reset

/* ===== documented partial: 98.98% (best known, integrated) =====
 * Codex deep-dive 2026-07-03: Category A (u16 zero-extensions on fn_80203B5C
 * damage results) fixed via (u16) casts; Category C (subfic/subic/subfe bool
 * idiom) fixed via #pragma optimize_for_size on. Residual is INDEPENDENT
 * register-coloring (did NOT cascade from A): prologue param-save order (0x578),
 * 0x4 mul mask reg, and renumbering in the 0x29/0x2a/0x26 field-check loops.
 * This is the coloring wall in a large function; not source-controllable. ===== */

/*
 * fn_80230568 (0x80230568, size 0x1194) -- fight_range_80211A00
 * PLATEAU DELIVERY: 98.87 (objdiff match_percent) as verified in worktree.
 *
 * RESIDUALS (all closable diffs closed; remaining classes):
 *  - order-only (2 instr): prologue param saves emitted mr r31,r3 before
 *    mr r27,r4; target emits side's copy first. No source form found that
 *    flips it (explicit param copies coalesce identically).
 *  - order/volatile-only (~7 instr): 0xd inner save/restore block
 *    (lbl_80399F58[7] swap): identical instructions, li/lwz scheduling and
 *    volatile scratch pair r4/r3 vs r5/r4.
 *  - instruction-shape (3 instr): 0x29 section's fn_80201C58 result copy
 *    goes through r0 (mr r0,r3 ... mr r25,r0) where target copies direct
 *    (mr r25,r3). Same registers; 750-scheduler coin-flip; scheduling
 *    601/602/603/604 pragmas all regress the function.
 *  - instruction-shape (5 instr): the (u8)fn_801F37B0(...) != 1 boolean in
 *    the 0x26 section: target materializes via subfic/subic/subfe+clrlwi.
 *    (carry-form != as value); mwcc 2.4.2b53 emits the or/srwi branch-fused
 *    form for every construction tried (12+ probe variants).
 *  - register-number-only (~20 instr): 0x2a section temps (cnt/val/arg/res/
 *    next want r26/r30/r27/r25/r24) and 0x26 loop webs (flagA/count/base/
 *    bound/i/j/t want r26/r29/r25/r30/r27/r26/r24). NOTE: an alternative
 *    form with 0x2a as block-locals declared in order {s32 n2a; u16 rs;
 *    s32 c2a; u32 v2a;} makes the whole 0x2a section byte-exact but drops
 *    overall match_percent to 98.83 by reshuffling 0x26; kept the higher-%
 *    form here.
 *
 * INTEGRATION REQUIREMENTS (verified-critical):
 *  - fn_80201890 MUST be visible as `extern u32 fn_80201890();` (K&R u32).
 *    A prior file-scope `extern u16 fn_80201890();` K&R decl CANNOT be
 *    overridden from block scope (K&R-over-K&R keeps the first return
 *    type) and forces a mask at the 0x2a assignment -> mismatch. The
 *    worktree's file-scope decl was changed u16 -> u32 (only consumers are
 *    unmatched partials).
 *  - fn_80207BF4 / fn_80201D84: block K&R u32 decls below DO override
 *    file-scope u16 prototypes (prototype-over-K&R composite keeps the
 *    later K&R return in mwcc 2.4.2) - no action needed.
 *  - fn_8012640C block prototype (void*, u32, u16, u16) is load-bearing:
 *    the u16 4th param produces the per-call clrlwi r6 masks of `side` and
 *    keeps `side` raw in r27 for the whole span.
 *  - lbl_8047B610 is used with the u8* PC convention (save via (u32) cast,
 *    restore via (u8*) cast); in a tree where it is u32, drop the (u8*).
 *  - No optimize_for_size / scheduling pragma: plain -O4,p codegen.
 *
 * Key matched idioms discovered (reusable for siblings fn_802301A8 etc.):
 *  - lbl_8047B618 set: `lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;`
 *    compiles to lwz/oris/ori/stw (a plain |= 0x1000020 materializes the
 *    constant and does NOT match).
 *  - 0x4 damage: `m = fn_80203B5C(ctx,0x10); m = m * (s16)cnt;` puts the
 *    clrlwi directly in r7 (expression form masks in place -> mismatch).
 *  - counter template: raw s32 locals, extsb only at uses:
 *    `next = (s8)b + 1; if ((s8)next >= (s8)a) clear else set(next)`.
 */
/*
 * fn_80230568 (0x80230568)
 * End-of-turn residual-effects sequencer for one fighter: runs ~20 field
 * checks (leech seed 0x25, statuses, counters 0xb/0xd/0xe/0x29/0x2a etc.)
 * in a fixed order. Each step is bracketed by lbl_8047B618 flag bits
 * 0x01000020 and followed by fn_801DA7AC() plus a liveness re-check
 * (fn_802062FC) that aborts the whole sequence with 1.
 */
#pragma optimize_for_size on
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_80200B10 fightOutPokemonInitJoutaiKeep
#define fn_80123B5C pokemonSearchWazaDataId
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F7258 fightSideGetValidFightTrainerPtr
#define fn_801F986C fightTrainerGetValidFightPokemonPtr
#define fn_80206608 fightPokemonCheckFightOut
#define fn_80202ADC fightPokemonCheckWriteJoutaiDataId
u8 fn_80230568(void* ctx, u32 side) {
    extern u16  fn_801EF634();
    extern u8   fn_802062FC();
    extern void fn_801DA7AC();
    extern u8   fn_80201704();
    extern s32  fn_80202360();
    extern void fn_8022DF08();
    extern void fn_8022BE2C();
    extern void fn_8022EDEC();
    extern void fn_80200B10();
    extern u8   fightOutPokemonCheckNoAttackFlag();
    extern void fn_802249B8();
    extern u32  fn_80123B5C();
    extern u32  fn_80077B60();
    extern void fn_80077B3C();
    extern u32  fn_801F7258();
    extern u32  fn_801F986C();
    extern u8   fn_80206608();
    extern u8   fn_80202ADC();
    extern void fn_8021C638();
    extern u8   fn_801F37B0();
    extern u32  fn_80205B8C();
    extern u32  fn_80207BF4();
    extern void fn_8011BBD8();
    extern s32  fn_8011BEB4();
    extern s32  fn_80132A38(s32, s32);
    extern void* fn_800FA280();
    extern u32  fn_80201D84();
    extern u32  fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(u32, u32);
    extern u32  fn_801F0134();
    extern u32  fn_80201890();
    extern u32  fn_8012640C(void*, u32, u16, u16);
    extern u32  fn_80201C58();
    extern void fn_80201FDC(void*, u32, s8);
    extern u8   lbl_803790AB[];
    extern u8   lbl_80378B72[];
    extern u8   lbl_8037925F[];
    extern u8   lbl_80379287[];
    extern u8   lbl_80379402[];
    extern u8   lbl_8037941F[];
    extern u8   lbl_803793C3[];
    extern u8   lbl_803793A5[];
    extern u8   lbl_8037930A[];
    extern u8   lbl_8037931E[];
    extern u8   lbl_80379BFE[];
    extern u8   lbl_80378D2C[];
    extern u8   lbl_80378D40[];
    extern u8   lbl_80379464[];
    extern u8   lbl_80379F58[];
    extern u8   lbl_80399F58[];

    u32 save;
    register u32 fieldD9;
    register u32 handle;
    register u32 moveId;
    register u32 cnt;
    register u32 tmp2;
    register u32 tmp;

  if ((u16)fn_801EF634(ctx) != 0) {
        return 0;
    }
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }
    fn_801F4C14(0, 0, 0x36, 0, (u32)ctx);
    handle = fn_80205B8C(ctx);
    fieldD9 = fn_8012640C(ctx, 0, 0xd9, 0);
    moveId = fn_80207BF4(ctx);
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x25 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x25) == 1 && (u8)fn_80201704(ctx) == 0) {
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, -(u16)fn_80203B5C(ctx, 0x10));
        fn_80211B94(lbl_8047B62C, (void*)lbl_803790AB, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    fn_8022DF08(ctx);
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    fn_8022BE2C(ctx, 0);
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    fn_8022BE2C(ctx, 1);
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x1c */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x1c) == 1) {
        u32 id = fn_80201D84((u32)ctx, 0x1c);
        if ((u16)id != 0) {
            tmp = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(id, side);
            if (tmp != 0) {
                if ((u8)fn_802062FC(tmp) == 1) {
                    fn_801F4C14(0, 0, 0x43, 0, tmp);
                    fn_8011BBD8(fieldD9, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 8));
                    *(u8*)((u8*)lbl_80379F58 + 0x160A4) = fn_801F0134(tmp, side);
                    *(u8*)((u8*)lbl_80379F58 + 0x160A5) = fn_801F0134(ctx, side);
                    fn_80211B94(lbl_8047B62C, (void*)lbl_80378B72, 0);
                }
            }
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x3 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 3) == 1) {
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 8));
        fn_80211B94(lbl_8047B62C, (void*)lbl_8037925F, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x4 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 4) == 1) {
        u16 dmg;
        s32 m;
        cnt = fn_80202360(ctx, 4);
        dmg = fn_80203B5C(ctx, 0x10);
        m = dmg * (s16)cnt;
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, m);
        fn_8020248C(ctx, 4, 0);
        if ((u8)fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x7c, 0, 0);
        }
        fn_80211B94(lbl_8047B62C, (void*)lbl_8037925F, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x6 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 6) == 1) {
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 8));
        fn_80211B94(lbl_8047B62C, (void*)lbl_80379287, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x17 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x17) == 1) {
        if ((u8)fn_802026E4(ctx, 8) == 1) {
            fn_8011BBD8(fieldD9, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 4));
            fn_80211B94(lbl_8047B62C, (void*)lbl_80379402, 0);
        } else {
            fn_80202810(ctx, 0x17);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x18 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x18) == 1) {
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 4));
        fn_80211B94(lbl_8047B62C, (void*)lbl_8037941F, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0xe */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0xe) == 1) {
        u8* msg;
        u32 vE;
        cnt = fn_80202108(ctx, 0xe);
        vE = fn_80201C58(ctx, 0xe);
        fn_8011BEB4(0, vE, 1, 0);
        fn_80132A38(0xd, (s32)fn_800FA280());
        if ((s8)cnt >= (s8)fn_80202234(ctx, 0xe)) {
            fn_80202810(ctx, 0xe);
            msg = lbl_803793C3;
        } else {
            *(u8*)((u8*)lbl_80379F58 + 0x160A4) = (u8)vE;
            *(u8*)((u8*)lbl_80379F58 + 0x160A5) = (u8)(vE >> 8);
            fn_8011BBD8(fieldD9, 0, 0x2d, 0, (u16)fn_80203B5C(ctx, 0x10));
            fn_80201FDC(ctx, 0xe, (s8)(cnt + 1));
            msg = lbl_803793A5;
        }
        fn_80211B94(lbl_8047B62C, (void*)msg, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0xb */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0xb) == 1) {
        s32 a;
        s32 next;
        fn_801F37B0(0, fn_802316FC, 0, 0);
        fn_801F4C14(0, 0, 0x36, 0, (u32)ctx);
        cnt = fn_80202108(ctx, 0xb);
        a = fn_80202234(ctx, 0xb);
        next = (s8)cnt + 1;
        if ((s8)next >= (s8)a) {
            fn_80202810(ctx, 0xb);
        } else {
            fn_80201FDC(ctx, 0xb, (s8)next);
        }
        if ((u8)fightOutPokemonCheckNoAttackFlag(ctx) == 1) {
        noAttack:
            fn_80200B10(ctx);
            lbl_80478D78[5] = 1;
        } else if ((u8)fn_802026E4(ctx, 0xb) != 1) {
            goto noAttack;
        } else {
            fn_8020248C(ctx, 0x22, 0);
            lbl_80478D78[5] = 0;
        }
        fn_80211B94(lbl_8047B62C, (void*)lbl_8037930A, 0);
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0xd */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0xd) == 1) {
        s32 a;
        s32 next;
        cnt = fn_80202108(ctx, 0xd);
        a = fn_80202234(ctx, 0xd);
        next = (s8)cnt + 1;
        if ((s8)next >= (s8)a) {
            fn_80202810(ctx, 0xd);
        } else {
            fn_80201FDC(ctx, 0xd, (s8)next);
        }
        if ((u8)fightOutPokemonCheckNoAttackFlag(ctx) == 1) {
            fn_80200B10(ctx);
        } else if ((u8)fn_802026E4(ctx, 0xd) == 0) {
            if ((u8)fn_802026E4(ctx, 0x22) == 1) {
                fn_80202810(ctx, 0x22);
                if ((u8)fn_802026E4(ctx, 9) == 0) {
                    u32 svPC;
                    cnt = (u32)lbl_80399F58;
                    save = *(u32*)(cnt + 0x1c);
                    lbl_80478D78[3] = 0x47;
                    *(u32*)(cnt + 0x1c) = (u32)lbl_80379BFE;
                    svPC = (u32)lbl_8047B610;
                    fn_802249B8(1, 0);
                    lbl_8047B610 = (u8*)svPC;
                    *(u32*)(cnt + 0x1c) = save;
                    if ((u8)fn_802026E4(ctx, 9) == 1) {
                        fn_80211B94(lbl_8047B62C, (void*)lbl_8037931E, 0);
                    }
                }
            }
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x29 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x29) == 1) {
        s32 n29;
        cnt = fn_80202108(ctx, 0x29);
        save = fn_80201C58(ctx, 0x29);
        n29 = (s8)cnt + 1;
        fn_80201FDC(ctx, 0x29, (s8)n29);
        if ((s8)fn_80123B5C(handle, save) < 0) {
            fn_80202810(ctx, 0x29);
        } else if ((s8)n29 >= (s8)fn_80202234(ctx, 0x29)) {
            fn_80202810(ctx, 0x29);
            fn_80211B94(lbl_8047B62C, (void*)lbl_80378D2C, 0);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x2a */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x2a) == 1) {
        u16 rs;
        fieldD9 = fn_80202108(ctx, 0x2a);
        side = fn_80201C58(ctx, 0x2a);
        cnt = fn_80201890(ctx, 0x2a);
        rs = (u16)fn_8012640C((void*)handle, 0, 0x7f, cnt);
        tmp2 = (s8)fieldD9 + 1;
        fn_80201FDC(ctx, 0x2a, (s8)tmp2);
        if (rs != (u16)side) {
            fn_80202810(ctx, 0x2a);
        } else if ((s8)tmp2 >= (s8)fn_80202234(ctx, 0x2a) ||
                   (u8)fn_8012640C((void*)handle, 0, 0x80, cnt) == 0) {
            fn_80202810(ctx, 0x2a);
            fn_80211B94(lbl_8047B62C, (void*)lbl_80378D40, 0);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x1d */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x1d) == 1) {
        s32 a;
        s32 next;
        side = fn_80202108(ctx, 0x1d);
        a = fn_80202234(ctx, 0x1d);
        next = (s8)side + 1;
        if ((s8)next >= (s8)a) {
            fn_80202810(ctx, 0x1d);
        } else {
            fn_80201FDC(ctx, 0x1d, (s8)next);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x24 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x24) == 1) {
        s32 a;
        s32 next;
        side = fn_80202108(ctx, 0x24);
        a = fn_80202234(ctx, 0x24);
        next = (s8)side + 1;
        if ((s8)next >= (s8)a) {
            fn_80202810(ctx, 0x24);
        } else {
            fn_80201FDC(ctx, 0x24, (s8)next);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x30 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x30) == 1) {
        s32 a;
        s32 next;
        side = fn_80202108(ctx, 0x30);
        a = fn_80202234(ctx, 0x30);
        next = (s8)side + 1;
        if ((s8)next >= (s8)a) {
            fn_80202810(ctx, 0x30);
        } else {
            fn_80201FDC(ctx, 0x30, (s8)next);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    /* 0x26 */
    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if ((u8)fn_802026E4(ctx, 0x26) == 1) {
        s32 a;
        s32 next;
        side = fn_80202108(ctx, 0x26);
        a = fn_80202234(ctx, 0x26);
        next = (s8)side + 1;
        if ((s8)next >= (s8)a) {
            fn_80202810(ctx, 0x26);
            if ((u8)fn_80203CCC(ctx) == 1) {
                if ((u16)moveId != 0x48 && (u16)moveId != 0xf) {
                    u8 found = (u8)fn_801F37B0(0, fn_8021C638, ctx, 1) != 1;
                    if (found == 0) {
                        handle = (u8)fn_801F54A4(0, 0, 0x34, 0);
                        tmp = fn_80077B60();
                        fn_80077B3C();
                        if (handle == 1 && (u8)tmp != 1) {
                            moveId = 0;
                            cnt = (u32)fn_801F025C(2, ctx);
                            tmp = (u16)fn_801F54A4(0, 0, 0x16, 0);
                            fn_801F54A4(0, 0, 0x17, 0);
                            for (side = 0; (u16)side < tmp; side++) {
                                fieldD9 = fn_801F7258(cnt, ((s16)side) & 0xffff);
                                if (fieldD9 != 0) {
                                    for (handle = 0; (u16)handle < 6; handle++) {
                                        tmp2 = fn_801F986C(fieldD9, handle);
                                        if (tmp2 != 0) {
                                            if ((u8)fn_80206608(tmp2) != 0) {
                                                if ((u8)fn_80202ADC(tmp2, 8) == 1) {
                                                    moveId++;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if ((u16)moveId < 1) {
                                goto notFound;
                            }
                            found = 1;
                            goto foundDone;
                        }
                    notFound:
                        found = 0;
                    foundDone:
                        if (found != 1) {
                            if ((u8)fn_802025B8(ctx, 8) == 2) {
                                fn_8020248C(ctx, 8, 0);
                                fn_80200B10(ctx);
                                if ((u8)fn_801FECD4(ctx) == 1) {
                                    fn_801FE7EC(ctx, 0x7c, 0, 0);
                                }
                                fn_801F4C14(0, 0, 0x47, 0, (u32)ctx);
                                fn_80211B94(lbl_8047B62C, (void*)lbl_80379464, 0);
                            }
                        }
                    }
                }
            }
        } else {
            fn_80201FDC(ctx, 0x26, (s8)next);
        }
    }
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    if ((u8)fn_802062FC(ctx) == 0) {
        return 1;
    }

    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    fn_8022EDEC(ctx, 1);
    lbl_8047B618 &= ~0x01000020;
    fn_801DA7AC();
    return 1;
}
#undef fn_800FA280
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_80123CD4
#undef fn_80123E70
#undef fn_801252E0
#undef fn_801254B4
#undef fn_8012640C
#undef fn_80132A38
#undef fn_80143AF0
#undef fn_80143B08
#undef fn_801F4C14
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80201400
#undef fn_80201704
#undef fn_80202810
#undef fn_80202998
#undef fn_80203B5C
#undef fn_80203BDC
#undef fn_80203CCC
#undef fn_80203EDC
#undef fn_80203FE4
#undef fn_802040E8
#undef fn_80205B8C
#undef fn_80205BE8
#undef fn_802062FC
#undef fn_80207BF4
#undef fn_801F0134
#undef fn_801F37B0
#undef fn_80200B10
#undef fn_80123B5C
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_801F7258
#undef fn_801F986C
#undef fn_80206608
#undef fn_80202ADC
#pragma optimize_for_size reset
/* ===== fable deep-dive #4 (harvested at credit-out; documented partial) ===== */

/* 0x80221104 | size: 0x100C */
#pragma opt_propagation off
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801C3430 battleGridUpdate
#define fn_801F0204 fightTargetDataBiosGetBuff
#define fn_801F0234 fightTargetDataBiosGetPtr
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F54A4 fightFloorGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80207C6C fightOutPokemonCreateSequence
#define fn_802085C4 fightOutPokemonWazaEffect
#define fn_80208750 fightOutPokemonLoadWazaEffect
#define fn_8020F108 fightWazaWzxTypeFuncMigawari
void fn_80221104(u32 a, u32 code) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u8 fn_801DDD28();
    extern void fn_801DA9E8();
    extern u8 fn_801DA94C();
    extern void fn_801DA8C4();
    extern void fn_80265598();
    extern void fn_80208750();
    extern void fn_801F0234();
    extern u32 fn_801F0204();
    extern void fn_802085C4();
    extern u8 fn_801F453C();
    extern u32 fn_80201C58();
    extern u16 fn_80205184();
    extern u32 fn_80207C6C();
    extern void battleGridReplacePokemon();
    extern void fn_801C3430(void);
    extern u32 fn_801254B4();
    extern void fn_801DB100();
    extern void _threadSwitch(void);
    extern u32 fn_80222110();
    extern u8 lbl_80378964[];
    extern u8 lbl_80379F58[];

    u32 ctx;
    u32 t29;
    u32 t28;
    u32 t27;
    u32 t26;
    u32 status;
    u16 eventFloor;
    u32 moveEffect;
    u8 weather;
    u32 replacementEffect;
    u32 oldStatus;
    u32 postStatus;
    u32 created;
    u8 val;
    u8 n;

    t29 = fn_801F54A4(0, 0, 0x14, 0);
    t29 = (u16)t29;
    ctx = fn_801F025C((u8)a, 0);
    t28 = fn_801F025C(0x11, 0);
    t27 = fn_801F025C(0x12, 0);
    n = lbl_80379F58[0x1601E] & 0xF;
    val = (u8)(lbl_80379F58[0x160A4] - (n - 1));

    if (code == 0x11 || code == 1 || code == 2 || code == 0x1A) {
        if (code == 1) {
            u8* p = lbl_80478D78;
            if (p != 0) {
                u8 b = p[3];
                if (b != 0) {
                    switch (b) {
                    case 0xF:
                    case 0x12:
                        t26 = 0x26;
                        break;
                    case 0x10:
                    case 0x13:
                    case 0x15:
                        t26 = 0x28;
                        break;
                    case 0x11:
                        t26 = 0x2A;
                        break;
                    case 0x14:
                        t26 = 0x2C;
                        break;
                    case 0x16:
                    case 0x19:
                        t26 = 0x27;
                        break;
                    case 0x17:
                    case 0x1A:
                    case 0x1C:
                        t26 = 0x29;
                        break;
                    case 0x18:
                        t26 = 0x2B;
                        break;
                    case 0x1B:
                        t26 = 0x3C;
                        break;
                    default:
                        t26 = fn_80222110(val, n);
                        break;
                    }
                } else {
                    t26 = fn_80222110(val, n);
                }
            } else {
                t26 = fn_80222110(val, n);
            }
            t27 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            status = fn_8012640C(ctx, 0, 0xEE, 0);
            if (status != 0) {
                if (fn_801DDD28(status, t26, 4, 0) != 0) {
                    fn_801DA9E8(status, t26, 4);
                    fn_80265598(ctx, t27, 1);
                }
            }
        }
        if (code == 0x1A) {
            t27 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            status = fn_8012640C(ctx, 0, 0xEE, 0);
            if (status != 0) {
                if (fn_801DDD28(status, 0x2D, 4, 0) != 0) {
                    fn_801DA9E8(status, 0x2D, 4);
                    fn_80265598(ctx, t27, 1);
                }
            }
        }
        if (code == 0x11) {
            fn_80208750(ctx, 0x121, 1, 0);
            fn_80208750(t28, 0x121, 2, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0x121, 1, 1, fn_801F0204());
            fn_801F0234(0x12);
            fn_802085C4(t28, 0x121, 2, 0, fn_801F0204());
        }
        if (code == 2) {
            fn_8020F108(0xA4, ctx, ctx, 0, 0);
        }
    } else if (lbl_8047B618 & 0x80) {
        fn_80211B94((void*)lbl_8047B62C, (void*)lbl_80378964, 0);
    } else if (code - 0xA <= 3 || code - 0x20 <= 2 || code == 0x1E) {
        if (code == 0xA) {
            fn_80208750(ctx, 0xF0, 1, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0xF0, 1, 0, fn_801F0204());
        }
        if (code == 0xB) {
            fn_80208750(ctx, 0xF1, 1, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0xF1, 1, 0, fn_801F0204());
        }
        if (code == 0xC) {
            fn_80208750(ctx, 0xC9, 2, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0xC9, 2, 0, fn_801F0204());
        }
        if (code == 0xD) {
            fn_80208750(ctx, 0x102, 2, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0x102, 2, 0, fn_801F0204());
        }
        if (code == 0x20) {
            fn_80208750(ctx, 0xF0, 1, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0xF0, 1, 0, fn_801F0204());
        }
        if (code == 0x22) {
            fn_80208750(ctx, 0xC9, 1, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0xC9, 1, 0, fn_801F0204());
        }
        if (code == 0x21) {
            fn_80208750(ctx, 0xF1, 1, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0xF1, 1, 0, fn_801F0204());
        }
        if (code == 0x1E) {
            if ((u8)fn_801F453C(0, 0) == 2) {
                fn_801F54A4(0, 0, 0x14, 0);
                t27 = fn_8012640C(ctx, 0, 0xEE, 0);
                if (t27 != 0) {
                    if (fn_801DDD28(t27, 0x80, 4, 0) != 0) {
                        fn_801DA9E8(t27, 0x80, 4);
                    }
                }
            } else if ((u8)fn_801F453C(0, 0) == 1) {
                fn_801F54A4(0, 0, 0x14, 0);
                t27 = fn_8012640C(ctx, 0, 0xEE, 0);
                if (t27 != 0) {
                    if (fn_801DDD28(t27, 0x7F, 4, 0) != 0) {
                        fn_801DA9E8(t27, 0x7F, 4);
                    }
                }
            } else if ((u8)fn_801F453C(0, 0) == 3) {
                fn_801F54A4(0, 0, 0x14, 0);
                t27 = fn_8012640C(ctx, 0, 0xEE, 0);
                if (t27 != 0) {
                    if (fn_801DDD28(t27, 0x7E, 4, 0) != 0) {
                        fn_801DA9E8(t27, 0x7E, 4);
                    }
                }
            } else if ((u8)fn_801F453C(0, 0) == 4) {
                fn_801F54A4(0, 0, 0x14, 0);
                t27 = fn_8012640C(ctx, 0, 0xEE, 0);
                if (t27 != 0) {
                    if (fn_801DDD28(t27, 0x7D, 4, 0) != 0) {
                        fn_801DA9E8(t27, 0x7D, 4);
                    }
                }
            } else {
                fn_801F54A4(0, 0, 0x14, 0);
                t27 = fn_8012640C(ctx, 0, 0xEE, 0);
                if (t27 != 0) {
                    if (fn_801DDD28(t27, 0x9D, 4, 0) != 0) {
                        fn_801DA9E8(t27, 0x9D, 4);
                    }
                }
            }
        }
    } else {
        if (code == 0x17) {
            t28 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t26 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t26 != 0) {
                if (fn_801DDD28(t26, 0x57, 4, 0) != 0) {
                    fn_801DA9E8(t26, 0x57, 4);
                    fn_80265598(ctx, t28, 1);
                }
            }
        }
        if (code == 0x1F) {
            fn_801F54A4(0, 0, 0x14, 0);
            t26 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t26 != 0) {
                if (fn_801DDD28(t26, 0x9D, 4, 0) != 0) {
                    fn_801DA9E8(t26, 0x9D, 4);
                }
            }
        }
        if (code == 0x18) {
            t26 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t28 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t28 != 0) {
                if (fn_801DDD28(t28, 0x58, 4, 0) != 0) {
                    fn_801DA9E8(t28, 0x58, 4);
                    fn_80265598(ctx, t26, 1);
                }
            }
        }
        if (code == 0x23) {
            t26 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t28 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t28 != 0) {
                if (fn_801DDD28(t28, 0xA3, 4, 0) != 0) {
                    fn_801DA9E8(t28, 0xA3, 4);
                    fn_80265598(ctx, t26, 1);
                }
            }
        }
        if (code == 0x24) {
            t26 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t28 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t28 != 0) {
                if (fn_801DDD28(t28, 0xD8, 4, 0) != 0) {
                    fn_801DA9E8(t28, 0xD8, 4);
                    fn_80265598(ctx, t26, 1);
                }
            }
        }
        if (code == 0x19) {
            fn_80208750(ctx, 1, 2, 0);
            fn_801F0234(0x12);
            fn_802085C4(ctx, 1, 2, 0, fn_801F0204());
        }
        if (code == 0xE) {
            t26 = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t28 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t28 != 0) {
                if (fn_801DDD28(t28, 0x38, 4, 0) != 0) {
                    fn_801DA9E8(t28, 0x38, 4);
                    fn_80265598(ctx, t26, 1);
                }
            }
        }
        if (code == 0x16) {
            fn_80208750(ctx, 0x111, 3, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0x111, 3, 0, fn_801F0204());
        }
        if (code == 0x12) {
            fn_80208750(ctx, 0xF8, 2, 0);
            fn_801F0234(0x12);
            fn_802085C4(ctx, 0xF8, 2, 0, fn_801F0204());
        }
        if (code == 0x13) {
            fn_80208750(t27, 0x161, 2, 0);
            fn_801F0234(0x12);
            fn_802085C4(t27, 0x161, 2, 0, fn_801F0204());
        }
        if (code == 0x15) {
            eventFloor = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t27 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t27 != 0) {
                if (fn_801DDD28(t27, 0x3B, 4, 0) != 0) {
                    fn_801DA9E8(t27, 0x3B, 4);
                    fn_80265598(ctx, eventFloor, 1);
                }
            }
        }
        if (code == 7) {
            eventFloor = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t27 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t27 != 0) {
                if (fn_801DDD28(t27, 0x39, 4, 0) != 0) {
                    fn_801DA9E8(t27, 0x39, 4);
                    fn_80265598(ctx, eventFloor, 1);
                }
            }
        }
        if (code == 0x1B) {
            eventFloor = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t27 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t27 != 0) {
                if (fn_801DDD28(t27, 0x32, 4, 0) != 0) {
                    fn_801DA9E8(t27, 0x32, 4);
                    fn_80265598(ctx, eventFloor, 1);
                }
            }
        }
        if (code == 0x1C) {
            fn_80208750(ctx, 0x36, 3, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0x36, 3, 0, fn_801F0204());
        }
        if (code == 0x1D) {
            fn_80208750(ctx, 0x74, 3, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0x74, 3, 0, fn_801F0204());
        }
        if (code == 9) {
            eventFloor = (u16)fn_801F54A4(0, 0, 0x14, 0);
            t27 = fn_8012640C(ctx, 0, 0xEE, 0);
            if (t27 != 0) {
                if (fn_801DDD28(t27, 0x86, 4, 0) != 0) {
                    fn_801DA9E8(t27, 0x86, 4);
                    fn_80265598(ctx, eventFloor, 1);
                }
            }
        }
        if (code == 6) {
            moveEffect = fn_80201C58(ctx, 0xE);
            fn_80208750(ctx, moveEffect, 2, 0);
            fn_801F0234(0x12);
            fn_802085C4(ctx, moveEffect, 2, 0, fn_801F0204());
        }
        if (code == 0x10) {
            if ((u16)fn_80205184(ctx) == 0x157) {
                fn_80208750(ctx, 0x157, 3, 0);
                fn_801F0234(0x11);
                fn_802085C4(ctx, 0x157, 3, 0, fn_801F0204());
            }
            if ((u16)fn_80205184(ctx) == 0xA8) {
                fn_80208750(ctx, 0xA8, 3, 0);
                fn_801F0234(0x11);
                fn_802085C4(ctx, 0xA8, 3, 0, fn_801F0204());
            }
        }
        if (code == 0) {
            weather = (u8)fn_801F453C(0, 1);
            oldStatus = fn_8012640C(ctx, 0, 0xEE, 0);
            created = fn_80207C6C(ctx, weather);
            switch ((s32)weather) {
            case 1:
                replacementEffect = 0x84;
                break;
            case 2:
                replacementEffect = 0x85;
                break;
            case 4:
                replacementEffect = 0x82;
                break;
            case 3:
            default:
                replacementEffect = 0x83;
                break;
            }
            fn_801DDD28(oldStatus, 0x81, 4, 0);
            fn_801DDD28(created, replacementEffect, 4, 0);
            fn_80265598(ctx, t29, 1);
            fn_801F54A4(0, 0, 0x14, 0);
            postStatus = fn_8012640C(ctx, 0, 0xEE, 0);
            if (postStatus != 0) {
                if (fn_801DDD28(postStatus, 0x81, 4, 0) != 0) {
                    fn_801DA9E8(postStatus, 0x81, 4);
                    while (1) {
                        if (fn_801DA94C(postStatus, 0x81, 4) == 0) {
                            break;
                        }
                        _threadSwitch();
                    }
                    fn_801DA8C4(postStatus, 0x81, 4);
                }
            }
            battleGridReplacePokemon(oldStatus, created);
            fn_801C3430();
            fn_801254B4(ctx, 0, 0xEE, 0, created);
            fn_801F54A4(0, 0, 0x14, 0);
            postStatus = fn_8012640C(ctx, 0, 0xEE, 0);
            if (postStatus != 0) {
                if (fn_801DDD28(postStatus, replacementEffect, 4, 0) != 0) {
                    fn_801DA9E8(postStatus, replacementEffect, 4);
                }
            }
            fn_801DB100(oldStatus);
        }
        if (code == 0x14) {
            fn_80208750(ctx, 0x75, 3, 0);
            fn_801F0234(0x11);
            fn_802085C4(ctx, 0x75, 3, 0, fn_801F0204());
        }
    }
}
#undef fn_801254B4
#undef fn_8012640C
#undef fn_801C3430
#undef fn_801F0204
#undef fn_801F0234
#undef fn_801F025C
#undef fn_801F453C
#undef fn_801F54A4
#undef fn_80205184
#undef fn_80207C6C
#undef fn_802085C4
#undef fn_80208750
#undef fn_8020F108
#pragma opt_propagation reset

/* ===== deep-dive #5 (sonnet, 89.7% WIP -- has residual instruction-shape
 * diffs beyond the coloring wall; correct high-level logic, several sibling
 * bug-fixes captured in header) ===== */

/* =========================================================================
 * fn_8023B498 (0x8023B498, size 0xED0 / 3792 bytes)
 * fight_range_80211A00 unit -- trainer-AI accessor range.
 *
 * STATUS: 89.7% byte-match (objdiff-cli), NOT at 100%. Delivered at plateau
 * per campaign playbook's plateau rule -- see "RESIDUAL" section below for
 * exact classification and what was tried.
 *
 * Semantics (recovered from disassembly, not from any header/symbol map):
 *   Trainer-AI move-value evaluator for a support-Pokemon slot (userPoke).
 *   1. Scans the opposing team (fn_801F1C18-supplied handle array) for
 *      several high-priority "counter" situations -- special/rare move
 *      lists, category-based double-checks, ohko/type checks -- each of
 *      which, on match, randomly selects among tied candidate move values
 *      (via fn_800E0C54 % n) and returns -1 immediately, optionally writing
 *      the chosen opponent handle / value through outMoveId / outValue.
 *   2. If no opponent short-circuits the decision, scores every
 *      (ally x move) combination: for each ally in allyArray[0..allyCount),
 *      for each move in moveList[0..moveCount), resolves a per-move-category
 *      "value" via a handler obtained from fn_8011BEB4(0, move, 0x1e, 0)
 *      (falling back to the always-0 stub fn_80253948), dispatched through
 *      a category (fn_8011BEB4(0, move, 5, 0), 0-7) that selects one of:
 *        - category 0: single handler evaluation
 *        - category 1: sum handler evaluation over a hit-count
 *        - categories 2-7: single handler evaluation (shared code)
 *      A parallel "message" evaluation (same handler, always single-call)
 *      feeds fn_80239EE8-based debug logging and sets flagB when the
 *      ally's known-move count is exceeded.
 *   3. Runs a battle-log-heavy scoring pass (fn_80239984 accumulator +
 *      fn_8023A118/fn_80239EE8 debug-message emitters, msg ids 0x34-0x3b,
 *      0x225/0x226) per ally, folding in a random jitter via
 *      fightTrainerAiAddValue when fn_8000815C() is set.
 *   4. Picks the best-scoring ally via fightTrainerAiGetValueAryMaxBanme,
 *      emits a final 0x228 debug message, and returns its index (or -1).
 *
 * RESIDUAL (89.7%, not 100%) -- classification:
 *   NOT label-only, NOT purely register-number-only: there remain a
 *   moderate number of genuine instruction-shape differences (~146 rows),
 *   layered on top of a large block of register-number-only rows (~287)
 *   that all cascade from ONE root cause (below). Two structural classes
 *   of residual, both investigated and NOT resolved after many iterations:
 *
 *   (a) PARAMETER SPILL CHOICE (root cause of most cascading diffs): the
 *       target keeps allyCount permanently in r17 for the whole function
 *       (spilling a *local*, allyMoveCount, to stack 0x110 instead). This
 *       build instead spills the allyCount PARAMETER to a stack slot and
 *       keeps allyMoveCount enregistered. Both choices are internally
 *       consistent/correct C, and total register/stack budget matches
 *       (both compiles use all of r14-r31 with exactly one spill-worthy
 *       victim); which specific value the allocator picks as "cheapest to
 *       spill" did not respond to: local-declaration reordering, wrapping
 *       loop1/loop2 locals in nested blocks to shrink their apparent live
 *       range, giving allyCount its own u16 copy for the loop2 bound
 *       (matching the target's dual register+stack-cache use), or changing
 *       allyCount's parameter type (u16 vs u32). This single divergence
 *       changes prologue register numbering for the whole function, which
 *       is why ~287 rows are flagged register-number-only.
 *   (b) LOOP CODEGEN STYLE: three loops (the zero-init loop for
 *       valueAry/threshAry/flagA/flagB, the opponent-scan loop1, and the
 *       final per-ally message loop3) compile here via pointer-walk
 *       (incrementing per-array pointers, ctr/bdnz for the zero-init loop)
 *       where the target uses index-multiply + cmplwi/blt. Both are
 *       standard -O4 loop-strength-reduction outcomes for the same C
 *       `for (k = 0; k < N; k++) { a[k]=...; b[k]=...; }` shape; which one
 *       mwcc picks did not respond to reordering the 4 zero-initialized
 *       arrays' declarations, to loop-variable width changes (u16 vs u32),
 *       or to `#pragma scheduling 602/603` (both regressed the score
 *       overall, consistent with the campaign playbook's note that the
 *       scheduling sweep fixes copy-interleave/order issues, not
 *       loop-strength-reduction or register-coloring choices).
 *   (c) SWITCH LOWERING: fn_8023B498's category dispatch (0-7, cases 2-7
 *       sharing one body) compiles here as a compare-chain instead of the
 *       target's real jumptable_8039A578 (.data, size 0x20, local scope).
 *       A jumptable WAS reproduced by writing all 8 cases out (not
 *       stacked), but that duplicates the shared body 6x and scored lower
 *       overall (82.2%) than the compare-chain (85.6% at that point in the
 *       investigation) -- so the compare-chain form was kept as the better
 *       local optimum. NEW LEVER DISCOVERED: removing a redundant
 *       `if (category <= 7)` guard around the switch (letting the switch's
 *       own implicit no-op handle out-of-range values, matching the
 *       target's actual two-site `val = 0` initialization: once on the
 *       early "debug disabled" exit, once unconditionally right before the
 *       dispatch) measurably improved the match (+0.14%) and is the
 *       correct C shape even though the jumptable itself didn't reappear.
 *
 * Real bugs found and fixed during this session (documented as they may
 * recur in sibling functions sharing these idioms):
 *   - The `fn_801FB1C0(ctx,0,0x43,0)->fn_801FB1C0(0,_,2,0)->fn_801FB1C0(0,_,X,0)`
 *     accessor chain must be written as the established NESTED single
 *     expression (`u16 v2 = fn_801FB1C0(0, fn_801FB1C0(ctx,0,0x43,0)&0xFFFF,
 *     2, 0);` then a separate 3rd call using v2) -- NOT as three separate
 *     named-variable statements. The separate-statement form forces an
 *     extra r0-route copy (`mr r0,r3`) at every occurrence; switching to
 *     the nested form recovered ~2.1% across the ~6 occurrences in this
 *     function alone.
 *   - Hex literal transcription error: the "message context" constant is
 *     0xEC6C/0xEC2C/0xEC2D (computed by the target as `lis r5,1` (0x10000)
 *     minus 0x1394/0x13d4/0x13d3), NOT 0xFC6C/0xFC2C/0xFC2D as initially
 *     transcribed from a hex-arithmetic slip. Cost ~12 call sites until
 *     caught via a `subi` immediate mismatch in the diff.
 *   - `rngVal % randDivisor` must be a SIGNED modulo (divw) to match;
 *     declaring the divisor u32 instead of s32 flips it to divwu.
 *   - Values threaded through TWO comparisons at different points (like
 *     `matched` here, compared once right after the call and again much
 *     later for the flagB gate) must NOT be narrowed at the assignment
 *     site (`u8 matched = (u8)fn_X(...)`) -- the target narrows at EACH
 *     comparison site instead (`u32 matched = fn_X(...); ... (u8)matched
 *     == 1 ... (u8)matched == 1 ...`). Narrowing-at-assignment bakes a
 *     single clrlwi into the assignment; narrowing-at-compare reproduces
 *     the target's two separate clrlwi's.
 *   - Do not introduce a fresh named local for a "final pick" value that
 *     is written through an out-pointer if an address-taken sibling
 *     variable already exists earlier in the same block for the same
 *     purpose (here: the s16 `val` used as fn_80236584's out-parameter
 *     inside the scan loop) -- reuse that SAME variable for the
 *     post-loop random pick. Because `&val` was taken inside the loop,
 *     the target keeps it stack-resident and reuses that exact stack
 *     slot for the final store, producing `sth ...; ...; lha ...`
 *     around the outMoveId/outValue write-through; introducing a new
 *     local instead keeps the value in a register and drops those two
 *     instructions.
 *   - Do not cache a resolved array element across two calls if the
 *     target re-reads it from the array a second time (here:
 *     `allyArray[bestIdx]` read twice via fn_801F4354 then fn_80205B8C in
 *     the final single-ally emission, instead of caching to a `bestAlly`
 *     local) -- caching caused an r0-route copy at that call.
 *   - When a loop stores to an array slot and then immediately compares
 *     against the SAME logical value for a running max, write the
 *     comparison against the ARRAY EXPRESSION (`if (best < arr[k])`)
 *     rather than the local variable just stored (`if (best < localVal)`)
 *     -- the target reloads from memory after the store rather than
 *     reusing the register, and only the array-expression form reproduces
 *     that reload.
 *
 * Compiler: mwcc 2.4.2b53, -O4,p, C89. #pragma optimize_for_size on is
 * REQUIRED for this function (removing it dropped the match from 88% to
 * 77% in a diagnostic run -- most of the function's field-accessor calls
 * and the divw/mod idioms depend on it).
 * ========================================================================= */

typedef struct { u16 v[11]; } SpecialMoveList22;
typedef struct { u16 a, b; } U16Pair;

extern SpecialMoveList22 lbl_8027A408;
extern U16Pair lbl_8047E628;
extern U16Pair lbl_8047E62C;

#pragma optimize_for_size on
s32 fn_8023B498(void* ctx, u32 userPoke, u16 moveCount, u16* moveList,
                u16 allyCount, u32* allyArray, u32* outMoveId, s16* outValue) {
    extern u32 fightTrainerGetStatus();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry(int a, void* ctx, u32* buf, int b, int c);
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32 context, u32 slot);
    extern u8 fightOutPokemonCheckFightOut(void* poke);
    extern u8 fn_80237F74(void* ctx, u32 poke, u32 fieldId);
    extern u32 fn_80236BFC(void* ctx, u32 poke, u32 param3);
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void pokemonGetMezamerupower();
    extern u8 fn_80236584(void* ctx, u32 poke, u32 moveId, s16* out, u32 flag);
    extern u32 fn_80237310(void* ctx, u32 param);
    extern u8 fn_8023785C(void* ctx, u32 param);
    extern u8 fn_80235714();
    extern u32 fn_8023831C(void* ctx, u32 param);
    extern s16 fightOutPokemonGetFightEntryId(void* ctx);
    extern u16 fn_802376EC(void* ctx, u32 param);
    extern u32 fn_8023C530();
    extern u8 fn_8023943C();
    extern u32 wazaGetStatus();
    extern u32 fn_80239984(u32 handle, void* ctx, u32 seq);
    extern u8 fn_80239EE8(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8);
    extern u8 fn_8023A118(u32 ctx, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7, u32 p8,
                           u16 p9, u16 p10, u32 p11);
    extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
    extern u8 fn_8000815C(void);
    extern u32 fightTrainerAiAddValue(s32 value, s32 delta);
    extern s32 fightTrainerAiGetValueAryMaxBanme(s32* valueAry, u16 count, u8 useRandom);
    extern s32 fightTrainerAiWazaDamageNull(void*, u32, u16, u32);
    extern u16 fn_800E0C54(void);

    typedef s32 (*Handler)();

    u8 flag_10c;
    s32 valueAry[8];
    s32 threshAry[8];
    s16 candArr[10];
    u32 buf98[8];
    u16 specialArr[11];
    u8 flagB[8];
    u8 flagA[8];
    u16 pairArr[4];
    u32 oppCount;
    u32 poke2Ctx;
    u32 allyCtx;
    u32 allyH;
    u16 i;
    u32 cur;
    u16 k;
    s16 val;
    u16 outA;
    u16 outB;
    u32 allyLimit;
    s32 bestThresh;
    s32 countWork;
    s32 scanWork;
    s32 bestIdx;
    u32 valueWork;

    valueWork = (u32)outValue;

    {
        u16 v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
        flag_10c = (u8)fightTrainerGetStatus(0, v2, 0x38, 0);
    }

    *(SpecialMoveList22*)specialArr = lbl_8027A408;
    *(volatile U16Pair*)&pairArr[0] = lbl_8047E628;
    *(volatile U16Pair*)&pairArr[2] = lbl_8047E62C;

    cur = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf98, 1, 1);
    oppCount = cur;
    poke2Ctx = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, userPoke);

    for (i = 0; i < (u16)oppCount; i++) {
        cur = buf98[i];
        if (cur == 0) continue;
        if (!fightOutPokemonCheckFightOut((void*)cur)) continue;
        if (userPoke == cur) continue;

        if ((u8)fn_80237F74(ctx, cur, 0x12) == 1 &&
            (u8)fn_80236BFC(ctx, cur, 0x3a) == 0) {
            countWork = 0;
            for (scanWork = 0; (u16)scanWork < 11; scanWork++) {
                u16 mv = specialArr[(u16)scanWork];
                if (mv == 0xed) {
                    pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(userPoke), &outA, &outB);
                    if (outB != 10) {
                        continue;
                    }
                }
                if ((u8)fn_80236584(ctx, userPoke, mv, &val, 1) != 0 && val >= 0) {
                    candArr[(s16)countWork++] = val;
                }
            }
            if ((s16)countWork > 0) {
                val = candArr[fn_800E0C54() % (s16)countWork];
                if (outMoveId) *outMoveId = cur;
                if (valueWork) *(s16*)valueWork = val;
                return -1;
            }
        }

        if (((u8)fn_80237F74(ctx, cur, 0x3e) == 1 || (u8)fn_80237F74(ctx, cur, 0x3f) == 1) &&
            (u8)fn_80237310(ctx, cur) == 1) {
            scanWork = 0;
            for (countWork = 0; (u16)countWork < 4; countWork++) {
                u16 mv = pairArr[(u16)countWork];
                if ((u8)fn_80236584(ctx, userPoke, mv, &val, 1) != 0 && val >= 0) {
                    candArr[(s16)scanWork++] = val;
                }
            }
            if ((s16)scanWork > 0) {
                val = candArr[fn_800E0C54() % (s16)scanWork];
                if (outMoveId) *outMoveId = cur;
                if (valueWork) *(s16*)valueWork = val;
                return -1;
            }
        }

        {
            u8 flag23 = 0;
            if ((u8)fn_80237F74(ctx, cur, 0x36) == 1) flag23 = 1;
            if ((u8)fn_8023785C(ctx, cur) == 2) {
                if ((u8)fn_80237F74(ctx, cur, 0x25) == 1 || (u8)fn_80237F74(ctx, cur, 0x4a) == 1) {
                    flag23 = 1;
                }
            }
            if ((u8)fn_8023785C(ctx, cur) == 3) {
                if ((u8)fn_80237F74(ctx, userPoke, 0x25) == 1 || (u8)fn_80237F74(ctx, userPoke, 0x4a) == 1) {
                    flag23 = 1;
                }
            }
            if (flag23 == 1) {
                if ((u8)fn_80236584(ctx, userPoke, 0x11d, &val, 1) != 0 && val >= 0) {
                    if (outMoveId) *outMoveId = cur;
                    if (valueWork) *(s16*)valueWork = val;
                    return -1;
                }
            }
        }

        {
            u8 flag23 = 0;
            if ((u8)fn_80235714(ctx, cur) == 0) {
                u16 cat;
                if ((u8)fn_80237F74(ctx, cur, 0x14) == 1) flag23 = 1;
                cat = (u16)fn_8023831C(ctx, cur);
                if (cat == 8 || cat == 9) flag23 = 1;
            }
            if (flag23 == 1) {
                if ((u8)fn_80236584(ctx, userPoke, 0xcf, &val, 1) != 0 && val >= 0) {
                    if (outMoveId) *outMoveId = cur;
                    if (valueWork) *(s16*)valueWork = val;
                    return -1;
                }
            }
        }

        if ((u8)fn_80235714(ctx, cur) == 1 && (u8)fn_80235714(ctx, userPoke) == 0) {
            if ((u8)fn_80236584(ctx, userPoke, 0xf4, &val, 1) != 0 && val >= 0) {
                if (outMoveId) *outMoveId = cur;
                if (valueWork) *(s16*)valueWork = val;
                return -1;
            }
        }
    }

    for (k = 0; k < 8; k++) {
        valueAry[k] = 0;
        threshAry[k] = 0;
        flagB[k] = 0;
        flagA[k] = 0;
    }

    allyLimit = (u16)allyCount;
    bestThresh = 0;
    for (k = 0; k < allyLimit; k++) {
        allyH = allyArray[k];
        if (allyH == 0) continue;
        if (fightOutPokemonGetFightEntryId((void*)allyH) < 0) continue;

        {
            u32 allyMoveCount = (u16)fn_802376EC(ctx, allyH);
            s32 maxVal = 0;
            u16 j;
            for (j = 0; j < moveCount; j++) {
                u32 matched;
                Handler h;
                valueWork = moveList[j];
                if ((u16)valueWork == 0 || (u16)valueWork == 0x165) continue;
                matched = fn_8023C530(ctx, userPoke, (u16)valueWork, allyH);
                if ((u8)matched == 1) flagA[k] = 1;
                if ((u8)fn_8023943C(ctx, (u16)valueWork, 1) == 0) continue;

                {
                    s32 val;
                    u16 v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
                    u32 buf60[8];
                    u32 hitCount;
                    u8 category;
                    u16 c;

                    if ((u8)fightTrainerGetStatus(0, v2, 0x2d, 0) != 1) goto damage_one_disabled;
                    h = (Handler)wazaGetStatus(0, (u16)valueWork, 0x1e, 0);
                    if (h) goto damage_one_ready;
                    h = (Handler)fightTrainerAiWazaDamageNull;
                    goto damage_one_ready;
damage_one_disabled:
                    val = 0;
                    goto damage_one_done;
damage_one_ready:
                    hitCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf60, 0, 1);
                    category = (u8)wazaGetStatus(0, (u16)valueWork, 5, 0);
                    val = 0;
                    switch (category) {
                    case 0:
                    case 3:
                        val = h(ctx, userPoke, (u16)valueWork, allyH);
                        break;
                    case 2:
                    case 4:
                    case 6:
                        for (c = 0; c < (u16)hitCount; c++) {
                            val += h(ctx, userPoke, (u16)valueWork, allyH);
                        }
                        break;
                    case 1:
                    case 5:
                    case 7:
                        val = h(ctx, userPoke, (u16)valueWork, allyH);
                        break;
                    }
damage_one_done:
                    if (maxVal < val) maxVal = val;
                }

                {
                    u16 v2 = (u16)fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0) & 0xFFFF, 2, 0);
                    s32 val2;
                    u32 buf40[8];

                    if ((u8)fightTrainerGetStatus(0, v2, 0x2d, 0) != 1) goto damage_two_disabled;
                    h = (Handler)wazaGetStatus(0, (u16)valueWork, 0x1e, 0);
                    if (h) goto damage_two_ready;
                    h = (Handler)fightTrainerAiWazaDamageNull;
                    goto damage_two_ready;
damage_two_disabled:
                    val2 = 0;
                    goto damage_two_done;
damage_two_ready:
                    fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, buf40, 0, 1);
                    (void)wazaGetStatus(0, (u16)valueWork, 5, 0);
                    val2 = h(ctx, userPoke, (u16)valueWork, allyH);
damage_two_done:
                    if ((s32)allyMoveCount < val2) {
                        if ((u8)matched == 1) flagB[k] = 1;
                    }
                }
            }
            threshAry[k] = maxVal;
            if (bestThresh < threshAry[k]) bestThresh = threshAry[k];
        }
    }

    countWork = (s32)flag_10c * 2 + 1;
    for (k = 0; k < allyCount; k++) {
        s16 valid;
        allyH = allyArray[k];
        if (allyH == 0) continue;
        valid = fightOutPokemonGetFightEntryId((void*)allyH);
        if (valid < 0) continue;

        {
            u16 species = (u16)valid;
            allyCtx = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allyH);

            fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                        allyCtx, fightOutPokemonGetPokemonPtr(allyH),
                        0, 0, 0x227, valueAry[k]);

            if ((u8)fightTrainerGetStatus(ctx, species, 0x52, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x34);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x34);
            }
            if ((u8)fightTrainerGetStatus(ctx, species, 0x53, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x35);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x35);
            }
            if ((u8)fightTrainerGetStatus(ctx, species, 0x55, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x36);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x36);
            }
            if (bestThresh <= threshAry[k]) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x37);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x37);
            }
            if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(0, ctx, 1, 1, 0, allyH) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x38);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x38);
            }
            if ((s32)fightTrainerGetStatus(ctx, species, 0x56, 0) == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x39);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x39);
            }
            if (flagA[k] == 0) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x3a);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x3a);
            }
            if (flagB[k] == 1) {
                valueAry[k] = fn_80239984(valueAry[k], ctx, 0x3b);
                fn_80239EE8(0xEC6C, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke), allyCtx,
                            fightOutPokemonGetPokemonPtr(allyH), 0, 0, 0x3b);
            }
            if ((u8)fn_8000815C() == 1) {
                u16 rngVal = fn_800E0C54();
                s32 delta = (s32)(rngVal % countWork) - flag_10c;
                valueAry[k] = fightTrainerAiAddValue(valueAry[k], delta);
                fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                            allyCtx, fightOutPokemonGetPokemonPtr(allyH),
                            0, 0, 0x225, delta);
            }
            fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                        allyCtx, fightOutPokemonGetPokemonPtr(allyH),
                        0, 0, 0x226, valueAry[k]);
        }
    }

    bestIdx = fightTrainerAiGetValueAryMaxBanme(valueAry, allyCount, 1);
    if (bestIdx < 0) {
        return -1;
    }
    {
        allyCtx = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, allyArray[bestIdx]);
        fn_8023A118(0xEC6C, 0xEC2C, 0xEC2D, poke2Ctx, fightOutPokemonGetPokemonPtr(userPoke),
                    allyCtx, fightOutPokemonGetPokemonPtr(allyArray[bestIdx]),
                    0, 0, 0x228, valueAry[bestIdx]);
    }
    return bestIdx;
}
#pragma optimize_for_size reset

/* ===== med-b0: named + C++ effect subs (fresh) ===== */

#define fn_80202810 fightOutPokemonWriteJoutaiDataId
void fightSeqSpecificationActionCounterInit(void* ctx) {
    extern u8 fn_802026E4();
    extern void fn_80202810();
    if (fn_802026E4(ctx, 0x2e) == 1) {
        fn_80202810(ctx, 0x2e);
    }
    if (fn_802026E4(ctx, 0x15) == 1) {
        fn_80202810(ctx, 0x15);
    }
    if (fn_802026E4(ctx, 0x28) == 1) {
        fn_80202810(ctx, 0x28);
    }
}
#undef fn_80202810

/*
 * fightSeqGetKetaguriIryoku (0x80213938 range) -- weight -> Low-Kick-style
 * power table lookup. lbl_80279F88 is a flat u16 array of interleaved
 * {threshold, power} pairs terminated by 0xFFFF; default power 0x78.
 */
u16 fightSeqGetKetaguriIryoku(u16 weight) {
    extern u16 lbl_80279F88[];
    u16 i;
    u16 thr;
    for (i = 0; (thr = lbl_80279F88[i]) != 0xFFFF; i += 2) {
        if (thr > weight) break;
    }
    if (thr != 0xFFFF) return lbl_80279F88[i + 1];
    return 0x78;
}

#define fn_802062FC fightOutPokemonCheckFightOut
s32 _fightSeqWsKuroikiriSub__FPvUsPv(void* ctx, u16 param2, void* param3) {
    extern u8 fn_802062FC();
    extern void fightOutPokemonInitAbiCntAll();
    if (!fn_802062FC(ctx)) return 0;
    fightOutPokemonInitAbiCntAll(ctx);
    return 1;
}
#undef fn_802062FC

#define fn_801FEF74 fightOutPokemonGetJoutaiMigawariHp
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
s32 _WsWkcActSubJoutaiMigawari__FPvUsPv(void* ctx, u16 param2, void* param3) {
    extern u8 fn_802026E4();
    extern s32 fn_801FEF74();
    extern void fn_80202810();
    if (fn_802026E4(ctx, 0x14) == 1 && fn_801FEF74(ctx) <= 0) {
        fn_80202810(ctx, 0x14);
    }
    return 1;
}
#undef fn_80202810
#undef fn_801FEF74

#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80200B10 fightOutPokemonInitJoutaiKeep
s32 _fightSeqTurnCheckSubFightOutPokemon__FPvUsPv(void* ctx, u16 param2, void* param3) {
    extern u8 fn_802062FC();
    extern void fn_80202810();
    extern u8 fn_802026E4();
    extern void fn_80200B10();
    if (!fn_802062FC(ctx)) return 1;
    fn_80202810(ctx, 0x11);
    if (fn_802026E4(ctx, 8) == 1 && fn_802026E4(ctx, 0x22) == 1) {
        fn_80200B10(ctx);
    }
    return 1;
}
#undef fn_80200B10
#undef fn_80202810
#undef fn_802062FC

/* ===== med-b2: 0x8021B-0x80222 fresh handlers ===== */

/*
 * fn_802221EC (0x802221EC)
 *
 * FightSeq party-index lock helper: resolves field 0xee off ctx b; if the
 * object exists and fn_801DDD28(obj, a, 4, 0) reports nonzero, notifies
 * fn_801DA9E8. When d==1, calls fn_80265598(b, partyCount, 1). When c==1,
 * spins on fn_801DA94C(obj, (u16)a, 4) via _threadSwitch() until it
 * returns 0, then releases via fn_801DA8C4, and if d==1 also calls
 * fn_8026532C(b, partyCount, 0).
 */
#define fn_801F54A4 fightFloorGetStatus
#define fn_8012640C pokemonGetStatus
void fn_802221EC(u32 a, u32 b, u32 c, u32 d) {
    extern u16 fn_801F54A4();
    extern u32 fn_8012640C();
    extern u8  fn_801DDD28(u32, u32, u32, u32);
    extern void fn_801DA9E8(u32, u32, u32);
    extern void fn_80265598(u32, u16, u32);
    extern u32 fn_801DA94C(u32, u16, u32);
    extern void _threadSwitch();
    extern void fn_801DA8C4(u32, u16, u32);
    extern void fn_8026532C(u32, u16, u32);
    u32 obj;
    u16 partyCount;

    partyCount = (u16)fn_801F54A4(0, 0, 0x14, 0);
    obj = (u32)fn_8012640C(b, 0, 0xee, 0);

    if (obj == 0) {
        return;
    }
    if ((u8)fn_801DDD28(obj, a, 4, 0) == 0) {
        return;
    }
    fn_801DA9E8(obj, a, 4);

    if ((u8)d == 1) {
        fn_80265598(b, partyCount, 1);
    }
    if ((u8)c != 1) {
        return;
    }

    c = (u16)a;
    for (;;) {
        if ((u8)fn_801DA94C(obj, c, 4) == 0) {
            break;
        }
        _threadSwitch();
    }
    fn_801DA8C4(obj, c, 4);

    if ((u8)d == 1) {
        fn_8026532C(b, partyCount, 0);
    }
}
#undef fn_8012640C
#undef fn_801F54A4

/*
 * fn_8021C75C (0x8021C75C)
 *
 * FightSeq opcode handler: stashes the slot-0x11 ctx into slot 0x43
 * (fn_801F4C14) -- also touching slot 0x12 via a side-effect-only
 * fn_801F025C(0x12, 0) call -- then records
 * -(u16)fn_80203B5C(ctx1, 1) into field 0x2d of ctx1's field-0xD9
 * object. If fn_80201704(ctx1) reports 1, takes the script-embedded
 * jump immediately. Otherwise, marks lbl_80478D78[5] to 1 if any of
 * event-flags 3..7 on ctx1 is set (else 0), applies event-state 8
 * (fn_8020248C/fn_80201EB0), and if fn_801FECD4(ctx1) reports 1
 * re-derives field 0x7c via fn_801FE7EC. PC advances by 5.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
#define fn_80201704 fightOutPokemonIsHpMantan
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_8021C75C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    extern void fn_801F4C14();
    extern u16 fn_80203B5C();
    extern u8  fn_80201704();
    extern u8  fn_802026E4();
    extern void fn_8020248C();
    extern void fn_80201EB0();
    extern u8  fn_801FECD4();
    extern void fn_801FE7EC();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xd9, 0);
    u16 val;

    fn_801F025C(0x12, 0);
    fn_801F4C14(0, 0, 0x43, 0, ctx1);

    val = (u16)fn_80203B5C(ctx1, 1);
    fn_8011BBD8(fieldD9, 0, 0x2d, 0, -val);

    if ((u8)fn_80201704(ctx1) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (fn_802026E4(ctx1, 3) == 1 ||
        fn_802026E4(ctx1, 4) == 1 ||
        fn_802026E4(ctx1, 5) == 1 ||
        fn_802026E4(ctx1, 6) == 1 ||
        fn_802026E4(ctx1, 7) == 1) {
        lbl_80478D78[5] = 1;
    } else {
        lbl_80478D78[5] = 0;
    }

    fn_8020248C(ctx1, 8, 0);
    fn_80201EB0(ctx1, 8, 3);

    if (fn_801FECD4(ctx1) == 1) {
        fn_801FE7EC(ctx1, 0x7c, 0, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 5;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_801F4C14
#undef fn_80203B5C
#undef fn_8011BBD8
#undef fn_80201704
#undef fn_801FECD4
#undef fn_801FE7EC
#pragma optimize_for_size reset

/*
 * fn_8021D224 (0x8021D224)
 *
 * FightSeq opcode handler: resolves ctx1's current move-effect id
 * (fn_8011BEB4(0, moveId, 9, 0)), field 0xf1 (a species/case selector)
 * and field 0xfc (a stage counter 0..3). If field 0xf1 isn't one of
 * {0xb6, 0xc5, 0xcb}, resets field 0xfc to 0. If fn_801F221C(0) reports
 * 1, disables the "canRoll" flag for this call. Rolls fn_800E0C54()
 * against a per-stage threshold table (lbl_8047E5F8[fieldFC]); when the
 * roll succeeds and canRoll is set, applies effect-specific event-state
 * clears for move-effects 0x6f/0x74 (also setting lbl_80478D78[5] to 0
 * or 1 respectively), advances fieldFC (clamped to 3), and writes it
 * back. Otherwise resets field 0xfc to 0, marks lbl_80478D78[5]=2, and
 * stashes 0x40 into slot 0x3b. PC always advances by 1.
 */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_801F221C fightFloorIsLastActionFightOutPokemon
#define fn_801F4C14 fightFloorSetStatus
void fn_8021D224(void) {
    extern u16 lbl_8047E5F8[4];
    extern u32 fn_801F025C();
    extern u16 fn_800E0C54(void);
    extern u16 fn_80205184();
    extern u32 fn_8012640C();
    extern s32 fn_8011BEB4(void*, u16, u32, u32);
    extern u32 fn_801254B4();
    extern u8  fn_801F221C();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    extern void fn_801F4C14();
    u8 canRoll = 1;
    u32 ctx1 = fn_801F025C(0x11, 0);
    u16 moveId = fn_80205184(ctx1);
    u16 fieldF1;
    u8 fieldFC;
    u16 effectId;
    u16 roll;

    effectId = (u16)fn_8011BEB4(0, moveId, 9, 0);
    fieldF1 = (u16)fn_8012640C(ctx1, 0, 0xf1, 0);
    fieldFC = (u8)fn_8012640C(ctx1, 0, 0xfc, 0);

    if (fieldF1 != 0xb6 && fieldF1 != 0xc5 && fieldF1 != 0xcb) {
        fieldFC = 0;
        fn_801254B4(ctx1, 0, 0xfc, 0, 0);
    }

    if (fn_801F221C(0) == 1) {
        canRoll = 0;
    }

    roll = (u16)fn_800E0C54();
    if (lbl_8047E5F8[fieldFC] > roll && canRoll != 0) {
        if (effectId == 0x6f) {
            if (fn_802025B8(ctx1, 0x2b) == 2) {
                fn_8020248C(ctx1, 0x2b, 0);
            }
            lbl_80478D78[5] = 0;
        }
        if (effectId == 0x74) {
            if (fn_802025B8(ctx1, 0x2c) == 2) {
                fn_8020248C(ctx1, 0x2c, 0);
            }
            lbl_80478D78[5] = 1;
        }
        fieldFC++;
        if (fieldFC > 3) {
            fieldFC = 3;
        }
        fn_801254B4(ctx1, 0, 0xfc, 0, fieldFC);
    } else {
        fn_801254B4(ctx1, 0, 0xfc, 0, 0);
        lbl_80478D78[5] = 2;
        fn_801F4C14(0, 0, 0x3b, 0, 0x40);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F025C
#undef fn_80205184
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_801254B4
#undef fn_801F221C
#undef fn_801F4C14

/*
 * fn_8021D9C0 (0x8021D9C0)
 *
 * FightSeq opcode handler: resolves the ctx for the script-embedded
 * slot byte at PC+1, then its trainer object (fn_801F4354(0, ctx)).
 * Derives two booleans -- slotActive (fn_80204A10(ctx)==1) and
 * canTrigger (fn_801F18DC(0)==1) -- with canTrigger forced to 1 when
 * fn_801F8000(trainer) reports 0 and slotActive is clear. Records five
 * trainer-AI fields (0x16/0xd from fn_802037DC(ctx), 0x22 from
 * fn_801F8000(trainer), 0x23/0x25 from fn_801F8100(trainer)) via
 * fn_80132A38, then classifies the result into lbl_80478D78[5]: 5 when
 * canTrigger; else, when slotActive, buckets fn_801F2350(0, ctx) into
 * {<0:1, ==0:0, <0x1e:1, <0x46:2, else:3}; else 4. PC advances by 2.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_80132A38 msgctrlSetValue
#define fn_801F8100 fightTrainerGetNamePtr
#define fn_801F2350 fightFloorGetEnemyFightSideManyDamageHpRate
void fn_8021D9C0(void) {
    extern u32 fn_801F025C();
    extern void* fn_801F4354();
    extern u8    fn_80204A10();
    extern u8    fn_801F18DC();
    extern u32   fn_801F8000();
    extern void* fn_802037DC();
    extern void  fn_80132A38(s32, s32);
    extern void* fn_801F8100();
    extern s32   fn_801F2350();
    u32 ctx1 = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    u32 obj1 = (u32)fn_801F4354(0, ctx1);
    u32 slotActive = ((u8)fn_80204A10(ctx1) == 1);
    u32 canTrigger = ((u8)fn_801F18DC(0) == 1);
    s32 ret8;

    if ((u32)fn_801F8000(obj1) == 0 && (u8)slotActive == 0) {
        canTrigger = 1;
    }

    fn_80132A38(0x16, (s32)fn_802037DC(ctx1));
    fn_80132A38(0xd, (s32)fn_802037DC(ctx1));
    fn_80132A38(0x22, (s32)fn_801F8000(obj1));
    fn_80132A38(0x23, (s32)fn_801F8100(obj1));
    fn_80132A38(0x25, (s32)fn_801F8100(obj1));

    if ((u8)canTrigger == 1) {
        lbl_80478D78[5] = 5;
    } else if ((u8)slotActive == 1) {
        ret8 = fn_801F2350(0, ctx1);
        if (ret8 < 0) {
            lbl_80478D78[5] = 1;
        } else if (ret8 == 0) {
            lbl_80478D78[5] = 0;
        } else if (ret8 < 0x1e) {
            lbl_80478D78[5] = 1;
        } else if (ret8 < 0x46) {
            lbl_80478D78[5] = 2;
        } else {
            lbl_80478D78[5] = 3;
        }
    } else {
        lbl_80478D78[5] = 4;
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_801F4354
#undef fn_80204A10
#undef fn_802037DC
#undef fn_80132A38
#undef fn_801F8100
#undef fn_801F2350
#pragma optimize_for_size reset

/*
 * fn_8021DB78 (0x8021DB78)
 *
 * Same shape as fn_8021D9C0 (ctx/trainer resolve, slotActive/canTrigger
 * derivation, five fn_80132A38 field writes), but the final bucket uses
 * fn_801F2434(0, ctx) instead of fn_801F2350 and different thresholds:
 * <0 or >=0x46 -> 0; >=0x28 -> 1; >=0xa -> 2; else -> 3.
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_80132A38 msgctrlSetValue
#define fn_801F8100 fightTrainerGetNamePtr
#define fn_801F2434 fightFloorGetEnemyFightSideLittleNokoriHpRate
void fn_8021DB78(void) {
    extern u32 fn_801F025C();
    extern void* fn_801F4354();
    extern u8    fn_80204A10();
    extern u8    fn_801F18DC();
    extern u32   fn_801F8000();
    extern void* fn_802037DC();
    extern void  fn_80132A38(s32, s32);
    extern void* fn_801F8100();
    extern s32   fn_801F2434();
    u32 ctx1 = fn_801F025C(*(u8*)(lbl_8047B610 + 1), 0);
    u32 obj1 = (u32)fn_801F4354(0, ctx1);
    u32 slotActive = ((u8)fn_80204A10(ctx1) == 1);
    u32 canTrigger = ((u8)fn_801F18DC(0) == 1);
    s32 ret8;

    if ((u32)fn_801F8000(obj1) == 0 && (u8)slotActive == 0) {
        canTrigger = 1;
    }

    fn_80132A38(0x16, (s32)fn_802037DC(ctx1));
    fn_80132A38(0xd, (s32)fn_802037DC(ctx1));
    fn_80132A38(0x22, (s32)fn_801F8000(obj1));
    fn_80132A38(0x23, (s32)fn_801F8100(obj1));
    fn_80132A38(0x25, (s32)fn_801F8100(obj1));

    if ((u8)canTrigger == 1) {
        lbl_80478D78[5] = 5;
    } else if ((u8)slotActive == 1) {
        ret8 = fn_801F2434(0, ctx1);
        if (ret8 >= 0x46 || ret8 < 0) {
            lbl_80478D78[5] = 0;
        } else if (ret8 >= 0x28) {
            lbl_80478D78[5] = 1;
        } else if (ret8 >= 0xa) {
            lbl_80478D78[5] = 2;
        } else {
            lbl_80478D78[5] = 3;
        }
    } else {
        lbl_80478D78[5] = 4;
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}
#undef fn_801F025C
#undef fn_801F4354
#undef fn_80204A10
#undef fn_802037DC
#undef fn_80132A38
#undef fn_801F8100
#undef fn_801F2434
#pragma optimize_for_size reset

/* ===== med-b1: 0x80216-0x8021B fresh handlers ===== */

/*
 * fn_80216650 (0x80216650)
 */
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_802656AC fightMenuGetFightOutPokemonPtrToStatusMenuId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
void fn_80216650(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_802656AC();
    extern void fn_8011BBD8();
    extern u32 fn_80205B8C();
    extern void menuFightStatusSetHP();
    u32 ctx1;
    u32 fieldD9;
    u32 ctx2;
    u8 flagA;
    u32 result;
    u8 flagB;
    u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);

    ctx1 = fn_801F025C(0x11, 0);
    fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    result = fn_802656AC(ctx1, val, 1);
    ctx2 = fn_801F025C(0x12, 0);
    flagA = (u8)fn_8012640C(ctx2, 0, 0xE6, 0);
    flagB = (u8)fn_8012640C(ctx2, 0, 0xE8, 0);

    if (flagA == 0 && flagB == 0 && lbl_80478D78[6] != 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        u16 poke_val = (u16)fn_8012640C(fn_80205B8C(ctx1), 0, 0x83, 0);
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, poke_val);
        menuFightStatusSetHP(result, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_8012640C
#undef fn_802656AC
#undef fn_80205B8C
#undef fn_8011BBD8

/*
 * fn_80217220 (0x80217220)
 */
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_800FA280 GSmsgGetGSchar
#define fn_80132A38 msgctrlSetValue
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6EEC fightSideInitJoutaiDataId
void fn_80217220(void) {
    extern u32 fn_801F54A4();
    extern u32 fn_801F025C();
    extern u8  fn_802026E4();
    extern void fn_801F4C14();
    extern s32 fn_8011BEB4();
    extern u8  fn_801F6E98();
    extern void fn_80202810();
    extern u32 fn_80201D84();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fn_80201C58();
    extern u32 fn_800FA280();
    extern void fn_80132A38();
    extern void fn_80211B94();
    extern u8  fn_801F6EEC();
    extern u8  lbl_80378EFD[];
    extern u8  lbl_80378F11[];
    extern u8  lbl_80378F25[];
    u16 val = (u16)fn_801F54A4(0, 0, 0x14, 0);
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 sub = fn_801F025C(2, ctx1);
    u32 ctx2 = fn_801F025C(0x12, 0);
    u32 tmp;
    u32 target;
    u16 tmpNarrow;

    if (fn_802026E4(ctx1, 0xe) == 1) {
        tmp = fn_80201D84(ctx1, 0xe);
        tmpNarrow = (u16)tmp;
        if (tmpNarrow != 0) {
            target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(tmp, val);
            if (target != 0) {
                fn_801F4C14(0, 0, 0x42, 0, target);
                {
                    u32 t = fn_80201C58(ctx1, 0xe);
                    fn_80132A38(0xd, fn_800FA280(fn_8011BEB4(0, t, 1, 0)));
                }
                fn_80202810(ctx1, 0xe);
                fn_80211B94(lbl_8047B62C, (void*)lbl_80378EFD, 0);
                fn_801F4C14(0, 0, 0x42, 0, ctx2);
            }
        }
    }

    if (fn_802026E4(ctx1, 0x1c) == 1) {
        fn_80202810(ctx1, 0x1c);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80378F11, 0);
    }

    if (fn_801F6E98(sub, 0x4a) == 1) {
        fn_801F6EEC(sub, 0x4a);
        fn_80211B94(lbl_8047B62C, (void*)lbl_80378F25, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_801F4C14
#undef fn_8011BEB4
#undef fn_800FA280
#undef fn_80132A38
#undef fn_80202810
#undef fn_801F6E98
#undef fn_801F6EEC
#pragma optimize_for_size reset

/*
 * fn_802195A0 (0x802195A0)
 */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
#define fn_801254B4 pokemonSetStatus
void fn_802195A0(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern u32 fn_80205B8C();
    extern u8  fn_802026E4();
    extern void fn_8011BBD8();
    extern void fn_801254B4();
    extern s32 lbl_8047B608;
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 ctx2;
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xD9, 0);
    u32 poke1 = fn_80205B8C(ctx1);
    s32 statA = (u16)fn_8012640C(poke1, 0, 0x83, 0);
    u32 poke2;
    s32 statB;

    ctx2 = fn_801F025C(0x12, 0);
    poke2 = fn_80205B8C(ctx2);
    statB = (u16)fn_8012640C(poke2, 0, 0x83, 0);

    if (fn_802026E4(ctx2, 0x14) == 0) {
        s32 avg = (statA + statB) / 2;
        lbl_8047B608 = statB - avg;
        fn_8011BBD8(fieldD9, 0, 0x2d, 0, statA - avg);
        fn_801254B4(ctx2, 0, 0x11b, 0, 0xFFFF);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205B8C
#undef fn_8011BBD8
#undef fn_801254B4
#pragma optimize_for_size reset

/* Trivial constant return. */
s32 fn_8023C368(void) { return 0; }

/*
 * u16 scaling helpers: scale a value by *10/25.
 * fn_80217BD0 inverts first (0xFF - x).
 */
#pragma optimize_for_size on
u16 fn_80217BEC(u16 x) { return (u16)((x * 10) / 25); }
u16 fn_80217BD0(u16 x) { return (u16)(((0xFF - x) * 10) / 25); }
#pragma optimize_for_size reset

/* ===== Wave 6 integrated matches ===== */

/* fn_80214794 (0x80214794) 100% */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_8020147C fightOutPokemonDoItemSoubi
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
void fn_80214794(void) {
    extern u8* lbl_8047B610;
    extern void* fn_801F025C();
    extern u32 fn_802040E8();
    extern u32 fn_8012640C();
    extern u8 fn_801FECD4(void* trainer);
    extern void fn_801FE7EC(void* trainer, u32 eventId, u32 param1, u32 param2);
    extern u8 fn_801F4C14(u32, u16, u32, u16, u32);
    extern u16 fn_8020147C(void* context, u16 moveId, u8 slot, u8 updateFlag);
    void* ctx;
    u32 val2;
    u16 val3;
    ctx = fn_801F025C(0x11, 0);
    val2 = fn_802040E8(ctx);
    val3 = (u16)fn_8012640C((u32)ctx, 0, 0xfa, 0);
    if (val3 != 0 && (u16)val2 == 0) {
        fn_801F4C14(0, 0, 0x56, 0, val3);
        fn_8020147C(ctx, val3, 1, 1);
        if (fn_801FECD4(ctx) == 1) {
            fn_801FE7EC(ctx, 0x82, 0, 0);
        }
        lbl_8047B610 += 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_802040E8
#undef fn_8012640C
#undef fn_801F4C14
#undef fn_8020147C
#undef fn_801FECD4
#undef fn_801FE7EC
#pragma optimize_for_size reset

/* fn_802149B8 (0x802149B8) 100% */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
void fn_802149B8(void) {
    extern u8* lbl_8047B610;
    extern u8  lbl_80478D78[1];
    extern void* fn_801F025C();
    extern u16 fn_80207BF4(void*);
    extern u32 fn_80205184(void*);
    extern u32 fn_8011BEB4();
    extern u8  fn_802025B8();
    extern void fn_8020248C();
    void* ctx;
    u32 val;
    u16 result;
    u8 flag;
    ctx = fn_801F025C(0x11, 0);
    fn_80207BF4(ctx);
    val = fn_80205184(ctx);
    result = (u16)fn_8011BEB4(0, val, 9, 0);
    flag = 0;
    if (result == 0xc9) {
        if ((u8)fn_802025B8(ctx, 0x38) == 2) {
            fn_8020248C(ctx, 0x38, 0);
            lbl_80478D78[5] = 0;
            flag = 1;
        }
    } else {
        if ((u8)fn_802025B8(ctx, 0x39) == 2) {
            fn_8020248C(ctx, 0x39, 0);
            lbl_80478D78[5] = 1;
            flag = 1;
        }
    }
    if (flag != 0) {
        lbl_8047B610 += 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}
#undef fn_801F025C
#undef fn_80207BF4
#undef fn_80205184
#undef fn_8011BEB4
#pragma optimize_for_size reset

/* fn_8021F1CC (0x8021F1CC) 100% */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_80203758 fightOutPokemonGetNamePtr
#define fn_80132A38 msgctrlSetValue
void fn_8021F1CC(void) {
    extern u32 fn_801F025C();
    extern u32 fn_802037DC();
    extern u32 fn_80203758();
    extern void fn_80132A38();
    void* ctx;
    u8 byte;
    u8 arg1 = *(u8*)(lbl_8047B610 + 1);
    ctx = (void*)fn_801F025C(arg1, 0);
    byte = *(u8*)(lbl_8047B610 + 2);
    switch (byte) {
    case 0:
    case 1:
        break;
    case 2:
        fn_80132A38(0xd, fn_802037DC(ctx));
        fn_80132A38(0xe, fn_80203758(ctx));
        break;
    }
    lbl_8047B610 += 3;
}
#undef fn_80132A38
#undef fn_80203758
#undef fn_802037DC
#undef fn_801F025C

/* fn_8021C588 (0x8021C588) 100% */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void fn_8021C588(void) {
    extern void* fn_801F025C(u32, u32);
    extern u16   fn_80207BF4(void*);
    extern void  fn_8021C638();
    extern u8    fn_801F37B0(u32, void*, void*, u32);
    void* ctx = fn_801F025C(0x12, 0);
    u16 moveId = fn_80207BF4(ctx);
    u32 target = *(u32*)(lbl_8047B610 + 1);
    u8 found = (u8)fn_801F37B0(0, fn_8021C638, ctx, 1) != 1;
    if (found != 0) {
        lbl_8047B610 = (u8*)target;
        return;
    }
    if (moveId == 0xf || moveId == 0x48) {
        lbl_80478D78[5] = 2;
        lbl_8047B610 = (u8*)target;
        return;
    }
    lbl_8047B610 += 5;
}
#undef fn_801F025C
#undef fn_80207BF4
#undef fn_801F37B0
#pragma optimize_for_size reset

/* fn_8021CE60 (0x8021CE60) 100% */
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8011BBD8 wazaSetStatus
#define fn_80201704 fightOutPokemonIsHpMantan
void fn_8021CE60(void) {
    extern void* fn_801F025C();
    extern void* fn_8012640C();
    extern void fn_801F4C14();
    extern u16 fn_80203B5C();
    extern void fn_8011BBD8();
    extern u8 fn_80201704();
    void* ctx1 = fn_801F025C(0x11, 0);
    void* ctx2;
    void* obj = fn_8012640C(ctx1, 0, 0xD9, 0);
    u8* pc;
    u8 sel;
    u8* target;
    u16 result;

    ctx2 = fn_801F025C(0x12, 0);
    pc = lbl_8047B610;
    sel = pc[5];
    target = *(u8**)(pc + 1);

    if (sel == 0x11) {
        ctx2 = ctx1;
        fn_801F4C14(0, 0, 0x43, 0, (u32)ctx1);
    }
    result = (u16)fn_80203B5C(ctx2, 2);
    fn_8011BBD8(obj, 0, 0x2d, 0, -(s32)result);
    if ((u8)fn_80201704(ctx2) == 1) {
        lbl_8047B610 = target;
    } else {
        lbl_8047B610 = lbl_8047B610 + 6;
    }
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_801F4C14
#undef fn_80203B5C
#undef fn_8011BBD8
#undef fn_80201704
#pragma optimize_for_size reset

/* fn_802151C0 (0x802151C0) 100% */
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80203D3C figthOutPokemonGetPokemonDataId
#define fn_8011BBD8 wazaSetStatus
void fn_802151C0(void) {
    extern void* fn_801F025C();
    extern void* fn_8012640C();
    extern u32 fn_80203D3C(void*);
    extern void fn_8011BBD8();
    extern u16 lbl_80279F88[];
    void* ctx1 = fn_801F025C(0x11, 0);
    void* obj = fn_8012640C(ctx1, 0, 0xD9, 0);
    void* ctx2 = fn_801F025C(0x12, 0);
    u32 level = fn_80203D3C(ctx2);
    u16 result = (u16)(u32)fn_8012640C(0, level, 0x5F, 0);
    u16 i;
    u16 thr;
    u16 val;

    for (i = 0; (thr = lbl_80279F88[i]) != 0xFFFF; i += 2) {
        if (thr > result) break;
    }
    val = (thr != 0xFFFF) ? lbl_80279F88[i + 1] : 0x78;
    fn_8011BBD8(obj, 0, 0x2f, 0, val);
    lbl_8047B610 += 1;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80203D3C
#undef fn_8011BBD8


/* ===== Newer master-only definitions retained after campaign union ===== */

void WS_ONNEN(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);

    if (fn_802025B8(ctx, 0x28) == 2) {
        fn_8020248C(ctx, 0x28, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    }
}

void WS_CHOUHATSU(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fn_802025B8(ctx, 0x30) != 2) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x30, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

void WS_ICHAMON(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fn_802025B8(ctx, 0x1b) != 2) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x1b, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

void WS_NEWOHARU(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);

    if (fn_802025B8(ctx, 0x25) != 2) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(ctx, 0x25, 0);
        lbl_8047B610 = lbl_8047B610 + 5;
    }
}

void WS_AKUBI(void) {
    extern u8 fightOutPokemonIsJoutaiNormal();
    u32 ctx = fightTargetGetPtrAsNowFightType(0x12, 0);

    if (fn_802025B8(ctx, 0x26) != 2) {
        goto deref;
    }
    if (fightOutPokemonIsJoutaiNormal(ctx) != 0) {
        goto plus5;
    }
deref:
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    return;
plus5:
    fn_8020248C(ctx, 0x26, 0);
    lbl_8047B610 = lbl_8047B610 + 5;
}

void WS_CHIISAKUNARU(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);

    if ((lbl_8047B618 & 0x2000000) != 0) {
        if (fn_802025B8(ctx, 0x23) == 2) {
            fn_8020248C(ctx, 0x23, 0);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

void WS_KIERUTAME_AFTAR(void) {
    extern u8 fn_802026E4();
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    u16 moveId = fightOutPokemonGetUseWazaDataId(ctx);

    switch (moveId) {
    case 0x13:
    case 0x154:
        if (fn_802026E4(ctx, 0x1f) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x1f);
        }
        break;
    case 0x5b:
        if (fn_802026E4(ctx, 0x20) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x20);
        }
        break;
    case 0x123:
        if (fn_802026E4(ctx, 0x21) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x21);
        }
        break;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

void WS_KIERUTAME(void) {
    u32 ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    u16 moveId = fightOutPokemonGetUseWazaDataId(ctx);

    switch (moveId) {
    case 0x13:
    case 0x154:
        if (fn_802025B8(ctx, 0x1f) == 2) {
            fn_8020248C(ctx, 0x1f, 0);
        }
        break;
    case 0x5b:
        if (fn_802025B8(ctx, 0x20) == 2) {
            fn_8020248C(ctx, 0x20, 0);
        }
        break;
    case 0x123:
        if (fn_802025B8(ctx, 0x21) == 2) {
            fn_8020248C(ctx, 0x21, 0);
        }
        break;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

inline u32 inline_fn() {
    return fightTargetGetPtrAsNowFightType(0x12, 0);
}

#pragma optimize_for_size on
void fn_8021C900(void) {
    u32 ctx1 = fightTargetGetPtrAsNowFightType(0x11, 0);
    u32 fieldD9 = pokemonGetStatus(ctx1, 0, 0xD9, 0);
    s32 val = wazaGetStatus(fieldD9, 0, 0x2d, 0);
    u32 ctx2 = fightTargetGetPtrAsNowFightType(0x12, 0);
    u32 poke2 = fightOutPokemonGetPokemonPtr(ctx2);
    s32 stat = pokemonGetStatus(poke2, 0, 0x87, 0);
    u8 op = ((FightSeqOpU8Operand*)lbl_8047B610)->operand;

    switch (op) {
    case 0:
        val *= -1;
        break;
    case 1:
        val = val / 2;
        if (val == 0) {
            val = 1;
        }
        if (stat / 2 < val) {
            val = stat / 2;
        }
        break;
    case 2:
        val = val << 1;
        break;
    }
    wazaSetStatus(fieldD9, 0, 0x2d, 0, val);
    lbl_8047B610 = lbl_8047B610 + 2;
}
#pragma optimize_for_size reset

/* UNION_BATCH: WS_ONNEN,WS_CHOUHATSU,WS_ICHAMON,WS_NEWOHARU,WS_AKUBI,WS_CHIISAKUNARU,WS_KIERUTAME_AFTAR,WS_KIERUTAME,inline_fn,fn_8021C900 */


/* ===== Newer master-only definitions retained after campaign union ===== */

#pragma optimize_for_size on
void fn_802232F4(void) {
    u8 gridOpened;
    u16 statusMenuId;
    u32 expAfter;
    s32 trainerIdx;
    u32 trainer;
    s32 partyIdx;
    u32 pokemon;
    u32 pokemonPtr;
    u16 expEachShare;
    s32 metCount;
    u16 trainerCount;
    u32 expGain;
    u32 defeatedLevel;
    u32 side;
    u32 fightOut;
    u32 target;
    u16 expEachMet;
    u32 pokemonDataId;
    u16 baseExp;
    struct {
        u32 value;
    } party;
    u32 openedMsg;
    s32 shareCount;

    statusMenuId = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    gridOpened = 0;
    target = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    pokemonDataId = figthOutPokemonGetPokemonDataId(target);
    baseExp = (u16)pokemonGetStatus(0, pokemonDataId, 0x10, 0);
    defeatedLevel = figthOutPokemonGetLevel(target);

    if ((u8)fightOutPokemonIsGcHeroFightOutPokemon(target) != 1 &&
        (u8)fightFloorGetStatus(0, 0, 0x24, 0) != 0) {
        side = fightTargetGetPtrAsNowFightType(3, target);
        if ((u8)fightSideCheckValid(side) != 0) {
            metCount = 0;
            shareCount = 0;
            trainerCount = (u16)fightFloorGetStatus(0, 0, 0x16, 0);

            {
                u32 scanTrainer;
                u32 scanParty;
                s32 scanTrainerIdx;
                s32 scanPartyIdx;
                u32 scanPokemon;

                for (scanTrainerIdx = 0; scanTrainerIdx < trainerCount; scanTrainerIdx++) {
                    scanTrainer =
                        fightSideGetValidFightTrainerPtr(side, scanTrainerIdx & 0xffff);
                    if (scanTrainer != 0 &&
                        (u8)fightTrainerCheckCanGetExp(scanTrainer) != 0) {
                        scanParty = fightTrainerGetStatus(scanTrainer, 0, 0x44, 0);
                        for (scanPartyIdx = 0; scanPartyIdx < 6; scanPartyIdx++) {
                            scanPokemon = fightTrainerCheckTemotiPokemonFightEntry(
                                scanTrainer,
                                heroGetStatus(scanParty, 3, scanPartyIdx & 0xffff));
                            if (scanPokemon != 0 &&
                                (u8)fightPokemonCheckFightOut(scanPokemon) != 0) {
                                if ((u8)fightOutPokemonCheckMeetEnemyFightPokemon(
                                        target, scanPokemon) == 1) {
                                    metCount++;
                                }
                                if ((u16)fightPokemonGetSoubiItemSoubiDataId(scanPokemon) ==
                                    0x19) {
                                    shareCount++;
                                }
                            }
                        }
                    }
                }
            }

            if (metCount != 0 || shareCount != 0) {
                expEachMet = (u16)((baseExp * (u8)defeatedLevel) / 7);
                if (shareCount != 0) {
                    expEachMet = (u16)(expEachMet >> 1);
                    expEachShare = (u16)(expEachMet / metCount);
                    if (expEachShare == 0) {
                        expEachShare = 1;
                    }
                    expEachMet = (u16)(expEachMet / shareCount);
                    if (expEachMet == 0) {
                        expEachMet = 1;
                    }
                } else {
                    expEachShare = (u16)(expEachMet / metCount);
                    if (expEachShare == 0) {
                        expEachShare = 1;
                    }
                    expEachMet = 0;
                }

                for (trainerIdx = 0; trainerIdx < trainerCount; trainerIdx++) {
                    trainer = fightSideGetStatus(side, 0, 7, trainerIdx & 0xffff);
                    if ((u8)fightTrainerCheckCanGetExp(trainer) != 0) {
                        party.value = fightTrainerGetStatus(trainer, 0, 0x44, 0);
                        for (partyIdx = 0; partyIdx < 6; partyIdx++) {
                            expGain = 0;
                            pokemon = fightTrainerCheckTemotiPokemonFightEntry(
                                trainer, heroGetStatus(party.value, 3, partyIdx & 0xffff));
                            if (pokemon == 0 || (u8)fightPokemonCheckFightOut(pokemon) == 0 ||
                                (u8)figthPokemonGetLevel(pokemon) >= 100) {
                                continue;
                            }

                            if ((u8)fightOutPokemonCheckMeetEnemyFightPokemon(target, pokemon) == 1) {
                                expGain = expEachShare;
                            }
                            if ((u16)fightPokemonGetSoubiItemSoubiDataId(pokemon) == 0x19) {
                                expGain += expEachMet;
                            }
                            if (expGain == 0) {
                                continue;
                            }
                            if ((u16)fightPokemonGetSoubiItemSoubiDataId(pokemon) == 0x28) {
                                expGain = (expGain * 150) / 100;
                            }
                            if ((u8)fightFloorGetStatus(0, 0, 0x2d, 0) == 1) {
                                expGain = (expGain * 150) / 100;
                            }
                            if ((u8)fightTrainerIsMineFightPokemon(trainer, pokemon) == 0) {
                                expGain = (expGain * 150) / 100;
                                openedMsg = 0x7534;
                            } else {
                                openedMsg = 0x7533;
                            }

                            pokemonPtr = fightPokemonGetPokemonPtr(pokemon);
                            if ((u8)pokemonIsDarkPokemon(pokemonPtr) == 1) {
                                if ((u8)pokemonGetDarkPokemonLevel(pokemonPtr) >= 3) {
                                    u32 darkExp = pokemonGetStatus(pokemonPtr, 0, 0xc6, 0);
                                    darkExp += expGain;
                                    pokemonSetStatus(pokemonPtr, 0, 0xc6, 0, darkExp);
                                    fightOut = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
                                    if (fightOut != 0 &&
                                        (u8)fightOutPokemonIsUseHensinBuff(fightOut) == 1) {
                                        fightOutPokemonSetHensinPokemonStatusId(fightOut, 0xc6, 0, 0);
                                    }
                                }
                                continue;
                            }

                            if (gridOpened == 0) {
                                battleGridUpdate();
                                fn_801EF8F4(1);
                                gridOpened = 1;
                            }

                            fightOut = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
                            if (fightOut != 0) {
                                fn_80265598(fightOut, statusMenuId, 1);
                            }
                            msgctrlSetValue(0xd, fightPokemonGetNicknamePtr(pokemon));
                            msgctrlSetValue(0xe, GSmsgGetGSchar(openedMsg));
                            msgctrlSetValue(0x2f, expGain);
                            openedMsg = fightMenuOpenMsg(0x7532);
                            fightMenuCloseMsg();
                            fightPokemonGetEffortFromPokemon(pokemon, 0, pokemonDataId);
                            for (;;) {
                                u32 oldLevel;
                                u32 expNow;
                                u32 nextExp;

                                if (expGain == 0) {
                                    break;
                                }
                                if ((u8)fightPokemonCheckFightOut(pokemon) == 0) {
                                    break;
                                }
                                oldLevel = figthPokemonGetLevel(pokemon);
                                if ((u8)oldLevel >= 100) {
                                    break;
                                }

                                fightPokemonToMenuLvupStatus(pokemon, lbl_80478278);
                                lbl_8047B64C = (void*)pokemon;
                                expNow = figthPokemonGetExp(pokemon);
                                nextExp = fightPokemonGetLevelToExp(
                                    pokemon, (oldLevel + 1) & 0xff);
                                expAfter = expNow + expGain;
                                if (expAfter >= nextExp) {
                                    expGain = expAfter - nextExp;
                                    fightPokemonGrowBasisStatus(pokemon, nextExp);
                                    msgctrlSetValue(0xd, fightPokemonGetNicknamePtr(pokemon));
                                    msgctrlSetValue(0x2f, (u8)figthPokemonGetLevel(pokemon));
                                    if ((u8)fightFloorGetStatus(0, 0, 0x27, 0) == 1 &&
                                        (u8)fightTrainerIsGcHero(trainer) == 1) {
                                        fightPokemonGetFriendFormPokemonFriendFilterId(pokemon, 0);
                                    }
                                    pokemonSetStatus(pokemon, 0, 0xd0, 0, 1);
                                    fightOut = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
                                    if (fightOut != 0) {
                                        if ((u8)fightOutPokemonIsUseHensinBuff(fightOut) == 1) {
                                            fightOutPokemonSetHensinStatusAfterLevelUp(fightOut);
                                        }
                                        if ((u8)fightOutPokemonIsUseHensinBuff(fightOut) == 1) {
                                            fightOutPokemonSetHensinFightPokemonStatusId(fightOut, 0xd0, 0);
                                        }
                                    }
                                    fightOut = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
                                    if (fightOut != 0) {
                                        u32 menuId = fightMenuGetFightOutPokemonPtrToStatusMenuId(
                                            fightOut, statusMenuId, 0);
                                        menuFightStatusStartAnimEXP(
                                            menuId,
                                            nextExp - fightPokemonGetLevelToExp(
                                                          pokemon, oldLevel & 0xff));
                                        menuFightStatusWaitAnimeEXP(menuId, 1);
                                        fightMenuFightOutPokemonRenewStatusMenu(fightOut, statusMenuId, 1);
                                    }
                                    fn_80211B94(lbl_8047B62C, lbl_80378724, 0);
                                    openedMsg = 0;
                                } else {
                                    expGain = 0;
                                    figthPokemonSetExp(pokemon, expAfter);
                                    fightOut = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
                                    if (fightOut != 0) {
                                        nextExp = fightMenuGetFightOutPokemonPtrToStatusMenuId(
                                            fightOut, statusMenuId, 0);
                                        menuFightStatusStartAnimEXP(
                                            nextExp,
                                            expAfter - fightPokemonGetLevelToExp(
                                                           pokemon, oldLevel & 0xff));
                                        fightMainWaitFrame(0x40);
                                        menuFightStatusWaitAnimeEXP(nextExp, 1);
                                        fightMenuFightOutPokemonRenewStatusMenu(fightOut, statusMenuId, 1);
                                    }
                                }
                                lbl_8047B64C = 0;
                            }

                            if ((u8)openedMsg == 1) {
                                fightMenuCloseMsg();
                            }
                            fightOut = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
                            if (fightOut != 0) {
                                fn_8026532C(fightOut, statusMenuId, 0);
                            }
                        }
                    }
                }
            }
        }
    }

    lbl_8047B610 += 2;
}
#pragma optimize_for_size reset

#pragma opt_propagation off
void fn_802317E4(void) {
    extern u16 fn_801EF634();
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
    u8 flagValue;
    u8* flags;
    u8* msg;
    u8 i;

    if (fn_801EF634() != 0) {
        return;
    }

    weather = (u8)fightFloorGetNowTenkouDataId(0, 0);
    fightFloorCreateFightOutPokemonPtrAry(0);
    fightFloorSortFightOutPokemonPtrAry(0, 0);
    selected = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_80231FC8, &selected, 0);

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
                    fn_80211B94(lbl_8047B62C, lbl_80378B30, 0);
                }
            }
        }
    }

    fn_801DA7AC();
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
                    fn_80211B94(lbl_8047B62C, lbl_80378B30, 0);
                }
            }
        }
    }

    fn_801DA7AC();
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
                    fn_80211B94(lbl_8047B62C, lbl_80378B30, 0);
                }
            }
        }
    }

    fn_801DA7AC();
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
                    fn_80211B94(lbl_8047B62C, lbl_80378B5B, 0);
                }
            }
        }
    }

    fn_801DA7AC();
    fightFloorLoopValidFightOutPokemon(0, fn_80232024, 0, 1);
    fn_801DA7AC();

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
        fn_80211B94(lbl_8047B62C, lbl_80378A5F, 0);
    }

    fn_801DA7AC();
    if (fn_801EF634() != 0) {
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
        flagValue = 0;
        lbl_80379F58[0x160A4] = weatherCode;
        flags[5] = flagValue;
        fn_80211B94(lbl_8047B62C, msg, 0);
    }

    fn_801DA7AC();
    if (fn_801EF634() != 0) {
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
        fn_80211B94(lbl_8047B62C, msg, 0);
    }

    fn_801DA7AC();
    if (fn_801EF634() != 0) {
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
        flagValue = 1;
        lbl_80379F58[0x160A4] = weatherCode;
        flags[5] = flagValue;
        fn_80211B94(lbl_8047B62C, msg, 0);
    }

    fn_801DA7AC();
    if (fn_801EF634() != 0) {
        return;
    }
}
#pragma opt_propagation reset
#pragma optimize_for_size reset

/*
 * fn_80232110 (0x80232110)
 *
 * Core battle-damage calculation.  The caller supplies attacker, defender,
 * move id, optional base power, and optional move type.  A zero base power or
 * negative type selects the corresponding value from the move-data table.
 */
#pragma optimize_for_size on
s32 fn_80232110(u32 attacker, u32 defender, u32 attackSide, u32 move,
                 u32 basePower, s32 moveType) {
    extern u32 fightFloorGetNowTenkouDataId();
    extern u32 fightFloorGetStatus();
    extern s32 wazaGetStatus();
    extern u32 pokemonGetStatus();
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u32 figthOutPokemonGetSoubiItemBuff();
    extern u32 figthOutPokemonGetPokemonDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightFloorGetFightOutPokemonPtrToHeroPtr();
    extern u8 figthOutPokemonGetLevel();
    extern u32 fightFloorGetValidFightOutPokemonCount();
    extern u32 heroGetStatus();
    extern u32 fn_8010C4A0();
    extern u32 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern u8 fightOutPokemonIsJoutaiNormal();
    extern u32 fightFloorCheckFightOutPokemonPtrAryJoutaiDataId();
    extern u32 fightOutPokemonIsNokoriHpFollowing();
    extern u32 fightAbicntDoKakeWaru();
    extern u8 fightSideIsJoutaiDataId();
    extern u32 tenkouDataBiosGetSolarFlag();
    extern u8 fn_802026E4();
    extern u8 lbl_80279ED0[];
    u32 weather;
    u8 defenseBuff;
    u8 specialDefenseBuff;
    u8 moveField5;
    u8 floorStatus2;
    u32 power;
    u16 type;
    u16 attackerAbility;
    u32 attackerItem;
    s32 attackerItemBuff;
    u32 attackerPokemonId;
    u32 attackerPokemon;
    u16 attack;
    u16 defense;
    u16 specialAttack;
    u16 specialDefense;
    u32 attackerHero;
    u8 attackBuff;
    u8 specialAttackBuff;
    u8 level;
    u16 defenderAbility;
    u32 defenderItem;
    u32 defenderPokemonId;
    u32 defenderPokemon;
    u32 defenderHero;
    u32 activeCount;
    u32 floorStatus1;
    s32 offset;
    u8 category;
    s32 damage;
    u32 defenseValue;
    u32 wideSpecialAttack;
    u32 levelFactor;

    weather = fightFloorGetNowTenkouDataId(0, 1);
    floorStatus1 = (u8)fightFloorGetStatus(0, 0, 0x1d, 0);
    floorStatus2 = fightFloorGetStatus(0, 0, 0x2c, 0);

    if ((u16)basePower == 0) {
        power = wazaGetStatus(0, move, 7, 0) & 0xffff;
    } else {
        power = basePower;
    }
    if (moveType < 0) {
        type = wazaGetStatus(0, move, 3, 0);
    } else {
        type = moveType;
    }

    moveField5 = wazaGetStatus(0, move, 5, 0);
    basePower = (u8)wazaGetStatus(pokemonGetStatus(attacker, 0, 0xd9, 0), 0, 0x2b, 0);
    attackerAbility = fightOutPokemonGetTokuseiDataId(attacker);
    attackerItem = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
    attackerItemBuff = figthOutPokemonGetSoubiItemBuff(attacker);
    attackerPokemonId = figthOutPokemonGetPokemonDataId(attacker);
    attackerPokemon = fightOutPokemonGetPokemonPtr(attacker);
    attack = pokemonGetStatus(attackerPokemon, 0, 0x88, 0);
    specialAttack = pokemonGetStatus(attackerPokemon, 0, 0x8a, 0);
    attackerHero = fightFloorGetFightOutPokemonPtrToHeroPtr(0, attacker);
    attackBuff = pokemonGetStatus(attacker, 0, 0xe6, 0);
    specialAttackBuff = pokemonGetStatus(attacker, 0, 0xe8, 0);
    level = figthOutPokemonGetLevel(attacker);

    defenderAbility = fightOutPokemonGetTokuseiDataId(defender);
    defenderItem = fightOutPokemonGetSoubiItemSoubiDataId(defender);
    figthOutPokemonGetSoubiItemBuff(defender);
    defenderPokemonId = figthOutPokemonGetPokemonDataId(defender);
    defenderPokemon = fightOutPokemonGetPokemonPtr(defender);
    defense = pokemonGetStatus(defenderPokemon, 0, 0x89, 0);
    specialDefense = pokemonGetStatus(defenderPokemon, 0, 0x8b, 0);
    defenderHero = fightFloorGetFightOutPokemonPtrToHeroPtr(0, defender);
    defenseBuff = pokemonGetStatus(defender, 0, 0xe7, 0);
    specialDefenseBuff = pokemonGetStatus(defender, 0, 0xe9, 0);
    activeCount = fightFloorGetValidFightOutPokemonCount(0, 1, defender, 0);

    if (attackerAbility == 0x25 || attackerAbility == 0x4a) {
        attack = (attack & 0x7fff) << 1;
    }
    if (floorStatus1 == 1 && (u8)heroGetStatus(attackerHero, 0xf, 0) == 1) {
        attack = (attack * 0x6e) / 100;
    }
    if (floorStatus1 == 1 && (u8)heroGetStatus(defenderHero, 0x13, 0) == 1) {
        defense = (defense * 0x6e) / 100;
    }
    if (floorStatus1 == 1 && (u8)heroGetStatus(attackerHero, 0x15, 0) == 1) {
        specialAttack = (specialAttack * 0x6e) / 100;
    }
    if (floorStatus1 == 1 && (u8)heroGetStatus(defenderHero, 0x15, 0) == 1) {
        specialDefense = (specialDefense * 0x6e) / 100;
    }

    offset = 0;
    for (floorStatus1 = 0; floorStatus1 < 0x11; floorStatus1++, offset += 2) {
        if ((u16)attackerItem == lbl_80279ED0[offset] && type == lbl_80279ED0[offset + 1]) {
            category = fn_8010C4A0(type);
            switch (category) {
            case 1:
                attack = (attack * (attackerItemBuff + 100)) / 100;
                break;
            case 2:
                specialAttack = (specialAttack * (attackerItemBuff + 100)) / 100;
                break;
            }
        }
    }

    if ((u16)attackerItem == 0x1d) {
        attack = (attack * 0x96) / 100;
    }
    if (floorStatus2 == 1 && (u16)attackerItem == 0x22 &&
        ((u16)attackerPokemonId == 0x198 || (u16)attackerPokemonId == 0x197)) {
        specialAttack = (specialAttack * 0x96) / 100;
    }
    if (floorStatus2 == 1 && (u16)defenderItem == 0x22 &&
        ((u16)defenderPokemonId == 0x198 || (u16)defenderPokemonId == 0x197)) {
        specialDefense = (specialDefense * 0x96) / 100;
    }
    if ((u16)attackerItem == 0x23 && (u16)attackerPokemonId == 0x175) {
        specialAttack = (specialAttack & 0x7fff) << 1;
    }
    if ((u16)defenderItem == 0x24 && (u16)defenderPokemonId == 0x175) {
        specialDefense = (specialDefense & 0x7fff) << 1;
    }
    if ((u16)attackerItem == 0x2d && (u16)attackerPokemonId == 0x19) {
        specialAttack = (specialAttack & 0x7fff) << 1;
    }
    if ((u16)defenderItem == 0x40 && (u16)defenderPokemonId == 0x84) {
        defense = (defense & 0x7fff) << 1;
    }
    if ((u16)attackerItem == 0x41 &&
        ((u16)attackerPokemonId == 0x68 || (u16)attackerPokemonId == 0x69)) {
        attack = (attack & 0x7fff) << 1;
    }
    if (defenderAbility == 0x2f && (type == 10 || type == 0xf)) {
        wideSpecialAttack = specialAttack;
        wideSpecialAttack = (s32)wideSpecialAttack / 2;
        specialAttack = wideSpecialAttack;
    }
    if (attackerAbility == 0x37) {
        attack = (attack * 0x96) / 100;
    }
    if (attackerAbility == 0x39 &&
        (u16)fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(0, 0x3a, 0, 0) != 0) {
        specialAttack = (specialAttack * 0x96) / 100;
    }
    if (attackerAbility == 0x3a &&
        (u16)fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(0, 0x39, 0, 0) != 0) {
        specialAttack = (specialAttack * 0x96) / 100;
    }
    if (attackerAbility == 0x3e && fightOutPokemonIsJoutaiNormal(attacker) == 0) {
        attack = (attack * 0x96) / 100;
    }
    if (defenderAbility == 0x3f && fightOutPokemonIsJoutaiNormal(defender) == 0) {
        defense = (defense * 0x96) / 100;
    }
    if (type == 0xd && (u16)fightFloorCheckFightOutPokemonPtrAryJoutaiDataId(0, 0x38) != 0) {
        power = (s32)(power & 0xffff) / 2 & 0xffff;
    }
    if (type == 10 && (u16)fightFloorCheckFightOutPokemonPtrAryJoutaiDataId(0, 0x39) != 0) {
        power = (s32)(power & 0xffff) / 2 & 0xffff;
    }
    if (type == 0xc && attackerAbility == 0x41 &&
        (u8)fightOutPokemonIsNokoriHpFollowing(attacker, 3) != 0) {
        power = (s32)((power & 0xffff) * 0x96) / 100 & 0xffff;
    }
    if (type == 10 && attackerAbility == 0x42 &&
        (u8)fightOutPokemonIsNokoriHpFollowing(attacker, 3) != 0) {
        power = (s32)((power & 0xffff) * 0x96) / 100 & 0xffff;
    }
    if (type == 0xb && attackerAbility == 0x43 &&
        (u8)fightOutPokemonIsNokoriHpFollowing(attacker, 3) != 0) {
        power = (s32)((power & 0xffff) * 0x96) / 100 & 0xffff;
    }
    if (type == 6 && attackerAbility == 0x44 &&
        (u8)fightOutPokemonIsNokoriHpFollowing(attacker, 3) != 0) {
        power = (s32)((power & 0xffff) * 0x96) / 100 & 0xffff;
    }

    if ((u16)wazaGetStatus(0, move, 9, 0) == 7) {
        defense >>= 1;
    }

    category = fn_8010C4A0(type);
    switch (category) {
    case 1:
        if (basePower == 2) {
            if (attackBuff > 6) {
                damage = fightAbicntDoKakeWaru(attackBuff, attack);
            } else {
                damage = attack;
            }
        } else {
            damage = fightAbicntDoKakeWaru(attackBuff, attack);
        }

        levelFactor = (s32)(((u32)level & 0xff) << 1) / 5;
        damage *= (u16)power;
        damage *= levelFactor + 2;

        if (basePower == 2) {
            if (defenseBuff < 6) {
                defenseValue = fightAbicntDoKakeWaru(defenseBuff, defense);
            } else {
                defenseValue = defense;
            }
        } else {
            defenseValue = fightAbicntDoKakeWaru(defenseBuff, defense);
        }

        damage /= (s32)defenseValue;
        damage /= 50;
        if (fn_802026E4(attacker, 6) == 1 && attackerAbility != 0x3e) {
            damage /= 2;
        }
        if (fightSideIsJoutaiDataId(attackSide, 0x48) == 1 && basePower == 1) {
            if ((u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2 &&
                (u16)activeCount >= 2) {
                damage = (damage / 3) << 1;
            } else {
                damage /= 2;
            }
        }
        if ((u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2 && moveField5 == 4 &&
            (u16)activeCount >= 2) {
            damage /= 2;
        }
        if (damage == 0) {
            damage = 1;
        }
        break;

    case 2:
        if (basePower == 2) {
            if (specialAttackBuff > 6) {
                damage = fightAbicntDoKakeWaru(specialAttackBuff, specialAttack);
            } else {
                damage = specialAttack;
            }
        } else {
            damage = fightAbicntDoKakeWaru(specialAttackBuff, specialAttack);
        }

        levelFactor = (s32)(((u32)level & 0xff) << 1) / 5;
        damage *= (u16)power;
        damage *= levelFactor + 2;

        if (basePower == 2) {
            if (specialDefenseBuff < 6) {
                defenseValue = fightAbicntDoKakeWaru(specialDefenseBuff, specialDefense);
            } else {
                defenseValue = specialDefense;
            }
        } else {
            defenseValue = fightAbicntDoKakeWaru(specialDefenseBuff, specialDefense);
        }

        damage /= (s32)defenseValue;
        damage /= 50;
        if (fightSideIsJoutaiDataId(attackSide, 0x49) == 1 && basePower == 1) {
            if ((u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2 &&
                (u16)activeCount >= 2) {
                damage = (damage / 3) << 1;
            } else {
                damage /= 2;
            }
        }
        if ((u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2 && moveField5 == 4 &&
            (u16)activeCount >= 2) {
            damage /= 2;
        }
        if ((u8)weather == 2) {
            switch (type) {
            case 10:
                damage /= 2;
                break;
            case 11:
                damage = (damage * 0xf) / 10;
                break;
            }
        }
        if ((u8)tenkouDataBiosGetSolarFlag((u8)weather) == 0 && (u16)move == 0x4c) {
            damage /= 2;
        }
        if ((u8)weather == 1) {
            switch (type) {
            case 10:
                damage = (damage * 0xf) / 10;
                break;
            case 11:
                damage /= 2;
                break;
            }
        }
        if (fn_802026E4(attacker, 0x3a) == 1 && type == 10) {
            damage = (damage * 0xf) / 10;
        }
        break;

    default:
        damage = 0;
        break;
    }

    return damage + 2;
}
#pragma optimize_for_size reset

#pragma optimize_for_size on
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
#pragma optimize_for_size reset

extern u8  fightTrainerIsAllyFightTargetPtr(u32 ctx, u32 poke, u16 floorVal);
extern u16 fightOutPokemonGetZokuseiDataId(u32 poke, u8 idx);
extern u16 fightOutPokemonGetTokuseiDataId(u32 poke);
extern u32 fn_8010C650(u16 waza, u16* types, u16 typeCount);
extern int fn_802026E4();
#pragma optimize_for_size on
u32 fn_8023793C(u32 ctx, u32 poke, u16 waza, s32 val) {
    u16 types[2];
    u16 tmp;
    u16 tk;
    struct {
        u16 value;
    } typeCount;
    u16 z;
    struct {
        u8 value;
    } i;
    u16 j;
    u16 floorVal;
    u32 result;
    u8 tkMatch;
    u32 valid;

    if (waza == 9) {
        return 0x3f;
    }

    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    fightTrainerGetStatus(0, tmp, 2, 0);
    fightFloorGetStatus(0, 0, 0x14, 0);

    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
    if ((u8)fightTrainerGetStatus(0, tmp, 0x2b, 0) == 1) {
        tk = fightOutPokemonGetTokuseiDataId(poke);
    } else {
        tk = 0;
    }
    tkMatch = (tk == 0x1a);
    if (tkMatch == 1 && waza == 4) {
        return 0x43;
    }

    typeCount.value = 0;
    i.value = typeCount.value;
    for (; i.value < 2; i.value++) {
        floorVal = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
        tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
        tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
        if ((u8)fightTrainerGetStatus(0, tmp, 0x2a, 0) == 1) {
            if ((u8)fightTrainerIsAllyFightTargetPtr(ctx, poke, floorVal) == 0) {
                z = fightOutPokemonGetZokuseiDataId(poke, i.value);
            } else {
                z = fightOutPokemonGetZokuseiDataId(poke, i.value);
            }
        } else {
            z = 9;
        }
        if (z != 9) {
            types[typeCount.value] = z;
            typeCount.value++;
        }
    }
    if (typeCount.value == 0) {
        return 0x3f;
    }

    result = fn_8010C650(waza, types, typeCount.value);
    if (waza == 0 || waza == 1) {
        tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
        tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
        if ((u8)fightTrainerGetStatus(0, tmp, 0x24, 0) == 1) {
            valid = fn_802026E4(poke, 0x19);
        } else {
            valid = 0;
        }
        if ((u8)valid == 1) {
            for (j = 0; j < typeCount.value; j++) {
                u16 v = types[j];
                if (v != 9 && v == 7) {
                    result = 0x3f;
                }
            }
        }
    }

    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    fightTrainerGetStatus(0, tmp, 2, 0);
    fightFloorGetStatus(0, 0, 0x14, 0);
    tmp = (u16)fightTrainerGetStatus(ctx, 0, 0x43, 0);
    tmp = (u16)fightTrainerGetStatus(0, tmp, 2, 0);
    if ((u8)fightTrainerGetStatus(0, tmp, 0x2b, 0) == 1) {
        tk = fightOutPokemonGetTokuseiDataId(poke);
    } else {
        tk = 0;
    }
    tkMatch = (tk == 0x19);
    if (tkMatch == 1 && (u16)result != 0x41 && (s16)val > 0) {
        return 0x43;
    }
    return result;
}
#pragma optimize_for_size reset

/* Keep these calls in the non-prototype form used by the original focused
 * attempts.  MWCC's call lowering changes materially when earlier campaign
 * definitions leave full prototypes visible this late in the translation
 * unit. */
extern u8 fn_80235714();
extern u32 fn_802367CC();
extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
extern u32 fightOutPokemonGetPokemonPtr();
extern u32 fn_80239984();
extern u8 fn_80239EE8();
extern u8 fn_80239CCC();
extern s32 fightTrainerAiAddValue();

#pragma dont_inline on
#pragma scheduling on
u32 fightTrainerAiWazaValueMeisou(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235974(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x208);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x208);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan;
                    }
                }
            }
        }
    }
    found = 0;
after_scan:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x209);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x209);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x20a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x20a);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x20b, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x20b, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueRyuunomai(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235910(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x204);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x204);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_ryuunomai;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_ryuunomai:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x205);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x205);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x206);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x206);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x207, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x207, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueBirudoAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235AA0(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x200);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x200);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_birudoappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_birudoappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x201);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x201);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x202);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x202);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x203, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x203, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueKosumopawaa(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235974(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fc);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fc);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kosumopawaa;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kosumopawaa:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fd);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fd);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fe);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fe);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1ff, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ff, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueKaihirituAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_802357CC(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f8);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kaihirituappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kaihirituappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f9);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1fa);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fa);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1fb, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1fb, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueDowasure(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235974(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f4);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_dowasure;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_dowasure:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f5);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f6);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1f7, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f7, count2);
        return result;
    }
}

/* UNION_BATCH: fn_802232F4,fn_802317E4,fn_802342CC,fn_8023793C,fightTrainerAiWazaValueMeisou,fightTrainerAiWazaValueRyuunomai,fightTrainerAiWazaValueBirudoAppu,fightTrainerAiWazaValueKosumopawaa,fightTrainerAiWazaValueKaihirituAppu,fightTrainerAiWazaValueDowasure */


/* ===== Newer master-only definitions retained after campaign union ===== */

u32 fightTrainerAiWazaValueTokukouAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_802359D8(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f0);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f0);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_tokukouappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_tokukouappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f1);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f1);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1f2);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f2);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1f3, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1f3, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueKousokuidou(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235910(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ec);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ec);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kousokuidou;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kousokuidou:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ed);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ed);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ee);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ee);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1ef, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ef, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueBougyoAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235A3C(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e8);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e8);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_bougyoappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_bougyoappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e9);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e9);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1ea);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1ea);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1eb, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1eb, count2);
        return result;
    }
}

u32 fightTrainerAiWazaValueKougekiAppu(u32 ctx, u32 poke, u32 msgArg) {
    u8 v = fn_80235AA0(ctx, poke);
    u32 acc = 0;
    u16 scanBuf[16];
    u32 stackArr[8];
    u32 count;
    u16 i;
    u8 found;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e4);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e4);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    for (i = 0; i < (u16)count; i++) {
        if (stackArr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackArr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_kougekiappu;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_kougekiappu:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e5);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e5);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x1e6);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e6);
    }

    {
        u32 result = 0;
        s32 count2 = (u8)v - 6;
        u32 scale;

        if (count2 < 0) {
            count2 = 0;
        }
        scale = fightTrainerGetStatus(0, 0x1e7, 0x3e, 0);
        count2 *= scale;
        result += fightTrainerAiAddValue(acc, count2);
        fn_80239CCC(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x1e7, count2);
        return result;
    }
}
#pragma scheduling reset

u32 fightTrainerAiWazaValueHaradaiko(u32 ctx, u32 poke, u32 msgArg) {
    u32 acc = 0;
    u16 scanBuf[10];
    u32 stackArr[8];
    u16 count16;
    u32* stackPtr;
    u16 i;
    u32 count;
    u8 found;
    u32 result;

    if ((u8)fn_80235714(ctx, poke) == 0) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x210);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x210);
    }

    count = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, ctx, stackArr, 1, 1);
    stackPtr = stackArr;
    count16 = (u16)count;
    for (i = 0; i < count16; i++) {
        if (stackPtr[i] != (u32)poke) {
            u16 n = (u16)fn_802367CC(ctx, stackPtr[i], scanBuf, 0, 1);
            if (n != 0) {
                u16 j;
                for (j = 0; j < n; j++) {
                    if (scanBuf[j] == 0x10a) {
                        found = 1;
                        goto after_scan_haradaiko;
                    }
                }
            }
        }
    }
    found = 0;
after_scan_haradaiko:

    if (found == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x211);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x211);
    }

    if ((u8)fn_80235714(ctx, poke) == 1) {
        u32 oldAcc = acc;
        acc = 0;
        acc += fn_80239984(oldAcc, ctx, 0x212);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x212);
    }

    result = acc;
    if ((u8)fn_802373B0(ctx, poke, -1, lbl_8047E630) == 1) {
        result = fn_80239984(acc, ctx, 0x213);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0, msgArg, 0, 0x213);
    }

    return result;
}

#pragma optimize_for_size on
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValuetorikku(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_8023831C(void*);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_80205B8C();
    u32 handle = 0;
    u16 v = fn_8023831C(ctx);

    if (v == 0x1d || v == 0x18) {
        handle = fn_80239984(0, ctx, 0x1e2);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e2);
    }
    if (fn_80237F74(ctx, param3, 0x3c) == 1) {
        handle = fn_80239984(handle, ctx, 0x1e3);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1e3);
    }
    return handle;
}
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma dont_inline reset

void WS_ALERTEND(void) { lbl_8047B614 = 2; }

void WS_SPEABIEND(void) { lbl_8047B614 = 2; }

void WS_SEQEND(void) { lbl_8047B614 = 1; }

void WS_WAZAEND(void) { lbl_8047B614 = 1; }

/* UNION_BATCH: fightTrainerAiWazaValueTokukouAppu,fightTrainerAiWazaValueKousokuidou,fightTrainerAiWazaValueBougyoAppu,fightTrainerAiWazaValueKougekiAppu,fightTrainerAiWazaValueHaradaiko,fightTrainerAiWazaValuetorikku,WS_ALERTEND,WS_SPEABIEND,WS_SEQEND,WS_WAZAEND */


/* ===== Newer master-only definitions retained after campaign union ===== */

void WS_SEQRET(void) { lbl_8047B614 = 2; }

void WS_GETEND(void) { lbl_8047B614 = 1; }

void WS_ITEMEND(void) { lbl_8047B614 = 1; }

#pragma opt_propagation off
void fn_80222554(void)
{
    u8* t = lbl_8047B610;
    u32 mask = 0xffffffff;
    u32 val = *(u32 *)(t + 5);
    u32 old;
    u32 *ptr = *(u32 **)(t + 1);
    old = *ptr;
    mask = val ^ mask;
    mask = old & mask;
    *ptr = mask;
    lbl_8047B610 += 9;
}
#pragma opt_propagation reset

void WS_TSUIKA_INDIRECT_ACT(void) { fn_802249B8(0, 0); }

void WS_TSUIKA_DIRECT_ACT(void) { fn_802249B8(1, 0); }

void WS_DAMAGE_LOSS_ONLY(void) { fn_802271E0(0, 1); lbl_8047B610 = lbl_8047B610 + 1; }

void WS_DAMAGE_LOSS(void) { fn_802271E0(1, 1); lbl_8047B610 = lbl_8047B610 + 1; }

void WS_KORAERU_CHECK(void) { fn_802271E0(1, 0); lbl_8047B610 = lbl_8047B610 + 1; }

void WS_TYPE_CHECK(void) { fn_802274F0(1, 1, 1, 0); }

/* UNION_BATCH: WS_SEQRET,WS_GETEND,WS_ITEMEND,fn_80222554,WS_TSUIKA_INDIRECT_ACT,WS_TSUIKA_DIRECT_ACT,WS_DAMAGE_LOSS_ONLY,WS_DAMAGE_LOSS,WS_KORAERU_CHECK,WS_TYPE_CHECK */

/* Initial C coverage for the residual report-absent functions.  These
 * deliberately use ordinary C bodies so every target has an iterable
 * objdiff baseline; signatures and behavior can be refined independently. */
#pragma dont_inline on
#define fn_801F0F04 fightActionFlowFifo
#define fn_801F1170 fightActionCheckValid
#define fn_801F11CC fightActionCreate
#define fn_80205224 fightOutPokemonGetMotoWazaDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_8020D888 fightActionBiosSetBuffDataPtr
#define fn_8020D920 fightActionBiosGetFightActionDataPtr
u32 fn_80211A78(void* ctx)
{
    extern u8 lbl_80375D30[];
    extern u8 lbl_803791FE[];
    extern void* pokemonGetStatus();
    extern void fn_801F0F04();
    extern u8 fn_801F1170();
    extern u8 fn_801F11CC();
    extern u8 fn_802026E4();
    extern u16 fn_80205224();
    extern u8 fn_802062FC();
    extern void fn_8020D888();
    extern u32 fn_8020D920();
    u8 localBuf[0x30];
    void* feData;
    u32 d920val;
    u8 result;

    if (fn_802062FC(ctx) == 0) {
        return 1;
    }
    feData = pokemonGetStatus(ctx, 0, 0xFE, 0);
    if (feData == NULL) {
        return 1;
    }
    if (fn_801F1170(feData) == 0) {
        return 1;
    }
    if (fn_80205224(ctx) != 0x108) {
        goto done;
    }
    if (fn_802026E4(ctx, 8) != 0) {
        goto done;
    }
    if ((u8)(u32)pokemonGetStatus(ctx, 0, 0xF9, 0) != 0) {
        goto done;
    }
    d920val = fn_8020D920(lbl_8047B62C);
    result = fn_801F11CC(localBuf, d920val, ctx, 0xC, 0, lbl_80375D30);
    if ((u32)result == 1) {
        goto set_buff;
    } else {
        __memcpy(localBuf, localBuf, 0);
        goto after_set_buff;
    }
set_buff:
    {
        fn_8020D888(localBuf, lbl_803791FE);
        result = 1;
    }
after_set_buff:
    if (result == 1) {
        fn_801F0F04(localBuf);
    }
done:
    return 1;
}
#undef fn_8020D920
#undef fn_8020D888
#undef fn_802062FC
#undef fn_80205224
#undef fn_801F11CC
#undef fn_801F1170
#undef fn_801F0F04
void fn_80211B94(void* ctx, void* script, u8 preserveState)
{
    extern void (*lbl_8027A00C[])();
    extern u8 lbl_80378798[];
    extern u8 lbl_80379F58[];
    extern void fn_8023011C();
    extern u32 fn_8022E34C();
    extern u32 fn_8022E1F8();
    extern void fn_80230088();
    extern void fn_8022EB9C();
    extern void fn_802136A4();
    extern void wazaSetStatus();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    u8* innerPc;
    u8 innerSeqState;
    u32 innerCtx;
    void* savedCtx;
    u8 savedSeqState;
    u8* savedPc;
    u32 fieldD9;
    u32 slot;
    u8 local37;
    u8 local38;

    savedPc = lbl_8047B610;
    savedCtx = lbl_8047B62C;
    lbl_8047B610 = script;
    savedSeqState = lbl_8047B614;
    if (preserveState == 0) {
        lbl_8047B614 = 0;
    }
    lbl_8047B62C = ctx;
run_script:
        lbl_8027A00C[*lbl_8047B610]();
        if (preserveState == 0) {
            goto check_unpreserved;
        }
        if (lbl_8047B614 == 1) {
            fightFloorLoopValidFightOutPokemon(0, fn_8023011C, 0, 0);
            innerCtx = (u32)lbl_8047B62C;
            innerSeqState = lbl_8047B614;
            innerPc = lbl_8047B610;
            lbl_8047B610 = lbl_80378798;
            lbl_8047B614 = 0;
            lbl_8047B62C = (void*)innerCtx;
            do {
                lbl_8027A00C[*lbl_8047B610]();
                if (lbl_8047B614 == 1) {
                    break;
                }
            } while (lbl_8047B614 != 2);
            lbl_8047B62C = (void*)innerCtx;
            lbl_8047B614 = innerSeqState;
            lbl_8047B610 = innerPc;
            local37 = 1;
            fightFloorLoopValidFightOutPokemon(0, fn_8022E34C, &local37, 0);
            fightFloorLoopValidFightOutPokemon(0, fn_8022E1F8, 0, 0);
            fightFloorLoopValidFightOutPokemon(0, fn_80230088, 0, 0);
            local38 = 0;
            fightFloorLoopValidFightOutPokemon(0, fn_8022EB9C, &local38, 0);
            lbl_8047B614 = 2;
        }
        if (lbl_8047B614 != 2) {
            goto run_script;
        }

    slot = fightTargetGetPtrAsNowFightType(0x11, 0);
    fieldD9 = 0;
    fieldD9 += pokemonGetStatus((void*)slot, 0, 0xD9, 0);
    fightFloorLoopValidFightOutPokemon(0, fn_802136A4, 0, 0);
    lbl_80478D78[3] = 0;
    lbl_8047B618 &= 0xF1E892AF;
    lbl_80478D78[4] = 0;
    lbl_8047B625 = 0;
    lbl_80379F58[0x16002] = 0;
    lbl_80379F58[0x160A1] = 0;
    pokemonSetStatus((void*)slot, 0, 0xF3, 0, 0);
    pokemonSetStatus((void*)slot, 0, 0xF4, 0, 9);
    wazaSetStatus(fieldD9, 0, 0x2D, 0, 0);
    goto finish;

check_unpreserved:
    if (lbl_8047B614 == 1) {
        goto finish;
    }
    if (lbl_8047B614 != 2) {
        goto run_script;
    }
finish:
    if (preserveState != 0) {
        lbl_8047B618 &= 0xFFFFFDFF;
        lbl_8047B618 &= 0xFFF7FFFF;
    }
    lbl_8047B62C = savedCtx;
    lbl_8047B614 = savedSeqState;
    lbl_8047B610 = savedPc;
}
#pragma opt_propagation off
void fn_80211E18(u32 r3,u32 r4)

{
    extern void* lbl_80375DF0[];
    extern void* lbl_80375E24[];
    extern void* lbl_80375E44[];
    extern u32 lbl_80279E7C[];
    extern u8 lbl_80379F58[];
    extern u32 GSmsgGetGSchar();
    extern void heroItemDecItemDataId(u32, u32, u32, s16);
    extern void msgctrlSetValue();
    extern u8 itemParamGetHPUp();
    extern u8 itemParamGetConfuseFlag();
    extern u8 itemParamGetParalyzeFlag();
    extern u8 itemParamGetFreezeFlag();
    extern u8 itemParamGetBurnFlag();
    extern u8 itemParamGetPoisonFlag();
    extern u8 itemParamGetSleepFlag();
    extern u8 itemParamGetGuardFlag();
    extern u8 itemParamGetSpAttackUp();
    extern u8 itemParamGetHitUp();
    extern u8 itemParamGetQuickUp();
    extern u8 itemParamGetDefenceUp();
    extern u8 itemParamGetAttackUp();
    extern u8 itemParamGetCriticalFlag();
    extern u32 itemParamGetPtr();
    extern void itemDataBiosGetItemEffectParam();
    extern void itemDataBiosGetPtr();
    extern void fn_801DA7AC();
    extern void fn_801EF8F4();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 fn_801F8000();
    extern u32 fightTrainerGetNamePtr();
    extern u32 fightTrainerGetStatus();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fn_80211B94();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern void fn_80265598();
    extern u8 lbl_8047B614;
  u8 bVar1;
  u8 cVar2;
  u16 uVar9;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  int iVar7;
  u32 sVar10;
  u8 cVar12;
  u32 uVar8;
  u8 cVar13;
  s16 uVar11;

  u32 uVar14;
  u8 uVar15;
  u32 itemCheck;
  register u32 itemId;

  itemId = r4;
  uVar9 = fightFloorGetStatus(0,0,0x14,0);
  uVar3 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar5 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar3);
  uVar6 = fightTrainerGetStatus(uVar5,0,0x44,0);
  iVar7 = (int)pokemonGetStatus(uVar3,0,0xe5,0);
  if (iVar7 != 0) {
    itemGetStatus(iVar7,0,0x1f,0);
    sVar10 = itemGetStatus(iVar7,0,0x20,0);
    cVar12 = itemGetStatus(iVar7,0,0x21,0);
    itemDataBiosGetPtr(itemId);
    itemDataBiosGetItemEffectParam();
    uVar8 = itemParamGetPtr();
    bVar1 = lbl_80379F58[0x1601E];
    cVar2 = lbl_80379F58[0x160A4];
    cVar13 = fn_802026E4(uVar3,0x2e);
    if (cVar13 == 1) {
      fightOutPokemonWriteJoutaiDataId(uVar3,0x2e);
    }
    cVar13 = fn_802026E4(uVar3,0x15);
    if (cVar13 == 1) {
      fightOutPokemonWriteJoutaiDataId(uVar3,0x15);
    }
    cVar13 = fn_802026E4(uVar3,0x28);
    if (cVar13 == 1) {
      fightOutPokemonWriteJoutaiDataId(uVar3,0x28);
    }
    if (cVar12 == 1) {
      uVar14 = (u32)lbl_80375E24[0];
    }
    else {
      cVar13 = itemGetStatus(0,itemId,2,0);
      if (cVar13 == 1) {
        uVar14 = (u32)lbl_80375DF0[itemId & 0xffff];
      }
      else {
        if (((itemId & 0xffff) == 0x50) || ((itemId & 0xffff) == 0x51)) {
          uVar14 = (u32)lbl_80375E44[0];
        }
        else {
          itemDataBiosGetPtr(itemId);
          itemDataBiosGetItemEffectParam();
          iVar7 = itemParamGetPtr();
          if (iVar7 == 0) {
            uVar15 = 7;
            goto item_type_done;
          }
          itemCheck = itemId;
          if ((itemCheck & 0xffff) == 0x13) {
            uVar15 = 1;
            goto item_type_done;
          }
          if (itemParamGetHPUp() != 0) {
            uVar15 = 2;
            goto item_type_done;
          }
          if (itemParamGetSleepFlag(iVar7) == 1 ||
              itemParamGetPoisonFlag(iVar7) == 1 ||
              itemParamGetBurnFlag(iVar7) == 1 ||
              itemParamGetFreezeFlag(iVar7) == 1 ||
              itemParamGetParalyzeFlag(iVar7) == 1 ||
              itemParamGetConfuseFlag(iVar7) == 1) {
            uVar15 = 3;
            goto item_type_done;
          }
          if (itemParamGetCriticalFlag(iVar7) == 1) {
            uVar15 = 4;
            goto item_type_done;
          }
          if (itemParamGetAttackUp(iVar7) != 0 ||
              itemParamGetDefenceUp(iVar7) != 0 ||
              itemParamGetQuickUp(iVar7) != 0 ||
              itemParamGetHitUp(iVar7) != 0 ||
              itemParamGetSpAttackUp(iVar7) != 0) {
            uVar15 = 5;
            goto item_type_done;
          }
          if (itemParamGetGuardFlag(iVar7) == 1) {
            uVar15 = 6;
          }
          else {
            uVar15 = 7;
          }
item_type_done:
          if ((u8)uVar15 == 7) {
            uVar14 = (u32)lbl_80375E24[0];
          }
          else {
            fightFloorSetStatus(0,0,0x4b,0,uVar3);
            lbl_80478D78[5] = 0;
            if ((s32)(uVar15 & 0xff) == 4) {
              goto item_type_4;
            }
            if ((s32)(uVar15 & 0xff) >= 4) {
              goto item_type_high;
            }
            if ((s32)(uVar15 & 0xff) >= 3) {
              goto item_type_3;
            }
            goto item_status_done;
item_type_high:
            if ((s32)(uVar15 & 0xff) == 6) {
              goto item_status_done;
            }
            if ((s32)(uVar15 & 0xff) >= 6) {
              goto item_status_done;
            }
            goto item_type_5;
item_type_3:
                if (itemParamGetSleepFlag(uVar8) != 1 ||
                    itemParamGetPoisonFlag(uVar8) != 1 ||
                    itemParamGetBurnFlag(uVar8) != 1 ||
                    itemParamGetFreezeFlag(uVar8) != 1 ||
                    itemParamGetParalyzeFlag(uVar8) != 1) {
                  goto map_item_ailment;
                }
                if (fn_802026E4(uVar3,8) == 1) {
                  lbl_80478D78[5] = 5;
                }
                else if (fn_802026E4(uVar3,3) == 1) {
                  lbl_80478D78[5] = 4;
                }
                else if (fn_802026E4(uVar3,4) == 1) {
                  lbl_80478D78[5] = 4;
                }
                else if (fn_802026E4(uVar3,6) == 1) {
                  lbl_80478D78[5] = 3;
                }
                else if (fn_802026E4(uVar3,7) == 1) {
                  lbl_80478D78[5] = 2;
                }
                else if (fn_802026E4(uVar3,5) == 1) {
                  lbl_80478D78[5] = 1;
                }
                goto item_status_done;
map_item_ailment:
                if (itemParamGetSleepFlag(uVar8) == 1) {
                  lbl_80478D78[5] = 5;
                }
                else if (itemParamGetPoisonFlag(uVar8) == 1) {
                  lbl_80478D78[5] = 4;
                }
                else if (itemParamGetBurnFlag(uVar8) == 1) {
                  lbl_80478D78[5] = 3;
                }
                else if (itemParamGetFreezeFlag(uVar8) == 1) {
                  lbl_80478D78[5] = 2;
                }
                else if (itemParamGetParalyzeFlag(uVar8) == 1) {
                  lbl_80478D78[5] = 1;
                }
                else if (itemParamGetConfuseFlag(uVar8) == 1) {
                  lbl_80478D78[5] = 0;
                }
            goto item_status_done;
item_type_4:
            lbl_80478D78[5] = 5;
            goto item_status_done;
item_type_5:
              lbl_80478D78[5] = 4;
              uVar11 = itemParamGetAttackUp(uVar8);
              if (uVar11 > 0) {
                uVar3 = GSmsgGetGSchar(lbl_80279E7C[1]);
                msgctrlSetValue(0xd,uVar3);
                lbl_80379F58[0x1601E] = 1;
                goto item_stat_result;
              }
              uVar11 = itemParamGetDefenceUp(uVar8);
              if (uVar11 > 0) {
                uVar3 = GSmsgGetGSchar(lbl_80279E7C[2]);
                msgctrlSetValue(0xd,uVar3);
                lbl_80379F58[0x1601E] = 2;
                goto item_stat_result;
              }
              uVar11 = itemParamGetQuickUp(uVar8);
              if (uVar11 > 0) {
                uVar3 = GSmsgGetGSchar(lbl_80279E7C[3]);
                msgctrlSetValue(0xd,uVar3);
                lbl_80379F58[0x1601E] = 3;
                goto item_stat_result;
              }
              uVar11 = itemParamGetHitUp(uVar8);
              if (uVar11 > 0) {
                uVar3 = GSmsgGetGSchar(lbl_80279E7C[6]);
                msgctrlSetValue(0xd,uVar3);
                lbl_80379F58[0x1601E] = 6;
                goto item_stat_result;
              }
              uVar11 = itemParamGetSpAttackUp(uVar8);
              if (uVar11 > 0) {
                uVar3 = GSmsgGetGSchar(lbl_80279E7C[4]);
                msgctrlSetValue(0xd,uVar3);
                lbl_80379F58[0x1601E] = 4;
              }
item_stat_result:
              if (uVar11 < 0) {
                if ((uVar11 == 1) || (uVar11 == -1)) {
                  uVar3 = GSmsgGetGSchar(0x76bd);
                  msgctrlSetValue(0xe,uVar3);
                  lbl_80379F58[0x1601E] = (lbl_80379F58[0x1601E] & 0xf) + 0x15;
                }
                else {
                  uVar3 = GSmsgGetGSchar(0x7628);
                  msgctrlSetValue(0xe,uVar3);
                  lbl_80379F58[0x160A4] = (lbl_80379F58[0x1601E] & 0xf) + 0x2d;
                }
                uVar3 = GSmsgGetGSchar(0x7629);
                msgctrlSetValue(0x41,uVar3);
              }
              else {
                if ((uVar11 == 1) || (uVar11 == -1)) {
                  uVar3 = GSmsgGetGSchar(0x76bd);
                  msgctrlSetValue(0xe,uVar3);
                  lbl_80379F58[0x160A4] = (lbl_80379F58[0x1601E] & 0xf) + 0xe;
                }
                else {
                  uVar3 = GSmsgGetGSchar(0x7626);
                  msgctrlSetValue(0xe,uVar3);
                  lbl_80379F58[0x160A4] = (lbl_80379F58[0x1601E] & 0xf) + 0x26;
                }
                uVar3 = GSmsgGetGSchar(0x7627);
                msgctrlSetValue(0x41,uVar3);
              }
item_status_done:
            uVar14 = (u32)lbl_80375E24[uVar15];
          }
        }
      }
    }
    lbl_8047B614 = 0;
    uVar3 = fn_801F8000(uVar5);
    msgctrlSetValue(0x22,uVar3);
    uVar3 = fightTrainerGetNamePtr(uVar5);
    msgctrlSetValue(0x23,uVar3);
    uVar3 = fightTrainerGetNamePtr(uVar5);
    msgctrlSetValue(0x13,uVar3);
    itemGetStatus(0,itemId & 0xffff,1,0);
    uVar3 = GSmsgGetGSchar();
    msgctrlSetValue(0x29,uVar3);
    fn_801EF8F4(1);
    if (cVar12 == 0) {
      fn_80265598(uVar4,uVar9,1);
    }
    fn_80211B94(r3,uVar14,1);
    if (cVar12 == 0) {
      heroItemDecItemDataId(uVar6,itemId,1,(s16)sVar10);
    }
    lbl_80379F58[0x1601E] = bVar1;
    lbl_80379F58[0x160A4] = cVar2;
    fn_801DA7AC();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
  }
  return;
}
#pragma opt_propagation reset
void fn_802128D0(u32 r3, u32 r4)

{
    extern u32 GSmsgGetGSchar();
    extern u32 wazaGetStatus();
    extern void msgctrlSetValue();
    extern void fn_801DA7AC();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightActionCheckValid();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 fn_80201890();
    extern u32 fn_80201C58();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightOutPokemonCreateFightActionAttackWaza(u32, u32, u32, u32, void*, u16, u32, s8, u32);
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fn_80211B94();
    extern int fn_8022B2CC();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern u32 pokemonGetStatus();
    extern u8 fightFloorGetNowTenkouDataId();
    extern u8 lbl_80478D78[1];
    extern u8 lbl_8047B614;
    extern u32 lbl_8047B618;
    extern u32 lbl_8047B644;
    extern u32 lbl_80379BFF[];
    extern u8 lbl_80375CA8[];
  u8 bVar1;
  u16 uVar10;
  u32 uVar2;
  u32 iVar3;
  u32 iVar4;
  u32 iVar5;
  u16 sVar11;
  u8 cVar14;
  u32 uVar6;
  u32 uVar12;
  struct {
    u16 value;
  } uVar7;
  u16 sVar13;
  u32 iVar9;

  uVar10 = fightFloorGetStatus(0,0,0x14,0);
  sVar11 = wazaGetStatus(0,r4,9,0);
  uVar2 = fightTargetGetPtrAsNowFightType(0x11,0);
  iVar3 = fightTargetGetPtrAsNowFightType(2,uVar2);
  iVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
  iVar5 = fightTargetGetPtrAsNowFightType(2,iVar4);
  cVar14 = fightOutPokemonCheckFightOut(uVar2);
  if (cVar14 == 0) {
    return;
  }
  pokemonGetStatus(uVar2,0,0xfe,0);
  cVar14 = fightActionCheckValid();
  if (cVar14 == 0) {
    return;
  }
  fightFloorSetStatus(0,0,0x51,0,0);
  fightFloorSetStatus(0,0,0x52,0,0);
  fightFloorSetStatus(0,0,0x53,0,0);
  lbl_80478D78[6] = 0;
  lbl_8047B644 = 0;
  cVar14 = fn_802026E4(uVar2,0x2a);
  if ((cVar14 == 1) && ((r4 & 0xffff) != 0xa5)) {
    uVar6 = fn_80201C58(uVar2,0x2a);
    uVar12 = fn_80201890(uVar2,0x2a);
    uVar7.value = (u16)pokemonGetStatus(fightOutPokemonGetPokemonPtr(uVar2),0,0x7f,(u16)uVar12);
    if ((uVar6 & 0xffff) != (uVar7.value & 0xffff)) {
      fightOutPokemonWriteJoutaiDataId(uVar2,0x2a);
    }
    uVar6 = fn_8022B2CC(uVar2,uVar7.value,uVar10,0,1,1, (void*)0xffffffff);
    iVar9 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(uVar6,uVar10);
    fightOutPokemonCreateFightActionAttackWaza(uVar2,0,0x13,0,lbl_80375CA8,uVar7.value,iVar9,uVar12,1);
    r4 = uVar7.value;
    fightFloorSetStatus(0,0,0x42,0,uVar6);
    iVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
    iVar5 = fightTargetGetPtrAsNowFightType(2,iVar4);
  }
  if (((r4 & 0xffff) == 0xa5) || ((r4 & 0xffff) == 0x164)) {
    lbl_8047B618 = lbl_8047B618 | 0x800;
  }
  sVar13 = wazaGetStatus(0,r4,9,0);
  if ((((sVar13 == 0x91) || (sVar13 == 0x27)) || (sVar13 == 0x4b)) ||
     (((sVar13 == 0x97 || (sVar13 == 0x9b)) || (sVar13 == 0x1a)))) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if ((bVar1 == 1) && (cVar14 = fn_802026E4(uVar2,0x22), cVar14 == 0)) {
    if (((sVar11 == 0x97) && (cVar14 = fightFloorGetNowTenkouDataId(0,1), cVar14 == 1)) &&
       (iVar9 = fn_8022B2CC(uVar2,r4,uVar10,0,0,1, (void*)0xffffffff), iVar9 != 0)) {
      iVar4 = iVar9;
      fightFloorSetStatus(0,0,0x43,0,iVar9);
      iVar5 = fightTargetGetPtrAsNowFightType(2,iVar9);
    }
  }
  else {
    iVar9 = fn_8022B2CC(uVar2,r4,uVar10,0,0,1, (void*)0xffffffff);
    if (iVar9 != 0) {
      iVar4 = iVar9;
      fightFloorSetStatus(0,0,0x43,0,iVar9);
      iVar5 = fightTargetGetPtrAsNowFightType(2,iVar9);
    }
  }
  cVar14 = fightOutPokemonCheckFightOut(iVar4);
  if (cVar14 != 0) goto LAB_0020fce0;
  if (iVar3 == iVar5) {
    iVar4 = fightTargetGetPtrAsNowFightType(0xf,uVar2);
    cVar14 = fightOutPokemonCheckFightOut();
    if (cVar14 == 0) goto LAB_0020fc94;
  }
  else {
LAB_0020fc94:
    iVar4 = fightTargetGetPtrAsNowFightType(0xe,iVar4);
    if (iVar4 == 0) {
      return;
    }
    cVar14 = fightOutPokemonCheckFightOut();
    if (cVar14 != 0) {
      goto set_target;
    }
    goto fn_802128D0_done;
  }
set_target:
  fightFloorSetStatus(0,0,0x43,0,iVar4);
  fightTargetGetPtrAsNowFightType(2,iVar4);
LAB_0020fce0:
  uVar6 = (u16)wazaGetStatus(0,r4,9,0);
  lbl_8047B614 = 0;
  wazaGetStatus(0,r4,1,0);
  uVar2 = GSmsgGetGSchar();
  msgctrlSetValue(0x28,uVar2);
  fn_80211B94(r3,lbl_80379BFF[uVar6 & 0xffff],1);
  fn_801DA7AC();
  fightMenuAllFightTrainerCloseStatusMenu(0);
  fightMenuAllFightOutPokemonCloseStatusMenu(0);
  fightMenuCloseMsg();
fn_802128D0_done:
  return;
}
#pragma optimize_for_size on
void fn_80212D6C(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011F634();
    extern void pokemonAddDpFormPokemonDpFilterId();
    extern u8 pokemonIsDarkPokemon();
    extern void msgctrlSetValue();
    extern void fn_801DA7AC();
    extern void fightMainWaitFrame();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetNamePtr();
    extern void fn_801FB974();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_802080A8();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern void fightMenuOpenMsg();
    extern void fn_8026532C();
    extern void fn_80265598();
  u32 uVar1;
  u32 uVar2;
  u8 cVar7;
  int uVar3;
  int uVar4;
  u32 uVar5;
  u32 uVar10;
  int iVar9;
  int iVar8;
  u32 uVar6;

  iVar9 = 0;
  iVar8 = 0;
  uVar6 = fightFloorGetStatus(0,0,0x14,0) & 0xffff;
  uVar1 = fightFloorGetStatus(0,0,0x36,0);
  uVar2 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar1);
  cVar7 = fn_802026E4(uVar1,0x2e);
  if (cVar7 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x2e);
  }
  cVar7 = fn_802026E4(uVar1,0x15);
  if (cVar7 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x15);
  }
  cVar7 = fn_802026E4(uVar1,0x28);
  if (cVar7 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x28);
  }
  fightOutPokemonGetPokemonPtr(uVar1);
  cVar7 = pokemonIsDarkPokemon();
  if (cVar7 == 1) {
    fightOutPokemonGetPokemonPtr(uVar1);
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
  msgctrlSetValue(0x13,fightTrainerGetNamePtr(uVar2));
  fn_801EF8F4(1);
  fightMenuOpenMsg(0x7704);
  fightMainWaitFrame(0x40);
  fightMenuCloseMsg();
  fn_801FB974(uVar2,1);
  fn_80265598(uVar1,uVar6,1);
  fightMenuOpenMsg(0x7705);
  fn_801FB974(uVar2,2);
  fightMenuCloseMsg();
  if ((u8)iVar9 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,8);
    fightOutPokemonWriteJoutaiDataId(uVar1,0x17);
    cVar7 = fightOutPokemonIsUseHensinBuff(uVar1);
    if (cVar7 == 1) {
      fightOutPokemonSetHensinPokemonStatusId(uVar1,0x7c,0,0);
    }
  }
  if ((u8)iVar8 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x3e);
    cVar7 = fightOutPokemonIsUseHensinBuff(uVar1);
    if (cVar7 == 1) {
      fightOutPokemonSetHensinPokemonStatusId(uVar1,200,0,0);
    }
    cVar7 = fightOutPokemonIsGcHeroFightOutPokemon(uVar1);
    if (cVar7 == 1) {
      uVar5 = fightOutPokemonGetPokemonPtr(uVar1);
      pokemonAddDpFormPokemonDpFilterId(uVar5,0,2);
      cVar7 = fightOutPokemonIsUseHensinBuff(uVar1);
      if (cVar7 == 1) {
        fightOutPokemonSetHensinPokemonStatusId(uVar1,0xc5,0,0);
      }
    }
  }
  fn_802080A8(uVar1,iVar9,iVar8,uVar10,1);
  fn_802080A8(uVar1,iVar9,iVar8,0,2);
  fightMenuCloseMsg();
  fn_8026532C(uVar1,uVar6,0);
  fn_801FB974(uVar2,3);
  fn_802080A8(uVar1,iVar9,iVar8,0,3);
  fn_801DA7AC();
  fightMenuAllFightTrainerCloseStatusMenu(0);
  fightMenuAllFightOutPokemonCloseStatusMenu(0);
  fightMenuCloseMsg();
  return;
}
#pragma optimize_for_size reset
#pragma opt_propagation off
void fn_80213270(void)

{
    extern u32 fn_800E0C54();
    extern void fn_801DA7AC();
    extern u16 fn_801EF634();
    extern void fightFloorInitFightOutPokemonPtrAryFightWazaJoutai();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetValidFightSidePtr();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern void fightSideInitJoutaiDataId();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern u32 fn_80213558();
    extern u32 fn_802301A8();
    extern u32 fn_80230318();
    extern void fn_8022FE80();
    extern void fn_802317E4();
    extern u8 lbl_80379F58[];
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B618;
  register u32 side;
  u16 sVar2;
  u16 uVar4;

  u32 uVar6;
  u32 finalFlags;
  u32 bVar5;
  u8 local_17;
  u8 local_18;

  fightMenuAllFightTrainerCloseStatusMenu(0);
  fightMenuAllFightOutPokemonCloseStatusMenu(0);
  fightMenuCloseMsg();
  for (bVar5 = 0; (bVar5 & 0xff) < 8; bVar5 = bVar5 + 1) {
    *(u8 *)(lbl_80478D78 + (u8)bVar5) = 0;
  }
  fightFloorLoopValidFightOutPokemon(0,_fightSeqTurnCheckSubFightOutPokemon__FPvUsPv,0,0);
  fightFloorInitFightOutPokemonPtrAryFightWazaJoutai(0);
  local_17 = 1;
  fightFloorLoopValidFightOutPokemon(0,fn_80213558,&local_17,0);
  for (uVar6 = 0; (uVar6 & 0xffff) < 2; uVar6 = uVar6 + 1) {
    side = fightFloorGetValidFightSidePtr(0,uVar6);
    if (side != 0) {
      fightSideInitJoutaiDataId(side,0x4d);
    }
  }
  fn_802317E4();
  fightFloorLoopValidFightOutPokemon(0,fn_80230568,0,1);
  fn_8022FE80();
  fightFloorLoopValidFightOutPokemon(0,fn_80230318,0,0);
  fn_801DA7AC();
  fightFloorLoopValidFightOutPokemon(0,fn_802301A8,0,1);
  fn_801DA7AC();
  local_18 = 0;
  fightFloorLoopValidFightOutPokemon(0,fn_80213558,&local_18,0);
  for (uVar6 = 0; (uVar6 & 0xffff) < 2; uVar6 = uVar6 + 1) {
    side = fightFloorGetValidFightSidePtr(0,uVar6);
    if (side != 0) {
      fightSideInitJoutaiDataId(side,0x4d);
    }
  }
  uVar6 = lbl_8047B618;
  uVar6 &= 0xfffffdff;
  *(volatile u32*)&lbl_8047B618 = uVar6;
  uVar6 &= 0xfff7ffff;
  *(volatile u32*)&lbl_8047B618 = uVar6;
  finalFlags = uVar6 & 0xffbfffff;
  uVar6 = finalFlags & 0xffefffff;
  *(volatile u32*)&lbl_8047B618 = finalFlags;
  *(volatile u32*)&lbl_8047B618 = uVar6;
  lbl_80379F58[0x16002] = 0;
  lbl_80379F58[0x160a1] = 0;
  for (bVar5 = 0; (bVar5 & 0xff) < 5; bVar5 = bVar5 + 1) {
    lbl_80478D78[(u8)bVar5] = 0;
  }
  sVar2 = fn_801EF634();
  if (sVar2 == 0) {
    sVar2 = fightFloorGetStatus(0,0,0xc,0);
    ++sVar2;
    if (0xff < sVar2) {
      sVar2 = 0xff;
    }
    fightFloorSetStatus(0,0,0xc,0,sVar2);
    uVar4 = fn_800E0C54();
    fightFloorSetStatus(0,0,0x5b,0,uVar4);
    fn_801DA7AC();
    fightMenuAllFightTrainerCloseStatusMenu(0);
    fightMenuAllFightOutPokemonCloseStatusMenu(0);
    fightMenuCloseMsg();
  }
  return;
}
#pragma opt_propagation reset
#pragma optimize_for_size on
u32 fn_80213558(u32 r3, u32 r4, char* r5)

{
    extern int fightOutPokemonGetJoutaiMigawariHp();
    extern void fn_80201FDC();
    extern u32 fn_80202108();
    extern u32 fn_80202234();
    extern u8 fn_802026E4();
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
    if (fn_802026E4(r3,0x12) == 1) {
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
  if ((fn_802026E4(r3,0x14) == 1) &&
      (iVar1 = fightOutPokemonGetJoutaiMigawariHp(r3), iVar1 <= 0)) {
    fightOutPokemonWriteJoutaiDataId(r3,0x14);
  }
  return 1;
}
#pragma optimize_for_size reset
void WS_STATUS_SET32(void)

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

  iVar1 = (int)lbl_8047B610;
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
void WS_STATUS_SET16(void)

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

  iVar1 = (int)lbl_8047B610;
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
void WS_STATUS_SET8(void)

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

  iVar1 = (int)lbl_8047B610;
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
void WS_STATUS_GET32(void)

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

  iVar1 = (int)lbl_8047B610;
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
void WS_STATUS_GET16(void)

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

  iVar1 = (int)lbl_8047B610;
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
void WS_STATUS_GET8(void)

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

  iVar1 = (int)lbl_8047B610;
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
void fn_80213A78(void)

{
    extern void fn_8012190C();
    extern u8 fn_80121ADC();
    extern void fn_80232FE4();
    extern u8 fn_802026E4();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightFloorGetFightOutPokemonPtrToHeroPtr();
    extern u32 fightFloorGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern int fightTrainerSearchInvalidFightPokemonPtr();
    extern u32 heroCatchPokemon();
    extern int heroGetStatus(u32, u32, u16);
    extern u8 pokemonCheckValid();
    extern u32 fightFloorGetFightPokemonEntryCntInc();
    extern void fightPokemonCreate();
    extern u8 fightTrainerIsGcHero();
    extern void fightPokemonGetFriendFormPokemonFriendFilterId();
    extern void pokemonSetOnZukanFlag();
    extern void pokemonSetOnDarkPokemonFlag();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonResetSeqStatus();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonSetHensinFightPokemonStatusId();
    extern void fightTrainerSetStatus();
  u8 bVar13;
  u32 uVar3;
  u32 uVar2;
  u32 uVar4;
  u32 uVar6;
  struct {
    u32 value;
  } uVar1;
  u32 uVar7;
  u32 caught;
  u32 uVar5;
  u16 uVar10;
  u16 firstItem;
  u16 uVar9;
  u32 uVar8;
  u8 cVar12;

  bVar13 = 0;
  uVar1.value = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar8 = (int)pokemonGetStatus(uVar1.value,0,0xe5,0);
  firstItem = itemGetStatus(uVar8,0,0x1e,0);
  uVar9 = itemGetStatus(0,firstItem,10,0);
  uVar2 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar1.value);
  uVar7 = fightFloorGetFightOutPokemonPtrToHeroPtr(0,uVar1.value);
  uVar3 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar4 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar3);
  uVar5 = fightFloorGetStatus(0,0,0x4a,0);
  pokemonGetStatus(uVar3,0,0xd5,0);
  uVar6 = fightPokemonGetPokemonPtr();
  uVar10 = fightFloorGetStatus(0,0,0x17,0);
  pokemonSetStatus((void*)uVar6,0,0xc9,0,0);
  uVar1.value = heroCatchPokemon(uVar7,uVar6,uVar5,uVar9,1);
  if ((short)uVar1.value == -1) {
    bVar13 = 1;
  }
  if ((bVar13 == 0) && (0 <= (short)uVar1.value) &&
      (uVar5 = fightTrainerSearchInvalidFightPokemonPtr(uVar2,uVar10), uVar5 != 0) &&
      (caught = heroGetStatus(uVar7,3,(u16)uVar1.value), caught != 0) &&
      (cVar12 = pokemonCheckValid(), cVar12 == 1)) {
    cVar12 = fn_80121ADC(caught,4);
    if (cVar12 == 1) {
      fn_8012190C(caught,4,1);
    }
    uVar1.value = fightFloorGetFightPokemonEntryCntInc(0);
    fightPokemonCreate(uVar5,caught,uVar1.value);
    pokemonSetStatus((void*)uVar5,0,0xcf,0,1);
    cVar12 = fightFloorGetStatus(0,0,0x27,0);
    if (((cVar12 == 1) && (cVar12 = fightFloorGetStatus(0,0,0x2e,0), cVar12 == 1)) &&
       (cVar12 = fightTrainerIsGcHero(uVar2), cVar12 == 1)) {
      fightPokemonGetFriendFormPokemonFriendFilterId(uVar5,3);
    }
  }
  cVar12 = fightFloorGetStatus(0,0,0x1f,0);
  if ((cVar12 == 1) && (cVar12 = fightTrainerIsGcHero(uVar2), cVar12 == 1)) {
    pokemonSetOnZukanFlag(uVar6,1);
    pokemonSetOnDarkPokemonFlag(uVar6,1);
  }
  pokemonSetStatus((void*)uVar6,0,0x83,0,0);
  uVar1.value = fightOutPokemonGetPokemonPtr(uVar3);
  pokemonSetStatus((void*)uVar1.value,0,0x83,0,0);
  fightFloorLoopValidFightOutPokemon(0,(u32)fn_80232FE4,uVar3,0);
  fightOutPokemonGetPokemonPtr(uVar3);
  pokemonInitJoutai();
  fightOutPokemonResetSeqStatus(uVar3,0);
  fightOutPokemonWriteJoutaiDataId(uVar3,0x17);
  cVar12 = fightOutPokemonIsUseHensinBuff(uVar3);
  if (cVar12 == 1) {
    fightOutPokemonSetHensinPokemonStatusId(uVar3,0x7c,0,0);
  }
  cVar12 = fn_802026E4(uVar3,0x3e);
  if (cVar12 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar3,0x3e);
    cVar12 = fightOutPokemonIsUseHensinBuff(uVar3);
    if (cVar12 == 1) {
      fightOutPokemonSetHensinPokemonStatusId(uVar3,200,0,0);
    }
  }
  uVar1.value = (int)pokemonGetStatus(uVar3,0,0xd6,0);
  pokemonSetStatus((void*)uVar1.value,0,0xd2,0,1);
  cVar12 = fightOutPokemonIsUseHensinBuff(uVar3);
  if (cVar12 == 1) {
    fightOutPokemonSetHensinFightPokemonStatusId(uVar3,0xd2,0);
  }
  fightTrainerSetStatus(uVar4,0,0x4a,0,1);
  if (bVar13 == 1) {
    fn_801EF8F4(1);
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
#pragma optimize_for_size on
void fn_80213E94(void)

{
    extern u32 __cvt_fp2unsigned(double);
    extern double sqrt(double);
    extern u32 fn_800E0C54();
    extern u8 pokemonIsDarkPokemon();
    extern void msgctrlSetValue();
    extern void fn_80165668();
    extern u8 fn_801EEAD0();
    extern u16 fn_801EEE44();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightFloorGetStatus();
    extern void fightTrainerHokakuThrowEffect();
    extern u8 fn_802026E4();
    extern u32 fightOutPokemonGetNicknamePtr();
    extern u32 figthOutPokemonGetLevel();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightOutPokemonHokakuEffect();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightMenuCloseMsg();
    extern void fightMenuOpenMsg();
    extern u8 lbl_80375E51[];
    extern u8 lbl_80375E5F[];
    extern u32 lbl_80279E1C[];
    extern u32 pokemonGetStatus();

  u16 floorStatusF;
  u32 uVar1;
  u32 trainerPtr;
  int uVar11;
  int catchProduct;
  u8 level;
  u16 uVar3;
  u16 uVar8;
  u32 uVar4;
  u32 uVar2;
  struct {
    u32 value;
  } uVar12;
  int currentHp;
  int iVar5;
  u16 uVar9;
  u16 sVar7;
  u8 cVar10;
  u32 sqrtResult;
  u8 local_58[8];

  uVar11 = 0;
  uVar12.value = 0;
  sVar7 = fightFloorGetStatus(0,0,0xd,0);
  floorStatusF = fightFloorGetStatus(0,0,0xf,0);
  uVar1 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar2 = pokemonGetStatus(uVar1,0,0xe5,0);
  itemGetStatus(uVar2,0,0x20,0);
  uVar3 = itemGetStatus(uVar2,0,0x1e,0);
  uVar2 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar4 = fightOutPokemonGetPokemonPtr();
  uVar8 = pokemonGetStatus(uVar4,0,0xc3,0);
  uVar9 = (u16)pokemonGetStatus(uVar4,0,0x6e,0);
  level = figthOutPokemonGetLevel(uVar2);
  currentHp = pokemonGetStatus(uVar4,0,0x83,0);
  iVar5 = pokemonGetStatus(uVar4,0,0x87,0);
  trainerPtr = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar1);
  cVar10 = pokemonIsDarkPokemon(uVar4);
  if (cVar10 == 1) {
    uVar9 = fn_801EEE44(uVar8);
  }
  else {
    uVar9 = (u8)pokemonGetStatus(0,uVar9,0x12,0);
  }
  switch (uVar3) {
  case 2:
    uVar12.value = 0x14;
    break;
  case 3:
    uVar12.value = 0xf;
    break;
  case 4:
  case 5:
    uVar12.value = 0xa;
    break;
  case 6:
    if ((fightOutPokemonIsZokuseiDataId(uVar2, 0xb) == 1) ||
        (fightOutPokemonIsZokuseiDataId(uVar2, 6) == 1)) {
      uVar12.value = 0x1e;
    } else {
      uVar12.value = 0xa;
    }
    break;
  case 7:
    if ((((u16)(floorStatusF - 3)) <= 1) || (floorStatusF == 5)) {
      uVar12.value = 0x23;
    } else {
      uVar12.value = 0xa;
    }
    break;
  case 8:
    if (level < 0x28) {
      uVar12.value = 0x28 - level;
      if ((u8)uVar12.value < 0xa) {
        uVar12.value = 0xa;
      }
    } else {
      uVar12.value = 0xa;
    }
    break;
  case 9:
    if (pokemonIsDarkPokemon(uVar4) == 1) {
      if (fn_801EEAD0(uVar8) == 1) {
        uVar11 = 1;
      }
    } else if ((u8)pokemonGetStatus(uVar4, 0, 0x63, 0) == 1) {
      uVar11 = 1;
    }
    if ((u8)uVar11 == 1) {
      uVar12.value = 0x1e;
    } else {
      uVar12.value = 0xa;
    }
    break;
  case 10:
    uVar12.value = (u16)fightFloorGetStatus(0, 0, 0xc, 0) + 0xa;
    if ((u8)uVar12.value > 0x28) {
      uVar12.value = 0x28;
    }
    break;
  case 11:
  case 12:
    uVar12.value = 0xa;
    break;
  }
  if ((uVar3 == 1) || (sVar7 == 0x11)) {
    uVar12.value = 4;
  }
  else {
    catchProduct = (int)(u8)uVar9;
    catchProduct *= (int)(u8)uVar12.value;
    uVar11 = (catchProduct / 10) * ((iVar5 * 3) - (currentHp * 2)) /
             (iVar5 * 3);
    cVar10 = fn_802026E4(uVar2,8);
    if ((cVar10 == 1) || (cVar10 = fn_802026E4(uVar2,7), cVar10 == 1)) {
      uVar11 = uVar11 << 1;
    }
    cVar10 = fn_802026E4(uVar2,3);
    if ((((cVar10 == 1) || (cVar10 = fn_802026E4(uVar2,4), cVar10 == 1)) ||
        (cVar10 = fn_802026E4(uVar2,5), cVar10 == 1)) ||
       (cVar10 = fn_802026E4(uVar2,6), cVar10 == 1)) {
      uVar11 = (u32)(uVar11 * 0xf) / 10;
    }
    if ((u32)uVar11 >= 0xff) {
      uVar12.value = 4;
    }
    else {
      sqrtResult = __cvt_fp2unsigned(
          sqrt((double)(0xff0000 / (u32)uVar11)));
      uVar11 = __cvt_fp2unsigned(sqrt((double)sqrtResult));
      uVar11 = (u32)0xffff0 / (u32)uVar11;
      uVar12.value = 0;
      while ((uVar12.value & 0xff) < 4) {
        if ((fn_800E0C54() & 0xffff) >= (u32)uVar11) {
          break;
        }
        uVar12.value = uVar12.value + 1;
      }
    }
  }
  fightTrainerHokakuThrowEffect(trainerPtr,uVar3,0);
  fightOutPokemonHokakuEffect(uVar2,uVar12.value,uVar3,0,local_58);
  fightTrainerHokakuThrowEffect(trainerPtr,uVar3,1);
  fightTrainerHokakuThrowEffect(trainerPtr,uVar3,3);
  fightOutPokemonHokakuEffect(uVar2,uVar12.value,uVar3,1,local_58);
  uVar4 = fightOutPokemonGetNicknamePtr(uVar2);
  msgctrlSetValue(0x16,uVar4);
  if ((uVar12.value & 0xff) >= 4) {
    fightTrainerHokakuThrowEffect(trainerPtr,uVar3,4);
    msgctrlSetValue(0x5d,0);
    fightMenuOpenMsg(0x771f);
    fn_80165668(0x3f6,0,0xff);
    fightTrainerHokakuThrowEffect(trainerPtr,uVar3,5);
    fightMenuCloseMsg();
    lbl_8047B610 = lbl_80375E51;
  }
  else {
    fightOutPokemonHokakuEffect(uVar2,uVar12.value,uVar3,2,local_58);
    lbl_80478D78[5] = (u8)uVar12.value;
    fightMenuOpenMsg(lbl_80279E1C[(uVar12.value << 24) >> 24]);
    fightOutPokemonHokakuEffect(uVar2,uVar12.value,uVar3,3,local_58);
    fightMenuCloseMsg();
    lbl_8047B610 = lbl_80375E5F;
  }
  fightTrainerHokakuThrowEffect(trainerPtr,uVar3,2);
  fightTrainerHokakuThrowEffect(trainerPtr,uVar3,6);
  fightOutPokemonHokakuEffect(uVar2,uVar12.value,uVar3,4,local_58);
  return;
}
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#pragma optimize_for_size on
void fn_802145C8(void)

{
    extern int fn_801F025C();
    extern void fn_801F4C14();
  u32 uVar1;
  int iVar2;
  int iVar3;
  int originalTarget;
  int iVar4;
  struct {
    int value;
  } status43;

  iVar2 = fn_801F025C(0x11,0);
  iVar3 = fn_801F025C(0x12,0);
  iVar4 = fn_801F025C(0x19,0);
  originalTarget = iVar2;
  fn_801F4C14(0,0,0x47,0,iVar2);
  uVar1 = __cntlzw(iVar3 - iVar2);
  if (uVar1 >> 5 != 0) {
    status43.value = iVar4;
    iVar2 = iVar4;
  }
  if ((s32)(uVar1 >> 5) == 0) {
    status43.value = iVar4;
  }
  fn_801F4C14(0,0,0x4b,0,originalTarget);
  fn_801F4C14(0,0,0x36,0,iVar2);
  fn_801F4C14(0,0,0x43,0,status43.value);
  lbl_8047B610 += 1;
  return;
}
#pragma optimize_for_size reset
#undef fn_801F025C
#undef fn_801F4C14
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80136428 tikeiDataBiosGetZokuseiDataId
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_80207B5C fightOutPokemonSetZokuseiDataId
#define fn_800FA280 GSmsgGetGSchar
#define fn_80132A38 msgctrlSetValue
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
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_80136428
#undef fn_80207AE0
#undef fn_80207B5C
#undef fn_800FA280
#undef fn_80132A38
#pragma optimize_for_size reset
#pragma opt_propagation off
#define fn_801F54A4 fightFloorGetStatus
#define fn_80136468 tikeiDataBiosGetFightKoukaId
void WS_HIMITUNOTIKARA(void)

{
    extern u32 fn_80136468();
    extern u32 fn_801F54A4();
    extern u8 lbl_80478D78[1];
  u16 uVar1;
  u8 uVar2;
  u32 pc;
  u8* status;
  uVar1 = fn_801F54A4(0,0,0xf,0);
  uVar2 = fn_80136468(uVar1);
  status = lbl_80478D78;
  pc = *(volatile u32*)&lbl_8047B610;
  status[3] = uVar2;
  lbl_8047B610 = (u8*)(pc + 1);
  return;
}
#undef fn_801F54A4
#undef fn_80136468
#pragma opt_propagation reset
#pragma optimize_for_size on
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011CB54 pokemonTokuseiDataBiosGetName
#define fn_8011CB6C pokemonTokuseiDataBiosGetPtr
#define fn_80132A38 msgctrlSetValue
#define fn_801F4C14 fightFloorSetStatus
#define fn_80202B88 fightOutPokemonIsAlly
#define fn_802062FC fightOutPokemonCheckFightOut
#pragma opt_propagation off
u32 fn_80214CFC(u32 r3, u32 r4, u32 r5)

{
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fn_800FA280();
    extern void fn_8011CB54();
    extern void fn_8011CB6C();
    extern void fn_80132A38();
    extern void fn_801F4C14();
    extern u8 fn_80202B88();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
  extern u8 lbl_8037960A[];
  extern void* lbl_8047B62C;
  u32 ctx;
  u32 other;

  ctx = r3;
  other = r5;
  {
    u32 value;

    value = fightOutPokemonGetTokuseiDataId(other);
    if (fn_802062FC(ctx) == 0) {
      return 1;
    }
    if (fn_80202B88(ctx,other) == 0) {
      fn_801F4C14(0,0,0x42,0,ctx);
      fn_8011CB6C(value);
      fn_8011CB54();
      value = fn_800FA280();
      fn_80132A38(0xd,value);
      fn_80211B94(lbl_8047B62C,lbl_8037960A,0);
    }
  }
  return 1;
}
#pragma opt_propagation reset
#undef fn_802062FC
#undef fn_80202B88
#undef fn_801F4C14
#undef fn_80132A38
#undef fn_8011CB6C
#undef fn_8011CB54
#undef fn_800FA280
#pragma optimize_for_size reset
#pragma optimize_for_size on
int fn_80215008(u32 trainer, u16* buffer, u32 maxCount, u32 context)

{
    extern u32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u16);
    extern u32 heroGetStatus(u32, u32, u16);
    extern u32 fightTrainerCheckTemotiPokemonFightEntry(u32, u32);
    extern u8 fightPokemonCheckFightOut(u32);
    extern u32 fightPokemonGetPokemonPtr(u32);
    extern u8 pokemonWazaCheckValid(u32, u16);
    extern u16 lbl_80279FA0[];
    u32 excludedPokemon;
    u32 fightPokemon;
    u32 heroPokemon;
    u32 pokemon;
    u16 moveId;
    s32 moveIndex;
    u16 tableMove;
    u8 blocked;
    s32 capacity;
    s32 heroIndex;
    u32 trainerData;
    s32 clearIndex;
    s32 count;
    u32 tableOffset;

    trainerData = fightTrainerGetStatus(trainer, 0, 0x44, 0);
    excludedPokemon = pokemonGetStatus(context, 0, 0xd5, 0);
    capacity = maxCount & 0xffff;
    for (clearIndex = 0; clearIndex < capacity; clearIndex++) {
        buffer[clearIndex] = 0;
    }

    count = 0;
    heroIndex = 0;
    do {
        heroPokemon = heroGetStatus(trainerData, 3, heroIndex);
        fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(trainer, heroPokemon);
        if (fightPokemon != 0 && fightPokemonCheckFightOut(fightPokemon) != 0 &&
            excludedPokemon != fightPokemon) {
            pokemon = fightPokemonGetPokemonPtr(fightPokemon);
            moveIndex = 0;
            do {
                if (pokemonWazaCheckValid(pokemon, moveIndex) != 0) {
                    moveId = pokemonGetStatus((unsigned long)pokemon, 0, 0x7f, moveIndex);
                    if (moveId == 0 || moveId == 0x165 || moveId == 0xd6 ||
                        moveId == 0x112 || moveId == 0x77 || moveId == 0x76) {
                        blocked = 1;
                    } else {
                        blocked = 0;
                    }
                    if (blocked == 0) {
                        tableOffset = 0;
                        while ((tableMove = lbl_80279FA0[tableOffset]) != 0xffff) {
                            if (moveId == tableMove) {
                                break;
                            }
                            tableOffset++;
                        }
                        if (tableMove == 0xffff && moveId != 0 && moveId != 0x165 &&
                            count < capacity) {
                            buffer[count] = moveId;
                            count++;
                        }
                    }
                }
                moveIndex++;
            } while (moveIndex < 4);
        }
        heroIndex++;
    } while (heroIndex < 6);
    return count;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
void fn_80215374(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fn_80201890();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern u32 fn_80215528();
    u32 pokemon;
    u32 stateMask;
    u32 amount;
    u32 pokemonData;
    u32 newValue;
    u8 currentValue;
    u8 slot;
    s8 moveSlot;
    s32 signedMoveSlot;
    s32 loopState[2];

    pokemon = fightTargetGetPtrAsNowFightType(0x11, 0);
    if (fn_802025B8(pokemon, 0x27) != 2) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (pokemon != 0) {
        amount = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
            0, 0x2e, 2, pokemon);
        if (amount != 0) {
            pokemonData = fightOutPokemonGetPokemonPtr(pokemon);
            moveSlot = pokemonSearchWazaDataId(pokemonData, 0x11e);
            if (moveSlot >= 0) {
                signedMoveSlot = moveSlot;
                currentValue = pokemonGetStatus(pokemonData, 0, 0x80, signedMoveSlot);
                newValue = amount < currentValue ? (u8)(currentValue - amount) : 0;
                pokemonSetStatus(pokemonData, 0, 0x80, signedMoveSlot, (u8)newValue);
                slot = moveSlot;
                if (fn_802026E4(pokemon, 0x10) == 0 &&
                    fn_802026E4(pokemon, 0x31) == 1) {
                    stateMask = fn_80201890(pokemon, 0x31);
                    if ((stateMask & (1 << slot)) == 0 &&
                        fightOutPokemonIsUseHensinBuff(pokemon) == 1) {
                        fightOutPokemonSetHensinPokemonStatusId(
                            pokemon, 0x80, slot, 0);
                    }
                }
            }
        }
    }

    loopState[0] = pokemon;
    loopState[1] = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_80215528, loopState, 0);
    if (loopState[1] == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fn_8020248C(pokemon, 0x27, 0);
        lbl_8047B610 += 5;
    }
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_80215528(u32 r3, u32 r4, u32* r5)

{
    extern s8 pokemonSearchWazaDataId();
    extern u8 pokemonWazaCheckValid();
    extern u8 fightOutPokemonIsAlly();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 pokemonGetStatus();
  u32 otherPokemon;
  u32 other;
  s32 value;
  u32 pokemon;
  s8 slot;
  s8 moveSlot;
  u16 move;

  other = *r5;
  value = r3;
  if (fightOutPokemonCheckFightOut() == 0) {
    return 1;
  }
  if (fightOutPokemonIsAlly(value,other) == 1) {
    return 1;
  }
  otherPokemon = fightOutPokemonGetPokemonPtr(other);
  pokemon = fightOutPokemonGetPokemonPtr(value);
  for (slot = 0; slot < 4; slot++) {
    value = (int)slot;
    if (pokemonWazaCheckValid(otherPokemon,value) == 0) {
      continue;
    }
    move = (int)pokemonGetStatus(otherPokemon,0,0x7f,value);
    moveSlot = pokemonSearchWazaDataId(pokemon,move);
    if (moveSlot < 0) {
      continue;
    }
    r5[1] = 1;
    return 0;
  }
  return 1;
}
#pragma optimize_for_size reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_802096E8 fightWazaIsHit
#define fn_80207BC0 fightOutPokemonSetTokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
void fn_80215614(void)

{
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern u32 fn_80205184();
    extern void fn_80207BC0();
    extern u8 fn_802096E8();
    extern u8 fn_80229934();
    extern u32 fn_80207BF4();
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
  uVar4 = fn_80205184((void*)uVar1);
  uVar5 = fn_801F025C(0x12,0);
  uVar6 = fn_80207BF4();
  if (((((uVar3 & 0xffff) == 0) && ((uVar6 & 0xffff) == 0)) || ((u16)uVar3 == 0x19)) ||
     ((((uVar6 & 0xffff) == 0x19 || (cVar7 = fn_802096E8(uVar2), cVar7 == 0)) ||
      (cVar7 = fn_80229934(uVar4,uVar1,uVar5), cVar7 == 1)))) {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    return;
  }
  fn_80207BC0(uVar1,uVar6);
  fn_80207BC0(uVar5,uVar3);
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205184
#undef fn_802096E8
#undef fn_80207BC0
#undef fn_80207BF4
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
void fn_80215720(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern u32 fn_80205184();
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
    fn_80205184((void*)uVar1);
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
#pragma optimize_for_size reset
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F54A4 fightFloorGetStatus
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
#undef fn_801F54A4
#undef fn_801F0134
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205184
#undef fn_8011BEB4
#undef fn_80205B8C
#undef fn_8011BBD8
#pragma optimize_for_size on
void fn_80215AEC(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern s32 pokemonGetStatus();
    extern u8 fn_802025B8();
    extern void fn_8020248C();
    extern void fn_80201764();
    extern u32 fn_80201890();
    extern void msgctrlSetValue();
    extern u16 fightOutPokemonMaxHpWaruValue();
    extern void wazaSetStatus();
    extern u8 fightOutPokemonIsHpMantan();

    switch (lbl_8047B610[1]) {
    case 0: {
        u32 value;
        u32 ctx;
        u32 finalValue;

        ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
        value = pokemonGetStatus(ctx, 0, 0xd5, 0);
        value = pokemonGetStatus(value, 0, 0xcb, 0);
        finalValue = pokemonGetStatus(value, 0, 0x77, 0);
        if (fn_802025B8(ctx, 0x35) != 2) {
            lbl_8047B610 = *(u8 **)(lbl_8047B610 + 2);
        } else {
            fn_8020248C(ctx, 0x35, 0);
            fn_80201764(ctx, 0x35, finalValue);
            lbl_8047B610 += 6;
        }
        break;
    }
    case 1: {
        u32 pokemon;
        u32 defender;
        u32 attacker;

        attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
        pokemon = pokemonGetStatus(attacker, 0, 0xd9, 0);
        defender = fightTargetGetPtrAsNowFightType(0x12, 0);
        msgctrlSetValue(0xd, fn_80201890(defender, 0x35));
        wazaSetStatus(pokemon, 0, 0x2d, 0,
                      -(s32)(u16)fightOutPokemonMaxHpWaruValue(defender, 2));
        if (fightOutPokemonIsHpMantan(defender) == 1) {
            lbl_8047B610 = *(u8 **)(lbl_8047B610 + 2);
        } else {
            lbl_8047B610 += 6;
        }
        break;
    }
    }
}
#pragma optimize_for_size reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_80207BC0 fightOutPokemonSetTokuseiDataId
void WS_NARIKIRI(void)

{
    extern u32 fn_801F025C();
    extern u32 fn_80207BF4();
    extern void fn_80207BC0();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fn_801F025C(0x11,0);
  fn_80207BF4();
  fn_801F025C(0x12,0);
  uVar2 = fn_80207BF4();
  if (((uVar2 & 0xffff) == 0) || ((uVar2 & 0xffff) == 0x19)) {
    goto jump;
  }
  fn_80207BC0(uVar1,uVar2);
  lbl_8047B610 += 5;
  return;
jump:
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}
#undef fn_801F025C
#undef fn_80207BF4
#undef fn_80207BC0
void fn_80215CF0(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern s32 fightFloorGetStatus();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u8 fn_802026E4();
    extern u8 fn_80142984();
    extern void fightOutPokemonDoItemSoubi();
    extern void pokemonSetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 itemGetStatus();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern u8 lbl_803798D8[];
    u32 attacker;
    u32 defender;
    u32 attackerItem;
    u32 ability;
    u32 defenderItem;
    u8 proceed;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    attackerItem = fightOutPokemonGetSoubiItemDataId(attacker);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    ability = fightOutPokemonGetTokuseiDataId(defender);
    defenderItem = fightOutPokemonGetSoubiItemDataId(defender);

    if ((u8)fightFloorGetStatus(0, 0, 0x2f, 0) == 1 &&
        fightOutPokemonIsGcHeroFightOutPokemon(attacker) == 0) {
        proceed = 0;
    } else if ((u8)fightFloorGetStatus(0, 0, 0x2f, 0) == 1 &&
               fn_802026E4(attacker, 0x3d) == 1) {
        proceed = 0;
    } else {
        proceed = 1;
    }

    if (proceed == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (fightFloorGetStatus(0, 0, 0x2f, 0) == 0) {
        goto second_check_true;
    }
    if (fn_802026E4(attacker, 0x3d) != 0) {
        goto second_check_false;
    }
    if (fn_802026E4(defender, 0x3d) == 0) {
        goto second_check_true;
    }
second_check_false:
    proceed = 0;
    goto second_check_done;
second_check_true:
    proceed = 1;
second_check_done:

    if (proceed == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (!(((u16)attackerItem != 0 || (u16)defenderItem != 0) &&
          (u16)attackerItem != 0xaf && (u16)defenderItem != 0xaf &&
          ((u16)attackerItem == 0 || fn_80142984(attackerItem) != 0) &&
          ((u16)defenderItem == 0 || fn_80142984(defenderItem) != 0))) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    {
        if ((u16)ability == 0x3c) {
            lbl_8047B610 = lbl_803798D8;
            return;
        }

        if ((u16)attackerItem != 0) {
            fightOutPokemonDoItemSoubi(attacker, 0, 0, 0);
        }
        if ((u16)defenderItem != 0) {
            fightOutPokemonDoItemSoubi(defender, 0, 0, 0);
        }
        if ((u16)attackerItem != 0) {
            fightOutPokemonDoItemSoubi(defender, attackerItem, 1, 0);
        }
        if ((u16)defenderItem != 0) {
            pokemonSetStatus(attacker, 0, 0xfb, 0, (u16)defenderItem);
        }

        if (fightOutPokemonIsUseHensinBuff(attacker) == 1) {
            fightOutPokemonSetHensinPokemonStatusId(attacker, 0x82, 0, 0);
        }
        if (fightOutPokemonIsUseHensinBuff(defender) == 1) {
            fightOutPokemonSetHensinPokemonStatusId(defender, 0x82, 0, 0);
        }
        fightOutPokemonWriteJoutaiDataId(attacker, 0x36);
        fightOutPokemonWriteJoutaiDataId(defender, 0x36);

        itemGetStatus(0, defenderItem, 1, 0);
        msgctrlSetValue(0xd, GSmsgGetGSchar());
        itemGetStatus(0, attackerItem, 1, 0);
        msgctrlSetValue(0xe, GSmsgGetGSchar());

        if ((u16)attackerItem == 0) {
            goto one_item_missing;
        }
        if ((u16)defenderItem == 0) {
            goto one_item_missing;
        }
        lbl_80478D78[5] = 2;
        goto item_state_done;
one_item_missing:
        if ((u16)defenderItem == 0) {
            goto defender_item_missing;
        }
        lbl_80478D78[5] = 0;
        goto item_state_done;
defender_item_missing:
        lbl_80478D78[5] = 1;
item_state_done:
        lbl_8047B610 += 5;
        return;
    }
}
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801363E8 tikeiDataBiosGetWazaId
#define fn_8011BEB4 wazaGetStatus
#define fn_80209C1C fightWazaSetUseWazaStatus
#define fn_800FA280 GSmsgGetGSchar
#define fn_80132A38 msgctrlSetValue
#define fn_801F4C14 fightFloorSetStatus
void fn_80216410(void)

{
    extern u32 fn_800FA280();
    extern u16 fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801363E8();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_80209C1C();
    extern void fn_80211B94();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
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
      lbl_8047B610 = (u8*)lbl_80379BFF[(uVar4 & 0xffff)];
  return;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_8012640C
#undef fn_801363E8
#undef fn_8011BEB4
#undef fn_80209C1C
#undef fn_800FA280
#undef fn_80132A38
#undef fn_801F4C14
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_JUUDEN(void)

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
#undef fn_801F025C
#pragma optimize_for_size on
void fn_80216A58(void)
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
    extern u8 fn_802026E4();
    extern void wazaSetStatus();
    extern u8 lbl_80478D78[1];
    u32 attacker;
    u32 moveData;
    u32 move;
    u32 trainer;
    u32 party;
    u32 defender;
    u8 initialIndex;
    u32 fightPokemon;
    u32 pokemon;
    u16 species;
    u8 level;
    u32 defenderPokemon;
    s32 damage;
    s32 result;
    u32 power;
    s32 defense;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    moveData = pokemonGetStatus(attacker, 0, 0xd9, 0);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, attacker);
    party = fightTrainerGetStatus(trainer, 0, 0x44, 0);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    if (fightOutPokemonCheckFightOut() == 0) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        return;
    }

    initialIndex = lbl_80478D78[0];
    pokemon = 0;
    goto check_party_index;
check_party_pokemon:
    pokemon = heroGetStatus(party, 3);
    fightPokemon = fightTrainerCheckTemotiPokemonFightEntry(trainer, pokemon);
    if (fightPokemon != 0 && fightPokemonCheckValid(fightPokemon) != 0) {
        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
        if (pokemonCheckFightOut(pokemon) == 1 && pokemonIsJoutaiNormal(pokemon) == 1) {
            goto found_party_pokemon;
        }
    }
    lbl_80478D78[0]++;
check_party_index:
    if (lbl_80478D78[0] < 6) {
        goto check_party_pokemon;
    }

found_party_pokemon:
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
        damage = damage / defense / 50 + 2;
        result = damage;
        if (fn_802026E4(attacker, 0x32) == 1) {
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
#pragma optimize_for_size reset
#pragma opt_propagation off
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8011BEB4 wazaGetStatus
void fn_80216D9C(void)

{
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_80201764();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern u32 fn_80205184();
    extern int fn_80232110();
  u16 uVar7;
  u32 uVar1;
  u32 uVar11;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u16 uVar10;
  u16 uVar8;
  int iVar6;
  register int divisor;
  register int product;
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
  fn_8020248C(uVar3,0x34,uVar2);
  fn_80201B2C(uVar3,0x34,uVar4);
  iVar6 = fn_80232110(uVar1,uVar3,uVar5,uVar4,uVar10,uVar8);
  cVar9 = fn_802026E4(uVar1,0x32);
  if (cVar9 == 1) {
    product = iVar6 * 0xf;
    divisor = 10;
    iVar6 = product / divisor;
  }
  fn_80201764(uVar3,0x34,iVar6);
  if ((uVar4 & 0xffff) == 0x161) {
    lbl_80478D78[5] = 1;
  }
  else {
    lbl_80478D78[5] = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 5;
  return;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_801F0134
#undef fn_8012640C
#undef fn_80205184
#undef fn_8011BEB4
#pragma opt_propagation reset
void fn_80216F50(void)

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
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_MARUKUNARU(void)

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
#undef fn_801F025C
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80201600 fightOutPokemonCopyAllAbiCnt
void WS_JIKOANJI(void)

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
#undef fn_801F025C
#undef fn_80201600
void fn_802175A8(void)

{
    extern u16 wazaGetStatus();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u16 fightActionGetKindDataId();
    extern u8 fightActionCheckValid();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonIsAlly();
    extern u8 fightOutPokemonCheckFightOut();
    extern u16 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_80379F58[];
    extern u32 lbl_8047B618;
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
  u32 context;
  u32 relativeTarget;
  u32 candidate;
  u32 action;
  u32 waza;
  u32 selected;

  context = fightTargetGetPtrAsNowFightType(0x14,0);
  relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(
      context,(u16)fightFloorGetStatus(0,0,0x14,0));
  if (fightOutPokemonCheckFightOut(context) == 0) {
    lbl_8047B648 = lbl_8047B649;
  } else {
    candidate = 0;
    selected = 0;
    while (lbl_8047B648 < lbl_8047B649) {
      candidate = fightFloorGetStatus(0,0,0x5d,lbl_8047B648);
      if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 &&
          context != candidate && fightOutPokemonIsAlly(context,candidate) != 1 &&
          (action = pokemonGetStatus(candidate,0,0xfe,0)) != 0 &&
          fightActionCheckValid() != 0 &&
          fightActionGetKindDataId(action) == 0x13) {
        waza = pokemonGetStatus(candidate,0,0xd9,0);
        if (fightOutPokemonGetUseWazaDataId(candidate) == 0xe4 &&
            (u16)relativeTarget == wazaGetStatus(waza,0,0x29,0) &&
            fn_802026E4(candidate,8) != 1 &&
            fn_802026E4(candidate,7) != 1 &&
            (s32)pokemonGetStatus(candidate,0,0xf9,0) != 1) {
          selected = candidate;
          break;
        }
      }
      lbl_8047B648++;
    }
    if (selected != 0) {
      fightFloorSetStatus(0,0,0x42,0,selected);
      lbl_80379F58[0x16002] = 1;
      lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
      pokemonSetStatus(candidate,0,0x112,0,1);
      lbl_8047B610 = lbl_8047B610 + 5;
      return;
    }
  }
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#pragma optimize_for_size on
void fn_80217AE4(void)

{
    extern void fn_8011BBD8();
    extern short fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 moveId;
  u32 uVar3;
  u16 sVar5;
  u32 uVar2;
  u16 uVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205B8C();
  uVar3 = (int)fn_8012640C(uVar1,0,0xd9,0);
  moveId = fn_80205184((void*)uVar1);
  sVar5 = fn_8011BEB4(0,moveId,9,0);
  uVar4 = (int)fn_8012640C(uVar2,0,0x99,0);
  if (sVar5 == 0x79) {
    uVar4 = (uVar4 * 10) / 0x19;
  }
  else {
    uVar4 = ((0xff - uVar4) * 10) / 0x19;
  }
  fn_8011BBD8(uVar3,0,0x2f,0,uVar4);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#pragma optimize_for_size reset
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_801F025C
#undef fn_80205184
#undef fn_80205B8C
void fn_80217D34(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u8 fn_802026E4();
  u8 index;
  u32 target;
  u32 statusId;
  s8 value;

  index = lbl_8047B610[1];
  target = fightTargetGetPtrAsNowFightType(0x12,0);
  switch ((u8)(s8)index) {
  case 1:
    statusId = 0xe6;
    break;
  case 2:
    statusId = 0xe7;
    break;
  case 3:
    statusId = 0xea;
    break;
  case 4:
    statusId = 0xe8;
    break;
  case 5:
    statusId = 0xe9;
    break;
  case 6:
    statusId = 0xeb;
    break;
  case 7:
    statusId = 0xec;
    break;
  case 0:
  default:
    statusId = 0;
    break;
  }
  value = pokemonGetStatus(target,0,statusId,0);
  if (fn_802026E4(target,9) == 1 && value >= 12) {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 2);
  } else {
    lbl_8047B610 += 6;
  }
  return;
}
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_802096E8 fightWazaIsHit
#define fn_80200B10 fightOutPokemonInitJoutaiKeep
#define fn_8020A2B8 fightWazaBiosCopy
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_8011BEB4 wazaGetStatus
#define fn_8011BBD8 wazaSetStatus
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
  u32 uVar3;
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar4;
  u8 cVar8;
  u8 uVar5;
  u32 uVar6;
  u16 sVar7;
  u32 uVar9;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  uVar3 = (int)fn_8012640C(uVar1,0,0xf8,0);
  uVar4 = fn_80205184((void*)uVar1);
  cVar8 = fn_802096E8(uVar2);
  if (cVar8 == 0) {
    fn_80200B10(uVar1);
    lbl_8047B610 = lbl_80375FDF;
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
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80205184
#undef fn_802096E8
#undef fn_80200B10
#undef fn_8020A2B8
#undef fn_80202810
#undef fn_8011BEB4
#undef fn_8011BBD8
#pragma optimize_for_size on
void fn_80218018(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u8 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u32 _fightSeqWsHorobinoutaSub__FPvUsPv();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    u32 attacker;
    u32 pokemon;
    s8 slot;
    u8 slot8;
    int slotIndex;
    u32 move;
    u32 target;
    u32 ability;
    u8 pp;
    u8 newPp;
    u32 flags;
    int callbackResult;
    u8* nextPc;

    callbackResult = 0;
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    if (fn_80229934(move, attacker, target) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    } else {
        fightFloorLoopValidFightOutPokemon(
            0, _fightSeqWsHorobinoutaSub__FPvUsPv, &callbackResult, 0);
        if (attacker != 0) {
            ability = (u8)fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
                0, 0x2e, 0, attacker);
            if (ability != 0) {
                pokemon = fightOutPokemonGetPokemonPtr(attacker);
                slot = pokemonSearchWazaDataId(pokemon, 0xc3);
                if (slot >= 0) {
                    slotIndex = slot;
                    pp = (u8)pokemonGetStatus(pokemon, 0, 0x80, slotIndex);
                    newPp = ability < pp ? (u8)(pp - ability) : 0;
                    pokemonSetStatus(pokemon, 0, 0x80, slotIndex, newPp);
                    slot8 = (u8)slot;
                    if (fn_802026E4(attacker, 0x10) == 0 &&
                        fn_802026E4(attacker, 0x31) == 1) {
                        flags = fn_80201890(attacker, 0x31);
                        if ((flags & (1 << slot8)) == 0 &&
                            fightOutPokemonIsUseHensinBuff(attacker) == 1) {
                            fightOutPokemonSetHensinPokemonStatusId(
                                attacker, 0x80, slot8, 0);
                        }
                    }
                }
            }
        }
        if (callbackResult == 0) {
            nextPc = *(u8**)(lbl_8047B610 + 1);
        } else {
            nextPc = lbl_8047B610 + 5;
        }
        lbl_8047B610 = nextPc;
    }
}
#pragma optimize_for_size reset
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#pragma optimize_for_size on
u32 _fightSeqWsHorobinoutaSub__FPvUsPv(void* r3, u16 r4, void* r5)

{
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802062FC();
    extern u32 fn_80207BF4();
  u32 pokemon;
  int* count;
  u8 cVar2;
  u32 sVar1;

  count = (int*)r5;
  pokemon = (u32)r3;
  cVar2 = fn_802062FC();
  if (cVar2 == 0) {
    return 1;
  }
  sVar1 = fn_80207BF4(pokemon);
  cVar2 = fn_802025B8(pokemon,0x1e);
  if (cVar2 != 2 || (u16)sVar1 == 0x2b) {
    return 1;
  }
  fn_8020248C(pokemon,0x1e,0);
  *count = *count + 1;
  return 1;
}
#pragma optimize_for_size reset
#undef fn_802062FC
#undef fn_80207BF4
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_MIYABURU(void)

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
#undef fn_801F025C
#define fn_801248C4 pokemonGetTokuseiDataId
#define fn_801252E0 pokemonInitJoutai
#define fn_8012A5B0 heroGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F7258 fightSideGetValidFightTrainerPtr
#define fn_801F9930 fightTrainerCheckTemotiPokemonFightEntry
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80202998 fightOutPokemonResetSeqStatus
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_80206780 fightOutPokemonCheckValid
#define fn_80206A04 fightPokemonCheckValid
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
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
  u32 uVar1;
  u32 sVar7;
  u32 iVar2;
  u32 uVar3;
  u32 uVar4;
  u8 cVar8;
  u32 uVar5;
  u32 trainerStatus;
  int iVar6;
  u32 uVar9;
  u32 uVar10;

  uVar1 = fn_801F025C(0x11,0);
  sVar7 = fn_80205184((void*)uVar1);
  iVar2 = fn_801F025C(0xe,uVar1);
  uVar3 = fn_801F025C(2,uVar1);
  uVar4 = (u16)fn_801F54A4(0,0,0x16,0);
  if ((sVar7 & 0xffff) == 0xd7) {
    lbl_80478D78[5] = 0;
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
      lbl_80478D78[5] |= 1;
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
        lbl_80478D78[5] |= 2;
      }
    }
    for (iVar6 = 0; iVar6 < uVar4; iVar6 = iVar6 + 1) {
      iVar2 = fn_801F7258(uVar3,iVar6 & 0xffff);
      if (iVar2 != 0) {
        uVar10 = fn_801FB1C0(iVar2,0,0x44,0);
        uVar9 = 0;
        do {
          trainerStatus = fn_801F9930(iVar2,
              fn_8012A5B0(uVar10,3,uVar9 & 0xffff));
          if ((trainerStatus != 0) && (cVar8 = fn_80206A04(), cVar8 != 0)) {
            uVar1 = fn_80205BE8(trainerStatus);
            sVar7 = fn_801248C4();
            if ((sVar7 & 0xffff) != 0x2b) {
              fn_801252E0(uVar1);
            }
          }
          uVar9 = uVar9 + 1;
        } while (uVar9 < 6);
      }
    }
  }
  else {
    lbl_80478D78[5] = 4;
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
    for (uVar1 = 0; uVar1 < uVar4; uVar1 = uVar1 + 1) {
      iVar2 = fn_801F7258(uVar3,uVar1 & 0xffff);
      if (iVar2 != 0) {
        iVar6 = fn_801FB1C0(iVar2,0,0x44,0);
        uVar10 = 0;
        do {
          uVar9 = fn_801F9930(iVar2,
              fn_8012A5B0(iVar6,3,uVar10 & 0xffff));
          if ((uVar9 != 0) && (cVar8 = fn_80206A04(), cVar8 != 0)) {
            fn_80205BE8(uVar9);
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
#undef fn_801248C4
#undef fn_801252E0
#undef fn_8012A5B0
#undef fn_801F025C
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_801F7258
#undef fn_801F9930
#undef fn_801FB1C0
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80202810
#undef fn_80202998
#undef fn_80205184
#undef fn_80205B8C
#undef fn_80205BE8
#undef fn_80206780
#undef fn_80206A04
#undef fn_80207BF4
void fn_80218824(void)
{
    extern u16 fn_800E0C54();
    extern u32 GSmsgGetGSchar();
    extern u32 wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus(u32, u32, u32, u32, u32);
    extern u8 pokemonWazaCheckValid();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonInitJoutaiKeep();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    u32 outPokemon;
    u32 pokemon;
    u16 slotMove;
    u16 move;
    u8 i;
    u8 pp;
    u8 amount;
    u32 mask;

    fightTargetGetPtrAsNowFightType(0x11, 0);
    outPokemon = fightTargetGetPtrAsNowFightType(0x12, 0);
    pokemon = fightOutPokemonGetPokemonPtr(outPokemon);
    move = pokemonGetStatus(outPokemon, 0, 0xF0, 0);

    if (move != 0 && move != 0x165 && move != 0xFFFF && move != 0x164) {
        pp = 0;
        for (i = 0; i < 4; i++) {
            if (pokemonWazaCheckValid(pokemon, i) != 0) {
                slotMove = pokemonGetStatus(pokemon, 0, 0x7F, i);
                if (move == slotMove) {
                    pp = pokemonGetStatus(pokemon, 0, 0x80, i);
                    break;
                }
            }
        }

        if (i >= 4) {
            lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        } else {
            if (pp < 2) {
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
            } else {
                amount = (u8)((fn_800E0C54() % 4) + 2);
                if (pp < amount) {
                    amount = pp;
                }

                msgctrlSetValue(0xD, GSmsgGetGSchar(wazaGetStatus(0, move, 1, 0)));
                msgctrlSetValue(0x2F, amount);
                pp -= amount;
                pokemonSetStatus(pokemon, 0, 0x80, i, pp);

                if (fn_802026E4(outPokemon, 0x10) == 0 &&
                    fn_802026E4(outPokemon, 0x31) == 1) {
                    mask = fn_80201890(outPokemon, 0x31);
                    if ((mask & (1 << i)) == 0 &&
                        fightOutPokemonIsUseHensinBuff(outPokemon) == 1) {
                        fightOutPokemonSetHensinPokemonStatusId(outPokemon, 0x80, i, 0);
                    }
                }

                if (pp == 0) {
                    fightOutPokemonInitJoutaiKeep(outPokemon);
                }
                lbl_8047B610 += 5;
            }
        }
        return;
    }
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
}
#pragma optimize_for_size on
void fn_80218A6C(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void wazaSetStatus();
    u32 ctx;
    u32 move;
    u32 pokemon;
    u16 currentHp;
    u16 maxHp;
    u8 key;
    s32 offset;
    s32 i;

    ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(ctx, 0, 0xd9, 0);
    pokemon = fightOutPokemonGetPokemonPtr(ctx);
    currentHp = (u16)pokemonGetStatus(pokemon, 0, 0x83, 0);
    maxHp = (u16)pokemonGetStatus(pokemon, 0, 0x87, 0);
    key = (u8)((currentHp * 0x30) / maxHp);
    if (key == 0 && currentHp != 0) {
        key = 1;
    }

    offset = 0;
    for (i = 6; i > 0; i--) {
        if (key <= lbl_80279F7C[offset]) {
            break;
        }
        offset += 2;
    }

    wazaSetStatus(move, 0, 0x2f, 0, lbl_80279F7C[offset + 1]);
    lbl_8047B610++;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
void WS_MICHIDURE(void)

{
    extern u32 fn_801F025C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern int fn_80229934();
    extern u32 fn_80205184();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar4;

  uVar1 = fn_801F025C(0x11,0);
  uVar2 = fn_80205184((void*)uVar1);
  uVar3 = fn_801F025C(0x12,0);
  cVar4 = fn_80229934(uVar2,uVar1,uVar3);
  if (cVar4 == 1) {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
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
#undef fn_801F025C
#undef fn_80205184
#pragma optimize_for_size reset
void fn_80218D24(void)

{
    extern u32 fn_800E0C54();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern u8 pokemonWazaCheckValid();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern int fn_8022B2CC();
    extern u16 lbl_8047B60C;
    extern u32 lbl_8047B618;
  u8 bVar1;
  u16 uVar6;
  u32 uVar2;
  u32 bVar9;
  s32 sVar10;
  u32 uVar4;
  u32 uVar3;
  u8 cVar8;
  u32 uVar5;
  u16 sVar7;
  u32 initIndex;
  u32 floorTarget;
  short local_28 [4];

  uVar6 = fightFloorGetStatus(0,0,0x14,0);
  uVar2 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar3 = fightOutPokemonGetPokemonPtr();
  {
    extern u32 pokemonGetStatus();
    uVar4 = pokemonGetStatus(uVar2,0,0xd9,0);
  }
  for (initIndex = 0; (u8)initIndex < 4; initIndex = initIndex + 1) {
    local_28[(u8)initIndex] = -1;
  }
  bVar9 = 0;
  for (sVar10 = 0; (s16)sVar10 < 4; sVar10 = sVar10 + 1) {
    cVar8 = pokemonWazaCheckValid(uVar3,(u16)sVar10);
    if (cVar8 != 0) {
      cVar8 = fightOutPokemonCheckCanOutOkWazaBanme(uVar2,(u16)sVar10,0,0);
      if ((cVar8 == 0) || (cVar8 == 6)) {
        {
          extern u32 pokemonGetStatus();
          sVar7 = (u16)pokemonGetStatus(uVar3,0,0x7f,(u16)sVar10);
        }
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
            sVar7 = (u16)wazaGetStatus(0,sVar7,9,0);
            if (((((sVar7 == 0x91) || (sVar7 == 0x27)) || (sVar7 == 0x4b)) ||
                ((sVar7 == 0x97 || (sVar7 == 0x9b)))) || (sVar7 == 0x1a)) {
              bVar1 = 1;
            }
            else {
              bVar1 = 0;
            }
            if (bVar1 == 0) {
              local_28[(u8)bVar9] = (s16)sVar10;
              bVar9 = bVar9 + 1;
            }
          }
        }
      }
    }
  }
  if ((u8)bVar9 != 0) {
    goto choose_move;
  }
failed:
  lbl_8047B610 = lbl_8047B610 + 5;
  goto done;

choose_move:
  uVar5 = fn_800E0C54();
  sVar10 = *(s16 *)((int)local_28 +
                   (((uVar5 & 0xffff) -
                     ((s32)(uVar5 & 0xffff) / (s32)(u8)bVar9) * (u8)bVar9) * 2 & 0x1fe));
  if ((s16)sVar10 < 0) {
    goto failed;
  }
  {
    extern u32 pokemonGetStatus(u32, u32, u32, u16);
    sVar7 = (u16)pokemonGetStatus(uVar3,0,0x7f,(u16)sVar10);
  }
  if (sVar7 == 0 || sVar7 == 0x165 || sVar7 == 0x163) {
    goto failed;
  }
  lbl_8047B60C = sVar7;
  wazaSetStatus(uVar4,0,0x26,0,(int)sVar10);
  lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
  floorTarget = fn_8022B2CC(uVar2,lbl_8047B60C,uVar6,0,1,1, (void*)0xffffffff);
  fightFloorSetStatus(0,0,0x43,0,floorTarget);
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
done:
  return;
}
u32 fightSeqRendouWazaCheck(u16 id)
{
    if (id == 0 || id == 0x165 || id == 0xd6 || id == 0x112 || id == 0x77 ||
        id == 0x76) {
        return 1;
    }
    return 0;
}
void fn_8021908C(void)

{
    extern u32 GSmsgGetGSchar();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern s8 pokemonSearchWazaDataId();
    extern void pokemonWazaCreate();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u8 fn_802026E4();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    u32 status;
    u32 firstPokemon;
    u32 target;
    u32 secondPokemon;
    u32 invalid;
    u16 move;
    u32 attacker;
    s8 slot;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    status = pokemonGetStatus(attacker, 0, 0xd9, 0);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    move = (u16)pokemonGetStatus(target, 0, 0xef, 0);
    wazaSetStatus(status, 0, 0x27, 0, 0xffff);

    if (fn_802026E4(attacker, 0x10) == 1) {
        goto failed;
    }

    if (move == 0x164 || (u16)(move - 0xa5) <= 1 || move == 0xffff ||
        move == 0 || move == 0x165) {
        invalid = 1;
    } else {
        invalid = 0;
    }
    if ((u8)invalid == 1) {
failed:
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        goto done;
    }

    firstPokemon = pokemonGetStatus(attacker, 0, 0xd5, 0);
    secondPokemon = pokemonGetStatus(attacker, 0, 0xd6, 0);
    {
        u32 callResult = fightPokemonGetPokemonPtr(firstPokemon);
        firstPokemon = callResult;
    }
    secondPokemon = fightPokemonGetPokemonPtr(secondPokemon);
    if (pokemonSearchWazaDataId(firstPokemon, move) >= 0) {
        goto failed;
    }

    slot = (s8)wazaGetStatus(status, 0, 0x26, 0);
    if (slot < 0) {
        goto failed;
    }

    pokemonWazaCreate(firstPokemon, slot, move);
    if (fightOutPokemonIsUseHensinBuff(attacker) == 1) {
        pokemonWazaCreate(secondPokemon, slot, move);
    }
    wazaGetStatus(0, move, 1, 0);
    msgctrlSetValue(0xd, GSmsgGetGSchar());
    lbl_8047B610 += 5;
done:
    return;
}
#pragma optimize_for_size on
void fn_80219354(void)
{
    extern u32 fn_800E0C54();
    extern u32 GSmsgGetGSchar();
    extern void fn_8010C4D4();
    extern u32 wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u8 fn_802026E4();
    extern u32 fightOutPokemonGetTeikouZokuseiDataIdAry();
    extern void fightOutPokemonSetZokuseiDataId();
    extern u32 pokemonGetStatus();
    u8 specialMove;
    u16 hostSide;
    u32 pokemon;
    u16 type;
    u16 sourceType;
    u32 target;
    u32 count;
    u16 random;
    u32 selected;
    u16 i;
    u32 typeArray[18];

    hostSide = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    pokemon = fightTargetGetPtrAsNowFightType(0x11, 0);
    pokemonGetStatus(pokemon, 0, 0xd9, 0);
    type = (u16)pokemonGetStatus(pokemon, 0, 0xf3, 0);
    pokemonGetStatus(pokemon, 0, 0x102, 0);
    pokemonGetStatus(pokemon, 0, 0x104, 0);
    sourceType = (u16)pokemonGetStatus(pokemon, 0, 0xf4, 0);
    target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
        (u16)pokemonGetStatus(pokemon, 0, 0xf2, 0), hostSide);

    if (type == 0 || type == 0x165 || type == 0xffff || type == 0x164) {
        lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    } else {
        if (target != 0) {
            type = (u16)wazaGetStatus(0, type, 9, 0);
            if (type == 0x91 || type == 0x27 || type == 0x4b ||
                type == 0x97 || type == 0x9b || type == 0x1a) {
                specialMove = 1;
            } else {
                specialMove = 0;
            }
            if (specialMove && fn_802026E4(target, 0x22) == 1) {
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
                return;
            }
        }
        count = fightOutPokemonGetTeikouZokuseiDataIdAry(pokemon, sourceType, typeArray);
        if ((count & 0xffff) == 0) {
            lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
        } else {
            random = (u16)fn_800E0C54();
            selected = typeArray[(u16)(random -
                (random / (s32)(u16)count) * (u16)count)];
            if ((s32)selected < 0) {
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
            } else {
                selected &= 0xffff;
                for (i = 0; i < 2; i++) {
                    fightOutPokemonSetZokuseiDataId(pokemon, (u8)i, selected);
                }
                fn_8010C4D4(selected);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                lbl_8047B610 += 5;
            }
        }
    }
}
#pragma optimize_for_size reset
#pragma opt_common_subs off
void fn_802196A8(void)

{
    extern s8 pokemonSearchWazaDataId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fn_80201764();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  u8 bVar1;
  u32 uVar2;
  u32 uVar3;
  u16 sVar4;
  s8 cVar6;
  u8 cVar7;
  u8 cVar8;
  u16 uVar5;

  uVar2 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar3 = fightOutPokemonGetPokemonPtr();
  sVar4 = (int)pokemonGetStatus(uVar2,0,0xf0,0);
  cVar6 = pokemonSearchWazaDataId(uVar3,sVar4);
  if (cVar6 < 0) {
    cVar7 = 0;
  }
  else {
    cVar7 = (int)pokemonGetStatus(uVar3,0,0x80,(int)cVar6);
  }
  if ((((sVar4 == 0xa5) || (sVar4 == 0xe3)) || (sVar4 == 0x77)) || (sVar4 == 0xffff)) {
    bVar1 = 1;
  }
  else {
    bVar1 = 0;
  }
  if (bVar1 == 1) {
    cVar7 = 0;
    cVar6 = -1;
  }
  cVar8 = fn_802025B8(uVar2,0x2a);
  if (((cVar8 == 2) && (cVar6 >= 0)) && (cVar7 != 0)) {
    fn_8020248C(uVar2,0x2a,0);
    uVar5 = (int)pokemonGetStatus(uVar3,0,0x7f,(int)cVar6);
    fn_80201B2C(uVar2,0x2a,uVar5);
    fn_80201764(uVar2,0x2a,(int)cVar6);
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}
#pragma opt_common_subs reset
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011BEB4 wazaGetStatus
#define fn_80123B5C pokemonSearchWazaDataId
#define fn_8012640C pokemonGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#pragma optimize_for_size on
void fn_80219838(void)

{
    extern u32 fn_800FA280();
    extern void fn_8011BEB4(u32, u16, u32, u32);
    extern s8 fn_80123B5C();
    extern u32 fn_8012640C();
    extern void fn_80132A38();
    extern u32 fn_801F025C();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern s8 fn_802025B8();
    extern u32 fn_80205B8C();
  u32 uVar1;
  u32 uVar2;
  u16 uVar3;
  u16 finalMove;
  s8 cVar4;
  u8 cVar5;
  u8 cVar6;

  uVar1 = fn_801F025C(0x12,0);
  uVar2 = fn_80205B8C(uVar1);
  uVar3 = (int)fn_8012640C(uVar1,0,0xf0,0);
  cVar4 = fn_80123B5C(uVar2,uVar3);
  if (cVar4 < 0) {
    cVar5 = 0;
  }
  else {
    cVar5 = (int)fn_8012640C(uVar2,0,0x80,(int)cVar4);
  }
  cVar6 = fn_802025B8(uVar1,0x29);
  if (((cVar6 == 2) && (cVar4 >= 0)) && (cVar5 != 0)) {
    finalMove = (int)fn_8012640C(uVar2,0,0x7f,(int)cVar4);
    fn_8011BEB4(0,finalMove,1,0);
    uVar2 = fn_800FA280();
    fn_80132A38(0xd,uVar2);
    fn_8020248C(uVar1,0x29,0);
    fn_80201B2C(uVar1,0x29,finalMove);
    lbl_8047B610 += 5;
    return;
  }
  lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
  return;
}
#pragma optimize_for_size reset
#undef fn_800FA280
#undef fn_8011BEB4
#undef fn_80123B5C
#undef fn_8012640C
#undef fn_80132A38
#undef fn_801F025C
#undef fn_80205B8C
#define fn_8011BBD8 wazaSetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F00D0 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F6C4C fightSideGetJoutaiUserFightTargetId
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_80202B88 fightOutPokemonIsAlly
#define fn_802062FC fightOutPokemonCheckFightOut
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
  short sVar8;
  u16 sVar7;
  u32 uVar2;
  u32 uVar1;
  int iVar3;
  u8 cVar9;
  u32 uVar4;
  int iVar5;
  u32 iVar10;
  u16 secondFloorStatus;

  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  sVar7 = (int)fn_8012640C(uVar1,0,0x105,0);
  sVar8 = (int)fn_8012640C(uVar1,0,0x104,0);
  if ((sVar7 == 0) || (sVar8 == 0)) {
    goto fail_19964;
  }
  iVar3 = fn_801F00D0(sVar7,uVar6);
  if (iVar3 == 0) {
    goto fail_19964;
  }
  cVar9 = fn_80202B88(uVar1,iVar3);
  if (cVar9 != 0) {
    goto fail_19964;
  }
  cVar9 = fn_802062FC(iVar3);
  if (cVar9 == 1) {
    fn_8011BBD8(uVar2,0,0x2d,0,(int)sVar8 << 1);
    uVar1 = fn_801F025C(3,uVar1);
    secondFloorStatus = fn_801F54A4(0,0,0x14,0);
    iVar10 = 0;
    cVar9 = fn_801F6E98(uVar1,0x4d);
    if (cVar9 == 1) {
      uVar4 = fn_801F6C4C(uVar1,0x4d);
      if ((uVar4 & 0xffff) != 0) {
        iVar5 = fn_801F00D0(uVar4,secondFloorStatus);
        if ((iVar5 != 0) && (fn_802062FC(iVar5) == 1)) {
          iVar10 = iVar5;
        }
      }
    }
    if (iVar10 != 0) {
      iVar3 = iVar10;
    }
    fn_801F4C14(0,0,0x43,0,iVar3);
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
fail_19964:
  fn_801254B4((void*)uVar1,0,0x118,0,1);
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}
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
  short sVar8;
  u16 sVar7;
  u32 uVar2;
  u32 uVar1;
  int iVar3;
  u8 cVar9;
  u32 uVar4;
  int iVar5;
  u32 iVar10;
  u16 secondFloorStatus;

  uVar6 = fn_801F54A4(0,0,0x14,0);
  uVar1 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar1,0,0xd9,0);
  sVar7 = (int)fn_8012640C(uVar1,0,0x103,0);
  sVar8 = (int)fn_8012640C(uVar1,0,0x102,0);
  if ((sVar7 == 0) || (sVar8 == 0)) {
    goto fail_19B2C;
  }
  iVar3 = fn_801F00D0(sVar7,uVar6);
  if (iVar3 == 0) {
    goto fail_19B2C;
  }
  cVar9 = fn_80202B88(uVar1,iVar3);
  if (cVar9 != 0) {
    goto fail_19B2C;
  }
  cVar9 = fn_802062FC(iVar3);
  if (cVar9 == 1) {
    fn_8011BBD8(uVar2,0,0x2d,0,(int)sVar8 << 1);
    uVar1 = fn_801F025C(3,uVar1);
    secondFloorStatus = fn_801F54A4(0,0,0x14,0);
    iVar10 = 0;
    cVar9 = fn_801F6E98(uVar1,0x4d);
    if (cVar9 == 1) {
      uVar4 = fn_801F6C4C(uVar1,0x4d);
      if ((uVar4 & 0xffff) != 0) {
        iVar5 = fn_801F00D0(uVar4,secondFloorStatus);
        if ((iVar5 != 0) && (fn_802062FC(iVar5) == 1)) {
          iVar10 = iVar5;
        }
      }
    }
    if (iVar10 != 0) {
      iVar3 = iVar10;
    }
    fn_801F4C14(0,0,0x43,0,iVar3);
    lbl_8047B610 = lbl_8047B610 + 5;
    return;
  }
fail_19B2C:
  fn_801254B4((void*)uVar1,0,0x118,0,1);
  lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  return;
}
#undef fn_8011BBD8
#undef fn_801254B4
#undef fn_8012640C
#undef fn_801F00D0
#undef fn_801F025C
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_801F6C4C
#undef fn_801F6E98
#undef fn_80202B88
#undef fn_802062FC
#pragma dont_inline reset
static inline u32 fn_80219E10_checkMove(u16 move)
{
    extern u16 lbl_80279FA0[];
    u16 entry;
    s32 index;

    if (move == 0) {
        return 0;
    }
    if (move == 0x165) {
        return 0;
    }
    if (move == 0x163) {
        return 0;
    }
    index = 0;
    while (1) {
        entry = lbl_80279FA0[index];
        if (entry == 0xffff) {
            break;
        }
        if (move == entry) {
            break;
        }
        index++;
    }
    if (entry != 0xffff) {
        return 0;
    }
    return 1;
}
void fn_80219E10(void)

{
    extern u32 fn_8000814C();
    extern u32 fn_800E0C54();
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 pokemonGetStatus();
    extern void fightWazaSetUseWazaStatus();
    extern int fn_8022B2CC();
    extern u32 lbl_8047B618;
    extern u16 lbl_80279FA0[];
    extern u32 lbl_80379BFF[];
    u16 floorId;
    u32 attacker;
    u32 status;
    u32 count;
    u32 offset;
    u32 move;
    u32 allowed;
    u32 random;
    u32 selected;
    u32 result;
    u32 i;
    u16 local_338[404];

    floorId = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    status = pokemonGetStatus(attacker, 0, 0xd9, 0);

    for (i = 0; i < 400; i++) {
        local_338[i] = 0;
    }

    count = 0;
    offset = 0;
    for (move = 0; move < 0x163; move++) {
        if (move != 0) {
            allowed = fn_80219E10_checkMove((u16)move);
            if ((u8)allowed != 0) {
                count++;
                local_338[offset] = (u16)move;
                offset++;
            }
        }
    }

    random = (u16)fn_800E0C54();
    selected = local_338[random % count];
    result = fn_8000814C();
    if ((u16)result != 0) {
        selected = result;
    }

    lbl_8047B618 &= 0xfffffbff;
    fightWazaSetUseWazaStatus(status, selected);
    {
        u32 callResult = fn_8022B2CC(attacker, selected, floorId, 0, 1, 1,
                                     (void*)0xffffffff);
        result = callResult;
    }
    fightFloorSetStatus(0, 0, 0x43, 0, result);
    result = wazaGetStatus(0, selected, 9, 0);
    lbl_8047B610 = (u8*)lbl_80379BFF[(u16)result];
}
#pragma dont_inline on
#pragma optimize_for_size on
u32 fightSeqCheckYubiwohuruWazaDataId(u16 r3)

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
#pragma optimize_for_size reset
#pragma optimize_for_size on
void fn_8021A054(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern void wazaSetStatus();
    extern u8 fn_802026E4();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 wazaGetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u8 fightOutPokemonUseHensinBuff();
    extern void fn_80201764();
    extern u32 fn_80201890();
    extern void fn_8020248C();
    extern void pokemonWazaCreate();
    extern void pokemonSetStatus();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern u16 lbl_80279FA0[];
    u32 attacker;
    u32 pokemon;
    u32 move;
    u16 entry;
    u32 index;
    u8 invalid;
    s8 slot;
    u8 pp;
    u32 flags;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    pokemon = pokemonGetStatus(attacker, 0, 0xd9, 0);
    move = pokemonGetStatus(
        fightTargetGetPtrAsNowFightType(0x12, 0), 0, 0xf0, 0) & 0xffff;
    wazaSetStatus(pokemon, 0, 0x27, 0, 0xffff);

    if (fn_802026E4(attacker, 0x10) == 1) {
        goto advance_alt;
    }

    if (move == 0) {
        invalid = 1;
    } else if (move == 0x165) {
        invalid = 1;
    } else if (move == 0x163) {
        invalid = 1;
    } else {
        index = 0;
        while ((entry = lbl_80279FA0[index]) != 0xfffe) {
            if (move == entry) {
                break;
            }
            index++;
        }
        invalid = entry != 0xfffe;
    }

    if (invalid != 0 || move == 0 || move == 0xffff ||
        move == 0x165 || move == 0x163) {
advance_alt:
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }

    if (pokemonSearchWazaDataId(
            fightOutPokemonGetPokemonPtr(attacker), move) >= 0) {
        goto advance_alt;
    }

    slot = (s8)wazaGetStatus(pokemon, 0, 0x26, 0);
    if (slot < 0) {
        goto advance_alt;
    }

    if (fightOutPokemonIsUseHensinBuff(attacker) == 0 &&
        fightOutPokemonUseHensinBuff(attacker) == 0) {
        goto advance_alt;
    }

    if (fn_802026E4(attacker, 0x31) == 0) {
        fn_8020248C(attacker, 0x31, 0);
    }

    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    pokemonWazaCreate(pokemon, slot, move);
    pp = (u8)wazaGetStatus(0, move, 2, 0);
    if (pp > 5) {
        pp = 5;
    }
    pokemonSetStatus(pokemon, 0, 0x80, slot, pp);
    wazaGetStatus(0, move, 1, 0);
    msgctrlSetValue(0xd, GSmsgGetGSchar());
    flags = fn_80201890(attacker, 0x31);
    flags |= 1 << slot;
    fn_80201764(attacker, 0x31, flags);
    lbl_8047B610 += 5;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fightSeqMonomaneNGCheck(u16 r3)

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
#pragma optimize_for_size reset
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_8021A338(void)

{
    extern void wazaSetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fn_80201764();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonMaxHpWaruValue();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    extern u8 lbl_80478D78[1];
    extern u32 lbl_8047B618;
  u32 attacker;
  u32 move;
  u32 pokemon;
  u32 flags;
  u8 clear;
  u16 hp;
  u16 amount;

  attacker = fightTargetGetPtrAsNowFightType(0x11,0);
  pokemon = fightOutPokemonGetPokemonPtr();
  hp = pokemonGetStatus(pokemon,0,0x83,0);
  move = pokemonGetStatus(attacker,0,0xd9,0);
  amount = fightOutPokemonMaxHpWaruValue(attacker,4);
  if ((s32)hp > (s32)amount) {
    goto check_status;
  }
failed:
  wazaSetStatus(move,0,0x2d,0,0);
  lbl_80478D78[5] = 1;
  goto done;
check_status:
  if (fn_802025B8(attacker,0x14) != 2) {
    goto failed;
  }
  wazaSetStatus(move,0,0x2d,0,amount);
  fn_8020248C(attacker,0x14,0);
  fn_80201764(attacker,0x14,amount);
  if (fn_802026E4(attacker,0xe) == 1) {
    fightOutPokemonWriteJoutaiDataId(attacker,0xe);
  }
  flags = *(volatile u32*)&lbl_8047B618;
  flags |= 0x100;
  clear = 0;
  lbl_80478D78[5] = clear;
  lbl_8047B618 = flags;
done:
  lbl_8047B610++;
  return;
}
#pragma opt_propagation reset
#pragma optimize_for_size reset
#pragma opt_propagation off
#pragma opt_lifetimes off
void fn_8021A478(void)

{
    extern void wazaSetStatus();
    extern u8 pokemonIsDarkPokemon();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern void fightOutPokemonCopyHensinStatus();
    extern u8 fightOutPokemonUseHensinBuff();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u16 fightOutPokemonIsJoutaiKie();
    extern void fn_80201764();
    extern void fn_80201B2C(u32, u32, u16);
    extern void fn_80201EB0();
    extern void fn_80201FDC(u32, u32, s8);
    extern void fn_8020248C();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonGetNamePtr();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u32 uVar6;
  u8 cVar9;
  u16 sVar8;
  u32 *puVar7;
  u8 lowByte;
  u8* failurePc;
  u8* failureFlag;
  u8* failureNextPc;

  uVar1 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar2 = pokemonGetStatus(uVar1,0,0xd9,0);
  uVar3 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar4 = fightOutPokemonGetPokemonPtr();
  uVar4 = pokemonGetStatus(uVar4,0,0x6f,0);
  uVar5 = fightOutPokemonGetPokemonPtr(uVar3);
  uVar6 = pokemonGetStatus(uVar5,0,0x75,0);
  wazaSetStatus(uVar2,0,0x27,0,0xffff);
  cVar9 = fn_802026E4(uVar3,0x10);
  if (cVar9 == 1) {
    goto failed;
  }
  sVar8 = fightOutPokemonIsJoutaiKie(uVar3);
  if (sVar8 != 0) {
    goto failed;
  }
  fightOutPokemonGetPokemonPtr(uVar3);
  cVar9 = pokemonIsDarkPokemon();
  if (cVar9 != 1) {
    goto check_hensin;
  }

failed:
  fightFloorSetStatus(0,0,0x3b,0,0x45);
  failurePc = lbl_8047B610;
  failureFlag = lbl_80478D78;
  failureNextPc = failurePc + 1;
  failureFlag[5] = failureNextPc - failurePc;
  lbl_8047B610 = failureNextPc;
  goto done;

check_hensin:
  cVar9 = fightOutPokemonIsUseHensinBuff(uVar1);
  if (cVar9 != 0) {
    goto apply_hensin;
  }
  cVar9 = fightOutPokemonUseHensinBuff(uVar1);
  if (cVar9 == 0) {
    goto failed;
  }

apply_hensin:
  cVar9 = fn_802026E4(uVar1,0x10);
  if (cVar9 == 0) {
    fn_8020248C(uVar1,0x10,0);
  }
  fn_80201764(uVar1,0x10,uVar4);
  lowByte = (u8)uVar6;
  fn_80201EB0(uVar1,0x10,(s8)lowByte);
  fn_80201FDC(uVar1,0x10,(s8)(u8)(uVar6 >> 8));
  fn_80201B2C(uVar1,0x10,uVar6 >> 0x10);
  cVar9 = fn_802026E4(uVar1,0x29);
  if (cVar9 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x29);
  }
  cVar9 = fn_802026E4(uVar1,0x31);
  if (cVar9 == 1) {
    fightOutPokemonWriteJoutaiDataId(uVar1,0x31);
  }
  msgctrlSetValue(0xd,fightOutPokemonGetNamePtr(uVar3));
  lbl_80478D78[5] = 0;
  fightOutPokemonCopyHensinStatus(uVar3,uVar1);
  puVar7 = (u32 *)pokemonGetStatus(uVar1,0,0x101,0);
  if (puVar7 != (void *)0) {
    *puVar7 = 0;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
done:
  return;
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_8011BBD8 wazaSetStatus
void fn_8021A984(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern u8 fn_801F453C();
    extern u8 fn_802026E4();
    extern u32 fn_80203B5C();
    extern u8 fn_802062FC();
    extern u8 fn_80207AE0();
    extern u32 fn_80207BF4();
  u32 uVar1;
  u32 uVar2;
  u32 sVar3;
  u32 uVar4;
  u8 cVar6;
  u32 cVar5;

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
#undef fn_8011BBD8
#undef fn_80207BF4
#undef fn_80207AE0
#undef fn_802062FC
#undef fn_80203B5C
#undef fn_801F453C
#undef fn_8012640C
#undef fn_801F025C
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
void WS_WEATHER_CHANGE(void)

{
    extern void fn_801F2934();
    extern u8 fn_801F2988();
    extern void fn_801F4C14();
    extern u8 lbl_80478D78[1];
  u8 cVar1;

  cVar1 = fn_801F2988(0,0x55);
  if (cVar1 != 2) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    lbl_80478D78[5] = 2;
  }
  else {
    fn_801F2934(0,0x55,0);
    lbl_80478D78[5] = 3;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F2934
#undef fn_801F4C14
#undef fn_801F2988
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_8021AC1C(void)

{
    extern u32 fn_800E0C54();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 fn_80201D84();
    extern u8 fn_802026E4();
    extern u32 figthOutPokemonGetLevel();
    extern int figthOutPokemonGetSoubiItemBuff();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_803797A0[];
  u16 canSteal;
  u32 attacker;
  u16 floorTarget;
  u32 relativeTarget;
  u32 attackerMove;
  u16 remainingPP;
  u32 attackerStatus;
  u32 attackerLevel;
  u32 itemDataId;
  u32 itemSubDataId;
  int itemBuff;
  u32 defenderPokemon;
  u8 stealBonus;
  u32 ability;
  u32 defenderLevel;
  u16 randomValue;
  u8 condition;
  register u16 threshold;
  int levelDelta;

  attacker = fightTargetGetPtrAsNowFightType(0x11,0);
  floorTarget = fightFloorGetStatus(0,0,0x14,0);
  relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker,
      floorTarget);
  attackerMove = fightOutPokemonGetUseWazaDataId(attacker);
  attackerStatus = pokemonGetStatus(attacker,0,0xd9,0);
  attackerLevel = figthOutPokemonGetLevel(attacker);
  attacker = fightTargetGetPtrAsNowFightType(0x12,0);
  itemDataId = fightOutPokemonGetSoubiItemDataId(attacker);
  itemSubDataId = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
  itemBuff = figthOutPokemonGetSoubiItemBuff(attacker);
  defenderPokemon = fightOutPokemonGetPokemonPtr(attacker);
  remainingPP = pokemonGetStatus(defenderPokemon,0,0x83,0);
  ability = fightOutPokemonGetTokuseiDataId(attacker);
  defenderLevel = figthOutPokemonGetLevel(attacker);
  fightFloorSetStatus(0,0,0x49,0,attacker);
  if ((u16)itemSubDataId == 0x27) {
    randomValue = (u16)fn_800E0C54();
    if ((int)randomValue % 100 < itemBuff) {
      pokemonSetStatus(attacker,0,0x11a,0,1);
    }
  }
  if ((u16)ability == 5) {
    fightFloorSetStatus(0,0,0x3b,0,0x40);
    lbl_8047B610 = lbl_803797A0;
    return;
  }
  condition = fn_802026E4(attacker,0x1d);
  if ((condition == 1) &&
      ((u16)relativeTarget == (u16)fn_80201D84(attacker,0x1d))) {
    if ((u8)attackerLevel >= (u8)defenderLevel) {
      canSteal = 1;
    }
    else {
      canSteal = 0;
    }
  }
  else {
    stealBonus = wazaGetStatus(0,attackerMove,6,0);
    levelDelta = (u8)attackerLevel - (u8)defenderLevel;
    threshold = levelDelta + stealBonus;
    randomValue = (u16)fn_800E0C54();
    if (((int)randomValue % 100 + 1 < (int)threshold) &&
        ((u8)attackerLevel >= (u8)defenderLevel)) {
      canSteal = 1;
    }
    else {
      canSteal = 0;
    }
  }
  if (canSteal) {
    condition = fn_802026E4(attacker,0x2c);
    if (condition != 0) {
      wazaSetStatus(attackerStatus,0,0x2d,0,remainingPP - 1);
      fightFloorSetStatus(0,0,0x3b,0,0x46);
    }
    else {
      if ((int)pokemonGetStatus(attacker,0,0x11a,0) != 0) {
        wazaSetStatus(attackerStatus,0,0x2d,0,remainingPP - 1);
        fightFloorSetStatus(0,0,0x3b,0,0x47);
        fightFloorSetStatus(0,0,0x56,0,(u16)itemDataId);
      }
      else {
        wazaSetStatus(attackerStatus,0,0x2d,0,remainingPP);
        fightFloorSetStatus(0,0,0x3b,0,0x44);
      }
    }
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    fightFloorSetStatus(0,0,0x3b,0,0x40);
    if ((u8)attackerLevel >= (u8)defenderLevel) {
      lbl_80478D78[5] = 0;
    }
    else {
      lbl_80478D78[5] = 1;
    }
    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
  }
}
#pragma optimize_for_size reset
#pragma opt_propagation reset
#pragma opt_lifetimes off
void fn_8021B1A4(void)

{
    extern u16 fn_800E0C54();
    extern u32 GSmsgGetGSchar();
    extern void fn_8010C4D4();
    extern void msgctrlSetValue();
    extern u32 wazaGetStatus();
    extern u8 pokemonWazaCheckValid();
    extern u32 pokemonGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightOutPokemonSetZokuseiDataId();
    u32 attacker;
    u32 pokemon;
    u32 type;
    u16 move;
    u32 selected;
    u32 count;
    s32 i;
    u32 index;
    u32 candidates[4];

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    {
        u32 init_i;
        for (init_i = 0; (u8)init_i < 4; init_i++) {
            candidates[(u8)init_i] = -1;
        }
    }

    count = 0;
    for (i = count; (u8)i < 4; i++) {
        if (pokemonWazaCheckValid(pokemon, (u8)i) == 1) {
            move = (u16)pokemonGetStatus(pokemon, 0, 0x7f, (u8)i);
            if (move != 0xa5 && move != 0x164) {
                type = wazaGetStatus(0, move, 3, 0) & 0xffff;
                if (type == 9) {
                    if (fightOutPokemonIsZokuseiDataId(attacker, 7) == 1) {
                        type = 7;
                    } else {
                        type = 0;
                    }
                }
                if (fightOutPokemonIsZokuseiDataId(attacker, type) == 0) {
                    candidates[(u8)count] = (u16)type;
                    count++;
                }
            }
        }
    }

    if ((u8)count == 0) {
failed:
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        goto done;
    }

    selected = (pokemon,
        candidates[(u8)((s32)(u16)fn_800E0C54() % (s32)(u8)count)]);
    if ((s32)selected < 0) {
        goto failed;
    }

    count = selected & 0xffff;
    for (index = 0; (u8)index < 2; index++) {
        fightOutPokemonSetZokuseiDataId(attacker, index, count);
    }
    fn_8010C4D4(count);
    msgctrlSetValue(0xd, GSmsgGetGSchar());
    lbl_8047B610 += 5;
done:
    return;
}
#pragma opt_lifetimes reset
#pragma optimize_for_size on
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80123CD4 pokemonWazaCheckValid
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
u32 fn_8021B364(u32 r3, u32 r4)

{
    extern u32 fn_8011BEB4();
    extern u8 fn_80123CD4();
    extern s32 fn_8012640C();
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
#undef fn_80207AE0
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_80123CD4
#undef fn_80205B8C
#pragma optimize_for_size reset
void fn_8021B484(void)
{
    extern u32 fn_800E0C54();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorSetStatus();
    extern u32 fn_801F87CC();
    extern u32 figthOutPokemonGetLevel();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 lbl_80378CCE[];
    u32 attacker;
    u32 defender;
    u32 trainer;
    u32 count;
    u32 attackerLevel;
    u32 defenderLevel;
    u32 random;
    u32 selected;
    s16 status;
    u32 proceed;
    u32 choices[6];

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, defender);
    count = fn_801F87CC(trainer, choices);
    if ((u16)count != 0) {
        goto have_choices;
    }
failed:
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
    goto done;

have_choices:
    attackerLevel = figthOutPokemonGetLevel(attacker);
    defenderLevel = figthOutPokemonGetLevel(defender);
    if ((u8)attackerLevel < (u8)defenderLevel) {
        goto random_check;
    }
success:
    proceed = 1;
    lbl_8047B610 = lbl_80378CCE;
    goto check_proceed;

random_check:
    random = fn_800E0C54();
    if ((u16)(((s32)(((u8)attackerLevel + (u8)defenderLevel) *
                     (u8)random) >> 8) + 1) >
        (s32)((defenderLevel >> 2) & 0x3f)) {
        goto success;
    }
    proceed = 0;
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);

check_proceed:
    if ((u8)proceed == 0) {
        goto done;
    }
    random = fn_800E0C54();
    selected = choices[(u16)((s32)(u16)random % (s32)(u16)count)];
    if (selected == 0) {
        goto failed;
    }
    status = (s16)pokemonGetStatus(selected, 0, 0xce, 0);
    if (status < 0) {
        goto failed;
    }
    fightFloorSetStatus(0, 0, 0x45, 0, defender);
    pokemonSetStatus(defender, 0, 0x121, 0, status);
    pokemonSetStatus(defender, 0, 0x119, 0, 1);
done:
    return;
}
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_ABARERU(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802025B8();
    extern u8 lbl_80478D78[1];
  u32 uVar1;
  u8 cVar2;

  uVar1 = fn_801F025C(0x11,0);
  cVar2 = fn_802025B8(uVar1,0xd);
  if (cVar2 == 2) {
    lbl_80478D78[3] = 0x75;
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F025C
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_8021B870 _fightSeqWsKuroikiriSub__FPvUsPv
void WS_KUROIKIRI(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8021B870();

  fn_801F37B0(0,fn_8021B870,0,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_8021B870
#undef fn_801F37B0
#pragma optimize_for_size reset
#pragma opt_propagation off
u8 fn_8021B910(r3, r4, r5, r6)
u32 r3;
u32 r4;
u32 r5;
u32 r6;

{
    extern u32 GSmsgGetGSchar();
    extern u32 wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u8 fightSideIsJoutaiDataId();
    extern u32 pokemonGetStatus(u32,u32,u16,u32);
    extern void pokemonSetStatus(u32,u32,u16,u32,u32);
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern s8 fn_802026E4();
    extern u8 fightWazaIsHit();
    extern void fn_80211B94();
    extern void fn_8022DCB8();
    extern u32 lbl_80279E7C[];
    extern u8 lbl_80377B05[];
    extern u8 lbl_80378CEB[];
    extern u8 lbl_803797F1[];
    extern u8 lbl_803798BB[];
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
  u8 bVar1;
  u8 bVar2;
  u8 bVar3;
  u8 bVar4;
  u8 bVar5;
  u32 uVar5;
  u32 uVar6;
  u32 sVar11;
  u32 sVar12;
  s8 cVar13;
  u32 uVar7;
  u8 cVar14;
  int iVar8;
  u8 cVar15;
  u32 uVar9;
  int iVar10;
  s8 cVar16;
  u16 statusField;
  u32 uVarFlags;
  u32 uVarMasked;
  bVar3 = 0;
  bVar5 = 0;
  if ((r5 & 0x40) != 0) {
    uVar5 = fightTargetGetPtrAsNowFightType(0x11,0);
    bVar2 = 0;
  }
  else {
    uVar5 = fightTargetGetPtrAsNowFightType(0x12,0);
    bVar2 = 1;
  }
  uVar6 = fightTargetGetPtrAsNowFightType(2,uVar5);
  sVar11 = fightOutPokemonGetTokuseiDataId(uVar5);
  sVar12 = fightOutPokemonGetUseWazaDataId(uVar5);
  uVarFlags = r5 & 0xff;
  uVarMasked = uVarFlags & 0xbf;
  if ((uVarMasked & 0x80) != 0) {
    bVar3 = 1;
  }
  if ((uVarFlags & 0x20) != 0) {
    bVar5 = 1;
  }
  switch (r4 & 0xff) {
  case 1:
    statusField = 0xe6;
    break;
  case 2:
    statusField = 0xe7;
    break;
  case 3:
    statusField = 0xea;
    break;
  case 4:
    statusField = 0xe8;
    break;
  case 5:
    statusField = 0xe9;
    break;
  case 6:
    statusField = 0xeb;
    break;
  case 7:
    statusField = 0xec;
    break;
  case 0:
  default:
    statusField = 0;
    break;
  }
  cVar13 = (int)pokemonGetStatus(uVar5,0,statusField,0);
  msgctrlSetValue(0xd,GSmsgGetGSchar(lbl_80279E7C[(u8)r4]));
  switch (r3 & 0xff) {
  case 0x10:
    cVar16 = 1;
    break;
  case 0x20:
    cVar16 = 2;
    break;
  case 0x90:
    cVar16 = -1;
    break;
  case 0xa0:
    cVar16 = -2;
    break;
  default:
    cVar16 = 0;
    break;
  }
  if (cVar16 < 0) {
    cVar14 = fightSideIsJoutaiDataId(uVar6,0x4c);
    if (((cVar14 == 1) && (bVar3 == 0)) && ((u16)sVar12 != 0xae)) {
      if ((uVarFlags & 0x1f) == 1) {
        iVar8 = (int)pokemonGetStatus(uVar5,0,0x113,0);
        if (iVar8 != 0) {
          lbl_8047B610 = (u8*)r6;
        }
        else {
          fightFloorSetStatus(0,0,0x4b,0,uVar5);
          pokemonSetStatus(uVar5,0,0x113,0,1);
          fn_80211B94(lbl_8047B62C,lbl_80378CEB,0);
          lbl_8047B610 = (u8*)r6;
        }
      }
      return 1;
    }
    if (((u16)sVar12 == 0xae) || (bVar5 == 1)) {
    }
    else {
      uVar6 = fightTargetGetPtrAsNowFightType(0x11,0);
      uVar6 = fightOutPokemonGetUseWazaDataId(uVar6);
      uVar7 = fightTargetGetPtrAsNowFightType(0x12,0);
      bVar1 = 0;
      cVar14 = wazaGetStatus(0,uVar6,0xe,0);
      cVar15 = fn_802026E4(uVar7,0x2b);
      if ((cVar15 == 1) && (cVar14 == 1)) {
        bVar4 = 1;
      }
      else {
        bVar4 = 0;
      }
      if (bVar4 == 1) {
        fightFloorSetStatus(0,0,0x3b,0,0x40);
        uVar9 = fightTargetGetPtrAsNowFightType(0x11,0);
        uVar7 = pokemonGetStatus(uVar9,0,0xd9,0);
        uVar6 = fightTargetGetPtrAsNowFightType(0x12,0);
        cVar14 = fightWazaIsHit(uVar7);
        if (cVar14 == 0) {
          pokemonSetStatus(uVar6,0,0xf3,0,0);
          pokemonSetStatus(uVar6,0,0xf4,0,9);
          lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        }
        else {
          uVar7 = fightTargetGetPtrAsNowFightType(0x11,0);
          iVar8 = fightTargetGetPtrAsNowFightType(2,uVar7);
          uVar7 = fightTargetGetPtrAsNowFightType(0x12,0);
          iVar10 = fightTargetGetPtrAsNowFightType(2,uVar7);
          cVar14 = fn_802026E4(uVar7,0x15);
          if (((cVar14 == 1) && ((u32)iVar8 != (u32)iVar10)) &&
             ((lbl_8047B618 & 0x1000000) == 0)) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
          fn_8022DCB8(uVar9,uVar6,0);
        }
        bVar1 = 1;
        lbl_80478D78[6] = bVar1;
      }
      if (bVar1) {
        lbl_8047B610 = lbl_80377B05;
        return 1;
      }
    }
    if (((((u16)sVar11 == 0x1d) || ((u16)sVar11 == 0x49)) &&
         (bVar3 == 0)) && ((u16)sVar12 != 0xae)) {
      if ((uVarFlags & 0x1f) == 1) {
        iVar8 = (int)pokemonGetStatus(uVar5,0,0x113,0);
        if (iVar8 != 0) {
          lbl_8047B610 = (u8*)r6;
        }
        else {
          fightFloorSetStatus(0,0,0x4b,0,uVar5);
          pokemonSetStatus(uVar5,0,0x113,0,1);
          fn_80211B94(lbl_8047B62C,lbl_803797F1,0);
          lbl_8047B610 = (u8*)r6;
        }
      }
      return 1;
    }
    if (((u16)sVar11 == 0x33) && (bVar3 == 0) && (statusField == 0xeb)) {
      if ((uVarFlags & 0x1f) == 1) {
        fightFloorSetStatus(0,0,0x4b,0,uVar5);
        fn_80211B94(lbl_8047B62C,lbl_803798BB,0);
        lbl_8047B610 = (u8*)r6;
      }
      return 1;
    }
    if (((u16)sVar11 == 0x34) && (bVar3 == 0) && (statusField == 0xe6)) {
      if ((uVarFlags & 0x1f) == 1) {
        fightFloorSetStatus(0,0,0x4b,0,uVar5);
        fn_80211B94(lbl_8047B62C,lbl_803798BB,0);
        lbl_8047B610 = (u8*)r6;
      }
      return 1;
    }
    if (((u16)sVar11 == 0x13) && ((uVarFlags & 0x1f) == 0)) {
      return 1;
    }
    if (cVar16 <= -2) {
      uVar6 = GSmsgGetGSchar(0x7628);
      msgctrlSetValue(0xe,uVar6);
    }
    else {
      uVar6 = GSmsgGetGSchar(0x76bd);
      msgctrlSetValue(0xe,uVar6);
    }
    uVar6 = GSmsgGetGSchar(0x7629);
    msgctrlSetValue(0x41,uVar6);
    if (cVar13 <= 0) {
      lbl_80478D78[5] = 2;
    }
    else {
      lbl_80478D78[5] = bVar2;
    }
  }
  else {
    if (cVar16 >= 2) {
      uVar6 = GSmsgGetGSchar(0x7626);
      msgctrlSetValue(0xe,uVar6);
    }
    else {
      uVar6 = GSmsgGetGSchar(0x76bd);
      msgctrlSetValue(0xe,uVar6);
    }
    uVar6 = GSmsgGetGSchar(0x7627);
    msgctrlSetValue(0x41,uVar6);
    if (cVar13 >= 12) {
      lbl_80478D78[5] = 2;
    }
    else {
      lbl_80478D78[5] = bVar2;
    }
  }
  cVar13 = cVar13 + cVar16;
  if (cVar13 < 0) {
    cVar13 = 0;
  }
  if ('\f' < cVar13) {
    cVar13 = '\f';
  }
  pokemonSetStatus(uVar5,0,statusField,0,(int)cVar13);
  if ((lbl_80478D78[5] == 2) && ((uVarFlags & 1) != 0)) {
    fightFloorSetStatus(0,0,0x3b,0,0x40);
  }
  if ((lbl_80478D78[5] == 2) && ((uVarFlags & 1) == 0)) {
    return 1;
  }
  return 0;
}
#pragma opt_propagation reset
#pragma optimize_for_size on
u32 fn_8021C638(u32 r3, u32 r4, u32 r5)

{
    extern void fightFloorSetStatus();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
  u8 cVar3;
  u32 originalR3 = r3;

  r3 = fightOutPokemonGetTokuseiDataId(r5);
  cVar3 = fightOutPokemonCheckFightOut(originalR3);
  if (cVar3 == 0) {
    return 1;
  }
  cVar3 = fn_802026E4(originalR3,0xb);
  if ((cVar3 == 1) && ((u16)r3 != 0x2b)) {
    fightFloorSetStatus(0,0,0x4b,0,originalR3);
    if (originalR3 == r5) {
      lbl_80478D78[5] = 0;
    }
    else {
      lbl_80478D78[5] = 1;
    }
    return 0;
  }
  return 1;
}
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void WS_NEKODAMASHI(void)

{
    extern u32 fn_801F025C();
    extern s32 fn_8012640C();
  u32 uVar2;
  u16 sVar3;

  uVar2 = fn_801F025C(0x11,0);
  sVar3 = (int)fn_8012640C(uVar2,0,0xed,0);
  if (sVar3 != 0) {
    lbl_8047B610 = lbl_8047B610 + 5;
  }
  else {
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
  }
  return;
}
#undef fn_8012640C
#undef fn_801F025C
#pragma optimize_for_size on
void fn_8021CA00(void)

{
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u32 pokemonGetStatus();
    extern u8 fightWazaIsHit();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    u32 fightTarget;
    u32 move;
    u32 relativeTarget;
    u16 floorId;
    u8* pc;

    fightTarget = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = pokemonGetStatus(fightTarget, 0, 0xd9, 0);
    floorId = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(fightTarget, floorId);
    {
        u32 callResult = fightTargetGetPtrAsNowFightType(0x12, 0);
        fightTarget = callResult;
    }

    if (fightWazaIsHit(move) == 0 || fn_802025B8(fightTarget, 0x1c) != 2) {
        fightFloorSetStatus(0, 0, 0x3b, 0, 0x40);
        pc = lbl_8047B610;
        lbl_8047B610 = pc + (lbl_80478D78[5] = 1);
    } else if (fightOutPokemonIsZokuseiDataId(fightTarget, 0xc) == 1) {
        fightFloorSetStatus(0, 0, 0x3b, 0, 0x40);
        pc = lbl_8047B610;
        lbl_8047B610 = pc + 1;
        lbl_80478D78[5] = 2;
    } else {
        fn_8020248C(fightTarget, 0x1c, relativeTarget);
        pc = lbl_8047B610;
        lbl_8047B610 = pc + 1;
        lbl_80478D78[5] = 0;
    }
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80209C1C fightWazaSetUseWazaStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_80201248 fightOutPokemonGetFightOutPokemonEnemyOumuWazaDataIdAry
#define fn_801254B4 pokemonSetStatus
void fn_8021CCE0(void)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern void fn_801254B4();
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
    lbl_8047B610 = (u8*)lbl_80379BFF[(u16)uVar3];
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
    fn_801254B4((void*)uVar1,0,0x118,0,1);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80209C1C
#undef fn_801F4C14
#undef fn_8011BEB4
#undef fn_80201248
#undef fn_801254B4
#pragma optimize_for_size reset
void fn_8021CF3C(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
    u32 attacker;
    u32 candidate;
    u32 target;
    u32 branch;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    branch = *(u32*)(lbl_8047B610 + 1);
    lbl_8047B648++;
    target = 0;
    while (lbl_8047B648 < lbl_8047B649) {
        candidate = fightFloorGetStatus(0, 0, 0x5d, lbl_8047B648);
        if (candidate != 0 && fightOutPokemonCheckFightOut(candidate) != 0 &&
            attacker != candidate) {
            target = candidate;
            break;
        }
        lbl_8047B648++;
    }
    if (target != 0) {
        fightFloorSetStatus(0, 0, 0x43, 0, target);
        lbl_8047B610 = (u8*)branch;
    } else {
        lbl_8047B610 += 5;
    }
}
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_802656AC fightMenuGetFightOutPokemonPtrToStatusMenuId
#define fn_801F349C fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
#define fn_80011E68 menuFightStatusSetHP
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
void fn_8021D090(void)
{
    extern void fn_80011E68();
    extern void fn_8011BBD8();
    extern u32 fn_801F025C();
    extern u32 fn_801F349C();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_80205B8C();
    extern u8 fn_802062FC();
    extern u32 fn_802656AC();
    extern u8 lbl_803797BB[];
    extern u8 lbl_8047B648;
    extern u8 lbl_8047B649;
    u16 floorStatus;
    u32 attacker;
    u32 wazaData;
    u32 statusMenu;
    u32 special;
    u32 pokemon;
    u32 candidate;
    u32 current;

  floorStatus = fn_801F54A4(0,0,0x14,0);
  attacker = fn_801F025C(0x11,0);
  wazaData = (int)fn_8012640C(attacker,0,0xd9,0);
  statusMenu = fn_802656AC(attacker,floorStatus,1);
  special = fn_801F349C(0,6,0,0,0);
  if (special == 0) {
    pokemon = fn_80205B8C(attacker);
    fn_8011BBD8(wazaData,0,0x2d,0,
                 (u16)fn_8012640C(pokemon,0,0x83,0));
    fn_80011E68(statusMenu,0);
    lbl_8047B649 = 8;
    candidate = 0;
    lbl_8047B648 = 0;
    while (lbl_8047B648 < lbl_8047B649) {
      current = fn_801F54A4(0,0,0x5d,lbl_8047B648);
      if (current != 0 && fn_802062FC(current) != 0 &&
          attacker != current) {
        candidate = current;
        break;
      }
      lbl_8047B648++;
    }
    if (candidate != 0) {
      fn_801F4C14(0,0,0x43,0,candidate);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  else {
    fn_801F4C14(0,0,0x42,0,special);
    lbl_8047B610 = lbl_803797BB;
  }
}
#undef fn_801F4C14
#undef fn_802062FC
#undef fn_80011E68
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_801F349C
#undef fn_802656AC
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F54A4
#pragma optimize_for_size on
void fn_8021D40C(void)
{
    extern u32 fightFloorGetStatus();
    extern void fightFloorSetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fightFloorGetFightPokemonPtrToFightTrainerPtr();
    extern u32 fightTrainerCheckFightPokemonFightOut();
    extern u8 fightSideIsJoutaiDataId();
    extern u32 fightSideGetJoutaiUserFightTargetId();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightOutPokemonInitJoutaiKeep();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fn_802026E4();
    extern s32 fn_80201C58();
    extern s8 pokemonSearchWazaDataId();
    extern s32 pokemonGetStatus();
    extern void fn_80265598();
    u16 floorStatus;
    u32 ctx;
    u32 moveId;

    floorStatus = fightFloorGetStatus(0, 0, 0x14, 0);
    ctx = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
    moveId = fightOutPokemonGetUseWazaDataId(ctx);

    switch (lbl_8047B610[2]) {
    case 0:
        fightOutPokemonInitJoutaiKeep(ctx);
        break;
    case 1: {
        u32 defender;
        u32 side;
        u32 redirected;
        u32 targetId;

        fightTargetGetPtrAsNowFightType(0x11, 0);
        defender = fightTargetGetPtrAsNowFightType(0x12, 0);
        fightFloorSetStatus(0, 0, 0x36, 0, defender);
        side = fightTargetGetPtrAsNowFightType(3, defender);
        floorStatus = fightFloorGetStatus(0, 0, 0x14, 0);
        redirected = 0;
        if (fightSideIsJoutaiDataId(side, 0x4d) == 1) {
            targetId = fightSideGetJoutaiUserFightTargetId(side, 0x4d);
            if ((u16)targetId != 0) {
                u32 target;

                target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                    targetId, floorStatus);
                if (target != 0 && fightOutPokemonCheckFightOut(target) == 1) {
                    redirected = target;
                }
            }
        }
        if (redirected == 0) {
            redirected = ctx;
        }
        fightFloorSetStatus(0, 0, 0x43, 0, redirected);
        break;
    }
    case 3: {
        u32 target;

        target = fn_8022B2CC(ctx, moveId, floorStatus, 0, 1, 1, -1);
        if (target != 0) {
            fightFloorSetStatus(0, 0, 0x43, 0, target);
        }
        break;
    }
    case 4:
        if ((u8)pokemonGetStatus(ctx, 0, 0x120, 0) == 1) {
            lbl_80478D78[0] = 1;
        } else {
            lbl_80478D78[0] = 0;
        }
        break;
    case 6: {
        u32 pokemon;
        u32 trainer;
        u32 outPokemon;

        pokemon = (u32)lbl_8047B64C;
        if (pokemon == 0) {
            break;
        }
        trainer = fightFloorGetFightPokemonPtrToFightTrainerPtr(0, pokemon);
        if (trainer == 0) {
            break;
        }
        outPokemon = fightTrainerCheckFightPokemonFightOut(trainer, pokemon);
        if (outPokemon == 0) {
            break;
        }
        if (fn_802026E4(outPokemon, 0x36) == 1) {
            s32 otherMove;

            otherMove = fn_80201C58(outPokemon, 0x36);
            if (pokemonSearchWazaDataId(
                    fightOutPokemonGetPokemonPtr(outPokemon), otherMove) < 0) {
                fightOutPokemonWriteJoutaiDataId(outPokemon, 0x36);
            }
        }
        break;
    }
    case 7:
        fn_80265598(ctx, floorStatus, 1);
        break;
    }

    lbl_8047B610 += 3;
}
#pragma optimize_for_size reset
void fn_8021D688(void)

{
    typedef struct ItemEffectRecord {
        s32 type;
        s16 value;
        s16 padding;
    } ItemEffectRecord;
    extern void wazaSetStatus();
    extern u8 itemParamGetHPUp();
    extern u8 itemParamGetConfuseFlag();
    extern u8 itemParamGetParalyzeFlag();
    extern u8 itemParamGetFreezeFlag();
    extern u8 itemParamGetBurnFlag();
    extern u8 itemParamGetPoisonFlag();
    extern u8 itemParamGetSleepFlag();
    extern u8 itemParamGetGuardFlag();
    extern u8 itemParamGetSpAttackUp();
    extern u8 itemParamGetHitUp();
    extern u8 itemParamGetQuickUp();
    extern u8 itemParamGetDefenceUp();
    extern u8 itemParamGetAttackUp();
    extern u8 itemParamGetCriticalFlag();
    extern u32 itemParamGetPtr();
    extern u32 itemDataBiosGetItemEffectParam();
    extern u32 itemDataBiosGetPtr();
    extern s32 fn_80144574();
    extern void fn_801DA36C();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
  struct {
    u32 value;
  } context;
  u32 waza;
  u32 fightPokemon;
  u32 pokemon;
  u16 savedStatus;
  u16 item;
  u32 itemParam;
  u8 category;
  u32 effectTarget;
  s32 effectCount;
  struct {
    s32 value;
  } amount;
  u16 i;
  ItemEffectRecord effects[33];

  amount.value = 0;
  context.value = fightTargetGetPtrAsNowFightType(0x11,0);
  waza = pokemonGetStatus(context.value,0,0xd9,0);
  fightPokemon = pokemonGetStatus(context.value,0,0xd6,0);
  pokemon = pokemonGetStatus(context.value,0,0xd5,0);
  fightPokemon = fightPokemonGetPokemonPtr(fightPokemon);
  savedStatus = pokemonGetStatus(fightPokemon,0,0x83,0);
  item = itemGetStatus(pokemonGetStatus(context.value,0,0xe5,0),0,0x1e,0);
  itemParam = itemDataBiosGetPtr(item);
  itemParam = itemDataBiosGetItemEffectParam(itemParam);
  itemParam = itemParamGetPtr(itemParam);
  if (itemParam == 0) {
    category = 7;
  } else if (item == 0x13) {
    category = 1;
  } else if (itemParamGetHPUp(itemParam) != 0) {
    category = 2;
  } else if (itemParamGetSleepFlag(itemParam) == 1 ||
             itemParamGetPoisonFlag(itemParam) == 1 ||
             itemParamGetBurnFlag(itemParam) == 1 ||
             itemParamGetFreezeFlag(itemParam) == 1 ||
             itemParamGetParalyzeFlag(itemParam) == 1 ||
             itemParamGetConfuseFlag(itemParam) == 1) {
    category = 3;
  } else if (itemParamGetCriticalFlag(itemParam) == 1) {
    category = 4;
  } else if (itemParamGetAttackUp(itemParam) != 0 ||
             itemParamGetDefenceUp(itemParam) != 0 ||
             itemParamGetQuickUp(itemParam) != 0 ||
             itemParamGetHitUp(itemParam) != 0 ||
             itemParamGetSpAttackUp(itemParam) != 0) {
    category = 5;
  } else if (itemParamGetGuardFlag(itemParam) == 1) {
    category = 6;
  } else {
    category = 7;
  }
  effectTarget = pokemonGetStatus(context.value,0,0xee,0);
  effectCount = fn_80144574(effects,0,pokemon,item,0);
  if ((category == 1) || (category == 2)) {
    for (i = 0; (s32)i < (s16)effectCount; i++) {
      if (effects[i].type == 0x15) {
        amount.value = effects[i].value;
      }
    }
    pokemonSetStatus(fightPokemon,0,0x83,0,savedStatus);
    amount.value = -amount.value;
    wazaSetStatus(waza,0,0x2d,0,amount.value);
  }
  for (i = 0; (s32)i < (s16)effectCount; i++) {
    if (effects[i].type == 0xf) {
      fn_801DA36C(effectTarget,2);
    }
    if (effects[i].type == 9) {
      fn_801DA36C(effectTarget,1);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
void WS_SCA_END_SET(void)

{
    extern void fn_801F37B0();
    extern u32 fn_8021DF3C();

  fn_801F37B0(0,fn_8021DF3C,0,0);
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#undef fn_801F37B0
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
void fn_8021E04C(void)

{
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 fn_8012640C();
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
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_8012640C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#pragma opt_propagation off
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_80209C1C fightWazaSetUseWazaStatus
#define fn_8011BBD8 wazaSetStatus
void fn_8021E600(void)

{
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_8012640C();
    extern u32 fn_801F025C();
    extern void fn_801F54A4();
    extern void fn_80209C1C();
    extern u16 lbl_8047B60C;
    extern u32 lbl_80379BFF[];
  u32 uVar2;
  u32 uVar1;
  u16 uVar3;
  u16 statusValue;

  fn_801F54A4(0,0,0x14,0);
  uVar2 = fn_801F025C(0x11,0);
  uVar2 = (int)fn_8012640C(uVar2,0,0xd9,0);
  statusValue = lbl_8047B60C;
  uVar1 = statusValue;
  uVar3 = fn_8011BEB4(0,uVar1,9,0);
  if (*(char *)(lbl_8047B610 + 1) == 0) {
    goto zero_case;
  }
  fn_80209C1C(uVar2,uVar1);
  goto done;
zero_case:
  fn_8011BBD8(uVar2,0,0x27,0,uVar1 & 0xffff);
  fn_80209C1C(uVar2,uVar1);
done:
  lbl_8047B610 = (u8*)lbl_80379BFF[((s16)uVar3) & 0xffff];
  return;
}
#undef fn_801F54A4
#undef fn_801F025C
#undef fn_8012640C
#undef fn_8011BEB4
#undef fn_80209C1C
#undef fn_8011BBD8
#pragma opt_propagation reset
#pragma optimize_for_size reset
#define fn_801F22D8 fightFloorReplaceFightOutPokemonAttackToDefense
void WS_SWITCH_A_D(void)

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
#undef fn_801F22D8
#pragma optimize_for_size on
void fn_8021EAE8(void)

{
    extern u32 GSmsgGetGSchar();
    extern u8 wazaGetStatus(u32, u16, u32, u32);
    extern void pokemonWazaCreate(u32, u16, u16);
    extern void msgctrlSetValue();
    extern u32 fightPokemonGetPokemonPtr();
    extern s32 fightMenuWazaWasure();
    extern u8 fightMenuYesNo();
    extern void fightMenuCloseMsg();
    extern u8 fightMenuOpenMsg();
    extern u32 pokemonGetStatus(u32, u32, u32, u16);
    extern u16 lbl_8047B61C;
    extern void* lbl_8047B64C;
  u32 pokemon;
  s32 choice;
  struct {
    u16 value;
  } move;
  u32 text;

  pokemon = fightPokemonGetPokemonPtr(lbl_8047B64C);
  if (fightMenuYesNo() == 1) {
    while (1) {
      fightMenuCloseMsg();
      choice = fightMenuWazaWasure(pokemon,lbl_8047B61C);
      if (choice == -1 || choice >= 4) {
        break;
      }
      move.value = pokemonGetStatus(pokemon,0,0x7f,choice);
      if (wazaGetStatus(0,move.value,0x19,0) == 1) {
        if (fightMenuOpenMsg(0x7635) == 1) {
          fightMenuCloseMsg();
        }
      } else {
        pokemonWazaCreate(pokemon,choice,lbl_8047B61C);
        wazaGetStatus(0,move.value,1,0);
        text = GSmsgGetGSchar();
        msgctrlSetValue(0xe,text);
        msgctrlSetValue(0x5d,0x468);
        lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
        return;
      }
    }
  }
  lbl_8047B610 += 5;
  return;
}
#pragma optimize_for_size reset
#define fn_80123584 pokemonGetOboeWazaDataBanme
#define fn_8012361C pokemonOboeWaza
#define fn_801236F8 pokemonGetOboeWazaDataId
#define fn_8012640C pokemonGetStatus
#define fn_80203E7C figthPokemonGetLevel
#define fn_80205BE8 fightPokemonGetPokemonPtr
#pragma optimize_for_size on
void WS_WAZAOBOE_CHECK(void)

{
    extern u32 fn_80123584();
    extern s8 fn_8012361C();
    extern u32 fn_801236F8();
    extern u32 fn_80203E7C(u32);
    extern u32 fn_80205BE8();
    extern u16 lbl_8047B61C;
    extern u8 lbl_8047B642;
    extern void* lbl_8047B64C;
  u32 uVar8;
  u32 uVar7;
  u32 pokemon;
  u32 uVar2;
  u32 uVar1;
  u8 uVar4;
  s8 cVar5;
  u16 uVar3;
  u8* pc;

  pokemon = (u32)lbl_8047B64C;
  uVar2 = fn_80203E7C((u32)lbl_8047B64C);
  uVar1 = fn_80205BE8(pokemon);
  pc = lbl_8047B610;
  uVar8 = *(u32 *)(pc + 1);
  uVar7 = *(u32 *)(pc + 5);
  if (*(char *)(pc + 9) != 0) {
    uVar4 = fn_80123584(uVar1,uVar2);
    lbl_8047B642 = uVar4;
  }
  while (1) {
    cVar5 = fn_8012361C(uVar1,uVar2,0,&lbl_8047B642);
    if (cVar5 == -1) {
      uVar3 = fn_801236F8(uVar1,uVar2,&lbl_8047B642);
      lbl_8047B61C = uVar3;
      lbl_8047B610 += 10;
      return;
    }
    if (cVar5 != -2) break;
    lbl_8047B642 = (char)lbl_8047B642 + 1;
  }
  if (cVar5 == -3) {
    lbl_8047B610 = (u8*)uVar7;
    return;
  }
  lbl_8047B642 = (char)lbl_8047B642 + 1;
  uVar3 = (int)fn_8012640C(uVar1,0,0x7f,cVar5);
  lbl_8047B61C = uVar3;
  lbl_8047B610 = (u8*)uVar8;
  return;
}
#pragma optimize_for_size reset
#undef fn_80123584
#undef fn_8012361C
#undef fn_801236F8
#undef fn_8012640C
#undef fn_80203E7C
#undef fn_80205BE8
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_801F3178 fightFloorSetMeetEnemyFightPokemonEnemySideAll
#define fn_801F3074 fightFloorRegistFightTrainerEnemyPokemonFightSideAll
#define fn_801254B4 pokemonSetStatus
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_801F75F8 fightSideSetStatus
#define fn_801F6D9C fightSideGetCountAsJoutaiDataId
#define fn_80203B5C fightOutPokemonMaxHpWaruValue
#define fn_8012640C pokemonGetStatus
#define fn_8020A2B8 fightWazaBiosCopy
#define fn_8011BBD8 wazaSetStatus
#define fn_80209FAC fightWazaInitJoutai
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F2F3C fightFloorCreateFightPokemonEnemyAryEnemySideAll
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
    extern u32 fn_80207BF4();
    extern void fn_801254B4(void*, u32, u32, u32, u32);
    extern s32 fn_8012640C();
    extern void* lbl_8047B62C;
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
  fn_801254B4((void*)uVar2,0,0x119,0,0);
  fn_801254B4((void*)uVar2,0,0x121,0,0xffffffff);
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
  fn_801254B4((void*)uVar2,0,0xf9,0,1);
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
#undef fn_801F025C
#undef fn_80207BF4
#undef fn_801F3178
#undef fn_801F3074
#undef fn_801254B4
#undef fn_801F6E98
#undef fn_80207AE0
#undef fn_801F75F8
#undef fn_801F6D9C
#undef fn_80203B5C
#undef fn_8012640C
#undef fn_8020A2B8
#undef fn_8011BBD8
#undef fn_80209FAC
#undef fn_801F4C14
#undef fn_801F2F3C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_8012640C pokemonGetStatus
#define fn_801F9600 fightTrainerSelectIrekaeFightPokemon
#define fn_801F150C fightFloorSetTuusinErrorFightResult
#define fn_801F4C14 fightFloorSetStatus
#define fn_801254B4 pokemonSetStatus
void fn_8021F24C(void)

{
    extern u32 fn_801F025C();
    extern void fn_801F150C();
    extern u32 fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern int fn_801F8A18();
    extern short fn_801F9600();
  u32 uVar2;
  u32 uVar7;
  u32 uVar1;
  u16 uVar4;
  u8 cVar6;
  u32 iVar3;
  short sVar5;

  u8 targetType;
  u16 local_18 [4];

  uVar4 = fn_801F54A4(0,0,0x14,0);
  targetType = *(volatile u8 *)(lbl_8047B610 + 1);
  uVar7 = *(volatile u32 *)(lbl_8047B610 + 2);
  uVar1 = fn_801F025C(targetType,0);
  uVar2 = fn_801F4354(0,uVar1);
  cVar6 = (int)fn_8012640C(uVar1,0,0x119,0);
  if (cVar6 == 1) {
    lbl_8047B610 = lbl_8047B610 + 6;
  }
  else {
    local_18[0] = 0;
    iVar3 = fn_801F8A18(uVar2,local_18);
    if (iVar3 == 0) {
      lbl_8047B610 = (u8*)uVar7;
    }
    else {
      sVar5 = fn_801F9600(uVar2,uVar4,0,uVar1);
      if (sVar5 < 0) {
        if (sVar5 == -2) {
          fn_801F150C(0);
        }
        lbl_8047B610 = (u8*)uVar7;
      }
      else {
        fn_801F4C14(0,0,0x45,0,uVar1);
  fn_801254B4((void*)uVar1,0,0x121,0,(int)sVar5);
  fn_801254B4((void*)uVar1,0,0x119,0,1);
        lbl_8047B610 = lbl_8047B610 + 6;
      }
    }
  }
  return;
}
#undef fn_801254B4
#undef fn_801F4C14
#undef fn_801F150C
#undef fn_801F9600
#undef fn_8012640C
#undef fn_801F4354
#undef fn_801F025C
#undef fn_801F54A4
#pragma optimize_for_size reset
void fn_8021F664(void)
{
    extern void fn_8010AE2C();
    extern u32 fn_80232D28();
    extern u32 pokemonCreateSequence();
    extern void msgctrlSetValue();
    extern void battleGridUpdate();
    extern void battleGridAddPokemon();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightTrainerGetStatus();
    extern u32 fightTrainerGetEntryIdToFightPokemonPtr();
    extern void fightTrainerBallThrowEffect();
    extern u32 pokemonGetStatus();
    extern void fightOutPokemonCopyAllAbiCnt();
    extern void fn_802019BC();
    extern u8 fn_802026E4();
    extern void fightPokemonWriteJoutaiDataId();
    extern u8 fightPokemonCheckWriteJoutaiDataId();
    extern u32 fightOutPokemonGetNicknamePtr();
    extern void fn_80204970();
    extern u32 fightPokemonGetPokemonPtr();
    extern void fightOutPokemonCreate();
    extern void fightOutPokemonRegWzxLoad();
    extern void fightOutPokemonDasuEffect();
    struct AbilityList {
        u16 values[11];
    };
    extern const struct AbilityList lbl_80279FE0;
    u32 context;
    u32 trainerPokemon;
    u32 trainer;
    u8 scriptMode;
    u8 targetId;
    u16 copyMode;
    u8 i;
    s16 entryId;
    u32 ability;
    u32 pokemon;
    u32 sequence;
    u8 hasAbility;
    u32 gridPokemon;
    u32 callbackData[2];

    context = fightTargetGetPtrAsNowFightType(
        (targetId = lbl_8047B610[1], scriptMode = lbl_8047B610[2], targetId), 0);
    entryId = pokemonGetStatus(context, 0, 0x121, 0);
    pokemon = pokemonGetStatus(context, 0, 0xD5, 0);
    trainer = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, context);
    trainerPokemon = fightTrainerGetStatus(trainer, 0, 0x47, 0);

    if (fightPokemonCheckWriteJoutaiDataId(pokemon, 4) == 1) {
        fightPokemonWriteJoutaiDataId(pokemon, 4, 1);
    }

    gridPokemon = fightTrainerGetEntryIdToFightPokemonPtr(trainer, entryId);
    fn_80204970(gridPokemon, pokemon);
    fn_8010AE2C(pokemon, 0, 0);
    fightPokemonGetPokemonPtr(pokemon);
    sequence = pokemonCreateSequence();
    fightOutPokemonCreate(context, pokemon, sequence);
    fightOutPokemonRegWzxLoad(context);

    {
        struct AbilityList abilities;
        u32 first;
        u32 second;
        const u32* abilitySource = (const u32*)&lbl_80279FE0 - 1;
        u32* abilityDestination = (u32*)&abilities - 1;
        u32 copyIndex;

        for (copyIndex = 0; copyIndex < 2; copyIndex++) {
            first = abilitySource[2];
            second = abilitySource[1];
            abilitySource += 2;
            abilityDestination += 2;
            abilityDestination[-1] = second;
            abilityDestination[0] = first;
        }
        copyMode = 0;
        *(volatile u32*)(abilityDestination + 1) =
            *(volatile const u32*)(abilitySource + 1);
        *(volatile u16*)(abilityDestination + 2) =
            *(volatile const u16*)(abilitySource + 2);

        if (scriptMode == 1) {
            copyMode = 0x7F;
        }
        if (copyMode == 0x7F) {
            fightOutPokemonCopyAllAbiCnt(context, trainerPokemon);
            for (i = 0; i < 11; i++) {
                ability = abilities.values[i];
                hasAbility = fn_802026E4(trainerPokemon, ability);
                if (hasAbility == 1) {
                    fn_802019BC(context, trainerPokemon, ability);
                }
            }
        }
    }

    callbackData[0] = context;
    callbackData[1] = copyMode;
    fightFloorLoopValidFightOutPokemon(0, fn_80232D28, callbackData, 0);

    if (fn_802026E4(trainerPokemon, 0x34) == 1) {
        fn_802019BC(context, trainerPokemon, 0x34);
    }
    if (fn_802026E4(trainerPokemon, 0x35) == 1) {
        fn_802019BC(context, trainerPokemon, 0x35);
    }

    msgctrlSetValue(0xD, fightOutPokemonGetNicknamePtr(context));
    {
    u32 addedPokemon = fightTrainerGetStatus(trainer, 0, 0x4C, 0);
    if (addedPokemon != 0) {
        sequence = pokemonGetStatus(context, 0, 0xEE, 0);
        if (sequence != 0) {
            battleGridAddPokemon(addedPokemon, sequence);
        }
    }
    }
    if (scriptMode != 2) {
        fightTrainerBallThrowEffect(trainer, context, 0);
    }
    fightOutPokemonDasuEffect(context, 0);
    battleGridUpdate();
    if (scriptMode != 2) {
        fightTrainerBallThrowEffect(trainer, context, 1);
    }
    lbl_8047B610 += 3;
}
void fn_8021FAD4(void)
{
    extern u32 fightFloorGetStatus();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightFloorReplaceFightOutPokemonAttackToDefense();
    extern void fightFloorSetStatus();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u16 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fightOutPokemonCheckNoAttackFlag();
    extern u8 fightOutPokemonCheckValid();
    extern u8 fightOutPokemonIsAlly();
    extern u16 fightOutPokemonIsJoutaiKie();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u16 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetMotoWazaDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fightOutPokemonInitJoutaiKie();
    extern void fightOutPokemonSetVisibility();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonSetOumuWazaDataId();
    extern u8 fightWazaIsHit();
    extern u8 fightWazaIsJoutaiDataId();
    extern void fightWazaInitLoop();
    extern s32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern s8 pokemonSearchWazaDataId();
    extern u32 wazaGetStatus();
    extern u8 fn_802026E4();
    extern u8 fn_802025B8();
    extern s32 fn_80201C58();
    extern void fn_80201B2C();
    extern void fn_8020248C();
    extern void fn_8022D2CC();
    extern u8 fn_8022D6BC();
    extern void fn_8022D394();
    extern void fn_8022D20C();
    extern u32 fn_8022BB84();
    extern void fn_802207D4();
    extern void fn_8022B5C8();
    extern void _WsWkcActSubJoutaiMigawari__FPvUsPv();
    extern void fn_801DA3CC();
    extern u8 lbl_8047B627;
    extern u8 lbl_80378D02[];
    extern u8 lbl_803792C9[];
    extern u8 lbl_80379BF4[];
    extern u32 lbl_80379BFF[];
    u16 floorStatus[1];
    volatile u32 stopState;
    u8 modeValue;
    u32 mode;
    u32 attacker;
    u32 defender;
    u16 heldItem;
    u16 movePower;
    u32 usedMove;
    u16 moveEffect;
    u32 originalMove;
    u32 moveStatus;
    u8 moveType;
    u8 stop;
    u8 moveHit;
    s16 defenderValue11C;
    s16 defenderValue11E;
    u16 moveJoutai;
    u32 validOriginalMove;
    s8 count;
    u16 state;
    u32 other;
    u32 flags1;
    u32 flags2;

    floorStatus[0] = fightFloorGetStatus(0, 0, 0x14, 0);
    stop = 0;
    stopState = lbl_8047B610[2];
    mode = lbl_8047B610[1];
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    if (attacker != 0) {
        heldItem = fightOutPokemonGetSoubiItemSoubiDataId();
        moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
        moveHit = fightWazaIsHit(moveStatus);
        usedMove = fightOutPokemonGetUseWazaDataId(attacker);
        originalMove = fightOutPokemonGetMotoWazaDataId(attacker);
        validOriginalMove = originalMove & ~-((u16)originalMove == 0xFFFF);
        movePower = wazaGetStatus(moveStatus, 0, 0x30, 0, (u16)originalMove);
        moveType = wazaGetStatus(0, usedMove, 5, 0);
        moveEffect = wazaGetStatus(0, usedMove, 9, 0);
        moveJoutai = wazaGetStatus(0, usedMove, 7, 0);
    } else {
        heldItem = 0;
        moveStatus = 0;
        moveHit = 0;
        usedMove = 0;
        originalMove = 0;
        validOriginalMove = 0;
        movePower = 9;
        moveType = 1;
        moveEffect = 0;
        moveJoutai = 0;
    }

    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    if (defender != 0) {
        defenderValue11C = pokemonGetStatus(defender, 0, 0x11C, 0);
        defenderValue11E = pokemonGetStatus(defender, 0, 0x11E, 0);
    } else {
        defenderValue11C = 0;
        defenderValue11E = 0;
    }
    modeValue = (u8)mode;

    do {
        switch (lbl_8047B627) {
        case 0:
            if (fn_802026E4(defender, 0x13) == 1 &&
                fightOutPokemonCheckFightOut(defender) == 1 &&
                attacker != defender &&
                fightOutPokemonIsAlly(attacker, defender) == 0 &&
                moveHit == 1 &&
                (defenderValue11C != 0 || defenderValue11E != 0) &&
                moveJoutai != 0) {
                count = pokemonGetStatus(defender, 0, 0xE6, 0);
                if (count < 12) {
                    pokemonSetStatus(defender, 0, 0xE6, 0, (s8)(count + 1));
                    fn_80211B94(lbl_8047B62C, lbl_80378D02, 0);
                    stop = 1;
                }
            }
            lbl_8047B627++;
            break;

        case 1:
            if (fn_802026E4(defender, 7) == 1 &&
                fightOutPokemonCheckFightOut(defender) == 1 &&
                attacker != defender &&
                pokemonGetStatus(defender, 0, 0x11E, 0) != 0 &&
                fightWazaIsHit(moveStatus) == 1 &&
                (u16)wazaGetStatus(0, usedMove, 3, 0) == 10) {
                fightOutPokemonWriteJoutaiDataId(defender, 7);
                if (fightOutPokemonIsUseHensinBuff(defender) == 1) {
                    fightOutPokemonSetHensinPokemonStatusId(defender, 0x7C, 0, 0);
                }
                fn_80211B94(lbl_8047B62C, lbl_803792C9, 0);
                stop = 1;
            }
            lbl_8047B627++;
            break;

        case 2:
            fn_8022D2CC(attacker, defender);
            lbl_8047B627++;
            break;

        case 3:
            if (fn_8022D6BC(attacker, defender) != 0) {
                stop = 1;
            }
            lbl_8047B627++;
            break;

        case 4:
            fightFloorLoopValidFightOutPokemon(0, fn_8022D394, 0, 0);
            lbl_8047B627++;
            break;

        case 5:
            fn_8022D20C(attacker, defender);
            lbl_8047B627++;
            break;

        case 6:
            if ((lbl_8047B618 & 0x02000000) != 0 && heldItem == 0x1D &&
                (u16)originalMove != 0xA5 && (u16)originalMove != 0x164 &&
                fn_802025B8(attacker, 0x36) == 2) {
                if ((u16)originalMove == 0xE2 && fightWazaIsJoutaiDataId(moveStatus, 0x45) == 1) {
                    lbl_8047B627++;
                    break;
                }
                fn_8020248C(attacker, 0x36, 0);
                fn_80201B2C(attacker, 0x36, originalMove);
            }
            if (fn_802026E4(attacker, 0x36) == 1) {
                other = fn_80201C58(attacker, 0x36);
                if (pokemonSearchWazaDataId(fightOutPokemonGetPokemonPtr(attacker), other) < 0) {
                    fightOutPokemonWriteJoutaiDataId(attacker, 0x36);
                }
            }
            lbl_8047B627++;
            break;

        case 7:
            fightFloorLoopValidFightOutPokemon(0, fn_802207D4, 0, 0);
            lbl_8047B627++;
            break;

        case 8:
            fightFloorLoopValidFightOutPokemon(0, fn_8022B5C8, 0, 0);
            lbl_8047B627++;
            break;

        case 9:
            fn_8022BB84(attacker, defender);
            lbl_8047B627++;
            break;

        case 10:
            if (fightOutPokemonCheckFightOut(attacker) == 1) {
                state = fightOutPokemonIsJoutaiKie(attacker);
                if (state != 0) {
                    if (state != 0x1F) {
                        fightOutPokemonSetVisibility(attacker, 0);
                    } else {
                        other = pokemonGetStatus(attacker, 0, 0xEE, 0);
                        fn_801DA3CC(other, 3);
                    }
                }
            }
            lbl_8047B627++;
            break;

        case 11:
            if (fightOutPokemonCheckFightOut(attacker) == 1 &&
                (moveHit == 0 || fightOutPokemonIsJoutaiKie(attacker) == 0 ||
                 fightOutPokemonCheckNoAttackFlag(attacker) == 1)) {
                fightOutPokemonInitJoutaiKie(attacker);
                pokemonSetStatus(attacker, 0, 0x115, 0, 1);
                fightOutPokemonSetVisibility(attacker, 1);
                other = pokemonGetStatus(attacker, 0, 0xEE, 0);
                if (other != 0) {
                    fn_801DA36C((void*)other, 3);
                }
            }
            lbl_8047B627++;
            break;

        case 12:
            if (fightOutPokemonCheckFightOut(defender) == 1 &&
                pokemonGetStatus(defender, 0, 0x115, 0) == 0 &&
                fightOutPokemonIsJoutaiKie(defender) == 0) {
                fightOutPokemonSetVisibility(defender, 1);
                other = pokemonGetStatus(defender, 0, 0xEE, 0);
                if (other != 0) {
                    fn_801DA36C((void*)other, 3);
                }
            }
            lbl_8047B627++;
            break;

        case 13:
            fightFloorLoopValidFightOutPokemon(0, _WsWkcActSubJoutaiMigawari__FPvUsPv, 0, 0);
            lbl_8047B627++;
            break;

        case 14:
            if ((lbl_8047B618 & 0x1000) != 0) {
                fightFloorReplaceFightOutPokemonAttackToDefense(0);
                lbl_8047B618 &= 0xFFFFEFFF;
                attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
                if (attacker != 0) {
                    heldItem = fightOutPokemonGetSoubiItemSoubiDataId();
                    moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
                    moveHit = fightWazaIsHit(moveStatus);
                    usedMove = fightOutPokemonGetUseWazaDataId(attacker);
                    originalMove = fightOutPokemonGetMotoWazaDataId(attacker);
                    validOriginalMove = originalMove & ~-((u16)originalMove == 0xFFFF);
                    movePower = wazaGetStatus(moveStatus, 0, 0x30, 0, (u16)originalMove);
                    moveType = wazaGetStatus(0, usedMove, 5, 0);
                    moveEffect = wazaGetStatus(0, usedMove, 9, 0);
                    moveJoutai = wazaGetStatus(0, usedMove, 7, 0);
                } else {
                    heldItem = 0;
                    moveStatus = 0;
                    moveHit = 0;
                    usedMove = 0;
                    originalMove = 0;
                    validOriginalMove = 0;
                    movePower = 9;
                    moveType = 1;
                    moveEffect = 0;
                    moveJoutai = 0;
                }
                defender = fightTargetGetPtrAsNowFightType(0x12, 0);
                if (defender != 0) {
                    defenderValue11C = pokemonGetStatus(defender, 0, 0x11C, 0);
                    defenderValue11E = pokemonGetStatus(defender, 0, 0x11E, 0);
                } else {
                    defenderValue11C = 0;
                    defenderValue11E = 0;
                }
            }

            if ((u16)wazaGetStatus(0, validOriginalMove, 9, 0) != 0x7F ||
                ((u16)wazaGetStatus(0, validOriginalMove, 9, 0) == 0x7F && moveHit == 0)) {
                pokemonSetStatus(attacker, 0, 0xEF, 0, (u16)originalMove);
            }
            if (fightOutPokemonCheckFightOut(attacker) == 1 &&
                (u16)wazaGetStatus(0, validOriginalMove, 9, 0) != 0x7F) {
                if ((lbl_8047B618 & 0x02000000) != 0) {
                    pokemonSetStatus(attacker, 0, 0xF0, 0, (u16)originalMove);
                    pokemonSetStatus(attacker, 0, 0xF1, 0, (u16)usedMove);
                } else {
                    pokemonSetStatus(attacker, 0, 0xF0, 0, 0xFFFF);
                    pokemonSetStatus(attacker, 0, 0xF1, 0, 0xFFFF);
                }
                if (fightOutPokemonCheckFightOut(defender) == 1) {
                    state = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floorStatus[0]);
                    pokemonSetStatus(defender, 0, 0xF2, 0, state);
                }
                if ((lbl_8047B618 & 0x02000000) != 0 && moveHit == 1) {
                    if ((u16)originalMove == 0xFFFF) {
                        pokemonSetStatus(defender, 0, 0xF3, 0, (u16)originalMove);
                    } else {
                        pokemonSetStatus(defender, 0, 0xF3, 0, (u16)usedMove);
                        pokemonSetStatus(defender, 0, 0xF4, 0, movePower);
                    }
                } else {
                    pokemonSetStatus(defender, 0, 0xF3, 0, 0xFFFF);
                }
            }
            lbl_8047B627++;
            break;

        case 15:
            if (fightOutPokemonCheckValid(attacker) == 1 &&
                (u8)wazaGetStatus(0, validOriginalMove, 0x11, 0) == 1 &&
                (lbl_8047B618 & 0x02000000) != 0 && attacker != defender &&
                fightOutPokemonCheckValid(defender) == 1 && moveHit == 1) {
                fightOutPokemonSetOumuWazaDataId(defender, attacker, originalMove);
            }
            lbl_8047B627++;
            break;

        case 16:
            if ((lbl_8047B618 & 0x80000) == 0 &&
                (u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2 &&
                pokemonGetStatus(attacker, 0, 0x109, 0) == 0 &&
                moveType == 4 && (lbl_8047B618 & 0x200) == 0) {
                other = fightTargetGetPtrAsNowFightType(0xE, defender);
                if (fightOutPokemonCheckFightOut(other) == 1) {
                    fightFloorSetStatus(0, 0, 0x43, 0, other);
                    fightWazaInitLoop(moveStatus);
                    {
                    void* messageControl = lbl_8047B62C;
                    u8* message = lbl_80379BF4;
                    flags1 = lbl_8047B618 & 0xFFFFFFBF;
                    lbl_80478D78[3] = 0;
                    *(volatile u32*)&lbl_8047B618 = flags1;
                    flags2 = lbl_8047B618 & 0xFFFFBFFF;
                    *(volatile u32*)&lbl_8047B618 = flags2;
                    lbl_80478D78[6] = 0;
                    lbl_8047B618 = flags2 | 0x200;
                    lbl_8047B627 = 0;
                    fn_80211B94(messageControl, message, 0);
                    lbl_8047B610 = (u8*)lbl_80379BFF[moveEffect];
                    return;
                    }
                }
                lbl_8047B618 |= 0x200;
            }
            lbl_8047B627++;
            break;

        case 17:
            if (modeValue == 0) {
                fightMenuAllFightTrainerCloseStatusMenu(0);
                fightMenuAllFightOutPokemonCloseStatusMenu(0);
                fightMenuCloseMsg();
            }
            break;
        }

        if (modeValue == 1 && stop == 0) {
            lbl_8047B627 = 0x11;
        }
        if (modeValue == 2 && (u8)stopState == lbl_8047B627) {
            lbl_8047B627 = 0x11;
        }
    } while (lbl_8047B627 != 0x11 && stop == 0);

    if (lbl_8047B627 == 0x11 && stop == 0) {
        lbl_8047B610 += 3;
    }
}
void fn_80220868(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 pokemonGetStatus();
    extern u8 fightSideIsJoutaiDataId();
    extern void fn_80220B8C();
    extern u8 lbl_8047B64A;
    u8 flags;
    u32 statusId;
    u16 baseCommand;
    u32 ctx;
    u32 side;
    int flag4;
    u32 ability;
    u8 mask;
    u8 status;
    u32 bitIndex;
    u32 command;
    s32 count;

    bitIndex = 0;
    command = 0;
    count = 0;
    ctx = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
    side = fightTargetGetPtrAsNowFightType(2, ctx);
    ability = fightOutPokemonGetTokuseiDataId(ctx);
    mask = lbl_8047B610[2];
    flags = lbl_8047B610[3];

    if ((flags & 1) != 0) {
        baseCommand = 0x15;
        if ((flags & 2) != 0) {
            baseCommand = 0x2d;
        }

        while ((u8)(s8)mask != 0) {
            u32 currentMask;

            currentMask = (u8)mask;
            if ((currentMask & 1) != 0) {
                switch ((u8)bitIndex) {
                case 0:
                    statusId = 0xe6;
                    break;
                case 1:
                    statusId = 0xe7;
                    break;
                case 2:
                    statusId = 0xea;
                    break;
                case 3:
                    statusId = 0xe8;
                    break;
                case 4:
                    statusId = 0xe9;
                    break;
                case 5:
                    statusId = 0xeb;
                    break;
                case 6:
                    statusId = 0xec;
                    break;
                case 7:
                default:
                    statusId = 0;
                    break;
                }

                status = (u8)pokemonGetStatus(ctx, 0, statusId, 0);
                if ((flags & 8) != 0) {
                    if (status != 0) {
                        command = ((u8)bitIndex + baseCommand) & 0xffff;
                        count++;
                    }
                } else if (fightSideIsJoutaiDataId(side, 0x4c) == 0 &&
                           (u16)ability != 0x1d && (u16)ability != 0x49 &&
                           !((u16)ability == 0x33 && (u16)statusId == 0xeb) &&
                           !((u16)ability == 0x34 && (u16)statusId == 0xe6) &&
                           status != 0) {
                    command = ((u8)bitIndex + baseCommand) & 0xffff;
                    count++;
                }
            }
            mask = (s32)currentMask >> 1;
            bitIndex++;
        }

        if (count > 1) {
            if ((flags & 2) != 0) {
                command = 0x3a;
            } else {
                command = 0x39;
            }
        }
    } else {
        if ((flags & 2) != 0) {
            baseCommand = 0x26;
        } else {
            baseCommand = 0xe;
        }

        while ((u8)(s8)mask != 0) {
            u32 currentMask;

            currentMask = (u8)mask;
            if ((currentMask & 1) != 0) {
                switch ((u8)bitIndex) {
                case 0:
                    statusId = 0xe6;
                    break;
                case 1:
                    statusId = 0xe7;
                    break;
                case 2:
                    statusId = 0xea;
                    break;
                case 3:
                    statusId = 0xe8;
                    break;
                case 4:
                    statusId = 0xe9;
                    break;
                case 5:
                    statusId = 0xeb;
                    break;
                case 6:
                    statusId = 0xec;
                    break;
                case 7:
                default:
                    statusId = 0;
                    break;
                }

                status = (u8)pokemonGetStatus(ctx, 0, statusId, 0);
                if (status < 0xc) {
                    command = (baseCommand + (u8)bitIndex) & 0xffff;
                    count++;
                }
            }
            mask = (s32)currentMask >> 1;
            bitIndex++;
        }

        if (count > 1) {
            if ((flags & 2) != 0) {
                command = 0x38;
            } else {
                command = 0x37;
            }
        }
    }

    if ((flag4 = flags & 4) != 0 && count < 2) {
        lbl_8047B610 += 4;
        goto done;
    }
    if (count == 0 || lbl_8047B64A != 0) {
        goto advance;
    }
    fn_80220B8C(command);
    if (flag4 != 0 && count > 1) {
        lbl_8047B64A = 1;
    }
    lbl_8047B610 += 4;
    goto done;

advance:
    lbl_8047B610 += 4;
done:
    return;
}
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F54A4 fightFloorGetStatus
#define fn_8012640C pokemonGetStatus
#pragma optimize_for_size on
void fn_80220B8C(u32 r3)

{
    extern void fn_801DA9E8();
    extern u8 fn_801DDD28();
    extern u32 fn_801F025C();
    extern u32 fn_801F54A4();
    extern void fn_80265598();
    u32 ctx;
    u32 statusPtr;
    u32 floorStatus;
    u16 command;

    ctx = fn_801F025C(lbl_8047B610[1], 0);
    command = r3;
    if ((command == 0x37) || (command == 0x38)) {
        floorStatus = fn_801F54A4(0, 0, 0x14, 0) & 0xFFFF;
        statusPtr = fn_8012640C(ctx, 0, 0xee, 0);
        if ((statusPtr != 0) && (fn_801DDD28(statusPtr, 0x2c, 4, 0) != 0)) {
            fn_801DA9E8(statusPtr, 0x2c, 4);
            fn_80265598(ctx, floorStatus, 1);
        }
    } else if ((command == 0x39) || (command == 0x3a)) {
        statusPtr = (u16)fn_801F54A4(0, 0, 0x14, 0);
        floorStatus = fn_8012640C(ctx, 0, 0xee, 0);
        if ((floorStatus != 0) && (fn_801DDD28(floorStatus, 0x3c, 4, 0) != 0)) {
            fn_801DA9E8(floorStatus, 0x3c, 4);
            fn_80265598(ctx, statusPtr, 1);
        }
    } else {
#define APPLY_BATTLE_STATUS(statusId) \
        statusPtr = (u16)fn_801F54A4(0, 0, 0x14, 0); \
        floorStatus = fn_8012640C(ctx, 0, 0xee, 0); \
        if ((floorStatus != 0) && (fn_801DDD28(floorStatus, statusId, 4, 0) != 0)) { \
            fn_801DA9E8(floorStatus, statusId, 4); \
            fn_80265598(ctx, statusPtr, 1); \
        }
        switch (command) {
        case 0x0f:
        case 0x12:
        case 0x29:
        case 0x2c:
            APPLY_BATTLE_STATUS(0x26);
            break;
        case 0x10:
        case 0x13:
        case 0x15:
        case 0x2a:
        case 0x2d:
        case 0x2f:
            APPLY_BATTLE_STATUS(0x28);
            break;
        case 0x11:
        case 0x2b:
            APPLY_BATTLE_STATUS(0x2a);
            break;
        case 0x14:
        case 0x2e:
            APPLY_BATTLE_STATUS(0x2c);
            break;
        case 0x16:
        case 0x19:
        case 0x30:
        case 0x33:
            APPLY_BATTLE_STATUS(0x27);
            break;
        case 0x17:
        case 0x1a:
        case 0x1c:
        case 0x31:
        case 0x34:
            APPLY_BATTLE_STATUS(0x29);
            break;
        case 0x18:
        case 0x32:
            APPLY_BATTLE_STATUS(0x2b);
            break;
        case 0x1b:
            APPLY_BATTLE_STATUS(0x3c);
            break;
        }
#undef APPLY_BATTLE_STATUS
    }
}
#pragma optimize_for_size reset
#undef fn_801F025C
#undef fn_801F54A4
#undef fn_8012640C
#pragma optimization_level 3
void fn_8022106C(void* ctx, u32 param1, u32 param2) {
    extern u8 lbl_80379F58[];
    u8 code;
    u32 offset;
    s32 low;
    u32 result;

    offset = 0;
    code = lbl_80379F58[0x1601e];
    switch (code & 0xf0) {
        case 0x10: offset = 0xf; break;
        case 0x20: offset = 0x27; break;
        case 0x90: offset = 0x16; break;
        case 0xa0: offset = 0x2e; break;
        default: break;
    }
    low = code & 0xf;
    lbl_8047B610 = lbl_8047B610 + 1;
    low = low - 1;
    result = offset + low;
    lbl_80379F58[0x160a4] = result;
    lbl_80379F58[0x160a5] = 0;
    return;
}
#pragma optimization_level reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void fn_802222F4(void)

{
    extern void fn_801F025C();
    extern void fn_80221104();
  u8* pc;
  void* ptr1;
  u8 uVar1;
  u16 uVar3;

  fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  pc = lbl_8047B610;
  ptr1 = *(void**)(pc + 2);
  if (ptr1 != (void *)0) {
    uVar1 = *(u8*)ptr1;
  }
  else {
    uVar1 = 0;
  }
  if (*(u16**)(pc + 6) != (void *)0) {
    uVar3 = **(u16**)(pc + 6);
  }
  else {
    uVar3 = 0;
  }
  fn_80221104(*(u8 *)(pc + 1),uVar1,uVar3);
  lbl_8047B610 += 10;
  return;
}
#undef fn_801F025C
#define fn_801F025C fightTargetGetPtrAsNowFightType
void fn_80222370(void)

{
    extern void fn_801F025C();
    extern void fn_80221104();
  u16 uVar1;
  volatile u8* pc;
  u16* value;
  u32 arg2;

  fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  pc = lbl_8047B610;
  value = *(u16 **)(pc + 3);
  if (value != (void *)0) {
    uVar1 = *value;
  }
  else {
    uVar1 = 0;
  }
  arg2 = pc[2];
  fn_80221104(pc[1],arg2,uVar1);
  lbl_8047B610 += 7;
  return;
}
#undef fn_801F025C
#pragma optimize_for_size on
void fn_80222654(void)
{
    struct {
        u8* value;
    } dst;
    int i;
    u8* src;
    u8* offset;
    int count;

    count = lbl_8047B610[13];
    dst.value = *(u8**)(lbl_8047B610 + 1);
    src = *(u8**)(lbl_8047B610 + 5);
    offset = *(u8**)(lbl_8047B610 + 9);

    for (i = 0; i < count; i++) {
        *dst.value++ = src[i + offset[0]];
    }
    lbl_8047B610 += 14;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_802226A4(void)
{
    u8* pc = lbl_8047B610;
    int count;
    u32 rawDst;
    u32 rawSrc;
    struct {
        u8* value;
    } dst;
    struct {
        u8* value;
    } src;
    rawDst = *(u32*)(pc + 1);
    rawSrc = *(u32*)(pc + 5);
    count = pc[9];
    dst.value = (u8*)rawDst;
    src.value = (u8*)rawSrc;

    while (count-- > 0) {
        *dst.value++ = *src.value++;
    }
    lbl_8047B610 += 10;
}
#pragma opt_propagation reset
#pragma optimize_for_size reset
#pragma optimize_for_size on
void fn_8022275C(void)
{
#pragma optimization_level 3
    u8 count;
    u8* pc;
    u8* a;
    u8* b;
    u32 next;
    u8 i;
    u8 matches;

    count = lbl_8047B610[9];
    pc = lbl_8047B610;
    matches = 0;
    i = 0;
    a = *(u8**)(pc + 1);
    b = *(u8**)(pc + 5);
    next = *(u32*)(pc + 10);

    while (i < count) {
        if (*a == *b) {
            matches++;
        }
        a++;
        b++;
        i++;
    }
    if (matches != count) {
        lbl_8047B610 = (u8*)next;
    } else {
        lbl_8047B610 += 14;
    }
}
#pragma optimization_level reset
#pragma optimize_for_size reset
#pragma optimize_for_size on
#pragma optimization_level 3
void fn_802227D4(void)
{
    u8* pc = lbl_8047B610;
    u32 count = pc[9];
    u8* a = *(u8**)(pc + 1);
    u8* b = *(u8**)(pc + 5);
    u32 next = *(u32*)(pc + 10);
    u8 i = 0;

    while (i < (u8)count) {
        if (*a != *b) {
            lbl_8047B610 += 14;
            break;
        }
        a++;
        b++;
        i++;
    }
    if (i != (u8)count) {
        return;
    }
    lbl_8047B610 = (u8*)next;
}
#pragma optimization_level reset
#pragma optimize_for_size reset
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
#pragma opt_propagation off
void fn_8022290C(void) {
    int sel;
    u32 vb;
    u8* base;
    u32 pa;
    u8* nxt;

    base = lbl_8047B610;
    sel = base[1];
    pa = *(u32*)(base + 2);
    vb = *(u16*)(base + 6);
    nxt = *(u8**)(base + 8);
    lbl_8047B610 = base + 12;
    switch (sel) {
    case 0:
        if (*(u16*)pa != (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 1:
        if (*(u16*)pa == (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 2:
        if (*(u16*)pa <= (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 3:
        if (*(u16*)pa >= (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 4:
        if ((*(u16*)pa & (u16)(s16)vb) == 0) return;
        lbl_8047B610 = nxt;
        return;
    case 5:
        if ((*(u16*)pa & (u16)(s16)vb) != 0) return;
        lbl_8047B610 = nxt;
        return;
    }
}
void fn_802229EC(void) {
    int sel;
    u32 vb;
    u32 base;
    u32 pa;
    u32 nxt;

    base = *(u32*)&lbl_8047B610;
    sel = *(u8*)(base + 1);
    pa = *(u32*)(base + 2);
    vb = *(u8*)(base + 6);
    nxt = *(u32*)(base + 7);
    *(u32*)&lbl_8047B610 = base + 11;
    switch (sel) {
    case 0:
        if (*(u8*)pa != (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 1:
        if (*(u8*)pa == (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 2:
        if (*(u8*)pa <= (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 3:
        if (*(u8*)pa >= (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 4:
        if ((*(u8*)pa & (u8)(s8)vb) == 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 5:
        if ((*(u8*)pa & (u8)(s8)vb) != 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    }
}
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_80209F18 fightWazaInitLoop
#pragma opt_propagation off
#pragma scheduling 604
void fn_80222BD8(void)

{
    extern u32 fn_801F025C();
    extern void fn_80209F18();
    extern u32 lbl_8047B618;
    u32 target;
    u32 value;
    u32 state;
    u32 first;
    u8* result;
    u32 zero;

    target = fn_801F025C(0x11, 0);
    fn_8012640C(target, 0, 0xd9, 0);
    fn_80209F18();
    value = lbl_8047B618;
    zero = 0;
    state = (u32)lbl_8047B610;
    result = lbl_80478D78;
    first = value & 0xffffffbf;
    result[3] = zero;
    value = state + 1;
    lbl_8047B618 = first;
    state = first & 0xffffbfff;
    result[6] = zero;
    lbl_8047B618 = state;
    lbl_8047B610 = (u8*)value;
}
#pragma scheduling reset
#pragma opt_propagation reset
#undef fn_801F025C
#undef fn_8012640C
#undef fn_80209F18
u32 fn_80222EF0(u32 r3)

{
    extern void pokemonInit();
    extern void battleGridUpdate();
    extern void fightFloorGetFightOutPokemonPtrToHeroPtr();
    extern void fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightOutPokemonInit();
    extern void fightPokemonInit();
    extern void fn_80211B94();
    extern void* lbl_8047B62C;
    extern u8 lbl_8037879E[];
  u32 uVar4;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;
  u8 cVar7;
  u8 cVar6;
  u8 cVar8;
  int iVar5;

  fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,r3);
  fightFloorGetFightOutPokemonPtrToHeroPtr(0,r3);
  uVar1 = (int)pokemonGetStatus(r3,0,0xd5,0);
  uVar2 = (int)pokemonGetStatus(r3,0,0xd6,0);
  uVar3 = (int)pokemonGetStatus(uVar1,0,0xcb,0);
  uVar4 = (int)pokemonGetStatus(uVar1,0,0xcc,0);
  cVar6 = (int)pokemonGetStatus(r3,0,0x120,0);
  cVar7 = (int)pokemonGetStatus(uVar2,0,0xd2,0);
  cVar8 = fightOutPokemonCheckFightOut(r3);
  if (cVar8 == 0) {
    iVar5 = (int)pokemonGetStatus(r3,0,0x121,0);
    if (iVar5 < 0) {
      if (cVar6 == 1) {
        fightOutPokemonInit(r3);
        battleGridUpdate();
      }
      if (cVar7 == 1) {
        pokemonInit(uVar3);
        pokemonInit(uVar4);
        fightPokemonInit(uVar1);
        fightPokemonInit(uVar2);
        fightOutPokemonInit(r3);
        battleGridUpdate();
      }
    }
    else {
      fightFloorSetStatus(0,0,0x45,0,r3);
      fightFloorSetStatus(0,0,0x46,0,r3);
      if (cVar7 == 1) {
        pokemonInit(uVar3);
        pokemonInit(uVar4);
        fightPokemonInit(uVar1);
        fightPokemonInit(uVar2);
      }
      fn_80211B94(lbl_8047B62C,(u32)lbl_8037879E,0);
    }
  }
  return 1;
}
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F150C fightFloorSetTuusinErrorFightResult
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F85B0 fightTrainerGetFightOutPokemonToTemotiBanme
#define fn_801F8F24 fightTrainerGetEntryIdToFightPokemonPtr
#define fn_801F9600 fightTrainerSelectIrekaeFightPokemon
#define fn_801FA634 fightTrainerCheckValid
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80206608 fightPokemonCheckFightOut
#define fn_80206780 fightOutPokemonCheckValid
#define fn_802068C8 fightOutPokemonCreate
u32 fn_802230BC(u32 trainer, u32 selector)
{
    extern u16 fn_801EF634();
    extern void fn_801F150C();
    extern u32 fn_801F54A4();
    extern u32 fn_801F85B0();
    extern u32 fn_801F8A18();
    extern void fn_801F8F24();
    extern s32 fn_801F9600();
    extern u8 fn_801FA634();
    extern u32 fn_801FB1C0();
    extern u8 fn_802062FC();
    extern u8 fn_80206608();
    extern u8 fn_80206780();
    extern void fn_802068C8();
    u32 index;
    u16 count;
    u16 local_28[4];
    u32 current;
    u32 selected;
    u32 replacement;
    u32 partyIndex;
    s32 entry;

    if (fn_801EF634() != 0) {
        return 1;
    }
    if (fn_801FA634(trainer) == 0) {
        return 1;
    }

    count = (u16)fn_801F54A4(0, 0, 0x18, 0);
    for (index = 0; (index & 0xffff) < count; index++) {
        current = fn_801FB1C0(trainer, 0, 0x46, index);
        if (current == 0 || fn_802062FC(current) == 1 ||
            (u8)fn_8012640C(current, 0, 0x119, 0) == 1) {
            continue;
        }

        local_28[0] = 0;
        if (fn_801F8A18(trainer, local_28) == 0) {
            goto done;
        }

        entry = -1;
        selected = 0;
        if (fn_80206780(current) == 0) {
            partyIndex = fn_801F85B0(trainer, current);
            replacement = fn_801FB1C0(trainer, 0, 0x45, (u16)partyIndex);
            if (fn_80206608(replacement) == 1) {
                selected = replacement;
                entry = fn_8012640C(replacement, 0, 0xce, 0);
                entry = (s16)entry;
            }
        }

        if (selected == 0) {
            entry = fn_801F9600(trainer, selector, 0, current);
            if ((s16)entry < 0) {
                if ((s16)entry == -2) {
                    fn_801F150C(0);
                }
                goto done;
            }
            fn_801F8F24(trainer, entry);
        }

        if (fn_80206780(current) == 0) {
            partyIndex = fn_801F85B0(trainer, current);
            replacement = fn_801FB1C0(trainer, 0, 0x45, (u16)partyIndex);
            fn_802068C8(current, replacement, 0);
            fn_801254B4((void*)current, 0, 0x120, 0, 1);
        }
        fn_801254B4((void*)current, 0, 0x121, 0, (s16)entry);
        fn_801254B4((void*)current, 0, 0x119, 0, 1);
    }

done:
    return 1;
}
#undef fn_802068C8
#undef fn_80206780
#undef fn_80206608
#undef fn_802062FC
#undef fn_801FB1C0
#undef fn_801FA634
#undef fn_801F9600
#undef fn_801F8F24
#undef fn_801F85B0
#undef fn_801F54A4
#undef fn_801F150C
#undef fn_8012640C
#undef fn_801254B4
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
void WS_CHECK_TYPE(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_80207AE0();
  u32 uVar1;
  u8 cVar2;
  u8 typeId;
  u8* pc;
  u8* jumpTarget;
  u8* nextPc;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  pc = lbl_8047B610;
  cVar2 = fn_80207AE0(uVar1,
      (typeId = *(u8 *)(pc + 2),
       jumpTarget = (u8*)*(u32 *)(pc + 3), typeId));
  if (cVar2 == 1) {
    nextPc = jumpTarget;
  }
  else {
    nextPc = lbl_8047B610 + 7;
  }
  lbl_8047B610 = nextPc;
  return;
}
#undef fn_80207AE0
#undef fn_801F025C
#define fn_801F025C fightTargetGetPtrAsNowFightType
void WS_WAZAKOUKA_CHECK(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
  u32 uVar1;
  u8 cVar2;

  u8* jumpTarget;
  u8* nextPc;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  jumpTarget = (u8*)*(u32 *)(lbl_8047B610 + 7);
  cVar2 = fn_802026E4(uVar1,*(u32 *)(lbl_8047B610 + 2) & 0xffff);
  if (*(u8 *)(lbl_8047B610 + 6) == cVar2) {
    nextPc = jumpTarget;
  }
  else {
    nextPc = lbl_8047B610 + 0xb;
  }
  lbl_8047B610 = nextPc;
  return;
}
#undef fn_801F025C
#pragma optimize_for_size on
void fn_80223AF4(void)
{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern s32 pokemonGetStatus();
    u32 ctx;
    u32 statusId;
    u8 result;
    s16 mode;
    s16 expected;
    u8 status;
    u8 *pc;

    result = 0;
    ctx = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
    mode = lbl_8047B610[2];

    switch (lbl_8047B610[3]) {
    case 0:
        statusId = 0xe6;
        break;
    case 1:
        statusId = 0xe7;
        break;
    case 2:
        statusId = 0xea;
        break;
    case 3:
        statusId = 0xe8;
        break;
    case 4:
        statusId = 0xe9;
        break;
    case 5:
        statusId = 0xeb;
        break;
    case 6:
        statusId = 0xec;
        break;
    case 7:
    default:
        statusId = 0;
        break;
    }

    status = (u8)pokemonGetStatus(ctx, 0, statusId, 0);
    pc = lbl_8047B610;
    expected = (u8)pc[4];
    switch ((u8)(s8)mode) {
    case 0:
        if ((u8)status == (u8)expected) {
            result = 1;
        }
        break;
    case 1:
        if ((u8)status != (u8)expected) {
            result = 1;
        }
        break;
    case 2:
        if ((u8)status > (u8)expected) {
            result = 1;
        }
        break;
    case 3:
        if ((u8)status < (u8)expected) {
            result = 1;
        }
        break;
    case 4:
        if (((u8)status & (u8)expected) != 0) {
            result = 1;
        }
        break;
    case 5:
        if (((u8)status & (u8)expected) == 0) {
            result = 1;
        }
        break;
    }

    if (result != 0) {
        lbl_8047B610 = *(u8 **)(pc + 5);
    } else {
        lbl_8047B610 += 9;
    }
}
#pragma optimize_for_size reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
void WS_SIDECONDITION_CHECK(void)
{
    extern u32 fn_801F025C();
    extern u8 fn_801F6E98();
  u32 uVar1;
  u8 cVar2;
  u16 conditionId;
  u8* pc;
  u8* jumpTarget;
  u8* nextPc;

  uVar1 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  uVar1 = fn_801F025C(2,uVar1);
  pc = lbl_8047B610;
  cVar2 = fn_801F6E98(uVar1,
      (conditionId = *(u16 *)(pc + 2),
       jumpTarget = (u8*)*(u32 *)(pc + 4), conditionId));
  if (cVar2 == 1) {
    nextPc = jumpTarget;
  }
  else {
    nextPc = lbl_8047B610 + 8;
  }
  lbl_8047B610 = nextPc;
  return;
}
#undef fn_801F025C
#undef fn_801F6E98
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80203C5C fightOutPokemonIsJoutaiKaragenki
#define fn_80203CCC fightOutPokemonIsJoutaiNormal
#define fn_802062FC fightOutPokemonCheckFightOut
void WS_CONDITION_CHECK(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern u8 fn_80203C5C();
    extern u8 fn_80203CCC();
    extern u32 fn_802062FC();
  u32 ctx;
  u8* target;
  u16 kind;

  ctx = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  kind = *(u32 *)(lbl_8047B610 + 2);
  target = *(u8 **)(lbl_8047B610 + 6);
  if (kind == 1) {
    if ((u8)fn_80203CCC(ctx) == 0) {
      lbl_8047B610 = target;
    }
    else {
      lbl_8047B610 += 10;
    }
  }
  else if (kind == 2) {
    if ((u8)fn_80203C5C(ctx) == 1) {
      lbl_8047B610 = target;
    }
    else {
      lbl_8047B610 += 10;
    }
  }
  else {
    if ((u8)fn_802026E4(ctx, kind) == 1 &&
        (u8)fn_802062FC(ctx) == 1) {
      lbl_8047B610 = target;
    }
    else {
      lbl_8047B610 += 10;
    }
  }
  return;
}
#undef fn_802062FC
#undef fn_80203CCC
#undef fn_80203C5C
#undef fn_801F025C
#pragma optimize_for_size reset
void fn_80224158(void)

{
    extern u32 GSmsgGetGSchar();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern s8 pokemonSearchWazaDataId();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrAry();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern void fightSideSetStatus();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u32 fn_80201890();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonIsAlly();
    extern void fightPokemonGetFriendFormPokemonFriendFilterId();
    extern u8 fightOutPokemonIsHinsi();
    extern u32 figthOutPokemonGetLevel();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u32 fightOutPokemonGetMotoWazaDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightWazaInitJoutai();
    extern void fightWazaBiosCopy();
    extern void fn_80211B94();
    extern void pokemonSetStatus();
    extern u32 pokemonGetStatus();
    extern void fightMenuAllFightTrainerCloseStatusMenu();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    extern void fightMenuCloseMsg();
    extern u8 lbl_803786F4[];
    extern u8 lbl_80378703[];
    extern u8 lbl_80378712[];
    extern u8 lbl_80378D54[];
    extern u8 lbl_80379167[];
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
  u32 iVar1;
  u32 uVar6;
  u32 uVar2;
  u8 cVar13;
  u32 uVar22;
  u32 uVar5;
  u8 bVar14;
  u32 uVar7;
  u32 sVar11;
  u32 bVar15;
  u32 uVar9;
  u32 uVar17;
  u32 uVar8;
  u16 uVar12;

  int iVar16;
  u8 *puVar20;
  u32 *piVar21;
  s8 cVar18;
  u32 uVar4;
  u32 local_f8 [8];
  u8 auStack_d8 [176];

  fightMenuAllFightTrainerCloseStatusMenu(0);
  fightMenuAllFightOutPokemonCloseStatusMenu(0);
  fightMenuCloseMsg();
  iVar1 = (int)lbl_8047B610;
  if (*(u8 *)(iVar1 + 2) != 0) {
    u32 uVar19;
    u32 uVar3;

    uVar19 = fightTargetGetPtrAsNowFightType(*(u8 *)(iVar1 + 1),0);
    uVar3 = fightTargetGetPtrAsNowFightType(2,uVar19);
    cVar13 = fightOutPokemonIsHinsi(uVar19);
    if (cVar13 == 1) {
      fightSideSetStatus(uVar3,0,8,0,0);
      lbl_8047B610 = *(u8**)(lbl_8047B610 + 3);
      return;
    }
  }
  else {
    cVar13 = *(u8 *)(iVar1 + 1);
    if (cVar13 == 17) {
      iVar1 = fightTargetGetPtrAsNowFightType(0x11,0);
      pokemonGetStatus(iVar1,0,0xd5,0);
      uVar2 = fightTargetGetPtrAsNowFightType(0x12,0);
      puVar20 = lbl_803786F4;
    }
    else if (cVar13 == 18) {
      iVar1 = fightTargetGetPtrAsNowFightType(0x12,0);
      pokemonGetStatus(iVar1,0,0xd5,0);
      uVar2 = fightTargetGetPtrAsNowFightType(0x11,0);
      puVar20 = lbl_80378703;
    }
    else if (cVar13 == 21) {
      iVar1 = fightTargetGetPtrAsNowFightType(0x15,0);
      pokemonGetStatus(iVar1,0,0xd5,0);
      uVar2 = fightTargetGetPtrAsNowFightType(0x11,0);
      puVar20 = lbl_80378712;
    }
    else {
      iVar1 = 0;
      uVar2 = 0;
      puVar20 = lbl_80378703;
    }
    cVar13 = fightOutPokemonIsHinsi(iVar1);
    if (cVar13 == 1) {
      pokemonSetStatus(iVar1,0,0x120,0,1);
      cVar13 = fightOutPokemonIsGcHeroFightOutPokemon(iVar1);
      if (cVar13 == 1) {
        lbl_8047B618 = lbl_8047B618 | 0x400000;
        cVar13 = fightFloorGetStatus(0,0,0x27,0);
        if ((cVar13 == 1) && (iVar1 != 0)) {
          uVar22 = pokemonGetStatus(iVar1,0,0xd5,0);
          uVar6 = figthOutPokemonGetLevel(iVar1);
          sVar11 = fightFloorGetFightOutPokemonPtrAry(0,0,2,iVar1,local_f8);
          bVar15 = 0;
          piVar21 = local_f8;
          uVar17 = bVar15;
          for (; (u16)uVar17 < (u16)sVar11; uVar17 = uVar17 + 1) {
            if ((piVar21[(u16)uVar17] != 0) &&
                (bVar14 = figthOutPokemonGetLevel(piVar21[(u16)uVar17]),
                 (bVar15 & 0xff) < bVar14)) {
              bVar15 = figthOutPokemonGetLevel(piVar21[(u16)uVar17]);
            }
          }
          if ((bVar15 & 0xff) > (uVar6 & 0xff)) {
            if ((int)((bVar15 & 0xff) - (uVar6 & 0xff)) >= 0x1e) {
              fightPokemonGetFriendFormPokemonFriendFilterId(uVar22,8);
            }
            else {
              fightPokemonGetFriendFormPokemonFriendFilterId(uVar22,6);
            }
          }
        }
      }
      if ((lbl_8047B618 & 0x1000000) == 0) {
        uVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
        cVar13 = fn_802025B8(uVar4,0x28);
        if (cVar13 == 1) {
          uVar6 = fightTargetGetPtrAsNowFightType(0x11,0);
          uVar7 = fightOutPokemonGetPokemonPtr();
          uVar8 = pokemonGetStatus(uVar6,0,0xd9,0);
          uVar9 = fightOutPokemonGetMotoWazaDataId(uVar6);
          sVar11 = fightOutPokemonGetUseWazaDataId(uVar6);
          cVar13 = fightOutPokemonIsAlly(uVar6,uVar4);
          if ((((cVar13 == 0) && (cVar13 = fightOutPokemonCheckFightOut(uVar6), cVar13 == 1)) &&
              ((sVar11 & 0xffff) != 0xa5)) && ((sVar11 & 0xffff) != 0x164)) {
            cVar18 = pokemonSearchWazaDataId(uVar7,uVar9);
            if (cVar18 < 0) {
              cVar18 = wazaGetStatus(uVar8,0,0x26,0);
            }
            iVar16 = cVar18;
            pokemonSetStatus(uVar7,0,0x80,iVar16,0);
            cVar13 = fn_802026E4(uVar6,0x10);
            if ((cVar13 == 0) && (cVar13 = fn_802026E4(uVar6,0x31), cVar13 == 1)) {
              uVar5 = fn_80201890(uVar6,0x31);
              if (((uVar5 & 1 << (u32)(u8)cVar18) == 0) &&
                 (cVar13 = fightOutPokemonIsUseHensinBuff(uVar6), cVar13 == 1)) {
                fightOutPokemonSetHensinPokemonStatusId(uVar6,0x80,(u32)(u8)cVar18,0);
              }
            }
            cVar13 = fightOutPokemonIsUseHensinBuff(uVar6);
            if (cVar13 == 1) {
              fightOutPokemonSetHensinPokemonStatusId(uVar6,0x80,iVar16,0);
            }
            uVar12 = pokemonGetStatus(uVar7,0,0x7f,iVar16);
            wazaGetStatus(0,uVar12,1,0);
            uVar4 = GSmsgGetGSchar();
            msgctrlSetValue(0xd,uVar4);
            fn_80211B94(lbl_8047B62C,lbl_80379167,0);
          }
        }
      }
      if ((lbl_8047B618 & 0x40) != 0) {
        uVar4 = fightTargetGetPtrAsNowFightType(0x11,0);
        cVar13 = fightOutPokemonCheckFightOut();
        if (cVar13 == 1) {
          uVar4 = pokemonGetStatus(uVar4,0,0xd9,0);
          fightWazaBiosCopy(auStack_d8,uVar4);
          uVar2 = fightOutPokemonGetPokemonPtr(uVar2);
          uVar12 = pokemonGetStatus(uVar2,0,0x83,0);
          wazaSetStatus(uVar4,0,0x2d,0,uVar12);
          fightWazaInitJoutai(uVar4);
          fn_80211B94(lbl_8047B62C,lbl_80378D54,0);
          fightWazaBiosCopy(uVar4,auStack_d8);
        }
        lbl_8047B618 = lbl_8047B618 & 0xffffffbf;
      }
      uVar4 = fightFloorGetStatus(0,0,0x4b,0);
      fightFloorSetStatus(0,0,0x4b,0,iVar1);
      fn_80211B94(lbl_8047B62C,puVar20,0);
      fightFloorSetStatus(0,0,0x4b,0,uVar4);
    }
  }
  lbl_8047B610 = lbl_8047B610 + 7;
  return;
}
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#pragma optimize_for_size on
void fn_80224740(void)

{
    extern u32 fn_801F025C();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 lbl_8047B625;
    extern u16 lbl_80279EF4[];
  u32 uVar2;
  u32 sVar1;
  u8 cVar3;
  u8* pc;

  uVar2 = fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  sVar1 = lbl_80279EF4[lbl_80478D78[3]];
  if ((sVar1 != 0) && (cVar3 = fn_802026E4(), cVar3 == 1)) {
    fn_80202810(uVar2,sVar1);
  }
  pc = lbl_8047B610;
  lbl_80478D78[3] = 0;
  pc += 2;
  lbl_8047B625 = 0;
  lbl_8047B610 = pc;
  return;
}
#pragma optimize_for_size reset
#undef fn_801F025C
#undef fn_80202810
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
#undef fn_8012640C
#undef fn_801F025C
#undef fn_801F4C14
#undef fn_802037DC
#undef fn_80209618
#undef fn_8020990C
#undef fn_80209960
#undef fn_80262334
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
    initialEffectDamage = wazaGetStatus(move, 0, 0x2e, 0);
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
                            relativeRaw &= 0xffff;
                            pokemonSetStatus((void*)target, 0, 0x103, 0, relativeRaw);
                            pokemonSetStatus((void*)target, 0, 0x11d, 0, relativeRaw);
                        } else {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ally, floorId);
                            relativeRaw &= 0xffff;
                            pokemonSetStatus((void*)target, 0, 0x103, 0, relativeRaw);
                            pokemonSetStatus((void*)target, 0, 0x11d, 0, relativeRaw);
                        }
                    }
                    break;
                case 2:
                    if ((lbl_8047B618 & 0x100000) == 0) {
                        pokemonSetStatus((void*)target, 0, 0x104, 0, effectDamage);
                        pokemonSetStatus((void*)target, 0, 0x11e, 0, effectDamage);
                        if (lbl_8047B610[1] == 0x12) {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floorId);
                            relativeRaw &= 0xffff;
                            pokemonSetStatus((void*)target, 0, 0x105, 0, relativeRaw);
                            pokemonSetStatus((void*)target, 0, 0x11f, 0, relativeRaw);
                        } else {
                            relativeRaw = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(ally, floorId);
                            relativeRaw &= 0xffff;
                            pokemonSetStatus((void*)target, 0, 0x105, 0, relativeRaw);
                            pokemonSetStatus((void*)target, 0, 0x11f, 0, relativeRaw);
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
            pokemon = (s16)finalHp;
            if ((u8)menuIsCheck(maxHp) == 0) {
                fn_80265598(target, floorId, 1);
            }
            menuFightStatusStartAnimHP(maxHp, pokemon);
        }
    } else {
        target = fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
        if (pokemonGetStatus(target, 0, 0x11b, 0) == 0) {
            pokemonSetStatus((void*)target, 0, 0x11b, 0, 0xffff);
        }
    }
    lbl_8047B610 += 2;
}

void fn_80226F0C(void) {
    extern u32 lbl_8047B618;
    extern void _threadSwitch();
    extern u8 fn_801DA5C4();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void fightMenuAllFightOutPokemonCloseStatusMenu();
    u32 index;
    u32 move;
    u32 attacker;

    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    move = fightOutPokemonGetUseWazaDataId(attacker);
    pokemonGetStatus(attacker, 0, 0xd9, 0);
    fightTargetGetPtrAsNowFightType(0x12, 0);
    if ((lbl_8047B618 & 0x80) != 0 && (u16)move != 0x90) {
        if ((u16)move == 0xa4) {
            index = lbl_8047B610[1];
            do {
                if (fn_801DA5C4(index) == 1) {
                    break;
                }
                _threadSwitch();
            } while (1);
        }
    }
    if ((u8)index == 2 || (u8)index == 6) {
        fightMenuAllFightOutPokemonCloseStatusMenu(0);
    }
    lbl_8047B610 += 2;
    return;
}
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
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_8012640C pokemonGetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_802040E8 fightOutPokemonGetSoubiItemDataId
#define fn_80203FE4 fightOutPokemonGetSoubiItemSoubiDataId
#define fn_80203EDC figthOutPokemonGetSoubiItemBuff
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_8011BBD8 wazaSetStatus
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
#undef fn_801F4C14
#undef fn_802097C8
#undef fn_8020981C
#undef fn_801254B4
#undef fn_8011BBD8
#undef fn_80205B8C
#undef fn_80203EDC
#undef fn_80203FE4
#undef fn_802040E8
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_80205184
#undef fn_801F025C
#pragma optimize_for_size reset
#pragma optimize_for_size on
void fn_802274F0(u32 r3, char r4, char r5, char r6)
{
    struct CopyBlk802274F0 { u32 data[43]; };
    extern u32 zokuseiGetWazaJoutai();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u8 fightFloorGetNowTenkouDataId();
    extern void pokemonSetStatus();
    extern u32 pokemonGetStatus();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetZokuseiDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u8 fightWazaIsJoutaiSousai();
    extern u8 fightWazaIsJoutaiDataId();
    extern u8 fn_802026E4();
    extern void fn_802279F8();
    extern u32 lbl_8047B618;
  u32 uVar3;
  u32 uVar4;
  u32 uVar10;
  u32 uVar11;
  u16 sVar12;
  u32 iVar6;
  u32 uVar5;
  u16 uVar13;
  u32 uVar7;
  u32 uVar8;
  u32 uVar9;
  u32 sVar14;
  u8 cVar15;
  u8 iVar19;
  u8 flag40;
  u8 flag43;
  struct CopyBlk802274F0 backup;

  flag40 = 0;
  flag43 = 0;
  uVar4 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar10 = fightOutPokemonGetZokuseiDataId(uVar4,0);
  uVar11 = fightOutPokemonGetZokuseiDataId(uVar4,1);
  fightOutPokemonGetTokuseiDataId(uVar4);
  uVar5 = fightOutPokemonGetUseWazaDataId((void*)uVar4);
  sVar12 = wazaGetStatus(0,uVar5,7,0);
  iVar6 = pokemonGetStatus(uVar4,0,0xd9,0);
  uVar13 = wazaGetStatus(iVar6,0,0x30,0);
  uVar7 = fightTargetGetPtrAsNowFightType(0x12,0);
  uVar8 = fightOutPokemonGetZokuseiDataId(uVar7,0);
  uVar9 = fightOutPokemonGetZokuseiDataId(uVar7,1);
  sVar14 = fightOutPokemonGetTokuseiDataId(uVar7);
  if (iVar6 != 0) {
    backup = *(struct CopyBlk802274F0*)iVar6;
  }
  if ((((uVar5 & 0xffff) != 0xa5) && ((uVar5 & 0xffff) != 0x164)) &&
     ((r6 != 1 || (sVar12 != 0)))) {
    if (((r3 & 0xff) == 1) &&
        (((u16)uVar10 == uVar13 || ((u16)uVar11 == uVar13)))) {
      s32 damage = wazaGetStatus(iVar6,0,0x2d,0);
      wazaSetStatus(iVar6,0,0x2d,0,(damage * 0xf) / 10);
    }
    if (((u16)sVar14 == 0x1a) && (uVar13 == 4)) {
      if (r5 == 1) {
        fightFloorSetStatus(0,0,0x3b,0,0x43);
        fightFloorSetStatus(0,0,0x3b,0,0x40);
      pokemonSetStatus((void*)uVar7,0,0xf3,0,0);
      pokemonSetStatus((void*)uVar7,0,0xf4,0,9);
      }
      lbl_80478D78[6] = 4;
    }
    else {
      cVar15 = fn_802026E4(uVar7,0x19);
      if ((cVar15 != 1) ||
         ((((uVar8 & 0xffff) != 7 && ((uVar9 & 0xffff) != 7)) || (1 < uVar13)))) {
        uVar3 = zokuseiGetWazaJoutai(uVar13,uVar8);
        fn_802279F8(iVar6,uVar3,uVar5,r3);
        if ((uVar8 & 0xffff) != (uVar9 & 0xffff)) {
          uVar3 = zokuseiGetWazaJoutai(uVar13,uVar9);
          fn_802279F8(iVar6,uVar3,uVar5,r3);
        }
      }
    }
    if ((u16)sVar14 == 0x19) {
      u16 moveEffect = wazaGetStatus(0,uVar5,9,0);
      if ((moveEffect == 0x97) &&
          (cVar15 = fightFloorGetNowTenkouDataId(0,0), cVar15 == 1)) {
        iVar19 = 2;
      }
      else if ((((moveEffect == 0x91) || ((moveEffect == 0x27 || (moveEffect == 0x4b)))) ||
                (moveEffect == 0x97)) || ((moveEffect == 0x9b || (moveEffect == 0x1a)))) {
        if ((lbl_8047B618 & 0x8000000) != 0) {
          iVar19 = 1;
        }
        else {
          iVar19 = 2;
        }
      }
      else {
        iVar19 = 2;
      }
      if (iVar19 == 2) {
        if (fightWazaIsJoutaiDataId(iVar6,0x41) != 0 &&
            fightWazaIsJoutaiSousai(iVar6) != 1) {
          goto skip_status_19;
        }
        if (sVar12 == 0) {
          goto skip_status_19;
        }
        if (r5 == 1) {
          fightFloorSetStatus(0,0,0x3b,0,0x40);
      pokemonSetStatus((void*)uVar7,0,0xf3,0,0);
      pokemonSetStatus((void*)uVar7,0,0xf4,0,9);
        }
        lbl_80478D78[6] = 3;
      }
    skip_status_19:;
    }
    cVar15 = fightWazaIsJoutaiDataId(iVar6,0x43);
    if (cVar15 == 1) {
    pokemonSetStatus((void*)uVar4,0,0x108,0,1);
    }
  }
  if ((r3 & 0xff) == 0) {
    if (fightWazaIsJoutaiDataId(iVar6,0x40) == 1) {
      flag40 = 1;
    }
    if (fightWazaIsJoutaiDataId(iVar6,0x43) == 1) {
      flag43 = 1;
    }
    if (iVar6 != 0) {
      *(struct CopyBlk802274F0*)iVar6 = backup;
    }
    if (flag40 == 1) {
      fightFloorSetStatus(0,0,0x3b,0,0x40);
    }
    if (flag43 == 1) {
      fightFloorSetStatus(0,0,0x3b,0,0x43);
    }
  }
  if (r4 == 1) {
    lbl_8047B610 = lbl_8047B610 + 1;
  }
  return;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_802096E8 fightWazaIsHit
#define fn_802097C8 fightWazaWriteJoutaiDataId
#define fn_8020981C fightWazaCheckWriteJoutaiDataId
#define fn_8020990C fightWazaIsJoutaiDataId
#define fn_80209960 fightWazaInitJoutaiDataId
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
#pragma optimize_for_size reset
#pragma optimize_for_size on
void WS_CRITICAL_CHECK(void)

{
    extern void wazaSetStatus();
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fn_800E0C54();
    extern u32 wazaGetStatus();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern u8 fightFloorGetStatus();
  u16 targetAbility;
    extern u8 fn_802026E4();
    extern u32 figthOutPokemonGetPokemonDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u8 fightWazaCriticalDataBiosGetBunbo();
    extern u32 lbl_80478D60;
    extern u32 pokemonGetStatus();
  u32 score;
  u32 scoreTemp;
  u32 finalScore;
  u32 move209Bonus;
  u32 move209Status;
  int itemSpeciesBonus2;
  int itemSpeciesBonus1;
  u32 move43Bonus;
  u32 item;
  u32 attacker;
    extern u32 fightTargetGetPtrAsNowFightType();
  u32 target;
  u32 item41Bonus;
  u32 maximum;
  u32 maximumTemp;
  u16 random;
  u32 status;
    extern void fightWazaCriticalDataBiosGetPtr();
  u32 move;
  u32 denominator;

  attacker = fightTargetGetPtrAsNowFightType(0x11,0);
  fightOutPokemonGetTokuseiDataId(attacker);
  item = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
  status = pokemonGetStatus(attacker,0,0xd9,0);
  move = fightOutPokemonGetUseWazaDataId((void*)attacker);
  {
    u32 species;
    species = figthOutPokemonGetPokemonDataId(attacker);
    target = fightTargetGetPtrAsNowFightType(0x12,0);

    itemSpeciesBonus1 = 0;
    if ((u16)item == 0x3f && (u16)species == 0x71) {
      itemSpeciesBonus1 = 1;
    }
    itemSpeciesBonus2 = 0;
    if ((u16)item == 0x42 && (u16)species == 0x53) {
      itemSpeciesBonus2 = 1;
    }
  }

  score = fn_802026E4(attacker,0xf);
  scoreTemp = __cntlzw(1 - (score & 0xff));
  score = scoreTemp >> 4;
  score = score & 0xffffffe;
  move43Bonus = (u16)wazaGetStatus(0,move,9,0) == 0x2b;
  score += (u16)wazaGetStatus(0,move,9,0) == 0x4b;
  score += (u16)((0, wazaGetStatus(0,move,9,0))) == 200;
  move209Status = wazaGetStatus(0,move,9,0);
  maximumTemp = (int)lbl_80478D60;
  maximum = maximumTemp - 1;
  if (itemSpeciesBonus2 && itemSpeciesBonus2) {
  }
  score = ((((score + ((u16)item == 0x29)) + move43Bonus) +
            itemSpeciesBonus1 * 2) +
           (((u16)move209Status == 0xd1) + itemSpeciesBonus2 * (0, 2)));
  finalScore = score & 0xffff;
  if (finalScore <= maximum) {
    goto score_ready;
  }
  finalScore = maximum & 0xffff;
score_ready:
  targetAbility = fightOutPokemonGetTokuseiDataId(target);
  if (targetAbility == 4) {
    goto outer_normal;
  }
  targetAbility = fightOutPokemonGetTokuseiDataId(target);
  if (targetAbility == 0x4b) {
    goto outer_normal;
  }
  if (fightFloorGetStatus(0,0,0x29,0) != 1) {
    goto outer_normal;
  }
  fightWazaCriticalDataBiosGetPtr(finalScore);
  denominator = fightWazaCriticalDataBiosGetBunbo();
  random = fn_800E0C54();
  if (random % (u8)denominator == 0) {
    goto critical;
  }
  if (fn_802026E4(attacker,0x3e) != 1 || (u16)move != 0x164) {
    goto inner_normal;
  }
  random = fn_800E0C54();
  if (random % 100 >= 0x5a) {
    goto inner_normal;
  }
critical:
  wazaSetStatus(status,0,0x2b,0,2);
  goto LAB_0022506c;
inner_normal:
  wazaSetStatus(status,0,0x2b,0,1);
  goto LAB_0022506c;
outer_normal:
  wazaSetStatus(status,0,0x2b,0,1);
LAB_0022506c:
  lbl_8047B610 = lbl_8047B610 + 1;
  return;
}
#pragma optimize_for_size reset
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_802096E8
#undef fn_802097C8
#undef fn_8020981C
#undef fn_8020990C
#undef fn_80209960
void fn_8022808C(void)

{
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 lbl_8047B618;
    u32 attacker;
    u32 statusSlot;
    u32 target;
    u32 ability;
    u32 move;
    u32 usedFlags;
    u8 deduction;
    u32 pokemon;
    u8 pp;
    u8 moveField;

    deduction = 1;
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    statusSlot = pokemonGetStatus(attacker, 0, 0xd9, 0);
    pokemon = fightOutPokemonGetPokemonPtr(attacker);
    target = fightTargetGetPtrAsNowFightType(0x12, 0);
    ability = fightOutPokemonGetTokuseiDataId(target);

    if ((lbl_8047B618 & 0xa00) == 0) {
        move = fightOutPokemonGetUseWazaDataId(attacker);
        moveField = (u8)wazaGetStatus(0, move, 5, 0);
        statusSlot = (s8)wazaGetStatus(statusSlot, 0, 0x26, 0);
        pp = (u8)pokemonGetStatus(pokemon, 0, 0x80, (s8)statusSlot);

        if ((s32)pokemonGetStatus(attacker, 0, 0x118, 0) == 0) {
            switch (moveField) {
            case 6:
                deduction = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
                    0, 0x2e, 0, attacker) + 1;
                break;
            case 4:
            case 7:
                deduction = fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(
                    0, 0x2e, 2, attacker) + 1;
                break;
            default:
                if (attacker != target && (u16)ability == 0x2e) {
                    deduction = 2;
                }
                break;
            }
        }

        if (pp != 0) {
            pokemonSetStatus((void*)attacker, 0, 0x111, 0, 1);
            if (deduction < pp) {
                pp -= deduction;
            } else {
                pp = 0;
            }
            pokemonSetStatus((void*)pokemon, 0, 0x80, (s8)statusSlot, pp);

            if (fn_802026E4(attacker, 0x10) == 0 &&
                fn_802026E4(attacker, 0x31) == 1) {
                usedFlags = fn_80201890(attacker, 0x31);
                if ((usedFlags & (1 << (u8)statusSlot)) == 0 &&
                    fightOutPokemonIsUseHensinBuff(attacker) == 1) {
                    fightOutPokemonSetHensinPokemonStatusId(
                        attacker, 0x80, (u8)statusSlot, 0);
                }
            }
        }
    }

    lbl_8047B618 &= 0xfffff7ff;
    lbl_8047B610++;
}
void fn_802282D8(void)

{
    extern u8 fightMenuWazaOutMsg();
    extern void fightOutPokemonGetNicknamePtr();
    extern void _threadSwitch();
    extern u8 fn_801DA94C();
    extern void fightTrainerGetStatus();
  u32 uVar3;
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
  u16 uVar6;
    extern void fn_8026532C();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u8 fn_801DDD28();
  u8 cVar9;
    extern void fn_801DA8C4();
    extern void fightMenuCloseMsg();
    extern u32 pokemonGetStatus();
    extern u8 lbl_80478D78[1];
  u32 uVar2;
    extern u32 lbl_8047B618;
    extern void fn_80265598();
  u32 uVar1;
    extern void* lbl_8047B62C;
    extern void fn_801DA9E8();
    extern void fn_80211B94();
  u32 iVar5;

  uVar6 = fightFloorGetStatus(0,0,0x14,0);
  if ((lbl_8047B618 & 0x600) == 0) {
    extern u8 lbl_80378964[];
    u16 uVar7;
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    u8 cVar8;
    u32 iVar4;
    uVar1 = fightTargetGetPtrAsNowFightType(0x11,0);
    uVar2 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,uVar1);
    fightOutPokemonGetNicknamePtr(uVar1);
    uVar3 = fightOutPokemonGetUseWazaDataId(uVar1);
    iVar4 = pokemonGetStatus(uVar1,0,0xee,0);
    fightTrainerGetStatus(uVar2,0,0x4c,0);
    uVar7 = fightFloorGetStatus(0,0,0x14,0);
    iVar5 = pokemonGetStatus(uVar1,0,0xee,0);
    if ((iVar5 != 0) && (cVar8 = fn_801DDD28(iVar5,0x9e,4,0), cVar8 != 0)) {
      fn_801DA9E8(iVar5,0x9e,4);
      fn_80265598(uVar1,uVar7,1);
    }
    fn_80265598(uVar1,uVar6,1);
    cVar8 = fightMenuWazaOutMsg(uVar1,uVar3);
    if (iVar4 != 0) {
      while (1) {
        cVar9 = fn_801DA94C(iVar4,0x9e,4);
        if (cVar9 == 0) break;
        _threadSwitch();
      }
      fn_801DA8C4(iVar4,0x9e,4);
    }
    else {
      fn_80211B94(lbl_8047B62C,lbl_80378964,0);
    }
    if (cVar8 == 1) {
      fightMenuCloseMsg();
    }
    fn_8026532C(uVar1,uVar6,0);
    lbl_8047B618 = lbl_8047B618 | 0x400;
  }
  lbl_8047B610 += 1;
  lbl_80478D78[7] = 0;
  return;
}
#pragma opt_propagation off
void fn_802284B0(void)
{
    extern u32 fightFloorGetStatus();
    extern void fightFloorSetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern s32 figthOutPokemonGetSoubiItemBuff();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetWazaZokuseiDataId();
    extern u32 fightOutPokemonIsJoutaiKie();
    extern u32 fightFloorGetNowTenkouDataId();
    extern u32 wazaGetStatus();
    extern u8 fightWazaIsHit();
    extern s32 fightAbicntFitMinMax();
    extern void fightWazaHitKakurituDataBiosGetPtr();
    extern u32 fightWazaHitKakurituDataBiosGetKake();
    extern u32 fightWazaHitKakurituDataBiosGetWaru();
    extern u8 fightWazaCheckWriteJoutaiDataId();
    extern void fightWazaWriteJoutaiDataId();
    extern s32 fn_800E0C54();
    extern u8 fn_8010C4A0();
    extern u16 fn_80201D84();
    extern u8 fn_802026E4();
    extern void fn_802274F0();
    extern u8 fn_80228DAC();
    extern u8 fn_80229934();
    extern u8 fn_8022DCB8();
    extern u32 lbl_8047B618;
    u32 hitAttacker;
    u32 hitStatus;
    u32 hitDefender;
    u32 attackerSide;
    u32 defenderSide;
    u32 defenderNow;
    u8 handled;
    u32 usedMove;
    u32 blockDefender;
    u8 moveFlag;
    u8 canBlock;
    u32 attacker;
    u32 defender;
    u32 moveStatus;
    u8 moveCategory;
    u32 moveType;
    u32 heldItem;
    s32 itemBuff;
    u32 attackerAbility;
    u32 defenderAbility;
    u32 weather;
    u32 move;
    u32 relativeTarget;
    u16 floorTarget;

    floorTarget = fightFloorGetStatus(0, 0, 0x14, 0);
    usedMove = *(u16*)(lbl_8047B610 + 5);
    move = usedMove;
    attacker = fightTargetGetPtrAsNowFightType(0x11, 0);
    relativeTarget = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(attacker, floorTarget);
    moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
    attackerAbility = fightOutPokemonGetTokuseiDataId(attacker);
    defender = fightTargetGetPtrAsNowFightType(0x12, 0);
    defenderAbility = fightOutPokemonGetTokuseiDataId();
    heldItem = fightOutPokemonGetSoubiItemSoubiDataId(defender);
    itemBuff = figthOutPokemonGetSoubiItemBuff(defender);
    weather = fightFloorGetNowTenkouDataId(0, 1);
    if (((move & 0xFFFF) == 0xFFFF) || ((move & 0xFFFF) == 0xFFFE)) {
        if (((u16)move == 0xFFFF) && (fn_802026E4(defender, 0x1D) == 1) &&
            ((u16)relativeTarget == fn_80201D84(defender, 0x1D))) {
            lbl_8047B610 += 7;
            return;
        }
        if ((u16)fightOutPokemonIsJoutaiKie(defender) != 0) {
            lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
            return;
        }

        usedMove = fightTargetGetPtrAsNowFightType(0x11, 0);
        usedMove = fightOutPokemonGetUseWazaDataId(usedMove);
        blockDefender = fightTargetGetPtrAsNowFightType(0x12, 0);
        handled = 0;
        moveFlag = wazaGetStatus(0, usedMove, 0xE, 0);
        if ((fn_802026E4(blockDefender, 0x2B) == 1) && (moveFlag == 1)) {
            canBlock = 1;
        } else {
            canBlock = 0;
        }
        if (canBlock == 1) {
            fightFloorSetStatus(0, 0, 0x3B, 0, 0x40);
            hitAttacker = fightTargetGetPtrAsNowFightType(0x11, 0);
            hitStatus = pokemonGetStatus(hitAttacker, 0, 0xD9, 0);
            hitDefender = fightTargetGetPtrAsNowFightType(0x12, 0);
            if (fightWazaIsHit(hitStatus) == 0) {
                pokemonSetStatus(hitDefender, 0, 0xF3, 0, 0);
                pokemonSetStatus(hitDefender, 0, 0xF4, 0, 9);
                lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
            } else {
                attackerSide = fightTargetGetPtrAsNowFightType(
                    2, fightTargetGetPtrAsNowFightType(0x11, 0));
                defenderNow = fightTargetGetPtrAsNowFightType(0x12, 0);
                defenderSide = fightTargetGetPtrAsNowFightType(2, defenderNow);
                if ((fn_802026E4(defenderNow, 0x15) == 1) &&
                    (attackerSide != defenderSide) &&
                    ((lbl_8047B618 & 0x01000000) == 0)) {
                    lbl_8047B618 |= 0x40;
                }
                if (fn_8022DCB8(hitAttacker, hitDefender, 0) == 0) {
                    lbl_8047B610 += 7;
                }
            }
            handled = 1;
            lbl_80478D78[6] = handled;
        }
        if (handled == 0) {
            lbl_8047B610 += 7;
        }
        return;
    }

    if ((move & 0xFFFF) == 0) {
        move = fightOutPokemonGetUseWazaDataId(attacker);
        moveType = fightOutPokemonGetWazaZokuseiDataId(attacker);
    } else {
        moveType = wazaGetStatus(0, move, 3, 0) & 0xFFFF;
    }
    moveCategory = wazaGetStatus(0, move, 5, 0);
    if (fn_80229934(move, attacker, defender) == 1) {
        fightFloorSetStatus(0, 0, 0x3B, 0, 0x45);
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    } else {
        usedMove = fightTargetGetPtrAsNowFightType(0x11, 0);
        usedMove = fightOutPokemonGetUseWazaDataId(usedMove);
        blockDefender = fightTargetGetPtrAsNowFightType(0x12, 0);
        handled = 0;
        moveFlag = wazaGetStatus(0, usedMove, 0xE, 0);
        if ((fn_802026E4(blockDefender, 0x2B) == 1) && (moveFlag == 1)) {
            canBlock = 1;
        } else {
            canBlock = 0;
        }
        if (canBlock == 1) {
            fightFloorSetStatus(0, 0, 0x3B, 0, 0x40);
            hitDefender = fightTargetGetPtrAsNowFightType(0x11, 0);
            hitStatus = pokemonGetStatus(hitDefender, 0, 0xD9, 0);
            hitAttacker = fightTargetGetPtrAsNowFightType(0x12, 0);
            if (fightWazaIsHit(hitStatus) == 0) {
                pokemonSetStatus(hitAttacker, 0, 0xF3, 0, 0);
                pokemonSetStatus(hitAttacker, 0, 0xF4, 0, 9);
                lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
            } else {
                attackerSide = fightTargetGetPtrAsNowFightType(
                    2, fightTargetGetPtrAsNowFightType(0x11, 0));
                defenderNow = fightTargetGetPtrAsNowFightType(0x12, 0);
                defenderSide = fightTargetGetPtrAsNowFightType(2, defenderNow);
                if ((fn_802026E4(defenderNow, 0x15) == 1) &&
                    (attackerSide != defenderSide) &&
                    ((lbl_8047B618 & 0x01000000) == 0)) {
                    lbl_8047B618 |= 0x40;
                }
                if (fn_8022DCB8(hitDefender, hitAttacker, move) == 0) {
                    lbl_8047B610 += 7;
                }
            }
            handled = 1;
            lbl_80478D78[6] = handled;
        }

        if ((handled == 0) && (fn_80228DAC(attacker, defender, move) == 0)) {
            u8 attackerAccuracy;
            u8 defenderEvasion;
            u32 accuracyState;
            u32 baseAccuracy;
            s8 stage;
            s32 adjustedStage;
            u16 accuracyProduct;
            u16 accuracy;

            attackerAccuracy = pokemonGetStatus(attacker, 0, 0xEB, 0);
            defenderEvasion = pokemonGetStatus(defender, 0, 0xEC, 0);
            adjustedStage = fn_802026E4(defender, 0x19);
            accuracyState = (u8)adjustedStage;
            adjustedStage = attackerAccuracy + (12 - defenderEvasion);
            stage = adjustedStage - 6;
            if (accuracyState == 1) {
                stage = attackerAccuracy;
            }
            stage = fightAbicntFitMinMax(stage);
            baseAccuracy = (u8)wazaGetStatus(0, move, 6, 0);
            if (((u8)weather == 1) &&
                ((u16)wazaGetStatus(0, move, 9, 0) == 0x98)) {
                baseAccuracy = 0x32;
            }

            fightWazaHitKakurituDataBiosGetPtr(stage);
            accuracyProduct = baseAccuracy *
                (u8)fightWazaHitKakurituDataBiosGetKake();
            fightWazaHitKakurituDataBiosGetPtr(stage);
            accuracy = (s32)accuracyProduct /
                (s32)(u8)fightWazaHitKakurituDataBiosGetWaru();
            if ((u16)attackerAbility == 0xE) {
                accuracy = (accuracy * 0x82) / 100;
            }
            if (((u8)weather == 3) && ((u16)defenderAbility == 8)) {
                accuracy = (accuracy * 0x50) / 100;
            }
            if (((u16)attackerAbility == 0x37) &&
                (fn_8010C4A0(moveType) == 1)) {
                accuracy = (accuracy * 0x50) / 100;
            }
            if ((u16)heldItem == 0x16) {
                accuracy = (accuracy * (100 - itemBuff)) / 100;
            }

            {
                u32 random = fn_800E0C54();
                s32 roll = (u16)random % 100 + 1;

                if (roll > accuracy) {
                    u8* resultBase;
                    u8 oldResult;

                    if (fightWazaCheckWriteJoutaiDataId(moveStatus, 0x40) == 2) {
                        fightWazaWriteJoutaiDataId(moveStatus, 0x40, 0);
                    }
                    if (((u16)fightFloorGetStatus(0, 0, 0x19, 0) >= 2) &&
                        ((moveCategory == 4) || (moveCategory == 6))) {
                        lbl_80478D78[6] = 2;
                    } else {
                        lbl_80478D78[6] = 0;
                    }
                    resultBase = lbl_80478D78;
                    oldResult = resultBase[6];
                    fn_802274F0(0, 0, 0, 1);
                    if ((resultBase[6] == 3) &&
                        ((oldResult == 2) || (oldResult == 0))) {
                        resultBase[6] = oldResult;
                    }
                }
            }

            {
                hitDefender = fightTargetGetPtrAsNowFightType(0x11, 0);
                hitStatus = pokemonGetStatus(hitDefender, 0, 0xD9, 0);
                hitAttacker = fightTargetGetPtrAsNowFightType(0x12, 0);
                if (fightWazaIsHit(hitStatus) == 0) {
                    pokemonSetStatus(hitAttacker, 0, 0xF3, 0, 0);
                    pokemonSetStatus(hitAttacker, 0, 0xF4, 0, 9);
                    lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
                    return;
                } else {
                    attackerSide = fightTargetGetPtrAsNowFightType(
                        2, fightTargetGetPtrAsNowFightType(0x11, 0));
                    defenderNow = fightTargetGetPtrAsNowFightType(0x12, 0);
                    defenderSide = fightTargetGetPtrAsNowFightType(2, defenderNow);
                    if ((fn_802026E4(defenderNow, 0x15) == 1) &&
                        (attackerSide != defenderSide) &&
                        ((lbl_8047B618 & 0x01000000) == 0)) {
                        lbl_8047B618 |= 0x40;
                    }
                    if (fn_8022DCB8(hitDefender, hitAttacker, move) == 0) {
                        lbl_8047B610 += 7;
                    }
                }
            }
        }
    }
}
#pragma opt_propagation reset
#define fn_8011BEB4 wazaGetStatus
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F453C fightFloorGetNowTenkouDataId
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_802096E8 fightWazaIsHit
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
  u32 lateMoveType;
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
  if (cVar11 != 1) {
    goto first_check_done;
  }
  sVar10 = fn_80201D84(r4,0x1d);
  if (sVar10 == sVar9) {
    uVar2 = fn_801F025C(0x11,0);
    uVar3 = (int)fn_8012640C(uVar2,0,0xd9,0);
    uVar4 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar3);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
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
first_check_done:
  if (((lbl_8047B618 & 0x10000) == 0) &&
     (cVar11 = fn_802026E4(r4,0x1f), cVar11 == 1)) {
    fn_801F4C14(0,0,0x3b,0,0x40);
    uVar4 = fn_801F025C(0x11,0);
    uVar2 = (int)fn_8012640C(uVar4,0,0xd9,0);
    uVar3 = fn_801F025C(0x12,0);
    cVar11 = fn_802096E8(uVar2);
    if (cVar11 == 0) {
    fn_801254B4((void*)uVar3,0,0xf3,0,0);
    fn_801254B4((void*)uVar3,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
    }
    else {
      uVar2 = fn_801F025C(0x11,0);
      uVar5 = fn_801F025C(2,uVar2);
      uVar2 = fn_801F025C(0x12,0);
      uVar6 = fn_801F025C(2,uVar2);
      cVar11 = fn_802026E4(uVar2,0x15);
      if (((cVar11 == 1) && (uVar5 != uVar6)) &&
         ((lbl_8047B618 & 0x1000000) == 0)) {
        lbl_8047B618 = lbl_8047B618 | 0x40;
      }
      cVar11 = fn_8022DCB8(uVar4,uVar3,r5);
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
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
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
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
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
  lateMoveType = fn_8011BEB4(0,r5,9,0);
  lateMoveType = (u16)lateMoveType;
  cVar11 = (int)fn_801F453C(0,1);
  if ((cVar11 == 2) && (lateMoveType == 0x98)) {
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
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
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
    lateMoveType = fn_8011BEB4(0,r5,9,0);
    if ((lateMoveType == 0x11) || (lateMoveType == 0x4e)) {
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
    fn_801254B4((void*)uVar4,0,0xf3,0,0);
    fn_801254B4((void*)uVar4,0,0xf4,0,9);
    lbl_8047B610 = (u8*)*(u32 *)(lbl_8047B610 + 1);
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
#undef fn_8011BEB4
#undef fn_801254B4
#undef fn_8012640C
#undef fn_801F0134
#undef fn_801F025C
#undef fn_801F453C
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_802096E8
u32 fn_80229704(u32 statusId, u32 target)

{
    extern u8 fn_80077B3C();
    extern u8 fn_80077B60();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u32 fightSideGetValidFightTrainerPtr();
    extern u32 fightTrainerGetValidFightPokemonPtr();
    extern u8 fightPokemonCheckWriteJoutaiDataId();
    extern u8 fightPokemonCheckFightOut();
    u8 disabled8;
    u8 disabled7;
    u32 side;
    u16 trainerCount;
    u16 found;
    u32 trainer;
    u16 trainerIndex;
    u32 pokemonIndex;
    u32 pokemon;
    u32 enabled;

    enabled = (u8)fightFloorGetStatus(0, 0, 0x34, 0);
    disabled8 = fn_80077B60();
    disabled7 = fn_80077B3C();
    if (enabled == 1) {
        if ((u16)statusId == 8) {
            if ((u8)disabled8 != 1) {
                found = 0;
                side = fightTargetGetPtrAsNowFightType(2, target);
                trainerCount = (u16)fightFloorGetStatus(0, 0, 0x16, 0);
                fightFloorGetStatus(0, 0, 0x17, 0);
                for (trainerIndex = 0; trainerIndex < trainerCount;
                     trainerIndex++) {
                    trainer = fightSideGetValidFightTrainerPtr(side, trainerIndex);
                    if (trainer != 0) {
                        for (pokemonIndex = 0; (u16)pokemonIndex < 6; pokemonIndex++) {
                            pokemon = fightTrainerGetValidFightPokemonPtr(
                                trainer, pokemonIndex);
                            if (pokemon != 0 && fightPokemonCheckFightOut(pokemon) != 0 &&
                                fightPokemonCheckWriteJoutaiDataId(pokemon, statusId) == 1) {
                                found++;
                            }
                        }
                    }
                }
                if (found >= 1) {
                    return 1;
                }
            }
        } else if ((u16)statusId == 7 && disabled7 != 1) {
            found = 0;
            side = fightTargetGetPtrAsNowFightType(2, target);
            trainerCount = (u16)fightFloorGetStatus(0, 0, 0x16, 0);
            fightFloorGetStatus(0, 0, 0x17, 0);
            for (trainerIndex = 0; trainerIndex < trainerCount; trainerIndex++) {
                trainer = fightSideGetValidFightTrainerPtr(side, trainerIndex);
                if (trainer != 0) {
                    for (pokemonIndex = 0; (u16)pokemonIndex < 6; pokemonIndex++) {
                        pokemon = fightTrainerGetValidFightPokemonPtr(trainer, pokemonIndex);
                        if (pokemon != 0 && fightPokemonCheckFightOut(pokemon) != 0 &&
                            fightPokemonCheckWriteJoutaiDataId(pokemon, 7) == 1) {
                            found++;
                        }
                    }
                }
            }
            if (found >= 1) {
                return 1;
            }
        }
    }
    return 0;
}
u8 fn_80229934(u32 r3, u32 r4, u32 r5)

{
    extern u8 fn_80077AAC();
    extern u8 fn_80077AD0();
    extern u8 fn_80077B18();
    extern void fn_80077B3C();
    extern u8 fn_80077B60();
    extern u16 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u32 fightSideGetFightPokemonNum();
    extern int fightSideGetValidFightTrainerPtr();
    extern int fightTrainerGetValidFightPokemonPtr();
    extern u8 fightPokemonCheckWriteJoutaiDataId();
    extern u8 fightPokemonCheckFightOut();
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

  cVar8 = fightFloorGetStatus(0,0,0x34,0);
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
      uVar1 = fightTargetGetPtrAsNowFightType(2,r4);
      uVar4 = fightFloorGetStatus(0,0,0x16,0);
      uVar5 = fightFloorGetStatus(0,0,0x17,0);
      uVar6 = fightSideGetFightPokemonNum(uVar1,uVar4,uVar5);
      if ((uVar6 <= 1) && (cVar10 != 1)) {
        return 1;
      }
    }
    else if ((uVar13 == 0x52) || (uVar13 == 0x31)) {
      if (cVar11 != 1) {
        return 1;
      }
    }
    else {
      sVar7 = wazaGetStatus(0,r3,9,0);
      if (sVar7 == 1) {
        u32 found;
        u32 side;
        u16 trainerCount;
        u16 trainerIndex;
        u32 trainer;
        u32 pokemonIndex;
        u32 pokemon;

        found = 0;
        side = fightTargetGetPtrAsNowFightType(2,r5);
        trainerCount = fightFloorGetStatus(0,0,0x16,0);
        fightFloorGetStatus(0,0,0x17,0);
        for (trainerIndex = 0; trainerIndex < trainerCount; trainerIndex++) {
          trainer = fightSideGetValidFightTrainerPtr(side,trainerIndex);
          if (trainer != 0) {
            for (pokemonIndex = 0; (pokemonIndex & 0xffff) < 6; pokemonIndex++) {
              pokemon = fightTrainerGetValidFightPokemonPtr(trainer,pokemonIndex);
              if (((pokemon != 0) && (cVar8 = fightPokemonCheckFightOut(), cVar8 != 0)) &&
                 (cVar8 = fightPokemonCheckWriteJoutaiDataId(pokemon,8), cVar8 == 1)) {
                found = found + 1;
              }
            }
          }
        }
        if ((cVar12 != 1) && ((u16)found >= 1)) {
          return 1;
        }
      }
    }
  }
  return 0;
}
void fn_80229C90(void)

{
    extern u32 wazaGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 fightWazaIsHit();
    extern u8 fn_802026E4();
    extern u8 fn_8022DCB8();
    extern u32 lbl_8047B618;
  u32 uVar1;
  u32 initialFlag;
  u8 cVar6;
  u8 cVar7;
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
  cVar7 = fn_802026E4(uVar1,0x2b);
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
      cVar6 = fn_802026E4(uVar2.value,0x15);
      if (cVar6 == 1) {
        if (uVar4 != uVar5) {
          if ((lbl_8047B618 & 0x1000000) == 0) {
            lbl_8047B618 = lbl_8047B618 | 0x40;
          }
        }
      }
      cVar6 = fn_8022DCB8(uVar1,uVar3.value,0);
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
void WS_HITCHECK(void)

{
    struct HitCheckLoopState {
      u32 attacker;
      u32 result;
    };
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern s8 pokemonSearchWazaDataId();
    extern u16 fn_801EF634();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fightFloorSetStatus();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonInitJoutaiKeep();
    extern u32 fn_80201890();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightWazaWriteJoutaiDataId();
    extern u8 fightWazaCheckWriteJoutaiDataId();
    extern void fightWazaBiosCopy();
    extern void fn_80211B94();
    extern u32 fn_8022A504();
    extern u32 fn_8022A6C8();
    extern u8 fn_8022F2F8();
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
  struct {
    u32 value;
  } uVar5;
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
  u16 uVar11;
  s32 ability;
  u16 attackerAbility;
  u16 moveEffect;
  u8 checkValue;
  u8 specialResult;
  s32 hitResult;
  u8 moveSlot;
  s32 status114;
  u8* nextPc;

  struct HitCheckLoopState loopState;

  iVar2 = fightTargetGetPtrAsNowFightType(0x11,0);
  uVar3 = fightOutPokemonGetPokemonPtr();
  iVar4 = fightTargetGetPtrAsNowFightType(0x12,0);
  floorState = fn_801EF634();
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
      cVar12 = fn_8022F2F8();
      if (cVar12 == 0) {
        fightOutPokemonGetTokuseiDataId(iVar2);
        uVar5.value = fightOutPokemonGetUseWazaDataId((void*)iVar2);
        ability = (u16)fightOutPokemonGetTokuseiDataId(iVar4);
        bVar1 = 0;
        switch (ability) {
          case 0x2b:
            if ((fightOutPokemonCheckFightOut(iVar4) != 0) &&
                ((u8)wazaGetStatus(0,uVar5.value,0x17,0) == 1)) {
              specialResult = fn_802026E4(iVar2,0x22);
              if (specialResult == 1) {
                lbl_8047B618 = lbl_8047B618 | 0x800;
              }
              bVar1 = 1;
              lbl_8047B610 = lbl_8037989E;
            }
            break;
          }
        if ((u8)bVar1 == 0) {
          iVar6 = pokemonGetStatus(iVar2,0,0xd9,0);
          uVar7 = fightOutPokemonGetUseWazaDataId((void*)iVar2);
          moveFlag = wazaGetStatus(0,uVar7,0xe,0);
          cVar13 = wazaGetStatus(0,uVar7,0xf,0);
          cVar14 = wazaGetStatus(iVar6,0,0x26,0);
          if (((cVar14 < 0) || ((uVar7 & 0xffff) == 0xa5)) || ((uVar7 & 0xffff) == 0x164)) {
            checkValue = 0;
          }
          else {
            checkValue = pokemonGetStatus(uVar3,0,0x80);
          }
          if (checkValue != 0 || (uVar7 & 0xffff) == 0xa5 ||
              (uVar7 & 0xffff) == 0x164) {
            goto normal_hit;
          }
          if (fn_802026E4(iVar2,0x22) != 0 || (lbl_8047B618 & 0x800200) != 0) {
            goto normal_hit;
          }
          {
            specialResult = fightWazaCheckWriteJoutaiDataId(iVar6,0x40);
            if (specialResult == 2) {
              fightWazaWriteJoutaiDataId(iVar6,0x40,0);
            }
            lbl_8047B610 = lbl_80379021;
            lbl_8047B618 = lbl_8047B618 | 0x80000;
            return;
          }
        normal_hit:
          {
            uVar8 = lbl_8047B618;
            lbl_8047B618 = uVar8 & 0xff7fffff;
            if ((uVar8 & 0x2000000) == 0) {
              if (fn_802026E4(iVar2,0x22) == 0) {
                hitResult = (u8)fn_8022A6C8(iVar2);
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
            specialResult = fn_802026E4(iVar4,0x37);
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
                  if (fn_802026E4(iVar4,0x10) != 0) {
                    goto after_hensin_update;
                  }
                  if (fn_802026E4(iVar4,0x31) != 1) {
                    goto after_hensin_update;
                  }
                  uVar8 = fn_80201890(iVar4,0x31);
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
              fn_80211B94(lbl_8047B62C,lbl_8037917B,0);
              if ((iVar6 != 0) && (iVar9 != 0)) {
                fightWazaBiosCopy(iVar9,iVar6);
                wazaSetStatus(iVar9,0,0x27,0,uVar7 & 0xffff);
              }
            }
            else {
              loopState.attacker = iVar2;
              loopState.result = 0;
              fightFloorLoopValidFightOutPokemon(0,fn_8022A504,&loopState,1);
              iVar6 = loopState.result;
              if (loopState.result != 0) {
                iVar9 = pokemonGetStatus(iVar2,0,0xd9,0);
                iVar6 = pokemonGetStatus(iVar6,0,0xd9,0);
                if ((iVar6 != 0) && (iVar9 != 0)) {
                  uVar11 = wazaGetStatus(iVar6,0,0x28,0);
                  fightWazaBiosCopy(iVar6,iVar9);
                  wazaSetStatus(iVar6,0,0x27,0,uVar11);
                }
              }
              status114 = pokemonGetStatus(iVar4,0,0x114,0);
              if (status114 == 1) {
                pokemonSetStatus((void*)iVar4,0,0x114,0,0);
                fn_80211B94(lbl_8047B62C,lbl_803796F3,0);
              }
              specialResult = fn_802026E4(iVar4,0x2b);
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
                    (specialResult = fn_802026E4(iVar2,0x22), specialResult == 1)) {
                  fightOutPokemonInitJoutaiKeep(iVar2);
                  fightFloorSetStatus(0,0,0x3b,0,0x40);
                  pokemonSetStatus((void*)iVar4,0,0xf3,0,0);
                  pokemonSetStatus((void*)iVar4,0,0xf4,0,9);
                  nextPc = *(u8* volatile*)&lbl_8047B610;
                  nextPc += 1;
                  lbl_80478D78[6] = 1;
                  lbl_8047B610 = nextPc;
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
#pragma optimize_for_size on
u32 fn_8022A504(u32 r3, u32 r4, u32* r5)

{
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 wazaGetStatus();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fn_802026E4();
    extern u16 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern s8 pokemonSearchWazaDataId();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fn_80201890();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightFloorSetStatus();
    extern void fn_80211B94();
    extern void* lbl_8047B62C;
    extern u8 lbl_8037919D[];
    void* context;
    u32* state;
    u32 pokemon;
    u8 moveSlot;
    s32 slot;
    u32 iVar7;
    u32 uVar1;
    u8 cVar4;
    u8 cVar5;
    u16 sVar3;
    s8 bVar6;
    u32 uVar2;

    context = (void*)r3;
    state = r5;
    iVar7 = *state;
    uVar1 = fightOutPokemonGetUseWazaDataId(iVar7);
    cVar4 = wazaGetStatus(0, uVar1, 0x10, 0);
    cVar5 = fightOutPokemonCheckFightOut(context);
    if (cVar5 == 0) {
        return 1;
    }
    cVar5 = fn_802026E4(context, 0x33);
    if ((cVar5 == 1) && (cVar4 == 1)) {
        if ((iVar7 != 0) && (context != 0) &&
            (sVar3 = fightOutPokemonGetTokuseiDataId(iVar7), sVar3 == 0x2e)) {
            pokemon = fightOutPokemonGetPokemonPtr(context);
            bVar6 = pokemonSearchWazaDataId(pokemon, 0x121);
            if (bVar6 >= 0) {
                slot = bVar6;
                cVar4 = pokemonGetStatus(pokemon, 0, 0x80, slot);
                if (cVar4 != 0) {
                    cVar4--;
                }
                pokemonSetStatus((void*)pokemon, 0, 0x80, slot, cVar4);
                moveSlot = bVar6;
                cVar5 = fn_802026E4(context, 0x10);
                if ((cVar5 == 0) &&
                    (cVar5 = fn_802026E4(context, 0x31), cVar5 == 1) &&
                    (uVar2 = fn_80201890(context, 0x31),
                     (uVar2 & (1 << (u32)moveSlot)) == 0) &&
                    (cVar5 = fightOutPokemonIsUseHensinBuff(context), cVar5 == 1)) {
                    fightOutPokemonSetHensinPokemonStatusId(context, 0x80, moveSlot, 0);
                }
            }
        }
        fightOutPokemonWriteJoutaiDataId(context, 0x33);
        fightFloorSetStatus(0, 0, 0x4b, 0, context);
        fn_80211B94(lbl_8047B62C, lbl_8037919D, 0);
        state[1] = (u32)context;
    }
    return 1;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011BBD8 wazaSetStatus
#define fn_8011BEB4 wazaGetStatus
#define fn_8011CE18 pokemonSeikakuDataBiosGetPtr
#define fn_8011FC74 pokemonIsDarkPokemon
#define fn_80123CD4 pokemonWazaCheckValid
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_801F0134 fightTargetGetTragetPtrToRelativeHostSideFightTargetId
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F4354 fightFloorGetFightOutPokemonPtrToFightTrainerPtr
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F8100 fightTrainerGetNamePtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FFEC8 fightOutPokemonCheckCanOutOkWazaBanme
#define fn_80203E0C figthOutPokemonGetLevel
#define fn_80203FE4 fightOutPokemonGetSoubiItemSoubiDataId
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_80208404 fightOutPokemonDarkPokemonEffect
#define fn_802085C4 fightOutPokemonWazaEffect
#define fn_802086E8 fightOutPokemonFreeWazaEffect
#define fn_80208750 fightOutPokemonLoadWazaEffect
#define fn_80208ED0 fightOutPokemonModosuEffect
#define fn_802099AC fightWazaCreate
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#pragma opt_lifetimes off
u32 fn_8022A6C8(u32 r3)

{
    extern u16 fn_800E0C54();
    extern u32 fn_800FA280();
    extern void fn_8011BBD8();
    extern u32 fn_8011BEB4();
    extern u32 fn_8011CC54();
    extern u32 fn_8011CC6C();
    extern u32 fn_8011CC84();
    extern u32 fn_8011CC9C();
    extern u32 fn_8011CCB4();
    extern u32 fn_8011CCCC();
    extern u32 fn_8011CCE4();
    extern void fn_8011CE18();
    extern u32 fn_8011FC74();
    extern u32 fn_80123CD4();
    extern void fn_80132A38();
    extern u32 fn_80142984();
    extern u32 fn_801F0134();
    extern u32 fn_801F025C();
    extern int fn_801F4354();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern u32 fn_801F8100();
    extern int fn_801F8A18();
    extern void fn_801FB1C0();
    extern void fn_801FBA24();
    extern u32 fn_801FFEC8();
    extern u32 fn_802026E4();
    extern void fn_80203E0C();
    extern void fn_80203FE4();
    extern u32 fn_80205B8C();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern void fn_80208404();
    extern void fn_80208554();
    extern void fn_802085C4();
    extern void fn_802086E8();
    extern void fn_80208750();
    extern void fn_80208ED0();
    extern void fn_802099AC(u32, s8, u16, u32, u32);
    extern int fn_8022B2CC();
    extern int fn_8022BE2C();
    extern int fn_80232110();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern u16 lbl_8047B60C;
    extern u32 lbl_8047B618;
    extern u8 lbl_803799ED[];
    extern u8 lbl_803799EF[];
    extern u8 lbl_803799F4[];
    extern u8 lbl_803799FE[];
  u8 bVar1;
  u16 uVar11;
  int iVar5;
  u32 uVar6;
  u8 uVar14;
  s32 uVar17;
  u32 uVar3;
  u32 uVar4;
  u32 uVar7;
  u32 uVar2;
  u32 uVarMove;
  u32 activeMove;
  u32 uVar8;
  u8 cVar15;
  u32 uVar9;
  u32 uVar10;
  u16 local_48;
  short local_44[4];

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
  activeMove = 0;
  activeMove += fn_80205184((void*)r3);
  uVar7 = fightOutPokemonGetSoubiItemDataId(r3);
  fn_80203FE4(r3);
  uVar8 = fn_80205B8C(r3);
  uVar14 = (int)fn_8012640C(uVar8,0,0xbf,0);
  cVar15 = (u8)fn_8011BEB4(uVar4,0,0x32,0);
  if ((cVar15 != 1) && ((u8)fn_801F54A4(0,0,0x31,0) != 0)) {
    fn_80205B8C(r3);
    cVar15 = (u8)fn_8011FC74();
    if ((cVar15 != 0) && ((u8)fn_802026E4(r3,0x3e) != 0) &&
        ((u16)activeMove != 0x164)) {
      fn_80132A38(0x13,fn_801F8100(iVar5));
      uVar17 = fn_800E0C54() % 100;
      fn_8011CE18(uVar14);
      uVarMove = fn_8011CCE4();
      if (uVar17 < (uVarMove & 0xff)) {
        u8 initIndex;
        u32 bVar16;
        u32 sVar12;
        for (initIndex = 0; initIndex < 4; initIndex++) {
          local_44[initIndex] = -1;
        }
        for (bVar16 = 0, sVar12 = 0; (s16)sVar12 < 4; sVar12 = sVar12 + 1) {
          u16 moveId;
          activeMove = sVar12 & 0xffff;
          cVar15 = fn_80123CD4(uVar2,activeMove);
          if ((((cVar15 != 0) && (cVar15 = fn_801FFEC8(r3,activeMove,0,0), cVar15 == 0)) &&
              (moveId = (int)fn_8012640C(uVar2,0,0x7f,activeMove), moveId != 0)) &&
             ((moveId != 0x165 && (moveId != 0x163)))) {
            local_44[(u8)bVar16] = sVar12;
            bVar16 = bVar16 + 1;
          }
        }
        if ((u8)bVar16 != 0) {
          extern u32 fn_8012640C(u32, u32, u32, u16);
          u32 randomValue;
          s32 quotient;
          short selectedSlot;
          randomValue = fn_800E0C54();
          quotient = (s32)(randomValue & 0xffff) / (u8)bVar16;
          selectedSlot = *(short *)((int)local_44 +
                                   (((randomValue & 0xffff) - quotient * (u8)bVar16)
                                    * 2 & 0x1fe));
          if (((selectedSlot >= 0) && (uVar2 = (u16)fn_8012640C(uVar2,0,0x7f,selectedSlot), (u16)uVar2 != 0)) &&
             (((u16)uVar2 != 0x165 && ((u16)uVar2 != 0x163)))) {
            u32 targetOutPokemon;
            u32 flags;
            u32 relativeTarget;
            lbl_8047B60C = (u16)uVar2;
            flags = lbl_8047B618 & 0xfffffbff;
            *(volatile u32*)&lbl_8047B618 = flags;
            *(volatile u32*)&lbl_8047B618 = flags | 0x200000;
            targetOutPokemon = fn_8022B2CC(r3,(u16)uVar2,uVar11,(u32)fn_8022B29C,1,1, (void*)0xffffffff);
            relativeTarget = fn_801F0134(targetOutPokemon, uVar11);
            fn_802099AC(uVar4, selectedSlot, (u16)uVar2, relativeTarget, 0);
            fn_801F4C14(0,0,0x43,0,targetOutPokemon);
            fn_8011BEB4(0,(u16)uVar2,1,0);
            fn_80132A38(0x28,fn_800FA280());
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
            lbl_8047B618 = lbl_8047B618 | 0x400;
            lbl_8047B610 = lbl_803799ED;
            return 2;
          }
        }
        goto fallback_7710;
      }
      else {
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CCCC();
        uVarMove = (uVarMove & 0xff) + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
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
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CCB4();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
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
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC9C();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
          fn_80208404(r3,0,1,0);
          fn_80208404(r3,0,2,0);
          fn_801F4C14(0,0,0x43,0,r3);
          uVar10 = fn_80232110(r3,r3,uVar3,1,0x28, (void*)0xffffffff);
          fn_8011BBD8(uVar4,0,0x2d,0,uVar10);
          fn_801254B4((void*)r3,0,0x107,0,1);
          lbl_8047B618 = lbl_8047B618 | 0x80000;
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          fn_80208404(r3,0,2,1);
          fn_802624CC(0x770f);
          fn_80208404(r3,0,2,2);
          fn_8026246C();
          fn_80208404(r3,0,1,3);
          fn_80208404(r3,0,2,3);
          lbl_8047B610 = lbl_803799EF;
          return 2;
        }
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC84();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 >= uVarMove) {
          goto after_fallback_7710;
        }
fallback_7710:
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
        lbl_8047B610 = lbl_803799F4;
        return 1;
after_fallback_7710:
        fn_8011CE18(uVar14);
        uVar10 = fn_8011CC6C();
        uVarMove = uVarMove + (uVar10 & 0xff);
        if (uVar17 < uVarMove) {
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
            if ((((uVar9 == 0x2c) || ((uVar7 - 0x85 & 0xffff) <= 0xe)) ||
                ((uVar7 - 0xa8 & 0xffff) <= 6)) || ((uVar9 == 0xb4 || (uVar9 == 0xb9)))) {
              bVar1 = 1;
            }
            else {
              bVar1 = 0;
            }
            if ((bVar1 == 1) && (cVar15 = fn_8022BE2C(r3,2), cVar15 != 0)) {
              fn_80208404(r3,0,1,3);
              fn_802086E8(r3,0x85,3);
              lbl_8047B610 = lbl_803799F4;
              return 1;
            }
            fn_802085C4(r3,0x85,3,0, (void*)0xffffffff);
            fn_802624CC(0x7711);
            fn_80208554(r3,0x85,3,6);
            fn_8026246C();
          }
          fn_80208404(r3,0,1,3);
          fn_802086E8(r3,0x85,3);
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
        fn_8011CE18(uVar14);
        uVar7 = fn_8011CC54();
        if (uVar17 < uVarMove + (uVar7 & 0xff)) {
          fn_80208404(r3,0,1,0);
          fn_80208404(r3,0,0,0);
          fn_80208ED0(r3,0);
          fn_80208404(r3,0,1,1);
          fn_80208404(r3,0,1,2);
          local_48 = 0;
          uVar8 = fn_801F8A18(iVar5,&local_48);
          if (uVar8 != 0) {
            fn_80208ED0(r3,1);
            fn_802624CC(0x7713);
            fn_80208ED0(r3,2);
            fn_8026246C();
            fn_80208404(r3,0,1,3);
            fn_80208404(r3,0,0,3);
            fn_80208ED0(r3,3);
            fn_80208ED0(r3,4);
            lbl_8047B610 = lbl_803799FE;
            return 1;
          }
          fn_80208404(r3,0,0,1);
          fn_802624CC(0x7714);
          fn_80208404(r3,0,0,2);
          fn_8026246C();
          fn_80208404(r3,0,1,3);
          fn_80208404(r3,0,0,3);
          fn_80208ED0(r3,3);
          lbl_8047B610 = lbl_803799F4;
          return 1;
        }
      }
    }
  }
  return 0;
}
#pragma opt_lifetimes reset
#undef fn_800FA280
#undef fn_8011BBD8
#undef fn_8011BEB4
#undef fn_8011CE18
#undef fn_8011FC74
#undef fn_80123CD4
#undef fn_801254B4
#undef fn_8012640C
#undef fn_80132A38
#undef fn_801F0134
#undef fn_801F025C
#undef fn_801F4354
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_801F8100
#undef fn_801FB1C0
#undef fn_801FFEC8
#undef fn_80203E0C
#undef fn_80203FE4
#undef fn_80205184
#undef fn_80205B8C
#undef fn_80208404
#undef fn_802085C4
#undef fn_802086E8
#undef fn_80208750
#undef fn_80208ED0
#undef fn_802099AC
#undef fn_8026246C
#undef fn_802624CC
#pragma optimize_for_size reset
#define fn_8011BEB4 wazaGetStatus
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
#define fn_801F2654 fightFloorGetValidFightOutPokemonCount
#define fn_801F3624 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId
#define fn_801F025C fightTargetGetPtrAsNowFightType
u32 fn_8022B2CC(r3, r4, r5, r6, r7, r8, in_r9)
u32 r3;
u32 r4;
u32 r5;
u32 r6;
u32 r7;
u32 r8;
s8 in_r9;
{
    extern u32 fn_8011BEB4();
    extern u32 fn_801F025C();
    extern u32 fn_801F2654();
    extern u32 fn_801F3624();
    extern u8 fn_80207AE0();
    extern u32 fightFloorGetFightOutPokemonPtrRandom();
    extern u32 fightTargetGetPtr();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightFloorGetStatus();
    extern u8 fightSideIsJoutaiDataId();
    extern u32 fightSideGetJoutaiUserFightTargetId();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst();
    extern void pokemonSetStatus();
    u32 result;
    u32 count;
    u32 abilityCount;
    u16 floorStatus;
    struct {
        u32 value;
    } redirected;
    u32 targetAbility;
    u16 moveType;
    u8 category;

    result = 0;
    if (in_r9 < 0) {
        category = fn_8011BEB4(0, r4, 5, 0);
    } else {
        category = in_r9;
    }
    if (((u16)r4 == 0xae) && (fn_80207AE0(r3, 7) == 0)) {
        category = 5;
    }
    moveType = fn_8011BEB4(0, r4, 3, 0);
    count = fn_801F2654(0, 0, r3, 1);
    abilityCount = fn_801F3624(0, 0x1f, 2, r3);

    switch (category) {
    case 0:
        if ((u16)count >= 2) {
            if ((u8)r7 == 1) {
                if (r6 != 0) {
                    result = ((u32 (*)(u32, u32, u32))r6)(r3, r4, r5);
                } else {
                    result = fightFloorGetFightOutPokemonPtrRandom(0, 1, 2, r3);
                }
            } else {
                result = fn_801F025C(0x12, 0);
            }
            if ((u8)r8 == 1) {
                struct {
                    u32 value;
                } side;
                targetAbility = fightOutPokemonGetTokuseiDataId(result);
                side.value = fn_801F025C(3, r3);
                floorStatus = fightFloorGetStatus(0, 0, 0x14, 0);
                redirected.value = 0;
                if (fightSideIsJoutaiDataId(side.value, 0x4d) == 1) {
                    u32 targetId = fightSideGetJoutaiUserFightTargetId(side.value, 0x4d);
                    if ((u16)targetId != 0) {
                        u32 target = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                            targetId, floorStatus);
                        if ((target != 0) && (fightOutPokemonCheckFightOut(target) == 1)) {
                            redirected.value = target;
                        }
                    }
                }
                if (redirected.value != 0) {
                    result = redirected.value;
                } else if (((u16)targetAbility != 0x1f) && (moveType == 0xd) &&
                           ((u16)abilityCount != 0)) {
                    result = fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(
                        0, 0x1f, 1, 2, r3);
                    pokemonSetStatus(result, 0, 0x114, 0, 1);
                }
            }
        } else if ((u8)r7 == 1) {
            result = fightFloorGetFightOutPokemonPtrRandom(0, 1, 3, r3);
        }
        break;
    case 1:
    case 4:
    case 6:
    case 7:
        if ((u8)r7 == 1) {
            result = fightTargetGetPtr(0xf, r3, r5);
            if (fightOutPokemonCheckFightOut(result) == 0) {
                result = fn_801F025C(0xe, result);
            }
        }
        break;
    case 3:
        if ((u8)r7 == 1) {
            result = fightFloorGetFightOutPokemonPtrRandom(0, 1, 2, r3);
        }
        break;
    case 2:
    case 5:
        if ((u8)r7 == 1) {
            result = r3;
        }
        break;
    }
    return result;
}
#undef fn_8011BEB4
#undef fn_80207AE0
#undef fn_801F2654
#undef fn_801F3624
#undef fn_801F025C
u32 fn_8022B5C8(void* rawCtx)
{
    extern u8 lbl_80379A3C[];
    extern u8 lbl_80379A5A[];
    extern u8 lbl_80379A78[];
    extern u8 lbl_80379A96[];
    extern u8 lbl_80379AB4[];
    extern u8 lbl_80379AD2[];
    extern u8 lbl_80379AEE[];
    extern u8 lbl_80379B0C[];
    extern u16 lbl_80279FD0[8];
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u16 fightOutPokemonGetSoubiItemSoubiDataId();
    extern void figthOutPokemonGetSoubiItemBuff();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightFloorSetStatus();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fn_80119F50();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern u8 fightOutPokemonIsJoutaiNormal();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonResetSeqStatus();
    extern s32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    u8 result = 0;
    u32 pokemon;
    u8* next;
    u16 itemType;
    u8* msg;
    u16 i;
    u32 ctx;

    ctx = (u32)rawCtx;
    pokemon = fightOutPokemonGetPokemonPtr(ctx);
    next = (u8*)(u32)fightOutPokemonGetSoubiItemDataId(ctx);
    itemType = fightOutPokemonGetSoubiItemSoubiDataId(ctx);
    figthOutPokemonGetSoubiItemBuff(ctx);
    msg = 0;

    if (fightOutPokemonCheckFightOut(ctx) == 0) {
        return 1;
    }

    fightFloorSetStatus(0, 0, 0x56, 0, (u16)(u32)next);

    switch ((u16)itemType) {
    case 2: {
        next = 0;
        if (fn_802026E4(ctx, 5) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 5);
            next = lbl_80379A3C;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 4: {
        next = 0;
        if (fn_802026E4(ctx, 3) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 3);
            fightOutPokemonWriteJoutaiDataId(ctx, 4);
            next = lbl_80379A5A;
        }
        msg = next;
        if (next != 0) {
            result = 1;
            break;
        }

        next = 0;
        if (fn_802026E4(ctx, 4) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 4);
            fightOutPokemonWriteJoutaiDataId(ctx, 3);
            next = lbl_80379A5A;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 5: {
        next = 0;
        if (fn_802026E4(ctx, 6) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 6);
            next = lbl_80379A78;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 6: {
        next = 0;
        if (fn_802026E4(ctx, 7) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 7);
            next = lbl_80379A96;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 3: {
        next = 0;
        if (fn_802026E4(ctx, 8) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 8);
            fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
            next = lbl_80379AB4;
        }
        msg = next;
        if (next != 0) {
            result = 1;
        }
        break;
    }
    case 8: {
        next = 0;
        if (fn_802026E4(ctx, 9) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 9);
            next = lbl_80379AD2;
        }
        msg = next;
        if (next != 0) {
            result = 2;
        }
        break;
    }
    case 0x1C: {
        next = 0;
        if (fn_802026E4(ctx, 0xA) == 1) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0xA);
            next = lbl_80379AEE;
        }
        msg = next;
        if (next != 0) {
            fn_80119F50(0xA);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            lbl_80478D78[5] = 0;
            result = 2;
        }
        break;
    }
    case 9:
        if (fightOutPokemonIsJoutaiNormal(ctx) == 0 ||
            fn_802026E4(ctx, 9) == 1) {
            lbl_80478D78[5] = 0;
            if (fn_802026E4(ctx, 3) == 1 ||
                fn_802026E4(ctx, 4) == 1) {
                fn_80119F50(3);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fn_802026E4(ctx, 8) == 1) {
                fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
                fn_80119F50(8);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fn_802026E4(ctx, 5) == 1) {
                fn_80119F50(5);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fn_802026E4(ctx, 6) == 1) {
                fn_80119F50(6);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fn_802026E4(ctx, 7) == 1) {
                fn_80119F50(7);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            if (fn_802026E4(ctx, 9) == 1) {
                fn_80119F50(9);
                msgctrlSetValue(0xD, GSmsgGetGSchar());
            }
            pokemonInitJoutai(pokemon);
            fightOutPokemonWriteJoutaiDataId(ctx, 9);
            fightOutPokemonResetSeqStatus(ctx, 0);
            result = 1;
            msg = lbl_80379AEE;
        }
        break;
    case 0x17:
        for (i = 0; i < 7; i++) {
            if (pokemonGetStatus(ctx, 0, lbl_80279FD0[i], 0) < 6) {
                pokemonSetStatus(ctx, 0, lbl_80279FD0[i], 0, 6);
                result = 5;
            }
        }
        if (result != 0) {
            msg = lbl_80379B0C;
        }
        break;
    }

    if (result != 0) {
        fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
        fightFloorSetStatus(0, 0, 0x49, 0, ctx);
        if (result == 1 && fightOutPokemonIsUseHensinBuff(ctx) == 1) {
            fightOutPokemonSetHensinPokemonStatusId(ctx, 0x7C, 0, 0);
        }
        if (msg != 0) {
            fn_80211B94(lbl_8047B62C, msg, 0);
        }
    }
    return 1;
}
#pragma optimize_for_size on
u32 fn_8022BB84(u32 attacker, u32 target)
{
    extern u32 fn_800E0C54();
    extern u8 lbl_80379B61[];
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fightOutPokemonGetSoubiItemDataId();
    extern u32 fightOutPokemonGetSoubiItemSoubiDataId();
    extern s32 figthOutPokemonGetSoubiItemBuff();
    extern s32 wazaGetStatus();
    extern void wazaSetStatus();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u8 fightWazaIsHit();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fightOutPokemonIsHpMantan();
    extern void fightFloorSetStatus();
    extern void fn_802249B8(u32, u32);
    u32 move;
    u32 itemData;
    u32 itemType;
    s32 itemBuff;
    u8 hit;
    u8 moveFlag;
    s16 value11C;
    s16 value11E;
    u32 moveData;
    s32 damage;
    s32 heal;
    u32 result;
    u8* saved;

    move = pokemonGetStatus(attacker, 0, 0xD9, 0);
    itemData = fightOutPokemonGetSoubiItemDataId(attacker);
    itemType = fightOutPokemonGetSoubiItemSoubiDataId(attacker);
    itemBuff = figthOutPokemonGetSoubiItemBuff(attacker);
    fightOutPokemonGetSoubiItemDataId(target);
    fightOutPokemonGetSoubiItemSoubiDataId(target);
    figthOutPokemonGetSoubiItemBuff(target);

    result = 0;
    if (wazaGetStatus(move, 0, 0x2D, 0) == 0) {
        return 0;
    }

    moveData = fightOutPokemonGetUseWazaDataId(attacker);
    hit = fightWazaIsHit(move);
    moveFlag = wazaGetStatus(0, moveData, 0x12, 0);
    value11C = pokemonGetStatus(target, 0, 0x11C, 0);
    value11E = pokemonGetStatus(target, 0, 0x11E, 0);
    damage = pokemonGetStatus(target, 0, 0x11B, 0);

    switch ((u16)itemType) {
    case 0x1E:
        if (hit == 1 && (value11C != 0 || value11E != 0) && moveFlag == 1 &&
            ((s32)(fn_800E0C54() & 0xFFFF) % 100) < itemBuff &&
            fightOutPokemonCheckFightOut(target) == 1) {
            lbl_80478D78[3] = 8;
            saved = lbl_8047B610;
            fn_802249B8(0, 0);
            lbl_8047B610 = saved;
        }
        break;
    case 0x3E:
        if (hit == 1 && damage != 0 && damage != 0xFFFF && attacker != target &&
            fightOutPokemonIsHpMantan(attacker) == 0 &&
            fightOutPokemonCheckFightOut(attacker) == 1) {
            fightFloorSetStatus(0, 0, 0x56, 0, (u16)itemData);
            fightFloorSetStatus(0, 0, 0x49, 0, attacker);
            fightFloorSetStatus(0, 0, 0x4B, 0, attacker);
            heal = -(damage / itemBuff);
            if (heal == 0) {
                heal = -1;
            }
            wazaSetStatus(move, 0, 0x2D, 0, heal);
            pokemonSetStatus(target, 0, 0x11B, 0, 0);
            fn_80211B94(lbl_8047B62C, lbl_80379B61, 0);
            result = 1;
        }
        break;
    }
    return result;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_8022D394(u32 pokemon)
{
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fn_802026E4();
    extern void fn_80119F50();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonResetSeqStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void fightFloorSetStatus();
    extern void fn_80211B94();
    extern void* lbl_8047B62C;
    extern u8 lbl_8037994A[];
    u8 result;
    u32 ability;
    u32 pokemonPtr;

    ability = pokemon;
    while (1) {
        if (fightOutPokemonCheckFightOut(ability) == 0) {
            return 1;
        }

        pokemon = fightOutPokemonGetTokuseiDataId(ability);
        pokemonPtr = fightOutPokemonGetPokemonPtr(ability);
        result = 0;

        switch ((u16)pokemon) {
        case 0x11:
            if (fn_802026E4(ability, 3) == 1 ||
                fn_802026E4(ability, 4) == 1) {
                fn_80119F50(3);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 1;
            }
            break;

        case 0x14:
            if (fn_802026E4(ability, 9) == 1) {
                fn_80119F50(9);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 2;
            }
            break;

        case 7:
            if (fn_802026E4(ability, 5) == 1) {
                fn_80119F50(5);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 1;
            }
            break;

        case 0xf:
        case 0x48:
            if (fn_802026E4(ability, 8) == 1) {
                fightOutPokemonWriteJoutaiDataId(ability, 0x17);
                fn_80119F50(8);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 1;
            }
            break;

        case 0x29:
            if (fn_802026E4(ability, 6) == 1) {
                fn_80119F50(6);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 1;
            }
            break;

        case 0x28:
            if (fn_802026E4(ability, 7) == 1) {
                fn_80119F50(7);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 1;
            }
            break;

        case 0xc:
            if (fn_802026E4(ability, 10) == 1) {
                fn_80119F50(10);
                msgctrlSetValue(0xd, GSmsgGetGSchar());
                result = 3;
            }
            break;
        }

        if (result == 0) {
            goto return_one;
        }

        switch (result) {
        case 1:
            pokemonInitJoutai(pokemonPtr);
            fightOutPokemonResetSeqStatus(ability, 0);
            goto apply_hensin;
        case 2:
            fightOutPokemonWriteJoutaiDataId(ability, 9);
            goto apply_hensin;
        case 3:
            fightOutPokemonWriteJoutaiDataId(ability, 10);
        default:
apply_hensin:
            if (fightOutPokemonIsUseHensinBuff(ability) == 1) {
                fightOutPokemonSetHensinPokemonStatusId(ability, 0x7c, 0, 0);
            }
            fightFloorSetStatus(0, 0, 0x4b, 0, ability);
            break;
        }

        fn_80211B94(lbl_8047B62C, lbl_8037994A, 0);
    }

return_one:
    return 1;
}
#pragma optimize_for_size reset
#pragma opt_propagation off
u32 fn_8022D6BC(u32 attacker, u32 defender)
{
    extern u32 pokemonGetStatus();
    extern u16 wazaGetStatus();
    extern void wazaSetStatus();
    extern u16 fightFloorGetStatus();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u32 fightOutPokemonGetWazaZokuseiDataId();
    extern u8 fightWazaIsHit();
    extern u8 fightOutPokemonGetSex();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u8 fightOutPokemonIsZokuseiDataId();
    extern void fightOutPokemonSetZokuseiDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern u16 fightOutPokemonMaxHpWaruValue();
    extern void fn_8010C4D4();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void fn_80211B94();
    extern u8 fn_802025B8();
    extern void fn_8020248C();
    extern u8 lbl_803798F3[];
    extern u8 lbl_80379907[];
    extern u8 lbl_8037992F[];
    extern u8 lbl_80379932[];
    extern void* lbl_8047B62C;
    extern u32 lbl_8047B618;
    extern const u16 lbl_8047E600;
    extern const u8 lbl_8047E602;

    s16 status11C;
    s16 status11E;
    u32 defenderAbility;
    u32 moveId;
    u16 moveJoutai;
    u32 moveType;
    u8 moveStatus13;
    u32 moveStatus;
    u8 moveHit;
    u8 status107;
    u32 result;
    u8 attackerSex;
    u8 defenderSex;
    u32 target;
    u32 attackerAbility;
    u8 values[3];

    status11C = pokemonGetStatus(defender, 0, 0x11C, 0);
    status11E = pokemonGetStatus(defender, 0, 0x11E, 0);
    attackerAbility = fightOutPokemonGetTokuseiDataId(attacker);
    defenderAbility = fightOutPokemonGetTokuseiDataId(defender);
    moveId = fightOutPokemonGetUseWazaDataId(attacker);
    moveJoutai = (u16)wazaGetStatus(0, moveId, 7, 0);
    moveType = fightOutPokemonGetWazaZokuseiDataId(attacker);
    moveStatus13 = wazaGetStatus(0, moveId, 0xD, 0);
    moveStatus = pokemonGetStatus(attacker, 0, 0xD9, 0);
    moveHit = fightWazaIsHit(moveStatus);
    status107 = pokemonGetStatus(attacker, 0, 0x107, 0);
    *(u16*)values = lbl_8047E600;
    values[2] = lbl_8047E602;
    result = 0;
    attackerSex = fightOutPokemonGetSex(attacker);
    defenderSex = fightOutPokemonGetSex(defender);
    target = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(
        defender, fightFloorGetStatus(0, 0, 0x14, 0));

    switch ((u16)defenderAbility) {
    case 0x10:
        if (moveHit == 1 && (u16)moveId != 0xA5 && moveJoutai != 0 &&
            (u16)moveId != 0x164 && (status11C != 0 || status11E != 0) &&
            fightOutPokemonIsZokuseiDataId(defender, moveType) == 0 &&
            fightOutPokemonCheckFightOut(defender) == 1) {
            u32 i;
            for (i = 0; (u8)i < 2; i++) {
                fightOutPokemonSetZokuseiDataId(defender, i, moveType);
            }
            fn_8010C4D4(moveType);
            msgctrlSetValue(0xD, GSmsgGetGSchar());
            fn_80211B94(lbl_8047B62C, lbl_803798F3, 0);
            result = 1;
        }
        break;

    case 0x18:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            wazaSetStatus(moveStatus, 0, 0x2D, 0,
                          fightOutPokemonMaxHpWaruValue(attacker, 0x10));
            fn_80211B94(lbl_8047B62C, lbl_80379907, 0);
            result = 1;
        }
        break;

    case 0x1B:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            if (fn_800E0C54() % 10 == 0) {
                u16 pick = (u16)fn_800E0C54();
                u32 pickDivisor = 3;
                u32 quotient = pick / pickDivisor;
                void* msg = lbl_8047B62C;
                u8 value;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                value = values[(u8)(pick - quotient * pickDivisor)];
                lbl_80478D78[3] = value;
                lbl_80478D78[3] = value + 0x40;
                quotient = 0;
                fn_80211B94(msg, lbl_8037992F, quotient);
                result = 1;
            }
        }
        break;

    case 0x26:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            if (fn_800E0C54() % 3 == 0) {
                u32 value = 0x42;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                lbl_80478D78[3] = value;
                value = 0;
                fn_80211B94(lbl_8047B62C, lbl_8037992F, value);
                result = 1;
            }
        }
        break;

    case 9:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && (status11C != 0 || status11E != 0) &&
            moveStatus13 != 0) {
            if (fn_800E0C54() % 3 == 0) {
                u32 value = 0x45;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                lbl_80478D78[3] = value;
                value = 0;
                fn_80211B94(lbl_8047B62C, lbl_8037992F, value);
                result = 1;
            }
        }
        break;

    case 0x31:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && moveStatus13 != 0 &&
            (status11C != 0 || status11E != 0)) {
            if (fn_800E0C54() % 3 == 0) {
                u32 value = 0x43;
                lbl_8047B618 = lbl_8047B618 | 0x2000;
                lbl_80478D78[3] = value;
                value = 0;
                fn_80211B94(lbl_8047B62C, lbl_8037992F, value);
                result = 1;
            }
        }
        break;

    case 0x38:
        if (moveHit == 1 && fightOutPokemonCheckFightOut(attacker) == 1 &&
            status107 == 0 && moveStatus13 != 0 &&
            (status11C != 0 || status11E != 0) &&
            fightOutPokemonCheckFightOut(defender) == 1) {
            if (fn_800E0C54() % 3 == 0 &&
                (u16)attackerAbility != 0xC &&
                attackerSex != defenderSex && fn_802025B8(attacker, 0xA) == 2 &&
                attackerSex != 2 && defenderSex != 2) {
                fn_8020248C(attacker, 0xA, target);
                fn_80211B94(lbl_8047B62C, lbl_80379932, 0);
                result = 1;
            }
        }
        break;
    }

    return result;
}
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8022DF08(u32 ctx)
{
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightFloorGetNowTenkouDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern u8 fightOutPokemonIsHpMantan();
    extern u16 fightOutPokemonMaxHpWaruValue();
    extern void wazaSetStatus();
    extern u8 fightOutPokemonIsJoutaiNormal();
    extern u32 pokemonGetJoutaiMsgId();
    extern u32 GSmsgGetGSchar();
    extern void msgctrlSetValue();
    extern void pokemonInitJoutai();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fightOutPokemonResetSeqStatus();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern void fightOutPokemonSetHensinPokemonStatusId();
    extern void pokemonSetStatus();
    extern void fn_80211B94();
    extern u8 lbl_8037967E[];
    extern u8 lbl_8037969F[];
    extern u8 lbl_803796B5[];
    extern u8 lbl_80379F58[];
    extern void* lbl_8047B62C;
    u32 pokemon;
    u32 weather;
    u32 ability;
    u32 move;

    pokemon = fightOutPokemonGetPokemonPtr();
    {
        u32 callResult = fightFloorGetNowTenkouDataId(0, 1);
        weather = (u8)callResult;
    }
    if (ctx == 0) {
        return;
    }
    if (fightOutPokemonCheckFightOut(ctx) == 0) {
        return;
    }

    ability = fightOutPokemonGetTokuseiDataId(ctx);
    fightFloorSetStatus(0, 0, 0x36, 0, ctx);
    move = pokemonGetStatus(ctx, 0, 0xD9, 0);

    switch ((u16)ability) {
    case 0x2C:
        if (weather == 2 && fightOutPokemonIsHpMantan(ctx) == 0) {
            u32 amount = (u16)fightOutPokemonMaxHpWaruValue(ctx, 0x10);
            wazaSetStatus(move, 0, 0x2D, 0, -(s32)amount);
            fn_80211B94(lbl_8047B62C, lbl_8037967E, 0);
        }
        break;

    case 0x3D:
        if (fightOutPokemonIsJoutaiNormal(ctx) == 0) {
            u16 random = fn_800E0C54();
            s32 divisor = 3;
            if (random % divisor == 0) {
                fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
                msgctrlSetValue(0xD, GSmsgGetGSchar(pokemonGetJoutaiMsgId(pokemon)));
                pokemonInitJoutai(pokemon);
                fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
                fightOutPokemonResetSeqStatus(ctx, 0);
                if (fightOutPokemonIsUseHensinBuff(ctx) == 1) {
                    fightOutPokemonSetHensinPokemonStatusId(ctx, 0x7C, 0, 0);
                }
                fn_80211B94(lbl_8047B62C, lbl_8037969F, 0);
            }
        }
        break;

    case 3: {
        s8 count = pokemonGetStatus(ctx, 0, 0xEA, 0);
        if (count < 12 && (u16)pokemonGetStatus(ctx, 0, 0xED, 0) != 2) {
            fightFloorSetStatus(0, 0, 0x4B, 0, ctx);
            pokemonSetStatus(ctx, 0, 0xEA, 0, (s8)(count + 1));
            lbl_80379F58[0x160A4] = 0x11;
            lbl_80379F58[0x160A5] = 0;
            fn_80211B94(lbl_8047B62C, lbl_803796B5, 0);
        }
        break;
    }

    case 0x36: {
        u32 value = pokemonGetStatus(ctx, 0, 0xF9, 0);
        value = __cntlzw(value & 0xFF);
        pokemonSetStatus(ctx, 0, 0xF9, 0, (value & 0x1FE0) >> 5);
        break;
    }
    }
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_800FA280 GSmsgGetGSchar
#define fn_8011CB54 pokemonTokuseiDataBiosGetName
#define fn_8011CB6C pokemonTokuseiDataBiosGetPtr
#define fn_80132A38 msgctrlSetValue
#define fn_801F2598 fightFloorGetFightOutPokemonPtrRandom
#define fn_801F4C14 fightFloorSetStatus
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_802037DC fightOutPokemonGetNicknamePtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207BC0 fightOutPokemonSetTokuseiDataId
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
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
    extern u32 fn_80207BF4();
    extern void fn_80211B94();
    extern void* lbl_8047B62C;
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
#pragma optimize_for_size reset
#pragma optimize_for_size on
#undef fn_800FA280
#undef fn_8011CB54
#undef fn_8011CB6C
#undef fn_80132A38
#undef fn_801F2598
#undef fn_801F4C14
#undef fn_80202810
#undef fn_802037DC
#undef fn_802062FC
#undef fn_80207BC0
#undef fn_80207BF4
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_801F4C14 fightFloorSetStatus
u32 fn_8022E34C(u32 r3, u32 r4, char* r5)

{
    extern void fn_801F4C14();
    extern u16 fn_80207BF4();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 lbl_803795FE[];
    extern u8 lbl_803795F5[];
    extern void* lbl_8047B62C;
  u16 sVar1;
  u32 uVar3;
  void* context;
  u8 cVar2;

  context = (void*)r3;
  sVar1 = fn_80207BF4();
  cVar2 = fn_802062FC(context);
  if (cVar2 == 0) {
    return 1;
  }
  uVar3 = (u32)((*r5 == 1) ? lbl_803795F5 : lbl_803795FE);
  if ((sVar1 == 0x16) &&
      (cVar2 = fn_802026E4(context,0x3b), cVar2 == 1)) {
      fn_80202810(context,0x3b);
      fn_801F4C14(0,0,0x4b,0,context);
      fn_80211B94(lbl_8047B62C,uVar3,0);
  }
  return 1;
}
#pragma optimize_for_size reset
#undef fn_80207BF4
#undef fn_802062FC
#undef fn_80202810
#undef fn_801F4C14
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801F2934 fightFloorWriteJoutaiDataId
#define fn_801F2988 fightFloorCheckWriteJoutaiDataId
#define fn_801F37B0 fightFloorLoopValidFightOutPokemon
#define fn_801F4C14 fightFloorSetStatus
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80207BF4 fightOutPokemonGetTokuseiDataId
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
    extern void* lbl_8047B62C;
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
      fn_801254B4((void*)r3,0,0x116,0,1);
      return;
    case 0x24:
      if ((int)fn_8012640C(r3,0,0x117,0) != 0) {
        return;
      }
      cVar3 = fn_802025B8(r3,0x3c);
      if (cVar3 == 2) {
        fn_8020248C(r3,0x3c,0);
      }
      fn_801254B4((void*)r3,0,0x117,0,1);
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
#undef fn_801254B4
#undef fn_8012640C
#undef fn_801F2934
#undef fn_801F2988
#undef fn_801F37B0
#undef fn_801F4C14
#undef fn_802062FC
#undef fn_80207BF4
#define fn_8011F910 pokemonAddDpFormPokemonDpFilterId
#define fn_8011FC74 pokemonIsDarkPokemon
#define fn_801254B4 pokemonSetStatus
#define fn_8012640C pokemonGetStatus
#define fn_801C3430 battleGridUpdate
#define fn_801F000C fightMainWaitFrame
#define fn_801F2A7C fightFloorGetGcHeroFightTrainerPtr
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801FE710 fightOutPokemonSetHensinFightPokemonStatusId
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80208404 fightOutPokemonDarkPokemonEffect
#define fn_802085C4 fightOutPokemonWazaEffect
#define fn_802086E8 fightOutPokemonFreeWazaEffect
#define fn_80208750 fightOutPokemonLoadWazaEffect
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#define fn_802653FC fightMenuFightOutPokemonRenewStatusMenu
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
            fn_801254B4((void*)uVar1,0,0xd1,0,1);
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
            fn_801254B4((void*)uVar1,0,0xd1,0,1);
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
#undef fn_8011F910
#undef fn_8011FC74
#undef fn_801254B4
#undef fn_8012640C
#undef fn_801C3430
#undef fn_801F000C
#undef fn_801F2A7C
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_801FE710
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80204A10
#undef fn_80205B8C
#undef fn_802062FC
#undef fn_80208404
#undef fn_802085C4
#undef fn_802086E8
#undef fn_80208750
#undef fn_8026246C
#undef fn_802624CC
#undef fn_802653FC
#pragma optimize_for_size on
#pragma opt_propagation off
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F4C14 fightFloorSetStatus
u32 fn_8022EB9C(u32 r3, u32 r4, u8* r5)

{
    extern u8 lbl_80379F58[];
    extern u8 lbl_803796D7[];
    extern void fn_801F4C14();
    extern u8 fn_802062FC();
    extern void fn_80211B94();
    extern u8 fn_8022EC40();
    extern void* lbl_8047B62C;
  u8 cVar1;
  register u32 id = r3;
  u8* out = r5;

  cVar1 = fn_802062FC(id);
  if (cVar1 == 0) {
    return 1;
  }
  cVar1 = fn_8022EC40(id);
  if (cVar1 != 0) {
    lbl_80379F58[0x1609b] = cVar1 + -1;
    fn_801F4C14(0,0,0x4b,0,id);
    fn_80211B94(lbl_8047B62C,lbl_803796D7,0);
    if (out != (void *)0) {
      *out = 1;
    }
  }
  return 1;
}
#undef fn_801F4C14
#undef fn_802062FC
#pragma opt_propagation reset
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_8011FC74 pokemonIsDarkPokemon
#define fn_8012640C pokemonGetStatus
#define fn_801F4C14 fightFloorSetStatus
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FE7EC fightOutPokemonSetHensinPokemonStatusId
#define fn_801FECD4 fightOutPokemonIsUseHensinBuff
#define fn_80202810 fightOutPokemonWriteJoutaiDataId
#define fn_80204A10 fightOutPokemonIsGcHeroFightOutPokemon
#define fn_80205184 fightOutPokemonGetUseWazaDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_80208404 fightOutPokemonDarkPokemonEffect
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#define fn_802653FC fightMenuFightOutPokemonRenewStatusMenu
u32 fn_8022EDEC(u32 r3, u8 r4)

{
    extern u32 fn_800E0C54();
    extern u32 fn_8011F6D8();
    extern u8 fn_8011FC74();
    extern void fn_801F4C14();
    extern u32 fn_801F54A4();
    extern void fn_801FE7EC();
    extern u8 fn_801FECD4();
    extern void fn_8020248C();
    extern u8 fn_802025B8();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fn_80204A10();
    extern void fn_80205B8C();
    extern u8 fn_802062FC();
    extern void fn_802080A8();
    extern void fn_80208404();
    extern void fn_8026246C();
    extern void fn_802624CC();
    extern void fn_8026532C();
    extern void fn_802653FC();
    extern void fn_80265598();
  u16 uVar4;
  u32 uVar1;
  u8 cVar6;
  u32 uVar2;
  u32 sVar5;
  int uVar3;
  int randomResult;

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
              sVar5 = fn_80205184((void*)r3);
              uVar3 = fn_800E0C54();
              if (((int)((uVar3 & 0xffff) % 100) < (int)(uVar2 & 0xff)) &&
                  ((sVar5 & 0xffff) == 0x164)) {
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
              else {
                return 0;
              }
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
              randomResult = (u16)fn_800E0C54();
              if ((randomResult % 0x100) != 0) {
                return 0;
              }
              if (fn_802026E4(r3,8) == 1) {
                __memcpy((void*)r3,(void*)r3,0);
              }
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
#undef fn_8011FC74
#undef fn_8012640C
#undef fn_801F4C14
#undef fn_801F54A4
#undef fn_801FE7EC
#undef fn_801FECD4
#undef fn_80202810
#undef fn_80204A10
#undef fn_80205184
#undef fn_80205B8C
#undef fn_802062FC
#undef fn_80208404
#undef fn_8026246C
#undef fn_802624CC
#undef fn_802653FC
#pragma optimize_for_size reset
#pragma opt_propagation off
u32 fn_8022F2F8(void)

{
    extern s32 fn_800E0C54();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8 fn_802026E4();
    extern s32 fn_80202108();
    extern s8 fn_80202234();
    extern u16 fn_80201C58();
    extern u8 fn_8021C638();
    extern u8 fn_8022EDEC();
    extern u32 fn_8022B2CC();
    extern u8 fightFloorLoopValidFightOutPokemon();
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fightOutPokemonGetUseWazaDataId();
    extern u8 fightOutPokemonIsUseHensinBuff();
    extern u8 fightFloorCheckHuuinWazaFightOutPokemon();
    extern u8 fightWazaCheckWriteJoutaiDataId();
    extern u32 pokemonGetStatus();
    extern u32 wazaGetStatus();
    extern u8 lbl_80376D00[];
    extern u8 lbl_80378C23[];
    extern u8 lbl_80378C3B[];
    extern u8 lbl_80378CB6[];
    extern u8 lbl_80378D14[];
    extern u8 lbl_8037903A[];
    extern u8 lbl_8037914F[];
    extern u8 lbl_80379212[];
    extern u8 lbl_80379217[];
    extern u8 lbl_8037922A[];
    extern u8 lbl_803792A0[];
    extern u8 lbl_803792B3[];
    extern u8 lbl_803792DF[];
    extern u8 lbl_803792F2[];
    extern u8 lbl_80379332[];
    extern u8 lbl_80379388[];
    extern u8 lbl_803793D7[];
    extern u8 lbl_803793EA[];
    extern u8 lbl_80379992[];
    extern u32 lbl_8047B620;
    extern u32 lbl_8047B644;
    u8* flagBase;
    u16 floorStatus;
    u32 subCtx;
    u32 transformed;
    u32 target;
    u32 ability;
    u32 moveId;
    u16 moveStatus7;
    u16 moveStatus9;
    u32 ctx;
    u32 result;
    s8 count;
    u32 selectedTarget;
    s16 storedTarget;
    int flag;
    s32 countValue;
    s32 divisor;
    u16 randomValue;
    u8 loopMiss;
    s8 threshold;
    s8 nextCount;
    u32 relativeTarget;
    u32 effectValue;

    ctx = fightTargetGetPtrAsNowFightType(0x11, 0);
    transformed = pokemonGetStatus(ctx, 0, 0xd9, 0);
    subCtx = fightTargetGetPtrAsNowFightType(2, ctx);
    target = ctx;
    ability = fightOutPokemonGetTokuseiDataId(ctx);
    moveId = fightOutPokemonGetUseWazaDataId(ctx);
    moveStatus7 = wazaGetStatus(0, moveId, 7, 0);
    moveStatus9 = wazaGetStatus(0, moveId, 9, 0);
    result = 0;
    floorStatus = fightFloorGetStatus(0, 0, 0x14, 0) & 0xffff;
    flagBase = lbl_80478D78;

    do {
        switch (lbl_8047B644) {
        case 0:
            fightOutPokemonWriteJoutaiDataId(ctx, 0x15);
            fightOutPokemonWriteJoutaiDataId(ctx, 0x28);
            lbl_8047B644++;
            break;
        case 1:
            if (fn_802026E4(ctx, 8) == 1) {
                loopMiss = fightFloorLoopValidFightOutPokemon(0, fn_8021C638, ctx, 1) != 1;
                if (loopMiss != 0) {
                    fightOutPokemonWriteJoutaiDataId(ctx, 8);
                    fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
                    flagBase[5] = 1;
                    fn_80211B94(lbl_8047B62C, lbl_8037922A, 0);
                    result = 2;
                }
                else {
                    flag = (u16)ability == 0x30;
                    countValue = flag + 1;
                    countValue = fn_80202108(ctx, 8, flag) - -countValue;
                    if ((s8)countValue >= fn_80202234(ctx, 8)) {
                        fightOutPokemonWriteJoutaiDataId(ctx, 8);
                        fightOutPokemonWriteJoutaiDataId(ctx, 0x17);
                    }
                    else {
                        fn_80201FDC(ctx, 8, countValue);
                    }
                    if (fn_802026E4(ctx, 8) == 1) {
                        if ((u16)moveId != 0xad && (u16)moveId != 0xd6) {
                            result = 2;
                            lbl_8047B618 = lbl_8047B618 | 0x80000;
                            lbl_8047B610 = lbl_80379217;
                        }
                    }
                    else {
                        flagBase[5] = 0;
                        fn_80211B94(lbl_8047B62C, lbl_8037922A, 0);
                        result = 2;
                    }
                }
            }
            lbl_8047B644++;
            break;
        case 2:
            if (fn_802026E4(ctx, 7) == 1) {
                randomValue = fn_800E0C54();
                divisor = 5;
                if ((randomValue % divisor) != 0) {
                    if (moveStatus9 == 0x7d) {
                        goto case2_early_increment;
                    }
                    lbl_8047B618 = lbl_8047B618 | 0x80000;
                    lbl_8047B610 = lbl_803792A0;
                    goto case2_result;
                case2_early_increment:
                    lbl_8047B644++;
                    break;
                }
                fightOutPokemonWriteJoutaiDataId(ctx, 7);
                flagBase[5] = 0;
                fn_80211B94(lbl_8047B62C, lbl_803792B3, 0);
            case2_result:
                result = 2;
            }
            lbl_8047B644++;
            break;
        case 3:
            if ((u16)ability == 0x36 && (u8)pokemonGetStatus(ctx, 0, 0xf9, 0) != 0) {
                if (fightWazaCheckWriteJoutaiDataId(transformed, 0x40) == 2) {
                    fightWazaWriteJoutaiDataId(transformed, 0x40, 0);
                }
                fightOutPokemonInitJoutaiKeep(ctx);
                lbl_8047B618 = lbl_8047B618 | 0x80000;
                lbl_8047B610 = lbl_80379992;
                flagBase[5] = 0;
                result = 1;
            }
            lbl_8047B644++;
            break;
        case 4:
            if (fn_802026E4(ctx, 0x12) == 1) {
                fightOutPokemonWriteJoutaiDataId(ctx, 0x12);
                fightOutPokemonInitJoutaiKeep(ctx);
                result = 1;
                lbl_8047B618 = lbl_8047B618 | 0x80000;
                lbl_8047B610 = lbl_80376D00;
            }
            lbl_8047B644++;
            break;
        case 5:
            if (fn_802026E4(ctx, 0x11) == 1) {
                fightOutPokemonWriteJoutaiDataId(ctx, 0x11);
                pokemonSetStatus(ctx, 0, 0x110, 0, 1);
                fightOutPokemonInitJoutaiKeep(ctx);
                result = 1;
                lbl_8047B618 = lbl_8047B618 | 0x80000;
                lbl_8047B610 = lbl_803792F2;
            }
            lbl_8047B644++;
            break;
        case 6:
            if (fn_802026E4(ctx, 0x29) == 1) {
                u16 disabledMove = fn_80201C58(ctx, 0x29);
                if (disabledMove == (u16)moveId &&
                    (u16)moveId != 0 && (u16)moveId != 0x165) {
                    fightOutPokemonInitJoutaiKeep(ctx);
                    pokemonSetStatus(ctx, 0, 0x10d, 0, 1);
                    lbl_8047B618 = lbl_8047B618 | 0x80000;
                    msgctrlSetValue(0x11, ctx);
                    result = 1;
                    lbl_8047B610 = lbl_80378D14;
                }
            }
            lbl_8047B644++;
            break;
        case 7:
            if (fn_802026E4(ctx, 0x30) == 1 && moveStatus7 == 0) {
                fightOutPokemonInitJoutaiKeep(ctx);
                pokemonSetStatus(ctx, 0, 0x10e, 0, 1);
                lbl_8047B618 = lbl_8047B618 | 0x80000;
                msgctrlSetValue(0x11, ctx);
                result = 1;
                lbl_8047B610 = lbl_8037903A;
            }
            lbl_8047B644++;
            break;
        case 8:
            if (fightFloorCheckHuuinWazaFightOutPokemon(0, ctx, moveId) == 1) {
                fightOutPokemonInitJoutaiKeep(ctx);
                pokemonSetStatus(ctx, 0, 0x10b, 0, 1);
                lbl_8047B618 = lbl_8047B618 | 0x80000;
                msgctrlSetValue(0x11, ctx);
                result = 1;
                lbl_8047B610 = lbl_8037914F;
            }
            lbl_8047B644++;
            break;
        case 9:
            if (fn_802026E4(ctx, 9) == 1) {
                countValue = fn_80202108(ctx, 9);
                if ((s8)countValue >= fn_80202234(ctx, 9)) {
                    fightOutPokemonWriteJoutaiDataId(ctx, 9);
                    fn_80211B94(lbl_8047B62C, lbl_80379388, 0);
                }
                else {
                    fn_80201FDC(ctx, 9, (s8)(countValue + 1));
                    if ((fn_800E0C54() & 1) != 0) {
                        flagBase[5] = 0;
                        fn_80211B94(lbl_8047B62C, lbl_80379332, 0);
                    }
                    else {
                        flagBase[5] = 1;
                        fightFloorSetStatus(0, 0, 0x43, 0, target);
                        effectValue = fn_80232110(ctx, ctx, subCtx, 1, 0x28, -1);
                        wazaSetStatus(transformed, 0, 0x2d, 0, effectValue);
                        pokemonSetStatus(ctx, 0, 0x107, 0, 1);
                        lbl_8047B618 = lbl_8047B618 | 0x80000;
                        lbl_8047B610 = lbl_80379332;
                    }
                }
                result = 1;
            }
            lbl_8047B644++;
            break;
        case 10:
            if (fn_802026E4(ctx, 5) == 1 && ((u16)fn_800E0C54() % 4) == 0) {
                pokemonSetStatus(ctx, 0, 0x106, 0, 1);
                fightOutPokemonInitJoutaiKeep(ctx);
                result = 1;
                lbl_8047B618 = lbl_8047B618 | 0x80000;
                lbl_8047B610 = lbl_803792DF;
            }
            lbl_8047B644++;
            break;
        case 11:
            if (fn_802026E4(ctx, 0xa) == 1) {
                relativeTarget = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                    fn_80201D84(ctx, 0xa), floorStatus);
                fightFloorSetStatus(0, 0, 0x4b, 0,
                    relativeTarget);
                if ((fn_800E0C54() & 1) != 0) {
                    fn_80211B94(lbl_8047B62C, lbl_803793D7, 0);
                }
                else {
                    fightOutPokemonInitJoutaiKeep(ctx);
                    pokemonSetStatus(ctx, 0, 0x10c, 0, 1);
                    lbl_8047B618 = lbl_8047B618 | 0x80000;
                    fn_80211B94(lbl_8047B62C, lbl_803793D7, 0);
                    lbl_8047B610 = lbl_803793EA;
                }
                result = 1;
            }
            lbl_8047B644++;
            break;
        case 12:
            if (fn_802026E4(ctx, 0xc) == 1) {
                countValue = fn_80202108(ctx, 0xc);
                threshold = fn_80202234(ctx, 0xc);
                nextCount = (s8)countValue + 1;
                if (nextCount >= threshold) {
                    fightOutPokemonWriteJoutaiDataId(ctx, 0xc);
                    fightOutPokemonWriteJoutaiDataId(ctx, 0x22);
                    lbl_8047B618 = lbl_8047B618 | 0x2000000;
                    storedTarget = pokemonGetStatus(ctx, 0, 0xf5, 0);
                    selectedTarget = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                        (u16)pokemonGetStatus(ctx, 0, 0xf6, 0), floorStatus);
                    if (storedTarget == 0) {
                        goto case12_invalid_target;
                    }
                    fightWazaSetUseWazaStatus(transformed, 0x75);
                    moveId = fightOutPokemonGetUseWazaDataId(ctx);
                    lbl_8047B620 = storedTarget * 2;
                    if (fightOutPokemonCheckFightOut(selectedTarget) != 0) {
                        goto case12_valid_target;
                    }
                    selectedTarget = fn_8022B2CC(ctx, moveId, floorStatus, 0, 1, 1, 0);
                    if (fightOutPokemonCheckFightOut(selectedTarget) == 0) {
                        goto case12_invalid_target;
                    }
                case12_valid_target:
                    fightFloorSetStatus(0, 0, 0x43, 0, selectedTarget);
                    lbl_8047B610 = lbl_80378C3B;
                    target = selectedTarget;
                    goto case12_target_done;
                case12_invalid_target:
                    lbl_8047B610 = lbl_80378CB6;
                case12_target_done:;
                }
                else {
                    fn_80201FDC(ctx, 0xc, nextCount);
                    lbl_8047B610 = lbl_80378C23;
                }
                result = 1;
            }
            lbl_8047B644++;
            break;
        case 13:
            if (fn_802026E4(ctx, 7) == 1) {
                if (moveStatus9 == 0x7d) {
                    fightOutPokemonWriteJoutaiDataId(ctx, 7);
                    flagBase[5] = 1;
                    fn_80211B94(lbl_8047B62C, lbl_803792B3, 0);
                }
                result = 2;
            }
            lbl_8047B644++;
            break;
        case 14:
            if (fn_802026E4(ctx, 0x3e) == 0) {
                if (fn_8022EDEC(ctx, 0) == 1) {
                    fightOutPokemonInitJoutaiKeep(ctx);
                    result = 2;
                    lbl_8047B610 = lbl_80379212;
                }
            }
            else if (fn_8022EDEC(ctx, 2) == 1) {
                result = 2;
            }
            lbl_8047B644++;
            break;
        case 15:
            break;
        }
    } while ((s32)lbl_8047B644 != 0xf && (u8)result == 0);

    if ((u8)result == 2 && fightOutPokemonIsUseHensinBuff(ctx) == 1) {
        fightOutPokemonSetHensinPokemonStatusId(ctx, 0x7c, 0, 0);
    }
    return result;
}
#pragma switch_tables reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_8022FE80(void) {
    extern u8 lbl_8027A00C[];
    extern u8 lbl_80378798[];
    extern u8 lbl_8047B614;
    extern void* lbl_8047B62C;
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fn_8022E1F8();
    extern void fn_8022E34C();
    extern void fn_8022EB9C();
    extern void fn_80230088();
    extern void fn_8023011C();
    u8 sp[2];
    u32 saved610;
    u8 saved614;
    u32 saved62c;
    u8* start;
    u32 dispatch;

    fightFloorLoopValidFightOutPokemon(0, fn_8023011C, 0, 0);
    saved62c = (u32)lbl_8047B62C;
    start = lbl_80378798;
    saved614 = lbl_8047B614;
    dispatch = (u32)lbl_8027A00C;
    saved610 = (u32)lbl_8047B610;
    lbl_8047B614 = 0;
    lbl_8047B610 = start;
    lbl_8047B62C = (void*)saved62c;
    do {
        ((void (*)(void))*(u32*)(dispatch + (*lbl_8047B610 * 4)))();
    } while (lbl_8047B614 != 1 && lbl_8047B614 != 2);

    lbl_8047B62C = (void*)saved62c;
    lbl_8047B614 = saved614;
    lbl_8047B610 = (u8*)saved610;
    sp[1] = 1;
    fightFloorLoopValidFightOutPokemon(0, fn_8022E34C, &sp[1], 0);
    fightFloorLoopValidFightOutPokemon(0, fn_8022E1F8, 0, 0);
    fightFloorLoopValidFightOutPokemon(0, fn_80230088, 0, 0);
    sp[0] = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_8022EB9C, &sp[0], 0);
}
#pragma opt_propagation reset
#pragma optimize_for_size reset
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_8022FF90(void) {
    extern u8 lbl_8027A00C[];
    extern u8 lbl_80378798[];
    extern u8 lbl_8047B614;
    extern void* lbl_8047B62C;
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fn_8022E1F8();
    extern void fn_8022E34C();
    extern void fn_8022EB9C();
    extern void fn_80230088();
    u8 sp[2];
    u32 saved610;
    u8 saved614;
    u32 saved62c;
    u8* start;
    u32 dispatch;

    start = lbl_80378798;
    dispatch = (u32)lbl_8027A00C;
    saved62c = (u32)lbl_8047B62C;
    saved614 = lbl_8047B614;
    saved610 = (u32)lbl_8047B610;
    lbl_8047B614 = 0;
    lbl_8047B610 = start;
    lbl_8047B62C = (void*)saved62c;
    do {
        ((void (*)(void))*(u32*)(dispatch + (*lbl_8047B610 * 4)))();
    } while (lbl_8047B614 != 1 && lbl_8047B614 != 2);

    lbl_8047B62C = (void*)saved62c;
    lbl_8047B614 = saved614;
    lbl_8047B610 = (u8*)saved610;
    sp[1] = 1;
    fightFloorLoopValidFightOutPokemon(0, fn_8022E34C, &sp[1], 0);
    fightFloorLoopValidFightOutPokemon(0, fn_8022E1F8, 0, 0);
    fightFloorLoopValidFightOutPokemon(0, fn_80230088, 0, 0);
    sp[0] = 0;
    fightFloorLoopValidFightOutPokemon(0, fn_8022EB9C, &sp[0], 0);
}
#pragma opt_propagation reset
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_802062FC fightOutPokemonCheckFightOut
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F4C14 fightFloorSetStatus
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
#undef fn_802062FC
#undef fn_801F54A4
#undef fn_801F4C14
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_802301A8(u32 r3)
{
    extern void wazaSetStatus();
    extern void msgctrlSetValue();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fn_80211B94();
    extern void fn_8022FE80();
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
    extern u8 lbl_80378E1E[];
    extern u8 lbl_80378E46[];
    s8 threshold;
    s8 count;
    s32 signedCount;
    u32 moveStatus;
    u32 context;
    u32 pokemon;
    u16 hp;
    u8* script;

    context = r3;
    if (fightOutPokemonCheckFightOut(context) == 0) {
        return 1;
    }

    lbl_8047B618 = (lbl_8047B618 | 0x1000000) | 0x20;
    if (fn_802026E4(context, 0x1E) == 1) {
        threshold = fn_80202234(context, 0x1E);
        count = fn_80202108(context, 0x1E);
        fightFloorSetStatus(0, 0, 0x36, 0, context);
        moveStatus = pokemonGetStatus(context, 0, 0xD9, 0);
        signedCount = count;
        msgctrlSetValue(0x2F, (s8)threshold - signedCount);

        if ((int)count < threshold) {
            goto increment_counter;
        }
        pokemon = fightOutPokemonGetPokemonPtr(context);
        hp = pokemonGetStatus(pokemon, 0, 0x83, 0);
        wazaSetStatus(moveStatus, 0, 0x2D, 0, hp);
        fightOutPokemonWriteJoutaiDataId(context, 0x1E);
        script = lbl_80378E1E;
        goto show_message;
increment_counter:
        fn_80201FDC(context, 0x1E, (s8)(signedCount + 1));
        script = lbl_80378E46;
show_message:
        fn_80211B94(lbl_8047B62C, script, 0);
        fn_8022FE80();
    }
    lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
    return 1;
}
#pragma optimize_for_size reset
u32 fn_80230318(u32 r3, u32 r4)

{
    extern u32 GSmsgGetGSchar();
    extern void wazaSetStatus();
    extern void wazaGetStatus();
    extern void msgctrlSetValue();
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetStatus();
    extern u32 pokemonGetStatus();
    extern void pokemonSetStatus();
    extern u32 fn_80201890();
    extern u32 fn_80201C58();
    extern u32 fn_80201D84();
    extern void fn_80201FDC();
    extern s8 fn_80202108();
    extern s8 fn_80202234();
    extern u8 fn_802026E4();
    extern void fn_80202810();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightWazaCreate();
    extern void fightWazaBiosCopy();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern void fn_80211B94();
    extern void fn_8022FE80();
    extern u8 lbl_80378F39[];
    extern u32 lbl_8047B618;
    extern void* lbl_8047B62C;
  s8 cVar5;
  s8 cVar6;
  u32 uVar1;
  u32 uVar2;
  u32 uVar3;

  u8 auStack_c8 [172];

  if (fightOutPokemonCheckFightOut() == 0) {
    return 1;
  }
  lbl_8047B618 = lbl_8047B618 | 0x1000020;
  cVar5 = fn_802026E4(r3,0x34);
  if (cVar5 == 1) {
    cVar5 = fn_80202234(r3,0x34);
    cVar6 = fn_80202108(r3,0x34);
    if (cVar6 < cVar5) {
      goto increment_state;
    }
        uVar1 = fn_80201890(r3,0x34);
        uVar2 = fn_80201C58(r3,0x34);
        uVar3 = fn_80201D84(r3,0x34);
        uVar3 = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(uVar3,r4);
        if (fightOutPokemonCheckFightOut() == 0) {
          uVar3 = fightTargetGetPtrAsNowFightType(0xe,uVar3);
        }
        if ((uVar2 & 0xffff) == 0xf8) {
          lbl_80478D78[5] = 0;
        }
        else {
          lbl_80478D78[5] = 1;
        }
        wazaGetStatus(0,uVar2,1,0);
        msgctrlSetValue(0xd,GSmsgGetGSchar());
        fightFloorSetStatus(0,0,0x42,0,r3);
        r4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(r3,r4);
        fightFloorSetStatus(0,0,0x36,0,uVar3);
        {
          u32 callResult = pokemonGetStatus(uVar3,0,0xd9,0);
          uVar3 = callResult;
        }
        fightWazaBiosCopy(auStack_c8,uVar3);
        fightWazaCreate(uVar3,0xffffffff,uVar2,r4,0);
        wazaSetStatus(uVar3,0,0x2d,0,uVar1);
        pokemonSetStatus((void*)r3,0,0x11b,0,0xffff);
        fightOutPokemonWriteJoutaiDataId(r3,0x34);
        fn_80211B94(lbl_8047B62C,lbl_80378F39,0);
        fightWazaBiosCopy(uVar3,auStack_c8);
        fn_8022FE80();
        goto clear_flags;
increment_state:
    fn_80201FDC(r3,0x34,(s8)(cVar6 + 1));
  }
clear_flags:
  lbl_8047B618 = lbl_8047B618 & 0xfeffffdf;
  return 1;
}
#pragma optimize_for_size on
u32 fn_80232024(u32 r3)

{
    extern void fightFloorSetStatus();
    extern void fn_80201FDC();
    extern u32 fn_80202108();
    extern u32 fn_80202234();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonCheckFightOut();
    extern void fn_80211B94();
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
  check = fn_802026E4(r3,0x35);
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
      fn_80211B94(lbl_8047B62C,lbl_80379052,0);
      fightOutPokemonWriteJoutaiDataId(r3,0x35);
    }
  }
  return 1;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_80232D28(u32 pokemon, u32 hostSide, u32* data)
{
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern void fightOutPokemonSetOumuWazaDataId();
    extern u32 fn_80201D84();
    extern void fn_80201FDC();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonIsAlly();
    extern void fn_80203198();
    extern u8 fightOutPokemonCheckFightOut();
    u16 statusTarget;
    u32 other;
    u32 targetId;
    u32 relative;
    u8 matched;

    statusTarget = (u16)data[1];
    other = data[0];
    if (fightOutPokemonCheckFightOut() == 0) {
        return 1;
    }
    if (pokemon == other) {
        return 1;
    }

    if (statusTarget == 0x7f) {
        if (fightOutPokemonIsAlly(other, pokemon) == 0) {
            if (fn_802026E4(pokemon, 0x1d) == 1 &&
                (targetId = fn_80201D84(pokemon, 0x1d), (targetId & 0xffff) != 0) &&
                (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                     (u16)targetId, hostSide)) != 0 &&
                relative == other) {
                matched = 1;
            } else {
                matched = 0;
            }
            if (matched == 1) {
                fn_80201FDC(pokemon, 0x1d, 0);
            }
        }
    } else {
        if (fn_802026E4(pokemon, 0x16) == 1 &&
            (targetId = fn_80201D84(pokemon, 0x16), (targetId & 0xffff) != 0) &&
            (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                 (u16)targetId, hostSide)) != 0 &&
            relative == other) {
            matched = 1;
        } else {
            matched = 0;
        }
        if (matched == 1) {
            fightOutPokemonWriteJoutaiDataId(pokemon, 0x16);
        }
        if (fn_802026E4(pokemon, 0x1d) == 1 &&
            (targetId = fn_80201D84(pokemon, 0x1d), (targetId & 0xffff) != 0) &&
            (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
                 (u16)targetId, hostSide)) != 0 &&
            relative == other) {
            matched = 1;
        } else {
            matched = 0;
        }
        if (matched == 1) {
            fightOutPokemonWriteJoutaiDataId(pokemon, 0x1d);
        }
    }
    if (fn_802026E4(pokemon, 10) == 1 &&
        (targetId = fn_80201D84(pokemon, 10), (targetId & 0xffff) != 0) &&
        (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
             (u16)targetId, hostSide)) != 0 &&
        relative == other) {
        matched = 1;
    } else {
        matched = 0;
    }
    if (matched == 1) {
        fightOutPokemonWriteJoutaiDataId(pokemon, 10);
    }
    if (fn_802026E4(pokemon, 0xe) == 1 &&
        (targetId = fn_80201D84(pokemon, 0xe), (targetId & 0xffff) != 0) &&
        (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
             (u16)targetId, hostSide)) != 0 &&
        relative == other) {
        matched = 1;
    } else {
        matched = 0;
    }
    if (matched == 1) {
        fightOutPokemonWriteJoutaiDataId(pokemon, 0xe);
    }
    if (fightOutPokemonIsAlly(pokemon, other) == 0) {
        fightOutPokemonSetOumuWazaDataId(pokemon, other, 0);
        fn_80203198(pokemon, other);
    }
    return 1;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_80232FE4(u32 pokemon, u32 hostSide, u32 other)

{
    extern u32 fightTargetGetRelativeHostSideFightTargetIdToTragetPtr();
    extern void fightOutPokemonSetOumuWazaDataId();
    extern u32 fn_80201D84();
    extern u8 fn_802026E4();
    extern void fightOutPokemonWriteJoutaiDataId();
    extern u8 fightOutPokemonIsAlly();
    extern void fn_80203198();
    extern u8 fightOutPokemonCheckFightOut();
  u8 matched;
  u32 targetId;
  u32 relative;

  if (fightOutPokemonCheckFightOut() == 0) {
    return 1;
  }
  if (other == pokemon) {
    return 1;
  }
  if (fn_802026E4(pokemon,0x16) == 1 &&
      (targetId = fn_80201D84(pokemon,0x16), (targetId & 0xffff) != 0) &&
      (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
          targetId,hostSide)) != 0 && relative == other) {
    matched = 1;
  } else {
    matched = 0;
  }
  if (matched == 1) {
    fightOutPokemonWriteJoutaiDataId(pokemon,0x16);
  }
  if (fn_802026E4(pokemon,10) == 1 &&
      (targetId = fn_80201D84(pokemon,10), (targetId & 0xffff) != 0) &&
      (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
          targetId,hostSide)) != 0 && relative == other) {
    matched = 1;
  } else {
    matched = 0;
  }
  if (matched == 1) {
    fightOutPokemonWriteJoutaiDataId(pokemon,10);
  }
  if (fn_802026E4(pokemon,0xe) == 1 &&
      (targetId = fn_80201D84(pokemon,0xe), (targetId & 0xffff) != 0) &&
      (relative = fightTargetGetRelativeHostSideFightTargetIdToTragetPtr(
          targetId,hostSide)) != 0 && relative == other) {
    matched = 1;
  } else {
    matched = 0;
  }
  if (matched == 1) {
    fightOutPokemonWriteJoutaiDataId(pokemon,0xe);
  }
  if (fightOutPokemonIsAlly(pokemon,other) == 0) {
    fightOutPokemonSetOumuWazaDataId(pokemon,other,0);
    fn_80203198(pokemon,other);
  }
  return 1;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
void fn_802331F4(u32 r3, u32 r4, int r5, u32 r6, u16 r7)
{
    typedef struct TrainerSnapshot {
        u32 words[85];
    } TrainerSnapshot;
    extern u32 fn_800E0C54();
    extern int fn_8010C54C();
    extern u32 fightFloorGetFightPokemonEntryCntInc();
    extern u32 fightFloorGetStatus();
    extern u8 fightTrainerIsAllyFightTargetPtr();
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern void fightPokemonCreate();
    extern u16 pokemonGetStatus();
    extern u32 fn_80233DB0();
  u8 cVar1;
  u32 uVar2;
  struct {
    u32 *value;
  } puVar3;
  u32 *puVar4;
  u32 uVar5;
  u16 uVar10;
  int iVar6;
  int iVar7;
  u8 cVar17;
  struct {
    u32 value;
  } uVar11;
  struct {
    u32 value;
  } uVar8;
  u16 uVar12;
  u16 uVar13;
  u32 uVar14;
  u32 uVar15;
  u16 sVar16;
  int iVar9;
  u32 *puVar18;
  u32 *puVar19;
  u32 uVar20;
  u8 bVar21;
  struct {
    u32 value;
  } uVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  int iVar26;
  u8 local_318 [8];
  u32 auStack_310 [5];
  u32 savedTrainer0[88];
  u32 savedTrainer1[85];
#define local_50 (*(volatile u32 *)&savedTrainer0[86])
#define local_4c (*(volatile u32 *)&savedTrainer0[87])

  uVar10 = fightTrainerGetStatus(r3,0,0x43,0);
  uVar10 = fightTrainerGetStatus(0,uVar10,2,0);
  iVar6 = fightTrainerGetStatus(r3,0,0x45,0);
  if ((iVar6 != 0) && (iVar7 = fightTrainerGetStatus(r3,0,0x45,1), iVar7 != 0)) {
    *(TrainerSnapshot *)(savedTrainer0 + 1) = *(TrainerSnapshot *)iVar6;
    *(TrainerSnapshot *)(savedTrainer1 + 1) = *(TrainerSnapshot *)iVar7;
    for (uVar20 = 0; (u16)uVar20 < (u16)r6; uVar20 = uVar20 + 1) {
      *(u32 *)(r5 + (uVar20 & 0xffff) * 4) = 0;
    }
    uVar11.value = 0;
    uVar20 = 0;
    for (; (u16)uVar11.value < 6; uVar11.value = uVar11.value + 1) {
      local_318[(u16)uVar11.value] = 0;
    }
    cVar17 = fightTrainerGetStatus(0,uVar10,0x22,0);
    uVar11.value = 1;
    if (cVar17 != 1) {
      uVar11.value = r7;
    }
    cVar17 = fightTrainerGetStatus(0,uVar10,0x1f,0);
    if (cVar17 == 1) {
      u8 *typeBase = local_318;
      int resultOffset;
loop_type0_top:
      if ((u16)uVar20 >= (u16)r6) goto loop_type0_done;
      uVar11.value = fn_80233DB0(r3,r4,r5,r6,0,auStack_310);
      if ((u16)uVar11.value == 0) goto loop_type0_done;
      {
        uVar8.value = fn_800E0C54();
        resultOffset = (uVar20 & 0xffff) * 4;
        *(u32 *)(r5 + resultOffset) =
            auStack_310[(u16)((int)(uVar8.value & 0xffff) % (int)(u16)uVar11.value)];
        uVar5 = fightFloorGetFightPokemonEntryCntInc(0);
        fightPokemonCreate(iVar6,*(u32 *)(r5 + resultOffset),uVar5);
        fightFloorGetStatus(0,0,0x14,0);
        uVar12 = fightTrainerGetStatus(r3,0,0x43,0);
        uVar12 = fightTrainerGetStatus(0,uVar12,2,0);
        uVar5 = fightPokemonGetPokemonPtr(iVar6);
        uVar13 = (int)pokemonGetStatus(uVar5,0,0xc9,0);
        cVar17 = fightTrainerGetStatus(0,uVar12,0x23,0);
        if (cVar17 == 1) {
          cVar17 = fightTrainerGetStatus(0,uVar13,0x1c,0);
        }
        else {
          cVar17 = 0;
        }
        typeBase[uVar20 & 0xffff] = cVar17;
        uVar20 = uVar20 + 1;
        goto loop_type0_top;
      }
loop_type0_done:
      ;
    }
    else {
      {
      u8 *typeOneBase = local_318;
      u32 outputIndex;
      uVar15 = 0;
loop_type1_top:
      if ((u16)uVar15 >= (u16)uVar11.value) goto loop_type1_done;
      uVar14 = fn_80233DB0(r3,r4,r5,r6,1,auStack_310);
      if ((u16)uVar14 == 0) goto loop_type1_done;
      {
        uVar22.value = fn_800E0C54();
        typeOneBase[uVar20 & 0xffff] = 2;
        outputIndex = uVar20 & 0xffff;
        uVar20 = uVar20 + 1;
        uVar15 = uVar15 + 1;
        *(u32 *)(r5 + outputIndex * 4) =
            auStack_310[(u16)((int)(uVar22.value & 0xffff) % (int)(u16)uVar14)];
        goto loop_type1_top;
      }
loop_type1_done:
      ;
      }
      {
      u8 *typeTwoBase = (u8 *)auStack_310;
      puVar18 = (u32 *)local_318;
      uVar15 = 0;
loop_type2_top:
      if ((u16)uVar15 >= (u16)uVar11.value) goto loop_type2_done;
      uVar14 = fn_80233DB0(r3,r4,r5,r6,2,auStack_310);
      if ((u16)uVar14 == 0) goto loop_type2_done;
      {
        uVar22.value = fn_800E0C54();
        ((u8 *)puVar18)[uVar20 & 0xffff] = 3;
        uVar8.value = uVar20 & 0xffff;
        uVar20 = uVar20 + 1;
        uVar15 = uVar15 + 1;
        *(u32 *)(r5 + uVar8.value * 4) =
            ((u32 *)typeTwoBase)[(u16)((int)(uVar22.value & 0xffff) % (int)(u16)uVar14)];
        goto loop_type2_top;
      }
loop_type2_done:
      ;
      }
      {
      u8 *typeThreeBase = local_318;
      int resultOffset;
loop_type3_top:
      if ((u16)uVar20 >= (u16)r6) goto loop_type3_done;
      uVar11.value = fn_80233DB0(r3,r4,r5,r6,3,auStack_310);
      if ((u16)uVar11.value == 0) goto loop_type3_done;
      {
        uVar8.value = fn_800E0C54();
        resultOffset = (uVar20 & 0xffff) * 4;
        *(u32 *)(r5 + resultOffset) =
            auStack_310[(u16)((int)(uVar8.value & 0xffff) % (int)(u16)uVar11.value)];
        uVar5 = fightFloorGetFightPokemonEntryCntInc(0);
        fightPokemonCreate(iVar6,*(u32 *)(r5 + resultOffset),uVar5);
        fightFloorGetStatus(0,0,0x14,0);
        uVar12 = fightTrainerGetStatus(r3,0,0x43,0);
        uVar12 = fightTrainerGetStatus(0,uVar12,2,0);
        uVar5 = fightPokemonGetPokemonPtr(iVar6);
        uVar13 = (int)pokemonGetStatus(uVar5,0,0xc9,0);
        cVar17 = fightTrainerGetStatus(0,uVar12,0x23,0);
        if (cVar17 == 1) {
          cVar17 = fightTrainerGetStatus(0,uVar13,0x1c,0);
        }
        else {
          cVar17 = 0;
        }
        typeThreeBase[uVar20 & 0xffff] = cVar17;
        uVar20 = uVar20 + 1;
        goto loop_type3_top;
      }
loop_type3_done:
      ;
      }
      puVar3.value = auStack_310;
loop_fill_top:
      if ((u16)uVar20 >= (u16)r6) goto loop_fill_done;
      uVar11.value = fn_80233DB0(r3,r4,r5,r6,0,auStack_310);
      if ((u16)uVar11.value == 0) goto loop_fill_done;
      {
        uVar22.value = fn_800E0C54();
        iVar26 = (uVar20 & 0xffff) * 4;
        uVar8.value = uVar20 & 0xffff;
        *(u32 *)(r5 + iVar26) =
            puVar3.value[(u16)((int)(uVar22.value & 0xffff) % (int)(u16)uVar11.value)];
        uVar5 = fightFloorGetFightPokemonEntryCntInc(0);
        fightPokemonCreate(iVar6,*(u32 *)(r5 + iVar26),uVar5);
        fightFloorGetStatus(0,0,0x14,0);
        uVar12 = fightTrainerGetStatus(r3,0,0x43,0);
        uVar12 = fightTrainerGetStatus(0,uVar12,2,0);
        uVar5 = fightPokemonGetPokemonPtr(iVar6);
        uVar13 = (int)pokemonGetStatus(uVar5,0,0xc9,0);
        cVar17 = fightTrainerGetStatus(0,uVar12,0x23,0);
        if (cVar17 == 1) {
          cVar17 = fightTrainerGetStatus(0,uVar13,0x1c,0);
        }
        else {
          cVar17 = 0;
        }
        local_318[uVar8.value] = cVar17;
        if (local_318[uVar8.value] == 2) {
          local_318[uVar8.value] = 3;
        }
        uVar20 = uVar20 + 1;
        goto loop_fill_top;
      }
loop_fill_done:
      ;
    }
    cVar17 = fightTrainerGetStatus(0,uVar10,0x1f,0);
    if (cVar17 != 1) {
      cVar17 = fightTrainerGetStatus(0,uVar10,0x21,0);
      if (cVar17 == 1) {
        for (r3 = 0; (u16)r3 < (u16)r6; r3 = r3 + 1) {
          iVar26 = (r3 & 0xffff) * 4;
          if (*(u32 *)(r5 + iVar26) != 0) {
            for (uVar8.value = r3 + 1 & 0xffff; (u16)uVar8.value < (u16)r6;
                uVar8.value = uVar8.value + 1) {
              iVar23 = (uVar8.value & 0xffff) * 4;
              if (*(u32 *)(r5 + iVar23) != 0) {
                uVar11.value = (u16)pokemonGetStatus(*(u32 *)(r5 + iVar26),0,0xc9,0);
                uVar15 = (u16)pokemonGetStatus(*(u32 *)(r5 + iVar23),0,0xc9,0);
                if (uVar11.value > uVar15) {
                  uVar5 = *(u32 *)(r5 + iVar26);
                  cVar17 = local_318[r3 & 0xffff];
                  cVar1 = local_318[uVar8.value & 0xffff];
                  *(u32 *)(r5 + iVar26) = *(u32 *)(r5 + iVar23);
                  local_318[r3 & 0xffff] = cVar1;
                  *(u32 *)(r5 + iVar23) = uVar5;
                  local_318[uVar8.value & 0xffff] = cVar17;
                }
              }
            }
          }
        }
      }
      else {
        for (uVar20 = 0; (u16)uVar20 < (u16)r6; uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          if (*(u32 *)(r5 + iVar26) != 0) {
            for (uVar8.value = uVar20 + 1 & 0xffff; (u16)uVar8.value < (u16)r6;
                uVar8.value = uVar8.value + 1) {
              iVar23 = (uVar8.value & 0xffff) * 4;
              iVar25 = *(u32 *)(r5 + iVar23);
              if ((((u32)iVar25 != 0) && (cVar17 = local_318[uVar20 & 0xffff], cVar17 != 2)) &&
                 (local_318[uVar8.value & 0xffff] == 2)) {
                uVar5 = *(u32 *)(r5 + iVar26);
                local_318[uVar20 & 0xffff] = local_318[uVar8.value & 0xffff];
                *(u32 *)(r5 + iVar26) = iVar25;
                *(u32 *)(r5 + iVar23) = uVar5;
                local_318[uVar8.value & 0xffff] = cVar17;
              }
            }
          }
        }
        for (uVar20 = 0; (u16)uVar20 < (u16)r6; uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          uVar8.value = uVar20 & 0xffff;
          if ((*(u32 *)(r5 + iVar26) != 0) && (local_318[uVar8.value] != 2)) {
            for (uVar22.value = uVar20 + 1 & 0xffff; (u16)uVar22.value < (u16)r6;
                uVar22.value = uVar22.value + 1) {
              iVar23 = (uVar22.value & 0xffff) * 4;
              uVar2 = uVar22.value & 0xffff;
              iVar25 = *(u32 *)(r5 + iVar23);
              if ((((u32)iVar25 != 0) && (local_318[uVar8.value] != 0)) && (local_318[uVar8.value] != 1)) {
                if ((local_318[uVar2] == 0) || (local_318[uVar2] == 1)) {
                  uVar5 = *(u32 *)(r5 + iVar26);
                  cVar17 = *(volatile u8 *)(local_318 + uVar8.value);
                  cVar1 = *(volatile u8 *)(local_318 + uVar2);
                  *(u32 *)(r5 + iVar26) = iVar25;
                  local_318[uVar8.value] = cVar1;
                  *(u32 *)(r5 + iVar23) = uVar5;
                  local_318[uVar2] = cVar17;
                }
              }
            }
          }
        }
        local_4c = r6 & 0xffff;
        for (uVar20 = 0; (u16)uVar20 < local_4c; uVar20 = uVar20 + 1) {
          iVar26 = (uVar20 & 0xffff) * 4;
          uVar8.value = uVar20 & 0xffff;
          if ((*(u32 *)(r5 + iVar26) != 0) &&
             ((local_318[uVar8.value] == 1 || (local_318[uVar8.value] == 0)))) {
            local_50 = r6 & 0xffff;
            for (uVar22.value = uVar20 + 1 & 0xffff; (u16)uVar22.value < local_50; uVar22.value = uVar22.value + 1) {
              iVar23 = (uVar22.value & 0xffff) * 4;
              uVar2 = uVar22.value & 0xffff;
              if (*(u32 *)(r5 + iVar23) != 0) {
                if ((local_318[uVar2] == 1) || (local_318[uVar2] == 0)) {
                  iVar25 = 0;
                  for (bVar21 = 0; bVar21 < 2; ++bVar21) {
                    uVar10 = fightFloorGetStatus(0,0,0x14,0);
                    uVar12 = fightTrainerGetStatus(r3,0,0x43,0);
                    uVar12 = fightTrainerGetStatus(0,uVar12,2,0);
                    uVar5 = fightPokemonGetPokemonPtr(iVar6);
                    uVar13 = (int)pokemonGetStatus(uVar5,0,0x6e,0);
                    cVar17 = fightTrainerGetStatus(0,uVar12,0x2a,0);
                    if (cVar17 == 1) {
                      cVar17 = fightTrainerIsAllyFightTargetPtr(r3,iVar6,uVar10);
                      if (cVar17 == 0) {
                        sVar16 = (int)pokemonGetStatus(0,uVar13,0x16,bVar21);
                      }
                      else {
                        sVar16 = (int)pokemonGetStatus(0,uVar13,0x16,bVar21);
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
                  for (bVar21 = 0; bVar21 < 2; ++bVar21) {
                    uVar10 = fightFloorGetStatus(0,0,0x14,0);
                    uVar12 = fightTrainerGetStatus(r3,0,0x43,0);
                    uVar12 = fightTrainerGetStatus(0,uVar12,2,0);
                    uVar5 = fightPokemonGetPokemonPtr(iVar7);
                    uVar13 = (int)pokemonGetStatus(uVar5,0,0x6e,0);
                    cVar17 = fightTrainerGetStatus(0,uVar12,0x2a,0);
                    if (cVar17 == 1) {
                      cVar17 = fightTrainerIsAllyFightTargetPtr(r3,iVar7,uVar10);
                      if (cVar17 == 0) {
                        sVar16 = (int)pokemonGetStatus(0,uVar13,0x16,bVar21);
                      }
                      else {
                        sVar16 = (int)pokemonGetStatus(0,uVar13,0x16,bVar21);
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
                    cVar17 = local_318[uVar8.value];
                    cVar1 = local_318[uVar2];
                    *(u32 *)(r5 + iVar26) = *(u32 *)(r5 + iVar23);
                    local_318[uVar8.value] = cVar1;
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
    *(TrainerSnapshot *)iVar6 = *(TrainerSnapshot *)(savedTrainer0 + 1);
    *(TrainerSnapshot *)iVar7 = *(TrainerSnapshot *)(savedTrainer1 + 1);
  }
  return;
#undef local_50
#undef local_4c
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
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
#pragma optimize_for_size reset
#pragma optimize_for_size on
#pragma opt_propagation off
#undef fn_801233F4
#undef fn_80123FBC
#undef fn_8012640C
#undef fn_8012A5B0
#undef fn_801F4804
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_80205BE8
#undef fn_80206AEC
void fn_80234A0C(s32 trainer)
{
    u32 partyLimit;
    s32 opponentCount;
    s32 partyCount;
    u16 minBaseStats;
    u32 party[8];
    u32 moveScores[8];
    u32 opponents[8];
    u16 moves[10];
    u32 statScores[8];
    u32 callbackD[5];
    u32 callback4D[5];
    u16 moveType;
    u16 hiddenPower;
    extern u32 fightTrainerGetStatus();
    extern void fightTrainerInitEnemyPokemonFightOutStatus();
    extern s32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern s16 fightOutPokemonGetFightEntryId();
    extern u32 pokemonGetStatus();
    extern u32 figthPokemonGetPokemonDataId();
    extern u32 fightFloorGetStatus();
    extern u32 fightOutPokemonGetNowHpPercentage();
    extern u8 figthPokemonGetLevel();
    extern u32 fightPokemonGetPokemonPtr();
    extern u8 fightFloorGetNowTenkouDataId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void pokemonGetMezamerupower();
    extern u32 wazaGetStatus();
    extern u8 wazaIsWazaTypeId();
    extern u32 fn_80236268();
    extern s32 fn_8023C370();
    extern void fightTrainerSetStatus();
    u16 trainerData;
    u16 partyIndex;
    struct {
        u32 value;
    } pokemon;
    u32 pokemonPtr;
    u16 pokemonData;
    struct {
        u32 value;
    } opponent;
    u16 status;
    s32 maxMoveScore;
    u16 clearIndex;
    u32 outPokemon;
    u16 moveCount;
    u16 moveClearIndex;
    u16 moveIndex;
    u16 opponentIndex;
    u16 selectedMoveIndex;
    u16 finalIndex;
    u16 index;
    struct {
        u32 value;
    } pokemonDataId;
    u16 move;
    u16 minHp;
    u16 baseStats;
    u8 weather;
    s16 entryId;
    u8 minLevel;
    s32 maxStatScore;

    fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
    fightTrainerInitEnemyPokemonFightOutStatus(trainer, 1);

    for (clearIndex = 0; clearIndex < 8; clearIndex++) {
        moveScores[clearIndex] = 0;
        statScores[clearIndex] = 0;
    }

    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, party, 0, 1);
    opponentCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(0, trainer, opponents, 1, 1);
    minHp = 0xffff;
    maxMoveScore = -0xffff;
    minBaseStats = 0xffff;
    maxStatScore = -0xffff;
    minLevel = 0xff;
    partyLimit = (u16)partyCount;

    for (partyIndex = 0; partyIndex < partyLimit; partyIndex++) {
        index = partyIndex;
        outPokemon = party[index];
        if (outPokemon == 0) {
            continue;
        }
        if (fightOutPokemonGetFightEntryId(outPokemon) < 0) {
            continue;
        }

        pokemonDataId.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonDataId.value = figthPokemonGetPokemonDataId(pokemonDataId.value);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);

        moveCount = 0;
        for (moveClearIndex = 0; moveClearIndex < 10; moveClearIndex++) {
            if (moves != NULL) {
                moves[moveClearIndex] = moveCount;
            }
        }
        for (moveIndex = 0; moveIndex < 4; moveIndex++) {
            pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
            fightFloorGetStatus(0, 0, 0x14, 0);
            fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
            fightPokemonGetPokemonPtr(pokemon.value);
            move = (u16)pokemonGetStatus(
                fightPokemonGetPokemonPtr(pokemon.value), 0, 0x7f, (u8)moveIndex);
            if (move != 0 && (move != 0x165 || move != 0x163)) {
                if (moves != NULL) {
                    moves[moveCount] = move;
                }
                moveCount++;
            }
        }

        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        status = fightOutPokemonGetNowHpPercentage(outPokemon);
        if (minHp > status) {
            minHp = status;
        }

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        status = figthPokemonGetLevel(pokemon.value);
        if (minLevel > (u8)status) {
            minLevel = (u8)status;
        }

        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        baseStats = (u16)pokemonGetStatus(0, pokemonDataId.value, 5, 0);
        baseStats += (u16)pokemonGetStatus(0, pokemonDataId.value, 7, 0);
        if (minBaseStats > baseStats) {
            minBaseStats = baseStats;
        }

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonPtr = fightPokemonGetPokemonPtr(pokemon.value);
        pokemonData = pokemonGetStatus(pokemonPtr, 0, 0x6e, 0);
        statScores[index] += (u16)pokemonGetStatus(0, pokemonData, 3, 0);

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonPtr = fightPokemonGetPokemonPtr(pokemon.value);
        pokemonData = pokemonGetStatus(pokemonPtr, 0, 0x6e, 0);
        statScores[index] += (u16)pokemonGetStatus(0, pokemonData, 4, 0);

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonPtr = fightPokemonGetPokemonPtr(pokemon.value);
        pokemonData = pokemonGetStatus(pokemonPtr, 0, 0x6e, 0);
        statScores[index] += (u16)pokemonGetStatus(0, pokemonData, 5, 0);

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonPtr = fightPokemonGetPokemonPtr(pokemon.value);
        pokemonData = pokemonGetStatus(pokemonPtr, 0, 0x6e, 0);
        statScores[index] += (u16)pokemonGetStatus(0, pokemonData, 6, 0);

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonPtr = fightPokemonGetPokemonPtr(pokemon.value);
        pokemonData = pokemonGetStatus(pokemonPtr, 0, 0x6e, 0);
        statScores[index] += (u16)pokemonGetStatus(0, pokemonData, 7, 0);

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonPtr = fightPokemonGetPokemonPtr(pokemon.value);
        pokemonData = pokemonGetStatus(pokemonPtr, 0, 0x6e, 0);
        statScores[index] += (u16)pokemonGetStatus(0, pokemonData, 8, 0);

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        statScores[index] *= figthPokemonGetLevel(pokemon.value);
        if (maxStatScore < (s32)statScores[index]) {
            maxStatScore = statScores[index];
        }

        for (opponentIndex = 0; opponentIndex < (u16)opponentCount; opponentIndex++) {
            opponent.value = opponents[opponentIndex];
            if (opponent.value == 0) {
                continue;
            }
            for (selectedMoveIndex = 0; selectedMoveIndex < moveCount; selectedMoveIndex++) {
                move = moves[selectedMoveIndex];
                if (move == 0 || move == 0x165) {
                    continue;
                }

                trainerData = fightTrainerGetStatus(trainer, 0, 0x43, 0);
                trainerData = fightTrainerGetStatus(0, (u16)trainerData, 2, 0);
                weather = fightFloorGetNowTenkouDataId(0, 0);
                callbackD[0] = 0xd;
                callbackD[1] = 0;
                callbackD[2] = 0;
                callbackD[3] = 0;
                callbackD[4] = trainer;
                fightFloorLoopValidFightOutPokemon(0, fn_80236268, callbackD, 0);
                if (callbackD[1] != 0) {
                    weather = 0;
                } else {
                    callback4D[0] = 0x4d;
                    callback4D[1] = 0;
                    callback4D[2] = 0;
                    callback4D[3] = 0;
                    callback4D[4] = trainer;
                    fightFloorLoopValidFightOutPokemon(0, fn_80236268, callback4D, 0);
                    if (callback4D[1] != 0) {
                        weather = 0;
                    }
                }

                if ((u8)fightTrainerGetStatus(0, (u16)trainerData, 0x2a, 0) == 1) {
                    if (move == 0xa5 || move == 0x164 || move == 0xf8 || move == 0x161) {
                        moveType = 9;
                    } else if (move == 0xed) {
                        pokemonGetMezamerupower(
                            fightOutPokemonGetPokemonPtr(outPokemon), 0, &hiddenPower);
                        moveType = hiddenPower;
                    } else if (move == 0x137) {
                        if (weather == 2) {
                            hiddenPower = 0xb;
                        } else if (weather == 3) {
                            hiddenPower = 5;
                        } else if (weather == 1) {
                            hiddenPower = 0xa;
                        } else if (weather == 4) {
                            hiddenPower = 0xf;
                        } else {
                            hiddenPower = 0;
                        }
                        moveType = hiddenPower;
                    } else {
                        moveType = (u16)wazaGetStatus(0, move, 3, 0);
                    }
                } else {
                    moveType = 9;
                }

                if (moveType != 9) {
                    fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
                    if (wazaIsWazaTypeId(move, 1) != 0) {
                        moveScores[index] += fn_8023C370(trainer, outPokemon, move, opponent.value, 0);
                    }
                }
            }
        }
        if (maxMoveScore < (s32)moveScores[index]) {
            maxMoveScore = moveScores[index];
        }
    }

    for (finalIndex = 0; finalIndex < (u16)partyCount; finalIndex++) {
        index = finalIndex;
        outPokemon = party[index];
        if (outPokemon == 0) {
            continue;
        }
        entryId = fightOutPokemonGetFightEntryId(outPokemon);
        if (entryId < 0) {
            continue;
        }

        pokemonDataId.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        pokemonDataId.value = figthPokemonGetPokemonDataId(pokemonDataId.value);

        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        if ((s32)minHp >= (s32)fightOutPokemonGetNowHpPercentage(outPokemon)) {
            fightTrainerSetStatus(trainer, (u16)entryId, 0x52, 0, 1);
        }

        pokemon.value = pokemonGetStatus(outPokemon, 0, 0xd6, 0);
        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        if (minLevel >= figthPokemonGetLevel(pokemon.value)) {
            fightTrainerSetStatus(trainer, (u16)entryId, 0x53, 0, 1);
        }

        fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
        baseStats = (u16)pokemonGetStatus(0, pokemonDataId.value, 5, 0);
        baseStats += (u16)pokemonGetStatus(0, pokemonDataId.value, 7, 0);
        if (minBaseStats >= baseStats) {
            fightTrainerSetStatus(trainer, (u16)entryId, 0x54, 0, 1);
        }
        if (maxMoveScore <= (s32)moveScores[index]) {
            fightTrainerSetStatus(trainer, (u16)entryId, 0x55, 0, 1);
        }
        if (maxStatScore <= (s32)statScores[index]) {
            fightTrainerSetStatus(trainer, (u16)entryId, 0x56, 0, 1);
        }
    }
}
#pragma opt_propagation reset
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8020156C fightOutPokemonGetAllAbiCnt
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
#pragma optimize_for_size reset
#pragma optimize_for_size on
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
#undef fn_801FB1C0
#undef fn_8020156C
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_80235BE4(u32 trainer, u32 floor, u32 fightOut, u32* outValue)

{
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fightFloorGetStatus();
    extern u8 fightTrainerIsAllyFightTargetPtr();
    extern u32 fightTrainerGetStatus();
    extern u8 fn_802026E4();
    extern u8 fightOutPokemonCheckValid();
    extern u32 fightOutPokemonGetZokuseiDataId();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern u32 fn_80236268();
  u8 bVar1;
  u8 bVar2;
  u32 iVar3;
  u32 iVar4;
  u32 iVar5;
  u8 cVar10;
  u8 flag;
  u16 uVar7;
  u16 uVar8;
  u32 sVar9;
  u32 uVar6;
  u32 check17[5];
  u32 check47[5];
  u32 check2A[5];

  bVar1 = 0;
  bVar2 = 0;
  cVar10 = fightOutPokemonCheckValid(fightOut);
  if (cVar10 == 0) {
    return 0;
  }
  check17[0] = 0x17;
  check17[1] = 0;
  check17[2] = 2;
  check17[3] = fightOut;
  check17[4] = trainer;
  fightFloorLoopValidFightOutPokemon(floor, fn_80236268, check17, 0);
  iVar5 = check17[1];
  check47[0] = 0x47;
  check47[1] = 0;
  check47[2] = 2;
  check47[3] = fightOut;
  check47[4] = trainer;
  fightFloorLoopValidFightOutPokemon(floor, fn_80236268, check47, 0);
  iVar4 = check47[1];
  check2A[0] = 0x2a;
  check2A[1] = 0;
  check2A[2] = 0;
  check2A[3] = fightOut;
  check2A[4] = trainer;
  fightFloorLoopValidFightOutPokemon(floor, fn_80236268, check2A, 0);
  iVar3 = check2A[1];
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 == 2) {
    goto element_two_true;
  }
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 != 2) {
    goto element_two_false;
  }
element_two_true:
  flag = 1;
  goto element_two_done;
element_two_false:
  flag = 0;
element_two_done:
  if (flag == 1) {
    goto set_first_flag;
  }
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x2b,0);
  if (cVar10 == 1) {
    uVar6 = fightOutPokemonGetTokuseiDataId(fightOut);
  }
  else {
    uVar6 = 0;
  }
  uVar6 = __cntlzw(0x1a - (uVar6 & 0xffff));
  if ((uVar6 >> 5 & 0xff) != 1) {
    goto first_flag_done;
  }
set_first_flag:
  bVar1 = 1;
first_flag_done:
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,0);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 == 8) {
    goto element_eight_true;
  }
  uVar7 = fightFloorGetStatus(0,0,0x14,0);
  uVar8 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar8 = fightTrainerGetStatus(0,uVar8,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar8,0x2a,0);
  if (cVar10 == 1) {
    cVar10 = fightTrainerIsAllyFightTargetPtr(trainer,fightOut,uVar7);
    if (cVar10 == 0) {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
    else {
      sVar9 = fightOutPokemonGetZokuseiDataId(fightOut,1);
    }
  }
  else {
    sVar9 = 9;
  }
  if ((u16)sVar9 != 8) {
    goto element_eight_false;
  }
element_eight_true:
  flag = 1;
  goto element_eight_done;
element_eight_false:
  flag = 0;
element_eight_done:
  if (flag == 1) {
    bVar2 = 1;
  }
LAB_00233098:
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(fightOut,0x16);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 == 1) {
    goto return_one;
  }
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(fightOut,0xe);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 == 1) {
    goto return_one;
  }
  uVar7 = fightTrainerGetStatus(trainer,0,0x43,0);
  uVar7 = fightTrainerGetStatus(0,uVar7,2,0);
  cVar10 = fightTrainerGetStatus(0,uVar7,0x24,0);
  if (cVar10 == 1) {
    cVar10 = fn_802026E4(fightOut,0x25);
  }
  else {
    cVar10 = 0;
  }
  if (cVar10 != 1) {
    goto check_outputs;
  }
return_one:
  return 1;

check_outputs:
  if (iVar5 != 0) {
    if (outValue != (void *)0) {
      *outValue = iVar5;
    }
    return 2;
  }
  if ((iVar4 != 0) && (!bVar1)) {
    if (outValue != (void *)0) {
      *outValue = iVar4;
    }
    return 2;
  }
  if ((iVar3 != 0) && (bVar2 == 1)) {
    if (outValue != (void *)0) {
      *outValue = iVar3;
    }
    return 2;
  }
  return 0;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_80236268(u32 attacker, u32 unused, u32* state)

{
    extern u8 fightOutPokemonCheckFightOut();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightOutPokemonGetTokuseiDataId();
    u32 originalAttacker = attacker;
    u32 reference = state[3];
    u32 trainer;
    u32 relative;
    u32 attackerSide;
    s32 mode;
    u16 trainerId;
    u32 actual;
    u8 matches;

    trainer = state[4];
    if (fightOutPokemonCheckFightOut(originalAttacker) == 0) {
        return 1;
    }

    if (reference == 0) {
        relative = 0;
    } else if ((s32)state[2] == 1) {
        relative = fightTargetGetPtrAsNowFightType(2, reference);
    } else if ((s32)state[2] == 2) {
        relative = fightTargetGetPtrAsNowFightType(3, reference);
    } else {
        relative = 0;
    }

    attackerSide = fightTargetGetPtrAsNowFightType(2, originalAttacker);
    mode = state[2];
    if ((mode == 1 || mode == 2) && relative == 0) {
        return 1;
    }
    if (mode == 0) {
        if (reference != 0 && reference == originalAttacker) {
            return 1;
        }
        goto mode_done;
    }
    if (mode == 1) {
        goto compare_side;
    }
    if (mode != 2) {
        goto invalid_mode;
    }
compare_side:
    if (relative == attackerSide) {
        goto mode_done;
    }
    return 1;
invalid_mode:
    return 1;
mode_done:

    attacker = (u16)state[0];
    trainerId = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    fightTrainerGetStatus(0, trainerId, 2, 0);
    if (attacker == 0) {
        matches = 0;
    } else {
        fightFloorGetStatus(0, 0, 0x14, 0);
        trainerId = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
        trainerId = (u16)fightTrainerGetStatus(0, trainerId, 2, 0);
        if ((u8)fightTrainerGetStatus(0, trainerId, 0x2b, 0) == 1) {
            actual = fightOutPokemonGetTokuseiDataId(originalAttacker);
        } else {
            actual = 0;
        }
        if (attacker == (u16)actual) {
            matches = 1;
        } else {
            matches = 0;
        }
    }

    if (matches == 1) {
        state[1] = originalAttacker;
        return 0;
    }
    return 1;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_80236584(u32 ctx, u32 fightOut, u16 wantedMove, s16* outSlot, u8 checkUsable)
{
    extern void fightFloorGetStatus();
    extern u16 fightTrainerGetStatus();
    extern u32 pokemonGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u8 fightOutPokemonCheckCanOutOkWazaBanme();
    u16 moveIds[10];
    s16 slots[10];
    struct {
        u16 value;
    } count;
    u16 slot;
    struct {
        u16 value;
    } i;

    fightFloorGetStatus(0, 0, 0x14, 0);
    fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0), 2, 0);

    count.value = 0;
    for (i.value = count.value; i.value < 10; i.value++) {
        if (moveIds != 0) {
            moveIds[i.value] = count.value;
        }
        if (slots != 0) {
            slots[i.value] = -1;
        }
    }

    for (slot = 0; slot < 4; slot++) {
        struct {
            u32 value;
        } pokemon;
        u8 slot8;
        slot8 = (u8)slot;
        pokemon.value = pokemonGetStatus(fightOut, 0, 0xD6, 0);
        fightFloorGetStatus(0, 0, 0x14, 0);
        fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0), 2, 0);
        fightPokemonGetPokemonPtr(pokemon.value);
        pokemon.value = fightPokemonGetPokemonPtr(pokemon.value);
        pokemon.value = (u16)pokemonGetStatus(pokemon.value, 0, 0x7F, slot8);
        if (pokemon.value == 0) {
            continue;
        }
        if (pokemon.value == 0x165) {
            if (pokemon.value == 0x163) {
                continue;
            }
        }
        if (checkUsable == 1) {
            fightFloorGetStatus(0, 0, 0x14, 0);
            fightTrainerGetStatus(0, fightTrainerGetStatus(ctx, 0, 0x43, 0), 2, 0);
            if (fightOutPokemonCheckCanOutOkWazaBanme(fightOut, slot8, 0, 0) != 0) {
                continue;
            }
        }
        if (moveIds != 0) {
            moveIds[count.value] = pokemon.value;
        }
        if (slots != 0) {
            slots[count.value] = slot;
        }
        count.value++;
    }

    for (i.value = 0; i.value < count.value; i.value++) {
        if (wantedMove == moveIds[i.value]) {
            if (outSlot != 0) {
                *outSlot = slots[i.value];
            }
            return 1;
        }
    }
    return 0;
}
#pragma optimize_for_size reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_801FFEC8 fightOutPokemonCheckCanOutOkWazaBanme
u32 fn_802367CC(void* ctx, u32 fightOut, void* moveIds, u32 slots,
                u32 checkUsable)
{
    extern void fn_801F54A4();
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    extern u8 fn_801FFEC8();
    extern u32 fn_80205BE8();
    u16 trainer;
    u32 pokemon;
    u8 slot8;
    u16 slot;
    u32 count;

  fn_801F54A4(0,0,0x14,0);
  trainer = fn_801FB1C0(ctx,0,0x43,0);
  fn_801FB1C0(0,trainer,2,0);
  count = 0;
  for (slot = count; slot < 10; slot++) {
    if (moveIds != 0) {
      ((u16*)moveIds)[slot] = count;
    }
    if (slots != 0) {
      ((s16*)slots)[slot] = -1;
    }
  }

  for (slot = 0; slot < 4; slot++) {
    slot8 = (u8)slot;
    pokemon = (0, fn_8012640C(fightOut,0,0xd6,0));
    fn_801F54A4(0,0,0x14,0);
    trainer = fn_801FB1C0(ctx,0,0x43,0);
    fn_801FB1C0(0,trainer,2,0);
    fn_80205BE8(pokemon);
    pokemon = fn_80205BE8(pokemon);
    pokemon = (u16)fn_8012640C(pokemon,0,0x7f,slot8);
    if (pokemon == 0) {
      continue;
    }
    if (pokemon == 0x165) {
      if (pokemon == 0x163) {
        continue;
      }
    }
    if ((u8)checkUsable == 1) {
      fn_801F54A4(0,0,0x14,0);
      trainer = fn_801FB1C0(ctx,0,0x43,0);
      fn_801FB1C0(0,trainer,2,0);
      if (fn_801FFEC8(fightOut,slot8,0,0) != 0) {
        continue;
      }
    }
    if (moveIds != 0) {
      ((u16*)moveIds)[(u16)count] = pokemon;
    }
    if (slots != 0) {
      ((s16*)slots)[(u16)count] = slot;
    }
    count++;
  }
  return count;
}
#undef fn_801FFEC8
#undef fn_80205BE8
#undef fn_8012640C
#undef fn_801FB1C0
#undef fn_801F54A4
u32 fn_802369B8(u32 trainer, u32 fightPokemon, u16* outMoves,
                s16* outSlots, u32 requirePp)
{
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
    u16 species;
    u16 index;
    u32 move;
    u32 pokemon;
    u32 count;

    fightFloorGetStatus(0, 0, 0x14, 0);
    species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    fightTrainerGetStatus(0, species, 2, 0);

    count = 0;
    for (index = 0; index < 10; index++) {
        if (outMoves != 0) {
            outMoves[index] = 0;
        }
        if (outSlots != 0) {
            outSlots[index] = -1;
        }
    }

    for (index = 0; index < 4; index++) {
        fightFloorGetStatus(0, 0, 0x14, 0);
        species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
        fightTrainerGetStatus(0, species, 2, 0);
        fightPokemonGetPokemonPtr(fightPokemon);
        pokemon = fightPokemonGetPokemonPtr(fightPokemon);
        move = (u16)pokemonGetStatus(pokemon, 0, 0x7f, (u8)index);

        if (move != 0 && (move != 0x165 || move != 0x163)) {
            if ((u8)requirePp == 1) {
                fightFloorGetStatus(0, 0, 0x14, 0);
                species = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
                fightTrainerGetStatus(0, species, 2, 0);
                fightPokemonGetPokemonPtr(fightPokemon);
                pokemon = fightPokemonGetPokemonPtr(fightPokemon);
                if ((u8)pokemonGetStatus(pokemon, 0, 0x80, (u8)index) == 0) {
                    continue;
                }
            }
            if (outMoves != 0) {
                outMoves[(u16)count] = move;
            }
            if (outSlots != 0) {
                outSlots[(u16)count] = index;
            }
            count++;
        }
    }
    return count;
}
#pragma optimize_for_size on
int fn_80236D60(u32 r3, u32 r4, u32 r5)

{
    extern u32 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  struct {
    u32 value;
  } uVar1;
  u16 uVar4;
  u16 uVar2;
  u32 uVar3;

  uVar1.value = (int)pokemonGetStatus(r4,0,0xd6,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar4 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar4,2,0);
  uVar1.value = fightPokemonGetPokemonPtr(uVar1.value);
  uVar4 = (int)pokemonGetStatus(uVar1.value,0,0x6e,0);
  uVar2 = (int)pokemonGetStatus(0,uVar4,8,0);
  uVar1.value = (int)pokemonGetStatus(r5,0,0xd6,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar4 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar4,2,0);
  uVar1.value = fightPokemonGetPokemonPtr(uVar1.value);
  uVar4 = (int)pokemonGetStatus(uVar1.value,0,0x6e,0);
  uVar3 = (int)pokemonGetStatus(0,uVar4,8,0);
  return (uVar2 & 0xffff) - (uVar3 & 0xffff);
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_802010C8 fightOutPokemonIsJoutaiKie
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
#undef fn_801FB1C0
#undef fn_802010C8
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
int fn_802373B0(u32 ctx, u32 fightOut, int direction, f32 ratio)
{
    extern u32 fn_801FB1C0();
    extern u32 fn_8012640C();
    extern u32 fn_80205BE8();
    extern f32 lbl_8047E618;
    extern f32 lbl_8047E61C;
    extern f64 lbl_8047E620;
    u32 pokemon;
    u32 pokemonPtr;
    u16 trainer;
    s32 current;
    u16 maximum;
    s32 scaled;
    u32 result;

    pokemon = (0, fn_8012640C(fightOut, 0, 0xD6, 0));
    trainer = fn_801FB1C0(ctx, 0, 0x43, 0);
    fn_801FB1C0(0, trainer, 2, 0);
    trainer = fn_801FB1C0(ctx, 0, 0x43, 0);
    fn_801FB1C0(0, trainer, 2, 0);
    pokemonPtr = fn_80205BE8(pokemon);
    current = (u16)fn_8012640C(pokemonPtr, 0, 0x83, 0);
    trainer = fn_801FB1C0(ctx, 0, 0x43, 0);
    fn_801FB1C0(0, trainer, 2, 0);
    pokemonPtr = fn_80205BE8(pokemon);
    maximum = fn_8012640C(pokemonPtr, 0, 0x87, 0);

    if (lbl_8047E61C == ratio) {
        ratio = lbl_8047E618;
    }
    scaled = (s32)((f32)maximum * ratio);

    if ((s8)direction == 0) {
        if (current == scaled) {
            result = 1;
            goto done;
        }
    } else if ((s8)direction > 0) {
        if (current >= scaled) {
            result = 1;
            goto done;
        }
    } else if ((s8)direction < 0) {
        if (current <= scaled) {
            result = 1;
            goto done;
        }
    }
    result = 0;
done:
    return result;
}
#undef fn_80205BE8
#undef fn_8012640C
#undef fn_801FB1C0
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_8012640C pokemonGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
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
#undef fn_8012640C
#undef fn_801FB1C0
#undef fn_80205BE8
#pragma optimize_for_size reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_801F8424 fightTrainerIsAllyFightTargetPtr
#define fn_80207B8C fightOutPokemonGetZokuseiDataId
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
#undef fn_801F54A4
#undef fn_801FB1C0
#undef fn_801F8424
#undef fn_80207B8C
#pragma optimize_for_size on
u32 fn_80238060(u32 r3, u32 r4, u8 r5)

{
    extern u32 pokemonWazaGetMaxPP();
    extern void fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  u32 uVar1;
  u16 uVar6;
  u16 uVar7;
  u32 uVar2;
  u8 uVar3;
  u8 uVar4;
  u8 uVar5;

  uVar1 = (int)pokemonGetStatus(r4,0,0xd6,0);
  uVar6 = fightTrainerGetStatus(r3,0,0x43,0);
  uVar6 = fightTrainerGetStatus(0,uVar6,2,0);
  fightFloorGetStatus(0,0,0x14,0);
  uVar7 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  fightPokemonGetPokemonPtr(uVar1);
  uVar2 = fightPokemonGetPokemonPtr(uVar1);
  uVar3 = (int)pokemonGetStatus(uVar2,0,0x80,r5);
  fightFloorGetStatus(0,0,0x14,0);
  uVar7 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar7,2,0);
  fightPokemonGetPokemonPtr(uVar1);
  uVar1 = fightPokemonGetPokemonPtr(uVar1);
  uVar4 = pokemonWazaGetMaxPP(uVar1,r5);
  uVar5 = fightTrainerGetStatus(0,uVar6,0x37,0);
  return (uVar3 * 100) / uVar4 <= uVar5;
}
#pragma optimize_for_size reset
#pragma optimize_for_size on
u32 fn_8023881C(u32 r3, u32 r4)

{
  u16 statusValues[2];
    extern u32 fightTrainerGetStatus();
    extern u32 fightPokemonGetPokemonPtr();
    extern u32 pokemonGetStatus();
  u16 uVar5;
  u16 uVar6;
  u32 uVar1;
  u8 uVar4;
  u8 bVar7;

  uVar5 = fightTrainerGetStatus(r3,0,0x43,0);
  uVar5 = fightTrainerGetStatus(0,uVar5,2,0);
  uVar6 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar6,2,0);
  uVar1 = fightPokemonGetPokemonPtr(r4);
  statusValues[0] = (int)pokemonGetStatus(uVar1,0,0x83,0);
  uVar6 = fightTrainerGetStatus(r3,0,0x43,0);
  fightTrainerGetStatus(0,uVar6,2,0);
  uVar1 = fightPokemonGetPokemonPtr(r4);
  statusValues[1] = (int)pokemonGetStatus(uVar1,0,0x87,0);
  uVar4 = fightTrainerGetStatus(0,uVar5,0x2c,0);
  bVar7 = (statusValues[0] * 100) / statusValues[1] <= uVar4;
  return bVar7;
}
#pragma optimize_for_size reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F8424 fightTrainerIsAllyFightTargetPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
int fn_802389D4(u32 r3, u32 r4)
{
    extern int fn_8010C54C();
    extern u32 pokemonGetStatus(u32, u32, u32, u8);
    extern u32 fn_801F54A4();
    extern u8 fn_801F8424();
    extern u32 fn_801FB1C0();
    extern u32 fn_80205BE8();
  u16 uVar5;
  u16 uVar4;
  u16 uVar3;
  u32 uVar1;
  u8 cVar7;
  u32 sVar6;
  int iVar2;
  u32 bVar9;
  int iVar8;

  iVar8 = 0;
  for (bVar9 = 0; (bVar9 & 0xff) < 2; bVar9 = bVar9 + 1) {
    uVar3 = fn_801F54A4(0,0,0x14,0);
    uVar4 = fn_801FB1C0(
        0, (u16)fn_801FB1C0(r3,0,0x43,0), 2, 0);
    uVar1 = fn_80205BE8(r4);
    uVar5 = (int)pokemonGetStatus(uVar1,0,0x6e,0);
    cVar7 = fn_801FB1C0(0,uVar4,0x2a,0);
    if (cVar7 == 1) {
      cVar7 = fn_801F8424(r3,r4,uVar3);
      if (cVar7 == 0) {
        sVar6 = pokemonGetStatus(0,uVar5,0x16,bVar9) & 0xffff;
      }
      else {
        sVar6 = pokemonGetStatus(0,uVar5,0x16,bVar9) & 0xffff;
      }
    }
    else {
      sVar6 = 9;
    }
    if ((u16)sVar6 != 9) {
      iVar2 = fn_8010C54C(sVar6,0);
      iVar8 = iVar8 + iVar2;
    }
  }
  return iVar8;
}
#undef fn_80205BE8
#undef fn_801FB1C0
#undef fn_801F8424
#undef fn_801F54A4
#define fn_801F54A4 fightFloorGetStatus
#define fn_801F8424 fightTrainerIsAllyFightTargetPtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205BE8 fightPokemonGetPokemonPtr
#define fn_801248C4 pokemonGetTokuseiDataId
#define fn_8012640C pokemonGetStatus
u32 fn_80238B0C(u32 r3, u32 r4, u32 r5, short r6)
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
  u32 uVar10;
  u16 uVar11;
  u32 uVar12;
  u32 bVar9;
  u16 local_38 [2];

  if ((r5 & 0xffff) == 9) {
    return 0x3f;
  }
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
    if (((uVar1 >> 5 & 0xff) == 1) && ((r5 & 0xffff) == 4U)) {
      return 0x43;
    }
      uVar1 = 0;
      for (bVar9 = 0; (bVar9 & 0xff) < 2; bVar9 = bVar9 + 1) {
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
            uVar7 = (int)fn_8012640C(0,uVar6,0x16,bVar9 & 0xFFFFu);
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
        return 0x3f;
      }
        uVar10 = fn_8010C650(r5,local_38,uVar1);
        uVar11 = fn_801FB1C0(r3,0,0x43,0);
        fn_801FB1C0(0,uVar11,2,0);
        fn_801F54A4(0,0,0x14,0);
        uVar11 = fn_801FB1C0(r3,0,0x43,0);
        uVar11 = fn_801FB1C0(0,uVar11,2,0);
        uVar12 = fn_80205BE8(r4);
        cVar8 = fn_801FB1C0(0,uVar11,0x2b,0);
        if (cVar8 == 1) {
          uVar3 = fn_801248C4(uVar12);
        }
        else {
          uVar3 = 0;
        }
        uVar3 = __cntlzw(0x19 - (uVar3 & 0xffff));
        if ((((uVar3 >> 5 & 0xff) == 1) && ((uVar10 & 0xffff) != 0x41)) && (0 < r6)) {
          return 0x43;
        }
  return uVar10;
}
#undef fn_8012640C
#undef fn_801248C4
#undef fn_80205BE8
#undef fn_801FB1C0
#undef fn_801F8424
#undef fn_801F54A4
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8011BEB4 wazaGetStatus
char fn_802392A8(u32 r3, u32 r4)

{
    extern int fn_8011BEB4();
    extern u32 fn_801FB1C0();
    extern u32 fn_80229B70();
    extern s8 fn_80229BD8();
  u16 uVar2;
  u8 cVar3;
  u8 bVar4;
  u8 bVar5;
  u16 uVar1;

  uVar2 = fn_801FB1C0(r3,0,0x43,0);
  uVar1 = fn_801FB1C0(0,uVar2,2,0);
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
  bVar5 = bVar4 >= bVar5;
  return bVar5;
}
#undef fn_8011BEB4
#undef fn_801FB1C0
u32 fn_802395C8(u32 trainer, u32 move, u32 target)
{
    extern u32 fightTrainerGetStatus();
    extern u8 fightFloorGetNowTenkouDataId();
    extern void fightFloorLoopValidFightOutPokemon();
    extern void fn_80236268();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void pokemonGetMezamerupower();
    extern u32 wazaGetStatus();
    u16 hiddenPower;
    u32 callbackD[5];
    u32 callback4D[5];
    u16 trainerData;
    struct {
        u8 value;
    } weather;

    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    weather.value = fightFloorGetNowTenkouDataId(0, 0);

    callbackD[0] = 0xd;
    callbackD[1] = 0;
    callbackD[2] = 0;
    callbackD[3] = 0;
    callbackD[4] = trainer;
    fightFloorLoopValidFightOutPokemon(0, fn_80236268, callbackD, 0);
    if (callbackD[1] != 0) {
        weather.value = 0;
    } else {
        callback4D[0] = 0x4d;
        callback4D[1] = 0;
        callback4D[2] = 0;
        callback4D[3] = 0;
        callback4D[4] = trainer;
        fightFloorLoopValidFightOutPokemon(0, fn_80236268, callback4D, 0);
        if (callback4D[1] != 0) {
            weather.value = 0;
        }
    }

    if ((u8)fightTrainerGetStatus(0, trainerData, 0x2a, 0) == 1) {
        if ((u16)move == 0xa5 || (u16)move == 0x164 ||
            (u16)move == 0xf8 || (u16)move == 0x161) {
            return 9;
        }
        if ((u16)move == 0xed) {
            pokemonGetMezamerupower(
                fightOutPokemonGetPokemonPtr(target), 0, &hiddenPower);
            return hiddenPower;
        }
        if ((u16)move == 0x137) {
            if (weather.value == 2) {
                hiddenPower = 0xb;
            } else if (weather.value == 3) {
                hiddenPower = 5;
            } else if (weather.value == 1) {
                hiddenPower = 0xa;
            } else if (weather.value == 4) {
                hiddenPower = 0xf;
            } else {
                hiddenPower = 0;
            }
            return hiddenPower;
        }
        return (u16)wazaGetStatus(0, move, 3, 0);
    }

    return 9;
}
#define fn_801FB1C0 fightTrainerGetStatus
#pragma optimize_for_size on
int fn_802398E4(int r3, int r4, u32 r5, u32 r6)

{
    extern int fn_801FB1C0();
  int iVar1;
  int result;
  int factor;

  iVar1 = fn_801FB1C0(0,r6,0x3e,0);
  factor = (short)(((r4 & 0xff) * 100) / 0xff) + -0x32;
  iVar1 = (factor *= iVar1);
  iVar1 = (short)(iVar1 / 0x32);
  result = r3;
  if (iVar1 > 0) {
    result = r3 + iVar1;
    if (result > 200) {
      result = 200;
    }
  } else {
    if (iVar1 < 0) {
      result = r3 + iVar1;
      if (result < -200) {
        result = -200;
      }
    }
  }
  return result;
}
#pragma optimize_for_size reset
u32 fn_80239984(int r3, void* r4, u32 r5)

{
    extern int fn_801FB1C0();
  int iVar1;
  int result;

  iVar1 = fn_801FB1C0(0,r5,0x3e,0);
  result = r3;
  if (iVar1 > 0) {
    result = r3 + iVar1;
    if (result > 200) {
      result = 200;
    }
  } else {
    if (iVar1 < 0) {
      result = r3 + iVar1;
      if (result < -200) {
        result = -200;
      }
    }
  }
  return result;
}
#undef fn_801FB1C0
#define fn_800FA280 GSmsgGetGSchar
#define fn_80103BA8 menuGetKeyInfo
#define fn_8011BEB4 wazaGetStatus
#define fn_8012640C pokemonGetStatus
#define fn_80132A38 msgctrlSetValue
#define fn_801F8100 fightTrainerGetNamePtr
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_8026246C fightMenuCloseMsg
#define fn_802624CC fightMenuOpenMsg
#pragma optimize_for_size on
u32
fn_80239A40(u32 r3, u32 r4, u32 r5, u32 r6, u32 r7,
    u32 r8, u32 r9, u32 r10, u8 param_9)

{
    extern u8 fn_80008164();
    extern u32 fn_800FA280();
    extern void fn_80103BA8();
    extern void fn_8011BEB4();
    extern void fn_80132A38();
    extern u32 fn_801F8100();
    extern int fn_801FB1C0();
    extern void fn_8026246C();
    extern u8 fn_802624CC();
  int iVar6;
  u32 iVar1;
  u32 iVar2;
  int iVar3;
  int factor;
  u32 uVar4;
  u8 cVar5;
  u16 local_58 [14];

  iVar6 = param_9;
  iVar1 = fn_801FB1C0(0,r10,0x40,0);
  iVar2 = fn_801FB1C0(0,r10,0x41,0);
  iVar3 = fn_801FB1C0(0,r10,0x3e,0);
  factor = (short)((iVar6 * 100) / 0xff) + -0x32;
  iVar3 = (int)(short)((factor * iVar3) / 0x32);
  iVar6 = 0;
  if (iVar3 > 0) {
    if (iVar3 > 200) {
      iVar6 = 200;
    } else {
      iVar6 = iVar3;
    }
  }
  else {
    if (iVar3 < 0) {
      if (iVar3 < -200) {
        iVar6 = -200;
      } else {
        iVar6 = iVar3;
      }
    }
  }
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
  fn_80132A38(0x2f,iVar6);
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
#pragma optimize_for_size reset
#undef fn_800FA280
#undef fn_80103BA8
#undef fn_8011BEB4
#undef fn_8012640C
#undef fn_80132A38
#undef fn_801F8100
#undef fn_801FB1C0
#undef fn_8026246C
#undef fn_802624CC
#pragma opt_common_subs off
int fn_8023A308(u32 r3, u32 r4, u32 r5)

{
    extern u32 fn_800E0C54();
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightTrainerGetStatus();
    extern void fightOutPokemonCreateFightActionAttackWaza();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern u32 fn_8022B2CC();
    extern u32 fn_802367CC();
    extern u32 fn_80238270();
    extern u32 fn_8023A118();
    extern int fn_8023A740();
    extern int fn_8023B498();
    extern void fn_8023C368();
    extern u8 lbl_80375CA8[];
  s32 sVar1;
  u16 sVar2;
  u16 uVar12;
  u32 uVar3;
  u32 uVar4;
  u32 uVar5;
  u8 cVar13;
  u32 uVar6;
  int iVar7;
  u32 uVar8;
  u32 uVar9;
  u32 uVar10;
  int iVar11;
  u32 selectedTarget;
  u32 overrideTarget;
  u32 local_64;
  s16 local_68 [2];
  u16 asStack_2c [10];
  s16 asStack_40 [10];
  u32 aiStack_60 [8];

  uVar12 = fightTrainerGetStatus(r3,0,0x43,0);
  uVar12 = fightTrainerGetStatus(0,uVar12,2,0);
  uVar3 = fn_802367CC(r3,r4,asStack_2c,asStack_40,1);
  if ((uVar3 & 0xffff) == 0) {
    uVar4 = 0;
  }
  else {
    uVar5 = fightFloorGetFightTrainerFightOutPokemonPtrAry(0,r3,aiStack_60,0,1);
    if ((uVar5 & 0xffff) == 0) {
      uVar4 = 0;
    }
    else {
      cVar13 = fightTrainerGetStatus(0,uVar12,0x30,0);
      if (cVar13 == 1) {
        uVar4 = fightOutPokemonGetPokemonPtr(r4);
        fn_8023A118(0xec6b,0xec2c,0xec2d,r3,uVar4,0,0,0,0,0,0);
        uVar6 = fn_800E0C54();
        iVar7 = (int)(uVar6 & 0xffff) % (int)(uVar5 & 0xffff);
      }
      else {
        local_64 = 0;
        local_68[0] = 0xffff;
        iVar7 = fn_8023B498(r3,r4,uVar3,asStack_2c,uVar5,aiStack_60,&local_64,local_68);
      }
      if (iVar7 < 0) {
        iVar11 = local_64;
        if ((local_64 != 0) && (local_68[0] >= 0)) {
          uVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(local_64,r5);
          uVar3 = fn_80238270(r3,r4,local_68[0] & 0xff);
          if ((uVar3 & 0xffff) != 0) {
            fightOutPokemonCreateFightActionAttackWaza(r4,0,0x13,0,lbl_80375CA8,uVar3 & 0xffff,uVar4,(s8)local_68[0],0);
            uVar4 = fightOutPokemonGetPokemonPtr(iVar11);
            uVar8 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,iVar11);
            uVar9 = fightOutPokemonGetPokemonPtr(r4);
            uVar10 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,r4);
            fn_8023A118(0xec6c,0xec2c,0xec2d,uVar10,uVar9,uVar8,uVar4,0,0,0x228,0);
            uVar4 = fightOutPokemonGetPokemonPtr(r4);
            uVar8 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0,r4);
            fn_8023A118(0xec64,0xec2c,0xec32,uVar8,uVar4,0,0,uVar3,0,0x228,0);
            return 1;
          }
          return 0;
        }
        uVar4 = 0;
      }
      else {
        selectedTarget = aiStack_60[iVar7];
        if (selectedTarget == 0) {
          uVar4 = 0;
        }
        else {
          cVar13 = fightTrainerGetStatus(0,uVar12,0x31,0);
          if (cVar13 == 1) {
            uVar4 = fightOutPokemonGetPokemonPtr(r4);
            fn_8023A118(0xec6b,0xec2c,0xec32,r3,uVar4,0,0,0,0,0,0);
            uVar5 = fn_800E0C54();
            iVar11 = (int)(uVar5 & 0xffff) % (int)(uVar3 & 0xffff);
          }
          else {
            iVar11 = fn_8023A740(r3,r4,uVar3,asStack_2c,asStack_40,selectedTarget,r5);
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
                overrideTarget = fn_8022B2CC(r4,sVar2,r5,fn_8023C368,1,0, (void*)0xffffffff);
                if (overrideTarget != 0) {
                  selectedTarget = overrideTarget;
                }
                if (selectedTarget == 0) {
                  uVar4 = 0;
                }
                else {
                  uVar4 = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(selectedTarget,r5);
                  fightOutPokemonCreateFightActionAttackWaza(r4,0,0x13,0,lbl_80375CA8,sVar2,uVar4,(s8)sVar1,0);
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
#pragma opt_common_subs reset
#pragma optimize_for_size on
#pragma opt_lifetimes off
#define AI_SCORE(code)                                                        \
    do {                                                                      \
        score[i] = fn_80239984(score[i], trainer, (code));                    \
        fn_80239EE8(0xEC64, trainerPtr,                                       \
                    fightOutPokemonGetPokemonPtr(pokemon), 0, 0, move, 0,    \
                    (code));                                                  \
    } while (0)

#define AI_SCORE_VALUE(code, value)                                           \
    do {                                                                      \
        u8 scoreValue = (u8)(value);                                          \
        score[i] = fn_802398E4(score[i], scoreValue, trainer, (code));        \
        fn_80239A40(0xEC64, trainer,                                          \
                    fightOutPokemonGetPokemonPtr(pokemon), 0, 0, move, 0,    \
                    (code), scoreValue);                                      \
    } while (0)

s32 fn_8023A740(u32 trainer, u32 pokemon, u32 moveCount,
                 register u16* moveList,
                 s16* volatile moveData, u32 target, u32 unused) {
    typedef s32 (*AiMoveFunc)(u32, u32, u32, u32);
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u16 fightFloorGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fn_80236C80();
    extern u16 fn_802376EC();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u32 wazaGetStatus();
    extern s32 fightTrainerAiWazaDamageNull();
    extern s32 fightTrainerAiWazaValueNull();
    extern s32 fightTrainerAiWazaValueToriaezutukae();
    extern s32 fn_802395C8();
    extern u32 fn_80239244();
    extern u8 fn_802393A0();
    extern u8 fn_80239498();
    extern s32 fn_80239500();
    extern u8 fn_8023C530();
    extern u8 fn_8023753C();
    extern u8 fn_802392A8();
    extern u8 fn_80238060();
    extern u8 fn_80239154();
    extern u8 fn_8000815C();
    extern u16 fightFloorGetValidFightOutPokemonCount();
    extern s32 fn_802398E4();
    extern s32 fn_80239984();
    extern void fn_80239A40();
    extern void fn_80239EE8();
    extern void fn_8023A118();

    s32 score[10];
    s32 damage[10];
    s32 difference[10];
    s32 moveMetric[10];
    u32 scratch[8];
    u16 trainerValue16;
    u32 side;
    u16 reserve16;
    u16 reserve17;
    u16 reserve18;
    u16 trainerData;
    u8 randomBase;
    u32 trainerValue;
    u16 baseDamage;
    struct {
        s32 value;
    } maxDamage;
    s32 maxDifference;
    s32 maxMetric;
    u16 initIndex;
    u16 i;
    u16 j;
    u16 k;
    u32 trainerPtr;
    u16 move;
    s32 data;
    u32 moveType;
    u32 moveKind;
    AiMoveFunc damageFunc;
    AiMoveFunc valueFunc;
    AiMoveFunc checkFunc;
    s32 damageValue;
    s32 scoreValueTemp;
    s32 randomRange;
    s32 delta;
    s32 selected;

    (void)unused;
    side = fightTargetGetPtrAsNowFightType(3, trainer);
    reserve16 = (u16)fightFloorGetStatus(0, 0, 0x16, 0);
    reserve17 = (u16)fightFloorGetStatus(0, 0, 0x17, 0);
    reserve18 = (u16)fightFloorGetStatus(0, 0, 0x18, 0);
    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    randomBase = (u8)fightTrainerGetStatus(0, trainerData, 0x38, 0);
    trainerPtr = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, pokemon);
    trainerValue = fn_80236C80(trainer, pokemon);
    fightSideGetHikaeFightPokemonNum(side, reserve16, reserve17, reserve18);

    for (initIndex = 0; initIndex < 10; initIndex++) {
        score[initIndex] = 0;
        damage[initIndex] = 0;
        difference[initIndex] = 0;
        moveMetric[initIndex] = 0;
    }

    baseDamage = fn_802376EC(trainer, target);
    maxDamage.value = -0xFFFF;
    maxDifference = -0xFFFF;
    maxMetric = 0;

    for (i = 0; i < (u16)moveCount; i++) {
        move = moveList[i];
        if (move == 0 || move == 0x165) {
            continue;
        }

        if ((u8)fightTrainerGetStatus(
                0,
                (u16)fightTrainerGetStatus(
                    0,
                    (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0),
                    2, 0),
                0x2D, 0) != 1) {
            goto damage_zero;
        }
        damageFunc = (AiMoveFunc)wazaGetStatus(0, move, 0x1E, 0);
        if (damageFunc != 0) {
            goto damage_call;
        }
        damageFunc = fightTrainerAiWazaDamageNull;
        goto damage_call;
damage_zero:
        damageValue = 0;
        goto damage_done;
damage_call:
        fightFloorGetFightTrainerFightOutPokemonPtrAry(
            0, trainer, scratch, 0, 1);
        wazaGetStatus(0, move, 5, 0);
        damageValue = damageFunc(trainer, pokemon, move, target);
damage_done:
        damage[i] = damageValue;

        difference[i] = (s32)baseDamage - damage[i];
        if (fn_8023943C(trainer, move, 1) == 1) {
            if (maxDamage.value < damage[i]) {
                maxDamage.value = damage[i];
            }
            if (difference[i] <= 0) {
                if (maxDifference < difference[i]) {
                    maxDifference = difference[i];
                }
                moveMetric[i] = fn_802393A0(trainer, move);
                if (maxMetric < moveMetric[i]) {
                    maxMetric = moveMetric[i];
                }
            }
        }
    }

    randomRange = randomBase * 2 + 1;
    trainerValue16 = (u8)trainerValue;

    for (i = 0; i < (u16)moveCount; i++) {
        move = moveList[i];
        if (move == 0) {
            continue;
        }
        data = moveData[i];
        if (data < 0) {
            continue;
        }

        moveType = fn_802395C8(trainer, move, pokemon);
        moveKind = fn_80239244(trainer, move);
        checkFunc = (AiMoveFunc)wazaGetStatus(0, move, 0x1C, 0);
        if ((u8)fightTrainerGetStatus(
                0,
                (u16)fightTrainerGetStatus(
                    0,
                    (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0),
                    2, 0),
                0x32, 0) != 1) {
            goto value_zero;
        }
        valueFunc = (AiMoveFunc)wazaGetStatus(0, move, 0x1C, 0);
        if (valueFunc != 0) {
            goto value_call;
        }
        valueFunc = fightTrainerAiWazaValueNull;
        goto value_call;
value_zero:
        scoreValueTemp = 0;
        goto value_done;
value_call:
        scoreValueTemp = valueFunc(trainer, pokemon, move, target);
value_done:
        score[i] = scoreValueTemp;

        fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                    trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0, 0,
                    move, 0, 0x227, score[i]);

        if (fn_8023C530(trainer, pokemon, move, target) == 1) {
            if (checkFunc == fightTrainerAiWazaValueToriaezutukae) {
                AI_SCORE(0x3E);
            } else {
                AI_SCORE(0x3C);
            }
        } else {
            AI_SCORE(0x3D);
        }

        if ((u16)moveType != 9 && fn_8023943C(trainer, move, 1) == 1) {
            if ((u16)fn_8023793C(trainer, target, moveType,
                                  fn_80239500(trainer, move)) == 0x42) {
                AI_SCORE(0x3F);
            }
            if ((u16)fn_8023793C(trainer, target, moveType,
                                  fn_80239500(trainer, move)) == 0x43) {
                AI_SCORE(0x40);
            }
        }

        for (j = 0; j < 3; j++) {
            u8 effect = fn_80239498(trainer, move, (u8)j);
            if (effect != 0) {
                u8 value = (u8)fightTrainerGetStatus(
                    0, trainerValue16, 0xC, effect);
                AI_SCORE_VALUE(0x41, value);
            }
        }

        if ((u16)moveType != 9) {
            for (j = 0; j < 2; j++) {
                u16 type = (u16)fightTrainerGetStatus(
                    0, trainerData, 0x39, j);
                if (type != 9 && (u16)moveType == type) {
                    u8 value = (u8)fightTrainerGetStatus(
                        0, trainerData, 0x3A, j);
                    AI_SCORE_VALUE(0x42, value);
                }
            }
        }

        for (k = 0; k < 3; k++) {
            u8 effect = fn_80239498(trainer, move, (u8)k);
            if (effect != 0) {
                for (j = 0; j < 2; j++) {
                    u8 type = (u8)fightTrainerGetStatus(
                        0, trainerData, 0x3B, j);
                    if (type != 0 && effect == type) {
                        u8 value = (u8)fightTrainerGetStatus(
                            0, trainerData, 0x3C, j);
                        AI_SCORE_VALUE(0x43, value);
                    }
                }
            }
        }

        if (fn_8023943C(trainer, move, 1) == 1) {
            if (maxDamage.value <= damage[i]) {
                AI_SCORE(0x44);
            }
            delta = difference[i];
            if (delta < 0) {
                AI_SCORE(0x45);
            }
            if (delta <= 0) {
                AI_SCORE(0x46);
            }
            if (delta <= 0 && maxMetric <= moveMetric[i]) {
                AI_SCORE(0x47);
            }
        }

        if (fn_8023943C(trainer, move, 2) == 1 &&
            fn_8023753C(trainer, pokemon) == 1) {
            AI_SCORE(0x48);
        }
        if (fn_802392A8(trainer, move) == 1) {
            AI_SCORE(0x49);
        }
        if ((u8)moveKind == 4 &&
            fightFloorGetValidFightOutPokemonCount(0, 1, target, 1) >= 2) {
            AI_SCORE(0x4A);
        }
        if (fn_80238060(trainer, pokemon, (u8)data) == 1) {
            AI_SCORE(0x4B);
        }
        if (fn_80239154(trainer, move) == 1) {
            AI_SCORE(0x4C);
        }

        if (fn_8000815C() == 1) {
            delta = (u16)fn_800E0C54() % randomRange - randomBase;
            score[i] = fightTrainerAiAddValue(score[i], delta);
            fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                        trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0,
                        0, move, 0, 0x225, delta);
        }

        fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                    trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0, 0,
                    move, 0, 0x226, score[i]);
    }

    selected = fightTrainerAiGetValueAryMaxBanme(score, moveCount, 1);
    if (selected < 0) {
        return -1;
    }
    fn_8023A118(0xEC64, 0xEC2C, 0xEC32,
                trainerPtr, fightOutPokemonGetPokemonPtr(pokemon), 0, 0,
                moveList[selected], 0, 0x228, score[selected]);
    return selected;
}

#undef AI_SCORE_VALUE
#undef AI_SCORE
#pragma opt_lifetimes reset
#pragma optimize_for_size reset
u32 fn_8023C370(u32 trainer, u32 pokemon, u32 move, u32 target, u32 dispatch)
{
    typedef u32 (*DamageFunc)(u32, u32, u32, u32);
    extern u32 fightTrainerGetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u32 fightTrainerAiWazaDamageNull();
    u32 party[8];
    u32 trainerData;
    DamageFunc damageFunc;
    u32 partyCount;
    u8 category;
    u16 count;
    u16 i;
    u32 result;

    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    if ((u8)fightTrainerGetStatus(0, trainerData, 0x2d, 0) == 1) {
        damageFunc = (DamageFunc)wazaGetStatus(0, move, 0x1e, 0);
        if (damageFunc == 0) {
            damageFunc = fightTrainerAiWazaDamageNull;
        }
    } else {
        return 0;
    }

    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(
        0, trainer, party, 0, 1);
    category = (u8)wazaGetStatus(0, move, 5, 0);
    result = 0;

    if ((u8)dispatch == 1) {
        switch (category) {
        case 0:
        case 3:
            result = damageFunc(trainer, pokemon, move, target);
            break;

        case 2:
        case 4:
        case 6:
            count = (u16)partyCount;
            for (i = 0; i < count; i++) {
                result += damageFunc(trainer, pokemon, move, target);
            }
            break;

        case 1:
        case 5:
        case 7:
            result = damageFunc(trainer, pokemon, move, target);
            break;
        }
    } else {
        result = damageFunc(trainer, pokemon, move, target);
    }

    return result;
}
u32 fn_8023C530(s32 trainer, s32 pokemon, s32 move, s32 target) {
    u8 valid;
    typedef u32 (*HitFunc)(u32, u32, u32, u32);
    extern u32 fightTrainerGetStatus();
    extern u32 wazaGetStatus();
    extern u32 fightFloorGetFightTrainerFightOutPokemonPtrAry();
    extern u8 fightOutPokemonCheckFightOut();
    extern u8 fn_80237F74();
    extern u8 fn_802026E4();
    extern s32 fn_80239500();
    extern u32 fn_802395C8();
    extern u32 fightTrainerAiWazaHitNull();
    u32 party[8];
    u32 trainerData;
    u32 partyCount;
    HitFunc hitFunc;
    u8 category;
    u32 result;

    trainerData = (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0);
    trainerData = (u16)fightTrainerGetStatus(0, trainerData, 2, 0);
    if ((u8)fightTrainerGetStatus(0, trainerData, 0x34, 0) == 1) {
        hitFunc = (HitFunc)wazaGetStatus(0, move, 0x1D, 0);
        if (hitFunc == 0) {
            hitFunc = fightTrainerAiWazaHitNull;
        }
    } else {
        return 1;
    }
    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(
        0, trainer, party, 0, 1);
    category = (u8)wazaGetStatus(0, move, 5, 0);
    result = 0;
    switch (category) {
    case 0:
    case 3: {
        u8 valid;
        s32 moveValue;
        u32 moveType;

        moveValue = fn_80239500(trainer, move);
        moveType = fn_802395C8(trainer, move, pokemon);
        valid = 1;
        if (fightOutPokemonCheckFightOut(target) == 0) {
            valid = 0;
        } else if ((u16)move != 0 && (u16)move != 0x165) {
            if (fn_80237F74(trainer, target, 0xA) == 1 &&
                (u16)moveType == 0xD && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0xB) == 1 &&
                (u16)moveType == 0xB && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x12) == 1 &&
                (u16)moveType == 0xA && fn_802026E4(target, 7) == 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x2B) == 1 &&
                (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
                valid = 0;
            }
        }
        if (valid == 0) {
            return 0;
        }
        return hitFunc(trainer, pokemon, move, target);
    }

    case 2:
    case 4:
    case 6: {
        u16 i;
        u16 count;
        u8 valid;
        s32 moveValue;
        u32 moveType;

        count = (u16)partyCount;
        for (i = 0; i < count; i++) {
            moveValue = fn_80239500(trainer, move);
            moveType = fn_802395C8(trainer, move, pokemon);
            valid = 1;
            if (fightOutPokemonCheckFightOut(target) == 0) {
                valid = 0;
            } else if ((u16)move != 0 && (u16)move != 0x165) {
                if (fn_80237F74(trainer, target, 0xA) == 1 &&
                    (u16)moveType == 0xD && (s16)moveValue != 0) {
                    valid = 0;
                }
                if (fn_80237F74(trainer, target, 0xB) == 1 &&
                    (u16)moveType == 0xB && (s16)moveValue != 0) {
                    valid = 0;
                }
                if (fn_80237F74(trainer, target, 0x12) == 1 &&
                    (u16)moveType == 0xA &&
                    fn_802026E4(target, 7) == 0) {
                    valid = 0;
                }
                if (fn_80237F74(trainer, target, 0x2B) == 1 &&
                    (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
                    valid = 0;
                }
            }
            if (valid == 0) {
                result = 0;
            } else {
                result = hitFunc(trainer, pokemon, move, party[i]);
            }
            if ((u8)result == 1) {
                return result;
            }
        }
        return result;
    }

    case 1:
    case 5:
    case 7: {
        s32 moveValue;
        u32 moveType;

        moveValue = fn_80239500(trainer, move);
        moveType = fn_802395C8(trainer, move, pokemon);
        valid = 1;
        if (fightOutPokemonCheckFightOut(target) == 0) {
            valid = 0;
        } else if ((u16)move != 0 && (u16)move != 0x165) {
            if (fn_80237F74(trainer, target, 0xA) == 1 &&
                (u16)moveType == 0xD && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0xB) == 1 &&
                (u16)moveType == 0xB && (s16)moveValue != 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x12) == 1 &&
                (u16)moveType == 0xA && fn_802026E4(target, 7) == 0) {
                valid = 0;
            }
            if (fn_80237F74(trainer, target, 0x2B) == 1 &&
                (u8)wazaGetStatus(0, move, 0x17, 0) == 1) {
                valid = 0;
            }
        }
        if (valid == 0) {
            result = 0;
            break;
        }
        result = hitFunc(trainer, pokemon, move, target);
        break;
    }
    }
    return result;
}
#pragma optimize_for_size on
#define fn_801F1990 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_802399FC fightTrainerAiAddValue
u32 fightTrainerAiWazaValueYumekui(u32 r3, u32 r4, u32 r5, u32 r6)

{
    extern u8 fn_801F1990();
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_802376EC();
    extern int fn_80239984();
    extern int fn_802399FC();
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
    extern int fn_8023C370();
  u32 uVar1;
  u32 uVar2;
  int iVar3;
  u32 uVar5;
  u8 cVar6;

  uVar1 = fn_802376EC();
  uVar2 = fn_8023C370(r3,r4,r5,r6,1);
  iVar3 = fn_801FB1C0(0,0x223,0x3e,0);
  uVar2 = ((int)(((int)uVar2 / 2) * 100) /
          (int)(uVar1 & 0xffff)) / iVar3;
  {
    u32 callResult = fn_802399FC(0,uVar2);
    uVar1 = callResult;
  }
  uVar5 = fn_80205B8C(r4);
  fn_80239CCC(0xec64,r3,uVar5,0,0,r5,0,0x223,uVar2);
  cVar6 = fn_801F1990(0,r3,1,1,0x10e,r4);
  if (cVar6 == 1) {
    {
      u32 callResult = fn_80239984(uVar1,r3,0x224);
      uVar1 = callResult;
    }
    uVar5 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar5,0,0,r5,0,0x224);
  }
  return uVar1;
}
#undef fn_802399FC
#undef fn_80205B8C
#undef fn_801FB1C0
#undef fn_801F1990
#pragma optimize_for_size reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#define fn_801F1990 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut
u32 fightTrainerAiWazaValueKawarawari(u32 r3, u32 r4, u32 r5, u32 r6)

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
#undef fn_801F025C
#undef fn_801F6E98
#undef fn_80205B8C
#undef fn_801F1990
#pragma optimize_for_size on
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueTeiryouDameeji(u32 arg0, u32 arg1, u32 arg2, u32 arg3)

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
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma optimization_level 2
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueHikarinokabe(u32 r3, u32 r4, u32 r5, u32 r6)
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
u32 fightTrainerAiWazaValueRihurekutaa(u32 r3, u32 r4, u32 r5, u32 r6)
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
  if (uVar3 > uVar2) {
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
#undef fn_80205B8C
#pragma optimization_level 4
#pragma optimize_for_size on
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueOnibi(void* ctx, u32 param1, u32 param2, u32 param3)
{
    extern u16 fn_80236FFC(void*, u32);
    extern u16 fn_8023715C(void*, u32);
    extern u16 fn_80236520(void*, u32);
    extern u16 fn_801F1C18(u32, void*, void*, u32, u32);
    extern u16 fn_802377E8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[8];
    u32 handle = 0;
    u16 a = fn_80236FFC(ctx, param3);
    u16 b = fn_8023715C(ctx, param3);
    u16 c = fn_80236520(ctx, param3);
    u16 count;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 0, 1);
    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, arr[i]);
        if (v == 0xca || v == 0x168 || v == 0x12f || v == 0xd5) {
            handle = fn_80239984(0, ctx, 0x217);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x217);
            break;
        }
    }
    if (b > a) {
        handle = fn_80239984(handle, ctx, 0x218);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x218);
    }
    if (c == 0x11f) {
        handle = fn_80239984(handle, ctx, 0x219);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x219);
    }
    return handle;
}
#undef fn_801F1C18
#undef fn_80205B8C
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueDoku(void* ctx, u32 param1, u32 param2, u32 param3)
{
    extern u16 fn_801F1C18(u32, void*, void*, u32, u32);
    extern u16 fn_80236520(void*, u32);
    extern u16 fn_802377E8(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[10];
    u32 handle = 0;
    u16 count;
    u16 c;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 0, 1);
    c = fn_80236520(ctx, param3);
    for (i = 0; i < count; i++) {
        u16 v = fn_802377E8(ctx, arr[i]);
        if (v == 0xca || v == 0x168 || v == 0x12f || v == 0xd5) {
            handle = fn_80239984(0, ctx, 0x215);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x215);
            break;
        }
    }
    if (c == 0x11f) {
        handle = fn_80239984(handle, ctx, 0x216);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x216);
    }
    return handle;
}
#undef fn_801F1C18
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueBatontatti(u32 r3, u32 r4, u32 r5)

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
#undef fn_80205B8C
#pragma optimize_for_size reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueMarukunaru(u32 r3, u32 r4, u32 r5)

{
    extern int fn_801FB1C0();
    extern u32 fn_80205B8C();
    extern u32 fn_80235714();
    extern u32 fn_80235A3C();
    extern u32 fn_802367CC();
    extern int fn_80239984();
    extern s32 fightTrainerGetStatus(u32, u32, u32, u32);
    extern u32 fightTrainerAiAddValue(u32, s32);
    extern u32 fn_80239CCC();
    extern u32 fn_80239EE8();
  u32 uVar4;
  int uVar1;
  u8 cVar5;
  u32 uVar2;
  int iVar3;
  u16 uVar6;
  struct {
    u32 value;
  } uVar7;
  u16 local_30 [12];

  uVar4 = fn_802367CC(r3,r4,local_30,0,1);
  uVar1 = fn_80235A3C(r3,r4);
  uVar7.value = 0;
  cVar5 = fn_80235714(r3,r4);
  if (cVar5 == 0) {
    uVar7.value = fn_80239984(0,r3,0x20c);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20c);
  }
  cVar5 = fn_80235714(r3,r4);
  if (cVar5 == 1) {
    uVar7.value = fn_80239984(uVar7.value,r3,0x20e);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20e);
  }
  uVar1 = (uVar1 & 0xff) - 6;
  if (uVar1 < 0) {
    uVar1 = 0;
  }
  iVar3 = fightTrainerGetStatus(0,0x20f,0x3e,0);
  uVar1 = uVar1 * iVar3;
  uVar7.value = fightTrainerAiAddValue(uVar7.value,uVar1);
  uVar2 = fn_80205B8C(r4);
  fn_80239CCC(0xec64,r3,uVar2,0,0,r5,0,0x20f,uVar1);
  for (uVar6 = 0; uVar6 < (u16)uVar4; uVar6++) {
    if ((local_30[uVar6] == 0xcd) || (local_30[uVar6] == 0x12d)) {
      {
        u32 callResult = fn_80239984(uVar7.value,r3,0x20d);
        uVar7.value = callResult;
      }
      uVar2 = fn_80205B8C(r4);
      fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x20d);
      break;
    }
  }
  return uVar7.value;
}
u32 fightTrainerAiWazaValueSukirusuwappu(u32 arg0, u32 arg1, u32 arg2, u32 arg3)

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
#undef fn_80205B8C
extern u32* lbl_80478DF8;
extern int wazaGetStatus();
extern int pokemonGetStatus();
extern u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut();
extern s32 fightTrainerAiWazaValueJisin();
extern s32 fightTrainerAiWazaValueJibaku();
extern s32 fightTrainerAiWazaValueNull();

u32 fightTrainerAiWazaValueMamoru(u32 ctx, u32 poke, u32 msgArg,
                                  u32 otherPoke)
{
    typedef s32 (*WazaValueFunc)();
    u32 party[8];
    u16 moveBuf[10];
    u32 moveCount;
    u32 partyCount;
    u32 finalMoveCount;
    u32 score;
    u16 i;
    u16 j;
    u32 waza;
    WazaValueFunc valueFunc;
    u8 allyJisin;
    u8 allyJibaku;
    u8 queuedJisin;
    u8 queuedJibaku;

    score = 0;
    allyJisin = 0;
    allyJibaku = 0;
    queuedJisin = 0;
    queuedJibaku = 0;

    partyCount = fightFloorGetFightTrainerFightOutPokemonPtrAry(
        0, ctx, party, 1, 1);
    waza = fn_80236520((void*)ctx, poke);
    fn_80236520((void*)ctx, otherPoke);

    for (i = 0; i < (u16)partyCount; i++) {
        if (party[i] == poke) {
            continue;
        }
        moveCount = (u16)fn_802367CC((void*)ctx, party[i], moveBuf, 0, 1);
        if (moveCount == 0) {
            continue;
        }
        for (j = 0; j < moveCount; j++) {
            valueFunc = (WazaValueFunc)wazaGetStatus(0, moveBuf[j], 0x1C, 0);
            if (valueFunc == 0) {
                valueFunc = fightTrainerAiWazaValueNull;
            }
            if (valueFunc == fightTrainerAiWazaValueJisin) {
                allyJisin = 1;
            }
            if (valueFunc == fightTrainerAiWazaValueJibaku) {
                allyJibaku = 1;
            }
        }
    }

    for (moveCount = 0; (moveCount & 0xFFFF) < *lbl_80478DF8; moveCount++) {
        valueFunc = (WazaValueFunc)wazaGetStatus(0, moveCount, 0x1C, 0);
        if (valueFunc == 0) {
            valueFunc = fightTrainerAiWazaValueNull;
        }
        if (valueFunc == fightTrainerAiWazaValueJisin ||
            valueFunc == fightTrainerAiWazaValueJibaku) {
            if ((u8)fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(
                    0, ctx, 1, 1, moveCount, 0) == 1) {
                if (valueFunc == fightTrainerAiWazaValueJisin) {
                    queuedJisin = 1;
                }
                if (valueFunc == fightTrainerAiWazaValueJibaku) {
                    queuedJibaku = 1;
                }
            }
        }
    }

    if (allyJisin == 1 || allyJibaku == 1) {
        score = fn_80239984(score, (void*)ctx, 0x1D7);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1D7);
    }
    if (queuedJisin == 1 || queuedJibaku == 1) {
        score = fn_80239984(score, (void*)ctx, 0x1D8);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1D8);
    }

    if ((u8)fn_80237F74((void*)ctx, poke, 3) == 1 &&
        fn_8023943C(ctx, waza, 4) == 1) {
        score = fn_80239984(score, (void*)ctx, 0x1D9);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1D9);
    }

    if (((u16)waza == 0xB6 || (u16)waza == 0xC5) &&
        pokemonGetStatus(poke, 0, 0xFC, 0) != 0) {
        score = fn_80239984(score, (void*)ctx, 0x1DA);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0, 0,
                    msgArg, 0, 0x1DA);
    }

    if ((u8)fn_80237DBC(ctx, (void*)poke, 7) == 1) {
        if (allyJibaku == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DB);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DB);
        }
        if (queuedJibaku == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DC);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DC);
        }
    }

    if ((u8)fn_80237DBC(ctx, (void*)poke, 2) == 1 ||
        (u8)fn_80237F74((void*)ctx, poke, 0x1A) == 1 ||
        (u8)fn_80237F74((void*)ctx, poke, 0x19) == 1) {
        if (allyJisin == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DD);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DD);
        }
        if (queuedJisin == 1) {
            score = fn_80239984(score, (void*)ctx, 0x1DE);
            fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke), 0,
                        0, msgArg, 0, 0x1DE);
        }
    }

    for (i = 0; i < (u16)partyCount; i++) {
        if (party[i] == poke) {
            continue;
        }
        finalMoveCount = fn_802367CC((void*)ctx, party[i], moveBuf, 0, 1);
        if ((u16)finalMoveCount == 0) {
            continue;
        }
        for (j = 0; j < (u16)finalMoveCount; j++) {
            if (moveBuf[j] == 0x10A) {
                score = fn_80239984(score, (void*)ctx, 0x1DF);
                fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(poke),
                            0, 0, msgArg, 0, 0x1DF);
                break;
            }
        }
        if ((u16)j < (u16)finalMoveCount) {
            break;
        }
    }
    return score;
}
#pragma optimize_for_size on
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueTakuwaeru(void* ctx, u32 param1, u32 param2)
{
    extern s16 fn_80202360(u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    s16 val;
    u32 handle = 0;

    if (fn_80236BFC(ctx, param1, 0x2d) == 1) {
        val = fn_80202360(param1, 0x2d);
    } else {
        val = 0;
    }
    if (val == 0) {
        handle = fn_80239984(0, ctx, 0x1d4);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d4);
    } else if (val == 1) {
        handle = fn_80239984(0, ctx, 0x1d5);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d5);
    } else if (val == 2) {
        handle = fn_80239984(0, ctx, 0x1d6);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d6);
    }
    return handle;
}
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueJyuuden(void* ctx, u32 param1, u32 param2)
{
    extern u16 fn_802367CC(void*, u32, void*, u32, u32);
    extern u16 fn_802395C8(void*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern u32 fn_80205B8C(u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 buf[10];
    u32 handle = 0;
    u16 count;
    u16 i;

    count = fn_802367CC(ctx, param1, buf, 0, 1);
    for (i = 0; i < count; i++) {
        if (buf[i] == 0x10c) {
            continue;
        }
        if (fn_802395C8(ctx, buf[i], param1) == 0xd) {
            handle = fn_80239984(0, ctx, 0x1d2);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d2);
            break;
        }
    }
    if (fn_80236BFC(ctx, param1, 0x24) == 1) {
        handle = fn_80239984(handle, ctx, 0x1d3);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1d3);
    }
    return handle;
}
#undef fn_80205B8C
#pragma optimize_for_size reset
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueNemuriKoudou(u32 r3, u32 r4, u32 r5)

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
#undef fn_80205B8C
#define fn_80205B8C fightOutPokemonGetPokemonPtr
#pragma optimize_for_size on
u32 fightTrainerAiWazaValueMigawari(u32 r3, u32 r4, u32 r5)

{
    extern u32 fn_80205B8C();
    extern u32 fn_802373B0();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
    extern const f32 lbl_8047E634;

  u8 cVar2;
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
#pragma optimize_for_size reset
#undef fn_80205B8C
#pragma opt_lifetimes off
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueSawagu(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fn_801F1C18();
    extern int fn_80202108();
    extern s8 fn_80202234();
    extern u32 fn_80205B8C();
    extern u32 fn_802367CC();
    extern u8 fn_80236BFC();
    extern u8 fn_80237F74();
    extern int fn_80239984();
    extern u32 fn_80239EE8();
  u32 uVar3;
  u32 uVar4;
  s8 cVar5;
  u8 cVar11;
  u32 uVar1;
  s8 cVar6;
  u32 uVar2;
  int iVar8;
  int iVar7;
  u32 uVar10;
  u32 uVar9;
  u32 uVar8;
  u32 local_54 [8];
  u16 local_68 [10];

  uVar9 = 0;
  uVar3 = fn_801F1C18(0,r3,local_54,1,1);
  for (uVar10 = 0; (u16)uVar10 < (u16)uVar3; uVar10 = uVar10 + 1) {
    iVar7 = local_54[(u16)uVar10];
    if (r4 != iVar7) {
      cVar11 = fn_80236BFC(r3,iVar7,8);
      if (cVar11 == 0) {
        cVar5 = -1;
      }
      else {
        uVar1 = fn_80237F74(r3,iVar7,0x30);
        uVar8 = ((u32)__cntlzw(1 - (uVar1 & 0xff)) >> 5) + 1;
        iVar8 = fn_80202108(iVar7,8);
        uVar8 = iVar8 + uVar8;
        if ((s8)uVar8 >= (cVar6 = fn_80202234(iVar7,8))) {
          cVar5 = 1;
        }
        else {
          cVar5 = 0;
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
  for (uVar10 = 0; (u16)uVar10 < (u16)uVar3; uVar10 = uVar10 + 1) {
    if (r4 != local_54[(u16)uVar10]) {
      uVar4 = fn_802367CC(r3,local_54[(u16)uVar10],local_68,0,1);
      if ((u16)uVar4 != 0) {
        for (uVar8 = 0; (u16)uVar8 < (u16)uVar4; uVar8 = uVar8 + 1) {
          if (local_68[(u16)uVar8] == 0x9c) {
            uVar9 = fn_80239984(uVar9,r3,0x1cd);
            uVar2 = fn_80205B8C(r4);
            fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1cd);
            break;
          }
        }
        if ((u16)uVar8 < (u16)uVar4) break;
      }
    }
  }
  cVar11 = fn_80236BFC(r3,r6,8);
  if (cVar11 == 0) {
    cVar5 = -1;
  }
  else {
    uVar1 = fn_80237F74(r3,r6,0x30);
    uVar8 = ((u32)__cntlzw(1 - (uVar1 & 0xff)) >> 5) + 1;
    iVar8 = fn_80202108(r6,8);
    uVar4 = iVar8;
    uVar4 += uVar8;
    if ((s8)uVar4 >= (cVar6 = fn_80202234(r6,8))) {
      cVar5 = 1;
    }
    else {
      cVar5 = 0;
    }
  }
  if (cVar5 == 0) {
    uVar9 = fn_80239984(uVar9,r3,0x1ce);
    uVar2 = fn_80205B8C(r4);
    fn_80239EE8(0xec64,r3,uVar2,0,0,r5,0,0x1ce);
  }
  return uVar9;
}
#undef fn_801F1C18
#undef fn_80205B8C
#pragma opt_lifetimes reset
#pragma optimize_for_size on
#define fn_8025CB3C fightTrainerAiCheckOumu
u32 fightTrainerAiWazaValueOomugaesi(void* ctx, u32 slot, u16 species, u32 extra) {
    extern u32 fn_8023CA9C();
    extern u32 fn_8025CB3C();
    u32 currentSpecies;
    u32 result;
    currentSpecies = fn_8025CB3C(ctx);
    if ((u16)currentSpecies == species) {
        goto ret_zero;
    }
    if ((u16)currentSpecies == 0) {
        goto ret_zero;
    }
    result = fn_8023CA9C(ctx, slot, currentSpecies, extra);
    goto done;
ret_zero:
    result = 0;
done:
    return result;
}
#undef fn_8025CB3C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801FB1C0 fightTrainerGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
s32 fightTrainerAiWazaValueRandamuSentaku(void* ctx, u32 param1, u32 param2) {
    extern s32 fn_801FB1C0(u32, u32, u32, u32);
    extern u32 fn_80205B8C();
    extern u32 fightTrainerAiAddValue(u32, s32);
    s32 handle;
    s32 scale;
    s32 mod;
    u16 rng;

    scale = fn_801FB1C0(0, 0x1cb, 0x3e, 0);
    rng = fn_800E0C54();
    mod = rng % (scale + 1);
    handle = fightTrainerAiAddValue(0, mod);
    fn_80239CCC(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1cb, mod);
    return handle;
}
#undef fn_801FB1C0
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801F1C18 fightFloorGetFightTrainerFightOutPokemonPtrAry
#define fn_801F54A4 fightFloorGetStatus
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueHurahuradansu(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_801F1C18(u32, void*, u32*, u32, u32);
    extern u16 fn_801F54A4(u32, u32, u32, u32);
    extern u32 fn_80205B8C(u32);
    extern u8 fn_80237F74(void*, u32, u32);
    extern u32 fn_8023831C(void*, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 arr[10];
    u32 handle = 0;
    u32 count;
    u16 i;

    count = fn_801F1C18(0, ctx, arr, 1, 1);
    if (fn_801F54A4(0, 0, 0x18, 0) >= 2) {
        handle = fn_80239984(0, ctx, 0x1c7);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c7);
    }
    for (i = 0; i < (u16)count; i++) {
        if (param1 != arr[i]) {
            u16 v = fn_8023831C(ctx, arr[i]);
            if (v == 8 || v == 9) {
                handle = fn_80239984(handle, ctx, 0x1c8);
                fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c8);
                break;
            }
        }
    }
    for (i = 0; i < (u16)count; i++) {
        if (arr[i] != param1 && fn_80237F74(ctx, arr[i], 0x14) == 1) {
            handle = fn_80239984(handle, ctx, 0x1c9);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c9);
            break;
        }
    }
    for (i = 0; i < (u16)count; i++) {
        if (arr[i] != param1) {
            handle = fn_80239984(handle, ctx, 0x1ca);
            fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1ca);
            break;
        }
    }
    return handle;
}
#undef fn_801F1C18
#undef fn_801F54A4
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma optimize_for_size on
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_801F6E98 fightSideIsJoutaiDataId
#define fn_801F6D9C fightSideGetCountAsJoutaiDataId
#define fn_80205B8C fightOutPokemonGetPokemonPtr
u32 fightTrainerAiWazaValueMakibisi(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern void* fn_801F025C();
    extern u32 fn_80205B8C();
    extern u8 fn_801F6E98(void*, u32);
    extern s16 fn_801F6D9C(void*, u32);
    u32 handle = 0;
    void* obj = fn_801F025C(2, param3);
    s16 val;

    if (fn_801F6E98(obj, 0x4a) == 1) {
        val = fn_801F6D9C(obj, 0x4a);
    } else {
        val = 0;
    }
    if (val == 0) {
        handle = fn_80239984(0, ctx, 0x1c4);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c4);
    } else if (val == 1) {
        handle = fn_80239984(0, ctx, 0x1c5);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c5);
    } else if (val == 2) {
        handle = fn_80239984(0, ctx, 0x1c6);
        fn_80239EE8(0xEC64, ctx, fn_80205B8C(param1), 0, 0, param2, 0, 0x1c6);
    }
    return handle;
}
#undef fn_801F025C
#undef fn_801F6E98
#undef fn_801F6D9C
#undef fn_80205B8C
#pragma optimize_for_size reset
#pragma dont_inline reset
