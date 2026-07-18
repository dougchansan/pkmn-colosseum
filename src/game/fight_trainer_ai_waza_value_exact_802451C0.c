/**
 * @file fight_trainer_ai_waza_value_exact_802451C0.c
 * @brief Byte-exact fightTrainerAiWazaValue.cpp range, 0x802451C0 - 0x80245FC4.
 *
 * Physically split from fight_trainer_ai_waza_value.c so this
 * translation unit owns only the functions in the stated range.
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
extern void fn_801EF8F4();

/* Sound functions */
extern void soundStop();     /* Stop sound */
extern void fn_80165A20();     /* Fade out music */
extern void fn_801659FC();     /* Start BGM */

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
u32 evolutionWazaLearn();
int fightTrainerAiWazaValueKuroikiri(void* ctx, u32 param1, u32 param2, u32 param3);
void fightTrainerAiWazaValueHimitunotikara(void* ctx, u32 param1, u32 param2, u32 param3);
s32 fightTrainerAiSelectIrekaeDasuFightPokemon(void* ctx, u32 param1, u32 param2, u32 param3);
u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType);
u32 fightMenuFightTrainerGcHeroOpenMenu(void* ctx, u32 param1, u32 param2);
u8 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(u32, void*, u32, u32, u32, u32);
s32 fightTrainerGetStatus(u32, u32, u32, u32);
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, void*, u32, u32);
u32 fightOutPokemonGetPokemonPtr(u32);
u8 fn_80237310(void*, u32);
u8 fn_80237F74(void*, u32, u32);
u8 fn_802384B4(void*, u32, u32);
u8 fn_80239564(void*, u32);
u8 fn_80235B04(void*, u32, u32);
u16 fn_80238980(void*, u32);
u8 fn_80238E30(void*, u32, u32);
u8 fn_80239058(void*, u32, u32);
u32 fn_80239984(u32, void*, u32);
u32 fightTrainerAiAddValue(u32, s32);
void fn_80239CCC(u32, void*, u32, u32, u32, u32, u32, u32, s32);
void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);

