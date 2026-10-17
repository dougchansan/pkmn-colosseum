/**
 * @file fight_floor.c
 * @brief Fight-floor core: turn/result state, party enumeration, PokemonGet/Set dispatch.
 *
 * Split out of the former game/pokemon.c CodeCandidate bucket
 * (0x801F000C-0x801F7F80), which was mislabeled "pokemon" but is
 * entirely the XD-era fight-engine cluster. Address range covered by
 * this translation unit: 0x801F150C-0x801F640C (64 functions), per
 * config/GC6E01/splits.txt.
 */

#include "game/pokemon_fight_types.h"

u32 _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(
    void* obj, u32 slot, void* data);

/* Pokemon XD's _fightFloorInitFightOutPokemonPtrAry__FPP15FightOutPokemon
 * (GXXE01 0x801F5A84, size 0x28; TeamOrre/xd-decomp symbols.txt 4989794e,
 * body trevor403/xd-asm b1087f18 code/func_FUN_801F5A84.s): clears the
 * 8-entry fight-out Pokemon pointer array. XD calls it from
 * fightFloorGetFightTrainerFightOutPokemonPtrAry (0x801F2298) and the other
 * pointer-array getters; Colosseum expands it at each caller, where the
 * stored zero is a copy of the counter's zero (`li rN,0; mr rM,rN`), which
 * only an inlined counter produces. */
static inline void _fightFloorInitFightOutPokemonPtrAry(void* ary) {
    u16 i;
    for (i = 0; i < 8; i++)
        ((u32*)ary)[i] = 0;
}

/* fightFloorGetValidFightSidePtr (0x801F47B4), compile-only copy for the
 * callers that expand it before its definition; see the note at
 * fightFloorLoopValidFightOutPokemon. */
/* RULE-EXCEPTION(user-approved): compile-only inline copy of a real fight_floor function defined later in this file — see docs/RULE_EXCEPTIONS.md */
static inline void* _fightFloorGetValidFightSidePtr(void* floor, u16 side) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    extern u8 fightSideCheckValid(void*);
    void* p;

    p = (void*)fightFloorGetStatus(floor, 0, 0x35, side);
    if (fightSideCheckValid(p) == 0)
        return NULL;
    return p;
}

/* fightFloorGetGcHeroFightTrainerPtr (0x801F2A7C): the first GC-hero
 * trainer on either side. fightFloorIsGcHeroWin expands it through
 * fightFloorIsGcHeroHostSide. */
/* RULE-EXCEPTION(user-approved): compile-only inline copy of a real fight_floor function defined later in this file — see docs/RULE_EXCEPTIONS.md */
static inline void* _fightFloorGetGcHeroFightTrainerPtr(void* floor) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    extern void* fightSideGetValidFightTrainerPtr(void*, u16);
    extern u8 fightTrainerIsGcHero(void*);
    void* sidePtr;
    u16 i;
    u16 side;
    u16 trainers;
    void* trainer;

    fightFloorGetStatus(floor, 0, 0x14, 0);
    trainers = fightFloorGetStatus(floor, 0, 0x16, 0);
    for (side = 0; side < 2; side++) {
        sidePtr = _fightFloorGetValidFightSidePtr(floor, side);
        if (sidePtr != NULL) {
            for (i = 0; i < trainers; i++) {
                trainer = fightSideGetValidFightTrainerPtr(sidePtr, i);
                if (trainer != NULL && fightTrainerIsGcHero(trainer) == 1)
                    return trainer;
            }
        }
    }
    return NULL;
}

#if defined(FIGHT_FLOOR_801F150C_801F1588)

/* 0x801F150C | size: 0x48 | small */
void fightFloorSetTuusinErrorFightResult(void* param) {
    extern void fightFloorLoopValidFightOutPokemon(void*, void*, u32, u32);
    extern void fightFloorSetFightResultId(void*, u32);
    extern s32 _fightFloorSetTuusinErrorFightResultSub__FPvUsPv(void*);
    void* obj;

    obj = param;
    fightFloorLoopValidFightOutPokemon(obj, _fightFloorSetTuusinErrorFightResultSub__FPvUsPv, 0, 0);
    fightFloorSetFightResultId(obj, 1);
}

/* 0x801F1554 | size: 0x34 */
extern u32 pokemonSetStatus(void* context, u32 slot, u16 tableId, u32 flags, u32 value);
s32 _fightFloorSetTuusinErrorFightResultSub__FPvUsPv(void* context) {
    pokemonSetStatus(context, 0, 0x112, 0, 1);
    return 1;
}

#endif

#if defined(FIGHT_FLOOR_801F1588_801F1700)

/* 0x801F1588 | size: 0x178 | medium */
void fightFloorSetTimeOutAllFightResult(void* param) {
    extern u16 std103_battleGetResult();
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern void fightFloorSetFightResultId(void*, u32);
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern u32 fightSideGetFightPokemonNum(u32, u16, u16);
    extern u32 fightSideGetFightPokemonMaxHp(u32, u16, u16);
    extern u32 fightSideGetFightPokemonNokoriHp(u32, u16, u16);
    void* obj;
    u32 pct1;
    u16 slot1;
    u16 slot2;
    u32 op;
    u32 base1;
    u32 base2;
    u32 pct2;

    obj = param;
    if ((u16)std103_battleGetResult(obj) == 1) {
        return;
    }
    slot1 = (u16)fightFloorGetStatus(obj, 0, 0x16, 0);
    slot2 = (u16)fightFloorGetStatus(obj, 0, 0x17, 0);
    op = fightTargetGetPtrAsNowFightType(4, 0);
    base1 = fightSideGetFightPokemonNum(op, slot1, slot2);
    base2 = fightSideGetFightPokemonNokoriHp(op, slot1, slot2);
    pct1 = base2 * 0x64 / fightSideGetFightPokemonMaxHp(op, slot1, slot2);

    op = fightTargetGetPtrAsNowFightType(5, 0);
    base2 = fightSideGetFightPokemonNum(op, slot1, slot2);
    pct2 = fightSideGetFightPokemonNokoriHp(op, slot1, slot2);
    pct2 = pct2 * 0x64 / fightSideGetFightPokemonMaxHp(op, slot1, slot2);

    fightFloorSetFightResultId(obj, 0);
    if (((u16)base1) > ((u16)base2)) {
        fightFloorSetFightResultId(obj, 2);
    }
    if (((u16)base1) < ((u16)base2)) {
        fightFloorSetFightResultId(obj, 3);
    }
    if ((u16)std103_battleGetResult() == 0) {
        if (pct1 >= pct2) {
            fightFloorSetFightResultId(obj, 2);
        }
        if (pct1 <= pct2) {
            fightFloorSetFightResultId(obj, 3);
        }
    }
}



#endif

#if defined(FIGHT_FLOOR_801F1700_801F1990)

/* 0x801F1700 | size: 0x58 | small */
u32 fightFloorIsUseFightTimerCommand(void* param) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern s32 menuCBRule_GetCommandTimeLimit(u32);
    u32 val;
    u8 flag;
    s32 r;

    val = fightFloorGetStatus(param, 0, 0x34, 0);
    r = val;
    flag = r & 0xFF;
    r = menuCBRule_GetCommandTimeLimit(r);
    if (flag == 1 && r > 0) {
        return 1;
    }
    return 0;
}

/* 0x801F1758 | size: 0x58 | small */
u32 fightFloorIsUseFightTimerAll(void* param) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern s32 menuCBRule_GetBattleTimeLimit(u32);
    u32 val;
    u8 flag;
    s32 r;

    val = fightFloorGetStatus(param, 0, 0x34, 0);
    r = val;
    flag = r & 0xFF;
    r = menuCBRule_GetBattleTimeLimit(r);
    if (flag == 1 && r > 0) {
        return 1;
    }
    return 0;
}

/* 0x801F17B0 | size: 0xD8 | medium */
void fightFloorInitFightStart(void* obj) {
    extern void* fightFloorGetStatus(void*, u32, u32, u32);
    extern void* fightSideGetStatus(void*, u32, u32, u32);
    extern void* fightTrainerGetStatus(void*, u32, u32, u32);
    extern void pokemonSetStatus(void*, u32, u32, u32, u32);
    void* a;
    void* b;
    void* c;
    u32 j;
    u32 k;
    u32 i;

    i = 0;
    while ((i & 0xFFFF) < 2) {
        a = fightFloorGetStatus(obj, 0, 0x35, i);
        if (a != 0) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                b = fightSideGetStatus(a, 0, 0x7, j);
                if (b != 0) {
                    k = 0;
                    while ((k & 0xFFFF) < 2) {
                        c = fightTrainerGetStatus(b, 0, 0x46, k);
                        if (c != 0) {
                            pokemonSetStatus(c, 0, 0xfa, 0, 0);
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F1888 | size: 0x54 | small */
u32 fn_801F1888(u32 param_1) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    u16 v;

    v = fightFloorGetStatus(param_1, 0, 0x1A, 0);
    if (v == 0xA || v == 0x11 || v == 0xB) {
        return 1;
    }
    return 0;
}

/* 0x801F18DC | size: 0x3C | small */
u8 fn_801F18DC(u32 param_1) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);

    return (0x10 - (fightFloorGetStatus(param_1, 0, 0x1a, 0) & 0xFFFF)) == 0;
}

/* 0x801F1918 | size: 0x74 | small */
void fightFloorSetMenuFightAction(void* obj, u32 arg2, void* dst) {
    extern u32 fightOutPokemonGetNicknamePtr(u32);
    extern void GScharCpy(void*, u32);
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    void* d;
    void* o;

    o = obj;
    d = dst;
    GScharCpy(d, fightOutPokemonGetNicknamePtr(arg2));
    if ((fightFloorGetStatus(o, 0, 0x22, 0) & 0xFF) == 1) {
        *(u8*)((u8*)d + 0x16) = 0;
    } else {
        *(u8*)((u8*)d + 0x16) = 1;
    }
}

/* 0x801F198C | size: 0x4 | trivial */
void fightFloorSetShadow(void) {}

#endif

#if defined(FIGHT_FLOOR_801F1990_801F1A6C)

/* 0x801F1990 | size: 0xDC | medium */
u32 fightFloorGetFightTrainerFightOutPokemonIsFightActionAttackWazaOut(void* arg1, void* arg2, u8 arg3, u8 arg4, void* arg5, void* arg6) {
    extern void* _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv(void*, u16, void*);
    extern void fightFloorLoopValidFightOutPokemon(void*, void*, void*, u32);
    extern u32 fightOutPokemonIsFightActionAttackWazaOut(void*, void*, void*);
    void* ary[8];
    struct { void* r4; void* arr; u32 cnt; u32 a; u32 b; } sdata;
    u16 count;
    u16 i;
    _fightFloorInitFightOutPokemonPtrAry(ary);
    sdata.r4 = arg2;
    sdata.arr = ary;
    sdata.cnt = 0;
    sdata.a = arg3;
    sdata.b = arg4;
    fightFloorLoopValidFightOutPokemon(arg1, _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv, &sdata, 0);
    count = sdata.cnt;
    for (i = 0; i < count; i++) {
        if ((u8)fightOutPokemonIsFightActionAttackWazaOut(ary[i], arg5, arg6) == 1) return 1;
    }
    return 0;
}

#endif

#if defined(FIGHT_FLOOR_801F1A6C_801F1B14)

/* 0x801F1A6C | size: 0xA8 | medium */
u16 fightFloorGetFightTrainerFightPokemonPtrAry(u32 obj, u32 slot, u32* arr, u8 r6, u8 r7_orig) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern void fightFloorLoopValidFightTrainer(u32, void*, void*, u32);
    extern void _fightFloorGetFightTrainerFightPokemonPtrArySub__FPvUsPv(void);
    u32 buf[6];
    u16 i;
    u32 val;
    u32 res;
    i = 0;
    val = i;
    while ((u16)i < 0x18) {
        arr[i] = val;
        i++;
    }
    buf[0] = slot;
    buf[1] = r6;
    buf[2] = 0;
    buf[3] = (u32)arr;
    res = fightFloorGetStatus(0, 0, 0x17, 0);
    buf[4] = res;
    buf[5] = r7_orig;
    fightFloorLoopValidFightTrainer(obj, (void*)_fightFloorGetFightTrainerFightPokemonPtrArySub__FPvUsPv, buf, 0);
    return (u16)buf[2];
}

#endif

#if defined(FIGHT_FLOOR_801F1B14_801F1F30)

/* 0x801F1B14 | size: 0x184 | medium */
u32 _fightFloorGetFightTrainerFightPokemonPtrArySub__FPvUsPv(u32 p, u16 slot, u32 d) {
    extern u8 fightTrainerCheckValid(void*);
    extern u8 fightTrainerIsAllyFightTargetPtr(void*, void*, u16);
    extern void* fightTrainerGetValidFightPokemonPtr(void*, u16);
    extern u8 fightPokemonCheckFightOut(void*);
    struct W { void* trainer; s32 flag1; u32 count; void** arr; u32 maxcnt; s32 flag2; };
    void* obj = (void*)p;
    struct W* w = (struct W*)d;
    void* trainer;
    void** arr;
    u16 i;
    void* ptr;
    u16 maxcnt;

    trainer = w->trainer;
    arr = w->arr;
    if (fightTrainerCheckValid(obj) == 0)
        return 1;
    if (w->flag1 == 0) {
        if (fightTrainerIsAllyFightTargetPtr(obj, trainer, slot) == 1)
            return 1;
    } else {
        if (fightTrainerIsAllyFightTargetPtr(obj, trainer, slot) == 0)
            return 1;
    }
    maxcnt = w->maxcnt;
    for (i = 0; i < maxcnt; i++) {
        ptr = fightTrainerGetValidFightPokemonPtr(obj, i);
        if (ptr != NULL) {
            if (w->flag2 != 1 || fightPokemonCheckFightOut(ptr) != 0) {
                arr[w->count] = ptr;
                w->count++;
            }
        }
    }
    return 1;
}

/* 0x801F1C18 | size: 0x80 | small */
u16 fightFloorGetFightTrainerFightOutPokemonPtrAry(u32 obj, u32 slot, u32* arr, u8 r6, u8 r7) {
    extern void fightFloorLoopValidFightOutPokemon(u32, void*, void*, u32);
    extern void _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv(void);
    u32 buf[5];
    _fightFloorInitFightOutPokemonPtrAry(arr);
    buf[0] = slot;
    buf[1] = (u32)arr;
    buf[2] = 0;
    buf[3] = r6;
    buf[4] = r7;
    fightFloorLoopValidFightOutPokemon(obj, (void*)_fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv, buf, 0);
    return (u16)buf[2];
}

/* 0x801F1C98 | size: 0xC0 | medium */
u32 _fightFloorGetFightTrainerFightOutPokemonPtrArySub__FPvUsPv(u32 p, u16 slot, void* data) {
    extern u8 fightOutPokemonCheckFightOut(void*);
    extern u8 fightTrainerIsAllyFightTargetPtr(void*, void*, u16);
    struct W { void* trainer; void** arr; u32 count; s32 flag; s32 mode; };
    void* obj = (void*)p;
    struct W* w = data;
    void* trainer = w->trainer;
    void** arr = w->arr;
    if (w->mode == 1) {
        if (fightOutPokemonCheckFightOut(obj) == 0)
            return 1;
    }
    if (w->flag == 0) {
        if (fightTrainerIsAllyFightTargetPtr(trainer, obj, slot) == 1)
            return 1;
    } else {
        if (fightTrainerIsAllyFightTargetPtr(trainer, obj, slot) == 0)
            return 1;
    }
    arr[w->count] = obj;
    w->count++;
    return 1;
}

/* 0x801F1D5C | size: 0x60 | small */
void fightFloorGetFightOutPokemonPtrAry(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32* buf) {
    extern void _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(u32, u32*, u32, u32, u32);
    _fightFloorInitFightOutPokemonPtrAry(buf);
    _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(arg0, buf, arg1, arg2, arg3);
}

/*
 * Pokemon XD's fightFloorIsGcHeroHostSide (GXXE01 0x801F24AC, size 0x60;
 * TeamOrre/xd-decomp symbols.txt 4989794e, body trevor403/xd-asm b1087f18
 * code/func_FUN_801F24AC.s): reads the floor's fight type, then
 * fightFloorGetGcHeroFightTrainerPtr(floor), then
 * fightTargetIsHostSide(trainer, type). XD reads the type through
 * fightFloor_GetFightType; Colosseum has no such getter and reads the same
 * value as fightFloorGetStatus field 0x14. XD's fightFloorIsGcHeroWin
 * (0x801F2434) calls it; Colosseum expands it here, with
 * fightFloorGetGcHeroFightTrainerPtr expanded inside it.
 */
static inline u8 fightFloorIsGcHeroHostSide(void* floor) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    extern u8 fightTargetIsHostSide(void*, u16);
    u16 type;
    void* trainer;

    type = fightFloorGetStatus(floor, 0, 0x14, 0);
    trainer = _fightFloorGetGcHeroFightTrainerPtr(floor);
    if (trainer != NULL && fightTargetIsHostSide(trainer, type) == 1)
        return 1;
    return 0;
}

