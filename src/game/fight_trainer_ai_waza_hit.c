/**
 * @file fight_trainer_ai_waza_hit.c
 * @brief Candidate fightTrainerAiWazaHit.cpp range, 0x8025C264 - 0x8025C5A4.
 */
#include "game/fight_trainer_ai_waza_hit_shared.h"

void _fightTrainerAiWazaHitCheck(void* ctx, u32 param1, u32 param2, u32 param3, u32 param4) {
    extern void wazaGetStatus();
    extern void fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    extern void fightFloorGetFightOutPokemonPtrAry();
    extern void fightFloorSortFightOutPokemonPtrArySub();
    extern void fightFloorGetStatus();
    extern void fn_80201D84();
    extern void fn_80229934();
    extern void fn_80229B70();
    extern void fn_80229BD8();
    extern void fn_80235B04();
    extern void fn_80236BFC();
    extern void fn_80237288();
    extern void fightTrainerAiCheckGuard();
    u8 sp[0x50];
    u32 r0 = 0;
    u32 r3 = (u32)ctx;
    u32 r7 = param4;
    u32 r24 = 0;
    u32 r25 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    u32 r1 = (u32)sp;
    u32 r4 = param1;
    u32 r5 = param2;
    u32 r6 = param3;

    r25 = r3;
    r26 = r4;
    r27 = r5;
    r28 = r6;
    r29 = r7;
    r3 = 0x0;
    r4 = 0x0;
    r5 = 0x14;
    r6 = 0x0;
    fightFloorGetStatus();
    r4 = r3 & 0xFFFF;
    r3 = r26;
    fightTargetGetTragetPtrToRelativeHostSideFightTargetId();
    r31 = r3;
    r3 = r25;
    r4 = 0x0;
    r5 = 0x1;
    fn_80235B04();
    r4 = r27;
    r3 = 0x0;
    r5 = 0x9;
    r6 = 0x0;
    wazaGetStatus();
    r30 = r3 & 0xFFFF;
    if (r28 == (u32)0x0) {
        r3 = 0x1;
        return;
    }
    r7 = (u32)sp + 0x8;
    r3 = 0x0;
    r4 = 0x1;
    r5 = 0x0;
    r6 = 0x0;
    fightFloorGetFightOutPokemonPtrAry();
    r24 = r3;
    r4 = (u32)sp + 0x8;
    r3 = 0x0;
    r5 = 0x8;
    r6 = 0x0;
    fightFloorSortFightOutPokemonPtrArySub();
    r4 = (u32)sp + 0x8;
    r0 = r24 & 0xFFFF;
    r6 = 0x0;
    r7 = 0x0;
    r5 = 0x0;
    while (1) {
        r3 = r5 & 0xFFFF;
        if (r3 >= (u32)r0) break;
        r3 = *(u32*)(r4 + r3);
        if (r3 != (u32)0x0) {
            if (r26 == (u32)r3) {
                r6 = r5;
            }
            if (r28 == (u32)r3) {
                r7 = r5;
        }
        }
        r5 = r5 + 0x1;

    }
    r3 = r6 & 0xFFFF;
    r0 = r7 & 0xFFFF;
    r0 = r3 - r0;
    r24 = (u32)r0 >> 31;

    if (r29 == (u32)0xffff || r29 == (u32)0xfffe) {

        if (r29 == (u32)0xffff) {
            r3 = r25;
            r4 = r28;
            r5 = 0x1d;
            fn_80236BFC();
            r0 = r3 & 0xFF;
            if (r0 == (u32)0x1) {
                r3 = r28;
                r4 = 0x1d;
                fn_80201D84();
                r3 = r3 & 0xFFFF;
                r0 = r31 & 0xFFFF;
                if (r0 == (u32)r3) {
                    r3 = -0x1;
                    return;
        }
        }
        }
        r3 = r25;
        r4 = r26;
        fn_80237288();
        r0 = r3 & 0xFF;
        if ((r0 == (u32)0x1) && (r24 == (u32)0x1)) {

            r3 = 0x0;
            return;
        }
        r3 = r25;
        r4 = r28;
        r5 = r27;
        fightTrainerAiCheckGuard();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            r3 = 0x0;
            return;
        }
        r3 = 0x1;
        return;
    }
    r3 = r27;
    r4 = r26;
    r5 = r28;
    fn_80229934();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r25;
    r4 = r28;
    r5 = r27;
    fightTrainerAiCheckGuard();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = 0x0;
        return;
    }
    r3 = r25;
    r4 = r28;
    r5 = 0x1d;
    fn_80236BFC();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = r28;
        r4 = 0x1d;
        fn_80201D84();
        r3 = r3 & 0xFFFF;
        r0 = r31 & 0xFFFF;
        if (r3 == (u32)r0) {
            r3 = -0x1;
            return;
    }
    }
    if (r24 == (u32)0x1 && r0 == (u32)0x1 && r0 != (u32)0x39 && r0 != (u32)0xfa) {
        r3 = r25;
        r4 = r28;
        r5 = 0x1f;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if (r0 == (u32)0x1) {
            if (r30 != (u32)0x92) {
                if (r30 != (u32)0x95) {
                    if (r30 != (u32)0x98) {
                        if (r30 != (u32)0xcf) {
                            r3 = 0x0;
                            return;
        }
        }
        }
        }
        }
        r3 = r25;
        r4 = r28;
        r5 = 0x20;
        fn_80236BFC();
        r0 = r3 & 0xFF;
        if ((r0 == (u32)0x1) && (r30 != (u32)0x93)) {

            r3 = 0x0;
            return;
        }
        r3 = r25;
        r4 = r28;
        r5 = 0x21;
        fn_80236BFC();
        r0 = r3 & 0xFF;

        r0 = r27 & 0xFFFF;

        r3 = 0x0;
        return;
    }
    r3 = r27;
    fn_80229B70();
    r0 = r3 & 0xFF;
    if (r0 == (u32)0x1) {
        r3 = -0x1;
        return;
    }
    r3 = r27;
    fn_80229BD8();
    r0 = r3 & 0xFF;
    r3 = 0x1;
    if (r0 != (u32)0x1) return;
    r3 = -0x1;

    return;
}