/* Address: 0x802451C0 | Size: 0x258 (600 bytes) */
u32 fightTrainerAiWazaValueAmagoi(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 pokemon[23];
    u32 handle;
    u8 weather;
    u16 count;
    u16 i;

    handle = 0;
    weather = fn_80235B04(ctx, 0, 0);
    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, pokemon, 1, 1);

    if (weather != 2) {
        handle = fn_80239984(0, ctx, 0x14e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14e);
    }

    for (i = 0; i < count; i++) {
        if (fn_80238E30(ctx, pokemon[i], 0xb) == 1 ||
            fn_80239058(ctx, pokemon[i], 0x21) == 1 ||
            fn_80239058(ctx, pokemon[i], 0x2c) == 1) {
            handle = fn_80239984(handle, ctx, 0x14f);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14f);
            break;
        }
    }

    for (i = 0; i < count; i++) {
        if (fn_80238980(ctx, pokemon[i]) == 0x181) {
            handle = fn_80239984(handle, ctx, 0x150);
            fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x150);
            break;
        }
    }

    if (weather == 2) {
        handle = fn_80239984(handle, ctx, 0x151);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x151);
    }
    return handle;
}
/* Address: 0x80245418 | Size: 0x160 (352 bytes) */
u32 fightTrainerAiWazaValueNemurare(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32, void*, u32*, u32, u32);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_802384B4(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 entries[26];
    u32 setup;
    u32 compareValue;
    u16 count;
    u16 index;

    setup = 0;
    count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, entries, 0, 1);
    if (fn_80237310(ctx, param3) == 1) {
        setup = fn_80239984(0, ctx, 0x14c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14c);
    }

    index = 0;
    while (index < count) {
        compareValue = pokemonGetStatus(param3, 0, 0xd5, 0);
        if (compareValue == entries[index]) {
            ;
        } else {
            if (fn_802384B4(ctx, entries[index], 8) == 1) {
                setup = fn_80239984(setup, ctx, 0x14d);
                fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14d);
                break;
            }
        }
        index++;
    }

    return setup;
}
/* Address: 0x80245578 | Size: 0x1A0 (416 bytes) */
u32 fightTrainerAiWazaValueNemuru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80237310(void*, u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_8023831C(void*);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fn_8023831C(ctx);
    if ((state & 0xffff) == 3 || (state & 0xffff) == 9) {
        handle = fn_80239984(0, ctx, 0x148);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x148);
    }
    if (fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(handle, ctx, 0x149);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x149);
    }
    if (fn_80237310(ctx, param1) == 0) {
        handle = fn_80239984(handle, ctx, 0x14a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14a);
    }
    handle = fn_80239984(handle, ctx, 0x14b);
    fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x14b);
    return handle;
}
/* Address: 0x80245718 | Size: 0x98 */
u32 fightTrainerAiWazaValueKaihuku2(void* ctx, u32 param1, u32 param2) {
    extern const f32 lbl_8047E630;
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;

    handle = 0;
    if (fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(0, ctx, 0x147);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x147);
    }
    return handle;
}
/* Address: 0x802457B0 | Size: 0x168 (360 bytes) */
u32 fightTrainerAiWazaValueKaihuku1(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern f32 lbl_8047E630;
    extern u32 fn_80235B04();
    extern u8 fn_802373B0(void* a, u32 b, s32 c, f32 d);
    extern s32 fn_80239984();
    extern u32 fightOutPokemonGetPokemonPtr();
    extern void fn_80239EE8();
    s32 handle = 0;
    u32 state;

    state = fn_80235B04(ctx, 0, 1);
    if ((u8)state == 1) {
        handle = fn_80239984(0, ctx, 0x144);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x144);
    }
    if ((u8)fn_802373B0(ctx, param1, -1, lbl_8047E630) == 1) {
        handle = fn_80239984(handle, ctx, 0x145);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x145);
    }
    if ((u8)state == 2 || (u8)state == 4 || (u8)state == 3) {
        handle = fn_80239984(handle, ctx, 0x146);
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x146);
    }
    return handle;
}
/* Address: 0x80245918 | Size: 0x98 */
u32 fightTrainerAiWazaValueKituke(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle = 0;

    if (fn_80236BFC(ctx, param3, 5) == 1) {
        u32 tmp = fn_80239984(0, ctx, 0x143);
        handle = tmp;
        fn_80239EE8(0xEC64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x143);
    }
    return handle;
}
/* Address: 0x802459B0 | Size: 0x44C (1100 bytes) */
u32 fightTrainerAiWazaValueOiuti(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32, u32);
    extern u32 fn_801F8A18(u32, u16*);
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235BE4(void*, u32, u32, u32);
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u16 tmp;
    u32 found;
    u32 handle;
    u32 pokemon;
    u32 status;

    handle = 0;
    status = fn_80235BE4(ctx, 0, param3, 0);
    pokemon = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    tmp = 0;
    found = fn_801F8A18(pokemon, &tmp);
    if (found == 0) {
        status = 1;
    }
    if ((u8)fn_80236BFC(ctx, param3, 3) == 1) {
        handle = fn_80239984(0, ctx, 0x139);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x139);
    }
    if ((u8)fn_80236BFC(ctx, param3, 4) == 1) {
        handle = fn_80239984(handle, ctx, 0x13a);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13a);
    }
    if ((u8)fn_80236BFC(ctx, param3, 6) == 1) {
        handle = fn_80239984(handle, ctx, 0x13b);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13b);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1c) == 1) {
        handle = fn_80239984(handle, ctx, 0x13c);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13c);
    }
    if ((u8)fn_80236BFC(ctx, param3, 9) == 1) {
        handle = fn_80239984(handle, ctx, 0x13d);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13d);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0xa) == 1) {
        handle = fn_80239984(handle, ctx, 0x13e);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13e);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x18) == 1) {
        handle = fn_80239984(handle, ctx, 0x13f);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x13f);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x1e) == 1) {
        handle = fn_80239984(handle, ctx, 0x140);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x140);
    }
    if ((u8)fn_80236BFC(ctx, param3, 0x26) == 1) {
        handle = fn_80239984(handle, ctx, 0x141);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x141);
    }
    if ((u8)status != 0) {
        handle = fn_80239984(handle, ctx, 0x142);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x142);
    }
    return handle;
}
/* Address: 0x80245DFC | Size: 0x14C (332 bytes) */
u32 fightTrainerAiWazaValueUezaabooru(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32);
    extern u32 fn_80235B04(void*, u32, u32);
    extern u32 fn_80239984(u32, void*, u32);
    extern void fn_80239EE8(u32, void*, u32, u32, u32, u32, u32, u32);
    u32 handle;
    u32 state;

    handle = 0;
    state = fn_80235B04(ctx, 0, 1);
    if ((u8)state == 1 || (u8)state == 2) {
        handle = fn_80239984(0, ctx, 0x136);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x136);
    }
    if ((u8)state == 4) {
        handle = fn_80239984(handle, ctx, 0x137);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x137);
    }
    if ((u8)state == 3) {
        handle = fn_80239984(handle, ctx, 0x138);
        fn_80239EE8(0xec64, ctx, fightOutPokemonGetPokemonPtr(param1), 0, 0, param2, 0, 0x138);
    }
    return handle;
}
/* Address: 0x80245F48 | Size: 0x7C | Pattern: field_accessor */
u32 fightTrainerAiWazaValueSizennotikara(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u16 tikeiDataBiosGetWazaId(u16);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 fn_8023CA9C(void*, u32, u16, u32);
    u16 value;

    value = tikeiDataBiosGetWazaId((u16)fightFloorGetStatus(0, 0, 0xf, 0));
    if (value != (u16)param) {
        return fn_8023CA9C(ctx, slot, value, extra);
    }
    return 0;
}