/* 0x801F1DBC | size: 0x174 | medium */
u32 fightFloorIsGcHeroWin(void* floor, u16 result) {
    if (result == 2 || result == 3) {
        if (fightFloorIsGcHeroHostSide(floor) == 1) {
            if (result == 2)
                return 1;
        } else {
            if (result == 3)
                return 1;
        }
    }
    return 0;
}

#endif

#if defined(FIGHT_FLOOR_801F1F30_801F1F7C)

/* 0x801F1F30 | size: 0x4C | small */
u8 fightFloorCheckHuuinWazaFightOutPokemon(u32 param_1, u32 param_2, u16 param_3) {
    extern void fightFloorLoopValidFightOutPokemon(u32, void*, void*, u32);
    extern void _fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv(void);
    u32 buf[3];

    buf[0] = param_2;
    buf[1] = param_3;
    buf[2] = 0;
    fightFloorLoopValidFightOutPokemon(param_1, (void*)_fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv, buf, 0);
    return (u8)buf[2];
}

#endif

#if defined(FIGHT_FLOOR_801F1F7C_801F2020)

/* 0x801F1F7C | size: 0x90 | medium */
u32 _fightFloorCheckHuuinWazaFightOutPokemonSub__FPvUsPv(u32 p, u16 slot, void* data) {
    extern u8 fightOutPokemonCheckFightOut(void*);
    extern u8 fightOutPokemonIsAlly(void*, void*);
    extern u8 fightOutPokemonIsPokemonJoutaiDataId(void*, u32);
    extern void* fightOutPokemonGetPokemonPtr(void*);
    extern s8 pokemonSearchWazaDataId(void*, u16);
    struct W { void* mon; u32 waza; u32 result; };
    void* obj = (void*)p;
    struct W* s = data;
    void* mon;
    u16 waza;

    waza = s->waza;
    mon = s->mon;
    if (fightOutPokemonCheckFightOut(obj) == 0)
        return 1;
    if (fightOutPokemonIsAlly(obj, mon) == 0 && fightOutPokemonIsPokemonJoutaiDataId(obj, 0x27) == 1
        && pokemonSearchWazaDataId(fightOutPokemonGetPokemonPtr(obj), waza) >= 0) {
        s->result = 1;
        return 0;
    }
    return 1;
}

#endif

#if defined(FIGHT_FLOOR_801F2020_801F2350)

/* 0x801F2020 | size: 0x1FC | medium */
u32 fightFloorCheckFightActionFightOutPokemonIrekaeSelect(u32 obj, u32 r4arg, u32* out) {
    extern u32 fightOutPokemonCheckValid(u32);
    extern void fightFloorLoopValidFightOutPokemon(u32, void*, void*, u32);
    extern u32 fightOutPokemonIsZokuseiDataId(u32, u32);
    extern u32 fightOutPokemonGetTokuseiDataId(u32);
    extern u32 fightOutPokemonIsPokemonJoutaiDataId(u32, u32);
    extern void _fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv(void);
    u32 buf1[4];
    u32 buf2[4];
    u32 buf3[4];
    u32 r27;
    u32 r26;
    u32 r28;
    u32 r31;
    u32 r30;
    u32 r29;
    u32 r25;
    r27 = r4arg;
    r26 = obj;
    r28 = (u32)out;
    r30 = 0;
    r29 = 0;
    if ((u8)fightOutPokemonCheckValid(r27) == 0)
        return 0;
    buf1[0] = 0x17;
    buf1[1] = 0;
    buf1[2] = 2;
    buf1[3] = r27;
    fightFloorLoopValidFightOutPokemon(r26, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf1, 0);
    buf2[0] = 0x47;
    buf2[1] = 0;
    buf2[2] = 2;
    buf2[3] = r27;
    r31 = buf1[1];
    fightFloorLoopValidFightOutPokemon(r26, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf2, 0);
    r25 = buf2[1];
    buf3[0] = 0x2a;
    buf3[1] = 0;
    buf3[2] = 0;
    buf3[3] = r27;
    fightFloorLoopValidFightOutPokemon(r26, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf3, 0);
    r26 = buf3[1];
    if ((u8)fightOutPokemonIsZokuseiDataId(r27, 2) == 1) goto _setR30;
    if ((u16)fightOutPokemonGetTokuseiDataId(r27) != 0x1a) goto _afterR30;
_setR30:
    r30 = 1;
_afterR30:
    if ((u8)fightOutPokemonIsZokuseiDataId(r27, 8) == 1) r29 = 1;
    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(r27, 0x16) == 1) goto _ret1;
    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(r27, 0xe) == 1) goto _ret1;
    if ((u8)fightOutPokemonIsPokemonJoutaiDataId(r27, 0x25) == 1) { _ret1: return 1; }
    if (r31 != 0) {
        if (r28 != 0) *(u32*)r28 = r31;
        return 2;
    }
    if (r25 != 0) {
        if ((u8)r30 == 0) {
            if (r28 != 0) *(u32*)r28 = r25;
            return 2;
        }
    }
    if (r26 != 0) {
        if ((u8)r29 == 1) {
            if (r28 != 0) *(u32*)r28 = r26;
            return 2;
        }
    }
    return 0;
}

/* 0x801F221C | size: 0xBC | medium */
u32 fightFloorIsLastActionFightOutPokemon(u32 obj) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 fightOutPokemonCheckFightOut(void);
    extern u32 fightActionCheckValid(void);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    u32 a;
    u32 pkmn;
    u32 i;
    u32 result;
    a = obj;
    result = 1;
    i = 0;
    while ((u16)i < 8) {
        pkmn = fightFloorGetStatus(a, 0, 0x59, i);
        if (pkmn == 0)
            goto _next;
        if ((u8)fightOutPokemonCheckFightOut() == 0)
            goto _next;
        if (pokemonGetStatus(pkmn, 0, 0xfe, 0) == 0)
            goto _next;
        if ((u8)fightActionCheckValid() == 0)
            goto _next;
        if ((s32)pokemonGetStatus(pkmn, 0, 0x112, 0) != 0)
            goto _next;
        result = 0;
        break;
    _next:
        i++;
    }
    return result;
}

/* 0x801F22D8 | size: 0x78 | small */
void fightFloorReplaceFightOutPokemonAttackToDefense(u32 obj) {
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern void fightFloorSetStatus(u32, u32, u32, u32, u32);
    u32 r30;
    u32 r31;
    u32 tmp;
    r30 = obj;
    r31 = fightTargetGetPtrAsNowFightType(0x11, r30);
    tmp = fightTargetGetPtrAsNowFightType(0x12, r30);
    fightFloorSetStatus(r30, 0, 0x36, 0, tmp);
    fightFloorSetStatus(r30, 0, 0x43, 0, r31);
}

#endif

#if defined(FIGHT_FLOOR_801F2350_801F2654)

/* 0x801F2350 | size: 0xE4 | medium */
s32 fightFloorGetEnemyFightSideManyDamageHpRate(u32 unused, u32 r4arg) {
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u8 fightOutPokemonEnemyCheckValid(u32);
    extern u32 fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(u32);
    extern u8 fightOutPokemonCheckValid(u32);
    extern u16 fightOutPokemonEnemyBiosGetInitHp(u32);
    extern u32 fightOutPokemonEnemyBiosGetDamage(u32);
    u32 r31;
    s32 r30;
    u32 r29;
    u16 initHp;
    u32 r28;
    u32 acc;
    s32 score;
    u32 poke;
    if (r4arg == 0) return -1;
    r31 = pokemonGetStatus(r4arg, 0, 0x122, 0);
    r30 = -1;
    r28 = 0;
    while ((u16)r28 < 4) {
        r29 = r31 + (u16)r28 * 12;
        if ((u8)fightOutPokemonEnemyCheckValid(r29) != 0) {
            poke = fightOutPokemonEnemyBiosGetTargetFightOutPokemonPtr(r29);
            if (poke != 0) {
                if ((u8)fightOutPokemonCheckValid(poke) != 0) {
                    initHp = fightOutPokemonEnemyBiosGetInitHp(r29);
                    acc = fightOutPokemonEnemyBiosGetDamage(r29);
                    if (initHp == 0) initHp = 1;
                    score = ((u16)acc * 0x64) / initHp;
                    if (score > r30) r30 = score;
                }
            }
        }
        r28++;
    }
    return r30;
}

/* 0x801F2434 | size: 0x164 | medium */
s32 fightFloorGetEnemyFightSideLittleNokoriHpRate(u32 floor, void* pkmn) {
    extern u32 _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(u32, void*, u32, u32, void*);
    extern u32 fightOutPokemonNowHpWaruValue(void*, u32);
    extern u32 fightOutPokemonMaxHpWaruValue(void*, u32);
    void* ary[8];
    u32 count;
    u16 i;
    void* p;
    u32 now;
    u32 max;
    s32 rate;
    s32 r;
    if (pkmn == NULL) return -1;
    _fightFloorInitFightOutPokemonPtrAry(ary);
    count = _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(floor, ary, 1, 2, pkmn);
    if ((u16)count == 0) return -1;
    for (i = 0; i < (u16)count; i++) {
        p = ary[i];
        if (p != NULL) {
            now = fightOutPokemonNowHpWaruValue(p, 1);
            max = fightOutPokemonMaxHpWaruValue(p, 1);
            if ((u16)max == 0) max = 1;
            rate = (u16)now * 100 / (u16)max;
            break;
        }
    }
    for (i = 0; i < (u16)count; i++) {
        p = ary[i];
        if (p != NULL) {
            now = fightOutPokemonNowHpWaruValue(p, 1);
            max = fightOutPokemonMaxHpWaruValue(p, 1);
            if ((u16)max == 0) max = 1;
            r = (u16)now * 100 / (u16)max;
            if (r < rate) rate = r;
        }
    }
    return rate;
}

/* 0x801F2598 | size: 0xBC | medium */
void* fightFloorGetFightOutPokemonPtrRandom(u32 param_1, u32 param_2, u32 param_3, u32 param_4) {
    extern u32 _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(u32 a, void* arr, u32 b, u32 c, u32 d);
    extern u32 GSrand(void);
    void* arr[8];
    u32 count;
    u16 idx;
    _fightFloorInitFightOutPokemonPtrAry(arr);
    count = _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(param_1, arr, param_2, param_3, param_4);
    if ((u16)count == 0) return 0;
    idx = (u16)GSrand() % (u16)count;
    if (idx >= 8) return 0;
    if (arr[idx] == 0) return 0;
    return arr[idx];
}

#endif

#if defined(FIGHT_FLOOR_801F2654_801F26A8)

/* 0x801F2654 | size: 0x54 | small -- 100% MATCH */
u16 fightFloorGetValidFightOutPokemonCount(u32 param_1, u8 param_2, u32 param_3, u8 param_4) {
    extern u32 _fightFloorGetValidFightOutPokemonCountSub__FPvUsPv(u32, u32, u32*);
    extern u16 fightFloorLoopValidFightOutPokemon(u32 ctx, void* fn, u32* data, u32 d);
    u32 data[4];
    data[0] = param_3;
    data[1] = 0;
    data[2] = param_2;
    data[3] = param_4;
    fightFloorLoopValidFightOutPokemon(param_1, (void*)_fightFloorGetValidFightOutPokemonCountSub__FPvUsPv, data, 0);
    return (u16)data[1];
}

#endif

#if defined(FIGHT_FLOOR_801F26A8_801F27D4)

