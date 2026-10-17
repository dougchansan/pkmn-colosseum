/**
 * @file fight_trainer_ai_waza_damage.c
 * @brief game/pxdvs/app/fight/fightTrainerAiWazaDamage.cpp -- split from colosseum_battle.c (the
 *        Colosseum battle-flow/AI bucket, 0x802405C0-0x80265EC4),
 *        address range 0x80250980-0x80253950, 212 fns.
 *
 * XD source unit: game/pxdvs/app/fight/fightTrainerAiWazaDamage.cpp
 * Physically split out of the pre/post-battle mega-file by address
 * (functions located and bucketed by name via config/GC6E01/symbols.txt,
 * since this TU uses plain named C bodies with no address-comment
 * markers).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

/* =========================================================================
 * Duplicated declarations (verbatim from the original colosseum_battle.c
 * preamble, present in every split segment so each TU keeps the same
 * external visibility it had before the split)
 * ========================================================================= */
extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();

/* Battle system functions */
extern void battleCameraSelect();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void std104_soundSetBackgroundMusic();     /* Fade out music */
extern void std105_soundPlayEffect();     /* Start BGM */

/* SDA2 float constants used by asm wrappers */
extern f32 lbl_8047E678;
extern f32 lbl_8047E67C;

/* SDA1 globals used by asm wrappers */
extern u32 lbl_8047B668;
extern u32 lbl_8047B66C;
extern u32 lbl_8047B670;

/* Data labels used by asm wrappers */
extern u8  lbl_8039A6B8[];
extern u8  lbl_8039A6A8[];
extern int lbl_804782BC[];
extern u8  lbl_804782E0[];
extern u8  lbl_804783E0[];

/* Forward declarations for functions used as addresses in asm wrappers */
void ShortCommandProc(int r3);
void ReadProc(int r3);
void WriteProc(int r3);
void __GBASyncCallback(int r3);
u32  __GBASync(int r3);
u32  __GBATransfer(int r3, u32 r4, u32 r5, u32 r6);

/* Forward declarations for asm wrapper bl targets (use () form for compat) */
extern void DSPInit();
extern void set__5GSvecFfff();
extern int  _fadeEffectGetRandom__FUl();
extern u32  pokemonBiosGetCatchTrainerRnd();
extern u32  pokemonBiosGetRnd();
extern u16  pokemonBiosGetPokemonDataId();
extern u32  savedataGetStatus();
extern int  fadeCheck();
extern int  fadeSet();
extern int  wazaSequenceSysRelease();
extern int  fn_801DADC0();
extern void OSRegisterResetFunction();
extern void OSInitAlarm();
extern void OSInitThreadQueue();
extern void* memcpy();

/* Forward declarations for converted functions */
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);

