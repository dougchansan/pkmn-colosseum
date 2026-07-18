/**
 * @file fight_range_exact_8021D9C0.c
 * @brief Exact pure-C fight-sequence island, 0x8021D9C0 - 0x8021FAD4.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_80478D78[1];
extern u8 fn_801DDD28();
extern void fn_801DA9E8();
extern u32 fn_801F8A18();
extern void* lbl_8047B64C;
extern u8 lbl_80478278[0x10];
extern u32 lbl_8047B618;
extern u8 lbl_8047B642;
extern u16 lbl_8047B61C;

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021D9C0(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u8 fn_801F18DC();
    extern u32 fn_801F8000();
    extern void* fightOutPokemonGetNicknamePtr();
    extern void msgctrlSetValue(s32, s32);
    extern void* fightTrainerGetNamePtr();
    extern s32 fightFloorGetEnemyFightSideManyDamageHpRate();
    u32 ctx1 = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    u32 obj1 = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ctx1);
    u32 slotActive = ((u8)fightOutPokemonIsGcHeroFightOutPokemon(ctx1) == 1);
    u32 canTrigger = ((u8)fn_801F18DC(0) == 1);
    s32 ret8;
    if ((u32)fn_801F8000(obj1) == 0 && (u8)slotActive == 0) {
        canTrigger = 1;
    }
    msgctrlSetValue(0x16, (s32)fightOutPokemonGetNicknamePtr(ctx1));
    msgctrlSetValue(0xd, (s32)fightOutPokemonGetNicknamePtr(ctx1));
    msgctrlSetValue(0x22, (s32)fn_801F8000(obj1));
    msgctrlSetValue(0x23, (s32)fightTrainerGetNamePtr(obj1));
    msgctrlSetValue(0x25, (s32)fightTrainerGetNamePtr(obj1));
    if ((u8)canTrigger == 1) {
        lbl_80478D78[5] = 5;
    } else if ((u8)slotActive == 1) {
        ret8 = fightFloorGetEnemyFightSideManyDamageHpRate(0, ctx1);
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

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DB78(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void* fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u8 fightOutPokemonIsGcHeroFightOutPokemon();
    extern u8 fn_801F18DC();
    extern u32 fn_801F8000();
    extern void* fightOutPokemonGetNicknamePtr();
    extern void msgctrlSetValue(s32, s32);
    extern void* fightTrainerGetNamePtr();
    extern s32 fightFloorGetEnemyFightSideLittleNokoriHpRate();
    u32 ctx1 = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    u32 obj1 = (u32)fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ctx1);
    u32 slotActive = ((u8)fightOutPokemonIsGcHeroFightOutPokemon(ctx1) == 1);
    u32 canTrigger = ((u8)fn_801F18DC(0) == 1);
    s32 ret8;
    if ((u32)fn_801F8000(obj1) == 0 && (u8)slotActive == 0) {
        canTrigger = 1;
    }
    msgctrlSetValue(0x16, (s32)fightOutPokemonGetNicknamePtr(ctx1));
    msgctrlSetValue(0xd, (s32)fightOutPokemonGetNicknamePtr(ctx1));
    msgctrlSetValue(0x22, (s32)fn_801F8000(obj1));
    msgctrlSetValue(0x23, (s32)fightTrainerGetNamePtr(obj1));
    msgctrlSetValue(0x25, (s32)fightTrainerGetNamePtr(obj1));
    if ((u8)canTrigger == 1) {
        lbl_80478D78[5] = 5;
    } else if ((u8)slotActive == 1) {
        ret8 = fightFloorGetEnemyFightSideLittleNokoriHpRate(0, ctx1);
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

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
u8* fn_8021DD24(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DD34(void) {
    lbl_8047B610++;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DD44(void) {
    extern void* fightTargetGetPtrAsNowFightType();
    extern void fightOutPokemonSetVisibility();
    extern void* pokemonGetStatus();
    void* obj = (void*)fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
    if (obj != ((void*)0)) {
        void* p;
        fightOutPokemonSetVisibility(obj, 1);
        p = pokemonGetStatus(obj, 0, 0xee, 0);
        if (p != ((void*)0)) {
            fn_801DA36C(p, 3);
        }
    }
    lbl_8047B610 += 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DDB8(void) {
    lbl_8047B610++;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DDC8(void) {
    lbl_8047B610++;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DDD8(void) {
    extern void fightPokemonToMenuLvupStatus();
    extern void fightMenuCloseLevelUpStatusMenu();
    u8 buf1[24];
    u8 buf2[16];
    fightPokemonToMenuLvupStatus(lbl_8047B64C, buf1);
    fightMenuSubMenuLvupStatus(buf1, lbl_80478278, buf2);
    fightMenuOpenLevelUpStatusMenu(buf2, 1);
    fightMenuOpenLevelUpStatusMenu(buf1, 0);
    fightMenuCloseLevelUpStatusMenu();
    lbl_8047B610 += 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DE3C(void) {
    lbl_8047B610++;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DE4C(void) {
    extern int fightTargetGetPtrAsNowFightType();
    extern int fightOutPokemonDoItemSoubi();
    extern u8 fightOutPokemonIsUseHensinBuff(void* trainer);
    extern void fightOutPokemonSetHensinPokemonStatusId(void* trainer, u32 eventId, u32 param1,
                                                        u32 param2);
    void* ctx = (void*)fightTargetGetPtrAsNowFightType(lbl_8047B610[1], 0);
    fightOutPokemonDoItemSoubi(ctx, 0, 0, 1);
    if (fightOutPokemonIsUseHensinBuff(ctx) == 1) {
        fightOutPokemonSetHensinPokemonStatusId(ctx, 0x82, 0, 0);
    }
    lbl_8047B610 += 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void WS_KORAERU_CHECK(void) {
    fn_802271E0(1, 0);
    lbl_8047B610 = lbl_8047B610 + 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma dont_inline on
void WS_SCA_END_SET(void) {
    extern void fightFloorLoopValidFightOutPokemon();
    extern u32 fn_8021DF3C();
    fightFloorLoopValidFightOutPokemon(0, fn_8021DF3C, 0, 0);
    lbl_8047B610 = lbl_8047B610 + 1;
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
s32 fn_8021DF3C(void* arg) {
    extern u32 pokemonSetStatus();
    pokemonSetStatus(arg, 0, 0x112, 0, 1);
    return 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021DF70(void) {
    lbl_8047B610++;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021DF80(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u32 fightFloorGetStatus();
    u8 slot = *(u8*)(lbl_8047B610 + 1);
    u16 cand;
    u32 fieldEE;
    u32 ctx = fightTargetGetPtrAsNowFightType(slot, 0);
    u32 operand = *(u32*)(lbl_8047B610 + 3);
    if ((lbl_8047B618 & 0x80) != 0) {
        goto skip;
    }
    if (operand != 7) {
        goto skip;
    }
    cand = fightFloorGetStatus(0, 0, 0x14, 0);
    fieldEE = pokemonGetStatus(ctx, 0, 0xee, 0);
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

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma dont_inline on
void fn_8021E04C(void) {
    extern void fn_801DA9E8();
    extern s8 fn_801DDD28();
    extern u32 pokemonGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern void fn_80265598();
    extern u32 lbl_8047B618;
    u16 uVar3;
    int iVar2;
    u8 cVar4;
    int iVar7;
    u16 uVar6;
    u32 iVar5;
    u32 uVar1;
    fightFloorGetStatus(0, 0, 0x14, 0);
    uVar1 = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    iVar5 = *(u32*)(lbl_8047B610 + 2);
    if ((lbl_8047B618 & 0x80) == 0) {
        if (iVar5 == 9) {
            uVar6 = fightFloorGetStatus(0, 0, 0x14, 0);
            iVar7 = (int)pokemonGetStatus(uVar1, 0, 0xee, 0);
            if ((iVar7 != 0) && (cVar4 = fn_801DDD28(iVar7, 0x34, 4, 0), cVar4 != 0)) {
                fn_801DA9E8(iVar7, 0x34, 4);
                fn_80265598(uVar1, uVar6, 1);
            }
        }
        if (iVar5 == 0x18) {
            uVar3 = fightFloorGetStatus(0, 0, 0x14, 0);
            iVar2 = (int)pokemonGetStatus(uVar1, 0, 0xee, 0);
            if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2, 0x35, 4, 0), cVar4 != 0)) {
                fn_801DA9E8(iVar2, 0x35, 4);
                fn_80265598(uVar1, uVar3, 1);
            }
        }
        if (iVar5 == 0x17) {
            uVar3 = fightFloorGetStatus(0, 0, 0x14, 0);
            iVar2 = (int)pokemonGetStatus(uVar1, 0, 0xee, 0);
            if ((iVar2 != 0) && (cVar4 = fn_801DDD28(iVar2, 0x36, 4, 0), cVar4 != 0)) {
                fn_801DA9E8(iVar2, 0x36, 4);
                fn_80265598(uVar1, uVar3, 1);
            }
        }
        if (iVar5 == 10) {
            uVar3 = fightFloorGetStatus(0, 0, 0x14, 0);
            iVar5 = (int)pokemonGetStatus(uVar1, 0, 0xee, 0);
            if ((iVar5 != 0) && (cVar4 = fn_801DDD28(iVar5, 0x37, 4, 0), cVar4 != 0)) {
                fn_801DA9E8(iVar5, 0x37, 4);
                fn_80265598(uVar1, uVar3, 1);
            }
        }
    }
    lbl_8047B610 = lbl_8047B610 + 6;
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021E288(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus(u32 a, u32 b, u32 c, u32 d);
    extern void* pokemonGetStatus(void* context, u32 slot, u16 tableId, u32 flags);
    extern u8 fn_802026E4(void* ctx, void* typeObj);
    extern u8 fn_801DDD28(void* ptr, u32 field, u32 size, u32 flags);
    extern void fn_801DA9E8(void* ptr, u32 field, u32 size);
    extern void fn_80265598();
    void* ctx;
    u8* pc;
    fightFloorGetStatus(0, 0, 0x14, 0);
    pc = (u8*)lbl_8047B610;
    ctx = (void*)fightTargetGetPtrAsNowFightType(pc[1], 0);
    if ((lbl_8047B618 & 0x80) == 0) {
        if (fn_802026E4(ctx, (void*)0x8) == 1) {
            u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
            void* poke = pokemonGetStatus(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x2e, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x2e, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x5) == 1) {
            u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
            void* poke = pokemonGetStatus(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x2f, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x2f, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x7) == 1) {
            u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
            void* poke = pokemonGetStatus(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x30, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x30, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x6) == 1) {
            u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
            void* poke = pokemonGetStatus(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x31, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x31, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x3) == 1) {
            u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
            void* poke = pokemonGetStatus(ctx, 0, 0xee, 0);
            if (poke != 0) {
                if (fn_801DDD28(poke, 0x32, 4, 0) != 0) {
                    fn_801DA9E8(poke, 0x32, 4);
                    fn_80265598(ctx, val, 1);
                }
            }
        }
        if (fn_802026E4(ctx, (void*)0x4) == 1) {
            u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
            void* poke = pokemonGetStatus(ctx, 0, 0xee, 0);
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
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma dont_inline on
#pragma opt_propagation off
void fn_8021E600(void) {
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    extern u32 pokemonGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorGetStatus();
    extern void fightWazaSetUseWazaStatus();
    extern u16 lbl_8047B60C;
    extern u32 lbl_80379BFF[];
    u32 uVar2;
    u32 uVar1;
    u16 uVar3;
    u16 statusValue;
    fightFloorGetStatus(0, 0, 0x14, 0);
    uVar2 = fightTargetGetPtrAsNowFightType(0x11, 0);
    uVar2 = (int)pokemonGetStatus(uVar2, 0, 0xd9, 0);
    statusValue = lbl_8047B60C;
    uVar1 = statusValue;
    uVar3 = wazaGetStatus(0, uVar1, 9, 0);
    if (*(char*)(lbl_8047B610 + 1) == 0) {
        goto zero_case;
    }
    fightWazaSetUseWazaStatus(uVar2, uVar1);
    goto done;
zero_case:
    wazaSetStatus(uVar2, 0, 0x27, 0, uVar1 & 0xffff);
    fightWazaSetUseWazaStatus(uVar2, uVar1);
done:
    lbl_8047B610 = (u8*)lbl_80379BFF[((s16)uVar3) & 0xffff];
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
u8* fn_8021E6CC(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 2;
    return pc;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
u8* fn_8021E6DC(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 2;
    return pc;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
u8* fn_8021E6EC(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 2;
    return pc;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma dont_inline on
void WS_SWITCH_A_D(void) {
    extern void fightFloorReplaceFightOutPokemonAttackToDefense();
    extern u32 lbl_8047B618;
    u32 uVar1;
    u32 uVar2;
    fightFloorReplaceFightOutPokemonAttackToDefense(0);
    uVar1 = lbl_8047B618;
    uVar2 = uVar1 | 0x1000;
    if ((uVar1 & 0x1000) != 0) {
        uVar2 = uVar1 & 0xffffefff;
    }
    lbl_8047B610 = lbl_8047B610 + 1;
    lbl_8047B618 = uVar2;
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
u8* fn_8021E744(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 2;
    return pc;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021E754(void) {
    extern u32 fightFloorGetStatus();
    extern u32 fightFloorGetGcHeroFightTrainerPtr();
    extern u32 fightTrainerGetStatus(u32, u32, u32, u16);
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u8* fightTrainerKindDataBiosGetPtr(u16 idx);
    extern u16 fightTrainerKindDataBiosGetSyoukinBairitu(u8 * ptr);
    extern u8 fightPokemonCheckValid();
    extern u32 figthPokemonGetLevel();
    extern u32 pokemonGetStatus();
    extern void scriptAddPremium();
    extern void heroAddPokedoru();
    extern void msgctrlSetValue();
    extern u32 heroGetStatus();
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
    a26 = (u16)fightFloorGetStatus(0, 0, 0x18, 0);
    if ((u8)fightFloorGetStatus(0, 0, 0x25, 0) == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }
    ctx1 = fightFloorGetGcHeroFightTrainerPtr(0);
    if (ctx1 == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }
    field44 = fightTrainerGetStatus(ctx1, 0, 0x44, 0);
    if (field44 == 0) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }
    field48 = (u8)fightTrainerGetStatus(ctx1, 0, 0x48, 0);
    ctx2 = fightTargetGetPtrAsNowFightType(9, ctx1);
    t43 = (u16)fightTrainerGetStatus(ctx2, 0, 0x43, 0);
    fieldVal = fightTrainerKindDataBiosGetSyoukinBairitu(
        fightTrainerKindDataBiosGetPtr((u16)fightTrainerGetStatus(0, t43, 4, 0)));
    best = 0;
    for (i = 0; i < 6; i++) {
        v1 = fightTrainerGetStatus(ctx2, 0, 0x45, i);
        if (fightPokemonCheckValid(v1)) {
            t = figthPokemonGetLevel(v1);
            if ((u8)best < (u8)t)
                best = t;
        }
    }
    for (i = 0; i < 6; i++) {
        v2 = fightTrainerGetStatus(ctx1, 0, 0x45, i);
        if (fightPokemonCheckValid(v2) && (s32)pokemonGetStatus(v2, 0, 0xcf, 0) == 1) {
            t = figthPokemonGetLevel(v2);
            if ((u8)best < (u8)t)
                best = t;
        }
    }
    if ((u8)fightFloorGetStatus(0, 0, 0x26, 0) == 0) {
        if (a26 <= 1) {
            mult = 1;
        } else {
            mult = 2;
        }
    } else {
        mult = 4;
    }
    amount = mult * (fieldVal * ((best & 0xFF) * field48));
    if ((u8)fightFloorGetStatus(0, 0, 0x26, 0) == 1) {
        scriptAddPremium(amount);
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
        return;
    }
    heroAddPokedoru(field44, amount);
    msgctrlSetValue(0x2f, amount);
    msgctrlSetValue(0x4b, amount);
    msgctrlSetValue(0x13, heroGetStatus(field44, 1, 0));
    lbl_8047B610 = lbl_8047B610 + 5;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021E9F4(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u8 fightWazaIsHit();
    u32 ctxA = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    u32 ctx2 = fightTargetGetPtrAsNowFightType(0x11, 0);
    u32 fieldD9 = pokemonGetStatus(ctx2, 0, 0xd9, 0);
    if (fightWazaIsHit(fieldD9) == 1) {
        if ((lbl_8047B618 & 0x100) != 0) {
            lbl_8047B610 = lbl_8047B610 + 2;
            return;
        } else if ((lbl_8047B618 & 0x80) != 0) {
            fightOutPokemonDamageEffect(ctxA);
        }
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021EA94(void) {
    extern u32 fightMenuYesNo();
    if ((u8)fightMenuYesNo() == 1) {
        lbl_8047B642++;
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma dont_inline on
void fn_8021EAE8(void) {
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
            choice = fightMenuWazaWasure(pokemon, lbl_8047B61C);
            if (choice == -1 || choice >= 4) {
                break;
            }
            move.value = pokemonGetStatus(pokemon, 0, 0x7f, choice);
            if (wazaGetStatus(0, move.value, 0x19, 0) == 1) {
                if (fightMenuOpenMsg(0x7635) == 1) {
                    fightMenuCloseMsg();
                }
            } else {
                pokemonWazaCreate(pokemon, choice, lbl_8047B61C);
                wazaGetStatus(0, move.value, 1, 0);
                text = GSmsgGetGSchar();
                msgctrlSetValue(0xe, text);
                msgctrlSetValue(0x5d, 0x468);
                lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
                return;
            }
        }
    }
    lbl_8047B610 += 5;
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma dont_inline on
void WS_WAZAOBOE_CHECK(void) {
    extern u32 pokemonGetOboeWazaDataBanme();
    extern s8 pokemonOboeWaza();
    extern u32 pokemonGetOboeWazaDataId();
    extern u32 figthPokemonGetLevel(u32);
    extern u32 fightPokemonGetPokemonPtr();
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
    uVar2 = figthPokemonGetLevel((u32)lbl_8047B64C);
    uVar1 = fightPokemonGetPokemonPtr(pokemon);
    pc = lbl_8047B610;
    uVar8 = *(u32*)(pc + 1);
    uVar7 = *(u32*)(pc + 5);
    if (*(char*)(pc + 9) != 0) {
        uVar4 = pokemonGetOboeWazaDataBanme(uVar1, uVar2);
        lbl_8047B642 = uVar4;
    }
    while (1) {
        cVar5 = pokemonOboeWaza(uVar1, uVar2, 0, &lbl_8047B642);
        if (cVar5 == -1) {
            uVar3 = pokemonGetOboeWazaDataId(uVar1, uVar2, &lbl_8047B642);
            lbl_8047B61C = uVar3;
            lbl_8047B610 += 10;
            return;
        }
        if (cVar5 != -2)
            break;
        lbl_8047B642 = (char)lbl_8047B642 + 1;
    }
    if (cVar5 == -3) {
        lbl_8047B610 = (u8*)uVar7;
        return;
    }
    lbl_8047B642 = (char)lbl_8047B642 + 1;
    uVar3 = (int)pokemonGetStatus(uVar1, 0, 0x7f, cVar5);
    lbl_8047B61C = uVar3;
    lbl_8047B610 = (u8*)uVar8;
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021ECF8(void) {
    extern s32 wazaGetStatus();
    extern u32 GSmsgGetGSchar();
    extern u32 fightPokemonGetNicknamePtr();
    extern void msgctrlSetValue();
    u32 val = (u32)lbl_8047B64C;
    u32 t = GSmsgGetGSchar(wazaGetStatus(0, lbl_8047B61C, 1, 0));
    u32 result = fightPokemonGetNicknamePtr(val);
    msgctrlSetValue(0xd, result);
    msgctrlSetValue(0xe, t);
    lbl_8047B610 = lbl_8047B610 + 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021ED70(void) {
    extern u8 fightOutPokemonCheckFightOut();
    extern void fightOutPokemonModosuEffect();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    u32 ctx;
    u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    ctx = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    if (fightOutPokemonCheckFightOut(ctx) == 1) {
        fightOutPokemonModosuEffect(ctx, 0);
        fn_80265598(ctx, val, 1);
        fightOutPokemonModosuEffect(ctx, 1);
        fightOutPokemonModosuEffect(ctx, 2);
        fightOutPokemonModosuEffect(ctx, 3);
        fightOutPokemonModosuEffect(ctx, 4);
        fn_8026532C(ctx, val, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021EE38(void) {
    lbl_8047B610 = lbl_8047B610 + 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021EE48(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern int fightOutPokemonGetVoiceSndId();
    u32 ctx = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    fn_80166A50((u16)fightOutPokemonGetVoiceSndId(ctx), 0, 0xff, 0);
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021EE98(void) {
    fn_80165668(*(u16*)(lbl_8047B610 + 1), 0, 0xff);
    lbl_8047B610 = lbl_8047B610 + 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021EED4(void) {
    fn_80166A50(*(u16*)(lbl_8047B610 + 1), 0, 0xff, 0);
    lbl_8047B610 = lbl_8047B610 + 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021EF14(void) {
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma dont_inline on
void fn_8021EF24(void) {
    extern void wazaSetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorCreateFightPokemonEnemyAryEnemySideAll();
    extern void fightFloorRegistFightTrainerEnemyPokemonFightSideAll();
    extern void fightFloorSetMeetEnemyFightPokemonEnemySideAll();
    extern void fightFloorSetStatus();
    extern short fightSideGetCountAsJoutaiDataId();
    extern s8 fightSideIsJoutaiDataId();
    extern void fightSideSetStatus();
    extern int fightSideGetStatus();
    extern u32 fightOutPokemonMaxHpWaruValue();
    extern s8 fightOutPokemonIsZokuseiDataId();
    extern void fightWazaInitJoutai();
    extern void fightWazaBiosCopy();
    extern void fn_80211B94();
    extern void fn_8022D084();
    extern void fn_8022E410();
    extern void fn_8022E6F0();
    extern u32 fightOutPokemonGetTokuseiDataId();
    extern void pokemonSetStatus(void*, u32, u32, u32, u32);
    extern s32 pokemonGetStatus();
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
    u8 auStack_c8[184];
    uVar2 = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    uVar3 = fightTargetGetPtrAsNowFightType(2, uVar2);
    sVar6 = fightOutPokemonGetTokuseiDataId(uVar2);
    fightFloorSetMeetEnemyFightPokemonEnemySideAll(0);
    fightFloorRegistFightTrainerEnemyPokemonFightSideAll(0);
    pokemonSetStatus((void*)uVar2, 0, 0x119, 0, 0);
    pokemonSetStatus((void*)uVar2, 0, 0x121, 0, 0xffffffff);
    iVar4 = fightSideGetStatus(uVar3, 0, 8, 0);
    if ((((iVar4 == 0) && (cVar8 = fightSideIsJoutaiDataId(uVar3, 0x4a), cVar8 == 1)) &&
         (cVar8 = fightOutPokemonIsZokuseiDataId(uVar2, 2), cVar8 == 0)) &&
        ((u16)sVar6 != 0x1a)) {
        fightSideSetStatus(uVar3, 0, 8, 0, 1);
        uVar7 = fightOutPokemonMaxHpWaruValue(
            uVar2, (5 - (short)fightSideGetCountAsJoutaiDataId(uVar3, 0x4a)) * 2 & 0xfffe);
        uVar5 = (int)pokemonGetStatus(fightTargetGetPtrAsNowFightType(0x11, 0), 0, 0xd9, 0);
        fightWazaBiosCopy(auStack_c8, uVar5);
        wazaSetStatus(uVar5, 0, 0x2d, 0, uVar7);
        fightWazaInitJoutai(uVar5);
        fightFloorSetStatus(0, 0, 0x4b, 0, uVar2);
        cVar8 = *(char*)(lbl_8047B610 + 1);
        if (cVar8 == 18) {
            uVar1 = (u32)lbl_80378DAF;
        } else if ((cVar8 == 17) || (cVar8 == 20)) {
            uVar1 = (u32)lbl_80378D7C;
        } else {
            uVar1 = (u32)lbl_80378DE2;
        }
        fn_80211B94(lbl_8047B62C, uVar1, 0);
        fightWazaBiosCopy(uVar5, auStack_c8);
        iVar4 = fightSideGetStatus(uVar3, 0, 8, 0);
        if (iVar4 == 0) {
            lbl_8047B610 = lbl_8047B610 + 2;
            return;
        }
    }
    sVar6 = fightOutPokemonGetTokuseiDataId(uVar2);
    if ((u16)sVar6 == 0x36) {
        pokemonSetStatus((void*)uVar2, 0, 0xf9, 0, 1);
    }
    fn_8022E6F0(uVar2, 0);
    fn_8022E410(uVar2);
    fn_8022D084(uVar2);
    fn_8022E6F0(uVar2, 1);
    fightFloorCreateFightPokemonEnemyAryEnemySideAll(0);
    fightSideSetStatus(uVar3, 0, 8, 0, 0);
    lbl_8047B610 = lbl_8047B610 + 2;
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021F1CC(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightOutPokemonGetNicknamePtr();
    extern u32 fightOutPokemonGetNamePtr();
    extern void msgctrlSetValue();
    void* ctx;
    u8 byte;
    u8 arg1 = *(u8*)(lbl_8047B610 + 1);
    ctx = (void*)fightTargetGetPtrAsNowFightType(arg1, 0);
    byte = *(u8*)(lbl_8047B610 + 2);
    switch (byte) {
        case 0:
        case 1:
            break;
        case 2:
            msgctrlSetValue(0xd, fightOutPokemonGetNicknamePtr(ctx));
            msgctrlSetValue(0xe, fightOutPokemonGetNamePtr(ctx));
            break;
    }
    lbl_8047B610 += 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma dont_inline on
void fn_8021F24C(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern void fightFloorSetTuusinErrorFightResult();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern void fightFloorSetStatus();
    extern u32 fightFloorGetStatus();
    extern int fn_801F8A18();
    extern short fightTrainerSelectIrekaeFightPokemon();
    u32 uVar2;
    u32 uVar7;
    u32 uVar1;
    u16 uVar4;
    u8 cVar6;
    u32 iVar3;
    short sVar5;
    u8 targetType;
    u16 local_18[4];
    uVar4 = fightFloorGetStatus(0, 0, 0x14, 0);
    targetType = *(volatile u8*)(lbl_8047B610 + 1);
    uVar7 = *(volatile u32*)(lbl_8047B610 + 2);
    uVar1 = fightTargetGetPtrAsNowFightType(targetType, 0);
    uVar2 = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, uVar1);
    cVar6 = (int)pokemonGetStatus(uVar1, 0, 0x119, 0);
    if (cVar6 == 1) {
        lbl_8047B610 = lbl_8047B610 + 6;
    } else {
        local_18[0] = 0;
        iVar3 = fn_801F8A18(uVar2, local_18);
        if (iVar3 == 0) {
            lbl_8047B610 = (u8*)uVar7;
        } else {
            sVar5 = fightTrainerSelectIrekaeFightPokemon(uVar2, uVar4, 0, uVar1);
            if (sVar5 < 0) {
                if (sVar5 == -2) {
                    fightFloorSetTuusinErrorFightResult(0);
                }
                lbl_8047B610 = (u8*)uVar7;
            } else {
                fightFloorSetStatus(0, 0, 0x45, 0, uVar1);
                pokemonSetStatus((void*)uVar1, 0, 0x121, 0, (int)sVar5);
                pokemonSetStatus((void*)uVar1, 0, 0x119, 0, 1);
                lbl_8047B610 = lbl_8047B610 + 6;
            }
        }
    }
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021F39C(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u8 fightFloorCheckFightActionFightOutPokemonIrekaeSelect();
    u32 ctxA = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    u32 result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ctxA);
    u16 buf;
    if (*(u8*)(lbl_8047B610 + 6) == 1 &&
        fightFloorCheckFightActionFightOutPokemonIrekaeSelect(0, ctxA, 0) == 1) {
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

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021F458(void) {
    extern u32 fightFloorGetStatus();
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern u32 fightTrainerGetStatus();
    extern u8 fn_802026E4();
    u32 ctx1;
    u32 trainerData;
    u32 result;
    u16 val;
    u8 mode;
    u8 slot;
    val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    ctx1 = fightTargetGetPtrAsNowFightType(
        (slot = *(u8*)(lbl_8047B610 + 1), mode = *(u8*)(lbl_8047B610 + 2), slot), 0);
    pokemonGetStatus(ctx1, 0, 0xd5, 0);
    result = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ctx1);
    trainerData = fightTrainerGetStatus(result, 0, 0x47, 0);
    if (mode != 2) {
        fightTrainerBallThrowEffect(result, ctx1, 2);
    }
    if ((u8)fightOutPokemonIsGcHeroFightOutPokemon(ctx1) == 0) {
        fn_80265598(ctx1, val, 0);
    } else {
        fn_80265598(ctx1, val, 1);
    }
    if (mode != 2) {
        fightOutPokemonDasuEffect(ctx1, 1);
        fightOutPokemonDasuEffect(ctx1, 2);
    }
    fightOutPokemonDasuEffect(ctx1, 3);
    fightOutPokemonDasuEffect(ctx1, 4);
    fn_8026532C(ctx1, val, 0);
    if (mode != 2) {
        fightTrainerBallThrowEffect(result, ctx1, 3);
    }
    fightOutPokemonDasuEffect(ctx1, 5);
    if (mode == 1 && (u8)fn_802026E4(ctx1, 0x14) == 1 &&
        fightOutPokemonGetJoutaiMigawariHp(ctx1) > 0) {
        fightMenuCloseMsg();
        fightWazaWzxTypeFuncMigawari(0xa4, ctx1, ctx1, 0, 0);
        for (;;) {
            if ((u8)fn_801DA5C4(6) == 1) {
                break;
            }
            _threadSwitch();
        }
        fightOutPokemonFreeAllSequenceWaza(ctx1);
    }
    if ((u8)fightFloorGetStatus(0, 0, 0x1e, 0) == 1 &&
        (u8)fightOutPokemonIsGcHeroFightOutPokemon(ctx1) == 0) {
        fightOutPokemonSetOnZukanFlag(ctx1, 0);
        fightOutPokemonSetOnDarkPokemonFlag(ctx1, 0);
    }
    fightOutPokemonInit(trainerData);
    lbl_8047B610 = lbl_8047B610 + 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma dont_inline on
#pragma optimize_for_size on
void fn_8021F664(void) {
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
        *(volatile u32*)(abilityDestination + 1) = *(volatile const u32*)(abilitySource + 1);
        *(volatile u16*)(abilityDestination + 2) = *(volatile const u16*)(abilitySource + 2);
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

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
void fn_8021F92C(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr();
    extern u32 fightTrainerGetStatus();
    u32 ctx = fightTargetGetPtrAsNowFightType(*(u8*)(lbl_8047B610 + 1), 0);
    fn_801FCEC4(
        fightTrainerGetStatus(fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, ctx), 0, 0x47, 0),
        ctx);
    lbl_8047B610 = lbl_8047B610 + 2;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8021F998(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightFloorGetStatus();
    extern u8 fn_802026E4();
    u32 ctx;
    u16 val = (u16)fightFloorGetStatus(0, 0, 0x14, 0);
    ctx = fightTargetGetPtrAsNowFightType(0x14, 0);
    if ((u8)fightOutPokemonCheckFightOut(ctx) == 1) {
        if ((u8)fn_802026E4(ctx, 0x14) == 1 && fightOutPokemonGetJoutaiMigawariHp(ctx) > 0) {
            fightOutPokemonWriteJoutaiDataId(ctx, 0x14);
            fightOutPokemonFreeAllSequenceWaza(ctx);
            fightWazaWzxTypeFuncMigawari(0xa4, ctx, ctx, 0, 0);
            for (;;) {
                if ((u8)fn_801DA5C4(6) == 1) {
                    break;
                }
                _threadSwitch();
            }
        }
        fightOutPokemonFreeAllSequenceWaza(ctx);
        fightOutPokemonModosuEffect(ctx, 0);
        fn_80265598(ctx, val, 1);
        fightOutPokemonModosuEffect(ctx, 1);
        fightOutPokemonModosuEffect(ctx, 2);
        fightOutPokemonModosuEffect(ctx, 3);
        fightOutPokemonModosuEffect(ctx, 4);
        fn_8026532C(ctx, val, 0);
    }
    lbl_8047B610 = lbl_8047B610 + 1;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma opt_propagation reset