/* 0x801F26A8 | size: 0x12C | medium */
u32 _fightFloorGetValidFightOutPokemonCountSub__FPvUsPv(u32 p, u16 slot, void* data) {
    extern u8 fightOutPokemonCheckFightOut(void*);
    extern void* fightTargetGetPtrAsNowFightType(u32, void*);
    struct W { void* target; u32 count; s32 mode; u32 flag; };
    void* obj = (void*)p;
    struct W* w = data;
    void* target;
    void* side;
    void* mine;

    target = w->target;
    if ((u8)w->flag == 1 && fightOutPokemonCheckFightOut(obj) == 0)
        return 1;
    if (target == NULL)
        side = NULL;
    else if (w->mode == 1)
        side = fightTargetGetPtrAsNowFightType(2, target);
    else if (w->mode == 2)
        side = fightTargetGetPtrAsNowFightType(3, target);
    else
        side = NULL;
    mine = fightTargetGetPtrAsNowFightType(2, obj);
    if ((w->mode == 1 || w->mode == 2) && side == NULL)
        return 1;
    if (w->mode == 0) {
        if (target != NULL && target == obj)
            return 1;
    } else if (w->mode == 1 || w->mode == 2) {
        if (side != mine)
            return 1;
    } else {
        return 1;
    }
    w->count++;
    return 1;
}

#endif

#if defined(FIGHT_FLOOR_801F27D4_801F2B5C)

/* 0x801F27D4 | size: 0x30 | small */
void fightFloorInitFightOutPokemonPtrAryFightWazaJoutai(u32 param_1) {
    extern void fightFloorLoopValidFightOutPokemon(u32, void*, u32, u32);
    extern u32 _fightFloorInitFightOutPokemonPtrAryFightWazaJoutaiSub__FPvUsPv(u32);

    fightFloorLoopValidFightOutPokemon(param_1, (void*)_fightFloorInitFightOutPokemonPtrAryFightWazaJoutaiSub__FPvUsPv, 0, 0);
}

/* 0x801F2804 | size: 0x34 | small */
u32 _fightFloorInitFightOutPokemonPtrAryFightWazaJoutaiSub__FPvUsPv(u32 param_1) {
    extern void pokemonGetStatus(u32, u32, u32, u32);
    extern void fightWazaInitJoutai(void);

    pokemonGetStatus(param_1, 0, 0xd9, 0);
    fightWazaInitJoutai();
    return 1;
}

/* 0x801F2838 | size: 0x54 | small */
void fightFloorSetNowKaisuuJoutaiDataId(u32 param_1, u32 param_2, u32 param_3) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern void fn_8011AB50(u32 a, u32 b, u32 c);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) == 0x9) {
        fn_8011AB50(param_1, param_2, param_3);
    }
}

/* 0x801F288C | size: 0x54 | small */
u32 fightFloorGetNowKaisuuJoutaiDataId(u32 param_1, u32 param_2) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern u32 statusGetJoutai05(u32 a, u32 b);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) != 0x9) {
        return (u32)-1;
    }
    return statusGetJoutai05(param_1, param_2);
}

/* 0x801F28E0 | size: 0x54 | small */
u32 fightFloorGetKaisuuJoutaiDataId(u32 param_1, u32 param_2) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern u32 fn_8011AE40(u32 a, u32 b);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) != 0x9) {
        return (u32)-1;
    }
    return fn_8011AE40(param_1, param_2);
}

/* 0x801F2934 | size: 0x54 | small */
void fightFloorWriteJoutaiDataId(u32 param_1, u32 param_2, u32 param_3) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern void statusWriteJoutaiDataId(u32 a, u32 b, u32 c);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) == 0x9) {
        statusWriteJoutaiDataId(param_1, param_2, param_3);
    }
}

/* 0x801F2988 | size: 0x54 | small */
u32 fightFloorCheckWriteJoutaiDataId(u32 param_1, u32 param_2) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern u32 statusCheckWriteJoutaiDataId(u32 a, u32 b);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) != 0x9) {
        return 0;
    }
    return statusCheckWriteJoutaiDataId(param_1, param_2);
}

/* 0x801F29DC | size: 0x54 | small */
u32 fightFloorIsJoutaiDataId(u32 param_1, u32 param_2) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern u32 fn_8011B67C(u32 a, u32 b);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) != 0x9) {
        return 0;
    }
    return fn_8011B67C(param_1, param_2);
}

/* 0x801F2A30 | size: 0x4C | small */
void fightFloorInitJoutaiDataId(u32 param_1, u32 param_2) {
    extern u16 joutaiDataBiosGetStatusKind(u32 battle_mon);
    extern void fn_8011B788(u32 a, u32 b);
    if ((u16)joutaiDataBiosGetStatusKind(param_2) == 0x9) {
        fn_8011B788(param_1, param_2);
    }
}

/* 0x801F2A7C | size: 0xE0 | medium */
/* 0x801F2A7C | size: 0xE0 | medium -- best: 98.84%, reg r28/r31 swap wall */
u32 fightFloorGetGcHeroFightTrainerPtr(u32 ctx) {
    extern u32 fightFloorGetStatus(u32 poke, u32 b, u32 field, u32 d);
    extern u32 fightTrainerIsGcHero(u32 mon);
    extern u32 fightSideGetValidFightTrainerPtr(u32, u32);
    u32 pokemon = ctx;
    u32 partyMon;
    u32 moveIndex;
    u32 partyIndex;
    u16 moveCount;
    u32 moveMon;

    fightFloorGetStatus(pokemon, 0, 0x14, 0);
    moveCount = (u16)fightFloorGetStatus(pokemon, 0, 0x16, 0);
    partyIndex = 0;
    while ((u16)partyIndex < 2) {
        partyMon = fightFloorGetStatus(pokemon, 0, 0x35, partyIndex);
        if ((u8)fightSideCheckValid(partyMon) == 0) {
            partyMon = 0;
        }
        if (partyMon != 0) {
            moveIndex = 0;
            while ((u16)moveIndex < moveCount) {
                moveMon = fightSideGetValidFightTrainerPtr(partyMon, moveIndex);
                if (moveMon != 0) {
                    if ((u8)fightTrainerIsGcHero(moveMon) == 1) {
                        return moveMon;
                    }
                }
                moveIndex++;
            }
        }
        partyIndex++;
    }
    return 0;
}

#endif

#if defined(FIGHT_FLOOR_801F2B5C_801F32B0)

/* 0x801F2B5C | size: 0x3E0 | large
 *
 * Same shape as Pokemon XD's fightFloorLoopValidFightTrainer (GXXE01
 * 0x801F311C; TeamOrre/xd-decomp symbols.txt 4989794e, trevor403/xd-asm
 * b1087f18 code/func_FUN_801F311C.s): manual trainers (AI data id 0) first,
 * on threads or directly, then the AI trainers. XD calls
 * fightFloorGetValidFightSidePtr; Colosseum expands it in all three loops.
 * The side loops count with the variable declared after the trainer counter,
 * which is what puts the side counter in retail's register. */
void fightFloorLoopValidFightTrainer(u32 floor, void (*func)(u32, u32, u32), u32 work, u8 useThread) {
    extern void _threadSwitch(void);
    extern u8 GSthreadIsRunning(u32 thread);
    extern void GSthreadClose(u32 thread);
    extern u32 floorGetCurrentContext(void);
    extern u32 GSthreadCreate(u32, u32, u32, u32, u32, void*);
    extern void GSthreadSetArgs(u32 thread, ...);
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    extern void* fightSideGetValidFightTrainerPtr(void*, u16);
    typedef void (*LoopFunc)(void*, u16, u32);
    u32 thread[4];
    u16 trainerNum;
    u16 type;
    u16 trainerNo;
    u16 sideNo;
    u16 count;
    void* sidePtr;
    void* trainer;
    u16 aiId;
    type = fightFloorGetStatus(0, 0, 0x14, 0);
    trainerNum = fightFloorGetStatus(0, 0, 0x16, 0);
    for (sideNo = 0; sideNo < 4; sideNo++)
        thread[sideNo] = 0;
    if (useThread == 1) {
        count = 0;
        for (sideNo = 0; sideNo < 2; sideNo++) {
            sidePtr = _fightFloorGetValidFightSidePtr((void*)floor, sideNo);
            if (sidePtr != NULL) {
                for (trainerNo = 0; trainerNo < trainerNum; trainerNo++) {
                    trainer = fightSideGetValidFightTrainerPtr(sidePtr, trainerNo);
                    if (trainer != NULL) {
                        aiId = fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
                        fightTrainerGetStatus(trainer, 0, 0x4b, 0);
                        if (aiId == 0 && count < 4) {
                            thread[count] = GSthreadCreate(0x12, floorGetCurrentContext(), 0x2000, 1, 0, (void*)func);
                            if (thread != NULL) {
                                GSthreadSetArgs(thread[count], 3, trainer, type, work);
                                count++;
                            }
                        }
                    }
                }
            }
        }
        do {
            _threadSwitch();
            for (count = 0; count < 4; count++) {
                if (thread[count] != 0 && GSthreadIsRunning(thread[count]) == 1)
                    break;
            }
        } while (count < 4);
        for (sideNo = 0; sideNo < 4; sideNo++) {
            if (thread[sideNo] != 0) {
                GSthreadClose(thread[sideNo]);
                thread[sideNo] = 0;
            }
        }
    } else {
        for (sideNo = 0; sideNo < 2; sideNo++) {
            sidePtr = _fightFloorGetValidFightSidePtr((void*)floor, sideNo);
            if (sidePtr != NULL) {
                for (trainerNo = 0; trainerNo < trainerNum; trainerNo++) {
                    trainer = fightSideGetValidFightTrainerPtr(sidePtr, trainerNo);
                    if (trainer != NULL) {
                        aiId = fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
                        fightTrainerGetStatus(trainer, 0, 0x4b, 0);
                        if (aiId == 0)
                            ((LoopFunc)func)(trainer, type, work);
                    }
                }
            }
        }
    }
    for (sideNo = 0; sideNo < 2; sideNo++) {
        sidePtr = _fightFloorGetValidFightSidePtr((void*)floor, sideNo);
        if (sidePtr != NULL) {
            for (trainerNo = 0; trainerNo < trainerNum; trainerNo++) {
                trainer = fightSideGetValidFightTrainerPtr(sidePtr, trainerNo);
                if (trainer != NULL) {
                    aiId = fightTrainerGetStatus(0, (u16)fightTrainerGetStatus(trainer, 0, 0x43, 0), 2, 0);
                    fightTrainerGetStatus(trainer, 0, 0x4b, 0);
                    if (aiId != 0)
                        ((LoopFunc)func)(trainer, type, work);
                }
            }
        }
    }
}