/* Address: 0x80250980 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage213(void) { return 0; }

/* Address: 0x80250988 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage212(void) { return 0; }

/* Address: 0x80250990 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage211(void) { return 0; }

/* Address: 0x80250998 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage210(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage209_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage209_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x802509A0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage209(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage209_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage209_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80250A24 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage208(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage207_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage207_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80250A2C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage207(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage207_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage207_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80250AB0 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage206(void) { return 0; }

/* Address: 0x80250AB8 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage205(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage204_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage204_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80250AC0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage204(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage204_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage204_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80250B44 | Size: 0x78 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage203(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u32 fightSeqGetNromalWazaDamage(void*, u32, u32, u32, u32, u32, void*, u32);
    extern u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
    extern void _fightTrainerAiWazaDamage203SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void*, u32, u32);
    u32 damage;
    u8 boost;

    boost = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    damage = fightSeqGetNromalWazaDamage(ctx, param, slot, extra, 0, 0, _fightTrainerAiWazaDamage203SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon, 0);
    if (boost != 0) {
        damage <<= 1;
    }
    return damage;
}

/* Address: 0x80250BBC | Size: 0xA8 */
u32 _fightTrainerAiWazaDamage203SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 param1, u32 param2) {
    extern u32 wazaSetStatus(u32, u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightTrainerAiGetNowTenkou(void*, u32, u32);
    u32 value;
    u32 status;
    u32 mapped;

    status = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    value = pokemonGetStatus(param2, 0, 0xd9, 0);
    status = status & 0xFF;
    if (status == 2) {
        mapped = 0xb;
    } else if (status == 3) {
        mapped = 5;
    } else if (status == 1) {
        mapped = 0xa;
    } else if (status == 4) {
        mapped = 0xf;
    } else {
        mapped = 0;
    }
    return wazaSetStatus(value, 0, 0x30, 0, mapped & 0xFFFF);
}

static inline u32 fightTrainerAiWazaDamage202_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage202_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80250C64 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage202(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage202_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage202_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80250CE8 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage201(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage200_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage200_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80250CF0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage200(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage200_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage200_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80250D74 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage199(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage198_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage198_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80250D7C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage198(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage198_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage198_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

static inline u32 fightTrainerAiWazaDamage197_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage197_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80250E00 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage197(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage197_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage197_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80250E84 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage196(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage196SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage196SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80250EC4 | Size: 0x90 */
void _fightTrainerAiWazaDamage196SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 param1, u32 param2) {
    extern void wazaSetStatus(u32, u32, u32, u32, u32);
    extern u32 fightSeqGetKetaguriIryoku(u32);
    extern u32 fightTrainerAiGetPokemonDataId(void*, u32);
    u32 sourceValue;
    u32 convertedValue;
    u32 scaledValue;

    sourceValue = (u32)pokemonGetStatus(param2, 0, 0xd9, 0);
    convertedValue = fightTrainerAiGetPokemonDataId(ctx, param2);
    scaledValue = fightSeqGetKetaguriIryoku((u32)pokemonGetStatus(0, convertedValue, 0x5f, 0) & 0xffff);
    wazaSetStatus(sourceValue, 0, 0x2f, 0, scaledValue & 0xffff);
}

/* Address: 0x80250F54 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage195(void) { return 0; }

/* Address: 0x80250F5C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage194(void) { return 0; }

/* Address: 0x80250F64 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage193(void) { return 0; }

/* Address: 0x80250F6C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage192(void) { return 0; }

/* Address: 0x80250F74 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage191(void) { return 0; }

/* Address: 0x80250F7C | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage190(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage190SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage190SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80250FBC | Size: 0xB4 */
void _fightTrainerAiWazaDamage190SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 param1, u32 param2) {
    extern void wazaSetStatus(u32, u32, u32, u32, u32);
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fn_80237664(void*, u32);
    extern u32 fn_802376EC(void*, u32);
    u32 value;
    u32 current;
    u32 maximum;
    u16 amount;

    value = pokemonGetStatus(param2, 0, 0xd9, 0);
    current = fn_802376EC(ctx, param2);
    maximum = fn_80237664(ctx, param2);
    amount = wazaGetStatus(value, 0, 0x2f, 0);
    amount = (s32)(amount * (current & 0xFFFF)) / (s32)(maximum & 0xFFFF);
    if (amount == 0) {
        amount = 1;
    }
    wazaSetStatus(value, 0, 0x2f, 0, amount);
}

/* Address: 0x80251070 | Size: 0x5C | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage189(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u32 fn_802376EC(void*, u32);
    s32 first;
    s32 second;

    first = fn_802376EC(ctx, slot) & 0xffff;
    second = fn_802376EC(ctx, arg3) & 0xffff;
    if (first >= second) {
        return 0;
    }
    return second - first;
}

static inline u32 fightTrainerAiWazaDamage188_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage188_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x802510CC | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage188(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage188_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage188_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80251150 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage187(void) { return 0; }

/* Address: 0x80251158 | Size: 0x3C | Pattern: simple_wrapper */
extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6, u32 p7);
u32 fightTrainerAiWazaDamage186(void* ctx, u32 param1, u32 param2, u32 param3) {
    return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0);
}

/* Address: 0x80251194 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage185(void* ctx, u32 param1, u32 param2, u32 param3) {
    return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0);
}

/* Address: 0x802511D0 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage184(void) { return 0; }

/* Address: 0x802511D8 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage183(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage182_helper(void* ctx, u32 arg1, u32 arg2, u32 arg3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    return fightSeqGetNromalWazaDamage(ctx, arg1, arg2, arg3, 0, 0, 0, 0);
}

/* Address: 0x802511E0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage182(void* ctx, u32 param1, u32 param2, u32 param3) {
    int new_var;
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage182_helper(ctx, param2, param1, param3);

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        new_var = ((u16)param2) == 0x39;
        if (new_var) {
            v1 = ((0, v1)) << 1;
            new_var = ((u16)param2) == 0x39;
        }
    }
    return v1;
}

/* Address: 0x80251264 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage181(void) { return 0; }

/* Address: 0x8025126C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage180(void) { return 0; }

/* Address: 0x80251274 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage179(void) { return 0; }

/* Address: 0x8025127C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage178(void) { return 0; }

/* Address: 0x80251284 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage177(void) { return 0; }

/* Address: 0x8025128C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage176(void) { return 0; }

/* Address: 0x80251294 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage175(void) { return 0; }

/* Address: 0x8025129C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage174(void) { return 0; }

/* Address: 0x802512A4 | Size: 0xAC */
u32 fightTrainerAiWazaDamage173(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 tikeiDataBiosGetWazaId(u32);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 fightTrainerAiGetWazaDamageValue(void*, u32, u32, u32, u32);
    u32 paramType;
    u32 other;
    u32 otherType;

    other = tikeiDataBiosGetWazaId(fightFloorGetStatus(0, 0, 0xf, 0) & 0xFFFF);
    paramType = wazaGetStatus(0, param, 9, 0) & 0xFFFF;
    param = other;
    otherType = wazaGetStatus(0, param, 9, 0) & 0xFFFF;
    if (otherType != paramType) {
        return fightTrainerAiGetWazaDamageValue(ctx, slot, param, extra, 1);
    }
    return 0;
}

/* Address: 0x80251350 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage172(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage171_helper1(u32 value) {
    return value << 1;
}

static inline u32 fightTrainerAiWazaDamage171_helper2(u32 value) {
    return value;
}

/* Address: 0x80251358 | Size: 0x78 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage171(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x5) == 1) {
        v1 <<= 1;
    }
    return v1;
}

static inline u32 fightTrainerAiWazaDamage170_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage170_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x802513D0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage170(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage170_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage170_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80251454 | Size: 0x70 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage169(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u32 fightSeqGetNromalWazaDamage(void*, u32, u32, u32, u32, u32, u32, u32);
    extern u8 fightAiIsKaragenki(void*, u32);
    u32 savedSlot;
    void* savedCtx;
    u32 new_var;
    u32 damage;

    savedSlot = slot;
    new_var = fightSeqGetNromalWazaDamage(ctx, param, savedSlot, extra, 0, 0, 0, 0);
    savedCtx = ctx;
    damage = new_var;
    if (fightAiIsKaragenki(savedCtx, savedSlot) == 1) {
        damage <<= 1;
    }
    return damage;
}

/* Address: 0x802514C4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage168(void) { return 0; }

/* Address: 0x802514CC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage167(void) { return 0; }

/* Address: 0x802514D4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage166(void) { return 0; }

/* Address: 0x802514DC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage165(void) { return 0; }

/* Address: 0x802514E4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage164(void) { return 0; }

/* Address: 0x802514EC | Size: 0x98 */
s32 fightTrainerAiWazaDamage162(void* ctx, u32 param1, u32 param2) {
    extern u8 joutaiDataBiosGetMaxStack(u32);
    extern s16 fightOutPokemonGetPokemonJoutaiStack(u32, u32);
    extern u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    s16 shift;
    u32 hp;

    shift = 1;
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x2d) == 1) {
        shift = fightOutPokemonGetPokemonJoutaiStack(param1, 0x2d);
    }
    shift = joutaiDataBiosGetMaxStack(0x2d) - shift;
    if (shift < 0) {
        shift = 0;
    }
    shift = 1 << shift;
    hp = fn_80237664(ctx, param1) & 0xFFFF;
    return -((s32)hp / shift);
}

/* Address: 0x80251584 | Size: 0x88 */
s32 fightTrainerAiWazaDamage161(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s16 fightOutPokemonGetPokemonJoutaiStack(u32, u32);
    extern s32 fightSeqGetNromalWazaDamage(void*, u32, u32, u32, u32, u32, u32, u32);
    extern u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    s16 multiplier;

    multiplier = 1;
    if (fightTrainerAiIsPokemonJoutai(ctx, param1, 0x2d) == 1) {
        multiplier = fightOutPokemonGetPokemonJoutaiStack(param1, 0x2d);
    }
    return multiplier * fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0);
}

/* Address: 0x8025160C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage160(void) { return 0; }

/* Address: 0x80251614 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage159(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80251650 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage158(void) { return 0; }

/* Address: 0x80251658 | Size: 0x28 | Ghidra import */
int fightTrainerAiWazaDamage157(void)

{
    extern u32 fn_80237664();
  u32 uVar1;
  
  uVar1 = fn_80237664();
  return -(uVar1 >> 1 & 0x7fff);
}

/* Address: 0x80251680 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage156(void) { return 0; }
/* Address: 0x80251688 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage155(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x802516C4 | Size: 0xD4 (212 bytes) */
u32 fightTrainerAiWazaDamage154(void* ctx, u32 unused, u32 param2, u32 param3) {
    extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fn_80216CF8(u32, u32, u8, u32, u8);
    extern u32 fightAiGetLevel();
    extern u32 fightTrainerAiGetPokemonDataId();
    extern u32 fightAiGetLevel2();
    extern u32 fightTrainerAiGetPokemonDataId2();
    u32 entries[24];
    u32 total;
    u16 count;
    u16 index;
    u32 mappedEntry;
    u32 entryType;
    u32 convertedParam;
    u32 convertedState;

    total = 0;
    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, entries, 1, 1);
    index = 0;
    while (index < count) {
        mappedEntry = fightTrainerAiGetPokemonDataId2(ctx, entries[index]);
        entryType = fightAiGetLevel2(ctx, entries[index]);
        convertedParam = fightTrainerAiGetPokemonDataId(ctx, param3);
        convertedState = fightAiGetLevel(ctx, param3);
        total += fn_80216CF8(param2, mappedEntry, (u8)entryType, convertedParam, (u8)convertedState);
        index++;
    }
    return total;
}

