#include "dolphin/types.h"

extern u8* lbl_8047B610;

#pragma opt_unroll_loops off
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
    short local_28[4];

    uVar6 = fightFloorGetStatus(0, 0, 0x14, 0);
    uVar2 = fightTargetGetPtrAsNowFightType(0x11, 0);
    uVar3 = fightOutPokemonGetPokemonPtr();
    {
        extern u32 pokemonGetStatus();
        uVar4 = pokemonGetStatus(uVar2, 0, 0xd9, 0);
    }
    for (initIndex = 0; (u8)initIndex < 4; initIndex = initIndex + 1) {
        local_28[(u8)initIndex] = -1;
    }
    bVar9 = 0;
    for (sVar10 = 0; (s16)sVar10 < 4; sVar10 = sVar10 + 1) {
        cVar8 = pokemonWazaCheckValid(uVar3, (u16)sVar10);
        if (cVar8 != 0) {
            cVar8 = fightOutPokemonCheckCanOutOkWazaBanme(uVar2, (u16)sVar10, 0, 0);
            if ((cVar8 == 0) || (cVar8 == 6)) {
                {
                    extern u32 pokemonGetStatus();
                    sVar7 = (u16)pokemonGetStatus(uVar3, 0, 0x7f, (u16)sVar10);
                }
                if ((sVar7 != 0) && ((sVar7 != 0x165 && (sVar7 != 0x163)))) {
                    if ((sVar7 == 0) ||
                        ((((sVar7 == 0x165 || (sVar7 == 0xd6)) ||
                           (sVar7 == 0x112)) ||
                          ((sVar7 == 0x77 || (sVar7 == 0x76)))))) {
                        bVar1 = 1;
                    } else {
                        bVar1 = 0;
                    }
                    if (((!bVar1) && (sVar7 != 0x108)) && (sVar7 != 0xfd)) {
                        sVar7 = (u16)wazaGetStatus(0, sVar7, 9, 0);
                        if (((((sVar7 == 0x91) || (sVar7 == 0x27)) ||
                               (sVar7 == 0x4b)) ||
                              ((sVar7 == 0x97 || (sVar7 == 0x9b)))) ||
                             (sVar7 == 0x1a)) {
                            bVar1 = 1;
                        } else {
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
    sVar10 = *(s16*)((int)local_28 +
                     (((uVar5 & 0xffff) -
                       ((s32)(uVar5 & 0xffff) / (s32)(u8)bVar9) * (u8)bVar9) *
                              2 &
                      0x1fe));
    if ((s16)sVar10 < 0) {
        goto failed;
    }
    {
        extern u32 pokemonGetStatus(u32, u32, u32, u16);
        sVar7 = (u16)pokemonGetStatus(uVar3, 0, 0x7f, (u16)sVar10);
    }
    if (sVar7 == 0 || sVar7 == 0x165 || sVar7 == 0x163) {
        goto failed;
    }
    lbl_8047B60C = sVar7;
    wazaSetStatus(uVar4, 0, 0x26, 0, (int)sVar10);
    lbl_8047B618 = lbl_8047B618 & 0xfffffbff;
    floorTarget =
        fn_8022B2CC(uVar2, lbl_8047B60C, uVar6, 0, 1, 1, (void*)0xffffffff);
    fightFloorSetStatus(0, 0, 0x43, 0, floorTarget);
    lbl_8047B610 = (u8*)*(u32*)(lbl_8047B610 + 1);
done:
    return;
}
#pragma opt_unroll_loops reset