/* 0x801F2F3C | size: 0x138 | medium */
void fightFloorCreateFightPokemonEnemyAryEnemySideAll(u32 param_1) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern void *fightSideGetValidFightTrainerPtr(void*, u32);
    extern u32 fightSideCheckValid(void*);
    extern void *fightTrainerGetValidFightOutPokemonPtr(void*, u32);
    extern void *fightTargetGetPtr(u32, void*, u32);
    extern void _fightFloorCreateFightPokemonEnemyArySub(void*, void*);
    extern u32 fightOutPokemonCheckFightOut(void*);
    u32 count;
    u32 cols;
    u32 rows;
    void *pkmn;
    void *team;
    u32 j;
    u32 k;
    void *row_obj;
    void *cell;
    u32 i;

    pkmn = (void*)param_1;
    count = fightFloorGetStatus(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fightFloorGetStatus(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fightFloorGetStatus(pkmn, 0, 0x18, 0) & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < 2u) {
        team = (void*)fightFloorGetStatus(pkmn, 0, 0x35, i);
        if (!(fightSideCheckValid(team) & 0xFF))
            team = NULL;
        if (team != NULL) {
            j = 0;
            while ((j & 0xFFFF) < cols) {
                row_obj = fightSideGetValidFightTrainerPtr(team, j);
                if (row_obj != NULL) {
                    k = 0;
                    while ((k & 0xFFFF) < rows) {
                        cell = fightTrainerGetValidFightOutPokemonPtr(row_obj, k);
                        if (cell != NULL) {
                            if (fightOutPokemonCheckFightOut(cell) & 0xFF) {
                                void *slot = fightTargetGetPtr(3, cell, count);
                                _fightFloorCreateFightPokemonEnemyArySub(cell, slot);
                            }
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F3074 | size: 0x104 | medium */
void fightFloorRegistFightTrainerEnemyPokemonFightSideAll(void *param_1) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern u32 fightSideCheckValid(void*);
    extern u32 fightSideGetValidFightTrainerPtr(u32, u32);
    extern void *fightTargetGetPtr(u32, void*, u32);
    extern void fightSideRegistFightSideEnemyPokemonFightAll(
        void*, void*, u32, u32, u32);
    void *pkmn;
    u32 team;
    u32 j;
    u32 i;
    u32 count;
    u32 cols;
    u32 rows;
    u32 row_obj;

    pkmn = param_1;
    count = fightFloorGetStatus(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fightFloorGetStatus(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fightFloorGetStatus(pkmn, 0, 0x18, 0) & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < 2) {
        team = fightFloorGetStatus(pkmn, 0, 0x35, i);
        if (!(fightSideCheckValid((void *)team) & 0xFF)) {
            team = 0;
        }
        if (team != 0) {
            j = 0;
            while ((j & 0xFFFF) < cols) {
                row_obj = fightSideGetValidFightTrainerPtr(team, j);
                if (row_obj != 0) {
                    fightSideRegistFightSideEnemyPokemonFightAll(
                        fightTargetGetPtr(3, (void *)row_obj, count), (void *)row_obj, count,
                        cols, rows);
                }
                j++;
            }
        }
        i++;
    }
}

/* 0x801F3178 | size: 0x138 | medium */
void fightFloorSetMeetEnemyFightPokemonEnemySideAll(u32 param_1) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern void *fightSideGetValidFightTrainerPtr(void*, u32);
    extern u32 fightSideCheckValid(void*);
    extern void *fightTrainerGetValidFightOutPokemonPtr(void*, u32);
    extern void *fightTargetGetPtr(u32, void*, u32);
    extern void fightOutPokemonSetMeetEnemyFightPokemonEnemySideAll(void*, void*);
    extern u32 fightOutPokemonCheckFightOut(void*);
    u32 count;
    u32 cols;
    u32 rows;
    void *pkmn;
    void *team;
    u32 j;
    u32 k;
    void *row_obj;
    void *cell;
    u32 i;

    pkmn = (void*)param_1;
    count = fightFloorGetStatus(pkmn, 0, 0x14, 0) & 0xFFFF;
    cols  = fightFloorGetStatus(pkmn, 0, 0x16, 0) & 0xFFFF;
    rows  = fightFloorGetStatus(pkmn, 0, 0x18, 0) & 0xFFFF;
    i = 0;
    while ((i & 0xFFFF) < 2u) {
        team = (void*)fightFloorGetStatus(pkmn, 0, 0x35, i);
        if (!(fightSideCheckValid(team) & 0xFF))
            team = NULL;
        if (team != NULL) {
            j = 0;
            while ((j & 0xFFFF) < cols) {
                row_obj = fightSideGetValidFightTrainerPtr(team, j);
                if (row_obj != NULL) {
                    k = 0;
                    while ((k & 0xFFFF) < rows) {
                        cell = fightTrainerGetValidFightOutPokemonPtr(row_obj, k);
                        if (cell != NULL) {
                            if (fightOutPokemonCheckFightOut(cell) & 0xFF) {
                                void *slot = fightTargetGetPtr(3, cell, count);
                                fightOutPokemonSetMeetEnemyFightPokemonEnemySideAll(cell, slot);
                            }
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
}

#endif

#if defined(FIGHT_FLOOR_801F32B0_801F32EC)

/* 0x801F32B0 | size: 0x3C | small */
u8 fightFloorIsIrekaeFightOutPokemon(u32 param_1) {
    extern void fightFloorLoopValidFightTrainer(u32, void*, void*, u32);
    extern void _fightFloorIsIrekaeFightOutPokemonSub__FPvUsPv(void);
    u8 local;

    local = 0;
    fightFloorLoopValidFightTrainer(param_1, (void*)_fightFloorIsIrekaeFightOutPokemonSub__FPvUsPv, &local, 0);
    return local;
}

#endif

#if defined(FIGHT_FLOOR_801F32EC_801F33E8)

/* 0x801F32EC | size: 0xFC | medium */
u32 _fightFloorIsIrekaeFightOutPokemonSub__FPvUsPv(void *param_1, u32 r4_unused, u8 *out_buf) {
    void *trainer;
    extern u32 fightTrainerCheckValid(void*);
    extern void *fightTrainerGetIrekaeFightPokemon(void*, u16*);
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern void *fightTrainerGetStatus(void*, u32, u32, u32);
    extern u32 fightOutPokemonCheckFightOut(void*);
    extern u32 fightTrainerIsGcHero(void*);
    u32 i;
    void *pkmn;
    u32 count;
    u8 *outptr;
    u16 tmp;

    trainer = pkmn = param_1;
    if (!(fightTrainerCheckValid(trainer) & 0xFF))
        return 1;
    tmp = 0;
    if (!fightTrainerGetIrekaeFightPokemon(pkmn, &tmp))
        return 1;
    count = fightFloorGetStatus(0, 0, 0x18, 0) & 0xFFFF;
    outptr = out_buf;
    i = 0;
    while ((i & 0xFFFF) < count) {
        void *entry = fightTrainerGetStatus(pkmn, 0, 0x46, i);
        if (entry != NULL) {
            if (fightOutPokemonCheckFightOut(entry) & 0xFF) {
                /* nonzero: skip */
            } else if (outptr != NULL) {
                if (*outptr != 2) {
                    if ((fightTrainerIsGcHero(trainer) & 0xFF) == 1) {
                        *outptr = 2;
                    } else {
                        *outptr = 1;
                    }
                }
            }
        }
        i++;
    }
    return 1;
}

#endif

#if defined(FIGHT_FLOOR_801F33E8_801F37B0)

/* 0x801F33E8 | size: 0x48 | small */
u16 fightFloorCheckFightOutPokemonPtrAryJoutaiDataId(void *param_1, u16 param_2) {
    extern u32 fightFloorLoopValidFightOutPokemon(void*, void*, void*, u8);
    extern u32 _fightFloorCheckFightOutPokemonPtrAryJoutaiDataIdSub__FPvUsPv(void*, u32, void*);
    u32 buf[2];

    buf[0] = param_2;
    buf[1] = 0;
    fightFloorLoopValidFightOutPokemon(param_1, (void*)_fightFloorCheckFightOutPokemonPtrAryJoutaiDataIdSub__FPvUsPv, buf, 0);
    return (u16)buf[1];
}

/* 0x801F3430 | size: 0x6C | small */
u32 _fightFloorCheckFightOutPokemonPtrAryJoutaiDataIdSub__FPvUsPv(void *param_1, u32 r4_unused, u32 *buf) {
    extern u32 fightOutPokemonCheckFightOut(void*);
    extern u32 fightOutPokemonIsPokemonJoutaiDataId(void*, u16);
    void *obj;

    if (!(fightOutPokemonCheckFightOut(obj = param_1) & 0xFF))
        return 1;
    if ((fightOutPokemonIsPokemonJoutaiDataId(obj, (u16)buf[0]) & 0xFF) == 1)
        buf[1]++;
    return 1;
}

/* 0x801F349C | size: 0x50 | small */
void *fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(void *param_1, u16 param_2, void *param_3, u8 param_4, u32 param_5) {
    extern u32 fightFloorLoopValidFightOutPokemon(void*, void*, void*, void*);
    extern void _fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv(void*, u32, void*);
    u32 buf[4];

    buf[0] = param_2;
    buf[1] = 0;
    buf[2] = param_4;
    buf[3] = param_5;
    fightFloorLoopValidFightOutPokemon(param_1, (void*)_fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv, buf, param_3);
    return (void*)buf[1];
}

/* 0x801F34EC | size: 0x138 | medium */
u32 _fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirstSub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fightOutPokemonCheckFightOut();
    extern void* fightTargetGetPtrAsNowFightType(u32, void*);
    extern u16 fightOutPokemonGetTokuseiDataId(void*);
    struct { u32 tokuseiId; void* result; s32 side; void* pokemon; }* s = data;
    void* new_var;
    void* pokemon;
    void* sideRef;
    void* objRef;

    pokemon = s->pokemon;
    if ((u8)fightOutPokemonCheckFightOut() == 0)
        return 1;
    if (pokemon == 0) {
        sideRef = 0;
    } else if (s->side == 1) {
        sideRef = fightTargetGetPtrAsNowFightType(2, pokemon);
    } else if (s->side == 2) {
        sideRef = fightTargetGetPtrAsNowFightType(3, pokemon);
    } else {
        sideRef = 0;
    }
    objRef = fightTargetGetPtrAsNowFightType(2, new_var = obj);
    if (s->side == 1 || s->side == 2) {
        if (sideRef == 0)
            return 1;
    }
    if (s->side == 0) {
        if (pokemon == 0) goto _checkId;
        if (pokemon != new_var) goto _checkId;
        return 1;
    } else if (s->side == 1 || s->side == 2) {
        if (sideRef == objRef) goto _checkId;
        return 1;
    } else {
        return 1;
    }
_checkId:
    if ((u16)s->tokuseiId == (u16)fightOutPokemonGetTokuseiDataId(new_var)) {
        s->result = new_var;
        return 0;
    }
    return 1;
}

/* 0x801F3624 | size: 0x54 | small */
u16 fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataId(void *param_1, u16 param_2, u8 param_3, u32 param_4) {
    extern u32 fightFloorLoopValidFightOutPokemon(void*, void*, void*, u32);
    extern void _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(void*, u32, void*);
    u32 buf[4];

    buf[0] = param_2;
    buf[1] = 0;
    buf[2] = param_3;
    buf[3] = param_4;
    fightFloorLoopValidFightOutPokemon(param_1, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, buf, 0);
    return (u16)buf[1];
}

/* 0x801F3678 | size: 0x138 | medium */
u32 _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(void* obj, u32 slot, void* data) {
    extern u32 fightOutPokemonCheckFightOut();
    extern void* fightTargetGetPtrAsNowFightType(u32, void*);
    extern u16 fightOutPokemonGetTokuseiDataId(void*);
    struct { u32 tokuseiId; u32 count; s32 side; void* pokemon; }* s = data;
    void* new_var;
    void* pokemon;
    void* sideRef;
    unsigned char pad;
    void* objRef;

    pokemon = s->pokemon;
    if ((u8)fightOutPokemonCheckFightOut() == 0)
        return 1;
    if (pokemon == 0) {
        sideRef = 0;
    } else if (s->side == 1) {
        sideRef = fightTargetGetPtrAsNowFightType(2, pokemon);
    } else if (s->side == 2) {
        sideRef = fightTargetGetPtrAsNowFightType(3, pokemon);
    } else {
        sideRef = 0;
    }
    objRef = fightTargetGetPtrAsNowFightType(2, new_var = obj);
    if (s->side == 1 || s->side == 2) {
        if (sideRef == 0)
            return 1;
    }
    if (s->side == 0) {
        if (pokemon == 0) goto _checkId;
        if (pokemon != new_var) goto _checkId;
        return 1;
    } else if (s->side == 1 || s->side == 2) {
        if (sideRef == objRef) goto _checkId;
        return 1;
    } else {
        return 1;
    }
_checkId:
    if ((u16)s->tokuseiId == (u16)fightOutPokemonGetTokuseiDataId(new_var)) {
        s->count = s->count + 1;
    }
    return 1;
}

#endif

/* The valid side / trainer / fight-out Pokemon chain, shared by
 * fightFloorLoopValidFightOutPokemon (0x801F37B0) and the search in case
 * 0x5D of fightFloorGetStatus (0x801F54A4). */
#if defined(FIGHT_FLOOR_801F37B0_801F3B24) || defined(FIGHT_FLOOR_801F54A4_801F61BC)

static inline void* fightFloorLoopValidFightOutPokemonSub(void* floor, u16 side, u16 trainer, u16 pokemon) {
    extern void *fightSideGetValidFightTrainerPtr(void*, u16);
    extern void *fightTrainerGetValidFightOutPokemonPtr(void*, u16);
    void* p;

    p = _fightFloorGetValidFightSidePtr(floor, side);
    if (p == NULL)
        return NULL;
    p = fightSideGetValidFightTrainerPtr(p, trainer);
    if (p == NULL)
        return NULL;
    p = fightTrainerGetValidFightOutPokemonPtr(p, pokemon);
    if (p == NULL)
        return NULL;
    return p;
}
#endif

#if defined(FIGHT_FLOOR_801F37B0_801F3B24)

/*
 * Pokemon XD's _fightFloorLoopValidFightOutPokemonSub__FP11FIGHT_FLOORUsUsUs
 * (GXXE01 0x801F3D40, size 0x6C, scope:local; TeamOrre/xd-decomp
 * symbols.txt 4989794e, body trevor403/xd-asm b1087f18
 * code/func_FUN_801F3D40.s): side -> trainer -> Pokemon lookup, NULL at the
 * first missing level. XD's fightFloorLoopValidFightOutPokemon (0x801F3BEC)
 * calls it once per (pokemon, trainer, side) in the same triple loop, with
 * the loop counters as its u16 arguments; Colosseum expands it there, with
 * its guards kept (retail tests the side pointer and re-tests the Pokemon
 * pointer after the call).
 *
 * XD's Sub calls fightFloorGetValidFightSidePtr (GXXE01 0x801F47F0), and so
 * does this Sub: Colosseum's own fightFloorGetValidFightSidePtr (0x801F47B4,
 * exact in fight_floor_exact_801F4460.c) is expanded inside the Sub's
 * expansion. The copy above, _fightFloorGetValidFightSidePtr, is a
 * compile-only copy of that same-TU function (the same body compiled alone
 * gives retail's 0x801F47B4 bytes exactly); it carries the underscore so the
 * unit that also compiles the real function (fight_floor_exact_801F4460.c,
 * 0x801F4460-0x801F61BC since fightFloorGetStatus joined it) holds both.
 * Its expansion keeps the result in the Sub's pointer register:
 * `mr r21,r3; bl fightSideCheckValid; clrlwi.; bne; li r21,0`, followed by
 * the Sub's own `cmplwi r21,0` guard. fightFloorLoopValidFightTrainer
 * (0x801F2B5C carve) expands the same function three more times, with the
 * same shape (0x801F2BFC, 0x801F2D9C, 0x801F2E6C). With it, the Sub makes
 * XD's three calls in XD's order. Sister-title clause,
 * docs/CAMPAIGN_OPERATIONS.md.
 *
 * The callback takes count as a u16, like the counter it is given. When the
 * callback was typed (void*, u32, void*), MWCC hoisted the u16->u32
 * conversion out of the triple loop into a copy (mr r31,r22) that retail
 * does not have.
 */

/* 0x801F37B0 | size: 0x1D4 | medium */
u8 fightFloorLoopValidFightOutPokemon(void *floor, void *callback, void *buf, u8 flag) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    extern u8 fightOutPokemonCheckValid(void*);
    typedef u8 (*cbk_t)(void*, u16, void*);
    u16 trainer, pokemon, side;
    u16 count, trainers, pokemons;
    u8 ret;
    u16 i;
    void *p;

    ret = 1;
    count = fightFloorGetStatus(floor, 0, 0x14, 0);
    trainers = fightFloorGetStatus(floor, 0, 0x16, 0);
    pokemons = fightFloorGetStatus(floor, 0, 0x18, 0);
    if (flag == 1) {
        for (i = 0; i < 8; i++) {
            p = (void*)fightFloorGetStatus(floor, 0, 0x59, i);
            if (p != NULL && fightOutPokemonCheckValid(p)) {
                if (!((cbk_t)callback)(p, count, buf)) {
                    ret = 0;
                    break;
                }
            }
        }
    } else {
        for (pokemon = 0; pokemon < pokemons; pokemon++) {
            for (trainer = 0; trainer < trainers; trainer++) {
                for (side = 0; side < 2; side++) {
                    p = fightFloorLoopValidFightOutPokemonSub(floor, side, trainer, pokemon);
                    if (p != NULL) {
                        if (!((cbk_t)callback)(p, count, buf)) {
                            ret = 0;
                            goto end;
                        }
                    }
                }
            }
        }
    }
end:
    return ret;
}

/* 0x801F3984 | size: 0x1A0 | medium */
u8 fightFloorSetFightResultId(void* floor, u16 id) {
    extern u16 std103_battleGetResult(void);
    extern void battleSetResult(u16);
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    u16 now;
    u8 kind;

    now = std103_battleGetResult();
    kind = fightFloorGetStatus(floor, 0, 0x23, 0);
    if (now == id) {
        return 0;
    }
    if (now == 1) {
        return 1;
    }
    if (id == 0 || id == 1) {
        battleSetResult(id);
        return 1;
    }
    if (kind == 0) {
        if (now == 3 || now == 5) {
            return 0;
        }
        if (id == 7) {
            battleSetResult(3);
            return 1;
        }
        if (id == 6) {
            battleSetResult(5);
            return 1;
        }
    } else {
        if (now == 7 || now == 6) {
            return 0;
        }
        if (now == 3 && id == 2) {
            battleSetResult(7);
            return 1;
        }
        if (now == 2 && id == 3) {
            battleSetResult(7);
            return 1;
        }
        if (now == 4 && id == 5) {
            battleSetResult(6);
            return 1;
        }
        if (now == 5 && id == 4) {
            battleSetResult(6);
            return 1;
        }
    }
    battleSetResult(id);
    return 1;
}

#endif

#if defined(FIGHT_FLOOR_801F3B24_801F3BB4)

/* 0x801F3B24 | size: 0x90 | medium */
void fightFloorSortFightOutPokemonPtrAry(void* obj, u32 param2) {
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern void fightFloorSortFightOutPokemonPtrArySub(void*, u32*, u16, u32);
    extern void fightFloorSetStatus(void*, int, int, int, u32*);
    u32 buf[8];
    u32 i;
    i = 0;
    while ((i & 0xFFFF) < 8) {
        *(u32*)((u8*)buf + ((u16)i << 2)) = fightFloorGetStatus(obj, 0, 0x59, (int)i);
        i++;
    }
    fightFloorSortFightOutPokemonPtrArySub(obj, buf, 8, param2);
    fightFloorSetStatus(obj, 0, 0x5a, 0, buf);
}

#endif

#if defined(FIGHT_FLOOR_801F3BB4_801F4220)

/* 0x801F3BB4 | size: 0x134 | medium */
void fightFloorSortFightOutPokemonPtrArySub(void* floor, u32* ary_, u16 num, u8 usePri) {
    void** ary = (void**)ary_;
    void* tmp;
    s32 pri2;
    s32 pri1;
    u16 i;
    u16 j;

    for (i = 0; i < num - 1; i++) {
        for (j = i + 1; j < num; j++) {
            if (ary[i] == NULL && ary[j] == NULL)
                continue;
            if (ary[i] == NULL) {
                tmp = ary[i];
                ary[i] = ary[j];
                ary[j] = tmp;
            } else if (ary[j] != NULL) {
                if (usePri == 0) {
                    pri1 = 0;
                    pri2 = 0;
                } else {
                    pri1 = fightOutPokemonGetFightActionPri((struct Pokemon*)ary[i]);
                    pri2 = fightOutPokemonGetFightActionPri((struct Pokemon*)ary[j]);
                }
                if ((s8)pri1 < (s8)pri2)
                    continue;
                if ((s8)pri1 > (s8)pri2) {
                    tmp = ary[i];
                    ary[i] = ary[j];
                    ary[j] = tmp;
                } else if ((u8)fightFloorCmpfightOutPokemonNimbleness(floor, ary[i], ary[j], usePri) == 0) {
                    tmp = ary[i];
                    ary[i] = ary[j];
                    ary[j] = tmp;
                }
            }
        }
    }
}

/*
 * Pokemon XD's fightFloorCmpfightOutPokemonNimbleness (GXXE01 0x801F4300;
 * TeamOrre/xd-decomp symbols.txt 4989794e, body trevor403/xd-asm b1087f18
 * code/func_FUN_801F4300.s) calls fightFloorGetNowTenkouDataId(floor, 1),
 * the badge-check and first-attack-random getters, then
 * fightFloorGetFightOutPokemonPtrToHeroPtr(floor, p) and
 * fightOutPokemonGetNowNimbleness for each Pokemon. XD's ToHeroPtr
 * (0x801F4478) calls fightFloorGetFightOutPokemonPtrToFightTrainerPtr, which
 * looks the trainer up through fightFloorGetFightPokemonPtrToFightTrainerPtr.
 * Colosseum expands all four here. The copies below are compile-only copies
 * of this TU's own functions, which are defined after this one:
 * fightFloorGetNowTenkouDataId (0x801F453C),
 * fightFloorGetFightPokemonPtrToFightTrainerPtr (0x801F4460),
 * fightFloorGetFightOutPokemonPtrToFightTrainerPtr (0x801F4354) and
 * fightFloorGetFightOutPokemonPtrToHeroPtr (0x801F4220). Colosseum reads the
 * badge-check flag and the first-attack random as fightFloorGetStatus fields
 * 0x1D and 0x5B.
 */
/* RULE-EXCEPTION(user-approved): compile-only inline copy of a real fight_floor function defined later in this file — see docs/RULE_EXCEPTIONS.md */
static inline s32 _fightFloorGetNowTenkouDataId(void* floor, u8 checkTokusei) {
    extern u8 fightFloorLoopValidFightOutPokemon(void*, void*, void*, u8);
    extern s32 fightFloorGetStatus(void*, u32, u32, u32);
    u32 buf2[4];
    u32 buf1[4];

    if (checkTokusei == 1) {
        buf1[0] = 13;
        buf1[1] = 0;
        buf1[2] = 0;
        buf1[3] = 0;
        fightFloorLoopValidFightOutPokemon(floor, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, buf1, 0);
        if ((u16)buf1[1] != 0)
            return 0;
        buf2[0] = 0x4d;
        buf2[1] = 0;
        buf2[2] = 0;
        buf2[3] = 0;
        fightFloorLoopValidFightOutPokemon(floor, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, buf2, 0);
        if ((u16)buf2[1] != 0)
            return 0;
    }
    if (fightFloorGetStatus(floor, 0, 0xa, 0x4e) == 1) return 0;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x4f) == 1) return 1;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x50) == 1) return 2;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x51) == 1) return 3;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x52) == 1) return 4;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x53) == 1) return 1;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x54) == 1) return 2;
    if (fightFloorGetStatus(floor, 0, 0xa, 0x55) == 1) return 3;
    return 0;
}