/* Address: 0x80251798 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage153(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage152_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage152_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x802517A0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage152(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage152_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage152_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80251824 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage151(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80251860 | Size: 0x78 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage150(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x23) == 1) {
        v1 <<= 1;
    }
    return v1;
}

/* Address: 0x802518D8 | Size: 0x78 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage149(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1f) == 1) {
        v1 <<= 1;
    }
    return v1;
}

/* Address: 0x80251950 | Size: 0xBC */
u32 fightTrainerAiWazaDamage148(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern u8 fightOutPokemonIsPokemonJoutaiDataId(u32, u32);
    extern u32 fightSeqCalcDamage(u32, u32, u32, u32, u32, u32);
    u32 damage;
    u32 stat7;
    u32 stat3;
    u32 target;

    stat7 = wazaGetStatus(0, param, 7, 0) & 0xFFFF;
    stat3 = wazaGetStatus(0, param, 3, 0) & 0xFFFF;
    target = fightTargetGetPtrAsNowFightType(2, extra);
    damage = fightSeqCalcDamage(slot, extra, target, param, stat7, stat3);
    if (fightOutPokemonIsPokemonJoutaiDataId(slot, 0x32) == 1) {
        damage = (s32)(damage * 0xf) / 0xa;
    }
    return damage;
}

