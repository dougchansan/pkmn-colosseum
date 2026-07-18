/**
 * @file fight_trainer_ai_waza_hit_exact_802570D0.c
 * @brief Byte-exact fightTrainerAiWazaHit.cpp range, 0x802570D0 - 0x8025C264.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

s32 fightTrainerAiWazaHit119(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit118(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32 type, u32 elem);
    extern u8 fightSideIsJoutaiDataId(u32 a, u32 b);
    extern u8 fn_80235AA0(void* ctx, u32 elem);
    extern u8 fn_80236BFC(void* ctx, u32 elem, u32 type);
    extern u8 fn_80237F74(void* ctx, u32 elem, u32 type);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    u32 side = fightTargetGetPtrAsNowFightType(2, param3);
    u8 abiVal = fn_80235AA0(ctx, param3);
    s32 gate;
    u8 flag;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fn_80236BFC(ctx, param3, 0x9) == 1 && abiVal >= 0xc) {
        if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x20, 1, 1) == 0) {
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

s32 fightTrainerAiWazaHit117(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit116(void) { return 1; }

int fightTrainerAiWazaHit115(void)

{
    u32 r3;

    extern u32 fn_80235B04();
  u32 uVar1;

  uVar1 = fn_80235B04(r3,0,0);
  return (uVar1 & 0xff) != 3;
}

u32 fightTrainerAiWazaHit114(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u32 fn_80229934();
    extern u32 fightTrainerAiCheckHorobinouta();

    if ((u8)fn_80229934(param, slot, arg3) == 1) {
        return 0;
    }
    return (u16)fightTrainerAiCheckHorobinouta(ctx) != 0;
}

u32 fightTrainerAiWazaHit113(u32 r3, u32 r4, u32 r5, u32 r6)
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

u32 fightTrainerAiWazaHit112(void* ctx, u32 slot, u32 param) {
    extern u8 fn_80119DD0(u32);
    extern u32 fightTargetGetPtrAsNowFightType(u32);
    extern u32 fightSideGetCountAsJoutaiDataId(u32, u32);
    extern u8 fightSideIsJoutaiDataId(u32, u32);
    u32 target;
    u32 count;

    target = fightTargetGetPtrAsNowFightType(3);
    count = 0;
    if (fightSideIsJoutaiDataId(target, 0x4a) == 1) {
        count = fightSideGetCountAsJoutaiDataId(target, 0x4a);
    }
    if ((s16)count >= (s32)fn_80119DD0(0x4a)) {
        return 0;
    }
    return 1;
}

u32 fightTrainerAiWazaHit111(void) { return 1; }

s32 fightTrainerAiWazaHit109(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80235AA0(void* ctx, u32 elem);
    extern u8 fn_80235A3C(void* ctx, u32 elem);
    extern u8 fn_80235910(void* ctx, u32 elem);
    extern u8 fn_80237DBC(void* ctx, u32 a, u32 type);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern u8 fn_80237288(void* ctx, u32 elem);
    extern u8 fightTrainerAiCheckAbiCnt(void*, u32, u32, u32, u32, u32, u32);
    u8 a = fn_80235AA0(ctx, param3);
    u8 b = fn_80235A3C(ctx, param3);
    u8 c = fn_80235910(ctx, param3);

    if (fn_80237DBC(ctx, param1, 7) == 1) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        if (fn_80236BFC(ctx, param3, 0x18) != 1) {
            if (fn_80237288(ctx, param3) != 1) {
                goto ret1;
            }
        }
        return 0;
    }

    if (((c == 0) && (a >= 0xc)) && (b >= 0xc)) {
        return 0;
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 3, 0x41) != 0) {
        goto ret1;
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 1, 0x41) != 0) {
        goto ret1;
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 2, 0x41) != 0) {
        goto ret1;
    }
ret0:
    return 0, 0;

    return 0;
ret1:
    return 1;
}

s32 fightTrainerAiWazaHit108(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 7, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

u32 fightTrainerAiWazaHit107(void* ctx, u32 param1, u32 param2, u32 extra) {
    extern u8 fn_80236BFC(void*, u32, u32);

    if (fn_80236BFC(ctx, extra, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, extra, 0x17) == 1) {
        return 0;
    }
    return fn_80236BFC(ctx, extra, 8) != 0;
}

u32 fightTrainerAiWazaHit106(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    s32 gate;

    gate = _fightTrainerAiWazaHitCheck(ctx, slot, param, extra, 0xFFFE);
    if (fn_80236BFC(ctx, extra, 0x16) == 1) {
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

s32 fightTrainerAiWazaHit105(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit104(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit103(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit102(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightTrainerFightPokemonPtrAry(u32 zero, void* ctx, void* buf, u32 one1, u32 one2);
    extern u8 fn_80239058(void* ctx, u32 elem, u32 type);
    extern u8 fn_80238748(void* ctx, u32 elem);
    u32 buf[0x17];
    u32 count = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, buf, 1, 1);
    u32 *bufPtr;
    u16 i;
    u8 flag;

    bufPtr = buf;
    flag = 0;
    for (i = 0; i < (u16)count; i++) {
        if (bufPtr[i] == 0) {
            continue;
        }
        if ((u16)param2 == 0xd7 && fn_80239058(ctx, bufPtr[i], 0x2b) == 1) {
            continue;
        }
        if (fn_80238748(ctx, bufPtr[i]) != 0) {
            continue;
        }
        flag = 1;
        break;
    }
    if (flag == 0) {
        return 0;
    }
    return 1;
}

s32 fightTrainerAiWazaHit101(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit100(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 one);
    extern u16 fn_802367CC(void* ctx, u32 param3, void* arrayA, void* arrayB, u32 flag);
    extern u32 fn_802364BC(void* ctx, u32 param3);
    extern u8 fn_80237288(void* ctx, u32 elem);
    extern u8 fn_802381C4(void* ctx, u32 param3, u8 v);
    u16 arrayA[0xe];
    s16 arrayB[0xa];
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0xFFFF);
    u16 count = fn_802367CC(ctx, param3, arrayA, arrayB, 0);
    u32 a = fn_802364BC(ctx, param3);
    u8 flag;
    u8 i;

    if ((u16)a == 0 || (u16)a == 0x165 || (u16)a == 0xffff || fn_80237288(ctx, param3) == 1) {
        return 0;
    }
    flag = 0;
    for (i = 0; i < count; i++) {
        if (arrayB[i] < 0) {
            continue;
        }
        if ((u16)a != arrayA[i]) {
            continue;
        }
        flag = fn_802381C4(ctx, param3, (u8)arrayB[i]);
        break;
    }
    if (flag == 0) {
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

s32 fightTrainerAiWazaHit099(void* ctx, u32 param1, u32 param2, u32 param3) {
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

int fightTrainerAiWazaHit098(u32 arg0, u32 arg1, u32 arg2, u32 arg3)
{
    extern u32 fn_80229934();

    return (u8)fn_80229934(arg2, arg1, arg3) != 1;
}

s32 fightTrainerAiWazaHit097(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_802367CC(void* ctx, u32 param1, void* buf, u32 zero, u32 one);
    extern u8 fn_80236BFC(void* ctx, u32 param1, u32 flag);
    extern u8 fightSeqRendouWazaCheck(u32 elem);
    extern u8 fn_8021901C(int elem);
    u16 buf[0xa];
    u16 count = fn_802367CC(ctx, param1, buf, 0, 1);
    u16 i;

    if (!fn_80236BFC(ctx, param1, 8)) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        u16 elem = buf[i];
        if (elem == 0x165 || elem == 0x163) {
            continue;
        }
        if (fightSeqRendouWazaCheck(elem) != 0) {
            continue;
        }
        if (elem == 0x108 || elem == 0xfd) {
            continue;
        }
        if (fn_8021901C(elem) == 0) {
            break;
        }
    }
    if (i >= count) {
        return 0;
    }
    return 1;
}

s32 fightTrainerAiWazaHit095(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802367CC(void* ctx, u32 param1, void* buf, u32 zero1, u32 zero2);
    extern u32 fn_80236458(void* ctx, u32 param3);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern u8 fn_80219270(u32 elem);
    u16 buf[0xa];
    u32 count = fn_802367CC(ctx, param1, buf, 0, 0);
    u32 target = fn_80236458(ctx, param3);
    u16 i;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param1, 0x10) == 1) {
        goto ret0;
    }
    if (fn_80219270(target) != 1) {
        goto scan;
    }
ret0:
    return 0;
scan:
    for (i = 0; i < (u16)count; i++) {
        if (buf[i] == (u16)target) {
            return 0;
        }
    }
    return 1;
}

u32 fightTrainerAiWazaHit094(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    s32 gate;

    gate = _fightTrainerAiWazaHitCheck(ctx, slot, param, extra, 0xFFFF);
    if (fn_80236BFC(ctx, extra, 0x14) == 1) {
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

u32 fightTrainerAiWazaHit093(void) { return 1; }

s32 fightTrainerAiWazaHit092(void* ctx, u32 param1, u32 param2, u32 param3) {
    u32 *new_var;
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u8 fn_80236BFC(void* ctx, u32 param1, u32 flag);
    extern u32 fn_80239500(void* ctx, u32 param2);
    s32 gate;
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u32 v1 = fn_802395C8(ctx, param2, param1);

    if (!fn_80236BFC(ctx, param1, 0x8)) {
        return 0;
    }
    new_var = &v1;
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fn_8023793C(ctx, param3, *new_var, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (((0, gate)) == (-1)) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit091(void* ctx, u32 slot, u32 param, u32 extra) {
    extern u8 fn_80236BFC(void*, u32, u32);
    extern s32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    s32 gate;

    gate = _fightTrainerAiWazaHitCheck(ctx, slot, param, extra, 0xFFFF);
    if (fn_80236BFC(ctx, extra, 0x14) == 1) {
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

s32 fightTrainerAiWazaHit090(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_802367CC(void* ctx, u32 param3, void* arrayA, void* arrayB, u32 flag);
    extern u32 fn_802364BC(void* ctx, u32 param3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightSeqIsEncoreNgWazaDataId(u32 elem);
    extern u8 fn_802381C4(void* ctx, u32 param3, u8 v);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    u16 arrayA[0xe];
    s16 arrayB[0xa];
    u16 count = fn_802367CC(ctx, param3, arrayA, arrayB, 0);
    u32 targetVal = fn_802364BC(ctx, param3);
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    u8 flag;
    u16 i;

    if (fightSeqIsEncoreNgWazaDataId(targetVal) == 1) {
        return 0;
    }
    flag = 0;
    for (i = 0; i < count; i++) {
        if (arrayB[i] < 0) {
            continue;
        }
        if ((u16)targetVal != arrayA[i]) {
            continue;
        }
        flag = fn_802381C4(ctx, param3, (u8)arrayB[i]);
        break;
    }
    if (flag == 0) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x2a) == 1) {
        return 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;

    return;
}

s32 fightTrainerAiWazaHit089(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit088(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit087(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit086(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_802367CC(void* ctx, u32 param3, void* arrayA, void* arrayB, u32 flag);
    extern u16 fn_802364BC(void* ctx, u32 param3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fn_802381C4(void* ctx, u32 param3, u8 v);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    u16 arrayA[0xe];
    s16 arrayB[0xa];
    u16 count = fn_802367CC(ctx, param3, arrayA, arrayB, 0);
    u16 target = fn_802364BC(ctx, param3);
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    u8 flag = 0;
    u16 i;

    for (i = 0; i < count; i++) {
        if (arrayB[i] < 0) {
            continue;
        }
        if (target != arrayA[i]) {
            continue;
        }
        flag = fn_802381C4(ctx, param3, (u8)arrayB[i]);
        break;
    }
    if (flag == 0) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x29) == 1) {
        return 0;
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;

    return;
}

u32 fightTrainerAiWazaHit085(void) { return 1; }

s32 fightTrainerAiWazaHit084(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern u8 fn_80237DBC(void* ctx, u32 a, u32 type);
    s32 gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);

    if (fn_80236BFC(ctx, param3, 0x1c) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0xc) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
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

u32 fightTrainerAiWazaHit083(void) { return 1; }

s32 fightTrainerAiWazaHit082(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u16 fn_802367CC(void* ctx, u32 elem, void* arrA, void* arrB, u32 flag);
    extern u32 fn_802364BC(void* ctx, u32 param3);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 one);
    extern u8 fightSeqMonomaneNGCheck(u32 elem);
    u16 buf[0x10];
    u16 count = fn_802367CC(ctx, param1, buf, 0, 0);
    u32 targetVal = fn_802364BC(ctx, param3);
    s32 gate;
    u16 i;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0xFFFF);
    if (fn_80236BFC(ctx, param1, 0x10) == 1 ||
        fightSeqMonomaneNGCheck(targetVal) != 0 ||
        (u16)targetVal == 0 || (u16)targetVal == 0xffff || (u16)targetVal == 0x165 || (u16)targetVal == 0x163) {
        return 0;
    }
    for (i = 0; i < count; i++) {
        if ((u16)targetVal == buf[i]) {
            return 0;
        }
    }
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit081(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit080(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit079(void* ctx, u32 slot, u32 param) {
    extern u32 fn_80236BFC(void*, u32, u32);
    extern u32 fn_802373B0(void*, u32, s32, f32);
    extern f32 lbl_8047E650;
    u32 result;

    if ((fn_80236BFC(ctx, slot, 0x14) & 0xff) == 1) {
        return 0;
    }

    result = fn_802373B0(ctx, slot, -1, lbl_8047E650) & 0xff;
    return result != 1;
}

s32 fightTrainerAiWazaHit078(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit077(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit076(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit075(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit073(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit072(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit071(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit070(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit069(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit068(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit067(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightTargetGetPtrAsNowFightType(u32 type, u32 elem);
    extern u8 fightSideIsJoutaiDataId(u32 a, u32 b);
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern u8 fn_80236BFC(void* ctx, u32 elem, u32 type);
    extern u8 fn_80237F74(void* ctx, u32 elem, u32 type);
    extern u8 fn_80237310(void* ctx, u32 elem);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u32 side = fightTargetGetPtrAsNowFightType(2, param3);
    u32 v1 = fn_802395C8(ctx, param2, param1);
    s32 gate;
    u8 flag;

    if (fn_80237F74(ctx, param3, 0x7) == 1) {
        return 0;
    }
    if (fn_80237F74(ctx, param3, 0x11) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0x14) == 1) goto allowedLabel;
    if (fn_80237F74(ctx, param3, 0x7) == 1) {
        flag = 0;
        goto flagCheckDone;
    }
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
    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x5) == 1) {
        return 0;
    }
    if (fn_80237310(ctx, param3) == 0) {
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

u8 fightTrainerAiWazaHit066(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    extern u8 fn_80237310(void*, u32);
    extern s32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern u8 fightSideIsJoutaiDataId(u32, u32);
    s32 r;
    u8 flag;

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x4) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x8) == 1) {
        return 0;
    }
    if (fn_80237310(ctx, param3) == 0) {
        return 0;
    }

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        flag = 0;
    } else {
        if (fn_80237F74(ctx, param3, 0x14) != 1) {
            if (fn_80237F74(ctx, param3, 0x7) != 1) {
                if (fn_80237F74(ctx, param3, 0xf) != 1) {
                    if (fn_80237F74(ctx, param3, 0x48) != 1) {
                        if (fn_80237F74(ctx, param3, 0x29) != 1) {
                            if (fn_80237F74(ctx, param3, 0x28) != 1) {
                                fn_80237F74(ctx, param3, 0xc);
                            }
                        }
                    }
                }
            }
        }
        flag = 1;
    }
    if (flag == 0) {
        return 0;
    }

    r = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fightSideIsJoutaiDataId(fightTargetGetPtrAsNowFightType(2, param3), 0x4b) == 1) {
        return 0;
    }
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit065(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightSideCheckWriteJoutaiDataId();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fightTargetGetPtrAsNowFightType(2);
  uVar2 = fightSideCheckWriteJoutaiDataId(uVar1,0x48);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}

s32 fightTrainerAiWazaHit062(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0xa0, 0x5, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit060(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0xa0, 0x3, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit059(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0xa0, 0x2, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit058(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0xa0, 0x1, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0xa0) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

int fightTrainerAiWazaHit057(void* ctx, u32 slot, u32 param, u32 arg3) {
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237288(void*, u32);

    if ((u8)fn_80236BFC(ctx, arg3, 0x10) == 1) {
        goto fail;
    }
    if ((u8)fn_80237288(ctx, arg3) != 1) {
        goto success;
    }
fail:
    return 0;
success:
    return 1;
}

s32 fightTrainerAiWazaHit054(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x20, 0x5, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit053(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x20, 0x4, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit052(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x20, 0x3, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit051(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x20, 0x2, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit050(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x20, 0x1, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x20) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit049(void* ctx, u32 param1, u32 param2, u32 param3) {
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
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x9) == 1) {
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

s32 fightTrainerAiWazaHit048(void* ctx, u32 param1, u32 param2, u32 param3) {
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

int fightTrainerAiWazaHit047(void)
{
    u32 r3;
    u32 r4;
    extern u32 fn_80236BFC();
  return (fn_80236BFC(r3, r4, 0xf) & 0xFF) != 1;
}

u32 fightTrainerAiWazaHit046(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightSideCheckWriteJoutaiDataId();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fightTargetGetPtrAsNowFightType(2);
  uVar2 = fightSideCheckWriteJoutaiDataId(uVar1,0x4c);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}

u32 fightTrainerAiWazaHit045(void* trainerCtx, u32 trainerSlot, u32 resultSlot, u32 resultType) {
    extern u16 fn_8023793C();
    extern u32 fn_80239500();
    extern u32 fn_802395C8();
    extern u32 _fightTrainerAiWazaHitCheck();
    s32 resultVal;
    u32 statusVal;
    statusVal = fn_802395C8(trainerCtx, resultSlot, trainerSlot);
    resultVal = _fightTrainerAiWazaHitCheck(trainerCtx, trainerSlot, resultSlot, resultType, 0);
    if (fn_8023793C(trainerCtx, resultType, statusVal, fn_80239500(trainerCtx, resultSlot)) == 0x43) {
        resultVal = 0;
    }
    if (resultVal == 0) {
        return 0;
    }
    if (resultVal == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit044(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit043(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit042(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit041(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit040(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit039(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit038(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237774(void* ctx, u32 elem);
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 one);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern u8 fn_80237F74(void* ctx, u32 a, u32 type);
    s32 gate;
    u8 a = fn_80237774(ctx, param1);
    u8 b = fn_80237774(ctx, param3);
    u32 v1 = fn_802395C8(ctx, param2, param1);
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0xFFFF);

    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        gate = 0;
    }
    if (fn_80237F74(ctx, param3, 5) == 1) {
        gate = 0;
    }
    if (a < b) {
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

u32 fightTrainerAiWazaHit037(void* ctx, u32 param1, u32 param2) {
    extern f32 lbl_8047E648;
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_802373B0(void*, u32, s32, f32);
    extern u8 fightTrainerAiCheckSawagu(void*, u32);

    if (fn_80236BFC(ctx, param1, 8) == 1) {
        return 0;
    }
    if (fightTrainerAiCheckSawagu(ctx, param1) == 1) {
        return 0;
    }
    return fn_802373B0(ctx, param1, 0, lbl_8047E648) != 1;
}

s32 fightTrainerAiWazaHit036(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit035(void)

{
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 fightSideCheckWriteJoutaiDataId();
  u32 uVar1;
  u32 uVar2;

  uVar1 = fightTargetGetPtrAsNowFightType(2);
  uVar2 = fightSideCheckWriteJoutaiDataId(uVar1,0x49);
  uVar2 = __cntlzw(2 - (uVar2 & 0xff));
  return uVar2 >> 5;
}

s32 fightTrainerAiWazaHit034(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u8 fightTrainerAiWazaHit033(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80237F74(void*, u32, u32);
    extern u8 fn_80236BFC(void*, u32, u32);
    extern u8 fn_80237DBC(void*, u32, u32);
    extern u8 fn_80237310(void*, u32);
    extern s32 _fightTrainerAiWazaHitCheck(void*, u32, u32, u32, u32);
    extern u32 fightTargetGetPtrAsNowFightType(u32, u32);
    extern u8 fightSideIsJoutaiDataId(u32, u32);
    s32 r;
    u8 flag;

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x4) == 1) {
        return 0;
    }
    if (fn_80237310(ctx, param3) == 0) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x3) == 1) {
        return 0;
    }
    if (fn_80237DBC(ctx, param3, 0x8) == 1) {
        return 0;
    }

    if (fn_80237F74(ctx, param3, 0x11) == 1) {
        flag = 0;
    } else {
        if (fn_80237F74(ctx, param3, 0x14) != 1) {
            if (fn_80237F74(ctx, param3, 0x7) != 1) {
                if (fn_80237F74(ctx, param3, 0xf) != 1) {
                    if (fn_80237F74(ctx, param3, 0x48) != 1) {
                        if (fn_80237F74(ctx, param3, 0x29) != 1) {
                            if (fn_80237F74(ctx, param3, 0x28) != 1) {
                                fn_80237F74(ctx, param3, 0xc);
                            }
                        }
                    }
                }
            }
        }
        flag = 1;
    }
    if (flag == 0) {
        return 0;
    }

    r = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fightSideIsJoutaiDataId(fightTargetGetPtrAsNowFightType(2, param3), 0x4b) == 1) {
        return 0;
    }
    if (r == 0) {
        return 0;
    }
    if (r == -1) {
        return 1;
    }
    return 1;
}

u32 fightTrainerAiWazaHit032(void* ctx, u32 slot, u32 param2, u32 param3) {
    extern const f32 lbl_8047E648;
    extern u8 fn_802373B0(void*, u32, s32, f32);

    return fn_802373B0(ctx, slot, 0, lbl_8047E648) != 1;
}

s32 fightTrainerAiWazaHit031(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit030(void)

{
    extern int fightTrainerAiCheckTextureZokusei();
  u8 cVar1;

  cVar1 = fightTrainerAiCheckTextureZokusei();
  return cVar1 != 0;
}

s32 fightTrainerAiWazaHit029(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit028(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fightFloorGetFightOutPokemonPtrToFightTrainerPtr(u32 zero, u32 param3);
    extern u8 fn_80237F74(void* ctx, u32 a, u32 type);
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u16 fn_801F87CC(u32 v, void* buf);
    u8 buf[0x14];
    u32 prev = fightFloorGetFightOutPokemonPtrToFightTrainerPtr(0, param3);
    s32 gate;

    if (fn_80237F74(ctx, param3, 0x15) == 1) {
        return 0;
    }
    if (fn_80236BFC(ctx, param3, 0x25) == 1) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fn_801F87CC(prev, buf) == 0) {
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

s32 fightTrainerAiWazaHit027(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit026(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit025(void) { return 1; }

s32 fightTrainerAiWazaHit024(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 0x7, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit023(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 0x6, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit020(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 0x3, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit019(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 0x2, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit018(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x90, 0x1, 0x1) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x90) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit017(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit016(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 0x7, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit013(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 0x4, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit011(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 0x2, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

s32 fightTrainerAiWazaHit010(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern s8 fightSeqCondChgActParaIdToValue(u32 id);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    extern u8 fightTrainerAiCheckAbiCnt(void* ctx, u32 param1, u32 param3, u32 param2, u32 a, u32 b, u32 c);
    s32 gate;

    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (fn_80236BFC(ctx, param3, 0x14) == 1) {
            return 0;
        }
        gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    }
    if (fightTrainerAiCheckAbiCnt(ctx, param1, param3, param2, 0x10, 0x1, 0x41) == 0) {
        return 0;
    }
    if (fightSeqCondChgActParaIdToValue(0x10) < 0) {
        if (gate == 0) {
            return 0;
        }
        if (gate == -1) {
            return 1;
        }
    }
    return 1;
}

u32 fightTrainerAiWazaHit009(void)

{
    extern u16 fightTrainerAiCheckOumu();
  u16 sVar1;

  sVar1 = fightTrainerAiCheckOumu();
  return sVar1 != 0;
}

s32 fightTrainerAiWazaHit008(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u8 fn_80236BFC(void* ctx, u32 param3, u32 flag);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u32 v1 = fn_802395C8(ctx, param2, param1);
    s32 gate;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) {
        return 0;
    }
    if (!fn_80236BFC(ctx, param3, 0x8)) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
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

s32 fightTrainerAiWazaHit007(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u32 fn_802395C8(void* ctx, u32 param2, u32 param1);
    extern u32 fightFloorGetFightTrainerFightPokemonPtrAry(u32 zero, void* ctx, void* buf, u32 one1, u32 one2);
    extern u8 fn_8022967C(u32 param2);
    extern u8 fightTrainerAiCheckSimerike(void* ctx);
    extern u32 fn_80239500(void* ctx, u32 param2);
    extern u16 fn_8023793C(void* ctx, u32 param3, u32 v1, u32 v3);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 zero);
    u8 buf[0x60];
    u32 v1 = fn_802395C8(ctx, param2, param1);
    u32 flinchCount = fightFloorGetFightTrainerFightPokemonPtrAry(0, ctx, buf, 1, 1);
    s32 gate;

    if (fn_8022967C(param2) == 1 && (u16)flinchCount <= 1) {
        return 0;
    }
    if (fightTrainerAiCheckSimerike(ctx) == 1) {
        return 0;
    }
    if (fn_8023793C(ctx, param3, v1, fn_80239500(ctx, param2)) == 0x43) {
        return 0;
    }
    gate = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (gate == 0) {
        return 0;
    }
    if (gate == -1) {
        return 1;
    }
    return 1;
}

s32 fightTrainerAiWazaHit006(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit005(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit004(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit003(void* ctx, u32 param1, u32 param2, u32 param3) {
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

s32 fightTrainerAiWazaHit002(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHit001(void* ctx, u32 param1, u32 param2, u32 param3) {
    extern u8 fn_80236BFC(void* ctx, u32 a, u32 type);
    extern u8 fightTrainerAiCheckSawagu(void* ctx, u32 a);
    extern u8 fn_80237310(void* ctx, u32 a);
    extern u8 fn_80237F74(void* ctx, u32 a, u32 type);
    extern s32 _fightTrainerAiWazaHitCheck(void* ctx, u32 p1, u32 p2, u32 p3, u32 z);
    extern u32 fightTargetGetPtrAsNowFightType(s32 a, u32 b);
    extern u8 fightSideIsJoutaiDataId(u32 a, u32 b);
    s32 result;
    u8 ok;

    if (fn_80236BFC(ctx, param3, 0x14) == 1) return 0;
    if (fn_80236BFC(ctx, param3, 0x8) == 1) return 0;
    if (fightTrainerAiCheckSawagu(ctx, param3) == 1) return 0;
    if (fn_80237310(ctx, param3) == 0) return 0;
    if (fn_80237F74(ctx, param3, 0x11) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0x14) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0x7) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0xf) == 1 || fn_80237F74(ctx, param3, 0x48) == 1) {
        ok = 0;
        goto check;
    }
    if (fn_80237F74(ctx, param3, 0x29) == 1) goto ok1;
    if (fn_80237F74(ctx, param3, 0x28) == 1) goto ok1;
    fn_80237F74(ctx, param3, 0xc);
ok1:
    ok = 1;
check:;
    if (ok == 0) return 0;
    result = _fightTrainerAiWazaHitCheck(ctx, param1, param2, param3, 0);
    if (fightSideIsJoutaiDataId(fightTargetGetPtrAsNowFightType(2, param3), 0x4b) == 1) return 0;
    if (result == 0) return 0;
    if (result == -1) return 1;
    return 1;
}

s32 fightTrainerAiWazaHit000(void* ctx, u32 param1, u32 param2, u32 param3) {
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

u32 fightTrainerAiWazaHitNull(void) { return 1; }