/* RULE-EXCEPTION(user-approved): compile-only inline copy of a real fight_floor function defined later in this file — see docs/RULE_EXCEPTIONS.md */
static inline void* _fightFloorGetFightPokemonPtrToFightTrainerPtr(void* floor, void* pokemon) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern u32 fightSideGetStatus(void*, u32, u32, u32);
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    void* side;
    u32 j;
    u32 k;
    u32 i;
    void* trainer;
    void* p;

    for (i = 0; (u16)i < 2; i++) {
        side = (void*)fightFloorGetStatus(floor, 0, 0x35, i);
        if (side != NULL) {
            for (j = 0; (u16)j < 2; j++) {
                trainer = (void*)fightSideGetStatus(side, 0, 7, j);
                if (trainer != NULL) {
                    for (k = 0; (u16)k < 6; k++) {
                        p = (void*)fightTrainerGetStatus(trainer, 0, 0x45, k);
                        if (p != NULL && pokemon == p)
                            return trainer;
                    }
                }
            }
        }
    }
    return NULL;
}

/* RULE-EXCEPTION(user-approved): compile-only inline copy of a real fight_floor function defined later in this file — see docs/RULE_EXCEPTIONS.md */
static inline void* _fightFloorGetFightOutPokemonPtrToFightTrainerPtr(void* floor, void* outPokemon) {
    extern u32 pokemonGetStatus(void*, u32, u32, u32);
    void* pokemon;
    void* trainer;

    pokemon = (void*)pokemonGetStatus(outPokemon, 0, 0xd5, 0);
    if (pokemon == NULL)
        return NULL;
    trainer = _fightFloorGetFightPokemonPtrToFightTrainerPtr(floor, pokemon);
    if (trainer == NULL)
        return NULL;
    return trainer;
}

/* RULE-EXCEPTION(user-approved): compile-only inline copy of a real fight_floor function defined later in this file — see docs/RULE_EXCEPTIONS.md */
static inline void* _fightFloorGetFightOutPokemonPtrToHeroPtr(void* floor, void* outPokemon) {
    extern u32 fightTrainerGetStatus(void*, u32, u32, u32);
    void* trainer;
    void* hero;

    trainer = _fightFloorGetFightOutPokemonPtrToFightTrainerPtr(floor, outPokemon);
    if (trainer == NULL)
        return NULL;
    hero = (void*)fightTrainerGetStatus(trainer, 0, 0x44, 0);
    if (hero == NULL)
        return NULL;
    return hero;
}

/* 0x801F3CE8 | size: 0x538 | large */
s32 fightFloorCmpfightOutPokemonNimbleness(void* floor, void* p1, void* p2, u8 usePri) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    extern u32 fightOutPokemonGetNowNimbleness(void*, u8, s32, u16, void*);
    extern u32 fightOutPokemonGetCmpNimblenessWazaDataId(void*);
    extern s32 wazaGetStatus(u32, u32, u32, u32);
    extern s32 GSrand(void);
    s32 tenkou;
    u8 badge;
    u16 rnd;
    u32 nimble1;
    u32 nimble2;
    u32 waza1;
    u32 waza2;
    s8 pri1;
    s8 pri2;

    if (p1 == NULL || p2 == NULL)
        return 1;
    tenkou = _fightFloorGetNowTenkouDataId(floor, 1);
    badge = fightFloorGetStatus(floor, 0, 0x1d, 0);
    rnd = fightFloorGetStatus(floor, 0, 0x5b, 0);
    nimble1 = fightOutPokemonGetNowNimbleness(p1, badge, tenkou, rnd, _fightFloorGetFightOutPokemonPtrToHeroPtr(floor, p1));
    nimble2 = fightOutPokemonGetNowNimbleness(p2, badge, tenkou, rnd, _fightFloorGetFightOutPokemonPtrToHeroPtr(floor, p2));
    if (usePri == 0) {
        waza1 = 0;
        waza2 = 0;
    } else {
        waza1 = fightOutPokemonGetCmpNimblenessWazaDataId(p1);
        waza2 = fightOutPokemonGetCmpNimblenessWazaDataId(p2);
    }
    pri1 = wazaGetStatus(0, waza1, 4, 0);
    pri2 = wazaGetStatus(0, waza2, 4, 0);
    if (pri1 != 0 || pri2 != 0) {
        if (pri1 > pri2)
            return 1;
        if (pri1 < pri2)
            return 0;
    }
    if (nimble1 > nimble2)
        return 1;
    if (nimble1 < nimble2)
        return 0;
    if ((GSrand() & 1) == 0)
        goto lose;
    return 1;
lose:
    return 0;
}

#endif

#if defined(FIGHT_FLOOR_801F4220_801F4354)

/* 0x801F4220 | size: 0x134 | medium */
void* fightFloorGetFightOutPokemonPtrToHeroPtr(void* obj, void* search_val) {
    extern u32 pokemonGetStatus(void*, int, int, int);
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern u32 fightSideGetStatus(void*, int, int, int);
    extern u32 fightTrainerGetStatus(void*, int, int, int);
    u32 target, side, i, k, j, team, val;
    target = pokemonGetStatus(search_val, 0, 0xd5, 0);
    if (!target) {
        side = 0;
        goto check;
    }
    side = 0; i = 0; k = 0; j = 0; team = 0;
    while ((i & 0xFFFF) < 2) {
        team = fightFloorGetStatus(obj, 0, 0x35, (int)i);
        if (team) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                side = fightSideGetStatus((void*)team, 0, 7, (int)j);
                if (side) {
                    k = 0;
                    while ((k & 0xFFFF) < 6) {
                        val = fightTrainerGetStatus((void*)side, 0, 0x45, (int)k);
                        if (val != 0 && target == val) {
                            if (!i && !i && !i) {
                            }
                            goto found;
                        }
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    side = 0;
found:
    if (!side)
        side = 0;
check:
    if (!side)
        return 0;
    {
        u32 r = fightTrainerGetStatus((void*)side, 0, 0x44, 0);
        if (r == 0) r = 0;
        return (void*)r;
    }
}

#endif

#if defined(FIGHT_FLOOR_801F4354_801F4460)

/* The trainer search is fightFloorGetFightPokemonPtrToFightTrainerPtr's
 * body (0x801F4460, defined after this function), expanded in place. */
/* RULE-EXCEPTION(user-approved): single-use inline helper duplicating a real
 * function's body -- see docs/RULE_EXCEPTIONS.md. */
static inline void* fightFloorSearchFightTrainerPtr(void* obj, void* search_val) {
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern u32 fightSideGetStatus(void*, int, int, int);
    extern u32 fightTrainerGetStatus(void*, int, int, int);
    u32 team, j, k, i, side, val;
    team = 0; j = 0; k = 0; i = 0;
    while ((i & 0xFFFF) < 2) {
        team = fightFloorGetStatus(obj, 0, 0x35, (int)i);
        if (team) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                side = fightSideGetStatus((void*)team, 0, 7, (int)j);
                if (side) {
                    k = 0;
                    while ((k & 0xFFFF) < 6) {
                        val = fightTrainerGetStatus((void*)side, 0, 0x45, (int)k);
                        if (val != 0 && search_val == (void*)val)
                            return (void*)side;
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    return 0;
}

/* 0x801F4354 | size: 0x10C | medium */
void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr(void* obj, void* search_val) {
    extern u32 pokemonGetStatus(void*, int, int, int);
    void* pokemon;
    void* trainer;

    pokemon = (void*)pokemonGetStatus(search_val, 0, 0xd5, 0);
    if (pokemon == 0)
        return 0;
    trainer = fightFloorSearchFightTrainerPtr(obj, pokemon);
    if (trainer == 0)
        return 0;
    return trainer;
}

#endif

#if defined(FIGHT_FLOOR_801F4460_801F54A4)

/* 0x801F4460 | size: 0xDC | medium */
void* fightFloorGetFightPokemonPtrToFightTrainerPtr(void* obj, void* search_val) {
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern u32 fightSideGetStatus(void*, int, int, int);
    extern u32 fightTrainerGetStatus(void*, int, int, int);
    u32 team, j, k, i, side, val;
    team = 0; j = 0; k = 0; i = 0;
    while ((i & 0xFFFF) < 2) {
        team = fightFloorGetStatus(obj, 0, 0x35, (int)i);
        if (team) {
            j = 0;
            while ((j & 0xFFFF) < 2) {
                side = fightSideGetStatus((void*)team, 0, 7, (int)j);
                if (side) {
                    k = 0;
                    while ((k & 0xFFFF) < 6) {
                        val = fightTrainerGetStatus((void*)side, 0, 0x45, (int)k);
                        if (val != 0 && search_val == (void*)val)
                            return (void*)side;
                        k++;
                    }
                }
                j++;
            }
        }
        i++;
    }
    return 0;
}

/* 0x801F453C | size: 0x1DC */
s32 fightFloorGetNowTenkouDataId(void* param_1, u32 param_2) {
    extern u32 fightFloorLoopValidFightOutPokemon(void*, void*, void*, u32);
    extern void _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(void*, u32, void*);
    extern s32 fightFloorGetStatus(void*, u32, u32, u32);
    void* pkmn;
    u32 buf1[4];
    u32 buf2[4];

    pkmn = param_1;
    if ((u8)param_2 == 1) {
        buf1[0] = 13;
        buf1[1] = 0;
        buf1[2] = 0;
        buf1[3] = 0;
        fightFloorLoopValidFightOutPokemon(pkmn, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, buf1, 0);
        if ((u16)buf1[1] != 0)
            return 0;

        buf2[0] = 0x4d;
        buf2[1] = 0;
        buf2[2] = 0;
        buf2[3] = 0;
        fightFloorLoopValidFightOutPokemon(pkmn, (void*)_fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, buf2, 0);
        if ((u16)buf2[1] != 0)
            return 0;
    }
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x4e) == 1) return 0;
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x4f) == 1) return 1;
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x50) == 1) return 2;
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x51) == 1) return 3;
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x52) == 1) return 4;
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x53) == 1) return 1;
    if (fightFloorGetStatus(pkmn, 0, 0xa, 0x54) == 1) return 2;
    return (fightFloorGetStatus(pkmn, 0, 0xa, 0x55) == 1) ? 3 : 0;
}