/* Address: 0x80251A0C | Size: 0x78 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage147(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x20) == 1) {
        v1 <<= 1;
    }
    return v1;
}

/* Address: 0x80251A84 | Size: 0x78 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage146(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x1f) == 1) {
        v1 <<= 1;
    }
    return v1;
}

/* Address: 0x80251AFC | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage145(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80251B38 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage144(void) { return 0; }

/* Address: 0x80251B40 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage143(void) { return 0; }

/* Address: 0x80251B48 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage142(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage140_helper1(u32 arg0, int arg1) {
    return arg0 << arg1;
}

static inline u32 fightTrainerAiWazaDamage140_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80251B50 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage140(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 new_var;
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    new_var = v1;
    return new_var;
}

/* Address: 0x80251BD4 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage139(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80251C58 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage138(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80251CDC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage137(void) { return 0; }

/* Address: 0x80251CE4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage136(void) { return 0; }

/* Address: 0x80251CEC | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage135(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage135SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage135SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80251D2C | Size: 0x88 */
void _fightTrainerAiWazaDamage135SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 param1, u32 param2) {
    extern void* fightOutPokemonGetPokemonPtr();
    extern void pokemonGetMezamerupower();
    extern void wazaSetStatus();
    void* handle;
    u16 var_a;
    u16 var_8;

    handle = pokemonGetStatus(param2, 0, 0xd9, 0);
    pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(param2), &var_a, &var_8);
    wazaSetStatus(handle, 0, 0x2f, 0, var_a);
    wazaSetStatus(handle, 0, 0x30, 0, var_8);
}

/* Address: 0x80251DB4 | Size: 0x90 */
int fightTrainerAiWazaDamage134(void* ctx, u32 param1, u32 param2) {
    extern u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    u32 status;
    s32 value;

    status = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    if ((u8)status == 0) {
        value = (fn_80237664(ctx, param1) >> 1) & 0x7FFF;
    } else if ((u8)status == 1) {
        value = ((s32)(fn_80237664(ctx, param1) & 0xFFFF) * 0x14) / 0x1E;
    } else {
        value = (fn_80237664(ctx, param1) >> 2) & 0x3FFF;
    }
    return -value;
}

/* Address: 0x80251E44 | Size: 0x90 */
int fightTrainerAiWazaDamage133(void* ctx, u32 param1, u32 param2) {
    extern u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    u32 status;
    s32 value;

    status = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    if ((u8)status == 0) {
        value = (fn_80237664(ctx, param1) >> 1) & 0x7FFF;
    } else if ((u8)status == 1) {
        value = ((s32)(fn_80237664(ctx, param1) & 0xFFFF) * 0x14) / 0x1E;
    } else {
        value = (fn_80237664(ctx, param1) >> 2) & 0x3FFF;
    }
    return -value;
}

/* Address: 0x80251ED4 | Size: 0x90 */
int fightTrainerAiWazaDamage132(void* ctx, u32 param1, u32 param2) {
    extern u8 fightTrainerAiGetNowTenkou(void*, u32, u32);
    extern u32 fn_80237664(void*, u32);
    u32 status;
    s32 value;

    status = fightTrainerAiGetNowTenkou(ctx, 0, 1);
    if ((u8)status == 0) {
        value = (fn_80237664(ctx, param1) >> 1) & 0x7FFF;
    } else if ((u8)status == 1) {
        value = ((s32)(fn_80237664(ctx, param1) & 0xFFFF) * 0x14) / 0x1E;
    } else {
        value = (fn_80237664(ctx, param1) >> 2) & 0x3FFF;
    }
    return -value;
}

