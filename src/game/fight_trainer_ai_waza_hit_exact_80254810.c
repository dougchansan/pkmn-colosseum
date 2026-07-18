/**
 * @file fight_trainer_ai_waza_hit_exact_80254810.c
 * @brief Byte-exact fightTrainerAiWazaHit.cpp range, 0x80254810 - 0x80256ED0.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

s32 fightTrainerAiWazaHit191(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 one);
    extern u8 fn_80229934(u32 param2, u32 param1, u32 param3);
    extern u8 fn_80237F74(void* ctx, u32 a, u32 type);
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0xFFFF);

    if (fn_80229934(param2, param1, param3) == 1) {
        return 0;
    }
    if (fn_80237F74(ctx, param1, 0x19) == 1) {
        return 0;
    }
    if (fn_80237F74(ctx, param3, 0x19) == 1) {
        return 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit190(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit189(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u16 fn_802376EC(void* ctx, u32 elem);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u32 v1 = fn_802395C8(ctx, param2, param1);
    u16 a = fn_802376EC(ctx, param1);
    u16 b = fn_802376EC(ctx, param3);
    s32 gate;

    if (b <= a) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit188(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit187(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74(void* ctx, u32 elem, u32 type);
    extern u8 fn_80236BFC(void* ctx, u32 elem, u32 type);
    extern u32 fightTargetGetPtrAsNowFightType(u32 type, u32 elem);
    extern u8 fightSideIsJoutaiDataId(u32 a, u32 b);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckSawagu(void* ctx, u32 a);
    extern u8 fn_80237310(void* ctx, u32 a);
    extern u8 fn_80229704(u32 type, u32 elem);
    s32 gate;

    if (fn_80237F74(ctx, param3, 0x48) == 1) {
        return 0;
    }
    if (fn_80237F74(ctx, param3, 0xf) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fightSideIsJoutaiDataId(fightTargetGetPtrAsNowFightType(2, param3), 0x4b) == 1) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fightTrainerAiCheckSawagu(ctx, param3) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x26) == 1) {
        return 0;
    }
    if (fn_80237310(ctx, param3) == 0) {
        return 0;
    }
    if (fn_80229704(8, param3) != 1) {
        return 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit186(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit185(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

u32 fightTrainerAiWazaHit184(void)

{
    extern u16 fn_80236B98();
  u16 sVar1;

  sVar1 = fn_80236B98();
  return sVar1 != 0;
}

u32 fightTrainerAiWazaHit183(void) { return 1; }

s32 fightTrainerAiWazaHit182(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

int fightTrainerAiWazaHit181(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x25);
  return (uVar1 & 0xFF) != 1;
}

u32 fightTrainerAiWazaHit180(u32 r3, u32 r4)
{
    extern u32 fn_80215008();
    u8 auStack_38[0x38];

    u32 uVar1 = fn_80215008(r3, auStack_38, 0x18, r4);
    return (-uVar1 & ~uVar1) >> 0x1f;
}

int fightTrainerAiWazaHit179(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x35);
  return (uVar1 & 0xFF) != 1;
}

u32 fightTrainerAiWazaHit178(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u8 fn_80237F74(void*, u32, u32);
    extern s32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    s32 gate;

    gate = _fightTrainerAiWazaHitCheck(ctx, slot, param, extra, 0xFFFF);
    if (fn_80237F74(ctx, extra, 0x19) == 1) {
        return 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit177(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_802383A4(void* ctx, u32 elem);
    extern u8 fn_80216048(u32 elem);
    extern u8 fn_80142984(u16 elem);
    extern u8 fn_80237F74(void* ctx, u32 a, u32 type);
    u16 A = fn_802383A4(ctx, param1);
    u16 B = fn_802383A4(ctx, param3);

    if (fn_80216048(param1) == 0) {
        return 0;
    }
    if (A == 0 && B == 0) {
        goto ret0;
    }
    if (A == 0xaf) {
        goto ret0;
    }
    if (B == 0xaf) {
        goto ret0;
    }
    if (A != 0 && fn_80142984(A) == 0) {
        goto ret0;
    }
    if (B != 0 && fn_80142984(B) == 0) {
    ret0:
        return 0;
    }
    if (fn_80237F74(ctx, param3, 0x3c) == 1) {
        return 0;
    }
    return 1;
}

u32 fightTrainerAiWazaHit176(void* ctx, u32 slot, u32 param) {
    extern u32 fightTargetGetPtrAsNowFightType(u32);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u8 fightOutPokemonCheckFightOut(u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    u32 target;

    target = fightTargetGetPtrAsNowFightType(0xe);
    if (target == 0) {
        return 0;
    }
    if ((fightFloorGetStatus(0, 0, 0x19, 0) & 0xFFFF) < 2) {
        goto fail;
    }
    if (fightOutPokemonCheckFightOut(target) != 1) {
        goto fail;
    }
    if (fn_80236BFC(ctx, slot, 0x32) != 0) {
        goto fail;
    }
    if (fn_80236BFC(ctx, target, 0x32) == 0) {
        goto success;
    }
fail:
    return 0;
success:
    return 1;
}

u32 fightTrainerAiWazaHit175(void* ctx, u32 slot, u32 param, u32 param3) {
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    u32 result;

    result = _fightTrainerAiWazaHitCheck(ctx, slot, param, param3, 0);
    if ((fn_80236BFC(ctx, param3, 0x30) & 0xff) == 1) {
        return 0;
    }
    if ((s32)result == 0) {
        return 0;
    }
    if ((s32)result == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit174(void) { return 1; }

u32 fightTrainerAiWazaHit173(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u32 wazaGetStatus(u32, u32, u32, u32);
    extern u32 tikeiDataBiosGetWazaId(u32);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u32 fn_8023C530(void*, u32, u32, u32);
    u32 paramType;
    u32 other;
    u32 otherType;

    other = tikeiDataBiosGetWazaId(fightFloorGetStatus(0, 0, 0xf, 0) & 0xFFFF);
    paramType = wazaGetStatus(0, param, 9, 0) & 0xFFFF;
    param = other;
    otherType = wazaGetStatus(0, param, 9, 0) & 0xFFFF;
    if (otherType != paramType) {
        return fn_8023C530(ctx, slot, param, extra);
    }
    return 1;
}

u32 fightTrainerAiWazaHit172(void) { return 1; }

s32 fightTrainerAiWazaHit171(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit170(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit169(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit168(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235AA0(void* ctx, u32 elem);
    extern u8 fn_802359D8(void* ctx, u32 elem);
    extern u8 fightTrainerAiCheckGuard(void* ctx, u32 param3, u32 param2);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    u8 q1 = fn_80235AA0(ctx, param3);
    u8 q2 = fn_802359D8(ctx, param3);

    if (fightTrainerAiCheckGuard(ctx, param3, param2) == 1) {
        return 0;
    }
    if (q1 == 0 && q2 == 0) {
        return 0;
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0xa0, 1, 1) == 0 &&
        fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0xa0, 4, 1) == 0) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    return 1;
}

u32 fightTrainerAiWazaHit167(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC();
    extern u8 fn_80237DBC();
    extern u8 fn_80237F74();
    extern u8 fn_80237310();
    extern s32 _fightTrainerAiWazaHitCheck();
    extern s32 fightTargetGetPtrAsNowFightType();
    extern u8 fightSideIsJoutaiDataId();
    s32 handle;
    u8 flag;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) return 0;
    if (fn_80236BFC(ctx, param3, 0x6) == 1) return 0;
    if (fn_80237DBC(ctx, param3, 0xA) == 1) return 0;
    if (fn_80237F74(ctx, param3, 0x29) == 1) return 0;

    if (fn_80237F74(ctx, param3, 0x11) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x14) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x7) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0xF) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x48) == 1) goto flag1;
    if (fn_80237F74(ctx, param3, 0x29) == 1) {
        flag = 0;
        goto check;
    }
    if (fn_80237F74(ctx, param3, 0x28) != 1) {
        fn_80237F74(ctx, param3, 0xC);
    }
flag1:
    flag = 1;
check:
    if (flag == 0) return 0;

    if (fn_80237310(ctx, param3) == 0) return 0;

    handle = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fightSideIsJoutaiDataId(fightTargetGetPtrAsNowFightType(2, param3), 0x4B) == 1) return 0;
    if (handle == 0) return 0;
    if (handle == -1) return 1;
    return 1;
}

s32 fightTrainerAiWazaHit166(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32 type, u32 elem);
    extern u8 fightSideIsJoutaiDataId(u32 a, u32 b);
    extern u8 fn_80235910(void* ctx, u32 elem);
    extern u8 fn_80236BFC(void* ctx, u32 elem, u32 type);
    extern u8 fn_80237F74(void* ctx, u32 elem, u32 type);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    u32 side = fightTargetGetPtrAsNowFightType(2, param3);
    u8 abiVal = fn_80235910(ctx, param3);
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    u8 flag;

    if (fn_80236BFC(ctx, param3, 0x9) == 1 && abiVal >= 0xc) {
        if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 4, 1) == 0) {
            return 0;
        }
    }
    if (fn_80237F74(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80237F74(ctx, param3, 0x11) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0x14) == 1) {
        flag = 0;
        goto flagCheckDone;
    }
    if (fn_80237F74(ctx, param3, 0x7) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0xf) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0x48) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0x29) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0x28) == 1) goto allowedLabel;
    fn_80237F74(ctx, param3, 0xc);
allowedLabel:
    flag = 1;
flagCheckDone:
    if (flag == 0) {
        return 0;
    }
    if (fightSideIsJoutaiDataId(side, 0x4b) == 1) {
        return 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit165(void* ctx, u32 slot, u32 param, u32 param3) {
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    u32 result;

    result = _fightTrainerAiWazaHitCheck(ctx, slot, param, param3, 0);
    if ((fn_80236BFC(ctx, param3, 0x1b) & 0xff) == 1) {
        return 0;
    }
    if ((s32)result == 0) {
        return 0;
    }
    if ((s32)result == -1) {
        return 1;
    }
    return 1;
}

int fightTrainerAiWazaHit164(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;

  uVar1 = fn_80235B04(r3,0,0);
  return (uVar1 & 0xff) != 4;
}

u32 fightTrainerAiWazaHit162(void* ctx, u32 slot, u32 param) {
    extern f32 lbl_8047E648;
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);

    if (fn_80236BFC(ctx, slot, 0x2d) == 0) {
        return 0;
    }
    return fn_802373B0(ctx, slot, 0, lbl_8047E648) != 1;
}

s32 fightTrainerAiWazaHit161(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u8 fn_80236BFC(void* ctx, u32 param1, u32 flag);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_80236BFC(ctx, param1, 0x2d) == 0) {
        gate = 0;
    }
    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit160(void* ctx, u32 slot, u32 param) {
    extern u8 fn_80236BFC(void* ctx, u32 slot, u32 param);
    extern u32 fn_80202360(u32 slot, u32 param);
    extern u8 fn_80119DD0(u32 param);
    u32 result;

    result = slot;
    if (fn_80236BFC(ctx, slot, 0x2d) == 0) {
        result = 0;
    } else {
        result = fn_80202360(result, 0x2d);
    }
    if ((s16)result >= (s32)fn_80119DD0(0x2d)) {
        return 0;
    }
    return 1;
}

s32 fightTrainerAiWazaHit159(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit158(u32 r3_in, u32 r4)
{
    extern u32 pokemonGetStatus();
  u32 uVar1;
  u8 result;
  uVar1 = pokemonGetStatus(r4, 0, 0xed, 0) & 0xFFFF;
  result = uVar1 != 0;
  return result;
}

u32 fightTrainerAiWazaHit157(void* ctx, u32 slot, u32 param2, u32 param3) {
    extern const f32 lbl_8047E648;
    extern u8 fn_802373B0(void*, u32, s32, f32);

    return fn_802373B0(ctx, slot, 0, lbl_8047E648) != 1;
}

u32 fightTrainerAiWazaHit156(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u32 fightTrainerAiCheckAbiCnt();
    u32 result = fightTrainerAiCheckAbiCnt(ctx, slot, arg3, param, 0x10, 0x2, 0x41) & 0xFF;
    return (result != 0) ? 1 : 0;
}

s32 fightTrainerAiWazaHit155(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit154(u32 r3, u32 r4, u32 r5, u32 r6)
{
    extern int _fightTrainerAiWazaHitCheck();
  int iVar1;
  iVar1 = _fightTrainerAiWazaHitCheck(r3, r4, r5, r6, 0);
  if (iVar1 == 0) {
    return 0;
  }
  if (iVar1 == -1) {
    return 1;
  }
  return 1;
}

u32 fightTrainerAiWazaHit153(void) { return 0; }

s32 fightTrainerAiWazaHit152(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit151(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit150(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit149(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

int fightTrainerAiWazaHit148(u32 r3, u32 r4, u32 r5, u32 r6)

{
  extern u32 fn_80236BFC();
  u32 uVar1;

  uVar1 = fn_80236BFC(r3,r6,0x34);
  return (uVar1 & 0xff) != 1;
}

s32 fightTrainerAiWazaHit147(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit146(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit145(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit144(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit143(void) { return 1; }

u32 fightTrainerAiWazaHit142(void* ctx, u32 slot, u32 param) {
    extern f32 lbl_8047E64C;
    extern u32 fn_80235AA0(void);
    extern u32 fn_802373B0(void*, u32, s32, f32);

    if ((fn_80235AA0() & 0xFF) >= 0xC) {
        goto ret0;
    }
    if ((fn_802373B0(ctx, slot, -1, lbl_8047E64C) & 0xFF) == 0) {
        goto ret1;
    }
ret0:
    return 0;
ret1:
    return 1;
}

s32 fightTrainerAiWazaHit140(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit139(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit138(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

int fightTrainerAiWazaHit137(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;

  uVar1 = fn_80235B04(r3,0,0);
  return (uVar1 & 0xff) != 1;
}

int fightTrainerAiWazaHit136(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;

  uVar1 = fn_80235B04(r3,0,0);
  return (uVar1 & 0xff) != 2;
}

s32 fightTrainerAiWazaHit135(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightOutPokemonGetPokemonPtr(u32 v);
    extern void pokemonGetMezamerupower(u32, void*, void*);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, s16 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u16 hi;
    u16 lo;
    s32 gate;

    pokemonGetMezamerupower(fightOutPokemonGetPokemonPtr(param1), &hi, &lo);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fn_8023793C(ctx, param3, lo, (s16)hi) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit134(void* ctx, u32 slot, u32 param2, u32 param3) {
    extern const f32 lbl_8047E648;
    extern u8 fn_802373B0(void*, u32, s32, f32);

    return fn_802373B0(ctx, slot, 0, lbl_8047E648) != 1;
}

u32 fightTrainerAiWazaHit133(void* ctx, u32 slot, u32 param2, u32 param3) {
    extern const f32 lbl_8047E648;
    extern u8 fn_802373B0(void*, u32, s32, f32);

    return fn_802373B0(ctx, slot, 0, lbl_8047E648) != 1;
}

u32 fightTrainerAiWazaHit132(void* ctx, u32 slot, u32 param2, u32 param3) {
    extern const f32 lbl_8047E648;
    extern u8 fn_802373B0(void*, u32, s32, f32);

    return fn_802373B0(ctx, slot, 0, lbl_8047E648) != 1;
}

s32 fightTrainerAiWazaHit130(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit129(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

u32 fightTrainerAiWazaHit128(void* ctx, u32 a, u32 b, u32 c) {
    extern s32 fn_802395C8();
    extern s32 fn_80239500();
    extern s32 fn_8023793C();
    s32 res1;

    res1 = fn_802395C8(ctx, b, a);
    return !((u16)fn_8023793C(ctx, c, res1, fn_80239500(ctx, b)) == 0x43);
}

u32 fightTrainerAiWazaHit127(void)
{
    extern int fn_801F8A18();
  u32 r3;
  int iVar1;
  u16 local_8[4];

  local_8[0] = 0;
  iVar1 = fn_801F8A18(r3, local_8);
  return (-iVar1 != 0) ? 1 : 0;
}

s32 fightTrainerAiWazaHit126(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit125(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

u32 fightTrainerAiWazaHit124(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightSideCheckWriteJoutaiDataId();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fightTargetGetPtrAsNowFightType(2);
  uVar2 = fightSideCheckWriteJoutaiDataId(uVar1,0x4b);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}

s32 fightTrainerAiWazaHit123(void* ctx, u32 param1, u32 param2, u32 param3) {
    s32 gate = wazaHitGateCheck(ctx, param1, param2, param3);
    s32 result;
    if (gate == 0) {
        result = 0;
    } else if (gate == -1) {
        result = 1;
    } else {
        result = 1;
    }
    return result;
}

s32 fightTrainerAiWazaHit122(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit121(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    s32 gate;
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}