/* 0x801F4718 | size: 0x9C | medium */
void* fightFloorCreateFightOutPokemonPtrAry(void* obj) {
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern void _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(void*, void*, int, int, int);
    u32* tmp;
    u32 subfield;
    u32 i, fill;
    i = fightFloorGetStatus(obj, 0, 0x5a, 0);
    tmp = (u32*)i;
    if (tmp) {
        fill = 0; i = fill;
        while ((i & 0xFFFF) < 8) {
            *(u32*)((u8*)tmp + ((u16)i << 2)) = fill;
            i++;
        }
    }
    i = fill;
    subfield = fightFloorGetStatus(obj, 0, 0x5a, 0);
    if (subfield == 0)
        return 0;
    _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(obj, (void*)subfield, 0, 0, 0);
}

/* 0x801F47B4 | size: 0x50 | small */
void* fightFloorGetValidFightSidePtr(void* a, u32 b) {
    extern u32 fightFloorGetStatus(void*, int, int, u32);
    void *ret;
    u32 result = fightFloorGetStatus(a, 0, 0x35, b);
    if ((u8)fightSideCheckValid(result) != 0) {
        ret = 0;
        goto ret_result;
    }
    ret = 0;
    goto end;
ret_result:
    ret = (void*)result;
end:
    return ret;
}

/* 0x801F4804 | size: 0x5C | small */
s16 fightFloorGetFightPokemonEntryCntInc(void* obj) {
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern void fightFloorSetStatus(void*, int, int, int, s16);
    s16 val = (s16)fightFloorGetStatus(obj, 0, 0x58, 0);
    fightFloorSetStatus(obj, 0, 0x58, 0, (s16)(val + 1));
    return val;
}

/* 0x801F4860 | size: 0x260 | large */
void fightFloorInit(void* obj, u32 param2) {
    extern u32 fightFloorGetStatus(void*, int, int, int);
    extern void fn_8011B950(u32, u32);
    extern void fightFloorSetStatus(void*, int, int, int, u16);
    extern void fightSideInitAry(u32, u32);
    u32 tmp;
    u32 fill;
    u32* zarr;
    CopyBuf buf;
    u16* tbl;
    u32 i;
    if (!obj) return;
    tmp = fightFloorGetStatus(obj, 0, 9, 0);
    fn_8011B950(tmp, 1);
    fightFloorSetStatus(obj, 0, 0xc, 0, 0);
    fightFloorSetStatus(obj, 0, 0xd, 0, 0);
    tmp = fightFloorGetStatus(obj, 0, 0x35, 0);
    fightSideInitAry(tmp, 2);
    buf = lbl_80279C28;
    tbl = (u16*)&buf;
    i = 0;
    while ((i & 0xFFFF) < 0xd) {
        fightFloorSetStatus(obj, 0, tbl[(u16)i], 0, 0);
        i++;
    }
    fightFloorSetStatus(obj, 0, 0x50, 0, 0);
    fightFloorSetStatus(obj, 0, 0x51, 0, 0);
    fightFloorSetStatus(obj, 0, 0x52, 0, 0);
    fightFloorSetStatus(obj, 0, 0x53, 0, 0);
    fightFloorSetStatus(obj, 0, 0x54, 0, 0);
    fightFloorSetStatus(obj, 0, 0x55, 0, 0);
    fightFloorSetStatus(obj, 0, 0x56, 0, 0);
    fightFloorSetStatus(obj, 0, 0x57, 0, 0);
    fightFloorSetStatus(obj, 0, 0x58, 0, 0);
    zarr = (u32*)fightFloorGetStatus(obj, 0, 0x5a, 0);
    if (zarr) {
        fill = 0; i = fill;
        while ((i & 0xFFFF) < 8) {
            *(u32*)((u8*)zarr + ((u16)i << 2)) = fill;
            i++;
        }
    }
    fightFloorSetStatus(obj, 0, 0x5b, 0, 0);
    fightFloorSetStatus(obj, 0, 0xd, 0, param2);
    fightFloorSetStatus(obj, 0, 0xc, 0, 1);
}

/* 0x801F4AC0 | size: 0x154 | medium */
void fightFloorInitFightTarget(void* obj) {
    extern void fightFloorSetStatus(void*, int, int, int, int);
    CopyBuf buf;
    u16* tbl;
    u32 i;
    buf = lbl_80279C28;
    tbl = (u16*)&buf;
    i = 0;
    while ((i & 0xFFFF) < 0xd) {
        fightFloorSetStatus(obj, 0, tbl[(u16)i], 0, 0);
        i++;
    }
    fightFloorSetStatus(obj, 0, 0x50, 0, 0);
    fightFloorSetStatus(obj, 0, 0x51, 0, 0);
    fightFloorSetStatus(obj, 0, 0x52, 0, 0);
    fightFloorSetStatus(obj, 0, 0x53, 0, 0);
    fightFloorSetStatus(obj, 0, 0x54, 0, 0);
    fightFloorSetStatus(obj, 0, 0x55, 0, 0);
    fightFloorSetStatus(obj, 0, 0x56, 0, 0);
    fightFloorSetStatus(obj, 0, 0x57, 0, 0);
}

/* 0x801F4C14 | size: 0x890 | massive */
u8 fightFloorSetStatus(u32 p1, u16 p2, u32 p3, u16 p4, u32 p5) {
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 fightFloorDataBiosGetPtr(u16);
    extern u32 fightFloorBiosGetFightFloorPtr(void);
    extern void fightFloorDataBiosSetName(u32, u32);
    extern void fightFloorDataBiosSetFloorDataId(u32, u16);
    extern void fightFloorDataBiosSetFightSideDataId(u32, u8, u16);
    extern void fightFloorDataBiosSetTikeiDataId(u32, u8);
    extern void fightFloorDataBiosSetSyoukaiWzxDataId(u32, u32);
    extern void fightFloorDataBiosSetBgmSndId(u32, u32);
    extern void fightFloorDataBiosSetEnvSndId(u32, u32);
    extern u32 joutaiDataBiosGetStatusKind(u16);
    extern u32 statusCheckWriteJoutaiDataId(u32, u16);
    extern void statusWriteJoutaiDataId(u32, u16, u16);
    extern void fightFloorBiosSetTurnCount(u32, u16);
    extern void fightFloorBiosSetEncountDataId(u32, u16);
    extern u32 fightOutPokemonCheckValid(u32);
    extern void msgctrlSetValue(u32, u32);
    extern u32 fightOutPokemonGetTokuseiDataId(u32);
    extern u32 pokemonTokuseiDataBiosGetPtr(u16);
    extern void pokemonTokuseiDataBiosGetName(void);
    extern u32 GSmsgGetGSchar(void);
    extern void fightFloorBiosSetAttackPokemonPtr(u32, u32);
    extern u32 fightFloorBiosGetAttackPokemonPtr(u32);
    extern u32 pokemonSetStatus(u32, u32, u32, u32, u32);
    extern void fightFloorBiosSetDefensePokemonPtr(u32, u32);
    extern void fightFloorBiosSetEscapePokemonPtr(u32, u32);
    extern void fightFloorBiosSetIrekaePokemonPtr(u32, u32);
    extern void fightFloorBiosSetKizetuPokemonPtr(u32, u32);
    extern void fightFloorBiosSetTuikakoukaPokemonPtr(u32, u32);
    extern void fightFloorBiosSetTokuseiPokemonPtr(u32, u32);
    extern u32 fightOutPokemonGetSoubiItemDataId(u32);
    extern void fightFloorBiosSetItemPokemonPtr(u32, u32);
    extern void fightFloorBiosSetEncountFloorId(u32);
    extern void fightFloorBiosSetAppointPokemonPtr(u32, u32);
    extern void fightFloorBiosSetAppointTrainerPtr(u32, u32);
    extern void fightFloorBiosSetAppointWazaPtr(u32, u32);
    extern void fightFloorBiosSetAppointSidePtr(u32, u32);
    extern void fightFloorBiosSetAppointItemPtr(u32, u32);
    extern void fightFloorBiosSetAppointMsgId(u32, u32);
    extern void fightFloorBiosSetAttackMsgId(u32, u32);
    extern void fightFloorBiosSetCriticalMsgId(u32, u32);
    extern void fightFloorBiosSetWazakoukaMsgId(u32, u32);
    extern void fightFloorBiosSetAppointPokemonDataId(u32, u16);
    extern void fightFloorBiosSetAppointWazaDataId(u32, u16);
    extern u32 itemGetStatus(u32, u32, u32, u32);
    extern void fightFloorBiosSetAppointItemDataId(u32, u16);
    extern void fightFloorBiosSetAppointTokuseiDataId(u32, u16);
    extern void fightFloorBiosSetFightPokemonEntryCnt(u32, s16);
    extern void fightFloorBiosSetFightOutPokemonPtrAry(u32, u16, u32);
    extern void fightFloorBiosSetFightOutPokemonPtrAryPtr(u32, u32);
    extern void fightFloorBiosSetFirstAttackRnd(u32, u16);
    extern void fightFloorSetFightResultId(u32, u16);
    extern u32 fightTargetGetTragetPtrToRelativeHostSideFightTargetId(u32, u16);
    u32 ret;
    u32 tmp;

    ret = 0;
    tmp = (u16)fightFloorGetStatus(p1, 0, 0x14, 0);
    if ((u16)p3 == 0) goto fn_801F4C14_invalid;
    if ((u16)p3 < 0x60) goto fn_801F4C14_range_ok;
    fn_801F4C14_invalid: return 0;
    fn_801F4C14_range_ok:
    if ((u16)p3 < 8) {
        p1 = fightFloorDataBiosGetPtr(p2);
        if (p1 == 0) { return 0; }
    } else if ((u16)p3 < 0x5f && p1 == 0) {
        p1 = fightFloorBiosGetFightFloorPtr();
        if (p1 == 0) { return 0; }
    }
    switch ((u16)p3) {
    case 0x1:
        fightFloorDataBiosSetName(p1, p5);
        break;
    case 0x2:
        fightFloorDataBiosSetFloorDataId(p1, (u16)p5);
        break;
    case 0x3:
        fightFloorDataBiosSetFightSideDataId(p1, (u8)p4, (u16)p5);
        break;
    case 0x4:
        fightFloorDataBiosSetTikeiDataId(p1, (u8)p5);
        break;
    case 0x5:
        fightFloorDataBiosSetSyoukaiWzxDataId(p1, p5);
        break;
    case 0x6:
        fightFloorDataBiosSetBgmSndId(p1, p5);
        break;
    case 0x7:
        fightFloorDataBiosSetEnvSndId(p1, p5);
        break;
    case 0xa: {
        u32 _r3;
        _r3 = joutaiDataBiosGetStatusKind((u16)p5);
        if ((u16)_r3 != 0x9) {
            _r3 = 0;
        } else {
            _r3 = statusCheckWriteJoutaiDataId(p1, (u16)p5);
        }
        ret = _r3;
        if ((u8)ret != 0x2) break;
        _r3 = joutaiDataBiosGetStatusKind((u16)p5);
        if ((u16)_r3 != 0x9) break;
        statusWriteJoutaiDataId(p1, (u16)p5, (u16)p4);
        break;
    }
    case 0xc:
        fightFloorBiosSetTurnCount(p1, (u16)p5);
        break;
    case 0xd:
        fightFloorBiosSetEncountDataId(p1, (u16)p5);
        break;
    case 0x36:
        if (p5 != 0) {
            if ((u8)fightOutPokemonCheckValid(p5) == 0x1) {
                msgctrlSetValue(0xf, p5);
                {
                    u32 _v;
                    _v = fightOutPokemonGetTokuseiDataId(p5);
                    _v = pokemonTokuseiDataBiosGetPtr((u16)_v);
                    pokemonTokuseiDataBiosGetName();
                    _v = GSmsgGetGSchar();
                    msgctrlSetValue(0x1a, _v);
                }
                msgctrlSetValue(0x1f, p5);
                msgctrlSetValue(0x21, p5);
                msgctrlSetValue(0x20, p5);
            } else {
                msgctrlSetValue(0xf, 0);
                msgctrlSetValue(0x1a, 0);
                msgctrlSetValue(0x1f, 0);
                msgctrlSetValue(0x21, 0);
                msgctrlSetValue(0x20, 0);
            }
        } else {
            msgctrlSetValue(0xf, 0);
            msgctrlSetValue(0x1a, 0);
            msgctrlSetValue(0x1f, 0);
            msgctrlSetValue(0x21, 0);
            msgctrlSetValue(0x20, 0);
        }
        fightFloorBiosSetAttackPokemonPtr(p1, p5);
        break;
    case 0x37:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xda, 0, p5);
        break;
    case 0x38:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xdb, 0, p5);
        break;
    case 0x39:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xdc, 0, p5);
        break;
    case 0x3a:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xdd, 0, p5);
        break;
    case 0x3b:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xde, 0, p5);
        break;
    case 0x3c:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xdf, 0, p5);
        break;
    case 0x3d:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xe0, 0, p5);
        break;
    case 0x3e:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xe1, 0, p5);
        break;
    case 0x3f:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xe2, 0, p5);
        break;
    case 0x40:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xe3, 0, p5);
        break;
    case 0x41:
        pokemonSetStatus(fightFloorBiosGetAttackPokemonPtr(p1), 0, 0xe4, 0, p5);
        break;
    case 0x42:
        if (p5 != 0) {
            if ((u8)fightOutPokemonCheckValid(p5) == 0x1) {
                msgctrlSetValue(0x10, p5);
                {
                    u32 _v;
                    _v = fightOutPokemonGetTokuseiDataId(p5);
                    _v = pokemonTokuseiDataBiosGetPtr((u16)_v);
                    pokemonTokuseiDataBiosGetName();
                    _v = GSmsgGetGSchar();
                    msgctrlSetValue(0x1b, _v);
                }
                msgctrlSetValue(0x42, p5);
                msgctrlSetValue(0x44, p5);
                msgctrlSetValue(0x43, p5);
            } else {
                msgctrlSetValue(0x10, 0);
                msgctrlSetValue(0x1b, 0);
                msgctrlSetValue(0x42, 0);
                msgctrlSetValue(0x44, 0);
                msgctrlSetValue(0x43, 0);
            }
        } else {
            msgctrlSetValue(0x10, 0);
            msgctrlSetValue(0x1b, 0);
            msgctrlSetValue(0x42, 0);
            msgctrlSetValue(0x44, 0);
            msgctrlSetValue(0x43, 0);
        }
        fightFloorBiosSetDefensePokemonPtr(p1, p5);
        break;
    case 0x43:
        fightFloorSetStatus(p1, 0, 0x42, 0, p5);
        {
            u32 species;
            species = fightTargetGetTragetPtrToRelativeHostSideFightTargetId(p5, (u16)tmp);
            fightFloorSetStatus(p1, 0, 0x40, 0, (u32)(u16)species);
        }
        break;
    case 0x44:
        fightFloorBiosSetEscapePokemonPtr(p1, p5);
        break;
    case 0x45:
        fightFloorBiosSetIrekaePokemonPtr(p1, p5);
        break;
    case 0x46:
        fightFloorBiosSetKizetuPokemonPtr(p1, p5);
        break;
    case 0x47:
        if (p5 != 0) {
            if ((u8)fightOutPokemonCheckValid(p5) == 0x1) {
                msgctrlSetValue(0x12, p5);
                {
                    u32 _v;
                    _v = fightOutPokemonGetTokuseiDataId(p5);
                    _v = pokemonTokuseiDataBiosGetPtr((u16)_v);
                    pokemonTokuseiDataBiosGetName();
                    _v = GSmsgGetGSchar();
                    msgctrlSetValue(0x1d, _v);
                }
            } else {
                msgctrlSetValue(0x12, 0);
                msgctrlSetValue(0x1d, 0);
            }
        } else {
            msgctrlSetValue(0x12, 0);
            msgctrlSetValue(0x1d, 0);
        }
        fightFloorBiosSetTuikakoukaPokemonPtr(p1, p5);
        break;
    case 0x48:
        fightFloorBiosSetTokuseiPokemonPtr(p1, p5);
        break;
    case 0x49:
        if (p5 != 0 && (u8)fightOutPokemonCheckValid(p5) == 0x1) {
            u32 species;
            species = fightOutPokemonGetSoubiItemDataId(p5);
            fightFloorSetStatus(p1, 0, 0x56, 0, (u32)(u16)species);
        }
        fightFloorBiosSetItemPokemonPtr(p1, p5);
        break;
    case 0x4a:
        fightFloorBiosSetEncountFloorId(p5);
        break;
    case 0x4b:
        if (p5 != 0) {
            if ((u8)fightOutPokemonCheckValid(p5) == 0x1) {
                msgctrlSetValue(0x1e, p5);
                {
                    u32 _v;
                    _v = fightOutPokemonGetTokuseiDataId(p5);
                    _v = pokemonTokuseiDataBiosGetPtr((u16)_v);
                    pokemonTokuseiDataBiosGetName();
                    _v = GSmsgGetGSchar();
                    msgctrlSetValue(0x1c, _v);
                }
            } else {
                msgctrlSetValue(0x1e, 0);
                msgctrlSetValue(0x1c, 0);
            }
        } else {
            msgctrlSetValue(0x1e, 0);
            msgctrlSetValue(0x1c, 0);
        }
        fightFloorBiosSetAppointPokemonPtr(p1, p5);
        break;
    case 0x4c:
        fightFloorBiosSetAppointTrainerPtr(p1, p5);
        break;
    case 0x4d:
        fightFloorBiosSetAppointWazaPtr(p1, p5);
        break;
    case 0x4e:
        fightFloorBiosSetAppointSidePtr(p1, p5);
        break;
    case 0x4f:
        fightFloorBiosSetAppointItemPtr(p1, p5);
        break;
    case 0x50:
        fightFloorBiosSetAppointMsgId(p1, p5);
        break;
    case 0x51:
        fightFloorBiosSetAttackMsgId(p1, p5);
        break;
    case 0x52:
        fightFloorBiosSetCriticalMsgId(p1, p5);
        break;
    case 0x53:
        fightFloorBiosSetWazakoukaMsgId(p1, p5);
        break;
    case 0x54:
        fightFloorBiosSetAppointPokemonDataId(p1, (u16)p5);
        break;
    case 0x55:
        fightFloorBiosSetAppointWazaDataId(p1, (u16)p5);
        break;
    case 0x56:
        itemGetStatus(0, (u16)p5, 1, 0);
        msgctrlSetValue(0x29, GSmsgGetGSchar());
        fightFloorBiosSetAppointItemDataId(p1, (u16)p5);
        break;
    case 0x57:
        pokemonTokuseiDataBiosGetPtr((u16)p5);
        pokemonTokuseiDataBiosGetName();
        msgctrlSetValue(0x1c, GSmsgGetGSchar());
        fightFloorBiosSetAppointTokuseiDataId(p1, (u16)p5);
        break;
    case 0x58:
        fightFloorBiosSetFightPokemonEntryCnt(p1, (s16)p5);
        break;
    case 0x59:
        fightFloorBiosSetFightOutPokemonPtrAry(p1, (u16)p4, p5);
        break;
    case 0x5a:
        fightFloorBiosSetFightOutPokemonPtrAryPtr(p1, p5);
        break;
    case 0x5b:
        fightFloorBiosSetFirstAttackRnd(p1, (u16)p5);
        break;
    case 0x5c:
        fightFloorSetFightResultId(p1, (u16)p5);
        break;
    case 0x5e:
        msgctrlSetValue(0x2f, p5);
        break;
    }
    return (u8)ret;
}