/* Address: 0x80251F64 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage130(void) { return 20; }

/* Address: 0x80251F6C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage129(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80251FF0 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage128(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    int iVar1;
  iVar1 = fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,0,0);
  return iVar1 << 1;
}

/* Address: 0x80252030 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage127(void) { return 0; }


/* Address: 0x80252038 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage126(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage126SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage126SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80252078 | Size: 0x44 | Pattern: field_accessor */
u32 _fightTrainerAiWazaDamage126SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern u32 wazaSetStatus();
    u32 val = (u32)pokemonGetStatus((void*)param, 0, 0xd9, 0);
    return (u32)wazaSetStatus(val, 0, 0x2f, 0, 0x46);
}

/* Address: 0x802520BC | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage125(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252140 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage124(void) { return 0; }

/* Address: 0x80252148 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage123(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage123SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage123SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80252188 | Size: 0x80 | Pattern: field_accessor */
void _fightTrainerAiWazaDamage123SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern void wazaSetStatus(u32, u32, u32, u32, u32);
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightSeqGetOngaeshiIryoku(u32);
    extern u32 fn_8023842C(void*, u32);
    u32 value;
    u32 amount;

    value = pokemonGetStatus(param, 0, 0xd9, 0);
    wazaGetStatus(value, 0, 0x2f, 0);
    amount = fightSeqGetOngaeshiIryoku(fn_8023842C(ctx, param));
    wazaSetStatus(value, 0, 0x2f, 0, amount & 0xFFFF);
}

/* Address: 0x80252208 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage122(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage122SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage122SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80252248 | Size: 0x44 | Pattern: field_accessor */
u32 _fightTrainerAiWazaDamage122SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern u32 wazaSetStatus();
    u32 val = (u32)pokemonGetStatus((void*)param, 0, 0xd9, 0);
    return (u32)wazaSetStatus(val, 0, 0x2f, 0, 0x28);
}

/* Address: 0x8025228C | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage121(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage121SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage121SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x802522CC | Size: 0x80 | Pattern: field_accessor */
void _fightTrainerAiWazaDamage121SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern void wazaSetStatus(u32, u32, u32, u32, u32);
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 fightAiScale10Div25(u32);
    extern u32 fn_8023842C(void*, u32);
    u32 value;
    u32 amount;

    value = pokemonGetStatus(param, 0, 0xd9, 0);
    wazaGetStatus(value, 0, 0x2f, 0);
    amount = fightAiScale10Div25(fn_8023842C(ctx, param));
    wazaSetStatus(value, 0, 0x2f, 0, amount & 0xFFFF);
}

/* Address: 0x8025234C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage120(void) { return 0; }

/* Address: 0x80252354 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage119(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80252390 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage118(void) { return 0; }

/* Address: 0x80252398 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage117(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage117SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage117SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x802523D8 | Size: 0x90 */
void _fightTrainerAiWazaDamage117SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 param1, u32 param2) {
    extern void wazaSetStatus(u32, u32, u32, u32, u32);
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u8 fightTrainerAiIsPokemonJoutai(void*, u32, u32);
    u32 value;
    u32 result;
    u32 amount;

    value = pokemonGetStatus(param2, 0, 0xd9, 0);
    amount = wazaGetStatus(value, 0, 0x2f, 0) & 0xFFFF;
    if (fightTrainerAiIsPokemonJoutai(ctx, param2, 0x1a) == 1) {
        amount = (amount << 1) & 0xFFFF;
    }
    wazaSetStatus(value, 0, 0x2f, 0, amount & 0xFFFF);
}

/* Address: 0x80252468 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage116(void) { return 0; }

/* Address: 0x80252470 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage115(void) { return 0; }

/* Address: 0x80252478 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage114(void) { return 0; }

/* Address: 0x80252480 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage113(void) { return 0; }

/* Address: 0x80252488 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage112(void) { return 0; }

/* Address: 0x80252490 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage111(void) { return 0; }

/* Address: 0x80252498 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage109(void) { return 0; }

/* Address: 0x802524A0 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage108(void) { return 0; }

/* Address: 0x802524A8 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage107(void) { return 0; }

/* Address: 0x802524B0 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage106(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage105_helper(void* ctx, u32 arg1, u32 arg2, u32 arg3, int arg4, int arg5, int arg6) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    return fightSeqGetNromalWazaDamage(ctx, arg1, arg2, arg3, arg4, 0, arg5, arg6);
}

/* Address: 0x802524B8 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage105(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    int new_var;
    u32 v1 = fightTrainerAiWazaDamage105_helper(ctx, param2, param1, param3, 0, 0, 0);
    u32 new_var2;
    new_var2 = param2;

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if (new_var = ((u16)new_var2) == 0x39) {
            v1 = ((0, v1)) << 1;
        }
    }
    return v1;
}

/* Address: 0x8025253C | Size: 0xB4 */
u32 fightTrainerAiWazaDamage104(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void*, u32, u32, u32, u32, u32, u32, u32);
    extern u32 _fightTrainerAiWazaDamage104SubPre3__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void);
    extern u32 _fightTrainerAiWazaDamage104SubPre2__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void);
    extern u32 _fightTrainerAiWazaDamage104SubPre1__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void);
    u32 result;

    result = fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, (u32)_fightTrainerAiWazaDamage104SubPre1__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon, 0);
    result += fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, (u32)_fightTrainerAiWazaDamage104SubPre2__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon, 0);
    result += fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, (u32)_fightTrainerAiWazaDamage104SubPre3__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon, 0);
    return result;
}

