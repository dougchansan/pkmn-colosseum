/**
 * @file fight_trainer_ai_waza_hit_exact_80253950.c
 * @brief Byte-exact fightTrainerAiWazaHit.cpp range, 0x80253950 - 0x802546E8.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

u32 fightTrainerAiWazaHit213(void* ctx, u32 slot, u32 param) {
    extern u32 tikeiDataBiosGetZokuseiDataId(u32);
    extern u32 fightFloorGetStatus(u32, u32, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    u32 value;
    u32 result;

    value = tikeiDataBiosGetZokuseiDataId(fightFloorGetStatus(0, 0, 0xf, 0) & 0xFFFF);
    result = fn_80237DBC(ctx, slot, value & 0xFF);
    result = result != 1;
    return result;
}

u32 fightTrainerAiWazaHit212(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235910(void*, u32);
    extern u32 fn_80235AA0(void);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_80235AA0();
    secondStatus = fn_80235910(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 1, 0x41) & 0xff) == 0) {
        if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 3, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

u32 fightTrainerAiWazaHit211(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235974(void*, u32);
    extern u32 fn_802359D8(void*, u32);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_802359D8(ctx, param1);
    secondStatus = fn_80235974(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 4, 0x41) & 0xff) == 0) {
        if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 5, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

int fightTrainerAiWazaHit210(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x39);
  return (uVar1 & 0xFF) != 1;
}

s32 fightTrainerAiWazaHit209(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit208(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235A3C(void*, u32);
    extern u32 fn_80235AA0(void*, u32);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_80235AA0(ctx, param1);
    secondStatus = fn_80235A3C(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 1, 0x41) & 0xff) == 0) {
        if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 2, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit207(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit206(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_80235974(void*, u32);
    extern u32 fn_80235A3C(void*, u32);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    u32 firstStatus;
    u32 secondStatus;

    firstStatus = fn_80235A3C(ctx, param1);
    secondStatus = fn_80235974(ctx, param1);
    if ((firstStatus & 0xff) >= 0xc) {
        if ((secondStatus & 0xff) >= 0xc) {
            return 0;
        }
    }
    if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 2, 0x41) & 0xff) == 0) {
        if ((fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 5, 0x41) & 0xff) == 0) {
            return 0;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit205(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235AA0(void* ctx, u32 elem);
    extern u8 fn_80235A3C(void* ctx, u32 elem);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    u8 a = fn_80235AA0(ctx, param3);
    u8 b = fn_80235A3C(ctx, param3);

    if (a == 0 && b == 0) {
        return 0;
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 1, 1) != 0) {
        goto ret1;
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 2, 1) != 0) {
        goto ret1;
    }
    return 0;
ret1:
    return 1;
}

s32 fightTrainerAiWazaHit204(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit203(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235B04(void* ctx, u32 zero, u32 one);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u8 state = fn_80235B04(ctx, 0, 1);
    s32 gate;
    u32 v1;

    if (state == 2) {
        v1 = 0xb;
    } else if (state == 3) {
        v1 = 0x5;
    } else if (state == 1) {
        v1 = 0xa;
    } else if (state == 4) {
        v1 = 0xf;
    } else {
        v1 = 0;
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

s32 fightTrainerAiWazaHit202(void* ctx, u32 param1, u32 param2, u32 param3) {
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

int fightTrainerAiWazaHit201(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  u32 uVar1;
  uVar1 = fn_80236BFC(r3, r4, 0x38);
  return (uVar1 & 0xFF) != 1;
}

s32 fightTrainerAiWazaHit200(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit199(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32 type, u32 elem);
    extern u8 fightSideIsJoutaiDataId(u32 a, u32 b);
    extern u8 fn_80236BFC(void* ctx, u32 elem, u32 type);
    extern u8 fn_80237F74(void* ctx, u32 elem, u32 type);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u32 side = fightTargetGetPtrAsNowFightType(2, param3);
    s32 gate;
    u8 flag;

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
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x9) == 1) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
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

s32 fightTrainerAiWazaHit198(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit197(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit196(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit195(void) { return 1; }

int fightTrainerAiWazaHit194(u32 r3, u32 r4, u32 r5, u32 r6)

{
  extern u32 fn_80236BFC();
  u32 uVar1;

  uVar1 = fn_80236BFC(r3,r6,0x28);
  return (uVar1 & 0xff) != 1;
}

u32 fightTrainerAiWazaHit193(void)

{
    extern u32 fn_8023720C();
  u32 uVar1;

  uVar1 = fn_8023720C();
  uVar1 = __cntlzw(1 - (uVar1 & 0xff));
  return uVar1 >> 5 & 0xff;
}