#endif

#if defined(FIGHT_FLOOR_801F54A4_801F61BC)

#pragma push
#if 0
asm void fightFloorGetStatus(void) {
#include "src/game/pokemon_fn_801F54A4.inc"
}
#else
/* Exact (2026-09-30). The index-th valid Pokemon of the floor, counted over
 * (pokemon, trainer, side) in the same triple loop as
 * fightFloorLoopValidFightOutPokemon, through the same chain inline; case
 * 0x5D of fightFloorGetStatus.
 *
 * Every register here follows from MWCC's allocator, replayed with
 * tools/local_campaign.py explain (the GC/2.6 replay reproduces this unit):
 *  - the counts and counters are the inline's own locals, u16 like the loop
 *    function's (a u32 counter's conversion hoists out of the loops into a
 *    saved register retail does not use), with n declared after the counts:
 *    inline locals take virtual registers in reverse declaration order and
 *    colour highest first, so n is r29, c18 r28, c16 r27;
 *  - the searched index is the dispatcher's own u16 object, written by the
 *    inline once the counts are in. Retail truncates arg into r31 in the loop
 *    preheader ("clrlwi r31,r23,16") and pops that web before pkm. Only an
 *    object outside the inline's coalesce window keeps a low enough virtual
 *    register to survive simplify's first pass alongside pkm; an inline
 *    local or a (u16) cast at the call is merged into the conversion temp,
 *    numbered after everything else, and lands in r30 with pkm in r31 (107
 *    lines). A dispatcher local assigned before the call is the right web but
 *    puts the truncation before the count calls (2 lines). Writing it through
 *    the pointer after the calls is the C shape with both properties: after
 *    inlining it is a plain register object, no memory is touched, and the
 *    value is used. It is unusual, and it is the only shape found that
 *    matches; the replay is the evidence. */
static inline u32 fightFloorSearchFightOutPokemon(u8* pkm, u32 arg, u16* index)
{
    extern u32 fightSideCheckValid(u32);
    extern u32 fightSideGetValidFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetValidFightOutPokemonPtr(u32, u32);
    extern s32 fightFloorGetStatus(u8* pkm, u32 slot, u32 field, u32 arg);
    u16 j, i, k;
    u16 c16, c18;
    u32 n;

    fightFloorGetStatus(pkm, 0, 0x14, 0);
    c16 = fightFloorGetStatus(pkm, 0, 0x16, 0);
    c18 = fightFloorGetStatus(pkm, 0, 0x18, 0);
    n = 0;
    *index = arg;
    for (i = 0; i < c18; i++) {
        for (j = 0; j < c16; j++) {
            for (k = 0; k < 2; k++) {
                void* v = fightFloorLoopValidFightOutPokemonSub(pkm, k, j, i);
                if (v != NULL && (u16)n == *index) {
                    return (u32)v;
                }
                n++;
            }
        }
    }
    return 0;
}