/* Address: 0x802525F0 | Size: 0x44 | Pattern: field_accessor */
u32 _fightTrainerAiWazaDamage104SubPre3__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern u32 wazaSetStatus();
    u32 val = (u32)pokemonGetStatus((void*)param, 0, 0xd9, 0);
    return (u32)wazaSetStatus(val, 0, 0x2f, 0, 0x1e);
}

/* Address: 0x80252634 | Size: 0x44 | Pattern: field_accessor */
u32 _fightTrainerAiWazaDamage104SubPre2__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern u32 wazaSetStatus();
    u32 val = (u32)pokemonGetStatus((void*)param, 0, 0xd9, 0);
    return (u32)wazaSetStatus(val, 0, 0x2f, 0, 0x14);
}

/* Address: 0x80252678 | Size: 0x44 | Pattern: field_accessor */
u32 _fightTrainerAiWazaDamage104SubPre1__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 slot, u32 param) {
    extern u32 wazaSetStatus();
    u32 val = (u32)pokemonGetStatus((void*)param, 0, 0xd9, 0);
    return (u32)wazaSetStatus(val, 0, 0x2f, 0, 0xa);
}

static inline u32 fightTrainerAiWazaDamage103_helper(u32 arg0, int arg1) {
    return arg0 << arg1;
}

/* Address: 0x802526BC | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage103(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if (param3) {
        }
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage103_helper(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252740 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage102(void) { return 0; }

/* Address: 0x80252748 | Size: 0x74 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage101(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u32 fightSeqGetNromalWazaDamage(void*, u32, u32, u32, u32, u32, u32, u32);
    extern u32 fn_802376EC(void*, u32);
    u32 cap;
    u32 damage;

    cap = fn_802376EC(ctx, extra);
    damage = fightSeqGetNromalWazaDamage(ctx, param, slot, extra, 0, 0, 0, 0);
    if ((s32)(cap & 0xFFFF) <= (s32)damage) {
        return (cap & 0xFFFF) - 1;
    }
    return damage;
}

/* Address: 0x802527BC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage100(void) { return 5; }

/* Address: 0x802527C4 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage099(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    extern void _fightTrainerAiWazaDamage099SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon();
  return fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,(u32)_fightTrainerAiWazaDamage099SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon,0);
}


/* Address: 0x80252804 | Size: 0x90 */
void _fightTrainerAiWazaDamage099SubPre__FP13FIGHT_TRAINERUsP15FightOutPokemonP15FightOutPokemon(void* ctx, u32 param1, u32 param2) {
    extern void* pokemonGetStatus();
    extern u32 fn_802376EC();
    extern s32 fn_80237664();
    extern u8 fightSeqGetJitabataIryoku();
    extern void wazaSetStatus();
    void* a;
    u32 b;
    s32 t;

    a = pokemonGetStatus(param2, 0, 0xD9, 0);
    b = fn_802376EC(ctx, param2);
    t = fn_80237664(ctx, param2);
    wazaSetStatus(a, 0, 0x2F, 0, fightSeqGetJitabataIryoku(b, t));
}

/* Address: 0x80252894 | Size: 0x28 | Pattern: call_return_u16 */
extern u32 fn_802376EC(void*, u32, u32);
u16 fightTrainerAiWazaDamage098(void* ctx, u32 p1, u32 p2, u32 p3) { return (u16)fn_802376EC(ctx, p3, p2); }