/* 0x801F54A4 | size: 0xD18 | PokemonGet: field dispatcher */
s32 fightFloorGetStatus(u8* pkm, u32 slot, u32 field, u32 arg) {
    extern struct Pokemon* fightFloorDataBiosGetPtr(u32);
    extern struct Pokemon* fightFloorBiosGetFightFloorPtr(void);
    extern u32 joutaiDataBiosGetStatusKind(u32);
    extern u32 fn_8011B67C(u8*, u32);
    extern u32 statusCheckWriteJoutaiDataId(u8*, u32);
    extern void fightFloorLoopValidFightOutPokemon(u8*, u32 (*)(u8*, u32*), u32*, u32);
    extern u32 _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv(u8*, u32*);
    extern u32 pokemonGetStatus(u32, u32, u32, u32);
    extern u32 std103_battleGetResult(void);
    extern u8 fightFloorDataBiosGetTikeiDataId(u8*);
    extern u32 fightFloorDataBiosGetSyoukaiWzxDataId(u8*);
    extern u32 fightFloorDataBiosGetBgmSndId(u8*);
    extern u32 fightFloorDataBiosGetEnvSndId(u8*);
    extern u16 fightFloorBiosGetTurnCount(u8*);
    extern u16 fightFloorBiosGetEncountDataId(u8*);
    extern u32 fightFloorBiosGetAttackPokemonPtr(u8*);
    extern u32 fightFloorBiosGetDefensePokemonPtr(u8*);
    extern u32 fightFloorBiosGetEscapePokemonPtr(u8*);
    extern u32 fightFloorBiosGetIrekaePokemonPtr(u8*);
    extern u32 fightFloorBiosGetAppointPokemonPtr(u8*);
    extern u32 fightFloorBiosGetAppointTrainerPtr(u8*);
    extern u32 fightFloorBiosGetAppointWazaPtr(u8*);
    extern u32 fightFloorBiosGetAppointSidePtr(u8*);
    extern u32 fightFloorBiosGetAppointItemPtr(u8*);
    extern u32 fightFloorBiosGetAppointMsgId(u8*);
    extern u32 fightFloorBiosGetAttackMsgId(u8*);
    extern u32 fightFloorBiosGetCriticalMsgId(u8*);
    extern u32 fightFloorBiosGetWazakoukaMsgId(u8*);
    extern u32 fightFloorBiosGetAppointPokemonDataId(u8*);
    extern u32 fightFloorBiosGetAppointWazaDataId(u8*);
    extern u32 fightFloorBiosGetAppointItemDataId(u8*);
    extern u32 fightFloorBiosGetAppointTokuseiDataId(u8*);
    extern u32 fightFloorBiosGetFightPokemonEntryCnt(u8*);
    extern u32 fightFloorBiosGetFirstAttackRnd(u8*);
    extern u32 fightFloorBiosGetKizetuPokemonPtr(u8*);
    extern u32 fightFloorBiosGetTuikakoukaPokemonPtr(u8*);
    extern u32 fightFloorBiosGetTokuseiPokemonPtr(u8*);
    extern u32 fightFloorBiosGetItemPokemonPtr(u8*);
    extern u32 fightFloorBiosGetEncountFloorId(void);
    extern u32 fightSideCheckValid(u32);
    extern u32 fightSideGetValidFightTrainerPtr(u32, u32);
    extern u32 fightTrainerGetValidFightOutPokemonPtr(u32, u32);
    extern u32 fightEncountDataBiosGetPtr(u32);
    extern u32 fightEncountDataBiosGetFightFloorDataId(u32);
    extern u32 fightEncountDataBiosGetTrainer(u32);
    extern u32 fightEncountDataBiosGetFightKind(u32);
    extern u32 fightEncountGetBgmSndDataId(u16);
    extern u32 fightEncountGetEnvSndDataId(u16);
    extern u32 fightTypeDataBiosGetPtr(u32);
    extern u32 fightTypeDataBiosGetName(u32);
    extern u32 fightTypeDataBiosGetTrainerNum(u32);
    extern u32 fightTypeDataBiosGetEntryPokemonNum(u32);
    extern u32 fightTypeDataBiosGetFightoutPokemonNum(u32);
    extern u32 fightTypeGetFightSideFightOutPokemonMax(u32);
    extern u32 fightKindDataBiosGetPtr(u32);
    extern u32 fightKindDataBiosGetName(u32);
    extern u32 fightKindDataBiosGetBackSaveDataFlag(u32);
    extern u32 fightKindDataBiosGetDoBadgeCheckFlag(u32);
    extern u32 fightKindDataBiosGetDoZukanMitaFlag(u32);
    extern u32 fightKindDataBiosGetDoZukanTukamaetaFlag(u32);
    extern u32 fightKindDataBiosGetUseItemFlag(u32);
    extern u32 fightKindDataBiosGetCallFlag(u32);
    extern u32 fightKindDataBiosGetNigeruFlag(u32);
    extern u32 fightKindDataBiosGetDrawFlag(u32);
    extern u32 fightKindDataBiosGetGetExpFlag(u32);
    extern u32 fightKindDataBiosGetOkanePoolFlag(u32);
    extern u32 fightKindDataBiosGetGetOkaneFlag(u32);
    extern u32 fightKindDataBiosGetGetNekoniKobanFlag(u32);
    extern u32 fightKindDataBiosGetGetFriendFlag(u32);
    extern u32 fightKindDataBiosGetGetInfectPokerusFlag(u32);
    extern u32 fightKindDataBiosGetDoCriticalAttackFlag(u32);
    extern u32 fightKindDataBiosGetDoHizukiAiFlag(u32);
    extern u32 fightKindDataBiosGetDoHizukiMiyaburiFlag(u32);
    extern u32 fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(u32);
    extern u32 fightKindDataBiosGetKeikentihueruFlag(u32);
    extern u32 fightKindDataBiosGetBossFlag(u32);
    extern u32 fightKindDataBiosGetDorobouFlag(u32);
    extern u32 fightKindDataBiosGetMonohiroiFlag(u32);
    extern u32 fightKindDataBiosGetDarkpokemonHypermodeFlag(u32);
    extern u32 fightKindDataBiosGetPokemonStatusMenuSubbarFlag(u32);
    extern u32 fightKindDataBiosGetHostEnemyMsgFlag(u32);
    u16 f;

    f = (u16)field;
    if (f >= 0x60) {
        return 0;
    }
    if (f == 0) {
        return (s32)fightFloorBiosGetFightFloorPtr();
    }
    if (f < 8) {
        if ((pkm = (u8*)fightFloorDataBiosGetPtr(slot)) == NULL) {
            return 0;
        }
    } else if (f < 0x5F) {
        if (pkm == NULL) {
            if ((pkm = (u8*)fightFloorBiosGetFightFloorPtr()) == NULL) {
                return 0;
            }
        }
    }

    switch ((u16)field) {
    case 0x1:
        return fightFloorDataBiosGetName(pkm);
    case 0x2:
        return fightFloorDataBiosGetFloorDataId(pkm);
    case 0x3:
        return (u16)fightFloorDataBiosGetFightSideDataId(pkm, (u8)arg);
    case 0x4:
        return fightFloorDataBiosGetTikeiDataId(pkm);
    case 0x5:
        return fightFloorDataBiosGetSyoukaiWzxDataId(pkm);
    case 0x6:
        return fightFloorDataBiosGetBgmSndId(pkm);
    case 0x7:
        return fightFloorDataBiosGetEnvSndId(pkm);
    case 0x9: {
        extern u8* fightFloorBiosGetJoutaiPtr();
        return (s32)fightFloorBiosGetJoutaiPtr(pkm, arg);
    }
    case 0xA: {
        u32 v;
        arg &= 0xFFFF;
        if ((u16)joutaiDataBiosGetStatusKind(arg) != 9) {
            v = 0;
        } else {
            v = fn_8011B67C(pkm, arg);
        }
        return (u8)v;
    }
    case 0xB: {
        u32 v;
        arg &= 0xFFFF;
        if ((u16)joutaiDataBiosGetStatusKind(arg) != 9) {
            v = 0;
        } else {
            v = statusCheckWriteJoutaiDataId(pkm, arg);
        }
        return (u8)v;
    }
    case 0xC:
        return fightFloorBiosGetTurnCount(pkm);
    case 0xD:
        return fightFloorBiosGetEncountDataId(pkm);
    case 0xE:
        return (u16)fightEncountDataBiosGetFightFloorDataId(fightEncountDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0xD, 0)));
    case 0xF:
        return fightFloorGetStatus(NULL, (u16)fightFloorGetStatus(pkm, 0, 0xE, 0), 4, 0);
    case 0x10:
        return fightFloorGetStatus(NULL, (u16)fightFloorGetStatus(pkm, 0, 0xE, 0), 5, 0);
    case 0x11:
        return fightEncountGetBgmSndDataId((u16)fightFloorGetStatus(pkm, 0, 0xD, 0));
    case 0x12:
        return fightEncountGetEnvSndDataId((u16)fightFloorGetStatus(pkm, 0, 0xD, 0));
    case 0x13: {
        u32 v;
        if ((u8)arg == 1) {
            u32 a[4];
            u32 b[4];
            a[0] = 0xD;
            a[1] = 0;
            a[2] = 0;
            a[3] = 0;
            fightFloorLoopValidFightOutPokemon(pkm, _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, a, 0);
            if ((u16)a[1] != 0) {
                v = 0;
                goto done13;
            }
            b[0] = 0x4D;
            b[1] = 0;
            b[2] = 0;
            b[3] = 0;
            fightFloorLoopValidFightOutPokemon(pkm, _fightFloorCheckFightOutPokemonPtrAryPokemonTokuseiDataIdSub__FPvUsPv, b, 0);
            if ((u16)b[1] != 0) {
                v = 0;
                goto done13;
            }
        }
        if (fightFloorGetStatus(pkm, 0, 0xA, 0x4E) == 1) {
            v = 0;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x4F) == 1) {
            v = 1;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x50) == 1) {
            v = 2;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x51) == 1) {
            v = 3;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x52) == 1) {
            v = 4;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x53) == 1) {
            v = 1;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x54) == 1) {
            v = 2;
        } else if (fightFloorGetStatus(pkm, 0, 0xA, 0x55) == 1) {
            v = 3;
        } else {
            v = 0;
        }
    done13:
        return (u8)v;
    }
    case 0x14:
        return (u8)fightEncountDataBiosGetTrainer(fightEncountDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0xD, 0)));
    case 0x15:
        return fightTypeDataBiosGetName(fightTypeDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x14, 0)));
    case 0x16:
        return (u8)fightTypeDataBiosGetTrainerNum(fightTypeDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x14, 0)));
    case 0x17:
        return (u8)fightTypeDataBiosGetEntryPokemonNum(fightTypeDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x14, 0)));
    case 0x18:
        return (u8)fightTypeDataBiosGetFightoutPokemonNum(fightTypeDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x14, 0)));
    case 0x19:
        return (u16)fightTypeGetFightSideFightOutPokemonMax((u16)fightFloorGetStatus(pkm, 0, 0x14, 0));
    case 0x1A:
        return (u8)fightEncountDataBiosGetFightKind(fightEncountDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0xD, 0)));
    case 0x1B:
        return fightKindDataBiosGetName(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x1C:
        return (u8)fightKindDataBiosGetBackSaveDataFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x1D:
        return (u8)fightKindDataBiosGetDoBadgeCheckFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x1E:
        return (u8)fightKindDataBiosGetDoZukanMitaFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x1F:
        return (u8)fightKindDataBiosGetDoZukanTukamaetaFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x20:
        return (u8)fightKindDataBiosGetUseItemFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x21:
        return (u8)fightKindDataBiosGetCallFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x22:
        return (u8)fightKindDataBiosGetNigeruFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x23:
        return (u8)fightKindDataBiosGetDrawFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x24:
        return (u8)fightKindDataBiosGetGetExpFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x26:
        return (u8)fightKindDataBiosGetOkanePoolFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x25:
        return (u8)fightKindDataBiosGetGetOkaneFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x27:
        return (u8)fightKindDataBiosGetGetNekoniKobanFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x28:
        return (u8)fightKindDataBiosGetGetFriendFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x29:
        return (u8)fightKindDataBiosGetGetInfectPokerusFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x2A:
        return (u8)fightKindDataBiosGetDoCriticalAttackFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x2B:
        return (u8)fightKindDataBiosGetDoHizukiAiFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x2C:
        return (u8)fightKindDataBiosGetDoHizukiMiyaburiFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x2D:
        return (u8)fightKindDataBiosGetDoItemSoubiTokukoutokubouupFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x2E:
        return (u8)fightKindDataBiosGetKeikentihueruFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x2F:
        return (u8)fightKindDataBiosGetBossFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x30:
        return (u8)fightKindDataBiosGetDorobouFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x31:
        return (u8)fightKindDataBiosGetMonohiroiFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x32:
        return (u8)fightKindDataBiosGetDarkpokemonHypermodeFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x33:
        return (u8)fightKindDataBiosGetPokemonStatusMenuSubbarFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x34:
        return (u8)fightKindDataBiosGetHostEnemyMsgFlag(fightKindDataBiosGetPtr((u16)fightFloorGetStatus(pkm, 0, 0x1A, 0)));
    case 0x35: {
        extern u8* fightFloorBiosGetFightSidePtr();
        return (s32)fightFloorBiosGetFightSidePtr(pkm, arg);
    }
    case 0x36:
        return fightFloorBiosGetAttackPokemonPtr(pkm);
    case 0x37:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xDA, 0);
    case 0x38:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xDB, 0);
    case 0x39:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xDC, 0);
    case 0x3A:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xDD, 0);
    case 0x3B:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xDE, arg);
    case 0x3C:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xDF, arg);
    case 0x3D:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xE0, arg);
    case 0x3E:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xE1, 0);
    case 0x3F:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xE2, 0);
    case 0x40:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xE3, 0);
    case 0x41:
        return pokemonGetStatus(fightFloorBiosGetAttackPokemonPtr(pkm), 0, 0xE4, 0);
    case 0x42:
        return fightFloorBiosGetDefensePokemonPtr(pkm);
    case 0x44:
        return fightFloorBiosGetEscapePokemonPtr(pkm);
    case 0x45:
        return fightFloorBiosGetIrekaePokemonPtr(pkm);
    case 0x46:
        return fightFloorBiosGetKizetuPokemonPtr(pkm);
    case 0x47:
        return fightFloorBiosGetTuikakoukaPokemonPtr(pkm);
    case 0x48:
        return fightFloorBiosGetTokuseiPokemonPtr(pkm);
    case 0x49:
        return fightFloorBiosGetItemPokemonPtr(pkm);
    case 0x4A:
        return fightFloorBiosGetEncountFloorId();
    case 0x4B:
        return fightFloorBiosGetAppointPokemonPtr(pkm);
    case 0x4C:
        return fightFloorBiosGetAppointTrainerPtr(pkm);
    case 0x4D:
        return fightFloorBiosGetAppointWazaPtr(pkm);
    case 0x4E:
        return fightFloorBiosGetAppointSidePtr(pkm);
    case 0x4F:
        return fightFloorBiosGetAppointItemPtr(pkm);
    case 0x50:
        return fightFloorBiosGetAppointMsgId(pkm);
    case 0x51:
        return fightFloorBiosGetAttackMsgId(pkm);
    case 0x52:
        return fightFloorBiosGetCriticalMsgId(pkm);
    case 0x53:
        return fightFloorBiosGetWazakoukaMsgId(pkm);
    case 0x54:
        return (u16)fightFloorBiosGetAppointPokemonDataId(pkm);
    case 0x55:
        return (u16)fightFloorBiosGetAppointWazaDataId(pkm);
    case 0x56:
        return (u16)fightFloorBiosGetAppointItemDataId(pkm);
    case 0x57:
        return (u16)fightFloorBiosGetAppointTokuseiDataId(pkm);
    case 0x58:
        return (s16)fightFloorBiosGetFightPokemonEntryCnt(pkm);
    case 0x59:
        return fightFloorBiosGetFightOutPokemonPtrAry(pkm, (u16)arg);
    case 0x5A:
        return (s32)fightFloorBiosGetFightOutPokemonPtrAryPtr(pkm);
    case 0x5B:
        return (u16)fightFloorBiosGetFirstAttackRnd(pkm);
    case 0x5C:
        return (u16)std103_battleGetResult();
    case 0x5D: {
        u16 index;

        return fightFloorSearchFightOutPokemon(pkm, arg, &index);
    }
    default:
        return 0;
    }
}
#endif
#pragma pop

#endif

#if defined(FIGHT_FLOOR_801F61BC_801F61EC)

/* 0x801F61BC | size: 0x30 */
struct Pokemon* fightFloorGetNowPtr(void) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u32);
    return (struct Pokemon*)fightFloorGetStatus(0, 0, 0, 0);
}

#endif

#if defined(FIGHT_FLOOR_801F61EC_801F640C)

/* Pokemon XD's _fightFloorSetFightOutPokemonPtrAry__FPP15FightOutPokemonP15FightOutPokemon
 * (GXXE01 0x801F5A48, size 0x3C; TeamOrre/xd-decomp symbols.txt 4989794e,
 * body trevor403/xd-asm b1087f18 code/func_FUN_801F5A48.s): stores the
 * Pokemon in the first free slot and returns its index, or -1. XD's
 * _fightFloorCreateFightOutPokemonPtrAry (0x801F58B0) calls it, after
 * _fightFloorInitFightOutPokemonPtrAry and fightFloorGetValidFightSidePtr;
 * Colosseum expands all three. */
static inline s16 _fightFloorSetFightOutPokemonPtrAry(void** ary, void* pokemon) {
    u16 i;

    for (i = 0; i < 8; i++) {
        if (ary[i] == NULL) {
            ary[i] = pokemon;
            return i;
        }
    }
    return -1;
}

/* 0x801F61EC | size: 0x220 | large */
u32 _fightFloorCreateFightOutPokemonPtrAry__FP11FIGHT_FLOORPP15FightOutPokemonbUcP15FightOutPokemon(void* floor, void** ary, u8 fightOutOnly, u8 mode, void* target) {
    extern u32 fightFloorGetStatus(void*, u32, u32, u16);
    extern void* fightTargetGetPtr(u32, void*, u16);
    extern void* fightSideGetValidFightTrainerPtr(void*, u16);
    extern void* fightTrainerGetValidFightOutPokemonPtr(void*, u16);
    extern u8 fightOutPokemonCheckFightOut(void*);
    extern u8 fightOutPokemonCheckValid(void*);
    void* pokemon;
    void* sidePtr;
    void* targetSide;
    u16 trainerNo;
    u16 pokemonNo;
    u16 side;
    u16 trainerNum;
    u16 pokemonNum;
    u32 count;
    void* trainer;
    u16 type;

    _fightFloorInitFightOutPokemonPtrAry(ary);
    count = 0;
    type = fightFloorGetStatus(floor, 0, 0x14, 0);
    trainerNum = fightFloorGetStatus(floor, 0, 0x16, 0);
    pokemonNum = fightFloorGetStatus(floor, 0, 0x18, 0);
    if (mode != 0) {
        if (fightOutPokemonCheckValid(target) == 0)
            return 0;
        targetSide = fightTargetGetPtr(2, target, type);
    } else {
        targetSide = NULL;
    }
    for (side = 0; side < 2; side++) {
        sidePtr = _fightFloorGetValidFightSidePtr(floor, side);
        if (sidePtr == NULL)
            continue;
        if (mode == 1) {
            if (targetSide != sidePtr)
                continue;
        } else if (mode == 2) {
            if (targetSide == sidePtr)
                continue;
        }
        for (trainerNo = 0; trainerNo < trainerNum; trainerNo++) {
            trainer = fightSideGetValidFightTrainerPtr(sidePtr, trainerNo);
            if (trainer == NULL)
                continue;
            for (pokemonNo = 0; pokemonNo < pokemonNum; pokemonNo++) {
                pokemon = fightTrainerGetValidFightOutPokemonPtr(trainer, pokemonNo);
                if (pokemon == NULL)
                    continue;
                if (fightOutOnly == 1 && fightOutPokemonCheckFightOut(pokemon) == 0)
                    continue;
                if (mode == 3 && pokemon == target)
                    continue;
                if (_fightFloorSetFightOutPokemonPtrAry(ary, pokemon) >= 0)
                    count++;
            }
        }
    }
    return count;
}



#endif