/* Address: 0x802528BC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage097(void) { return 0; }

/* Address: 0x802528C4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage095(void) { return 0; }

/* Address: 0x802528CC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage094(void) { return 0; }

/* Address: 0x802528D4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage093(void) { return 0; }

/* Address: 0x802528DC | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage092(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80252918 | Size: 0x54 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage091(void* ctx, u32 slot, u32 arg2, u32 param) {
    extern u32 fn_802376EC();
    u32 val1, val2, avg;
    val1 = fn_802376EC(ctx, slot) & 0xFFFF;
    val2 = fn_802376EC(ctx, param) & 0xFFFF;
    avg = (s32)(val1 + val2) / 2;
    return val2 - avg;
}

/* Address: 0x8025296C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage090(void) { return 0; }

/* Address: 0x80252974 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage089(void) { return 0; }

/* Address: 0x8025297C | Size: 0x24 | Pattern: call_return_u8 */
extern u32 fightAiGetLevel(void*);
u8 fightTrainerAiWazaDamage088(void* ctx) { return (u8)fightAiGetLevel(ctx); }

/* Address: 0x802529A0 | Size: 0x24 | Pattern: call_return_u8 */
u8 fightTrainerAiWazaDamage087(void* ctx) { return (u8)fightAiGetLevel(ctx); }

/* Address: 0x802529C4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage086(void) { return 0; }

/* Address: 0x802529CC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage085(void) { return 0; }

/* Address: 0x802529D4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage084(void) { return 0; }

/* Address: 0x802529DC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage083(void) { return 0; }

/* Address: 0x802529E4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage082(void) { return 0; }

/* Address: 0x802529EC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage081(void) { return 0; }

/* Address: 0x802529F4 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage080(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252A78 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage079(void) { return 0; }

/* Address: 0x80252A80 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage078(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252B04 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage077(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    int iVar1;
  iVar1 = fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,0,0);
  return iVar1 << 1;
}


/* Address: 0x80252B44 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage076(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252BC8 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage075(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80252C04 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage073(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252C88 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage072(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252D0C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage071(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

static inline u32 fightTrainerAiWazaDamage070_helper1(u32 arg0) {
    return arg0 << 1;
}

static inline u32 fightTrainerAiWazaDamage070_helper2(void* ctx, u32 arg1, u32 arg2, u32 arg3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    return fightSeqGetNromalWazaDamage(ctx, arg1, arg2, arg3, 0, 0, 0, 0);
}

/* Address: 0x80252D90 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage070(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, volatile unsigned int param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage070_helper2(ctx, param2, param1, param3);

    if (1 == fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21)) {
        if (0x39 == (u16)param2) {
            v1 = fightTrainerAiWazaDamage070_helper1(v1);
        }
    }
    return v1;
}

/* Address: 0x80252E14 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage069(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252E98 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage068(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80252F1C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage067(void) { return 0; }

/* Address: 0x80252F24 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage066(void) { return 0; }

/* Address: 0x80252F2C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage065(void) { return 0; }

/* Address: 0x80252F34 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage062(void) { return 0; }

/* Address: 0x80252F3C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage060(void) { return 0; }

/* Address: 0x80252F44 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage059(void) { return 0; }

/* Address: 0x80252F4C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage058(void) { return 0; }

/* Address: 0x80252F54 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage057(void) { return 0; }

/* Address: 0x80252F5C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage054(void) { return 0; }

/* Address: 0x80252F64 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage053(void) { return 0; }

/* Address: 0x80252F6C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage052(void) { return 0; }

/* Address: 0x80252F74 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage051(void) { return 0; }

/* Address: 0x80252F7C | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage050(void) { return 0; }

/* Address: 0x80252F84 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage049(void) { return 0; }

/* Address: 0x80252F8C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage048(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80253010 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage047(void) { return 0; }

/* Address: 0x80253018 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage046(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage045_helper1(u32 arg0) {
    return arg0 << 1;
}

static inline u32 fightTrainerAiWazaDamage045_helper2(void* ctx, u32 arg1, u32 arg2, u32 arg3, int arg4, int arg5) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    return fightSeqGetNromalWazaDamage(ctx, arg1, arg2, arg3, arg4, 0, arg5, 0);
}

/* Address: 0x80253020 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage045(void* ctx, u32 param1, u32 param2, u32 param3) {
    u8 new_var;
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage045_helper2(ctx, param2, param1, param3, 0, 0);
    int new_var2;
    new_var2 = 0x21;

    if (!param2) {
    }
    if ((new_var = fightTrainerAiIsPokemonJoutai(ctx, param3, new_var2)) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = (0, fightTrainerAiWazaDamage045_helper1(v1));
            new_var2++;
            new_var2--;
        }
    }
    return v1;
}

/* Address: 0x802530A4 | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage044(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    int iVar1;
  iVar1 = fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,0,0);
  return iVar1 << 1;
}


/* Address: 0x802530E4 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage043(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80253168 | Size: 0x88 */
u32 fightTrainerAiWazaDamage042(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0xfa) {
            v1 <<= 1;
        }
    }
    return v1;
}

/* Address: 0x802531F0 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage041(void) { return 40; }

/* Address: 0x802531F8 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage040(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253234 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage039(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253270 | Size: 0x28 | Pattern: call_return_u16 */
u16 fightTrainerAiWazaDamage038(void* ctx, u32 p1, u32 p2, u32 p3) { return (u16)fn_802376EC(ctx, p3, p2); }

/* Address: 0x80253298 | Size: 0x28 | Ghidra import */
int fightTrainerAiWazaDamage037(void)

{
    extern u32 fn_80237664();
  u32 uVar1;
  
  uVar1 = fn_80237664();
  return -(uVar1 & 0xffff);
}
static inline u32 fightTrainerAiWazaDamage036_helper(u32 arg0) {
    return arg0 << 1;
}

/* Address: 0x802532C0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage036(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, volatile int param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage036_helper(v1);
        }
    }
    return v1;
}

/* Address: 0x80253344 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage035(void) { return 0; }

static inline u32 fightTrainerAiWazaDamage034_helper(void* ctx, u32 arg1, u32 arg2, u32 arg3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    return fightSeqGetNromalWazaDamage(ctx, arg1, arg2, arg3, 0, 0, 0, 0);
}

/* Address: 0x8025334C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage034(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 *new_var;
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage034_helper(ctx, param2, param1, param3);

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            new_var = &v1;
            v1 = (*new_var) << 1;
        }
    }
    return v1;
}

/* Address: 0x802533D0 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage033(void) { return 0; }

/* Address: 0x802533D8 | Size: 0x28 | Ghidra import */
int fightTrainerAiWazaDamage032(void)

{
    extern u32 fn_80237664();
  u32 uVar1;
  
  uVar1 = fn_80237664();
  return -(uVar1 >> 1 & 0x7fff);
}
static inline u32 fightTrainerAiWazaDamage031_helper1(u32 arg0) {
    return arg0 << 1;
}

static inline u32 fightTrainerAiWazaDamage031_helper2(u32 arg0) {
    return arg0;
}

/* Address: 0x80253400 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage031(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage031_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage031_helper1(v1);
        }
    }
    return v1;
}

/* Address: 0x80253484 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage030(void) { return 0; }

/* Address: 0x8025348C | Size: 0x40 | Ghidra import */
u32 fightTrainerAiWazaDamage029(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern u32 fightSeqGetNromalWazaDamage();
    int iVar1;
  iVar1 = fightSeqGetNromalWazaDamage(r3,r5,r4,r6,0,0,0,0);
  return iVar1 * 3;
}

/* Address: 0x802534CC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage028(void) { return 0; }


/* Address: 0x802534D4 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage027(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253510 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage026(void) { return 0; }

/* Address: 0x80253518 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage025(void) { return 0; }

/* Address: 0x80253520 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage024(void) { return 0; }

/* Address: 0x80253528 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage023(void) { return 0; }

/* Address: 0x80253530 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage020(void) { return 0; }

/* Address: 0x80253538 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage019(void) { return 0; }

/* Address: 0x80253540 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage018(void) { return 0; }

/* Address: 0x80253548 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage017(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x802535CC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage016(void) { return 0; }

/* Address: 0x802535D4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage013(void) { return 0; }

/* Address: 0x802535DC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage011(void) { return 0; }

/* Address: 0x802535E4 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage010(void) { return 0; }

/* Address: 0x802535EC | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage009(void) { return 0; }

/* Address: 0x802535F4 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage008(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253630 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage007(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x8025366C | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage006(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x802536F0 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage005(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x80253774 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage004(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x802537F8 | Size: 0x3C | Pattern: simple_wrapper */
u32 fightTrainerAiWazaDamage003(void* ctx, u32 param1, u32 param2, u32 param3) { return fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0); }

/* Address: 0x80253834 | Size: 0x84 | Pattern: field_accessor */
u32 fightTrainerAiWazaDamage002(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = fightTrainerAiWazaDamage140_helper2(fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));
    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 = fightTrainerAiWazaDamage140_helper1(v1, 1);
        }
    }
    return v1;
}

/* Address: 0x802538B8 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamage001(void) { return 0; }

/* Address: 0x802538C0 | Size: 0x88 */
u32 fightTrainerAiWazaDamage000(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightSeqGetNromalWazaDamage(void* ctx, u32 param2, u32 param1, u32 param3, u32 zero1, u32 zero2, u32 zero3, u32 zero4);
    extern u8 fightTrainerAiIsPokemonJoutai(void* ctx, u32 param3, u32 flag);
    u32 v1 = (0, fightSeqGetNromalWazaDamage(ctx, param2, param1, param3, 0, 0, 0, 0));

    if (fightTrainerAiIsPokemonJoutai(ctx, param3, 0x21) == 1) {
        if ((u16)param2 == 0x39) {
            v1 <<= 1;
        }
    }
    return v1;
}

/* Address: 0x80253948 | Size: 0x8 | Pattern: return_constant */
u32 fightTrainerAiWazaDamageNull(void) { return 0; }
